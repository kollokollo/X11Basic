/* runtime.c routines for the interpreter program flow control (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "defs.h"
#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#endif
#include "x11basic.h"
#include "xbasic.h"
#include "memory.h"
#include "type.h"
#include "parser.h"
#include "kommandos.h"
#include "variablen.h"
#include "parameter.h"
#include "bytecode.h"
#include "virtual-machine.h"
#include "ccs.h"


static void *obh;       /* old break handler  */

/* X11-Basic standard error routine */

int globalerr=0;

static void run_bytecode_from_handler(int jumppc) {
  batch=1;
  int n;
  PARAMETER par[1];

  STRING bcpc;
  bcpc.pointer=programbuffer;
  bcpc.len=programbufferlen;
  par[0].integer=0;
  par[0].typ=PL_INT;
  par[0].panzahl=0;
  par[0].ppointer=NULL;

  BYTECODE_HEADER *bh=(BYTECODE_HEADER *)bcpc.pointer;
  char *rodata=programbuffer+sizeof(BYTECODE_HEADER)+bh->textseglen;

  if(stack_check(sp)) {
    stack[++sp]=bcpc.len;  /*Return wird diesen Wert holen, dann virt machine beenden.*/
    virtual_machine(bcpc,jumppc, &n,par,1,rodata);
    sp--;	
  } else {printf("Stack overflow! PC=%d\n",pc); batch=0;}
}

/* default error handler: stop program and print error message */
static void default_error(char errnr, const char *bem) {
  batch=0; /* Stop program */
  if(pc>0) printf("ERROR at line %d: %s\n",original_line(pc-1),error_text(errnr,bem));
  else if(program_adr) printf("ERROR at offset $%x: %s\n",program_adr,error_text(errnr,bem));
  else printf("ERROR: %s\n",error_text(errnr,bem));
#ifdef ANDROID
  invalidate_screen();
#endif
}



/* Evoke error handling. Either do a default action or trigger the 
 * function which has been defined with ON ERROR GOSUB.
 */

void xberror(char errnr, const char *bem) {
  globalerr=errnr; /*save error number for ERR system variable */
  if(errcont) { /* ON ERROR CONT or ON ERROR GOSUB */
    errcont=0; /* reset to default action for the next time */
    if(errorpc!=-1) { /* ON ERROR GOSUB or GOTO */
      int osp=sp;
      int pc2=errorpc; /* Number of procedure or label */
      if(errorpctype&8)       pc2=procs[pc2].zeile;  /* ON ERROR GOSUB */
      else if(errorpctype&16) pc2=labels[pc2].zeile; /* ON ERROR GOTO */
      else if(errorpctype!=0) {
        printf("ERROR ERROR--> TODO errorpctype=%x\n",errorpctype);
	batch=0; /* Stop program */
      }
      if((errorpctype&7)==0) {
        if(stack_check(sp)) {stack[sp++]=pc;pc=pc2+1;}
        else {printf("Stack overflow! PC=%d\n",pc); batch=0;}
        programmlauf();  /* run the error procedure */
	
        if(osp!=sp) pc=stack[--sp]; /* wenn error innerhalb der func. */
      } else {
        /* Bytecode: */
	run_bytecode_from_handler(errorpc);
      }
    }
  } else default_error(errnr,bem);
}

extern int breakcont;
extern int breakpc;

static void break_handler( int signum) {
  if(batch) {
    if(breakcont) {
      breakcont=0;
      if(breakpc!=-1) {
        int pc2=breakpc;
        if(breakpctype==8) pc2=procs[pc2].zeile;
        else if(breakpctype==16) pc2=labels[pc2].zeile;
        else if(breakpctype!=0) {
          printf("BREAK/E ERROR--> TODO\n");
	  batch=0;
        }
        if((breakpctype&7)==0) {
          int osp=sp;
          if(stack_check(sp)) {stack[sp++]=pc;pc=pc2+1;}
          else {printf("Stack overflow! PC=%d\n",pc); batch=0;}
          programmlauf();
          if(osp!=sp) {
	    pc=stack[--sp]; /* wenn error innerhalb der func. */
          }
        } else {
          /* Bytecode: */
	  run_bytecode_from_handler(breakpc);
	}
      }
    } else { /* default action */
      puts("** PROGRAM-STOP");
      batch=0; /* stop program */
    }
    signal(signum, break_handler);
  } else {
    puts("** X11BASIC-QUIT");
#ifdef ANDROID
    backlog("** X11BASIC-QUIT (break)");
#endif
#ifdef DOOCS
    doocssig_handler(signum);
#endif  
    signal(SIGINT, SIG_DFL);
    raise(signum);
  }
}
#ifdef ANDROID
extern void android_sigaction(int signum, siginfo_t *info, void *reserved);
#endif

