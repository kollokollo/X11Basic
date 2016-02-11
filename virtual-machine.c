/* VIRTUAL-MACHINE.C (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif
#include "defs.h"
#include "x11basic.h"
#include "xbasic.h"
#include "parser.h"
#include "variablen.h"
#include "parameter.h"
#include "functions.h"
#include "number.h"
#include "bytecode.h"
#include "array.h"
#include "io.h"
#include "virtual-machine.h"

extern int datapointer;



/* Aus Geschwindigkeitsgruenden kann der verbose mode abgeschaltet werden, wenn er keinen sinn
macht (z.B. beim Android) */
// #define USE_VERBODE

#ifdef USE_VERBOSE
extern int verbose;
#define  VERBOSE(...)  if(verbose) printf(__VA_ARGS__)
#else 
#define  VERBOSE(...) 
#endif

#define TYPEMISMATCH(...) printf("ERROR: type mismatch for %s.\n",__VA_ARGS__)
#define VMERROR(a,...) {printf("VM-ERROR: " a "\n",##__VA_ARGS__);batch=0;}

/*for every platform where no real compiler exists define some functions
  static --> higher performance */

#if defined ANDROID || defined WINDOWS
#define ISTATIC inline static
#define STATIC static
#else
#define ISTATIC
#define STATIC
#endif


char *rodata=NULL;




#define vm_x2f(a) cast_to_real(a-1)
#define vm_x2c(a) cast_to_complex(a-1)
#define vm_x2i(a) cast_to_int(a-1)
#define vm_x2ai(a) cast_to_arbint(a-1)

STATIC int vm_add(PARAMETER *sp) {    /* binaer addition */
  VERBOSE("vm_add ");
  sp--;
  int rt=(PL_CONSTGROUP|combine_type(sp[-1].typ&PL_BASEMASK,sp[0].typ&PL_BASEMASK,'+'));
  cast_to_x(sp,rt);
  cast_to_x(sp-1,rt);
  switch(rt) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     sp[-1].integer+=sp->integer; break;
  case PL_COMPLEX: sp[-1].imag+=sp->imag;
  case PL_FLOAT:   sp[-1].real+=sp->real;       break;
  case PL_ARBINT:  mpz_add(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)(sp-1)->pointer,*(ARBINT *)sp->pointer);  break;
  case PL_STRING: {
    STRING a;
    a.pointer=sp[-1].pointer;
    a.len=sp[-1].integer;
    sp[-1].pointer=malloc(a.len+sp->integer+1);
    sp[-1].integer=a.len+sp->integer;
    memcpy(sp[-1].pointer,a.pointer,a.len);
    memcpy(sp[-1].pointer+a.len,sp->pointer,sp->integer);
    free(a.pointer);
    }
    break;
  case PL_ARRAY: {
    ARRAY a;
    a=*((ARRAY *)&(sp[-1].integer));
    array_add(a,*((ARRAY *)&(sp->integer)));
    *((ARRAY *)&(sp[-1].integer))=a;
    }
    break;
  default: TYPEMISMATCH("ADD");
  }
  free_parameter(sp);
  return(-1);
}
STATIC int vm_and(PARAMETER *sp) {    /* binaer addition */
  VERBOSE("vm_and ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT) {
    sp[-1].integer&=sp->integer;
  } else {
    cast_to_arbint(sp);
    cast_to_arbint(sp-1);
    mpz_and(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)(sp-1)->pointer,*(ARBINT *)sp->pointer);
  }
  free_parameter(sp);
  return(-1);
}
STATIC int vm_or(PARAMETER *sp) {    /* binaer addition */
  VERBOSE("vm_or ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT) {
    sp[-1].integer|=sp->integer;
  } else {
    cast_to_arbint(sp);
    cast_to_arbint(sp-1);
    mpz_ior(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)(sp-1)->pointer,*(ARBINT *)sp->pointer);
  }
  free_parameter(sp);
  return(-1);
}
STATIC int vm_xor(PARAMETER *sp) {    /* binaer addition */
  VERBOSE("vm_xor ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT) {
    sp[-1].integer^=sp->integer;
  } else {
    cast_to_arbint(sp);
    cast_to_arbint(sp-1);
    mpz_eor(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)(sp-1)->pointer,*(ARBINT *)sp->pointer);
  }
  free_parameter(sp);
  return(-1);
}
STATIC int vm_not(PARAMETER *sp) {    /* binaer addition */
  VERBOSE("vm_not ");
  if(sp[-1].typ==PL_INT) {
    sp[-1].integer=~(sp[-1].integer);
  } else {
    cast_to_arbint(sp-1);
    mpz_com(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)(sp-1)->pointer);
  }
  return(0);
}
STATIC int vm_sub(PARAMETER *sp) {    /* binaer subtraktion */
  VERBOSE("vm_sub ");
  sp--;
  int rt=(PL_CONSTGROUP|combine_type(sp[-1].typ&PL_BASEMASK,sp[0].typ&PL_BASEMASK,'+'));
  cast_to_x(sp,rt);
  cast_to_x(sp-1,rt);
  switch(rt) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     sp[-1].integer-=sp->integer; break;
  case PL_COMPLEX: sp[-1].imag-=sp->imag;
  case PL_FLOAT:   sp[-1].real-=sp->real;       break;
  case PL_ARBINT:  mpz_sub(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)(sp-1)->pointer,*(ARBINT *)sp->pointer);  break;
  case PL_ARRAY: {
    ARRAY a;
    a=*((ARRAY *)&(sp[-1].integer));
    array_sub(a,*((ARRAY *)&(sp->integer)));
    *((ARRAY *)&(sp[-1].integer))=a;
    }
    break;
  default: TYPEMISMATCH("SUB");
  }
  free_parameter(sp);
  return(-1);
}
STATIC int vm_mul(PARAMETER *sp) {    /* binaer multiplikation */
  VERBOSE("vm_mul ");
  sp--;
  int rt=(PL_CONSTGROUP|combine_type(sp[-1].typ&PL_BASEMASK,sp[0].typ&PL_BASEMASK,'+'));
  cast_to_x(sp,rt);
  cast_to_x(sp-1,rt);
  switch(rt) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     sp[-1].integer*=sp->integer; break;
  case PL_COMPLEX: *((COMPLEX *)&(sp[-1].real))=complex_mul(*((COMPLEX *)&(sp[-1].real)),*((COMPLEX *)&(sp[0].real)));break;
  case PL_FLOAT:   sp[-1].real*=sp->real;       break;
  case PL_ARBINT:  mpz_mul(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)(sp-1)->pointer,*(ARBINT *)sp->pointer);  break;
  case PL_ARRAY: {
    ARRAY a;
    a=*((ARRAY *)&(sp[-1].integer));
    mul_array(a,*((ARRAY *)&(sp->integer)));
    *((ARRAY *)&(sp[-1].integer))=a;
    }
    break;
  default: TYPEMISMATCH("MUL");
  }
  free_parameter(sp);
  return(-1);
}

/*  Hier kann man bei den arbint operationen noch optimieren: 
    der zweite Operand darf auch long sein....*/


STATIC int vm_pow(PARAMETER *sp) {    /* binaer potenzieren */
  VERBOSE("vm_pow ");
  sp--;
  int rt=(PL_CONSTGROUP|combine_type(sp[-1].typ&PL_BASEMASK,sp[0].typ&PL_BASEMASK,'+'));
  if(rt==PL_INT) rt=PL_FLOAT;  /*  Naja ...*/
  if(rt==PL_ARBINT) cast_to_x(sp,PL_INT);  /*  Hm ...*/
  else cast_to_x(sp,rt);
  cast_to_x(sp-1,rt);
  switch(rt) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_COMPLEX: *((COMPLEX *)&(sp[-1].real))=complex_pow(*((COMPLEX *)&(sp[-1].real)),*((COMPLEX *)&(sp[0].real)));break;
  case PL_FLOAT:   sp[-1].real=pow((sp-1)->real,sp->real);       break;
  case PL_ARBINT:  mpz_pow_ui(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)(sp-1)->pointer,sp->integer);  break;
  default: TYPEMISMATCH("POW");
  }
  free_parameter(sp);
  return(-1);
}

