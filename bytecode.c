/* BYTECODE.C   The X11-basic bytecode compiler         (c) Markus Hoffmann
*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "parameter.h"
#include "xbasic.h"
#include "parser.h"

#include "functions.h"
#include "wort_sep.h"
#include "bytecode.h"

#define BCADD(a) bcpc.pointer[bcpc.len++]=a

#define BCADDPUSHX(a) BCADD(BC_PUSHX);BCADD(strlen(a));\
                      {int adr=add_rodata(a,strlen(a));\
		      memcpy(&bcpc.pointer[bcpc.len],&adr,sizeof(int));}\
                      bcpc.len+=sizeof(int);

extern STRING bcpc;
extern STRING strings;
extern BYTECODE_SYMBOL *symtab;
extern int anzsymbols;

int *relocation;
int anzreloc=0;
const int relocation_offset=sizeof(BYTECODE_HEADER);
int *bc_index;
extern int verbose;

extern int donops;
extern int docomments;
extern char *rodata;
extern int rodatalen;
extern int bssdatalen;


/* X11-Basic needs these declar<ations:  */


extern void memdump(unsigned char *,int);

int find_comm(char *); 

#ifdef WINDOWS
#include <windows.h>
HINSTANCE hInstance;
#endif

static int typstack[128];
static int typsp=0;

#define TP(a) typstack[typsp++]=a
#define TR(a) typstack[typsp-1]=a
#define TO()  if(--typsp<0) printf("WARNING: typestack <zero!\n")
#define TA(a)  if((typsp-=a)<0) printf("WARNING: typestack zero or below!\n")
#define TL    typstack[typsp-1]
#define TL2    typstack[typsp-2]

static void bc_jumptosr2(int ziel);
static void plist_to_stack(PARAMETER *pin, short *pliste, int anz, int pmin, int pmax);

static void bc_push_float(double d) {
  BCADD(BC_PUSHF);
  TP(PL_FLOAT);
  memcpy(&bcpc.pointer[bcpc.len],&d,sizeof(double));
  bcpc.len+=sizeof(double);
}
static void bc_push_string(STRING str) {
  int i;
  int adr;
  BCADD(BC_PUSHS);
  TP(PL_STRING);
  i=str.len;
  adr=add_rodata(str.pointer,str.len);
  memcpy(&bcpc.pointer[bcpc.len],&i,sizeof(int));
  bcpc.len+=sizeof(int);
  memcpy(&bcpc.pointer[bcpc.len],&adr,sizeof(int));
  bcpc.len+=sizeof(int);
}
static void bc_push_integer(int i) {
  if(i==0) BCADD(BC_PUSH0);
  else if(i==1) BCADD(BC_PUSH1);
  else if(i==2) BCADD(BC_PUSH2);
  else if(i==-1) BCADD(BC_PUSHM1);
  else if(i<128 && i>0) {
    BCADD(BC_PUSHB);
    BCADD(i);
  } else if(i<0x7fff && i>-32768) {
    short ss=(short)i;
    BCADD(BC_PUSHW);
    memcpy(&bcpc.pointer[bcpc.len],&ss,sizeof(short));
    bcpc.len+=sizeof(short);
  } else {
    BCADD(BC_PUSHI);
    memcpy(&bcpc.pointer[bcpc.len],&i,sizeof(int));
    bcpc.len+=sizeof(int);
  }
  TP(PL_INT);
}

/* Push variable inhalt on stack */
static void bc_pushv(int vnr);

static void bc_pushv_name(char *var) {
  char w1[strlen(var)+1],w2[strlen(var)+1];
  int typ=type(var);
  char *r=varrumpf(var);
  int vnr;
  short f,ss;
  int e=klammer_sep(var,w1,w2);
  if((typ& ARRAYTYP)) {     /*  Ganzes Array */ 
    vnr=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP));
    bc_pushv(vnr);
  } else {
    if(e>1) {  /*   Array Element */
      vnr=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP));
      f=count_parameters(w2);   /* Anzahl indizes z"ahlen*/
      ss=vnr;
      e=wort_sep(w2,',',TRUE,w1,w2);
      while(e) {
        bc_parser(w1);  /*  jetzt Indizies Zeugs aufm stack */
	if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
        e=wort_sep(w2,',',TRUE,w1,w2);
      }
      
    /* Hier muesste man noch cheken, ob alle indizies integer sind.
       sonst ggf. umwandeln. Hierfuer brauchen wir ROLL */
      BCADD(BC_PUSHARRAYELEM);
      memcpy(&bcpc.pointer[bcpc.len],&ss,sizeof(short));
      bcpc.len+=sizeof(short);
      memcpy(&bcpc.pointer[bcpc.len],&f,sizeof(short));
      bcpc.len+=sizeof(short);
      TA(f);
      if(variablen[vnr].typ==INTTYP) TP(PL_INT);
      else if(variablen[vnr].typ==FLOATTYP)  TP(PL_FLOAT);
      else if(variablen[vnr].typ==STRINGTYP) TP(PL_STRING);
      else if(variablen[vnr].typ==ARRAYTYP) TP(variablen[vnr].pointer.a->typ);
      else TP(PL_LEER);
    } else {  /*  Normale Variable */
      vnr=add_variable(r,typ,0);
      bc_pushv(vnr);
    } 
  }
  free(r);
}
/* Push variable inhalt on stack */

static void bc_pushv(int vnr) {
  short ss=vnr;
  BCADD(BC_PUSHV);
  memcpy(&bcpc.pointer[bcpc.len],&ss,sizeof(short));
  bcpc.len+=sizeof(short);
  if(variablen[vnr].typ==INTTYP) TP(PL_INT);
  else if(variablen[vnr].typ==FLOATTYP) TP(PL_FLOAT);
  else if(variablen[vnr].typ==STRINGTYP) TP(PL_STRING);
  else if(variablen[vnr].typ==ARRAYTYP) TP(PL_ARRAY);
  else TP(PL_LEER);
}
static void bc_pushvv(int vnr) {
  short ss=vnr;
  BCADD(BC_PUSHVV);
  memcpy(&bcpc.pointer[bcpc.len],&ss,sizeof(short));
  bcpc.len+=sizeof(short);
  switch(variablen[vnr].typ) {
  case INTTYP:    TP(PL_IVAR);     break;
  case FLOATTYP:  TP(PL_FVAR);     break;
  case STRINGTYP: TP(PL_SVAR);     break;
  case ARRAYTYP:  
    switch(variablen[vnr].pointer.a->typ) {
    case INTTYP:    TP(PL_IARRAYVAR); break;
    case FLOATTYP:  TP(PL_FARRAYVAR); break;
    case STRINGTYP: TP(PL_SARRAYVAR); break;
    default: TP(PL_ARRAYVAR); 
    }
    break;
  default:
    printf("pushvv: Hier ist was zu verbessern.\n");
    TP(PL_LEER);
  }
}
static void bc_pushvvindex(int vnr,int anz) {
  short ss=vnr;
  BCADD(BC_PUSHVVI);
  memcpy(&bcpc.pointer[bcpc.len],&ss,sizeof(short));
  bcpc.len+=sizeof(short);
  ss=anz;
  memcpy(&bcpc.pointer[bcpc.len],&ss,sizeof(short));
  bcpc.len+=sizeof(short);
  TA(anz);
  if(variablen[vnr].typ==INTTYP) TP(PL_IVAR);
  else if(variablen[vnr].typ==FLOATTYP) TP(PL_FVAR);
  else if(variablen[vnr].typ==STRINGTYP) TP(PL_SVAR);
  else if(variablen[vnr].typ==ARRAYTYP) TP(PL_ARRAYVAR);
  else TP(PL_LEER);
}

/* Put content from stack in variable */

static void bc_zuweis(int vnr) {
  short ss=vnr;
  BCADD(BC_ZUWEIS);
  memcpy(&bcpc.pointer[bcpc.len],&ss,sizeof(short));
  bcpc.len+=sizeof(short);
  TO();
}

/* make a local copy of var */

static void bc_local(int vnr) {
  short ss=vnr;
  BCADD(BC_LOCAL);
  memcpy(&bcpc.pointer[bcpc.len],&ss,sizeof(short));
  bcpc.len+=sizeof(short);
}

/* Hier ist schon was auf dem Stack. */
static void bc_zuweis_name(char *var) {
  char w1[strlen(var)+1],w2[strlen(var)+1];
  int typ=vartype(var);
  char *r=varrumpf(var);
  int vnr;
  short f,ss;
  int e=klammer_sep(var,w1,w2);
  if(typ & ARRAYTYP) {     /*  Ganzes Array */ 
    vnr=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP));
    if(TL!=PL_ARRAY) printf("WARNING: no ARRAY for assignment!\n");
    bc_zuweis(vnr);
  } else {
    if(e>1) {  /*   Array Element */
      vnr=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP));
      f=count_parameters(w2);   /* Anzahl indizes z"ahlen*/
      ss=vnr;
      e=wort_sep(w2,',',TRUE,w1,w2);
      while(e) {
        bc_parser(w1);  /*  jetzt Indizies Zeugs aufm stack */
	if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
        e=wort_sep(w2,',',TRUE,w1,w2);
      }
      BCADD(BC_ZUWEISINDEX);
      memcpy(&bcpc.pointer[bcpc.len],&ss,sizeof(short));
      bcpc.len+=sizeof(short);
      memcpy(&bcpc.pointer[bcpc.len],&f,sizeof(short));
      bcpc.len+=sizeof(short);
      TA(f);
      TO();
    } else {  /*  Normale Variable */
      vnr=add_variable(r,typ,0);
      bc_zuweis(vnr);
    } 
  }
  free(r);
}
static void bc_zuweisung(char *var,char *arg) {
  bc_parser(arg);  /*  jetzt ist Zeugs aufm stack */
  bc_zuweis_name(var);
}

