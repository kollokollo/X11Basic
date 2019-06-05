/* parameter.c (c) Markus Hoffmann             */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "defs.h"
#include "x11basic.h"
#include "xbasic.h"
#include "type.h"
#include "variablen.h"
#include "parser.h"
#include "array.h"

#include "wort_sep.h"
#include "parameter.h"
#include "number.h"


  
static int fit_solltyp(int typ, int solltyp) {
  switch(solltyp) {
  case PL_VAR:
    if((typ&ARRAYTYP)!=ARRAYTYP) return(1);
    break;
  case PL_NVAR:
    if((typ&ARRAYTYP)!=ARRAYTYP && typ!=STRINGTYP) return(1);
    break;
  case PL_IVAR:
    if((typ&ARRAYTYP)!=ARRAYTYP && typ==INTTYP) return(1);
    break;
  case PL_FVAR:
    if((typ&ARRAYTYP)!=ARRAYTYP && typ==FLOATTYP) return(1);
    break;
  case PL_CVAR:
    if((typ&ARRAYTYP)!=ARRAYTYP && typ==COMPLEXTYP) return(1);
    break;
  case PL_AIVAR:
    if((typ&ARRAYTYP)!=ARRAYTYP && typ==ARBINTTYP) return(1);
    break;
  case PL_SVAR:
    if((typ&ARRAYTYP)!=ARRAYTYP && typ==STRINGTYP) return(1);
    break;
  case PL_ARRAYVAR:
     if((typ&ARRAYTYP)==ARRAYTYP) return(1);
     break;
  case PL_IARRAYVAR:
     if((typ&ARRAYTYP)==ARRAYTYP && (typ&TYPMASK)==INTTYP) return(1);
     break;
  case PL_FARRAYVAR:
     if((typ&ARRAYTYP)==ARRAYTYP && (typ&TYPMASK)==FLOATTYP) return(1);
     break;
  case PL_AIARRAYVAR:
     if((typ&ARRAYTYP)==ARRAYTYP && (typ&TYPMASK)==ARBINTTYP) return(1);
     break;
  case PL_CARRAYVAR:
     if((typ&ARRAYTYP)==ARRAYTYP && (typ&TYPMASK)==COMPLEXTYP) return(1);
     break;
  case PL_SARRAYVAR:
     if((typ&ARRAYTYP)==ARRAYTYP && (typ&TYPMASK)==STRINGTYP) return(1);
     break;
  case PL_ALLVAR:  return(1);
  default:         printf("ERROR: Fit type --> unnown %x\n",solltyp);
  }
  return(0);
}
 
 /*Fuellt die Struktur PARAMETER ausgehend von ASCII w1 und solltyp.
  Hierbei nur Vorbereitung, also keine Parseraufrufe (bei Auswertung der
  Index-Liste). Es treten nur Variablen und Array-indizies als Variablen
  auf. Es werden nur gefuellt:
  p->typ --> Variablentyp
  p->pointer=Null, da vaiableninhalt sich noch aendern koennte
  p->integer --> vnr
  p->panzahl
  p->ppointer
  
  
  Solltyp ist PL_xVAR, kann auch gemacht werden aus
  (PL_VARGROUP|type), type aus vartype
  
  
  Rückgabe: 
   >=0 -- alles OK
  -1 -- Nicht gefunden.
  
  
  */


int prepare_vvar(char *w1,PARAMETER *p, unsigned int solltyp) {
  // printf("## prepare VVAR... <%s>\n",w1);
  char k1[strlen(w1)+1],k2[strlen(w1)+1];
  int typ=vartype(w1);
  int e=klammer_sep(w1,k1,k2);
  p->pointer=NULL;
  p->integer=-1;
  p->typ=NOTYP; /* Falls type mismatch auftritt, definiertes Ergebnis */
//  printf("prepare_vvar: w1=<%s>  solltyp=%x\n",w1,solltyp);
  if(e==1 || strlen(k2)==0) {  /* Keine Argumente in Klammer oder keine klammern*/
  //  printf("Keine Klammern/keine Argumente... typ=%x\n",typ);
  //  printf("---> Typ=%x Solltyp=%x  ....\n",typ,solltyp);
    if(fit_solltyp(typ,solltyp)) {
      char *r=varrumpf(w1);
  //    printf("FIT.\n");
      /*p->integer soll vnr bekommen.*/
      if(typ&ARRAYTYP) {
        p->integer=add_variable(r,ARRAYTYP,typ&TYPMASK,V_DYNAMIC,NULL);
      } else p->integer=add_variable(r,typ,0,V_DYNAMIC,NULL);
      free(r);
      /*Parameter Typ eintragen.*/
      p->typ=(PL_VARGROUP|typ);
    } 
  } else { /* Es sind indizies da. */
    // printf("Es sind idicies da...\n");
    typ&=TYPMASK;
    if(fit_solltyp(typ,solltyp)) {
      char *r=varrumpf(w1);
      p->integer=add_variable(r,ARRAYTYP,typ,V_DYNAMIC,NULL);  /*  vnr */
      free(r);
       /*Parameter Typ eintragen.*/
      p->typ=(PL_VARGROUP|typ);
      p->panzahl=count_parameters(k2);   /* Anzahl indizes z"ahlen*/
      p->ppointer=malloc(sizeof(PARAMETER)*p->panzahl);
      /*hier die Indizies in einzelne zu evaluierende Ausdruecke
        separieren*/
      //  printf("eintragen: k2=<%s>\n",k2);
      make_preparlist(p->ppointer,k2);
      // dump_parameterlist(p,1);
    } 
  }
 // printf("----> vnr=%d\n",p->integer);
  if(p->integer<0) xberror(76,w1);   /*illegal variable name */
  else if(p->typ==NOTYP) xberror(13,w1);  /* Type mismatch */
  return(p->integer);
}



