/* CCS.C (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "options.h"
#include "vtypes.h"
#include "globals.h"
#include "protos.h"

/* Falls mit Kontrollsystem  */

#define MAXPIDS 250
int notify_handler(int , int , int );
int pids[MAXPIDS];
int isubs[MAXPIDS];
int pidanz=0;

#ifndef WINDOWS

#ifdef CONTROL

#include "ccsdef.h"
#include "piddef.h"
#include "kernel.h"
#include "supman.h"
#include "errdef.h"
#include "attrdef.h"
#include "resourcebaseman.h"

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
#endif

int notify_handler(int pid, int overflow, int entries) {
  int i,pc2,flag=0;
  for(i=0;i<pidanz;i++) {
    if(pids[i]==pid) {
      int oldbatch,osp=sp;
      pc2=procs[isubs[i]].zeile;
      if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
      else {printf("Stack-Overflow ! PC=%d\n",pc); batch=0;}
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


int cssize(char *n) {
#ifdef CONTROL
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
#endif
  return(0);
}

int cstyp(char *n) {
   int typ=0;
#ifdef CONTROL
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
#endif
  return(typ);
}

int cspid(char *n) {
  int pid;
#ifdef CONTROL
  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) printf("ERROR in ccs_convert_parametername_id\n");
  ccs_err=CCSERR;
#endif
  return(pid);
}

char *cspname(int pid) {
  char *pname=malloc(80);
#ifdef CONTROL
  ccs_convert_id_parametername( pid, pname );
  if (CCSERR) printf("ERROR in ccs_convert_id_parametername (%s) !\n", ccs_get_error_message());
  ccs_err=CCSERR;
#endif
  return(pname);
}

char *csunit(char *n) {
  char *unit=malloc(32);
#ifdef CONTROL
  int pid;
  union {int i;float f;double d;} min, max;
  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) printf("ERROR in ccs_convert_parametername_id\n");
  else {
    ccs_resource_read_parameter_minmax( pid, &min, &max, unit );
    if (CCSERR) printf("ERROR in ccs_resource_read_parameter_minmax (%s) !\n", ccs_get_error_message());
  }
  ccs_err=CCSERR;
#endif
  return(unit);
}


double csget(char *n) {
#ifdef CONTROL
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
#endif
  return(0.0);
}

double csmin(char *n) {
#ifdef CONTROL
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
#endif
  return(0.0);
}
double csmax(char *n) {
#ifdef CONTROL
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
#endif
  return(0.0);
}
double csres(char *n) {
#ifdef CONTROL
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
#endif
  return(0.0);
}


ARRAY *csvget(char *n,int nn, int o) {
#ifdef CONTROL
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
	error(46,n); /* Parameter hat falschen Typ */
	return(ergebnis);
    }
  }
#else
  return(nullmatrix(FLOATTYP,1,&nn));
#endif
}


char *csgets(char *n) {
  char *ergebnis,s[256];
#ifdef CONTROL
  int pid,j;

  ccs_convert_parametername_id(n, &pid );
  if (CCSERR) {
    printf("ERROR in ccs_convert_parametername_id\n");
    ergebnis=malloc(8);
    strcpy(ergebnis,"<ERROR>");
    ccs_err=CCSERR;
    return(ergebnis);
  } else {

/* switch appropriate to type */

    if ( IS_PID_STRING(pid) ) {
      int bytes;
      ccs_get_value( pid, s, &bytes, sizeof(s), NULL );
      if (CCSERR) {
        printf("ERROR in ccs_get_value: %s\n", ccs_get_error_message());
	ergebnis=malloc(8);
  	strcpy(ergebnis,"<ERROR>");
	ccs_err=CCSERR;
  	return(ergebnis);
      } 
      ergebnis=malloc(strlen(s)+1);
      strcpy(ergebnis,s);
      ccs_err=CCSERR;
      return(ergebnis);
    } else {
      error(47,n); /* Parameter hat falschen Typ */
      ergebnis=malloc(8);
      strcpy(ergebnis,"<ERROR>");
      ccs_err=CCSERR;
      return(ergebnis);
    }
  }
#else
  ergebnis=malloc(8);
  strcpy(ergebnis,"<ERROR>");
  return(ergebnis);
#endif
}

