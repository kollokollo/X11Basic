/* array.c (c) Markus Hoffmann             */

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
#include "variablen.h"

#include "xbasic.h"
#include "type.h"
#include "wort_sep.h"
#include "array.h"
#include "parser.h"
#include "parameter.h"
#include "number.h"
#include "mathematics.h"

/******************** Array variable routines **************************/
/* fill array with a constant */
void fill_int_array(const ARRAY *arr, const int inh) {
  int anz=anz_eintraege(arr); 
  int *pp=(int *)(arr->pointer+arr->dimension*INTSIZE); 
  while(--anz>=0) pp[anz]=inh;
}
void fill_float_array(const ARRAY *arr, const double inh) {
  int anz=anz_eintraege(arr);
  double *pp=(double *)(arr->pointer+arr->dimension*INTSIZE);
  while(--anz>=0) pp[anz]=inh;
}
void fill_complex_array(const ARRAY *arr, const COMPLEX inh) {
  int anz=anz_eintraege(arr);
  COMPLEX *pp=(COMPLEX *)(arr->pointer+arr->dimension*INTSIZE);
  while(--anz>=0) pp[anz]=inh;
}
void fill_arbint_array(const ARRAY *arr, const ARBINT inh) {
  int anz=anz_eintraege(arr);
  ARBINT *pp=(ARBINT *)(arr->pointer+arr->dimension*INTSIZE);
  while(--anz>=0) {
    mpz_init(pp[anz]);
    mpz_set(pp[anz],inh);
  }
}
void fill_string_array(const ARRAY *arr, const STRING inh) {
  int anz=anz_eintraege(arr); 
  STRING *varptr=(STRING *)(arr->pointer+arr->dimension*INTSIZE); 
  while(--anz>=0) {
    varptr[anz].pointer=realloc(varptr[anz].pointer,inh.len+1);
    memcpy(varptr[anz].pointer,inh.pointer,inh.len);
    varptr[anz].len=inh.len; 
  }	
}
void fill_array_array(const ARRAY *arr, const ARRAY inh) {
  int anz=anz_eintraege(arr); 
  ARRAY *varptr=(ARRAY *)(arr->pointer+arr->dimension*INTSIZE); 
  while(--anz>=0) {
    free_array(&(varptr[anz]));
    varptr[anz]=double_array(&inh);
  }	
}


/********************* ARRAY Structure Routines ************************/
/* create array (need to be freed later) and fill it with a constant value */
/* Erzeuge jeweils entsprechendes Array und belege es mit value vor. */

ARRAY create_int_array(const uint32_t dimension, const uint32_t *dimlist,const int value) {
  ARRAY ergebnis;
  int anz=1,j;
  int *varptr;
  ergebnis.typ=INTTYP;
  ergebnis.dimension=dimension;
  if(dimension>0) {for(j=0;j<dimension;j++) anz=anz*dimlist[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*sizeof(int));
  varptr=(int *)(((char *)ergebnis.pointer)+dimension*INTSIZE);
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(uint32_t));
  while(--anz>=0) varptr[anz]=value;    
  return(ergebnis);
}
ARRAY create_float_array(const uint32_t dimension, const uint32_t *dimlist,const double value) {
  ARRAY ergebnis;
  int anz=1,j;
  double *varptr;
  ergebnis.typ=FLOATTYP;
  ergebnis.dimension=dimension;
  if(dimension>0) {for(j=0;j<dimension;j++) anz=anz*dimlist[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*sizeof(double));
  varptr=(double *)(((char *)ergebnis.pointer)+dimension*INTSIZE);
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(uint32_t));
  while(--anz>=0) varptr[anz]=value;    
  return(ergebnis);
}
ARRAY create_complex_array(const uint32_t dimension, const uint32_t *dimlist,const COMPLEX *value) {
  ARRAY ergebnis;
  int anz=1,j;
  COMPLEX *varptr;
  ergebnis.typ=COMPLEXTYP;
  ergebnis.dimension=dimension;
  if(dimension>0) {for(j=0;j<dimension;j++) anz=anz*dimlist[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*sizeof(COMPLEX));
  varptr=(COMPLEX *)(((char *)ergebnis.pointer)+dimension*INTSIZE);
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(uint32_t));
  while(--anz>=0) varptr[anz]=*value;    
  return(ergebnis);
}
ARRAY create_arbint_array(const uint32_t dimension, const uint32_t *dimlist,ARBINT value) {
  ARRAY ergebnis;
  int anz=1,j;
  ARBINT *varptr;
  ergebnis.typ=ARBINTTYP;
  ergebnis.dimension=dimension;
  if(dimension>0) {for(j=0;j<dimension;j++) anz=anz*dimlist[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*sizeof(ARBINT));
  varptr=(ARBINT *)(((char *)ergebnis.pointer)+dimension*INTSIZE);
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(uint32_t));
  while(--anz>=0) {
    mpz_init(varptr[anz]);
    mpz_set(varptr[anz],value);
  }
  return(ergebnis);
}



ARRAY create_string_array(const uint32_t dimension,const uint32_t *dimlist,const STRING *value) {
  ARRAY ergebnis;
  int anz=1,j;
  STRING *varptr;
  ergebnis.typ=STRINGTYP;
  ergebnis.dimension=dimension;
  if(dimension>0) {for(j=0;j<dimension;j++) anz=anz*dimlist[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*sizeof(STRING));
  varptr=(STRING *)(((char *)ergebnis.pointer)+dimension*INTSIZE);
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(uint32_t));
  while(--anz>=0) varptr[anz]=double_string(value);    
  return(ergebnis);
}





ARRAY create_array_array(const uint32_t dimension, const uint32_t *dimlist,const ARRAY *value) {
  ARRAY ergebnis;
  int anz=1,j;
  ARRAY *varptr;
  ergebnis.typ=ARRAYTYP;
  ergebnis.dimension=dimension;
  if(dimension>0) {for(j=0;j<dimension;j++) anz=anz*dimlist[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*sizeof(ARRAY));
  varptr=(ARRAY *)(((char *)ergebnis.pointer)+dimension*INTSIZE);
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(uint32_t));
  while(--anz>=0) varptr[anz]=double_array(value);
  return(ergebnis);
}

/*  Erzeuge ein Array und fuelle es mit 0 bzw mit Strings von Laenge 0
    oder Arrays mit NOTYP und dimension 0 */

