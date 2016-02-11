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
#include "defs.h"
#include "x11basic.h"
#include "xbasic.h"
#include "parameter.h"
#include "variablen.h"
#include "functions.h"
#include "bytecode.h"
#include "array.h"
#include "io.h"

extern int verbose;
extern int datapointer;

char *rodata;

int vm_x2i(PARAMETER *sp) {    /* cast to integer */
  if(verbose) printf("x2i ");
  if(sp[-1].typ==PL_INT) return(0);
  else if(sp[-1].typ==PL_FLOAT) {
    sp[-1].integer=(int)sp[-1].real;
    sp[-1].typ=PL_INT;  
  } else printf("X2I: something is wrong.\n");
  return(0);
}
int vm_x2f(PARAMETER *sp) {    /* cast to integer */
  if(verbose) printf("x2f ");
  if(sp[-1].typ==PL_FLOAT) return(0);
  else if(sp[-1].typ==PL_INT) {
    sp[-1].real=(double)sp[-1].integer;
    sp[-1].typ=PL_FLOAT;  
  } else printf("X2F: something is wrong.\n");
  return(0);
}

int vm_add(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("vm_add ");
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
  } else {
    puts("ERROR: incompatible types for ADD.");
    free_parameter(sp);
  }
  return(-1);
}
int vm_sub(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("vm_sub ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          sp[-1].integer-=sp[0].integer;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) sp[-1].real-=sp[0].real;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT)   sp[-1].real-=(double)sp[0].integer;
  else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) {
    sp[-1].real=sp[0].real-(double)sp[-1].integer;
    sp[-1].typ=PL_FLOAT;
  } else {
    puts("ERROR: incompatible types for SUB.");
    free_parameter(sp);
  }
  return(-1);
}
int vm_mul(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("vm_mul ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          sp[-1].integer*=sp[0].integer;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) sp[-1].real*=sp[0].real;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT)   sp[-1].real*=(double)sp[0].integer;
  else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) {
    sp[-1].real=sp[0].real*(double)sp[-1].integer;
    sp[-1].typ=PL_FLOAT;
  } else {
    puts("ERROR: incompatible types for MUL.");
    free_parameter(sp);
  }
  return(-1);
}
int vm_pow(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("vm_pow ");
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
    puts("ERROR: incompatible types for POW.");
    free_parameter(sp);
  }
    sp[-1].typ=PL_FLOAT;
  return(-1);
}
#if 0
int vm_div(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("vm_div ");
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
    puts("ERROR: incompatible types for DIV.");
    free_parameter(sp);
  }
  return(-1);
}
#endif
int vm_mod(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("vm_mod ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT) {
   sp[-1].real=fmod((double)sp[-1].integer,(double)sp[0].integer);
   sp[-1].typ=PL_FLOAT;
  } else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) 
    sp[-1].real=fmod(sp[-1].real,sp[0].real);
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT)   
    sp[-1].real=fmod(sp[-1].real,(double)sp[0].integer);
  else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) {
    sp[-1].real=fmod((double)sp[-1].integer,sp[0].real);
    sp[-1].typ=PL_FLOAT;
  } else {
    puts("ERROR: incompatible types for MOD.");
    free_parameter(sp);
  }
  return(-1);
}


