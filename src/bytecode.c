/* BYTECODE.C   The X11-basic bytecode compiler         (c) Markus Hoffmann
*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "keywords.h"
#include "array.h"
#include "xbasic.h"
#include "type.h"
#include "parser.h"
#include "parameter.h"
#include "number.h"
#include "io.h"

#include "functions.h"
#include "wort_sep.h"
#include "bytecode.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif
#ifndef S_IRGRP
#define S_IRGRP 0
#endif

#define BCADD(a) cb->textseg[cb->textseglen++]=a
static int compile_zeile;


#define BCADDPUSHX(a) if(a) {BCADD(BC_PUSHX);BCADD(strlen(a));\
                      {int adr=add_rodata(cb,a,strlen(a));\
		      CP4(&cb->textseg[cb->textseglen],&adr,cb->textseglen);}\
                      } else { \
		      printf("Compiler-ERROR: Null pointer in X-String at line %d.\n",compile_zeile); \
		      exit(0);}


const int relocation_offset=sizeof(BYTECODE_HEADER);
extern int verbose;



/* X11-Basic needs these declar<ations:  */

#ifdef WINDOWS
#include <windows.h>
#endif

static int typstack[128];
static int typsp=0;

/* Typestack add a */
static inline void TP(int a) {typstack[typsp]=(a);typsp++;}
/* Typestack replace last*/
#define TR(a) typstack[typsp-1]=(a)
/* Typestack remove last */
#define TO()  if(--typsp<0) printf("WARNING: typestack<0 at line %d.\n",compile_zeile)
/* Typestack remove last a */
#define TA(a)  if((typsp-=(a))<0) printf("WARNING: typestack<0 at line %d.\n",compile_zeile)
#define TL    typstack[typsp-1]
#define TL2    typstack[typsp-2]
static inline void TEXCH() {
  int tmp=typstack[typsp-1];
  typstack[typsp-1]=typstack[typsp-2];
  typstack[typsp-2]=tmp;
}


/* Include a comment into the text segment 
 * 
 */
static void bc_comment(COMPILE_BLOCK *cb,const char *c) {
  if(cb->comp_flags&COMPILE_COMMENTS) {
    int len=strlen(c);
    BCADD(BC_COMMENT);    
    BCADD(len);
    memcpy(&cb->textseg[cb->textseglen],c,len);
    cb->textseglen+=len;
    if(verbose>1) printf(" bc_comment <%s> ",c);
  }
}

static void bc_jumptosr2(COMPILE_BLOCK *cb,int ziel);
static void plist_to_stack(COMPILE_BLOCK *cb,PARAMETER *pin, short *pliste, int anz, int pmin, int pmax);

static void bc_push_float(COMPILE_BLOCK *cb,double d) {
  BCADD(BC_PUSHF);
  TP(PL_FLOAT);
  CP8(&cb->textseg[cb->textseglen],&d,cb->textseglen);
}
static void bc_push_complex(COMPILE_BLOCK *cb,COMPLEX c) {
  BCADD(BC_PUSHC);
  TP(PL_COMPLEX);
  CP8(&cb->textseg[cb->textseglen],&(c.r),cb->textseglen);
  CP8(&cb->textseg[cb->textseglen],&(c.i),cb->textseglen);
}
static void bc_push_string(COMPILE_BLOCK *cb,STRING str) {
  int i;
  int adr;
  BCADD(BC_PUSHS);
  TP(PL_STRING);
  i=str.len;
  adr=add_rodata(cb,str.pointer,str.len);
  CP4(&cb->textseg[cb->textseglen],&i,cb->textseglen);
  CP4(&cb->textseg[cb->textseglen],&adr,cb->textseglen);
}
static void bc_push_arbint(COMPILE_BLOCK *cb,ARBINT a) {
  char *buf=mpz_get_str(NULL,32,a);
  BCADD(BC_PUSHAI);
  TP(PL_ARBINT);
  int i=strlen(buf);
  int adr=add_rodata(cb,buf,i);
  free(buf);
  CP4(&cb->textseg[cb->textseglen],&i,cb->textseglen);
  CP4(&cb->textseg[cb->textseglen],&adr,cb->textseglen);
}

static void bc_push_array(COMPILE_BLOCK *cb,ARRAY arr) {
  BCADD(BC_PUSHA);
  TP(PL_ARRAY);
  STRING str=array_to_string(arr);
  int i=str.len;
  int adr=add_rodata(cb,str.pointer,str.len);
  CP4(&cb->textseg[cb->textseglen],&i,cb->textseglen);
  CP4(&cb->textseg[cb->textseglen],&adr,cb->textseglen);
  free(str.pointer);
}


static void bc_push_integer(COMPILE_BLOCK *cb,int i) {
  if(i==0) BCADD(BC_PUSH0);
  else if(i==1) BCADD(BC_PUSH1);
  else if(i==2) BCADD(BC_PUSH2);
  else if(i==-1) BCADD(BC_PUSHM1);
  else if(i<128 && i>=-128) {
    BCADD(BC_PUSHB);
    BCADD(i);
  } else if(i<0x7fff && i>-32768) {
    short ss=(short)i;
    BCADD(BC_PUSHW);
    CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
  } else {
    BCADD(BC_PUSHI);
    CP4(&cb->textseg[cb->textseglen],&i,cb->textseglen);
  }
  TP(PL_INT);
}

/* Push variable inhalt on stack */
static void bc_pushv(COMPILE_BLOCK *cb,int vnr);

static void bc_pushv_name(COMPILE_BLOCK *cb,const char *var) {
  int typ=vartype(var);
  char *r=varrumpf(var);

  if((typ& ARRAYTYP)) {     /*  Ganzes Array */ 
    bc_pushv(cb,add_variable(r,ARRAYTYP,typ&TYPMASK,V_DYNAMIC,NULL));
  } else {
    char w1[strlen(var)+1],w2[strlen(var)+1];
    int e=klammer_sep(var,w1,w2);
    if(e==0) ; /* Leer oder syntax Fehler */
    else if(e>1) {  /*   Array Element */
      int vnr=add_variable(r,ARRAYTYP,typ&TYPMASK,V_DYNAMIC,NULL);
      short f=count_parameters(w2);   /* Anzahl indizes z"ahlen*/
      short ss=vnr;
      while(wort_sep(w2,',',TRUE,w1,w2)) {
        bc_parser(cb,w1);  /*  jetzt Indizies Zeugs aufm stack */
	if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
      }
      
    /* Hier muesste man noch cheken, ob alle indizies integer sind.
       sonst ggf. umwandeln. Hierfuer brauchen wir ROLL */
      BCADD(BC_PUSHARRAYELEM);
      CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
      CP2(&cb->textseg[cb->textseglen],&f,cb->textseglen);
      TA(f);
      typ=variablen[vnr].typ;
      if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
      TP(PL_CONSTGROUP|typ);
    } else {  /*  Normale Variable */
      bc_pushv(cb,add_variable(r,typ&TYPMASK,0,V_DYNAMIC,NULL));
    } 
  }
  free(r);
}

/* Push variable inhalt on stack */
static void bc_pushv(COMPILE_BLOCK *cb,int vnr) {
  short ss=vnr;
  BCADD(BC_PUSHV);
  CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
  TP(PL_CONSTGROUP|variablen[vnr].typ);
}
static void bc_pushvv(COMPILE_BLOCK *cb,int vnr) {
  short ss=vnr;
  BCADD(BC_PUSHVV);
  CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
  int typ=variablen[vnr].typ;
  if(typ==ARRAYTYP) typ|=variablen[vnr].pointer.a->typ;
  TP(PL_VARGROUP|typ);
}


static void bc_pushvvindex(COMPILE_BLOCK *cb,int vnr,int anz) {
  short ss=vnr;
  BCADD(BC_PUSHVVI);
  CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
  ss=anz;
  CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
  TA(anz);
  TP(PL_VARGROUP|variablen[vnr].typ);
}


static void bc_pushdimargindex(COMPILE_BLOCK *cb,int vnr,int anz) {
  short ss=vnr;
  BCADD(BC_PUSHDIMARGI);
  CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
  ss=anz;
  CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
  TA(anz);
  TP(PL_VARGROUP|variablen[vnr].typ);
}

/* Put content from stack in variable */

static void bc_zuweis(COMPILE_BLOCK *cb,int vnr) {
  short ss=vnr;
 // printf("bc-zuweis: TL=%x, v-typ=%x\n",TL,variablen[vnr].typ);
  switch(variablen[vnr].typ) {
  case INTTYP:
    if(TL!=PL_INT) BCADD(BC_X2I);
    BCADD(BC_ZUWEISi);
    break;
  case FLOATTYP:
    if(TL==PL_INT) BCADD(BC_I2F);
    else if(TL!=PL_FLOAT) BCADD(BC_X2F);
    BCADD(BC_ZUWEISf);
    break;
  case COMPLEXTYP:
    if(TL==PL_INT) {BCADD(BC_I2F);BCADD(BC_F2C);}
    else if(TL==PL_FLOAT) BCADD(BC_F2C);
    else if(TL!=PL_COMPLEX) BCADD(BC_X2C);
    BCADD(BC_ZUWEISc);
    break;
  default:
    BCADD(BC_ZUWEIS);
  }
  CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
  TO();
  if(verbose>1) printf("ZUWEIS ");
}
static void bc_zuweis_nopretyp(COMPILE_BLOCK *cb,int vnr) {
  short ss=vnr;
 // printf("bc-zuweis: TL=%x, v-typ=%x\n",TL,variablen[vnr].typ);
  BCADD(BC_ZUWEIS);
  CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
  TO();
  if(verbose>1) printf("ZUWEIS ");
}

/* make a local copy of var */

static void bc_local(COMPILE_BLOCK *cb,int vnr) {
  short ss=vnr;
  BCADD(BC_LOCAL);
  CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
}

/* Hier ist schon was auf dem Stack. */
static void bc_zuweis_name(COMPILE_BLOCK *cb,const char *var) {
  int typ=vartype(var);
  char *r=varrumpf(var);
  int vnr;
  if(typ & ARRAYTYP) {     /*  Ganzes Array */ 
    vnr=add_variable(r,ARRAYTYP,typ&TYPMASK,V_DYNAMIC,NULL);
    if(TL!=PL_ARRAY) printf("WARNING: no ARRAY for assignment! at line %d.\n",compile_zeile);
    bc_zuweis(cb,vnr);
  } else {
    char w1[strlen(var)+1],w2[strlen(var)+1];
    int e=klammer_sep(var,w1,w2);
    if(e==0) ;      /* Leer oder Syntax Fehler */
    else if(e>1) {  /*   Array Element */
      vnr=add_variable(r,ARRAYTYP,typ&TYPMASK,V_DYNAMIC,NULL);
      short f=count_parameters(w2);   /* Anzahl indizes z"ahlen*/
      short ss=vnr;
      while(wort_sep(w2,',',TRUE,w1,w2)) {
        bc_parser(cb,w1);  /*  jetzt Indizies Zeugs aufm stack */
	if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
      }
      BCADD(BC_ZUWEISINDEX);
      CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
      CP2(&cb->textseg[cb->textseglen],&f,cb->textseglen);
      TA(f);
      TO();
    } else {  /*  Normale Variable */
      bc_zuweis(cb,add_variable(r,typ&TYPMASK,0,V_DYNAMIC,NULL));
    } 
  }
  free(r);
}
static void bc_zuweisung(COMPILE_BLOCK *cb,char *var,char *arg) {
  bc_parser(cb,arg);  /*  jetzt ist Zeugs aufm stack */
  bc_zuweis_name(cb,var);
}


/* Indexliste aus Parameterarray (mit EVAL) oder aus int-Konstanten
 * auf Stack.
 */

static int bc_indexliste(COMPILE_BLOCK *cb,PARAMETER *p,int n) {
  int i;
  for(i=0;i<n;i++) {
    if(p[i].typ==PL_EVAL) {
      if(((char *)p[i].pointer)[0]==':') bc_push_integer(cb,-1);
      else {
        bc_parser(cb,(char *)p[i].pointer);
	if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
      }
    } else if(p[i].typ==PL_INT) {
      bc_push_integer(cb,p[i].integer);
    } else {
      printf("ERROR: bc_indexliste: unexpected type...\n");
      bc_push_integer(cb,0);
    }
  }
  return(i);
}
static int push_typeliste(COMPILE_BLOCK *cb,const char *argument, int *varlist,int n);

/*Binaere logische operation, das kann normal int sein oder ARBINT.*/

static void bc_logic(COMPILE_BLOCK *cb,const char *w1,const char *w2,unsigned char operator,unsigned char operator2) {
  bc_parser(cb,w1);
  switch(TL) {
  case PL_INT: break;
  case PL_ARBINT: break;
  default:
    BCADD(BC_X2AI);
    TR(PL_ARBINT);
  }
  int typ1=TL;
  bc_parser(cb,w2);
  switch(TL) {
  case PL_INT: break;
  case PL_ARBINT: break;
  default:
    BCADD(BC_X2AI);
    TR(PL_ARBINT);
  }
  int typ2=TL;
  if(typ1==PL_INT && typ2==PL_INT) BCADD(operator2);
  else BCADD(operator);
  TO();
  TR(PL_CONSTGROUP|combine_type(typ1&PL_BASEMASK,typ2&PL_BASEMASK,'+'));
}
static void bc_not(COMPILE_BLOCK *cb) {
  if(TL==PL_INT) BCADD(BC_NOTi);
  else if(TL!=PL_ARBINT) {BCADD(BC_X2AI);BCADD(BC_NOT);TR(PL_ARBINT);}
  else BCADD(BC_NOT);
}
/* Richtiges vorhersagen des ergebnistyps (ohne sonderfaelle): MOD DIV */