ARRAY create_array(const unsigned short typ, const uint32_t dimension, const uint32_t *dimlist) {
  ARRAY ergebnis;
  int anz=1;
  ergebnis.typ=typ;
  ergebnis.dimension=dimension;
  if(dimension>0 && dimlist) {int j;for(j=0;j<dimension;j++) anz=anz*dimlist[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*typlaenge(typ));
  /* dimlist kopieren */
  if(dimension>0 && dimlist) memcpy(ergebnis.pointer,dimlist,dimension*sizeof(uint32_t));
  
  switch(typ) {
    case NOTYP: break;
    case STRINGTYP: {
      STRING *varptr=(STRING *)(ergebnis.pointer+dimension*INTSIZE);
      while(--anz>=0) varptr[anz]=create_string(NULL);
      } break;
    case ARBINTTYP: {
      ARBINT *varptr=(ARBINT *)(ergebnis.pointer+dimension*INTSIZE);
      while(--anz>=0) mpz_init(varptr[anz]);
      } break;
    case INTTYP: {
      int *varptr=(int *)(ergebnis.pointer+dimension*INTSIZE);
      while(--anz>=0) varptr[anz]=0;
      } break;
    case FLOATTYP: {
      double *varptr=(double *)(ergebnis.pointer+dimension*INTSIZE);
      while(--anz>=0) varptr[anz]=0;
      } break;
    case COMPLEXTYP: {
      COMPLEX *varptr=(COMPLEX *)(ergebnis.pointer+dimension*INTSIZE);
      while(--anz>=0) {varptr[anz].r=0;varptr[anz].i=0;}
      } break;
    case ARRAYTYP: {
      ARRAY *varptr=(ARRAY *)(ergebnis.pointer+dimension*INTSIZE);
      while(--anz>=0) varptr[anz]=create_array(NOTYP,0,NULL);
      } break;
  }
  return(ergebnis);
}
/*  Erzeuge ein Array und fuelle es soweit möglich mit Inhalt aus einem anderen Array.
    Erweitere mit 0 bzw mit Strings von Laenge 0
    oder Arrays mit NOTYP und dimension 0 
    
    Es geht noch nicht: Bei mehrdimensionalen Arrays verschieben sich die Elemente. 
    Wenn die Dimension geanedert wird, was sollen wir tun?
    
    
    
    */

ARRAY recreate_array(const int typ, const int dimension, const int *dimlist, const ARRAY *uarr) {
  if(uarr==NULL) return(create_array(typ, dimension, dimlist));
  if(typ!=uarr->typ) xberror(13,"");  /* Type mismatch */
  ARRAY ergebnis;
  ergebnis.typ=typ;
  ergebnis.dimension=dimension;
  int idx[max(dimension,uarr->dimension)];
  int anz=1;
  if(dimension>0 && dimlist) {int j;for(j=0;j<dimension;j++) anz=anz*((int *)dimlist)[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*typlaenge(typ));
  /* dimlist kopieren */
  if(dimension>0 && dimlist) memcpy(ergebnis.pointer,dimlist,dimension*sizeof(int));

  /* Fuer den Fall, dass das neue Array eine kleinere Dimension besitzt als das
     alte, muesste die Dimlist mit 0 vorbelegt werden.*/
  bzero(idx,sizeof(idx));

  switch(typ) {
    case NOTYP: break;
    case STRINGTYP: {
      STRING *varptr=(STRING *)(ergebnis.pointer+dimension*INTSIZE);    
      while(--anz>=0) {
        anz2idx(anz,&ergebnis,idx);
        if(check_indexliste(uarr,idx)) varptr[anz]=string_array_element(uarr,idx);
        else varptr[anz]=create_string(NULL);
      }
    } break;
    case INTTYP: {
      int *varptr=(int *)(ergebnis.pointer+dimension*INTSIZE);
      while(--anz>=0) {
        anz2idx(anz,&ergebnis,idx);
        if(check_indexliste(uarr,idx)) varptr[anz]=int_array_element(uarr,idx);
        else varptr[anz]=0;
      }
    } break;
    case FLOATTYP: {
      double *varptr=(double *)(ergebnis.pointer+dimension*INTSIZE);
      while(--anz>=0) {
        anz2idx(anz,&ergebnis,idx);
        if(check_indexliste(uarr,idx)) varptr[anz]=float_array_element(uarr,idx);
        else varptr[anz]=0;
      }
    } break;
    case COMPLEXTYP: {
      COMPLEX *varptr=(COMPLEX *)(ergebnis.pointer+dimension*INTSIZE);
      while(--anz>=0) {
        anz2idx(anz,&ergebnis,idx);
        if(check_indexliste(uarr,idx)) varptr[anz]=complex_array_element(uarr,idx);
        else varptr[anz].r=varptr[anz].i=0;
      }
    } break;
    case ARBINTTYP: {
      ARBINT *varptr=(ARBINT *)(ergebnis.pointer+dimension*INTSIZE);
      while(--anz>=0) {
        anz2idx(anz,&ergebnis,idx);
        if(check_indexliste(uarr,idx)) {mpz_init(varptr[anz]);arbint_array_element(uarr,idx,varptr[anz]);}
        else mpz_init(varptr[anz]);
      }
    } break;
    case ARRAYTYP: {
      ARRAY *varptr=(ARRAY *)(ergebnis.pointer+dimension*INTSIZE);
      while(--anz>=0) {
        anz2idx(anz,&ergebnis,idx);
        if(check_indexliste(uarr,idx)) varptr[anz]=array_array_element(uarr,idx);
        else varptr[anz]=create_array(NOTYP,0,NULL);
      }
    } break;
   default: printf("ERROR: Recreate array unknown type\n");
  }
  return(ergebnis);
}
void free_array(ARRAY *arr) {
 //  printf("free_array: %d   %p/%p\n",arr->typ,arr,arr->pointer);
  if(arr->pointer) {
    int anz=anz_eintraege(arr);
    if(arr->typ==STRINGTYP) {
      STRING *varptr=(STRING *)(arr->pointer+arr->dimension*INTSIZE); 
      while(--anz>=0) free(varptr[anz].pointer); 	  
    } else if(arr->typ==ARRAYTYP) {  /*ARRAY of ARRAYs ....*/
      ARRAY *varptr=(ARRAY *)(arr->pointer+arr->dimension*INTSIZE); 
      while(--anz>=0) free_array(&varptr[anz]); 	  
    }
    free(arr->pointer);
    arr->pointer=NULL;
  }
  arr->typ=NOTYP;  
} 

/*************** convert/cast arrays to different type ******************/
ARRAY convert_to_complexarray(const ARRAY *a) {
  ARRAY ergeb=create_array(COMPLEXTYP,a->dimension,a->pointer);
  COMPLEX *varptr=(COMPLEX *)(ergeb.pointer+ergeb.dimension*INTSIZE); 
  int anz=anz_eintraege(a);
  switch(a->typ) {
  case FLOATTYP: {
    double *pp2=(double *)(a->pointer+a->dimension*INTSIZE);
    while(--anz>=0) {
      varptr[anz].r=pp2[anz];
      varptr[anz].i=0;
    }
    }
    break;
  case INTTYP: {
    int *pp2=(int *)(a->pointer+a->dimension*INTSIZE);
    while(--anz>=0) {
      varptr[anz].r=(double)pp2[anz];
      varptr[anz].i=0;
    }
    }
    break;
  }
  return(ergeb);
}
ARRAY convert_to_floatarray(const ARRAY *a) {
  ARRAY ergeb=create_array(FLOATTYP,a->dimension,a->pointer);
  double *varptr=(double *)(ergeb.pointer+ergeb.dimension*INTSIZE); 
  int anz=anz_eintraege(a);
  switch(a->typ) {
    case COMPLEXTYP:
      {COMPLEX *pp2=(COMPLEX *)(a->pointer+a->dimension*INTSIZE);
       while(--anz>=0) varptr[anz]=pp2[anz].r; }
      break;
    case INTTYP:
      {int *pp2=(int *)(a->pointer+a->dimension*INTSIZE); 
       while(--anz>=0) varptr[anz]=(double)pp2[anz]; }
      break;
  }
  return(ergeb);
}
ARRAY convert_to_intarray(const ARRAY *a) {
  ARRAY ergeb=create_array(INTTYP,a->dimension,a->pointer);
  int *varptr=(int *)(ergeb.pointer+ergeb.dimension*INTSIZE); 
  int anz=anz_eintraege(a);
   switch(a->typ) {
    case COMPLEXTYP:
      {COMPLEX *pp2=(COMPLEX *)(a->pointer+a->dimension*INTSIZE);
       while(--anz>=0) varptr[anz]=(int)pp2[anz].r; }
      break;
    case FLOATTYP:
      {double *pp2=(double *)(a->pointer+a->dimension*INTSIZE); 
       while(--anz>=0) varptr[anz]=(int)pp2[anz]; }
      break;
  }
  return(ergeb);
}
ARRAY convert_to_arbintarray(const ARRAY *a) {
  ARRAY ergeb=create_array(ARBINTTYP,a->dimension,a->pointer);
  ARBINT *varptr=(ARBINT *)(ergeb.pointer+ergeb.dimension*INTSIZE); 
  int anz=anz_eintraege(a);
   switch(a->typ) {
    case COMPLEXTYP:
      {COMPLEX *pp2=(COMPLEX *)(a->pointer+a->dimension*INTSIZE);
       while(--anz>=0) mpz_set_d(varptr[anz],pp2[anz].r); }
      break;
    case FLOATTYP:
      {double *pp2=(double *)(a->pointer+a->dimension*INTSIZE); 
       while(--anz>=0) mpz_set_d(varptr[anz],pp2[anz]); }
      break;
    case INTTYP:
      {double *pp2=(double *)(a->pointer+a->dimension*INTSIZE); 
       while(--anz>=0) mpz_set_si(varptr[anz],(int)pp2[anz]); }
      break;
  }
  return(ergeb);
}
ARRAY convert_to_xarray(const ARRAY *a,int rt) {
  if(a->typ!=rt) {
    switch(rt) {
    case INTTYP: 
      return(convert_to_intarray(a));
    case FLOATTYP: 
      return(convert_to_floatarray(a));
    case COMPLEXTYP: 
      return(convert_to_complexarray(a));
    case ARBINTTYP: 
      return(convert_to_arbintarray(a));
    default: xberror(96,""); /* ARRAY %s has the wrong type. */ 
    }
  } 
  return(double_array(a));
}
/***********clone an array structure **********************/
/* Kopiert ein Array  */
ARRAY double_array(const ARRAY *a) {
  int anz=anz_eintraege(a);
  ARRAY b=*a;
  int size=b.dimension*INTSIZE+anz*typlaenge(b.typ);
  // printf("double array: %p/%p\n",a,a->pointer);
  b.pointer=malloc(size);
  // printf("  array: %p --> %p\n",a->pointer,b.pointer);
  memcpy(b.pointer,a->pointer,size);
  if(b.typ==STRINGTYP) {
    STRING *ppp1=(STRING *)(b.pointer+b.dimension*INTSIZE);
    STRING *ppp2=(STRING *)(a->pointer+b.dimension*INTSIZE);

    while(--anz>=0) {
      ppp1[anz].pointer=malloc(ppp2[anz].len+1);
      memcpy(ppp1[anz].pointer,ppp2[anz].pointer,ppp2[anz].len);
      (ppp1[anz].pointer)[ppp2[anz].len]=0;
    }
  } else if(b.typ==ARRAYTYP) {
    ARRAY *ppp1=(ARRAY *)(b.pointer+b.dimension*INTSIZE);
    ARRAY *ppp2=(ARRAY *)(a->pointer+b.dimension*INTSIZE);
    while(--anz>=0) ppp1[anz]=double_array(&(ppp2[anz]));
  } 
  return(b);
}


/*********** make a unity matrix  **********************/
ARRAY einheitsmatrix(const unsigned short typ, const uint32_t dimension, const uint32_t *dimlist) {
  ARRAY ergebnis=nullmatrix(typ,dimension,dimlist);
 // int anz=anz_eintraege(ergebnis);
  int i,a=0,j;
  int q=dimlist[0];
  for(j=0;j<dimension;j++) {if(q>dimlist[j]) q=dimlist[j];}
  
  switch(typ) {
  case INTTYP: {
    int *varptr=(int *)(ergebnis.pointer+dimension*INTSIZE);
    for(j=0;j<q;j++) {
      for(i=0;i<dimension;i++) a=j+a*dimlist[i]; 
      varptr[a]=1;
    }
    }
    break;
  case FLOATTYP: {
    double *varptr=(double *)(ergebnis.pointer+dimension*INTSIZE);
    /* Jetzt die Diagonalelemente */
    for(j=0;j<q;j++) {
      a=j;
      for(i=0;i<dimension-1;i++) a+=j*dimlist[i]; 
      varptr[a]=1.0;
    }
    }
    break;
  case COMPLEXTYP: {
    COMPLEX *varptr=(COMPLEX *)(ergebnis.pointer+dimension*INTSIZE);
    /* Jetzt die Diagonalelemente */
    for(j=0;j<q;j++) {
      a=j;
      for(i=0;i<dimension-1;i++) a+=j*dimlist[i]; 
      varptr[a].r=1.0;
      varptr[a].i=0;
    }
    }
    break;
  case ARBINTTYP: {
    ARBINT *varptr=(ARBINT *)(ergebnis.pointer+dimension*INTSIZE);
    /* Jetzt die Diagonalelemente */
    for(j=0;j<q;j++) {
      a=j;
      for(i=0;i<dimension-1;i++) a+=j*dimlist[i]; 
      mpz_set_si(varptr[a],1);
    }
    }
    break;
  default: xberror(96,""); /* ARRAY %s has the wrong type. */ 
  }
  return(ergebnis);
}

#if 0
/* Uebernimmt einen Speicherbereich in ein Array */

static ARRAY form_array(const unsigned short typ, const int dimension, const int *dimlist, const char *inhalt) {
  int j,anz=1,dlen;
  char *pp;
  ARRAY ergebnis;
  ergebnis.typ=typ;
  ergebnis.dimension=dimension;
  
  if(dimension) {
    for(j=0;j<dimension;j++) anz=anz*dimlist[j];
  }
  dlen=typlaenge(typ);
  
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*dlen);
  pp=(char *)(ergebnis.pointer+dimension*INTSIZE);
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(int));
  memcpy(pp,inhalt,dlen*anz);
  return(ergebnis);
}
#endif



