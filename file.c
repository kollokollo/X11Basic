/* Erweiterungen fuer die Datei Ein- und Ausgabe ....   */
/* (c) von Markus Hoffmann                                  */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#define FALSE 0
#define TRUE (!FALSE)


char *lineinput( FILE *n, char *line)    /* liest eine ganze Zeile aus einem ASCII-File ein */
{ char c; int i;
 
  i=0;
  while((c=fgetc(n))!=EOF)
    {
      if(c=='\n')
	{
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

long lof( FILE *n)
{	long laenge,position;
	
	position=ftell(n);
	if(fseek(n,0,2)==0){
		laenge=ftell(n);
		if(fseek(n,position,0)==0){
			return(laenge);
		} else {
			return(-1);
		}
	} else {
		return(-1);
	}
}


int eof(FILE *n) {
  char c=fgetc(n);
  ungetc(c,n);
  return c==EOF;
}

int bsave( char *name, char *adr, long len)
{ int   fdis;


	fdis=creat(name,0644);
	if (fdis==-1) return(fdis);
	
	while(len>32000) {
		write(fdis,adr,32000);
		len-=32000; adr+=32000;
	}
	write(fdis,adr,len);
	
	
	close(fdis);
	return(0);
}

long bload( char *name, char *adr, long len)
{	FILE *fdis;
	long gelesen=0;
	
	fdis=fopen(name,"r");
	if (fdis==NULL) return(0);
        if(len==-1) len=lof(fdis);
        if(len>0) gelesen=fread(adr,1,len,fdis);
	fclose(fdis);
	return(gelesen);
}

int exist( char *name )

{	int   fdis;
		
	fdis=open(name,0x8000);
	if (fdis==-1) return(FALSE);
	close(fdis);
	return(TRUE);
}


