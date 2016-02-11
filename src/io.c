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
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
/* fuer Dynamisches Linken von shared Object-Files   */
#include <dlfcn.h>

#include "file.h"
#include "defs.h"
#include "globals.h"
#include "protos.h"


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
STRING f_lineinputs(char *n) {
  int i=get_number(n);
  STRING inbuf;
  if(filenr[i]) {
    FILE *fff=dptr[i];
    inbuf.pointer=malloc(MAXSTRLEN);
    lineinput(fff,inbuf.pointer);
  } else {
    inbuf.pointer=malloc(10);
    strcpy(inbuf.pointer,"<ERROR>");
    error(24,""); /* File nicht geoeffnet */
  }
  inbuf.len=strlen(inbuf.pointer);
  return(inbuf);
}
STRING f_inputs(char *n) {
  char s[strlen(n)+1],t[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,s,t);
  STRING inbuf;
  if(e==2) {
    int i=get_number(s);
    int anz=(int)parser(t);
    FILE *fff=stdin;
    if(filenr[i]) {
      fff=dptr[i];
      inbuf.pointer=malloc(anz+1);
      inbuf.len=(int)fread(inbuf.pointer,1,anz,fff);
      inbuf.pointer[anz]=0;
      return(inbuf);
    } else error(24,""); /* File nicht geoeffnet */
  } else error(32,"INPUT$"); /* Syntax Error */
  inbuf.pointer=malloc(10);
  strcpy(inbuf.pointer,"<ERROR>");  
  inbuf.len=strlen(inbuf.pointer);
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

/* Internetroutinen */

int init_sockaddr(struct sockaddr_in *name,const char *hostname, unsigned short int port) {
  struct hostent *hostinfo;
  name->sin_family=AF_INET;
  name->sin_port=htons(port);
  hostinfo=gethostbyname(hostname);
  if(hostinfo==NULL) return(-1);
  name->sin_addr=*(struct in_addr *)hostinfo->h_addr;
  return(0);
}

int make_socket(unsigned short int port) {
  int sock;
  struct sockaddr_in name;
  sock=socket(PF_INET, SOCK_STREAM,0);
  if(sock<0) return(-1);
  name.sin_family=AF_INET;
  name.sin_port=htons(port);
  name.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(sock,(struct sockaddr *) &name, sizeof(name))<0) return(-1);
  return(sock);
}

/* Universelle OPEN-Funktion. Oeffnet Files, Devices, und sockets   */
/* OPEN "I",#1,filename$[,port]   */


void c_open(char *n) {
  char modus,special=0;
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int number=1,e,port=5555,i=0;
  char *filename=NULL;
  char modus2[5]="r";
  
  e=wort_sep(n,',',TRUE,w1,w2);
  while(e) {
     if(strlen(w1)) {
       switch(i) {
         case 0: { /* Modus */     
	   char *mod=s_parser(w1);      /* Modus */
           modus=toupper(mod[0]);
	   special=toupper(mod[1]);
           free(mod);
           if(modus=='I') strcpy(modus2,"r");
           else if(modus=='O') strcpy(modus2,"w");
           else if(modus=='U') strcpy(modus2,"r+");
           else if(modus=='A') strcpy(modus2,"a+");
           else error(21,""); /* bei Open nur erlaubt ...*/
	   break;
	 }
	 case 1: {  number=get_number(w1);    /* Nummer */ break; } 
	 case 2: { filename=s_parser(w1); break; } 
	 case 3: { port=(int)parser(w1); break; } 	   
         default: break;
       }
     }
     i++;
     e=wort_sep(w2,',',TRUE,w1,w2);
  }
  
  if(filenr[number]) error(22,"");  /* File schon geoeffnet  */
  else if(number>99 || number<1) error(23,"");  /* File # falsch  */
  else {

      
      /* Hier koennte man Named Pipes einbauen .... */

      /*  Sockets  */
      if(special=='C') { /* Connect */
        int sock;
	struct sockaddr_in servername;
	 
        printf("Open Socket #%d: modus=%s adr=%s port=%d\n",number,modus2,filename, port);
        sock=socket(PF_INET, SOCK_STREAM,0);
	if(sock<0) {
	  printf("ERROR: socket\n");
	  dptr[number]=NULL;
	} else {
	  if(init_sockaddr(&servername,filename,port)<0) {
	    printf("ERROR: init\n");
	    dptr[number]=NULL;
	  } else { 
	    if(0>connect(sock,(struct sockaddr *) &servername, sizeof(servername))) {
              printf("ERROR: connect\n");
	      dptr[number]=NULL;
	    } else {
              dptr[number]=fdopen(sock,modus2);
	    }
	  }
	}
      } else if(special=='S') { /* serve */
        int sock;
        printf("Create Socket #%d: modus=%s adr=%s port=%d\n",number,modus2,filename, port);
        sock=make_socket(port);
	if(sock<0) {
	  printf("ERROR: make_socket\n");
	  dptr[number]=NULL;
	} else {
	  if(listen(sock,1)<0) {
	    printf("ERROR: listen\n");
	    dptr[number]=NULL;
	  } else { 
	    
              dptr[number]=fdopen(sock,modus2);
	    
	  }
	}
 

      } else if(special=='A') { /* accept */
        int sock,sock2;
	size_t size;	
	struct sockaddr_in clientname;
#ifdef DEBUG
        printf("Accept Socket #%d: modus=%s adr=%s port=%d\n",number,modus2,filename, port);
#endif
        if(filenr[port]) {
	  sock=fileno(dptr[port]);
	  size=sizeof(clientname);
	  sock2=accept(sock,(struct sockaddr *) &clientname,&size);
	  if(sock2<0) {
	    printf("ERROR: accept\n");
	    dptr[number]=NULL;
	  } else {
#ifdef DEBUG	   
	    printf("Verbindung von %s auf Port $%hd an %d\n",inet_ntoa(clientname.sin_addr), 
	      ntohs(clientname.sin_port),sock2);
#endif
            dptr[number]=fdopen(sock2,modus2);
	    
	  }
	} else printf("Socket #d nicht geoeffnet.\n",port); 
      } else dptr[number]=fopen(filename,modus2);
      if(dptr[number]==NULL) io_error(errno,"OPEN");
      else  filenr[number]=1;
  }
  free(filename);
}

