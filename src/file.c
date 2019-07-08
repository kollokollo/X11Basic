/* file.c  (c) Markus Hoffmann   */
/* Extensions for the standard file i/o operations.
 */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "file.h"

#ifndef FALSE
#define FALSE 0
#define TRUE (!FALSE)
#endif

void io_error(int,char *);

/* read a whole line from an open (ASCII) file n 
 * reads until \n or EOF 
 * reads at maximum size bytes. */

char *lineinput(FILE *n, char *line,int size) {
  int c; int i=0;
  while((c=fgetc(n))!=EOF) {
    if(c==(int)'\n') {
      line[i]=(int)'\0';
      return line;
    } else line[i++]=(char)c;
    if(i>=size-1) break;
  }
  line[i]='\0';
  return line;
}

/* reads from an open (ASCII) file n 
 * until comma \n or EOF
 * If parts of the data is encloses in "", 
 * do not stop at commas inside the enclosing.
 * reads at maximum size bytes. */

char *input(FILE *n, char *line,int size) {
  int c; 
  int  i=0,ff=0;
  while((c=fgetc(n))!=EOF) {
    if(c==(int)'\n' || (c==(int)',' && ff==0)) {
      line[i]='\0';
      return line;
    }
    else if(c==(int)'\"') ff=!ff;
    else line[i++]=(char)c;
    if(i>=size-1) break;
  }
  line[i]='\0';
  return line;
}

/* Returns the length of the open file n */

size_t lof(FILE *n) {	
  long position=ftell(n);
  if(position==-1) {
    io_error(errno,"lof");
    return(0);
  }
  if(fseek(n,0,SEEK_END)==0) {
    long laenge=ftell(n);
    if(laenge<0) io_error(errno,"ftell");
    if(fseek(n,position,0)<0) io_error(errno,"fseek"); 
    return(laenge);
  } else io_error(errno,"fseek");
  return(0);
}


/* Returns the eof condition of an open file n */

int myeof(FILE *n) {
  int c=fgetc(n);
  ungetc(c,n);
  return c==EOF;
}
#ifndef O_BINARY
#define O_BINARY 0
#endif
#ifndef S_IRGRP
#define S_IRGRP 0
#endif


/* Saves an area in memory starting at adr with length len to a file
   with filename name.
   RETURNS: 0 on success and -1 on error */

int bsave(const char *name, char *adr, size_t len) { 
  int fdis=open(name,O_CREAT|O_BINARY|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR|S_IRGRP);
  if(fdis==-1) return(-1);
  if(write(fdis,adr,len)==-1) {close(fdis); return(-1);}
  return(close(fdis));
}

/* loads the file with filename name to a memory location at address adr.
   a maximum of len bytes are read. if len==-1 the whole file is read. 
   RETURNS the number of read bytes or 0 on error.
   */

size_t bload(const char *name, char *adr, size_t len) {	
  FILE *fdis=fopen(name,"rb");
  if(fdis==NULL) return(0);
  if(len==-1) len=lof(fdis);
  if(len>0) len=fread(adr,1,len,fdis);
  fclose(fdis);
  return(len);
}

/* Checks if a give file name exists */

#if 0
int exist(const char *name ) {	/* This is a safe but slow implementation */
  int fdis=open(name,0x8000);
  if (fdis==-1) return(FALSE);
  close(fdis);
  return(TRUE);
}
#else
int exist(const char *filename) {
  struct stat fstats;
  int retc=stat(filename, &fstats);
  if(retc==-1) return(FALSE);
  return(TRUE);
}
#endif

int stat_device(const char *filename) {
  struct stat fstats;
  int retc=stat(filename, &fstats);
  if(retc==-1) io_error(errno,"stat");
  return(fstats.st_dev);  
}
int stat_inode(const char *filename) {
  struct stat fstats;
  int retc=stat(filename, &fstats);
  if(retc==-1) io_error(errno,"stat");
  return(fstats.st_ino);  
}
int stat_mode(const char *filename) {
  struct stat fstats;
  int retc=stat(filename, &fstats);
  if(retc==-1) io_error(errno,"stat");
  return(fstats.st_mode);  
}
int stat_nlink(const char *filename) {
  struct stat fstats;
  int retc=stat(filename, &fstats);
  if(retc==-1) io_error(errno,"stat");
  return(fstats.st_nlink);  
}
int stat_uid(const char *filename) {
  struct stat fstats;
  int retc=stat(filename, &fstats);
  if(retc==-1) io_error(errno,"stat");
  return(fstats.st_uid);  
}
int stat_gid(const char *filename) {
  struct stat fstats;
  int retc=stat(filename, &fstats);
  if(retc==-1) io_error(errno,"stat");
  return(fstats.st_gid);  
}
int stat_size(const char *filename) {
  struct stat fstats;
  int retc=stat(filename, &fstats);
  if(retc==-1) io_error(errno,"stat");
  return(fstats.st_size);  
}
