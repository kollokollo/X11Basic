
/* runtime.c Sachen fuer den Interpreter-Programmablauf (c) Markus Hoffmann*/


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "defs.h"
#include "x11basic.h"
#include "xbasic.h"
#include "kommandos.h"


static void *obh;       /* old break handler  */


/* Standard-Fehlerroutine   */

int globalerr=0;

void xberror(char errnr, const char *bem) {
  extern int globalerr;
  globalerr=errnr;
  if(errcont) {   
    errcont=0;
    if(errorpc!=-1) {
      int osp=sp;
        if(sp<STACKSIZE) {stack[sp++]=pc;pc=errorpc+1;}
        else {printf("Stack overflow ! PC=%d\n",pc); batch=0;}
        programmlauf();
	
        if(osp!=sp) {
	  pc=stack[--sp]; /* wenn error innerhalb der func. */
        }
      }
  } else { 
    batch=0;   
#ifdef GERMAN
    printf("FEHLER in Zeile %d: %s\n",original_line(pc-1),error_text(errnr,bem));
#else
    if(pc>0) printf("ERROR at line %d: %s\n",original_line(pc-1),error_text(errnr,bem));
    else printf("ERROR: %s\n",error_text(errnr,bem));
#endif
#ifdef ANDROID
    invalidate_screen();
#endif
  }
}

extern int breakcont;
extern int breakpc;

static void break_handler( int signum) {
  if(batch) {
    if(breakcont) {
      breakcont=0;
      if(breakpc!=-1) {
        int osp=sp;
        if(sp<STACKSIZE) {stack[sp++]=pc;pc=breakpc+1;}
        else {printf("Stack overflow ! PC=%d\n",pc); batch=0;}
        programmlauf();
        if(osp!=sp) {
	  pc=stack[--sp]; /* wenn error innerhalb der func. */
        }
      }
    } else {
      puts("** PROGRAM-STOP");
      batch=0;
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
      if(pc>1) printf("    pc-1  : %s\n",program[pc-2]);
      printf("--> pc=%d : %s\n",pc-1,program[pc-1]);
      printf("    pc+1  : %s\n",program[pc]);
    } else puts("PC negativ !");
    puts("** PROGRAM-STOP");
  } else {
#ifdef ANDROID
   // signal(signum,android_sigaction);
#else
    signal(signum,SIG_DFL);
#endif
    printf("Stack-Pointer: SP=%d\n",sp);
    c_dump(NULL,0);
    puts("** fatal error ** X11BASIC-QUIT");    
  }
#ifdef ANDROID
  invalidate_screen();
  backlog("** X11BASIC-QUIT (fatal)");
  android_sigaction(signum,NULL,NULL);
#else
   raise(signum);
#endif
}
#ifdef WINDOWS
void alarm(int dummy) {

}
#endif

static void timer_handler( int signum) {
  if(alarmpc==-1) {
#ifdef GERMAN
    printf("** Uninitialisierter Interrupt #%d \n",signum);
#else
    printf("** Uninitialized interrupt #%d \n",signum);
#endif
  } else {
    int oldbatch,osp=sp,pc2;
      pc2=procs[alarmpc].zeile;
      
      if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
      else {printf("Stack overflow ! PC=%d\n",pc); batch=0;}
      oldbatch=batch;batch=1;
      programmlauf();
      batch=min(oldbatch,batch);
      if(osp!=sp) {
	pc=stack[--sp]; /* wenn error innerhalb der func. */
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
#ifndef ANDROID
  signal(SIGILL, fatal_error_handler);
  signal(SIGSEGV, fatal_error_handler);
#endif
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
