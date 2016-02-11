/* io.c I/O_Routinen  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>

#include "file.h"
#include "xbasic.h"

void io_error(int,char *);

/*******************************/
/* Ein- und Ausgaberoutinen    */


void c_print(char *n) {
  char v[strlen(n)+1];
  char c;
  FILE *fff;
  int i;
  fff=stdout;
  strcpy(v,n);
   if(v[0]=='#') {
       char *buffer=malloc(strlen(v)+1);
       wort_sep(v,',',TRUE,buffer,v);
       i=get_number(buffer);
       if(filenr[i]) fff=dptr[i];
       else error(24,""); /* File nicht geoeffnet */
       free(buffer);
    }
  if(strlen(v)) {
      if(v[strlen(v)-1]==';' || v[strlen(v)-1]==',' || v[strlen(v)-1]=='\'') {
        char *buffer;
        c=v[strlen(v)-1];
        v[strlen(v)-1]=0;
        buffer=print_arg(v);
        fprintf(fff,"%s",buffer);
        if(c=='\'') fprintf(fff," ");
        else if(c==',') fprintf(fff,"\011");
	free(buffer);
      } else {
        char *buffer=print_arg(v);
        fprintf(fff,"%s\n",buffer);
	free(buffer);
      }
    } else fprintf(fff,"\n");
}

void c_input(char *n) {
  char s[strlen(n)+1],t[strlen(n)+1];
  char *u,*text;
  char inbuf[MAXSTRLEN];
  int e;
  FILE *fff=stdin;
  
  if(n[0]=='#') {
    char *buffer=malloc(strlen(n)+1);
    int i;
    
    wort_sep(n,',',TRUE,buffer,s);
    i=get_number(buffer);
    if(filenr[i]) fff=dptr[i];
    else error(24,""); /* File nicht geoeffnet */
    free(buffer);
  } else strcpy(s,n);
  if(strlen(s)) {
   
    e=arg2(s,TRUE,s,t);
    if(s[0]=='\"') {
      u=s_parser(s);
      text=malloc(strlen(u)+4);
      strcpy(text,u);
      free(u);
      if(e==3) strcat(text," ? ");
      else if(e!=2) strcat(text," ");
      e=arg2(t,TRUE,s,t);
    } else {
      text=malloc(3);
      strcpy(text,"? ");
    }
        
    while(e!=0) {
      xtrim(s,TRUE,s);
      if(fff==stdin) u=do_gets(text);
      else {
        u=input(fff,inbuf);
      }
      if(type2(s) & STRINGTYP) zuweiss(s,u);
      else zuweis(s,parser(u));
      e=arg2(t,TRUE,s,t);
    }
    free(text);
  }
  
}
char *lineinputs(char *n) {
  int i=get_number(n);
  char *inbuf;
  if(filenr[i]) {
    FILE *fff=dptr[i];
    inbuf=malloc(MAXSTRLEN);
    lineinput(fff,inbuf);
  } else {
    inbuf=malloc(10);
    strcpy(inbuf,"<ERROR>");
    error(24,""); /* File nicht geoeffnet */
  }
  return(inbuf);
}
char *inputs(char *n) {
  char s[strlen(n)+1],t[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,s,t);
  char *inbuf;
  if(e==2) {
    int i=get_number(s);
    int anz=(int)parser(t);
    FILE *fff=stdin;
    if(filenr[i]) {
      fff=dptr[i];
      inbuf=malloc(anz+1);
      fread(inbuf,1,anz,fff);
      inbuf[anz]=0;
    } else {
      inbuf=malloc(10);
      strcpy(inbuf,"<ERROR>");
      error(24,""); /* File nicht geoeffnet */
    }
  } else {
    inbuf=malloc(10);
    strcpy(inbuf,"<ERROR>");
    error(32,"INPUT$"); /* Syntax Error */
  }
  return(inbuf);
}
void c_lineinput(char *n) {
  char s[strlen(n)+1],t[strlen(n)+1];
  char *u,*text;
  char inbuf[MAXSTRLEN];
  int e;
  FILE *fff=stdin;
  if(n[0]=='#') {
    char *buffer=malloc(strlen(n)+1);
    int i;
    
    wort_sep(n,',',TRUE,buffer,s);
    i=get_number(buffer);
    if(filenr[i]) fff=dptr[i];
    else error(24,""); /* File nicht geoeffnet */
    free(buffer);
  } else strcpy(s,n);
  if(strlen(s)) {
    e=arg2(s,TRUE,s,t);
    if(s[0]=='\"') {
      u=s_parser(s);
      text=malloc(strlen(u)+4);
      strcpy(text,u);
      free(u);
      if(e==3) strcat(text," ? ");
      if(e!=2) strcat(text," ");
      e=arg2(t,TRUE,s,t);
    } else {
      text=malloc(3);
      strcpy(text,"? ");
    }
        
    while(e!=0) {
      xtrim(s,TRUE,s);
      if(fff==stdin) u=do_gets(text);
      else {
        u=lineinput(fff,inbuf);
      }
      if(type2(s) & STRINGTYP) zuweiss(s,u);
      else zuweis(s,parser(u));
      e=arg2(t,TRUE,s,t);
    }
    free(text);
  }
}
/********************/
/* File-Routinen    */