void c_link(PARAMETER *plist, int e) {
  int number;
  if(e==2) {
    number=plist[0].integer;
    if(filenr[number]) error(22,"");  /* File schon geoeffnet  */
    else {
      dptr[number]=dlopen(plist[1].pointer,RTLD_LAZY);
      if(dptr[number]==NULL) io_error(errno,"LINK");
      else  filenr[number]=2;
    } 
  }
}

const struct {int sf; char xf; } table[] = {
    { EACCES,  -36 }, /* Zugriff nicht m"oglich */
    { ENOTDIR, -36 }, 
    { ELOOP,   -36 }, 
    { ENXIO,   -36 }, 
    { ENOENT,  -33 }, /* File not found */
    { EMFILE,  -35 }, /* Zu viele Dateien offen */
    { ENFILE,  -12 }, /* Zur Zeit sind keine weiteren offenen Files möglich */
    { EFBIG,   -13 }, /* File zu gross */
    { EBADF,   -37 }, /* Ungültiges Handle */
    { ENOTSOCK,-37 },
    { EINVAL,  -37 },
    { ETIMEDOUT,-2 }, /* Timeout */
    { ECONNREFUSED, -48 }, /* Verbindungsaufbau verweigert */
    { EISCONN, -47 }, /* Verbindung schon geöffnet */
    { ENOTCONN,-45 }, /* keine Verbindung */
    { EADDRINUSE,-44 },/* Besetzt, Verbindung nicht moeglich */
    { EADDRNOTAVAIL,-43 },/* Verbindungsaufbau nicht moeglich */
    { ENETUNREACH,-43 },
    { ENOSPC,  -28 }, /* No Space left on device */
    { EOPNOTSUPP,-32 }, /* Ungueltige Funktionsnummer */
    { EPIPE,   -50 }, /* Verbindung wurde unterbrochen */
    { ENOBUFS, -23 }, /* File Table overflow */
    { EROFS,   -30 }, /* FILE SYSTEM SCHREIBGESCHUetzt */
    { EIO,      -1 } /* Allgemeiner IO-Fehler */
  };
const int anztabs=sizeof(table)/sizeof(struct {int sf; char xf; });
  
void io_error(int n, char *s) {
  int i;
  for(i=0;i<anztabs;i++) {
    if(n==table[i].sf) {
      error(table[i].xf,s);
      return;    
    }
  }
  printf("errno=%d\n",n);
  error(-1,s);  /* Allgemeiner IO-Fehler */
}


void c_close(char *w) {
  int i;

  if(strlen(w)) {
    i=get_number(w);
    if(filenr[i]==1) { 
      if(fclose(dptr[i])==EOF) io_error(errno,"CLOSE");
      else filenr[i]=0;
    } else if(filenr[i]==2) { 
      if(dlclose(dptr[i])==EOF) io_error(errno,"UNLINK");
      else filenr[i]=0;
    }
    else error(24,w); /* File nicht geoeffnet...*/
  } else {
    for(i=0;i<100;i++) {
	if(filenr[i]==1) {
          if(fclose(dptr[i])==EOF) io_error(errno,"CLOSE");
	  else filenr[i]=0;
        }
    }
  }
}


