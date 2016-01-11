/* doocs-wrapper.cc                                (c) Markus Hoffmann  */
/* Wrapper for the DOOCSserver library. We want to use all functions 
   without the C++ compiler */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

/*  first version                       08.07.2007    (c) Markus Hoffmann  */


#include <stdio.h>
#include <signal.h>
#include <string>
#include <rpc/pmap_clnt.h>

#include "config.h"
#include <doocs/eq_errors.h>
#include <doocs/doocsutil.h>
#include <doocs/d_fct.h>
#include <doocs/eq_fct_errors.h>
#include <doocs/eq_fct.h>
#include <doocs/eq_data.h>
#include <doocs/eq_adr.h>
#include <doocs/eq_errors.h>
#include <doocs/eq_client.h>
#include <doocs/eq_svr.h>
#include <doocs/DOOCShash.h>
#include <doocs/DOOCSsleep.h>
#include <doocs/eq_sts_codes.h>

#include "defs.h"
#include "vtypes.h"
#include "ptypes.h"

/* Protopypen fuer X11-Basic-Funktionen */

extern "C" void xtrim(char *t,int f, char *w );
extern "C" char *varrumpf(char *n);
extern "C" int variable_exist(char *name, int typ);
extern "C" int variable_exist_or_create(char *name, int typ);
extern "C" int do_dimension(int vnr);

extern "C" double parser(char *);
extern "C" STRING string_parser(char *);
extern "C" int type2(char *);
extern "C" char *s_parser(char *);
extern "C" int wort_sep (char *,char ,int ,char *, char *);
extern "C" STRING create_string(char *);
extern "C" void xberror(int,char *);
extern "C" ARRAY create_array(int, int, int*);
extern "C" ARRAY doocsnames(char *n);
extern "C" ARRAY doocsvget(char *n,int nn, int o);
extern "C" STRING doocsgets(char *n);
extern "C" STRING doocsinfos(char *n);
extern "C" double doocsget(char *n);
extern "C" void c_doocsput(char *w);
extern "C" int vartype(char *name);
extern "C" void string_zuweis_by_vnr(int, STRING);
extern "C" int procnr(char *n,int typ);
extern int ccs_err;
extern int pc,sp;
extern VARIABLE variablen[];
extern PROCEDURE procs[];
extern int stack[];

#define MAXDOOCSPIDS 52
#define RETRY 1

int doocsexportvars[100];
int anzdoocsexportvars=0;
int doocspids[MAXDOOCSPIDS];
int doocsrsubs[MAXDOOCSPIDS];
int doocswsubs[MAXDOOCSPIDS];
int doocspidanz=0;

/* Protopypen fuer DOOCS-Funktionen */

extern "C" {
void        init_localtime_r_mx    (void);
void        destroy_localtime_r_mx (void);
struct tm   *localtime_r           (const time_t *clock, struct tm *res);
}
	
std::vector<EqFct *>::iterator eq_iter; // list of all locations
std::vector<EqFct *>   *eq_list;

char *object_name = PACKAGE_NAME; // name of this object (used in error messages)
extern  char    *cnfp;

int initDOOCSserver(/*int, char *argv []*/);
extern char                   *clnt_name;
extern u_int                  clnt_new;
extern struct cl_tab	       clnt_tab [MAX_CLIENTS];
extern struct permission      access_perm;

extern char   	               banner      [16];
extern MUTEX_T mx_svr,mx_clnt,mx_error,mx_wr,mx_sig,mx_usr1,mx_usr2,mx_ext; 

extern COND_T  	cv_wr,cv_usr1,cv_usr2,cv_ext,cv_mode;
extern int flag_wr,block,flag_sig,flag_usr1,flag_usr2;
extern int sg_proc_usr1[3];
extern int	sig_ext,ext_threads,ext_counter,ext_flag;
extern long     eq_prog,eq_vers;

extern void eq_cancel		  (void);
extern void get_eq_libvers	   (char *, char *, int);
extern void create_all_threads    (int);
extern void exit_all_threads	   (void);
extern void switch_mt  	   (void);

extern fio_disp               table;

