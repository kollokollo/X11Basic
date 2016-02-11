/* MAIN.C 

   (c) Markus Hoffmann 
*/

 /* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"
#include "defs.h"

#define FALSE 0
#define TRUE (!FALSE)

void reset_input_mode();
void x11basicStartup();
void programmlauf();
char *do_gets (char *); 
void intro();
void loadprg(char *);
void kommando(char *);


extern int param_anzahl;
extern char **param_argumente;
extern const char xbasic_name[];
extern int pc,sp,err,errcont,everyflag,batch,echo;



const char version[]=VERSION;           /* Programmversion           */
const char vdate[]=VERSION_DATE;
int loadfile=FALSE;
char ifilename[100]="input.bas";       /* Standartfile             */

int prglen=0;
int runfile,daemonf;
int programbufferlen=0;
char *programbuffer=NULL;
char *program[MAXPRGLEN];

void usage(){
  printf("\n Bedienungsanleitung: \n");
  printf(" -------------------- \n\n");
  printf("%s [-e -h -l] [<filename>] --- Basic-Programm ausfuehren  [%s]\n",xbasic_name,ifilename);
  printf("\n"); 
  printf("-l                  --- Programm nur laden \n");
  printf("-e <kommando>       --- Basic Kommando ausfuehren \n");
  printf("--eval <ausdruck>   --- Num. Ausdruck auswerten  \n");
  printf("-h --help           --- Usage  \n");
/*  printf("--help <topic>      --- Print help on topic \n");  */
  printf("\n");
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
      usage();
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
   if(quitflag) c_quit("");
}

main(int anzahl, char *argumente[]) {
  char buffer[MAXSTRLEN],*zw; 
 
  x11basicStartup();   /* initialisieren   */  

  set_input_mode(1,0);  /* Terminalmode auf noncanonical, no echo */
  atexit(reset_input_mode);
  param_anzahl=anzahl;
  param_argumente=argumente;
  
  if(anzahl<2) {    /* Kommandomodus */
    intro();
    usage();
    batch=0;
  } else {
    kommandozeile(anzahl, argumente);    /* Kommandozeile bearbeiten */
    if(loadfile) {
      if(exist(ifilename)) {
        loadprg(ifilename);
	if (runfile) c_run("");     
      } else printf("ERROR: %s not found !\n",ifilename);
    }
  } 
  
  /* Programmablaufkontrolle  */
  for(;;) {
    programmlauf();
    echo=batch=0;
    if(daemonf) zw=simple_gets("");
    else zw=do_gets("> ");
    if(zw==NULL) c_quit("");
    else {
      strcpy(buffer,zw);  
      kommando(buffer);
    }
  }
}
