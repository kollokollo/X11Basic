/* functions.c   Sammlung von Funktionen (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif

#include "defs.h"
#ifdef HAVE_CACOS
#include <complex.h>
#endif
#include "x11basic.h"
#include "variablen.h"
#include "xbasic.h"
#include "type.h"
#include "parser.h"
#include "parameter.h"
#include "gkommandos.h"
#include "functions.h"
#include "afunctions.h"
#include "array.h"
#include "wort_sep.h"
#include "graphics.h"
#include "number.h"
#include "decode.h"
#include "aes.h"
#include "io.h"
#include "file.h"
#include "window.h"

#ifndef WINDOWS
  #include <fnmatch.h>
#else
  #include "Windows.extension/fnmatch.h"
#endif

#define NOIMP(a)  printf("The %s function is not implemented \n" \
  " in this version of X11-Basic because the COMPLEXMATH library \n" \
  " was not present at compile time.\n",a)



#ifdef DUMMY_LIST
#define cimag NULL
#define creal NULL
#define atan2 NULL
#define ceil NULL
#define exp NULL
#define floor NULL
#define sin NULL
#define hypot NULL
#define log NULL




#define f_call NULL
#define f_det NULL
#define f_eventf NULL
#define f_exec NULL
#define f_freefile NULL
#define f_gray NULL
#define inp8 NULL
#define inp32 NULL
#define inp16 NULL
#define inpf NULL
#define f_ioctl NULL
#define shm_attach NULL
#define shm_malloc NULL
#define f_symadr NULL
#define type NULL
#define f_abs NULL
#define f_acos NULL
#define f_acosh NULL
#define f_add NULL
#define f_and NULL
#define carg NULL
#define arrptr NULL
#define f_asc NULL
#define f_asin NULL
#define f_asinh NULL
#define f_atan NULL
#define f_atanh NULL
#define f_atan NULL
#define f_bchg NULL
#define f_bclr NULL
#define f_bset NULL
#define f_btst NULL
#define f_byte NULL
#define f_call NULL
#define f_card NULL
#define f_cint NULL
#define f_color_rgb NULL
#define f_combin NULL
#define myconj NULL
#define myconj NULL
#define conj NULL
#define f_cos NULL
#define f_cosh NULL
#define f_crc NULL
#define f_cspid NULL
#define f_cssize NULL
#define f_cvi NULL
#define f_cvl NULL
#define f_cvf NULL
#define f_det NULL
#define f_device NULL
#define f_dimf NULL
#define f_div NULL
#define f_doocssize NULL
#define f_doocstyp NULL
#define f_dpeek NULL
#define f_eof NULL
#define f_even NULL
#define f_eventf NULL
#define f_exec NULL
#define f_exist NULL
#define f_exp NULL
#define f_fak NULL
#define fork NULL
#define f_nop NULL
#define f_form_alert NULL
#define f_form_center NULL
#define f_form_dial NULL
#define f_form_do NULL
#define f_freefile NULL
#define f_gcd NULL
#define f_get_color NULL
#define f_glob NULL
#define f_gray NULL
#define f_inode NULL
#define inp8 NULL
#define inp32 NULL
#define inp16 NULL
#define inpf NULL
#define f_instr NULL
#define f_int NULL
#define f_invert NULL
#define f_ioctl NULL
#define f_julian NULL
#define f_lcm NULL
#define f_len NULL
#define f_listselect NULL
#define f_log NULL
#define f_loc NULL
#define f_lof NULL
#define f_log NULL
#define f_log10 NULL
#define f_lpeek NULL
#define f_malloc NULL
#define f_max NULL
#define f_min NULL
#define f_mod NULL
#define f_mode NULL
#define f_mshrink NULL
#define f_mul NULL
#define f_nextprime NULL
#define f_nlink NULL
#define f_not NULL
#define f_objc_draw NULL
#define f_objc_find NULL
#define f_objc_offset NULL
#define f_odd NULL
#define f_or NULL
#define f_peek NULL
#define f_point NULL
#define f_powm NULL
#define f_point NULL
#define rand NULL
#define f_random NULL
#define f_realloc NULL
#define f_rinstr NULL
#define f_rol NULL
#define f_ror NULL
#define f_root NULL
#define f_rsrc_gaddr NULL
#define f_sensor NULL
#define f_sgn NULL
#define f_shl NULL
#define shm_attach NULL
#define shm_malloc NULL
#define f_shr NULL
#define f_sin NULL
#define f_sinh NULL
#define f_size NULL
#define f_sqrt NULL
#define f_sqrt NULL
#define f_srand NULL
#define f_sub NULL
#define f_succ NULL
#define f_swap NULL
#define f_symadr NULL
#define f_tally NULL
#define f_tan NULL
#define f_tanh NULL
#define f_tinesize NULL
#define f_tinetyp NULL
#define type NULL
#define f_valf NULL
#define f_variat NULL
#define f_varptr NULL
#define f_ubound NULL
#define f_word NULL
#define f_wort_sep NULL
#define f_xor NULL
#define f_val NULL
#define f_round NULL
#define f_rnd NULL
#define f_rad NULL
#define f_pred NULL
#define f_ltextlen NULL
#define f_gasdev NULL
#define f_frac NULL
#define f_eval NULL
#define f_deg NULL
#define f_cvd NULL

#else

#ifndef HAVE_LOGB
static double logb(double a) {return(log(a)/log(2));}
#endif
#ifndef HAVE_LOG1P
static double log1p(double a) {return(log(1+a));}
#endif
#ifndef HAVE_EXPM1
static double expm1(double a) {return(exp(a)-1);}
#endif
#ifndef HAVE_CACOS
static COMPLEX cacos(COMPLEX a) {NOIMP("cacos");return(a);}
static COMPLEX cacosh(COMPLEX a) {NOIMP("cacosh");return(a);}
static COMPLEX casin(COMPLEX a) {NOIMP("casin");return(a);}
static COMPLEX casinh(COMPLEX a) {NOIMP("casin");return(a);}
static COMPLEX catan(COMPLEX a) {NOIMP("catan");return(a);}
static COMPLEX catanh(COMPLEX a) {NOIMP("catanh");return(a);}
static COMPLEX clog(COMPLEX a) {
  COMPLEX b;
  b.r=log(hypot(a.r,a.i));
  b.i=atan2(a.i,a.r);
  return(b);
}
static COMPLEX myconj(COMPLEX a)  {a.i=-a.i;return(a);}
static COMPLEX cexp(COMPLEX a) {
  COMPLEX b;
  double r=exp(a.r);
  b.r=r*(cos(a.i)-sin(a.i));
  b.i=r*(sin(a.i)+cos(a.i));
  return(b);
}
static double cimag(COMPLEX a) {return(a.i);}
static double creal(COMPLEX a) {return(a.r);}
static COMPLEX csin(COMPLEX a) {
  COMPLEX i;i.r=0;i.i=1;
  COMPLEX i2;i2.r=0;i2.i=2;
  COMPLEX iz=complex_mul(i,a);
  COMPLEX iz2;
  iz2.r=-iz.r;
  iz2.i=-iz.i;
  return(complex_div(complex_sub(cexp(iz),cexp(iz2)),i2));
}
static COMPLEX ccos(COMPLEX a) {
  COMPLEX i;i.r=0;i.i=1;
  COMPLEX iz=complex_mul(i,a);
  COMPLEX iz2;
  iz2.r=-iz.r;
  iz2.i=-iz.i;
  return(complex_div(complex_add(cexp(iz),cexp(iz2)),FLOAT2COMPLEX(2)));
}
static COMPLEX ccosh(COMPLEX a) {
  COMPLEX iz2;
  iz2.r=-a.r;
  iz2.i=-a.i;
  return(complex_div(complex_add(cexp(a),cexp(iz2)),FLOAT2COMPLEX(2)));
}
static COMPLEX csinh(COMPLEX a) {
  COMPLEX iz2;
  iz2.r=-a.r;
  iz2.i=-a.i;
  return(complex_div(complex_sub(cexp(a),cexp(iz2)),FLOAT2COMPLEX(2)));
}
static COMPLEX ctan(COMPLEX a) {return(complex_div(csin(a),ccos(a)));}
static COMPLEX ctanh(COMPLEX a) {return(complex_div(csinh(a),ccosh(a)));}
static COMPLEX csqrt(COMPLEX a) {NOIMP("csqrt");return(a);}
#endif
#ifndef HAVE_CLOG10
static COMPLEX clog10(COMPLEX a) {return(complex_div(((COMPLEX (*)())clog)(a),FLOAT2COMPLEX(10)));}
#endif

#ifdef WINDOWS
static COMPLEX myconj(COMPLEX a)  {a.i=-a.i;return(a);}
#endif
extern int shm_malloc(int,int);
extern int shm_attach(int);

static char   *arrptr(PARAMETER *p,int e) {return((char *)variablen[p->integer].pointer.a); }

/*Funktionen (mit f_ prefix )*/

