/* Erweiterungen fuer die Datei Ein- und Ausgabe ....   */

/* (c) von Markus Hoffmann                              */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif

char *lineinput( FILE *n, char *line) {   /* liest eine ganze Zeile aus einem ASCII-File ein */
  char c; int i=0;
 
  while((c=fgetc(n))!=EOF) {
      if(c=='\n') {
	  line[i]='\0';
	  return line;
	}
	else line[i++]=c;
    }
    line[i]='\0';
    return line;
}
char *input( FILE *n, char *line)    /* liest bis Komma oder Zeilenende einem ASCII-File ein */
{ char c; 
  int  i=0,ff=0;
  while((c=fgetc(n))!=EOF) {
      if(c=='\n' || (c==',' && ff==0)) {
	  line[i]='\0';
	  return line;
	}
	else if(c=='\"') ff=!ff;
	else line[i++]=c;
    }
    line[i]='\0';
    return line;
}

long lof( FILE *n) {	
  long laenge,position;
	
  position=ftell(n);
  if(fseek(n,0,2)==0){
    laenge=ftell(n);
    if(fseek(n,position,0)==0)  return(laenge);
  }
  return(-1);
}


int myeof(FILE *n) {
  char c=fgetc(n);
  ungetc(c,n);
  return c==EOF;
}

int bsave( char *name, char *adr, long len) { 
  int fdis=creat(name,0644);
  if(fdis==-1) return(-1);
  if(write(fdis,adr,len)==-1) io_error(errno,"write");
  return(close(fdis));
}

long bload( char *name, char *adr, long len) {	
  FILE *fdis;
  long gelesen=0;
	
	fdis=fopen(name,"r");
	if (fdis==NULL) return(0);
        if(len==-1) len=lof(fdis);
        if(len>0) gelesen=fread(adr,1,len,fdis);
	fclose(fdis);
	return(gelesen);
}
#if 0
int exist( char *name ) {	
  int   fdis=open(name,0x8000);
  if (fdis==-1) return(FALSE);
  close(fdis);
  return(TRUE);
}
#else
int exist(char *filename) {
  struct stat fstats;
  int retc=stat(filename, &fstats);
  if(retc==-1) return(FALSE);
  return(TRUE);
}
#endif