/* Mache ein ARRAY aus Konstante [1,2,3;4,5,6]  */

ARRAY array_const(const char *s) {
  ARRAY ergebnis;
  char t[strlen(s)+1],t2[strlen(s)+1],s2[strlen(s)+1];
  int e,i=0,j=0,f,dx=0,dy,anz=1;
  if(*s=='[') strcpy(t,s+1);
  else strcpy(t,s);
  if(t[strlen(t)-1]==']') t[strlen(t)-1]=0;
  
  /* Typ Bestimmen */
  ergebnis.typ=type_list(t)&(~CONSTTYP)&(~ARRAYTYP);
  
    /* Ein oder Zweidimensional ? */
  ergebnis.dimension=e=wort_sep(t,';',TRUE,t,s2);
  while(e) {
    f=wort_sep(t,',',TRUE,t2,t);
    while(f) {
      j++;
      f=wort_sep(t,',',TRUE,t2,t);
    }
    dx=max(dx,j);
    j=0;
    i++;
    e=wort_sep(s2,';',TRUE,t,s2);
  }
  dy=i;
  anz=dx*dy;
  #if 0
  printf("ARRAY-Const:  %s\n",s);
  printf("============\n");
  printf("Dimension: %d\n",ergebnis.dimension);
  printf("Typ:       %d\n",ergebnis.typ);
  printf("Dim: %dx%d  \n",dy,dx);
  printf("Anz: %d  \n",anz);
  #endif
  
  ergebnis.pointer=malloc(ergebnis.dimension*INTSIZE+anz*typlaenge(ergebnis.typ));
  // printf("A-pointer: %p l=%d\n",ergebnis.pointer,ergebnis.dimension*INTSIZE+anz*typlaenge(ergebnis.typ));
  if((ergebnis.typ&TYPMASK)==STRINGTYP) {
    int i;
    STRING *varptr=(STRING *)(ergebnis.pointer+ergebnis.dimension*INTSIZE);
    for(i=0;i<anz;i++) {
      varptr[i].len=0;   /* Laenge */
      varptr[i].pointer=NULL;
    }
  }
  if(ergebnis.dimension==1) *((uint32_t *)(ergebnis.pointer))=dx;
  else if(ergebnis.dimension==2) {
    ((uint32_t *)(ergebnis.pointer))[0]=dy;
    ((uint32_t *)(ergebnis.pointer))[1]=dx;
  }
  i=j=0;
  e=wort_sep(s,';',TRUE,t,s2);
  while(e) {
    f=wort_sep(t,',',TRUE,t2,t);
    while(f) {
     // printf("t2=%s %g\n",t2,parser(t2));
      if((ergebnis.typ&TYPMASK)==INTTYP) ((int *)(ergebnis.pointer+ergebnis.dimension*INTSIZE))[j]=(int)parser(t2);
      else if((ergebnis.typ&TYPMASK)==FLOATTYP) ((double *)(ergebnis.pointer+ergebnis.dimension*INTSIZE))[j]=parser(t2);
      else if((ergebnis.typ&TYPMASK)==STRINGTYP){
        STRING *varptr=(STRING *)(ergebnis.pointer+ergebnis.dimension*INTSIZE);
	free(varptr[j].pointer);
        varptr[j]=string_parser(t2);
      } else {
        printf("ERROR: The expression <%s> is invalid.\n",s);
      }
      j++;
      f=wort_sep(t,',',TRUE,t2,t);
    }
    i++;
    j=i*dx;
    e=wort_sep(s2,';',TRUE,t,s2);
  }
  return(ergebnis);
}