/* Anzahl der Parameter zaehlen.*/

int count_parameters(const char *n) {
  if(n==NULL || *n==0) return(0);
  int i=1;
  char *pos=searchchr2(n,',');
  while(pos) {
    i++;
    pos=searchchr2(++pos,',');
  }
  return(i);
}

PARAMETER double_parameter(PARAMETER *p) {
  PARAMETER ret=*p;
  switch(p->typ) {
  case PL_KEY:
  case PL_STRING:
    ret.pointer=malloc(p->integer+1);
    memcpy(ret.pointer,p->pointer,p->integer+1);
    break;
  case PL_ARBINT:
    mpz_init(*(ARBINT *)ret.pointer);
    mpz_set(*(ARBINT *)ret.pointer,*(ARBINT *)p->pointer);
    break;
  case PL_ARRAY:
    *(ARRAY *)&(ret.integer)=double_array((ARRAY *)&(p->integer));
  }
  return(ret);
} 



void free_parameter(PARAMETER *p) {
  switch(p->typ) {
  case PL_STRING:
  case PL_EVAL:
  case PL_KEY:
    free(p->pointer);
  case PL_INT:
  case PL_FLOAT:
  case PL_COMPLEX:
  case PL_LEER:
  case PL_FILENR:
  case PL_LABEL:
  case PL_PROC:
  case PL_FUNC:
    break;
  case PL_ARBINT:
    mpz_clear(*((ARBINT *)p->pointer));
    free(p->pointer);
    break;
  case PL_ALLVAR:
  case PL_ARRAYVAR:
  case PL_IARRAYVAR:
  case PL_AIARRAYVAR:
  case PL_FARRAYVAR:
  case PL_CARRAYVAR:
  case PL_SARRAYVAR:
  case PL_VAR:
  case PL_IVAR:
  case PL_AIVAR:
  case PL_FVAR:
  case PL_CVAR:
  case PL_NVAR:
  case PL_SVAR:
    if(p->panzahl) {
      free_pliste(p->panzahl,p->ppointer);
      p->panzahl=0;
    }
    break;
  case PL_ARRAY:
  case PL_IARRAY:
  case PL_AIARRAY:
  case PL_FARRAY:
  case PL_CARRAY:
  case PL_SARRAY:
  case PL_NARRAY:
  case PL_CFARRAY:
    free_array((ARRAY *)&(p->integer));
    break;
  default:
    printf("WARNING: free_parameter, unknown typ $%x, PC=%d.\n",p->typ,pc);
  }
  p->typ=PL_LEER;
}