STATIC int vm_div(PARAMETER *sp) {    /* binaer dividieren, ggf integer */
  VERBOSE("vm_div ");
  sp--;
  int rt=(PL_CONSTGROUP|combine_type(sp[-1].typ&PL_BASEMASK,sp[0].typ&PL_BASEMASK,'/'));
  if(rt==ARBFLOATTYP || rt==ARBCOMPLEXTYP) rt=ARBINTTYP;
  cast_to_x(sp,rt);
  cast_to_x(sp-1,rt);
  switch(rt) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_COMPLEX: *((COMPLEX *)&(sp[-1].real))=complex_div(*((COMPLEX *)&(sp[-1].real)),*((COMPLEX *)&(sp[0].real)));break;
  case PL_FLOAT:   sp[-1].real/=sp->real;       break;
  case PL_ARBINT:  mpz_div(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)(sp-1)->pointer,*(ARBINT *)sp->pointer);  break;
  default: TYPEMISMATCH("DIV");
  }
  free_parameter(sp);
  return(-1);
}




STATIC int vm_mod(PARAMETER *sp) {    /* binaer rest */
  VERBOSE("vm_mod ");
  sp--;
  int rt=(PL_CONSTGROUP|combine_type(sp[-1].typ&PL_BASEMASK,sp[0].typ&PL_BASEMASK,'+'));
  cast_to_x(sp,rt);
  cast_to_x(sp-1,rt);
  switch(rt) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     sp[-1].integer%=sp->integer; break;
  case PL_FLOAT:   sp[-1].real=fmod(sp[-1].real,sp->real);       break;
  case PL_ARBINT:  mpz_mod(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)(sp-1)->pointer,*(ARBINT *)sp->pointer);  break;
  default: TYPEMISMATCH("MOD");
  }
  free_parameter(sp);
  return(-1);
}


STATIC int vm_equal(PARAMETER *sp) {    /* binaer vergleich */
  VERBOSE("vm_equal ");
  sp--;
  if(sp->typ!=(sp-1)->typ) {
    int rt=(PL_CONSTGROUP|combine_type(sp[-1].typ&PL_BASEMASK,sp[0].typ&PL_BASEMASK,'+'));
    if(rt==PL_LEER) {  /*  Wenn die Typen inkompatibel sind, dann ungleich!*/
      free_parameter(sp);
      free_parameter(sp-1);
      (sp-1)->integer=0;
      (sp-1)->typ=PL_INT;
      return(-1);
    }
    cast_to_x(sp,rt);
    cast_to_x(sp-1,rt);
  }
  switch(sp->typ) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     sp[-1].integer=(sp[-1].integer==sp[0].integer)?-1:0; break;
  case PL_COMPLEX: sp[-1].integer=(sp[-1].real==sp[0].real && sp[-1].imag==sp[0].imag)?-1:0; break;
  case PL_FLOAT:   sp[-1].integer=(sp[-1].real==sp[0].real)?-1:0;  break;
  case PL_ARBINT:  sp[-1].integer=(mpz_cmp(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)sp->pointer)==0)?-1:0;  break;
  case PL_STRING: {
    int v=(sp[-1].integer-sp[0].integer);
    if(v==0) v=memcmp(sp[-1].pointer,sp[0].pointer,sp[-1].integer);
    sp[-1].integer=v?0:-1;
    }
    break;
  case PL_ARRAY:    /*   Array compare haben wir noch nicht .....*/
    
    xberror(9,"Compare ARRAY"); /*Function or command %s not implemented*/
    break;
  default: TYPEMISMATCH("EQUAL");
  }
  free_parameter(sp);
  free_parameter(sp-1);
  (sp-1)->typ=PL_INT;
  return(-1);
}



STATIC int vm_greater(PARAMETER *sp) {    /* binaer groesser vergleich */
  VERBOSE("vm_greater ");
  sp--;
  if(sp->typ!=(sp-1)->typ) {
    int rt=(PL_CONSTGROUP|combine_type(sp[-1].typ&PL_BASEMASK,sp[0].typ&PL_BASEMASK,'+'));
    cast_to_x(sp,rt);
    cast_to_x(sp-1,rt);
  }
  switch(sp->typ) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     sp[-1].integer=(sp[-1].integer>sp[0].integer)?-1:0; break;
  case PL_COMPLEX: sp[-1].integer=(hypot(sp[-1].real,sp[-1].imag)>hypot(sp[0].real,sp[0].imag))?-1:0; break;
  case PL_FLOAT:   sp[-1].integer=(sp[-1].real>sp[0].real)?-1:0;  break;
  case PL_ARBINT:  sp[-1].integer=(mpz_cmp(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)sp->pointer)>0)?-1:0;  break;
  case PL_STRING: {
    int v=memcmp(sp[-1].pointer,sp[0].pointer,min(sp[-1].integer,sp->integer)+1);
    sp[-1].integer=(v>0)?-1:0;
    }
    break;
  case PL_ARRAY:    /*   Array compare haben wir noch nicht .....*/
    
    xberror(9,"Compare ARRAY"); /*Function or command %s not implemented*/
    break;
  default: TYPEMISMATCH("GREATER");
  }
  free_parameter(sp);
  free_parameter(sp-1);
  (sp-1)->typ=PL_INT;
  return(-1);
}


STATIC int vm_less(PARAMETER *sp) {    /* binaer kleiner vergleich */
  VERBOSE("vm_less ");
  sp--;
  if(sp->typ!=(sp-1)->typ) {
    int rt=(PL_CONSTGROUP|combine_type(sp[-1].typ&PL_BASEMASK,sp[0].typ&PL_BASEMASK,'+'));
    cast_to_x(sp,rt);
    cast_to_x(sp-1,rt);
  }
  switch(sp->typ) {   /* Jetzt gibt es nur noch zwei gleiche typen.*/
  case PL_INT:     sp[-1].integer=(sp[-1].integer<sp[0].integer)?-1:0; break;
  case PL_COMPLEX: sp[-1].integer=(hypot(sp[-1].real,sp[-1].imag)<hypot(sp[0].real,sp[0].imag))?-1:0; break;
  case PL_FLOAT:   sp[-1].integer=(sp[-1].real<sp[0].real)?-1:0;  break;
  case PL_ARBINT:  sp[-1].integer=(mpz_cmp(*(ARBINT *)(sp-1)->pointer,*(ARBINT *)sp->pointer)<0)?-1:0;  break;
  case PL_STRING: {
    int v=memcmp(sp[-1].pointer,sp[0].pointer,min(sp[-1].integer,sp->integer)+1);
    sp[-1].integer=(v<0)?-1:0;
    }
    break;
  case PL_ARRAY:    /*   Array compare haben wir noch nicht .....*/
    
    xberror(9,"Compare ARRAY"); /*Function or command %s not implemented*/
    break;
  default: TYPEMISMATCH("GREATER");
  }
  free_parameter(sp);
  free_parameter(sp-1);
  (sp-1)->typ=PL_INT;
  return(-1);
}

