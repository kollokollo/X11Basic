/* afunctions.c   Array functions (c) Markus Hoffmann*/

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
#include "variablen.h"
#include "xbasic.h"
#include "type.h"
#include "parser.h"
#include "parameter.h"
#include "array.h"
#include "afunctions.h"
#include "mathematics.h"
#include "functions.h"
#include "number.h"

#ifdef DUMMY_LIST
#define f_nop NULL
#define f_nullmat  NULL
#define f_einsmat  NULL
#define f_convolut  NULL
#define f_csvgeta   NULL
#define f_csvgeta   NULL
#define string_to_array  NULL
#define f_doocsgeta   NULL
#define f_doocsnames  NULL
#define f_doocsgeta   NULL
#define f_fft    NULL
#define inv_array   NULL
#define f_smula   NULL
#define f_solvea   NULL
#define f_tinegeta  NULL
#define f_tinegeta  NULL
#define f_tinehistorya   NULL
#define trans_array   NULL

#else
static ARRAY f_smula(PARAMETER *plist, int e) {
  int rt=combine_type(((ARRAY *)&(plist->integer))->typ,plist[1].typ&PL_BASEMASK,'*');
  ARRAY ergeb=convert_to_xarray((ARRAY *)&(plist->integer),rt);
  int anz=anz_eintraege(&ergeb),j;
  switch(rt) {
   case FLOATTYP: {
    double *pp1=(double *)(ergeb.pointer+ergeb.dimension*INTSIZE); 
    double m=p2float(&plist[1]);
    for(j=0;j<anz;j++) pp1[j]*=m;
    } break;
  case INTTYP: {
    int *pp1=(int *)(ergeb.pointer+ergeb.dimension*INTSIZE); 
    int m=p2int(&plist[1]);
    for(j=0;j<anz;j++) pp1[j]*=m;
    } break;
  case COMPLEXTYP: {
    COMPLEX *pp1=(COMPLEX *)(ergeb.pointer+ergeb.dimension*INTSIZE);
    COMPLEX m=p2complex(&plist[1]); 
    for(j=0;j<anz;j++) {pp1[j]=complex_mul(m,pp1[j]);}
    } break;
  case ARBINTTYP: {
    ARBINT *pp1=(ARBINT *)(ergeb.pointer+ergeb.dimension*INTSIZE); 
    ARBINT m;
    mpz_init(m);
    p2arbint(&plist[1],m);
    for(j=0;j<anz;j++) mpz_mul(pp1[j],pp1[j],m); 
    mpz_clear(m);
    } 
    break;
  default: xberror(96,""); /* ARRAY %s has the wrong type. */
  }
  return(ergeb);
}
static ARRAY f_nullmat(PARAMETER *plist, int e) {
    int dimlist[2]={plist[0].integer,plist[1].integer};
    return(nullmatrix(FLOATTYP,e,dimlist));
}
static ARRAY f_einsmat(PARAMETER *plist, int e) {
    int dimlist[2]={plist[0].integer,plist[0].integer};
    return(einheitsmatrix(FLOATTYP,2,dimlist));
}

/*Determinante berechnen*/

PARAMETER f_det(PARAMETER *plist, int e) {
  PARAMETER ret;
  ret.typ=PL_FLOAT;  /*   TODO !!!*/
  ret.real=array_det((ARRAY *)&(plist->integer));   
  return(ret);
}


