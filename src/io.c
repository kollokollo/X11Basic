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
#include <math.h>
#ifndef WINDOWS
#include <termios.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <termio.h>
#else
#include <windows.h>

#endif
#include <sys/time.h>

/* fuer Dynamisches Linken von shared Object-Files   */

#ifndef WINDOWS
#ifndef __hpux
#include <dlfcn.h>
#endif
#ifdef __hpux
#define RTLD_LAZY       0x00000002  /* bind deferred */
#endif

#include <dlfcn.h>
#endif

#include "file.h"
#include "defs.h"
#include "globals.h"
#include "protos.h"


#ifdef WINDOWS
#define FD_SETSIZE 4096
#define EINPROGRESS   WSAEINPROGRESS
#define EWOULDBLOCK   WSAEWOULDBLOCK
#define gettimeofday(a,b) QueryPerformanceCounter(a)
#else 
#define send(s,b,l,f) write(s,b,l)
#define recv(s,b,l,f) read(s,b,l)
#define closesocket(s) close(s)
#define ioctlsocket(a,b,c) ioctl(a,b,c)
#endif

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
        STRING buffer;
        c=v[strlen(v)-1];
        v[strlen(v)-1]=0;
        buffer=print_arg(v);
	fwrite(buffer.pointer,1,buffer.len,fff);
        if(c=='\'') fputc(' ',fff);
        else if(c==',') fputc('\011',fff);
	free(buffer.pointer);
      } else {
        STRING buffer=print_arg(v);
	fwrite(buffer.pointer,1,buffer.len,fff);
        fputc('\n',fff);
	free(buffer.pointer);
      }
  } else fputc('\n',fff);
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
#ifndef WINDOWS
  struct hostent *hostinfo;
  name->sin_family=AF_INET;
  name->sin_port=htons(port);
  hostinfo=gethostbyname(hostname);
  if(hostinfo==NULL) return(-1);
  name->sin_addr=*(struct in_addr *)hostinfo->h_addr;
#endif
  return(0);
}

int make_socket(unsigned short int port) {
  int sock;
#ifndef WINDOWS
  struct sockaddr_in name;
  sock=socket(PF_INET, SOCK_STREAM,0);
  if(sock<0) return(-1);
  name.sin_family=AF_INET;
  name.sin_port=htons(port);
  name.sin_addr.s_addr=htonl(INADDR_ANY);
  if(bind(sock,(struct sockaddr *) &name, sizeof(name))<0) return(-1);
#endif
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
#ifndef WINDOWS
	struct sockaddr_in servername;
	 
    /* printf("Open Socket #%d: modus=%s adr=%s port=%d\n",number,modus2,filename, port);*/
        sock=socket(PF_INET, SOCK_STREAM,0);
	if(sock<0) {
	  io_error(errno,"socket");
	  dptr[number]=NULL;
	} else {
	  if(init_sockaddr(&servername,filename,port)<0) {
	    io_error(errno,"init_sockadr");
	    dptr[number]=NULL;
	  } else { 
	    if(0>connect(sock,(struct sockaddr *) &servername, sizeof(servername))) {
              io_error(errno,"connect");
	      dptr[number]=NULL;
	    } else {
              dptr[number]=fdopen(sock,modus2);
	    }
	  }
	}
#endif	
      } else if(special=='S') { /* serve */
        int sock;
	#ifndef WINDOWS
   /*  printf("Create Socket #%d: modus=%s adr=%s port=%d\n",number,modus2,filename, port);*/
        sock=make_socket(port);
	if(sock<0) {
	  io_error(errno,"make_socket");
	  dptr[number]=NULL;
	} else {
	  if(listen(sock,1)<0) {
	    io_error(errno,"listen");
	    dptr[number]=NULL;
	  } else { 
              dptr[number]=fdopen(sock,modus2);
	  }
	}
	#endif
      } else if(special=='A') { /* accept */
        int sock,sock2;
	size_t size;	
#ifndef WINDOWS
	struct sockaddr_in clientname;
#ifdef DEBUG
        printf("Accept Socket #%d: modus=%s adr=%s port=%d\n",number,modus2,filename, port);
#endif
        if(filenr[port]) {
	  sock=fileno(dptr[port]);
	  size=sizeof(clientname);
	  sock2=accept(sock,(struct sockaddr *) &clientname,&size);
	  if(sock2<0) {
	    io_error(errno,"accept");
	    dptr[number]=NULL;
	  } else {
#ifdef DEBUG	   
	    printf("Verbindung von %s auf Port $%hd an %d\n",inet_ntoa(clientname.sin_addr), 
	      ntohs(clientname.sin_port),sock2);
#endif
            dptr[number]=fdopen(sock2,modus2);
	  }
	} else printf("Socket #d nicht geoeffnet.\n",port); 
#endif
      } else dptr[number]=fopen(filename,modus2);
      if(dptr[number]==NULL) {io_error(errno,"OPEN");free(filename);return;}
      else  filenr[number]=1;
      if(special=='X') {  /* Fuer Serielle Devices !  */
        int fp=fileno(dptr[number]);
#ifndef WINDOWS
	struct termios ttyset;
	if(isatty(fp)) {
	  printf("Ist ein tty !\n");
          /* Stream buffering ausschalten */
	  setbuf(dptr[number],NULL);
	  /* Nicht-Canoschen Mode setzen */    
	  tcgetattr(fp,&ttyset);
          ttyset.c_lflag &= ~(ICANON|ECHO);
	  ttyset.c_cc[VMIN]=1;
	  ttyset.c_cc[VTIME]=0;
	
          ttyset.c_cflag = (CBAUD|CSIZE|CREAD) & port;
          ttyset.c_cflag |= CLOCAL;
          ttyset.c_cflag |= CREAD;
          ttyset.c_iflag = ttyset.c_oflag = ttyset.c_lflag = (unsigned short) 0;
          ttyset.c_oflag = (ONLRET);
          if(tcsetattr(fp,TCSADRAIN,&ttyset)<0)   printf("X: fileno=%d ERROR\n",fp);
      } else printf("File ist kein TTY ! Einstellungen ignoriert !\n");
  #endif

  
      }
  }
  free(filename);
}