int vm_equal(PARAMETER *sp) {    /* binaer and */
  if(verbose) printf("vm_equal ");
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
    puts("ERROR: incompatible types for EQUAL.");
    free_parameter(sp);
  }
  sp[-1].typ=PL_INT;
  return(-1);
}
int vm_greater(PARAMETER *sp) {    /* binaer and */
  if(verbose) printf("vm_greater ");
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
    puts("ERROR: incompatible types for GREATER.");
    free_parameter(sp);
  }
  sp[-1].typ=PL_INT;
  return(-1);
}
int vm_less(PARAMETER *sp) {    /* binaer and */
  if(verbose) printf("vm_less ");
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
    puts("ERROR: incompatible types for LESS.");
    free_parameter(sp);
  }
  sp[-1].typ=PL_INT;
  return(-1);
}
int vm_sysvar(PARAMETER *sp,int n) {    /*  */
  if(verbose) printf("vm_%s ",sysvars[n].name);	   
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
int vm_ssysvar(PARAMETER *sp,int n) {    /*  */
  if(verbose) printf("vm_%s ",syssvars[n].name);
  STRING a=(syssvars[n].routine)();
  sp->integer=a.len;
  sp->pointer=a.pointer;
  sp->typ=PL_STRING;
  return(1);
}
int vm_asysvar(PARAMETER *sp,int n) {    /*  */
  if(verbose) printf("vm_%s ",sysvars[n].name);
printf("#####Sysvar ARRAY: %s not implemented\n",sysvars[n].name);
xberror(9,"Sysvar ARRAY"); /*Function or command %s not implemented*/
      sp->real=4711;
      sp->typ=PL_FLOAT;
      return(1);
}
int vm_dup(PARAMETER *sp) {    /*  */
  if(verbose) printf("vm_dup ");
  sp[0]=sp[-1];   
  if(sp->typ==PL_KEY || sp->typ==PL_STRING) {
    sp[0].pointer=malloc(sp[0].integer+1);
    memcpy(sp[0].pointer,sp[-1].pointer,sp[0].integer+1);
  } else if(sp->typ==PL_ARRAY) {
    printf("WARNING: Pointerinhalt muesste hier dupliziert werden !\n");    
  }
  return(1);
}
int vm_exch(PARAMETER *sp) {    /*  */
  if(verbose) printf("vm_exch ");
  sp[0]=sp[-1];   
  sp[-1]=sp[-2];   
  sp[-2]=sp[0];   
  return(0);
}
int vm_neg(PARAMETER *sp) {    /*  */
  if(verbose) printf("vm_neg ");
  if(sp[-1].typ==PL_INT)          sp[-1].integer=-sp[-1].integer;
  else if(sp[-1].typ==PL_FLOAT)   sp[-1].real=-sp[-1].real;
  else puts("ERROR: incompatible types for NEG.");
  return(0);
}
void cast_to_real(PARAMETER *sp) {
  if(verbose) printf("vm_cast-to-real ");
  if(sp->typ==PL_FLOAT) ;
  else if(sp->typ==PL_LEER) ;
  else if(sp->typ==PL_INT) {
    sp->typ=PL_FLOAT;
    sp->real=(double)sp->integer;
  } else printf("ERROR: CAST to real not possible for $%x/not implemented.\n",sp->typ);
}
void cast_to_int(PARAMETER *sp) {
  if(verbose) printf("vm_cast-to-int ");
  if(sp->typ==PL_INT) ;
  else if(sp->typ==PL_LEER) ;
  else if(sp->typ==PL_FLOAT) {
    sp->typ=PL_INT;
    sp->integer=(int)sp->real;
  } else puts("ERROR: CAST to int not possible/not implemented.");
}
int vm_sfunc(PARAMETER *sp,int i, int anzarg) {    /*  */
  if(verbose) printf("vm_%s(%d) ",psfuncs[i].name,anzarg);
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
    if(sp[0].typ==PL_KEY) {
      char *w2=sp[0].pointer;
      STRING s=(psfuncs[i].routine)(w2);
      free(w2);
      sp[0].pointer=s.pointer;
      sp[0].integer=s.len;
      sp[0].typ=PL_STRING;
      return 1-anzarg;
    } else {
      printf("ERROR: something is wrong with bytecode.\n");
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
    else {
      printf("Hier stimmt was nicht: falscher Typ fuer Funktion.\n");
    }
    STRING s=(psfuncs[i].routine)(a);
    free_parameter(sp);
    sp[0].pointer=s.pointer;
    sp[0].integer=s.len;
    sp[0].typ=PL_STRING;
    return 1-anzarg;
  }

  puts("ERROR: INCOMPLETE"
       ", diese Funktion bekommt ihre Parameter nicht.");
  return 1-anzarg;
}

int vm_func(PARAMETER *sp,int i, int anzarg) {    /*  */
  if(verbose) printf("vm_.%s(%d) ",pfuncs[i].name,anzarg);
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
    char *w2;
    w2=sp[0].pointer;
    if(pfuncs[i].opcode&F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(w2);
      sp[0].typ=PL_INT;
    } else {
      sp[0].real=(pfuncs[i].routine)(w2);
      sp[0].typ=PL_FLOAT;
    }
    free(w2);
    return 1-anzarg;
  }
  if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_DQUICK) {
    cast_to_real(&sp[0]);
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
    cast_to_real(&sp[0]);
    cast_to_real(&sp[1]);
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
    cast_to_int(&sp[0]);
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
    cast_to_int(&sp[0]);
    cast_to_int(&sp[1]);
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
    if(sp[0].typ==PL_STRING) {
      a.len=sp[0].integer;
      a.pointer=sp[0].pointer;
    } else 
    xberror(47,(char *)pfuncs[i].name); /*  Parameter %s falsch, kein String */
//    printf("Got a string: <%s>\n",a.pointer);
    if(pfuncs[i].opcode&F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(a);
      sp[0].typ=PL_INT;
    } else {
      sp[0].real=(pfuncs[i].routine)(a);
      sp[0].typ=PL_FLOAT;
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
  puts("ERROR: INCOMPLETE"
       ", diese Funktion bekommt ihre Parameter nicht.");

  return 1-anzarg;
}



int vm_comm(PARAMETER *sp,int i, int anzarg) {    /*  */
  if(verbose) printf("vm_%s(%d)_%d\n",comms[i].name,anzarg,i);
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
  puts("ERROR: INCOMPLETE"
       ", diese Funktion bekommt ihre Parameter nicht.");
  return -anzarg;
}
int vm_pushvv(int vnr,PARAMETER *sp) {    /*  */
  if(verbose) printf("vm_pushvv_%d\n",vnr);
    sp->integer=vnr;
  //  printf("vnr=%d\n",vnr);
    sp->pointer=varptr_indexliste(&variablen[vnr],NULL,0);
    if(verbose) printf("vm_pushvv_%d\n",vnr);
    if(variablen[vnr].typ==INTTYP) sp->typ=PL_IVAR;
    else if(variablen[vnr].typ==FLOATTYP) sp->typ=PL_FVAR;
    else if(variablen[vnr].typ==STRINGTYP) sp->typ=PL_SVAR;
    else sp->typ=PL_ALLVAR;
    return(1);
}

int vm_pushvvi(int vnr,PARAMETER *sp,int dim) {    /*  */
  int *indexliste=NULL;
  PARAMETER *p=&sp[-dim];
  //dump_parameterlist(p,dim);
  if(verbose) printf("vm_pushvvi_%d\n",vnr);

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
    printf("ERROR: pushvvi\n");
  }
  free(indexliste);
  return(-dim+1);
}