static void bc_zuweis_vnr(int vnr, int *indexliste, int dim) {
char *var;
  char w1[strlen(var)+1],w2[strlen(var)+1];
  int typ=vartype(var);
  char *r=varrumpf(var);

  short f,ss;
  int e=klammer_sep(var,w1,w2);
  if(typ & ARRAYTYP) {     /*  Ganzes Array */ 
    vnr=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP));
    if(TL!=PL_ARRAY) printf("WARNING: no ARRAY for assignment!\n");
    bc_zuweis(vnr);
  } else {
    if(e>1) {  /*   Array Element */
      vnr=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP));
      f=count_parameters(w2);   /* Anzahl indizes z"ahlen*/
      ss=vnr;
      e=wort_sep(w2,',',TRUE,w1,w2);
      while(e) {
        bc_parser(w1);  /*  jetzt Indizies Zeugs aufm stack */
	if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
        e=wort_sep(w2,',',TRUE,w1,w2);
      }
      BCADD(BC_ZUWEISINDEX);
      memcpy(&bcpc.pointer[bcpc.len],&ss,sizeof(short));
      bcpc.len+=sizeof(short);
      memcpy(&bcpc.pointer[bcpc.len],&f,sizeof(short));
      bcpc.len+=sizeof(short);
      TA(f);
      TO();
    } else {  /*  Normale Variable */
      vnr=add_variable(r,typ,0);
      bc_zuweis(vnr);
    } 
  }
  free(r);
}

/*Indexliste aus Parameterarray (mit EVAL) auf stack */

static int bc_indexliste(PARAMETER *p,int n) {
  int i;
  for(i=0;i<n;i++) {
    if(p[i].typ==PL_EVAL) {
      if(((char *)p[i].pointer)[0]==':') bc_push_integer(-1);
      else {
        bc_parser((char *)p[i].pointer);
	if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
      }
    } else bc_push_integer(0);
  }
  return(i);
}


