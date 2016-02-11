
/* runtime.c Sachen fuer den Interpreter-Programmablauf (c) Markus Hoffmann*/


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "protos.h"
#include "globals.h"
#include "options.h"


void *obh;       /* old break handler  */


/* Standard-Fehlerroutine   */


void error(char err, char *bem) {
#ifdef GERMAN
  printf("Zeile %d: %s\n",pc-1,error_text(err,bem));
#else
  printf("Line %d: %s\n",pc-1,error_text(err,bem));
#endif
  if(!errcont) batch=0;   
}

void break_handler( int signum) {
  if(batch) {
    puts("** PROGRAM-STOP");
    batch=0;
    signal(signum, break_handler);
  } else {
    puts("** X11BASIC-QUIT");
    signal(SIGINT, obh);
    raise(signum);
  }
}
void fatal_error_handler( int signum) {
  int obatch=batch;
  switch(signum) {
  case SIGILL: error(104,""); break;
#ifndef WINDOWS
  case SIGBUS: error(102,""); break;
#endif
  case SIGSEGV: error(101,""); break;
#ifndef WINDOWS
  case SIGPIPE: error(110,""); break;
#endif
  default:
    printf("** Fataler BASIC-Interpreterfehler #%d \n",signum);
    batch=0;
  }
  if(obatch) {
    if(pc>0) {
      if(pc>1) printf("    pc-1  : %s\n",program[pc-2]);
      printf("--> pc=%d : %s\n",pc-1,program[pc-1]);
      printf("    pc+1  : %s\n",program[pc]);
    } else puts("PC negativ !");
    puts("** PROGRAM-STOP");
  } else {
    printf("Stack-Pointer: SP=%d\n",sp);
    c_dump("");
    puts("** fatal error ** X11BASIC-QUIT");    
    signal(signum,SIG_DFL);
  }
  raise(signum);
}


void timer_handler( int signum) {
  if(alarmpc==-1) printf("** Uninitialisierter Interrupt #%d \n",signum);
  else {
    int oldbatch,osp=sp,pc2;
      pc2=procs[alarmpc].zeile;
      
      if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
      else {printf("Stack overflow ! PC=%d\n",pc); batch=0;}
      oldbatch=batch;batch=1;
      programmlauf();
      batch=min(oldbatch,batch);
      if(osp!=sp) {
	pc=stack[--sp]; /* wenn error innerhalb der func. */
        puts("Fehler innerhalb Interrupt-FUNCTION.");
      }
      
  }
  signal(signum, timer_handler);
  if(everyflag) alarm(everytime); 
}


/* Initialisierungsroutine  */

void x11basicStartup() {

#ifdef CONTROL  
  cs_init();        /* Kontrollsystem anmelden */
#endif
  /* Signal- und Interrupt-Handler installieren  */
  obh=signal(SIGINT, break_handler);
  signal(SIGILL, fatal_error_handler);
  signal(SIGSEGV, fatal_error_handler);
#ifndef WINDOWS
  signal(SIGBUS, fatal_error_handler);
  signal(SIGPIPE, fatal_error_handler);
  signal(SIGALRM, timer_handler);
#endif
}
