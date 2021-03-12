/* XBBC.C   The X11-basic bytecode compiler         (c) Markus Hoffmann
*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif

#if defined WINDOWS || defined ANDROID
#define EX_CANTCREAT	73	/* can't create (user) output file */
#define EX_NOINPUT	66	/* cannot open input */
#define EX_DATAERR      65      /* data format error */
#define EX_OK 0
#else
#include <sysexits.h>
#endif

#include "defs.h"
#include "x11basic.h"
#include "bytecode.h"

char ifilename[128]="new.bas";   /* default input file name */
static char *ofilename="b.b";    /* default output file name */
#ifdef ATARI
int compile_flags=COMPILE_DEFAULT|COMPILE_VERBOSE;
int verbose=1;
#else
int compile_flags=COMPILE_DEFAULT;
int verbose=0;
#endif
int loadfile=FALSE;
int dostrip=0;                   /* dont write symbol tables */
int programbufferlen=0;
char *programbuffer=NULL;
char **program=NULL;
int prglen=0;


static void intro(){
  puts("**************************************************\n"
       "*  X11-Basic bytecode compiler                   *\n"
       "*              by Markus Hoffmann 1997-2021 (c)  *");
  printf("* V." VERSION "/%04x      date:    " __DATE__ " " __TIME__ " *\n",BC_VERSION);
  puts("**************************************************\n");
}
static void usage(){
  printf("Usage:\n%s [-o <outputfile> -h] [<filename>]\t--- compile program\t[%s]\n\n","xbbc",ifilename);
  printf("-o <outputfile>\t--- put result in file\t[%s]\n",ofilename);
  puts("-h --help\t--- Usage\n"
       "-c\t\t--- also compile in comments\n"
       "-n\t\t--- also compile in nop's\n"
       "-s\t\t--- strip symbols\n"
       "-v\t\t--- be more verbose");
}


/* process command line  */

static void kommandozeile(int anzahl, char *argumente[]) {
  for(int count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-o")) ofilename=argumente[++count];
    else if(!strcmp(argumente[count],"-h")) {
      intro();usage();
    } else if(!strcmp(argumente[count],"-n")) compile_flags=compile_flags|COMPILE_NOOPS;
    else if(!strcmp(argumente[count],"-c"))   compile_flags=compile_flags|COMPILE_COMMENTS;
    else if(!strcmp(argumente[count],"-s"))   dostrip=!dostrip;
    else if(!strcmp(argumente[count],"-v"))   verbose++;
    else if(!strcmp(argumente[count],"-q"))   verbose--;
    else if(!strcmp(argumente[count],"--help")) {
      intro();usage();
    } else if(*(argumente[count])=='-') printf("%s: Unknown option %s ignored.\n",argumente[0],argumente[count]); 
    else {
      if(!loadfile) {
        loadfile=TRUE;
        strcpy(ifilename,argumente[count]);
      }
    }
  }
}

int main(int anzahl, char *argumente[]) {
  /* Initialize data segment buffer */
  if(anzahl<2) { /* Nothing to do, just display intro ... */
    intro();usage();
  } else {
    kommandozeile(anzahl, argumente);    /* process command line */
    if(loadfile) {
      if(exist(ifilename)) {
        libx11basic_init();  /* prepare everything, so that loadprg() can work. */
	#ifdef ATARI
	intro();
	printf("<-- %s\n",ifilename);
	#endif
        int prgerrors=loadprg(ifilename);
        if(prgerrors<0) {
          printf("%s: ERROR: program %s contains errors. Cannot compile.\n",argumente[0],ifilename);
	  exit(EX_DATAERR);
	} else if(prgerrors) {
          printf("%s: WARNING: program %s contains warnings. Will compile anyway.\n",argumente[0],ifilename);
	}
	
	/* Initialize compile block: */
	
	COMPILE_BLOCK cb;
	bzero(&cb,sizeof cb);
	if(verbose>0) compile_flags|=COMPILE_VERBOSE;
#ifdef ATARI
	printf("Compile ... \n");
#endif
	if(compile(&cb,compile_flags)<0) {
  	  printf("ERROR: compilation failed. Saving the result anyways.\n");
	}
	int ret=save_bytecode(ofilename,&cb,dostrip);
	if(ret==-1) exit(EX_CANTCREAT);
	#ifdef ATARI
	printf("done.\n");
	#endif
	free_cb(&cb);
      } else {
        printf("%s: ERROR: %s: file not found!\n",argumente[0],ifilename);
	#ifdef ATARI
	sleep(2);
	#endif
        exit(EX_NOINPUT);
      }
    } else exit(EX_NOINPUT);
  }
#ifdef ATARI
  sleep(4);
#endif
  return(EX_OK);
}
