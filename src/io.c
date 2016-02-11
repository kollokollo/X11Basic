/* io.c I/O_Routinen  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

/* termio.h (weil obsolet) entfernt.    11.08.2003   MH  */

#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#ifndef WINDOWS
#ifdef __hpux
#define CCTS_OFLOW      0x00010000      /* CTS flow control of output */
#define CRTS_IFLOW      0x00020000      /* RTS flow control of input */
#define CRTSCTS         (CRTS_IFLOW|CCTS_OFLOW) /* RTS/CTS flow control */
#include <sys/termios.h>
#else
#include <termios.h>
#endif
#include <fcntl.h>
#include <sys/ioctl.h>
#ifdef HAVE_SYS_KD_H
#include <sys/kd.h>
#endif
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <netdb.h>
#else
#include <windows.h>
#endif
#include <sys/time.h>

/* fuer Dynamisches Linken von shared Object-Files   */

#ifdef WINDOWS
#undef HAVE_DLOPEN
#endif

#ifdef HAVE_DLOPEN
#include <dlfcn.h>
#ifdef __hpux
#define RTLD_LAZY       0x00000002  /* bind deferred */
#endif
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




/* Get the number of rows and columns for this screen. */
void getrowcols(int *rows, int *cols) {
#ifndef WINDOWS
#ifdef TIOCGWINSZ
	struct winsize ws;

	if (ioctl(0, TIOCGWINSZ, &ws) >= 0) {
		*rows = ws.ws_row;
		*cols = ws.ws_col;
	}	
#else
//#  ifdef TIOCGSIZE */
	struct ttysize ws;

	if (ioctl(0, TIOCGSIZE, &ws) >= 0) {
		*rows = ws.ts_lines;
		*cols = ws.ts_cols;
	}
#endif
#endif
}



/*******************************/
/* Ein- und Ausgaberoutinen    */

FILE *get_fileptr(int n) {
  if(n==-1) return(stderr);  
  else if(n==-2) return(stdin);  
  else if(n==-4) return(stdout);  
  else if(n<0 || n>=ANZFILENR) return(NULL);
  else if(filenr[n]) return(dptr[n]);
  else return(NULL);
}

int f_freefile() {
  int i=1;
  while(i<ANZFILENR) {
    if(filenr[i]==0) return(i);
    i++;
  }
  return(-1);
}
#if 0
int f_map(PARAMETER *plist,int e) {
  int ergebnis;
  ergebnis=mmap(0, size_t length, int prot , int flags, int
       fd, off_t offset);
  if(ergebnis==MAP_FAILED) io_error(errno,"mmap");
  return(ergebnis);
}
#endif
void c_msync(PARAMETER *plist,int e) {
  if(e>=2) {
  #ifndef WINDOWS
      if(msync((void *)plist[0].integer, plist[1].integer,MS_SYNC|MS_INVALIDATE))
        io_error(errno,"msync"); 
  #endif
  }
}
void c_unmap(PARAMETER *plist,int e) {
  if(e>=2) {
   #ifndef WINDOWS 
      if(munmap((void *)plist[0].integer, plist[1].integer))
        io_error(errno,"munmap"); 
   #endif
  }
}
void c_locate(PARAMETER *plist,int e) {
  printf("\033[%.3d;%.3dH",plist[0].integer,plist[1].integer);
}
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
  char *u,*v,*text;
  char inbuf[MAXSTRLEN];
  int e,e2;
  FILE *fff=stdin;
  
  if(*n=='#') {
    wort_sep(n,',',TRUE,t,s);
    e=get_number(t);
    if(filenr[e]) fff=dptr[e];
    else error(24,""); /* File nicht geoeffnet */
  } else strcpy(s,n);
  
  if(strlen(s)) {   
    e=arg2(s,TRUE,s,t);
    if(*s=='\"') {
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
     /* Wenn Terminal: Jetzt ganze Zeile einlesen */
    if(fff==stdin) v=do_gets(text);
    u=inbuf;
    while(e!=0) {
      xtrim(s,TRUE,s);
      if(fff==stdin) {
        e2=arg2(v,TRUE,u,v);
        while(e2==0) {
	  v=do_gets(text);
	  e2=arg2(v,TRUE,u,v);
	}
      } else u=input(fff,inbuf);
     
      if(type2(s) & STRINGTYP) zuweiss(s,u);
      else zuweis(s,parser(u));
      e=arg2(t,TRUE,s,t);
    }
    free(text);
  }  
}
STRING f_lineinputs(char *n) {
  int i=get_number(n);
  if(filenr[i]) {
    STRING inbuf;
    FILE *fff=dptr[i];
    inbuf.pointer=malloc(MAXSTRLEN);
    lineinput(fff,inbuf.pointer);
    inbuf.len=strlen(inbuf.pointer);
    return(inbuf);
  } else {
    error(24,""); /* File nicht geoeffnet */
    return(vs_error());
  }
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
  } else if(e==1) {
     int anz=(int)parser(s);
     inbuf.pointer=malloc(anz+1);
     inbuf.len=(int)fread(inbuf.pointer,1,anz,stdin);
     inbuf.pointer[anz]=0;
     return(inbuf);
  } else error(32,"INPUT$"); /* Syntax Error */
  return(vs_error());
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
    fff=get_fileptr(get_number(buffer));
    free(buffer);
    if(fff==NULL) {error(24,"");return;} /* File nicht geoeffnet */
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

