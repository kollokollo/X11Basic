/* CCS.C (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>

#include "defs.h"
#include "vtypes.h"
#include "protos.h"

/* Falls mit Kontrollsystem  */

#ifdef CONTROL

#include "ccsdef.h"
#include "piddef.h"
#include "kernel.h"
#include "supman.h"
#include "errdef.h"
#include "attrdef.h"
#include "resourcebaseman.h"
#define MAXPIDS 250
int notify_handler(int , int , int );
int pids[MAXPIDS];
int isubs[MAXPIDS];
int pidanz=0;

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

#endif
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
	printf("Csget: Parameter hat falschen Typ %s\n",n);
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
      printf("Csget: Parameter hat falschen Typ %s\n",n);
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