STATIC int vm_sysvar(PARAMETER *sp,int n) {    /*  */
  VERBOSE("vm_%s ",sysvars[n].name);
  int typ=sysvars[n].opcode&TYPMASK;
  if(typ==INTTYP) {
    sp->integer=((int (*)())sysvars[n].routine)();
    sp->typ=PL_INT;
  } else if(typ==FLOATTYP) {
    sp->real=(sysvars[n].routine)();
    sp->typ=PL_FLOAT;
  } else {
    sp->real=0;
    sp->typ=PL_FLOAT;
  }
  return(1);
}
ISTATIC int vm_ssysvar(PARAMETER *sp,int n) {    /*  */
  VERBOSE("vm_%s ",syssvars[n].name);
  STRING a=(syssvars[n].routine)();
  sp->integer=a.len;
  sp->pointer=a.pointer;
  sp->typ=PL_STRING;
  return(1);
}
ISTATIC int vm_asysvar(PARAMETER *sp,int n) {    /*  */
  VERBOSE("vm_%s ",sysvars[n].name);
  VMERROR("#####Sysvar ARRAY: %s not implemented.",sysvars[n].name);
  xberror(9,"Sysvar ARRAY"); /*Function or command %s not implemented*/
  sp->real=4711;
  sp->typ=PL_FLOAT;
  return(1);
}
STATIC int vm_dup(PARAMETER *sp) {    /* dupliziere Parameter */
  VERBOSE("vm_dup ");
  sp[0]=sp[-1];
  switch(sp->typ) {
  case PL_KEY:
  case PL_STRING:
    sp[0].pointer=malloc(sp[0].integer+1);
    memcpy(sp[0].pointer,sp[-1].pointer,sp[0].integer+1);
    break;
  case PL_ARBINT:
    mpz_init(*(ARBINT *)sp->pointer);
    mpz_set(*(ARBINT *)sp->pointer,*(ARBINT *)(sp-1)->pointer);
    break;
  case PL_ARRAY:
    *(ARRAY *)&(sp->integer)=double_array((ARRAY *)&(sp[-1].integer));
  }
  return(1);
}
inline static int vm_exch(PARAMETER *sp) {    /*  */
  VERBOSE("vm_exch ");
  *sp=sp[-1];   
  sp[-1]=sp[-2];   
  sp[-2]=*sp;   
  return(0);
}
ISTATIC int vm_neg(PARAMETER *sp) {    /*  */
  VERBOSE("vm_neg ");
  switch(sp[-1].typ) {
  case PL_INT:          sp[-1].integer=-sp[-1].integer; break;
  case PL_COMPLEX:      sp[-1].imag=-sp[-1].imag;
  case PL_FLOAT:        sp[-1].real=-sp[-1].real; break;
  case PL_ARBINT:       mpz_neg(*(ARBINT *)sp[-1].pointer,*(ARBINT *)sp[-1].pointer);break;
  default: TYPEMISMATCH("NEG");
  }
  return(0);
}
STATIC int vm_sfunc(PARAMETER *sp,int i, int anzarg) {    /*  */
  VERBOSE("vm_%s(%d) ",psfuncs[i].name,anzarg);
  if(anzarg<psfuncs[i].pmin) {
    xberror(42,(char *)psfuncs[i].name); /* Zu wenig Parameter  */
    return 1-anzarg;
  }
  if(anzarg>psfuncs[i].pmax && !(psfuncs[i].pmax==-1)) {
    xberror(45,(char *)psfuncs[i].name); /* Zu viele Parameter  */
    return 1-anzarg;
  } 
  sp-=anzarg;
  if((psfuncs[i].opcode&FM_TYP)==F_SIMPLE || psfuncs[i].pmax==0) {
    STRING s=(psfuncs[i].routine)();
    sp[0].pointer=s.pointer;
    sp[0].integer=s.len;
    sp[0].typ=PL_STRING;
    return 1-anzarg;
  }  
  if((psfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
    if(sp->typ==PL_KEY) {
      STRING s=(psfuncs[i].routine)(sp->pointer);
      free(sp->pointer);
      sp->pointer=s.pointer;
      sp->integer=s.len;
      sp->typ=PL_STRING;
      return 1-anzarg;
    } else {
      VMERROR("SFUNC");
      return 1-anzarg;
    }
  }
  if((psfuncs[i].opcode&FM_TYP)==F_PLISTE) {
    PARAMETER *plist;
    STRING s;
    int e=make_pliste3(psfuncs[i].pmin,psfuncs[i].pmax,(unsigned short *)psfuncs[i].pliste,
                 &sp[0],&plist,anzarg);
    s=(psfuncs[i].routine)(plist,anzarg);
    if(e!=-1) free_pliste(e,plist);

    sp[0].pointer=s.pointer;
    sp[0].integer=s.len;
    sp[0].typ=PL_STRING;
    return 1-anzarg;
  }
  if((psfuncs[i].opcode&FM_TYP)==F_SQUICK) {
    STRING a;
    if(sp[0].typ==PL_STRING) {
      a.len=sp[0].integer;
      a.pointer=sp[0].pointer;
    } else 
      xberror(47,(char *)pfuncs[i].name); /*  Parameter %s falsch, kein String */
    
    STRING s=(psfuncs[i].routine)(a);
    free_parameter(sp);
    sp[0].pointer=s.pointer;
    sp[0].integer=s.len;
    sp[0].typ=PL_STRING;
    return 1-anzarg;
  }
  if((psfuncs[i].opcode&FM_TYP)==F_IQUICK) {
    int a=0;
    if(sp[0].typ==PL_INT)        a=sp[0].integer;
    else if(sp[0].typ==PL_FLOAT) a=(int)sp[0].real;
    else TYPEMISMATCH("Funktion");
    
    STRING s=(psfuncs[i].routine)(a);
    free_parameter(sp);
    sp[0].pointer=s.pointer;
    sp[0].integer=s.len;
    sp[0].typ=PL_STRING;
    return 1-anzarg;
  }

  VMERROR("INCOMPLETE"
       ", this function does not get its parameters.");
  return 1-anzarg;
}

STATIC int vm_func(PARAMETER *sp,int i, int anzarg) {    /*  */
  VERBOSE("vm_.%s(%d) ",pfuncs[i].name,anzarg);
  if(anzarg<pfuncs[i].pmin) {
    xberror(42,(char *)pfuncs[i].name); /* Zu wenig Parameter  */
    return 1-anzarg;
  }
  if((anzarg>pfuncs[i].pmax && !(pfuncs[i].pmax==-1))) {
    xberror(45,(char *)pfuncs[i].name); /* Zu viele Parameter  */
    return 1-anzarg;
  } 	      
  sp-=anzarg;
  if((pfuncs[i].opcode&FM_TYP)==F_SIMPLE || pfuncs[i].pmax==0) {
    switch(pfuncs[i].opcode&FM_RET) {
    case F_IRET: 
       sp->integer=((int (*)())pfuncs[i].routine)();
       sp->typ=PL_INT;
       break;
    case F_CRET:
       *((COMPLEX *)&(sp->real))=((COMPLEX (*)())pfuncs[i].routine)();
       sp->typ=PL_COMPLEX;
       break;
    case F_DRET:
       sp->real=(pfuncs[i].routine)();
       sp->typ=PL_FLOAT;
       break;
    case F_AIRET: 
       sp->pointer=malloc(sizeof(ARBINT));
       mpz_init(*(ARBINT *)sp->pointer);
       ((void (*)())pfuncs[i].routine)(*(ARBINT *)sp->pointer);
       sp->typ=PL_ARBINT;
       break;
    case F_ANYRET:
    case F_NRET:
    case F_ANYIRET:  {
       *sp=((ppfunc)(pfuncs[i].routine))();
       break;
     }
    default: xberror(13,"");  /* Type mismatch */
    }
    return 1-anzarg;
  } else if((pfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
    switch(pfuncs[i].opcode&FM_RET) {
    case F_IRET: 
       sp->integer=((int (*)())pfuncs[i].routine)(sp->pointer);
       sp->typ=PL_INT;
       free(sp->pointer);
       break;
    case F_CRET:
       *((COMPLEX *)&(sp->real))=((COMPLEX (*)())pfuncs[i].routine)(sp->pointer);
       sp->typ=PL_COMPLEX;
       free(sp->pointer);
       break;
    case F_DRET:
       sp->real=(pfuncs[i].routine)(sp->pointer);
       sp->typ=PL_FLOAT;
       free(sp->pointer);
       break;
    case F_AIRET: {
       ARBINT b;
       mpz_init(b);
       ((void (*)())pfuncs[i].routine)(b,sp->pointer);
       free_parameter(sp);
       sp->pointer=malloc(sizeof(ARBINT));
       mpz_init(*(ARBINT *)sp->pointer);
       mpz_set(*(ARBINT *)sp->pointer,b);
       mpz_clear(b);
       sp->typ=PL_ARBINT;
       }
       break;
    case F_ANYRET: 
    case F_NRET:
    case F_ANYIRET: {
       PARAMETER b=((ppfunc)(pfuncs[i].routine))(sp->pointer);
       free_parameter(sp);
       *sp=b;
       break;
     }
    default: xberror(13,"");  /* Type mismatch */
       free(sp->pointer);
    }
    return 1-anzarg;
  } else if((pfuncs[i].opcode&FM_TYP)==F_PLISTE) {
    PARAMETER *plist;
    PARAMETER rpar;
    int e=make_pliste3(pfuncs[i].pmin,pfuncs[i].pmax,(unsigned short *)pfuncs[i].pliste,sp,&plist,anzarg);
    switch(pfuncs[i].opcode&FM_RET) {
    case F_IRET: 
      rpar.integer=((int (*)())pfuncs[i].routine)(plist,anzarg);
      rpar.typ=PL_INT;
      break;
    case F_CRET:
      *((COMPLEX *)&(rpar.real))=((COMPLEX (*)())pfuncs[i].routine)(plist,anzarg);
      rpar.typ=PL_COMPLEX;
      break;
    case F_DRET:
      rpar.real=(pfuncs[i].routine)(plist,anzarg);
      rpar.typ=PL_FLOAT;
      break;
    case F_AIRET: 
       rpar.typ=PL_ARBINT;
       rpar.pointer=malloc(sizeof(ARBINT));
       mpz_init(*(ARBINT *)rpar.pointer);
       ((void (*)())pfuncs[i].routine)(*(ARBINT *)rpar.pointer,plist,anzarg);
       break;
    case F_ANYRET:
    case F_NRET:
    case F_ANYIRET:
       rpar=((ppfunc)(pfuncs[i].routine))(plist,anzarg);
       break;
    default: xberror(13,"");  /* Type mismatch */
    }
    if(e!=-1) free_pliste(e,plist);
    e=anzarg;
    while(--e>=0) free_parameter(&sp[e]);
    sp[0]=rpar;
    return 1-anzarg;
  }
 
  if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_DQUICK) {
    if(anzarg>0) cast_to_real(&sp[0]);
    if((pfuncs[i].opcode&FM_RET)==F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(sp[0].real);
      sp[0].typ=PL_INT;
    } else if((pfuncs[i].opcode&FM_RET)==F_CRET) {
      *((COMPLEX *)&(sp[0].real))=((COMPLEX (*)())pfuncs[i].routine)(sp[0].real);
      sp->typ=PL_COMPLEX;
    } else {
      sp[0].real=(pfuncs[i].routine)(sp[0].real);
      sp[0].typ=PL_FLOAT;
    }   
    return 1-anzarg;
  }
  if(pfuncs[i].pmax==2 && (pfuncs[i].opcode&FM_TYP)==F_DQUICK) {
    if(anzarg>0) cast_to_real(&sp[0]);
    if(anzarg>1) cast_to_real(&sp[1]);
    if((pfuncs[i].opcode&FM_RET)==F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(sp[0].real,sp[1].real);
      sp[0].typ=PL_INT;
    } else if((pfuncs[i].opcode&FM_RET)==F_CRET) {
      *((COMPLEX *)&(sp[0].real))=((COMPLEX (*)())pfuncs[i].routine)(sp[0].real,sp[1].real);
      sp->typ=PL_COMPLEX;
    } else {
      sp[0].real=(pfuncs[i].routine)(sp[0].real,sp[1].real);
      sp[0].typ=PL_FLOAT;
    }   
    return 1-anzarg;
  }
  if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_IQUICK) {
    if(anzarg>0) cast_to_int(&sp[0]);
    if((pfuncs[i].opcode&FM_RET)==F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(sp[0].integer);
      sp[0].typ=PL_INT;
    } else if((pfuncs[i].opcode&FM_RET)==F_CRET) {
      *((COMPLEX *)&(sp[0].real))=((COMPLEX (*)())pfuncs[i].routine)(sp[0].integer);
      sp->typ=PL_COMPLEX;
    } else {
      sp[0].real=(pfuncs[i].routine)(sp[0].integer);
      sp[0].typ=PL_FLOAT;
    }   
    return 1-anzarg;
  }
  if(pfuncs[i].pmax==2 && (pfuncs[i].opcode&FM_TYP)==F_IQUICK) {
    if(anzarg>0) cast_to_int(&sp[0]);
    if(anzarg>1) cast_to_int(&sp[1]);
    if((pfuncs[i].opcode&FM_RET)==F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(sp[0].integer,sp[1].integer);
      sp[0].typ=PL_INT;
    } else if((pfuncs[i].opcode&FM_RET)==F_CRET) {
      *((COMPLEX *)&(sp[0].real))=((COMPLEX (*)())pfuncs[i].routine)(sp[0].integer,sp[1].integer);
      sp->typ=PL_COMPLEX;
    } else {
      sp->real=(pfuncs[i].routine)(sp[0].integer,sp[1].integer);
      sp->typ=PL_FLOAT;
    }   
    return 1-anzarg;
  }
  if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_CQUICK) {
    if(anzarg>0) cast_to_complex(sp);
    COMPLEX *a=(COMPLEX *)(&(sp[0].real));
    if((pfuncs[i].opcode&FM_RET)==F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(*a);
      sp[0].typ=PL_INT;
    } else if((pfuncs[i].opcode&FM_RET)==F_CRET) {
      *((COMPLEX *)&(sp[0].real))=((COMPLEX (*)())pfuncs[i].routine)(*a);
      sp->typ=PL_COMPLEX;
    } else {
      sp[0].real=(pfuncs[i].routine)(*a);
      sp[0].typ=PL_FLOAT;
    }   
    return 1-anzarg;
  }
  if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_SQUICK) {
    STRING a;
    if(sp->typ==PL_STRING) {
      a.len=sp->integer;
      a.pointer=sp->pointer;
    } else 
    xberror(47,(char *)pfuncs[i].name); /*  Parameter %s falsch, kein String */
//    printf("Got a string: <%s>\n",a.pointer);
    if((pfuncs[i].opcode&FM_RET)==F_IRET) {
      sp->integer=((int (*)())pfuncs[i].routine)(a);
      sp->typ=PL_INT;
    } else if((pfuncs[i].opcode&FM_RET)==F_CRET) {
      sp->real=(double)((int (*)())pfuncs[i].routine)(a);
      sp->imag=0;
      sp->typ=PL_COMPLEX;
    } else {
      sp->real=(pfuncs[i].routine)(a);
      sp->typ=PL_FLOAT;
    }
    free(a.pointer);
    return 1-anzarg;
  }
  VMERROR("INCOMPLETE"
       ", this function does not get its parameters.");
  return 1-anzarg;
}