static PARAMETER f_random(PARAMETER *plist,int e) {
 extern gmp_randstate_t state;
 extern int randstate_isinit;
 PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
  pret.typ=plist->typ;
  switch(pret.typ) {
  case PL_INT: 
    pret.integer=(int)((double)rand()/RAND_MAX*((double)plist->integer));
    break;
  case PL_COMPLEX:
    pret.imag=(double)rand()/RAND_MAX*plist->imag;
  case PL_FLOAT:
    pret.real=(double)rand()/RAND_MAX*plist->real;
    break;
  case PL_ARBINT:
    if(!randstate_isinit) {
      gmp_randinit_default(state);
      randstate_isinit=1;
    }
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_urandomm(*(ARBINT *)(pret.pointer),state,*(ARBINT *)(plist->pointer));
    break;  
  default: xberror(13,"RANDOM");  /* Type mismatch */
  }
  return(pret);
}

static PARAMETER f_add(PARAMETER *plist,int e) {
  PARAMETER pret; bzero(&pret,sizeof(PARAMETER));
  pret.typ=(PL_CONSTGROUP|combine_type(plist->typ&PL_BASEMASK,plist[1].typ&PL_BASEMASK,'+'));
  cast_to_x(plist,pret.typ);
  cast_to_x(plist+1,pret.typ);
  switch(pret.typ) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     pret.integer=plist->integer+plist[1].integer; break;
  case PL_COMPLEX: pret.imag=plist->imag+plist[1].imag;
  case PL_FLOAT:   pret.real=plist->real+plist[1].real;       break;
  case PL_ARBINT:  
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_add(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer,*(ARBINT *)(plist+1)->pointer); 
    break;
  case PL_STRING: 
    pret.pointer=malloc(plist->integer+(plist+1)->integer+1);
    pret.integer=plist->integer+(plist+1)->integer;
    memcpy(pret.pointer,plist->pointer,plist->integer);
    memcpy(pret.pointer+plist->integer,(plist+1)->pointer,(plist+1)->integer);
    ((char *)pret.pointer)[pret.integer]=0;
    break;
  default: xberror(13,"ADD");  /* Type mismatch */
  }
  return(pret);
}
static PARAMETER f_sub(PARAMETER *plist,int e) {
  PARAMETER pret; bzero(&pret,sizeof(PARAMETER));
  pret.typ=(PL_CONSTGROUP|combine_type(plist->typ&PL_BASEMASK,plist[1].typ&PL_BASEMASK,'+'));
  cast_to_x(plist,pret.typ);
  cast_to_x(plist+1,pret.typ);
  switch(pret.typ) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     pret.integer=plist->integer-plist[1].integer; break;
  case PL_COMPLEX: pret.imag=plist->imag-plist[1].imag;
  case PL_FLOAT:   pret.real=plist->real-plist[1].real;       break;
  case PL_ARBINT:  
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_sub(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer,*(ARBINT *)(plist+1)->pointer); 
    break;
  default: xberror(13,"SUB");  /* Type mismatch */
  }
  return(pret);
}
static PARAMETER f_mul(PARAMETER *plist,int e) {
  PARAMETER pret; bzero(&pret,sizeof(PARAMETER));
  pret.typ=(PL_CONSTGROUP|combine_type(plist->typ&PL_BASEMASK,plist[1].typ&PL_BASEMASK,'+'));
  cast_to_x(plist,pret.typ);
  cast_to_x(plist+1,pret.typ);
  switch(pret.typ) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     pret.integer=plist->integer*plist[1].integer; break;
  case PL_COMPLEX: 
    *(COMPLEX *)&pret.real=complex_mul(*(COMPLEX *)&(plist->real),*(COMPLEX *)&(plist[1].real));
    break;
  case PL_FLOAT:   pret.real=plist->real*plist[1].real;       break;
  case PL_ARBINT:  
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_mul(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer,*(ARBINT *)(plist+1)->pointer); 
    break;
  default: xberror(13,"MUL");  /* Type mismatch */
  }
  return(pret);
}
static PARAMETER f_div(PARAMETER *plist,int e) {
  PARAMETER pret; bzero(&pret,sizeof(PARAMETER));
  pret.typ=(PL_CONSTGROUP|combine_type(plist->typ&PL_BASEMASK,plist[1].typ&PL_BASEMASK,'/'));
  cast_to_x(plist,pret.typ);
  cast_to_x(plist+1,pret.typ);
  switch(pret.typ) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     pret.integer=plist->integer/plist[1].integer; break;
  case PL_COMPLEX: 
    *(COMPLEX *)&pret.real=complex_div(*(COMPLEX *)&(plist->real),*(COMPLEX *)&(plist[1].real));
    break;
  case PL_FLOAT:   pret.real=plist->real/plist[1].real;       break;
  case PL_ARBINT:  
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_div(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer,*(ARBINT *)(plist+1)->pointer); 
    break;
  default: xberror(13,"DIV");  /* Type mismatch */
  }
  return(pret);
}
static PARAMETER f_max(PARAMETER *plist,int e) {
  if(e==1) return(double_parameter(plist));
  PARAMETER pret; bzero(&pret,sizeof(PARAMETER));
  pret.typ=plist->typ;
  int i;
  for(i=1;i<e;i++) pret.typ=(PL_CONSTGROUP|combine_type(pret.typ&PL_BASEMASK,plist[i].typ&PL_BASEMASK,'+'));
  for(i=0;i<e;i++) cast_to_x(&plist[i],pret.typ);
  switch(pret.typ) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     
    pret.integer=plist->integer;
    for(i=1;i<e;i++) {if(plist[i].integer>pret.integer) pret.integer=plist[i].integer;}    
    break;
  case PL_FLOAT:
    pret.real=plist->real;
    for(i=1;i<e;i++) {if(plist[i].real>pret.real) pret.real=plist[i].real;}    
    break;
  case PL_COMPLEX: 
    pret.real=plist->real;
    pret.imag=plist->imag;
    for(i=1;i<e;i++) {if(plist[i].real*plist[i].real+plist[i].imag*plist[i].imag>pret.real*pret.real+pret.imag*pret.imag) {pret.real=plist[i].real;pret.imag=plist[i].imag;}}
    break;
  case PL_ARBINT:
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_set(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer);
    for(i=1;i<e;i++) {
      if(mpz_cmp(*(ARBINT *)pret.pointer,*(ARBINT *)plist[i].pointer)<0) {
        mpz_set(*(ARBINT *)pret.pointer,*(ARBINT *)plist[i].pointer);
      }
    }
    break;
  default: xberror(13,"MAX");  /* Type mismatch */
  }
  return(pret);
}
static PARAMETER f_min(PARAMETER *plist,int e) {
  if(e==1) return(double_parameter(plist));
  PARAMETER pret; bzero(&pret,sizeof(PARAMETER));
  pret.typ=plist->typ;
  int i;
  for(i=1;i<e;i++) pret.typ=(PL_CONSTGROUP|combine_type(pret.typ&PL_BASEMASK,plist[i].typ&PL_BASEMASK,'+'));
  for(i=0;i<e;i++) cast_to_x(&plist[i],pret.typ);
  switch(pret.typ) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     
    pret.integer=plist->integer;
    for(i=1;i<e;i++) {if(plist[i].integer<pret.integer) pret.integer=plist[i].integer;}    
    break;
  case PL_FLOAT:
    pret.real=plist->real;
    for(i=1;i<e;i++) {if(plist[i].real<pret.real) pret.real=plist[i].real;}    
    break;
  case PL_COMPLEX: 
    pret.real=plist->real;
    pret.imag=plist->imag;
    for(i=1;i<e;i++) {if(plist[i].real*plist[i].real+plist[i].imag*plist[i].imag<pret.real*pret.real+pret.imag*pret.imag) {pret.real=plist[i].real;pret.imag=plist[i].imag;}}
    break;
  case PL_ARBINT:
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_set(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer);
    for(i=1;i<e;i++) {
      if(mpz_cmp(*(ARBINT *)pret.pointer,*(ARBINT *)plist[i].pointer)>0) {
        mpz_set(*(ARBINT *)pret.pointer,*(ARBINT *)plist[i].pointer);
      }
    }
    break;
  default: xberror(13,"MIN");  /* Type mismatch */
  }
  return(pret);
}


