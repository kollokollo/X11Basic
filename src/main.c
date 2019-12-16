/* MAIN.C  for the X11-Basic interpreter xbasic (c) Markus Hoffmann */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ================================================================
 * X11BASIC is free software and comes with NO WARRANTY - 
 * read the file COPYING for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

#include "defs.h"
#include "x11basic.h"
#ifdef ANDROID
  #include "terminal.h"
#endif
#ifdef WINDOWS
  #include <windows.h>
  #include "file.h"
#endif

#ifdef WINDOWS_NATIVE
  HINSTANCE hInstance;
#endif


#ifndef ANDROID
static
#endif
int loadfile=FALSE;
#ifdef GERMAN
char ifilename[100]="neu.bas";       /* Standartfile             */
#else
char ifilename[100]="new.bas";
#endif
int prglen=0;
int verbose=0;
int runfile;
int daemonf=0;
static int quit_on_end=0;
int programbufferlen=0;
char *programbuffer=NULL;
char **program=NULL;

#ifdef ANDROID
void intro() {
  putchar(27);
  printf("c");
  printf("*************************************\n"
         "* %10s          V.%5s       *\n"
         "*  by Markus Hoffmann 1997-2020 (c) *\n"
         "*                                   *\n"
         "* lib %29s *\n"
         "*************************************\n\n",
	     xbasic_name,libversion,libvdate);
}
#else
static void intro() {
  printf("**********************************************************\n"
         "*    %10s                     V." VERSION "               *\n"
         "*                       by Markus Hoffmann 1997-2020 (c) *\n"
#ifdef NOGRAPHICS
         "*    ====> Version without graphics support <====        *\n"
#else
         "***  " FEATURE1 FEATURE2 FEATURE3 FEATURE4 FEATURE5 FEATURE6 FEATURE7 
	      FEATURE8 FEATURE9 FEATURE10 FEATURE11 "     ***\n"
#endif
#ifdef GERMAN
         "* Programmversion vom     %30s *\n"
         "* Library V. %s vom     %30s *\n"
#else
         "* version date:           %30s *\n"
         "* library V.%s date:    %30s *\n"
#endif
         "**********************************************************\n\n",
	     xbasic_name,VERSION_DATE,libversion,libvdate);
}

#endif

static void usage() {
  printf(
#ifdef GERMAN
    "Bedienung: %s [-e -h -l] [<filename>] --- Basic-Programm ausführen  [%s]\n\n"
    " -l\t\t--- Programm nur laden\n"
    " -e <kommando>\t--- Basic Kommando ausführen\n"
    " --eval <ausdruck>\t--- Num. Ausdruck auswerten\n"
    " --quitonend\t--- Verlasse Interbreter bei END\n"
    " -h --help\t--- Diese Kurzhilfe\n"
    " --help <Stichwort>\t--- Hilfe zum Stichwort/Befehl\n"
#else
    "Usage: %s [-e -h -l] [<filename>] --- run basic program [%s]\n\n"
    " -l\t\t--- do not run the program (only load)\n"
    " -e <command>\t--- execute basic command\n"
    " --eval <exp>\t--- evaluate num. expression\n"
    " --quitonend\t--- quit on END statement\n"
    " -h --help\t--- Usage\n"
    " --help <topic>\t--- Print help on topic\n"
#endif
#if defined FRAMEBUFFER && !defined ANDROID
    " --framebuffer <dev>\t--- set framebuffer device for output [%s]\n"
    " --mouse <dev>\t--- set mouse device  [/dev/input/mice]\n"
    " --keyboard <dev>\t--- set keyboard device  [/dev/input/event4]\n"
    ,xbasic_name,ifilename,fbdevname);
#else
    ,xbasic_name,ifilename);
#endif
}

#ifdef WINDOWS
static void show_embedded_docu(char *filename) {
  if(exist(filename)) {
    printf("Embedded documentation of the X11-Basic program %s:\n\n",filename);
    char line[256];
    FILE *dptr=fopen(filename,"r");
    while(!myeof(dptr)) {
      lineinput(dptr,line,255);
      if(*line=='#' && line[1]=='#') puts(line);
    }
    fclose(dptr);
  } else {
    intro();
  }
  simple_gets("Press RETURN to close the window.");
  quit_x11basic(0);
}
#endif