STATIC int vm_comm(PARAMETER *sp,int i, int anzarg) {    /*  */
  VERBOSE("vm_%s(%d)_%d\n",comms[i].name,anzarg,i);
// printf("SP=%p\n",sp);
#ifdef EXTRACHECK
  if(anzarg<comms[i].pmin) {
    xberror(42,comms[i].name); /* Zu wenig Parameter  */
    return -anzarg;
  }
  if((anzarg>comms[i].pmax && !(comms[i].pmax==-1)) || (anzarg && (comms[i].opcode&PM_TYP)==P_SIMPLE)) {
    xberror(45,comms[i].name); /* Zu viele Parameter  */
    return -anzarg;
  }  
#endif
  switch(comms[i].opcode&PM_TYP) {
  case P_IGNORE: return -anzarg;
  case P_ARGUMENT: {
    char *w2=sp[-1].pointer;
    (comms[i].routine)(w2);    
    free_parameter(&sp[-1]);
    }
    return -anzarg;
  case P_SIMPLE:
    (comms[i].routine)();
    return -anzarg;
  case P_PLISTE: {
    PARAMETER *plist;
    int e=make_pliste3(comms[i].pmin,comms[i].pmax,(unsigned short *)comms[i].pliste,
                 &sp[-anzarg],&plist,anzarg);
    (comms[i].routine)(plist,e);
    if(e!=-1) free_pliste(e,plist);
    }
    return -anzarg;
  } 
  VMERROR("INCOMPLETE"
       ", this function does not get its parameters.");
  return -anzarg;
}