#ifdef CONTROL
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
  if(pc2==-1)   error(19,w2); /* Procedure nicht gefunden */
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
 
    typ=type2(n);
    if(typ & ARRAYTYP) {
      r=varrumpf(n);
      vnr=variable_exist(r,typ);
      free(r);
      if(vnr==-1) error(15,n); /* Feld nicht dimensioniert */ 
      else {
        int anz=min(do_dimension(vnr),nn);
        if( !(typ & (FLOATTYP | INTTYP))) {printf("CSVPUT: Muss Float-ARRAY sein. \n");return;}


	/*  printf("CSVPUT: pid=%d, vnr=%d, nn=%d, anz=%d \n",pid,vnr,nn,anz); */

	if ( IS_PID_ANALOG(pid) ) {
          float *buffer=calloc(anz,sizeof(float));
	  
	  if(typ & FLOATTYP) {
	    double *varptr=(double  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
	    for(i=0;i<anz;i++) buffer[i]=(float)varptr[i];
          } else if(typ & INTTYP) {
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
	  if(typ & FLOATTYP) {
	    double *varptr=(double  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
	    for(i=0;i<anz;i++) buffer[i]=(int)varptr[i];
          } else if(typ & INTTYP) {
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

    if ( IS_PID_ANALOG(pid) && (type2(t) & (FLOATTYP|INTTYP)) ) {
        f=(float)parser(t);
        ccs_put_value_secure( pid, 1, &f, &j );
	if (CCSERR)   printf("ERROR in ccs_set_value: %s\n", ccs_get_error_message()); 
    } else if ( IS_PID_DIGITAL(pid) && (type2(t) & (FLOATTYP|INTTYP))  ) {
	i=(int)parser(t);
	ccs_put_value_secure( pid, 1, &i, &j );
	if (CCSERR)   printf("ERROR in ccs_set_value: %s\n", ccs_get_error_message()); 
    } else if ( IS_PID_STRING(pid) && (type2(t) & STRINGTYP) ) { 
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

    if ( IS_PID_ANALOG(pid) && (type2(t) & FLOATTYP) ) {
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
#else
#include "prolog.h"
#include "errors.h"
#include "rpctypes.h"
#include "toolkit.h"
#endif
#include "servdef.h"

extern int nofeclog;
extern int ListenForGlobals;
extern int ListenForMCasts;

void tmonitorCallback(int id,int cc) {
  int i;
#ifdef DEBUG
  printf("Callback id=%d rc=%d!\n",id,cc);
#endif
  notify_handler(id,0,cc);
  if (cc) printf("error: %s\n>",RPCLastErrorString);
}

/* Verhindert, dass Logfile angelegt wird und bestimmt die Geschwindigkeit der 
   Abfragen */

void fix_tine_start() {
    MaxNumConnections = 3;
    ListenForGlobals = ListenForMCasts = FALSE;
    /* MaxPollingRate = MinPollingRate = 200; */
    MaxPollingRate = MinPollingRate = 50;
    nofeclog = TRUE;
    defaultClientTimeout=100;
}
int my_ExecLinkEx(char *dev, char *prop, DTYPE *dout, DTYPE *din,short access,int buflen) {
  int rc,retry=2;
  fix_tine_start();
  rc=ExecLinkEx(dev,prop,dout,din,access,buflen);
  if(rc==98) {retry=5; defaultClientTimeout=1000;MaxPollingRate = MinPollingRate = 200;}
  if(rc==97 || rc==92 || rc==33) retry=0;
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
  if ((cc=my_ExecLinkEx(srv,"PROPS",&dout,&din,CA_READ,200)) != 0) return cc;
  srvProps->prpSize = ISWAP(srvProps->prpSize);
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
  int rc;
  if(convert_name_convention(n,dev,prop)) 
    printf("Syntax-Error in Parameter-Name %s\n",n);
  else {
    PropertyQueryStruct prpinfo;
    
    /* get size of Vector */
    if(rc=GetPropertyInformation(dev,prop,&prpinfo)) 
      printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
    else return(prpinfo.prpSize);
  }
  return(0);
}
int tinetyp(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc;
  if(convert_name_convention(n,w1,w2)) 
	printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
  else {
    PropertyQueryStruct prpinfo;
    
    /* get size of Vector */
    rc=GetPropertyInformation(w1,w2,&prpinfo);
    if(rc) printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
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
  int rc=convert_name_convention(n,w1,w2);
  if(rc) 
	printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
   else {
    float mmax,mmin;
    fix_tine_start();    
    rc=GetDevicePropertyEGU(w1,w2,&mmax,&mmin,unit);
     if(rc) printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
  }
  return(unit);
}
char *tineinfo(char *n) {
  char *des=malloc(80);
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc=convert_name_convention(n,w1,w2);
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
  if(rc) 
	printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
   else {
    fix_tine_start();
     
    rc=GetDevicePropertyEGU(w1,w2,&mmax,&mmin,unit);
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
      din.data.lptr = &start;
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
ARRAY *tinevget(char *n,int nn, int o) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc=convert_name_convention(n,w1,w2);
  char *buf;
  int buflen;
  int f,j;
  ARRAY *ergebnis=malloc(sizeof(ARRAY));
  
  ergebnis->dimension=1;
  if(rc) {
    printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
    ergebnis->pointer=malloc(40);
    ((int *)ergebnis->pointer)[0]=1;
  } else {
      /* get size of Vector */

    PropertyQueryStruct prpinfo;  
    DTYPE dout;
    
    rc=GetPropertyInformation(w1,w2,&prpinfo);
    if(rc) {
      printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
      ergebnis->pointer=malloc(40);
      ((int *)ergebnis->pointer)[0]=1;
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
    if(rc) {
      printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
      free(buf);
      ergebnis->pointer=malloc(40);
      ((int *)ergebnis->pointer)[0]=1;
      return(ergebnis);
    }
      
    /* switch appropriate to type */

    f=LFMT(prpinfo.prpFormat);
    if(f==CF_FLOAT) {
      double *varptr;
      ergebnis->typ=FLOATARRAYTYP;
      ergebnis->pointer=malloc(1*INTSIZE+nn*sizeof(double));
      ((int *)ergebnis->pointer)[0]=nn;
      varptr=ergebnis->pointer+INTSIZE;
      for(j=0;j<nn;j++) varptr[j]=(double)(((float *)buf)[j+o]);
      free(buf);
    } else if(f==CF_DOUBLE) {
      double *varptr;
      ergebnis->typ=FLOATARRAYTYP;
      ergebnis->pointer=malloc(1*INTSIZE+nn*sizeof(double));
      ((int *)ergebnis->pointer)[0]=nn;
      varptr=ergebnis->pointer+INTSIZE;
      for(j=0;j<nn;j++) varptr[j]=(double)(((double *)buf)[j+o]);
      free(buf);
    } else if(f==CF_BYTE) {
      double *varptr;
      ergebnis->typ=FLOATARRAYTYP;
      ergebnis->pointer=malloc(1*INTSIZE+nn*sizeof(double));
      ((int *)ergebnis->pointer)[0]=nn;
      varptr=ergebnis->pointer+INTSIZE;
      for(j=0;j<nn;j++) varptr[j]=(double)(((char *)buf)[j+o]);
      free(buf);
    } else if(f==CF_SHORT) {
      double *varptr;
      ergebnis->typ=FLOATARRAYTYP;
      ergebnis->pointer=malloc(1*INTSIZE+nn*sizeof(double));
      ((int *)ergebnis->pointer)[0]=nn;
      varptr=ergebnis->pointer+INTSIZE;
      for(j=0;j<nn;j++) varptr[j]=(double)(((short *)buf)[j+o]);
      free(buf);
    } else if(f==CF_LONG) {
      double *varptr;
      ergebnis->typ=FLOATARRAYTYP;
      ergebnis->pointer=malloc(1*INTSIZE+nn*sizeof(double));
      ((int *)ergebnis->pointer)[0]=nn;
      varptr=ergebnis->pointer+INTSIZE;
      for(j=0;j<nn;j++) varptr[j]=(double)(((long *)buf)[j+o]);
      free(buf);
    } else {
      printf("output format type %d is not implemented !\n",LFMT(prpinfo.prpFormat));
      error(46,n); /* Parameter hat falschen Typ */
      free(buf);
      ergebnis->pointer=malloc(40);
      ((int *)ergebnis->pointer)[0]=1;
    }
  }
 return(ergebnis);
}
ARRAY *tinehistory(char *n,int start, int stop) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int rc=convert_name_convention(n,w1,w2);
  char *buf;
  int buflen,nn;
  int f,j;
  ARRAY *ergebnis=malloc(sizeof(ARRAY));
  UINT32 startstopArray[3];
  DTYPE dout,din;
  double *varptr;
  
  ergebnis->dimension=2;

  
  if(rc) {
    printf("Syntax-Error %d in Parameter-Name %s\n",rc,n);
    ergebnis->pointer=malloc((4+2)*sizeof(int));
    ergebnis->typ=INTTYP;
    ((int *)ergebnis->pointer)[0]=1;
    ((int *)ergebnis->pointer)[1]=1;
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
    if(rc) {
      printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
      free(buf);
      ergebnis->pointer=malloc((4+2)*sizeof(int));
      ergebnis->typ=INTTYP;
      ((int *)ergebnis->pointer)[0]=1;
      ((int *)ergebnis->pointer)[1]=1;
      return(ergebnis);
    }
    nn=((int *)buf)[1];
    ergebnis->typ=FLOATARRAYTYP;
    ergebnis->pointer=malloc(2*INTSIZE+2*nn*sizeof(double));
    ((int *)ergebnis->pointer)[0]=nn;
    ((int *)ergebnis->pointer)[1]=2;
    varptr=ergebnis->pointer+2*INTSIZE;
      for(j=0;j<nn;j++) {
        varptr[2*j]=(double)(((float *)buf)[2*j+2]);
        varptr[2*j+1]=(double)(((int *)buf)[2*j+1+2]);
      }  
      free(buf);
  }
  return(ergebnis);
}

void c_tinemonitor(char *n) {
  int i,flag,newpid,pc2;
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,w1,w2);
  static PropertyQueryStruct prpinfo;
  if(e<2) {
    printf("Syntax-Error bei TINEMONITOR. <%s>\n",n);
  } else {
    char dev[strlen(w1)+1],prop[strlen(w1)+1];
    char *test=s_parser(w1);
    int rc=convert_name_convention(test,dev,prop);
    char *buf;
    int buflen;
    if(rc) printf("Syntax-Error in Parameter-Name %s\n",test);
    else {   
        
      DTYPE dout;
      rc=GetPropertyInformation(dev,prop,&prpinfo);
      if(rc) printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
      else {
        pc2=procnr(w2,1);
        if(pc2==-1)   error(19,w2); /* Procedure nicht gefunden */
        else {

        buflen=prpinfo.prpSize*getFormatSize(LFMT(prpinfo.prpFormat));
        buf=malloc(buflen);
        dout.dFormat = LFMT(prpinfo.prpFormat);
        dout.dArrayLength = prpinfo.prpSize;
        dout.dTag[0] = 0;
        dout.data.vptr = buf;
        rc=AttachLink(dev,prop,&dout,NULL,CA_READ,buflen,tmonitorCallback,CM_POLL);
        if(rc<0) {
	  printf("Tine-Error AttachLink: %d  %s\n",-rc,erlst[-rc]);
	  free(buf);
        } else
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
    } 
    free(test);    
  }
}

void c_tineput(char *w) {
  char n[strlen(w)+1],t[strlen(w)+1];
  int pid,j,e,i,rc;
  float f;
  e=wort_sep(w,',',TRUE,n,t);
  if(e<2) {
    printf("Syntax-Error bei TINEPUT. <%s>\n",w);
    return;
  } else {
    char *test=s_parser(n);
    char w1[strlen(test)+1],w2[strlen(test)+1];
    rc=convert_name_convention(test,w1,w2);
    free(test);
    if(rc) {
	printf("Syntax-Error %d in Parameter-Name %s\n",rc,test);
    } else {
      PropertyQueryStruct prpinfo;  
      DTYPE dout;
      int buflen;
      char *buf;
    /* get size of Vector */
      rc=GetPropertyInformation(w1,w2,&prpinfo);
      if(rc) {
        printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
        return;
      }
      if((type2(t) & (FLOATTYP|INTTYP)) && prpinfo.prpSize!=1) {
        printf("Error: Property ist Vektor (%d)!\n",prpinfo.prpSize);
        return;
      }
      buflen=prpinfo.prpSize*getFormatSize(LFMT(prpinfo.prpFormat));
      buf=malloc(buflen);
      dout.dFormat = LFMT(prpinfo.prpFormat);
      dout.dArrayLength = prpinfo.prpSize;
      dout.dTag[0] = 0;
      dout.data.vptr = buf;
      
/* switch appropriate to type */
      if(type2(t) & (FLOATTYP|INTTYP)) {
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
      } else if(type2(t) & STRINGTYP) {
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
      rc=my_ExecLinkEx(w1,w2,NULL,&dout,CA_WRITE,buflen);
      if(rc) printf("Tine-Error: %d  %s\n",rc,erlst[rc]);
      ccs_err=rc;
      free(buf);
    }
  }
}
#endif