extern "C" void doocssig_handler(int sig) {
  char *cn;
  int flag;
  EqFct *p;

  switch (sig) {             
    case SIGPIPE:
      MUTEX_LOCK (&mx_clnt);
      cn = clnt_name;
      if (!cn) cn = "unknown";
      printf("%s-Server: SIGPIPE, link to client '%s' lost\n",banner,cn);
      MUTEX_UNLOCK (&mx_clnt);
      break;
    case SIGINT:
    case SIGTERM:
      MUTEX_LOCK (&mx_wr);
      flag = block;
      MUTEX_UNLOCK (&mx_wr);
      if (flag) break;
        if (sig == SIGINT) printf("%s %s",banner, "signal INT  received\n");
        else               printf("%s %s",banner, "signal TERM received\n");
        eq_cancel ();        // terminate all user-specific threads
        exit_all_threads (); // terminate all server-specific threads
        table.close_all ();  // close all files

                  printf("%s%s",banner, "done. Safe exit");

                  MUTEX_LOCK (&mx_svr);  // block all threads

                  for (eq_iter = eq_list->begin (); eq_iter != eq_list->end (); ++eq_iter) {
                       p = *eq_iter;
                       if (p) delete p;
                  }

                  MUTEX_DESTROY (&mx_sig);
                  MUTEX_DESTROY (&mx_wr);
                  MUTEX_DESTROY (&mx_usr1);
                  MUTEX_DESTROY (&mx_usr2);
                  MUTEX_DESTROY (&mx_error);
                  MUTEX_DESTROY (&mx_ext);

                  COND_DESTROY (&cv_mode);
                  COND_DESTROY (&cv_wr);
                  COND_DESTROY (&cv_usr1);
                  COND_DESTROY (&cv_usr2);
                  COND_DESTROY (&cv_ext);

                  MUTEX_UNLOCK (&mx_svr);
                  pmap_unset (eq_prog, eq_vers);

	          #ifdef __GNUC__
	          destroy_localtime_r_mx ();
	          #endif

                  delete [] cnfp;
    break;
  default:
    printf("%s We have got a signal nr. %d \n",banner,sig);
  }
}


//
// the external interrupts functions
//

extern	int		fct_code;
extern	int		chan_count;
extern  int		ring_buffer;
extern  int		memory_buffer;
time_t			walltime;

#define	Code_xbasic	100
#define xbasic_DEV 	"xbasic_channels"

/* Wichtige Klasse: Die regelt die lese und Schreibzugriffe auf Variablen. */

class D_xbasic : public D_fct {
public:
  int varnumber;
D_xbasic (char *pn, int vn) : D_fct(pn) {varnumber=vn;};
        void    get   ( EqAdr*, EqData* , EqData*, EqFct* );
        void    set   ( EqAdr*, EqData* , EqData*, EqFct* );
	int	data_type();
	int	length(); 	
	virtual int	permission ()	{ return (PERM_GROUP | PERM_OPER_GROUP); }
};

int D_xbasic::length() {
#ifdef DEBUG
  printf("Somebody asked for length of var nr %d\n",this->varnumber);
#endif
  return(do_dimension(this->varnumber));
}
int D_xbasic::data_type() {
#ifdef DEBUG
  printf("Somebody asked for type of var nr %d\n",this->varnumber);
#endif
  if(variablen[this->varnumber].typ==INTARRAYTYP) return(DATA_A_INT);
  else if(variablen[this->varnumber].typ==FLOATARRAYTYP) return(DATA_A_FLOAT);
  else if(variablen[this->varnumber].typ==STRINGARRAYTYP) return(DATA_A_USTR);
  else if(variablen[this->varnumber].typ==STRINGTYP) return(DATA_STRING);
  else if(variablen[this->varnumber].typ==INTTYP) return(DATA_INT);
  else if(variablen[this->varnumber].typ==FLOATTYP) return(DATA_FLOAT);
  else return(DATA_NULL);
}
void	D_xbasic::get   ( EqAdr*, EqData* , EqData* res, EqFct* ) {
  int pc2,i;
#ifdef DEBUG
  printf("Somebody asked for value of var nr %d\n",this->varnumber);
#endif
  /* Hier koennten wir auch Read-callbacks einbauen !
    am besten aber in form von Funktionen:
   
   DOOCScallback a,wproc,rfunc
  */
  
 /* schaue, ob es fuer diese Variable eine read-Callback-routine gibt: 
    Das problem hier ist, diese callbackroutine wird erst aufgerufen, nachdem der
    wert schon gelesen ist. Kaum zu verhindern!
 
 */
  if(doocspidanz) {
    for(i=0;i<doocspidanz;i++) {
      if(doocspids[i]==this->varnumber) {
      /* achtung, wir duerfen dies nicht von einem beliebigen thread ausfuehren !
      deshalb muessen wir das wie ein gosub behandel, ruecksprung auf den stack
      etc... */
        if(doocsrsubs[i]!=-1) {
          pc2=procs[doocsrsubs[i]].zeile;
          if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
          else printf("Stack-Overflow ! PC=%d\n",pc); 
        }
      }
    }
  }

  if(variablen[this->varnumber].typ==INTARRAYTYP) {
    int len=do_dimension(this->varnumber);
    int i;
    int *varptr=(int  *)(variablen[this->varnumber].pointer+variablen[this->varnumber].opcode*INTSIZE);
    res->set_type(DATA_A_INT);
    res->length(len);
    for(i=0;i<len;i++) {
      res->set(varptr[i],i);
    }
  } else if(variablen[this->varnumber].typ==FLOATARRAYTYP) {
    int len=do_dimension(this->varnumber);
    int i;
    double *varptr=(double  *)(variablen[this->varnumber].pointer+variablen[this->varnumber].opcode*INTSIZE);
    res->length(len);
    res->set_type(DATA_A_FLOAT);
    for(i=0;i<len;i++) {
      res->set((float)varptr[i],i);
    }
  } else if(variablen[this->varnumber].typ==STRINGARRAYTYP) {
    int len=do_dimension(this->varnumber);
    int i;
    USTR uuu;
    STRING *varptr=(STRING  *)(variablen[this->varnumber].pointer+variablen[this->varnumber].opcode*INTSIZE);
    res->length(len);
    res->set_type(DATA_A_USTR);
    for(i=0;i<len;i++) {
      uuu.str_data.str_data_len=varptr[i].len;
      (varptr[i].pointer)[varptr[i].len]=0;
      uuu.str_data.str_data_val=varptr[i].pointer;
      res->set(&uuu,i);
    }  
  } else if(variablen[this->varnumber].typ==STRINGTYP) {
    res->length(variablen[this->varnumber].opcode);
    (variablen[this->varnumber].pointer)[variablen[this->varnumber].opcode]=0;
    res->set((char *)variablen[this->varnumber].pointer);
  } else if(variablen[this->varnumber].typ==INTTYP) res->set(variablen[this->varnumber].opcode);
  else if(variablen[this->varnumber].typ==FLOATTYP) res->set((float)variablen[this->varnumber].zahl);
  else ; /* do nothing */
}