void dump_parameterlist(PARAMETER *p, int n) {
  if(n) {
    int j;
    printf("\n%d parameters:\n",n);
    for(j=0;j<n;j++) {
      printf("%2d: ",j);
      switch(p[j].typ) {
        case PL_INT:    printf(" int %d\n",p[j].integer); break;
        case PL_FLOAT:  printf(" flt %g\n",p[j].real); break;
        case PL_COMPLEX:printf(" cpx (%g+%gi)\n",p[j].real,p[j].imag); break;
        case PL_ARBINT: {
	  char *buf=mpz_get_str(NULL,10,*((ARBINT *)p[j].pointer));
	  printf(" bigint %s\n",buf);
	  free(buf);
	  } 
	  break;
        case PL_NUMBER: printf(" num %g\n",p[j].real); break;
        case PL_STRING: printf("   $ <%s> len=%d\n",(char *)p[j].pointer,p[j].integer);break;
        case PL_KEY:    printf(" KEY %d <%s>\n",p[j].arraytyp,(char *)p[j].pointer);break;
        case PL_LEER:   printf(" <empty>\n");break;
        case PL_FILENR: printf("   # %d\n",p[j].integer);break;
        case PL_EVAL:   printf(" EVAL: <%s>.%x\n",(char *)p[j].pointer,p[j].arraytyp);break;
        case PL_LABEL:  printf(" <label>\n");break;
        case PL_PROC:   printf(" <proc>\n");break;
        case PL_FUNC:   printf(" <func>\n");break;
        case PL_ARRAY:  printf(" <array,$%x,dim=%d>\n",p[j].arraytyp,p[j].integer);break;
	case PL_FVAR:   printf(" <var,%d,%s>\n",p[j].integer,variablen[p[j].integer].name);break;
	case PL_CVAR:   printf(" <var#,%d,%s>\n",p[j].integer,variablen[p[j].integer].name);break;
	case PL_AIVAR:  printf(" <var&,%d,%s>\n",p[j].integer,variablen[p[j].integer].name);break;
	case PL_IVAR:   printf(" <var%%,%d,%s>\n",p[j].integer,variablen[p[j].integer].name);break; 
        case PL_SVAR:   printf(" <var$,%d,%s>\n", p[j].integer,variablen[p[j].integer].name);break; 
        case PL_ALLVAR: printf(" <allvar,%d,%s>\n",p[j].integer,variablen[p[j].integer].name);break;
	case PL_NVAR:   printf(" <nvar,%d,%s>\n", p[j].integer,variablen[p[j].integer].name);break;
	case PL_VAR:    printf(" <var,%d,%s>\n",  p[j].integer,variablen[p[j].integer].name);break;
        case PL_ARRAYVAR:  
                        printf(" <array(),%d,%s>\n",p[j].integer,variablen[p[j].integer].name); break;
        case PL_IARRAYVAR: 
                        printf(" <array%%(),%d,%s>\n",p[j].integer,variablen[p[j].integer].name); break;
        case PL_FARRAYVAR: 
                        printf(" <array(),%d,%s>\n",p[j].integer,variablen[p[j].integer].name); break;
        case PL_CARRAYVAR: 
                        printf(" <array#(),%d,%s>\n",p[j].integer,variablen[p[j].integer].name); break;
        case PL_SARRAYVAR: 
                        printf(" <array$(),%d,%s>\n",p[j].integer,variablen[p[j].integer].name); break;
        default:   printf("<typ=$%x %d %g %p>\n",p[j].typ,p[j].integer,p[j].real,(void *)p[j].pointer);
      }
      if(p[j].panzahl>0 && (p[j].typ&PL_GROUPMASK)==PL_VARGROUP) {
         printf("%d Index-Parameters:\n",p[j].panzahl);
         if(p[j].ppointer) dump_parameterlist(p[j].ppointer,p[j].panzahl);
      }
    }
  }
}


/*Macht aus einem Ausdruck eine Parameter-Struktur, aber
  noch nicht laufzeit, d.h. keine parser-Aufrufe! Dafür dürfen type() 
  aufrufe verwendet werden.*/

