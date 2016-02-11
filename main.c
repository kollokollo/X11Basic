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
#ifdef WINDOWS
#define EX_OK 0
#else
#include <sysexits.h>
#endif
#include "defs.h"
#include "ptypes.h"
#include "parser.h"

void reset_input_mode();
void x11basicStartup();
void programmlauf();
char *do_gets (char *);

void loadprg(char *);
void kommando(char *);

extern int param_anzahl;
extern char **param_argumente;
extern const char xbasic_name[];
extern int pc,sp,err,errcont,everyflag,batch,echoflag;

const char version[]=VERSION;        /* Programmversion          */
const char vdate[]=VERSION_DATE;
extern const char libversion[];
extern const char libvdate[];
int loadfile=FALSE;
#ifdef GERMAN
char ifilename[100]="neu.bas";       /* Standartfile             */
#else
char ifilename[100]="new.bas";
#endif
int prglen=0;
int runfile,daemonf;
int programbufferlen=0;
char *programbuffer=NULL;
char *program[MAXPRGLEN];

void intro() {
  printf("**********************************************************\n"
         "*    %10s                     V.%5s              *\n"
         "*                       by Markus Hoffmann 1997-2010 (c) *\n"
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
  char buffer[100];

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
        strncpy(buffer,argumente[count+1],100);
        xtrim(buffer,TRUE,buffer);
        c_help(buffer);
      } else usage();
      quitflag=1;
    } else if (strcmp(argumente[count],"--daemon")==FALSE) {
      intro();
      daemonf=1;
    } else {
      if(!loadfile) {
        loadfile=TRUE;
        strcpy(ifilename,argumente[count]);
      }
    }
   }
   if(quitflag) c_quit("",0);
}
#ifdef WINDOWS

#include <windows.h>
HINSTANCE hInstance;
#endif
extern char *simple_gets(char *);
int main(int anzahl, char *argumente[]) {
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
	if(runfile) c_run("");
      } else printf("ERROR: %s not found !\n",ifilename);
    }
  }

  /* Programmablaufkontrolle  */
  for(;;) {
    programmlauf();
    echoflag=batch=0;
    if(daemonf) zw=simple_gets("");
    else zw=do_gets("> ");
    if(zw==NULL) c_quit("",0);
    else {
      strcpy(buffer,zw);
      kommando(buffer);
    }
  }
  return(EX_OK);
}
