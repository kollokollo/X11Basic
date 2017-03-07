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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif

#if defined WINDOWS || defined ANDROID
#define EX_CANTCREAT	73	/* can't create (user) output file */
#define EX_NOINPUT	66	/* cannot open input */
#define EX_OK 0
#else
#include <sysexits.h>
#endif

#include "defs.h"
#include "x11basic.h"
#include "bytecode.h"
#include "variablen.h"
#include "xbasic.h"
#include "type.h"
#include "parser.h"
#include "parameter.h"
#include "file.h"
#include "io.h"

#ifdef ANDROID
extern char ifilename[];   /* Standard inputfile  */
char ofilename[128]="b.b";       /* Standard outputfile     */
extern int verbose;
extern int dostrip;                   /* dont write symbol tables */
extern BYTECODE_SYMBOL *symtab;
extern int donops;
extern int docomments;
extern int programbufferlen;
extern char *programbuffer;
extern char **program;
extern int prglen;
#else
char ifilename[128]="new.bas";   /* Standard inputfile  */
char ofilename[128]="b.b";       /* Standard outputfile     */
#ifdef ATARI
int verbose=1;
#else
int verbose=0;
#endif
int loadfile=FALSE;
int dostrip=0;                   /* dont write symbol tables */
BYTECODE_SYMBOL *symtab;
int donops=0;
int docomments=0;
int programbufferlen=0;
char *programbuffer=NULL;
char **program=NULL;
int prglen=0;
const char version[]="1.25"; /* Version Number. Put some useful information here */
const char vdate[]="2017-01-01";   /* Creation date.  Put some useful information here */
STRING bcpc;
#endif

#ifdef DUMMY_LIST
/*Kurzform der Error-Routine, tritt hier nur bei IO-Errors auf.*/
void xberror(char errnr, const char *bem) {
  printf("ERROR: #%d %s\n",errnr,bem);
  perror(strerror(errno));
}
#else 
void xberror(char errnr, const char *bem);
#endif


STRING strings;     /* Holds comments and sybol names */
int anzsymbols;
extern char *rodata;
int rodatalen=0;
int bssdatalen=0;

#ifndef O_BINARY
#define O_BINARY 0
#endif
#ifndef S_IRGRP
#define S_IRGRP 0
#endif

int save_bytecode(const char *name,char *adr,int len,char *dadr,int dlen) {
  int fdis=open(name,O_CREAT|O_BINARY|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP);
  if(fdis==-1) return(-1);
  if(verbose) {printf("--> %s [",name);fflush(stdout);}
  BYTECODE_HEADER h;
  bzero(&h,sizeof(BYTECODE_HEADER));
  add_rodata(ifilename,strlen(ifilename));
  
  /* align to even adresses: */
  len=(len+3)&0xfffffffc;
  rodatalen=(rodatalen+3)&0xfffffffc;
  dlen=(dlen+3)&0xfffffffc;
  
  h.BRAs=BC_BRAs;
  h.offs=sizeof(BYTECODE_HEADER)-2;
  h.textseglen=len;
  h.rodataseglen=rodatalen;
  h.sdataseglen=dlen;
  h.dataseglen=0;
  h.bssseglen=bssdatalen;
  if(dostrip) {h.symbolseglen=0;h.stringseglen=0;}
  else {
    h.symbolseglen=anzsymbols*sizeof(BYTECODE_SYMBOL);
    h.stringseglen=strings.len;
    h.stringseglen=(h.stringseglen+3)&0xfffffffc;
  }
  h.version=BC_VERSION;
  h.relseglen=0;

  if(verbose) {
    printf("Info:\n"
           "  Size of   Text-Segment: %d\n"
           "  Size of roData-Segment: %d\n"
           "  Size of   Data-Segment: %d\n",(int)h.textseglen,(int)h.rodataseglen,(int)h.sdataseglen);
    printf("  Size of    bss-Segment: %d\n"
           "  Size of String-Segment: %d\n",(int)h.bssseglen,(int)h.stringseglen);
    printf("  Size of Symbol-Segment: %d (%d symbols)\n",(int)h.symbolseglen,anzsymbols);
  }
  #ifdef ATARI
  WSWAP((char *)&h.version);
  LWSWAP((short *)&h.textseglen);
  LWSWAP((short *)&h.rodataseglen);
  LWSWAP((short *)&h.sdataseglen);
  LWSWAP((short *)&h.dataseglen);
  LWSWAP((short *)&h.bssseglen);
  LWSWAP((short *)&h.symbolseglen);
  LWSWAP((short *)&h.stringseglen);
  LWSWAP((short *)&h.relseglen);
  WSWAP((char *)&h.flags);
  #endif
  if(write(fdis,&h,sizeof(BYTECODE_HEADER))==-1) io_error(errno,"write");
  if(write(fdis,adr,len)==-1) io_error(errno,"write");
  if(rodatalen) {if(write(fdis,rodata,rodatalen)==-1) io_error(errno,"write");}
  if(dlen) {if(write(fdis,dadr,dlen)==-1) io_error(errno,"write");}
  #ifdef ATARI
  LWSWAP((short *)&h.symbolseglen);
  LWSWAP((short *)&h.stringseglen);
  LWSWAP((short *)&h.relseglen);
  #endif
  
  if(write(fdis,strings.pointer,h.stringseglen)==-1) io_error(errno,"write strings");
#ifdef ATARI
  int i;
  int n=h.symbolseglen/sizeof(BYTECODE_SYMBOL);
  if(n>0) {
    for(i=0;i<n;i++) {
        LWSWAP((short *)&symtab[i].name);
        LWSWAP((short *)&symtab[i].adr);
    }
  }
#endif
  if(write(fdis,symtab,h.symbolseglen)==-1) io_error(errno,"write symtab");
  if(verbose) printf("] done.\n");
  return(close(fdis));
}