int make_parameter_stage2(char *n,unsigned short ap, PARAMETER *pret) {
  if(!n ||*n==0) pret->typ=PL_LEER;
  else {
    pret->typ=ap;  /*Solltyp des Parameters*/
    switch(ap) {
      char *pos2,*pos;
      int typ;
    case PL_LABEL: 
      pret->integer=labelnr(n);
      if(pret->integer==-1) xberror(20,n);/* Label nicht gefunden */
      break;
    case PL_PROC:
      pos=searchchr(n,'(');
      if(pos!=NULL) {
        *pos=0;pos++;
        pos2=pos+strlen(pos)-1;
        if(*pos2!=')') xberror(32,n); /* Syntax error */
        else *pos2=0;
      } else pos=n+strlen(n);
      pret->integer=procnr(n,1);
      if(pret->integer==-1) xberror(19,n); /* Procedure nicht gefunden */
      break;
    case PL_FILENR:   /*   TODO: Hier schon auf Konstanten testen !!!!*/
      if((typ=type(n))&CONSTTYP) {
        pret->typ=PL_FILENR;
        if(*n=='#') pret->integer=(int)parser(n+1);
	else pret->integer=(int)parser(n);
      } else {
        pret->typ=PL_EVAL;
	pret->arraytyp=typ;
	if(*n=='#') *((STRING *)&(pret->integer))=create_string(n+1);
	else *((STRING *)&(pret->integer))=create_string(n);
      }
      break;
    case PL_INT:	/* Integer */
    case PL_FLOAT:  /* Float  */
    case PL_COMPLEX:/* Complex */
    case PL_ARBINT: /* Big Int */
    case PL_ARBFLOAT: /* Arbitrary prec float */
    case PL_NUMBER: /* Integer or FLOAT or COMPLEX or ARBINT */
    case PL_CFAI:
    case PL_CF:
    case PL_ANYVALUE:	    /* Integer or FLOAT oder string*/
    case PL_ANY:
    case PL_STRING: /* String */
    case PL_ARRAY:  /* Array */
    case PL_NARRAY:  /* Array */
    case PL_CFARRAY:  /* Array */
    case PL_IARRAY: /* Int-Array */
    case PL_FARRAY: /* float-Array */
    case PL_CARRAY: /* complex-Array */
    case PL_AIARRAY: /* big int-Array */
    case PL_SARRAY: /* String-Array */
      if((typ=type(n))&CONSTTYP) {
        switch(ap) {
        case PL_INT:	/* Integer Float, complex, arbint  convertiert nach int */
	  eval2parnumtype(n,pret,typ); 
	  pret->integer=p2int(pret);
	  free_parameter(pret);
          pret->typ=PL_INT;
	  break;
	case PL_ANY:       /* Integer or FLOAT oder string oder ARRAY*/
	case PL_ANYVALUE:  /* Integer or FLOAT oder string*/
	  eval2partype(n,pret,typ);
	  break;
	case PL_NUMBER:	/* Integer or FLOAT or complex or ARBINT */ 
	  eval2parnumtype(n,pret,typ);
	  break;
	case PL_CFAI:	/*  FLOAT or complex or ARBINT */ 
	  eval2parnumtype(n,pret,typ);
	  if(pret->typ==PL_INT) cast_to_real(pret);
	  break;
	case PL_CF:	/*  FLOAT or complex */ 
	  eval2parnumtype(n,pret,typ);
	  if(pret->typ==PL_INT || pret->typ==PL_ARBINT) cast_to_real(pret);
	  break;
	case PL_FLOAT:  /* Float, complex, arbint oder INT convertiert nach float */
	  eval2parnumtype(n,pret,typ); 
	  pret->real=p2float(pret);
	  free_parameter(pret);
          pret->typ=PL_FLOAT;
	  break;
	case PL_COMPLEX:  /* Complex */
	  eval2parnumtype(n,pret,typ); 
	  *(COMPLEX *)&(pret->real)=p2complex(pret);
	  free_parameter(pret);
          pret->typ=PL_COMPLEX;
	  break;
	case PL_ARBFLOAT:  /* Arb float, treat as integer */
	case PL_ARBINT:  /* Big integer */
	  { ARBINT a;
	  mpz_init(a);
	  eval2parnumtype(n,pret,typ); 
	  p2arbint(pret,a);
	  free_parameter(pret);
          pret->typ=PL_ARBINT;
	  pret->pointer=malloc(sizeof(ARBINT));
	  mpz_init(*((ARBINT *)pret->pointer));
          mpz_set(*((ARBINT *)pret->pointer),a);
	  mpz_clear(a);}
	  break;
	case PL_STRING: /* String */
	  pret->typ=PL_STRING;
	  *((STRING *)&(pret->integer))=string_parser(n);
          break;
        case PL_ARRAY:  /* Array */
        case PL_IARRAY: /* Int-Array */    /*Wenn es sich wirklich um ein konstantes ARRAY handelt */
        case PL_FARRAY: /* float-Array */  
        case PL_CARRAY: /* complex-Array */
        case PL_AIARRAY: /* big int-Array */
        case PL_SARRAY: /* String-Array */
        case PL_NARRAY: /* Number-Array */
        case PL_CFARRAY: /* Number-Array */
	  pret->typ=PL_ARRAY;
	  *((ARRAY *)&(pret->integer))=array_parser(n);
	  break;
	default:
          pret->typ=PL_EVAL;
          pret->arraytyp=typ;  /* für spaeter */
	  *((STRING *)&(pret->integer))=create_string(n);
	}
      } else {  /* nicht const typ*/
        pret->typ=PL_EVAL;
	*((STRING *)&(pret->integer))=create_string(n);
	    /* Typ in Parameter eintragen, dann hat man es zu laufzeit schneller*/
        pret->arraytyp=typ;  /* Muss man spaeter natuerlich noch auswerten....*/
      }
      break;
    case PL_VAR:       
    case PL_NVAR:      
    case PL_SVAR:      
    case PL_CVAR:      
    case PL_IVAR:      
    case PL_FVAR:      
    case PL_AIVAR:      
    case PL_ARRAYVAR:  
    case PL_IARRAYVAR: 
    case PL_FARRAYVAR: 
    case PL_AIARRAYVAR: 
    case PL_CARRAYVAR: 
    case PL_SARRAYVAR: 
    case PL_ALLVAR:
      prepare_vvar(n,pret,ap); 
      break;
    case PL_KEY: /* Keyword */
      pret->arraytyp=keyword2num(n);
      *((STRING *)&(pret->integer))=create_string(n);
      break;
    case PL_EVAL:
      pret->arraytyp=vartype(n);
      *((STRING *)&(pret->integer))=create_string(n);
      break;
    default:
      printf("ERROR: illegal parameter type $%x\n",ap);
      return(-1);
    }
  }
  return(0);
}