ISTATIC int vm_pushvv(int vnr,PARAMETER *sp) {    /*  */
    VERBOSE("vm_pushvv_%d\n",vnr);
    sp->integer=vnr;
    sp->pointer=varptr_indexliste(&variablen[vnr],NULL,0);
    int typ=variablen[vnr].typ;
    if(typ==ARRAYTYP) typ|=variablen[vnr].pointer.a->typ;
    sp->typ=(PL_VARGROUP|variablen[vnr].typ);
    return(1);
}


static void make_indexliste_plist(int dim, PARAMETER *p, int *index) {
  while(--dim>=0) index[dim]=p2int(&p[dim]);
}




STATIC int vm_pushvvi(int vnr,PARAMETER *sp,int dim) {    /*  */
  int *indexliste=NULL;
  PARAMETER *p=&sp[-dim];
  //dump_parameterlist(p,dim);
  VERBOSE("vm_pushvvi_%d\n",vnr);

  if(dim) {
    indexliste=(int *)malloc(dim*sizeof(int));
    make_indexliste_plist(dim,p,indexliste);
 //   printf("Index=%d\n",*indexliste); 
  }
  p->integer=vnr;
  p->pointer=varptr_indexliste(&variablen[vnr],indexliste,dim);
 // printf("Pointer=%x\n",(int)p->pointer);
  p->typ=(PL_VARGROUP|variablen[vnr].pointer.a->typ);
  if(indexliste) free(indexliste);
  return(-dim+1);
}


ISTATIC int vm_zuweis(int vnr,PARAMETER *sp) {    /*  */
  VERBOSE("vm_zuweis_%d\n",vnr);
  zuweis_v_parameter(&variablen[vnr],&sp[-1]);
  free_parameter(sp-1);
  return(-1);
}

STATIC int vm_zuweisindex(int vnr,PARAMETER *sp,int dim) {    /*  */
  int *indexliste=NULL;
  VERBOSE("vm_zuweisindex_%d_%d \n",vnr,dim);
//printf("ZUWEISINDEX: vnr=%d dim=%d",vnr,dim);
//dump_parameterlist(&sp[-dim],dim);
//printf("value: ");
//dump_parameterlist(&sp[-dim-1],1);

//printf("vdim=%d\n",variablen[vnr].opcode);

  if(dim) {
    indexliste=(int *)malloc(dim*sizeof(int));
    make_indexliste_plist(dim,&sp[-dim],indexliste); 
  }
  zuweispbyindex(vnr,indexliste,dim,&sp[-dim-1]);
  free(indexliste);
  return(-dim-1);
}

STATIC int vm_pusharrayelem(int vnr, PARAMETER *sp, int dim) {    /*  */
  int typ;
  int *indexliste;
  typ=variablen[vnr].typ;
  VERBOSE("vm_pusharrayelem_%d_%d ",vnr,dim);

//printf("\nPUSHARRAYELEM: vnr=%d dim=%d\n",vnr,dim);
//dump_parameterlist(&sp[-dim],dim);

  if(typ==ARRAYTYP) {

    if(dim>0) {
      char *varptr;
      int subtyp=variablen[vnr].pointer.a->typ;
      PARAMETER *p=&sp[-dim];
      indexliste=(int *)malloc(dim*sizeof(int));
      make_indexliste_plist(dim,&sp[-dim],indexliste); 
      varptr=varptr_indexliste(&variablen[vnr],indexliste,dim);
      free_parameter(p);
      if(subtyp==INTTYP) {
        p->typ=PL_INT;
	p->integer=*((int *)varptr);
      } else if(subtyp==FLOATTYP) {
        p->typ=PL_FLOAT;
	p->real=*((double *)varptr);
      } else if(subtyp==COMPLEXTYP) {
        p->typ=PL_COMPLEX;
	*(COMPLEX *)&p->real=*((COMPLEX *)varptr);
      } else if(subtyp==ARBINTTYP) {
        p->typ=PL_ARBINT;
	p->pointer=malloc(sizeof(ARBINT));
	mpz_init(*(ARBINT *)p->pointer);
	mpz_set(*(ARBINT *)p->pointer,*((ARBINT *)varptr));
      } else if(subtyp==STRINGTYP) {
        p->typ=PL_STRING;
	p->integer=((STRING *)varptr)->len;
	p->pointer=malloc(p->integer+1);
	memcpy(p->pointer,((STRING *)varptr)->pointer,p->integer);
	((char *)p->pointer)[p->integer]=0;
      } else VMERROR("pusharrayelem"); 
      free(indexliste);
    } else VMERROR("ARRAYELEM"); 
  } else VMERROR("ARRAYELEM"); 

  return(-dim+1);
}

STATIC void  push_v(PARAMETER *p, const VARIABLE *v) {
  p->panzahl=0;
  p->typ=(PL_CONSTGROUP|v->typ);
  switch(v->typ) {
  case INTTYP:   p->integer=*(v->pointer.i); return;
  case FLOATTYP: p->real=*(v->pointer.f);    return;
  case COMPLEXTYP: *((COMPLEX *)&(p->real))=*(v->pointer.c); return;
  case STRINGTYP: *((STRING *)&(p->integer))=double_string(v->pointer.s); return;
  case ARRAYTYP:  *((ARRAY *)&(p->integer))=double_array(v->pointer.a);   return;
  case ARBINTTYP: 
    p->pointer=malloc(sizeof(ARBINT));
    mpz_init(*(ARBINT *)p->pointer);   
    mpz_set(*(ARBINT *)p->pointer,*(v->pointer.ai));
    return;
  default: 
    printf("pushv: Something is wrong, var <%s> typ=$%x cannot push.\n",v->name,v->typ);
  }
}

inline static int vm_pushv(unsigned short vnr,PARAMETER *sp) {    /*  */
  VERBOSE("vm_pushv_%d ",vnr);
  push_v(sp,&variablen[vnr]);
  return(1);
}