void D_xbasic::set(EqAdr* a, EqData* di, EqData* doi, EqFct* e) {
  int len,i,pc2;
  char s[USTR_LENGTH],t[USTR_LENGTH];
#ifdef DEBUG
  printf("Somebody writes to var nr %d, Property: %s\n",this->varnumber,a->property());
#endif
    if(variablen[this->varnumber].typ==STRINGTYP) {
      STRING a;
      if(di->type()==DATA_STRING16) {
        a.len=di->length();
        a.pointer=(char *)malloc(a.len+1);
	di->get_string(s,USTR_LENGTH);
	printf("String:  <%s>\n",s);
        di->get_string(a.pointer,a.len);
        string_zuweis_by_vnr(this->varnumber,a);
        free(a.pointer);
      } else if(di->type()==DATA_A_USTR) {
 	di->get_string(s,USTR_LENGTH);
	printf("String:  <%s>\n",s);
	a=create_string(s);
        string_zuweis_by_vnr(this->varnumber,a);
        free(a.pointer);
      } else {
  	di->get_string(s,USTR_LENGTH);
	printf("String:  <%s>\n",s);
 	a=create_string(s);
        string_zuweis_by_vnr(this->varnumber,a);
        free(a.pointer);
      }
      
    } else if(variablen[this->varnumber].typ==INTTYP) variablen[this->varnumber].opcode=di->get_int();
    else if(variablen[this->varnumber].typ==FLOATTYP) variablen[this->varnumber].zahl=(double)di->get_float();
    else {
      di->get_string(s,USTR_LENGTH);
//      di->get_string_arg(t,0);

      printf("Time:    %d\n",(long)di->time());
      printf("Float:   %f\n",di->get_float());
      printf("String:  <%s>\n",s);
//      printf("String2: <%s>\n",t);
      printf("Type:    %d\n",di->type());
      printf("Length:  %d\n",di->length());
    }
     
 
 /* schaue, ob es fuer diese Variable eine Callback-routine gibt: */
  if(doocspidanz) {
    for(i=0;i<doocspidanz;i++) {
      if(doocspids[i]==this->varnumber) {
      /* achtung, wir duerfen dies nicht von einem beliebigen thread ausfuehren !
      deshalb muessen wir das wie ein gosub behandeln, Ruecksprung auf den Stack
      etc... */
        if(doocswsubs[i]!=-1) {
          pc2=procs[doocswsubs[i]].zeile;
          if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
          else printf("Stack-Overflow ! PC=%d\n",pc); 
        }
      }
    }
  }
}

/* Definition der Server-Klasse fuer X11-Basic  */

class EqFct_xbasic : public EqFct {
public:
  char		descr[STRING_LENGTH+1];
//  D_name	desc_;
//  D_spectrum	sin_fct_;
//  D_hist	hist_;
//  D_dig_filter	dig_filter_;
  D_xbasic       *doocsparams[100];
  ArchiveFile	*archloc_p;
  static int anzdoocsparams;

  EqFct_xbasic ( );		// constructor
  ~EqFct_xbasic ( ) { ; };	// destructor
  
  void	update ();
  void	init ();	// started after creation of all Eq's 
  int	fct_code()	{ return Code_xbasic; };
  void	online();
  static int	conf_done;
};

EqFct*	xbasic_fct;

void EqFct_xbasic::online() {        // called by init() and RPC "set online"
  int	error;
#ifdef DEBUG
  puts("*** doocsserver online.");
#endif
  // do some online/alive test
  error = sts_ok;
  if (!error) {
    g_sts_.error (0);
    g_sts_.newerror (0);
    g_sts_.online (1);
  }
  else g_sts_.online (0);
}