int make_pliste(int pmin,int pmax,unsigned short *pliste,char *n, PARAMETER **pr){
  PARAMETER *pr1;
  int ii=make_pliste2(pmin,pmax,pliste,n,&pr1,count_parameters(n));
  int ii2=make_pliste3(pmin,pmax,pliste,pr1,pr,ii);
  if(ii!=ii2) printf("make_pliste: Something is wrong.\n");
  free_pliste(ii,pr1);
  return(ii2);
}

/* Hier noch nicht laufzeit, d.h. keine parser-Aufrufe!*/

int make_pliste2(int pmin,int pmax,unsigned short *pliste,char *n, PARAMETER **pr,int ii){
  char buf[strlen(n)+1];
  char *w1,*pos=NULL;
  PARAMETER *pret;
  int anzpar,i=0;
  unsigned short ap;
  if(pmax==-1) anzpar=ii;
  else anzpar=pmax;
 // printf("PARAMETER vorbereiten: (%d) \n",ii);
  *pr=pret=calloc(anzpar,sizeof(PARAMETER));
  if(n && *n) {
    strcpy(buf,n);
    pos=w1=buf;
    while(pos && i<anzpar) {
      w1=pos;
      pos=searchchr2(pos,',');
      if(pos) *pos++=0;
      if(i>pmin && pmax==-1) ap=pliste[pmin];
      else ap=pliste[i];
      make_parameter_stage2(w1,ap,&pret[i]);
      i++;
    }
  }
  if(i<pmin) xberror(42,""); /* Zu wenig Parameter  */
  else if(i==pmax && pos) xberror(45,""); /* Zu viele Parameter  */
  return(i);
}

#define PARERR() {printf("Error: line %d make_parameter_stage3: parameter incompatibel. typ=$%x solltyp=$%x\n",pc,ip,ap);dump_parameterlist(pin,1);}


/*Hier Ergaenzungen von pre-pliste zu aktueller (Zu Laufzeit). 
  Jetzt koennen variableninhalte als fix betrachtet werden.
  Return: 0 = alles OK, -1 = Fehler
 */
