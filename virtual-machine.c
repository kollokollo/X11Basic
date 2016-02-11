/* VIRTUAL-MACHINE.C (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "config.h"
#include "defs.h"
#include "globals.h"
#include "protos.h"
#include "functions.h"
#include "ptypes.h"
#include "bytecode.h"

extern int verbose;
extern int datapointer;

int bc_not(PARAMETER *sp) {    /* Logisches NOT, INPUT: [float|int] RET: int */
  if(verbose) printf("bc_not ");
  if(sp[-1].typ==PL_INT)          sp[-1].integer=~sp[-1].integer;
  else if(sp[-1].typ==PL_FLOAT) {
    sp[-1].integer=~((int)sp[-1].real);
    sp[-1].typ=PL_INT;
  } else puts("ERROR: incompatible types for NOT.");
  return(0);
}
int bc_add(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("bc_add ");
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
    free_parameter(sp[0]);
  }
  return(-1);
}
int bc_sub(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("bc_sub ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          sp[-1].integer-=sp[0].integer;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) sp[-1].real-=sp[0].real;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT)   sp[-1].real-=(double)sp[0].integer;
  else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) {
    sp[-1].real=sp[0].real-(double)sp[-1].integer;
    sp[-1].typ=PL_FLOAT;
  } else {
    puts("ERROR: incompatible types for SUB.");
    free_parameter(sp[0]);
  }
  return(-1);
}
int bc_mul(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("bc_mul ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          sp[-1].integer*=sp[0].integer;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) sp[-1].real*=sp[0].real;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT)   sp[-1].real*=(double)sp[0].integer;
  else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) {
    sp[-1].real=sp[0].real*(double)sp[-1].integer;
    sp[-1].typ=PL_FLOAT;
  } else {
    puts("ERROR: incompatible types for MUL.");
    free_parameter(sp[0]);
  }
  return(-1);
}
int bc_pow(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("bc_pow ");
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
    free_parameter(sp[0]);
  }
    sp[-1].typ=PL_FLOAT;
  return(-1);
}
int bc_div(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("bc_div ");
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
    free_parameter(sp[0]);
  }
  return(-1);
}
int bc_mod(PARAMETER *sp) {    /* binaer addition */
  if(verbose) printf("bc_mod ");
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
    free_parameter(sp[0]);
  }
  return(-1);
}
int bc_and(PARAMETER *sp) {    /* binaer and */
  if(verbose) printf("bc_and ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          sp[-1].integer&=sp[0].integer;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) {
    sp[-1].integer=((int)sp[-1].real & (int)sp[0].real);
    sp[-1].typ=PL_INT;
  } else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT) {
    sp[-1].integer=((int)sp[-1].real & sp[0].integer);
    sp[-1].typ=PL_INT;
  } else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) sp[-1].integer&=(int)sp[0].real;
  else {
    puts("ERROR: incompatible types for AND.");
    free_parameter(sp[0]);
  }
  return(-1);
}
int bc_or(PARAMETER *sp) {    /* binaer and */
  if(verbose) printf("bc_or ");
  sp--;
  if(sp[-1].typ==PL_INT && sp[0].typ==PL_INT)          sp[-1].integer|=sp[0].integer;
  else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_FLOAT) {
    sp[-1].integer=((int)sp[-1].real | (int)sp[0].real);
    sp[-1].typ=PL_INT;
  } else if(sp[-1].typ==PL_FLOAT && sp[0].typ==PL_INT) {
    sp[-1].integer=((int)sp[-1].real | sp[0].integer);
    sp[-1].typ=PL_INT;
  } else if(sp[-1].typ==PL_INT && sp[0].typ==PL_FLOAT) sp[-1].integer|=(int)sp[0].real;
  else {
    puts("ERROR: incompatible types for OR.");
    free_parameter(sp[0]);
  }
  return(-1);
}
int bc_equal(PARAMETER *sp) {    /* binaer and */
  if(verbose) printf("bc_equal ");
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
    free_parameter(sp[0]);
  }
  sp[-1].typ=PL_INT;
  return(-1);
}
int bc_greater(PARAMETER *sp) {    /* binaer and */
  if(verbose) printf("bc_greater ");
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
    free_parameter(sp[0]);
    free_parameter(sp[-1]);
    sp[-1].integer=(v>0)?-1:0;
  }  else {
    puts("ERROR: incompatible types for GREATER.");
    free_parameter(sp[0]);
  }
  sp[-1].typ=PL_INT;
  return(-1);
}
int bc_less(PARAMETER *sp) {    /* binaer and */
  if(verbose) printf("bc_greater ");
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
    free_parameter(sp[0]);
    free_parameter(sp[-1]);
    sp[-1].integer=(v>0)?-1:0;
  } else {
    puts("ERROR: incompatible types for LESS.");
    free_parameter(sp[0]);
  }
  sp[-1].typ=PL_INT;
  return(-1);
}
int bc_sysvar(PARAMETER *sp,int n) {    /*  */
  if(verbose) printf("bc_%s ",sysvars[n].name);	   
  if((sysvars[n].opcode)==PL_INT) {
    sp->integer=((int (*)())sysvars[n].routine)();
    sp->typ=PL_INT;
  } else if((sysvars[n].opcode)==PL_FLOAT) {
    sp->real=(sysvars[n].routine)();
    sp->typ=PL_FLOAT;
  } else {
    sp->real=0;
    sp->typ=PL_FLOAT;
  }
  return(1);
}
int bc_ssysvar(PARAMETER *sp,int n) {    /*  */
  if(verbose) printf("bc_%s ",syssvars[n].name);
  STRING a=(syssvars[n].routine)();
  sp->integer=a.len;
  sp->pointer=a.pointer;
  sp->typ=PL_STRING;
  return(1);
}
int bc_asysvar(PARAMETER *sp,int n) {    /*  */
  if(verbose) printf("bc_%s ",sysvars[n].name);
printf("#####Sysvar ARRAY: %s not implemented\n",sysvars[n].name);
      sp->real=4711;
      sp->typ=PL_FLOAT;
      return(1);
}
int bc_dup(PARAMETER *sp) {    /*  */
  if(verbose) printf("bc_dup ");
  sp[0]=sp[-1];   /* Achtung, der pointerinhalt muss kopiert werden !*/
  return(1);
}
int bc_exch(PARAMETER *sp) {    /*  */
  if(verbose) printf("bc_exch ");
  sp[0]=sp[-1];   
  sp[-1]=sp[-2];   
  sp[-2]=sp[0];   
  return(0);
}
int bc_neg(PARAMETER *sp) {    /*  */
  if(verbose) printf("bc_neg ");
  if(sp[-1].typ==PL_INT)          sp[-1].integer=-sp[-1].integer;
  else if(sp[-1].typ==PL_FLOAT)   sp[-1].real=-sp[-1].real;
  else puts("ERROR: incompatible types for NEG.");
  return(0);
}
void cast_to_real(PARAMETER *sp) {
  if(sp->typ==PL_FLOAT) ;
  else if(sp->typ==PL_INT) {
    sp->typ=PL_FLOAT;
    sp->real=(double)sp->integer;
  } else puts("ERROR: CAST to real not possible/not implemented.");
}
void cast_to_int(PARAMETER *sp) {
  if(sp->typ==PL_INT) ;
  else if(sp->typ==PL_FLOAT) {
    sp->typ=PL_INT;
    sp->integer=(int)sp->real;
  } else puts("ERROR: CAST to int not possible/not implemented.");
}
int bc_sfunc(PARAMETER *sp,int i, int anzarg) {    /*  */
  if(verbose) printf("bc_%s(%d) ",psfuncs[i].name,anzarg);
  if(anzarg<psfuncs[i].pmin) {
    xberror(42,psfuncs[i].name); /* Zu wenig Parameter  */
    return 1-anzarg;
  }
  if(anzarg>psfuncs[i].pmax && !(psfuncs[i].pmax==-1)) {
    xberror(45,psfuncs[i].name); /* Zu viele Parameter  */
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
    int j;
    PARAMETER *plist;
    short *pliste=(short *)psfuncs[i].pliste;
    STRING s;
    plist=&sp[0];
     /* Jetzt muessen die Parameter gecastet werden, falls noetig */
    for(j=0;j<anzarg;j++) {
      if(pliste[j]==plist[j].typ) {
        if(verbose) printf("Par#%d-->OK ",j);
      } else {
        if((pliste[j]==PL_FLOAT || pliste[j]==PL_NUMBER ) && plist[j].typ==PL_INT) {
          plist[j].typ=PL_FLOAT;
	  plist[j].real=(double)plist[j].integer;
          if(verbose) printf("Par#%d-->CAST ",j);
        } else if((pliste[j]==PL_FLOAT || pliste[j]==PL_NUMBER ) && plist[j].typ==PL_FLOAT) {
          if(verbose) printf("Par#%d-->*OK ",j);
        } else if(pliste[j]==PL_INT && plist[j].typ==PL_FLOAT) {
          plist[j].typ=PL_INT;
	  plist[j].integer=(int)plist[j].real;
          if(verbose) printf("Par#%d-->CAST ",j);
        } else if(pliste[j]==PL_FILENR && plist[j].typ==PL_INT) {
          if(verbose) printf("Par#%d-->*OK ",j);
        } else if(plist[j].typ==PL_LEER) {
          if(verbose) printf("Par#%d-<empty>->OK ",j);
	} else
          printf("\nPar#%d %x %x-->???\n",j,pliste[j],plist[j].typ);
      }
    }
    s=(psfuncs[i].routine)(plist,anzarg);
    if(anzarg) {  for(j=0;j<anzarg;j++)  free_parameter(plist[j]);}
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
      xberror(47,pfuncs[i].name); /*  Parameter %s falsch, kein String */
    
    STRING s=(psfuncs[i].routine)(a);
    free_parameter(sp[0]);
    sp[0].pointer=s.pointer;
    sp[0].integer=s.len;
    sp[0].typ=PL_STRING;
    return 1-anzarg;
  }
  if((psfuncs[i].opcode&FM_TYP)==F_IQUICK) {
    int a;
    if(sp[0].typ==PL_INT)        a=sp[0].integer;
    else if(sp[0].typ==PL_FLOAT) a=(int)sp[0].real;
    else {
      printf("Hier stimmt was nicht: falscher Typ fuer Funktion.\n");
    }
    STRING s=(psfuncs[i].routine)(a);
    free_parameter(sp[0]);
    sp[0].pointer=s.pointer;
    sp[0].integer=s.len;
    sp[0].typ=PL_STRING;
    return 1-anzarg;
  }

  puts("ERROR: INCOMPLETE"
       ", diese Funktion bekommt ihre Parameter nicht.");
  return 1-anzarg;
}

