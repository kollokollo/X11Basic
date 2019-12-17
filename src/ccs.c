/* CCS.C (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

/*  bug mit notify-handler behoben      18.01.2005    (c) Markus Hoffmann*/
/*  ccs_err fuer tine verbessert        18.01.2005    (c) Markus Hoffmann*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "svariablen.h"
#include "xbasic.h"
#include "type.h"
#include "parser.h"
#include "parameter.h"
#include "ccs.h"



#if defined CONTROL || defined TINE
static int ccs_err;
int v_ccserr() {return(ccs_err);}
#endif

/* Falls mit Kontrollsystem  */

#define MAXPIDS 250
#if defined CONTROL || defined TINE
int notify_handler(int , int , int );
int pids[MAXPIDS];
int isubs[MAXPIDS];
int pidanz=0;
#endif

#ifdef CONTROL

#include "ccsdef.h"
#include "piddef.h"
#include "kernel.h"
#include "supman.h"
#include "errdef.h"
#include "attrdef.h"
#include "resourcebaseman.h"


int v_ccsaplid() {return(aplid);}

/* Kontrollsystemstuff */

void cs_init() {
  ccs_attach_control( xbasic_name, &aplid );
  if (CCSERR) printf("ERROR in ccs_attach_control\n");
  else {
    /* attach to notification system */

    ccs_attach_notification( xbasic_name, CCS_NOTIFY_ASYNC, notify_handler, MAXPIDS, 1);
    if ( CCSERR ) printf ( "CCS: Keine Verbindung zum Benachrichtigungs-System: %s\n", ccs_get_error_message() );
#ifdef DEBUG
    else printf("Erfolgreich an Kontrollsystem angemeldet. aplid=%d\n",aplid);
#endif
  }
  ccs_err=CCSERR;
}

void cs_exit() {
  ccs_detach_notification();
  ccs_detach_control();
}
#endif

#if defined CONTROL || defined TINE
int notify_handler(int pid, int overflow, int entries) {
  int i,pc2,flag=0;
  printf("ent=%d\n",entries);
  for(i=0;i<pidanz;i++) {
    if(pids[i]==pid) {
      int oldbatch,osp=sp;
      pc2=procs[isubs[i]].zeile;
      if(stack_check(sp)) {stack[sp++]=pc;pc=pc2+1;}
      else xberror(75,""); /* Stack Overflow! */
      oldbatch=batch;batch=1;
      programmlauf();
      batch=min(oldbatch,batch);
      if(osp!=sp) {
	pc=stack[--sp]; /* wenn error innerhalb der func. */
        printf("Fehler innerhalb Interrupt-FUNCTION. \n");
      }
      flag=1;
    }
  }
  if(flag==0) printf("Uninitialisierter Interrupt: pid=%d, over=%d, ent=%d \n",pid,overflow,entries);
}
#endif
#ifdef CONTROL
int cssize(char *n) {
  int pid;
  ROUTE route;
  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) {
	printf("ERROR in ccs_convert_parametername_id\n");
	
  } else {
    /* get size of Vector */
    ccs_convert_pid_netroute( pid, &route );
    if (CCSERR) printf("ERROR in ccs_convert_pid_netroute\n");
    else return(route.size);
  }
  ccs_err=CCSERR;
  return(0);
}

int cstyp(char *n) {
  int typ=0;
  int pid;
  ROUTE route;
  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) {
	printf("ERROR in ccs_convert_parametername_id\n");
	
  } else {
    if ( IS_PID_ANALOG(pid) ) typ=typ|FLOATTYP;
    else if ( IS_PID_DIGITAL(pid) ) typ=typ|INTTYP;
    else if ( IS_PID_STRING(pid) ) typ=typ|STRINGTYP;
    /* get size of Vector */
    ccs_convert_pid_netroute( pid, &route );
    if (CCSERR) printf("ERROR in ccs_convert_pid_netroute\n");
    else {
      if(route.size>1) typ=typ|ARRAYTYP;
    }
  }
  ccs_err=CCSERR;

  return(typ);
}

int cspid(char *n) {
  int pid;
  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) printf("ERROR in ccs_convert_parametername_id\n");
  ccs_err=CCSERR;
  return(pid);
}


char *cspname(int pid) {
  char *pname=malloc(80);
  ccs_convert_id_parametername( pid, pname );
  if (CCSERR) printf("ERROR in ccs_convert_id_parametername (%s) !\n", ccs_get_error_message());
  ccs_err=CCSERR;
  return(pname);
}

char *csunit(char *n) {
  char *unit=malloc(32);
  int pid;
  union {int i;float f;double d;} min, max;
  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) printf("ERROR in ccs_convert_parametername_id\n");
  else {
    ccs_resource_read_parameter_minmax( pid, &min, &max, unit );
    if (CCSERR) printf("ERROR in ccs_resource_read_parameter_minmax (%s) !\n", ccs_get_error_message());
  }
  ccs_err=CCSERR;
  return(unit);
}


double csget(char *n) {

  int pid,j;
  float data;
  int i;

  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) printf("ERROR in ccs_convert_parametername_id\n");
  else {

/* switch appropriate to type */

    if ( IS_PID_ANALOG(pid) ) {
      ccs_get_value( pid, &data, &j, 1, NULL );
      if (CCSERR) printf("ERROR in ccs_get_value: %s\n", ccs_get_error_message());
      else return((double)data);
    } else if ( IS_PID_DIGITAL(pid) ) {
      ccs_get_value( pid, &i, &j, 1, NULL );
      if (CCSERR) printf("ERROR in ccs_get_value: %s\n", ccs_get_error_message());
      else return((double)i);
    } else 	printf("Csget: Parameter hat falschen Typ %s\n",n);
  }
  return(0.0);
}

double csmin(char *n) {

  int pid;
  char unit[32];
  union {int i;float f;double d; char *s;} min,max;

  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) printf("ERROR in ccs_convert_parametername_id\n");
  else {
    ccs_resource_read_parameter_minmax( pid, &min, &max, unit );
    if (CCSERR) printf(" ERROR in ccs_resource_read_parameter_minmax: %s\n", ccs_get_error_message());
/* switch appropriate to type */

    if ( IS_PID_ANALOG(pid) ) return((double)min.f);
    else if ( IS_PID_DIGITAL(pid) ) return((double)min.i);
    else 	printf("Csget: Parameter hat falschen Typ %s\n",n);
  }

  return(0.0);
}
double csmax(char *n) {

  int pid;
  char unit[32];
  union {int i;float f;double d; char *s;} min,max;

  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) printf("ERROR in ccs_convert_parametername_id\n");
  else {
    ccs_resource_read_parameter_minmax( pid, &min, &max, unit );
    if (CCSERR) printf(" ERROR in ccs_resource_read_parameter_minmax: %s\n", ccs_get_error_message());
/* switch appropriate to type */

    if ( IS_PID_ANALOG(pid) ) return((double)max.f);
    else if ( IS_PID_DIGITAL(pid) ) return((double)max.i);
    else 	printf("Csget: Parameter hat falschen Typ %s\n",n);
  }

  return(0.0);
}
double csres(char *n) {

  int pid;
  union {int i;float f;double d; char *s;} res;

  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) printf("ERROR in ccs_convert_parametername_id\n");
  else {

    ccs_resource_read_parameter_resolution( pid, &res, NULL );


    if (CCSERR) printf(" ERROR in ccs_resource_read_parameter_resolution: %s\n", ccs_get_error_message());
    if ( IS_PID_ANALOG(pid) ) return((double)res.f);
    else if ( IS_PID_DIGITAL(pid) ) return((double)res.i);
    else 	printf("Csget: Parameter hat falschen Typ %s\n",n);
  }
  return(0.0);
}