int get_number(char *w) {
    if(w[0]=='#') w++;
    return((int)parser(w));
}

void c_open(char *w) {
  char modus;
  char v[strlen(w)+1],t[strlen(w)+1];
  int number,e;
  char *buffer,modus2[5];
      
  e=wort_sep(w,',',TRUE,v,t);
  if(e) {
     char *mod;
     /* Modus */
     mod=s_parser(v);
     modus=toupper(mod[0]);
     free(mod);
  }
  e=wort_sep(t,',',TRUE,v,t);
  if(e) number=get_number(v);    /* Nummer */
  
  e=wort_sep(t,',',TRUE,v,t);
  if(e) {
    buffer=s_parser(v);   /* Filenamen */

    if(filenr[number]) error(22,"");  /* File schon geoeffnet  */
    else if(number>99 || number<1) error(23,"");  /* File # falsch  */
    else {
      if(modus=='I') strcpy(modus2,"r");
      else if(modus=='O') strcpy(modus2,"w");
      else if(modus=='U') strcpy(modus2,"r+");
      else if(modus=='A') strcpy(modus2,"a+");
      else error(21,""); /* bei Open nur erlaubt ...*/
    
      dptr[number]=fopen(buffer,modus2);
      if(dptr[number]==NULL) io_error(errno,"OPEN");
      else  filenr[number]=1;
    
    }
    free(buffer);
  } else error(32,"OPEN"); /*Sytax error*/
}

void io_error(int n, char *s) {
  switch(n) {
  case EACCES:
  case ENOTDIR:
  case ELOOP:
  case ENXIO:
    error(-36,s); /* Zugriff nicht m"oglich */
    break;
  case ENOENT:
    error(-33,s); /* File not found */
    break;
  case EMFILE:
    error(-35,s); /* Zu viele Dateien offen */
    break;
  case ENFILE:
    error(-12,s); /* Zur Zeit sind keine weiteren offenen Files möglich */
    break;
  case EFBIG:
    error(-13,s); /* File zu gross */
    break;
  case EBADF:
    error(-37,s); /* Ungültiges Handle */
    break;
  
  case ENOSPC:
    error(-28,s); /* No Space left on device */
    break;
  case EROFS:
    error(-30,s); /* FILE SYSTEM SCHREIBGESCHUetzt */
    break;
  case EIO:
  default:      
    printf("errno=%d\n",n);
    error(-1,s);  /* Allgemeiner IO-Fehler */
  }
}


void c_close(char *w) {
  int i;

  if(strlen(w)) {
    i=get_number(w);
    if(filenr[i]) { 
      if(fclose(dptr[i])==EOF) io_error(errno,"CLOSE");
      else filenr[i]=0;
    } else error(24,w); /* File nicht geoeffnet...*/

  } else {
    printf("CLOSE: alles [");
    for(i=0;i<100;i++) {
	if(filenr[i]) {
	  printf("#%d ",i);
          if(fclose(dptr[i])==EOF) io_error(errno,"CLOSE");
	  else filenr[i]=0;
        }
    }
    printf("]\n");
  }
}