int bc_func(PARAMETER *sp,int i, int anzarg) {    /*  */
  if(verbose) printf("bc_.%s(%d) ",pfuncs[i].name,anzarg);
  if(anzarg<pfuncs[i].pmin) {
    xberror(42,pfuncs[i].name); /* Zu wenig Parameter  */
    return 1-anzarg;
  }
  if((anzarg>pfuncs[i].pmax && !(pfuncs[i].pmax==-1))) {
    xberror(45,pfuncs[i].name); /* Zu viele Parameter  */
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
      xberror(47,pfuncs[i].name); /*  Parameter %s falsch, kein String */
    
    if(pfuncs[i].opcode&F_IRET) {
      sp[0].integer=((int (*)())pfuncs[i].routine)(a);
      sp[0].typ=PL_INT;
    } else {
      sp[0].real=(pfuncs[i].routine)(a);
      sp[0].typ=PL_FLOAT;
    }
    free_parameter(sp[0]);
    return 1-anzarg;
  }
  if((pfuncs[i].opcode&FM_TYP)==F_PLISTE) {
    int j;
    PARAMETER *plist,rpar;
    short *pliste=(short *)pfuncs[i].pliste;
    plist=&sp[0];
    /* Jetzt muessen die Parameter gecastet werden, falls noetig */
    for(j=0;j<anzarg;j++) {
      if(pliste[j]==plist[j].typ) {
        if(verbose) printf("Par %d OK\n",j);
      } else {
        if((pliste[j]==PL_FLOAT || pliste[j]==PL_NUMBER ) && plist[j].typ==PL_INT) {
          plist[j].typ=PL_FLOAT;
	  plist[j].real=(double)plist[j].integer;
          if(verbose) printf("Par %d CAST\n",j);
        } else if((pliste[j]==PL_FLOAT || pliste[j]==PL_NUMBER ) && plist[j].typ==PL_FLOAT) {
          if(verbose) printf("Par %d *OK\n",j);
        } else if(pliste[j]==PL_INT && plist[j].typ==PL_FLOAT) {
          plist[j].typ=PL_INT;
	  plist[j].integer=(int)plist[j].real;
          if(verbose) printf("Par %d CAST\n",j);
        } else if(pliste[j]==PL_FILENR && plist[j].typ==PL_INT) {
          if(verbose) printf("Par %d *OK\n",j);
        } else if(plist[j].typ==PL_LEER) {
          if(verbose) printf("Par %d <empty> OK\n",j);
	} else
          printf("Par %d %x %x\n",j,pliste[j],plist[j].typ);
      }
    }



    if(pfuncs[i].opcode&F_IRET) {
      rpar.integer=((int (*)())pfuncs[i].routine)(plist,anzarg);
      rpar.typ=PL_INT;
    } else {
      rpar.real=(pfuncs[i].routine)(plist,anzarg);
      rpar.typ=PL_FLOAT;
    }
    if(anzarg) {  for(j=0;j<anzarg;j++)  free_parameter(plist[j]);}
    sp[0]=rpar;
    return 1-anzarg;
  }
  puts("ERROR: INCOMPLETE"
       ", diese Funktion bekommt ihre Parameter nicht.");

  return 1-anzarg;
}
int bc_comm(PARAMETER *sp,int i, int anzarg) {    /*  */
  if(verbose) printf("bc_%s(%d)_%d\n",comms[i].name,anzarg,i);
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
  if(comms[i].opcode&P_IGNORE) return -anzarg;
  if((comms[i].opcode&PM_TYP)==P_ARGUMENT) {
    char *w2;
    w2=sp[-1].pointer;
    (comms[i].routine)(w2);    
    free_parameter(sp[-1]);
    return -anzarg;
  }  
  if((comms[i].opcode&PM_TYP)==P_SIMPLE) {
    (comms[i].routine)();
    return -anzarg;
  }      
  if((comms[i].opcode&PM_TYP)==P_PLISTE) {  
    int j;
    PARAMETER *plist;
    short *pliste=(short *)comms[i].pliste;
    plist=&sp[-anzarg];
    /* Jetzt muessen die Parameter gecastet werden, falls noetig */
    for(j=0;j<anzarg;j++) {
      if(pliste[j]==plist[j].typ) {
        if(verbose) printf("Par %d OK\n",j);
      } else {
        if((pliste[j]==PL_FLOAT || pliste[j]==PL_NUMBER ) && plist[j].typ==PL_INT) {
          plist[j].typ=PL_FLOAT;
	  plist[j].real=(double)plist[j].integer;
          if(verbose) printf("Par %d CAST\n",j);
        } else if((pliste[j]==PL_FLOAT || pliste[j]==PL_NUMBER ) && plist[j].typ==PL_FLOAT) {
          if(verbose) printf("Par %d *OK\n",j);
        } else if(pliste[j]==PL_INT && plist[j].typ==PL_FLOAT) {
          plist[j].typ=PL_INT;
	  plist[j].integer=(int)plist[j].real;
          if(verbose) printf("Par %d CAST\n",j);
        } else if(pliste[j]==PL_FILENR && plist[j].typ==PL_INT) {
          if(verbose) printf("Par %d *OK\n",j);
        } else if(plist[j].typ==PL_LEER) {
          if(verbose) printf("Par %d <empty> OK\n",j);
	} else
          printf("Par %d %x %x\n",j,pliste[j],plist[j].typ);
      }
    }
    (comms[i].routine)(plist,anzarg);
    if(anzarg) {  for(j=0;j<anzarg;j++)  free_parameter(plist[j]);}
    return -anzarg;
  } 
  puts("ERROR: INCOMPLETE"
       ", diese Funktion bekommt ihre Parameter nicht.");
  return -anzarg;
}
int bc_pushvv(PARAMETER *sp) {    /*  */
  char *key,*buf;
  int typ,vnr;
  if(sp[-1].typ==PL_KEY) {
    key=malloc(sp[-1].integer+1);
    memcpy(key,sp[-1].pointer,sp[-1].integer);
    key[sp[-1].integer]=0;
    if(verbose) printf("bc_pushvv_%s\n",key);
    buf=indirekt2(key);
    free(key);
    sp[-1].integer=typ=vartype(buf);
    if(typ & ARRAYTYP) {
      printf("pushvv: Arrays gehen noch nicht.\n");
      exit(0);
    } else if(typ & STRINGTYP) {
      printf("pushvv: Stringvariablen gehen noch nicht.\n");
      exit(0);
    } else if(typ & INTTYP) {
      printf("pushvv: Intvariablen gehen noch nicht.\n");
      exit(0);
    } else {  
      char *r=varrumpf(buf);

      vnr=variable_exist_or_create(r,typ);
      if(verbose) printf("Variable %s: vnr=%d,typ=%d\n",buf,vnr,typ);
      if(!(sp[-1].integer&FLOATTYP) && !(sp[-1].integer&INTTYP)) xberror(58,buf); /* Variable hat falschen Typ */
      sp[-1].pointer=varptr(buf);
      if(sp[-1].pointer==NULL) printf("Fehler bei varptr.\n");
      free(r);
      sp[-1].typ=PL_NVAR;
    }
    
    return(0);
  } else {
    printf("pushvv: something is wrong,\n");
    exit(0);
  }
}
int bc_zuweis(PARAMETER *sp) {    /*  */
  char *key,*buf;
  int typ;
  if(sp[-1].typ=PL_KEY) {
    key=malloc(sp[-1].integer+1);
    memcpy(key,sp[-1].pointer,sp[-1].integer);
    key[sp[-1].integer]=0;
    if(verbose) printf("bc_zuweis_%s\n",key);
    buf=indirekt2(key);
    free(key);
    free_parameter(sp[-1]);
    typ=vartype(buf);
    if(typ & ARRAYTYP) {
      ARRAY a;
      if(sp[-2].typ==PL_ARRAY) {
        a.typ=sp[-2].typ;
        a.dimension=sp[-2].integer;
        a.pointer=sp[-2].pointer;
        array_zuweis_and_free(buf,a);
        /* free_parameter(sp[-2]); nicht mehr noetig */
      } else printf("ERROR: Typen ungleich... %d %d\n",typ,sp[-2].typ);
    } else if(typ & STRINGTYP) {
      STRING a;
      if(sp[-2].typ==PL_STRING) {
        a.len=sp[-2].integer;
        a.pointer=sp[-2].pointer;
        zuweis_string(buf,a);
        free_parameter(sp[-2]);
      } else printf("ERROR: Typen ungleich... %d %d\n",typ,sp[-2].typ);
    } else if(typ & INTTYP) {
      cast_to_int(&sp[-2]);
      zuweisi(buf,sp[-2].integer);
    } else {
      cast_to_real(&sp[-2]);
      zuweis(buf,sp[-2].real);
    }
    free(buf);    
    return(-2);
  } else {
    printf("ERROR: something is wrong with bytecode.\n");
  }
  free_parameter(sp[-1]);
  return(-1);  
}
int bc_zuweisindex(PARAMETER *sp,int dim) {    /*  */
  char *key;
  int index[dim];
  int i,vnr;
  if(sp[-1].typ==PL_KEY) {
    key=malloc(sp[-1].integer+1);
    memcpy(key,sp[-1].pointer,sp[-1].integer);
    key[sp[-1].integer]=0;
    if(verbose) printf("bc_zuweis_index_%s_%d(",key,dim);
    free(sp[-1].pointer);
    /* Index-Liste holen */
    for(i=0;i<dim;i++) {
      if(sp[-1-dim+i].typ==PL_INT) index[i]=sp[-1-dim+i].integer;
      else if(sp[-1-dim+i].typ==PL_FLOAT) index[i]=(int)sp[-1-dim+i].real;
      else printf("ERROR!! ");
      if(verbose) printf("%d,",index[i]);  
    }
    if(verbose) printf(") ");
    sp-=dim+1;
    
    /* typ von KEY ?*/
    if(key[strlen(key)-1]=='%') {
      key[strlen(key)-1]=0;
      if((vnr=variable_exist(key,INTARRAYTYP))!=-1) {
        if(dim!=variablen[vnr].opcode) xberror(18,"");  /* Falsche Anzahl Indizies */
      /*  printf("Zuweis: %d %d %g \n",sp[-1].typ,sp[-1].integer,sp[-1].real);*/
        if(sp[-1].typ==PL_FLOAT) zuweisibyindex(vnr,index,(int)sp[-1].real);
        else if(sp[-1].typ==PL_INT) zuweisibyindex(vnr,index,sp[-1].integer);
        else printf("ERROR: Zuweis, Typen ungleich.\n");
      } else xberror(15,key);  /* Feld nicht dimensioniert  */
    } else if(key[strlen(key)-1]=='$') {
      key[strlen(key)-1]=0;
      
      if((vnr=variable_exist(key,STRINGARRAYTYP))!=-1) {
        STRING s;
        if(dim!=variablen[vnr].opcode) xberror(18,"");  /* Falsche Anzahl Indizies */
        if(sp[-1].typ==PL_STRING) {
	  s.pointer=sp[-1].pointer;
	  s.len=sp[-1].integer;
        /*  printf("Zuweis: %d <%s> \n",sp[-1].typ,s.pointer);*/
 	  zuweissbyindex(vnr,index,s);
	  free(s.pointer);
	} else printf("ERROR: Zuweis, Typen ungleich.\n");
      } else xberror(15,key);  /* Feld nicht dimensioniert  */
    } else {
      if((vnr=variable_exist(key,FLOATARRAYTYP))!=-1) {
        if(dim!=variablen[vnr].opcode) xberror(18,"");  /* Falsche Anzahl Indizies */
       /* printf("Zuweis: %d %d %g \n",sp[-1].typ,sp[-1].integer,sp[-1].real);*/
        if(sp[-1].typ==PL_FLOAT) zuweisbyindex(vnr,index,sp[-1].real);
        else if(sp[-1].typ==PL_INT) zuweisbyindex(vnr,index,(double)sp[-1].integer);
        else printf("ERROR: Zuweis, Typen ungleich.\n");
      } else  xberror(15,key); /* Feld nicht dimensioniert  */
    }
    free(key);    
  } else {
    printf("ERROR: something is wrong with bytecode.\n");
    free_parameter(sp[-1]);
    return(-1);
  }
  return(-dim-1);
}