void	EqFct_xbasic::init ( ) {
// char	HistFile[80];
#ifdef DEBUG
  puts("*** doocsserver init.");
#endif
  //
  // create one archive file per location
  //
//  sprintf( HistFile, "%s/X11BASIC_%s.hist", "data", name_.value() );
//  archloc_p  = new ArchiveFile( HistFile,xbasic_DEV, NOINDEX );

  // Init history
//  hist_.book_hist( archloc_p, name_str(), ring_buffer, memory_buffer );

  // do an online/alive test
  online();
};

int EqFct_xbasic::conf_done = 0;
int EqFct_xbasic::anzdoocsparams = 0;

EqFct_xbasic::EqFct_xbasic ( ) :	/* Constructor */
  EqFct		("NAME = location" )
//  ,desc_	("SIN.DESC ion of channel")
//  ,sin_fct_	("SIN.TD sine fct.", (u_int)SPEC_LEN, this)
//  ,hist_	("SIN.HIST history", this)
//  ,dig_filter_	("SIN.DIG_FILT dig. filter f. hist.")
{ 
  D_fct* t;
  int i;
  char s[100];
  for(i=0;i<anzdoocsexportvars;i++) {
    s[0]=0;
    strcpy(s,variablen[doocsexportvars[i]].name);
    if(variablen[doocsexportvars[i]].typ&STRINGTYP) strcat(s,"$");
    else if(variablen[doocsexportvars[i]].typ&INTTYP) strcat(s,"%");
    if(variablen[doocsexportvars[i]].typ&ARRAYTYP) strcat(s,"()");
    strcat(s," X11-Basic Variable");
    doocsparams[anzdoocsparams++]= new D_xbasic(s,doocsexportvars[i]); 
  }
//  t = &desc_ ;		fct_list->push_back ( t );
//  t = &sin_fct_ ;	fct_list->push_back ( t );
//  t = &hist_ ;		fct_list->push_back ( t );
//  t = &dig_filter_ ;	fct_list->push_back ( t );
  if(anzdoocsparams) {
    for(i=0;i<anzdoocsparams;i++) {
      t=doocsparams[i];	fct_list->push_back ( t ); 
    }
  }
  if (!conf_done) {
    list_append();
    conf_done = 1;
  }
  archloc_p = 0;
}


void EqFct_xbasic::update ( ) {
#ifdef DEBUG
  puts("*** doocs server update.");
#endif
  if(g_sts_.online() ) {	// device is online
    /* Was soll man hier machen ? Datennahme triggern? */
    set_error( (int)no_error );
  } else {			// device is offline   
    set_error( (int)went_offline ); // Set device offline    
  }
}

void	eq_call_back (int i) {  
  EqFct* eqn;
#ifdef DEBUG
  printf("eq_call_back called with %d.\n",i);
#endif
  switch (i) {
  case 1:		// "{"  ==> new EqFct
    switch (fct_code) {
    case CodeFct:
      eqn =  new EqFct ("DUMMY");
      break;
    case CodeSvr:
      eqn =  new EqFctSvr ();
      break;
    case Code_xbasic:
      eqn =  new EqFct_xbasic ();
      break;
    default:
      printf(" Undefined fct_code ! exit ! \n");
      exit(-1);
      break;
    }
    xbasic_fct = eqn;
    eq_list->push_back(eqn);

    chan_count++;
    break;
  case 3:		// "}"  ==> read settings
    list_append(xbasic_fct);
    break;
  default:
    break;
  }
}

void	eq_init_prolog ()	// called before init of all EqFct's
{
//  puts("eq_init_prolog called.");
}
void	eq_init_epilog ()	// called at end of init of all EqFct's
{
//  puts("eq_init_epilog called.");
}
void refresh_prolog ()		// called before "update"
{
  // puts("refresh_prolog called.");
}
void refresh_epilog ()	{	// called after "update"
 //  puts("refresh_epilog called.");
}

void interrupt_usr1_prolog (int i) {}
void interrupt_usr1_epilog (int i) {}
void interrupt_usr2_prolog () {}
void interrupt_usr2_epilog () {}
void post_init_prolog() {}
void post_init_epilog() {}
void eq_cancel() {}

