/* XBVM.C   The X11-basic virtual machine.          (c) Markus Hoffmann
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
#include "x11basic.h"
#include "xbasic.h"
#include "variablen.h"
#include "parameter.h"
#include "bytecode.h"
#include "file.h"
#include "wort_sep.h"
#include "kommandos.h"
#include "virtual-machine.h"

void reset_input_mode();
void x11basicStartup();

void kommando(char *);
extern char *databuffer;
extern int databufferlen;
extern int param_anzahl;
extern char **param_argumente;
extern const char xbasic_name[];
extern int pc,sp,err,errcont,everyflag,echoflag;

const char version[]=VERSION;        /* Programmversion          */
const char vdate[]=VERSION_DATE;
extern const char libversion[];
extern const char libvdate[];
int loadfile=FALSE;
char ifilename[100]="b.b";
int prglen=0;
int runfile,daemonf;
int programbufferlen=0;
char *programbuffer=NULL;
char **program=NULL;

#ifdef WINDOWS
#include <windows.h>
HINSTANCE hInstance;
#endif

STRING bcpc;

int verbose=0;

char selfseek[]="4007111";

void intro(){
  printf("**********************************************************\n"
         "*    %10s                     V.%5s              *\n"
         "*                       by Markus Hoffmann 1997-2011 (c) *\n"
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

void usage(){
  printf(
#ifdef GERMAN
    "Bedienung: %s [-e -h -l] [<filename>] --- Basic-Programm ausführen  [%s]\n\n"
    " -l\t\t--- Programm nur laden\n"
    " -e <kommando>\t--- Basic Kommando ausführen\n"
    " --eval <ausdruck>\t--- Num. Ausdruck auswerten\n"
    " -h --help\t--- Diese Kurzhilfe\n"
    " --help <Stichwort>\t--- Hilfe zum Stichwort/Befehl\n"
    "-q --quiet\t--- weniger Ausgaben\n"
    "-v --verbose\t--- mehr Ausgaben\n"
#else
    "Usage: %s [-e -h -l] [<filename>] --- run basic program [%s]\n\n"
    " -l\t\t--- do not run the program (only load)\n"
    " -e <command>\t--- execute basic command\n"
    " --eval <exp>\t--- evaluate num. expression\n"
    " -h --help\t--- Usage\n"
    " --help <topic>\t--- Print help on topic\n"
    "-q --quiet\t--- be more quiet\n"
    "-v --verbose\t--- be more verbose\n"
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
    } else if (strcmp(argumente[count],"-v")==FALSE) {
      verbose++;
    } else if (strcmp(argumente[count],"-q")==FALSE) {
      verbose--;
    } else if (strcmp(argumente[count],"-h")==FALSE) {
      intro();
      usage();
      quitflag=1;
    } else if (strcmp(argumente[count],"--help")==FALSE) {
      intro();
      if(count<anzahl-1 && *argumente[count+1]!='-') {
        strncpy(buffer,argumente[count+1],100);
        xtrim(buffer,TRUE,buffer);
        do_help(buffer);
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
   if(quitflag) c_quit(NULL,0);
}


int loadbcprg(char *filename) {  
  int len;
  char *adr;
  FILE *dptr;
  dptr=fopen(filename,"rb"); len=lof(dptr); fclose(dptr);
  bcpc.pointer=adr=malloc(len+1);
  bload(filename,bcpc.pointer,len);
  bcpc.len=len;
  if(verbose) printf("%s loaded (%d Bytes)\n",filename,bcpc.len);
 // memdump(bcpc.pointer,32);
 
  /* Ueberpruefe ob ein gueltiger Header dabei ist */

  if(adr[0]==BC_BRAs && adr[1]==sizeof(BYTECODE_HEADER)-2) {
    BYTECODE_HEADER *bytecode=(BYTECODE_HEADER *)adr;
    if(verbose) printf("Bytecode header found (V.%x)\n",bytecode->version);
    if(bytecode->version!=BC_VERSION) {
      printf("WARNING: This Bytecode was compiled for a different version of"
      "X11-Basic.\n ERROR.\n");
      return(-1);
    }
    /* Sicherstellen, dass der Speicherberiech auch gross genug ist fuer bss segment*/
    if(bytecode->bssseglen>bytecode->relseglen+bytecode->stringseglen+bytecode->symbolseglen) {
      bcpc.len+=bytecode->bssseglen-bytecode->stringseglen-bytecode->symbolseglen;
      bcpc.pointer=adr=realloc(adr,bcpc.len);
    }
    return(0);
  } else {
    printf("VM: ERROR, file format not recognized. $%02x $%02x\n",adr[0],adr[1]);
    return(-1);
  }
}



void doit(STRING bcpc) {
  PARAMETER *p;
  int n;
  if(verbose) printf("Virtual Machine: %d bytes.\n",bcpc.len);
  do_run();
  p=virtual_machine(bcpc,&n);
  if(verbose) printf("done.\n");
  dump_parameterlist(p,n);  
  free_pliste(n,p);
}

int main(int anzahl, char *argumente[]) {

  x11basicStartup();   /* initialisieren   */

  set_input_mode(1,0);  /* Terminalmode auf noncanonical, no echo */
  atexit(reset_input_mode);
  param_anzahl=anzahl;
  param_argumente=argumente;


   /* Programm ist vom Compiler hintendrangepackt. */
    if(atoi(selfseek)!=4007111) {
      int s=atoi(selfseek);
      char filename[strlen(argumente[0])+8];
      char filename2[strlen(argumente[0])+8];
      FILE *dptr;
      wort_sep(argumente[0],'.',FALSE,filename,filename2);
      if(!exist(filename)) strcat(filename,".exe");
      if(!exist(filename)) printf("ERROR: could not link X11-Basic code.\n");
      if(verbose) printf("selfseek=%s %d\n",selfseek,s);
      dptr=fopen(filename,"rb"); bcpc.len=lof(dptr); fclose(dptr);
      bcpc.pointer=malloc(bcpc.len+1);
      bload(filename,bcpc.pointer,bcpc.len);
      bcpc.pointer+=s;
      bcpc.len-=s;

      if(verbose) printf("%s loaded (%d Bytes)\n",argumente[0],bcpc.len);
      if(bytecode_init(bcpc.pointer)) doit(bcpc);
      else printf("Something is wrong, no code!\n");
    }
    
  if(anzahl<2) {
    intro();
  } else {
    kommandozeile(anzahl, argumente);    /* Kommandozeile bearbeiten */
    if(loadfile) {
      if(exist(ifilename)) {
        if(loadbcprg(ifilename)==0) {
  	  if(runfile) {
            if(bytecode_init(bcpc.pointer)) doit(bcpc);
          } else printf("xbvm: nothing to do.\n");
	} else printf("xbvm: could not run %s.\n",ifilename);
      } else printf("ERROR: %s not found !\n",ifilename);
    }
  }
  return(EX_OK);
}