int vm_zuweis(int vnr,PARAMETER *sp) {    /*  */
  if(verbose) printf("vm_zuweis_%d\n",vnr);
  zuweis_v_parameter(&variablen[vnr],&sp[-1]);
  free_parameter(&sp[-1]);
  return(-1);
}


/* Weise aus parameter zu einer Vaiable */

void zuweis_v_parameter(VARIABLE *v,PARAMETER *p) {
  if(v->typ==INTTYP && p->typ==PL_INT)          *(v->pointer.i)=p->integer;
  else if(v->typ==INTTYP && p->typ==PL_FLOAT)   *(v->pointer.i)=(int)p->real;
  else if(v->typ==FLOATTYP && p->typ==PL_FLOAT) *(v->pointer.f)=p->real;
  else if(v->typ==FLOATTYP && p->typ==PL_INT)   *(v->pointer.f)=(double)p->integer;
  else if(v->typ==STRINGTYP && p->typ==PL_STRING) {
    free(v->pointer.s->pointer);
    *(v->pointer.s)=double_string((STRING *)&(p->integer));
  } else if(v->typ==ARRAYTYP && p->typ==PL_ARRAY) {
    free_array(v->pointer.a);
    *(v->pointer.a)=double_array((ARRAY *)&(p->integer));
  } else {
    printf("zuweis_v_parameter: $%x->$%x kann nicht konvertieren.\n",p->typ,v->typ);
    dump_parameterlist(p,1);
  }
}
int vm_zuweisindex(int vnr,PARAMETER *sp,int dim) {    /*  */
  int *indexliste=NULL;
  if(verbose) printf("vm_suweisindex_%d_%d \n",vnr,dim);
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

int vm_pusharrayelem(int vnr, PARAMETER *sp, int dim) {    /*  */
  int typ;
  int *indexliste;
  typ=variablen[vnr].typ;
  if(verbose) printf("vm_pusharrayelem_%d_%d ",vnr,dim);

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
      } else printf("Something is wrong.\n"); 
      free(indexliste);
    } else printf("ARRAYELEM: something is wrong.\n");

  } else printf("ARRAYELEM: something is wrong.\n");

  return(-dim+1);
}

