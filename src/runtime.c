
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

extern const char version[];           /* Programmversion           */
extern const char vdate[];
extern const char xbasic_name[];

void *obh;       /* old break handler  */


/* Standard-Fehlerroutine   */


void error(char err, char *bem) {
  printf("Zeile %d: %s\n",pc-1,error_text(err,bem));
  if(!errcont) batch=0;   
}

void break_handler( int signum) {
  if(batch) {
    printf("** PROGRAM-STOP \n");
    batch=0;
    signal(signum, break_handler);
  } else {
    printf("** X11BASIC-QUIT \n");
    signal(SIGINT, obh);
    raise(signum);
  }
}
void fatal_error_handler( int signum) {
  printf("** Fataler BASIC-Interpreterfehler #%d \n",signum);
  if(batch) {
    if(pc>=0) {
      printf("    pc-1  : %s\n",program[pc-2]);
      printf("--> pc=%d : %s\n",pc-1,program[pc-1]);
      printf("    pc+1  : %s\n",program[pc]);
    } else printf("PC negativ !\n");
      printf("Stack-Pointer: SP=%d\n",sp);
      batch=0;
  } else {
    c_dump("");
    printf("Programm-Abbruch...\n");    
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
        printf("Fehler innerhalb Interrupt-FUNCTION. \n");
      }
      
  }
  signal(signum, timer_handler);
  if(everyflag) alarm(everytime); 
}

void intro(){
  printf("***************************************************************\n");
  printf("*               %s                 V. %s                *\n",xbasic_name, version);
  printf("*                   von Markus Hoffmann 1997-2001 (c)         *\n");
  printf("*                                                             *\n");
  printf("* Programmversion vom %s           *\n",vdate);
  printf("***************************************************************\n\n"); 
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
  signal(SIGBUS, fatal_error_handler);
  signal(SIGALRM, timer_handler);
}