/************** Array/Matrix multiplication ***************************/

ARRAY mul_array(ARRAY a1, ARRAY a2) {
  /* Teste ob die Typen fuer eine Multiplikation infrage kommen*/
  if(a1.dimension>2 || a2.dimension>2 || (a1.typ==STRINGTYP) || (a2.typ==STRINGTYP)) {
    xberror(83,""); /*Matrizenprodukt nicht definiert*/
    return(nullmatrix(a1.typ,a1.dimension,a1.pointer));
  } 
  /* Teste, ob die Dimensionierung fuer eine Multiplikation taugt: */
  int s1,z1,s2,z2;
  void *ar1,*ar2,*ar3;
  if(a1.dimension==0) s1=z1=1;
  else if(a1.dimension==1) {z1=1;s1=((int *)a1.pointer)[0];}
  else {z1=((int *)a1.pointer)[0];s1=((int *)a1.pointer)[1];}
  if(a2.dimension==0) s2=z2=1;
  else if(a2.dimension==1) {z2=1;s2=((int *)a2.pointer)[0];}
  else {z2=((int *)a2.pointer)[0];s2=((int *)a2.pointer)[1];}	
  if(s1!=z2) {
    xberror(81,""); /*Matrizen haben nicht die gleiche Ordnung*/
    return(nullmatrix(a1.typ,a1.dimension,a1.pointer));
  }
  ARRAY ergebnis;
  int s3,i,j,k;
  /*Jetzt typen anpassen:*/
  int rt=combine_type(a1.typ,a2.typ,'+');
  if(a1.typ!=rt) {
    ergebnis=convert_to_xarray(&a1,rt);
    free_array(&a1);
    a1=ergebnis;
  }
  if(a2.typ!=rt) {
    ergebnis=convert_to_xarray(&a2,rt);
    free_array(&a2);
    a2=ergebnis;
  }
  /*Pointer auf daten bestimmen*/
  ar1=a1.pointer+a1.dimension*INTSIZE;
  ar2=a2.pointer+a2.dimension*INTSIZE;
  /*Dimension des Ergebnisses bestimmen, pointer auf daten des ergebnisses*/
  s3=s1;
  if(s1<=1) ergebnis.dimension=0;
  else ergebnis.dimension=2;
  ergebnis.typ=rt;
  ergebnis.pointer=malloc(ergebnis.dimension*INTSIZE+s3*s3*typlaenge(rt));
  ar3=ergebnis.pointer+ergebnis.dimension*INTSIZE;
  for(i=0;i<ergebnis.dimension;i++) ((int *)ergebnis.pointer)[i]=s3;

    for(i=0;i<s3;i++) {
      for(j=0;j<s3;j++) {
    	switch(rt) {
    	  case INTTYP:  ((int *)   ar3)[i*s3+j]=0; break;
    	  case FLOATTYP:((double *)ar3)[i*s3+j]=0; break;
    	  case COMPLEXTYP:((COMPLEX *)ar3)[i*s3+j]=FLOAT2COMPLEX(0); break;
    	  case ARBINTTYP: mpz_init(((ARBINT *)ar3)[i*s3+j]); break;
    	}
    	for(k=0;k<min(s2,z1);k++) {
    	  switch(rt) {
    	    case INTTYP:    ((int *)	ar3)[i*s3+j]+=((int *)   ar1)[k*s1+j]*((int *)   ar2)[i*s2+k]; break;
    	    case FLOATTYP:  ((double *) ar3)[i*s3+j]+=((double *)ar1)[k*s1+j]*((double *)ar2)[i*s2+k]; break;
    	    case COMPLEXTYP:((COMPLEX *)ar3)[i*s3+j]=complex_add(((COMPLEX *)ar3)[i*s3+j],complex_mul(((COMPLEX *)ar1)[k*s1+j],((COMPLEX *)ar2)[i*s2+k])); break;
    	    case ARBINTTYP: mpz_addmul(((ARBINT *)ar3)[i*s3+j],((ARBINT *)ar1)[k*s1+j],((ARBINT *)ar2)[i*s2+k]); break;
    	  }
    	}
      }
    }
  return(ergebnis);
}