int bc_parser(char *funktion){  /* Rekursiver Parser */
  char *pos,*pos2;
  char s[strlen(funktion)+1],w1[strlen(funktion)+1],w2[strlen(funktion)+1];
  int i,e,typ;

  static int bcerror;
 
//   printf("Bytecode Parser: <%s>\n",funktion);

  /* Erst der Komma-Operator */
  
  if(searchchr2(funktion,',')!=NULL){
    if(wort_sep(funktion,',',TRUE,w1,w2)>1) {
      bc_parser(w1);bc_parser(w2);return(bcerror);
    }
  }

   typ=type(funktion);
   if(typ&CONSTTYP) {
     /* Hier koennen wir stark vereinfachen, wenn wir schon wissen, dass der 
        Ausdruck konstant ist.
	*/
     //printf("#### Konstante: <%s> -->",funktion);
    if(typ&ARRAYTYP) printf("Array constant, simplification not yet possible.\n");
    else if(typ&INTTYP) {
      int si;
      if(typ&FILENRTYP) bc_push_integer(si=(int)parser(funktion+1));
      else  bc_push_integer(si=(int)parser(funktion));
      if(verbose) printf("<%d>.c ",si);
      return(bcerror);
    } else if (typ&FLOATTYP) {
      double d=parser(funktion);
      bc_push_float(d);
      if(verbose) printf("<%g>.cf ",d);
      return(bcerror);
    } else if (typ&STRINGTYP) {
      STRING str=string_parser(funktion);
      bc_push_string(str);
      if(verbose) printf("<\"%s\">.c ",str.pointer);
      free(str.pointer);
      return(bcerror);
    }
   }


  /* Logische Operatoren AND OR NOT ... */
  xtrim(funktion,TRUE,s);  /* Leerzeichen vorne und hinten entfernen, Grossbuchstaben */
  if(strlen(s)==0) {BCADD(BC_PUSHLEER);TP(PL_LEER);return(bcerror);}
  if(searchchr2_multi(funktion,"&|")!=NULL) {
    if(wort_sepr2(s,"&&",TRUE,w1,w2)>1) {bc_parser(w1);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_AND);TO();return(bcerror);}
    if(wort_sepr2(s,"||",TRUE,w1,w2)>1) {bc_parser(w1);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_OR);TO();return(bcerror);}    
  }

  if(searchchr2(funktion,' ')!=NULL) {
    if(wort_sepr2(s," AND ",TRUE,w1,w2)>1)  {bc_parser(w1);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_AND);TO();return(bcerror);}	/* von rechts !!  */
    if(wort_sepr2(s," OR ",TRUE,w1,w2)>1)   {bc_parser(w1);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_OR);TO(); return(bcerror);}
    if(wort_sepr2(s," NAND ",TRUE,w1,w2)>1) {bc_parser(w1);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_AND);TO();BCADD(BC_NOT);return(bcerror);}
    if(wort_sepr2(s," NOR ",TRUE,w1,w2)>1)  {bc_parser(w1);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_OR); TO();BCADD(BC_NOT);return(bcerror);}
    if(wort_sepr2(s," XOR ",TRUE,w1,w2)>1)  {bc_parser(w1);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_XOR);TO();return(bcerror);}
    if(wort_sepr2(s," EOR ",TRUE,w1,w2)>1)  {bc_parser(w1);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_XOR);TO();return(bcerror);}  
    if(wort_sepr2(s," EQV ",TRUE,w1,w2)>1)  {bc_parser(w1);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_XOR);TO();BCADD(BC_NOT);return(bcerror);}
    if(wort_sepr2(s," IMP ",TRUE,w1,w2)>1)  {bc_parser(w1);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_XOR);TO();BCADD(BC_NOT);bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_OR);TO();return(bcerror);}
    if(wort_sepr2(s," MOD ",TRUE,w1,w2)>1)  {bc_parser(w1);bc_parser(w2);BCADD(BC_MOD);if(TL==PL_FLOAT || TL2==PL_FLOAT) {TO();TR(PL_FLOAT);} else TO(); return(bcerror);}
    if(wort_sepr2(s," DIV ",TRUE,w1,w2)>1) {bc_parser(w1);if(TL!=PL_FLOAT) BCADD(BC_X2F);TR(PL_FLOAT);bc_parser(w2);if(TL!=PL_FLOAT) BCADD(BC_X2F);TR(PL_FLOAT);BCADD(BC_DIV);TO();return(bcerror);}
    if(wort_sepr2(s,"NOT ",TRUE,w1,w2)>1) {
      if(strlen(w1)==0) {bc_parser(w2);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);BCADD(BC_NOT);return(bcerror);}    /* von rechts !!  */
      /* Ansonsten ist NOT Teil eines Variablennamens */
    }
  }

  /* Erst Vergleichsoperatoren mit Wahrheitwert abfangen (lowlevel < Addition)  */
  if(searchchr2_multi(s,"<=>")!=NULL) {
    if(wort_sep2(s,"==",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);BCADD(BC_EQUAL);TO();TR(PL_INT);return(bcerror);}
    if(wort_sep2(s,"<>",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);BCADD(BC_EQUAL);TO();TR(PL_INT);BCADD(BC_NOT);return(bcerror);}
    if(wort_sep2(s,"><",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);BCADD(BC_EQUAL);TO();TR(PL_INT);BCADD(BC_NOT);return(bcerror);}
    if(wort_sep2(s,"<=",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);BCADD(BC_GREATER);TO();TR(PL_INT);BCADD(BC_NOT);return(bcerror);}
    if(wort_sep2(s,">=",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);BCADD(BC_LESS);TO();TR(PL_INT);BCADD(BC_NOT);return(bcerror);}
    if(wort_sep(s,'=',TRUE,w1,w2)>1)   {bc_parser(w1);bc_parser(w2);BCADD(BC_EQUAL);TO();TR(PL_INT);return(bcerror);}
    if(wort_sep(s,'<',TRUE,w1,w2)>1)   {bc_parser(w1);bc_parser(w2);BCADD(BC_LESS);TO();TR(PL_INT);return(bcerror);}
    if(wort_sep(s,'>',TRUE,w1,w2)>1)   {bc_parser(w1);bc_parser(w2);BCADD(BC_GREATER);TO();TR(PL_INT);return(bcerror);}
  }
  /* Addition/Subtraktion/Vorzeichen  */
  if(searchchr2_multi(s,"+-")!=NULL) {
    if(wort_sep_e(s,'+',TRUE,w1,w2)>1) {
      if(strlen(w1)) {
        bc_parser(w1);bc_parser(w2);
        if(typstack[typsp-2]==PL_STRING && typstack[typsp-1]==PL_STRING) {
          BCADD(BC_ADDs);
	  TO();
        } else if(typstack[typsp-2]==PL_INT && typstack[typsp-1]==PL_INT) {
          BCADD(BC_ADDi);
	  TO();
        } else if(typstack[typsp-2]==PL_FLOAT && typstack[typsp-1]==PL_FLOAT) {
          BCADD(BC_ADDf);
	  TO();
        } else if(typstack[typsp-2]==PL_FLOAT && typstack[typsp-1]==PL_INT) {
          BCADD(BC_X2F);
          BCADD(BC_ADDf);
	  TO();TR(PL_FLOAT);
        } else if(typstack[typsp-2]==PL_INT && typstack[typsp-1]==PL_FLOAT) {
	  BCADD(BC_EXCH);
          BCADD(BC_X2F);
          BCADD(BC_ADDf);
	  TO();TR(PL_FLOAT);
	} else {
	printf("\nADD of TYP: A=%d, b=%d  <%s>\n",typstack[typsp-2],typstack[typsp-1],funktion);
          BCADD(BC_ADD);
	  TO();
	  TR(PL_LEER);
	}
	return(bcerror);
      } else {bc_parser(w2);return(bcerror);}   /* war Vorzeichen + */
    }
    if(wort_sepr_e(s,'-',TRUE,w1,w2)>1) {       /* von rechts !!  */
      if(strlen(w1)) {
        bc_parser(w1);bc_parser(w2);
	if(typstack[typsp-2]==PL_INT && typstack[typsp-1]==PL_INT) {
          BCADD(BC_SUBi);
	  TO();
        } else if(typstack[typsp-2]==PL_FLOAT && typstack[typsp-1]==PL_FLOAT) {
          BCADD(BC_SUBf);
	  TO();
        } else if(typstack[typsp-2]==PL_FLOAT && typstack[typsp-1]==PL_INT) {
          BCADD(BC_X2F);
	  typstack[typsp-1]=PL_FLOAT;
          BCADD(BC_SUBf);
	  TO();
        } else if(typstack[typsp-2]==PL_INT && typstack[typsp-1]==PL_FLOAT) {
	  BCADD(BC_EXCH);
          BCADD(BC_X2F);
	  BCADD(BC_EXCH);
          BCADD(BC_SUBf);
	  typstack[typsp-2]=PL_FLOAT;
	  TO();
        } else {
	printf("\nSUB of TYP: A=%d, b=%d\n",typstack[typsp-2],typstack[typsp-1]);
  	  BCADD(BC_SUB);
	  TO();
	  TR(PL_LEER);
	}
	return(bcerror);
      } else {bc_parser(w2);BCADD(BC_NEG);return(bcerror);}   /* war Vorzeichen - */
    }
  }
  if(searchchr2_multi(s,"*/^")!=NULL) {
    if(wort_sepr(s,'*',TRUE,w1,w2)>1) {
      if(strlen(w1)) {
        bc_parser(w1);bc_parser(w2);
        if(typstack[typsp-2]==PL_INT && typstack[typsp-1]==PL_INT) {
          BCADD(BC_MULi);
	  TO();
        } else if(typstack[typsp-2]==PL_FLOAT && typstack[typsp-1]==PL_FLOAT) {
          BCADD(BC_MULf);
	  TO();
        } else if(typstack[typsp-2]==PL_FLOAT && typstack[typsp-1]==PL_INT) {
          BCADD(BC_X2F);
          BCADD(BC_MULf);
	  TO();TR(PL_FLOAT);
        } else if(typstack[typsp-2]==PL_INT && typstack[typsp-1]==PL_FLOAT) {
	  BCADD(BC_EXCH);
          BCADD(BC_X2F);
          BCADD(BC_MULf);
	  TO();TR(PL_FLOAT);
	} else {
	printf("\nMUL of TYP: A=%d, b=%d\n",typstack[typsp-2],typstack[typsp-1]);
  	  BCADD(BC_MUL);
	  TO();
	  TR(PL_LEER);
	}
	return(bcerror);
      } else {
        printf("Pointer not yet integrated! %s\n",w2);   /* war pointer - */
        return(bcerror);
      }
    }
    if(wort_sepr(s,'/',TRUE,w1,w2)>1) {
      if(strlen(w1)) {
        bc_parser(w1);if(TL!=PL_FLOAT) BCADD(BC_X2F);TR(PL_FLOAT);
        bc_parser(w2);if(TL!=PL_FLOAT) BCADD(BC_X2F);TR(PL_FLOAT);
        BCADD(BC_DIV);TO();
        return(bcerror);
      } else { xberror(51,w2); return(bcerror); }/* "Parser: Syntax error?! "  */
    }
    if(wort_sepr(s,'^',TRUE,w1,w2)>1) {
      if(strlen(w1)) {
        bc_parser(w1);if(TL!=PL_FLOAT) BCADD(BC_X2F);TR(PL_FLOAT);
	bc_parser(w2);
	if(TL==PL_INT && bcpc.pointer[bcpc.len-1]==BC_PUSH2) {
	//  printf("Quadrat gefunden !\n");
	  bcpc.len--;  /*Letztes PUSH2 rueckgaengig machen*/
	  TO();
	  BCADD(BC_DUP);
	  if(TL==PL_INT) BCADD(BC_MULi);
	  else BCADD(BC_MULf);
	} else {
  	  if(TL!=PL_FLOAT) BCADD(BC_X2F);TR(PL_FLOAT);
	  BCADD(BC_POW);TO();
	}
	return(bcerror);
      }    /* von rechts !!  */
      else { xberror(51,w2); return(bcerror); } /* "Parser: Syntax error?! "  */
    }
  }
  if(*s=='(' && s[strlen(s)-1]==')')  { /* Ueberfluessige Klammern entfernen */
    s[strlen(s)-1]=0;
    bc_parser(s+1);
    return(bcerror);
  } 
  /* SystemFunktionen Subroutinen und Arrays */
  
  
  pos=searchchr(s,'(');
  if(pos!=NULL) {
    pos2=s+strlen(s)-1;
    *pos++=0;
    if(*pos2!=')') {
      printf("Closing bracket is missing!\n");
      xberror(51,w2); /* "Parser: Syntax error?! "  */
      return(bcerror);
    }
    /* $-Funktionen und $-Felder   */
    *pos2=0;
    /* Benutzerdef. Funktionen mit parameter*/
    if(*s=='@') {
      int pc2,typ;
      int anzpar=0;

      /* Anzahl Parameter ermitteln */
      anzpar=count_parameters(pos);
      if(verbose) printf("function call <%s>(%s)\n",s+1,pos);

      /* Bei Benutzerdef Funktionen muessen wir den Parametertyp erraten.*/

      if(anzpar) bc_parser(pos);  /* Parameter auf den Stack */

      bc_push_integer(anzpar);
      /* Funktionsnr finden */
      pc2=procnr(s+1,2|4);  /*  FUNCTION + DEFFN */
      if(pc2==-1) { 
        xberror(44,s+1); /* Funktion  nicht definiert */
        return(bcerror);
      }
      if(verbose) printf("function procnr=%d, anzpar=%d\n",pc2,anzpar);
      typ=procs[pc2].typ;
      if(typ==4) {
        int e;
	int *ix;
	BCADD(BC_BLKSTART); /* Stackponter erhoehen etc. */
        BCADD(BC_POP); /* Das waere jetzt die Anzahl der Parameter als int auf Stack*/
        TO();
        e=procs[pc2].anzpar;
	ix=procs[pc2].parameterliste;
	while(--e>=0) {
          bc_local(ix[e]);  /* Rette alten varinhalt */
          typsp=1;
          bc_zuweis(ix[e]); /* Weise vom stack zu */
        }
        bc_parser(pcode[procs[pc2].zeile].argument);
	BCADD(BC_BLKEND);
      } else {
        bc_jumptosr2(procs[pc2].zeile);
        TO();
        TA(anzpar);   
        if(type(s+1)&STRINGTYP) TP(PL_STRING);
        else TP(PL_FLOAT);
      }
      /*Der Rueckgabewert sollte nach Rueckkehr auf dem Stack liegen.*/
      return(bcerror);
    }
    if((i=find_func(s))!=-1) {
      /* printf("Funktion %s gefunden. Nr. %d\n",pfuncs[i].name,i); */
      /* Jetzt erst die Parameter auf den Stack packen */
//printf("func %s: \n",pfuncs[i].name);      
      if((pfuncs[i].opcode&FM_TYP)==F_SIMPLE || pfuncs[i].pmax==0) {
        BCADD(BC_PUSHFUNC);BCADD(i);BCADD(0);
	if(pfuncs[i].opcode&F_IRET) TP(PL_INT);
	else TP(PL_FLOAT);                     /* Fuer den Rueckgabewert*/
        return(bcerror);
      } else if((pfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
        BCADDPUSHX(pos);
        BCADD(BC_PUSHFUNC);BCADD(i);BCADD(1);
	if(pfuncs[i].opcode&F_IRET) TP(PL_INT);
	else TP(PL_FLOAT);                     /* Fuer den Rueckgabewert*/
        return(bcerror);
      } else if((pfuncs[i].opcode&FM_TYP)==F_PLISTE || 
        (pfuncs[i].opcode&FM_TYP)==F_DQUICK ||
	 (pfuncs[i].opcode&FM_TYP)==F_IQUICK ||
	 (pfuncs[i].opcode&FM_TYP)==F_SQUICK ||
	 (pfuncs[i].opcode&FM_TYP)==F_AQUICK) {                

	int anz;
	anz=count_parameters(pos);
	if((pfuncs[i].pmin>anz && pfuncs[i].pmin!=-1) || 
	   (pfuncs[i].pmax<anz && pfuncs[i].pmax!=-1))  
	     printf("Warning: Function %d: wrong number of arguments (%d).\n",i,anz); /*Programmstruktur fehlerhaft */
	if(anz) {
          PARAMETER *par=(PARAMETER *)malloc(sizeof(PARAMETER)*anz);
  	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int ii=0;
          e=wort_sep(pos,',',TRUE,w1,w2);
		while(e) {
	          if(strlen(w1)) {
                    par[ii].typ=PL_EVAL;
		    par[ii].pointer=strdup(w1);
     		  } else  {
		    par[ii].typ=PL_LEER;
		    par[ii].pointer=NULL;
		  }
		  e=wort_sep(w2,',',TRUE,w1,w2);
                  ii++;
                }
          plist_to_stack(par,(short *)pfuncs[i].pliste,anz,pfuncs[i].pmin,pfuncs[i].pmax);
	  for(ii=0;ii<anz;ii++) free(par[ii].pointer);
          free(par);
	}
	BCADD(BC_PUSHFUNC);BCADD(i);BCADD(anz);
	TA(anz);
	if(pfuncs[i].opcode&F_IRET) TP(PL_INT);
	else TP(PL_FLOAT);                     /* Fuer den Rueckgabewert*/
	return(bcerror);
      } else {
        printf("Internal ERROR. Function not correktly defined. %s\n",s);
	return(bcerror);
      }
    } else if((i=find_afunc(s))!=-1) {
      if((pafuncs[i].opcode&FM_TYP)==F_SIMPLE || pafuncs[i].pmax==0) {
        BCADD(BC_PUSHFUNC);BCADD(i);BCADD(0);
	TP(PL_LEER); /* Fuer den Rueckgabewert*/
        return(bcerror);
      } else if((pafuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
        BCADDPUSHX(pos);
        BCADD(BC_PUSHAFUNC);BCADD(i);BCADD(1);
        TP(PL_LEER); /* Fuer den Rueckgabewert*/
        return(bcerror);
      } else if((pafuncs[i].opcode&FM_TYP)==F_PLISTE) {                
        bc_parser(pos);
	BCADD(BC_PUSHAFUNC);BCADD(i);BCADD(count_parameters(pos));
	TA(count_parameters(pos));
	TP(PL_LEER); /* Fuer den Rueckgabewert*/
	return(bcerror);
      } else if(pafuncs[i].pmax==1 && (pafuncs[i].opcode&FM_TYP)==F_AQUICK) {
        bc_parser(pos);
 	BCADD(BC_PUSHAFUNC);BCADD(i);BCADD(1);
	TO();
        TP(PL_LEER); /* Fuer den Rueckgabewert*/
	return(bcerror);
      } else if(pafuncs[i].pmax==1 && (pafuncs[i].opcode&FM_TYP)==F_SQUICK) {
        bc_parser(pos);
 	BCADD(BC_PUSHAFUNC);BCADD(i);BCADD(1);
	TO();
        TP(PL_LEER); /* Fuer den Rueckgabewert*/
	return(bcerror);
      }
    } else if((i=find_sfunc(s))!=-1) {
      if((psfuncs[i].opcode&FM_TYP)==F_SIMPLE || psfuncs[i].pmax==0) {
        BCADD(BC_PUSHSFUNC);BCADD(i);BCADD(0);
        TP(PL_STRING); /* Fuer den Rueckgabewert*/
        return(bcerror);
      } else if((psfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
        BCADDPUSHX(pos);
        BCADD(BC_PUSHSFUNC);BCADD(i);BCADD(1);
        TP(PL_STRING); /* Fuer den Rueckgabewert*/
        return(bcerror);
      } else if((psfuncs[i].opcode&FM_TYP)==F_PLISTE) {                
        bc_parser(pos);
	BCADD(BC_PUSHSFUNC);BCADD(i);BCADD(count_parameters(pos));
	TA(count_parameters(pos));
        TP(PL_STRING); /* Fuer den Rueckgabewert*/
	return(bcerror);
      } else if(psfuncs[i].pmax==2 && (psfuncs[i].opcode&FM_TYP)==F_DQUICK) {
        bc_parser(pos);
	BCADD(BC_PUSHSFUNC);BCADD(i);BCADD(2);
	TA(2);
        TP(PL_STRING); /* Fuer den Rueckgabewert*/
        return(bcerror);
      } else if(psfuncs[i].pmax==1 && ((psfuncs[i].opcode&FM_TYP)==F_SQUICK ||
                                       (psfuncs[i].opcode&FM_TYP)==F_AQUICK ||
                                       (psfuncs[i].opcode&FM_TYP)==F_IQUICK ||
                                       (psfuncs[i].opcode&FM_TYP)==F_DQUICK
				      )) {
        bc_parser(pos);
	BCADD(BC_PUSHSFUNC);BCADD(i);BCADD(1);
	TO();
        TP(PL_STRING); /* Fuer den Rueckgabewert*/
        return(bcerror);
      }
    } else {
       char buf[strlen(s)+1+strlen(pos)+1+1];
       sprintf(buf,"%s(%s)",s,pos);
       bc_pushv_name(buf);
       return(bcerror);
    }
  } else {  /* Also keine Klammern */
    /* Dann Systemvariablen und einfache Variablen */
    /* Liste durchgehen */
    char c=*s;
    int i=0,a=anzsysvars-1,b,l=strlen(s);
    for(b=0; b<l; c=s[++b]) {
      while(c>(sysvars[i].name)[b] && i<a) i++;
      while(c<(sysvars[a].name)[b] && a>i) a--;
      if(i==a) break;
    }
  if(strcmp(s,sysvars[i].name)==0) {    
      /*  printf("Sysvar %s gefunden. Nr. %d\n",sysvars[i].name,i);*/
      BCADD(BC_PUSHSYS);
      BCADD(i);
      if(sysvars[i].opcode&INTTYP) TP(PL_INT);
      else TP(PL_FLOAT);
    return(bcerror);
  } 
  /* Stringsysvars */
  /* Liste durchgehen */
  i=0;
  c=*s;
  a=anzsyssvars;
  l=strlen(s);
  for(b=0; b<l; c=s[++b]) {
    while(c>(syssvars[i].name)[b] && i<a) i++;
    while(c<(syssvars[a].name)[b] && a>i) a--;
    if(i==a) break;
  }
  if(strcmp(s,syssvars[i].name)==0) {
	    /*  printf("Sysvar %s gefunden. Nr. %d\n",syssvars[i].name,i);*/
    BCADD(BC_PUSHSSYS);
    BCADD(i);
    TP(PL_STRING);
    return(bcerror);
  }

  /* Arraysysvars  */
   
  /* erst integer abfangen (xxx% oder xxx&), dann rest */
  if(*s=='@')   {
    int pc2;
    /* Funktionsnr finden */
     if(verbose) printf("function call <%s>\n",s+1);
     pc2=procnr(s+1,2);
    if(pc2==-1) { 
      xberror(44,s+1); /* Funktion  nicht definiert */
      return(bcerror);
    }
      if(verbose) printf("function procnr=%d\n",pc2);
      bc_push_integer(0); /*Keine Parameter*/
       bc_jumptosr2(procs[pc2].zeile);
       TO();
       TP(PL_LEER);
      /*Der Rueckgabewert sollte nach Rueckkehr auf dem Stack liegen.*/
      return(bcerror);
    }
    if(s[0]=='#') { /* Filenummer ?*/
      int i=0;
      if(verbose) printf("file number: %s\n",s);
      while(s[++i]) s[i-1]=s[i];
      s[i-1]=s[i];
    }
    /* Testen ob und was fuer eine Konstante das ist. */
    e=type(s);
    if(e&CONSTTYP) {
      if (e&INTTYP) {
        bc_push_integer((int)parser(s));
        return(bcerror);
      } else if (e&FLOATTYP) {  
        bc_push_float(parser(s));
        return(bcerror);
      } else if (e&STRINGTYP) {
        STRING str=string_parser(s);
	bc_push_string(str);
        free(str.pointer);
        return(bcerror);
      }  
    } else {           /* Keine Konstante, also Variable */
      bc_pushv_name(s);
      return(bcerror);
    }
  }
  xberror(51,s); /* Syntax error */
  bcerror=51;
  return(bcerror);
}


static int add_string(char *n) {
  int ol=strings.len;
  if(n) {
    int i=0;
    /* Suche, ob schon vorhanden */
    while(i<ol) {
      if(strncmp(&((strings.pointer)[i]),n,strlen(n)+1)==0) return(i);
      i+=strlen(&((strings.pointer)[i]))+1;
    }  
    strings.len+=strlen(n)+1;
    strings.pointer=realloc(strings.pointer,strings.len);
    strncpy(&((strings.pointer)[ol]),n,strlen(n)+1);
    return(ol);
  } else return(0);
}

static void add_symbol(int adr,char *name, unsigned char typ,unsigned char subtyp) {
  symtab=realloc(symtab,(anzsymbols+1)*sizeof(BYTECODE_SYMBOL));

  symtab[anzsymbols].name=add_string(name);
  symtab[anzsymbols].typ=typ;
  symtab[anzsymbols].subtyp=subtyp;
  symtab[anzsymbols].adr=adr;
  anzsymbols++;
}


int add_rodata(char *data,int len) {
  if(len==0) return(0);
  if(rodatalen==0) {
    rodata=realloc(rodata,len);
    memcpy(rodata,data,len);
    rodatalen=len;
    return(0);
  } else {
    char *a=memmem(rodata, rodatalen,data,len);
    if(a==NULL) {
      rodata=realloc(rodata,rodatalen+len);
      memcpy(rodata+rodatalen,data,len);
      rodatalen+=len;
      return(rodatalen-len);
    } else {
      return((int)(a-rodata));
    }
  }
}


inline static int add_bss(int len) {
  bssdatalen+=len;
  return(bssdatalen-len);
}


static void bc_kommando(int idx) {
  char buffer[strlen(program[idx])+1];
  char w1[strlen(program[idx])+1];
  char w2[strlen(program[idx])+1],zeile[strlen(program[idx])+1];
  char *pos;
  int i,a,b;

  xtrim(program[idx],TRUE,zeile);
  wort_sep2(zeile," !",TRUE,zeile,buffer);
  if(docomments && strlen(buffer)) add_string(buffer);
  xtrim(zeile,TRUE,zeile);
  if(wort_sep(zeile,' ',TRUE,w1,w2)==0) {
    if(donops) {
      BCADD(BC_NOOP);
      if(verbose) printf(" bc_noop ");
    }
    return;
  }  /* Leerzeile */
  if(w1[0]=='\'') {
    if(docomments) {
      BCADD(BC_COMMENT);    
      BCADD(strlen(program[idx]));
      memcpy(&bcpc.pointer[bcpc.len],program[idx],strlen(program[idx]));
      bcpc.len+=strlen(program[idx]);
      if(verbose) printf(" bc_comment <%s> ",program[idx]);
      add_string(program[idx]);
    }
    return;
  }  /* Kommentar */
  if(w1[strlen(w1)-1]==':') {
    if(donops) {
      BCADD(BC_NOOP);
      if(verbose) printf(" bc_noop ");
      add_string(w1);
    }
    return;
  }  /* nixtun, label */
  if(w1[0]=='@') {
    /* Wenn Parameter, */
    /* Parameter auf den Stack*/
    /* Anzahl Parameter ermitteln */
    /* Zieladresse ermitteln */
    /* BC_BSR  */
    return;
  }
  if(w1[0]=='&') {
    bc_parser(w1+1);
    if(TL!=PL_STRING) printf("WARNING: EVAL <%s> wants a string!\n",w1+1);
    BCADD(BC_EVAL);
    TO();
    if(verbose) printf(" bc_eval <%s> ",w1+1);
    return;
  }
  if(w1[0]=='~') {
    bc_parser(zeile+1);
    /* einmal POP, um den Rueckgabewert zu vernichten */
    BCADD(BC_POP);
    TO();
    return;
  }
  if((pos=searchchr2(w1,'='))!=NULL) { /* Zuweisung */
    *pos++=0;
    bc_zuweisung(w1,pos);
    return;
  }
  if(isdigit(w1[0]) || w1[0]=='(') {
     bc_parser(w1);
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
    if(i==a) break;
  }
  if((i==a && strncmp(w1,comms[i].name,strlen(w1))==0) ||
     (i!=a && strcmp(w1,comms[i].name)==0) ) {
#ifdef DEBUG
      if(b<strlen(w1)) printf("Befehl %s vervollstaendigt --> %s\n",w1,comms[i].name);
#endif
      if(comms[i].opcode&P_IGNORE) {BCADD(BC_NOOP);return;}
      if((comms[i].opcode&PM_TYP)==P_ARGUMENT){
        BCADD(BC_PUSHX);BCADD(strlen(w2));
        memcpy(&bcpc.pointer[bcpc.len],w2,strlen(w2));
	bcpc.len+=strlen(w2);
        BCADD(BC_PUSHCOMM);BCADD(i);BCADD(1);
        return;
      } 
      if((comms[i].opcode&PM_TYP)==P_SIMPLE) {BCADD(BC_PUSHCOMM);BCADD(i);BCADD(0);return;}
      if((comms[i].opcode&PM_TYP)==P_PLISTE) {
        if(strlen(w2)) bc_parser(w2);
	/* Anzahl Prameter herausfinden */
        /* Warnen wenn zuwenig oder zuviel parameter */
        BCADD(BC_PUSHCOMM);BCADD(i);
	BCADD(count_parameters(w2));
	TA(count_parameters(w2));
        return;
      } else xberror(38,w1); /* Befehl im Direktmodus nicht moeglich */
    } else if(i!=a) {
       printf("Command uneindeutig ! <%s...%s>\n",comms[i].name,comms[a].name);
    }  else xberror(32,w1);  /* Syntax Error */
}

static void bc_restore(int offset) {
  if(verbose) printf("RESORE:offset=%d ",offset);
  bcpc.pointer[bcpc.len++]=BC_RESTORE;
  memcpy(&bcpc.pointer[bcpc.len],&offset,sizeof(int));
  bcpc.len+=sizeof(int);        
}

static void bc_jumpto(int from, int ziel, int eqflag) {
       /* Hier jetzt ein JUMP realisieren */
  if(TL!=PL_INT && eqflag) printf("WARNING: EQ: no int on stack !\n");
      if(ziel<=from) { /* Dann ist das Ziel schon bekannt */
        int a,b;
        a=bc_index[ziel];
	add_symbol(a,NULL,STT_NOTYPE,0);
	b=a-bcpc.len;
	if(verbose) printf("Delta=%d ",b);
        if(b<=127 && b>=-126) {
          if(verbose>1) printf(" %s.s ",(eqflag?"BEQ":"BRA"));
	  bcpc.pointer[bcpc.len++]=(eqflag?BC_BEQs:BC_BRAs);
	  bcpc.pointer[bcpc.len++]=b-2;
	} else if(b<=0x7fff && b>=-32765) {
	  short o=b-1-sizeof(short);
          if(verbose>1) printf(" %s ",(eqflag?"BEQ":"BRA"));
	  bcpc.pointer[bcpc.len++]=(eqflag?BC_BEQ:BC_BRA);
          memcpy(&bcpc.pointer[bcpc.len],&o,sizeof(short));
          bcpc.len+=sizeof(short);        
	} else {
          if(verbose>1) printf(" %s ",(eqflag?"JEQ":"JMP"));
          bcpc.pointer[bcpc.len++]=(eqflag?BC_JEQ:BC_JMP);
          memcpy(&bcpc.pointer[bcpc.len],&a,sizeof(int));
          bcpc.len+=sizeof(int);
	}
	if(verbose>1) printf("---> $%x ",a);
      } else {
        if(verbose>1) printf(" %s ",(eqflag?"JEQ":"JMP"));
        bcpc.pointer[bcpc.len++]=(eqflag?BC_JEQ:BC_JMP);
        memcpy(&bcpc.pointer[bcpc.len],&ziel,sizeof(int));
        relocation[anzreloc++]=bcpc.len;
        bcpc.len+=sizeof(int);      
      }
  if(eqflag) TO();
}

static void bc_jumptosr(int from, int ziel) {
       /* Hier jetzt ein JUMP realisieren */
      if(ziel<=from) { /* Dann ist das Ziel schon bekannt */
        int a,b;
        a=bc_index[ziel];
	b=a-bcpc.len;
	if(verbose>1) printf("Delta=%d ",b);
        if(b<=0x7fff && b>=-32766) {
	  short o=b-1-sizeof(short);
          if(verbose>1) printf(" BSR ");
	  BCADD(BC_BSR);
          memcpy(&bcpc.pointer[bcpc.len],&o,sizeof(short));
          bcpc.len+=sizeof(short);        
	} else {
          if(verbose>1) printf(" JSR ");
          BCADD(BC_JSR);
          memcpy(&bcpc.pointer[bcpc.len],&a,sizeof(int));
          bcpc.len+=sizeof(int);        
	}
      } else {
        if(verbose>1) printf(" JSR ");
        BCADD(BC_JSR);
        memcpy(&bcpc.pointer[bcpc.len],&ziel,sizeof(int));
        relocation[anzreloc++]=bcpc.len;
        bcpc.len+=sizeof(int);      
      }
}

static void bc_jumptosr2(int ziel) {
       /* Hier jetzt ein JUMP realisieren */
        if(verbose>1) printf(" JSR ");
        BCADD(BC_JSR);
        memcpy(&bcpc.pointer[bcpc.len],&ziel,sizeof(int));
        relocation[anzreloc++]=bcpc.len;
        bcpc.len+=sizeof(int);      
}

static void plist_to_stack(PARAMETER *pin, short *pliste, int anz, int pmin, int pmax) {
  int i,ii,anzpar;
  unsigned short ap,ip;
  if(pmax==-1) anzpar=anz;
  else anzpar=min(anz,pmax);
  if(anzpar<pmin) printf("Not enough parameters.\n");

  i=0;
  while(i<anzpar) {
    if(i>pmin && pmax==-1) ap=pliste[pmin];
    else ap=pliste[i];
    ip=pin[i].typ;
    switch(ap) {
      int vnr;  
      int *indexliste;;
    case PL_LEER:
      BCADD(BC_PUSHLEER);TP(PL_LEER);
      if(verbose>1) printf("%d: empty \n",ii);
      break;
    case PL_LABEL: 
    case PL_PROC:
      printf("Hier label? %d\n",pin[i].integer);
      break;
    case PL_FILENR:
    case PL_INT:    /* Integer */
      if(ip==PL_LEER) {BCADD(BC_PUSHLEER);TP(PL_LEER);}
      else if(ip==PL_EVAL) {
        bc_parser(pin[i].pointer);if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
      } else if(ip==PL_INT || ip==PL_FILENR) {
        bc_push_integer(pin[i].integer);	
	if(verbose>1) printf("<%d> ",pin[i].integer);
      } else printf("WARNING: something is wrong! %x\n",pin[i].typ);
      break;
    case PL_NUMBER:  /* Float oder Number */
    case PL_FLOAT:  /* Float oder Number */
      if(ip==PL_LEER) {BCADD(BC_PUSHLEER);TP(PL_LEER);}
      else if(ip==PL_EVAL) {
        bc_parser(pin[i].pointer);if(TL!=PL_FLOAT) BCADD(BC_X2F);TR(PL_FLOAT);
      } else if(ip==PL_NUMBER || ip==PL_FLOAT) {
	if(verbose>1) printf("<%g>.f ",pin[i].real);
        bc_push_float(pin[i].real);
      } else printf("WARNING: something is wrong! %x\n",pin[i].typ);
      break;
    case PL_ANYVALUE:
      if(ip==PL_LEER) {BCADD(BC_PUSHLEER);TP(PL_LEER);}
      else if(ip==PL_EVAL) {
        int typ=type(pin[i].pointer);
	bc_parser(pin[i].pointer);
 	if(typ&INTTYP) {
	  if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
	} else if(typ&FLOATTYP) {
	  if(TL!=PL_FLOAT) BCADD(BC_X2F);TR(PL_FLOAT);
	} else if(typ&STRINGTYP) {
	  if(TL!=PL_STRING) printf("WARNING: something is wrong. Hier ist kein String rausgekommen!\n %s",(char *)pin[i].pointer);
	} else {
	  printf("WARNING: unknown type. $%x\n",typ);
	}
      } else if(ip==PL_NUMBER || ip==PL_FLOAT) {
	if(verbose>1) printf("<%g>.f ",pin[i].real);
        bc_push_float(pin[i].real);
      } else if(ip==PL_INT || ip==PL_FILENR) {
        bc_push_integer(pin[i].integer);	
	if(verbose>1) printf("<%d> ",pin[i].integer);
      } else if(ip==PL_STRING) {
        STRING str;
	str.len=pin[i].integer;
	if(verbose>1) printf("\"%s\" ",(char *)pin[i].pointer);
	str.pointer=pin[i].pointer;
	bc_push_string(str);
      } else printf("WARNING: something is wrong! %x\n",pin[i].typ);
      break;
    case PL_STRING: /* String */
      if(ip==PL_LEER) {BCADD(BC_PUSHLEER);TP(PL_LEER);}
      else if(ip==PL_EVAL) {
        bc_parser(pin[i].pointer);if(TL!=PL_STRING) printf("Hier ist kein String rausgekommen!\n %s",(char *)pin[i].pointer);
      } else if(ip==PL_STRING) {
        STRING str;
	str.len=pin[i].integer;
	if(verbose>1) printf("\"%s\" ",(char *)pin[i].pointer);
	str.pointer=pin[i].pointer;
	bc_push_string(str);
      } else printf("WARNING: something is wrong! %x\n",pin[i].typ);

      break;
    case PL_ARRAY:  /* Array */
    case PL_IARRAY: /* Int-Array */
    case PL_FARRAY: /* float-Array */
    case PL_SARRAY: /* String-Array */
      if(ip==PL_LEER) {BCADD(BC_PUSHLEER);TP(PL_LEER);}
      else if(pin[i].typ!=PL_EVAL) printf("WARNING: something is wrong! %x\n",pin[i].typ);
      else {bc_parser(pin[i].pointer);if(TL!=PL_ARRAY) printf("WARNING: Hier ist kein Array rausgekommen!\n %s",(char *)pin[i].pointer);}
      break;
    case PL_VAR:   /* Variable */
    case PL_NVAR:   /* Variable */
    case PL_SVAR:   /* Variable */
    case PL_ARRAYVAR: /* Variable */
    case PL_IARRAYVAR: /* Variable */
    case PL_FARRAYVAR: /* Variable */
    case PL_SARRAYVAR: /* Variable */
    case PL_ANYVAR:  /* Varname */    
      vnr=pin[i].integer;
      if(pin[i].panzahl) {
	bc_indexliste(pin[i].ppointer,pin[i].panzahl);
	bc_pushvvindex(vnr,pin[i].panzahl);
      } else bc_pushvv(vnr);
      break;
    case PL_KEY: /* Keyword */
      BCADDPUSHX(pin[i].pointer);
      TP(PL_KEY);
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

static int push_indexliste(PARAMETER *p,int n) {
  int i;
  for(i=0;i<n;i++) {
    switch(p[i].typ) {
    case PL_KEY:
    case PL_EVAL:
      if(((char *)p[i].pointer)[0]==':') {
	bc_push_integer(-1);
      } else {
        bc_parser((char *)p[i].pointer);
        if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
      }
      break;
    case PL_INT:
      bc_push_integer(p[i].integer);
      break;
    case PL_NUMBER:
    case PL_FLOAT:
      bc_push_integer((int)p[i].real);
      break;
    default:
      printf("WARNING: get_indexliste: unknown type.\n");
      bc_push_integer(0);
    }
  }
  return(i);
}

void compile(int verbose) {
  int i,a,b;

  bc_index=malloc(prglen*sizeof(int));
  relocation=malloc(max(1000,prglen)*sizeof(int));
  anzreloc=0;
  bcpc.len=0;
  strings.len=0;
  strings.pointer=NULL;
  anzsymbols=0;
  symtab=NULL;
  typsp=0;
  if(verbose) {
    printf("%d\tlines.\n",prglen);
    printf("%d\tprocedures.\n",anzprocs);
    printf("%d\tlabels.\n",anzlabels);
    printf("%d\tvariables.\n",anzvariablen);
    if(verbose>1) printf("PASS A:\n");
  }
  add_string("compiled by xbbc");
  for(i=0;i<prglen;i++) {
    bc_index[i]=bcpc.len;
    if(typsp) printf("WARNING: typsp=%d\n",typsp);
    if(verbose>1) printf("%3d:$%08x %x_[%08xx%d](%d)%s \t |",i,bcpc.len,pcode[i].etyp,(unsigned int)pcode[i].opcode,pcode[i].integer,
      pcode[i].panzahl,program[i]);
fflush(stdout);
/* Sonderbehandlungen fuer ... */
#if 0
    if((pcode[i].opcode&PM_COMMS)==find_comm("INC")) {
      int vnr;
      if(verbose>1) printf(" INC ");
    //  printf("INC hat fehler! int=%d\n",pcode[i].integer);
    //  printf("INC hat fehler! panz=%d\n",pcode[i].panzahl);
      vnr=(pcode[i].ppointer)->integer;
      bc_pushv(vnr);  /* Variable */
      BCADD(BC_PUSH1);
      
        /* Hier herausfinden, was fuer eine Variable das ist und dann versch.adds benutzen.*/
      BCADD(BC_ADD);
      bc_zuweis(vnr); /* Zuweisung */
    } else 
    if((pcode[i].opcode&PM_COMMS)==find_comm("DEC")) {
      int vnr;
      if(verbose>1) printf(" DEC ");
      vnr=(pcode[i].ppointer)->integer;
      bc_pushv(vnr);  /* Variable */
      BCADD(BC_PUSH1);
       /* Hier herausfinden, was fuer eine Variable das ist und dann versch.adds benutzen.*/
      
      if(TL==PL_INT) {
        BCADD(BC_SUBi);
      } else if(TL==PL_FLOAT) {
        BCADD(BC_X2F);
        BCADD(BC_SUBf);
      } else {
        BCADD(BC_SUB);
      }
      bc_zuweis(vnr); /* Zuweisung */
    } else 
#endif
    if((pcode[i].opcode&PM_COMMS)==find_comm("LOCAL")) {
      int e=pcode[i].panzahl;
      while(--e>=0) bc_local((pcode[i].ppointer)[e].integer);
    }
/* Jetzt behandlungen nach Pcode */
    else if((pcode[i].opcode&PM_SPECIAL)==P_DATA) {
      /*Erzeugt keinen Code...*/
    }
//    else if((pcode[i].opcode&PM_COMMS)==find_comm("DIM")) {
  //    printf("DIM:\n");
 //   }
    else if((pcode[i].opcode&PM_COMMS)==find_comm("RESTORE")) {
      int j;
      if(verbose>1) printf(" RESTORE ");
      if(strlen(pcode[i].argument)) {
        j=labelnr(pcode[i].argument);
        if(j==-1) xberror(20,pcode[i].argument);/* Label nicht gefunden */
        else {
          bc_restore((int)labels[j].datapointer);
          if(verbose>1) printf(" %d %s\n",j,labels[j].name);
	  add_symbol((int) labels[j].datapointer,labels[j].name,STT_DATAPTR,0);
        }
      } else {
        bc_restore(0);
	if(verbose>1) printf(" \n");
      }
    } else if((pcode[i].opcode&PM_SPECIAL)==P_EXITIF) {
      char w1[strlen(pcode[i].argument)+1];
      char w2[strlen(pcode[i].argument)+1];
      if(verbose>1) printf(" EXIT IF ");
      wort_sep(pcode[i].argument,' ',TRUE,w1,w2);
      if(verbose>1) printf(" <%s> <%s> ",w1,w2);
      bc_parser(w2);
      if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
      BCADD(BC_NOT);
      bc_jumpto(i,pcode[i].integer,1);
    } else if((pcode[i].opcode&PM_SPECIAL)==P_SELECT) {
      bc_parser(pcode[i].argument);
      if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
      if(verbose>1) printf(" SELECT ");
      /*Vergleichswert liegt jetzt auf dem Stack.*/ 
      int npc=pcode[i].integer;
      char *w1=NULL,*w2,*w3;
      int l2,l=0,e;
      if(npc==-1) xberror(36,"SELECT"); /*Programmstruktur fehlerhaft */
      /* Case-Anweisungen finden */
      while(1) {
    //    printf("branch to line %d. <%s>\n",npc-1,program[npc-1]);
        if((pcode[npc-1].opcode&PM_SPECIAL)==P_CASE) {
          l2=strlen(program[npc-1])+1;
          if(l2>l || w1==NULL) {
            l=l2+256;
           w1=realloc(w1,l);
          }
          xtrim(program[npc-1],TRUE,w1);
          wort_sep_destroy(w1,' ',TRUE,&w2,&w3);
       
          e=wort_sep_destroy(w3,',',TRUE,&w2,&w3);
          while(e) {
	    BCADD(BC_DUP);TP(TL);   /* Vergleichswert auf Stack duplizieren fuer den folgenden Test */
            bc_parser(w2);
	    if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
	    BCADD(BC_EQUAL);TO();TR(PL_INT);BCADD(BC_NOT);
	    bc_jumpto(i,npc,1);
	    e=wort_sep_destroy(w3,',',TRUE,&w2,&w3);
          }
          npc=pcode[npc-1].integer;
	  if(npc==-1) xberror(36,"SELECT/CASE"); /*Programmstruktur fehlerhaft */
       } else if((pcode[npc-1].opcode&PM_SPECIAL)==P_DEFAULT){
          
          bc_jumpto(i,npc,0);
          npc=pcode[npc-1].integer;
	  if(npc==-1) xberror(36,"SELECT/DEFAULT"); /*Programmstruktur fehlerhaft */
	  
       } else break;
    } 
     
     /*Wenn er vorher weggesprungen ist, dann verbleibt ein Wert auf dem Stack !!*/
     /*TODO */
     } else if((pcode[i].opcode&PM_SPECIAL)==P_ENDSELECT) { 
       {BCADD(BC_POP);TO();} /* Stack korrigieren und weitermachen */
   
    } else if((pcode[i].opcode&PM_SPECIAL)==P_CASE) { 
      if(verbose>1) printf(" CASE ");
      int j,f=0,o=0;
      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_ENDSELECT)  && f==0) break;
        else if(o==P_SELECT) f++;
        else if(o==P_ENDSELECT) f--;
      }
      if(j==prglen) xberror(36,"SELECT/CASE"); /*Programmstruktur fehlerhaft */
      if(o==P_ENDSELECT) bc_jumpto(i,j,0); 
    } else if((pcode[i].opcode&PM_SPECIAL)==P_DEFAULT) { 
      if(verbose>1) printf(" DEFAULT ");
        int j,f=0,o=0;
      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_ENDSELECT)  && f==0) break;
        else if(o==P_SELECT) f++;
        else if(o==P_ENDSELECT) f--;
      }
      if(j==prglen) xberror(36,"SELECT/CASE"); /*Programmstruktur fehlerhaft */
      if(o==P_ENDSELECT) bc_jumpto(i,j,0); 
    } else if((pcode[i].opcode&PM_SPECIAL)==P_IF) {
      int j,f=0,o=0;
      bc_parser(pcode[i].argument);
      if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
      if(verbose>1) printf(" IF ");
     /*  a=pcode[i].integer; zeigt auf ENDIF */
      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_ENDIF || o==P_ELSE|| o==P_ELSEIF)  && f==0) break;
        else if(o==P_IF) f++;
        else if(o==P_ENDIF) f--;
      }
      if(j==prglen) xberror(36,"IF"); /*Programmstruktur fehlerhaft */
      if(o==P_ENDIF || o==P_ELSE) bc_jumpto(i,j+1,1);
      else if(o==P_ELSEIF) {
        bc_jumpto(i,j,1); /* Die elseif muss ausgewertet werden*/
      } else {BCADD(BC_POP);TO();} /* Stack korrigieren und weitermachen */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_ELSEIF) {
      int j,f=0,o=0;
      char w1[strlen(pcode[i].argument)+1];
      char w2[strlen(pcode[i].argument)+1];
      pcode[i].opcode&=~P_PREFETCH;
      bc_jumpto(i,pcode[i].integer,0); /* von oben draufgelaufen, gehe zum endif */
      /* Korrigiere bc_index */
      bc_index[i]=bcpc.len;
      /* Wenn wir vom Sprung kommen, landen wir hier */
      if(verbose>1) printf(" ELSE IF (corr=$%x) ",bcpc.len);

      /* Bedingung separieren */
      wort_sep(pcode[i].argument,' ',TRUE,w1,w2);
      
      /* Bedingung auswerten */
      bc_parser(w2);
      if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
        
      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_ENDIF || o==P_ELSE|| o==P_ELSEIF)  && f==0) break;
        else if(o==P_IF) f++;
        else if(o==P_ENDIF) f--;
      }
      if(j==prglen) xberror(36,"ELSE IF"); /*Programmstruktur fehlerhaft */
      if(o==P_ENDIF || o==P_ELSE) bc_jumpto(i,j+1,1);
      else if(o==P_ELSEIF) {
        bc_jumpto(i,j,1); /* Die elseif muss ausgewertet werden*/
      } else {BCADD(BC_POP);TO();} /* Stack korrigieren und weitermachen */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_GOSUB) {
      if(pcode[i].integer==-1) bc_kommando(i);
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
            pos[0]=0;pos++;
            pos2=pos+strlen(pos)-1;
            if(pos2[0]!=')') {
	      puts("GOSUB: Syntax error @ parameter list");
	      structure_warning(i,"GOSUB"); /*Programmstruktur fehlerhaft */
            } else {
	      pos2[0]=0;
	      anzpar=count_parameters(pos);
	      if(anzpar) bc_parser(pos);  /* Parameter auf den Stack */
	    }
          } else pos=buf+strlen(buf);
 
   //     }
	bc_push_integer(anzpar);
	bc_jumptosr(i,procs[pcode[i].integer].zeile);
	TO();
	TA(anzpar);

      }
    } else if((pcode[i].opcode&(PM_SPECIAL|P_NOCMD))==P_PROC) {
      int e;
      int *ix;

      if(verbose>1) printf(" PROC_%d %s ",pcode[i].integer,(char *)procs[pcode[i].integer].parameterliste);

      add_symbol(bcpc.len,procs[pcode[i].integer].name,STT_FUNC,0);

      BCADD(BC_BLKSTART); /* Stackponter erhoehen etc. */
      BCADD(BC_POP); /* Das waere jetzt die anzahl der Parameter als int auf stack*/
      /* Wir muessen darauf vertrauen, dass die anzahl Parameter stimmt */
      /* Jetzt von Liste von hinten aufloesen */
      e=procs[pcode[i].integer].anzpar;
      ix=procs[pcode[i].integer].parameterliste;
      while(--e>=0) {
        bc_local(ix[e]);  /* Rette alten varinhalt */
        typsp=1;
        bc_zuweis(ix[e]); /* Weise vom stack zu */
      }
      typsp=0;
    } else if((pcode[i].opcode&PM_SPECIAL)==P_RETURN) {
      if(verbose>1) printf(" RETURN %s ",pcode[i].argument);
      if(pcode[i].argument && strlen(pcode[i].argument)) {
        bc_parser(pcode[i].argument);
	TO();
      }      
      BCADD(BC_BLKEND);
      BCADD(BC_RTS);
      typsp=0;
    } else if((pcode[i].opcode&PM_SPECIAL)==P_WHILE) {
      bc_parser(pcode[i].argument);
      if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
      if(verbose>1) printf(" WHILE ");
      bc_jumpto(i,pcode[i].integer,1);      
    } else if((pcode[i].opcode&PM_SPECIAL)==P_UNTIL) {
      bc_parser(pcode[i].argument);
      if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
      if(verbose>1) printf(" UNTIL ");
      bc_jumpto(i,pcode[i].integer,1);      
    } else if((pcode[i].opcode&PM_SPECIAL)==P_FOR) {
      char w1[strlen(pcode[i].argument)+1],w2[strlen(pcode[i].argument)+1];
      wort_sep(pcode[i].argument,' ',TRUE,w1,w2);
      wort_sep(w1,'=',TRUE,w1,w2);
      if(verbose>1) printf(" FOR ");
      bc_zuweisung(w1,w2);
    } else if((pcode[i].opcode&PM_SPECIAL)==P_NEXT) {
      int pp=pcode[i].integer;  /* Zeile it dem zugehoerigen For */
      if(pp==-1) xberror(36,"NEXT"); /*Programmstruktur fehlerhaft */
      else {
        char *w1,*w2,*w3,*var,*limit,*step;
	int ss=0,e,st=0;
	// printf("Next: bz=%d\n",pp);
        w1=strdup(pcode[pp].argument);
        w2=malloc(strlen(w1)+1); 
        w3=malloc(strlen(w1)+1); 
        var=malloc(strlen(w1)+1); 
        step=malloc(strlen(w1)+1); 
        limit=malloc(strlen(w1)+1); 
        wort_sep(w1,' ',TRUE,w2,w3);
        /* Variable bestimmem */
        if(searchchr2(w2,'=')!=NULL) {
          wort_sep(w2,'=',TRUE,var,w1);
        } else printf("Syntax Error ! FOR %s\n",w2); 
	// printf("Variable ist: %s \n",var);
        wort_sep(w3,' ',TRUE,w1,w2);
   
        if(strcmp(w1,"TO")==0) ss=0; 
        else if(strcmp(w1,"DOWNTO")==0) ss=1; 
        else printf("Syntax Error ! FOR %s\n",w1);

        /* Limit bestimmem  */
        e=wort_sep(w2,' ',TRUE,limit,w3);
        if(e==0) printf("Syntax Error ! FOR %s\n",w2);
        else {
          if(e==1) ;
          else {
            /* Step-Anweisung auswerten  */
            wort_sep(w3,' ',TRUE,w1,step);
            if(strcmp(w1,"STEP")==0) st=1;
            else printf("Syntax Error ! FOR %s\n",w1); 
          }
        }
        /* Variable */
        bc_pushv_name(var);
	
        /* Jetzt den Inkrement realisieren a=a+step*/
	if(st) bc_parser(step);
	else {
	  BCADD(BC_PUSH1);TP(PL_INT);
	}
        /* Bei downto negativ */
	if(ss) BCADD(BC_NEG);
        BCADD(BC_ADD);TO();
        BCADD(BC_DUP);TP(TL);   /* Ergebnis duplizieren fuer den folgenden Test */
        bc_zuweis_name(var); /* Zuweisung */
        /* Jetzt auf Schleifenende testen */
        bc_parser(limit);
	if(ss) BCADD(BC_LESS);
	else BCADD(BC_GREATER);
	TO();TR(PL_INT);
        
        free(w1);free(w2);free(w3);free(var);free(step);free(limit);
        bc_jumpto(i,pp+1,1);
      }	
    } 
    else if((pcode[i].opcode&P_ENDPROC)==P_ENDPROC) {if(verbose>1) printf(" * ");typsp=0;}
    else if(pcode[i].opcode==P_ZUWEIS) {
      int vnr=pcode[i].integer;
      int ii=pcode[i].panzahl;
      int dim=variablen[vnr].pointer.a->dimension;
      int typ=variablen[vnr].typ;
      bc_parser(pcode[i].argument);
      if(typ&INTTYP) {
        if(TL!=PL_INT) BCADD(BC_X2I);TR(PL_INT);
      } else if(typ&FLOATTYP) {
        if(TL!=PL_FLOAT) BCADD(BC_X2F);TR(PL_FLOAT);
      }
      if(ii) {
        short ss=vnr;
	short f=ii;
	  push_indexliste(pcode[i].ppointer,ii);
	//  if((typ&ARRAYTYP) && ii!=dim) xberror(18,"");  /* Falsche Anzahl Indizies */
          BCADD(BC_ZUWEISINDEX);
          memcpy(&bcpc.pointer[bcpc.len],&ss,sizeof(short));
          bcpc.len+=sizeof(short);
          memcpy(&bcpc.pointer[bcpc.len],&f,sizeof(short));
          bcpc.len+=sizeof(short);
          TA(f);
          TO();
      } else bc_zuweis(vnr);
    }
    else if(pcode[i].opcode&P_IGNORE) {if(verbose>1) printf(" * ");}
    else if(pcode[i].opcode&P_INVALID) xberror(32,program[i]); /*Syntax nicht korrekt*/
    else if(pcode[i].opcode&P_EVAL)  {
      if(verbose>1) printf(" EVAL ");
      bc_kommando(i);
    } else if((pcode[i].opcode&PM_COMMS)>=anzcomms) {
      puts("Precompiler error...");
      bc_kommando(i);
    } else if((pcode[i].opcode&PM_TYP)==P_SIMPLE) {
        if(verbose>1) printf(" SIMPLE_COMM ");
        BCADD(BC_PUSHCOMM);BCADD(pcode[i].opcode&PM_COMMS);BCADD(0);
      } else {
      if((pcode[i].opcode&PM_TYP)==P_ARGUMENT) {
        if(verbose>1) printf(" ARGUMENT_COMM ");
	BCADDPUSHX(pcode[i].argument);
	BCADD(BC_PUSHCOMM);BCADD(pcode[i].opcode&PM_COMMS);BCADD(1);
      }
      else if((pcode[i].opcode&PM_TYP)==P_PLISTE) {
        int j=(pcode[i].opcode&PM_COMMS);
        if(verbose>1) printf(" PLISTE(%d) ",pcode[i].panzahl);
	if(verbose>1) printf("COMMS=%d(%d)\n",j,comms[j].pmax);

	plist_to_stack((PARAMETER *)pcode[i].ppointer,(short *)comms[j].pliste,pcode[i].panzahl,comms[j].pmin,comms[j].pmax);
        BCADD(BC_PUSHCOMM);BCADD(pcode[i].opcode&PM_COMMS);BCADD(pcode[i].panzahl);
	TA(pcode[i].panzahl);
	if(pcode[i].panzahl<comms[j].pmin)  xberror(42,""); /* Zu wenig Parameter  */
        if(comms[j].pmax!=-1 && (pcode[i].panzahl>comms[j].pmax)) xberror(45,""); /* Zu viele Parameter  */
      }
      else bc_kommando(i);
    }
    if(pcode[i].opcode&P_PREFETCH) {
      bc_jumpto(i,pcode[i].integer,0);
    }
     if(verbose>1) printf("\n");
  }  
  /* Jetzt alle Labels in Symboltabelle eintragen:*/
  if(anzlabels) {
    for(i=0;i<anzlabels;i++) {
      add_symbol(bc_index[labels[i].zeile]-sizeof(BYTECODE_HEADER),labels[i].name,STT_LABEL,0);
    }
  }
  /* Jetzt alle Variablen in Symboltabelle eintragen:*/
  if(anzvariablen) {
    int typ;
    int adr;
    for(i=0;i<anzvariablen;i++) {
      typ=variablen[i].typ;
      if(typ==INTTYP) adr=add_bss(sizeof(int));
      else if(typ==FLOATTYP) adr=add_bss(sizeof(double));
      else if(typ==STRINGTYP) adr=add_bss(sizeof(STRING));
      else if(typ==ARRAYTYP) adr=add_bss(sizeof(ARRAY));      
      if(typ==ARRAYTYP) typ|=variablen[i].pointer.a->typ;
      add_symbol(adr,variablen[i].name,STT_OBJECT,typ);
    }
  }
  if(verbose>1) printf("PASS B: %d relocations\n",anzreloc);
  else if(verbose) printf("%d\trelocations.\n",anzreloc);
  for(i=0;i<anzreloc;i++) {
    b=relocation[i];
    memcpy(&a,&bcpc.pointer[relocation[i]],sizeof(int));
    if(verbose>1) printf("%d:$%08x ziel=%d ---> $%x\n",i,b,a,bc_index[a]);

    /* Wenn a negativ, dann zeigt er in das data-Segment */

    if(a>=0) {
      a=bc_index[a]+relocation_offset;
      add_symbol(a-sizeof(BYTECODE_HEADER),NULL,STT_NOTYPE,0);
    } else {
      a=bcpc.len-a;
      add_symbol(a-sizeof(BYTECODE_HEADER),NULL,STT_DATAPTR,0);
    }
    memcpy(&bcpc.pointer[relocation[i]],&a,sizeof(int));
  }
  free(relocation);
  free(bc_index);
}