#ifndef ANDROID
char *rodata;
static void intro(){
  puts("*************************************************************\n"
       "*           X11-Basic bytecode compiler                     *\n"
       "*                    by Markus Hoffmann 1997-2017 (c)       *");
  printf("* V.%s/%04x date:   %30s    *\n",version,BC_VERSION,vdate);
  puts("*************************************************************\n");
}
static void usage(){
  printf("Usage: %s [-o <outputfile> -h] [<filename>] --- compile program [%s]\n\n","bytecode",ifilename);
  printf("-o <outputfile>\t--- put result in file [%s]\n",ofilename);
  puts("-h --help\t--- Usage\n"
       "-c\t\t--- also compile in comments\n"
       "-n\t\t--- also compile in nop's\n"
       "-s\t\t--- strip symbols\n"
       "-v\t\t--- be more verbose");
}


static void kommandozeile(int anzahl, char *argumente[]) {
  int count;

  /* Kommandozeile bearbeiten   */
  for(count=1;count<anzahl;count++) {
    if (strcmp(argumente[count],"-o")==FALSE) {
      strcpy(ofilename,argumente[++count]);
    } else if (strcmp(argumente[count],"-h")==FALSE) {
      intro();
      usage();
    } else if (strcmp(argumente[count],"-n")==FALSE) {
      donops=!donops;
    } else if (strcmp(argumente[count],"-c")==FALSE) {
      docomments=!docomments;
    } else if (strcmp(argumente[count],"-s")==FALSE) dostrip=!dostrip;
    else if (strcmp(argumente[count],"-v")==FALSE) verbose++;
    else if (strcmp(argumente[count],"-q")==FALSE) verbose--;
    else if (strcmp(argumente[count],"--help")==FALSE) {
      intro();usage();
    } else {
      if(!loadfile) {
        loadfile=TRUE;
        strcpy(ifilename,argumente[count]);
      }
    }
   }
}

#ifdef ATARI
#define MAX_CODE 16000
#else
#define MAX_CODE 256000
#endif
int main(int anzahl, char *argumente[]) {
  /* Initialize data segment buffer */
  if(anzahl<2) {    /* Kommandomodus */
    intro();usage();
  } else {
    kommandozeile(anzahl, argumente);    /* Kommandozeile bearbeiten */
    if(loadfile) {
      if(exist(ifilename)) {
        int ret;
	#ifdef ATARI
	intro();
	printf("<-- %s\n",ifilename);
	#endif
        loadprg(ifilename);

    bcpc.pointer=malloc(MAX_CODE);
#ifdef ATARI
    if(bcpc.pointer==NULL) {
      perror("malloc");
      printf("ERROR: Need at least %d Bytes free memory. %d\n",MAX_CODE,malloc(-1));
      sleep(5);
      exit(-1);
    } else printf("Buffer at %p\n",bcpc.pointer);
#endif


	#ifdef ATARI
	printf("Compile ... \n");
	#endif
	compile(verbose);
	// printf("%p LEN=%d\n",bcpc.pointer,bcpc.len);
	ret=save_bytecode(ofilename,(char *)bcpc.pointer,bcpc.len,databuffer,databufferlen);
	if(ret==-1) exit(EX_CANTCREAT);
	#ifdef ATARI
	printf("done.\n");
	#endif
      } else {
        printf("%s: ERROR: %s: file not found!\n",argumente[0],ifilename);
	#ifdef ATARI
	sleep(2);
	#endif
        exit(EX_NOINPUT);
      }
    } else exit(EX_NOINPUT);
    free(bcpc.pointer);
  }
  #ifdef ATARI
  sleep(4);
  #endif
  return(EX_OK);
}
#endif