/* Transponiere ein 2-d-Array    */

ARRAY trans_array(ARRAY a) {
  ARRAY b=double_array(&a);
  if(a.dimension==1) {
    int anz=1,size,j;
    /* Erweitere die Dimension. */
    for(j=0;j<a.dimension;j++) anz=anz*((int *)a.pointer)[j];
    size=typlaenge(a.typ);
    b.dimension++;
    b.pointer=realloc(b.pointer,b.dimension*INTSIZE+anz*size);
    ((int *)b.pointer)[0]=((int *)a.pointer)[0];
    ((int *)b.pointer)[1]=1;
    /* mems may overlap */
    memmove(b.pointer+b.dimension*INTSIZE,a.pointer+a.dimension*INTSIZE,anz*size);
  } else if(a.dimension==2) {
    int size,i,j;
    int sw=((int *)b.pointer)[1];
    ((int *)b.pointer)[1]=((int *)b.pointer)[0];
    ((int *)b.pointer)[0]=sw;
    size=typlaenge(a.typ);
    for(i=0;i<((int *)b.pointer)[0];i++) {
      for(j=0;j<((int *)b.pointer)[1];j++) {
         memcpy(b.pointer+b.dimension*INTSIZE+(i*((int *)a.pointer)[0]+j)*size,
	        a.pointer+a.dimension*INTSIZE+(j*((int *)b.pointer)[0]+i)*size,size);
      }
    }
  } else xberror(85,"TRANS"); /* Transposition nur f�r zweidimensionale Matrizen.*/  
  return(b);
} 

double array_det(const ARRAY *a) {
  if(a->typ!=INTTYP && a->typ!=FLOATTYP) {  /*  TODO !!! */
    xberror(96,""); /* ARRAY %s has the wrong type. */
    return(0);
  }
  ARRAY b=*a;
  double det=0;
  if(a->typ==INTTYP) b=convert_to_floatarray(a);
  if(b.dimension==0) det=*((double *)b.pointer);
  else if(b.dimension==1) det=*((double *)(b.pointer+INTSIZE));
  else if(b.dimension>2) xberror(89,""); /* Das Array muss zweidimensional sein*/
  else if(((int *)b.pointer)[0]!=((int *)b.pointer)[1]) xberror(86,""); /* Matrix nicht quadratisch */
  else det=determinante(b.pointer+b.dimension*INTSIZE,((int *)b.pointer)[0]);
  if(a->typ==INTTYP) free_array(&b);
  return(det);
}


ARRAY inv_array(ARRAY a) {
  ARRAY ergeb;
  if(a.typ==INTTYP) ergeb=convert_to_floatarray(&a);
  else ergeb=double_array(&a);
  if(a.typ!=INTTYP && a.typ!=FLOATTYP) {
    xberror(96,""); /* ARRAY %s has the wrong type. */
    return(ergeb);
  }
  
  if(a.dimension==0) {
    double d=*((double *)ergeb.pointer);
    if(d==0) xberror(0,""); /* Division durch Null */
    else d=1/d;
    *((double *)ergeb.pointer)=d;
    return(ergeb);
  } else if(a.dimension!=2) {
    xberror(89,""); /* Array must be two dimensional */
    return(ergeb);
  } else {
    if(((int *)a.pointer)[0]!=((int *)a.pointer)[1]) {
      xberror(86,""); /* Matrix nicht quadratisch */
      return(ergeb);
    } else {
      int anzzeilen=((int *)a.pointer)[0];
      double *pp=(double *)(ergeb.pointer+ergeb.dimension*INTSIZE);
      inverse(pp,anzzeilen);
    
    #if 0
      /*Interface zu den numerical recipes Funktionen (c) Markus Hoffmann 
  ist obsolet, da NUMREC f"ur INVERSE nicht mehr verwendet wird. */ 
      double maxsing=0;
      int i,j,elim=0;
      int fsing=0;
      int anzzeilen=((int *)a.pointer)[0];
      int anzspalten=((int *)a.pointer)[1];
      double *pp=(double *)(a.pointer+a.dimension*INTSIZE);
      double *u = malloc(sizeof(double)*anzzeilen*anzspalten);
      double *v = malloc(sizeof(double)*anzspalten*anzspalten);
      double *ergebnis = malloc(sizeof(double)*anzspalten*anzspalten);
      double *singulars = malloc(sizeof(double)*anzspalten);
      double *col;
      double *univ = malloc(sizeof(double)*anzspalten);
      memcpy(u,pp,sizeof(double)*anzzeilen*anzspalten);
      singulars=SVD(u,singulars,v,anzzeilen,anzspalten);
      /* Groessten Singulaerwert rausfinden */
      for(i=0;i<anzspalten;i++) {
        if(fabs(singulars[i])>maxsing) maxsing=fabs(singulars[i]);
      }
     /* Zaehle Anzahl der Singulaeren Werte (d.h. Eigenwerte=0) */
     /* Akzeptiere nur Eigenwerte die mindestens 1e-10 vom groessten sind,
        ansonsten setze sie zu 0 */

      for(i=0;i<anzspalten;i++) {
        if(singulars[i]==0) fsing++;
        if(fabs(singulars[i])/maxsing<1e-10 && singulars[i]) {
          printf("** %g\n",singulars[i]);
          singulars[i]=0;
          elim++;
        }
      }
      if(fsing || elim) printf("Found %d singularities and eliminated another %d.\n",fsing,elim);

      /* Jetzt die inverse Matrix ausrechnen   */
      for(i=0;i<anzspalten;i++) {
        for(j=0;j<anzspalten;j++) univ[j]=0;
        univ[i]=1;
        col=backsub(singulars,u,v,univ,anzzeilen,anzspalten);
	for(j=0;j<anzspalten;j++) ergebnis[i*anzspalten+j]=col[j];
        free(col);
      }
      free(u);free(v);
      free(singulars);
      free(univ);
      ergeb=form_array(a.typ,2,a.pointer,(char *)ergebnis);
      free(ergebnis);
      #endif
      return(ergeb);
    }
  }
}
void array_smul(ARRAY a1, double m) {
  int anz=anz_eintraege(&a1),j;
  switch (a1.typ) {
  case FLOATTYP: {
    double *pp1=(double *)(a1.pointer+a1.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]=m*pp1[j];
    } break;
  case INTTYP: {
    int *pp1=(int *)(a1.pointer+a1.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]=m*pp1[j];
    }break;
  case COMPLEXTYP: {
    COMPLEX *pp1=(COMPLEX *)(a1.pointer+a1.dimension*INTSIZE); 
    for(j=0;j<anz;j++) {pp1[j].r*=m;pp1[j].i*=m;}
    }break;
  case ARBINTTYP: {
    ARBINT *pp1=(ARBINT *)(a1.pointer+a1.dimension*INTSIZE); 
    for(j=0;j<anz;j++) mpz_mul_si(pp1[j],pp1[j],(int)m);
    } break;
  default: xberror(96,""); /* ARRAY %s has the wrong type. */
  }
}
void array_add(ARRAY a1, ARRAY a2) {
  int anz=min(anz_eintraege(&a1),anz_eintraege(&a2)),j;
  int rt=combine_type(a1.typ,a2.typ,'+');
  if(a1.typ!=rt) {
    xberror(96,""); /* ARRAY %s has the wrong type. */ 
    return;
  }
  if(a2.typ!=rt) {
    ARRAY tmp=convert_to_xarray(&a2,rt);
    free_array(&a2);
    a2=tmp;
  }
  switch(rt) {
  case INTTYP: {
    int *pp1=(int *)(a1.pointer+a1.dimension*INTSIZE); 
    int *pp2=(int *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]+=pp2[j];
    } break;
  case FLOATTYP: {
    double *pp1=(double *)(a1.pointer+a1.dimension*INTSIZE); 
    double *pp2=(double *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]+=pp2[j];
    } break;
  case COMPLEXTYP: {
    COMPLEX *pp1=(COMPLEX *)(a1.pointer+a1.dimension*INTSIZE); 
    COMPLEX *pp2=(COMPLEX *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]=complex_add(pp1[j],pp2[j]);
    } break;
  case ARBINTTYP: {
    ARBINT *pp1=(ARBINT *)(a1.pointer+a1.dimension*INTSIZE); 
    ARBINT *pp2=(ARBINT *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) mpz_add(pp1[j],pp1[j],pp2[j]);
    } break;
  case STRINGTYP: {
    STRING *pp1=(STRING *)(a1.pointer+a1.dimension*INTSIZE); 
    STRING *pp2=(STRING *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) {
      pp1[j].pointer=realloc(pp1[j].pointer,pp2[j].len+pp1[j].len);
      memcpy(pp1[j].pointer+pp1[j].len,pp2[j].pointer,pp2[j].len);
      pp1[j].len+=pp2[j].len; 
    }	
    } break;
    default:  xberror(96,""); /* ARRAY %s has the wrong type. */ 
  }
}