static void bc_binaer1(COMPILE_BLOCK *cb,const char *w1,const char *w2,unsigned char operator) {
  bc_parser(cb,w1);
  int typ1=TL;
  bc_parser(cb,w2);
  int typ2=TL;
  BCADD(operator);
  TO();
  TR(PL_CONSTGROUP|combine_type(typ1&PL_BASEMASK,typ2&PL_BASEMASK,'+'));
}
static void bc_mul(COMPILE_BLOCK *cb){
  int t1=TL;
  int t2=TL2;
  int tr=PL_CONSTGROUP|combine_type(t1&PL_BASEMASK,t2&PL_BASEMASK,'*');
  if(t1==PL_INT && t2==PL_INT) BCADD(BC_MULi);
  else if(t1==PL_FLOAT && t2==PL_FLOAT) BCADD(BC_MULf);
  else if(t1==PL_COMPLEX && t2==PL_COMPLEX) BCADD(BC_MULc);
  else if(t1==PL_INT && t2==PL_FLOAT) {BCADD(BC_I2F);BCADD(BC_MULf);}
  else if(t1==PL_FLOAT && t2==PL_INT) {BCADD(BC_EXCH);BCADD(BC_I2F);BCADD(BC_MULf);}
  else BCADD(BC_MUL);
  TO();
  TR(tr);
}
static void bc_add(COMPILE_BLOCK *cb) {
  int t1=TL;
  int t2=TL2;
  int tr=PL_CONSTGROUP|combine_type(t1&PL_BASEMASK,t2&PL_BASEMASK,'+');
  if(t1==PL_INT && t2==PL_INT) BCADD(BC_ADDi);
  else if(t1==PL_FLOAT && t2==PL_FLOAT) BCADD(BC_ADDf);
  else if(t1==PL_COMPLEX && t2==PL_COMPLEX) BCADD(BC_ADDc);
  else if(t1==PL_STRING && t2==PL_STRING) BCADD(BC_ADDs);
  else if(t1==PL_INT && t2==PL_FLOAT) {BCADD(BC_I2F);BCADD(BC_ADDf);}
  else if(t1==PL_FLOAT && t2==PL_INT) {BCADD(BC_EXCH);BCADD(BC_I2F);BCADD(BC_ADDf);}
  else BCADD(BC_ADD);
  TO();
  TR(tr);
}
static void bc_sub(COMPILE_BLOCK *cb) {
  int t1=TL;
  int t2=TL2;
  int tr=PL_CONSTGROUP|combine_type(t1&PL_BASEMASK,t2&PL_BASEMASK,'-');
  if(t1==PL_INT && t2==PL_INT) BCADD(BC_SUBi);
  else if(t1==PL_FLOAT && t2==PL_FLOAT) BCADD(BC_SUBf);
  else if(t1==PL_COMPLEX && t2==PL_COMPLEX) BCADD(BC_SUBc);
  else if(t1==PL_INT && t2==PL_FLOAT) {BCADD(BC_I2F);BCADD(BC_SUBf);}
  else if(t1==PL_FLOAT && t2==PL_INT) {BCADD(BC_EXCH);BCADD(BC_I2F);BCADD(BC_EXCH);BCADD(BC_SUBf);}
  else BCADD(BC_SUB);
  TO();
  TR(tr);
}

/*
 * Bytecode parser.
 */