ARRAY *csvget(char *n,int nn, int o) {
  int pid,j;
  ROUTE route;
  ARRAY *ergebnis=malloc(sizeof(ARRAY));

  ergebnis->dimension=1;
  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) {
	printf("ERROR in ccs_convert_parametername_id\n");
	ergebnis->pointer=malloc(40);
	((int *)ergebnis->pointer)[0]=1;
        return(ergebnis);
  } else {
    if(nn==0) {
      /* get size of Vector */
      ccs_convert_pid_netroute( pid, &route );
      if (CCSERR) {
	printf("ERROR in ccs_convert_pid_netroute\n");
	nn=1;
      } else nn=route.size;
    }
    /* switch appropriate to type */

    if ( IS_PID_ANALOG(pid) ) {
        double *varptr;
	float *fpt;
        ergebnis->typ=FLOATARRAYTYP;
	ergebnis->pointer=malloc(1*INTSIZE+nn*sizeof(double));
	((int *)ergebnis->pointer)[0]=nn;
	varptr=ergebnis->pointer+INTSIZE;
	fpt=malloc(nn*sizeof(float));
	ccs_get_subvector( pid,fpt, &j, o, nn, NULL );
	for(j=0;j<nn;j++) varptr[j]=(double)fpt[j];
	free(fpt);
	if (CCSERR) printf("ERROR in ccs_get_subvector: %s\n", ccs_get_error_message());
	return(ergebnis);
    } else if ( IS_PID_DIGITAL(pid) ) {
        int *varptr;
        ergebnis->typ=INTARRAYTYP;
	ergebnis->pointer=malloc(1*INTSIZE+nn*sizeof(int));
	((int *)ergebnis->pointer)[0]=nn;
	varptr=ergebnis->pointer+INTSIZE;
	ccs_get_subvector( pid, varptr, &j,o,nn, NULL );
	if (CCSERR) printf("ERROR in ccs_get_subvector: %s\n", ccs_get_error_message());
	return(ergebnis);
    } else {
	xberror(46,n); /* Parameter hat falschen Typ */
	return(ergebnis);
    }
  }
}

char *csgets(char *n) {

  int pid;

  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) {
    printf("ERROR in ccs_convert_parametername_id\n");
    ccs_err=CCSERR;
    return(strdup("<ERROR>"));
  } else {

/* switch appropriate to type */

    if ( IS_PID_STRING(pid) ) {
      int bytes;
      char s[256];
      ccs_get_value( pid, s, &bytes, sizeof(s), NULL );
      if (CCSERR) {
        printf("ERROR in ccs_get_value: %s\n", ccs_get_error_message());
	ccs_err=CCSERR;
        return(strdup("<ERROR>"));
      }
      ccs_err=CCSERR;
      return(strdup(s));
    } else {
      xberror(47,n); /* Parameter hat falschen Typ */
      ccs_err=CCSERR;
      return(strdup("<ERROR>"));
    }
  }
}
void c_cssetcallback(char *n) {
  int i,flag,newpid,pc2;
  char w1[strlen(n)+1],w2[strlen(n)+1];
  char *test;
  int e=wort_sep(n,',',TRUE,w1,w2);
  if(e<2) {
    printf("Syntax-Error bei CSSETCALLBACK. <%s>\n",n);
    return;
  }
  test=s_parser(w1);
  ccs_convert_parametername_id(test, &newpid );
  if (CCSERR) {
	printf("ERROR in ccs_convert_parametername_id <%s>\n",test);
        free(test);
        ccs_err=CCSERR;
        return;
  }
  free(test);
  pc2=procnr(w2,1);
  if(pc2==-1)   xberror(19,w2); /* Procedure nicht gefunden */
  else {
    if(pidanz<MAXPIDS) {
    ccs_disable_ccs_interrupts();
    ccs_release_interest(pids, pidanz, NULL);
    flag=-1;
    for(i=0;i<pidanz;i++) {
      if(pids[i]==newpid) flag=i;
    }
    if(flag==-1) {
      pids[pidanz]=newpid;
      isubs[pidanz++]=pc2;
    } else isubs[flag]=pc2;
    ccs_declare_interest(pids, pidanz  );
    ccs_enable_ccs_interrupts();
    } else printf("Zu viele Callbacks. max. <%d>\n",MAXPIDS);
  }
}

void c_csclearcallbacks(char *n) {
  ccs_release_interest(pids, pidanz, NULL);
  ccs_disable_ccs_interrupts();
}

