/* This header file is needed to compile output from xb2c */
/*  gcc delme.c -lx11basic -ldl -lm -lcurses -lncurses -lasound -lreadline -lX11 
 */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <x11basic/x11basic.h>

#define BC_STACKLEN 512
#define PL_LEER     0x00
#define PL_KEY      0x06
#define PL_INT     1
#define PL_FLOAT   2
#define PL_STRING  4
#define PL_ARRAY   8

/* Variablen Typen (unsigned char)*/

#define NOTYP             0
#define INTTYP            1
#define FLOATTYP          2
#define STRINGTYP         4
#define INDIRECTTYP       8
#define ARRAYTYP       0x10
#define CONSTTYP       0x20
#define FILENRTYP       0x40

/* X11-Basic needs these declarations:  */
#ifdef NOMAIN
extern int is_bytecode;
extern int programbufferlen;
extern char ifilename[];
extern char *programbuffer;
extern const char version[]; /* Version Number. */
extern const char vdate[];   /* Creation date.*/
extern char **program;    /* Other comments.  */
extern int prglen;
extern int datapointer;
extern PARAMETER *opstack;
extern PARAMETER *osp;
extern int verbose;
#else
extern int is_bytecode;
int programbufferlen;
char ifilename[]="dummy";     /* Program name.   Put some useful information here */
char *programbuffer;
const char version[]="1.20"; /* Version Number. Put some useful information here */
const char vdate[]="dummy";   /* Creation date.  Put some useful information here */
char **program={"compiled by xb2c"};    /* Other comments. Put some useful information here */
int prglen=sizeof(program)/sizeof(char *);
extern int datapointer;
PARAMETER *opstack;
PARAMETER *osp;
int verbose;
#endif

int vm_comm(PARAMETER *sp,int i, int anzarg);
void free_parameter(PARAMETER *p);
int add_variable_adr(char *name, unsigned char  typ, char *adr);
  /* Initialize the x11basic-library */
#define MAIN_INIT  programbufferlen=0; programbuffer=NULL; x11basicStartup(); set_input_mode(1,0); \
  atexit(reset_input_mode); \
  param_anzahl=anzahl; \
  param_argumente=argumente; \
  osp=opstack=calloc(BC_STACKLEN,sizeof(PARAMETER)); \
  programbufferlen=prglen=pc=sp=0; is_bytecode=1;


#define CLEAR {int a,j; a=((int)opstack-(int)osp)/sizeof(PARAMETER); \
      if(a) { opstack=osp; for(j=0;j<a;j++)  free_parameter(&opstack[j]); } }

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
#define PUSHX(a) opstack->integer=strlen(a); opstack->pointer=strdup(a); opstack->typ=PL_KEY; opstack++
/* TODO: binary data in Strings*/
#define PUSHS(a) opstack->integer=strlen(a); opstack->pointer=strdup(a); opstack->typ=PL_STRING; opstack++
/* TODO: Array constant */
#define PUSHA(a,l) opstack->integer=l; opstack->pointer=strdup(a); opstack->typ=PL_ARRAY; opstack++
#define PUSHLEER opstack->typ=PL_LEER; opstack++
#define POP      opstack--; free_parameter(opstack)
#define COUNT    opstack->integer=((int)opstack-(int)osp)/sizeof(PARAMETER); opstack->typ=PL_INT; opstack++
#define EVAL     opstack--; kommando(opstack->pointer); free(opstack->pointer)
#define PUSHARRAYELEM(a,b) opstack+=vm_pusharrayelem(a,opstack,b)
#define ZUWEISINDEX(a,b) opstack+=vm_zuweisindex(a,opstack,b)
#define PUSHV(a)  push_v(opstack,&variablen[a]); opstack++
// define PUSHV(a) opstack+=vm_pushv(a,opstack)
#define PUSHVV(a) opstack+=vm_pushvv(a,opstack)
#define PUSHVVI(a,b) opstack+=vm_pushvvi(a,opstack,b)
#define ZUWEIS(a) opstack+=vm_zuweis(a,opstack)
#define LOCAL(a)  do_local(a,sp)
#define MOD      opstack+=vm_mod(opstack)
#define NOT      (opstack-1)->integer=~(opstack-1)->integer
//#define X2I      vm_x2i(opstack)
#define X2I      if((opstack-1)->typ==PL_FLOAT) {(opstack-1)->integer=(int)(opstack-1)->real; (opstack-1)->typ=PL_INT;}
//#define X2F      vm_x2f(opstack)
#define X2F      if((opstack-1)->typ==PL_INT) {(opstack-1)->real=(double)(opstack-1)->integer; (opstack-1)->typ=PL_FLOAT;}

#define NEG      vm_neg(opstack)
#define EXCH     *opstack=opstack[-1];opstack[-1]=opstack[-2];opstack[-2]=*opstack

