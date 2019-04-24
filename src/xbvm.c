/* XBVM.C   The X11-Basic virtual machine.          (c) Markus Hoffmann
*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif

#ifdef WINDOWS
#define EX_OK 0
#else
#include <sysexits.h>
#endif
#include "defs.h"
#include "x11basic.h"
#include "xbasic.h"
#include "type.h"
#include "parser.h"
#include "variablen.h"
#include "parameter.h"
#include "bytecode.h"
#include "file.h"
#include "wort_sep.h"
#include "virtual-machine.h"
#include "io.h"
#include "number.h"

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

static void intro(){
  printf("****************************************************\n"
         "*  %10s virtual machine  V.%5s             *\n"
         "*                 by Markus Hoffmann 1997-2019 (c) *\n"
         "*                                                  *\n"
#ifdef GERMAN
         "* Programmvers. vom %30s *\n"
         "* Lib.  V. %s vom %30s *\n"
#else
         "* version date:     %30s *\n"
         "* lib. V.%s date: %30s *\n"
#endif
         "****************************************************\n\n",
	     xbasic_name,version,vdate,libversion,libvdate);
}

static void usage(){
  printf(
#ifdef GERMAN
    "Bedienung: %s [-e -h -l] [<filename>] --- Basic-Programm ausführen  [%s]\n\n"
    " -e <kommando>\t--- Basic Kommando ausführen\n"
    " --eval <ausdruck>\t--- Num. Ausdruck auswerten\n"
    " -h --help\t--- Diese Kurzhilfe\n"
    "-q --quiet\t--- weniger Ausgaben\n"
    "-v --verbose\t--- mehr Ausgaben\n"
#else
    "Usage: %s [-e -h -l] [<filename>] --- run basic program [%s]\n\n"
    " -e <command>\t--- execute basic command\n"
    " --eval <exp>\t--- evaluate num. expression\n"
    " -h --help\t--- Usage\n"
    "-q --quiet\t--- be more quiet\n"
    "-v --verbose\t--- be more verbose\n"
#endif
    ,xbasic_name,ifilename);
}
static STRING inhexs(const char *n) {
  int l=strlen(n);
  STRING ergebnis;
  ergebnis.len=(l+1)/2;
  ergebnis.pointer=malloc(ergebnis.len+1);  
  unsigned int value=0;
  int i=0;
  while(*n) {
    value<<=4;
    if(v_digit(*n)) value+=(int)(*n-'0');
    else if(*n>='a' && *n<='f') value+=(int)(*n-'a')+10;
    else if(*n>='A' && *n<='F') value+=(int)(*n-'A')+10;
    n++;
    if((i&1)) (ergebnis.pointer)[i>>1]=(value&0xff);
    i++;
  }
  (ergebnis.pointer)[ergebnis.len]=0;
  return(ergebnis);
}
static void doit(STRING bcpc) {
  PARAMETER *p;
  int n; 

  if(verbose) printf("Virtual Machine: %d bytes.\n",bcpc.len);
  if(verbose>1) memdump((unsigned char *)bcpc.pointer,bcpc.len);
  do_run();
  p=virtual_machine(bcpc,0,&n,NULL,0);
  if(verbose) printf("done.\n");
  dump_parameterlist(p,n);  
  free_pliste(n,p);
}
#if 0
static void compileandrun(char *ausdruck) {
  PARAMETER *p;
  int n;
  bcpc.len=0;
  printf("Expression: %s\n",ausdruck);
  bc_parser(ausdruck);
  memdump((unsigned char *)bcpc.pointer,bcpc.len);
  printf("Virtual Machine: %d Bytes\n",bcpc.len);
  p=virtual_machine(bcpc,0,&n,NULL,0);
  dump_parameterlist(p,n);
  free_pliste(n,p);
}
#endif
static void kommandozeile(int anzahl, char *argumente[]) {
  int count;

  /* Kommandozeile bearbeiten   */
  runfile=TRUE;
  for(count=1;count<anzahl;count++) {
    if (strcmp(argumente[count],"--eval")==FALSE) {
      printf("%.13g\n",parser(argumente[++count]));
    } else if (strcmp(argumente[count],"-e")==FALSE || strcmp(argumente[count],"--exec")==FALSE) {
      char *p=argumente[++count];
      if(p[0]==':') { /*Hier k"onnte man stattdessen codierten bytecode laden und starten...*/
        bcpc=inhexs(p+1);
	// memdump(bcpc.pointer,bcpc.len);
	doit(bcpc);
      } else kommando(p);
     
    } else if (strcmp(argumente[count],"-v")==FALSE) verbose++;
    else if (strcmp(argumente[count],"-q")==FALSE) verbose--;
    else if (strcmp(argumente[count],"-h")==FALSE) {
      intro();
      usage();
    } else if (strcmp(argumente[count],"--daemon")==FALSE) {
      daemonf=1;
    } else {
      if(!loadfile) {
        loadfile=TRUE;
        strcpy(ifilename,argumente[count]);
      }
    }
  }
}