/* Gleichungssystem loesen  d=Mx    x()=SOLVE(m(),d())*/
static ARRAY f_solvea(PARAMETER *plist, int e) {
  ARRAY ergeb,*arr1,*arr2;
  int anzzeilen,anzspalten;
  arr1=(ARRAY *)&(plist->integer);
  arr2=(ARRAY *)&(plist[1].integer);
 
  ergeb.typ=combine_type(arr1->typ,arr2->typ,'+');
  ergeb.dimension=1;
  if(plist->integer>2)  xberror(80,""); /* Matrizenoperationen nur für ein- oder zweidimensionale Felder*/
  else if(plist->integer!=2) xberror(81,""); /* "Matrizen haben nicht die gleiche Ordnung" */
  if(plist[1].integer!=1) xberror(81,""); /* "Matrizen haben nicht die gleiche Ordnung" */
  anzspalten=*((int *)(plist->pointer+sizeof(int)));
  anzzeilen=*((int *)(plist->pointer));

  if(anzzeilen!=*((int *)(plist[1].pointer))) xberror(81,""); /* "Matrizen haben nicht die gleiche Ordnung" */


  ARRAY a=convert_to_xarray(arr1,ergeb.typ);
  ARRAY b=convert_to_xarray(arr2,ergeb.typ);

  ergeb.pointer=malloc(INTSIZE+anzspalten*typlaenge(ergeb.typ));
  *((int *)ergeb.pointer)=anzspalten;

  if(ergeb.typ!=FLOATTYP) printf("ERROR: Operation for complex arrays not yet implemented.\n");
  else solve((double *)(b.pointer+b.dimension*INTSIZE),(double *)(a.pointer+a.dimension*INTSIZE),anzzeilen,anzspalten, (double *)(ergeb.pointer+INTSIZE));
  free_array(&a);
  free_array(&b);
  return(ergeb);
}
/* Diskrete Faltung x()=CONVOLUT(A(),h())*/
static ARRAY f_convolut(PARAMETER *plist, int e) {
  ARRAY *arr1=(ARRAY *)&(plist->integer);
  ARRAY ergeb;
  int n=anz_eintraege(arr1); 
  ARRAY *arr2=(ARRAY *)&(plist[1].integer);
  int n2=anz_eintraege(arr2);
  
  ergeb.typ=combine_type(arr1->typ,arr2->typ,'+');
  ergeb.dimension=arr1->dimension;
  ergeb.pointer=malloc(INTSIZE*ergeb.dimension+n*typlaenge(ergeb.typ));
  memcpy(ergeb.pointer,arr1->pointer,INTSIZE*ergeb.dimension);

  ARRAY a=convert_to_xarray(arr1,ergeb.typ);
  ARRAY h=convert_to_xarray(arr2,ergeb.typ);
  int i,j;
  int o=n2/2;
  switch(ergeb.typ) {
  case INTTYP: {
    int *varptr=(int  *)(ergeb.pointer+ergeb.dimension*INTSIZE);
    int *varptr1=(int  *)(a.pointer+a.dimension*INTSIZE);
    int *varptr2=(int  *)(h.pointer+h.dimension*INTSIZE);
    int sum;
    for(i=0;i<n;i++) {
       sum=0;
       for(j=0;j<n2;j++) {
         if(i+j-o>=0 && i+j-o<n) sum+=varptr1[i+j-o]*varptr2[j];
       }  
       varptr[i]=sum;
    }
  } break;
  case FLOATTYP: {
    double *varptr=(double  *)(ergeb.pointer+ergeb.dimension*INTSIZE);
    double *varptr1=(double  *)(a.pointer+a.dimension*INTSIZE);
    double *varptr2=(double  *)(h.pointer+h.dimension*INTSIZE);
    double sum;
    for(i=0;i<n;i++) {
       sum=0;
       for(j=0;j<n2;j++) {
         if(i+j-o>=0 && i+j-o<n) sum+=varptr1[i+j-o]*varptr2[j];
       }  
       varptr[i]=sum;
    }
  } break;
  case COMPLEXTYP: {
    COMPLEX *varptr=(COMPLEX  *)(ergeb.pointer+ergeb.dimension*INTSIZE);
    COMPLEX *varptr1=(COMPLEX  *)(a.pointer+a.dimension*INTSIZE);
    COMPLEX *varptr2=(COMPLEX  *)(h.pointer+h.dimension*INTSIZE);
    COMPLEX sum;
    for(i=0;i<n;i++) {
       sum.r=sum.i=0;
       for(j=0;j<n2;j++) {
         if(i+j-o>=0 && i+j-o<n) sum=complex_add(sum,complex_mul(varptr1[i+j-o],varptr2[j]));
       }  
       varptr[i]=sum;
    }
  } break;
  case ARBINTTYP: {
    ARBINT *varptr=(ARBINT  *)(ergeb.pointer+ergeb.dimension*INTSIZE);
    ARBINT *varptr1=(ARBINT  *)(a.pointer+a.dimension*INTSIZE);
    ARBINT *varptr2=(ARBINT  *)(h.pointer+h.dimension*INTSIZE);
    for(i=0;i<n;i++) {
       mpz_set_si(varptr[i],0);
       for(j=0;j<n2;j++) {
         if(i+j-o>=0 && i+j-o<n) mpz_addmul(varptr[i],varptr1[i+j-o],varptr2[j]);
       }  
    }
  } break;
  default:
    printf("ERROR: Operation for this array typ (%x) not implemented.\n",ergeb.typ);
  }
  free_array(&a);
  free_array(&h);
  return(ergeb);
}