/* Connect #n,"polarfrost.homenet.al",5556
 connect  socket to server and port */

void c_connect(PARAMETER *plist,int e) {
  struct sockaddr_in host_address;
  int host_address_size;
  unsigned char *address_holder;
  int sock;
  FILE *fff=get_fileptr(plist[0].integer);
  if(fff==NULL) {error(24,"");return;} /* File nicht geoeffnet */    
  sock=fileno(fff);
  if(init_sockaddr(&host_address,plist[1].pointer,plist[2].integer)<0) io_error(errno,"init_sockadr");
  else { 
    if(0>connect(sock,(struct sockaddr *) &host_address, sizeof(host_address))) 
      io_error(errno,"connect");    
  }
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
int make_UDP_socket(unsigned short int port) {
  int sock;
  struct sockaddr_in name;

  sock=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
  if(sock<0) return(-1);
  memset((void*)&name, 0, sizeof(name));
  name.sin_family=AF_INET;
  name.sin_addr.s_addr=INADDR_ANY;
  name.sin_port=htons(port);
  if(bind(sock,(struct sockaddr *) &name, sizeof(name))<0) return(-1);
  return(sock);
}





/* Universelle OPEN-Funktion. Oeffnet Files, Devices, und sockets   */
/* OPEN "I",#1,filename$[,port]   */

//#define DEBUG 1

void c_open(char *n) {
  char modus,special=0;
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int number=1,e,port=0,i=0;
  char *filename=NULL;
  char modus2[5]="r";
  int baud=9600,bits=8,stopbits=1,parity=0,sflow=0,hflow=0,dtr=0;
 /* 'OPEN "UX:9600,N,8,1,DS,CS,RS,CD",#1,"/dev/ttyS1",8+5+6*8 */
  e=wort_sep(n,',',TRUE,w1,w2);
  while(e) {
    if(strlen(w1)) {
      switch(i) {
      case 0: { /* Modus */     
        char *mod=s_parser(w1);      /* Modus */
        modus=toupper(mod[0]);
	special=toupper(mod[1]);
	if(special=='X') {
	  char ww1[strlen(mod)+1],ww2[strlen(mod)+1];
	  int ii=0,ee=wort_sep(mod,':',TRUE,ww1,ww2);
	  ee=wort_sep(ww2,',',TRUE,ww1,ww2);
	  while(ee) {
     	    if(strlen(ww1)) {
	      switch(ii) {
	        case 0: { baud=(int)parser(ww1);  break; } /* Baudrate */  
		case 2: { bits=(int)parser(ww1);  break; } /* Bits */  
		case 3: { stopbits=(int)parser(ww1);  break; } /* Stopbits */  

                default: 
		  if(strcmp(ww1,"N")==0) parity=0;
		  else if(strcmp(ww1,"O")==0) parity=1;
		  else if(strcmp(ww1,"E")==0) parity=2;
		  else if(strcmp(ww1,"XON")==0) sflow=1;
		  else if(strcmp(ww1,"CTS")==0) hflow=1;
		  else if(strcmp(ww1,"DTR")==0) dtr=1;
		  else
		printf("Unknown extra Option: %s\n",ww1);

	      }
	    }
	    ii++;
            ee=wort_sep(ww2,',',TRUE,ww1,ww2);
	    
	  }
#if DEBUG
	  printf("baud=%d, bits=%d, stopbits=%d, parity=%d\n",baud,bits,
	  stopbits,parity);
#endif
	}
        free(mod);
        if(modus=='I') strcpy(modus2,"r");
        else if(modus=='O') strcpy(modus2,"w");
        else if(modus=='U') strcpy(modus2,"r+");
        else if(modus=='A') strcpy(modus2,"a+");
        else error(21,""); /* bei Open nur erlaubt ...*/
	break;
      }
      case 1: { number=get_number(w1); break; } 
      case 2: { filename=s_parser(w1); break; } 
      case 3: { port=(int)parser(w1);  break; } 	   
      default: break;
      }
    }
    i++;
    e=wort_sep(w2,',',TRUE,w1,w2);
  } 	    
#ifdef DEBUG
   printf("number=%d, filename=<%s>, port=%d ($08x)\n",number,filename,port,port);
#endif

  if(filenr[number]) error(22,"");  /* File schon geoeffnet  */
  else if(number>99 || number<1) error(23,"");  /* File # falsch  */
  else {
    /*  Sockets  */
    if(special=='C') { /* Connect */
      int sock;
      struct sockaddr_in servername;
#ifdef DEBUG	 
     printf("Open Socket #%d: modus=%s adr=%s port=%d\n",number,modus2,filename, port);
#endif
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
	  } else dptr[number]=fdopen(sock,modus2);
	}
      }
    } else if(special=='S') { /* serve */
      int sock;
#ifdef DEBUG
   printf("Create Socket #%d: modus=%s adr=%s port=%d\n",number,modus2,filename, port);
#endif
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
    } else if(special=='U') { /* UDP datagramms */
      int sock;
#ifdef DEBUG
   printf("Create UDP Socket #%d: modus=%s adr=%s port=%d\n",number,modus2,filename, port);
#endif
      sock=make_UDP_socket(port);
      if(sock<0) {
	io_error(errno,"make_udp_socket");
	dptr[number]=NULL;
      } else {
         printf("sock=%d\n",sock);
          dptr[number]=fdopen(sock,modus2);
          if(dptr[number]==NULL) io_error(errno,"make_udp_socket");
      }
    } else dptr[number]=fopen(filename,modus2);
    if(dptr[number]==NULL) {io_error(errno,"OPEN");free(filename);return;}
    else  filenr[number]=1;
    if(special=='X') {  /* Fuer Serielle Devices !  */
      int fp=fileno(dptr[number]);
#ifndef WINDOWS
      struct termios ttyset;
      if(isatty(fp)) {
        /* Stream buffering ausschalten */
	setbuf(dptr[number],NULL);
	/* Nicht-Canoschen Mode setzen */    
	tcgetattr(fp,&ttyset);
#ifdef DEBUG	
	  printf("Old Port-Settings:\n");
	  printf("cflag=$%08x \n",ttyset.c_cflag);
	  printf("iflag=$%08x \n",ttyset.c_iflag);
	  printf("oflag=$%08x \n",ttyset.c_oflag);
	  printf("lflag=$%08x \n",ttyset.c_lflag);
//	  printf("cc(VMIN)=$%08x \n",ttyset.c_cc[VMIN]);
//	  printf("cc(VMAX)=$%08x \n",ttyset.c_cc[VMAX]);
#endif
	ttyset.c_cc[VMIN]=1;
	ttyset.c_cc[VTIME]=0;
	if(!port) {
/* CBAUD gibt es nicht in allen systemen ! */
#ifdef CBAUD
          port&=~CBAUD;      /* Bits loeschen !  */
#else
	  port=0;
#endif
	  if(baud==300) port|=B300;
	  else if(baud==1200) port|=B1200;
	  else if(baud==2400) port|=B2400;
	  else if(baud==4800) port|=B4800;
	  else if(baud==9600) port|=B9600;
#ifdef B19200
	  else if(baud==19200) port|=B19200;
#else /* B19200 */
#  ifdef EXTA
	  else if(baud==19200) port|=EXTA;

#endif
#endif
#ifdef B38400
	  else if(baud==38400) port|=B38400;
#else /* B38400 */
#  ifdef EXTB
	  else if(baud==38400) port|=EXTB;
#endif
#endif
#ifdef B57600
	  else if(baud==57600) port|=B57600;
#endif
#ifdef B115200
	  else if(baud==115200) port|=B115200;
#endif
#ifdef B230400
	  else if(baud==230400) port|=B230400;
#endif  
          else printf("Baud rate not supported !\n");
   	  port&=~CSIZE;      /* Bits loeschen !  */
          if(bits==7) port|=CS7;
          else if(bits==8) port|=CS8;
          else if(bits==6) port|=CS6;
          else if(bits==5) port|=CS5;
	  else printf("%d Bits not supported !\n",bits);
          port&=~(PARENB|PARODD);
          if(parity==2)      port|=PARENB;
          else if(parity==1) port|=(PARENB|PARODD);
          if(stopbits==2)    port|=CSTOPB;
          else               port&=~CSTOPB;

	}
/* 
  Set bps rate and hardware flow control and 8n1 (8bit,no parity,1 stopbit).
  Also don't hangup automatically and ignore modem status.
  Finally enable receiving characters.
*/
        ttyset.c_iflag = IGNBRK;
        ttyset.c_lflag = 0;
        ttyset.c_lflag &= ~(ICANON|ECHO);
	ttyset.c_oflag = (unsigned short) 0;

        ttyset.c_cflag = port|CLOCAL|CREAD;
        ttyset.c_cflag &= ~CRTSCTS;

#ifndef _DCDFLOW 
                /* Set Hardware-Flow */
           if(hflow) ttyset.c_iflag |= CRTSCTS;

#endif
           if(sflow) ttyset.c_iflag |= IXON | IXOFF;
           else ttyset.c_iflag &= ~(IXON|IXOFF|IXANY);

#ifdef DEBUG
          printf("IGNPAR  =$%08x \n",IGNPAR);
          printf("PARENB  =$%08x \n",PARENB);
          printf("PARODD  =$%08x \n",PARODD);

	  printf("port =$%08x \n",port);
	  printf("cflag=$%08x \n",ttyset.c_cflag);
	  printf("iflag=$%08x \n",ttyset.c_iflag);
	  printf("oflag=$%08x \n",ttyset.c_oflag);
	  printf("lflag=$%08x \n",ttyset.c_lflag);
#endif
        
        if(tcsetattr(fp,TCSADRAIN,&ttyset)<0)   printf("X: fileno=%d ERROR\n",fp);
        /* set RTS */
        /* Set RTS line. Sometimes dropped. Linux specific? */

	#if defined(TIOCM_RTS) && defined(TIOCMODG)
        {int mcs=0;

        ioctl(fp, TIOCMODG, &mcs);
        mcs |= TIOCM_RTS;
        ioctl(fp, TIOCMODS, &mcs);}
        #endif
        #ifdef _COHERENT
          ioctl(fp, TIOCSRTS, 0);
        #endif

        if(dtr) {
        /*
        * Drop DTR line and raise it again.
        */
#ifdef TIOCSDTR
          ioctl(fp, TIOCCDTR, 0);   
          sleep(1);  /* for one second */
          ioctl(fp, TIOCSDTR, 0);
#endif /* TIOCSDTR */
        }
      } else printf("No TTY: cannot set %s !\n");
  #endif /* WINDOWS */
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
#ifdef HAVE_DLOPEN
      dptr[number]=(FILE *)dlopen((char *)plist[1].pointer,RTLD_LAZY);
      if(dptr[number]==NULL) io_error(errno,"LINK");
      else  filenr[number]=2;
#else
      error(9,"LINK"); /*Function or command %s not yet implemented*/
#endif
#endif
    } 
  }
}
void c_send(PARAMETER *plist, int e) {
  FILE *fff;
  if(e>=2) {
    int sock;
    fff=get_fileptr(plist[0].integer);
    if(fff==NULL) {error(24,"");return;} /* File nicht geoeffnet */    
    sock=fileno(fff);
    if(e>=4) {
      struct sockaddr_in host_address;
      int host_address_size;

      memset((void*)&host_address,0,sizeof(host_address));
      host_address.sin_family=AF_INET;
      host_address.sin_port=htons(plist[3].integer);
      *((unsigned int*)&host_address.sin_addr.s_addr)=plist[2].integer;
      if(sendto(sock,plist[1].pointer,plist[1].integer,0,
	   (struct sockaddr*)&host_address,sizeof(host_address))<0)	
		io_error(errno,"sendto()");
    } else {
      if(send(sock,plist[1].pointer,plist[1].integer,0)<0)	
		io_error(errno,"send()");
    }	
  }
}
void c_receive(PARAMETER *plist, int e) {
  int number;
  FILE *fff;
  char buffer[1500];
	 struct	sockaddr_in	host_address;
	 int	host_address_size;
	 unsigned	char	*address_holder;

  if(e>=2) {
    int len,fdes;
    number=plist[0].integer;
    fff=get_fileptr(number);
    fdes=fileno(fff);
    if(fff==NULL) {error(24,"");return;} /* File nicht geoeffnet */    
    memset((void*)&host_address,0,sizeof(host_address));
    host_address.sin_family=AF_INET;
    host_address_size=sizeof(host_address);
    if((len=recvfrom(fdes,buffer,1500,0,(struct sockaddr*)&host_address,
       &host_address_size))<0) {io_error(errno,"recvfrom()");return;}
address_holder=(unsigned char*)&host_address.sin_addr.s_addr;
#if DEBUG
    printf("Port  obtained %d Bytes message '%s' from host %d.%d.%d.%d.\n",
    len,buffer,address_holder[0],address_holder[1],address_holder[2],
    address_holder[3]);
#endif
    zuweissbuf(plist[1].pointer,buffer,len);
    if(plist[2].integer&INTTYP) *((int *)plist[2].pointer)=host_address.sin_addr.s_addr;
    if(plist[2].integer&FLOATTYP) *((double *)plist[2].pointer)=(double)host_address.sin_addr.s_addr;
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
#ifdef ELNRNG
    { ELNRNG,       -48 }, /* 48: Link number out of range*/
#endif
#ifdef EBADE 
    { EBADE,        -52 }, /* 52: Invalid exchange*/
#endif
#ifdef EXFULL
    { EXFULL,       -54 }, /* 54: Exchange full*/
#endif
#ifdef ENOSTR
    { ENOSTR,       -60 }, /* 60: Device not a stream */
#endif  
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
#ifdef HAVE_DLOPEN
     if(dlclose(dptr[i])==EOF) io_error(errno,"UNLINK");
      else filenr[i]=0;
#endif
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
void c_bload(PARAMETER *plist,int e) {
  int len=-1;
  if(e>=2) {
    if(e==3) len=plist[2].integer;
    if(bload(plist[0].pointer,(char *)plist[1].integer,len)==-1)
      io_error(errno,"BLOAD");
  }
}

void c_bsave(PARAMETER *plist,int e) {
  if(e==3) { 
      if(bsave(plist[0].pointer,(char *)plist[1].integer,plist[2].integer)==-1)
        io_error(errno,"BSAVE");
  }
}
void c_bget(PARAMETER *plist,int e) {
  int i=plist[0].integer;
  if(e==3) {
    if(filenr[i]) fread((char *)plist[1].integer,1,plist[2].integer,dptr[i]);
    else error(24,""); /* File nicht geoeffnet */
  }
}
void c_bput(PARAMETER *plist,int e) {
  int i=plist[0].integer;
  if(e==3) {
    if(filenr[i]) fwrite((char *)plist[1].integer,1,plist[2].integer,dptr[i]);
    else error(24,""); /* File nicht geoeffnet */
  }
}
void c_bmove(PARAMETER *plist,int e) {   /* Memory copy  BMOVE quelladr%,zieladr%,anzahl%    */
  if(e==3) 
    memmove((char *)plist[1].integer,(char *)plist[0].integer,(size_t)plist[2].integer);
}
void c_unget(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  int i,e=wort_sep(n,',',TRUE,v,w);
  FILE *fff=stdin;
  if(e>1) {
    fff=get_fileptr(get_number(v));
    if(fff==NULL) {error(24,v);return;} /* File nicht geoeffnet */
    i=(int)parser(w);
  } else if(e==1) i=(int)parser(v);
  else {error(32,"PUTBACK");return;} /* Syntax error */
  ungetc(i,fff);
}

void c_flush(PARAMETER *plist,int e) {
  FILE *fff=stdout;
  if(e) fff=get_fileptr(plist[0].integer);
  if(fff==NULL) {error(24,"");return;} /* File nicht geoeffnet */  
  if(fflush(fff)) io_error(errno,"FLUSH");
}

void c_seek(PARAMETER *plist,int e) {
  int j=0,i=plist[0].integer;
  if(e>1) j=plist[1].integer;
  if(filenr[i]) {
    if(fseek(dptr[i],j,SEEK_SET)) io_error(errno,"SEEK");
  } else error(24,""); /* File nicht geoeffnet */
}
void c_relseek(PARAMETER *plist,int e) {
  int i=plist[0].integer;
  if(filenr[i]) {
    if(fseek(dptr[i],plist[1].integer,SEEK_CUR)) io_error(errno,"RELSEEK");
  } else error(24,""); /* File nicht geoeffnet */
}

int inp8(PARAMETER *plist,int e) {
  unsigned char ergebnis;
  FILE *fff=get_fileptr(plist[0].integer);
  if(fff==NULL) {error(24,"");return(-1);} /* File nicht geoeffnet */  
  fread(&ergebnis,1,1,fff);
  return((int)ergebnis);
}
int inpf(PARAMETER *plist,int e) {
  if(plist[0].integer==-2) return(kbhit() ? -1 : 0);
  else {
    FILE *fff=get_fileptr(plist[0].integer);
#ifndef WINDOWS
    int fp,i;
#endif
    if(fff==NULL) {error(24,"");return(-1);} /* File nicht geoeffnet */  
    fflush(fff);
#ifndef WINDOWS
    fp=fileno(fff);
    ioctl(fp, FIONREAD, &i);
    return(i); 
#else    
    return(((eof(fff)) ? 0 : -1)); 
#endif  
  }  
}
int inp16(PARAMETER *plist,int e) {
  unsigned short ergebnis;
  FILE *fff=get_fileptr(plist[0].integer);
  if(fff==NULL) {error(24,"");return(-1);} /* File nicht geoeffnet */  
  fread(&ergebnis,sizeof(short),1,fff);
  return((int)ergebnis);
}
int inp32(PARAMETER *plist,int e) {
  unsigned int ergebnis;
  FILE *fff=get_fileptr(plist[0].integer);
  if(fff==NULL) {error(24,"");return(-1);} /* File nicht geoeffnet */ 
  fread(&ergebnis,sizeof(long),1,fff);
  return(ergebnis);
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

int f_symadr(PARAMETER *plist,int e) {
  int adr=0;
  if(filenr[plist[0].integer]==2) {
    char *sym=malloc(plist[1].integer+1);
    char *derror;
    memcpy(sym,plist[1].pointer,plist[1].integer);
    sym[plist[1].integer]=0;
    #ifdef WINDOWS
      adr = (int)GetProcAddress(dptr[plist[0].integer],sym);
      if (adr==0) printf("ERROR: SYM_ADR: %s\n",GetLastError());
    #else
      #ifdef HAVE_DLOPEN
      adr = (int)dlsym(dptr[plist[0].integer],sym);
      if ((derror = (char *)dlerror()) != NULL) printf("ERROR: SYM_ADR: %s\n",derror);
      #else
        adr=-1;
        error(9,"SYM_ADR"); /*Function or command %s not implemented*/
      #endif
    #endif
    free(sym);
  } else error(24,""); /* File nicht geoeffnet */
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
    FILE *fff=get_fileptr(i);

    if(fff!=NULL) {
      e=wort_sep(w,',',TRUE,v,w);
      while(e) {
        typ=type2(v);
	if(typ & ARRAYTYP) {
	  ARRAY zzz=array_parser(v);    
	  for(j=0;j<zzz.dimension;j++) a=a*(((int *)zzz.pointer)[j]);

	  if(zzz.typ & FLOATTYP) {
            double *varptr=(double *)(zzz.pointer+zzz.dimension*INTSIZE);
            
	    fwrite(varptr,sizeof(double),a,fff);
	  } else if(zzz.typ & INTTYP) {
	    int *varptr=(int *)(zzz.pointer+zzz.dimension*INTSIZE);
	    fwrite(varptr,sizeof(int),a,fff);
	  } else if(zzz.typ & STRINGTYP) {
            STRING *varptr=(STRING *)(zzz.pointer+zzz.dimension*INTSIZE);
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

#ifdef TIMEVAL_WORKAROUND
  struct { int  tv_sec; 
           int  tv_usec; } tv;
#else 
    struct timeval tv;
#endif
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
      ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+11);
      sprintf(ergebnis.pointer+ergebnis.len,"\033[%.3d;%.3dH",(int)parser(w3),(int)parser(w4));
      ergebnis.len+=10;
    } else if(strncmp(a1,"TAB(",4)==0) {
      a1[strlen(a1)-1]=0;
      ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+7);
      sprintf(ergebnis.pointer+ergebnis.len,"\033[%.3dC",(int)parser(a1+4));
      ergebnis.len+=6;
    } else if(strncmp(a1,"SPC(",4)==0) {
      int i,j;
      a1[strlen(a1)-1]=0;
      i=(int)parser(a1+4);
      ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+i);
      for(j=ergebnis.len;j<ergebnis.len+i;j++) ergebnis.pointer[j]=' ';
      ergebnis.len+=i;
    } else if(strncmp(a1,"COLOR(",6)==0) {
      int i;
      a1[strlen(a1)-1]=0;
      i=wort_sep(a1+6,',',TRUE,w3,w4);
      ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+15);
      ergebnis.pointer[ergebnis.len++]='\033';
      ergebnis.pointer[ergebnis.len++]='[';      
      sprintf(ergebnis.pointer+ergebnis.len,"%.3d",(int)parser(w3));
      ergebnis.len+=3;
      i=wort_sep(w4,',',TRUE,w3,w4);
      while(i) {
        ergebnis.pointer[ergebnis.len++]=';';
        sprintf(ergebnis.pointer+ergebnis.len,"%.3d",(int)parser(w3));
        ergebnis.len+=3;
        i=wort_sep(w4,',',TRUE,w3,w4);
      }
      ergebnis.pointer[ergebnis.len++]='m';      
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


void memdump(unsigned char *adr,int l) {
  int i;
  printf("\033[35m");
  while(l>16) {
    printf("$%x: ",(int)adr);	
    for(i=0;i<16;i++) printf("%02x ",adr[i]);
    printf(" ");
    for(i=0;i<16;i++) {
      if(adr[i]>31) printf("%c",adr[i]);
      else printf(".");
    }
    printf("\n");
    adr+=16;
    l-=16;
  }
  if(l>0) {
    printf("$%x: ",(int)adr);
    for(i=0;i<16;i++) {
      if(i<l) printf("%02x ",adr[i]);
      else printf("   ");
    }
    printf(" ");
    for(i=0;i<l;i++) {
      if(adr[i]>31) printf("%c",adr[i]);
      else printf(".");
    }
    printf("\n");
  }
  printf("\033[m");
}

/* Sound the speaker */
void speaker(int frequency) {
#ifdef WINDOWS
  Beep(frequency,1);
#else
  int tone;
  int fd=1;
  if(frequency>0) tone=1190000/frequency;
  else tone=0;
  if (strncmp(getenv("TERM"), "xterm", 5) == 0) {
  fd = open("/dev/console", O_WRONLY);
  if(fd<0) {fd=1; io_error(errno,"/dev/console");}
 }
#ifdef KIOCSOUND
  ioctl(fd,KIOCSOUND,tone);
#endif
  if (fd>2) close(fd); /* console */
#endif
}


int f_ioctl(PARAMETER *plist,int e) {
  FILE *fff;
  int ret=0;
  if(e>=2) {
    int sock;
    fff=get_fileptr(plist[0].integer);
    if(fff==NULL) {error(24,"");return;} /* File nicht geoeffnet */    
    sock=fileno(fff);
    if (e==2) ret=ioctl(sock,plist[1].integer);
    else ret=ioctl(sock,plist[1].integer,(void *)plist[2].integer);
    if(ret==-1) io_error(errno,"ioctl");
  }
  return(ret);
}