int make_parameter_stage3(PARAMETER *pin,unsigned short ap,PARAMETER *pret) {
  unsigned short ip=pin->typ;
  pret->typ=ap;
  switch(ap) {
    int vnr;  
    int *indexliste;
  case PL_LEER:
    break;
  case PL_LABEL: 
  case PL_PROC:
    if(ip==PL_LABEL || ip==PL_PROC) {
      pret->integer=pin->integer;
      pret->arraytyp=pin->arraytyp;  /* Typ */
    } else {PARERR();return(-1);}
    break;
  case PL_FILENR:
  case PL_INT:    /* Integer */
    switch(ip) {
    case PL_LEER: pret->typ=PL_LEER; break;
    case PL_EVAL: 
      if(pin->arraytyp) eval2partype(pin->pointer,pret,pin->arraytyp); 
      else eval2par(pin->pointer,pret); 
      pret->integer=p2int(pret);
      free_parameter(pret);
      pret->typ=PL_INT;
      break;
    default: pret->integer=p2int(pin);
    }
    break;
  case PL_NUMBER:  /* Float oder Number */
    pret->typ=ip;
    switch(ip) {
    case PL_LEER:  break;
    case PL_EVAL:
      if(pin->arraytyp) eval2parnumtype(pin->pointer,pret,pin->arraytyp);
      else eval2parnum(pin->pointer,pret); 
      break;
    case PL_COMPLEX: pret->imag=pin->imag; /* no break */
    case PL_FLOAT:   pret->real=pin->real; break;
    case PL_INT:     pret->integer=pin->integer; break;
    case PL_ARBINT:
      pret->pointer=malloc(sizeof(ARBINT));
      mpz_init(*((ARBINT *)pret->pointer));
      mpz_set(*((ARBINT *)pret->pointer),*((ARBINT *)pin->pointer));
      break;
    default: {PARERR();return(-1);}
    }
    break;
  case PL_CFAI:  /* Float oder complex oder arbint */
    pret->typ=ip;
    switch(ip) {
    case PL_LEER:  break;
    case PL_EVAL:
      if(pin->arraytyp) eval2parnumtype(pin->pointer,pret,pin->arraytyp); 
      else eval2parnum(pin->pointer,pret);
      if(pret->typ==PL_INT) cast_to_real(pret);
      break;
    case PL_COMPLEX: pret->imag=pin->imag; /* no break */
    case PL_FLOAT:   pret->real=pin->real; break;
    case PL_INT:
      pret->real=(double)pin->integer;
      pret->typ=PL_FLOAT;
      break;
    case PL_ARBINT: 
      pret->pointer=malloc(sizeof(ARBINT));
      mpz_init(*((ARBINT *)pret->pointer));
      mpz_set(*((ARBINT *)pret->pointer),*((ARBINT *)pin->pointer));
      break;
    default: {PARERR();return(-1);}
    }
    break;
  case PL_CF:  /* Float oder complex oder arbint */
    pret->typ=ip;
    switch(ip) {
    case PL_LEER:  break;
    case PL_EVAL:
      if(pin->arraytyp) eval2parnumtype(pin->pointer,pret,pin->arraytyp); 
      else eval2parnum(pin->pointer,pret);
      if(pret->typ==PL_INT||pret->typ==PL_ARBINT) cast_to_real(pret);
      break;
    case PL_COMPLEX: pret->imag=pin->imag; /* no break */
    case PL_FLOAT:   pret->real=pin->real; break;
    case PL_INT:
      pret->real=(double)pin->integer;
      pret->typ=PL_FLOAT;
      break;
    case PL_ARBINT: 
      pret->real=mpz_get_d(*((ARBINT *)pin->pointer));
      pret->typ=PL_FLOAT;
      break;
    default: {PARERR();return(-1);}
    }
    break;
  case PL_FLOAT:  /* Float oder Number */
    switch(ip) {
    case PL_LEER: pret->typ=PL_LEER; break;
    case PL_EVAL: 
      if(pin->arraytyp) eval2parnumtype(pin->pointer,pret,pin->arraytyp);
      else eval2parnum(pin->pointer,pret);
      pret->real=p2float(pret);
      free_parameter(pret);
      pret->typ=PL_FLOAT;
      break; 
    default: pret->real=p2float(pin);
    }
    break;
  case PL_COMPLEX:  /* Complex */
    switch(ip) {
    case PL_LEER: pret->typ=PL_LEER; break;
    case PL_EVAL: 
      if(pin->arraytyp) eval2parnumtype(pin->pointer,pret,pin->arraytyp);
      else eval2parnum(pin->pointer,pret); 
      *(COMPLEX *)&(pret->real)=p2complex(pret);
      free_parameter(pret);
      pret->typ=PL_COMPLEX;
      break; 
    default: *(COMPLEX *)&(pret->real)=p2complex(pin);
    }
    break;
  case PL_ARBINT:  /* Big int */
    switch(ip) {
    case PL_LEER: pret->typ=PL_LEER; break;
    case PL_EVAL: {
      if(pin->arraytyp) eval2parnumtype(pin->pointer,pret,pin->arraytyp);
      else eval2parnum(pin->pointer,pret); 
      cast_to_arbint(pret);
      }
      break; 
    default:
      pret->pointer=malloc(sizeof(ARBINT));
      mpz_init(*(ARBINT *)(pret->pointer));
      p2arbint(pin,*(ARBINT *)(pret->pointer));
    }
    break;
  case PL_ANY:
  case PL_ANYVALUE:
    pret->typ=ip;
    switch(ip) {
    case PL_LEER: break;
    case PL_EVAL: 
      if(pin->arraytyp) eval2parnumtype(pin->pointer,pret,pin->arraytyp);
      else eval2par(pin->pointer,pret); 
      break;
    case PL_COMPLEX: pret->imag=pin->imag;  /*no break*/
    case PL_FLOAT:   pret->real=pin->real; break;
    case PL_INT:     pret->integer=pin->integer; break;
    case PL_ARBINT: 
      pret->pointer=malloc(sizeof(ARBINT));
      mpz_init(*((ARBINT *)pret->pointer));
      mpz_set(*((ARBINT *)pret->pointer),*((ARBINT *)pin->pointer));
      break;
    case PL_STRING:  *((STRING *)&(pret->integer))=double_string((STRING *)&(pin->integer)); break;
    default: {PARERR();return(-1);}
    }
    break;
  case PL_STRING: /* String */
    if(ip==PL_LEER) pret->typ=PL_LEER;
    else if(pin->typ==PL_EVAL) {
      *((STRING *)&(pret->integer))=string_parser(pin->pointer);
      pret->typ=PL_STRING;
    } else if(ip==PL_STRING) {
      *((STRING *)&(pret->integer))=double_string((STRING *)&(pin->integer));
      pret->typ=PL_STRING;
    } else {PARERR();return(-1);}
    break;
  case PL_ARRAY:  /* Array */
  case PL_IARRAY: /* Int-Array */
  case PL_FARRAY: /* float-Array */
  case PL_CARRAY: /* float-Array */
  case PL_NARRAY:
  case PL_CFARRAY:
  case PL_AIARRAY: /* float-Array */
  case PL_SARRAY: /* String-Array */
    if(ip==PL_LEER) pret->typ=PL_LEER;
    else if(pin->typ==PL_ARRAY || pin->typ==PL_EVAL) {
      ARRAY arr;
      if(pin->typ==PL_ARRAY) arr=double_array((ARRAY *)&(pin->integer));
      else arr=array_parser(pin->pointer);
      *((ARRAY *)&(pret->integer))=arr;	
      if(ap==PL_IARRAY && (arr.typ==FLOATTYP||arr.typ==COMPLEXTYP||arr.typ==ARBINTTYP)) {
	*((ARRAY *)&(pret->integer))=convert_to_intarray(&arr);
	free_array(&arr);
      } else if(ap==PL_FARRAY && (arr.typ==INTTYP||arr.typ==COMPLEXTYP||arr.typ==ARBINTTYP)) {
	*((ARRAY *)&(pret->integer))=convert_to_floatarray(&arr);
	free_array(&arr);
      } else if(ap==PL_CARRAY && (arr.typ==INTTYP||arr.typ==FLOATTYP||arr.typ==ARBINTTYP)) {
	*((ARRAY *)&(pret->integer))=convert_to_complexarray(&arr);
	free_array(&arr);
      } else if(ap==PL_AIARRAY && (arr.typ==INTTYP||arr.typ==FLOATTYP||arr.typ==COMPLEXTYP)) {
	*((ARRAY *)&(pret->integer))=convert_to_arbintarray(&arr);
	free_array(&arr);
      } else if((ap==PL_IARRAY && arr.typ==INTTYP) ||
             (ap==PL_FARRAY && arr.typ==FLOATTYP) ||(ap==PL_CARRAY && arr.typ==COMPLEXTYP) ||
	     (ap==PL_SARRAY && arr.typ==STRINGTYP)) {
	  ; /*Nix tun, alles gut.*/
      } else if(ap==PL_NARRAY && (arr.typ==INTTYP||arr.typ==COMPLEXTYP||arr.typ==FLOATTYP||arr.typ==ARBINTTYP)) {
	  ; /*Nix tun, alles gut.*/
      } else if(ap==PL_CFARRAY && (arr.typ==COMPLEXTYP||arr.typ==FLOATTYP)) {
	  ; /*Nix tun, alles gut.*/
      } else if(ap==PL_CFARRAY && (arr.typ==INTTYP||arr.typ==ARBINTTYP)) {
	*((ARRAY *)&(pret->integer))=convert_to_floatarray(&arr);
	free_array(&arr);
      } else if(ap==PL_ARRAY) {
	; /* nixtum*/
      } else {
        printf("line %d: Error: Parameter is wrong (typ=%x) ARRAY (need to be $%x). Cannot convert.\n",pc,ip,ap);
	dump_parameterlist(pin,1);
      }
    } else {PARERR();return(-1);}
    break;
  case PL_VAR:   /* Variable */
  case PL_NVAR:   /* Variable */
  case PL_IVAR:   /* Variable */
  case PL_FVAR:   /* Variable */
  case PL_SVAR:   /* Variable */
  case PL_CVAR:   /* Variable */
  case PL_AIVAR:   /* Variable */
  case PL_ARRAYVAR: /* Variable */
  case PL_IARRAYVAR: /* Variable */
  case PL_FARRAYVAR: /* Variable */
  case PL_SARRAYVAR: /* Variable */
  case PL_CARRAYVAR: /* Variable */
  case PL_AIARRAYVAR: /* Variable */
  case PL_ALLVAR:  /* Varname */    
    pret->typ=ip;
    if(ip==PL_LEER) break;  /* bei leerem Parameter: fertig */
    vnr=pret->integer=pin->integer;
    if(pin->pointer==NULL) {
      /* in pret->pointer wird die ermittelte VARPTR übergeben, auch wenn indicies da sind. */
      if(pin->panzahl) {
        indexliste=malloc(pin->panzahl*sizeof(int));
	get_indexliste(pin->ppointer,indexliste,pin->panzahl);
        // printf("Es sind %d indizien da.\n",pin->panzahl);
      } else indexliste=NULL;
      pret->pointer=varptr_indexliste(&variablen[vnr],indexliste,pin->panzahl);
      // printf("Ermittelte Adresse ist %p\n",pret->pointer);
      free(indexliste);
      if(pret->pointer==NULL) return(-1);
    } else pret->pointer=pin->pointer;
      
      // printf("Variable uebergeben. %d %s\n",vnr,varinfo(&variablen[vnr]));
    break;
  case PL_KEY: /* Keyword */
    if(ip==PL_LEER) pret->arraytyp=KEYW_NONE;
    else if(ip==PL_KEY) pret->arraytyp=pin->arraytyp;
    else if(ip==PL_EVAL) pret->arraytyp=keyword2num(pin->pointer);
    if(ip==PL_EVAL ||ip==PL_KEY) *((STRING *)&(pret->integer))=create_string(pin->pointer);
    else if(ip==PL_LEER) *((STRING *)&(pret->integer))=create_string("");
    else {PARERR();return(-1);}
    break;
  case PL_EVAL: /* Keyword */
    if(ip==PL_LEER) pret->typ=PL_LEER;
    else if(ip==PL_EVAL ||ip==PL_KEY) {
      pret->arraytyp=pin->arraytyp;
      *((STRING *)&(pret->integer))=create_string(pin->pointer);
    } else {PARERR();return(-1);}
    break;
  default:
    printf("ERROR: unknown parameter type. -->$%x \n",ap);
    return(-1);
  }
  return(0);
}

