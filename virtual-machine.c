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
#include "parameter.h"
#include "variablen.h"
#include "functions.h"
#include "bytecode.h"
#include "array.h"
#include "io.h"

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


char *rodata=NULL;;

inline static void vm_x2i(PARAMETER *sp) {    /* cast to integer */
  VERBOSE("x2i ");
  if(sp[-1].typ==PL_INT) return;
  else if(sp[-1].typ==PL_FLOAT) {
    sp[-1].integer=(int)sp[-1].real;
    sp[-1].typ=PL_INT;  
  } else VMERROR("X2I (typ=%x)",sp[-1].typ);
  return;
}
inline static void vm_x2f(PARAMETER *sp) {    /* cast to float */
  VERBOSE("x2f ");
  if(sp[-1].typ==PL_FLOAT) return;
  else if(sp[-1].typ==PL_INT) {
    sp[-1].real=(double)sp[-1].integer;
    sp[-1].typ=PL_FLOAT;  
  } else VMERROR("X2F (typ=%x)",sp[-1].typ);
  return;
}

STATIC int vm_add(PARAMETER *sp) {    /* binaer addition */
  VERBOSE("vm_add ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          sp[-1].integer+=sp[0].integer;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) sp[-1].real+=sp[0].real;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT)   sp[-1].real+=(double)sp[0].integer;
  else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) {
    sp[-1].real=sp[0].real+(double)sp[-1].integer;
    sp[-1].typ=PL_FLOAT;
  } else if(sp[-1].typ==PL_STRING && sp[0].typ==PL_STRING) {
    STRING a,b;
    a.pointer=sp[-1].pointer;
    a.len=sp[-1].integer;
    b.pointer=sp[0].pointer;
    b.len=sp[0].integer;
    sp[-1].pointer=malloc(a.len+b.len+1);
    sp[-1].integer=a.len+b.len;
    memcpy(sp[-1].pointer,a.pointer,a.len);
    memcpy(sp[-1].pointer+a.len,b.pointer,b.len);
    free(a.pointer);
    free(b.pointer);
  } else if(sp[-1].typ==PL_ARRAY && sp[0].typ==PL_ARRAY) {
    ARRAY a,b;
    a=*((ARRAY *)&(sp[-1].integer));
    b=*((ARRAY *)&(sp[0].integer));
    array_add(a,b);
    free_array(&b);
    *((ARRAY *)&(sp[-1].integer))=a;
  } else {
    TYPEMISMATCH("ADD");
    free_parameter(sp);
  }
  return(-1);
}
STATIC int vm_sub(PARAMETER *sp) {    /* binaer addition */
  VERBOSE("vm_sub ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          sp[-1].integer-=sp[0].integer;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) sp[-1].real-=sp[0].real;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT)   sp[-1].real-=(double)sp[0].integer;
  else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) {
    sp[-1].real=sp[0].real-(double)sp[-1].integer;
    sp[-1].typ=PL_FLOAT;
  } else if(sp[-1].typ==PL_ARRAY && sp[0].typ==PL_ARRAY) {
    ARRAY a,b;
    a=*((ARRAY *)&(sp[-1].integer));
    b=*((ARRAY *)&(sp[0].integer));
    array_sub(a,b);
    free_array(&b);
    *((ARRAY *)&(sp[-1].integer))=a;
  } else {
    TYPEMISMATCH("SUB");
    free_parameter(sp);
  }
  return(-1);
}
STATIC int vm_mul(PARAMETER *sp) {    /* binaer addition */
  VERBOSE("vm_mul ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          sp[-1].integer*=sp[0].integer;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) sp[-1].real*=sp[0].real;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT)   sp[-1].real*=(double)sp[0].integer;
  else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) {
    sp[-1].real=sp[0].real*(double)sp[-1].integer;
    sp[-1].typ=PL_FLOAT;
  } else if(sp[-1].typ==PL_ARRAY && sp[0].typ==PL_ARRAY) {
    ARRAY a,b,c;
    a=*((ARRAY *)&(sp[-1].integer));
    b=*((ARRAY *)&(sp[0].integer));
    c=mul_array(a,b);
    free_array(&b);
    free_array(&a);
    *((ARRAY *)&(sp[-1].integer))=c;
  } else {
    TYPEMISMATCH("MUL");
    free_parameter(sp);
  }
  return(-1);
}
STATIC int vm_pow(PARAMETER *sp) {    /* binaer addition */
  VERBOSE("vm_pow ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          
    sp[-1].real=pow((double)sp[-1].integer,(double)sp[0].integer);
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) 
   sp[-1].real=pow(sp[-1].real,sp[0].real);
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT)   
   sp[-1].real=pow(sp[-1].real,(double)sp[0].integer);
  else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) {
    sp[-1].real=pow(sp[0].real,(double)sp[-1].integer);
  } else {
    TYPEMISMATCH("POW");
    free_parameter(sp);
  }
    sp[-1].typ=PL_FLOAT;
  return(-1);
}
#if 0
int vm_div(PARAMETER *sp) {    /* binaer addition */
  VERBOSE("vm_div ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT) {
   sp[-1].real=(double)sp[-1].integer/(double)sp[0].integer;
   sp[-1].typ=PL_FLOAT;
  } else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) sp[-1].real/=sp[0].real;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT)   sp[-1].real/=(double)sp[0].integer;
  else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) {
    sp[-1].real=(double)sp[-1].integer/sp[0].real;
    sp[-1].typ=PL_FLOAT;
  } else {
    TYPEMISMATCH("DIV");
    free_parameter(sp);
  }
  return(-1);
}
#endif
STATIC int vm_mod(PARAMETER *sp) {    /* binaer addition */
  VERBOSE("vm_mod ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT) {
   sp[-1].integer=(int)fmod((double)sp[-1].integer,(double)sp[0].integer);
  } else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) 
    sp[-1].real=fmod(sp[-1].real,sp[0].real);
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT)   
    sp[-1].real=fmod(sp[-1].real,(double)sp[0].integer);
  else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) {
    sp[-1].real=fmod((double)sp[-1].integer,sp[0].real);
    sp[-1].typ=PL_FLOAT;
  } else {
    TYPEMISMATCH("MOD");
    free_parameter(sp);
  }
  return(-1);
}