void array_sub(ARRAY a1, ARRAY a2) {
  int anz=min(anz_eintraege(&a1),anz_eintraege(&a2)),j;
  int rt=combine_type(a1.typ,a2.typ,'+');
  if(a1.typ!=rt) {
    xberror(96,""); /* ARRAY %s has the wrong type. */ 
    return;
  }
  if(a2.typ!=rt) {
    ARRAY tmp=convert_to_xarray(&a2,rt);
    free_array(&a2);
    a2=tmp;
  }
  switch(rt) {
  case INTTYP: {
    int *pp1=(int *)(a1.pointer+a1.dimension*INTSIZE); 
    int *pp2=(int *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]-=pp2[j];
    } break;
  case FLOATTYP: {
    double *pp1=(double *)(a1.pointer+a1.dimension*INTSIZE); 
    double *pp2=(double *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]-=pp2[j];
    } break;
  case COMPLEXTYP: {
    COMPLEX *pp1=(COMPLEX *)(a1.pointer+a1.dimension*INTSIZE); 
    COMPLEX *pp2=(COMPLEX *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]=complex_sub(pp1[j],pp2[j]);
    } break;
  case ARBINTTYP: {
    ARBINT *pp1=(ARBINT *)(a1.pointer+a1.dimension*INTSIZE); 
    ARBINT *pp2=(ARBINT *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) mpz_sub(pp1[j],pp1[j],pp2[j]);
    } break;
    default:  xberror(96,""); /* ARRAY %s has the wrong type. */ 
  }
}




/* Wandelt ein ganzes Array in einen String um, so dass es auch wieder
   rueckverwandelt werden kann. Nuetzlich zum Speichern und versenden  
   
   */


STRING array_to_string(ARRAY inhalt) {
  STRING ergebnis;
  int i;
  char *ppp;
  int arraylen=anz_eintraege(&inhalt);
  
  /* Stringlaenge berechnen */
  int len=sizeof(unsigned short)+sizeof(uint32_t)+sizeof(uint32_t)*inhalt.dimension;
 
  switch(inhalt.typ) {
  case STRINGTYP: {
    /* Alle Stringlaengen aufsummieren */
    STRING *a=(STRING *)(inhalt.pointer+INTSIZE*inhalt.dimension);
    for(i=0;i<arraylen;i++) len+=a[i].len;
  }
  case INTTYP:
  case FLOATTYP:
  case COMPLEXTYP:
    len+=arraylen*typlaenge(inhalt.typ);
    break;
  case ARBINTTYP: {
    /*Alle Zahlen zu Stringsumwandeln*/
    len+=arraylen*sizeof(STRING);
    ARBINT *a=(ARBINT *)(inhalt.pointer+INTSIZE*inhalt.dimension);
    char *buf;
    for(i=0;i<arraylen;i++) {
      buf=mpz_get_str(NULL,32,a[i]);
      len+=strlen(buf);
      free(buf);
    }
    }
    break;
  case ARRAYTYP: {
    /*Alle Arrays zu Stringsumwandeln*/
    len+=arraylen*sizeof(STRING);
    ARRAY *a=(ARRAY *)(inhalt.pointer+INTSIZE*inhalt.dimension);
    STRING s;
    for(i=0;i<arraylen;i++) {
      s=array_to_string(a[i]);
      len+=s.len;
      free(s.pointer);
    }
    }
    break;
  default:
    printf("ERROR: Array type not supported here: cannot convert to string.\n");
  }
  ergebnis.pointer=malloc(len);
  ergebnis.len=len;
  
  ppp=(char *)ergebnis.pointer;
  
  *((unsigned short *)ppp)=inhalt.typ;
  ppp+=sizeof(unsigned short);
  
  *((uint32_t *)ppp)=inhalt.dimension;
  ppp+=sizeof(uint32_t);
  
  memcpy(ppp,inhalt.pointer,inhalt.dimension*sizeof(uint32_t));
  ppp+=inhalt.dimension*sizeof(uint32_t);
  
  /*Jetzt Arrayinhalt reinschreiben*/
  switch(inhalt.typ) {
    case INTTYP:
    case FLOATTYP:
    case COMPLEXTYP:
      memcpy(ppp,inhalt.pointer+INTSIZE*inhalt.dimension,arraylen*typlaenge(inhalt.typ));
    break;
    case STRINGTYP: {
      /* Alle Stringpointer relozieren */
      STRING *s=(STRING *)(inhalt.pointer+INTSIZE*inhalt.dimension);
      STRING *a=(STRING *)(ppp);      /*Verzeichnis*/
      char *ppp2,*found;
      ppp+=arraylen*sizeof(STRING);   /*Datenbereich*/
      ppp2=ppp;
      for(i=0;i<arraylen;i++) {
        a[i].len=s[i].len;
        /* Schau, ob der eintrag schon vorhanden ist: */
        found=(char *)memmem(ppp2,(int)(ppp-ppp2),s[i].pointer,s[i].len);
        if(found!=NULL) {
          a[i].pointer=(char *)(found-ppp2);
        } else {
          memcpy(ppp,s[i].pointer,s[i].len);
          a[i].pointer=(char *)(ppp-ppp2);
          ppp+=s[i].len;
        }
      }
      ergebnis.len=ppp-ergebnis.pointer; /* String ggf. kuerzen */
    }
    break;
    case ARBINTTYP: {
      ARBINT *s=(ARBINT *)(inhalt.pointer+INTSIZE*inhalt.dimension);
      STRING *a=(STRING *)(ppp);      /*Verzeichnis*/
      char *ppp2,*found;
      ppp+=arraylen*sizeof(STRING);   /*Datenbereich*/
      ppp2=ppp;
      char *buf;
      for(i=0;i<arraylen;i++) {
        buf=mpz_get_str(NULL,32,s[i]);
        a[i].len=strlen(buf);
        /* Schau, ob der eintrag schon vorhanden ist: */
        found=(char *)memmem(ppp2,(int)(ppp-ppp2),buf,a[i].len);
        if(found!=NULL) {
          a[i].pointer=(char *)(found-ppp2);
        } else {
          memcpy(ppp,buf,a[i].len);
          a[i].pointer=(char *)(ppp-ppp2);
          ppp+=a[i].len;
        }
	free(buf);
      }
      ergebnis.len=ppp-ergebnis.pointer; /* String ggf. kuerzen */
    }
    break;
    case ARRAYTYP: {
      ARRAY *s=(ARRAY *)(inhalt.pointer+INTSIZE*inhalt.dimension);
      STRING *a=(STRING *)(ppp);      /*Verzeichnis*/
      char *ppp2,*found;
      ppp+=arraylen*sizeof(STRING);   /*Datenbereich*/
      ppp2=ppp;
      STRING buf;
      for(i=0;i<arraylen;i++) {
        buf=array_to_string(s[i]);
        a[i].len=buf.len;
        /* Schau, ob der eintrag schon vorhanden ist: */
        found=(char *)memmem(ppp2,(int)(ppp-ppp2),buf.pointer,a[i].len);
        if(found!=NULL) {
          a[i].pointer=(char *)(found-ppp2);
        } else {
          memcpy(ppp,buf.pointer,a[i].len);
          a[i].pointer=(char *)(ppp-ppp2);
          ppp+=a[i].len;
        }
	free(buf.pointer);
      }
    }
    break;
  }
  return(ergebnis);
}