void c_bload(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  char *filename;
  int e=wort_sep(n,',',TRUE,w1,w2);
  int g,len=-1,a;
  if(e<2) printf("BLOAD: Zuwenig Parameter !\n");
  else {
    
    filename=s_parser(w1); 
    e=wort_sep(w2,',',TRUE,w1,w2);
    if(e==2) len=(int)parser(w2);
    g=bload(filename,(char *)(int)parser(w1),len);
    free(filename);
    if(g==-1) io_error(errno,"BLOAD");
  }
}
void c_bsave(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  char *filename;
  int e=wort_sep(n,',',TRUE,w1,w2);
  int g,len,a;
  if(e<2) printf("BSAVE: Zuwenig Parameter !\n");
  else {
    filename=s_parser(w1); 
    e=wort_sep(w2,',',TRUE,w1,w2);
    if(e!=2) printf("BSAVE: Zuwenig Parameter !\n");
    else {
      len=(int)parser(w2);
      g=bsave(filename,(char *)(int)parser(w1),len);
      if(g==-1) io_error(errno,"BSAVE");
    }
    free(filename);
  }
}
void c_bget(char *n) {

}
void c_bput(char *n) {

}
void c_bmove(char *n) {

}
void c_unget(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  int i,e=wort_sep(n,',',TRUE,v,w);
  FILE *fff=stdin;
  if(e>1) {
    i=get_number(v);
    if(filenr[i]) fff=dptr[i];      
    else {error(24,v);return;} /* File nicht geoeffnet */
    i=(int)parser(w);
  } else if(e==1) i=(int)parser(v);
  else {error(32,"PUTBACK");return;} /* Syntax error */
  ungetc(i,fff);
}

void c_flush(char *n) {
  FILE *fff=stdout;
  int i;
  if(strlen(n)) {
    i=get_number(n);
    if(filenr[i]) fff=dptr[i];      
    else {error(24,n);return;} /* File nicht geoeffnet */
  }
  fflush(fff);
}

void c_seek(char *n) {
 char v[strlen(n)+1],w[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,v,w);
  if(e>1) {
    int i=get_number(v);
    FILE *fff=NULL;

    if(filenr[i]) {
      fff=dptr[i];
      wort_sep(w,',',TRUE,v,w);
      if(fseek(fff,(int)parser(v),SEEK_SET)) io_error(errno,"SEEK");
    } else error(24,""); /* File nicht geoeffnet */
  } else error(32,"SEEK"); /* Syntax error */
}
void c_relseek(char *n) {
 char v[strlen(n)+1],w[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,v,w);
  if(e>1) {
    int i=get_number(v);
    FILE *fff=NULL;

    if(filenr[i]) {
      fff=dptr[i];
      wort_sep(w,',',TRUE,v,w);
      if(fseek(fff,(int)parser(v),SEEK_CUR)) io_error(errno,"RELSEEK");
    } else error(24,""); /* File nicht geoeffnet */
  } else error(32,"RELSEEK"); /* Syntax error */
}
char inp8(char *n) {
  int i=get_number(n);
  char ergebnis;
  FILE *fff;
  if(i==-2) fff=stdin;  
  else if(filenr[i]) fff=dptr[i];
  else {error(24,"");return(-1);} /* File nicht geoeffnet */
  fread(&ergebnis,sizeof(char),1,fff);
  return(ergebnis);
}
int inpf(char *n) {
  int i=get_number(n);
  FILE *fff=stdin;
  if(i==-2) {
    return(kbhit() ? -1 : 0);
  } else if(filenr[i]) {
    fff=dptr[i];
    return(((eof(fff)) ? 0 : -1));
  } else {error(24,"");return(-1);} /* File nicht geoeffnet */
}
short inp16(char *n) {
  int i=get_number(n);
  short ergebnis;
  FILE *fff=stdin;

  if(filenr[i]) {
    fff=dptr[i];
    fread(&ergebnis,sizeof(short),1,fff);
    return(ergebnis);
  } else error(24,""); /* File nicht geoeffnet */
  return(-1);
}
short inp32(char *n) {
  int i=get_number(n);
  unsigned int ergebnis;
  FILE *fff=stdin;

  if(filenr[i]) {
    fff=dptr[i];
    fread(&ergebnis,sizeof(int),1,fff);
    return(ergebnis);
  } else error(24,""); /* File nicht geoeffnet */
  return(-1);
}

