/* parameter.c (c) Markus Hoffmann             */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "defs.h"
#include "x11basic.h"
#include "xbasic.h"
#include "variablen.h"
#include "parser.h"
#include "array.h"

#include "wort_sep.h"
#include "parameter.h"


/*Fuellt die Struktur PARAMETER ausgehend von ASCII w1 und solltyp.
  Hierbei nur vorbereitung, also keine parseraufrufe (be Auswertung der
  Index-Liste). Es treten nur Variablen und Array-indizies als Variablen
  auf. Es werden nur gefuellt:
  p->typ --> Variablentyp
  p->pointer=Null, da vaiableninhalt sich noch aendern koennte
  p->integer --> vnr
  p->panzahl
  p->ppointer
  */
  

void prepare_vvar(char *w1,PARAMETER *p, unsigned int solltyp) {
  char k1[strlen(w1)+1],k2[strlen(w1)+1];
  int typ=vartype(w1);
  int e=klammer_sep(w1,k1,k2);

  p->pointer=NULL;
  if(e==1 || strlen(k2)==0) {  /* Keine Argumente in Klammer oder keine klammern*/
    if((typ&solltyp)==typ) {
      char *r=varrumpf(w1);
      /*p->integer soll vnr bekommen.*/
      if(typ&ARRAYTYP) {
        p->integer=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP));
      } else p->integer=add_variable(r,typ,0);
      free(r);
      /*Parameter Typ eintragen.*/
      if(typ==ARRAYTYP) p->typ=PL_ARRAYVAR;
      else if(typ==INTTYP) p->typ=PL_IVAR;
      else if(typ==FLOATTYP) p->typ=PL_FVAR;
      else if(typ==STRINGTYP) p->typ=PL_SVAR;
      else if(typ==(ARRAYTYP|INTTYP)) p->typ=PL_IARRAYVAR;
      else if(typ==(ARRAYTYP|FLOATTYP)) p->typ=PL_FARRAYVAR;
      else if(typ==(ARRAYTYP|STRINGTYP)) p->typ=PL_SARRAYVAR;
      else printf("vvar: ???\n");
    } else printf("prepare_vvar: ERROR: Variable hat falschen Typ. $%x/$%x  <%s>\n",typ,solltyp,w1);
  } else { /* Es sind indizies da. */
    typ&=~ARRAYTYP;
    if((typ&solltyp)==typ) {
      char *r=varrumpf(w1);
      p->integer=add_variable(r,ARRAYTYP,typ);  /*  vnr */
      free(r);
      if(typ==INTTYP) p->typ=PL_IVAR;
      else if(typ==FLOATTYP) p->typ=PL_FVAR;
      else if(typ==STRINGTYP) p->typ=PL_SVAR;
      else printf("vvar: ???\n");
      p->panzahl=count_parameters(k2);   /* Anzahl indizes z"ahlen*/
      p->ppointer=malloc(sizeof(PARAMETER)*p->panzahl);
      /*hier die Indizies in einzelne zu evaluierende Ausdruecke
        separieren*/
      make_preparlist(p->ppointer,k2);
    } else printf("ERROR: Variable hat falschen Typ. $%x/$%x\n",typ,solltyp);
  }
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