int bc_parser(COMPILE_BLOCK *cb,const char *funktion){  /* Rekursiver Parser */
  char *pos,*pos2;
  char w1[strlen(funktion)+1],w2[strlen(funktion)+1];
  int i,e,typ;

  static int bcerror;
 
 // printf("Bytecode Parser: <%s>\n",funktion);

  /* Erst der Komma-Operator */
  
  if(searchchr3(funktion,',')!=NULL){
    if(wort_sep(funktion,',',TRUE,w1,w2)>1) {
      bc_parser(cb,w1);bc_parser(cb,w2);return(bcerror);
    }
  }  
  char s[strlen(funktion)+1];
  xtrim(funktion,TRUE,s);  /* Leerzeichen vorne und hinten entfernen, Grossbuchstaben */

  typ=type(s);
  if(typ&CONSTTYP) {
     /* Hier koennen wir stark vereinfachen, wenn wir schon wissen, dass der 
        Ausdruck konstant ist.
	*/
   // printf("#### Konstante: <%s> -->",funktion);fflush(stdout);
    if(typ&ARRAYTYP) {
      ARRAY a;
      if(*s=='[' && s[strlen(s)-1]==']') {  /* Konstante */
        s[strlen(s)-1]=0;
        a=array_const(s+1);
      } else {
        printf("Array const syntax error. at line %d.\n",compile_zeile);
	a=array_const(s);
      }
      bc_push_array(cb,a);
      free_array(&a);
      if(verbose>1) printf("<-array->.c ");
      return(bcerror);
    } 
    switch(typ&TYPMASK) {
    case INTTYP: {
      int si;
      if(typ&FILENRTYP) bc_push_integer(cb,si=(int)parser(funktion+1));
      else  bc_push_integer(cb,si=(int)parser(funktion));
      if(verbose>1) printf("<%d>.c ",si);
      return(bcerror);
      }
    case FLOATTYP: {
      double d=parser(funktion);
      bc_push_float(cb,d);
      if(verbose>1) printf("<%g>.cf ",d);
      return(bcerror);
      }
    case COMPLEXTYP: {
      COMPLEX d=complex_parser(funktion);
      bc_push_complex(cb,d);
      if(verbose>1) printf("<%g+%gi>.cc ",d.r,d.i);
      return(bcerror); 
      }   
    case STRINGTYP: {
      STRING str=string_parser(funktion);
 //     memdump(str.pointer,str.len+1);
      bc_push_string(cb,str);
      if(verbose>1) printf("<\"%s\">.c ",str.pointer);
      free(str.pointer);
      }
      return(bcerror);
    case ARBINTTYP: {
      ARBINT a;
      mpz_init(a);
      arbint_parser(funktion,a);
      bc_push_arbint(cb,a);
      if(verbose>1) gmp_printf("<\"%Zd\">.c ",a);
      mpz_clear(a);
      return(bcerror);
      }
    }
  }   /*  CONSTTYP */


  /* Logische Operatoren AND OR NOT ... */
  if(*s==0) {BCADD(BC_PUSHLEER);TP(PL_LEER);return(bcerror);}
  if(searchchr2_multi(s,"&|")!=NULL) {
    if(wort_sepr2(s,"||",TRUE,w1,w2)>1) {bc_logic(cb,w1,w2,BC_OR,BC_ORi); return(bcerror);}    
    if(wort_sepr2(s,"&&",TRUE,w1,w2)>1) {bc_logic(cb,w1,w2,BC_AND,BC_ANDi);return(bcerror);}
  }

  if(searchchr2(s,' ')!=NULL) {
    if(wort_sepr2(s," OR ",TRUE,w1,w2)>1)   {bc_logic(cb,w1,w2,BC_OR,BC_ORi); return(bcerror);}
    if(wort_sepr2(s," AND ",TRUE,w1,w2)>1)  {bc_logic(cb,w1,w2,BC_AND,BC_ANDi);return(bcerror);}
    if(wort_sepr2(s," NAND ",TRUE,w1,w2)>1) {bc_logic(cb,w1,w2,BC_AND,BC_ANDi);bc_not(cb);return(bcerror);}
    if(wort_sepr2(s," NOR ",TRUE,w1,w2)>1)  {bc_logic(cb,w1,w2,BC_OR,BC_ORi); bc_not(cb);return(bcerror);}
    if(wort_sepr2(s," XOR ",TRUE,w1,w2)>1)  {bc_logic(cb,w1,w2,BC_XOR,BC_XORi);return(bcerror);}
    if(wort_sepr2(s," EOR ",TRUE,w1,w2)>1)  {bc_logic(cb,w1,w2,BC_XOR,BC_XORi);return(bcerror);}  
    if(wort_sepr2(s," EQV ",TRUE,w1,w2)>1)  {bc_logic(cb,w1,w2,BC_XOR,BC_XORi);bc_not(cb);return(bcerror);}
    if(wort_sepr2(s," IMP ",TRUE,w1,w2)>1)  {bc_logic(cb,w1,w2,BC_XOR,BC_XORi);bc_not(cb);bc_parser(cb,w2);if(TL!=PL_INT && TL!=PL_ARBINT) {BCADD(BC_X2I);TR(PL_INT);}BCADD(BC_OR);TO();return(bcerror);}
    if(wort_sepr2(s," MOD ",TRUE,w1,w2)>1)  {bc_binaer1(cb,w1,w2,BC_MOD);return(bcerror);}
    if(wort_sepr2(s," DIV ",TRUE,w1,w2)>1)  {bc_binaer1(cb,w1,w2,BC_DIV);if(TL!=PL_INT && TL!=PL_ARBINT) {BCADD(BC_X2I);TR(PL_INT);}return(bcerror);}
    if(wort_sepr2(s,"NOT ",TRUE,w1,w2)>1) {
      if(*w1==0) {bc_parser(cb,w2);bc_not(cb);return(bcerror);}    /* von rechts !!  */
      /* Ansonsten ist NOT Teil eines Variablennamens */
    }
  }

  /* Erst Vergleichsoperatoren mit Wahrheitwert abfangen (lowlevel < Addition)  */
  if(searchchr2_multi(s,"<=>")!=NULL) {
    if(wort_sep2(s,"==",TRUE,w1,w2)>1) {bc_parser(cb,w1);bc_parser(cb,w2);BCADD(BC_EQUAL);TO();TR(PL_INT);return(bcerror);}
    if(wort_sep2(s,"<>",TRUE,w1,w2)>1) {bc_parser(cb,w1);bc_parser(cb,w2);BCADD(BC_EQUAL);TO();TR(PL_INT);bc_not(cb);return(bcerror);}
    if(wort_sep2(s,"><",TRUE,w1,w2)>1) {bc_parser(cb,w1);bc_parser(cb,w2);BCADD(BC_EQUAL);TO();TR(PL_INT);bc_not(cb);return(bcerror);}
    if(wort_sep2(s,"<=",TRUE,w1,w2)>1) {bc_parser(cb,w1);bc_parser(cb,w2);BCADD(BC_GREATER);TO();TR(PL_INT);bc_not(cb);return(bcerror);}
    if(wort_sep2(s,">=",TRUE,w1,w2)>1) {bc_parser(cb,w1);bc_parser(cb,w2);BCADD(BC_LESS);TO();TR(PL_INT);bc_not(cb);return(bcerror);}
    if(wort_sep(s,'=',TRUE,w1,w2)>1)   {bc_parser(cb,w1);bc_parser(cb,w2);BCADD(BC_EQUAL);TO();TR(PL_INT);return(bcerror);}
    if(wort_sep(s,'<',TRUE,w1,w2)>1)   {bc_parser(cb,w1);bc_parser(cb,w2);BCADD(BC_LESS);TO();TR(PL_INT);return(bcerror);}
    if(wort_sep(s,'>',TRUE,w1,w2)>1)   {bc_parser(cb,w1);bc_parser(cb,w2);BCADD(BC_GREATER);TO();TR(PL_INT);return(bcerror);}
  }
  /* Addition/Subtraktion/Vorzeichen  */
 // printf("bc_parser: <%s>\n",s);
  // pos=searchchr2_multi(s,"+-");
  // printf("pos: <%s>\n",pos);
  // printf("*/^:%d   %c\n",(strchr("*/^",*(pos-1))==NULL),*(pos-1));
 //  printf("digittest: <%d>\n",vfdigittest(s,pos-1));
 
  /* Suche eine g"ultige Trennstelle f"ur + oder -  */
  /* Hier muessen wir testen, ob es nicht ein vorzeichen war oder Teil eines Exponenten ...*/

  pos=searchchr2_multi_r(s,"+-",s+strlen(s));  /* Finde n"achsten Kandidaten  von rechts*/
  while(pos!=NULL) {
    if(pos==s) {  /*Das +/-  war ganz am Anfang*/
      bc_parser(cb,s+1);
      if(*s=='-') {BCADD(BC_NEG);}
      return(bcerror);
    }
    if(pos>s && strchr("*/^",*(pos-1))==NULL && 
                                     !( *(pos-1)=='E' && pos-1>s && vfdigittest(s,pos-1) && v_digit(*(pos+1)))) {
    /* Kandidat war gut.*/
      char c=*pos;
      *pos=0;
      bc_parser(cb,s);
      bc_parser(cb,pos+1);
      int tr=combine_type(typstack[typsp-2]&PL_BASEMASK,typstack[typsp-1]&PL_BASEMASK,c);
      switch(tr) {
      case STRINGTYP: BCADD(BC_ADDs);  break;
      case INTTYP:    BCADD(c=='+'?BC_ADDi:BC_SUBi);  break;
      case FLOATTYP:
        if(typstack[typsp-1]==PL_INT) BCADD(BC_I2F);
        if(typstack[typsp-2]==PL_INT) {
	  BCADD(BC_EXCH);
          BCADD(BC_I2F);
	  if(c=='-') {BCADD(BC_EXCH);}
	}
	BCADD(c=='+'?BC_ADDf:BC_SUBf);  break;
      case COMPLEXTYP:
         if(typstack[typsp-1]==PL_INT) {BCADD(BC_I2F);typstack[typsp-1]=PL_FLOAT;}
         if(typstack[typsp-1]==PL_FLOAT) BCADD(BC_F2C);
         if(typstack[typsp-2]==PL_INT) {
 	   BCADD(BC_EXCH);
           BCADD(BC_I2F); 
	   BCADD(BC_F2C);
	   if(c=='-') {BCADD(BC_EXCH);}
	 } else if(typstack[typsp-2]==PL_FLOAT) {
  	   BCADD(BC_EXCH);
           BCADD(BC_F2C);
	   if(c=='-') {BCADD(BC_EXCH);}
	 }
         BCADD(c=='+'?BC_ADDc:BC_SUBc); break;
      case NOTYP:  printf("compile ERROR: ADD/SUB: Type mismatch at line %d. %x <--> %x <%s>\n",compile_zeile,
	  typstack[typsp-2],typstack[typsp-1],funktion);
      default:     /* Alle anderen Typenkombinationen kommen so in den Bytecode*/
         BCADD(c=='+'?BC_ADD:BC_SUB);
      }
      TO();TR(PL_CONSTGROUP|tr);
      return(bcerror);
    } 
    pos=searchchr2_multi_r(s,"+-",pos);  /* Finde n"achsten Kandidaten  von rechts*/
  }
  
  
  
  
  
  
  
  if(searchchr2_multi(s,"*/^")!=NULL) {
    if(wort_sepr(s,'*',TRUE,w1,w2)>1) {
      if(*w1) {
        bc_parser(cb,w1);
	bc_parser(cb,w2);
        if(typstack[typsp-2]==PL_INT && typstack[typsp-1]==PL_INT) {
          BCADD(BC_MULi);
	  TO();
        } else if(typstack[typsp-2]==PL_FLOAT && typstack[typsp-1]==PL_FLOAT) {
          BCADD(BC_MULf);
	  TO();
        } else if(typstack[typsp-2]==PL_COMPLEX && (typstack[typsp-1]==PL_FLOAT || typstack[typsp-1]==PL_INT)) {
  	  BCADD(BC_X2C);
          BCADD(BC_MULc);
	  TO();
	  TR(PL_COMPLEX);	  
        } else if((typstack[typsp-2]==PL_INT || typstack[typsp-2]==PL_FLOAT) && typstack[typsp-1]==PL_COMPLEX) {
	  BCADD(BC_EXCH);
          BCADD(BC_X2C);
          BCADD(BC_MULc);
	  TO();TR(PL_COMPLEX);
        } else if(typstack[typsp-2]==PL_COMPLEX && typstack[typsp-1]==PL_COMPLEX) {
          BCADD(BC_MULc);
	  TO();
        } else if(typstack[typsp-2]==PL_FLOAT && typstack[typsp-1]==PL_INT) {
          BCADD(BC_I2F);
          BCADD(BC_MULf);
	  TO();TR(PL_FLOAT);
        } else if(typstack[typsp-2]==PL_INT && typstack[typsp-1]==PL_FLOAT) {
	  BCADD(BC_EXCH);
          BCADD(BC_I2F);
          BCADD(BC_MULf);
	  TO();TR(PL_FLOAT);
        } else if(typstack[typsp-2]==PL_ARRAY && typstack[typsp-1]==PL_ARRAY) {
          BCADD(BC_MUL);
          TO();
	} else {   /* Alle anderen Typenkombinationen kommen so in den Bytecode*/
	  int tr=combine_type(typstack[typsp-2]&PL_BASEMASK,typstack[typsp-1]&PL_BASEMASK,'+');
	  if(tr==NOTYP) printf("compile ERROR: MUL: Type mismatch at line %d. %x <--> %x <%s>\n",compile_zeile,
	                        typstack[typsp-2],typstack[typsp-1],funktion);
  	  BCADD(BC_MUL);
	  TO();
	  TR(PL_CONSTGROUP|tr);
	}
	return(bcerror);
      } else {
        printf("Pointer not yet integrated! %s\n",w2);   /* war pointer - */
        return(bcerror);
      }
    }
    if(wort_sepr(s,'/',TRUE,w1,w2)>1) {
      if(*w1) {
        bc_parser(cb,w1);
	bc_parser(cb,w2);
        if(typstack[typsp-2]==PL_FLOAT && typstack[typsp-1]==PL_FLOAT) {
          BCADD(BC_DIVf);
	  TO();
        } else if(typstack[typsp-2]==PL_COMPLEX && (typstack[typsp-1]==PL_FLOAT || typstack[typsp-1]==PL_INT)) {
  	  BCADD(BC_X2C);
          BCADD(BC_DIVc);
	  TO();
	  TR(PL_COMPLEX);	  
        } else if((typstack[typsp-2]==PL_INT || typstack[typsp-2]==PL_FLOAT) && typstack[typsp-1]==PL_COMPLEX) {
	  BCADD(BC_EXCH);
          BCADD(BC_X2C);
	  BCADD(BC_EXCH);
          BCADD(BC_DIVc);
	  TO();TR(PL_COMPLEX);
        } else if(typstack[typsp-2]==PL_COMPLEX && typstack[typsp-1]==PL_COMPLEX) {
          BCADD(BC_DIVc);
	  TO();
        } else if(typstack[typsp-2]==PL_FLOAT && typstack[typsp-1]==PL_INT) {
          BCADD(BC_I2F);
          BCADD(BC_DIVf);
	  TO();TR(PL_FLOAT);
        } else if(typstack[typsp-2]==PL_INT && typstack[typsp-1]==PL_FLOAT) {
	  BCADD(BC_EXCH);
          BCADD(BC_I2F);
	  BCADD(BC_EXCH);
          BCADD(BC_DIVf);
	  TO();TR(PL_FLOAT);
	} else {   /* Alle anderen Typenkombinationen kommen so in den Bytecode*/
	  int tr=combine_type(typstack[typsp-2]&PL_BASEMASK,typstack[typsp-1]&PL_BASEMASK,'/');
	  if(tr==NOTYP) printf("compile ERROR: DIV: Type mismatch at line %d. %x <--> %x <%s>\n",compile_zeile,
	                        typstack[typsp-2],typstack[typsp-1],funktion);
  	  BCADD(BC_DIV);TO();TR(PL_CONSTGROUP|tr);
	}
        return(bcerror);
      } else { xberror(51,w2); return(bcerror); }/* "Parser: Syntax error?! "  */
    }
    if(wort_sepr(s,'^',TRUE,w1,w2)>1) {
      if(*w1) {
        bc_parser(cb,w1);
	bc_parser(cb,w2);
	if(TL==PL_INT && cb->textseg[cb->textseglen-1]==BC_PUSH2) {
	//  printf("Quadrat gefunden !\n");
	  cb->textseglen--;  /*Letztes PUSH2 rueckgaengig machen*/
	  TO();
	  BCADD(BC_DUP);
	  if(TL==PL_INT) BCADD(BC_MULi);
	  else if(TL==PL_FLOAT) BCADD(BC_MULf);
	  else if(TL==PL_COMPLEX) BCADD(BC_MULc);
	  else BCADD(BC_MUL);
	} else {
  	  int tr=combine_type(typstack[typsp-2]&PL_BASEMASK,typstack[typsp-1]&PL_BASEMASK,'+');
	  if(tr==NOTYP) printf("compile ERROR: POW: Type mismatch at line %d. %x <--> %x <%s>\n",compile_zeile,
	                        typstack[typsp-2],typstack[typsp-1],funktion);
	  BCADD(BC_POW);TO();TR(PL_CONSTGROUP|tr);
	}
	return(bcerror);
      }    /* von rechts !!  */
      else { xberror(51,w2); return(bcerror); } /* "Parser: Syntax error?! "  */
    }
  }
  if(*s=='(' && s[strlen(s)-1]==')')  { /* Ueberfluessige Klammern entfernen */
    s[strlen(s)-1]=0;
    bc_parser(cb,s+1);
    return(bcerror);
  } 
  /* SystemFunktionen Subroutinen und Arrays */
  
  
  pos=searchchr(s,'(');
  if(pos!=NULL) {
    pos2=s+strlen(s)-1;
    *pos++=0;
    if(*pos2!=')') {
      printf("Closing bracket is missing! at line %d.\n",compile_zeile);
      xberror(51,w2); /* "Parser: Syntax error?! "  */
      return(bcerror);
    }
    /* $-Funktionen und $-Felder   */
    *pos2=0;
    /* Benutzerdef. Funktionen mit parameter  */
    if(*s=='@') {
      int pc2,typ;
      int anzpar=0;

      /* Anzahl Parameter ermitteln */
      anzpar=count_parameters(pos);
      if(verbose>1) printf("function call <%s>(%s)\n",s+1,pos);

      /* Bei Benutzerdef Funktionen muessen wir den Parametertyp erraten.
      
        Besser: Anhand der Funktionsdef eine richtige Parameterliste machen und
	dann diese mit xxxx auf stack...
      
      
      */
      /* Funktionsnr finden */
      pc2=procnr(s+1,PROC_FUNC|PROC_DEFFN);  /*  FUNCTION + DEFFN */
      if(pc2==-1) { 
        xberror(44,s+1); /* Funktion  nicht definiert */
        return(bcerror);
      }

      if(anzpar!=procs[pc2].anzpar) {
        xberror(56,s); /* Falsche Anzahl Parameter */
        return(bcerror);
      }
      if(anzpar) {
        if(push_typeliste(cb,pos,procs[pc2].parameterliste,procs[pc2].anzpar))   {/* Parameter auf den Stack */
          printf("WARNING: something is wrong at line %d!\n",compile_zeile);  
	  return(bcerror);
        }
      }
      bc_push_integer(cb,anzpar);
 
      if(verbose>1) printf("function procnr=%d, anzpar=%d\n",pc2,anzpar);
      typ=procs[pc2].typ;
      if(typ==4) {   /* DEFFN */
	int oldtypsp=typsp;
	BCADD(BC_BLKSTART); /* Stackponter erhoehen etc. */
        BCADD(BC_POP); /* Das waere jetzt die Anzahl der Parameter als int auf Stack*/
        TO();
        int e=procs[pc2].anzpar;
	int *ix=procs[pc2].parameterliste;
	while(--e>=0) {
          bc_local(cb,ix[e]&(~V_BY_REFERENCE));  /* Rette alten varinhalt */
          bc_zuweis(cb,ix[e]&(~V_BY_REFERENCE)); /* Weise vom stack zu */
	  TO();
        }
	// printf("restore-typesp from %d to %d <%s>\n",typsp,oldtypsp,s);
        typsp=oldtypsp;
        bc_parser(cb,pcode[procs[pc2].zeile].argument);
	TO(); /* Typ vom parser weg*/
	BCADD(BC_BLKEND);
      } else {
        bc_jumptosr2(cb,procs[pc2].zeile);
      }
      TO(); /* Anzpar weg*/
      TA(anzpar); /* Einzelne Parameter weg */
      TP(PL_CONSTGROUP|(type(s+1)&TYPMASK)); /* neuer Typ*/
      /*Der Rueckgabewert sollte nach Rueckkehr auf dem Stack liegen.*/
      return(bcerror);
    }
    if((i=find_func(s))!=-1) {
      /* printf("Funktion %s gefunden. Nr. %d\n",pfuncs[i].name,i); */
      /* Jetzt erst die Parameter auf den Stack packen */
//printf("func %s: \n",pfuncs[i].name);      
      if((pfuncs[i].opcode&FM_TYP)==F_SIMPLE || pfuncs[i].pmax==0) {
        BCADD(BC_PUSHFUNC);BCADD(i);BCADD(0);
	TP(PL_CONSTGROUP|returntype(i,NULL,0));
        return(bcerror);
      } else if((pfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
        BCADDPUSHX(pos);
        BCADD(BC_PUSHFUNC);BCADD(i);BCADD(1);
        TP(PL_CONSTGROUP|returntype2(i,pos));
        return(bcerror);
      } else if((pfuncs[i].opcode&FM_TYP)==F_PLISTE || 
        (pfuncs[i].opcode&FM_TYP)==F_DQUICK ||
	 (pfuncs[i].opcode&FM_TYP)==F_IQUICK ||
	 (pfuncs[i].opcode&FM_TYP)==F_CQUICK ||
	 (pfuncs[i].opcode&FM_TYP)==F_SQUICK ||
	 (pfuncs[i].opcode&FM_TYP)==F_AQUICK) {

	int anz;
	anz=count_parameters(pos);
	if((pfuncs[i].pmin>anz && pfuncs[i].pmin!=-1) || 
	   (pfuncs[i].pmax<anz && pfuncs[i].pmax!=-1)) {
	     printf("Warning: Function %s: wrong number of arguments (%d). at line %d\n",pfuncs[i].name,anz,compile_zeile); /*Programmstruktur fehlerhaft */
        }
	if(anz) {
          PARAMETER *par=(PARAMETER *)malloc(sizeof(PARAMETER)*anz);
  	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int ii=0;
          e=wort_sep(pos,',',TRUE,w1,w2);
		while(e) {
	          if(*w1) {
                    par[ii].typ=PL_EVAL;
		    par[ii].pointer=strdup(w1);
     		  } else  {
		    par[ii].typ=PL_LEER;
		    par[ii].pointer=NULL;
		  }
		  e=wort_sep(w2,',',TRUE,w1,w2);
                  ii++;
                }
	  // printf("name=%s %s\n",pfuncs[i].name,pos);
          plist_to_stack(cb,par,(short *)pfuncs[i].pliste,anz,pfuncs[i].pmin,pfuncs[i].pmax);
	  for(ii=0;ii<anz;ii++) free(par[ii].pointer);
          free(par);
	}
	BCADD(BC_PUSHFUNC);BCADD(i);BCADD(anz);
	TA(anz);
	TP(PL_CONSTGROUP|returntype2(i,pos));
	return(bcerror);
      } else {
        printf("Internal ERROR. Function not correktly defined. %s at line %d.\n",s,compile_zeile);
	return(bcerror);
      }
    } else if((i=find_afunc(s))!=-1) {
      if((pafuncs[i].opcode&FM_TYP)==F_SIMPLE || pafuncs[i].pmax==0) {
        BCADD(BC_PUSHAFUNC);BCADD(i);BCADD(0);
      } else if((pafuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
        BCADDPUSHX(pos);
        BCADD(BC_PUSHAFUNC);BCADD(i);BCADD(1);
      } else if((pafuncs[i].opcode&FM_TYP)==F_PLISTE) { 
      /* TODO: das geht besser !*/
        bc_parser(cb,pos);
	BCADD(BC_PUSHAFUNC);BCADD(i);BCADD(count_parameters(pos));
	TA(count_parameters(pos));
      } else if(pafuncs[i].pmax==1 && (pafuncs[i].opcode&FM_TYP)==F_AQUICK) {
        bc_parser(cb,pos);
 	BCADD(BC_PUSHAFUNC);BCADD(i);BCADD(1);
	TO();
      } else if(pafuncs[i].pmax==1 && (pafuncs[i].opcode&FM_TYP)==F_SQUICK) {
        bc_parser(cb,pos);
 	BCADD(BC_PUSHAFUNC);BCADD(i);BCADD(1);
	TO();
      }
      TP(PL_ARRAY); /* Fuer den Rueckgabewert*/
      return(bcerror);
    } else if((i=find_sfunc(s))!=-1) {
      if((psfuncs[i].opcode&FM_TYP)==F_SIMPLE || psfuncs[i].pmax==0) {
        BCADD(BC_PUSHSFUNC);BCADD(i);BCADD(0);
      } else if((psfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
        BCADDPUSHX(pos);
        BCADD(BC_PUSHSFUNC);BCADD(i);BCADD(1);
      } else if((psfuncs[i].opcode&FM_TYP)==F_PLISTE) {                
        bc_parser(cb,pos);
	BCADD(BC_PUSHSFUNC);BCADD(i);BCADD(count_parameters(pos));
	TA(count_parameters(pos));
      } else if(psfuncs[i].pmax==2 && (psfuncs[i].opcode&FM_TYP)==F_DQUICK) {
        bc_parser(cb,pos);
	BCADD(BC_PUSHSFUNC);BCADD(i);BCADD(2);
	TA(2);
      } else if(psfuncs[i].pmax==1 && ((psfuncs[i].opcode&FM_TYP)==F_SQUICK ||
                                       (psfuncs[i].opcode&FM_TYP)==F_AQUICK ||
                                       (psfuncs[i].opcode&FM_TYP)==F_IQUICK ||
                                       (psfuncs[i].opcode&FM_TYP)==F_CQUICK ||
                                       (psfuncs[i].opcode&FM_TYP)==F_DQUICK
				      )) {
        bc_parser(cb,pos);
	BCADD(BC_PUSHSFUNC);BCADD(i);BCADD(1);
	TO();
      }
      TP(PL_STRING); /* Fuer den Rueckgabewert*/
      return(bcerror);
    } else {
       char buf[strlen(s)+1+strlen(pos)+1+1];
       sprintf(buf,"%s(%s)",s,pos);
       bc_pushv_name(cb,buf);
       return(bcerror);
    }
  } else {  /* Also keine Klammern */
    /* Dann Systemvariablen und einfache Variablen und userdef funtktionen */
    /* Liste durchgehen */
    char c=*s;
    int i=0,a=anzsysvars-1,b,l=strlen(s);
    for(b=0; b<l; c=s[++b]) {
      while(c>(sysvars[i].name)[b] && i<a) i++;
      while(c<(sysvars[a].name)[b] && a>i) a--;
      if(i>=a) break;
    }
    if(strcmp(s,sysvars[i].name)==0) {    
      /*  printf("Sysvar %s gefunden. Nr. %d\n",sysvars[i].name,i);*/
      BCADD(BC_PUSHSYS);
      BCADD(i);
      if((sysvars[i].opcode&TYPMASK)==INTTYP) TP(PL_INT);
      else TP(PL_FLOAT);
      return(bcerror);
    } 
    /* Stringsysvars */
    /* Liste durchgehen */
    i=0;
    c=*s;
    a=anzsyssvars-1;
    l=strlen(s);
    for(b=0; b<l; c=s[++b]) {
      while(c>(syssvars[i].name)[b] && i<a) i++;
      while(c<(syssvars[a].name)[b] && a>i) a--;
      if(i>=a) break;
    }
    if(strcmp(s,syssvars[i].name)==0) {
	    /*  printf("Sysvar %s gefunden. Nr. %d\n",syssvars[i].name,i);*/
      BCADD(BC_PUSHSSYS);
      BCADD(i);
      TP(PL_STRING);
      return(bcerror);
    }

  /* Arraysysvars 
    //TODO  */
   
  /* erst integer abfangen (xxx% oder xxx&), dann rest */
  
  
  
  
  if(*s=='@') {   /*  Funktionsaufrufe ohne Parameterliste ....*/
    /* Funktionsnr finden */
    if(verbose>1) printf("function call <%s>\n",s+1);
    
    int pc2=procnr(s+1,PROC_FUNC|PROC_DEFFN);
    if(pc2==-1) { 
      xberror(44,s+1); /* Funktion  nicht definiert */
      return(bcerror);
    } 
    if(procs[pc2].anzpar!=0) {
      xberror(56,s); /* Falsche Anzahl Parameter */
      return(bcerror);
    }
    bc_push_integer(cb,0); /* Keine Parameter*/

    if(verbose>1) printf("function procnr=%d\n",pc2);
    typ=procs[pc2].typ;
    if(typ==4) {   /* DEFFN */
      BCADD(BC_BLKSTART); /* Stackponter erhoehen etc. */
      BCADD(BC_POP); /* Das waere jetzt die Anzahl der Parameter als int auf Stack*/
      TO();  /* ANzpar weg */
      bc_parser(cb,pcode[procs[pc2].zeile].argument);
      TO(); /* Typ vom parser verwerfen*/
      BCADD(BC_BLKEND);
    } else {
      bc_jumptosr2(cb,procs[pc2].zeile);
      TO();  /* ANzpar weg */
    }
    TP(PL_CONSTGROUP|(type(s+1)&TYPMASK));
    /*Der Rueckgabewert sollte nach Rueckkehr auf dem Stack liegen.*/
    return(bcerror);
  } 
    if(*s=='#') { /* Filenummer ?*/
      int i=0;
      if(verbose>1) printf("file number: %s\n",s);
      while(s[++i]) s[i-1]=s[i];
      s[i-1]=s[i];
    }
    /* Testen ob und was fuer eine Konstante das ist. */
    e=type(s);
    if(e&CONSTTYP) {
      switch(e&TYPMASK) {
      case INTTYP:      bc_push_integer(cb,(int)parser(s));   return(bcerror);
      case FLOATTYP:    bc_push_float(cb,parser(s));          return(bcerror);
      case COMPLEXTYP:  bc_push_complex(cb,complex_parser(s));return(bcerror);
      case STRINGTYP: {
        STRING str=string_parser(s);
	bc_push_string(cb,str);
        free(str.pointer);
	}
        return(bcerror);
      case ARBINTTYP: {
        ARBINT a;
	mpz_init(a);
	arbint_parser(s,a);
	bc_push_arbint(cb,a);
        mpz_clear(a);
	}
        return(bcerror);
      default:
        printf("ERROR: cannot handle this const type <%s>: %s\n",type_name(e),s); 
      }
    } else {           /* Keine Konstante, also Variable */
      bc_pushv_name(cb,s);
      return(bcerror);
    }
  }
  xberror(51,s); /* Syntax error */
  bcerror=51;
  return(bcerror);
}


/* Put a string (0 terminated) into the stringbuffer and return the offset into it. 
 * The string is added if it does not yet exist. 
 */

static int add_string(COMPILE_BLOCK *cb,const char *n) {
  if(n) {
    int ol=cb->stringseglen;
    int i=0;
    /* Suche, ob schon vorhanden */
    while(i<ol) {
      if(!strncmp(&((cb->stringseg)[i]),n,strlen(n)+1)) return(i);
      i+=strlen(&((cb->stringseg)[i]))+1;
    }  
    cb->stringseglen+=strlen(n)+1;
    cb->stringseg=realloc(cb->stringseg,cb->stringseglen);
    strncpy(&((cb->stringseg)[ol]),n,strlen(n)+1);
    return(ol);
  } else return(0);
}

static void add_symbol(COMPILE_BLOCK *cb,int adr,char *name, unsigned char typ,unsigned char subtyp) {
  cb->symtab=realloc(cb->symtab,(cb->anzsymbols+1)*sizeof(BYTECODE_SYMBOL));
  cb->symtab[cb->anzsymbols].name=add_string(cb,name);
  cb->symtab[cb->anzsymbols].typ=typ;
  cb->symtab[cb->anzsymbols].subtyp=subtyp;
  cb->symtab[cb->anzsymbols].adr=adr;
  cb->anzsymbols++;
}


int add_rodata(COMPILE_BLOCK *cb,char *data,int len) {
  if(len==0) return(0);
  if(cb->rodataseglen==0) {
    cb->rodataseg=realloc(cb->rodataseg,(len+3)&0xfffffffc); /* auf loglaenge bringen*/
    memcpy(cb->rodataseg,data,len);
    cb->rodataseglen=len;
    return(0);
  } 
 // printf("addrodata: p=%p rl=%d data=<%s> l=%d\n",cb->rodataseg, cb->rodataseglen,data,len);
  char *a=memmem(cb->rodataseg,cb->rodataseglen,data,len);
  if(a==NULL) {
    cb->rodataseg=realloc(cb->rodataseg,(cb->rodataseglen+len+3)&0xfffffffc); /* auf loglaenge bringen*/
    memcpy(cb->rodataseg+cb->rodataseglen,data,len);
    cb->rodataseglen+=len;
    return(cb->rodataseglen-len);
  } 
  return((int)(a-cb->rodataseg));
}


inline static int add_bss(COMPILE_BLOCK *cb,int len) {
  cb->bsseglen+=len;
  return(cb->bsseglen-len);
}


static void bc_kommando(COMPILE_BLOCK *cb,int idx) {
  char buffer[strlen(program[idx])+1];
  char w1[strlen(program[idx])+1];
  char w2[strlen(program[idx])+1],zeile[strlen(program[idx])+1];
  char *pos;
  int i,a,b;
  xtrim(program[idx],TRUE,zeile);
  wort_sep2(zeile," !",TRUE,zeile,buffer);
  if(*buffer!=0) bc_comment(cb,buffer);
  xtrim(zeile,TRUE,zeile);
  if(wort_sep(zeile,' ',TRUE,w1,w2)==0) {
    if(cb->comp_flags&COMPILE_NOOPS) {
      BCADD(BC_NOOP);
      if(verbose>1) printf(" bc_noop ");
    }
    return;
  }  /* Leerzeile */
  if(*w1=='\'') {  /* comment */
    char *p=program[idx];
    while(*p==' ' || *p=='\t') p++;
    if(*p) bc_comment(cb,p+1);
    return;
  }  /* Kommentar */
  if(w1[strlen(w1)-1]==':') {
    if(cb->comp_flags&COMPILE_NOOPS) {
      BCADD(BC_NOOP);
      if(verbose>1) printf(" bc_noop ");
      add_string(cb,w1);
    }
    return;
  }  /* nixtun, label */
  if(*w1=='@') { /*sollte hier nicht vorkommen, da gosubs nicht ueber eval....*/
    /* TODO: Wenn Parameter, */
    /* Parameter auf den Stack*/
    /* Anzahl Parameter ermitteln */
    /* Zieladresse ermitteln */
    /* BC_BSR  */
    printf("ERROR: compiler error --> @ at line %d\n",compile_zeile);
    return;
  }
  if(*w1=='&') {
    bc_parser(cb,w1+1);
    if(TL!=PL_STRING) printf("WARNING: EVAL <%s> wants a string! at line %d\n",w1+1,compile_zeile);
    BCADD(BC_EVAL);
    TO();
    if(verbose>1) printf(" bc_eval <%s> ",w1+1);
    return;
  }
  if(*w1=='~') {
    bc_parser(cb,zeile+1);
    /* einmal POP, um den Rueckgabewert zu vernichten */
    BCADD(BC_POP);
    TO();
    return;
  }
  if((pos=searchchr2(w1,'='))!=NULL) { /* Zuweisung */
    *pos++=0;
    bc_zuweisung(cb,w1,pos);
    return;
  }
  if(isdigit(*w1) || *w1=='(') {
     bc_parser(cb,w1);
     /* Hm, der Kram landet dann auf dem Stack...*/
     return;
  }
  /* Kommandoliste durchsuchen, moeglichst effektiv ! */

  i=0;a=anzcomms-1;
  for(b=0; b<strlen(w1); b++) {
    while(w1[b]>(comms[i].name)[b] && i<a) i++;
    while(w1[b]<(comms[a].name)[b] && a>i) a--;
#ifdef DEBUG
    printf("%c:%d,%d: %s: %s <--> %s \n",w1[b],i,a,w1,comms[i].name,comms[a].name);
#endif
    if(i>=a) break;
  }
  if((i==a && strncmp(w1,comms[i].name,strlen(w1))==0) ||
     (i!=a && strcmp(w1,comms[i].name)==0) ) {
#ifdef DEBUG
      if(b<strlen(w1)) printf("Befehl %s vervollstaendigt --> %s\n",w1,comms[i].name);
#endif
      switch(comms[i].opcode&PM_TYP) {
      case P_IGNORE: BCADD(BC_NOOP);return;
      case P_ARGUMENT:
        BCADD(BC_PUSHX);BCADD(strlen(w2));
        memcpy(&cb->textseg[cb->textseglen],w2,strlen(w2));
	cb->textseglen+=strlen(w2);
        BCADD(BC_PUSHCOMM);BCADD(i);BCADD(1);
        return;
      case P_SIMPLE: BCADD(BC_PUSHCOMM);BCADD(i);BCADD(0);return;
      case P_PLISTE: 
        if(strlen(w2)) bc_parser(cb,w2);
	/* Anzahl Prameter herausfinden */
        /* Warnen wenn zuwenig oder zuviel parameter */
        BCADD(BC_PUSHCOMM);BCADD(i);
	BCADD(count_parameters(w2));
	TA(count_parameters(w2));
        return;
      default: xberror(38,w1); /* Befehl im Direktmodus nicht moeglich */
      }
    } else if(i!=a) {
       printf("Command not unique! <%s...%s>\n",comms[i].name,comms[a].name);
    }  else xberror(32,w1);  /* Syntax Error */
}

static void bc_restore(COMPILE_BLOCK *cb,int offset) {
  if(verbose>1) printf("RESORE:offset=%d ",offset);
  cb->textseg[cb->textseglen++]=BC_RESTORE;
  CP4(&cb->textseg[cb->textseglen],&offset,cb->textseglen);
}

/* 
 * Vollführt eine bedingte Verzweigung von Zeile from nach Zeile ziel. 
 * Wenn eqflag=1 dann wird 
 * der letzte Wert auf dem Stack mit Null verglichen und nur dann 
 * gesprungen.
 */


static void bc_jumpto(COMPILE_BLOCK *cb,int from, int ziel, int eqflag) {
  if(TL!=PL_INT && eqflag) printf("WARNING: EQ: no int on stack !\n");
  if(ziel<=from) { /* Dann ist das Ziel schon bekannt */
        int a=cb->bc_index[ziel];
	add_symbol(cb,a,NULL,STT_NOTYPE,0);
	int b=a-cb->textseglen;
	if(verbose>1) printf("Delta=%d ",b);
        if(b<=127 && b>=-126) {
          if(verbose>1) printf(" %s.s ",(eqflag?"BEQ":"BRA"));
	  cb->textseg[cb->textseglen++]=(eqflag?BC_BEQs:BC_BRAs);
	  cb->textseg[cb->textseglen++]=b-2;
	} else if(b<=0x7fff && b>=-32765) {
	  short o=b-1-sizeof(short);
          if(verbose>1) printf(" %s ",(eqflag?"BEQ":"BRA"));
	  cb->textseg[cb->textseglen++]=(eqflag?BC_BEQ:BC_BRA);
          CP2(&cb->textseg[cb->textseglen],&o,cb->textseglen);
	} else {
          if(verbose>1) printf(" %s ",(eqflag?"JEQ":"JMP"));
          cb->textseg[cb->textseglen++]=(eqflag?BC_JEQ:BC_JMP);
          CP4(&cb->textseg[cb->textseglen],&a,cb->textseglen);
	}
	if(verbose>1) printf("---> $%x ",a);
  } else {
     if(verbose>1) printf(" %s ",(eqflag?"JEQ":"JMP"));
     cb->textseg[cb->textseglen++]=(eqflag?BC_JEQ:BC_JMP);	    
     CP4(&cb->textseg[cb->textseglen],&ziel,cb->textseglen);
     cb->reltab[cb->anzreloc++]=cb->textseglen-4;
  }
  if(eqflag) TO();
}

static void bc_jumptosr(COMPILE_BLOCK *cb,int from, int ziel) {
       /* Hier jetzt ein JUMP realisieren */
      if(ziel<=from) { /* Dann ist das Ziel schon bekannt */
        int a,b;
        a=cb->bc_index[ziel];
	b=a-cb->textseglen;
	if(verbose>1) printf("Delta=%d ",b);
        if(b<=0x7fff && b>=-32766) {
	  short o=b-1-sizeof(short);
          if(verbose>1) printf(" BSR ");
	  BCADD(BC_BSR);
          CP2(&cb->textseg[cb->textseglen],&o,cb->textseglen);
	} else {
          if(verbose>1) printf(" JSR ");
          BCADD(BC_JSR);
          CP4(&cb->textseg[cb->textseglen],&a,cb->textseglen);
	}
  } else {
    if(verbose>1) printf(" JSR ");
    BCADD(BC_JSR);
    CP4(&cb->textseg[cb->textseglen],&ziel,cb->textseglen);
    cb->reltab[cb->anzreloc++]=cb->textseglen-4;
  }
}

static void bc_jumptosr2(COMPILE_BLOCK *cb,int ziel) {
  /* Hier jetzt ein JUMP realisieren */
  if(verbose>1) printf(" JSR ");
  BCADD(BC_JSR);
  CP4(&cb->textseg[cb->textseglen],&ziel,cb->textseglen);
  cb->reltab[cb->anzreloc++]=cb->textseglen-4;
}

static void bc_pushlabel(COMPILE_BLOCK *cb,int ziel) {
  BCADD(BC_PUSHLABEL);TP(PL_LABEL);
  CP4(&cb->textseg[cb->textseglen],&ziel,cb->textseglen);
  cb->reltab[cb->anzreloc++]=cb->textseglen-4;
}
static void bc_pushproc(COMPILE_BLOCK *cb,int ziel) {
  BCADD(BC_PUSHPROC);TP(PL_PROC);
  CP4(&cb->textseg[cb->textseglen],&ziel,cb->textseglen);
  cb->reltab[cb->anzreloc++]=cb->textseglen-4;
}
static void bc_pushnumparameter(COMPILE_BLOCK *cb,PARAMETER *p) {
  switch(p->typ) {
  case PL_LEER:   BCADD(BC_PUSHLEER);TP(PL_LEER);break;
  case PL_EVAL:   bc_parser(cb,p->pointer);	 break;
  case PL_FLOAT:  bc_push_float(cb,p->real);	 break;
  case PL_INT:
  case PL_FILENR: bc_push_integer(cb,p->integer); break;
  case PL_COMPLEX:bc_push_complex(cb,*((COMPLEX *)&(p->real)));break;
  case PL_ARBINT: bc_push_arbint(cb,*((ARBINT *)(p->pointer)));break;
  default: printf("WARNING: something is wrong at line %d! typ=<%s>\n",compile_zeile,type_name(p->typ));
  }
}
static void bc_pushvalueparameter(COMPILE_BLOCK *cb,PARAMETER *p) {
  switch(p->typ) {
  case PL_LEER:   BCADD(BC_PUSHLEER);TP(PL_LEER);break;
  case PL_EVAL:   bc_parser(cb,p->pointer);	 break;
  case PL_FLOAT:  bc_push_float(cb,p->real);	 break;
  case PL_INT:
  case PL_FILENR: bc_push_integer(cb,p->integer); break;
  case PL_COMPLEX:bc_push_complex(cb,*((COMPLEX *)&(p->real)));break;
  case PL_ARBINT: bc_push_arbint(cb,*((ARBINT *)(p->pointer)));break;
  case PL_STRING: {
    STRING str;
    str.len=p->integer;
    str.pointer=p->pointer;
    bc_push_string(cb,str);
    } break;
  default: printf("WARNING: something is wrong at line %d! typ=<%s>\n",compile_zeile,type_name(p->typ));
  }
}
static void bc_pushanyparameter(COMPILE_BLOCK *cb,PARAMETER *p) {
  switch(p->typ) {
  case PL_LEER:   BCADD(BC_PUSHLEER);TP(PL_LEER);break;
  case PL_EVAL:   bc_parser(cb,p->pointer);	 break;
  case PL_FLOAT:  bc_push_float(cb,p->real);	 break;
  case PL_INT:
  case PL_FILENR: bc_push_integer(cb,p->integer); break;
  case PL_COMPLEX:bc_push_complex(cb,*((COMPLEX *)&(p->real)));break;
  case PL_ARBINT: bc_push_arbint(cb,*((ARBINT *)(p->pointer)));break;
  case PL_STRING: {
    STRING str;
    str.len=p->integer;
    str.pointer=p->pointer;
    bc_push_string(cb,str);
    } break;
  case PL_ARRAY:
  case PL_IARRAY:
  case PL_FARRAY:
  case PL_CARRAY:
  case PL_AIARRAY:
  case PL_NARRAY:
  case PL_SARRAY: {
    ARRAY arr=*(ARRAY *)&(p->integer);
    bc_push_array(cb,arr);
    } break;
  default: printf("WARNING: something is wrong at line %d! typ=<%s>\n",compile_zeile,type_name(p->typ));
  }
}

static void bc_print_arg(COMPILE_BLOCK *cb,const char *ausdruck,char *code) {
  char w1[strlen(ausdruck)+1],w2[strlen(ausdruck)+1];
  char w3[strlen(ausdruck)+1],w4[strlen(ausdruck)+1];
  // printf("bc_print_arg: <%s>  code=<%s>\n",ausdruck,code);
  int e=arg2(ausdruck,TRUE,w1,w2);
  while(e) {
    // printf("Teilausdruck: <%s>  e=%d\n",w1,e);
    if(strncmp(w1,"AT(",3)==0) {
      w1[strlen(w1)-1]=0;
      wort_sep(w1+3,',',TRUE,w3,w4);
      if(strlen(code)) strcat(code,"+");
      sprintf(code+strlen(code),"CHR$(27)+\"[\"+STR$(INT(%s))+\";\"+STR$(INT(%s))+\"H\"",w3,w4);
    } else if(strncmp(w1,"TAB(",4)==0) {
       w1[strlen(w1)-1]=0;
       if(strlen(code)) strcat(code,"+");
       sprintf(code+strlen(code),"CHR$(13)+CHR$(27)+\"[\"+STR$(INT(%s))+\"C\"",w1+4);
    } else if(strncmp(w1,"SPC(",4)==0) {
       w1[strlen(w1)-1]=0;
       if(strlen(code)) strcat(code,"+");
       sprintf(code+strlen(code),"CHR$(27)+\"[\"+STR$(INT(%s))+\"C\"",w1+4);
    } else if(strncmp(w1,"COLOR(",6)==0) {
      w1[strlen(w1)-1]=0;
      wort_sep(w1+6,',',TRUE,w3,w4);
      if(strlen(code)) strcat(code,"+");
      sprintf(code+strlen(code),"CHR$(27)+\"[\"+STR$(INT(%s))+",w3);
      int i=wort_sep(w4,',',TRUE,w3,w4);
      while(i) {
        sprintf(code+strlen(code),"\";\"+STR$(INT(%s))+",w3);
        i=wort_sep(w4,',',TRUE,w3,w4);
      }
      sprintf(code+strlen(code),"\"m\"");
    } else {
      /* Hier noch PRINT USING abfangen ...*/
      int ee=wort_sep2(w1," USING ",TRUE,w1,w4);
      if(ee==2) {
        if(strlen(code)) strcat(code,"+");
        sprintf(code+strlen(code),"USING$(%s,%s)",w1,w4);
      } else if(strlen(w1)) {
        if(strlen(code)) strcat(code,"+");
	if((type(w1)&(~CONSTTYP))==STRINGTYP) sprintf(code+strlen(code),"%s",w1);
	else sprintf(code+strlen(code),"STR$(%s)",w1);
      }
    }  /* Hier liegt nun irgendeine Art String auf dem STack.*/
    
    if(e==2) ;
    else if(e==3) {
      if(strlen(code)) strcat(code,"+CHR$(9)");
      else strcat(code,"CHR$(9)");
    } else if(e==4) {
      if(strlen(code)) strcat(code,"+\" \"");
      else strcat(code,"\" \"");
    }
    e=arg2(w2,TRUE,w1,w2);
  }
  // printf("code=<%s>\n",code);
}

static void plist_to_stack(COMPILE_BLOCK *cb,PARAMETER *pin, short *pliste, int anz, int pmin, int pmax) {
  int anzpar;
  unsigned short ap,ip;
  if(pmax==-1) anzpar=anz;
  else anzpar=min(anz,pmax);
  if(anzpar<pmin) printf("Not enough parameters. at line %d.\n",compile_zeile);
  int i=0;
  while(i<anzpar) {
    if(i>pmin && pmax==-1) ap=pliste[pmin];
    else ap=pliste[i];
    ip=pin[i].typ;
    switch(ap) {
      int vnr;  
    case PL_LEER:
      BCADD(BC_PUSHLEER);TP(PL_LEER);
      if(verbose>1) printf(" empty \n");
      break;
    case PL_LABEL: 
      bc_pushlabel(cb,labels[pin[i].integer].zeile);
      break;
    case PL_PROC:
      bc_pushproc(cb,procs[pin[i].integer].zeile);
      break;
    case PL_FILENR:
    case PL_INT:    /* Integer */
      bc_pushnumparameter(cb,&pin[i]);
      if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
      break;
    case PL_COMPLEX:
      bc_pushnumparameter(cb,&pin[i]);
      if(TL!=PL_COMPLEX) BCADD(BC_X2C);TR(PL_COMPLEX);
      break;  
    case PL_ARBINT:
      bc_pushnumparameter(cb,&pin[i]);
      if(TL!=PL_ARBINT) BCADD(BC_X2AI);TR(PL_ARBINT);
      break;  
    case PL_NUMBER:  /* Float, Int, Complex oder ARBINT */
      bc_pushnumparameter(cb,&pin[i]);
      break;
    case PL_CFAI:  /* Float oder complex oder arbint */
      bc_pushnumparameter(cb,&pin[i]);
      if(TL==PL_INT) {BCADD(BC_I2F);TR(PL_FLOAT);}
      break;
    case PL_CF:  /* Float oder complex*/
      bc_pushnumparameter(cb,&pin[i]);
      if(TL==PL_INT) {BCADD(BC_I2F);TR(PL_FLOAT);}
      else if(TL==PL_ARBINT) {BCADD(BC_X2F);TR(PL_FLOAT);}
      break;
    case PL_FLOAT:  /* Float oder Number */
      bc_pushnumparameter(cb,&pin[i]);
      if(TL==PL_INT) {BCADD(BC_I2F);TR(PL_FLOAT);}
      else if(TL!=PL_FLOAT) {BCADD(BC_X2F);TR(PL_FLOAT);}
      break;
    case PL_ANY:
      bc_pushanyparameter(cb,&pin[i]);
      break;
    case PL_ANYVALUE:
      bc_pushvalueparameter(cb,&pin[i]);
      break;
    case PL_STRING: /* String */
      if(ip==PL_LEER) {BCADD(BC_PUSHLEER);TP(PL_LEER);}
      else if(ip==PL_EVAL) {
        bc_parser(cb,pin[i].pointer);if(TL!=PL_STRING) printf("WARNING: result is not a string!\n %s",(char *)pin[i].pointer);
      } else if(ip==PL_STRING) {
        STRING str;
	str.len=pin[i].integer;
	if(verbose>1) printf("\"%s\" ",(char *)pin[i].pointer);
	str.pointer=pin[i].pointer;
	bc_push_string(cb,str);	
      } else printf("WARNING: something is wrong at line %d! typ=<%s>\n",compile_zeile,type_name(pin[i].typ));

      break;
    case PL_DIMARG:  /* Array definition */
      if(ip==PL_DIMARG) { /* fertig ausgewetetes DIMARG (kommt eigentlich nicht vor) */
         ; /* nixtun */
      } else if((ip&PL_VARGROUP)==PL_VARGROUP) { /*vorausgewerteres DIMARG*/
        vnr=pin[i].integer;
        // printf("TODO: DIM Arrayvartyp vnr=%d dimension=%d\n",vnr,pin[i].panzahl);
        if(pin[i].panzahl) {
	  bc_indexliste(cb,pin[i].ppointer,pin[i].panzahl);
	  bc_pushdimargindex(cb,vnr,pin[i].panzahl);
        } 
      } else printf("WARNING: something is wrong at line %d! typ=<%s> %x\n",compile_zeile,type_name(pin[i].typ),pin[i].typ);
      break;
    case PL_ARRAY:  /* Array */
    case PL_IARRAY: /* Int-Array */
    case PL_FARRAY: /* float-Array */
    case PL_SARRAY: /* String-Array */
      if(ip==PL_LEER) {BCADD(BC_PUSHLEER);TP(PL_LEER);}
      else if(pin[i].typ!=PL_EVAL) printf("WARNING: something is wrong at line %d! typ=<%s>\n",compile_zeile,type_name(pin[i].typ));
      else {bc_parser(cb,pin[i].pointer);if(TL!=PL_ARRAY) printf("WARNING: result is not an array!\n %s",(char *)pin[i].pointer);}
      break;
    case PL_VAR:   /* Variable */
    case PL_NVAR:   /* Variable */
    case PL_SVAR:   /* Variable */
    case PL_ARRAYVAR: /* Variable */
    case PL_IARRAYVAR: /* Variable */
    case PL_FARRAYVAR: /* Variable */
    case PL_SARRAYVAR: /* Variable */
    case PL_ANYVAR:  /* Varname */ 
      if(ip==PL_LEER) {BCADD(BC_PUSHLEER);TP(PL_LEER);}
      else if(pin[i].typ==PL_EVAL) {
 	/* Hier muss die PLISTE noch erstellt werden....*/
        PARAMETER pret;
	pret.panzahl=0;
	if(prepare_vvar(pin[i].pointer,&pret,ap)==-1) {
	  printf("WARNING: something is wrong at line %d!\n",compile_zeile);
	  dump_parameterlist(&pin[i],1);
	} else {
          // dump_parameterlist(&pret,1);
	  vnr=pret.integer;
          if(pret.panzahl) {
	    bc_indexliste(cb,pret.ppointer,pret.panzahl);
	    bc_pushvvindex(cb,vnr,pret.panzahl);
          } else bc_pushvv(cb,vnr);
	  free_parameter(&pret);
        }
      } else {
        vnr=pin[i].integer;
        if(pin[i].panzahl) {
	  bc_indexliste(cb,pin[i].ppointer,pin[i].panzahl);
	  bc_pushvvindex(cb,vnr,pin[i].panzahl);
        } else bc_pushvv(cb,vnr);
      }
      break;
    case PL_KEY: /* Keyword */
      if(ip==PL_LEER) {BCADD(BC_PUSHLEER);TP(PL_LEER);}
      else {
        if(pin[i].arraytyp!=KEYW_UNKNOWN) {
	  BCADD(BC_PUSHK);
	  BCADD(pin[i].arraytyp);
	} else { BCADDPUSHX(pin[i].pointer); } 
	TP(PL_KEY);
      }
      break;
    case PL_EVAL: /* Keyword */
      if(ip==PL_LEER) {BCADD(BC_PUSHLEER);TP(PL_LEER);}
      else {
        BCADDPUSHX(pin[i].pointer);TP(PL_EVAL);
      }
      break;
    default:
      printf("unknown parameter type.$%x\n",ap);
    }    
    i++;
  }
}
/*Indexliste aus Parameterarray (mit EVAL) auf stack*/

static int push_indexliste(COMPILE_BLOCK *cb,PARAMETER *p,int n) {
  int i;
  for(i=0;i<n;i++) {
    switch(p[i].typ) {
    case PL_KEY:
    case PL_EVAL:
      if(((char *)p[i].pointer)[0]==':') {
	bc_push_integer(cb,-1);
      } else {
        bc_parser(cb,(char *)p[i].pointer);
        if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
      }
      break;
    case PL_INT:
      bc_push_integer(cb,p[i].integer);
      break;
    case PL_NUMBER:
    case PL_CF:
    case PL_CFAI:
    case PL_FLOAT:
      bc_push_integer(cb,p2int(&p[i]));
      break;
    default:
      printf("WARNING: get_indexliste: unknown type.\n");
      bc_push_integer(cb,0);
    }
  }
  return(i);
}


/*Schreibt werte aus einer kommaseparierten Liste auf den Stack, dabei werden die Typen anhand einer
Variablenliste berücksichtigt. 

Return: 0 -- alles OK
       -1 -- something is wrong
*/

static int push_typeliste(COMPILE_BLOCK *cb,const char *argument, int *varlist,int n) {
  char *pos=strdup(argument);
  char *w1,*w2;
  int i=0,vnr,typ;
  PARAMETER p;
  int e=wort_sep_destroy(pos,',',TRUE,&w1,&w2);
 // printf("Funktionsaufruf mit %d parametern. <%s>\n",n,argument); 
  p.pointer=NULL;
  p.panzahl=0;
  while(e) {
    if((varlist[i]&V_BY_REFERENCE)==V_BY_REFERENCE) {
      vnr=(varlist[i]&(~V_BY_REFERENCE));
      typ=variablen[vnr].typ;
     // printf("<%s> --> vnr=%d, typ=%x \n",w1,vnr,typ);
      if(typ==ARRAYTYP) typ|=variablen[vnr].pointer.a->typ;
      if(prepare_vvar(w1,&p,(PL_VARGROUP|typ))==-1) return(-1);
      // dump_parameterlist(&p,1);
      bc_pushvv(cb,p.integer); 
      free_parameter(&p);
    } else  {
      bc_parser(cb,w1);
    }
    i++;
    e=wort_sep_destroy(w2,',',TRUE,&w1,&w2);
  }
  free(pos);
  return(0);
}





/* compile PRINT statement...
 * Im Wesendlichen wollen wir hier den ganzen PRINT-Austruck 
 * in einen OUT STRING ausdruck umwandeln.
 * Return: 
 *        0 -- everything OK
 *       -1 -- error, compilation need to be aborded
 */
/* Das ganze geht noch nicht. PRINT USING (fur Strings) wird noch nicht korrekt behandelt, weiterhin
Parameter 0 und negativ bei PRINT AT() COLOR(), TAB(), SPC() etc...
Hier ist also noch ziemlicher Bahnhof ! */
static int compile_print(COMPILE_BLOCK *cb,P_CODE pcode) {
  if(pcode.panzahl>0) {
  // printf("PRINT Sonderbehandlung: \n");
  // printf("%d parameter:\n",pcode.panzahl);
  // dump_parameterlist(pcode.ppointer,pcode.panzahl);
  #ifdef ATARI
    char code[200];
  #else
    char code[1000];
  #endif
    code[0]=0;
    PARAMETER *p=pcode.ppointer;
    char *n;
    for(int j=0;j<pcode.panzahl;j++) {
      if(p[j].typ==PL_EVAL) {
        n=(char *)p[j].pointer;
        if(j==0) {
          if(*n=='#') {
            bc_parser(cb,n+1);
            //if(TL==PL_FLOAT) {BCADD(BC_F2I);TR(PL_INT);}
            // else 
            if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
            BCADD(BC_I2FILE);TR(PL_FILENR);
          } else {BCADD(BC_PUSHLEER);TP(PL_LEER); }
          if(*n=='#') continue;
        } 
        bc_print_arg(cb,n,code);  /* Ergebnis ist dann ein String-Parameter auf dem Stack.*/
        if(n[strlen(n)-1]!=';' && n[strlen(n)-1]!='\'' && j==pcode.panzahl-1) strcat(code,"+CHR$(10)");
        else if(n[strlen(n)-1]!=';' && n[strlen(n)-1]!='\'' && j<pcode.panzahl-1) strcat(code,"+CHR$(9)");
        // printf("n=<%s> --> <%s>\n",n,code);
      } else if(p[j].typ==PL_LEER) {
        ; /* nixtun !*/
      } else {
        /*Hier stimmt was nicht, denn das ist so noch nicht implementiert.*/
        printf("WARNING: noeval\n");
        plist_to_stack(cb,p+j,(short *)comms[find_comm("PRINT")].pliste,1,0,-1);
      }
    }
    if(TL!=PL_LEER && TL!=PL_FILENR) printf("WARNING: something is wrong at line %d! %x\n",compile_zeile,TL);
    // printf("marker %d\n",typsp);
    //printf("--> <%s>\n",code);
    if(*code) bc_parser(cb,code);
    else { /* offenbar war der ganze Ausdruck leer, also PRINT ohne Argumente */
      // printf("PRINT leerer Ausdruck ....\n");
     /* Wenn hier der String leer ist, bzw. auf dem Typstack ein LEER liegt, 
     sollten wir an dieser Stelle ein String mit chr$(10) machen.
     */

      STRING str;
      str.len=1;
      str.pointer=malloc(2);
      str.pointer[0]='\n';
      str.pointer[1]=0;
      bc_push_string(cb,str);
      free(str.pointer);
    }
    if(TL!=PL_STRING) printf("WARNING: something is wrong at line %d! %x\n",compile_zeile,TL);
    /* Hier muessen 2 Parameter auf dem Stack liegen: 
       1. Filenr oder leer
       2. STRING
       */
    BCADD(BC_PUSHCOMM);BCADD(find_comm("OUT"));
    BCADD(2); /* Parameter Anzahl = 2*/
    TA(2);    /* nimm zwei typen weg */
  } else {  /* PRINT without argument */
    BCADD(BC_PUSHCOMM);BCADD(find_comm("PRINT"));
    BCADD(pcode.panzahl);
    TA(pcode.panzahl);
  }
  return(0);
}



/* (re-)create structures for the compilation process...
 * If a bytecode has already been loaded, it need to be removed.
 */
static int last_proc_number;
static void init_compile(COMPILE_BLOCK *cb) {
  if(cb->status==0) {  /* fresh block, all memory need to be allocated. */
    cb->textseg=realloc(cb->textseg,MAX_CODE);
  #ifdef ATARI
    if(cb->textseg==NULL) {
      perror("malloc");
      printf("ERROR: Need at least %d Bytes free memory. %d\n",MAX_CODE,malloc(-1));
      sleep(5);
      exit(-1);
    } else printf("Buffer at %p\n",cb->textseg);
  #endif
  }
  cb->bc_index=realloc(cb->bc_index,prglen*sizeof(int));
  cb->prglen=prglen;
  cb->textseglen=0;
  cb->rodataseglen=0;
  cb->anzsymbols=0;
  cb->relseglen=max(1000,prglen)*sizeof(int);
  cb->reltab=realloc(cb->reltab,cb->relseglen);
  cb->anzreloc=0;
  cb->textseglen=0;
  cb->stringseglen=0;
  typsp=0;
  last_proc_number=NOTYP;
  cb->bc_version=BC_VERSION;
  cb->sdataseg=databuffer;
  cb->sdataseglen=databufferlen;
}
void free_cb(COMPILE_BLOCK *cb) {
  if(cb->status==0) {  /* all memory should be freed. */
    free(cb->textseg);
    cb->textseg=NULL;
    free(cb->rodataseg);
    cb->rodataseg=NULL;
    free(cb->dataseg);
    cb->dataseg=NULL;
    free(cb->bc_index);
    cb->bc_index=NULL;
    free(cb->reltab);
    cb->reltab=NULL;
  }
}
/* compiles one line and produces bytecode for it.
 * adds to bcpc.
 * return: 0 -- not done
 *         1 -- done
 *        -1 -- error, compilation needs to be stopped.
 */

static int compile_line(COMPILE_BLOCK *cb,int lnr) {
  cb->bc_index[lnr]=cb->textseglen;   /* Save bytecode offset for this line.*/
  compile_zeile=lnr;
  if(verbose>1) {
    printf("\n%3d:$%08x %x_[%08xx%d](%d)%s \t |",lnr,cb->textseglen,pcode[lnr].etyp,(unsigned int)pcode[lnr].opcode,pcode[lnr].integer,
      pcode[lnr].panzahl,program[lnr]);
      fflush(stdout);
  }
  if(pcode[lnr].opcode&P_INVALID) {
    xberror(32,program[lnr]); /* Syntax error at line ... */
    return(-1);
  }
  
  
  /* PREFETCH means that the command can be ignored as such, 
   * ELSE, LOOP, WEND, BREAK, also CONTINUE and GOTO. 
   * We just do the branch/jump to the other line.
   * With ELSEIF we have to make an exception.
   * because an expression has to be evaluated first in that case.
   */
    
  if((pcode[lnr].opcode&P_PREFETCH) && !((pcode[lnr].opcode&PM_SPECIAL)==P_ELSEIF)) {
    bc_jumpto(cb,lnr,pcode[lnr].integer,0);
    return(1);
  }
  /* direct commands: */
  switch(pcode[lnr].opcode) {
    case P_PROC: {
      int e;
      int *ix;
      last_proc_number=pcode[lnr].integer;  /* Save the procedure we are compiling, to reconstruct return type later.*/
      if(verbose>1) printf(" PROC_%d %s ",pcode[lnr].integer,(char *)procs[pcode[lnr].integer].parameterliste);
      add_symbol(cb,cb->textseglen,procs[pcode[lnr].integer].name,STT_FUNC,0); /* Save symbol for function/procedure */

      BCADD(BC_BLKSTART); /* Stackponter erhoehen etc. */
      BCADD(BC_POP); /* Das waere jetzt die anzahl der Parameter als int auf stack*/
        /* Wir muessen darauf vertrauen, dass die anzahl Parameter stimmt */
        /* Jetzt von Liste von hinten aufloesen */
      e=procs[pcode[lnr].integer].anzpar;
      ix=procs[pcode[lnr].integer].parameterliste;
      while(--e>=0) {
        bc_local(cb,ix[e]&(~V_BY_REFERENCE));  /* Rette alten varinhalt */
        typsp=1;
	/* Hier ist es nun ein Problem, da wir nicht wissen, welche
	Parametertype tatsaechlich auf dem Stack liegen....  TODO....*/
        bc_zuweis_nopretyp(cb,ix[e]&(~V_BY_REFERENCE)); /* Weise vom stack zu */
      }
      typsp=0;
    }
    /* not done ???*/
    case P_NOTHING: /*  empty line */
    case P_LABEL:   /*  Label */
    case P_DEFFN:   /*  Function macro */
      return(1);    /*  ... will not produce any code.*/
    case P_ZUWEIS: {
      int vnr=pcode[lnr].integer;
      int ii=pcode[lnr].panzahl;
      // int dim=variablen[vnr].pointer.a->dimension;
      int typ=variablen[vnr].typ;
      bc_parser(cb,pcode[lnr].argument);  /* Das Argument liegt nun aufm Stack*/

      if((typ&TYPMASK)==INTTYP) {
        if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
      } else if((typ&TYPMASK)==ARBINTTYP) {
        if(TL!=PL_ARBINT)	 {BCADD(BC_X2AI);TR(PL_ARBINT);}
      } else if((typ&TYPMASK)==FLOATTYP) {
        if(TL==PL_INT)        {BCADD(BC_I2F);TR(PL_FLOAT);}
        else if(TL!=PL_FLOAT) {BCADD(BC_X2F);TR(PL_FLOAT);}
      } else if((typ&TYPMASK)==COMPLEXTYP) {
        if(TL!=PL_COMPLEX) {BCADD(BC_X2C);TR(PL_COMPLEX);}
      } else if((typ&TYPMASK)==STRINGTYP) {
        if(TL!=PL_STRING) {
	  printf("ERROR: cannot convert <%s> (Typ=%x) to string.\n",pcode[lnr].argument,TL);
	  return(-1);
        }
      } 
      /* ARRAY braucht (hier) nicht konvertiert zu werden*/
	
      if(ii) { /* Bei Array Element Zuweisung oder SUBARRAY Zuweisung*/
        short ss=vnr;
        short f=ii;
        push_indexliste(cb,pcode[lnr].ppointer,ii);
      //  if((typ&ARRAYTYP) && ii!=dim) xberror(18,"");  /* Falsche Anzahl Indizies */
        BCADD(BC_ZUWEISINDEX);
        CP2(&cb->textseg[cb->textseglen],&ss,cb->textseglen);
        CP2(&cb->textseg[cb->textseglen],&f,cb->textseglen);
        TA(f);
        TO();
      } else {
        if(typ&ARRAYTYP) {
          if(TL!=PL_ARRAY) {
	    printf("ERROR: cannot convert to array. $%x\n",TL);
	    return(-1);
          }
        }
        bc_zuweis(cb,vnr);
      }  
    }
    return(1); /* P_ZUWEIS done.*/
  }  /*  switch */
  
  /* Extra treatment for PRINT ... */

  if((pcode[lnr].opcode&PM_COMMS)==find_comm("PRINT") || (pcode[lnr].opcode&PM_COMMS)==find_comm("?")) {
    if(compile_print(cb,pcode[lnr])<0) return(-1);
    return(1); /* done. */
  }
  
  /*  Extra tratment for INC and DEC:
   * We have deactivated this for now, because one also need to inrement and decrement
   * array indexed content....
   */

#if 0
    if((pcode[lnr].opcode&PM_COMMS)==find_comm("INC")) {
      int vnr;
      if(verbose>1) printf(" INC ");
      vnr=(pcode[lnr].ppointer)->integer;
      bc_pushv(vnr);  /* Variable */
      BCADD(BC_PUSH1);
      
        /* Hier herausfinden, was fuer eine Variable das ist und dann versch.adds benutzen.*/
      if(TL==PL_INT) {
        BCADD(BC_ADDi);
      } else if(TL==PL_FLOAT) {
        BCADD(BC_I2F);
        BCADD(BC_ADDf);
      } else {
        BCADD(BC_ADD);
      }
      bc_zuweis(vnr); /* Zuweisung */
      return(1); /* done */
    } 
    if((pcode[lnr].opcode&PM_COMMS)==find_comm("DEC")) {
      int vnr;
      if(verbose>1) printf(" DEC ");
      vnr=(pcode[lnr].ppointer)->integer;
      bc_pushv(vnr);  /* Variable */
      BCADD(BC_PUSH1);
      if(TL==PL_INT) {
        BCADD(BC_SUBi);
      } else if(TL==PL_FLOAT) {
        BCADD(BC_I2F);
        BCADD(BC_SUBf);
      } else {
        BCADD(BC_SUB);
      }
      bc_zuweis(vnr); /* Zuweisung */
      return(1); /* done */
    } 
#endif

/*  Einzelne Sonderbehandlungen nun hier:*/

  if((pcode[lnr].opcode&PM_COMMS)==find_comm("LOCAL")) {
    int e=pcode[lnr].panzahl;
    while(--e>=0) bc_local(cb,(pcode[lnr].ppointer)[e].integer);
    return(1); /* done */
  }
  if((pcode[lnr].opcode&PM_COMMS)==find_comm("RESTORE")) {
    int j;
    if(verbose>1) printf(" RESTORE ");
    if(strlen(pcode[lnr].argument)) {
      j=labelnr(pcode[lnr].argument);
      if(j==-1) {
        xberror(20,pcode[lnr].argument);/* Label not found */
	return(-1);
      } 
      bc_restore(cb,(int)labels[j].datapointer);
      if(verbose>1) printf(" %d %s\n",j,labels[j].name);
      add_symbol(cb,(int) labels[j].datapointer,labels[j].name,STT_DATAPTR,0);
    } else {
      bc_restore(cb,0);
      if(verbose>1) printf(" \n");
    }
    return(1); /* done */
  }

  
  
  return(0); /* not done, because we might need extra treatment...*/
}


/* Compiles the currently loaded program.
 * the compile() function will generate follwing structures:
 *
 *  rodata   --- realloc()
 *  symtab   --- realloc()
 *  strings.pointer -- realloc   (nicht von bytecode_load verwendet)
*/

void compile(COMPILE_BLOCK *cb,int compile_flags) {
  cb->comp_flags=compile_flags;
  init_compile(cb);
  
  if(cb->comp_flags&COMPILE_VERBOSE) {
    printf("%d\tlines.\n",prglen);
    printf("%d\tprocedures.\n",anzprocs);
    printf("%d\tlabels.\n",anzlabels);
    printf("%d\tvariables.\n",anzvariablen);
    if(verbose>1) printf("PASS A:\n");
    printf("Compile flags=%x\n",cb->comp_flags);
  }
  add_string(cb,"compiled by xbbc " VERSION);
  
  int a,b;
  int expected_typsp=0;
  
  for(int i=0;i<prglen;i++) {
    if(typsp!=expected_typsp) printf("COMPILER WARNING: typsp=%d at line %d.\n",typsp,i);
    if(compile_line(cb,i)) continue;
       
    /* Jetzt behandlungen nach Pcode PM_SPECIAL */
    
    switch(pcode[i].opcode&PM_SPECIAL) {
    case P_EXITIF: 
      if(verbose>1) printf(" EXIT IF ");
      bc_parser(cb,pcode[i].argument);
      if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
      BCADD(BC_NOTi);
      bc_jumpto(cb,i,pcode[i].integer,1);
    case P_DATA: continue;  /*Erzeugt keinen Code...*/
    case P_REM:
      bc_comment(cb,pcode[i].argument);
      continue;
    case P_SELECT: {
      bc_parser(cb,pcode[i].argument);
      if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
      if(verbose>1) printf(" SELECT ");
      /*Vergleichswert liegt jetzt auf dem Stack.*/ 
      expected_typsp++;
      int npc=pcode[i].integer;
      char *w1=NULL,*w2,*w3;
      int l2,l=0,e;
      if(npc==-1) xberror(36,"SELECT"); /*Programmstruktur fehlerhaft */
      /* Case-Anweisungen finden */
      while(1) {
       // printf("branch to line %d. <%s>\n",npc-1,program[npc-1]);
        if((pcode[npc-1].opcode&PM_SPECIAL)==P_CASE) {
          l2=strlen(program[npc-1])+1;
          if(l2>l || w1==NULL) {
            l=l2+256;
           w1=realloc(w1,l);
          }
	  strcpy(w1,pcode[npc-1].argument);
       
          e=wort_sep_destroy(w1,',',TRUE,&w2,&w3);
          while(e) {
	    BCADD(BC_DUP);TP(TL);   /* Vergleichswert auf Stack duplizieren fuer den folgenden Test */
            bc_parser(cb,w2);
	    if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
	    BCADD(BC_EQUAL);TO();TR(PL_INT);BCADD(BC_NOTi);
	    bc_jumpto(cb,i,npc,1);
	    e=wort_sep_destroy(w3,',',TRUE,&w2,&w3);
          }
          npc=pcode[npc-1].integer;
	  if(npc==-1) xberror(36,"SELECT/CASE"); /*Programmstruktur fehlerhaft */
        } else if((pcode[npc-1].opcode&PM_SPECIAL)==P_DEFAULT) {          
          bc_jumpto(cb,i,npc,0);
          npc=pcode[npc-1].integer;
	  if(npc==-1) xberror(36,"SELECT/DEFAULT"); /*Programmstruktur fehlerhaft */  
        } else break;
      } 
     
     /*Wenn er vorher weggesprungen ist, dann verbleibt ein Wert auf dem Stack !!*/
     /*TODO */
    } continue;
    case P_ENDSELECT: 
       BCADD(BC_POP);TO(); /* Stack korrigieren und weitermachen */
       expected_typsp--;
       continue;
    case P_CASE: {
      if(verbose>1) printf(" CASE ");
      int j,f=0,o=0;
      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_ENDSELECT)  && f==0) break;
        else if(o==P_SELECT) f++;
        else if(o==P_ENDSELECT) f--;
      }
      if(j==prglen) xberror(36,"SELECT/CASE"); /*Programmstruktur fehlerhaft */
      if(o==P_ENDSELECT) bc_jumpto(cb,i,j,0); 
    } continue;
    case P_DEFAULT: { 
      if(verbose>1) printf(" DEFAULT ");
        int j,f=0,o=0;
      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_ENDSELECT)  && f==0) break;
        else if(o==P_SELECT) f++;
        else if(o==P_ENDSELECT) f--;
      }
      if(j==prglen) xberror(36,"SELECT/CASE"); /*Programmstruktur fehlerhaft */
      if(o==P_ENDSELECT) bc_jumpto(cb,i,j,0); 
    } continue; 
    case P_IF: {
      bc_parser(cb,pcode[i].argument);               /* Ausdruck auswerten */
      if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}  /* Mache integer draus (wahrheitswert)*/
      if(verbose>1) printf(" IF ");
      // printf("Werte aus: Zeile %d: IF (ENDIF vor Zeile %d)\n",i,pcode[i].integer);
     /*  a=pcode[i].integer; zeigt auf ENDIF */
      int j,f=0,o=0;
      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_ENDIF || o==P_ELSE|| o==P_ELSEIF)  && f==0) break;
        else if(o==P_IF) f++;
        else if(o==P_ENDIF) f--;
      }
      if(j==prglen) xberror(36,"IF"); /*Programmstruktur fehlerhaft */