static int loadbcprg(char *filename) {  
  FILE *dptr=fopen(filename,"rb"); 
  int len=lof(dptr); 
  fclose(dptr);
  char *adr=bcpc.pointer=malloc(len+1);
  bload(filename,bcpc.pointer,len);
  bcpc.len=len;
  programbufferlen=bcpc.len;
  programbuffer=bcpc.pointer;
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


extern int fix_bytecode_header(BYTECODE_HEADER *bytecode);

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
   if(!exist(filename)) {
   #ifdef ATARI
     strcat(filename,".prg");
     printf("try <%s>\n",filename); 
     /*TODO cannot find of the name of the program arg[0]*/
     strcpy(filename,"xbc.prg");
     verbose=2;
   #else
     strcat(filename,".exe");
   #endif
   }
   if(!exist(filename)) printf("ERROR: could not link X11-Basic code.\n");
   if(verbose) printf("selfseek=%s %d\n",selfseek,s);
   dptr=fopen(filename,"rb"); 
   fseek(dptr,s,SEEK_SET);
   bcpc.len=lof(dptr)-s; 
   #ifdef ATARI
   printf("Asking for %d bytes.\n",bcpc.len);
   #endif
   bcpc.pointer=malloc(bcpc.len+1);
   #ifdef ATARI
   if(bcpc.pointer==NULL) {
     perror("malloc");
     sleep(5);
   }
   #endif
   if(fread(bcpc.pointer,1,bcpc.len,dptr)<bcpc.len) printf("read error.\n");
//   bload(filename,bcpc.pointer,bcpc.len);
//   memdump(bcpc.pointer,64);
//   verbose=4;
   fclose(dptr);
   #ifdef ATARI
   memdump(bcpc.pointer,64);
   sleep(5);
   #endif

   if(verbose) printf("%s loaded (%d Bytes)\n",argumente[0],bcpc.len);
   if(fix_bytecode_header((BYTECODE_HEADER *)bcpc.pointer)==0) {
     BYTECODE_HEADER *bytecode=(BYTECODE_HEADER *)bcpc.pointer;
    /* Sicherstellen, dass der Speicherberiech auch gross genug ist fuer bss segment*/
    if(bytecode->bssseglen>bytecode->relseglen+bytecode->stringseglen+bytecode->symbolseglen) {
      bcpc.len+=bytecode->bssseglen-bytecode->stringseglen-bytecode->symbolseglen;
      bcpc.pointer=realloc(bcpc.pointer,bcpc.len);
      #ifdef ATARI
      if(bcpc.pointer==NULL) {
        perror("malloc");
        sleep(5);
      }
      #endif
    }
     bytecode_init(bcpc.pointer); 
     programbufferlen=bcpc.len;
     programbuffer=bcpc.pointer;
    
     doit(bcpc);
   } else {
     printf("ERROR: Something is wrong, no code!\n");
     #ifdef ATARI
     sleep(5);
     #endif
   }
 } else if(anzahl<2) {
    intro();
    usage();
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