int bc_pusharrayelem(PARAMETER *sp, int dim) {    /*  */
  char *key;
  int index[dim];
  int i,vnr;
  if(sp[-1].typ==PL_KEY) {
    key=malloc(sp[-1].integer+1);
    
    memcpy(key,sp[-1].pointer,sp[-1].integer);
    key[sp[-1].integer]=0;
    if(verbose) printf("bc_var_index_%s_%d(",key,dim);
    free(sp[-1].pointer);
    
    /* Index-Liste holen */
    for(i=0;i<dim;i++) {
      if(sp[-1-dim+i].typ==PL_INT) index[i]=sp[-1-dim+i].integer;
      else if(sp[-1-dim+i].typ==PL_FLOAT) index[i]=(int)sp[-1-dim+i].real;
      else printf("ERROR!! ");
      if(verbose) printf("%d,",index[i]);  
    }
    if(verbose) printf(") ");
    sp-=dim;
    /* typ von KEY ?*/
    if(key[strlen(key)-1]=='%') {
      key[strlen(key)-1]=0;
      if((vnr=variable_exist(key,INTARRAYTYP))!=-1) {
        if(dim!=variablen[vnr].opcode) xberror(18,"");  /* Falsche Anzahl Indizies */
        sp[-1].integer=varintarrayinhalt(vnr,index);
      } else xberror(15,key);  /* Feld nicht dimensioniert  */
      sp[-1].typ=PL_INT;    
    } else if(key[strlen(key)-1]=='$') {
      key[strlen(key)-1]=0;
      
      if((vnr=variable_exist(key,STRINGARRAYTYP))!=-1) {
        STRING s;
        if(dim!=variablen[vnr].opcode) xberror(18,"");  /* Falsche Anzahl Indizies */
	s=varstringarrayinhalt(vnr,index);
        sp[-1].pointer=s.pointer;
	sp[-1].integer=s.len; 
      } else {
        xberror(15,key);  /* Feld nicht dimensioniert  */
	sp[-1].pointer=malloc(1);
	sp[-1].integer=0;
      }	
      sp[-1].typ=PL_STRING;
    } else {
      if((vnr=variable_exist(key,FLOATARRAYTYP))!=-1) {
        if(dim!=variablen[vnr].opcode) xberror(18,"");  /* Falsche Anzahl Indizies */
        sp[-1].real=varfloatarrayinhalt(vnr,index);
      } else  xberror(15,key); /* Feld nicht dimensioniert  */
      sp[-1].typ=PL_FLOAT;
    }
    free(key);    
  } else {
    printf("ERROR: something is wrong with bytecode.\n");
    free_parameter(sp[-1]);
    return(-1);
  }
  return(-dim);
}