static PARAMETER f_abs(PARAMETER *plist,int e) {
  PARAMETER pret; bzero(&pret,sizeof(PARAMETER));
  pret.typ=plist->typ;
  switch(pret.typ) {
  case PL_INT: pret.integer=abs(plist->integer); break;
  case PL_FLOAT: pret.real=fabs(plist->real); break;
  case PL_COMPLEX: 
    pret.real=hypot(plist->real,plist->imag); 
    pret.imag=0;
    break;
  case PL_ARBINT:
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_abs(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer);
    break;
  }
  return(pret);
}
static PARAMETER f_acos(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=cacos(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=cacos(*(complex double *)&(plist->real));
#endif
  else                       pret.real=acos(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_acosh(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=cacosh(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=cacosh(*(complex double *)&(plist->real));
#endif
  else                       pret.real=acosh(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_asin(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=casin(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=casin(*(complex double *)&(plist->real));
#endif
  else                       pret.real=asin(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_asinh(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=casinh(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=casinh(*(complex double *)&(plist->real));
#endif
  else                       pret.real=asinh(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_atan(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=catan(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=catan(*(complex double *)&(plist->real));
#endif
  else                       pret.real=atan(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_atanh(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=catanh(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=catanh(*(complex double *)&(plist->real));
#endif
  else                       pret.real=atanh(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_tan(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=ctan(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=ctan(*(complex double *)&(plist->real));
#endif
  else                       pret.real=tan(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_tanh(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=ctanh(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=ctanh(*(complex double *)&(plist->real));
#endif
  else                       pret.real=tanh(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_sin(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=csin(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=csin(*(complex double *)&(plist->real));
#endif
  else                       pret.real=sin(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_sinh(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=csinh(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=csinh(*(complex double *)&(plist->real));
#endif
  else                       pret.real=sinh(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_cos(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=ccos(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=ccos(*(complex double *)&(plist->real));
#endif
  else                       pret.real=cos(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_cosh(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=ccosh(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=ccosh(*(complex double *)&(plist->real));
#endif
  else                       pret.real=cosh(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_log(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifdef HAVE_COMPLEX_H
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=clog(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=clog(*(complex double *)&(plist->real));
#endif
  else
#endif
  pret.real=log(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_log10(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifdef HAVE_COMPLEX_H
#ifndef HAVE_CLOG10
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=((COMPLEX (*)())clog10)(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=clog10(*(complex double *)&(plist->real));
#endif
  else
#endif
  pret.real=log10(plist->real);
  pret.typ=plist->typ;
  return(pret);
}
static PARAMETER f_exp(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
#ifndef HAVE_CACOS
  if(plist->typ==PL_COMPLEX) *(COMPLEX *)&pret.real=cexp(*(COMPLEX *)&(plist->real));
#else
  if(plist->typ==PL_COMPLEX) *(complex double *)&pret.real=cexp(*(complex double *)&(plist->real));
#endif
  else                       pret.real=exp(plist->real);
  pret.typ=plist->typ;
  return(pret);
}

static PARAMETER f_mod(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
  pret.typ=(PL_CONSTGROUP|combine_type(plist->typ&PL_BASEMASK,plist[1].typ&PL_BASEMASK,'+'));
  cast_to_x(plist,pret.typ);
  cast_to_x(plist+1,pret.typ);
  switch(pret.typ) {
  case PL_INT: pret.integer=(plist->integer % plist[1].integer); break;
  case PL_FLOAT: pret.real=fmod(plist->real,plist[1].real); break;
  case PL_COMPLEX:  /*TODO */ 
    xberror(77,"MOD"); /*Function %s not defined for complex numbers.*/
    pret.real=0; 
    pret.imag=0;
    break;
  case PL_ARBINT:
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_mod(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer,*(ARBINT *)(plist[1].pointer));
    break;
  }
  return(pret);
}

static PARAMETER f_sqrt(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
  switch(plist->typ) {
  case PL_FLOAT: pret.real=sqrt(plist->real); break;
  case PL_COMPLEX: 
#ifndef HAVE_CACOS
    *(COMPLEX *)&pret.real=csqrt(*(COMPLEX *)&(plist->real)); 
#else
    *(complex double *)&pret.real=csqrt(*(complex double *)&(plist->real)); 
#endif
    break;
  case PL_ARBINT:
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_sqrt(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer);
    break;
  }
  pret.typ=plist->typ;
  return(pret);
}


static int    f_asc(STRING n)   {return((int)n.pointer[0]); }
static int    f_byte(int a)     {return(a&0xff);}
static int    f_card(int a)     {return(a&0xffff);}
static int    f_word(int a)     {return((signed int)(signed short)(a&0xffff));}
static int    f_swap(int a)     {return(((a&0xffff)<<16)|((a&0xffff0000)>>16));}

static double f_cvd(STRING n)   {return((double)(*((double *)n.pointer))); }
static double f_cvf(STRING n)   {return((double)(*((float *)n.pointer))); }
static int    f_cvi(STRING n)   {return((int)(*((short *)n.pointer))); }
static int    f_cvl(STRING n)   {return((int)(*((long *)n.pointer))); }
static double f_deg(double d)   {return(d/PI*180);}
static int    f_device(STRING n) {return(stat_device(n.pointer)); }
static int    f_dpeek(int adr)  {return((int)(*(short *)adr));}
static double f_eval(STRING n)  {return(parser(n.pointer));}
static int    f_exist(STRING n) {return(-exist(n.pointer)); }
static double f_frac(double b)  {return(b-((double)((int)b)));}
//static double f_arg(COMPLEX b) {return(atan2(b.i,b.r));}
//static double f_imag(COMPLEX b) {return(b.i);}
//static double f_real(COMPLEX b) {return(b.r);}
static int    f_inode(STRING n) {return(stat_inode(n.pointer)); }
static int    f_len(STRING n)   {return(n.len); }
static int    f_lpeek(int adr)  {return((int)(*(long *)adr));}
static double f_ltextlen(STRING n) {return((double)ltextlen(ltextxfaktor,ltextpflg,n.pointer)); }
static int    f_malloc(int size) {return((int)malloc((size_t)size));}
static int    f_mshrink(int adr,int size) {return(0);} /*always return zero*/
static int    f_mode(STRING n)  {return(stat_mode(n.pointer)); }
static int    f_nlink(STRING n) {return(stat_nlink(n.pointer)); }
       double f_nop(void *t)    {return(0.0);}
static int    f_odd(PARAMETER *plist,int e)      {
  switch(plist->typ) {
  case PL_ARBINT:
    return(mpz_odd_p(*(ARBINT *)(plist->pointer))? -1:0);
  case PL_COMPLEX:
  case PL_FLOAT:
       plist->integer=(int)plist->real;
  case PL_INT:
  default:
    return(plist->integer&1 ? -1:0);
  }
}
static int    f_even(PARAMETER *plist,int e)      {
  switch(plist->typ) {
  case PL_ARBINT:
    return(mpz_even_p(*(ARBINT *)(plist->pointer))? -1:0);
  case PL_COMPLEX:
  case PL_FLOAT:
       plist->integer=(int)plist->real;
  case PL_INT:
  default:
    return(plist->integer&1 ? 0:-1);
  }
}
static int    f_peek(int adr)   {return((int)(*(char *)adr));}
#ifndef NOGRAPHICS
static int f_point(double v1, double v2) {return(get_point((int)v1,(int)v2));}
#endif
static double f_rad(double d)   {return(d*PI/180);}
static int    f_realloc(int adr,int size) {return((int)realloc((char *)adr,(size_t)size));}
static double f_rnd(double d)   {return((double)rand()/RAND_MAX);}

static int    f_size(STRING n)  {return(stat_size(n.pointer)); }
static int    f_sgn(double b)   {return(sgn(b));}
static int    f_srand(double d) {srand((int)d);return(0);}
static int    f_succ(double b)  {return((int)(b+1));}

static double f_val(STRING n)   {return(myatof(n.pointer)); }
static int    f_valf(STRING n)   {
  if(n.len==0) return(0);
  return(myatofc(n.pointer));
}

#ifdef TINE
static double f_tinemax(STRING n) { return(tinemax(n.pointer)); }
static double f_tinemin(STRING n) { return(tinemin(n.pointer)); }
static double f_tineget(STRING n) { return(tineget(n.pointer)); }
static int f_tinesize(STRING n) {return(tinesize(n.pointer)); }
static int f_tinetyp(STRING n)  {return(tinetyp(n.pointer)); }
#endif
#ifdef DOOCS
static double f_doocsget(STRING n) { return(doocsget(n.pointer)); }
static int f_doocssize(STRING n) {return(doocssize(n.pointer)); }
static int f_doocstyp(STRING n)  {return(doocstyp(n.pointer)); }
static double f_doocstimestamp(STRING n) { return(doocstimestamp(n.pointer)); }
#endif
#ifdef CONTROL
static double f_csmax(STRING n)  {return(csmax(n.pointer)); }
static double f_csmin(STRING n)  {return(csmin(n.pointer)); }
static double f_csres(STRING n)  {return(csres(n.pointer)); }
static double f_csget(STRING n)  {return(csget(n.pointer)); }
static int f_cssize(STRING n)    {return(cssize(n.pointer)); }
static int f_cspid(STRING n)     {return(cspid(n.pointer)); }
#endif

/*  Logische Funktionen (selten benutzt) */

static PARAMETER f_xor(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
  if(plist->typ==PL_INT && plist[1].typ==PL_INT) {  /*Damit es schnell geht....*/
    pret.typ=PL_INT;
    pret.integer=plist->integer ^ plist[1].integer;
    return(pret);
  }
  pret.typ=PL_ARBINT;  /*Alle anderen Typen zu big int wandeln*/
  cast_to_x(plist,pret.typ);
  cast_to_x(plist+1,pret.typ);
  pret.pointer=malloc(sizeof(ARBINT));
  mpz_init(*(ARBINT *)pret.pointer);
  mpz_xor(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer,*(ARBINT *)(plist+1)->pointer); 
  return(pret);
}
static PARAMETER f_and(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
  if(plist->typ==PL_INT && plist[1].typ==PL_INT) {  /*Damit es schnell geht....*/
    pret.typ=PL_INT;
    pret.integer=plist->integer & plist[1].integer;
    return(pret);
  }
  pret.typ=PL_ARBINT;  /*Alle anderen Typen zu big int wandeln*/
  cast_to_x(plist,pret.typ);
  cast_to_x(plist+1,pret.typ);
  pret.pointer=malloc(sizeof(ARBINT));
  mpz_init(*(ARBINT *)pret.pointer);
  mpz_and(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer,*(ARBINT *)(plist+1)->pointer); 
  return(pret);
}
static PARAMETER f_or(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
  if(plist->typ==PL_INT && plist[1].typ==PL_INT) {  /*Damit es schnell geht....*/
    pret.typ=PL_INT;
    pret.integer=plist->integer | plist[1].integer;
    return(pret);
  }
  pret.typ=PL_ARBINT;  /*Alle anderen Typen zu big int wandeln*/
  cast_to_x(plist,pret.typ);
  cast_to_x(plist+1,pret.typ);
  pret.pointer=malloc(sizeof(ARBINT));
  mpz_init(*(ARBINT *)pret.pointer);
  mpz_ior(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer,*(ARBINT *)(plist+1)->pointer); 
  return(pret);
}
static PARAMETER f_not(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
  if(plist->typ==PL_INT) {  /*Damit es schnell geht....*/
    pret.typ=PL_INT;
    pret.integer=(~plist->integer);
    return(pret);
  }
  pret.typ=PL_ARBINT;  /*Alle anderen Typen zu big int wandeln*/
  cast_to_x(plist,pret.typ);
  pret.pointer=malloc(sizeof(ARBINT));
  mpz_init(*(ARBINT *)pret.pointer);
  mpz_com(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer); 
  return(pret);
}

/*  Bit manipulation functions */

static int f_shr(int v1, int v2)  {return(v1>>v2);}
static int f_shl(int v1, int v2)  {return(v1<<v2);}
static int f_ror(PARAMETER *plist,int e)  {
  int w=sizeof(int)*8;
  if(e>2) w=plist[2].integer;
  int n=plist[1].integer;
  unsigned int pat=(1<<w)-1;
  unsigned int a=plist->integer&pat;
  unsigned int b=plist->integer&(~pat);
  return(b|(a>>n)|((a<<(w-n))&pat));
}
static int f_rol(PARAMETER *plist,int e)  {
  int w=sizeof(int)*8;
  if(e>2) w=plist[2].integer;
  int n=plist[1].integer;
  unsigned int pat=(1<<w)-1;
  unsigned int a=plist->integer&pat;
  unsigned int b=plist->integer&(~pat);
  return(b|((a<<n)&pat)|(a>>(w-n)));
}

static int f_btst(PARAMETER *plist,int e) {
  switch(plist->typ) {
  case PL_INT: return(((plist->integer & (1 <<plist[1].integer))==0) ?  0 : -1);
  case PL_FLOAT:
  case PL_COMPLEX: cast_to_arbint(plist);
  case PL_ARBINT:
    return(mpz_tstbit(*(ARBINT *)(plist->pointer),plist[1].integer)?-1:0); 
  }
  return(0);
}
static PARAMETER f_bset(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
  switch(plist->typ) {
  case PL_INT:
    pret.typ=PL_INT; 
    pret.integer=plist->integer | (1 <<plist[1].integer);break;
  case PL_FLOAT:
  case PL_COMPLEX: cast_to_arbint(plist);
  case PL_ARBINT:
    pret.typ=PL_ARBINT;
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_set(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer); 
    mpz_setbit(*(ARBINT *)pret.pointer,plist[1].integer); 
    break;
  }
  return(pret);
}
static PARAMETER f_bclr(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
  switch(plist->typ) {
  case PL_INT:
    pret.typ=PL_INT; 
    pret.integer=plist->integer & ~(1 <<plist[1].integer);break;
  case PL_FLOAT:
  case PL_COMPLEX: cast_to_arbint(plist);
  case PL_ARBINT:
    pret.typ=PL_ARBINT;
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_set(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer); 
    mpz_clrbit(*(ARBINT *)pret.pointer,plist[1].integer); 
    break;
  }
  return(pret);
}
static PARAMETER f_bchg(PARAMETER *plist,int e) {
  PARAMETER pret;bzero(&pret,sizeof(PARAMETER));
  switch(plist->typ) {
  case PL_INT:
    pret.typ=PL_INT; 
    pret.integer=plist->integer ^ (1 <<plist[1].integer);break;
  case PL_FLOAT:
  case PL_COMPLEX: cast_to_arbint(plist);
  case PL_ARBINT:
    pret.typ=PL_ARBINT;
    pret.pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)pret.pointer);
    mpz_set(*(ARBINT *)pret.pointer,*(ARBINT *)plist->pointer); 
    mpz_combit(*(ARBINT *)pret.pointer,plist[1].integer); 
    break;
  }
  return(pret);
}

#ifdef ATARI
  double round(double a) {return((double)(int)(a+0.5));}
#endif


static int f_int(double b) {return((int)b);}
static int f_cint(double b) {return((int)round(b));}
static double f_pred(double b) {return(ceil(b-1));}

static int  f_dimf(PARAMETER *plist,int e) {
  return((double)do_dimension(&variablen[plist->integer]));
}

static void f_combin(ARBINT ret,PARAMETER *plist,int e) {
  if(plist[1].integer<0) xberror(88,""); /*FACT/COMBIN/VARIAT nicht definiert*/
  mpz_bin_uiui(ret,plist->integer,plist[1].integer);
}
static void f_fak(ARBINT ret,PARAMETER *plist,int e) {
  if(plist->integer<0) xberror(88,""); /*FACT/COMBIN/VARIAT nicht definiert*/
  mpz_fac_ui(ret,plist->integer);
}
static void f_root(ARBINT ret,PARAMETER *plist,int e) {
  if(plist[1].integer<=0) xberror(88,""); /*FACT/COMBIN/VARIAT/ROOT nicht definiert*/
  mpz_root(ret,*(ARBINT *)(plist->pointer),plist[1].integer);
}
static int f_variat(PARAMETER *plist,int e) {
  int n=plist[0].integer,k=plist[1].integer,i;
  double zz=1;
  if(k==n) return(1);  
  if(k>n || n<=0 || k<=0) {
    xberror(88,""); /*FACT/COMBIN/VARIAT nicht definiert*/
    return(0);
  }
  for(i=n-k+1;i<=n;i++)  zz*=i;
  return((int)zz);
}

static void f_gcd(ARBINT ret,PARAMETER *plist,int e) {
  mpz_gcd(ret,*(ARBINT *)plist->pointer,*(ARBINT *)plist[1].pointer);
}
static void f_lcm(ARBINT ret,PARAMETER *plist,int e) {
  mpz_lcm(ret,*(ARBINT *)plist->pointer,*(ARBINT *)plist[1].pointer);
}
static void f_nextprime(ARBINT ret,PARAMETER *plist,int e) {
  mpz_nextprime(ret,*(ARBINT *)plist->pointer);
}
static void f_invert(ARBINT ret,PARAMETER *plist,int e) {
  int i=mpz_invert(ret,*(ARBINT *)(plist->pointer),*(ARBINT *)(plist[1].pointer));
  if(i==0) mpz_set(ret,*(ARBINT *)(plist[1].pointer));
}
static void f_powm(ARBINT ret,PARAMETER *plist,int e) {
  mpz_powm(ret,*(ARBINT *)(plist->pointer),*(ARBINT *)(plist[1].pointer),*(ARBINT *)(plist[2].pointer));
}



static double f_gasdev(double d) { /* Gaussverteilter Zufall */
  static int flag=1;
  static double gset;
  double fac,rsq,v1,v2;
  flag=(!flag);
  if(flag) return(gset);
  else {
    do {
      v1=2*((double)rand()/RAND_MAX)-1;
      v2=2*((double)rand()/RAND_MAX)-1;
      rsq=v1*v1+v2*v2;
    } while(rsq>=1 || rsq==0);
    fac=sqrt(-2*log(rsq)/rsq);
    gset=v1*fac;
    return(v2*fac);
  }
}
static double f_round(PARAMETER *plist,int e) {
  if(e==1) return(round(plist->real));
  if(e>=2){
    int kommast=-plist[1].integer;
    return(round(plist[0].real/pow(10,kommast))*pow(10,kommast)); 
  }
  return(0);
}

#define IGREG (15+31L*(10+12L*1582))
static int f_julian(STRING n) { /* Julianischer Tag aus time$ */
  char buf[n.len+1],buf2[n.len+1];
  int e;
  int day,mon,jahr;	
  long jul;
  int ja,jy,jm;

  memcpy(buf,n.pointer,n.len);
  buf[n.len]=0;
  e=wort_sep(buf,'.',0,buf2,buf);
  if(e<2) return(-1);
  day=atoi(buf2);
  e=wort_sep(buf,'.',0,buf2,buf);
  if(e<2) return(-1);
  mon=atoi(buf2);
  jy=jahr=atoi(buf);
  if(jy==0) return(-1); /* Jahr 0 gibt es nicht */
  if(jy<0) ++jy;
  if(mon>2) jm=mon+1;
  else { --jy; jm=mon+13; }
  jul=(long)(floor(365.25*jy)+floor(30.6001*jm)+day+1720995);
  if(day+31L*(mon+12L*jahr)>=IGREG) {
    ja=(int)(0.01*jy);
    jul+=2-ja+(int)(0.25*ja);
  }
  return(jul);
}
#undef IGREG



static int f_crc(PARAMETER *plist,int e) { 
  if(e>1) return(update_crc(plist[1].integer, plist->pointer, plist->integer));
  return(update_crc(0L, plist->pointer, plist->integer));
}


static double f_sensor(int n) {
 //  printf("Sensor #%d\n",n);
  if(n>=0 && n<ANZSENSORS) return(sensordata[n]);
  else return(-1);
}


static int f_lof(PARAMETER *plist,int e) {
// dump_parameterlist(plist,e);
  if(filenr[plist[0].integer].typ) return(lof(filenr[plist[0].integer].dptr));
  else { xberror(24,""); return(0);} /* File nicht geoeffnet */
}
static int f_loc(PARAMETER *plist,int e) {
  if(filenr[plist[0].integer].typ) return(ftell(filenr[plist[0].integer].dptr));
  else { xberror(24,""); return(0);} /* File nicht geoeffnet */
}
static int f_eof(PARAMETER *plist,int e) {
  if(plist[0].integer==-2) return((myeof(stdin))?-1:0);
  else if(filenr[plist[0].integer].typ) {
    // fflush(filenr[plist[0].integer].dptr);
    // dieses fflush hat bei der WINDOWS version dazu geführt, dass
    // ans ende des files gesprungen wurde. Folgendes EOF war immer true
    return(myeof(filenr[plist[0].integer].dptr)?-1:0);
  } else { xberror(24,""); return(0);} /* File nicht geoeffnet */
}

static int f_wort_sep(PARAMETER *plist,int e) {
  STRING str1,str2;
  int ret;
  str1.pointer=malloc(plist->integer+1);
  str2.pointer=malloc(plist->integer+1);
  
  ret=wort_sep2(plist[0].pointer,plist[1].pointer,plist[2].integer,str1.pointer,str2.pointer);
  str1.len=strlen(str1.pointer);
  str2.len=strlen(str2.pointer);
  if(e>3)  varcaststring_and_free((STRING *)plist[3].pointer,str1);  
  else free_string(&str1);
  if(e>4)  varcaststring_and_free((STRING *)plist[4].pointer,str2);
  else free_string(&str2);
  return(ret);
}

static int f_instr(PARAMETER *plist,int e) {
  int start=0;
  if(plist[1].integer>plist->integer) return(0);
  if(e>2) start=plist[2].integer-1;
  if(start<0) start=0;
  if(start>=plist->integer) return(0);  
  char *pos=(char *)memmem(&(((char *)(plist->pointer))[start]),plist->integer-start,plist[1].pointer,plist[1].integer);
  if(pos!=NULL) return((int)(pos-(char *)plist->pointer)+1);
  return(0);
}
static int f_rinstr(PARAMETER *plist,int e) {
  if(plist[1].integer>plist->integer) return(0);
  int start=plist->integer-1;
  if(e>2) start=plist[2].integer-1;
  if(start<0) start=0;
  if(start>=plist->integer) return(0);  
  char *pos=rmemmem(plist->pointer,start,plist[1].pointer,plist[1].integer);
  if(pos!=NULL) return((int)(pos-(char *)plist->pointer)+1);
  return(0);
}
static int f_tally(PARAMETER *plist,int e) {
  int start=0;
  if(e>2) start=plist[2].integer-1;
  if(start<0) start=0;
  if(start>=plist->integer) return(0);  
  if(plist->integer<start+plist[1].integer) return(0);
  int i=start;
  char *t=plist->pointer;
  char *c=plist[1].pointer;
  int j=0;
  for(;;) {
/* suche erstes Vorkommen von c */
    while(i<plist->integer && t[i]!=*c) i++;
    if(i>=plist->integer) return(j); /* schon am ende ? */
    else {     /* ueberpruefe, ob auch der Rest von c vorkommt */
      if(memcmp(t+i,c,plist[1].integer)==0) {
        j++;
	i+=plist[1].integer;
      } else i++;     /* ansonsten weitersuchen */
    }
  }
}

static int f_glob(PARAMETER *plist,int e) {
  int flags=FNM_NOESCAPE;
  if(e>2) flags^=plist[2].integer;
  flags=fnmatch(plist[1].pointer,plist->pointer,flags);
  if(flags==0) return(-1);
  return(0);
}

#ifndef NOGRAPHICS
static int f_form_alert(PARAMETER *plist,int e) {
  return(form_alert(plist[0].integer,plist[1].pointer));
}
static int f_form_center(PARAMETER *plist,int e) {
  short x,y,w,h,ret;
  graphics();
  gem_init();
  ret=form_center((OBJECT *)plist->integer,&x,&y,&w,&h);
  if(e>4) {
    if(plist[1].typ!=PL_LEER) varcastint(plist[1].integer,plist[1].pointer,x);
    if(plist[2].typ!=PL_LEER) varcastint(plist[2].integer,plist[2].pointer,y);
    if(plist[3].typ!=PL_LEER) varcastint(plist[3].integer,plist[3].pointer,w);
    if(plist[4].typ!=PL_LEER) varcastint(plist[4].integer,plist[4].pointer,h);
  }
  return(ret);
}
static int f_form_dial(PARAMETER *plist,int e) {
    graphics();
    gem_init();
    return(form_dial(plist->integer,plist[1].integer,
      plist[2].integer,plist[3].integer,plist[4].integer,plist[5].integer,
      plist[6].integer,plist[7].integer,plist[8].integer));
}
static int f_form_do(PARAMETER *plist,int e) {
  short startob=0;
  if(e>1) startob=plist[1].integer;
  graphics();
  gem_init();
  return(form_do((OBJECT *)plist->integer,startob));
}
static int f_objc_draw(PARAMETER *plist,int e) {
  graphics();
  gem_init();
  /*TODO: Clipping */
  
  return(objc_draw((OBJECT *)plist->integer,plist[1].integer,plist[2].integer,
  0,0,plist[5].integer,plist[6].integer));
}
static int f_objc_find(PARAMETER *plist,int e) {
    return(objc_find((OBJECT *)plist[0].integer,0,7,plist[1].integer,plist[2].integer));
}
static int f_objc_offset(PARAMETER *plist,int e) {
  short x,y,ret;

  if(e>2 && plist[2].typ!=PL_LEER) {
    int typ;
    if((typ=variablen[plist[2].integer].typ)==ARRAYTYP) 
      typ=variablen[plist[2].integer].pointer.a->typ;
    if(typ==INTTYP) x=*((int *)plist[2].pointer);
    else if(typ==FLOATTYP) x=(int)*((double *)plist[2].pointer);
  }
  if(e>3 && plist[3].typ!=PL_LEER) {
    int typ;
    if((typ=variablen[plist[3].integer].typ)==ARRAYTYP) 
      typ=variablen[plist[3].integer].pointer.a->typ;
    if(typ==INTTYP) y=*((int *)plist[3].pointer);
    else if(typ==FLOATTYP) y=(int)*((double *)plist[3].pointer);
  }
  ret=objc_offset((OBJECT *)plist->integer,plist[1].integer,&x,&y);

  if(e>2 && plist[2].typ!=PL_LEER) varcastint(plist[2].integer,plist[2].pointer,x);
  if(e>3 && plist[3].typ!=PL_LEER) varcastint(plist[3].integer,plist[3].pointer,y);

  return(ret);
}
static int f_get_color(PARAMETER *plist,int e) {
  graphics();
  return(get_color(plist->integer>>8,plist[1].integer>>8,plist[2].integer>>8,255));
}
static int f_color_rgb(PARAMETER *plist,int e) {
  double a=1;
  if(e>3) a=plist[3].real;
  graphics();
  return(get_color((unsigned char)(plist[0].real*255.0),(unsigned char)(plist[1].real*255.0),
                   (unsigned char)(plist[2].real*255.0),(unsigned char)(a*255.0)));
}
static int f_rsrc_gaddr(PARAMETER *plist,int e) {
  char *ptr;
  int i=rsrc_gaddr(plist->integer,plist[1].integer,&ptr);
  if(i>0) return((int)ptr);
  else return(-1);
}


static int f_listselect(PARAMETER *plist,int e) {
  int sel=-1;
  if(e>2) sel=plist[2].integer;
  ARRAY a;
  a=*(ARRAY *)&(plist[1].integer);
  return(lsel_input(plist->pointer,(STRING *)(a.pointer+a.dimension*INTSIZE),anz_eintraege(&a),sel));
}
#endif




static int f_varptr(PARAMETER *p,int e) {
  switch(p->typ) {
  case PL_IVAR:
  case PL_FVAR:
  case PL_CVAR:
  case PL_NVAR:
  case PL_IARRAYVAR: /* Variable */
  case PL_FARRAYVAR: /* Variable */
  case PL_AIARRAYVAR: /* Variable */
    return((int)p->pointer);
  case PL_SVAR:
    return((int)((STRING *)p->pointer)->pointer);
  case PL_SARRAYVAR: /* Variable */
    if(p->panzahl>0) return((int)((STRING *)p->pointer)->pointer);
    else return((int)p->pointer);
  case PL_VAR:   /* Variable */
  case PL_ARRAYVAR: /* Variable */
  case PL_ALLVAR:  /* Varname */
  default:
  {
    int typ=variablen[p->integer].typ;
    if(typ==ARRAYTYP) typ=variablen[p->integer].pointer.a->typ;
    if(typ==STRINGTYP) return((int)((STRING *)p->pointer)->pointer);
    else if(typ==ARRAYTYP) return((int)((ARRAY *)p->pointer)->pointer);
    else return((int)p->pointer);
    }
  }  
}

/*Einzelne Dimensionierungen eines (mehrdimensionalen) Arrays*/

static int f_ubound(PARAMETER *p,int e) {
  int idx=0;
  if(e>1) idx=p[1].integer;
  if(variablen[p->integer].typ==ARRAYTYP) {
    ARRAY *a=variablen[p->integer].pointer.a;
    if(idx<0 || idx>=a->dimension) {
      xberror(16,""); /* Feldindex zu gross*/
      return(0);
    }
    return(((int *)a->pointer)[idx]);
  } else xberror(58,variablen[p->integer].name); /* Variable %s has incorrect type */
  return 0;
}



#endif
/*F_CONST fuer die Funktionen, welche bei constantem input imemr das gleiche 
  output liefern.
  */
  
  
  /* Folgende funktionen sollten noch implemntiert werden
  
  MEAN(ARRAY)
  RMS(ARRAY)
  STDDEV(ARRAY)
  int=MEDIAN(ARRAY)
  also
  AMAX(ARRAY)
  AMIN(ARRAY)
  NORM(ARRAY)
  
  */
  
const FUNCTION pfuncs[]= {  /* alphabetisch !!! */
 { F_CONST|F_ARGUMENT|F_DRET,  "!nulldummy", f_nop             ,0,0,{0}},
 { F_CONST|F_PLISTE|F_NRET,    "ABS"       , (pfunc)f_abs      ,1,1,{PL_NUMBER}},
 { F_CONST|F_PLISTE|F_NRET,    "ACOS"      , (pfunc)f_acos     ,1,1,{PL_CF}},
 { F_CONST|F_PLISTE|F_NRET,    "ACOSH"     , (pfunc)f_acosh    ,1,1,{PL_CF}},
 { F_CONST|F_PLISTE|F_ANYRET,  "ADD"       , (pfunc)f_add      ,2,2,{PL_ANYVALUE,PL_ANYVALUE}}, 
 { F_CONST|F_PLISTE|F_ANYIRET, "AND"       , (pfunc)f_and      ,2,2,{PL_NUMBER,PL_NUMBER}},
#ifdef HAVE_CACOS
 { F_CONST|F_CQUICK|F_DRET,    "ARG"       , (pfunc)carg              ,1,1,{PL_COMPLEX}},
#else
 { F_CONST|F_CQUICK|F_DRET,    "ARG"       , atan2              ,1,1,{PL_COMPLEX}},
#endif
 { F_PLISTE|F_IRET,            "ARRPTR"    , (pfunc)arrptr     ,1,1,{PL_ARRAYVAR}},
 { F_CONST|F_SQUICK|F_IRET,    "ASC"       , (pfunc)f_asc      ,1,1,{PL_STRING}},
 { F_CONST|F_PLISTE|F_NRET,    "ASIN"      , (pfunc)f_asin     ,1,1,{PL_CF}},
 { F_CONST|F_PLISTE|F_NRET,    "ASINH"     , (pfunc)f_asinh    ,1,1,{PL_CF}},
 { F_CONST|F_PLISTE|F_NRET,    "ATAN"      , (pfunc)f_atan     ,1,1,{PL_CF}},
 { F_CONST|F_DQUICK|F_DRET,    "ATAN2"     , atan2             ,2,2,{PL_FLOAT,PL_FLOAT}},
 { F_CONST|F_PLISTE|F_NRET,    "ATANH"     , (pfunc)f_atanh    ,1,1,{PL_CF}},
 { F_CONST|F_PLISTE|F_NRET,    "ATN"       , (pfunc)f_atan     ,1,1,{PL_CF}},

 { F_CONST|F_PLISTE|F_ANYIRET, "BCHG"      , (pfunc)f_bchg     ,2,2,{PL_NUMBER,PL_INT}},
 { F_CONST|F_PLISTE|F_ANYIRET, "BCLR"      , (pfunc)f_bclr     ,2,2,{PL_NUMBER,PL_INT}},
 { F_CONST|F_PLISTE|F_ANYIRET, "BSET"      , (pfunc)f_bset     ,2,2,{PL_NUMBER,PL_INT}},
 { F_CONST|F_PLISTE|F_IRET,    "BTST"      , (pfunc)f_btst     ,2,2,{PL_NUMBER,PL_INT}},
 { F_CONST|F_IQUICK|F_IRET,    "BYTE"      , (pfunc)f_byte     ,1,1,{PL_INT}},

 { F_PLISTE|F_IRET,            "CALL"      , (pfunc)f_call     ,1,-1,{PL_INT,PL_EVAL}},
 { F_CONST|F_IQUICK|F_IRET,    "CARD"      , (pfunc)f_card     ,1,1,{PL_INT}},
#ifdef ATARI
 { F_CONST|F_DQUICK|F_DRET,    "CBRT"      , ceil              ,1,1,{PL_FLOAT}},
#else
 { F_CONST|F_DQUICK|F_DRET,    "CBRT"      , cbrt              ,1,1,{PL_FLOAT}},
#endif
 { F_CONST|F_DQUICK|F_DRET,    "CEIL"      , ceil              ,1,1,{PL_FLOAT}},
 { F_CONST|F_DQUICK|F_IRET,    "CINT"      , (pfunc) f_cint    ,1,1,{PL_FLOAT}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,            "COLOR_RGB" , (pfunc)f_color_rgb,3,4,{PL_FLOAT,PL_FLOAT,PL_FLOAT,PL_FLOAT}},
#endif
 { F_CONST|F_PLISTE|F_AIRET,   "COMBIN"    , (pfunc)f_combin   ,2,2,{PL_INT,PL_INT}},
#ifdef WINDOWS
 { F_CONST|F_CQUICK|F_CRET,    "CONJ"     , (pfunc)myconj      ,1,1,{PL_COMPLEX}}, 
#elif !(defined HAVE_CACOS)
 { F_CONST|F_CQUICK|F_CRET,    "CONJ"     , (pfunc)myconj      ,1,1,{PL_COMPLEX}}, 
#else
 { F_CONST|F_CQUICK|F_CRET,    "CONJ"     , (pfunc)conj        ,1,1,{PL_COMPLEX}}, 
#endif
 { F_CONST|F_PLISTE|F_NRET,    "COS"       , (pfunc) f_cos     ,1,1,{PL_CF}},
 { F_CONST|F_PLISTE|F_NRET,    "COSH"      , (pfunc) f_cosh    ,1,1,{PL_CF}},
 { F_CONST|F_PLISTE|F_IRET,    "CRC"       , (pfunc) f_crc     ,1,2,{PL_STRING, PL_INT}},
#ifdef CONTROL
 { F_SQUICK|F_DRET,            "CSGET"     , f_csget           ,1,1,{PL_STRING}},
 { F_SQUICK|F_DRET,            "CSMAX"     , f_csmax           ,1,1,{PL_STRING}},
 { F_SQUICK|F_DRET,            "CSMIN"     , f_csmin           ,1,1,{PL_STRING}},
 { F_SQUICK|F_IRET,            "CSPID"     , (pfunc)f_cspid    ,1,1,{PL_STRING}},
 { F_SQUICK|F_DRET,            "CSRES"     , f_csres           ,1,1,{PL_STRING}},
 { F_SQUICK|F_IRET,            "CSSIZE"    , (pfunc)f_cssize   ,1,1,{PL_STRING}},
#endif
 { F_CONST|F_SQUICK|F_DRET,    "CVD"       , f_cvd             ,1,1,{PL_STRING}},
 { F_CONST|F_SQUICK|F_DRET,    "CVF"       , f_cvf             ,1,1,{PL_STRING}},
 { F_CONST|F_SQUICK|F_IRET,    "CVI"       , (pfunc)f_cvi      ,1,1,{PL_STRING}},
 { F_CONST|F_SQUICK|F_IRET,    "CVL"       , (pfunc)f_cvl      ,1,1,{PL_STRING}},
 { F_CONST|F_SQUICK|F_IRET,    "CVS"       , (pfunc)f_cvf      ,1,1,{PL_STRING}},

 { F_CONST|F_DQUICK|F_DRET,    "DEG"       , f_deg             ,1,1,{PL_FLOAT}},
 { F_CONST|F_PLISTE|F_NRET,    "DET"       , (pfunc)f_det      ,1,1,{PL_NARRAY}},
 { F_SQUICK|F_IRET,            "DEVICE"    , (pfunc)f_device   ,1,1,{PL_STRING}},
 { F_CONST|F_PLISTE|F_IRET,    "DIM?"      , (pfunc)f_dimf     ,1,1,{PL_ARRAYVAR}},
 { F_CONST|F_PLISTE|F_NRET,    "DIV"       , (pfunc)f_div      ,2,2,{PL_NUMBER,PL_NUMBER}},
#ifdef DOOCS
 { F_SQUICK|F_DRET,            "DOOCSGET"  , f_doocsget        ,1,1,{PL_STRING}},
 { F_SQUICK|F_IRET,            "DOOCSSIZE" , (pfunc) f_doocssize,1,1,{PL_STRING}},
 { F_SQUICK|F_DRET,            "DOOCSTIMESTAMP", f_doocstimestamp,1,1,{PL_STRING}},
 { F_SQUICK|F_IRET,            "DOOCSTYP"  , (pfunc)f_doocstyp ,1,1,{PL_STRING}},
#endif
 { F_IQUICK|F_IRET,            "DPEEK"     , (pfunc)f_dpeek    ,1,1,{PL_INT}},

 { F_PLISTE|F_IRET,            "EOF"       , (pfunc) f_eof     ,1,1,{PL_FILENR}},
 { F_CONST|F_SQUICK|F_DRET,    "EVAL"      , f_eval            ,1,1,{PL_STRING}},
 { F_CONST|F_PLISTE|F_IRET,    "EVEN"      , (pfunc)f_even     ,1,1,{PL_NUMBER}},
#ifndef NOGRAPHICS
 { F_IQUICK|F_IRET,            "EVENT?"    , (pfunc)f_eventf   ,1,1,{PL_INT}},
#endif
 { F_ARGUMENT|F_IRET,          "EXEC"      , (pfunc)f_exec     ,1,2,{PL_FLOAT,PL_FLOAT}},
 { F_SQUICK|F_IRET,            "EXIST"     , (pfunc)f_exist    ,1,1,{PL_STRING}},
 { F_CONST|F_PLISTE|F_NRET,    "EXP"       , (pfunc)f_exp      ,1,1,{PL_CF}},
#ifdef ATARI
 { F_CONST|F_DQUICK|F_DRET,    "EXPM1"     , exp               ,1,1,{PL_FLOAT}},
#else
 { F_CONST|F_DQUICK|F_DRET,    "EXPM1"     , expm1             ,1,1,{PL_FLOAT}},
#endif
 { F_CONST|F_PLISTE|F_AIRET,   "FACT"      , (pfunc)f_fak      ,1,1,{PL_INT}},
#ifdef ATARI
 { F_CONST|F_DQUICK|F_DRET,    "FIX"       , ceil              ,1,1,{PL_FLOAT}},
#else
 { F_CONST|F_DQUICK|F_DRET,    "FIX"       , trunc             ,1,1,{PL_FLOAT}},
#endif
 { F_CONST|F_DQUICK|F_DRET,    "FLOOR"     , floor             ,1,1,{PL_FLOAT}},
#ifdef HAVE_FORK
 { F_SIMPLE|F_IRET,            "FORK"      , (pfunc)fork       ,0,0     },
#else
 { F_SIMPLE|F_IRET,            "FORK"      , (pfunc)f_nop      ,0,0     },
#endif
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,            "FORM_ALERT", (pfunc)f_form_alert,2,2,{PL_INT,PL_STRING}},
 { F_PLISTE|F_IRET,            "FORM_CENTER", (pfunc)f_form_center,1,5,{PL_INT,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { F_PLISTE|F_IRET,            "FORM_DIAL" , (pfunc)f_form_dial,9,9,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,            "FORM_DO"   , (pfunc)f_form_do  ,1,2,{PL_INT,PL_INT}},
#endif
 { F_CONST|F_DQUICK|F_DRET,    "FRAC"      , f_frac            ,1,1,{PL_FLOAT}},
 { F_SIMPLE|F_IRET,            "FREEFILE"  , (pfunc)f_freefile ,0,0  },

#ifdef ATARI
 { F_CONST|F_DQUICK|F_DRET,    "GAMMA"     , sin               ,1,1,{PL_FLOAT}},
#else
 { F_CONST|F_DQUICK|F_DRET,    "GAMMA"     , tgamma            ,1,1,{PL_FLOAT}},
#endif
 { F_DQUICK|F_DRET,            "GASDEV"    , f_gasdev          ,0,1,{PL_FLOAT}},
 { F_CONST|F_PLISTE|F_AIRET,   "GCD"       , (pfunc)f_gcd      ,2,2,{PL_ARBINT,PL_ARBINT}},

#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,            "GET_COLOR" , (pfunc)f_get_color,3,3,{PL_INT,PL_INT,PL_INT}},
#endif
 { F_CONST|F_PLISTE|F_IRET,    "GLOB"      , (pfunc) f_glob    ,2,3,{PL_STRING,PL_STRING,PL_INT}},
 { F_CONST|F_IQUICK|F_IRET,    "GRAY"      , (pfunc) f_gray    ,1,1,{PL_INT}},

 { F_CONST|F_DQUICK|F_DRET,    "HYPOT"     , hypot             ,2,2,{PL_FLOAT,PL_FLOAT}},

 { F_CONST|F_CQUICK|F_DRET,    "IMAG"      , cimag             ,1,1,{PL_COMPLEX}},
 { F_SQUICK|F_IRET,            "INODE"     , (pfunc) f_inode   ,1,1,{PL_STRING}},
 { F_PLISTE|F_IRET,            "INP"       , (pfunc)inp8       ,1,1,{PL_FILENR}},
 { F_PLISTE|F_IRET,            "INP%"      , (pfunc)inp32      ,1,1,{PL_FILENR}},
 { F_PLISTE|F_IRET,            "INP&"      , (pfunc)inp16      ,1,1,{PL_FILENR}},
 { F_PLISTE|F_IRET,            "INP?"      , (pfunc) inpf      ,1,1,{PL_FILENR}},
 { F_CONST|F_PLISTE|F_IRET,    "INSTR"     , (pfunc) f_instr   ,2,3,{PL_STRING,PL_STRING,PL_INT}},
 { F_CONST|F_DQUICK|F_IRET,    "INT"       , (pfunc) f_int     ,1,1,{PL_FLOAT}},
 { F_CONST|F_PLISTE|F_AIRET,   "INVERT"    , (pfunc) f_invert  ,2,2,{PL_ARBINT,PL_ARBINT}},
 { F_PLISTE|F_IRET,            "IOCTL"     , (pfunc) f_ioctl   ,2,3,{PL_FILENR,PL_INT,PL_INT}},

 { F_CONST|F_SQUICK|F_IRET,    "JULIAN"    , (pfunc) f_julian  ,1,1,{PL_STRING}},

 { F_CONST|F_PLISTE|F_AIRET,   "LCM"       , (pfunc)f_lcm      ,2,2,{PL_ARBINT,PL_ARBINT}},
 { F_CONST|F_SQUICK|F_IRET,    "LEN"       ,(pfunc) f_len      ,1,1,{PL_STRING}},
#ifdef ATARI
 { F_CONST|F_DQUICK|F_DRET,    "LGAMMA"    , sin               ,1,1,{PL_FLOAT}},
#else
 { F_CONST|F_DQUICK|F_DRET,    "LGAMMA"    , lgamma            ,1,1,{PL_FLOAT}},
#endif
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,            "LISTSELECT", (pfunc)f_listselect ,2,3,{PL_STRING,PL_SARRAY,PL_INT}},
#endif
 { F_CONST|F_PLISTE|F_NRET,    "LN"        , (pfunc)f_log      ,1,1,{PL_CF}},
 { F_PLISTE|F_IRET,            "LOC"       , (pfunc)f_loc      ,1,1,{PL_FILENR}},
 { F_PLISTE|F_IRET,            "LOF"       , (pfunc)f_lof      ,1,1,{PL_FILENR}},
 { F_CONST|F_PLISTE|F_NRET,    "LOG"       , (pfunc)f_log      ,1,1,{PL_CF}},
 { F_CONST|F_PLISTE|F_NRET,    "LOG10"     , (pfunc)f_log10    ,1,1,{PL_CF}},
#ifdef ATARI
 { F_CONST|F_DQUICK|F_DRET,    "LOG1P"     , log               ,1,1,{PL_FLOAT}},
 { F_CONST|F_DQUICK|F_DRET,    "LOGB"      , log               ,1,1,{PL_FLOAT}},
#else
 { F_CONST|F_DQUICK|F_DRET,    "LOG1P"     , log1p             ,1,1,{PL_FLOAT}},
 { F_CONST|F_DQUICK|F_DRET,    "LOGB"      , logb              ,1,1,{PL_FLOAT}},
#endif
 { F_IQUICK|F_IRET,            "LPEEK"     , (pfunc)f_lpeek    ,1,1,{PL_INT}},
 { F_SQUICK|F_DRET,            "LTEXTLEN"  , f_ltextlen        ,1,1,{PL_STRING}},

 { F_IQUICK|F_IRET,            "MALLOC"    ,(pfunc) f_malloc   ,1,1,{PL_INT}},
 { F_CONST|F_PLISTE|F_NRET,    "MAX"       , (pfunc)f_max      ,1,-1,{PL_NUMBER,PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_PLISTE|F_NRET,    "MIN"       , (pfunc)f_min      ,1,-1,{PL_NUMBER,PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_PLISTE|F_NRET,    "MOD"       , (pfunc)f_mod      ,2,2,{PL_NUMBER,PL_NUMBER}},
 { F_SQUICK|F_IRET,            "MODE"      , (pfunc)f_mode     ,1,1,{PL_STRING}},
 { F_IQUICK|F_IRET,            "MSHRINK"   , (pfunc)f_mshrink  ,2,2,{PL_INT,PL_INT}},
 { F_CONST|F_PLISTE|F_NRET,    "MUL"       , (pfunc)f_mul      ,2,2,{PL_NUMBER,PL_NUMBER}},
 
 { F_CONST|F_PLISTE|F_AIRET,   "NEXTPRIME" , (pfunc)f_nextprime,1,1,{PL_ARBINT}},
 { F_SQUICK|F_IRET,            "NLINK"     , (pfunc)f_nlink    ,1,1,{PL_STRING}},
 { F_CONST|F_PLISTE|F_ANYIRET, "NOT"       , (pfunc)f_not      ,1,1,{PL_NUMBER}},

#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,            "OBJC_DRAW" , (pfunc)f_objc_draw,7,7,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,            "OBJC_FIND" , (pfunc)f_objc_find,3,3,{PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,            "OBJC_OFFSET",(pfunc)f_objc_offset,4,4,{PL_INT,PL_INT,PL_NVAR,PL_NVAR}},
#endif
 { F_CONST|F_PLISTE|F_IRET,    "ODD"       , (pfunc) f_odd     ,1,1,{PL_NUMBER}},
 { F_CONST|F_PLISTE|F_ANYIRET, "OR"        , (pfunc) f_or      ,2,2,{PL_NUMBER,PL_NUMBER}},

 { F_IQUICK|F_IRET,            "PEEK"      , (pfunc) f_peek    ,1,1,{PL_INT}},
#ifndef NOGRAPHICS
 { F_DQUICK|F_IRET,            "POINT"     , (pfunc)f_point    ,2,2,{PL_FLOAT, PL_FLOAT }},
#endif
 { F_CONST|F_PLISTE|F_AIRET,   "POWM"      , (pfunc) f_powm    ,3,3,{PL_ARBINT,PL_ARBINT,PL_ARBINT}},
 { F_CONST|F_DQUICK|F_DRET,    "PRED"      , f_pred            ,1,1,{PL_FLOAT}},
#ifndef NOGRAPHICS
 { F_DQUICK|F_IRET,            "PTST"      , (pfunc)f_point    ,2,2,{PL_FLOAT, PL_FLOAT }},
#endif

 { F_CONST|F_DQUICK|F_DRET,    "RAD"      , f_rad ,1,1     ,{PL_FLOAT}},
 { F_DQUICK|F_IRET,    "RAND"      ,(pfunc) rand ,1,1     ,{PL_FLOAT}},
 { F_PLISTE|F_NRET,    "RANDOM"    , (pfunc)f_random ,1,1     ,{PL_NUMBER}},

 { F_CONST|F_CQUICK|F_DRET,    "REAL"    , creal ,1,1     ,{PL_COMPLEX}},
 { F_IQUICK|F_IRET,    "REALLOC"    , (pfunc)f_realloc ,2,2     ,{PL_INT,PL_INT}},
 { F_CONST|F_PLISTE|F_IRET,    "RINSTR"    , (pfunc)f_rinstr ,2,3  ,{PL_STRING,PL_STRING,PL_INT}},
 { F_DQUICK|F_DRET,    "RND"       , f_rnd ,0,1     ,{PL_FLOAT}},
 { F_CONST|F_PLISTE|F_IRET,    "ROL"      , (pfunc)f_rol,2,3     ,{PL_INT,PL_INT,PL_INT}},
 { F_CONST|F_PLISTE|F_AIRET,   "ROOT"      , (pfunc)f_root      ,2,2,{PL_ARBINT,PL_INT}},
 { F_CONST|F_PLISTE|F_IRET,    "ROR"      , (pfunc)f_ror,2,3     ,{PL_INT,PL_INT,PL_INT}},
 { F_CONST|F_PLISTE|F_DRET,    "ROUND"     , f_round ,1,2   ,{PL_FLOAT,PL_INT}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "RSRC_GADDR", (pfunc)f_rsrc_gaddr ,2,2   ,{PL_INT,PL_INT}},
#endif

 { F_IQUICK|F_DRET,    "SENSOR"       ,(pfunc) f_sensor ,1,1     ,{PL_INT}},
 { F_CONST|F_DQUICK|F_IRET,    "SGN"       , (pfunc)f_sgn ,1,1     ,{PL_FLOAT}},
 { F_CONST|F_IQUICK|F_IRET,    "SHL"      , (pfunc)f_shl,2,2     ,{PL_INT,PL_INT}},
 { F_IQUICK|F_IRET,    "SHM_ATTACH"    , (pfunc)shm_attach ,1,1     ,{PL_INT}},
 { F_IQUICK|F_IRET,    "SHM_MALLOC"    , (pfunc)shm_malloc ,2,2     ,{PL_INT,PL_INT}},
 { F_CONST|F_IQUICK|F_IRET,    "SHR"      , (pfunc)f_shr,2,2     ,{PL_INT,PL_INT}},
 { F_CONST|F_PLISTE|F_NRET,    "SIN"       , (pfunc) f_sin ,1,1     ,{PL_CF}},
 { F_CONST|F_PLISTE|F_NRET,    "SINH"      , (pfunc) f_sinh ,1,1     ,{PL_CF}},
 { F_SQUICK|F_IRET,            "SIZE"     , (pfunc)f_size,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE|F_NRET,    "SQR"       , (pfunc)f_sqrt ,1,1,{PL_CFAI}},
 { F_CONST|F_PLISTE|F_NRET,    "SQRT"      , (pfunc)f_sqrt ,1,1,{PL_CFAI}},
 { F_DQUICK|F_IRET,            "SRAND"     ,(pfunc) f_srand ,1,1     ,{PL_FLOAT}},
 { F_CONST|F_PLISTE|F_NRET,    "SUB"     , (pfunc)f_sub ,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_DQUICK|F_IRET,    "SUCC"      ,(pfunc) f_succ ,1,1     ,{PL_FLOAT}},
 { F_CONST|F_IQUICK|F_IRET,    "SWAP"     , (pfunc)f_swap ,1,1     ,{PL_INT}},
 { F_PLISTE|F_IRET,            "SYM_ADR"   ,(pfunc) f_symadr ,2,2   ,{PL_FILENR,PL_STRING}},

 { F_CONST|F_PLISTE|F_IRET,    "TALLY"     ,(pfunc) f_tally ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},
 { F_CONST|F_PLISTE|F_NRET,    "TAN"       , (pfunc) f_tan ,1,1     ,{PL_CF}},
 { F_CONST|F_PLISTE|F_NRET,    "TANH"      , (pfunc) f_tanh ,1,1     ,{PL_CF}},

#ifdef TINE
 { F_SQUICK|F_DRET,  "TINEGET"     , f_tineget ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "TINEMAX"     , f_tinemax ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "TINEMIN"     , f_tinemin ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "TINESIZE"    , (pfunc)f_tinesize ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "TINETYP"    , (pfunc)f_tinetyp ,1,1   ,{PL_STRING}},
#endif
#ifdef ATARI
 { F_CONST|F_DQUICK|F_DRET, "TRUNC"     , ceil ,1,1     ,{PL_FLOAT}},
#else
 { F_CONST|F_DQUICK|F_DRET, "TRUNC"     , trunc ,1,1     ,{PL_FLOAT}},
#endif
 { F_ARGUMENT|F_IRET,       "TYP?"       , (pfunc)type ,1,1     ,{PL_ALLVAR}},

 { F_PLISTE|F_IRET,         "UBOUND"    , (pfunc)f_ubound     ,1,2,{PL_ARRAYVAR, PL_INT}},


 { F_CONST|F_SQUICK|F_DRET, "VAL"       , f_val ,1,1     ,{PL_STRING}},
 { F_CONST|F_SQUICK|F_IRET, "VAL?"       ,(pfunc) f_valf ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE|F_IRET, "VARIAT"    , (pfunc)f_variat ,2,2     ,{PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,         "VARPTR"    , (pfunc)f_varptr ,1,1     ,{PL_ALLVAR}},

 { F_CONST|F_IQUICK|F_IRET, "WORD"     , (pfunc)f_word ,1,1     ,{PL_INT}},
 { F_PLISTE|F_IRET,         "WORT_SEP" , (pfunc)f_wort_sep ,3,5 ,{PL_STRING,PL_STRING,PL_INT,PL_SVAR,PL_SVAR}},

 { F_CONST|F_PLISTE|F_ANYIRET,"XOR"      , (pfunc) f_xor,2,2     ,{PL_NUMBER,PL_NUMBER}},

};
const int anzpfuncs=sizeof(pfuncs)/sizeof(FUNCTION);