void c_bload(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  char *filename;
  int e=wort_sep(n,',',TRUE,w1,w2);
  int g,len=-1,a;
  if(e<2) error(42,"BLOAD"); /* Zu wenig Parameter */
  else {
    filename=s_parser(w1); 
    e=wort_sep(w2,',',TRUE,w1,w2);
    if(e==2) len=(int)parser(w2);
    g=bload(filename,(char *)(int)parser(w1),len);
    free(filename);
    if(g==-1) io_error(errno,"BLOAD");
  }
}
/* Fuehrt Code an Adresse aus */
void c_exec(char *n) { f_exec(n);}
int f_exec(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,w1,w2);
  typedef struct  {int feld[20];} GTT;
  GTT gtt;
  int (*adr)(GTT);
  int i=0;
  while(e) {
    if(i==0) adr=(int (*)())((int)parser(w1));
    else if(i<20) {
      if(strncmp(w1,"D:",2)==0) {
        *((double *)(&gtt.feld[i-1]))=parser(w1+2);
        if(sizeof(double)>(sizeof(int))) i+=(sizeof(double)/sizeof(int))-1;
      } else if(strncmp(w1,"F:",2)==0) {
        *((float *)(&gtt.feld[i-1]))=(float)parser(w1+2);
	if(sizeof(float)>(sizeof(int))) i+=(sizeof(float)/sizeof(int))-1;
      } else if(strncmp(w1,"L:",2)==0)  gtt.feld[i-1]=(int)parser(w1+2);
      else if(strncmp(w1,"W:",2)==0)  gtt.feld[i-1]=(int)parser(w1+2);
      else if(strncmp(w1,"B:",2)==0)  gtt.feld[i-1]=(int)parser(w1+2);
      else gtt.feld[i-1]=(int)parser(w1);
    }
    i++;
    e=wort_sep(w2,',',TRUE,w1,w2);
  }
  if(i==0) error(42,"EXEC"); /* Zu wenig Parameter */
  else return(adr(gtt));
  return(0);
}

void c_bsave(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  char *filename;
  int e=wort_sep(n,',',TRUE,w1,w2);
  int g,len,a;
  if(e<2) error(42,"BSAVE"); /* Zu wenig Parameter */
  else {
    filename=s_parser(w1); 
    e=wort_sep(w2,',',TRUE,w1,w2);
    if(e!=2) error(42,"BSAVE"); /* Zu wenig Parameter */
    else {
      len=(int)parser(w2);
      g=bsave(filename,(char *)(int)parser(w1),len);
      if(g==-1) io_error(errno,"BSAVE");
    }
    free(filename);
  }
}
void c_bget(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  char *adr;
  int anzahl;
  int e=wort_sep(n,',',TRUE,w1,w2);
  int i=get_number(w1);
  e=wort_sep(w2,',',TRUE,w1,w2);
  adr=(char *)(int)parser(w1);
  anzahl=(int)parser(w2);
  if(filenr[i]) {
    fread(adr,1,anzahl,dptr[i]);
  } else {error(24,w1);return;} /* File nicht geoeffnet */
}
void c_bput(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  char *adr;
  int anzahl;
  int e=wort_sep(n,',',TRUE,w1,w2);
  int i=get_number(w1);
  e=wort_sep(w2,',',TRUE,w1,w2);
  adr=(char *)(int)parser(w1);
  anzahl=(int)parser(w2);
  if(filenr[i]) {
    fwrite(adr,1,anzahl,dptr[i]);
  } else {error(24,w1);return;} /* File nicht geoeffnet */

}
void c_bmove(char *n) {   /* Memory copy  BMOVE quelladr%,zieladr%,anzahl%    */
  char w1[strlen(n)+1],w2[strlen(n)+1];
  char *ziel;
  int anzahl;
  int e=wort_sep(n,',',TRUE,w1,w2);
  char *quelle=(char *)(int)parser(w1);
  e=wort_sep(w2,',',TRUE,w1,w2);
  ziel=(char *)(int)parser(w1);
  anzahl=(int)parser(w2);
  memmove(ziel,quelle,(size_t)anzahl);
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
int inp8(char *n) {
  int i=get_number(n);
  char ergebnis;
  FILE *fff;
  if(i==-2) fff=stdin;  
  else if(filenr[i]) fff=dptr[i];
  else {error(24,"");return(-1);} /* File nicht geoeffnet */
  fread(&ergebnis,sizeof(char),1,fff);
  return((int)ergebnis);
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
int inp16(char *n) {
  int i=get_number(n);
  short ergebnis;
  FILE *fff=stdin;

  if(filenr[i]) {
    fff=dptr[i];
    fread(&ergebnis,sizeof(short),1,fff);
    return((int)ergebnis);
  } else error(24,""); /* File nicht geoeffnet */
  return(-1);
}
int inp32(char *n) {
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

/* Dynamisches Linken von Shared-Object-Files */


void *dyn_symbol(void *handle,char *name){
  char *error;
  char *cosine;
  cosine=dlsym(handle, name);
  if ((error = dlerror()) != NULL)  {
    printf("ERROR: %s\n",error);
    return(NULL);
  }
  return(cosine);
}
int f_symadr(char *n) { 
  char v[strlen(n)+1],w[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,v,w);
  int adr=0;
  if(e>1) {
    int i=get_number(v);
    
    if(filenr[i]==2) {
      char *sym=s_parser(w);
      char *error;
      adr = (int)dlsym(dptr[i],sym);
      if ((error = dlerror()) != NULL) printf("ERROR: SYM_ADR: %s\n",error);
      free(sym);
    } else error(24,v); /* File nicht geoeffnet */
  } else error(32,"SYM_ADR"); /* Syntax error */
  return(adr);
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