/* Rueckgabe ist die Anzahl der prozessierten Parameter 
   oder -1 wenn ein Fehler aufgetreten ist..
   */

int make_pliste3(int pmin,int pmax,unsigned short *pliste,PARAMETER *pin, PARAMETER **pout,int ii){
  PARAMETER *pret;
  unsigned short ap;
  int i=0,anzpar,err=0;
  if(pmax==-1) anzpar=ii;
  else anzpar=min(ii,pmax);
//  printf("PARAMETER vervollstaendigen: (%d) %d\n",ii,anzpar);

  *pout=pret=calloc(anzpar,sizeof(PARAMETER));
  while(i<anzpar) {
    if(i>pmin && pmax==-1) ap=pliste[pmin];
    else ap=pliste[i];
 //   printf("Par #%d: typ1=%x typ2=%x %s\n",i,ap,ip,plist_paramter(&pin[i]));
    err|=make_parameter_stage3(&pin[i],ap,&pret[i]);
    i++;
  }
  if(err) return(-1);
  return(i);
}

/*Macht aus einer kommaseparierten parliste eine PARAMETER-Array, 
  es durfen allerdings nur Konstanten erzeugt werden, also alles was schon vor der Laufzeit 
  vorbereitet werden kann. Der Rest bekommt ein EVAL und muss dann von make_plist3 ergaenzt werden.
  
  Im unterschied zu make_pliste2 hat diese Funktion keine Vorgaben, von welchem Typ die Parameter
  sein sollten. Wird hauptsaechlich fuer indizies bei Arrays verwendet, also integers.
  
  */