#if 0
      if(o==P_ENDIF) printf("ENDIF in Zeile %d gefunden.\n",j);
      else if(o==P_ELSE) printf("ELSE in Zeile %d gefunden.\n",j);
      else if(o==P_ELSEIF) printf("ELSEIF in Zeile %d gefunden.\n",j);
#endif
      if(o==P_ENDIF) bc_jumpto(cb,i,j,1);       /* auch wenn ENDIF ignoriert wird, dürfen wir nicht einfach in die Zeile 
                                                dahinter springen, da es eine ELSE IF Zeile sein könnte*/
      else if(o==P_ELSE) bc_jumpto(cb,i,j+1,1); /* In den Block rein springen */
      else if(o==P_ELSEIF) bc_jumpto(cb,i,j,1); /* Die elseif muss ausgewertet werden*/
      else {BCADD(BC_POP);TO();} /* Stack korrigieren und weitermachen */
    } continue; 
    case P_ELSEIF: {
      /* Bei ELSE IF macht es einen Unterschied, ob man von oben drauflaeuft oder es über die 
       * bc_index Tabelle anspringt. In ersterem Fall wird zum ENDIF gegangen in letzterm muss ja
       * der Ausdruck ausgewertet werden. 
       */
      bc_jumpto(cb,i,pcode[i].integer,0); /* von oben draufgelaufen, gehe zum endif */
      /* Korrigiere nun bc_index */
      cb->bc_index[i]=cb->textseglen;   /* Wenn wir vom Sprung kommen, landen wir hier. Achtung! */
      if(verbose>1) printf(" ELSE IF (corr=$%x) ",cb->textseglen);
      int j,f=0,o=0;

      /* Bedingung auswerten */
      bc_parser(cb,pcode[i].argument); if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}

      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_ENDIF || o==P_ELSE|| o==P_ELSEIF)  && f==0) break;
        else if(o==P_IF) f++;
        else if(o==P_ENDIF) f--;
      }
      if(j==prglen) xberror(36,"ELSE IF"); /*Programmstruktur fehlerhaft */