void c_csvput(char *w) {
  char n[strlen(w)+1],t[strlen(w)+1];
  char *test,*r;
  int pid,j,e,i,nn,typ,vnr,o=0;
  float f;
  ROUTE route;
  e=wort_sep(w,',',TRUE,n,t);
  if(e<2) {
    printf("Syntax-Error bei CSPUT. <%s>\n",w);
    return;
  }
  test=s_parser(n);
  ccs_convert_parametername_id(test, &pid );
  if (CCSERR) {
	printf("ERROR in ccs_convert_parametername_id <%s>\n",test);
        free(test);
        ccs_err=CCSERR;
        return;
  }
  free(test);
  e=wort_sep(t,',',TRUE,n,t);

      /* get size of Vector */
  ccs_convert_pid_netroute( pid, &route );
  if (CCSERR) {
    printf("ERROR in ccs_convert_pid_netroute\n");
    nn=1;
  } else nn=route.size;
  if(e==2) nn=min(nn,(int)parser(t));

     /* Typ bestimmem. Ist es Array ? */

    typ=type(n)&(~CONSTTYP);
    if(typ & ARRAYTYP) {
      r=varrumpf(n);
      vnr=variable_exist(r,typ);
      free(r);
      if(vnr==-1) xberror(15,n); /* Feld nicht dimensioniert */
      else {
        int anz=min(do_dimension(vnr),nn);
        if((typ&TYPMASK)!=FLOATTYP && (typ&TYPMASK)!=INTTYP) {printf("CSVPUT: Muss Float-ARRAY sein. \n");return;}


	/*  printf("CSVPUT: pid=%d, vnr=%d, nn=%d, anz=%d \n",pid,vnr,nn,anz); */

	if ( IS_PID_ANALOG(pid) ) {
          float *buffer=calloc(anz,sizeof(float));
	
	  if((typ&TYPMASK)==FLOATTYP) {
	    double *varptr=(double  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
	    for(i=0;i<anz;i++) buffer[i]=(float)varptr[i];
          } else if((typ&TYPMASK)==INTTYP) {
	    int *varptr=(int  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
	    for(i=0;i<anz;i++) buffer[i]=(float)varptr[i];
	  }
	
	  /* Schreiben !!! */
	  if(o) {
            ccs_put_subvector( pid,o,anz, buffer, &j);
	    if (CCSERR) printf("ERROR in ccs_put_subvector: %s\n", ccs_get_error_message());
	  } else {
	    ccs_put_value_secure( pid, anz, buffer, &j );
	    if (CCSERR)   printf("ERROR in ccs_set_value: %s\n", ccs_get_error_message());
	  }
	
          free(buffer);
	} else if ( IS_PID_DIGITAL(pid) ) {
	  int *buffer=calloc(anz,sizeof(int));
	  if((typ&TYPMASK)==FLOATTYP) {
	    double *varptr=(double  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
	    for(i=0;i<anz;i++) buffer[i]=(int)varptr[i];
          } else if((typ&TYPMASK)==INTTYP) {
	    int *varptr=(int  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
	    for(i=0;i<anz;i++) buffer[i]=(int)varptr[i];
	  }
	
	  /* Schreiben !!! */
	  if(o) {
	    ccs_put_subvector( pid,o,anz, buffer, &j);
	    if (CCSERR) printf("ERROR in ccs_put_subvector: %s\n", ccs_get_error_message());
	  } else {
	    ccs_put_value_secure( pid, anz, buffer, &j );
	    if (CCSERR)   printf("ERROR in ccs_set_value: %s\n", ccs_get_error_message());
	  }
	  free(buffer);
        } else printf("Csvput: Parameter hat falschen Typ %s\n",n);
      }
      ccs_err=CCSERR;
    } else printf("CSVPUT: Kein ARRAY. \n");
}



void c_csput(char *w) {


 char n[strlen(w)+1],t[strlen(w)+1];
 char *test;
 int pid,j,e,i;
 float f;
  e=wort_sep(w,',',TRUE,n,t);
  if(e<2) {
    printf("Syntax-Error bei CSPUT. <%s>\n",w);
    return;
  }
  test=s_parser(n);
  ccs_convert_parametername_id(test, &pid );
  if (CCSERR) {
	printf("ERROR in ccs_convert_parametername_id <%s>\n",test);
        free(test);
        ccs_err=CCSERR;
        return;
  }
  free(test);
/* switch appropriate to type */

    if ( IS_PID_ANALOG(pid) &&  ((type(t)&TYPMASK)==FLOATTYP ||(type(t)&TYPMASK)==INTTYP)) {
        f=(float)parser(t);
        ccs_put_value_secure( pid, 1, &f, &j );
	if (CCSERR)   printf("ERROR in ccs_set_value: %s\n", ccs_get_error_message());
    } else if ( IS_PID_DIGITAL(pid) && ((type(t)&TYPMASK)==FLOATTYP ||(type(t)&TYPMASK)==INTTYP)) {
	i=(int)parser(t);
	ccs_put_value_secure( pid, 1, &i, &j );
	if (CCSERR)   printf("ERROR in ccs_set_value: %s\n", ccs_get_error_message());
    } else if ( IS_PID_STRING(pid) && ((type(t)&TYPMASK)==STRINGTYP) ) {
        test=s_parser(t);
        ccs_put_value_secure( pid, strlen(test), test, &j );
	free(test);
	if (CCSERR)   printf("ERROR in ccs_set_value: %s\n", ccs_get_error_message());
    } else printf("Csput: Parameter hat falschen Typ %s\n",w);
  ccs_err=CCSERR;
}

void c_cssweep(char *w) {

/* CSSWEEP a$,endwert,stepweite,delay   */

 char n[strlen(w)+1],t[strlen(w)+1];
 char *test;
 int pid,j,e;
  e=wort_sep(w,',',TRUE,n,t);
  if(e<2) {
    printf("Syntax-Error bei CSSWEEP. <%s>\n",w);
    return;
  }
  test=s_parser(n);
  ccs_convert_parametername_id(test, &pid );
  if (CCSERR) {
	printf("ERROR in ccs_convert_parametername_id <%s>\n",test);
        free(test);
        return;
  }
  free(test);
/* switch appropriate to type */

    if ( IS_PID_ANALOG(pid) && (type(t)&TYPMASK)==FLOATTYP) {
        float end,deltatime;
	int nsteps;
	e=wort_sep(t,',',TRUE,n,t);
        end=(float)parser(n);
	e=wort_sep(t,',',TRUE,n,t);
        nsteps=(int)parser(n);
	e=wort_sep(t,',',TRUE,n,t);
        deltatime=(float)parser(n);
	if(e==0) printf("CSSWEEP: Zu wenig Parameter.\n");
        else {
	  float startvalue,minval,maxval,data;
	  ccs_get_value(pid, &startvalue, &j, 1, NULL );
          if (CCSERR) {
                 printf("CSSWEEP: %s\n", ccs_get_error_message());
		 return;
          }
	  /* get lower and upper boundary values */
          ccs_resource_read_parameter_minmax( pid, &minval, &maxval, NULL );
          if (CCSERR) {
                 printf("CSSWEEP: %s\n", ccs_get_error_message());
                 return;
          }
	  data=startvalue;
	  sweep_value( pid,startvalue, end, deltatime, nsteps );
	  ccs_put_value( pid, 1, &end, &j );  /* force last value to be exact */
	  if (CCSERR) {
		printf("CSSWEEP: %s\n", ccs_get_error_message());
		return;
	  }
        }
    } else printf("Cssweep: Parameter hat falschen Typ, muss analog sein. %s\n",w);
}

/* sweep for last value to new value 'v' within time dt, and use 'steps' steps */

void sweep_value( int pid, float lastvalue, float v, float dt, int steps ) {
  float ddt,dv,tv;
  int i, ret, wrt;

  dv=(v-lastvalue)/steps;  /* compute delta value */
  ddt=dt/steps;            /* compute intermediate delay */

	/* sweep value now */

	for (tv = lastvalue, i=0; i<steps; i++, tv += dv) {
		ccs_put_value( pid, 1, &tv, &wrt );
		if (CCSERR) {
			printf("CSSWEEP: %s\n", ccs_get_error_message());
			/* ignore intermediate errors */
		}
		ccs_wait(ddt); /* and wait intermediate delay */
	}
}

#endif


















/* Mit DESY-Kontrollsystem */

#ifdef TINE
#ifdef WINDOWS
  #include "rpctypes.h"
  #define RPCERR_SIZE 32
  extern char RPCLastErrorString[RPCERR_SIZE];
  extern unsigned short MaxPollingRate;
  extern int defaultClientTimeout;
  #define defaultClientTimeoutMsec defaultClientTimeout
  #include "servdef.h"
#else
  #include "tine/prolog.h"
  #include "tine/errors.h"
  #include "tine/rpctypes.h"
  #include "tine/toolkit.h"
  #include "tine/servdef.h"
#endif

extern int nofeclog;
extern int ListenForGlobals;
extern int ListenForMCasts;
extern int NGdebug;


/* Verhindert, dass Logfile angelegt wird und bestimmt die Geschwindigkeit der
   Abfragen */

void fix_tine_start() {
   /*    MaxNumConnections = 3;  */
  /*  ListenForGlobals = ListenForMCasts = FALSE; */
    /* MaxPollingRate = MinPollingRate = 200; */
    MaxPollingRate = MinPollingRate = 50;
   /* nofeclog = TRUE; */
    defaultClientTimeout=100;
    NGdebug=2;   /* hier wollen wir spaeter mal eine 0 haben */
}
extern void closeClients(void);

void tine_init() {
  static int flag=0;
  if(flag) return;
  fix_tine_start();
 
  initClient();
  atexit(closeClients);
  flag=1;
}



void tmonitorCallback(int id,int cc) {
  if (cc) printf("Callback id=%d : TRC error %d : %s\n",id,cc,GetRPCLastError(cc));
  else notify_handler(id,0,GetCompletionDataSizeFromCallbackId(id));
}

int my_ExecLinkEx(char *dev, char *prop, DTYPE *dout, DTYPE *din,short access,int buflen) {
  int rc,retry=0;
  fix_tine_start();
  rc=ExecLinkEx(dev,prop,dout,din,access,buflen);
  if(rc==98) {retry=4; defaultClientTimeout=1000;MaxPollingRate = MinPollingRate = 200;}
  while(retry-- && rc!=0) {
    if(rc) printf("retry(%d) with %d\n",retry,rc);
    rc=ExecLinkEx(dev,prop,dout,din,access,buflen);
  }
  return(rc);
}

int GetPropertyInformation(char *srv,char *prp,PropertyQueryStruct *srvProps){
  DTYPE dout,din;
  int cc = 0;

  din.dFormat = CF_NAME32;
  din.dArrayLength = 1;
  din.data.vptr = prp;
  dout.dFormat = CF_STRUCT;
  dout.dArrayLength = PROPERTYQUERYSTRUCT_SIZE;
  dout.data.vptr = srvProps;
  if((cc=my_ExecLinkEx(srv,"PROPS",&dout,&din,CA_READ|CA_RETRY,500))!=0) return cc;
  srvProps->prpSize=ISWAP(srvProps->prpSize);
  return 0;
}
#if 0
int tservers(char *cnt) {
  int i,cc,n = 256;
  NAME16 srvs[256];
  if ((cc=GetDeviceServersEx(cnt,srvs,&n)) != 0) return cc;
  printf("%d servers found :\n",n);
  for (i=0; i<n; i++)
  {
    printf("%16s ",srvs[i].name);
    if ((i+1)%4 == 0) printf("\n");
  }
  return 0;
}
int tprops(char *dev) {
  int i,cc,n = 256;
  NAME32 prps[256];
  if ((cc=GetDeviceProperties(dev,prps,&n)) != 0) return cc;
  printf("%d properties found :\n",n);
  for (i=0; i<n; i++)
  {
    printf("%32s ",prps[i].name);
    if ((i+1)%4 == 0) printf("\n");
  }
  return 0;
}
int tdevs(char *dev) {
  int i,cc,n = 256;
  NAME16 devs[256];
  char devname[32];
  if ((cc=GetDeviceNames(dev,devs,&n)) != 0) return cc;
  printf("%d devices found :\n",n);
  for (i=0; i<n; i++)
  {
    strncpy(devname,devs[i].name,16);
    if (devname[15] == '&')
    {
      i++; devname[15] = 0;
      strncat(devname,devs[i].name,16);
    }
    printf("%32s ",devname);
    if ((i+1)%4 == 0) printf("\n");
  }
  return 0;
}
#endif

/* DEVICE[PROPERTY]     */

int convert_name_convention(char *input,char *device, char *property){
  int flag=0;
  while(*input!='[' && *input!=0) {*device=*input;device++;input++;}
  *device=0;
  if(*input!='[') return(-1);
   input++;
  while((*input!=']' || flag) && *input!=0) {
    if(*input==']' && flag) flag--;
    else if(*input=='[') flag++;
    *property=*input; property++;input++;
  }
   if(*input!=']') return(-2);
  *property=0;
  return(0);
}
int tinesize(char *n) {
  char dev[strlen(n)+1],prop[strlen(n)+1];
  if(convert_name_convention(n,dev,prop)) printf("Syntax-Error in Parameter-Name %s\n",n);
  else {
    PropertyQueryStruct prpinfo;
    /* get size of Vector */
    if(ccs_err=GetPropertyInformation(dev,prop,&prpinfo))
      printf("Tine-Error: %d  %s\n",ccs_err,erlst[ccs_err]);
    else return(prpinfo.prpSize);
  }
  return(0);
}
int tinetyp(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  if(ccs_err=convert_name_convention(n,w1,w2))
	printf("Syntax-Error %d in Parameter-Name %s\n",ccs_err,n);
  else {
    PropertyQueryStruct prpinfo;

    /* get size of Vector */
    ccs_err=GetPropertyInformation(w1,w2,&prpinfo);
    if(ccs_err) printf("Tine-Error: %d  %s\n",ccs_err,erlst[ccs_err]);
    else {
      int typ=0,f=LFMT(prpinfo.prpFormat);
      if(f==CF_BYTE) typ=typ|STRINGTYP;
      else if(f==CF_SHORT) typ=typ|INTTYP;
      else if(f==CF_LONG) typ=typ|INTTYP;
      else if(f==CF_FLOAT) typ=typ|FLOATTYP;
      else if(f==CF_DOUBLE) typ=typ|FLOATTYP;
      else if(f==CF_TEXT) typ=typ|STRINGTYP;
      else if(f==CF_NAME16) typ=typ|STRINGTYP;
      else if(f==CF_NAME32) typ=typ|STRINGTYP;
      else printf("Unbekannter Paramertertyp: %d\n",prpinfo.prpFormat);
      if(prpinfo.prpSize>1) typ=typ|ARRAYTYP;
      return(typ);
    }
  }
  return(0);
}
char *tineunit(char *n) {
  char *unit=malloc(80);
  char w1[strlen(n)+1],w2[strlen(n)+1];
  ccs_err=convert_name_convention(n,w1,w2);
  if(ccs_err) printf("Syntax-Error %d in Parameter-Name %s\n",ccs_err,n);
  else {
    float mmax,mmin;
    fix_tine_start();
    ccs_err=GetDevicePropertyEGU(w1,w2,&mmax,&mmin,unit);
    if(ccs_err) printf("Tine-Error: %d  %s\n",ccs_err,erlst[ccs_err]);
  }
  return(unit);
}
char *tineinfo(char *n) {
  char *des=malloc(80);
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc=convert_name_convention(n,w1,w2);
  ccs_err=rc;
  if(rc)
	printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
  else {
    PropertyQueryStruct prpinfo;
    rc=GetPropertyInformation(w1,w2,&prpinfo);
    strncpy(des,prpinfo.prpDescription,80);
    if(rc) printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
  }
  return(des);
}
double tinemin(char *n) {
  char unit[80];float mmax,mmin;
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc=convert_name_convention(n,w1,w2);
  ccs_err=rc;
  if(rc)
	printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
   else {
    float mmax,mmin;
    fix_tine_start();
    rc=GetDevicePropertyEGU(w1,w2,&mmax,&mmin,unit);
     if(rc) printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
  }
  return((double)mmin);
}
double tinemax(char *n) {
  char unit[80];float mmax,mmin;
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc=convert_name_convention(n,w1,w2);
  ccs_err=rc;
  if(rc)  printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
  else {
    fix_tine_start();
    rc=GetDevicePropertyEGU(w1,w2,&mmax,&mmin,unit);
    ccs_err=rc;
    if(rc) printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
  }
  return((double)mmax);
}
double tineget(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc=convert_name_convention(n,w1,w2);
  char *buf;
  int buflen;
  double ergeb;
  if(rc)
	printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
  else {
    PropertyQueryStruct prpinfo;
    DTYPE dout;

    /* get size of Vector */
    rc=GetPropertyInformation(w1,w2,&prpinfo);
    if(rc) {
      printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
      return(0);
    }
    buflen=prpinfo.prpSize*getFormatSize(LFMT(prpinfo.prpFormat));
    buf=malloc(buflen);
    dout.dFormat = LFMT(prpinfo.prpFormat);
    dout.dArrayLength = prpinfo.prpSize;
    dout.dTag[0] = 0;
    dout.data.vptr = buf;
    rc=my_ExecLinkEx(w1,w2,&dout,NULL,CA_READ,buflen);
    if(rc) {
      printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
    } else {
      switch (LFMT(prpinfo.prpFormat)) {
    case CF_BYTE:
      ergeb=(double)(*(char *)buf); break;
    case CF_SHORT:
      ergeb=(double)(*(short *)buf);break;
    case CF_LONG:
      ergeb=(double)(*(long *)buf);break;
    case CF_FLOAT:
      ergeb=(double)(*(float *)buf);break;
    case CF_DOUBLE:
      ergeb=(double)(*(double *)buf);break;
    default: printf("output format type %d is not a number !\n",LFMT(prpinfo.prpFormat));
      }
    }
    free(buf);
  }
  ccs_err=rc;
  return(ergeb);
}

STRING tinegets(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc=convert_name_convention(n,w1,w2);
  char *buf;
  int buflen;
  STRING ergebnis;

  if(rc) {
	printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
        ergebnis=vs_error();
  } else {
    PropertyQueryStruct prpinfo;
    DTYPE dout;

    /* get size of Vector */
    rc=GetPropertyInformation(w1,w2,&prpinfo);
    if(rc) {
      printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
        ergebnis=vs_error();
    } else {
      buflen=prpinfo.prpSize*getFormatSize(LFMT(prpinfo.prpFormat));
      buf=malloc(buflen);
      dout.dFormat = LFMT(prpinfo.prpFormat);
      dout.dArrayLength = prpinfo.prpSize;
      dout.dTag[0] = 0;
      dout.data.vptr = buf;
      rc=my_ExecLinkEx(w1,w2,&dout,NULL,CA_READ,buflen);
      if(rc) {
        printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
	free(buf);
	ergebnis=vs_error();
      } else {
#if 0
        switch (LFMT(prpinfo.prpFormat)) {
          case CF_BYTE:
          case CF_TEXT:
          case CF_NAME16:
          case CF_NAME32:  break;
          default: printf("output format type %d is not a string !\n",LFMT(prpinfo.prpFormat));
        }
#endif
	ergebnis.pointer=buf;
	ergebnis.len=buflen;
      }
    }

  }
  ccs_err=rc;
  return(ergebnis);
}

/* Holt einen Wert, wobei ein bestimmter Timestamp vorgegeben werden kann */
/* Rueckgabe ist immer ein STring, auch wenn es sich um Arrays oder floats etc
   handelt.    Allgemeinste Form des Tine-Zugriffs.    */

STRING tinequery(char *n,int start) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc=convert_name_convention(n,w1,w2);
  char *buf;
  int buflen;
  STRING ergebnis;

  if(rc) {
	printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
        ergebnis=vs_error();
  } else {
    PropertyQueryStruct prpinfo;
    DTYPE dout,din;

    /* get size of Vector */
    rc=GetPropertyInformation(w1,w2,&prpinfo);
    if(rc) {
      printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
        ergebnis=vs_error();
    } else {
      buflen=prpinfo.prpSize*getFormatSize(LFMT(prpinfo.prpFormat));
      buf=malloc(buflen);
      dout.dFormat = LFMT(prpinfo.prpFormat);
      dout.dArrayLength = prpinfo.prpSize;
      dout.dTag[0] = 0;
      dout.data.vptr = buf;
      din.dArrayLength = 1;
      din.dFormat = CF_LONG;
      din.data.lptr = (UINT32 *)&start;
      rc=my_ExecLinkEx(w1,w2,&dout,&din,CA_READ,buflen);
      if(rc) {
        printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
	free(buf);
	ergebnis=vs_error();
      } else {
	ergebnis.pointer=buf;
	ergebnis.len=buflen;
      }
    }

  }
  ccs_err=rc;
  return(ergebnis);
}
ARRAY tinevget(char *n,int nn, int o) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc=convert_name_convention(n,w1,w2);
  char *buf;
  int buflen;
  int f,j;
  ARRAY ergebnis;

  ergebnis.dimension=1;
  if(rc) {
    printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
    ergebnis.pointer=malloc(40);
    ((int *)ergebnis.pointer)[0]=1;
  } else {
      /* get size of Vector */

    PropertyQueryStruct prpinfo;
    DTYPE dout;

    rc=GetPropertyInformation(w1,w2,&prpinfo);
    if(rc) {
      printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
      ergebnis.pointer=malloc(40);
      ((int *)ergebnis.pointer)[0]=1;
      return(ergebnis);
    }
    if(nn==0) nn=prpinfo.prpSize;
    nn=min(nn,prpinfo.prpSize-o);
    buflen=prpinfo.prpSize*getFormatSize(LFMT(prpinfo.prpFormat));
    buf=malloc(buflen);
    dout.dFormat = LFMT(prpinfo.prpFormat);
    dout.dArrayLength = prpinfo.prpSize;
    dout.dTag[0] = 0;
    dout.data.vptr = buf;
    rc=my_ExecLinkEx(w1,w2,&dout,NULL,CA_READ,buflen);
    ccs_err=rc;
    if(rc) {
      printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
      free(buf);
      ergebnis.pointer=malloc(40);
      ((int *)ergebnis.pointer)[0]=1;
      return(ergebnis);
    }

    /* switch appropriate to type */

    f=LFMT(prpinfo.prpFormat);
    if(f==CF_FLOAT) {
      double *varptr;
      ergebnis.typ=FLOATARRAYTYP;
      ergebnis.pointer=malloc(1*INTSIZE+nn*sizeof(double));
      ((int *)ergebnis.pointer)[0]=nn;
      varptr=ergebnis.pointer+INTSIZE;
      for(j=0;j<nn;j++) varptr[j]=(double)(((float *)buf)[j+o]);
      free(buf);
    } else if(f==CF_DOUBLE) {
      double *varptr;
      ergebnis.typ=FLOATARRAYTYP;
      ergebnis.pointer=malloc(1*INTSIZE+nn*sizeof(double));
      ((int *)ergebnis.pointer)[0]=nn;
      varptr=ergebnis.pointer+INTSIZE;
      for(j=0;j<nn;j++) varptr[j]=(double)(((double *)buf)[j+o]);
      free(buf);
    } else if(f==CF_BYTE) {
      double *varptr;
      ergebnis.typ=FLOATARRAYTYP;
      ergebnis.pointer=malloc(1*INTSIZE+nn*sizeof(double));
      ((int *)ergebnis.pointer)[0]=nn;
      varptr=ergebnis.pointer+INTSIZE;
      for(j=0;j<nn;j++) varptr[j]=(double)(((char *)buf)[j+o]);
      free(buf);
    } else if(f==CF_SHORT) {
      double *varptr;
      ergebnis.typ=FLOATARRAYTYP;
      ergebnis.pointer=malloc(1*INTSIZE+nn*sizeof(double));
      ((int *)ergebnis.pointer)[0]=nn;
      varptr=ergebnis.pointer+INTSIZE;
      for(j=0;j<nn;j++) varptr[j]=(double)(((short *)buf)[j+o]);
      free(buf);
    } else if(f==CF_LONG) {
      double *varptr;
      ergebnis.typ=FLOATARRAYTYP;
      ergebnis.pointer=malloc(1*INTSIZE+nn*sizeof(double));
      ((int *)ergebnis.pointer)[0]=nn;
      varptr=ergebnis.pointer+INTSIZE;
      for(j=0;j<nn;j++) varptr[j]=(double)(((long *)buf)[j+o]);
      free(buf);
    } else {
      printf("output format type %d is not implemented !\n",LFMT(prpinfo.prpFormat));
      xberror(46,n); /* Parameter hat falschen Typ */
      free(buf);
      ergebnis.pointer=malloc(40);
      ((int *)ergebnis.pointer)[0]=1;
    }
  }
 return(ergebnis);
}
ARRAY tinehistory(char *n,int start, int stop) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc=convert_name_convention(n,w1,w2);
  char *buf;
  int buflen,nn;
  int f,j;
  ARRAY ergebnis;
  UINT32 startstopArray[3];
  DTYPE dout,din;
  double *varptr;

  ergebnis.dimension=2;

  if(rc) {
    printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
    ergebnis.pointer=malloc((4+2)*sizeof(int));
    ergebnis.typ=INTTYP;
    ((int *)ergebnis.pointer)[0]=1;
    ((int *)ergebnis.pointer)[1]=1;
  } else {
      /* get size of History */

    buflen=65532;
    buf=malloc(buflen);

    startstopArray[0] = start;
    startstopArray[1] = stop;

    dout.dFormat = CF_FLTINT;
    dout.dArrayLength = (int)buflen/8;
    dout.dTag[0] = 0;
    dout.data.vptr = buf;
    din.dArrayLength = 2;
    din.dFormat = CF_LONG;
    din.data.lptr = startstopArray;
    rc=my_ExecLinkEx(w1,w2,&dout,&din,CA_READ,buflen);
    ccs_err=rc;
    if(rc) {
      printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
      free(buf);
      ergebnis.pointer=malloc((4+2)*sizeof(int));
      ergebnis.typ=INTTYP;
      ((int *)ergebnis.pointer)[0]=1;
      ((int *)ergebnis.pointer)[1]=1;
      return(ergebnis);
    }
  /*    nn=((int *)buf)[1];  */
  /*  nn=dout.dArrayLength;  */
    nn=GetCompletionDataSize(-1);
    ergebnis.typ=FLOATARRAYTYP;
    ergebnis.pointer=malloc(2*INTSIZE+2*nn*sizeof(double));
    ((int *)ergebnis.pointer)[0]=nn;
    ((int *)ergebnis.pointer)[1]=2;
    varptr=ergebnis.pointer+2*INTSIZE;
      for(j=0;j<nn;j++) {
        varptr[2*j]=(double)(((float *)buf)[2*j]);  /*+2 entfernt*/
        varptr[2*j+1]=(double)(((int *)buf)[2*j+1]);/*+2 entfernt*/
      }
      free(buf);
  }
  return(ergebnis);
}

/* tinemonitor "sender[PROP]",callback,interval */

void c_tinemonitor(PARAMETER *plist, int e) {
  static PropertyQueryStruct prpinfo;
  char dev[plist[0].integer+1],prop[plist[0].integer+1];
  int rc=convert_name_convention(plist[0].pointer,dev,prop);
  if(rc) printf("Syntax-Error in Parameter-Name %s\n",plist[0].pointer);
  else {
    tine_init();
    printf("dev=<%s>, prop=<%s>\n",dev,prop);
    ccs_err=GetPropertyInformation(dev,prop,&prpinfo);
    if(ccs_err) printf("Tine-Error: %d  %s\n",ccs_err,erlst[ccs_err]);
    else {
      DTYPE dout;
      int newpid,buflen=prpinfo.prpSize*getFormatSize(LFMT(prpinfo.prpFormat));
      char *buf=malloc(buflen);
      dout.dFormat = LFMT(prpinfo.prpFormat);
      dout.dArrayLength = prpinfo.prpSize;
      dout.dTag[0] = 0;
      dout.data.vptr = buf; /* wird nirgendwo freigegeben */
      newpid=AttachLink(dev,prop,&dout,NULL,CA_READ,100,tmonitorCallback,CM_REFRESH|CM_NETWORK);
      if(newpid<0) {
        ccs_err=-newpid;
	printf("Tine-Error AttachLink: %d  %s\n",ccs_err,erlst[ccs_err]);
	free(buf);
      } else {
	if(pidanz<MAXPIDS) {
          int i,flag;
            flag=-1;
            for(i=0;i<pidanz;i++) {
              if(pids[i]==newpid) flag=i;
            }
            if(flag==-1) {
              pids[pidanz]=newpid;
              isubs[pidanz++]=plist[1].integer;
            } else isubs[flag]=plist[1].integer;
        } else printf("Zu viele Callbacks. max. <%d>\n",MAXPIDS);
      }
    }
  }
}

void c_tineput(char *w) {
  char n[strlen(w)+1],t[strlen(w)+1];
  int pid,j,e,i;
  float f;
  e=wort_sep(w,',',TRUE,n,t);
  if(e<2) {
    printf("Syntax-Error bei TINEPUT. <%s>\n",w);
    return;
  } else {
    char *test=s_parser(n);
    char w1[strlen(test)+1],w2[strlen(test)+1];
    ccs_err=convert_name_convention(test,w1,w2);
    free(test);
    if(ccs_err) printf("Syntax-Error %d in Parameter-Name %s\n",ccs_err,test);
    else {
      PropertyQueryStruct prpinfo;
      DTYPE dout;
      int buflen;
      char *buf;
    /* get size of Vector */
      ccs_err=GetPropertyInformation(w1,w2,&prpinfo);
      if(ccs_err) {
        printf("Tine-Error: %d  %s\n",ccs_err,erlst[ccs_err]);
        return;
      }
      buflen=prpinfo.prpSize*getFormatSize(LFMT(prpinfo.prpFormat));
      buf=malloc(buflen);
      dout.dFormat = LFMT(prpinfo.prpFormat);
      dout.dArrayLength = prpinfo.prpSize;
      dout.dTag[0] = 0;
      dout.data.vptr = buf;

      if(type(t)&ARRAYTYP) {
        ARRAY abuffer=array_parser(t);
	int l=anz_eintraege(&abuffer);
	int i;
	void *ptr=(char *)abuffer.pointer+abuffer.dimension*INTSIZE;
	l=min(l,dout.dArrayLength);
        printf("Ist Vektor [%d]!!!\n",anz_eintraege(&abuffer));
	if(abuffer.typ==FLOATTYP ||abuffer.typ==INTTYP)) {
	  /* Tu was hier !!! */
	  switch (LFMT(prpinfo.prpFormat)) {
	  case CF_FLOAT:
	    if(abuffer.typ==FLOATTYP) {
  	      for(i=0;i<l;i++) ((float *)buf)[i]=(float)((double *)ptr)[i];
	    } else {
  	      for(i=0;i<l;i++) ((float *)buf)[i]=(float)((int *)ptr)[i];
	    }
	    break;
	  case CF_DOUBLE:
	    if(abuffer.typ==FLOATTYP) {
  	      for(i=0;i<l;i++) ((double *)buf)[i]=((double *)ptr)[i];
	    } else {
  	      for(i=0;i<l;i++) ((double *)buf)[i]=(double)((int *)ptr)[i];
	    }
	    break;
	  case CF_BYTE:
	    if(abuffer.typ==FLOATTYP) {
  	      for(i=0;i<l;i++) ((char *)buf)[i]=(char)((double *)ptr)[i];
	    } else {
  	      for(i=0;i<l;i++) ((char *)buf)[i]=(char)((int *)ptr)[i];
	    }
	    break;
	  case CF_SHORT:
	    if(abuffer.typ==FLOATTYP) {
  	      for(i=0;i<l;i++) ((short *)buf)[i]=(short)((double *)ptr)[i];
	    } else {
  	      for(i=0;i<l;i++) ((short *)buf)[i]=(short)((int *)ptr)[i];
	    }
	    break;  	
	  case CF_LONG:
	    if(abuffer.typ==FLOATTYP) {
  	      for(i=0;i<l;i++) ((long *)buf)[i]=(long)((double *)ptr)[i];
	    } else {
  	      for(i=0;i<l;i++) ((long *)buf)[i]=(long)((int *)ptr)[i];
	    }
	    break;  	
	  default:
            printf("output format type %d is not numeric !\n",LFMT(prpinfo.prpFormat));
	    free_array(&abuffer);
            free(buf);return;
	  }
	} else if((type(t)&TYPMASK)==STRINGTYP) {
          switch (LFMT(prpinfo.prpFormat)) {
          case CF_BYTE:
          case CF_TEXT:
          case CF_NAME8:
          case CF_NAME16:
          case CF_NAME16FI:
          case CF_NAME32:
          case CF_NAME48:
          case CF_STRUCT:
	  /* Tu was hier !!! */
	  printf("Art der Uebergabe noch nicht realisiert !\n");
          break;
          default:
            printf("output format type %d is not a Stringtype !\n",LFMT(prpinfo.prpFormat));
            free(buf);free_array(&abuffer); return;
          }

	}
	
	free_array(&abuffer);
      } else {
        /* switch appropriate to type */
        if((type(t)&TYPMASK)==FLOATTYP ||(type(t)&TYPMASK)==INTTYP) {
          switch (LFMT(prpinfo.prpFormat)) {
          case CF_BYTE:
            *((char *)buf)=(char)parser(t); break;
          case CF_SHORT:
            *((short *)buf)=(short)parser(t);break;
          case CF_LONG:
            *((long *)buf)=(long)parser(t);break;
          case CF_FLOAT:
            *((float *)buf)=(float)parser(t);break;
          case CF_DOUBLE:
            *((double *)buf)=(double)parser(t);break;
          default:
            printf("output format type %d is not a number !\n",LFMT(prpinfo.prpFormat));
            free(buf);return;
          }
        } else if((type(t)&TYPMASK)==STRINGTYP) {
          STRING sss=string_parser(t);
	  memcpy(buf,sss.pointer,min(sss.len,buflen));
	  free(sss.pointer);
	  buflen=min(sss.len,buflen);
#if 0
        printf("output format type %d  !\n",dout.dFormat);
	dout.dFormat=CF_NAME16FI;
#endif
        dout.dArrayLength = buflen/getFormatSize(LFMT(prpinfo.prpFormat));

#if 0
      /* Typen ueberpruefen */
      switch (LFMT(prpinfo.prpFormat)) {
      case CF_BYTE:
      case CF_TEXT:
      case CF_NAME8:
      case CF_NAME16:
      case CF_NAME16FI:
      case CF_NAME32:
      case CF_NAME48:
      case CF_STRUCT:
        break;
      default:
        printf("output format type %d is not a Stringtype !\n",LFMT(prpinfo.prpFormat));
        free(buf);return;
      }
      printf("Gebe #%d Daten\n",buflen);
#endif
        }
      }
      ccs_err=my_ExecLinkEx(w1,w2,NULL,&dout,CA_WRITE,buflen);
      if(ccs_err) printf("Tine-Error: %d  %s\n",ccs_err,erlst[ccs_err]);
      free(buf);
    }
  }
}