/* Fast Fourier Transform  */
static ARRAY f_fft(PARAMETER *plist, int e) {
  int isign=0;
  ARRAY ergeb=double_array((ARRAY *)&(plist->integer));
  int n=anz_eintraege(&ergeb);
  if(ergeb.typ!=FLOATTYP) {
    printf("ERROR: Operation for complex arrays not yet implemented.\n");
  } else {
    double *varptr=(double  *)(ergeb.pointer+ergeb.dimension*INTSIZE);
    if(e>1) isign=plist[1].integer;
    realft(varptr,n,isign);
  }
  return(ergeb);
}



#ifdef CONTROL
static ARRAY f_csvgeta(char *pos) {
  int o=0,nn=0;
  if(e>1) nn=plist[1].integer;
  if(e>2) o=plist[2].integer;
  return(csvget(plist[0].pointer,nn,o));
}
#endif
#ifdef TINE
static ARRAY f_tinegeta(PARAMETER *plist, int e) {
  int o=0,nn=0;
  if(e>1) nn=plist[1].integer;
  if(e>2) o=plist[2].integer;
  return(tinevget(plist[0].pointer,nn,o));
}
static ARRAY f_tinehistorya(PARAMETER *plist, int e) {
	  return(tinehistory(plist[0].pointer,plist[1].integer,plist[2].integer));
}
#endif
#ifdef DOOCS
ARRAY doocsnames(char *n);
static ARRAY f_doocsgeta(PARAMETER *plist, int e) {
  int o=0,nn=0;
  if(e>1) nn=plist[1].integer;
  if(e>2) o=plist[2].integer;
  return(doocsvget(plist[0].pointer,nn,o));
}
static ARRAY f_doocsnames(PARAMETER *plist, int e) {
  return(doocsnames(plist[0].pointer));
}
#endif
#endif

/*  Array funktionen */

const AFUNCTION pafuncs[]= {  /* alphabetisch !!! */
 { F_CONST|F_ARGUMENT,  "!nulldummy",  (afunc) f_nop ,0,0   ,{0}},
 { F_CONST|F_PLISTE,    "0"         , f_nullmat ,2,2   ,{PL_INT,PL_INT}},
 { F_CONST|F_PLISTE,    "1"         , f_einsmat ,1,1   ,{PL_INT}},
 { F_CONST|F_PLISTE,  "CONVOLUT"       , f_convolut ,2,2   ,{PL_NARRAY,PL_NARRAY}},
#ifdef CONTROL
 { F_PLISTE,    "CSGET"     , f_csvgeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_PLISTE,    "CSVGET"    , f_csvgeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
#endif
 { F_CONST|F_SQUICK,    "CVA"       , string_to_array ,1,1   ,{PL_STRING}},
#ifdef DOOCS
 { F_PLISTE,    "DOOCSGET"     , f_doocsgeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_PLISTE,    "DOOCSNAMES"     , f_doocsnames ,1,1   ,{PL_STRING}},
 { F_PLISTE,    "DOOCSVGET"    , f_doocsgeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
#endif
 { F_CONST|F_PLISTE,  "FFT"        , f_fft ,1,2   ,{PL_CFARRAY,PL_INT}},

 { F_CONST|F_AQUICK,  "INV"         , inv_array ,1,1   ,{PL_CFARRAY}},
 { F_CONST|F_PLISTE,  "SMUL"        , f_smula ,2,2   ,{PL_NARRAY,PL_NUMBER}},
 { F_CONST|F_PLISTE,  "SOLVE"       , f_solvea ,2,2   ,{PL_CFARRAY,PL_CFARRAY}},

#ifdef TINE
 { F_PLISTE,    "TINEGET"     , f_tinegeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_PLISTE,    "TINEVGET"    , f_tinegeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_PLISTE,    "TINEHISTORY" , f_tinehistorya ,3,3   ,{PL_STRING,PL_INT,PL_INT}},
#endif
 { F_CONST|F_AQUICK,    "TRANS"     , trans_array ,1,1   ,{PL_ARRAY}},

};
const int anzpafuncs=sizeof(pafuncs)/sizeof(AFUNCTION);