#if 0
      if(o==P_ENDIF) printf("ENDIF in Zeile %d gefunden.\n",j);
      else if(o==P_ELSE) printf("ELSE in Zeile %d gefunden.\n",j);
      else if(o==P_ELSEIF) printf("ELSEIF in Zeile %d gefunden.\n",j);
#endif

      if(o==P_ENDIF) bc_jumpto(cb,i,j,1);       /* auch wenn ENDIF ignoriert wird, dürfen wir nicht einfach in die Zeile 
                                                dahinter springen, da es eine ELSE IF Zeile sein könnte*/
      else if(o==P_ELSE) bc_jumpto(cb,i,j+1,1); /* In den Block rein springen */
      else if(o==P_ELSEIF) bc_jumpto(cb,i,j,1); /* Die elseif muss ausgewertet werden*/
      else {BCADD(BC_POP);TO();} /* Stack korrigieren und weitermachen */
    } continue;
    case P_GOSUB:
      if(pcode[i].integer==-1) bc_kommando(cb,i);
      else {
        char buf[strlen(pcode[i].argument)+1];
	char *pos,*pos2;
	int anzpar=0;
        if(verbose>1) printf(" GOSUB %d:%d ",pcode[i].integer,procs[pcode[i].integer].zeile);
 //       anzpar=pcode[i].panzahl;
 //       if(anzpar) {
          strcpy(buf,pcode[i].argument);
          pos=searchchr(buf,'(');
          if(pos!=NULL) {
            *pos=0;pos++;
            pos2=pos+strlen(pos)-1;
            if(*pos2!=')') {
	      puts("GOSUB: Syntax error @ parameter list");
	      structure_warning(original_line(i),"GOSUB"); /*Programmstruktur fehlerhaft */
            } else {
	      *pos2=0;
	      anzpar=count_parameters(pos);
	      
	      if(anzpar!=procs[pcode[i].integer].anzpar) {
                xberror(56,pcode[i].argument); /* Falsche Anzahl Parameter */
              }
              if(anzpar) if(push_typeliste(cb,pos,procs[pcode[i].integer].parameterliste,procs[pcode[i].integer].anzpar))
	                    printf("WARNING: something is wrong at line %d!\n",compile_zeile);  /* Parameter auf den Stack */
	    }
          } else pos=buf+strlen(buf);
 
   //     }
	bc_push_integer(cb,anzpar);
	bc_jumptosr(cb,i,procs[pcode[i].integer].zeile);
	TO();
	TA(anzpar);
      }
      continue; 
    case P_RETURN:
      if(verbose>1) printf(" RETURN %s ",pcode[i].argument);
      if(pcode[i].argument && strlen(pcode[i].argument)) {
        bc_parser(cb,pcode[i].argument);
	
	/*Jetzt noch den Returntype chekcen:*/
	
	switch(procs[last_proc_number].rettyp){
	  case INTTYP:
	    if(TL!=PL_INT) BCADD(BC_X2I);
            break;	
	  case FLOATTYP:
	    if(TL==PL_INT) BCADD(BC_I2F);
	    else if(TL!=PL_FLOAT) BCADD(BC_X2F);
            break;	
	  case COMPLEXTYP:
	    if(TL!=PL_COMPLEX) BCADD(BC_X2C);
            break;	
	  case ARBINTTYP:
	    if(TL!=PL_ARBINT) BCADD(BC_X2AI);
            break;	
	  case STRINGTYP:
	    if(TL!=PL_STRING) printf("compile error: Returntype mismatch!\n");
            break;	
	}
	TO();  /*Jetzt sollten wir wieder bei  typsp=0; auskommen....*/
      }      
      BCADD(BC_BLKEND);
      BCADD(BC_RTS);
      continue;
    case P_WHILE:
      bc_parser(cb,pcode[i].argument);
      if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
      if(verbose>1) printf(" WHILE ");
      bc_jumpto(cb,i,pcode[i].integer,1);
      continue;
    case P_UNTIL:
      bc_parser(cb,pcode[i].argument);
      if(TL!=PL_INT) {BCADD(BC_X2I);TR(PL_INT);}
      if(verbose>1) printf(" UNTIL ");
      bc_jumpto(cb,i,pcode[i].integer,1);
      continue;
    case P_FOR: {
      char w1[strlen(pcode[i].argument)+1],w2[strlen(pcode[i].argument)+1];
      int e=wort_sep2(pcode[i].argument," TO ",TRUE,w1,w2);
      if(e<2) e=wort_sep2(pcode[i].argument," DOWNTO ",TRUE,w1,w2);
      if(e<2) printf("Syntax Error in line %d ! FOR %s\n",i,pcode[i].argument); 
      wort_sep(w1,'=',TRUE,w1,w2);
      if(verbose>1) printf(" FOR ");
      bc_zuweisung(cb,w1,w2);
      } continue;
    case P_NEXT: {
      int pp=pcode[i].integer;  /* Zeile it dem zugehoerigen For */
      if(pp==-1) xberror(36,"NEXT"); /*Programmstruktur fehlerhaft */
      else {
//	printf("Next: FOR ist in Zeile=%d\n",pp);
        char *w1=strdup(pcode[pp].argument);
//	printf("Argument=<%s>\n",w1);
        char *w2=malloc(strlen(w1)+1); 
        char *w3=malloc(strlen(w1)+1); 
        char *var=malloc(strlen(w1)+1); 
        char *step=malloc(strlen(w1)+1); 
        char *limit=malloc(strlen(w1)+1); 
	int ss=0;
        int e=wort_sep2(w1," TO ",TRUE,w2,w3);
	if(e>=2) ss=0;
	else {
	  e=wort_sep2(w1," DOWNTO ",TRUE,w2,w3);
	  if(e>=2) ss=1;
          else printf("Syntax Error ! FOR/NEXT\n");
	}
        /* Variable bestimmem */
        if(searchchr2(w2,'=')!=NULL) {
          wort_sep(w2,'=',TRUE,var,w1);
        } else printf("Syntax Error in line %d ! FOR/NEXT %s\n",i,w2); 
	// printf("Variable ist: %s Zuweisungsausdruck <%s>\n",var,w1);
	
	int st=0; 

        /* Limit bestimmem  */
        e=wort_sep2(w3," STEP ",TRUE,limit,step);
        if(e==0) printf("Syntax Error ! FOR %s\n",w3);
        else {
          if(e==1) {
	   // printf("limit=<%s> step=NONE\n",limit); /* Kein Step */
          } else {
           // printf("limit=<%s> step=<%s>\n",limit,step);
            st=1;
          }
        }
		
        /* Wenn eine STEP anweisung angegeben wurde, muss ueberprueft werden, ob sie
	  negativ ist. Dann muss aus DOWNTO To werden und umgekehrt. Das macht es etwas
	  umstaendlicher und langsamer und produziert mehr code.*/
	if(st) {  /* Wenn Step-Value explizit angegeben wurde */
	  bc_parser(cb,step);
	  if(ss) BCADD(BC_NEG); /* Bei downto negativ */
	  
          BCADD(BC_DUP);TP(TL);   /* Step-value duplizieren  */
          BCADD(BC_PUSH0);TP(PL_INT);
	  BCADD(BC_LESS);TO();TR(PL_INT);
	  BCADD(BC_PUSH2);TP(PL_INT);
	  BCADD(BC_MULi);TO();
	  BCADD(BC_PUSH1);TP(PL_INT);
	  BCADD(BC_ADDi);TO(); 
	  BCADD(BC_EXCH); TEXCH();
	  /* jetzt liegen auf stack:
	     groesserkleinerflag, step value */
          bc_pushv_name(cb,var);/* Variable */
	  bc_add(cb);
	  BCADD(BC_DUP);TP(TL);   /* Ergebnis duplizieren fuer den folgenden Test */
          bc_zuweis_name(cb,var); /* Zuweisung */
  	  /* jetzt liegen auf stack:
	     groesserkleinerflag, variable+step-value */
          bc_parser(cb,limit);/* Jetzt auf Schleifenende testen */
	  bc_sub(cb); 
	  bc_mul(cb);
	  BCADD(BC_PUSH0);TP(PL_INT);
	  BCADD(BC_GREATER);TO();TR(PL_INT);
	} else {  /* Kein STEP wurde angegeben */
          bc_pushv_name(cb,var);/* Variable */
	  if(ss) BCADD(BC_PUSHM1); /* Bei downto negativ (-1) */
	  else BCADD(BC_PUSH1);
	  TP(PL_INT); 
	  bc_add(cb);
          BCADD(BC_DUP);TP(TL);   /* Ergebnis duplizieren fuer den folgenden Test */
          bc_zuweis_name(cb,var); /* Zuweisung */
          bc_parser(cb,limit);/* Jetzt auf Schleifenende testen */
	  if(ss) BCADD(BC_LESS);
	  else BCADD(BC_GREATER);
  	  TO();TR(PL_INT);
	}	        
        free(w1);free(w2);free(w3);free(var);free(step);free(limit);
        bc_jumpto(cb,i,pp+1,1);
      }	
    } continue;
    case P_ENDPROC: 
      if(verbose>1) printf(" * ");
      BCADD(BC_BLKEND);
      BCADD(BC_RTS);
      typsp=0;
      continue;
    } /* switch */
    
    /* Jetzt behandlung nach Typ:*/

    switch(pcode[i].opcode&PM_TYP) {
    case P_IGNORE:
      if(verbose>1) printf(" * ");
      continue;
    case P_EVAL:
      if(verbose>1) printf(" EVAL ");
      bc_kommando(cb,i);
      continue;
    case P_SIMPLE:
      if(verbose>1) printf(" SIMPLE_COMM ");
      BCADD(BC_PUSHCOMM);BCADD(pcode[i].opcode&PM_COMMS);BCADD(0);
      continue;
    case P_ARGUMENT:
      if(verbose>1) printf(" ARGUMENT_COMM ");
      BCADDPUSHX(pcode[i].argument);
      BCADD(BC_PUSHCOMM);BCADD(pcode[i].opcode&PM_COMMS);BCADD(1);
      continue;
    case P_PLISTE: {
      int j=(pcode[i].opcode&PM_COMMS);
      if(verbose>1) printf(" PLISTE(%d) ",pcode[i].panzahl);
      if(verbose>1) printf("COMMS=%d(%d)\n",j,comms[j].pmax);
      plist_to_stack(cb,(PARAMETER *)pcode[i].ppointer,(short *)comms[j].pliste,pcode[i].panzahl,comms[j].pmin,comms[j].pmax);
      BCADD(BC_PUSHCOMM);BCADD(pcode[i].opcode&PM_COMMS);BCADD(pcode[i].panzahl);
      TA(pcode[i].panzahl);
      if(pcode[i].panzahl<comms[j].pmin)  xberror(42,""); /* Zu wenig Parameter  */
      if(comms[j].pmax!=-1 && (pcode[i].panzahl>comms[j].pmax)) xberror(45,""); /* Zu viele Parameter  */
    } continue;
    default: 
    
      printf("ERROR Compile: Unknown statement -> %lx\n",pcode[i].opcode&PM_SPECIAL);
    } /*  switch */
    printf("Compiler ERROR: unknown code %08x in line %d\n",(unsigned int)pcode[i].opcode,i);
    if((pcode[i].opcode&PM_COMMS)>=anzcomms) puts("Precompiler error...");
    bc_kommando(cb,i);
  } /*  for */
  
  