#ifdef HAVE_BACKTRACE
/* Get some information about the crash... */

static void print_trace() {
  void *array[16];
  size_t size;
  char **strings;
  size_t i;
  size=backtrace(array,16);
  strings=backtrace_symbols(array,size);
  printf("Obtained %zd stack frames.\n",size);
  for(i=0;i<size;i++) printf("%02zd %s\n",i,strings[i]);
  free (strings);
}
#endif



static void fatal_error_handler( int signum) {
  int obatch=batch;
  switch(signum) {
  case SIGILL: xberror(104,""); break;
#ifndef WINDOWS
  case SIGBUS: xberror(102,""); break;
#endif
  case SIGSEGV: xberror(101,""); break;
#ifndef WINDOWS
  case SIGPIPE: xberror(110,""); break;
#endif
  default:
#ifdef GERMAN
    printf("** Fataler BASIC-Interpreterfehler #%d\n",signum);
#else
    printf("** Fatal interpreter error #%d\n",signum);
#endif
    batch=0;
  }
  if(obatch) {
    if(pc>0) {
      if(pc>1) printf("    pc-1 : %s\n",program[pc-2]);
      printf("--> pc=%d : %s\n",pc-1,program[pc-1]);
      printf("    pc+1 : %s\n",program[pc]);
    } else puts("PC negativ !");
    puts("** PROGRAM-STOP");
  } else {
#ifdef ANDROID
   // signal(signum,android_sigaction);
#else
    signal(signum,SIG_DFL);
#endif
    printf("Stack-Pointer: SP=%d/%d\n",sp,stack_size);
    c_dump(NULL,0);
    puts("** fatal error ** X11BASIC-QUIT");    
  }
#ifdef HAVE_BACKTRACE  
  print_trace();
#endif
  
#ifdef ANDROID
  invalidate_screen();
  backlog("** X11BASIC-QUIT (fatal)");
  android_sigaction(signum,NULL,NULL);
#else
   raise(signum);
#endif
}

/* Interrupt handler used for timer events, EVERY/AFTER*/

static void timer_handler( int signum) {
  if(alarmpc==-1) {
#ifdef GERMAN
    printf("** Uninitialisierter Interrupt #%d \n",signum);
#else
    printf("** Uninitialized interrupt #%d \n",signum);
#endif
  } else {
    if(alarmpctype==0) {
      int oldbatch,osp=sp,pc2;
      pc2=procs[alarmpc].zeile;
      if(stack_check(sp)) {stack[sp++]=pc;pc=pc2+1;}
      else {printf("Stack overflow! PC=%d\n",pc); batch=0;}
      oldbatch=batch;
      batch=1;
      programmlauf();
      batch=min(oldbatch,batch);
      if(osp!=sp) pc=stack[--sp]; /* wenn error innerhalb der func. */
      
    } else if(alarmpctype==1) {
      /* Bytecode: */
      run_bytecode_from_handler(alarmpc);
    } else {
      // TODO:
      void (*func)();	  
      func=(void *)INT2POINTER(alarmpc);
      func();
    }
  }
  signal(signum, timer_handler);
  if(everyflag) alarm(everytime); 
}


/* initialized memory areas. Must be called before using any routines.
 */
void libx11basic_init() {
  expand_stack();   /* Stack initialisieren */
  expand_vars();    /* Platz für Variablen initialisieren.*/
  expand_procs();   /* Platz für Procedures initialisieren.*/
  expand_labels();  /* Platz für Labels initialisieren.*/
}

/* initializes memory areas and 
 * interrupt handlers, which are needed for 
 * runing a program 
 */
void x11basicStartup() {
  libx11basic_init();
#ifdef CONTROL  
  cs_init();        /* Kontrollsystem anmelden */
#endif
#ifdef HAVE_MQTT  
  mqtt_init();        /* Kontrollsystem anmelden */
#endif
  /* Signal- und Interrupt-Handler installieren  */
  obh=signal(SIGINT, break_handler);
#ifndef ANDROID
  signal(SIGILL, fatal_error_handler);
  signal(SIGSEGV, fatal_error_handler);
#endif
// Alarm handling is still not working on WINDOWS, al though
// the alarm() function is there (declared in <io.h> instead of <unistd.h>)
#ifndef  WINDOWS  
  signal(SIGALRM, timer_handler);
#endif
#ifndef ANDROID
#ifndef  WINDOWS
  signal(SIGBUS, fatal_error_handler);
#endif

#ifdef DOOCS
  signal(SIGPIPE, doocssig_handler);
  signal(SIGTERM, doocssig_handler);
#else
#ifndef WINDOWS
  signal(SIGPIPE, fatal_error_handler);
#endif
#endif
#endif
}