static int vm_eval(PARAMETER *sp) {    /*  */
  VERBOSE("vm_eval ");
  if(sp[-1].typ==PL_STRING) {
    VERBOSE("vm_eval_<%s>\n",(char *)sp[-1].pointer);
    kommando(sp[-1].pointer);
    free(sp[-1].pointer);
  } else {
    TYPEMISMATCH("EVAL:no string");
    free_parameter(&sp[-1]);
  }
  return(-1);  
}


int program_adr=0;
PARAMETER *virtual_machine(const STRING bcpc, int offset, int *npar, const PARAMETER *plist, int inpar) {
  PARAMETER *opstack=calloc(BC_STACKLEN,sizeof(PARAMETER));
  PARAMETER *osp=opstack;
  register unsigned char cmd;
  register int i=offset;
  int j;
  
  int a,n;
  short ss,ss2;
  double d;
  char *buf;
  
  if(plist && inpar>0) {
    memcpy(osp,plist,sizeof(PARAMETER)*inpar);
    opstack+=inpar;
  }

#ifdef ANDROID
  backlog("enter virtual machine.");
#endif
  while(batch && i<bcpc.len && (cmd=bcpc.pointer[i])) {
    program_adr=i;
    i++;
    switch(cmd) {
    case BC_NOOP:  
      VERBOSE("vm_noop ");
      break;
    case BC_JSR:
      CP4(&a,&bcpc.pointer[i],i);
      VERBOSE("vm_jsr_%d\n",a);
      stack[sp]=i;
      i=a;
      // dump_parameterlist(opstack-8,8);
      break;
    case BC_JMP:
      CP4(&a,&bcpc.pointer[i],i);
      i=a;
      VERBOSE("vm_jmp_%d\n",a);
      break;
    case BC_JEQ:
      if((--opstack)->integer==0) {
        CP4(&a,&bcpc.pointer[i],i);
	i=a;
        VERBOSE("vm_jeq_%d ",a);
      } else {
        i+=sizeof(int);
        VERBOSE("vm_jeq_** ");
      }
      break;
    case BC_BRA:
      CP2(&ss,&bcpc.pointer[i],i);
      VERBOSE("vm_bra_%d \n",ss);
      i+=ss;
      break;
    case BC_BEQ:
      CP2(&ss,&bcpc.pointer[i],i);
      VERBOSE("vm_beq_%d ",ss);
      if((--opstack)->integer==0) i+=ss;
      break;
    case BC_BSR:
      CP2(&ss,&bcpc.pointer[i],i);
      VERBOSE("vm_bsr_%d \n",ss);
      stack[sp]=i;
      i+=ss;
      break;
    case BC_BLKSTART:
      VERBOSE("vm_{ ");
      sp++;
      break;
    case BC_BLKEND:
      VERBOSE("vm_} \n");
      restore_locals(sp--);
      break;
    case BC_RTS:
      VERBOSE("vm_rts \n");
      i=stack[sp];
      break;
    case BC_BRAs:
      VERBOSE("vm_bra.s_%d \n",bcpc.pointer[i]); 
      i+=bcpc.pointer[i];
      i++;
      break;
    case BC_BEQs:
      ss=bcpc.pointer[i++];
      VERBOSE("vm_beqs_%d ",ss);
      if((--opstack)->integer==0) i+=ss;
      break;
    case BC_PUSHC:
      CP8(&d,&bcpc.pointer[i],i);
      opstack->real=d;
      CP8(&d,&bcpc.pointer[i],i);
      opstack->imag=d;
      opstack->typ=PL_COMPLEX;
      opstack++;
      VERBOSE("(%g+%gi)",opstack->real,opstack->imag);
      break;
    case BC_PUSHF:
      CP8(&d,&bcpc.pointer[i],i);
      opstack->real=d;
      opstack->typ=PL_FLOAT;
      opstack++;
      VERBOSE("%g ",d);
      break;
    case BC_PUSHI:
      CP4(&a,&bcpc.pointer[i],i);
      opstack->integer=a;
      opstack->typ=PL_INT;
      opstack++;
      VERBOSE("%d ",a);
      break;
    case BC_LOADi:
      CP4(&a,&bcpc.pointer[i],i);
      opstack->integer=*((int *)a);
      opstack->typ=PL_INT;
      opstack++;
      VERBOSE("[$%x].i ",a);
      break;
    case BC_LOADf:
      CP4(&a,&bcpc.pointer[i],i);
      opstack->real=*((double *)a);
      opstack->typ=PL_FLOAT;
      opstack++;
      VERBOSE("[$%x].d ",a);
      break;
    case BC_LOADc:
      CP4(&a,&bcpc.pointer[i],i);
      opstack->real=*((double *)a);
      opstack->imag=0;
      opstack->typ=PL_COMPLEX;
      opstack++;
      VERBOSE("[$%x].d ",a);
      break;
    case BC_PUSHW:
      CP2(&ss,&bcpc.pointer[i],i);
      opstack->integer=ss;
      opstack->typ=PL_INT;
      opstack++;
      VERBOSE("%d ",ss);
      break;
    case BC_PUSHB:
      opstack->integer=bcpc.pointer[i++];
      opstack->typ=PL_INT;
      opstack++;
      VERBOSE("%d ",opstack[-1].integer);
      break;
    case BC_PUSHLEER:
      opstack->typ=PL_LEER;
      opstack++;
      VERBOSE("<empty> ");
      break;
    case BC_PUSH0:
      opstack->integer=0;
      opstack->typ=PL_INT;
      opstack++;
      VERBOSE("0 ");
      break;
    case BC_PUSH1:
      opstack->integer=1;
      opstack->typ=PL_INT;
      opstack++;
      VERBOSE("1 ");
      break;
    case BC_PUSH2:
      opstack->integer=2;
      opstack->typ=PL_INT;
      opstack++;
      VERBOSE("2 ");
      break;
    case BC_PUSHM1:
      opstack->integer=-1;
      opstack->typ=PL_INT;
      opstack++;
      VERBOSE("-1 ");
      break;
    case BC_PUSHS:  /*String konstante auf STack....*/
      { int len;
      CP4(&len,&bcpc.pointer[i],i);
      CP4(&a,&bcpc.pointer[i],i);
      opstack->integer=len;
      opstack->typ=PL_STRING;
      opstack->pointer=malloc(len+1);
      memcpy(opstack->pointer,rodata+a,len);
      ((char *)(opstack->pointer))[len]=0;
      opstack++;
      VERBOSE("\"%s\" ",(char *)opstack[-1].pointer);
      }
      break;
    case BC_PUSHAI:  /*Big int Konstante auf STack....*/
      { int len;
      CP4(&len,&bcpc.pointer[i],i);
      CP4(&a,&bcpc.pointer[i],i);
      opstack->typ=PL_ARBINT;
      char *buf=malloc(len+1);
      memcpy(buf,rodata+a,len);
      buf[len]=0;
      opstack->pointer=malloc(sizeof(ARBINT));
      mpz_init(*(ARBINT *)opstack->pointer);
      mpz_set_str(*(ARBINT *)opstack->pointer,buf,32);
      free(buf);
      buf=mpz_get_str(NULL,10,*(ARBINT *)opstack->pointer);
      opstack++;
      VERBOSE("\"%s\" ",buf);
      free(buf);
      }
      break;
    case BC_PUSHA:  /*Array konstante auf STack....*/
      { int len;
        STRING str;
      CP4(&len,&bcpc.pointer[i],i);
      CP4(&a,&bcpc.pointer[i],i);
      str.len=len;
      str.pointer=(char *)(rodata+a);
      opstack->typ=PL_ARRAY;
      *((ARRAY *)&(opstack->integer))=string_to_array(str);
      opstack++;
      VERBOSE("array ");
      }
      break;
    case BC_PUSHFUNC:
      a=bcpc.pointer[i++]&0xff;
      n=bcpc.pointer[i++]&0xff;
  //    dump_parameterlist(opstack-3,3);
      opstack+=vm_func(opstack,a,n);
      break;
    case BC_PUSHSFUNC:
      a=bcpc.pointer[i++]&0xff;
      n=bcpc.pointer[i++]&0xff;
      opstack+=vm_sfunc(opstack,a,n);
      break;
    case BC_PUSHCOMM:
      a=bcpc.pointer[i++]&0xff;
      n=bcpc.pointer[i++]&0xff;
      opstack+=vm_comm(opstack,a,n);
      VERBOSE("SP=%d\n",((int)opstack-(int)osp)/sizeof(PARAMETER));  
      break;
    case BC_PUSHSYS:
      a=bcpc.pointer[i++]&0xff;
      opstack+=vm_sysvar(opstack,a);
      break;
    case BC_PUSHSSYS:
      a=bcpc.pointer[i++]&0xff;
      opstack+=vm_ssysvar(opstack,a);
      break;
    case BC_PUSHASYS:
      a=bcpc.pointer[i++]&0xff;
      opstack+=vm_asysvar(opstack,a);
      break;
    case BC_PUSHX:
      { int len=bcpc.pointer[i++];
        CP4(&a,&bcpc.pointer[i],i);
        buf=malloc(len+1);
        memcpy(buf,rodata+a,len);
        buf[len]=0;
        opstack->integer=len;
        opstack->typ=PL_KEY;    /*  Kann auch EVAL oder P_ARGUMENT sein */
	opstack->arraytyp=KEYW_UNKNOWN; 
        opstack->pointer=buf;
        opstack++;
        VERBOSE("%s ",buf);
      }
      break;
    case BC_PUSHK:
      opstack->arraytyp=bcpc.pointer[i++];
      opstack->typ=PL_KEY;
      opstack->integer=0;
      opstack->pointer=NULL;
      opstack++;
      VERBOSE("%s ",keywords[opstack[-1].arraytyp]);
      break;
    case BC_COMMENT:
      a=bcpc.pointer[i++];
      buf=malloc(a+1);
      memcpy(buf,&bcpc.pointer[i],a);
      buf[a]=0;
      VERBOSE("%s ",buf);
      i+=a;
      free(buf);
      break;
    case BC_ADD:
      opstack+=vm_add(opstack);
      break;
    case BC_ADDi:
      VERBOSE("ADDi ");
      opstack--;(opstack-1)->integer+=opstack->integer;
      break;
    case BC_ADDf:
      VERBOSE("ADDf ");
      opstack--;(opstack-1)->real+=opstack->real;
      break;
    case BC_ADDc:
      VERBOSE("ADDc ");
      opstack--;(opstack-1)->real+=opstack->real;(opstack-1)->imag+=opstack->imag;
      break;
    case BC_SUBc:
      VERBOSE("SUBc ");
      opstack--;(opstack-1)->real-=opstack->real;(opstack-1)->imag-=opstack->imag;
      break;
    case BC_ADDs:
      VERBOSE("ADDs ");
      opstack--; 
      {
        int l=(opstack-1)->integer;
	char *p=(opstack-1)->pointer;
        (opstack-1)->integer+=opstack->integer;
        (opstack-1)->pointer=malloc((opstack-1)->integer+1);
        memcpy((opstack-1)->pointer,p,l);free(p);
        memcpy((opstack-1)->pointer+l,opstack->pointer,opstack->integer+1);
      }
      free(opstack->pointer);
      break;
    case BC_OR:
      VERBOSE("OR ");
      opstack+=vm_or(opstack);
      break;
    case BC_ORi:
      VERBOSE("ORi ");
      opstack--;
      (opstack-1)->integer|=opstack->integer;
      break;
    case BC_XOR:
      VERBOSE("XOR ");
      opstack+=vm_xor(opstack);
      break;
    case BC_XORi:
      VERBOSE("XORi ");
      opstack--;
      (opstack-1)->integer^=opstack->integer;
      break;
    case BC_SUB:
      opstack+=vm_sub(opstack);
      break;
    case BC_SUBi:
      VERBOSE("SUBi ");
      opstack--;(opstack-1)->integer-=opstack->integer;
      break;
    case BC_SUBf:
      VERBOSE("SUBf ");
      opstack--;(opstack-1)->real-=opstack->real;
      break;
    case BC_MUL:
      opstack+=vm_mul(opstack);
      break;
    case BC_MULi:
      VERBOSE("MULi ");
      opstack--;(opstack-1)->integer*=opstack->integer;
      break;
    case BC_MULf:
      VERBOSE("MULf ");
      opstack--;(opstack-1)->real*=opstack->real;
      break;
    case BC_MULc:
      VERBOSE("MULc ");
      opstack--;*(COMPLEX *)&((opstack-1)->real)=complex_mul(*(COMPLEX *)&((opstack-1)->real),*(COMPLEX *)&((opstack)->real));
      break;
    case BC_DIV:
      VERBOSE("DIV ");
      opstack+=vm_div(opstack);
      break;
    case BC_DIVf:
      VERBOSE("DIVf ");
      opstack--;(opstack-1)->real/=opstack->real;
      break;
    case BC_DIVc:
      VERBOSE("DIVc ");
      opstack--;*(COMPLEX *)&((opstack-1)->real)=complex_div(*(COMPLEX *)&((opstack-1)->real),*(COMPLEX *)&((opstack)->real));
      break;
    case BC_POW:
      opstack+=vm_pow(opstack);
      break;
    case BC_AND:
      VERBOSE("AND ");
      opstack+=vm_and(opstack);
      break;
    case BC_ANDi:
      VERBOSE("ANDi ");
      opstack--;
      (opstack-1)->integer&=opstack->integer;
      break;
    case BC_EQUAL:
      opstack+=vm_equal(opstack);
      break;
    case BC_GREATER:
      opstack+=vm_greater(opstack);
      break;
    case BC_LESS:
      opstack+=vm_less(opstack);
      break;
    case BC_DUP:
      opstack+=vm_dup(opstack);
      break;
    case BC_EXCH:
      opstack+=vm_exch(opstack);
      break;
    case BC_CLEAR:
      a=((int)opstack-(int)osp)/sizeof(PARAMETER);
      if(a) {
        opstack=osp;
	for(j=0;j<a;j++)  free_parameter(&opstack[j]);
      }
      VERBOSE("vm_clear ");
      break;
    case BC_COUNT:
      a=((int)opstack-(int)osp)/sizeof(PARAMETER);
      opstack->integer=a;
      opstack->typ=PL_INT;
      opstack++;
      VERBOSE("vm_count_%d ",a);
      break;
    case BC_NOT:
      VERBOSE("NOT ");
      vm_not(opstack);
      break;
    case BC_NOTi:
      VERBOSE("NOTi ");
      (opstack-1)->integer=~(opstack-1)->integer;
      break;
    case BC_NEG: vm_neg(opstack); break;
    case BC_X2I: vm_x2i(opstack); break;
    case BC_X2AI: vm_x2ai(opstack); break;
    case BC_I2F: 
      (opstack-1)->real=(double)(opstack-1)->integer;
      (opstack-1)->typ=PL_FLOAT;
      break;
    case BC_I2FILE: 
      (opstack-1)->typ=PL_FILENR;
      break;
    case BC_F2C: 
      (opstack-1)->imag=0;
      (opstack-1)->typ=PL_COMPLEX;
      break;
    case BC_X2F: vm_x2f(opstack); break;
    case BC_X2C: vm_x2c(opstack); break;
    case BC_MOD:
      VERBOSE("MOD ");
      opstack+=vm_mod(opstack);
      break;
    case BC_POP:
      VERBOSE("POP ");
      opstack--;
      free_parameter(opstack);
      break;
    case BC_ZUWEISi:
      CP2(&ss,&bcpc.pointer[i],i);
      *(variablen[ss].pointer.i)=(--opstack)->integer;
      break;
    case BC_ZUWEISf:
      CP2(&ss,&bcpc.pointer[i],i);
      *(variablen[ss].pointer.f)=(--opstack)->real;
      break;
    case BC_ZUWEISc:
      CP2(&ss,&bcpc.pointer[i],i);
      *(variablen[ss].pointer.c)=*((COMPLEX *)&((--opstack)->real));
      break;
    case BC_ZUWEIS:
      CP2(&ss,&bcpc.pointer[i],i);
      opstack+=vm_zuweis(ss,opstack);
      break;
    case BC_PUSHVV:
      CP2(&ss,&bcpc.pointer[i],i);
      opstack+=vm_pushvv(ss,opstack);
      break;
    case BC_LOCAL:
      CP2(&ss,&bcpc.pointer[i],i);
      VERBOSE("dolocal_%d ",ss);
      do_local(ss,sp);
      break;
    case BC_ZUWEISINDEX:
      CP2(&ss,&bcpc.pointer[i],i);
      CP2(&ss2,&bcpc.pointer[i],i);
      opstack+=vm_zuweisindex(ss,opstack,ss2);
      break;
    case BC_PUSHV:
      CP2(&ss,&bcpc.pointer[i],i);
      opstack+=vm_pushv(ss,opstack);
      break;
    case BC_PUSHARRAYELEM:      
      CP2(&ss,&bcpc.pointer[i],i);
      CP2(&ss2,&bcpc.pointer[i],i);
      opstack+=vm_pusharrayelem(ss,opstack,ss2);
      break;
    case BC_PUSHVVI:
      CP2(&ss,&bcpc.pointer[i],i);
      CP2(&ss2,&bcpc.pointer[i],i);
      opstack+=vm_pushvvi(ss,opstack,ss2);
      break;
    case BC_RESTORE:
      CP4(&datapointer,&bcpc.pointer[i],i);
      VERBOSE("vm_restore_%d\n",datapointer);
      break;
    case BC_EVAL:
      VERBOSE("vm_eval-%d ",a);
      opstack+=vm_eval(opstack);
      break;
    case BC_PUSHLABEL:
      CP4(&a,&bcpc.pointer[i],i);
      opstack->integer=a;
      opstack->typ=PL_LABEL;
      opstack->arraytyp=1;
      opstack++;
      break;
    case BC_PUSHPROC: 
      CP4(&a,&bcpc.pointer[i],i);
      opstack->integer=a;
      opstack->typ=PL_PROC;
      opstack->arraytyp=1;
      opstack++;
      break;
    default:
      VMERROR("BC_ILLEGAL instruction %2x at %p",(int)cmd,(void *)i);
      memdump((unsigned char *)&(bcpc.pointer[i]),16);
      xberror(104,""); /* Illegal Instruction */
    }
  }
  VERBOSE("\nVirtual machine exited at %p. batch=%d err=%d",(void *)i,batch,err);
  // printf("\nVirtual machine exited at %p. batch=%d err=%d\n",(void *)i,batch,err);
#ifdef ANDROID
  backlog("Virtual machine exited.");
#endif
  *npar=((int)opstack-(int)osp)/sizeof(PARAMETER);  
  return(osp);
}