#ifdef ATARI
  printf(" labels....");
  fflush(stdout);
#endif
 
   
  /* Jetzt alle Labels in Symboltabelle eintragen:*/
  if(anzlabels) {
    for(int i=0;i<anzlabels;i++) {
      add_symbol(cb,cb->bc_index[labels[i].zeile]-sizeof(BYTECODE_HEADER),labels[i].name,STT_LABEL,0);
    }
  }

#ifdef ATARI
  printf(" variables ...");
  fflush(stdout);
#endif

  /* Jetzt alle Variablen in Symboltabelle eintragen:*/
  if(anzvariablen) {
    int typ;
    int adr=0;
    for(int i=0;i<anzvariablen;i++) {
      typ=variablen[i].typ;
      adr=add_bss(cb,typlaenge(typ));
      if(typ==ARRAYTYP) typ|=variablen[i].pointer.a->typ;
      add_symbol(cb,adr,variablen[i].name,STT_OBJECT,typ);
    }
  }
  if(verbose>1) printf("PASS B: %d relocations\n",cb->anzreloc);
  else if(verbose) printf("%d\trelocations.\n",cb->anzreloc);
  int dummy=0;
  for(int i=0;i<cb->anzreloc;i++) {
    b=cb->reltab[i];
    CP4(&a,&cb->textseg[cb->reltab[i]],dummy);
    if(verbose>1) printf("%d:$%08x ziel=%d ---> $%x\n",i,b,a,cb->bc_index[a]);

    /* Wenn a negativ, dann zeigt er in das data-Segment */

    if(a>=0) {
      a=cb->bc_index[a]+relocation_offset;
      add_symbol(cb,a-sizeof(BYTECODE_HEADER),NULL,STT_NOTYPE,0);
    } else {
      a=cb->textseglen-a;
      add_symbol(cb,a-sizeof(BYTECODE_HEADER),NULL,STT_DATAPTR,0);
    }
    CP4(&cb->textseg[cb->reltab[i]],&a,dummy);
  }
}