ARRAY doocsnames(char *n) {
  int repeat,err_code,len;
  EqAdr  address;
  EqData *result;
  EqCall *call = new EqCall;
  ARRAY ergeb;
  address.adr(n);
#ifdef DEBUG 
  printf("#Facility: <%s>\n",(&address)->facility());
  printf("#device: <%s>\n",(&address)->device());
  printf("#location: <%s>\n",(&address)->location());
  printf("#property: <%s>\n",(&address)->property());
#endif
  for(repeat=0; repeat<RETRY;repeat++){
    result = call->names(&address);
    err_code = result->error();
    if(err_code == ERR_RPC  || err_code == ERR_ILL_FACILITY){
      result = call->set_option(&address,EQ_RETRY);
      continue;
    } else break;
  }
  if(result->error() == 0 ){
    len=result->length();
#ifdef DEBUG
    printf("Ergebnis-Laenge: %d\n",len);
#endif
    ergeb=create_array(STRINGTYP,1,&len);
    if(len) {
      char buff[80];
      int num,klu,klu2;    
      STRING *varptr;
      varptr=(STRING *)((long)ergeb.pointer+INTSIZE);
      strcpy(buff,"");
      for(num=0;num<len;num++){
	result->get_string(num, buff, 80);
#ifdef DEBUG
	printf("%d: %s\n",num,buff);				
#endif
	klu=strlen(buff)-1;
	while(klu>=0){
	  if(buff[klu]=='\t') break;
	  klu--;
	}
	klu2=klu;
	while(klu2<strlen(buff)){
	  if(buff[klu2]==' ') break;
	  klu2++;
	}
	buff[klu2]=0;
	varptr[num].len=strlen(&(buff[klu+1]));
	varptr[num].pointer=(char *)malloc(varptr[num].len+1);
	memcpy(varptr[num].pointer,&(buff[klu+1]),varptr[num].len);
	varptr[num].pointer[varptr[num].len]=0;
#ifdef DEBUG
	printf("%s \n",&(buff[klu+1]));
#endif
      }
    }	
  } else printf("DOOCS ERROR %d\n",ccs_err=result->error());
  return(ergeb);
}