/*******************************************/
/* These Routines allow the access from X11-Basic compiled 
   functions (in shared libraries etc.) */
/*******************************************/



/* Diese Routinen sollten als Interface zur Virtual machine dienen.
   Sie muessen in das virtual-machine.c file integriert werden. Ist nur
   wichtig, wenn man C-Libraries aus X11-Basic Funktionen compilieren
   moechte.*/

static void do_pusharg(va_list *arg, unsigned char typ, PARAMETER **sp) {
  ARRAY a;
  STRING s;
  double f;
  int i;
  PARAMETER *opstack=*sp;
//#ifdef SYSTEM64
  va_list arguments;     /*Patch von Matthias Vogl, 29.12.2012*/
  va_copy(arguments, *arg);
//#else
//  va_list arguments=*arg;
//#endif
    switch(typ) {
    case 'a': a=va_arg ( arguments, ARRAY );  *((ARRAY *)(opstack->integer))=double_array(&a); opstack->typ=PL_ARRAY; opstack++; break;
    case 's': s=va_arg ( arguments, STRING ); *((STRING *)(opstack->integer))=double_string(&s); opstack->typ=PL_STRING; opstack++; break;
    case 'f': f=va_arg ( arguments, double ); opstack->real=f; opstack->typ=PL_FLOAT; opstack++;  break;
    case 'i': i=va_arg ( arguments, int );    opstack->integer=i; opstack->typ=PL_INT; opstack++;    break;
    case '.':   va_arg ( arguments, int );  /*parameter abraeumen*/
    case ' ':
    default:
      opstack->typ=PL_LEER; opstack++;
      break;
    }
    *sp=opstack;
//#ifdef SYSTEM64
    va_copy(*arg, arguments); /*Patch von Matthias Vogl, 29.12.2012*/
//#else
//    *arg=arguments;
//#endif
}