int make_preparlist(PARAMETER *p,char *n) {
  char buf[strlen(n)+1];
  char *w1,*w2;
  int ii=0,e,typ;  
  strcpy(buf,n);
  e=wort_sep_destroy(buf,',',TRUE,&w1,&w2);
  while(e) {
    if(strlen(w1)) {
      typ=type(w1);
      if((typ&CONSTTYP)==CONSTTYP) {  /* Kostanten schon auswerten.*/
        eval2partype(w1,&p[ii],typ);
      } else {                        /*  Rest bleibt f"ur spaeter...*/
        p[ii].typ=PL_EVAL;
	p[ii].arraytyp=typ;
	*((STRING *)&(p[ii].integer))=create_string(w1);
      }
    } else  {
      p[ii].typ=PL_LEER;
      p[ii].pointer=NULL;
    }
    p[ii].panzahl=0;
    p[ii].ppointer=NULL;
    
    e=wort_sep_destroy(w2,',',TRUE,&w1,&w2);
    ii++;
  }
  return(ii);
}




/*Indexliste aus Parameterarray (mit EVAL) zu Laufzeit*/

int get_indexliste(PARAMETER *p,int *l,int n) {
  int i;
  for(i=0;i<n;i++) {
    switch(p[i].typ) {
    case PL_KEY:
    case PL_EVAL:
      if(((char *)p[i].pointer)[0]==':') l[i]=-1;
      else {
        l[i]=(int)parser((char *)p[i].pointer);
	if(l[i]<0) {
	   xberror(16,""); /* Feldindex zu gross*/
	   printf("ERROR: negative array index.<%s>\n",(char *)p[i].pointer);
	}
      }
      break;
    default:
      l[i]=p2int(&p[i]);
    }
  }
  return(i);
}