#ifndef ANDROID
static
#endif
void kommandozeile(int anzahl, char *argumente[]) {
  int count,quitflag=0;
  /* process command line parameters */
  runfile=TRUE;
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-l") || !strcmp(argumente[count],"--load-only")) runfile=FALSE;
    else if(!strcmp(argumente[count],"--eval")) {
      printf("%.13g\n",parser(argumente[++count]));
      quitflag=1;
    } else if(!strcmp(argumente[count],"-e") || !strcmp(argumente[count],"--exec")) {
      char *p=argumente[++count];
      if(*p==':') { /* If it is hex-coded bytecode, load and run it...*/
        STRING bcpc=inhexs(p+1);
	// memdump(bcpc.pointer,bcpc.len);
  	do_run(); /*set variables etc to zero */
  	run_bytecode(bcpc.pointer,bcpc.len);
      } else kommando(p);
      quitflag=1;
    } else if(!strcmp(argumente[count],"-h")) {
      intro();
      usage();
      quitflag=1;
    } else if(!strcmp(argumente[count],"--help")) {
      intro();
      if(count<anzahl-1 && *argumente[count+1]!='-') {
        char *buffer=strdup(argumente[count+1]);
        xtrim(buffer,TRUE,buffer);
        do_help(buffer);
	free(buffer);
      } else usage();
      quitflag=1;
#ifdef WINDOWS
    } else if(!strncmp(argumente[count],"-doc_",5)) {
      show_embedded_docu(argumente[count]+5);
#endif
    } else if(!strcmp(argumente[count],"--daemon")) {
      intro();
      daemonf=1;
    } 
    else if(!strcmp(argumente[count],"--quitonend")) quit_on_end=1;
    else if(!strcmp(argumente[count],"-v"))	     verbose++;
    else if(!strcmp(argumente[count],"-q"))	     verbose--;
#if defined FRAMEBUFFER && !defined ANDROID
    else if(!strcmp(argumente[count],"--framebuffer")) strncpy(fbdevname,      argumente[++count],256);
    else if(!strcmp(argumente[count],"--mouse"))       strncpy(mousedevname,   argumente[++count],256);
    else if(!strcmp(argumente[count],"--keyboard"))    strncpy(keyboarddevname,argumente[++count],256);
#endif
    else if(*(argumente[count])=='-') ; /* do nothing, these could be options for the BASIC program*/
    else {
      if(!loadfile) {
        loadfile=TRUE;
        strcpy(ifilename,argumente[count]);
      }
    }
  }
  if(quitflag) quit_x11basic(EX_OK);
}


#ifdef ANDROID
int orig_main(int anzahl, char *argumente[]) {
  static int isdirectmode=0;
#else
int main(int anzahl, char *argumente[]) {
#endif
#ifdef WINDOWS_NATIVE
  hInstance=GetModuleHandle(NULL);
#endif

  x11basicStartup();   /* initialisieren   */
  set_input_mode(1,0);  /* Terminalmode auf noncanonical, no echo */
  (void) atexit(reset_input_mode);
  param_anzahl=anzahl;
  param_argumente=argumente;

  if(anzahl<2) {    /* Kommandomodus */
    intro();
    batch=0;
  } else {
    kommandozeile(anzahl, argumente);    /* Kommandozeile bearbeiten */
    if(loadfile) {
      if(exist(ifilename)) {
        int prgerrors=loadprg(ifilename);
	if(prgerrors) {
	  printf("Errors detected in %s. Can not run.\n",ifilename);
	}
	if(!prgerrors && runfile) do_run();
      } else printf("ERROR: %s not found !\n",ifilename);
    }
  }

  char *buffer;
  char *zw;
  
  /* Programmablaufkontrolle  */
  for(;;) {
    programmlauf();
    echoflag=batch=0;
    if(quit_on_end) quit_x11basic(EX_OK);
#ifdef ANDROID
    if(isdirectmode) break;
    isdirectmode=1;
#endif
    if(daemonf) zw=simple_gets("");
    else zw=do_gets("> ");
    if(zw==NULL) quit_x11basic(0);
    else {
      buffer=strdup(zw);
      kommando(buffer);
      free(buffer);
    }
#ifdef ANDROID
    isdirectmode=0;
#endif
  }
  return(EX_OK);
}