ARRAY string_to_array(STRING in) {
  ARRAY out;
  int i,len,arraylen;
  
 // printf("String to array:\n");
 // memdump(in.pointer,in.len);

  out.pointer=in.pointer;
  out.typ=*((unsigned short *)in.pointer);
  out.pointer+=sizeof(unsigned short);

  out.dimension=*((uint32_t *)out.pointer);
  out.pointer+=sizeof(uint32_t);
  
  len=INTSIZE*out.dimension;
  arraylen=anz_eintraege(&out);
  len+=arraylen*typlaenge(out.typ);
 
 // printf("Array typ=%x dimension=%d\n",out.typ,out.dimension);
 // printf("arraylen=%d\n",arraylen);
  
  out.pointer=malloc(len);
  memcpy(out.pointer,in.pointer+sizeof(unsigned short)+sizeof(uint32_t),sizeof(uint32_t)*out.dimension);
  char *ppp=in.pointer+sizeof(unsigned short)+sizeof(uint32_t)+sizeof(uint32_t)*out.dimension;
  
  switch(out.typ) {
  case INTTYP:
  case FLOATTYP:
  case COMPLEXTYP:
    memcpy(out.pointer+INTSIZE*out.dimension,ppp,arraylen*typlaenge(out.typ));
    break;
  case STRINGTYP: {
    char *data=ppp+arraylen*sizeof(STRING); /*Datenbereich*/
/*Datenbereich*/
    STRING *s=(STRING *)(ppp);  
    STRING *a=(STRING *)(out.pointer+INTSIZE*out.dimension); 
    /* Alle Strings anlegen und pointer relozieren */
    unsigned long offset;
    for(i=0;i<arraylen;i++) {
      offset=(unsigned long)s[i].pointer;
      a[i].len=s[i].len;
      a[i].pointer=malloc(s[i].len+1);
      memcpy(a[i].pointer,data+offset,s[i].len);
      a[i].pointer[a[i].len]=0;
    }
    }
    break;
  case ARBINTTYP: {
    char *data=ppp+arraylen*sizeof(STRING); /*Datenbereich*/
    STRING *s=(STRING *)(ppp);  
    ARBINT *a=(ARBINT *)(out.pointer+INTSIZE*out.dimension); 
    unsigned long offset;
    char *buf;
    for(i=0;i<arraylen;i++) {
      offset=(unsigned long)s[i].pointer;
      buf=malloc(s[i].len+1);
      memcpy(buf,data+offset,s[i].len);
      buf[s[i].len]=0;
      mpz_init(a[i]);
      mpz_set_str(a[i],buf,32);
      free(buf);
    }
    }
    break;
  case ARRAYTYP: {
    char *data=ppp+arraylen*sizeof(STRING); /*Datenbereich*/
    STRING *s=(STRING *)(ppp); 
    ARRAY *a=(ARRAY *)(out.pointer+INTSIZE*out.dimension);
    unsigned long offset;
    STRING buf;
    for(i=0;i<arraylen;i++) {
      offset=(unsigned long)s[i].pointer;
      buf.pointer=malloc(s[i].len+1);
      memcpy(buf.pointer,data+offset,s[i].len);
      buf.pointer[s[i].len]=0;
      buf.len=s[i].len;
      a[i]=string_to_array(buf);
      free(buf.pointer);
    }
    }
    break;
  default:
    printf("ERROR: String does not code a valid array.\n");
  }
  return(out);
}


/********************* Hilfsfunktionen ********************************/

/* Get subarry (ARRAY *a,indexliste) zurueck: ARRAY b. */