int vm_pushv(int vnr,PARAMETER *sp) {    /*  */
  if(verbose) printf("vm_pushv_%d ",vnr);
 // printf("vnr=%d\n",vnr);
  push_v(sp,&variablen[vnr]);
  return(1);
}
void  push_v(PARAMETER *p, VARIABLE *v) {
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




int vm_eval(PARAMETER *sp) {    /*  */
  if(verbose) printf("vm_eval ");
  if(sp[-1].typ==PL_STRING) {
    if(verbose) printf("vm_eval_<%s>\n",(char *)sp[-1].pointer);
    kommando(sp[-1].pointer);
    free(sp[-1].pointer);
  } else {
    printf("ERROR: something is wrong with bytecode, no string.\n");
    free_parameter(&sp[-1]);
  }
  return(-1);  
}


PARAMETER *virtual_machine(STRING bcpc, int *npar) {
  PARAMETER *opstack=calloc(BC_STACKLEN,sizeof(PARAMETER));
  PARAMETER *osp=opstack;
  unsigned char cmd;
  int i=0,j;
  int a,n;
  short ss,ss2;
  double d;
  char *buf;



  while((cmd=bcpc.pointer[i]) && i<bcpc.len) {
    i++;
     switch(cmd) {
    case BC_NOOP:  
      if(verbose) printf("vm_noop ");
      break;
    case BC_JSR:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));i+=sizeof(int);
      if(verbose) printf("vm_jsr_%d\n",a);
      stack[sp]=i;
      i=a;
      break;
    case BC_JMP:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));i+=sizeof(int);
      if(verbose) printf("vm_jmp_%d\n",a);
      i=a;
      break;
    case BC_JEQ:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));i+=sizeof(int);
      if(verbose) printf("vm_jeq_%d ",a);
      if((--opstack)->integer==0) i=a;
      break;
    case BC_BRA:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      if(verbose) printf("vm_bra_%d \n",ss);
      i+=ss;
      break;
    case BC_BEQ:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      if(verbose) printf("vm_beq_%d ",ss);
      if((--opstack)->integer==0) i+=ss;
      break;
    case BC_BSR:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      if(verbose) printf("vm_bsr_%d \n",ss);
      stack[sp]=i;
      i+=ss;
      break;
    case BC_BLKSTART:
      if(verbose) printf("vm_{ ");
      sp++;
      break;
    case BC_BLKEND:
      if(verbose) printf("vm_} \n");
      restore_locals(sp--);
      break;
    case BC_RTS:
      if(verbose) printf("vm_rts \n");
      i=stack[sp];
      break;
    case BC_BRAs:
      if(verbose) printf("vm_bra.s_%d \n",bcpc.pointer[i]); 
      i+=(bcpc.pointer[i++]);
      break;
    case BC_BEQs:
      ss=bcpc.pointer[i++];
      if(verbose) printf("vm_beqs_%d ",ss);
      if((--opstack)->integer==0) i+=ss;
      break;
    case BC_PUSHF:
      memcpy(&d,&bcpc.pointer[i],sizeof(double));i+=sizeof(double);
      opstack->real=d;
      opstack->typ=PL_FLOAT;
      opstack++;
      if(verbose) printf("%g ",d);
      break;
    case BC_PUSHI:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));i+=sizeof(int);
      opstack->integer=a;
      opstack->typ=PL_INT;
      opstack++;
      if(verbose) printf("%d ",a);
      break;
    case BC_LOADi:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));i+=sizeof(int);
      opstack->integer=*((int *)a);
      opstack->typ=PL_INT;
      opstack++;
      if(verbose) printf("[$%x].i ",a);
      break;
    case BC_LOADf:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));i+=sizeof(int);
      opstack->real=*((double *)a);
      opstack->typ=PL_FLOAT;
      opstack++;
      if(verbose) printf("[$%x].d ",a);
      break;
    case BC_PUSHW:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      opstack->integer=ss;
      opstack->typ=PL_INT;
      opstack++;
      if(verbose) printf("%d ",ss);
      break;
    case BC_PUSHB:
      opstack->integer=bcpc.pointer[i++];
      opstack->typ=PL_INT;
      opstack++;
      if(verbose) printf("%d ",opstack[-1].integer);
      break;
    case BC_PUSHLEER:
      opstack->typ=PL_LEER;
      opstack++;
      if(verbose) printf("<empty> ");
      break;
    case BC_PUSH0:
      opstack->integer=0;
      opstack->typ=PL_INT;
      opstack++;
      if(verbose) printf("0 ");
      break;
    case BC_PUSH1:
      opstack->integer=1;
      opstack->typ=PL_INT;
      opstack++;
      if(verbose) printf("1 ");
      break;
    case BC_PUSH2:
      opstack->integer=2;
      opstack->typ=PL_INT;
      opstack++;
      if(verbose) printf("2 ");
      break;
    case BC_PUSHM1:
      opstack->integer=-1;
      opstack->typ=PL_INT;
      opstack++;
      if(verbose) printf("-1 ");
      break;
    case BC_PUSHS:  /*String konstante auf STack....*/
      { int len;
      memcpy(&len,&bcpc.pointer[i],sizeof(int));i+=sizeof(int);
      memcpy(&a,&bcpc.pointer[i],sizeof(int));i+=sizeof(int);
      opstack->integer=len;
      opstack->typ=PL_STRING;
      opstack->pointer=malloc(len+1);
      memcpy(opstack->pointer,rodata+a,len);
      ((char *)(opstack->pointer))[len]=0;
      opstack++;
      if(verbose) printf("\"%s\" ",(char *)opstack[-1].pointer);
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
      if(verbose) printf("SP=%d\n",((int)opstack-(int)osp)/sizeof(PARAMETER));  
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
        memcpy(&a,&bcpc.pointer[i],sizeof(int));i+=sizeof(int);
        buf=malloc(len+1);
        memcpy(buf,rodata+a,len);
        buf[len]=0;
        opstack->integer=len;
        opstack->typ=PL_KEY;
        opstack->pointer=buf;
        opstack++;
        if(verbose) printf("%s ",buf);
      }
      break;
    case BC_COMMENT:
      a=bcpc.pointer[i++];
      buf=malloc(a+1);
      memcpy(buf,&bcpc.pointer[i],a);
      buf[a]=0;
      if(verbose) printf("%s ",buf);
      i+=a;
      free(buf);
      break;
    case BC_ADD:
      opstack+=vm_add(opstack);
      break;
    case BC_ADDi:
      if(verbose) printf("ADDi ");
      opstack--;(opstack-1)->integer+=opstack->integer;
      break;
    case BC_ADDf:
      if(verbose) printf("ADDf ");
      opstack--;(opstack-1)->real+=opstack->real;
      break;
    case BC_ADDs:
      if(verbose) printf("ADDs ");
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
      if(verbose) printf("OR ");
      opstack--;
      (opstack-1)->integer=(opstack-1)->integer | opstack->integer;
      break;
    case BC_XOR:
      if(verbose) printf("XOR ");
      opstack--;
      (opstack-1)->integer=(opstack-1)->integer ^ opstack->integer;
      break;
    case BC_SUB:
      opstack+=vm_sub(opstack);
      break;
    case BC_SUBi:
      if(verbose) printf("SUBi ");
      opstack--;(opstack-1)->integer-=opstack->integer;
      break;
    case BC_SUBf:
      if(verbose) printf("SUBf ");
      opstack--;(opstack-1)->real-=opstack->real;
      break;
    case BC_MUL:
      opstack+=vm_mul(opstack);
      break;
    case BC_MULi:
      if(verbose) printf("MULi ");
      opstack--;(opstack-1)->integer*=opstack->integer;
      break;
    case BC_MULf:
      if(verbose) printf("MULf ");
      opstack--;(opstack-1)->real*=opstack->real;
      break;
    case BC_DIV:
      if(verbose) printf("DIV ");
      opstack--;(opstack-1)->real/=opstack->real;
      break;
    case BC_POW:
      opstack+=vm_pow(opstack);
      break;
    case BC_AND:
      if(verbose) printf("AND ");
      (opstack-1)->integer=(opstack-1)->integer & (opstack-2)->integer;
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
      if(verbose) printf("vm_clear ");
      break;
    case BC_COUNT:
      a=((int)opstack-(int)osp)/sizeof(PARAMETER);
      opstack->integer=a;
      opstack->typ=PL_INT;
      opstack++;
      if(verbose) printf("vm_count_%d ",a);
      break;
    case BC_NEG:
      vm_neg(opstack);
      break;
    case BC_NOT:
      if(verbose) printf("NOT ");
      (opstack-1)->integer=~(opstack-1)->integer;
      break;
    case BC_X2I:
      vm_x2i(opstack);
      break;
    case BC_X2F:
      vm_x2f(opstack);
      break;
    case BC_MOD:
      if(verbose) printf("MOD ");
      opstack+=vm_mod(opstack);
      break;
    case BC_POP:
      if(verbose) printf("POP ");
      opstack--;
      free_parameter(opstack);
      break;
    case BC_ZUWEIS:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      opstack+=vm_zuweis(ss,opstack);
      break;
    case BC_PUSHVV:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      opstack+=vm_pushvv(ss,opstack);
      break;
    case BC_LOCAL:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      if(verbose) printf("dolocal_%d ",ss);
      do_local(ss,sp);
      break;
    case BC_ZUWEISINDEX:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      memcpy(&ss2,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      opstack+=vm_zuweisindex(ss,opstack,ss2);
      break;
    case BC_PUSHV:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      opstack+=vm_pushv(ss,opstack);
      break;
    case BC_PUSHARRAYELEM:      
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      memcpy(&ss2,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      opstack+=vm_pusharrayelem(ss,opstack,ss2);
      break;
    case BC_PUSHVVI:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      memcpy(&ss2,&bcpc.pointer[i],sizeof(short));i+=sizeof(short);
      opstack+=vm_pushvvi(ss,opstack,ss2);
      break;
    case BC_RESTORE:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));i+=sizeof(int);
      if(verbose) printf("vm_restore_%d\n",a);
      datapointer=a;
      break;
    case BC_EVAL:
      if(verbose) printf("vm_eval-%d ",a);
      opstack+=vm_eval(opstack);
      break;
    default:
      printf("VM: BC_ILLEGAL instruction %2x at %d\n",(int)cmd,i);
      memdump((unsigned char *)&(bcpc.pointer[i]),16);
      xberror(104,""); /* Illegal Instruction */

      *npar=((int)opstack-(int)osp)/sizeof(PARAMETER);  
      return(osp);
    }
  }
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
  va_list arguments=*arg;
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
    *arg=arguments;
}
int pusharg(PARAMETER **opstack, char *typ,...)   {
  int count=0;
  unsigned char c;
  va_list arguments;    
  va_start ( arguments, typ ); 
  while(c=typ[count++]) do_pusharg(&arguments,c,opstack);
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
  while(c=typ[count++]) do_pusharg(&arguments,c,opstack);
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
  while(c=typ[count++]) do_pusharg(&arguments,c,opstack);
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
  while(c=typ[count++]) do_pusharg(&arguments,c,opstack);
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
  while(c=typ[count++]) do_pusharg(&arguments,c,opstack);
  va_end ( arguments );                  // Cleans up the list
  (*opstack)->integer=count; (*opstack)->typ=PL_INT; (*opstack)++;
  name();
  return(*((ARRAY *)&(osp->integer)));
}