char tine_eqn[32]="X11BASIC";
int tineexportvars[100];
int anztineexportvars=0;

int tineserver_callback(char *devName,char *Property, DTYPE *dout, DTYPE *din, short access) {
  int devicenumber=0,vnr=-1,typ,etyp,len;
  char *r;
  if(devName[0] == '#') devicenumber=atoi(&devName[1]);
  else devicenumber=GetDeviceNumber(tine_eqn,devName);
  typ=vartype(Property);
  r=varrumpf(Property);
  vnr=variable_exist(r,typ);
  free(r);
  printf("Got request for %s ",Property);
  printf("VNR=%d ",vnr);
  if(vnr==-1) return illegal_property;
  else {
    len=do_dimension(vnr);
    switch(typ&TYPMASK) {
    case STRINGTYP: etyp=CF_TEXT; break;
    case INTTYP:    etyp=CF_LONG; break;
    case FLOATTYP:  etyp=CF_DOUBLE; break;
    }

    if(access&CA_WRITE) {
      printf("write access: \n");
      if(din->dFormat!=etyp) return illegal_format;
      if(din->dArrayLength > len) return dimension_error;
      if(typ & ARRAYTYP) ;
      else if((typ&TYPMASK)==STRINGTYP) {
        STRING a;
	a.pointer=din->data.vptr;
	a.len=din->dArrayLength;  /* wirklich ??? */
	string_zuweis(&variablenn[vnr],a);
      }
      else if((typ&TYPMASK)==INTTYP) variablen[vnr].opcode=din->data.lptr[0];
      else if((typ&TYPMASK)==FLOATTYP) variablen[vnr].zahl=din->data.dptr[0];

    } else if(access&CA_READ) {
        printf("read access: Format=%d ALEN=%d\n",dout->dFormat,dout->dArrayLength);
  
      if(dout->dFormat!=etyp) return illegal_format;
      if(dout->dArrayLength < len) return dimension_error;
      if(typ & ARRAYTYP) ;
      else if((typ&TYPMASK)==STRINGTYP) {
        dout->data.vptr=variablen[vnr].pointer;
	dout->dArrayLength=variablen[vnr].opcode;
      } else if((typ&TYPMASK)==INTTYP) dout->data.lptr[0]=variablen[vnr].opcode;
      else if((typ&TYPMASK)==FLOATTYP) dout->data.dptr[0]=variablen[vnr].zahl;

    }
    return(0);
  }
}
#define MY_TINE_TCP_PORT        41
void c_tineserver(PARAMETER *plist, int e) {
  int i,cc,port_offset=MY_TINE_TCP_PORT;
  if(e>0) strncpy(tine_eqn,plist[0].pointer,31);
  if(e>1) port_offset=plist[0].integer;
  if((i=RegisterFecNameEx(tine_eqn,tine_eqn,
      "UNIX","Geb.24","X11Basic","M. Hoffmann",port_offset,"DEFAULT"))!=0)
      printf("Register FecName %s error: %d %s\n",tine_eqn,i,erlst[i]);
  else {
    /* init RPC server: */
    fix_tine_start();
    if((cc=SystemInit(FALSE))!=0) printf("SystemInit error: %s\n",erlst[cc]);
    else RegisterEquipmentModule(tine_eqn,tine_eqn,1,tineserver_callback,NULL,NULL,1000,NULL);
  }
}
void completion_callback(int id,int cc) {  
  #ifdef DEBUG
    printf("Completion Callback id=%d rc=%d!\n",id,cc);
  #endif
  if (cc) {
    printf("TRC error : %s\n",GetRPCLastError(cc));
    return;
  }
}
void c_tinebroadcast(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1],*r;
  int p,typ,vnr,rc;
  double tolerance=0;
  int minPeriod=-1,maxPeriod=-1;
  DTYPE din;
  strcpy(v,n);
  p=wort_sep(v,',',TRUE,w,v);
  xtrim(w,TRUE,w);
  typ=type(w);
  if(typ & CONSTTYP) xberror(32,"TINEBROADCAST");  /* Syntax error */
  else {
    r=varrumpf(w);
    vnr=variable_exist(r,typ);
    if(typ & ARRAYTYP) { /* ganzes Array  */
      if(vnr==-1) xberror(15,w); /* Feld nicht dimensioniert */   
      din.dFormat=CF_BYTE;
      din.dArrayLength=do_dimension(vnr);
      din.data.vptr=variablen[vnr].pointer;
    } else {
      /*Hier sollte man die Variable anlegen, falls nicht existent...*/
      if(vnr==-1) vnr=variable_exist_or_create(r,typ);
      switch(typ&TYPMASK) {
      case STRINGTYP:
	  din.dFormat=CF_TEXT;
	  din.dArrayLength=variablen[vnr].opcode;
  	  din.data.vptr=variablen[vnr].pointer;
	  break;
      case INTTYP:
          din.dFormat=CF_LONG;
	  din.dArrayLength=1;
          din.data.lptr=&variablen[vnr].opcode;
	  break;
      case FLOATTYP:
          din.dFormat=CF_DOUBLE;
	  din.dArrayLength=1;
          din.data.dptr=&variablen[vnr].zahl;
	  break; 
      } 

      din.dTag[0] = 0;

      rc=sendNetGlobal(w,&din,completion_callback,minPeriod,maxPeriod,tolerance); 
      if(rc<0) {
        printf("Tine-Error recvNetGlobal: %d  %s\n",-rc,erlst[-rc]);
	ccs_err=-rc;
      } 
    }
    free(r);
  }
}
void c_tinelisten(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1],*r;
  int i,flag,newpid,pc2=-1,p,typ,vnr,rc;
  DTYPE dout;
  strcpy(v,n);
  p=wort_sep(v,',',TRUE,w,v);
  xtrim(w,TRUE,w);
  typ=type(w);
  if(typ & CONSTTYP) xberror(32,"TINELISTEN");  /* Syntax error */
  else {
    r=varrumpf(w);
    vnr=variable_exist(r,typ);
    if(p>1) {
      pc2=procnr(v,1);
      if(pc2==-1)   xberror(19,v); /* Procedure nicht gefunden */
    }
    if(typ & ARRAYTYP) { /* ganzes Array  */
      if(vnr==-1) xberror(15,w); /* Feld nicht dimensioniert */
      
        dout.dFormat=CF_BYTE;
	dout.dArrayLength=do_dimension(vnr);
        dout.data.vptr=variablen[vnr].pointer;
    } else {
      /*Hier sollte man die Variable anlegen, falls nicht existent...*/
      if(vnr==-1) vnr=variable_exist_or_create(r,typ);
      switch(typ&TYPMASK) {
      case STRINGTYP:
	  dout.dFormat=CF_TEXT;
	  dout.dArrayLength=variablen[vnr].opcode;
  	  dout.data.vptr=variablen[vnr].pointer;
	  break;
      case INTTYP:
          dout.dFormat=CF_LONG;
	  dout.dArrayLength=1;
          dout.data.lptr=&variablen[vnr].opcode;
	  break;
      case FLOATTYP:
          dout.dFormat=CF_DOUBLE;
	  dout.dArrayLength=1;
          dout.data.dptr=&variablen[vnr].zahl;
	  break; 
      } 
    }
    dout.dTag[0] = 0;
    rc=recvNetGlobal(w,&dout,tmonitorCallback);
    if(rc<0) {
        printf("Tine-Error recvNetGlobal: %d  %s\n",-rc,erlst[-rc]);
	ccs_err=-rc;
    } else {
        /* Callback registrieren */
        if(pc2!=-1) {
   	  newpid=rc;
	  if(pidanz<MAXPIDS) {
            flag=-1;
            for(i=0;i<pidanz;i++) {
              if(pids[i]==newpid) flag=i;
            }
            if(flag==-1) {
              pids[pidanz]=newpid;
              isubs[pidanz++]=pc2;
            } else isubs[flag]=pc2;
          } else printf("Zu viele Callbacks. max. <%d>\n",MAXPIDS);
        }
    }
    free(r);
  }
}
void c_tineexport(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1],*r;
  int p,len=1,typ,vnr,etyp;
  strcpy(v,n);
  p=wort_sep(v,',',TRUE,w,v);
  while(p) {
    xtrim(w,TRUE,w);
    typ=type(w);
    if(typ & CONSTTYP) xberror(32,"TINEEXPORT");  /* Syntax error */
    else {
      r=varrumpf(w);
      vnr=variable_exist(r,typ);
      if(typ & ARRAYTYP) { /* ganzes Array  */
        if(vnr==-1) xberror(15,w); /* Feld nicht dimensioniert */
        else len=do_dimension(vnr);
      }
      switch(typ&TYPMASK) {
      case STRINGTYP:   etyp=CF_TEXT; break;
      case INTTYP:      etyp=CF_LONG; break;
      case FLOATTYP:    etyp=CF_DOUBLE; break;
      }
      RegisterProperty(tine_eqn,w,len,etyp,CA_WRITE|CA_READ,"X11-Basic variable");
      /*Hier sollte man die Variable anlegen, falls nicht existent...*/
      if(vnr==-1) vnr=variable_exist_or_create(r,typ);
      tineexportvars[anztineexportvars++]=vnr;
      free(r);
    }
    p=wort_sep(v,',',TRUE,w,v);
  }
}

