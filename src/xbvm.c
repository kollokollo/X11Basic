/* XBVM.C   The X11-Basic virtual machine.  (c) Markus Hoffmann */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#if defined WINDOWS || defined ANDROID
  #define EX_OK 0
  #define EX_DATAERR 0
  #define EX_NOINPUT 0
  #define EX_CONFIG 0
#else
  #include <sysexits.h>
#endif
#include "defs.h"
#include "x11basic.h"
#ifdef WINDOWS
  #include <windows.h>
#endif
#ifdef WINDOWS_NATIVE
  HINSTANCE hInstance;
#endif
#include "bytecode.h"


int loadfile=0;
char ifilename[100]="b.b";
int prglen=0;
int verbose=0;
int daemonf=0;
int programbufferlen=0;
char *programbuffer=NULL;
char **program=NULL;

#ifdef WINDOWS
#include <windows.h>
HINSTANCE hInstance;
#endif

STRING bcpc;

static char selfseek[]="4007111";

static void intro() {
  printf("**********************************************************\n"
         "*    %10s virtual machine     V." VERSION "               *\n"
         "*                       by Markus Hoffmann 1997-2019 (c) *\n"
#ifdef NOGRAPHICS
         "*    ====> Version without graphics support <====        *\n"
#else
         "***  " FEATURE1 FEATURE2 FEATURE3 FEATURE4 FEATURE5 FEATURE6 FEATURE7 
	      FEATURE8 FEATURE9 FEATURE10 "          ***\n"
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

static void usage() {
  printf(
#ifdef GERMAN
    "Bedienung:\n%s [-e -h -l] [<filename>]\t--- Basic-Programm ausführen\t[%s]\n\n"
    " -e <kommando>\t--- Basic Kommando ausführen\n"
    " --eval <ausdruck>\t--- Num. Ausdruck auswerten\n"
    " -h --help\t--- Diese Kurzhilfe\n"
    " -q --quiet\t--- weniger Ausgaben\n"
    " -v --verbose\t--- mehr Ausgaben\n"
#else
    "Usage:\n%s [-e -h -l] [<filename>]\t--- run basic program\t[%s]\n\n"
    " -e <command>\t--- execute basic command\n"
    " --eval <exp>\t--- evaluate num. expression\n"
    " -h --help\t--- Usage\n"
    " -q --quiet\t--- be more quiet\n"
    " -v --verbose\t--- be more verbose\n"
#endif
#if defined FRAMEBUFFER && !defined ANDROID
    " --framebuffer <dev>\t--- set framebuffer device for output [%s]\n"
    " --mouse <dev>\t--- set mouse device  [/dev/input/mice]\n"
    " --keyboard <dev>\t--- set keyboard device  [/dev/input/event4]\n"
    ,"xbvm",ifilename,fbdevname);
#else
    ,"xbvm",ifilename);
#endif
}

#if 0
static void compileandrun(char *ausdruck) {
  int n;
  bcpc.len=0;
  printf("Expression: %s\n",ausdruck);
  bc_parser(ausdruck);
  memdump((unsigned char *)bcpc.pointer,bcpc.len);
  printf("Virtual Machine: %d Bytes\n",bcpc.len);
  PARAMETER *p=virtual_machine(bcpc,0,&n,NULL,0);
  dump_parameterlist(p,n);
  free_pliste(n,p);
}
#endif


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

/* process command line parameters */

static void kommandozeile(int anzahl, char *argumente[]) {
  int count;
  /* process command line parameters */
  for(count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"--eval")) {
      printf("%.13g\n",parser(argumente[++count]));
    } else if(!strcmp(argumente[count],"-e") || !strcmp(argumente[count],"--exec")) {
      char *p=argumente[++count];
      if(p[0]==':') { /* If it is hex-coded bytecode, load and run it...*/
        bcpc=inhexs(p+1);
	// memdump(bcpc.pointer,bcpc.len);
  	do_run(); /*set variables etc to zero */
  	run_bytecode(bcpc.pointer,bcpc.len);
      } else kommando(p);
    } 
    else if(!strcmp(argumente[count],"-h")) {
      intro();
      usage();
    }
#ifdef WINDOWS
    else if(!strncmp(argumente[count],"-doc_",5)) show_embedded_docu(argumente[count]+5);
#endif
    else if(!strcmp(argumente[count],"--daemon"))    daemonf=1;
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
}


static int loadbcprg(char *filename) {  
  FILE *dptr=fopen(filename,"rb"); 
  bcpc.len=lof(dptr); 
  fclose(dptr);
  bcpc.pointer=malloc(bcpc.len+1);
  bload(filename,bcpc.pointer,bcpc.len);
  if(verbose) printf("%s loaded (%d Bytes)\n",filename,bcpc.len);
 // memdump(bcpc.pointer,32);
 
  /* Check if it is a valid header...*/
  if(fix_bytecode_header((BYTECODE_HEADER *)bcpc.pointer)==0) {
    BYTECODE_HEADER *bytecode=(BYTECODE_HEADER *)bcpc.pointer;
    bcpc.len=bytecode_make_bss(bytecode,&bcpc.pointer,bcpc.len);
    programbufferlen=bcpc.len;
    programbuffer=bcpc.pointer;
    return(0);
  } else {
    printf("ERROR: file format not recognized. $%02x $%02x\n",bcpc.pointer[0],bcpc.pointer[1]);
    return(-1);
  }
}


int main(int anzahl, char *argumente[]) {
#ifdef WINDOWS_NATIVE
  hInstance=GetModuleHandle(NULL);
#endif
  param_anzahl=anzahl;
  param_argumente=argumente;

  x11basicStartup();   /* initialize... */

  set_input_mode(1,0);  /* set terminal mode to non-canonical, no echo */
  atexit(reset_input_mode);


 /* if the programm was put 
  * into this binary (xbvm) , try to locate the bytecode 
  * */
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
    fclose(dptr);
   #ifdef ATARI
    memdump(bcpc.pointer,64);
    sleep(5);
   #endif

    if(verbose) printf("%s loaded (%d Bytes)\n",argumente[0],bcpc.len);
    if(fix_bytecode_header((BYTECODE_HEADER *)bcpc.pointer)==0) {
      BYTECODE_HEADER *bytecode=(BYTECODE_HEADER *)bcpc.pointer;
      programbufferlen=bcpc.len=bytecode_make_bss(bytecode,&bcpc.pointer,bcpc.len);
      programbuffer=bcpc.pointer;
      bytecode_init(bcpc.pointer); 
      do_run(); /*set variables etc to zero */
      run_bytecode(bcpc.pointer,bcpc.len);
      return(EX_OK);
    }
    printf("ERROR: Something is wrong, no code!\n");
   #ifdef ATARI
    sleep(5);
   #endif
    return(EX_DATAERR);
  } 
  if(anzahl<2) {
    intro();
    usage();
    return(EX_OK);
  }
  kommandozeile(anzahl, argumente);    /* process command line */
  if(loadfile) {
    if(!exist(ifilename)) {
      printf("ERROR: %s not found !\n",ifilename);
      return(EX_NOINPUT);
    }
    if(loadbcprg(ifilename)) {
      printf("xbvm: could not run %s.\n",ifilename);
      return(EX_CONFIG);
    }
    if(bytecode_init(bcpc.pointer)) {
      do_run(); /*set variables etc to zero */
      run_bytecode(bcpc.pointer,bcpc.len);
    }
  }
  return(EX_OK);
}