int bc_pushv(PARAMETER *sp) {    /*  */
  char *key,*buf;
  int typ,vnr;
  if(sp[-1].typ==PL_KEY) {
    key=malloc(sp[-1].integer+1);
    memcpy(key,sp[-1].pointer,sp[-1].integer);
    key[sp[-1].integer]=0;
    if(verbose) printf("bc_var_%s ",key);
    buf=indirekt2(key);
    free(key);
    free_parameter(sp[-1]);
    typ=vartype(buf);
    if(typ & ARRAYTYP) {
      printf("XBVM: Array an dieser Stelle noch nicht implementiert.\n");
    } else if(typ & STRINGTYP) {
      buf[strlen(buf)-1]=0;
      if((vnr=variable_exist(buf,STRINGTYP))!=-1) {
        sp[-1].integer=variablen[vnr].opcode;
        sp[-1].pointer=malloc(variablen[vnr].opcode);
	memcpy(sp[-1].pointer,variablen[vnr].pointer,sp[-1].integer);
      } else {
        sp[-1].integer=0;
        sp[-1].pointer=malloc(1);
      }
      ((char *)(sp[-1].pointer))[sp[-1].integer]=0;
      sp[-1].typ=PL_STRING;
    } else if(typ & INTTYP) {
      buf[strlen(buf)-1]=0;
      if((vnr=variable_exist(buf,INTTYP))!=-1) {
        sp[-1].integer=variablen[vnr].opcode;
      } else {
        sp[-1].integer=0;
      }
      sp[-1].typ=PL_INT;
    } else {
      if((vnr=variable_exist(buf,FLOATTYP))!=-1) {
        sp[-1].real=variablen[vnr].zahl;
      } else {
        sp[-1].real=0;
      }
      sp[-1].typ=PL_FLOAT;
    }
     free(buf);     
    return(0);
  } else {
    printf("ERROR: something is wrong with bytecode.\n");
    free_parameter(sp[-1]);
    return(-1);
  }
}
int bc_eval(PARAMETER *sp) {    /*  */
  char *key;
  int typ;
  if(sp[-1].typ=PL_STRING) {
    key=malloc(sp[-1].integer+1);
    memcpy(key,sp[-1].pointer,sp[-1].integer);
    key[sp[-1].integer]=0;
    free_parameter(sp[-1]);
    if(verbose) printf("bc_eval_<%s>\n",key);
    kommando(key);
    free(key);
  } else {
    printf("ERROR: something is wrong with bytecode, no string.\n");
    free_parameter(sp[-1]);
  }
  return(-1);  
}

