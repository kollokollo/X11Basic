/* This header file is needed to compile output from xb2c */
/*  gcc delme.c -lx11basic -ldl -lm -lcurses -lncurses -lasound -lreadline -lX11 virtual-machine.o
 */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <x11basic/x11basic.h>

typedef struct {
  short typ;
  int integer;
  double real;
  void *pointer;
} PARAMETER;

#define BC_STACKLEN 512
#define PL_LEER     1
#define PL_KEY     2
#define PL_INT     4
#define PL_FLOAT   8
#define PL_STRING  0x10

/* X11-Basic needs these declarations:  */

int programbufferlen=0;
char ifilename[]="dummy";     /* Program name.   Put some useful information here */
char *programbuffer=NULL;
const char version[]="1.17"; /* Version Number. Put some useful information here */
const char vdate[]="dummy";   /* Creation date.  Put some useful information here */
char *program[]={"compiled by xb2c"};    /* Other comments. Put some useful information here */
int prglen=sizeof(program)/sizeof(char *);
extern int datapointer;
PARAMETER *opstack;
PARAMETER *osp;
int verbose=0;
extern char *databuffer;
extern int databufferlen;


int bc_comm(PARAMETER *sp,int i, int anzarg);
void free_parameter(PARAMETER p);

  /* Initialize the x11basic-library */
#define MAIN_INIT  x11basicStartup(); set_input_mode(1,0); \
  atexit(reset_input_mode); \
  param_anzahl=anzahl; \
  param_argumente=argumente; \
  osp=opstack=malloc(BC_STACKLEN*sizeof(PARAMETER)); \
  clear_parameters(); programbufferlen=prglen=pc=sp=0;


inline char *acv(char *a) {
  if(a) {
    int len=strlen(a);
    char *p=malloc(len+1);
    strncpy(p,a,len);
    return(p);
  }
  return NULL;
} 


#define CLEAR {int a,j; a=((int)opstack-(int)osp)/sizeof(PARAMETER); \
      if(a) { opstack=osp; for(j=0;j<a;j++)  free_parameter(opstack[j]); } }

#define NOOP
#define RESTORE(a) datapointer=a
#define PUSH0    opstack->integer=0; opstack->typ=PL_INT; opstack++
#define PUSH1    opstack->integer=1; opstack->typ=PL_INT; opstack++
#define PUSH2    opstack->integer=2; opstack->typ=PL_INT; opstack++
#define PUSHM1   opstack->integer=-1; opstack->typ=PL_INT; opstack++
#define PUSHB(a) opstack->integer=a; opstack->typ=PL_INT; opstack++
#define PUSHI(a) opstack->integer=a; opstack->typ=PL_INT; opstack++
#define PUSHW(a) opstack->integer=a; opstack->typ=PL_INT; opstack++
#define PUSHF(a) opstack->real=a; opstack->typ=PL_FLOAT; opstack++
#define PUSHX(a) opstack->integer=strlen(a); opstack->pointer=acv(a); opstack->typ=PL_KEY; opstack++
#define PUSHS(a) opstack->integer=strlen(a); opstack->pointer=acv(a); opstack->typ=PL_STRING; opstack++

#define PUSHLEER opstack->typ=PL_LEER; opstack++
#define POP      opstack--; free_parameter(*opstack)
#define COUNT    opstack->integer=((int)opstack-(int)osp)/sizeof(PARAMETER); opstack->typ=PL_INT; opstack++
#define EVAL     opstack--; kommando(opstack->pointer); free(opstack->pointer)
#define PUSHARRAYELEM(a) opstack+=bc_pusharrayelem(opstack,a)
#define ZUWEISINDEX(a) opstack+=bc_zuweisindex(opstack,a)
#define PUSHV    opstack+=bc_pushv(opstack)
#define PUSHVV   opstack+=bc_pushvv(opstack)
#define ZUWEIS   opstack+=bc_zuweis(opstack)
#define MOD      opstack+=bc_mod(opstack)
#define NOT      (opstack-1)->integer=~(opstack-1)->integer
#define X2I      bc_x2i(opstack)
#define X2F      bc_x2f(opstack)
#define NEG      bc_neg(opstack)
#define EXCH     bc_exch(opstack)
#define DUP      opstack+=bc_dup(opstack)
#define EQUAL    opstack+=bc_equal(opstack)
#define LESS     opstack+=bc_less(opstack)
#define GREATER  opstack+=bc_greater(opstack)
#define POW      opstack+=bc_pow(opstack)
#define DIV      opstack--;(opstack-1)->real/=opstack->real
#define MUL      opstack+=bc_mul(opstack)
#define MULi     opstack--;(opstack-1)->integer*=opstack->integer
#define MULf     opstack--;(opstack-1)->real*=opstack->real
#define SUB      opstack+=bc_sub(opstack)
#define SUBi     opstack--;(opstack-1)->integer-=opstack->integer
#define SUBf     opstack--;(opstack-1)->real-=opstack->real
#define AND      obstack--;(opstack-1)->integer=(opstack-1)->integer & opstack->integer
#define OR       obstack--;(opstack-1)->integer=(opstack-1)->integer | opstack->integer
#define XOR      obstack--;(opstack-1)->integer=(opstack-1)->integer ^ opstack->integer

#define ADD      opstack+=bc_add(opstack)
#define ADDi     opstack--;(opstack-1)->integer+=opstack->integer
#define ADDf     opstack--;(opstack-1)->real+=opstack->real
#define ADDs     opstack--; {\
                 int l=(opstack-1)->integer;char *p=(opstack-1)->pointer;\
		 (opstack-1)->integer+=opstack->integer;\
		 (opstack-1)->pointer=malloc((opstack-1)->integer+1);\
		 memcpy((opstack-1)->pointer,p,l);free(p);\
		 memcpy((opstack-1)->pointer+l,opstack->pointer,opstack->integer+1);\
		 } free(opstack->pointer)
#define PUSHASYS(a) opstack+=bc_asysvar(opstack,a)
#define PUSHSSYS(a) opstack+=bc_ssysvar(opstack,a)
#define PUSHSYS(a)  opstack+=bc_sysvar(opstack,a)
#define PUSHCOMM(a,n) opstack+=bc_comm(opstack,a,n)
#define PUSHFUNC(a,n) opstack+=bc_func(opstack,a,n)
#define PUSHSFUNC(a,n) opstack+=bc_sfunc(opstack,a,n)

