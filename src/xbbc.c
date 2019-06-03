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
#define EX_DATAERR      65      /* data format error */
#define EX_OK 0
#else
#include <sysexits.h>
#endif

#include "defs.h"
#include "x11basic.h"
#include "bytecode.h"
#include "variablen.h"
#include "xbasic.h"
#include "memory.h"
#include "type.h"
#include "parser.h"
#include "parameter.h"
#include "file.h"
#include "io.h"

#ifdef ANDROID
extern char ifilename[];   /* Standard inputfile  */
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
static char *ofilename="b.b";       /* Standard outputfile     */
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
  if(verbose) {printf("--> %s [",name);fflush(stdout);}
  int fdis=open(name,O_CREAT|O_BINARY|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP);
  if(fdis==-1) {perror("open");return(-1);}
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
    printf("Info: BC_VERSION=%04x\n"
           "  Size of   Text-Segment: %d\n"
           "  Size of roData-Segment: %d\n"
           "  Size of   Data-Segment: %d\n",h.version,(int)h.textseglen,(int)h.rodataseglen,(int)h.sdataseglen);
    printf("  Size of    bss-Segment: %d\n"
           "  Size of String-Segment: %d\n",(int)h.bssseglen,(int)h.stringseglen);
    printf("  Size of Symbol-Segment: %d (%d symbols)\n",(int)h.symbolseglen,anzsymbols);
  }
  #ifdef IS_BIG_ENDIAN
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
  #ifdef IS_BIG_ENDIAN
  LWSWAP((short *)&h.symbolseglen);
  LWSWAP((short *)&h.stringseglen);
  LWSWAP((short *)&h.relseglen);
  #endif
  
  if(write(fdis,strings.pointer,h.stringseglen)==-1) io_error(errno,"write strings");
#ifdef IS_BIG_ENDIAN
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
  puts("**************************************************\n"
       "*  X11-Basic bytecode compiler                   *\n"
       "*              by Markus Hoffmann 1997-2019 (c)  *");
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


static void kommandozeile(int anzahl, char *argumente[]) {
  /* Kommandozeile bearbeiten   */
  for(int count=1;count<anzahl;count++) {
    if(!strcmp(argumente[count],"-o")) ofilename=argumente[++count];
    else if(!strcmp(argumente[count],"-h")) {
      intro();
      usage();
    } else if(!strcmp(argumente[count],"-n")) donops=!donops;
    else if(!strcmp(argumente[count],"-c"))   docomments=!docomments;
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
        int prgerrors=loadprg(ifilename);
        if(prgerrors<0) {
          printf("%s: ERROR: program %s contains errors. Cannot compile.\n",argumente[0],ifilename);
	  exit(EX_DATAERR);
	} else if(prgerrors) {
          printf("%s: WARNING: program %s contains warnings. Will compile anyway.\n",argumente[0],ifilename);
	}
        bcpc.pointer=malloc(MAX_CODE);
#ifdef ATARI
        if(bcpc.pointer==NULL) {
          perror("malloc");
          printf("ERROR: Need at least %d Bytes free memory. %d\n",MAX_CODE,malloc(-1));
          sleep(5);
          exit(-1);
        } else printf("Buffer at %p\n",bcpc.pointer);
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