/* API for The virtual machine */



int pusharg(PARAMETER **opstack, char *typ,...)   {
  int count=0;
  unsigned char c;
  va_list arguments;    
  va_start ( arguments, typ ); 
  while((c=typ[count++])) do_pusharg(&arguments,c,opstack);
  va_end ( arguments );                  // Cleans up the list
  (*opstack)->integer=count; (*opstack)->typ=PL_INT; (*opstack)++;
// jetzt kann die Funktion aufgerufen werden...
  return(count);
}

int callifunc(PARAMETER **opstack,void (*name)(),char *typ,...) {
  int count=0;
  unsigned char c;
  PARAMETER *osp=*opstack;
  va_list arguments;    
  va_start(arguments,typ); 
  while((c=typ[count++])) do_pusharg(&arguments,c,opstack);
  va_end ( arguments );                  // Cleans up the list
  (*opstack)->integer=count; (*opstack)->typ=PL_INT; (*opstack)++;
  name();
  return(osp->integer);
}
double callfunc(PARAMETER **opstack,void (*name)(),char *typ,...) {
  int count=0;
  unsigned char c;
  PARAMETER *osp=*opstack;
  va_list arguments;    
  va_start(arguments,typ); 
  while((c=typ[count++])) do_pusharg(&arguments,c,opstack);
  va_end ( arguments );                  // Cleans up the list
  (*opstack)->integer=count; (*opstack)->typ=PL_INT; (*opstack)++;
  name();
  return(osp->real);
}
STRING callsfunc(PARAMETER **opstack,void (*name)(),char *typ,...) {
  int count=0;
  unsigned char c;
  PARAMETER *osp=*opstack;
  va_list arguments;    
  va_start(arguments,typ); 
  while((c=typ[count++])) do_pusharg(&arguments,c,opstack);
  va_end ( arguments );                  // Cleans up the list
  (*opstack)->integer=count; (*opstack)->typ=PL_INT; (*opstack)++;
  name();
  return(*((STRING *)&(osp->integer)));
}
ARRAY callafunc(PARAMETER **opstack,void (*name)(),char *typ,...) {
  int count=0;
  unsigned char c;
  PARAMETER *osp=*opstack;
  va_list arguments;    
  va_start(arguments,typ); 
  while((c=typ[count++])) do_pusharg(&arguments,c,opstack);
  va_end ( arguments );                  // Cleans up the list
  (*opstack)->integer=count; (*opstack)->typ=PL_INT; (*opstack)++;
  name();
  return(*((ARRAY *)&(osp->integer)));
}