void free_parameter(PARAMETER *p) {
  switch(p->typ) {
    ARRAY a;

  case PL_STRING:
  case PL_EVAL:
  case PL_KEY:
    free(p->pointer);
  case PL_INT:
  case PL_FLOAT:
  case PL_LEER:
  case PL_NUMBER:
  case PL_FILENR:
  case PL_LABEL:
  case PL_PROC:
  case PL_FUNC:
    break;
  case PL_ALLVAR:
  case PL_ARRAYVAR:
  case PL_IARRAYVAR:
  case PL_FARRAYVAR:
  case PL_SARRAYVAR:
  case PL_VAR:
  case PL_IVAR:
  case PL_FVAR:
  case PL_NVAR:
  case PL_SVAR:
    if(p->panzahl) {
      free_pliste(p->panzahl,p->ppointer);
      p->panzahl=0;
    }
    break;
  case PL_ARRAY:
  case PL_IARRAY:
  case PL_FARRAY:
  case PL_SARRAY:
  case PL_NARRAY:
  case PL_ANYARRAY:
    a.typ=p->typ;
    a.dimension=p->integer;
    a.pointer=p->pointer;
    free_array(&a);
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
        case PL_NUMBER: printf(" num %g\n",p[j].real); break;
        case PL_STRING: printf("   $ <%s> len=%d\n",(char *)p[j].pointer,p[j].integer);break;
        case PL_KEY:    printf(" KEY <%s>\n",(char *)p[j].pointer);break;
        case PL_LEER:   printf(" <empty>\n");break;
        case PL_FILENR: printf("   # %d\n",p[j].integer);break;
        case PL_EVAL:   printf(" EVAL: <%s>\n",(char *)p[j].pointer);break;
        case PL_LABEL:  printf(" <label>\n");break;
        case PL_PROC:   printf(" <proc>\n");break;
        case PL_FUNC:   printf(" <func>\n");break;
        case PL_ARRAY:  printf(" <array,$%x,dim=%d>\n",p[j].arraytyp,p[j].integer);break;
	case PL_FVAR:   printf(" <var,%d,%s>\n",p[j].integer,variablen[p[j].integer].name);break;
	case PL_IVAR:   printf(" <var%%,%d,%s>\n",p[j].integer,variablen[p[j].integer].name);break; 
        case PL_SVAR:   printf(" <var$,%d,%s>\n", p[j].integer,variablen[p[j].integer].name);break; 
        case PL_ALLVAR: printf(" <allvar,%d,%s>\n",p[j].integer,variablen[p[j].integer].name);break;
	case PL_NVAR:   printf(" <nvar,%d,%s>\n", p[j].integer,variablen[p[j].integer].name);break;
	case PL_VAR:    printf(" <var,%d,%s>\n",  p[j].integer,variablen[p[j].integer].name);break;
        case PL_ARRAYVAR:  
        case PL_IARRAYVAR: 
        case PL_FARRAYVAR: 
        case PL_SARRAYVAR: 
        default:   printf("$%x %d %g %p\n",p[j].typ,p[j].integer,p[j].real,(void *)p[j].pointer);
      }
      if(p->panzahl>0 && (p[j].typ&~PL_BASEMASK)==(PL_VAR&~PL_BASEMASK)) {
         printf("%d Index-Parameters:\n",p->panzahl);
         if(p->ppointer) dump_parameterlist(p->ppointer,p->panzahl);
      }
    }
  }
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
      
     // pret[i].pointer=NULL; /* Default is NULL */
     // pret[i].ppointer=NULL;
     // pret[i].panzahl=0;
     // pret[i].integer=0;
     // pret[i].real=0.0;
      if(i>pmin && pmax==-1) ap=pliste[pmin];
      else ap=pliste[i];
      if(strlen(w1)){
        pret[i].typ=ap;
      // printf("ap=%x ",ap);
        switch(ap) {
	  char *pos2,*pos;
	  int typ;
        case PL_LABEL: 
          pret[i].integer=labelnr(w1);
          if(pret[i].integer==-1) {
	    xberror(20,w1);/* Label nicht gefunden */
	  }
          break;
	case PL_PROC:
	  pos=searchchr(w1,'(');
          if(pos!=NULL) {
            pos[0]=0;pos++;
            pos2=pos+strlen(pos)-1;
            if(pos2[0]!=')') {
	      xberror(32,w1); /* Syntax error */
            }
            else pos2[0]=0;
          } else pos=w1+strlen(w1);
          pret[i].integer=procnr(w1,1);
          if(pret[i].integer==-1) {
            xberror(19,w1); /* Procedure nicht gefunden */
          }
	  break;
	case PL_FILENR:   /*   TODO: Hier schon auf Konstanten testen !!!!*/
          if(type(w1)&CONSTTYP) {
            pret[i].typ=PL_FILENR;
            if(*w1=='#') pret[i].integer=(int)parser(w1+1);
	    else pret[i].integer=(int)parser(w1);
	  } else {
            pret[i].typ=PL_EVAL;
	    if(*w1=='#') *((STRING *)&(pret[i].integer))=create_string(w1+1);
	    else *((STRING *)&(pret[i].integer))=create_string(w1);
          }
	  break;
        case PL_INT:	/* Integer */
	case PL_FLOAT:  /* Float oder Nuber */
        case PL_NUMBER:	/* Integer or FLOAT */
	case PL_ANYVALUE:	/* Integer or FLOAT oder string*/
	case PL_STRING: /* String */
        case PL_ARRAY:  /* Array */
        case PL_IARRAY: /* Int-Array */
        case PL_FARRAY: /* float-Array */
        case PL_SARRAY: /* String-Array */
          if((typ=type(w1))&CONSTTYP) {
	     STRING str;
             switch(ap) {
             case PL_INT:	/* Integer */
	       pret[i].typ=PL_INT;
	       pret[i].integer=(int)parser(w1);
	       break;
	     case PL_ANYVALUE:  /* Integer or FLOAT oder string*/
	       if(typ&INTTYP) {
	         pret[i].typ=PL_INT;
	         pret[i].integer=(int)parser(w1);
	       } else if(typ&FLOATTYP) {
	         pret[i].typ=PL_FLOAT;
	         pret[i].real=parser(w1);
	       } else if(typ&STRINGTYP) {
	         pret[i].typ=PL_STRING;
	         str=string_parser(w1);
	         pret[i].integer=str.len;
	         pret[i].pointer=str.pointer;
	       } else {
	         printf("WARNING: unknown typ. $%x\n",typ);
	       }
	       break;
	     case PL_FLOAT:  /* Float oder Nuber */
             case PL_NUMBER:	/* Integer or FLOAT */
	       pret[i].typ=PL_FLOAT;
	       pret[i].real=parser(w1);
	       break;
	     case PL_STRING: /* String */
	       pret[i].typ=PL_STRING;
	       *((STRING *)&(pret[i].integer))=string_parser(w1);
               break;
             case PL_ARRAY:  /* Array */
             case PL_IARRAY: /* Int-Array */
             case PL_FARRAY: /* float-Array */
             case PL_SARRAY: /* String-Array */
               pret[i].typ=PL_EVAL;
	       *((STRING *)&(pret[i].integer))=create_string(w1);
	     }
	  } else {
            pret[i].typ=PL_EVAL;
	    *((STRING *)&(pret[i].integer))=create_string(w1);
          }
	  break;
 	case PL_VAR:       prepare_vvar(w1,&pret[i],INTTYP|FLOATTYP|STRINGTYP);          break;
 	case PL_NVAR:      prepare_vvar(w1,&pret[i],INTTYP|FLOATTYP);                    break;
        case PL_SVAR:      prepare_vvar(w1,&pret[i],STRINGTYP);                          break;
 	case PL_ARRAYVAR:  prepare_vvar(w1,&pret[i],ARRAYTYP|INTTYP|FLOATTYP|STRINGTYP); break;
 	case PL_IARRAYVAR: prepare_vvar(w1,&pret[i],INTTYP|ARRAYTYP);                    break;
 	case PL_FARRAYVAR: prepare_vvar(w1,&pret[i],FLOATTYP|ARRAYTYP);                  break;
 	case PL_SARRAYVAR: prepare_vvar(w1,&pret[i],STRINGTYP|ARRAYTYP);                 break;
	case PL_ALLVAR:    prepare_vvar(w1,&pret[i],INTTYP|FLOATTYP|STRINGTYP|ARRAYTYP); break;
 	case PL_KEY: /* Keyword */
        case PL_EVAL:
	  *((STRING *)&(pret[i].integer))=create_string(w1);
	  break;
	default:
          printf("illegal parameter type $%x\n",ap);
        }
      } else pret[i].typ=PL_LEER;           /* Hier dann Leertyp*/
      i++;
    }
  }
  if(i<pmin) xberror(42,""); /* Zu wenig Parameter  */
  else if(i==pmax && pos) xberror(45,""); /* Zu viele Parameter  */
  return(i);
}