ARRAY doocsvget(char *n,int nn, int o) {
  int repeat,err_code,len,typ,j,i;
  EqAdr  address;
  EqData data, *result= new EqData;
  EqData *buffer;  
  EqCall *call = new EqCall;
  ARRAY ergeb;
  char	str_buf[USTR_LENGTH];
  address.adr(n);
  for(repeat=0; repeat<RETRY;repeat++){
    buffer=call->get(&address,&data);
    result->copy_from(buffer);
    err_code=result->error();
    if(err_code == ERR_RPC  || err_code == ERR_ILL_FACILITY){
      buffer = call->set_option(&address, EQ_RETRY);
      continue;
    } else break;
  }
  if(err_code) {
    printf("DOOCS ERROR #%d:%s, couldnt get %s\n",ccs_err=err_code,
           result->get_string(str_buf, USTR_LENGTH),n);
    ergeb=create_array(INTTYP,0,&len);		      
    return(ergeb);
  }
  len=result->length();     /* get size of Vector */
  if(nn==0) nn=len;
  nn=min(len-o,nn);
  
  typ=result->type(); 
  if(typ==1 || typ==14 || typ==4) {     /* INT, IIII, BOOL*/
    int *varptr,klu,klu2,ll;
    ergeb=create_array(INTTYP,1,&nn);
    varptr=(int *)((long)ergeb.pointer+INTSIZE); 
    result->get_string(str_buf, USTR_LENGTH);
    klu=0;
    j=0;
    ll=strlen(str_buf);
    for(i=0;i<len;i++) {
      klu2=klu;
      for(klu=klu2;klu<ll;klu++) {
        if(str_buf[klu]=='\t') break;
      }
      str_buf[klu++]=0;
      if(i>=o && j<nn) varptr[j++]=atoi(&str_buf[klu2]);
    }
  } else if(typ==2|| typ==15 ) {/* FLOAT, IFFF */
    double *varptr;
    int klu,klu2,ll;
    ergeb=create_array(FLOATTYP,1,&nn);
    varptr=(double *)((long)ergeb.pointer+INTSIZE); 
    result->get_string(str_buf, USTR_LENGTH);
    klu=0;
    j=0;
    ll=strlen(str_buf);
    for(i=0;i<len;i++) {
      klu2=klu;
      for(klu=klu2;klu<ll;klu++) {
        if(str_buf[klu]=='\t') break;
      }
      str_buf[klu++]=0;
      if(i>=o && j<nn) varptr[j++]=(double)atof(&str_buf[klu2]);
    }
  } else if(typ==0) {/* NULL */
    ergeb=create_array(INTTYP,0,&nn);
  } else if(typ==5) {/* STRING16 */
    STRING *varptr;
    result->get_string(str_buf, USTR_LENGTH);
    nn=1;
    ergeb=create_array(STRINGTYP,1,&nn);
    varptr=(STRING *)((long)ergeb.pointer+INTSIZE);
    varptr->pointer=(char *)malloc(len+1);
    varptr->len=len;
    memcpy(varptr->pointer,str_buf,len); 
  } else if(typ==103) {/* A_USTR */
     int klu;    
     STRING *varptr;

     result->get_string(str_buf, USTR_LENGTH);
     klu=strlen(str_buf)-1;
     while(klu>=0){
       if(str_buf[klu]=='\t') break;
       klu--;
     }
     if(len!=1) printf("Dieser Fall ist noch nicht vorgekommen!\n");
    nn=1;
    ergeb=create_array(STRINGTYP,1,&nn);
    varptr=(STRING *)((long)ergeb.pointer+INTSIZE);
    varptr->pointer=(char *)malloc(strlen(&str_buf[klu+1])+1);
    varptr->len=strlen(&str_buf[klu+1]);
    memcpy(varptr->pointer,&str_buf[klu+1],varptr->len); 
  } else if(typ==101) {/* A_TDS */
    time_t  	t1;
    float 	f;
    int dimlist[]={len,2};
    int i;
    unsigned char   sts;
    double *varptr;
    ergeb=create_array(FLOATTYP,2,dimlist);
    varptr=(double *)((long)ergeb.pointer+2*INTSIZE);
    for(i=0;i<len;i++) {
      result->get_tds(&t1,&f,&sts, i);
      varptr[2*i]=(double)t1;
      varptr[2*i+1]=(double)f;
    }
  } else if(typ==19) {/* SPECTRUM */
    char *ptmp;
    int k;
    float f,f2,*f3;
    int dimlist[]={len,2};
    double *varptr;
    u_char   sts;
    time_t  	tm;
    result->get_spectrum(&ptmp,&k,&tm,&f,&f2,&sts,&f3,&k);
  //  printf("Get-Spectrum ergab: f123=%f,%f,%f t=%d k=%d\n",f,f2,*f3,tm,k);
    ergeb=create_array(FLOATTYP,2,dimlist);    
    varptr=(double *)((long)ergeb.pointer+2*INTSIZE);
    for(i=0;i<len;i++) {
      result->get_string(i, str_buf, USTR_LENGTH);
  //    printf("%d:%f <%s>\n",i,f+i*f2,str_buf);
      varptr[2*i]=(double)(f+i*f2);
      varptr[2*i+1]=(double)atof(str_buf);
    }
  } else {   
    result->get_string(str_buf, USTR_LENGTH);
    printf("Result: <%s>\n");
    printf("Length of result: %d\n",len);
    printf("Type of result: %d = %s\n",result->type(),result->type_string());
 
    printf("output format type %d is not implemented !\n",typ);
    xberror(46,n); /* Parameter hat falschen Typ */
    ergeb.pointer=malloc(40);
    ((int *)ergeb.pointer)[0]=1;
  }
  if(result)   	delete result;
  if(call)   	delete call;
 return(ergeb);
}
STRING doocsgets(char *n) {
  int repeat,err_code,len,typ;
  EqAdr  address;
  EqData data, *result= new EqData;
  EqData *buffer;  
  EqCall *call = new EqCall;
  STRING ergeb;
  char	str_buf[USTR_LENGTH];
  address.adr(n);
  for(repeat=0; repeat<RETRY;repeat++){
    buffer=call->get(&address,&data);
    result->copy_from(buffer);
    err_code=result->error();
    if(err_code == ERR_RPC  || err_code == ERR_ILL_FACILITY){
      buffer = call->set_option(&address, EQ_RETRY);
      continue;
    } else break;
  }
  if(err_code) {
    printf("DOOCS ERROR #%d:%s, couldnt get %s\n",ccs_err=err_code,
           result->get_string(str_buf, USTR_LENGTH),n);
    ergeb=create_string("<ERROR>");		      
    return(ergeb);
  }
  len=result->length();  /* get size */
  typ=result->type();
  result->get_string(str_buf, USTR_LENGTH);
  ergeb=create_string(str_buf);
  return(ergeb);
}
STRING doocsinfos(char *n) {
  int repeat,err_code;
  EqAdr  address;
  EqData *result;
  EqCall *call = new EqCall;
  STRING ergeb;
  char	str_buf[USTR_LENGTH];
  char  nn[strlen(n)+2];
  int klu;
  strcpy(nn,n);
  strcat(nn,"*");
  address.adr(nn);
  for(repeat=0; repeat<RETRY;repeat++){
    result=call->names(&address);
    err_code=result->error();
    if(err_code == ERR_RPC  || err_code == ERR_ILL_FACILITY){
      result = call->set_option(&address, EQ_RETRY);
      continue;
    } else break;
  }
  if(err_code) {
    printf("DOOCS ERROR #%d:%s, couldnt get %s\n",ccs_err=err_code,
           result->get_string(str_buf, USTR_LENGTH),n);
    ergeb=create_string("<ERROR>");		      
    return(ergeb);
  }
  result->get_string(str_buf, USTR_LENGTH);
  klu=strlen(str_buf)-1;
   while(klu>=0){
     if(str_buf[klu]=='\t') break;
     klu--;
   }
   while(klu<strlen(str_buf)){
     if(str_buf[klu++]==' ') break;
   }

  ergeb=create_string(&str_buf[klu]);
  return(ergeb);
}