/* Terminal-Einstellungen. Wichtig fuer die Zeichenweise eingabe */

struct termios saved_attributes;  /* fuer alte Einstellung */
				
void set_input_mode(int mi, int ti) {
  struct termios tattr;
  if(isatty(STDIN_FILENO)) {
    tcgetattr(STDIN_FILENO,&saved_attributes);
    tcgetattr(STDIN_FILENO,&tattr);
    tattr.c_lflag &= ~(ICANON|ECHO);
    tattr.c_cc[VMIN]=mi;
    tattr.c_cc[VTIME]=ti;
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&tattr);
  }

}
void set_input_mode_echo(int onoff) {
  struct termios tattr;
  if(isatty(STDIN_FILENO)) {
    tcgetattr(STDIN_FILENO,&tattr);
    if(onoff) tattr.c_lflag |= ECHO;
    else tattr.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&tattr);
  }

}
void reset_input_mode() {
  if(isatty(STDIN_FILENO)) tcsetattr(STDIN_FILENO,TCSANOW,&saved_attributes);
}
char *terminalname() {
  char *name=NULL,*erg;
  if(isatty(STDIN_FILENO)) name=ttyname(STDIN_FILENO);
  erg=malloc(strlen(name)+1);
  strcpy(erg,name);
  return(erg);
}

void c_out(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,v,w);
  if(e>1) {
    int i=get_number(v);
    int typ,vnr,j,a=1;
    char *r;
    FILE *fff=stdin;

    if(filenr[i]) {
      fff=dptr[i];
      e=wort_sep(w,',',TRUE,v,w);
      while(e) {
        typ=type2(v);
	if(typ & ARRAYTYP) {
	  ARRAY *zzz;
	  zzz=array_parser(v);    
	  for(j=0;j<zzz->dimension;j++) a=a*(((int *)zzz->pointer)[j]);

	  if(zzz->typ & FLOATTYP) {
            double *varptr=(double *)(zzz->pointer+zzz->dimension*INTSIZE);
            
	    fwrite(varptr,sizeof(double),a,fff);
	  } else if(zzz->typ & INTTYP) {
	    int *varptr=(int *)(zzz->pointer+zzz->dimension*INTSIZE);
	    fwrite(varptr,sizeof(int),a,fff);
	  } else if(zzz->typ & STRINGTYP) {
            STRING *varptr=(STRING *)(zzz->pointer+zzz->dimension*INTSIZE);
	    for(j=0;j<a;j++) {
	      fwrite(varptr[j].pointer,sizeof(char),varptr[j].len,fff);
	    }
	  }
	 
	  free_array(zzz);
	} else if(typ & FLOATTYP){
	 double zzz;
	 zzz=parser(v);
         fwrite(&zzz,sizeof(double),1,fff);
	} else if(typ & INTTYP){
	 int zzz;
	 zzz=(int)parser(v);
         fwrite(&zzz,sizeof(int),1,fff);
	} else if(typ & STRINGTYP){
	 char *zzz;
	 zzz=s_parser(v);
         fwrite(&zzz,sizeof(char),strlen(zzz),fff);
         free(zzz);
        }
	e=wort_sep(w,',',TRUE,v,w);
      }  
    } else error(24,""); /* File nicht geoeffnet */
  } else error(32,"OUT"); /* Syntax error */
}

/* kbhit-Funktion   */
int kbhit() {
  fd_set set;
/*  
#ifdef TIMEVAL_WORKAROUND
  struct {
               int  tv_sec; 
               int  tv_usec;   
       } tv;
#else */
    struct timeval tv;
/*#endif*/
   /* memset(&tv, 0, sizeof(tv));  */   
    tv.tv_sec=0; tv.tv_usec=0;
    FD_ZERO(&set);
    FD_SET(0, &set);

    select(1, &set, 0, 0, &tv);

    if (FD_ISSET(0, &set))      return(-1);
    else  return(0);
}


char *inkey() {
 static char ik[MAXSTRLEN];
 int i=0;
 while(kbhit()) ik[i++]=getc(stdin);
 ik[i]=0;
 return(ik);
}