void dump_stack(PARAMETER *p, int n) {
  if(n) {
    int j;
    printf("\n%d parameters were left on the stack:\n",n);
    for(j=0;j<n;j++) {
      printf("%2d: ",j);
      if(p[j].typ==PL_INT)         printf(" int %d\n",p[j].integer);
      else if(p[j].typ==PL_FLOAT)  printf(" flt %g\n",p[j].real);
      else if(p[j].typ==PL_STRING) printf("   $ <%s>\n",(char *)p[j].pointer);
      else if(p[j].typ==PL_KEY)    printf(" KEY <%s>\n",(char *)p[j].pointer);
      else if(p[j].typ==PL_LEER)   printf(" <empty>\n");
      else if(p[j].typ==PL_FILENR) printf("   # %d\n",p[j].integer);
      else printf("%d %d %g\n",p[j].typ,p[j].integer,p[j].real);
    }
  }
}

PARAMETER *virtual_machine(STRING bcpc, int *npar) {
  PARAMETER *opstack=malloc(BC_STACKLEN*sizeof(PARAMETER));
  PARAMETER *osp=opstack;
  char cmd;
  int i=0,j;
  int a,n;
  short ss;
  double d;
  char *buf;

  while((cmd=bcpc.pointer[i]) && i<bcpc.len) {
    i++;
     switch(cmd) {
    case BC_NOOP:
      break;
    case BC_JSR:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));
      if(verbose) printf("bc_jsr_%d\n",a);
      i+=sizeof(int);
      stack[sp++]=i;
      i=a;
      break;
    case BC_JMP:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));
      if(verbose) printf("bc_jmp_%d\n",a);
      i+=sizeof(int);
      i=a;
      break;
    case BC_JEQ:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));
      if(verbose) printf("bc_jeq_%d ",a);
      i+=sizeof(int);
      opstack--;
      if(opstack->typ==PL_INT) {
        if(opstack->integer==0) i=a;
      } else if(opstack->typ==PL_FLOAT) {
        if(opstack->real==0) i=a;
      } else {
        printf("ERROR: JEQ\n");
        free_parameter(*opstack);
      }
      if(verbose) {if(i==a) printf("\n");}
      break;
    case BC_BRA:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));
      if(verbose) printf("bc_bra_%d \n",ss);
      i+=ss-1;
      break;
    case BC_BSR:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));
      if(verbose) printf("bc_bsr_%d \n",ss);
      stack[sp++]=i;
      i+=ss-1;
      break;
    case BC_RTS:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));
      if(verbose) printf("bc_rts \n");
      i=stack[--sp];
      break;
    case BC_BEQ:
      memcpy(&ss,&bcpc.pointer[i],sizeof(short));
      i+=sizeof(short);
      if(verbose) printf("bc_beq_%d ",ss);
      opstack--;
      if(opstack->typ==PL_INT) {
        if(opstack->integer==0) i+=ss-1-sizeof(short);
      } else if(opstack->typ==PL_FLOAT) {
        if(opstack->real==0) i+=ss-1-sizeof(short);
      } else {
        printf("ERROR: BEQ\n");
        free_parameter(*opstack);
      }
      if(verbose) {printf("\n");}
      break;
    case BC_BRAs:
      if(verbose) printf("bc_bra.s_%d \n",bcpc.pointer[i]); 
      i+=bcpc.pointer[i++]-2;
      break;
    case BC_BEQs:
      ss=bcpc.pointer[i++];
      if(verbose) printf("bc_beqs_%d ",ss);
      opstack--;
      if(opstack->typ==PL_INT) {
        if(opstack->integer==0) i+=ss-2;
      } else if(opstack->typ==PL_FLOAT) {
        if(opstack->real==0) i+=ss-2;
      } else {
        printf("ERROR: BEQs\n");
        free_parameter(*opstack);
      }
      if(verbose) {printf("%d\n",i);}
      break;
    case BC_PUSHF:
      memcpy(&d,&bcpc.pointer[i],sizeof(double));
      i+=sizeof(double);
      opstack->real=d;
      opstack->typ=PL_FLOAT;
      opstack++;
      if(verbose) printf("%g ",d);
      break;
    case BC_PUSHI:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));
      i+=sizeof(int);
      opstack->integer=a;
      opstack->typ=PL_INT;
      opstack++;
      if(verbose) printf("%d ",a);
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
    case BC_PUSHS:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));
      i+=sizeof(int);
      opstack->integer=a;
      opstack->typ=PL_STRING;
      opstack->pointer=malloc(a+1);
      memcpy(opstack->pointer,&bcpc.pointer[i],a);
      ((char *)(opstack->pointer))[a]=0;
      i+=a;
      opstack++;
      if(verbose) printf("\"%s\" ",(char *)opstack[-1].pointer);
      break;
    case BC_PUSHFUNC:
      a=bcpc.pointer[i++]&0xff;
      n=bcpc.pointer[i++]&0xff;
      opstack+=bc_func(opstack,a,n);
      break;
    case BC_PUSHSFUNC:
      a=bcpc.pointer[i++]&0xff;
      n=bcpc.pointer[i++]&0xff;
      opstack+=bc_sfunc(opstack,a,n);
      break;
    case BC_PUSHCOMM:
      a=bcpc.pointer[i++]&0xff;
      n=bcpc.pointer[i++]&0xff;
      opstack+=bc_comm(opstack,a,n);
      if(verbose) printf("SP=%d\n",((int)opstack-(int)osp)/sizeof(PARAMETER));  
      break;
    case BC_PUSHSYS:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));
      i+=sizeof(int);
      opstack+=bc_sysvar(opstack,a);
      break;
    case BC_PUSHSSYS:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));
      i+=sizeof(int);
      opstack+=bc_ssysvar(opstack,a);
      break;
    case BC_PUSHASYS:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));
      i+=sizeof(int);
      opstack+=bc_asysvar(opstack,a);
      break;
    case BC_PUSHX:
      a=bcpc.pointer[i++];
      buf=malloc(a+1);
      memcpy(buf,&bcpc.pointer[i],a);
      buf[a]=0;
      opstack->integer=a;
      opstack->typ=PL_KEY;
      opstack->pointer=buf;
      i+=a;
      opstack++;
      if(verbose) printf("%s ",buf);
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
      opstack+=bc_add(opstack);
      break;
    case BC_OR:
      opstack+=bc_or(opstack);
      break;
    case BC_SUB:
      opstack+=bc_sub(opstack);
      break;
    case BC_MUL:
      opstack+=bc_mul(opstack);
      break;
    case BC_DIV:
      opstack+=bc_div(opstack);
      break;
    case BC_POW:
      opstack+=bc_pow(opstack);
      break;
    case BC_AND:
      opstack+=bc_and(opstack);
      break;
    case BC_EQUAL:
      opstack+=bc_equal(opstack);
      break;
    case BC_GREATER:
      opstack+=bc_greater(opstack);
      break;
    case BC_LESS:
      opstack+=bc_less(opstack);
      break;
    case BC_DUP:
      opstack+=bc_dup(opstack);
      break;
    case BC_EXCH:
      opstack+=bc_exch(opstack);
      break;
    case BC_CLEAR:
      a=((int)opstack-(int)osp)/sizeof(PARAMETER);
      if(a) {
        opstack=osp;
	for(j=0;j<a;j++)  free_parameter(opstack[j]);
      }
      if(verbose) printf("bc_clear ");
      break;
    case BC_COUNT:
      a=((int)opstack-(int)osp)/sizeof(PARAMETER);
      opstack->integer=a;
      opstack->typ=PL_INT;
      opstack++;
      if(verbose) printf("bc_count_%d ",a);
      break;
    case BC_NEG:
      opstack+=bc_neg(opstack);
      break;
    case BC_NOT:
      opstack+=bc_not(opstack);
      break;
    case BC_MOD:
      opstack+=bc_mod(opstack);
      break;
    case BC_POP:
      opstack--;
      free_parameter(*opstack);
      break;
    case BC_ZUWEIS:
      opstack+=bc_zuweis(opstack);
      break;
    case BC_PUSHVV:
      opstack+=bc_pushvv(opstack);
      break;
    case BC_ZUWEISINDEX:
      a=bcpc.pointer[i++]&0xff;
      opstack+=bc_zuweisindex(opstack,a);
      break;
    case BC_PUSHV:
      opstack+=bc_pushv(opstack);
      break;
    case BC_PUSHARRAYELEM:      
      a=bcpc.pointer[i++]&0xff;
      opstack+=bc_pusharrayelem(opstack,a);
      break;
    case BC_RESTORE:
      memcpy(&a,&bcpc.pointer[i],sizeof(int));
      if(verbose) printf("bc_restore_%d\n",a);
      i+=sizeof(int);
      datapointer=a;
      break;
    case BC_EVAL:
      if(verbose) printf("bc_eval ",a);
      opstack+=bc_eval(opstack);
      break;
    default:
      printf("VM: BC_ILLEGAL instruction %2x at %d\n",(int)cmd,i);
      memdump(&(bcpc.pointer[i]),16);
      *npar=((int)opstack-(int)osp)/sizeof(PARAMETER);  
      return(osp);
    }
  }
  *npar=((int)opstack-(int)osp)/sizeof(PARAMETER);  
  return(osp);
}