void c_link(PARAMETER *plist, int e) {
  int number;
  if(e==2) {
    number=plist[0].integer;
    if(filenr[number]) error(22,"");  /* File schon geoeffnet  */
    else {
#ifdef WINDOWS
      dptr[number]=LoadLibrary(plist[1].pointer);
      if(dptr[number]==NULL) io_error(GetLastError(),"LINK");
      else  filenr[number]=2;
#else
      dptr[number]=dlopen(plist[1].pointer,RTLD_LAZY);
      if(dptr[number]==NULL) io_error(errno,"LINK");
      else  filenr[number]=2;
#endif
    } 
  }
}

const struct {int sf; char xf; } table[] = {
    { 0,   7 }, /* 0: No error status currently */
    { EPERM,   -51 }, /* 1: Not super-user */
    { ENOENT,  -33 }, /* 2: No such file or directory*/
    { ESRCH,    -3 }, /* 3: No such process*/
    { EINTR,   -53 }, /* 4: Interrupted system call*/
    { EIO,      -1 }, /* 5: Allgemeiner IO-Fehler */
    { ENXIO,   -36 }, /* 6: No such device or address */
    { E2BIG,    -7 }, /* 7: Arg list too long */
    { ENOEXEC, -66 }, /* 8: Exec format error */
    { EBADF,   -37 }, /* 9: Bad file number*/
    { ECHILD,  -55 }, /* 10: No children*/
    { EAGAIN,  -72 }, /* 11: Try again*/
    { ENOMEM,  -12 }, /* 12: Not enough core*/
    { EACCES,  -13 }, /* 13: Permission denied*/
    { EFAULT,  -57 }, /* 14: Bad address*/
#ifndef WINDOWS
    { ENOTBLK, -58 }, /* 15: Block device required*/
#endif
    { EBUSY,   -59 }, /* 16: Mount device busy*/
    { EEXIST,  -17 }, /* 17: File exists*/
    { EXDEV,   -18 }, /* 18: Cross-device link*/
    { ENODEV,  -19 }, /* 19: No such device*/
    { ENOTDIR, -20 }, /* 20: Not a directory*/
    { EISDIR,  -21 }, /* 21: Is a directory*/
    { EINVAL,  -22 }, /* 22: Invalid argument*/
    { ENFILE,  -23 }, /* 23: File table overflow */
    { EMFILE,  -24 }, /* 24: Too many open files */
    { ENOTTY,  -25 }, /* 25: Not a typewriter */
#ifndef WINDOWS
    { ETXTBSY, -26 }, /* 26: Text file busy */
#endif
    { EFBIG,   -27 }, /* 27: File too large */
    { ENOSPC,  -28 }, /* 28: No space left on device */
    { ESPIPE,  -29 }, /* 29: Illegal seek */
    { EROFS,   -30 }, /* 30: Read-Only File-System */
    { EMLINK,  -31 }, /* 31: Too many links*/
    { EPIPE,   -32 }, /* 32: Broken pipe*/
    { EDOM,    -62 }, /* 33: Math arg out of domain of func*/
    { ERANGE,  -63 }, /* 34: Math result not representable*/
    { EDEADLK,      -69 }, /* 35: Resource deadlock would occur*/
    { ENAMETOOLONG, -70 }, /* 36: File name too long */

    { ENOSYS,       -38 }, /* 38: Function not implemented */
    { ENOTEMPTY,    -39 }, /* 39: Directory not empty */
#ifndef WINDOWS
    { ELOOP,        -71 }, /* 40: Too many symbolic links encountered */
    { EWOULDBLOCK,  -41 }, /* 41: Operation would block */
    { ENOMSG,       -42 }, /* 42: No message of desired type*/
    { EIDRM,        -43 }, /* 43: Identifier removed*/

    { ELNRNG,       -48 }, /* 48: Link number out of range*/

    { EBADE,        -52 }, /* 52: Invalid exchange*/

    { EXFULL,       -54 }, /* 54: Exchange full*/

    { ENOSTR,       -60 }, /* 60: Device not a stream */
    
    { ENOTSOCK,     -88 }, /* 88: Socket operation on non-socket */

    { EOPNOTSUPP,   -95 }, /* 95: Operation not supported on transport endpoint */

    { EADDRINUSE,   -98 }, /* 98: Address already in use */
    { EADDRNOTAVAIL,-99 }, /* 99: Cannot assign requested address */

    { ENETDOWN,    -100 }, /* 100: Network is down */
    { ENETUNREACH, -101 }, /* 101: Network is unreachable */

    { ENETRESET,   -102 }, /* 102: Network dropped connection because of reset */
    { ECONNABORTED,-103 }, /* 103: Software caused connection abort */

    { ECONNRESET,  -104 }, /* 104: Connection reset by peer*/

    { ENOBUFS,     -105 }, /* 105: No buffer space available */
    { EISCONN,     -106 }, /* 106: Transport endpoint is already connected*/
    { ENOTCONN,    -107 }, /* 107: Transport endpoint is not connected */
    { ETIMEDOUT,   -110 }, /* 110: Connection timed out */
    { ECONNREFUSED,-111 }  /* 111: Connection refused */
#endif
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
#ifdef WINDOWS
      if(FreeLibrary(dptr[i])==0) io_error(GetLastError(),"UNLINK");
      else filenr[i]=0;
#else
      if(dlclose(dptr[i])==EOF) io_error(errno,"UNLINK");
      else filenr[i]=0;
#endif
    }
    else error(24,w); /* File nicht geoeffnet...*/
  } else {
    for(i=0;i<ANZFILENR;i++) {
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
  if(fflush(fff)) io_error(errno,"FLUSH");
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
  int fp,i=get_number(n);
  char ergebnis;
  FILE *fff;
  if(i==-2) fff=stdin;  
  else if(filenr[i]) fff=dptr[i];
  else {error(24,"");return(-1);} /* File nicht geoeffnet */
  
  fread(&ergebnis,1,1,fff);
  return((int)ergebnis);
}
int inpf(char *n) {
  int fp,i=get_number(n);
  FILE *fff=stdin;
  if(i==-2) {
    return(kbhit() ? -1 : 0);
  } else if(filenr[i]) {
    fff=dptr[i];        
    fflush(fff);
#ifndef WINDOWS
    fp=fileno(fff);
    ioctl(fp, FIONREAD, &i);
    return(i); 
#else    
    return(((eof(fff)) ? 0 : -1)); 
#endif    
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
    fread(&ergebnis,sizeof(long),1,fff);
    return(ergebnis);
  } else error(24,""); /* File nicht geoeffnet */
  return(-1);
}

/* Terminal-Einstellungen. Wichtig fuer die Zeichenweise eingabe */

#ifndef WINDOWS
struct termios saved_attributes;  /* fuer alte Einstellung */
#endif				
void set_input_mode(int mi, int ti) {
#ifndef WINDOWS
  struct termios tattr;
  if(isatty(STDIN_FILENO)) {
    tcgetattr(STDIN_FILENO,&saved_attributes);
    tcgetattr(STDIN_FILENO,&tattr);
    tattr.c_lflag &= ~(ICANON|ECHO);
    tattr.c_cc[VMIN]=mi;
    tattr.c_cc[VTIME]=ti;
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&tattr);
  }
#endif
}
void set_input_mode_echo(int onoff) {
#ifndef WINDOWS
  struct termios tattr;
  if(isatty(STDIN_FILENO)) {
    tcgetattr(STDIN_FILENO,&tattr);
    if(onoff) tattr.c_lflag |= ECHO;
    else tattr.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&tattr);
  }