/*Hier Ergaenzungen von pre-pliste zu aktueller (Zu Laufzeit). 
Jetzt koennen variableninhalte
aus fix betrachtet werden.*/

int make_pliste3(int pmin,int pmax,unsigned short *pliste,PARAMETER *pin, PARAMETER **pout,int ii){
  PARAMETER *pret;
  unsigned short ap,ip;
  int i;
  int anzpar;
  if(pmax==-1) anzpar=ii;
  else anzpar=min(ii,pmax);
//  printf("PARAMETER vervollstaendigen: (%d) %d\n",ii,anzpar);

  *pout=pret=calloc(anzpar,sizeof(PARAMETER));
  
  i=0;
  while(i<anzpar) {
    // pret[i].pointer=NULL; /* Default is NULL */
    // pret[i].integer=0;
    // pret[i].real=0.0;
    // pret[i].panzahl=0;
    // pret[i].ppointer=NULL;
    if(i>pmin && pmax==-1) ap=pliste[pmin];
    else ap=pliste[i];
    ip=pin[i].typ;
    pret[i].typ=ap;
 //   printf("Par #%d: typ1=%x typ2=%x %s\n",i,ap,ip,plist_paramter(&pin[i]));
  
    switch(ap) {
      int vnr,typ;  
      int *indexliste;
      STRING str;

    case PL_LEER:
      break;
    case PL_LABEL: 
    case PL_PROC:
      if(ip==PL_LABEL || ip==PL_PROC) pret[i].integer=pin[i].integer;
      else printf("Error: Parameter %d/%d incompatibel. $%x\n",i,anzpar,ip);
      break;
    case PL_FILENR:
    case PL_INT:    /* Integer */
      if(ip==PL_LEER) pret[i].typ=PL_LEER;
      else if(ip==PL_EVAL) pret[i].integer=(int)parser(pin[i].pointer);
      else if(ip==PL_INT)  pret[i].integer=pin[i].integer;
      else if(ip==PL_FILENR)  pret[i].integer=pin[i].integer;
      else if(ip==PL_FLOAT) pret[i].integer=(int)pin[i].real;
      else {
        printf("Error: Parameter %d/%d incompatibel. $%x\n",i,anzpar,ip);
        dump_parameterlist(pin,anzpar);
      }
      break;
    case PL_NUMBER:  /* Float oder Nuber */
    case PL_FLOAT:  /* Float oder Nuber */
      if(ip==PL_LEER) pret[i].typ=PL_LEER;
      else if(ip==PL_EVAL) pret[i].real=parser(pin[i].pointer);
      else if(ip==PL_FLOAT) pret[i].real=pin[i].real;
      else if(ip==PL_INT) pret[i].real=(double)pin[i].integer;
      else {
        printf("Error: Parameter %d incompatibel. $%x\n",i,ip);
	dump_parameterlist(pin,anzpar);
      }
      break;
    case PL_ANYVALUE:
 
      if(ip==PL_LEER) pret[i].typ=PL_LEER;
      else if(ip==PL_EVAL) {
        typ=type(pin[i].pointer);
 	if(typ&INTTYP) {
	  pret[i].typ=PL_INT;
	  pret[i].integer=(int)parser(pin[i].pointer);
	} else if(typ&FLOATTYP) {
	  pret[i].typ=PL_FLOAT;
	  pret[i].real=parser(pin[i].pointer);
	} else if(typ&STRINGTYP) {
	  pret[i].typ=PL_STRING;
	  str=string_parser(pin[i].pointer);
	  pret[i].integer=str.len;
	  pret[i].pointer=str.pointer;
	} else {
	  printf("WARNING: unknown typ. $%x\n",typ);
	}
      } else if(ip==PL_FLOAT) {
        pret[i].real=pin[i].real;
	pret[i].typ=PL_FLOAT;
      } else if(ip==PL_INT) {
        pret[i].integer=pin[i].integer;
	pret[i].typ=PL_INT;
      } else if(ip==PL_STRING) {
        *((STRING *)&(pret[i].integer))=double_string((STRING *)&(pin[i].integer));
	pret[i].typ=PL_STRING;
      } else {
        printf("line %d: Error: Parameter %d incompatibel. $%x\n",pc,i,ip);
	dump_parameterlist(pin,anzpar);
      }
      break;
    case PL_STRING: /* String */
      if(ip==PL_LEER) pret[i].typ=PL_LEER;
      else if(pin[i].typ==PL_EVAL) {
	*((STRING *)&(pret[i].integer))=string_parser(pin[i].pointer);
      } else if(ip==PL_STRING) {
        *((STRING *)&(pret[i].integer))=double_string((STRING *)&(pin[i].integer));
      } else {
        printf("line %d: Error: Parameter %d incompatibel. $%x (should be string)\n",pc,i,ip);
	dump_parameterlist(pin,anzpar);
      }
      break;
    case PL_ARRAY:  /* Array */
    case PL_IARRAY: /* Int-Array */
    case PL_FARRAY: /* float-Array */
    case PL_SARRAY: /* String-Array */
      if(ip==PL_LEER) pret[i].typ=PL_LEER;
      else if(pin[i].typ==PL_ARRAY || pin[i].typ==PL_EVAL) {
        ARRAY arr;
	if(pin[i].typ==PL_ARRAY) arr=double_array((ARRAY *)&(pin[i].integer));
	else arr=array_parser(pin[i].pointer);
        *((ARRAY *)&(pret[i].integer))=arr;
	if(ap==PL_IARRAY && arr.typ==FLOATTYP) {
	  *((ARRAY *)&(pret[i].integer))=convert_to_intarray(&arr);
	  free_array(&arr);
	} else if(ap==PL_FARRAY && arr.typ==INTTYP) {
	  *((ARRAY *)&(pret[i].integer))=convert_to_floatarray(&arr);
	  free_array(&arr);
	} else if((ap==PL_IARRAY && arr.typ==INTTYP) ||
	   (ap==PL_FARRAY && arr.typ==FLOATTYP) ||
	   (ap==PL_SARRAY && arr.typ==STRINGTYP)) {
	  ;
        } else printf("line %d: Error: Parameter %d is wrong ARRAY type $%x (need to be $%x). Cannot convert.\n",pc,i,ip,ap);
      } else {
        printf("line %d: Error: Parameter %d incompatibel. $%x (should be Array)\n",pc,i,ip);
	dump_parameterlist(pin,anzpar);
      }
      break;
    case PL_VAR:   /* Variable */
    case PL_NVAR:   /* Variable */
    case PL_SVAR:   /* Variable */
    case PL_ARRAYVAR: /* Variable */
    case PL_IARRAYVAR: /* Variable */
    case PL_FARRAYVAR: /* Variable */
    case PL_SARRAYVAR: /* Variable */
    case PL_ALLVAR:  /* Varname */    
      pret[i].typ=ip;
      vnr=pret[i].integer=pin[i].integer;
      if(pin[i].pointer==NULL) {
        if(pin[i].panzahl) {
          indexliste=malloc(pin[i].panzahl*sizeof(int));
	  get_indexliste(pin[i].ppointer,indexliste,pin[i].panzahl);
          // printf("Es sind %d indizien da.\n",pin[i].panzahl);
        } else indexliste=NULL;
        pret[i].pointer=varptr_indexliste(&variablen[vnr],indexliste,pin[i].panzahl);
        free(indexliste);
      } else pret[i].pointer=pin[i].pointer;
      
      // printf("Variable uebergeben. %d %s\n",vnr,varinfo(&variablen[vnr]));
      break;
    case PL_KEY: /* Keyword */
      *((STRING *)&(pret[i].integer))=create_string(pin[i].pointer);
      break;
    case PL_EVAL: /* Keyword */
      if(ip==PL_LEER) pret[i].typ=PL_LEER;
      else if(ip==PL_EVAL ||ip==PL_KEY) {
        *((STRING *)&(pret[i].integer))=create_string(pin[i].pointer);
      } else printf("Error: Parameter %d incompatibel. $%x\n",i,ip);
      break;
    default:
      printf("unknown parameter type. -->$%x \n",ap);
    }
    i++;
  }
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
  int ii,e,typ;
  ii=0;
  strcpy(buf,n);
  e=wort_sep_destroy(buf,',',TRUE,&w1,&w2);
  while(e) {
    if(strlen(w1)) {
      typ=type(w1);
      if((typ&CONSTTYP)==CONSTTYP) {
        switch(typ) {
	case (CONSTTYP|INTTYP):
          p[ii].typ=PL_INT;
          p[ii].integer=(int)parser(w1);
	  break;	  
	case (CONSTTYP|FLOATTYP):
          p[ii].typ=PL_FLOAT;
          p[ii].real=parser(w1);
	  break;	  
	case (CONSTTYP|STRINGTYP):
          p[ii].typ=PL_STRING;
          *((STRING *)&(p[ii].integer))=string_parser(w1);
	  break;
	default:
          p[ii].typ=PL_EVAL;
	  *((STRING *)&(p[ii].integer))=create_string(w1);
	}
      } else {
        p[ii].typ=PL_EVAL;
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
      else l[i]=(int)parser((char *)p[i].pointer);
      break;
    case PL_INT:
      l[i]=p[i].integer;
      break;
    case PL_NUMBER:
    case PL_FLOAT:
      l[i]=(int)p[i].real;
      break;
    default:
      printf("WARNING: get_indexliste: illegal type.\n");
      l[i]=0;
    }
  }
  return(i);
}



void make_indexliste_plist(int dim, PARAMETER *p, int *index) {
  while(--dim>=0) {
      if(p[dim].typ==PL_INT) index[dim]=p[dim].integer;
      else if(p[dim].typ==PL_FLOAT) index[dim]=(int)p[dim].real;
      else printf("ERROR: no int!");    
  }
}