double doocsget(char *n) {
  int repeat,err_code,len,typ,i,l;
  EqAdr  address;
  EqData data, *result= new EqData;
  EqData *buffer;  
  EqCall *call = new EqCall;
  char	str_buf[USTR_LENGTH];
  address.adr(n);
  for(repeat=0; repeat<RETRY;repeat++){
    buffer=call->get(&address,&data);
    result->copy_from(buffer);
    err_code=result->error();
    if(err_code == ERR_RPC  || err_code == ERR_ILL_FACILITY){
      buffer = call->set_option(&address, EQ_RETRY);
      continue;
    } else break;
  }
  if(err_code) {
    printf("DOOCS ERROR #%d:%s, couldnt get %s\n",ccs_err=err_code,
           result->get_string(str_buf, USTR_LENGTH),n);
    return(0);
  }
  len=result->length();  /* get size */
  typ=result->type();
  result->get_string(str_buf, USTR_LENGTH);
  if(len!=1) printf("Parameter is a Vector[%d]!\n",len);     
  if(result)   	delete result;
  if(call)   	delete call;
  if(typ==DATA_FLOAT) return((double)atof(str_buf));
  else if(typ==DATA_INT) return((double)atoi(str_buf));
  else {
     printf("output format type %d is not a number !\n",typ);
    return(0);
  }
}


extern "C" double doocstimestamp(char *n);
double doocstimestamp(char *n) {
  int repeat,err_code;
  time_t time;
  EqAdr  address;
  EqData data, *result= new EqData;
  EqData *buffer;  
  EqCall *call = new EqCall;
  char	str_buf[USTR_LENGTH];
  address.adr(n);
  for(repeat=0; repeat<RETRY;repeat++){
    buffer=call->get(&address,&data);
    result->copy_from(buffer);
    err_code=result->error();
    if(err_code == ERR_RPC  || err_code == ERR_ILL_FACILITY){
      buffer = call->set_option(&address, EQ_RETRY);
      continue;
    } else break;
  }
  if(err_code) {
    printf("DOOCS ERROR #%d:%s, couldnt get %s\n",ccs_err=err_code,
           result->get_string(str_buf, USTR_LENGTH),n);
    return(0);
  }
  time=result->time();
  return(time);
}

extern "C" int doocstyp(char *n);
int doocstyp(char *n) {
  int repeat,err_code,typ;
  EqAdr  address;
  EqData data, *result= new EqData;
  EqData *buffer;  
  EqCall *call = new EqCall;
  char	str_buf[USTR_LENGTH];
  address.adr(n);
  for(repeat=0; repeat<RETRY;repeat++){
    buffer=call->get(&address,&data);
    result->copy_from(buffer);
    err_code=result->error();
    if(err_code == ERR_RPC  || err_code == ERR_ILL_FACILITY){
      buffer = call->set_option(&address, EQ_RETRY);
      continue;
    } else break;
  }
  if(err_code) {
    printf("DOOCS ERROR #%d:%s, couldnt get %s\n",ccs_err=err_code,
           result->get_string(str_buf, USTR_LENGTH),n);
    return(0);
  }

  /* get type */
  typ=result->type();

  if(result)   	delete result;
  if(call)   	delete call;
  return(typ);
}
extern "C" int doocssize(char *n);
int doocssize(char *n) {
  int repeat,err_code,len;
  EqAdr  address;
  EqData data, *result= new EqData;
  EqData *buffer;  
  EqCall *call = new EqCall;
  char	str_buf[USTR_LENGTH];
  address.adr(n);
  for(repeat=0; repeat<RETRY;repeat++){
    buffer=call->get(&address,&data);
    result->copy_from(buffer);
    err_code=result->error();
    if(err_code == ERR_RPC  || err_code == ERR_ILL_FACILITY){
      buffer = call->set_option(&address, EQ_RETRY);
      continue;
    } else break;
  }
  if(err_code) {
    printf("DOOCS ERROR #%d:%s, couldnt get %s\n",ccs_err=err_code,
           result->get_string(str_buf, USTR_LENGTH),n);
    return(0);
  }
  len=result->length();  /* get size */
  if(result)   	delete result;
  if(call)   	delete call;
  return(len);
}