#endif
}
void reset_input_mode() {
#ifndef WINDOWS
  if(isatty(STDIN_FILENO)) tcsetattr(STDIN_FILENO,TCSANOW,&saved_attributes);
#endif
}

/* Dynamisches Linken von Shared-Object-Files */

#if 0
void *dyn_symbol(void *handle,char *name){
  char *error;
  char *cosine;
#ifdef WINDOWS
  cosine=(char *)(int)GetProcAddress(handle,name);
  if((int)cosine==0) printf("ERROR: SYM_ADR: %s\n",GetLastError());
#else
  cosine=dlsym(handle, name);
  if ((error = dlerror()) != NULL)  {
    printf("ERROR: %s\n",error);
    return(NULL);
  }
#endif
  return(cosine);
}
#endif
int f_symadr(char *n) { 
  char v[strlen(n)+1],w[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,v,w);
  int adr=0;
  if(e>1) {
    int i=get_number(v);
    
    if(filenr[i]==2) {
      char *sym=s_parser(w);
      char *error;
      #ifdef WINDOWS
        adr = (int)GetProcAddress(dptr[i],sym);
      if (adr==0) printf("ERROR: SYM_ADR: %s\n",GetLastError());
    
      #else
      adr = (int)dlsym(dptr[i],sym);
      if ((error = dlerror()) != NULL) printf("ERROR: SYM_ADR: %s\n",error);
      #endif
      free(sym);
    } else error(24,v); /* File nicht geoeffnet */
  } else error(32,"SYM_ADR"); /* Syntax error */
  return(adr);
}