int save_bytecode(const char *name,COMPILE_BLOCK *cb, int dostrip) {
  if(verbose) {printf("--> %s [",name);fflush(stdout);}
  int fdis=open(name,O_CREAT|O_BINARY|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP);
  if(fdis==-1) {perror("open");return(-1);}
  BYTECODE_HEADER h;
  bzero(&h,sizeof(BYTECODE_HEADER));
  add_rodata(cb,ifilename,strlen(ifilename));
  int len=cb->textseglen;
  int rodatalen=cb->rodataseglen;
  int dlen=cb->dataseglen;
  /* align to even adresses: */
  len=(len+3)&0xfffffffc;
  rodatalen=(rodatalen+3)&0xfffffffc;
  dlen=(dlen+3)&0xfffffffc;
  
  h.BRAs=BC_BRAs;
  h.offs=sizeof(BYTECODE_HEADER)-2;
  h.textseglen=len;
  h.rodataseglen=rodatalen;
  h.sdataseglen=dlen;
  h.dataseglen=0;
  h.bssseglen=cb->bsseglen;
  if(dostrip) {h.symbolseglen=0;h.stringseglen=0;}
  else {
    h.symbolseglen=cb->anzsymbols*sizeof(BYTECODE_SYMBOL);
    h.stringseglen=cb->stringseglen;
    h.stringseglen=(h.stringseglen+3)&0xfffffffc;
  }
  h.version=cb->bc_version;
  h.relseglen=0;

  if(verbose) {
    printf("Info: BC_VERSION=%04x\n"
           "  Size of   Text-Segment: %d\n"
           "  Size of roData-Segment: %d\n"
           "  Size of   Data-Segment: %d\n",h.version,(int)h.textseglen,(int)h.rodataseglen,(int)h.sdataseglen);
    printf("  Size of    bss-Segment: %d\n"
           "  Size of String-Segment: %d\n",(int)h.bssseglen,(int)h.stringseglen);
    printf("  Size of Symbol-Segment: %d (%d symbols)\n",(int)h.symbolseglen,cb->anzsymbols);
  }
  #ifdef IS_BIG_ENDIAN
  WSWAP((char *)&h.version);
  LWSWAP((short *)&h.textseglen);
  LWSWAP((short *)&h.rodataseglen);
  LWSWAP((short *)&h.sdataseglen);
  LWSWAP((short *)&h.dataseglen);
  LWSWAP((short *)&h.bssseglen);
  LWSWAP((short *)&h.symbolseglen);
  LWSWAP((short *)&h.stringseglen);
  LWSWAP((short *)&h.relseglen);
  WSWAP((char *)&h.flags);
  #endif
  if(write(fdis,&h,sizeof(BYTECODE_HEADER))==-1) io_error(errno,"write");
  if(write(fdis,cb->textseg,len)==-1) io_error(errno,"write");
  if(rodatalen) {if(write(fdis,cb->rodataseg,rodatalen)==-1) io_error(errno,"write");}
  if(dlen) {if(write(fdis,cb->sdataseg,dlen)==-1) io_error(errno,"write");}
  /* zurückwandeln, damit wir damit rechnen können...*/
  #ifdef IS_BIG_ENDIAN
  LWSWAP((short *)&h.symbolseglen);
  LWSWAP((short *)&h.stringseglen);
  LWSWAP((short *)&h.relseglen);
  #endif
  
  if(write(fdis,cb->stringseg,h.stringseglen)==-1) io_error(errno,"write strings");
#ifdef IS_BIG_ENDIAN
  int n=h.symbolseglen/sizeof(BYTECODE_SYMBOL);
  if(n>0) {
    for(int i=0;i<n;i++) {
        LWSWAP((short *)&cb->symtab[i].name); // TODO: this destroys original symtab
        LWSWAP((short *)&cb->symtab[i].adr);
    }
  }
#endif
  if(write(fdis,cb->symtab,h.symbolseglen)==-1) io_error(errno,"write symtab");
  if(verbose) printf("] done.\n");
  return(close(fdis));
}