void c_doocsput(char *w) {
  char n[strlen(w)+1],t[strlen(w)+1];
  int e=wort_sep(w,',',TRUE,n,t);
  EqAdr  address;
  EqData data, *result= new EqData;
  EqCall *call = new EqCall;
  int typ,len,err_code,i; 
  char	str_buf[USTR_LENGTH];
  char *test;
  char *value;
  if(e<2) {
    printf("Syntax-Error bei DOOCSPUT. <%s>\n",w);
    return;
  }
  test=s_parser(n);
  address.adr(test);
  free(test);

  /* Parameter-Typ bestimmen */
  result=call->get(&address,&data);
  err_code=result->error();
  if(err_code) {
    printf("DOOCS ERROR #%d:%s, couldnt set %s\n",ccs_err=err_code,
           result->get_string(str_buf, USTR_LENGTH),n);
    return;
  }
  len=result->length();
  typ=result->type();
  if(type2(t)&ARRAYTYP) {
    printf("Error: arrays gehen noch nicht.\n");  
    return;
  } else if(type2(t) & STRINGTYP) {
    STRING sss=string_parser(t);
    value=sss.pointer;
  } else {
    value=(char *)malloc(64);
    sprintf(value,"%g",parser(t));
  }

  data.set_type( typ );    /* set the data type  */
  // write to the channel

  i=data.set_from_string(value);
  free(value);
  if(i) {
    result=call->set(&address, &data);
    err_code=result->error();
    if(err_code) {
      printf("DOOCS ERROR #%d:%s, couldnt set %s\n",ccs_err=err_code,
           result->get_string(str_buf, USTR_LENGTH),n);
    }
  } else printf("DOOCS ERROR: could not set value.\n");
}
extern "C" void c_doocsserver(PARAMETER *plist, int e);
void c_doocsserver(PARAMETER *plist, int e) {
  EqFct               *p;
  sigset_t	     mask;

  initDOOCSserver();
  MUTEX_LOCK (&mx_svr);

  if(e) xbasic_fct->name_.set_value((char *)plist[0].pointer);

  std::vector<EqFct *>::iterator eq_iter; // list of all locations
  for (eq_iter = eq_list->begin (); eq_iter != eq_list->end (); ++eq_iter) {
    p = *eq_iter;
    if (p) {
        p->lock ();
        p->post_init();
        p->unlock ();
#ifdef DEBUG	
        printf("LOCATION: <%s> \n",p->name_.value());	
#endif
    }
  }  
  
  MUTEX_UNLOCK (&mx_svr);
  switch_mt (); // switch to MULTITHREADED mode

  /* die blockierten Signale muessen (fuer diesen thread) wieder freigegeben 
     werden */
  sigemptyset (&mask);
  sigaddset (&mask, SIGALRM);
  sigaddset (&mask, SIGPIPE);
  sigaddset (&mask, SIGTERM);
  sigaddset (&mask, SIGBUS);
  sigaddset (&mask, SIGINT);
  sigaddset (&mask, SIGUSR1);
  sigaddset (&mask, SIGUSR2);
  sigaddset (&mask, SIGURG);
  pthread_sigmask(SIG_UNBLOCK, &mask, 0);
}


extern "C" void c_doocsexport(char *n);
void c_doocsexport(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1],*r;
  int p,len=1,typ,vnr,etyp;
  strcpy(v,n);
  p=wort_sep(v,',',TRUE,w,v);
  while(p) {
    xtrim(w,TRUE,w);
    typ=type2(w);
    if(typ & CONSTTYP) xberror(32,"DOOCSEXPORT");  /* Syntax error */
    else {
      r=varrumpf(w);
      vnr=variable_exist(r,typ);
      if(typ & ARRAYTYP) { /* ganzes Array  */
        if(vnr==-1) xberror(15,w); /* Feld nicht dimensioniert */
        else len=do_dimension(vnr);
      }
      /*Hier sollte man die Variable anlegen, falls nicht existent...*/
      if(vnr==-1) vnr=variable_exist_or_create(r,typ);
      doocsexportvars[anzdoocsexportvars++]=vnr;
      free(r);
    }
    p=wort_sep(v,',',TRUE,w,v);
  }
}

extern "C" void c_doocscallback(char *n);
void c_doocscallback(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,w1,w2);
  int typ,vnr,len,pc2=-1,pc3=-1;
  if(e<2) {
    xberror(32,"DOOCSCALLBACK");  /* Syntax error */
    return;
  }
  typ=type2(w1);
  if(typ & CONSTTYP) xberror(32,"DOOCSCALLBACK");  /* Syntax error */
  else {
    char *r=varrumpf(w1);
    vnr=variable_exist(r,typ);
    if(vnr==-1) xberror(57,w1);  /* Variable noch nicht initialisiert */
    else { 
      e=wort_sep(w2,',',TRUE,w1,w2);
      if(e==2) {
        pc3=procnr(w2,1);
        if(pc3==-1)   xberror(19,w2); /* Procedure nicht gefunden */
      }	
      if(strlen(w1)) {
        pc2=procnr(w1,1);
        if(pc2==-1)   xberror(19,w1); /* Procedure nicht gefunden */
      }
      if(pc2!=-1 || pc3!=-1) {
        if(doocspidanz<MAXDOOCSPIDS) {
          int i,flag=-1;
          for(i=0;i<doocspidanz;i++) {
            if(doocspids[i]==vnr) flag=i;
          }
          if(flag==-1) flag=doocspidanz++;
          doocspids[flag]=vnr;
	  doocswsubs[flag]=pc2;
	  doocsrsubs[flag]=pc3;
        } else printf("ERROR: Too many callbacks registered. max= %d\n",MAXDOOCSPIDS);
      } else {
        /* in diesem Fall trage die callbacks aus, falls vorhanden */
        if(doocspidanz) {
          int i,flag=-1;
          for(i=0;i<doocspidanz;i++) {
            if(doocspids[i]==vnr) flag=i;
          }
          if(flag!=-1) {
            doocspids[flag]=doocspids[--doocspidanz];
	    doocswsubs[flag]=doocswsubs[doocspidanz];
	    doocsrsubs[flag]=doocsrsubs[doocspidanz];
	  }
        }
      }
    }
  }
}