ARRAY get_subarray(ARRAY *arr,int *indexliste) {
  ARRAY ergebnis=*arr;
  int *aindex=(int *)arr->pointer;
  int bindex[arr->dimension];
  int adim,dim2=0,i,j,jj,k,anz=1,anz2,firsti=-1;

  adim=arr->dimension;
  if(adim) {
    for(i=0;i<adim;i++) {
      if(indexliste[i]==-1) {
        if(firsti==-1) firsti=i;
        bindex[dim2++]=aindex[i];
        anz=anz*aindex[i];
      }
//	printf("aindex[%d]=%d bindex[%d]=%d\n",i,aindex[i],i,bindex[i]);
    }
  }

/*Aindex sind die Dims fuer jede Dimension
  Bindex ist die Liste der dims fuer die Dimensionen, welche ubertragen werden*/

//printf("get_subarray: adim=%d dim2=%d anz=%d\n",adim,dim2,anz);
  

  ergebnis.dimension=dim2;
  ergebnis.pointer=malloc(ergebnis.dimension*INTSIZE+anz*typlaenge(ergebnis.typ));
   /* dimlist kopieren */
  memcpy(ergebnis.pointer,bindex,dim2*sizeof(int));
 
 
 /*bindex wird jetzt überschrieben*/
 
  /*Loop fuer die Komprimierung */
  for(j=0;j<anz;j++) {
    jj=j;
 //   printf("%d ---> ",j);
    /* Indexliste aus anz machen von hinten */
    for(k=arr->dimension-1;k>=0;k--) {
      if(indexliste[k]==-1) {
        if(k!=firsti) {
	  bindex[k]=jj/aindex[k];
	  jj=jj % aindex[k];
	} else {
	  bindex[k]=jj;
	  jj=0;
	}
      } else bindex[k]=indexliste[k];
    }
    
    /*bindex hat nun orginaldimensionierung fuer dimensionen mit festem index
      sowie die aus j erzeugte laufende indizies*/
   
  //  for(k=0;k<arr->dimension;k++) printf("bindex[%d]=%d ",k,bindex[k]);
  //  printf("\n");
    
    if(jj!=0) {/* Testen ob passt  */
      printf("ERROR: Something is wrong. jj=%d\n",jj); 
    }
    anz2=0;
    for(k=0;k<arr->dimension;k++)  anz2=anz2*aindex[k]+bindex[k];

 /* jetzt kopieren */

    switch(ergebnis.typ) {
    case INTTYP: 
      ((int *)(ergebnis.pointer+INTSIZE*ergebnis.dimension))[j]=((int *)(arr->pointer+INTSIZE*arr->dimension))[anz2];   
      break;
    case FLOATTYP: 
      ((double *)(ergebnis.pointer+INTSIZE*ergebnis.dimension))[j]=((double *)(arr->pointer+INTSIZE*arr->dimension))[anz2];   
      break;
    case COMPLEXTYP:
      ((COMPLEX *)(ergebnis.pointer+INTSIZE*ergebnis.dimension))[j]=((COMPLEX *)(arr->pointer+INTSIZE*arr->dimension))[anz2];   
      break;
    case ARBINTTYP:
      mpz_init(((ARBINT *)(ergebnis.pointer+INTSIZE*ergebnis.dimension))[j]);
      mpz_set(((ARBINT *)(ergebnis.pointer+INTSIZE*ergebnis.dimension))[j],
              ((ARBINT *)(arr->pointer+INTSIZE*arr->dimension))[anz2]);
      break;
    case STRINGTYP:
      ((STRING *)(ergebnis.pointer+INTSIZE*ergebnis.dimension))[j]=double_string(&((STRING *)(arr->pointer+INTSIZE*arr->dimension))[anz2]);
      break;
    case ARRAYTYP:
      ((ARRAY *)(ergebnis.pointer+INTSIZE*ergebnis.dimension))[j]=double_array(&((ARRAY *)(arr->pointer+INTSIZE*arr->dimension))[anz2]);
      break;
    }
  }
  return(ergebnis);
}

int make_indexliste(int dim, char *pos, int *index) {
  char w1[strlen(pos)+1],w2[strlen(pos)+1];
  int i=0,e,flag=0;
// printf("Make indexliste: dim=%d <%s>\n",dim,pos);
  e=wort_sep(pos,',',TRUE,w1,w2);
  for(i=0;i<dim;i++) {
    if(e==0) {
      xberror(18,"");  /* Falsche Anzahl Indizies */
      flag=1;
      break;
    }
    if(*w1==':' || *w1==0) {index[i]=-1;flag=1;}	
    else index[i]=(int)parser(w1);
    e=wort_sep(w2,',',TRUE,w1,w2);
  }
  return(flag);
}

/*Überprüft Indexliste auf Fehler*/
int check_indexliste(const ARRAY *a, const int *idx) {
  int dim=a->dimension;
  int *bbb=(int *)a->pointer;
  while(--dim>=0) {
    if(idx[dim]<0 || idx[dim]>=bbb[dim]) return(0);
  }
  return 1;
}

/* Macht aus einer laufenden Nummer einen Index-Vektor*/

void anz2idx(int anz,const ARRAY *a,int *idx) {
  int i=a->dimension;
  int *bbb=(int *)a->pointer;
  while(--i>=0) {
    idx[i]=(anz % bbb[i]);
    anz=(anz/bbb[i]);
  }
}
#if 0
int idx2idx(int jj,const ARRAY *a,int *idx, int *idx2) {
  int k=a->dimension;
  int firsti=0;
  int *bbb=(int *)a->pointer;
/*  TODO */ 
   /* Indexliste aus anz machen von hinten */
    while(--k>=0) {
      if(idx[k]==-1) {
        if(k!=firsti) {
	  idx2[k]=jj/bbb[k];
	  jj=jj % bbb[k];
	} else {
	  idx2[k]=jj;
	  jj=0;
	}
      } else idx2[k]=idx[k];
    }

  return(0);
}
#endif


/* Liest ein Arrayelement anhand Indizies. */

int int_array_element(const ARRAY *a, int *idx) {
  int ndim=0,anz=0;
  int *varptr=(int *)(a->pointer+a->dimension*INTSIZE);
  while(ndim<a->dimension) {
    anz=idx[ndim]+anz*((int *)a->pointer)[ndim]; 
    ndim++;
  }
  return(varptr[anz]);
}
void arbint_array_element(const ARRAY *a, int *idx, ARBINT ret) {
  int ndim=0,anz=0;
  ARBINT *varptr=(ARBINT *)(a->pointer+a->dimension*INTSIZE);
  while(ndim<a->dimension) {
    anz=idx[ndim]+anz*((int *)a->pointer)[ndim]; 
    ndim++;
  }
  mpz_set(ret,varptr[anz]);
}

double float_array_element(const ARRAY *a, int *idx) {
  int ndim=0,anz=0;
  double *varptr=(double *)(a->pointer+a->dimension*INTSIZE);
  while(ndim<a->dimension) {
    anz=idx[ndim]+anz*((int *)a->pointer)[ndim];
    ndim++;
  }
  return(varptr[anz]);
}
COMPLEX complex_array_element(const ARRAY *a, int *idx) {
  int ndim=0,anz=0;
  COMPLEX *varptr=(COMPLEX *)(a->pointer+a->dimension*INTSIZE);
  while(ndim<a->dimension) {
    anz=idx[ndim]+anz*((int *)a->pointer)[ndim];
    ndim++;
  }
  return(varptr[anz]);
}
STRING string_array_element(const ARRAY *a, int *idx) {
  int ndim=0,anz=0;
  STRING *varptr=(STRING *)(a->pointer+a->dimension*INTSIZE);
  while(ndim<a->dimension) {
    anz=idx[ndim]+anz*((int *)a->pointer)[ndim];
    ndim++;
  }
  return(double_string(&varptr[anz]));
}
ARRAY array_array_element(const ARRAY *a, int *idx) {
  int ndim=0,anz=0;
  ARRAY *varptr=(ARRAY *)(a->pointer+a->dimension*INTSIZE);
  while(ndim<a->dimension) {
    anz=idx[ndim]+anz*((int *)a->pointer)[ndim];
    ndim++;
  }
  return(double_array(&varptr[anz]));
}

/*********** Subarray functions ****************/

/*bestimmt die dimension eines Subarrays anhand der indexliste*/
int subarraydimf(int *indexliste, int n) {
  int dim=0;
  if(indexliste) {
    while(--n>=0) { if(indexliste[n]<0) dim++; }
  }
  return dim;
}