#define DUP      opstack+=vm_dup(opstack)
#define EQUAL    opstack+=vm_equal(opstack)
#define LESS     opstack+=vm_less(opstack)
#define GREATER  opstack+=vm_greater(opstack)
#define POW      opstack+=vm_pow(opstack)
#define DIV      opstack--;(opstack-1)->real/=opstack->real
#define MUL      opstack+=vm_mul(opstack)
#define MULi     opstack--;(opstack-1)->integer*=opstack->integer
#define MULf     opstack--;(opstack-1)->real*=opstack->real
#define SUB      opstack+=vm_sub(opstack)
#define SUBi     opstack--;(opstack-1)->integer-=opstack->integer
#define SUBf     opstack--;(opstack-1)->real-=opstack->real
#define AND      opstack--;(opstack-1)->integer=(opstack-1)->integer & opstack->integer
#define OR       opstack--;(opstack-1)->integer=(opstack-1)->integer | opstack->integer
#define XOR      opstack--;(opstack-1)->integer=(opstack-1)->integer ^ opstack->integer

#define ADD      opstack+=vm_add(opstack)
#define ADDi     opstack--;(opstack-1)->integer+=opstack->integer
#define ADDf     opstack--;(opstack-1)->real+=opstack->real
#define ADDs     opstack--; {\
                 int l=(opstack-1)->integer;char *p=(opstack-1)->pointer;\
		 (opstack-1)->integer+=opstack->integer;\
		 (opstack-1)->pointer=malloc((opstack-1)->integer+1);\
		 memcpy((opstack-1)->pointer,p,l);free(p);\
		 memcpy((opstack-1)->pointer+l,opstack->pointer,opstack->integer+1);\
		 } free(opstack->pointer)
#define PUSHASYS(a) opstack+=vm_asysvar(opstack,a)
#define PUSHSSYS(a) opstack+=vm_ssysvar(opstack,a)
#define PUSHSYS(a)  opstack+=vm_sysvar(opstack,a)
#define PUSHCOMM(a,n) opstack+=vm_comm(opstack,a,n)
#define PUSHFUNC(a,n) opstack+=vm_func(opstack,a,n)
#define PUSHSFUNC(a,n) opstack+=vm_sfunc(opstack,a,n)

/* simplified commands */

#define COMM_BEEP putchar('\007')
#define COMM_BELL putchar('\007')
#define COMM_VSYNC activate()
#define COMM_SHOWPAGE activate()
#define COMM_END puts("done.");batch=0
#define COMM_INC opstack--;if(variablen[opstack->integer].typ&FLOATTYP) (*((double *)opstack->pointer))++;  else if(variablen[opstack->integer].typ&INTTYP) (*((int *)opstack->pointer))++
#define COMM_DEC opstack--;if(variablen[opstack->integer].typ&FLOATTYP) (*((double *)opstack->pointer))--;  else if(variablen[opstack->integer].typ&INTTYP) (*((int *)opstack->pointer))--

/* conditional helpers*/
#define JUMPIFZERO if((--opstack)->integer==0) goto

/* simplified functions */

#define PFUNC_ABS (opstack-1)->real=fabs((opstack-1)->real)
#define PFUNC_ACOS (opstack-1)->real=acos((opstack-1)->real)
#define PFUNC_ACOSH (opstack-1)->real=acosh((opstack-1)->real)
#define PFUNC_ASIN (opstack-1)->real=asin((opstack-1)->real)
#define PFUNC_ASINH (opstack-1)->real=asinh((opstack-1)->real)
#define PFUNC_ATAN (opstack-1)->real=atan((opstack-1)->real)
#define PFUNC_ATN (opstack-1)->real=atan((opstack-1)->real)
#define PFUNC_ATANH (opstack-1)->real=atanh((opstack-1)->real)

#define PFUNC_CBRT (opstack-1)->real=cbrt((opstack-1)->real)
#define PFUNC_CEIL (opstack-1)->real=ceil((opstack-1)->real)
#define PFUNC_COS (opstack-1)->real=cos((opstack-1)->real)
#define PFUNC_COSH (opstack-1)->real=cosh((opstack-1)->real)

#define PFUNC_EXP (opstack-1)->real=exp((opstack-1)->real)
#define PFUNC_EXPM1 (opstack-1)->real=expm1((opstack-1)->real)

#define PFUNC_FLOOR (opstack-1)->real=floor((opstack-1)->real)

#define PFUNC_HYPOT (opstack-1)->real=hypot((opstack-1)->real)

#define PFUNC_LN (opstack-1)->real=log((opstack-1)->real)
#define PFUNC_LOG (opstack-1)->real=log((opstack-1)->real)
#define PFUNC_LOG10 (opstack-1)->real=log10((opstack-1)->real)
#define PFUNC_LOG1P (opstack-1)->real=log1p((opstack-1)->real)
#define PFUNC_LOGB (opstack-1)->real=logb((opstack-1)->real)

#define PFUNC_RAND (opstack-1)->real=rand((opstack-1)->real)

#define PFUNC_SIN (opstack-1)->real=sin((opstack-1)->real)
#define PFUNC_SINH (opstack-1)->real=sinh((opstack-1)->real)

#define PFUNC_SQR (opstack-1)->real=sqrt((opstack-1)->real)
#define PFUNC_SQRT (opstack-1)->real=sqrt((opstack-1)->real)

#define PFUNC_TAN (opstack-1)->real=tan((opstack-1)->real)
#define PFUNC_TANH (opstack-1)->real=tanh((opstack-1)->real)