void c_tinedeliver(char *n) {
  if((ccs_err=SystemScheduleProperty(tine_eqn,n))!=0) {
    printf("SystemScheduleProperty Failed: %s\n",GetRPCLastError(ccs_err));
  }
}

void c_tinecycle(char *n) {
  SystemCycle(FALSE);
}
#endif


#ifdef HAVE_MQTT
#include <MQTTClient.h>
#include <time.h>
char clientID[64];
#define TIMEOUT     10000L
MQTTClient client;
int mqtt_isconnected=0;
volatile MQTTClient_deliveryToken deliveredtoken;
typedef struct {
  char *topic;
  int vnr;
  char *ptr;
  int procnr;
} SUBSCRIPTION;

SUBSCRIPTION subscriptions[100];
int anzsubscription;



/* This is a callback function. 
   The client application must provide an implementation of this function
to enable asynchronous notification of the loss of connection to the server.
It is
called by the client library if the client loses its connection to the server. The client
application must take appropriate action, such as trying to reconnect or reporting the problem. This
function is executed on a separate thread to the one on which the client application is running.
*/

void connlost(void *context, char *cause) {
  printf("\nMQTT-Connection lost\n");
  printf("     cause: %s\n", cause);
  
  /* TODO: sleep a while and then try to reconnect....*/
  
  
}
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message) {
  int i;
#if DEBUG
  char* payloadptr;
  printf("Message arrived\n");
  printf("     topic: %s\n", topicName);
  printf("   message: ");
  payloadptr = message->payload;
  for(i=0; i<message->payloadlen; i++) putchar(*payloadptr++);
  putchar('\n');
#endif
  STRING a;
  a.pointer=message->payload;
  a.len=message->payloadlen;
  for(i=0;i<anzsubscription;i++) {
    if(!strcmp(topicName,subscriptions[i].topic)) { /* Subscription found! */
      if(subscriptions[i].vnr>=0) varcaststring(subscriptions[i].vnr,subscriptions[i].ptr,a);
      if(subscriptions[i].procnr>=0) {
        /* create two local variables topic$ and message$ */
	PARAMETER p[2];
	p[0].typ=PL_STRING;
	p[0].pointer=topicName;
	p[0].integer=strlen(topicName);
	p[1].typ=PL_STRING;
	p[1].pointer=message->payload;
	p[1].integer=message->payloadlen;
//	printf("Topic: <%s> (%d)\n",topicName,topicLen);
	call_sub_with_parameterlist(subscriptions[i].procnr,&p[0],2);
      }
      break;
    }
  }
  MQTTClient_freeMessage(&message);
  MQTTClient_free(topicName);
  return 1;
}
void delivered(void *context, MQTTClient_deliveryToken dt) {
 //   printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

void mqtt_init() {
  atexit(mqtt_exit);
}
static void mqtt_publish(char *topic, STRING payload, int qos, int retain) {
  MQTTClient_message pubmsg = MQTTClient_message_initializer;
  MQTTClient_deliveryToken token;
  pubmsg.payload=payload.pointer;
  pubmsg.payloadlen=payload.len;
  pubmsg.qos =qos;
  pubmsg.retained = retain;
//  printf("publish to <%s> <%s> qos=%d\n",topic,payload.pointer,qos);
  MQTTClient_publishMessage(client,topic, &pubmsg, &token);
//  printf("done token=%d\n",token);
  // int rc=
  MQTTClient_waitForCompletion(client, token, TIMEOUT);
  // printf("Message with delivery token %d delivered\n", token);
}

static void mqtt_subscribe(char *topic,int qos) {
  MQTTClient_subscribe(client, topic, qos);
}
static void mqtt_unsubscribe(char *topic) {
  MQTTClient_unsubscribe(client, topic);
}
void mqtt_exit() {
  /* free the subscription list */
  while(anzsubscription>0) {
    anzsubscription--;
    free(subscriptions[anzsubscription].topic);
  }

  if(mqtt_isconnected) {
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    mqtt_isconnected=0;
  }
}
#endif

/* Define a broker for PUBLISH/SUBSCRIBE. Usually the mqtt url is given
   as well as a username and password if needed. Only one connection at a
   time is allowed. The old connection will be closed when BROKER is called
   a second time. 

   TODO: user credentials, persistence / clean session ? What happens when conneciton is lost?

 */

void c_broker(PARAMETER *plist, int e) {
#ifdef HAVE_MQTT

  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
  MQTTClient_willOptions will_opts;
  
  mqtt_exit(); /* Alte Verbindung beenden.*/
  sprintf(clientID,"X11-Basic-%ld",clock()); /* Make a unique client ID */
  MQTTClient_create(&client, plist[0].pointer, clientID,MQTTCLIENT_PERSISTENCE_NONE, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  conn_opts.reliable=0;
  
  if(e>1 && plist[1].typ>0) conn_opts.username=plist[1].pointer;
  if(e>2 && plist[2].typ>0) conn_opts.password=plist[2].pointer;
  if(e>3 && plist[3].typ>0) conn_opts.cleansession=plist[3].integer;
  
  will_opts.struct_id[0]='M';
  will_opts.struct_id[1]='Q';
  will_opts.struct_id[2]='T';
  will_opts.struct_id[3]='W';
  will_opts.struct_version=0;
  will_opts.topicName=clientID;
  will_opts.message="disconnect";
  will_opts.retained=0;
  will_opts.qos=0;
  
  conn_opts.will=&will_opts;
  
  
  
  MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
  int rc;
  if((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
    printf("MQTT Client: <%s> ",clientID);
    printf("Failed to connect, return code %d\n", rc);
  }
#else
  printf("MQTT support not compiled in.\n");
  xberror(9,"MQTT support");
#endif
}

/* Publish the content of a string (message) to a topic on 
   a (mqtt) server. This command could also work with message queues.
 */

void c_publish(PARAMETER *plist, int e) {
#ifdef HAVE_MQTT
  int qos=0; /* Quality of Service, default=0 */
  int retain=0; /* This is the default */
  if(e>2) qos=plist[2].integer;
  if(qos==0) retain=1;
  if(e>3) retain=(plist[3].integer!=0);
  mqtt_publish(plist[0].pointer, *((STRING *)&(plist[1].integer)), qos, retain);
#else
  printf("MQTT support not compiled in.\n");
  xberror(9,"MQTT support");
#endif
}


/* Subscribe to a topic and get message as well as call a function on
   reception.
   This currently can work with MQTT support.  
   Also used for unsubscribe.
 */


void c_subscribe(PARAMETER *plist, int e) {
#ifdef HAVE_MQTT
  int qos=0;  /* Quality of Service, default=0 */
  if(e>3) qos=plist[3].integer;
  if(qos>=0) {
    subscriptions[anzsubscription].ptr=plist[1].pointer;
    if(plist[1].typ==0) subscriptions[anzsubscription].vnr=-1;
    else subscriptions[anzsubscription].vnr=plist[1].integer;
    if(plist[2].typ==0) subscriptions[anzsubscription].procnr=-1;
    else subscriptions[anzsubscription].procnr=plist[2].integer;
    subscriptions[anzsubscription].topic=strdup(plist[0].pointer);
    anzsubscription++;
    mqtt_subscribe(plist[0].pointer, qos);
  } else { /* Unsubscribe */
    if(anzsubscription>0) {
      int i;
      for(i=0;i<anzsubscription;i++) {
        if(!strcmp(plist[0].pointer,subscriptions[i].topic)) { /* Subscription found! */
	  anzsubscription--;
	  if(i<anzsubscription) subscriptions[i]=subscriptions[anzsubscription];
        }
      }
    }
    mqtt_unsubscribe(plist[0].pointer);
  }
#else
  printf("MQTT support not compiled in.\n");
  xberror(9,"MQTT support");
#endif
}
