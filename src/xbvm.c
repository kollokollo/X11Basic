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
#include "defs.h"
#include "vtypes.h"
#include "ptypes.h"
#include "bytecode.h"

void reset_input_mode();
void x11basicStartup();
void programmlauf();

void kommando(char *);
extern char *databuffer;
extern int param_anzahl;
extern char **param_argumente;
extern const char xbasic_name[];
extern int pc,sp,err,errcont,everyflag,batch,echoflag;

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
char *program[MAXPRGLEN];

STRING bcpc;

int verbose=1;

void intro(){
  puts("***************************************************************");
  printf("*           %10s                     V.%5s            *\n",xbasic_name, version);
  puts("*                    by Markus Hoffmann 1997-2007 (c)         *");
  puts("*                                                             *");
  printf("* version date:           %30s      *\n",vdate);
  printf("* library V. %s date:   %30s      *\n",libversion,libvdate);
  puts("***************************************************************");
  puts("");
}

void usage(){
  puts("\n Usage:\n ------ \n");
  printf(" %s [-e -h -l] [<filename>] --- run basic program [%s]\n\n",xbasic_name,ifilename);
  puts("-l                  --- do not run the program (only load)");
  puts("-e <command>        --- execute basic command");
  puts("--eval <exp>        --- evaluate num. expression");
  puts("-h --help           --- Usage");
  puts("-q --quiet          --- be more quiet");
  puts("-v --verbose        --- be more verbose");
  puts("--help <topic>      --- Print help on topic\n");
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


void loadbcprg(char *filename) {  
  int len;
  FILE *dptr;
  clear_parameters();
  programbufferlen=prglen=pc=sp=0;
  dptr=fopen(filename,"r"); len=lof(dptr); fclose(dptr);
  bcpc.pointer=malloc(len);
  bload(filename,bcpc.pointer,len);
  bcpc.len=len;
  if(verbose) printf("%s loaded (%d Bytes)\n",filename,len);
  /* Ueberpruefe ob ein gueltiger Header dabei ist und setze databuffer */
  if(bcpc.pointer[0]==BC_BRAs && bcpc.pointer[1]==sizeof(BYTECODE_HEADER)) {
    if(verbose) printf("Bytecode header found (V.%d)\n",
    ((BYTECODE_HEADER *)bcpc.pointer)->version);
    databuffer=bcpc.pointer+((BYTECODE_HEADER *)bcpc.pointer)->textseglen+sizeof(BYTECODE_HEADER);
    if(verbose) printf("Databuffer $%08x contains: <%s>\n",(long)databuffer,databuffer);
  }
}

void doit() {
  PARAMETER *p;
  int n;
  if(verbose) printf("Virtual Machine: %d Bytes\n",bcpc.len);
  p=virtual_machine(bcpc,&n);  
  if(n) {
    int j;
    printf("\nAuf dem Stapel sind %d Parameter:\n",n);
    for(j=0;j<n;j++) {
      printf("%2d: ",j);
      if(p[j].typ==PL_INT) printf(" int %d\n",p[j].integer);
      else if(p[j].typ==PL_FLOAT) printf(" flt %g\n",p[j].real);
      else if(p[j].typ==PL_STRING) printf("   $ <%s>\n",p[j].pointer);
      else if(p[j].typ==PL_KEY) printf(" KEY <%s>\n",p[j].pointer);
      else printf("%d %d %g\n",p[j].typ,p[j].integer,p[j].real);
    }
  }
  free_pliste(n,p);
}
main(int anzahl, char *argumente[]) {
  char *obcpc;
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
        loadbcprg(ifilename);
	obcpc=bcpc.pointer;
	if(runfile) doit();
	free(obcpc);
      } else printf("ERROR: %s not found !\n",ifilename);
    }
  }
}
