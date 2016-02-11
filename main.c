/* MAIN.C                                           (c) Markus Hoffmann

*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#ifndef WINDOWS
#ifndef ATARI
  #include <pthread.h>    /* POSIX Threads */
#endif
#endif
#if defined WINDOWS || defined ANDROID
  #define EX_OK 0
#else
  #include <sysexits.h>
#endif
#ifdef ANDROID
  #include <jni.h>
  #include <android/log.h>
  #include <android/bitmap.h>
  #include "android.h"
#endif

#include "config.h"
#include "defs.h"
#include "x11basic.h"
#include "wort_sep.h"
#include "file.h"
#ifdef ANDROID
  #include "terminal.h"
#endif
#ifdef WINDOWS
  #include <windows.h>
  HINSTANCE hInstance;
#endif

char *do_gets (char *);

const char version[]=VERSION;        /* Programmversion          */
const char vdate[]=VERSION_DATE;
int loadfile=FALSE;
#ifdef GERMAN
char ifilename[100]="neu.bas";       /* Standartfile             */
#else
char ifilename[100]="new.bas";
#endif
int prglen=0;
int verbose=0;
int runfile,daemonf=0;
int programbufferlen=0;
char *programbuffer=NULL;
char **program=NULL;

#ifdef ANDROID
void intro() {
  putchar(27);
  printf("c");
  printf("*************************************\n"
         "* %10s          V.%5s       *\n"
         "*  by Markus Hoffmann 1997-2015 (c) *\n"
         "*                                   *\n"
         "* lib %29s *\n"
         "*************************************\n\n",
	     xbasic_name,libversion,libvdate);
}
#else
void intro() {
  printf("**********************************************************\n"
         "*    %10s                     V.%5s              *\n"
         "*                       by Markus Hoffmann 1997-2015 (c) *\n"
         "*                                                        *\n"
#ifdef GERMAN
         "* Programmversion vom     %30s *\n"
         "* Library V. %s vom     %30s *\n"
#else
         "* version date:           %30s *\n"
         "* library V.%s date:    %30s *\n"
#endif
         "**********************************************************\n\n",
	     xbasic_name,version,vdate,libversion,libvdate);
}

#endif

void usage() {
  printf(
#ifdef GERMAN
    "Bedienung: %s [-e -h -l] [<filename>] --- Basic-Programm ausführen  [%s]\n\n"
    " -l\t\t--- Programm nur laden\n"
    " -e <kommando>\t--- Basic Kommando ausführen\n"
    " --eval <ausdruck>\t--- Num. Ausdruck auswerten\n"
    " -h --help\t--- Diese Kurzhilfe\n"
    " --help <Stichwort>\t--- Hilfe zum Stichwort/Befehl\n"
#else
    "Usage: %s [-e -h -l] [<filename>] --- run basic program [%s]\n\n"
    " -l\t\t--- do not run the program (only load)\n"
    " -e <command>\t--- execute basic command\n"
    " --eval <exp>\t--- evaluate num. expression\n"
    " -h --help\t--- Usage\n"
    " --help <topic>\t--- Print help on topic\n"
#endif
    ,xbasic_name,ifilename);
}

void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;

  /* Kommandozeile bearbeiten   */
  runfile=TRUE;
  for(count=1;count<anzahl;count++) {
    if (strcmp(argumente[count],"-l")==FALSE)               runfile=FALSE;
    else if (strcmp(argumente[count],"--load-only")==FALSE) runfile=FALSE;
    else if (strcmp(argumente[count],"--eval")==FALSE) {
      printf("%.13g\n",parser(argumente[++count]));
      quitflag=1;
    } else if (strcmp(argumente[count],"-e")==FALSE) {
      kommando(argumente[++count]);
      quitflag=1;
    } else if (strcmp(argumente[count],"--exec")==FALSE) {
      kommando(argumente[++count]);
      quitflag=1;
    } else if (strcmp(argumente[count],"-h")==FALSE) {
      intro();
      usage();
      quitflag=1;
    } else if (strcmp(argumente[count],"--help")==FALSE) {
      intro();
      if(count<anzahl-1 && *argumente[count+1]!='-') {
        char *buffer=strdup(argumente[count+1]);
        xtrim(buffer,TRUE,buffer);
        do_help(buffer);
	free(buffer);
      } else usage();
      quitflag=1;
    } else if (strcmp(argumente[count],"--daemon")==FALSE) {
      intro();
      daemonf=1;
    } else if (strcmp(argumente[count],"-v")==FALSE) {
      verbose++;
    } else if (strcmp(argumente[count],"-q")==FALSE) {
      verbose--;
    } else {
      if(!loadfile) {
        loadfile=TRUE;
        strcpy(ifilename,argumente[count]);
      }
    }
   }
   if(quitflag) quit_x11basic(0);
}

extern char *simple_gets(char *);

#ifdef ANDROID
int orig_main(int anzahl, char *argumente[]) {
  static int isdirectmode=0;
#else
int main(int anzahl, char *argumente[]) {
#endif
  char buffer[MAXSTRLEN],*zw;
#ifdef WINDOWS
  hInstance=GetModuleHandle(NULL);
#endif

  x11basicStartup();   /* initialisieren   */

  set_input_mode(1,0);  /* Terminalmode auf noncanonical, no echo */
  atexit(reset_input_mode);
  param_anzahl=anzahl;
  param_argumente=argumente;

  if(anzahl<2) {    /* Kommandomodus */
    intro();
    batch=0;
  } else {
    kommandozeile(anzahl, argumente);    /* Kommandozeile bearbeiten */
    if(loadfile) {
      if(exist(ifilename)) {
        loadprg(ifilename);
	if(runfile) do_run();
      } else printf("ERROR: %s not found !\n",ifilename);
    }
  }

  /* Programmablaufkontrolle  */
  for(;;) {
    programmlauf();
    echoflag=batch=0;
#ifdef ANDROID
    if(isdirectmode) break;
    isdirectmode=1;
#endif
    if(daemonf) zw=simple_gets("");
    else zw=do_gets("> ");
    if(zw==NULL) quit_x11basic(0);
    else {
      strcpy(buffer,zw);
      kommando(buffer);
    }
#ifdef ANDROID
    isdirectmode=0;
#endif
  }
  return(EX_OK);
}