char *terminalname(int fp) {
  char *name=NULL,*erg;
  #ifndef WINDOWS
  if(isatty(fp)) name=ttyname(fp);
  #endif
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
#ifndef WINDOWS
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
#endif

char *inkey() {
   static char ik[MAXSTRLEN];
   int i=0;
   while(kbhit()) ik[i++]=getc(stdin);
   ik[i]=0;
   return(ik);
}


/* Baut einen String aus der Argumenteliste des PRINT-Kommandos zusammen */

STRING print_arg(char *ausdruck) {
  int e;
  char *a1,w1[strlen(ausdruck)+1],w2[strlen(ausdruck)+1];
  char w3[strlen(ausdruck)+1],w4[strlen(ausdruck)+1];
  STRING ergebnis;
  ergebnis.pointer=malloc(4);
  ergebnis.len=0;
  e=arg2(ausdruck,TRUE,w1,w2);
  while(e) {
    a1=indirekt2(w1);
  /*  printf("TEST: <%s> <%s> %d\n",w1,w2,e);*/
    if(strncmp(a1,"AT(",3)==0) {
      a1[strlen(a1)-1]=0;
      wort_sep(a1+3,',',TRUE,w3,w4);
      ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+16);
      sprintf(ergebnis.pointer+ergebnis.len,"\033[%.3d;%.3dH",(int)parser(w3),(int)parser(w4));
      ergebnis.len+=16;
    } else if(strncmp(a1,"TAB(",4)==0) {
      a1[strlen(a1)-1]=0;
      ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+8);
      sprintf(ergebnis.pointer+ergebnis.len,"\033[%.3dC",(int)parser(a1+4));
      ergebnis.len+=8;
    } else {
      if(strlen(a1)) {    
        int typ,ee;
	ee=wort_sep2(a1," USING ",TRUE,a1,w4);
	typ=type2(a1);
	
	if(typ & ARRAYTYP) {    /* Hier koennte man .... */
	  if(typ & STRINGTYP) ;
	  else ;
	} else if(typ & STRINGTYP) {
          STRING a3=string_parser(a1);
	  ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+a3.len);
          memcpy(ergebnis.pointer+ergebnis.len,a3.pointer,a3.len);
	  ergebnis.len+=a3.len;
	  free(a3.pointer);
        } else {
	  if(ee==2) {
	    STRING a3=string_parser(w4);
	    STRING e2=do_using(parser(a1),a3);
	    ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+e2.len);
	    memcpy(ergebnis.pointer+ergebnis.len,e2.pointer,e2.len);
            ergebnis.len+=e2.len;
	    free(a3.pointer);
	    free(e2.pointer);
	  } else {
	    char b[32];
	    sprintf(b,"%.13g",parser(a1));
	    ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+strlen(b));
	    memcpy(ergebnis.pointer+ergebnis.len,b,strlen(b));
	    ergebnis.len+=strlen(b);
          }
	}
      }
    }
    ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+1);
    if(e==2) ;
    else if(e==3) ergebnis.pointer[ergebnis.len++]='\011';   /* TAB */
    else if(e==4) ergebnis.pointer[ergebnis.len++]=' ';
    free(a1);
    e=arg2(w2,TRUE,w1,w2);
  }
  return(ergebnis);
}
STRING do_using(double num,STRING format){
  STRING dest;
  int a,p,p2,r,i,j; /* dummy */
  int neg,ln=0,vorz=1;
  char *des;
  const char *digits="01234567899";
  
  
  if (*format.pointer=='%') { /* c-style format */
    char b[32];
    sprintf(b,format.pointer,num);
    dest.len=strlen(b);
    dest.pointer=malloc(strlen(b));
    memcpy(dest.pointer,b,strlen(b));
  } else { /* basic-style format */
    dest.len=format.len;
    dest.pointer=malloc(format.len);
    des=dest.pointer;
    
   /* Zaehle die Rauten vor dem Punkt */
   a=r=p=0;
   while((format.pointer)[p] && (format.pointer)[p]!='.') {
     if((format.pointer)[p++]=='#') r++;
   }
   /* Zaehle die Rauten nach dem Punkt */
   while((format.pointer)[p]) {
     if((format.pointer)[p++]=='#') a++;
   }
   j=a+r;
   neg=(num<0);
   num=fabs(num);

   num+=0.5*pow(10.0,-(double)a);  /* zum Runden */
   
   for(i=0;i<format.len;i++) {
     if(format.pointer[i]=='+') {*des=(neg ? '-':'+'); vorz=0;}
     else if(format.pointer[i]=='-') {*des=(neg ? '-':' ');vorz=0;}
     else if(format.pointer[i]=='.') {*des='.';ln=1;}
     else if(format.pointer[i]=='#') {
       j--;
       p=(int)(num/pow(10,(double)--r));
       p2=(int)(num/pow(10,(double)(r-1)));
       num-=p*pow(10,(double)r);
       if(p) {
         *des=digits[p];ln=1;
       } else {
         if(vorz&&p2) { *des=(neg ? '-':' ');vorz=0;}
	 else *des=(ln?'0':' ');
       }
     } else *des=format.pointer[i];
     des++;
   }
  }
  return(dest);
}