STATIC int vm_equal(PARAMETER *sp) {    /* binaer and */
  VERBOSE("vm_equal ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          
    sp[-1].integer=(sp[-1].integer==sp[0].integer)?-1:0;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) {
    sp[-1].integer=(sp[-1].real==sp[0].real)?-1:0;
  } else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT) {
    sp[-1].integer=(sp[-1].real==(double)sp[0].integer)?-1:0;
  } else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) 
    sp[-1].integer=((double)sp[-1].integer==sp[0].real)?-1:0;
  else if(sp[-1].typ==PL_STRING && sp[0].typ==PL_STRING) {
    int v;
    v=(sp[-1].integer-sp[0].integer);
    if(v==0) v=memcmp(sp[-1].pointer,sp[0].pointer,min(sp[-1].integer,sp[0].integer));
    free(sp[0].pointer);
    free(sp[-1].pointer); 
    
    sp[-1].integer=v?0:-1;
    
  } else {
    TYPEMISMATCH("EQUAL");
    free_parameter(sp);
  }
  sp[-1].typ=PL_INT;
  return(-1);
}
STATIC int vm_greater(PARAMETER *sp) {    /* binaer and */
  VERBOSE("vm_greater ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          
    sp[-1].integer=(sp[-1].integer>sp[0].integer)?-1:0;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) {
    sp[-1].integer=(sp[-1].real>sp[0].real)?-1:0;
  } else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT) {
    sp[-1].integer=(sp[-1].real>(double)sp[0].integer)?-1:0;
  } else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) 
    sp[-1].integer=((double)sp[-1].integer>sp[0].real)?-1:0;
  else if(sp[-1].typ==PL_STRING && sp[0].typ==PL_STRING) {
    int v;
    v=(sp[-1].integer-sp[0].integer);
    if(v==0) v=memcmp(sp[-1].pointer,sp[0].pointer,sp[-1].integer);
    free_parameter(sp);
    free_parameter(&sp[-1]);
    sp[-1].integer=(v>0)?-1:0;
  }  else {
    TYPEMISMATCH("GREATER");
    free_parameter(sp);
  }
  sp[-1].typ=PL_INT;
  return(-1);
}
STATIC int vm_less(PARAMETER *sp) {    /* binaer and */
  VERBOSE("vm_less ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          
    sp[-1].integer=(sp[-1].integer<sp[0].integer)?-1:0;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) {
    sp[-1].integer=(sp[-1].real<sp[0].real)?-1:0;
  } else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT) {
    sp[-1].integer=(sp[-1].real<(double)sp[0].integer)?-1:0;
  } else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) 
    sp[-1].integer=((double)sp[-1].integer<sp[0].real)?-1:0;
  else if(sp[-1].typ==PL_STRING && sp[0].typ==PL_STRING) {
    int v;
    v=(sp[-1].integer-sp[0].integer);
    if(v==0) v=memcmp(sp[-1].pointer,sp[0].pointer,sp[-1].integer);
    free_parameter(sp);
    free_parameter(&sp[-1]);
    sp[-1].integer=(v>0)?-1:0;
  } else {
    TYPEMISMATCH("LESS");
    free_parameter(sp);
  }
  sp[-1].typ=PL_INT;
  return(-1);
}
STATIC int vm_sysvar(PARAMETER *sp,int n) {    /*  */
  VERBOSE("vm_%s ",sysvars[n].name);	   
  if(sysvars[n].opcode&INTTYP) {
    sp->integer=((int (*)())sysvars[n].routine)();
    sp->typ=PL_INT;
  } else if(sysvars[n].opcode&FLOATTYP) {
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
STATIC int vm_dup(PARAMETER *sp) {    /*  */
  VERBOSE("vm_dup ");
  sp[0]=sp[-1];   
  if(sp->typ==PL_KEY || sp->typ==PL_STRING) {
    sp[0].pointer=malloc(sp[0].integer+1);
    memcpy(sp[0].pointer,sp[-1].pointer,sp[0].integer+1);
  } else if(sp->typ==PL_ARRAY) {
    printf("WARNING: Pointerinhalt muesste hier dupliziert werden !\n");    
  }
  return(1);
}
inline static int vm_exch(PARAMETER *sp) {    /*  */
  VERBOSE("vm_exch ");
  sp[0]=sp[-1];   
  sp[-1]=sp[-2];   
  sp[-2]=sp[0];   
  return(0);
}
ISTATIC int vm_neg(PARAMETER *sp) {    /*  */
  VERBOSE("vm_neg ");
  if(sp[-1].typ==PL_INT)          sp[-1].integer=-sp[-1].integer;
  else if(sp[-1].typ==PL_FLOAT)   sp[-1].real=-sp[-1].real;
  else TYPEMISMATCH("NEG");
  return(0);
}
inline static void cast_to_real(PARAMETER *sp) {
  VERBOSE("vm_cast-to-real ");
  if(sp->typ==PL_FLOAT) ;
  else if(sp->typ==PL_LEER) ;
  else if(sp->typ==PL_INT) {
    sp->typ=PL_FLOAT;
    sp->real=(double)sp->integer;
  } else {
    VMERROR("CAST to real not possible for typ=$%x/value=%d,%g not implemented.",
    sp->typ,sp->integer,sp->real);
  }
}
inline static void cast_to_int(PARAMETER *sp) {
  VERBOSE("vm_cast-to-int ");
  if(sp->typ==PL_INT) ;
  else if(sp->typ==PL_LEER) ;
  else if(sp->typ==PL_FLOAT) {
    sp->typ=PL_INT;
    sp->integer=(int)sp->real;
  } else {
    VMERROR("CAST to int not possible for typ=$%x/not implemented.",sp->typ);
  }
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
    if(pfuncs[i].opcode&F_IRET){
       sp[0].integer=((int (*)())pfuncs[i].routine)();
       sp[0].typ=PL_INT;
    } else {
      sp[0].real=(pfuncs[i].routine)();
      sp[0].typ=PL_FLOAT;
    }
    return 1-anzarg;
  }  
  if((pfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
    if(pfuncs[i].opcode&F_IRET) {
      sp->integer=((int (*)())pfuncs[i].routine)(sp->pointer);
      sp->typ=PL_INT;
    } else {
      sp->real=(pfuncs[i].routine)(sp->pointer);
      sp->typ=PL_FLOAT;
    }
    free(sp->pointer);
    return 1-anzarg;
  }
  if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_DQUICK) {
    if(anzarg>0) cast_to_real(&sp[0]);
    if(pfuncs[i].opcode&F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(sp[0].real);
      sp[0].typ=PL_INT;
    } else {
      sp[0].real=(pfuncs[i].routine)(sp[0].real);
      sp[0].typ=PL_FLOAT;
    }   
    return 1-anzarg;
  }
  if(pfuncs[i].pmax==2 && (pfuncs[i].opcode&FM_TYP)==F_DQUICK) {
    if(anzarg>0) cast_to_real(&sp[0]);
    if(anzarg>1) cast_to_real(&sp[1]);
    if(pfuncs[i].opcode&F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(sp[0].real,sp[1].real);
      sp[0].typ=PL_INT;
    } else {
      sp[0].real=(pfuncs[i].routine)(sp[0].real,sp[1].real);
      sp[0].typ=PL_FLOAT;
    }   
    return 1-anzarg;
  }
  if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_IQUICK) {
    if(anzarg>0) cast_to_int(&sp[0]);
    if(pfuncs[i].opcode&F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(sp[0].integer);
      sp[0].typ=PL_INT;
    } else {
      sp[0].real=(pfuncs[i].routine)(sp[0].integer);
      sp[0].typ=PL_FLOAT;
    }   
    return 1-anzarg;
  }
  if(pfuncs[i].pmax==2 && (pfuncs[i].opcode&FM_TYP)==F_IQUICK) {
    if(anzarg>0) cast_to_int(&sp[0]);
    if(anzarg>1) cast_to_int(&sp[1]);
    if(pfuncs[i].opcode&F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(sp[0].integer,sp[1].integer);
      sp[0].typ=PL_INT;
    } else {
      sp[0].real=(pfuncs[i].routine)(sp[0].integer,sp[1].integer);
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
    if(pfuncs[i].opcode&F_IRET) {
      sp->integer=((int (*)())pfuncs[i].routine)(a);
      sp->typ=PL_INT;
    } else {
      sp->real=(pfuncs[i].routine)(a);
      sp->typ=PL_FLOAT;
    }
    free(a.pointer);
    return 1-anzarg;
  }
  if((pfuncs[i].opcode&FM_TYP)==F_PLISTE) {
    PARAMETER *plist;
    PARAMETER rpar;
    int e=make_pliste3(pfuncs[i].pmin,pfuncs[i].pmax,(unsigned short *)pfuncs[i].pliste,
                 &sp[0],&plist,anzarg);
    if(pfuncs[i].opcode&F_IRET) {
      rpar.integer=((int (*)())pfuncs[i].routine)(plist,anzarg);
      rpar.typ=PL_INT;
    } else {
      rpar.real=(pfuncs[i].routine)(plist,anzarg);
      rpar.typ=PL_FLOAT;
    }
    if(e!=-1) free_pliste(e,plist);
    sp[0]=rpar;
    return 1-anzarg;
  }
  VMERROR("INCOMPLETE"
       ", this function does not get its parameters.");
  return 1-anzarg;
}

STATIC int vm_comm(PARAMETER *sp,int i, int anzarg) {    /*  */
  VERBOSE("vm_%s(%d)_%d\n",comms[i].name,anzarg,i);
// printf("SP=%p\n",sp);
  if(comms[i].opcode&P_IGNORE) return -anzarg;
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
  if((comms[i].opcode&PM_TYP)==P_ARGUMENT) {
    char *w2;
    w2=sp[-1].pointer;
    (comms[i].routine)(w2);    
    free_parameter(&sp[-1]);
    return -anzarg;
  }  
  if((comms[i].opcode&PM_TYP)==P_SIMPLE) {
    (comms[i].routine)();
    return -anzarg;
  }      
  if((comms[i].opcode&PM_TYP)==P_PLISTE) {
    PARAMETER *plist;
    int e=make_pliste3(comms[i].pmin,comms[i].pmax,(unsigned short *)comms[i].pliste,
                 &sp[-anzarg],&plist,anzarg);
    (comms[i].routine)(plist,e);
    if(e!=-1) free_pliste(e,plist);
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
    if(variablen[vnr].typ==INTTYP) sp->typ=PL_IVAR;
    else if(variablen[vnr].typ==FLOATTYP) sp->typ=PL_FVAR;
    else if(variablen[vnr].typ==STRINGTYP) sp->typ=PL_SVAR;
    else if(variablen[vnr].typ==ARRAYTYP) {
      if(variablen[vnr].pointer.a->typ==INTTYP) sp->typ=PL_IARRAYVAR;
      else if(variablen[vnr].pointer.a->typ==FLOATTYP) sp->typ=PL_FARRAYVAR;
      else if(variablen[vnr].pointer.a->typ==STRINGTYP) sp->typ=PL_SARRAYVAR;
      else sp->typ=PL_ARRAYVAR;
    } else sp->typ=PL_ALLVAR;
    return(1);
}


static void make_indexliste_plist(int dim, PARAMETER *p, int *index) {
  while(--dim>=0) {
      if(p[dim].typ==PL_INT) index[dim]=p[dim].integer;
      else if(p[dim].typ==PL_FLOAT) index[dim]=(int)p[dim].real;
      else printf("ERROR: no int!");    
  }
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
  if(variablen[vnr].pointer.a->typ==INTTYP) p->typ=PL_IVAR;
  else if(variablen[vnr].pointer.a->typ==FLOATTYP) p->typ=PL_FVAR;
  else if(variablen[vnr].pointer.a->typ==STRINGTYP) p->typ=PL_SVAR;
  else {
    p->typ=PL_ALLVAR;
    TYPEMISMATCH("pushvvi");
  }
  if(indexliste) free(indexliste);
  return(-dim+1);
}





/* Weise aus parameter zu einer Vaiable */

static void zuweis_v_parameter(VARIABLE *v,PARAMETER *p) {
  if(v->typ==INTTYP && p->typ==PL_INT)          *(v->pointer.i)=p->integer;
  else if(v->typ==INTTYP && p->typ==PL_FLOAT)   *(v->pointer.i)=(int)p->real;
  else if(v->typ==FLOATTYP && p->typ==PL_FLOAT) *(v->pointer.f)=p->real;
  else if(v->typ==FLOATTYP && p->typ==PL_INT)   *(v->pointer.f)=(double)p->integer;
  else if(v->typ==STRINGTYP && p->typ==PL_STRING) {
    free(v->pointer.s->pointer);
    *(v->pointer.s)=double_string((STRING *)&(p->integer));
  } else if(v->typ==ARRAYTYP && p->typ==PL_ARRAY) {
    
    /*Was machen wir, wenn die Arraytypen nicht stimmen?*/
    
       ARRAY *zarr=v->pointer.a;
       ARRAY *arr=(ARRAY *)&(p->integer);
     
      if(arr->typ==zarr->typ) {
        free_array(v->pointer.a);
        *(v->pointer.a)=double_array((ARRAY *)&(p->integer));
      } else if(zarr->typ==INTTYP && arr->typ==FLOATTYP) {
        free_array(v->pointer.a);
        *(v->pointer.a)=convert_to_intarray(arr);
      } else if(zarr->typ==FLOATTYP && arr->typ==INTTYP) {
        free_array(v->pointer.a);
        *(v->pointer.a)=convert_to_floatarray(arr);
      } else {
          xberror(58,v->name); /* Variable %s has incorrect type*/  
	printf("Ziel-Array  hat folgenden Typ: %d\n",zarr->typ);
	printf("Quell-Array hat folgenden Typ: %d\n",arr->typ);
      }
      
  } else if(v->typ==INTTYP && p->typ==PL_IVAR) {
    erase_variable(v);
    v->pointer.i=(int *)p->pointer;
    v->flags=V_STATIC;
  } else if(v->typ==FLOATTYP && p->typ==PL_FVAR) {
    erase_variable(v);
    v->pointer.f=(double *)p->pointer;
    v->flags=V_STATIC;
  } else if(v->typ==STRINGTYP && p->typ==PL_SVAR) {
    erase_variable(v);
    v->pointer.s=(STRING *)p->pointer;
    v->flags=V_STATIC;
  } else if(v->typ==ARRAYTYP) {
    int atyp=v->pointer.a->typ;
    if(p->typ==PL_IARRAYVAR) {
      if(atyp!=INTTYP) {
        xberror(58,v->name); /* Variable %s has incorrect type*/ 
        dump_parameterlist(p,1);
      }
      erase_variable(v);
      v->pointer.a=(ARRAY *)p->pointer;
      v->flags=V_STATIC;
    } else if(p->typ==PL_FARRAYVAR) {
      if(atyp!=FLOATTYP) {
        xberror(58,v->name); /* Variable %s has incorrect type*/ 
        dump_parameterlist(p,1);
      }
      erase_variable(v);
      v->pointer.a=(ARRAY *)p->pointer;
      v->flags=V_STATIC;
    } else if(p->typ==PL_SARRAYVAR) {
      if(atyp!=STRINGTYP) {
        xberror(58,v->name); /* Variable %s has incorrect type*/ 
        dump_parameterlist(p,1);
      }
      erase_variable(v);
      v->pointer.a=(ARRAY *)p->pointer;
      v->flags=V_STATIC;
    } else if(p->typ==PL_ARRAYVAR) {
      erase_variable(v);
      v->pointer.a=(ARRAY *)p->pointer;
      v->flags=V_STATIC;
    } else {
      xberror(58,v->name); /* Variable %s has incorrect type*/ 
      dump_parameterlist(p,1);
    }
  } else {  
    xberror(58,v->name); /* Variable %s has incorrect type*/  
    printf("zuweis_v_parameter: $%x->$%x can not convert.\n",p->typ,v->typ);
    dump_parameterlist(p,1);
  }
}

ISTATIC int vm_zuweis(int vnr,PARAMETER *sp) {    /*  */
  VERBOSE("vm_zuweis_%d\n",vnr);
  zuweis_v_parameter(&variablen[vnr],&sp[-1]);
  free_parameter(&sp[-1]);
  return(-1);
}

STATIC int vm_zuweisindex(int vnr,PARAMETER *sp,int dim) {    /*  */
  int *indexliste=NULL;
  VERBOSE("vm_suweisindex_%d_%d \n",vnr,dim);
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

STATIC void  push_v(PARAMETER *p, VARIABLE *v) {
  p->panzahl=0;
  if(v->typ==INTTYP) {
    p->typ=PL_INT;
    p->integer=*(v->pointer.i);
  } else if(v->typ==FLOATTYP) {
    p->typ=PL_FLOAT;
    p->real=*(v->pointer.f);
  } else if(v->typ==STRINGTYP) {
    p->typ=PL_STRING;
    *((STRING *)&(p->integer))=double_string(v->pointer.s);
  } else if(v->typ==ARRAYTYP) {
    p->typ=PL_ARRAY;
    *((ARRAY *)&(p->integer))=double_array(v->pointer.a);
  } else printf("pushv: Something is wrong, var <%s> $%x->$%x kann nicht konvertieren.\n",
  v->name,v->typ,p->typ);
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


#define CP4(d,s,l) { register char *dd=(char *)(d); \
                   register char *ss=(char *)(s); \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd=*ss; i+=l;}
#define CP2(d,s,l) { register char *dd=(char *)(d); \
                   register char *ss=(char *)(s); \
                 *dd++=*ss++; \
                 *dd=*ss; i+=l;}

#define CP8(d,s,l) { register char *dd=(char *)(d); \
                   register char *ss=(char *)(s); \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd++=*ss++; \
                 *dd=*ss; i+=l;}


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
    i++;
    switch(cmd) {
    case BC_NOOP:  
      VERBOSE("vm_noop ");
      break;
    case BC_JSR:
      CP4(&a,&bcpc.pointer[i],sizeof(int));
      VERBOSE("vm_jsr_%d\n",a);
      stack[sp]=i;
      i=a;
      break;
    case BC_JMP:
      CP4(&a,&bcpc.pointer[i],0);
      i=a;
      VERBOSE("vm_jmp_%d\n",a);
      break;
    case BC_JEQ:
      if((--opstack)->integer==0) {
        CP4(&a,&bcpc.pointer[i],0);
	i=a;
        VERBOSE("vm_jeq_%d ",a);
      } else {
        i+=sizeof(int);
        VERBOSE("vm_jeq_** ");
      }
      break;
    case BC_BRA:
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
      VERBOSE("vm_bra_%d \n",ss);
      i+=ss;
      break;
    case BC_BEQ:
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
      VERBOSE("vm_beq_%d ",ss);
      if((--opstack)->integer==0) i+=ss;
      break;
    case BC_BSR:
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
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
    case BC_PUSHF:
      CP8(&d,&bcpc.pointer[i],sizeof(double));
      opstack->real=d;
      opstack->typ=PL_FLOAT;
      opstack++;
      VERBOSE("%g ",d);
      break;
    case BC_PUSHI:
      CP4(&a,&bcpc.pointer[i],sizeof(int));
      opstack->integer=a;
      opstack->typ=PL_INT;
      opstack++;
      VERBOSE("%d ",a);
      break;
    case BC_LOADi:
      CP4(&a,&bcpc.pointer[i],sizeof(int));
      opstack->integer=*((int *)a);
      opstack->typ=PL_INT;
      opstack++;
      VERBOSE("[$%x].i ",a);
      break;
    case BC_LOADf:
      CP4(&a,&bcpc.pointer[i],sizeof(int));
      opstack->real=*((double *)a);
      opstack->typ=PL_FLOAT;
      opstack++;
      VERBOSE("[$%x].d ",a);
      break;
    case BC_PUSHW:
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
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
      CP4(&len,&bcpc.pointer[i],sizeof(int));
      CP4(&a,&bcpc.pointer[i],sizeof(int));
      opstack->integer=len;
      opstack->typ=PL_STRING;
      opstack->pointer=malloc(len+1);
      memcpy(opstack->pointer,rodata+a,len);
      ((char *)(opstack->pointer))[len]=0;
      opstack++;
      VERBOSE("\"%s\" ",(char *)opstack[-1].pointer);
      }
      break;
    case BC_PUSHA:  /*Array konstante auf STack....*/
      { int len;
        STRING str;
      CP4(&len,&bcpc.pointer[i],sizeof(int));
      CP4(&a,&bcpc.pointer[i],sizeof(int));
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
        CP4(&a,&bcpc.pointer[i],sizeof(int));
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
      opstack--;
      (opstack-1)->integer=(opstack-1)->integer | opstack->integer;
      break;
    case BC_XOR:
      VERBOSE("XOR ");
      opstack--;
      (opstack-1)->integer=(opstack-1)->integer ^ opstack->integer;
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
    case BC_DIV:
      VERBOSE("DIV ");
      opstack--;(opstack-1)->real/=opstack->real;
      break;
    case BC_POW:
      opstack+=vm_pow(opstack);
      break;
    case BC_AND:
      VERBOSE("AND ");
      opstack--;
      (opstack-1)->integer=opstack->integer & (opstack-1)->integer;
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
    case BC_NEG:
      vm_neg(opstack);
      break;
    case BC_NOT:
      VERBOSE("NOT ");
      (opstack-1)->integer=~(opstack-1)->integer;
      break;
    case BC_X2I:
      vm_x2i(opstack);
      break;
    case BC_X2F:
      vm_x2f(opstack);
      break;
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
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
      *(variablen[ss].pointer.i)=(--opstack)->integer;
      break;
    case BC_ZUWEISf:
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
      *(variablen[ss].pointer.f)=(--opstack)->real;
      break;
    case BC_ZUWEIS:
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
      opstack+=vm_zuweis(ss,opstack);
      break;
    case BC_PUSHVV:
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
      opstack+=vm_pushvv(ss,opstack);
      break;
    case BC_LOCAL:
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
      VERBOSE("dolocal_%d ",ss);
      do_local(ss,sp);
      break;
    case BC_ZUWEISINDEX:
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
      CP2(&ss2,&bcpc.pointer[i],sizeof(short));
      opstack+=vm_zuweisindex(ss,opstack,ss2);
      break;
    case BC_PUSHV:
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
      opstack+=vm_pushv(ss,opstack);
      
      break;
    case BC_PUSHARRAYELEM:      
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
      CP2(&ss2,&bcpc.pointer[i],sizeof(short));
      opstack+=vm_pusharrayelem(ss,opstack,ss2);
      break;
    case BC_PUSHVVI:
      CP2(&ss,&bcpc.pointer[i],sizeof(short));
      CP2(&ss2,&bcpc.pointer[i],sizeof(short));
      opstack+=vm_pushvvi(ss,opstack,ss2);
      break;
    case BC_RESTORE:
      CP4(&datapointer,&bcpc.pointer[i],sizeof(int));
      VERBOSE("vm_restore_%d\n",datapointer);
      break;
    case BC_EVAL:
      VERBOSE("vm_eval-%d ",a);
      opstack+=vm_eval(opstack);
      break;
    case BC_PUSHLABEL:
      CP4(&a,&bcpc.pointer[i],sizeof(int));
      opstack->integer=a;
      opstack->typ=PL_LABEL;
      opstack->arraytyp=1;
      opstack++;
      break;
    case BC_PUSHPROC: 
      CP4(&a,&bcpc.pointer[i],sizeof(int));
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
