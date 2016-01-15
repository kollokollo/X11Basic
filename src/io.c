/* io.c I/O_Routinen  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

/* termio.h (weil obsolet) entfernt.    11.08.2003   MH  */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif

#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "defs.h"
#include <dirent.h>
#include <fnmatch.h>

#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_PTY_H
#include <pty.h>
#endif
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SYS_KD_H
#include <sys/kd.h>
#endif
#ifdef HAVE_USB
#include <usb.h>
// #include <libusb-1.0/libusb.h>
#define TIMEOUT 5000
#endif

#include "x11basic.h"
#include "xbasic.h"
#include "type.h"
#include "parser.h"
#include "svariablen.h"
#include "number.h"
#include "io.h"


#ifdef __hpux
  #define CCTS_OFLOW      0x00010000      /* CTS flow control of output */
  #define CRTS_IFLOW      0x00020000      /* RTS flow control of input */
  #define CRTSCTS         (CRTS_IFLOW|CCTS_OFLOW) /* RTS/CTS flow control */
  #include <sys/termios.h>
#endif
#if defined _WIN32
  #include <ws2tcpip.h>
  #include <winsock2.h>
  #include <windows.h>
  #define WEXITSTATUS(w)    (((w) >> 8) & 0xff)
  #define WIFEXITED(w)      (((w) & 0xff) == 0)
  #define NAME_MAX FILENAME_MAX
  struct winsize {
    unsigned short ws_row;	/* rows, in characters */
    unsigned short ws_col;	/* columns, in characters */
    unsigned short ws_xpixel;	/* horizontal size, pixels */
    unsigned short ws_ypixel;	/* vertical size, pixels */
  };
  #define fsync(a) ;
  #define forkpty(a,b,c,d) (-1)  
  #define FNM_FILE_NAME FNM_PATHNAME
#else
  #include <sys/wait.h> 
  #include <sys/types.h>
  #include <sys/ioctl.h>
  #include <termios.h>
  #include <fcntl.h>

#endif 

#ifndef WINDOWS
#ifndef ATARI
 #include <sys/mman.h>
#endif
#include <netinet/in.h>
#include <netdb.h>
#endif

double sensordata[ANZSENSORS];

/* fuer Dynamisches Linken von shared Object-Files   */

#ifdef HAVE_DLOPEN
#include <dlfcn.h>
#ifdef __hpux
#define RTLD_LAZY       0x00000002  /* bind deferred */
#endif
#endif

#include "file.h"
#include "variablen.h"
#include "parameter.h"

#include "wort_sep.h"
#include "array.h"
#include "sfunctions.h"

#ifdef HAVE_INOTIFY_INIT
#include <sys/inotify.h>
#endif

static int init_sockaddr(struct sockaddr_in *name,const char *hostname, unsigned short int port);
static int make_socket(unsigned short int port);
static int make_UDP_socket(unsigned short int port);

/* get the current cursor position */

#ifndef HAVE_FUTIMENS
  #define futimens(a,b)  (0);   /* not supported  */
#endif

#ifndef UTIME_NOW
  #define UTIME_NOW      ((1l << 30) - 1l)
  #define UTIME_OMIT     ((1l << 30) - 2l) 
#ifndef ANDROID
#ifndef ATARI
  struct timespec {
        time_t tv_sec;        /* seconds */
        long   tv_nsec;       /* nanoseconds */
  };
#endif
#endif
#endif

#if defined ANDROID
extern int lin,col;

void getcrsrowcol(int *_row, int *_col) {
  *_row=lin+1;
  *_col=col+1;
}
#else

void getcrsrowcol(int *_row, int *_col) {
  *_row=0;
  *_col=0;
}
#endif

/* Get the number of rows and columns for this screen. */
#ifdef ANDROID
extern struct winsize win;
void getrowcols(int *rows, int *cols) {
*cols=win.ws_col;
*rows=win.ws_row;
#else
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
#endif
}



/*******************************/
/* Ein- und Ausgaberoutinen    */

FILEINFO get_fileptr(int n) {
  FILEINFO ret;
  ret.typ=FT_FILE; 
  if(n==-1) ret.dptr=stderr;  
  else if(n==-2) ret.dptr=stdin;  
  else if(n==-4) ret.dptr=stdout;  
  else if(n<0 || n>=ANZFILENR) {
    ret.typ=FT_NONE;
    ret.dptr=NULL;
  } else if(filenr[n].typ) return(filenr[n]);
  else {
    ret.typ=FT_NONE;
    ret.dptr=NULL;
  }
  return(ret);
}

int f_freefile() {
  int i=1;
  while(i<ANZFILENR) {
    if(filenr[i].typ==0) return(i);
    i++;
  }
  return(-1);
}


#ifdef WINDOWS
static char fspath[NAME_MAX+1];
#endif
static char fspattern[NAME_MAX+1];
static char fsattr[32];
static DIR *dp=NULL;

STRING f_fsfirsts(PARAMETER *plist,int e) {
#ifdef WINDOWS
  strncpy(fspath,plist->pointer,min(plist->integer+1,NAME_MAX));
#endif
  if(e>1)  strncpy(fspattern,plist[1].pointer,min(plist[1].integer,NAME_MAX));
  else strcpy(fspattern,"*");
  if(e>2)  strncpy(fsattr,plist[2].pointer,min(plist[2].integer,31));
  else strcpy(fsattr,"f");
  // printf("FSFIRST: path=<%s>, pattern=<%s>, attr=<%s>\n",plist->pointer,fspattern,fsattr);
  if(dp) closedir(dp);
  dp=opendir(plist->pointer);
  // printf("OPENDIR: <%s> --> %p\n",plist->pointer,dp);
  return(f_fsnexts());
}
STRING f_fsnexts() {
  STRING ergebnis;
  struct dirent *ep;
  if(dp==NULL) {
    io_error(errno,"fsfirst/fsnext");
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
    ergebnis.pointer[ergebnis.len]=0;
    return(ergebnis);
  } 
//  printf("FSNEXT: pattern=<%s>, attr=<%s>\n",fspattern,fsattr);
  while(1) {
    ep=readdir(dp);
 //   printf("READDIR: --> %p\n",ep);
    if(!ep) {
      ergebnis.pointer=malloc(1);
      ergebnis.len=0;
      ergebnis.pointer[ergebnis.len]=0;
      return(ergebnis);
    }
    if(fnmatch(fspattern,ep->d_name,FNM_NOESCAPE|FNM_PERIOD|FNM_FILE_NAME)==0) break;
  }
  
  ergebnis.pointer=malloc(3+strlen(ep->d_name));
#ifdef WINDOWS
  char filename[NAME_MAX];
  sprintf(filename,"%s/%s",fspath,ep->d_name);
 // printf("FILENAME: <%s>\n",filename);
  struct stat fstats;
  int retc=stat(filename, &fstats);
  if(retc==-1) io_error(errno,filename);
  if(S_ISDIR(fstats.st_mode)) ergebnis.pointer[0]='d';
 // else if(S_ISLNK(fstats.st_mode)) ergebnis.pointer[0]='s';
  else
#elif defined ATARI
  if(0) ;
  else
#else
  if(ep->d_type==DT_DIR) ergebnis.pointer[0]='d';
  else if(ep->d_type==DT_LNK) ergebnis.pointer[0]='s';
  else 
#endif
  ergebnis.pointer[0]='-';    
  ergebnis.pointer[1]=' ';
  strcpy(ergebnis.pointer+2,ep->d_name);
  ergebnis.len=2+strlen(ep->d_name);
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
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
  #ifndef ATARI
      if(msync((void *)plist[0].integer, plist[1].integer,MS_SYNC|MS_INVALIDATE))
        io_error(errno,"msync"); 
  #endif
  #endif
  }
}
void c_unmap(PARAMETER *plist,int e) {
  if(e>=2) {
   #ifndef WINDOWS
   #ifndef ATARI 
      if(munmap((void *)plist[0].integer, plist[1].integer))
        io_error(errno,"munmap"); 
   #endif
   #endif
  }
}
void c_locate(PARAMETER *plist,int e) {
  printf("\033[%.3d;%.3dH",plist->integer+1,plist[1].integer+1);
}
void c_print(PARAMETER *plist,int e) {
  if(e) {
    int i;
    char *v;
    FILEINFO fff=get_fileptr(-4);  /* stdout */

    for(i=0;i<e;i++) {
      switch(plist[i].typ) {
      case PL_EVAL:
      case PL_KEY:
          v=plist[i].pointer;
          if(i==0 && *v=='#') {  /* Sonderbehandlung fuer erstes.. */
            fff=get_fileptr(get_number(v));
            if(fff.typ==0) {xberror(24,v); /* File nicht geoeffnet */return;}
	    if(e==1) fputc('\n',fff.dptr); /* Dann war # der einzige parameter.*/
          } else if(strlen(v)) {
	    STRING buffer=print_arg(v);
	    fwrite(buffer.pointer,1,buffer.len,fff.dptr);
            free(buffer.pointer);
	    if(i!=e-1) fputc('\011',fff.dptr);
	    else {
              if(v[strlen(v)-1]!=';' && v[strlen(v)-1]!='\'') 
	      fputc('\n',fff.dptr);
#ifdef ANDROID
  invalidate_screen();
#endif
            }
	  } else {
	    if(i!=e-1) fputc('\011',fff.dptr);
	  }
	  break;
      case PL_STRING:
        fwrite(plist[i].pointer,1,plist[i].integer,fff.dptr);
        if(i!=e-1) fputc('\011',fff.dptr);
        break;
      case PL_FILENR:
        if(i==0) {
	  fff=get_fileptr(plist[i].integer);
          if(fff.typ==0) {xberror(24,""); /* File nicht geoeffnet */return;}
	  if(e==1) fputc('\n',fff.dptr); /* Dann war # der einzige parameter.*/
	} else xberror(32,"PRINT"); /* Syntax Error */
	break;
      case PL_LEER:
        if(i!=e-1) fputc('\011',fff.dptr);
	break;
      default:
        dump_parameterlist(plist,e);
        xberror(32,"PRINT"); /* Syntax Error */
	return;
      }
    }
  } else {
    putchar('\n');
#ifdef ANDROID
  invalidate_screen();
#endif
  }
}

void c_input(const char *n) {
  char s[strlen(n)+1],t[strlen(n)+1];
  char *u,*v=NULL,*text;
  char inbuf[MAXSTRLEN];
  int e;
  
  FILE *fff=stdin;
  
  if(*n=='#') {
    wort_sep(n,',',TRUE,t,s);
    e=get_number(t);
    if(filenr[e].typ) fff=filenr[e].dptr;
    else xberror(24,""); /* File nicht geoeffnet */
  } else strcpy(s,n);
  
  if(strlen(s)) {   
    e=arg2(s,TRUE,s,t);
    if(*s=='\"') {
      u=s_parser(s);
      text=malloc(strlen(u)+4);
      strcpy(text,u);
      free(u);
      if(e==4) strcat(text," ");
      e=arg2(t,TRUE,s,t);
    } else text=strdup("? ");
    
     /* Wenn Terminal: Jetzt ganze Zeile einlesen */
    if(fff==stdin) v=do_gets(text);
    u=inbuf;
    while(e!=0) {
      xtrim(s,TRUE,s);
      if(fff==stdin) {
        /* It can be that do_gets retuned a NULL because of EOF*/
        if(!v) return;
        // int e2=
	arg2(v,TRUE,u,v);
      /*  while(e2==0) {
	  v=do_gets(text);
	  e2=arg2(v,TRUE,u,v);
	}
       */
      } else u=input(fff,inbuf,MAXSTRLEN);
  //    printf("INPUT, ZUWEIS: <%s> <%s>\n",s,u);
      if(vartype(s)==STRINGTYP) {
        STRING str;
	str.len=strlen(u);
	str.pointer=u;
	str=double_string(&str);
        zuweis_string_and_free(s,str);
      } else xzuweis(s,u);
     
      e=arg2(t,TRUE,s,t);
    }
    free(text);
  }  
}

/*
Es werden beliebig lange Zeilen eingelesen, dafuer wird der buffer mit realloc
vergroessert, wenn noetig. Zurueckgegeben wird Zeile als STRING. 
Liest bis \n oder eof oder 0. Die Laenge kann deshalb mit strlen ermittelt
werden. Zeile wird Nullterminiert
*/

static STRING longlineinput(FILE *n) {   /* liest eine ganze Zeile aus einem ASCII-File ein */
  int c; int i=0;
  STRING line;
  int l=MAXSTRLEN;
  line.pointer=malloc(l+1);
  line.len=0;
  
  while((c=fgetc(n))!=EOF) {
    if(c==(int)'\n') break;
    if(c==0) break;
    (line.pointer)[i++]=(char)c;
    if(i>l) {
      l+=MAXSTRLEN;
      line.pointer=realloc(line.pointer,l+1);
    }
  }
  (line.pointer)[i]='\0';
  line.len=i;
  return(line);
}
STRING f_lineinputs(PARAMETER *plist,int e) {
  int i=plist[0].integer;
  if(filenr[i].typ) {
    return(longlineinput(filenr[i].dptr));
  } else {
    xberror(24,""); /* File nicht geoeffnet */
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
    if(filenr[i].typ) {
      fff=filenr[i].dptr;
      inbuf.pointer=malloc(anz+1);
      inbuf.len=(int)fread(inbuf.pointer,1,anz,fff);
      inbuf.pointer[anz]=0;
      return(inbuf);
    } else xberror(24,""); /* File nicht geoeffnet */
  } else if(e==1) {
     int anz=(int)parser(s);
     inbuf.pointer=malloc(anz+1);
     inbuf.len=(int)fread(inbuf.pointer,1,anz,stdin);
     inbuf.pointer[anz]=0;
     return(inbuf);
  } else xberror(32,"INPUT$"); /* Syntax Error */
  return(vs_error());
}
void c_lineinput(const char *n) {
  char s[strlen(n)+1],t[strlen(n)+1];
  char *u,*text=NULL;
  int e,i=0,typ;
  FILEINFO fff=get_fileptr(-2);   /*   stdin */
  e=arg2(n,TRUE,s,t);
  while(strlen(s)) {
    typ=type(s);
    if(typ==(CONSTTYP|STRINGTYP)) {
       u=s_parser(s);
       if(text) {
         text=realloc(text,strlen(text)+strlen(u)+4);
	 strcpy(text+strlen(text),u);
       } else {
         text=malloc(strlen(u)+4);
	 strcpy(text,u);
       }
       free(u);
      if(e==4) strcat(text," ");
    } else if(i==0 && (typ&FILENRTYP)==FILENRTYP) {
      fff=get_fileptr(get_number(s));
      if(fff.typ==0) {xberror(24,"");return;} /* File nicht geoeffnet */
    } else {
      if(text==NULL) text=strdup("? ");
      if(fff.dptr==stdin) {
        u=do_gets(text);
	if(!u) return;   /*  EOF or such */
        if((typ&TYPMASK)==STRINGTYP) {
	  STRING str;
	  str.len=strlen(u);
	  str.pointer=u;
	  str=double_string(&str);
	  zuweis_string_and_free(s,str);
        } else zuweis(s,parser(u));
      } else {
        STRING a=longlineinput(fff.dptr);
        if((typ&TYPMASK)==STRINGTYP) zuweis_string_and_free(s,a);
        else {
	  zuweis(s,parser(a.pointer));
          free(a.pointer);
        }
      }
      free(text); text=NULL;
    }
    e=arg2(t,TRUE,s,t);
    i++;
  }
  if(text) free(text);
}
/********************/
/* File-Routinen    */

int get_number(const char *w) {
    if(*w=='#') w++;
    return((int)parser(w));
}

/* Internetroutinen */

static int init_sockaddr(struct sockaddr_in *name,const char *hostname, unsigned short int port) {
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
  FILEINFO fff=get_fileptr(plist->integer);
  if(fff.typ==0) xberror(24,"");    /* File nicht geoeffnet */
  else if(fff.typ==FT_USB) {
      /*  TODO  */ 
  } else {
    int sock=fileno(fff.dptr);
    struct sockaddr_in host_address;
    if(init_sockaddr(&host_address,plist[1].pointer,plist[2].integer)<0) io_error(errno,"init_sockadr");
    else { 
      if(0>connect(sock,(struct sockaddr *) &host_address, sizeof(host_address))) 
        io_error(errno,"connect");    
    }
  }
}
static int make_socket(unsigned short int port) {
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
static int make_UDP_socket(unsigned short int port) {
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

#if defined HAVE_USB && defined DEBUG


static void print_endpoint(struct usb_endpoint_descriptor *endpoint) {
  printf(" bEndpointAddress: %02xh\n", endpoint->bEndpointAddress);
  printf(" bmAttributes: %02xh\n", endpoint->bmAttributes);
  printf(" wMaxPacketSize: %d\n", endpoint->wMaxPacketSize);
  printf(" bInterval: %d\n", endpoint->bInterval);
  printf(" bRefresh: %d\n", endpoint->bRefresh);
  printf(" bSynchAddress: %d\n", endpoint->bSynchAddress);
}

static void print_altsetting(struct usb_interface_descriptor *interface) {
  int i;

  printf(" bInterfaceNumber: %d\n", interface->bInterfaceNumber);
  printf(" bAlternateSetting: %d\n", interface->bAlternateSetting);
  printf(" bNumEndpoints: %d\n", interface->bNumEndpoints);
  printf(" bInterfaceClass: %d\n", interface->bInterfaceClass);
  printf(" bInterfaceSubClass: %d\n", interface->bInterfaceSubClass);
  printf(" bInterfaceProtocol: %d\n", interface->bInterfaceProtocol);
  printf(" iInterface: %d\n", interface->iInterface);

  for (i = 0; i < interface->bNumEndpoints; i++)
    print_endpoint(&interface->endpoint[i]);
}


static void print_interface(struct usb_interface *interface) {
  int i;
  for (i = 0; i < interface->num_altsetting; i++)
    print_altsetting(&interface->altsetting[i]);
}

static void print_configuration(struct usb_config_descriptor *config) {
  int i;

  printf(" wTotalLength: %d\n", config->wTotalLength);
  printf(" bNumInterfaces: %d\n", config->bNumInterfaces);
  printf(" bConfigurationValue: %d\n", config->bConfigurationValue);
  printf(" iConfiguration: %d\n", config->iConfiguration);
  printf(" bmAttributes: %02xh\n", config->bmAttributes);
  printf(" MaxPower: %d\n", config->MaxPower);
  for (i = 0; i < config->bNumInterfaces; i++) print_interface(&config->interface[i]);
}
#endif

/* Universelle OPEN-Funktion. Oeffnet Files, Devices, und sockets   */
/* OPEN "I",#1,filename$[,port]   */

//#define DEBUG 1


static struct usb_dev_handle *open_USB_device(const char *filename, int idx) {
#ifdef HAVE_USB
  static int usb_is_init=0;
  static struct usb_bus *busses=NULL;
  struct usb_bus *bus_cur;
  struct usb_device *dev_cur;
  struct usb_device *dev=NULL;
  struct usb_dev_handle *dev_hdl = NULL;
#endif
  int vid=-1,pid=-1,ret;
  char w1[strlen(filename)+1],w2[strlen(filename)+1];
  ret=wort_sep(filename,':',TRUE,w1,w2);
  if(ret==2) {
    vid=(int)parser(w1); /* Vendor id */  
    pid=(int)parser(w2); /* Product id */  
  }
  // printf("OPEN USB: idx=%d, vid=0x%04x, pid=0x%04x\n",idx,vid,pid);

#ifdef HAVE_USB

  if(!usb_is_init) {usb_init();usb_is_init=1;}
  ret=usb_find_busses();
  if(ret<0) {
      printf("usb_find_busses failed with status %i\n", ret);
      errno=ENODEV;
      return(NULL);
  }
  ret=usb_find_devices();
  if(ret<0) {
      printf("usb_find_devices failed with status %i\n", ret);
      errno=ENODEV;
      return(NULL);
  }
  free(busses);
  busses=usb_get_busses();
  for(bus_cur=busses; bus_cur!=NULL; bus_cur=bus_cur->next) {
    for(dev_cur=bus_cur->devices; dev_cur!=NULL; dev_cur=dev_cur->next) {
    #ifdef DEBUG
      printf("Wir haben: %04x/%04x (bus <%s>, device <%s>)\n",dev_cur->descriptor.idVendor,dev_cur->descriptor.idProduct,
        bus_cur->dirname, dev_cur->filename);
    #endif
      if((dev_cur->descriptor.idVendor == vid && dev_cur->descriptor.idProduct == pid)) {
	dev=dev_cur;
#ifdef DEBUG
	printf("found.\n");
#endif
	break;
      }
    }
    if(dev!=NULL) break;
  }
  if(dev==NULL) {
    errno=ENOENT;    /* device not found.*/
    return(NULL);
  }

  dev_hdl=usb_open(dev);
  if(dev_hdl==NULL) {
    printf("usb_open failed: %s\n", usb_strerror());
    errno=ENOENT;
    return(NULL);
  }
#ifdef DEBUG
  if(dev->descriptor.bNumConfigurations) printf("%d Configurations.\n",dev->descriptor.bNumConfigurations);
  if(!dev->config) printf(" Couldn't retrieve descriptors\n");
  else {
    int i;
    for(i=0;i<dev->descriptor.bNumConfigurations;i++) {
      print_configuration(&dev->config[i]);
    }
  }
#endif
  return(dev_hdl);
#else
  printf("The %s function is not implemented \n"
  " in this version of X11-Basic because the LIBUSB library \n"
  " was not present at compile time.\n","OPEN \"Y\"");
  return(NULL);
#endif
}


static FILE *create_socket(int port, const char *modus2) {
#ifdef DEBUG
   printf("Create Socket: modus=%s port=%d\n",modus2,port);
#endif
  int sock=make_socket(port);
  if(sock<0)           { io_error(errno,"make_socket"); return(NULL); }
  if(listen(sock,1)<0) { io_error(errno,"listen");      return(NULL); }
  return(fdopen(sock,modus2));
}
static FILE *connect_socket(int port, const char *modus2, const char *filename) { 
  struct sockaddr_in servername;
#ifdef DEBUG	 
  printf("Open Socket: modus=%s adr=%s port=%d\n",modus2,filename, port);
#endif
  int sock=socket(PF_INET, SOCK_STREAM,0);
  if(sock<0)                                     { io_error(errno,"socket");        return(NULL); }
  if(init_sockaddr(&servername,filename,port)<0) { io_error(errno,"init_sockadr");  return(NULL); }
  if(0>connect(sock,(struct sockaddr *) &servername, sizeof(servername))) { io_error(errno,"connect");  return(NULL); }
  return(fdopen(sock,modus2));
}
static FILE *accept_socket(int port, const char *modus2) { 
#ifdef DEBUG
      printf("Accept Socket #%d: modus=%s adr=%s port=%d\n",number,modus2,filename, port);
#endif
  if(filenr[port].typ==0) {printf("Socket %d nicht geoeffnet.\n",port); io_error(errno,"accept_sockadr");  return(NULL); }
  int sock=fileno(filenr[port].dptr);
  struct sockaddr_in clientname;
  socklen_t size=sizeof(clientname);
  int sock2=accept(sock,(struct sockaddr *) &clientname,&size);
  if(sock2<0) { io_error(errno,"accept"); return(NULL); }
#ifdef DEBUG	   
  printf("Verbindung von %s auf Port $%hd an %d\n",inet_ntoa(clientname.sin_addr), ntohs(clientname.sin_port),sock2);
#endif
  return(fdopen(sock2,modus2));
}

static FILE *create_udp_socket(int port, const char *modus2) {
#ifdef DEBUG
   printf("Create UDP Socket: modus=%s port=%d\n",modus2,port);
#endif
   int sock=make_UDP_socket(port);
   if(sock<0)         { io_error(errno,"make_udp_socket"); return(NULL); }
#if DEBUG
   printf("sock=%d\n",sock);
#endif
  return(fdopen(sock,modus2));
}

#ifndef WINDOWS

/*
Constant  Description
TIOCM_LE  DSR (data set ready/line enable)
TIOCM_DTR DTR (data terminal ready)
TIOCM_RTS RTS (request to send)
TIOCM_ST  Secondary TXD (transmit)
TIOCM_SR  Secondary RXD (receive)
TIOCM_CTS CTS (clear to send)
TIOCM_CAR DCD (data carrier detect)
TIOCM_CD  Synonym for TIOCM_CAR
TIOCM_RNG RNG (ring)
TIOCM_RI  Synonym for TIOCM_RNG
TIOCM_DSR DSR (data set ready)

http://linux.die.net/man/4/tty_ioctl
*/

static void RS232_enable(int fp,int what) {
  int status;
#ifdef TIOCMGET
  if(ioctl(fp, TIOCMGET, &status) == -1) perror("unable to get portstatus");
  status |= what;    /* turn on something */
#endif
#ifdef TIOCMSET
  if(ioctl(fp, TIOCMSET, &status) == -1) 
#endif
  perror("unable to set portstatus");
}
static void RS232_disable(int fp,int what) {
  int status;
#ifdef TIOCMGET
  if(ioctl(fp, TIOCMGET, &status) == -1) perror("unable to get portstatus");
  status &= ~what;    /* turn off something */
#endif
#ifdef TIOCMSET
  if(ioctl(fp, TIOCMSET, &status) == -1) 
#endif
 perror("unable to set portstatus");
}

#endif

static void set_terminal_flags(FILEINFO *finfo, const char *pars, int port) {
  int baud=9600,bits=8,stopbits=1,parity=0,sflow=0,hflow=0,dtr=0;

  /* 'OPEN "UX:9600,N,8,1,DS,CS,RS,CD",#1,"/dev/ttyS1",8+5+6*8 */

  char ww1[strlen(pars)+1],ww2[strlen(pars)+1];
  int ii=0,ee=wort_sep(pars,':',TRUE,ww1,ww2);
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
  printf("baud=%d, bits=%d, stopbits=%d, parity=%d\n",baud,bits,stopbits,parity);
#endif

  int fp=fileno(finfo->dptr);
#ifndef WINDOWS
  struct termios ttyset;
  if(isatty(fp)) {
    setbuf(finfo->dptr,NULL); /* Stream buffering ausschalten */
    /* Nicht-Canoschen Mode setzen */    
    tcgetattr(fp,&ttyset);
    ttyset.c_cc[VMIN]=1;
    ttyset.c_cc[VTIME]=0;
    if(!port) {
/* CBAUD gibt es nicht in allen systemen ! */
#ifdef CBAUD
          port&=~CBAUD;      /* Bits loeschen !  */
#else
	  port=0;
#endif
          switch(baud) {
	  case 50: port|=B50; break;
	  case 75: port|=B75; break;
	  case 110: port|=B110; break;
	  case 134: port|=B134; break;
	  case 150: port|=B150; break;
	  case 200: port|=B200; break;
	  case 300: port|=B300; break;
	  case 600: port|=B600; break;
	  case 1200: port|=B1200; break;
	  case 1800: port|=B1800; break;
	  case 2400: port|=B2400; break;
	  case 4800: port|=B4800; break;
	  case 9600: port|=B9600; break;
#ifdef B19200 /* B19200 */
	  case 19200: port|=B19200; break;
#elif defined EXTA
	  case 19200: port|=EXTA; break;
#endif
#ifdef B38400 /* B38400 */
	  case 38400: port|=B38400; break;
#elif defined EXTB
	  case 38400: port|=EXTB; break;
#endif
#ifdef B57600
	  case 57600: port|=B57600; break;
#endif
#ifdef B115200
	  case 115200: port|=B115200; break;
#endif
#ifdef B230400
	  case 230400: port|=B230400; break;
#endif
#ifdef B460800
	  case 460800: port|=B460800; break;
#endif
#ifdef B500000
	  case 500000: port|=B500000; break;
#endif
#ifdef B576000
	  case 576000: port|=B576000; break;
#endif
#ifdef B921600
	  case 921600: port|=B921600; break;
#endif
#ifdef B1000000
	  case 1000000: port|=B1000000; break;
#endif
          default: printf("Baud rate not supported !\n");
          }
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
        if(tcsetattr(fp,TCSADRAIN,&ttyset)<0)   printf("X: fileno=%d ERROR\n",fp);

    #if defined(TIOCM_RTS) && defined(TIOCMGET)
    RS232_enable(fp,TIOCM_RTS);  /* set RTS */
    #endif

#if defined(TIOCM_DTR) && defined(TIOCMGET)
    /* Sometimes it must be: Drop DTR line and raise it again. */
    if(dtr) RS232_enable(fp,TIOCM_DTR);
    else RS232_disable(fp,TIOCM_DTR);
    
#endif
  } else printf("No TTY: cannot set attributes!\n");
  #else /*  WINDOWS */
  char baudstring[256];
  sprintf(baudstring,"baud=%d data=%d parity=%c stop=%d dtr=%s rts=on",baud,bits,"NOE"[parity],stopbits,((dtr==1)?"on":"off"));
  DCB port_settings;
  memset(&port_settings, 0, sizeof(port_settings));  /* clear the new struct  */
  port_settings.DCBlength = sizeof(port_settings);
  finfo->cport=CreateFileA("COM1",
                      GENERIC_READ|GENERIC_WRITE,
                      0,                          /* no share  */
                      NULL,                       /* no security */
                      OPEN_EXISTING,
                      0,                          /* no threads */
                      NULL);                      /* no templates */

  if(finfo->cport==INVALID_HANDLE_VALUE) {
    printf("unable to open comport\n");
    return;
  }
  if(!BuildCommDCBA(baudstring, &port_settings)) {
    printf("unable to set comport dcb settings\n");
    CloseHandle(finfo->cport);
    return;
  }
  if(!SetCommState(finfo->cport, &port_settings)) {
    printf("unable to set comport cfg settings\n");
    CloseHandle(finfo->cport);
    return;
  }
  COMMTIMEOUTS Cptimeouts;

  Cptimeouts.ReadIntervalTimeout         = MAXDWORD;
  Cptimeouts.ReadTotalTimeoutMultiplier  = 0;
  Cptimeouts.ReadTotalTimeoutConstant    = 0;
  Cptimeouts.WriteTotalTimeoutMultiplier = 0;
  Cptimeouts.WriteTotalTimeoutConstant   = 0;

  if(!SetCommTimeouts(finfo->cport, &Cptimeouts)) {
    printf("unable to set comport time-out settings\n");
    CloseHandle(finfo->cport);
    return;
  }
  #endif /* WINDOWS */
}



/*
   OPEN "AB"
   a= U / I / O / A
   b = C connect to socet
       X open serial
       Y open USB device
       S serve
       U UDP data
  
*/

void c_open(PARAMETER *plist, int e) {
  char *filename=plist[2].pointer;
  int port=0;
  if(e>=4) port=plist[3].integer;
  char modus=toupper(((char *)plist[0].pointer)[0]);
  char special=toupper(((char *)plist[0].pointer)[1]);
  int number=plist[1].integer;  /*File #*/

  if(number>99 || number<1) { xberror(23,"");  /* File # falsch  */        return;}
  if(filenr[number].typ)    { xberror(22,"");  /* File schon geoeffnet  */ return;}

   
  char *modus2;
  if(modus=='I') modus2="rb";
  else if(modus=='O') modus2="wb";
  else if(modus=='U') modus2="rb+";
  else if(modus=='A') modus2="ab+";
  else { xberror(21,""); /* bei Open nur erlaubt ...*/ return;}

#ifdef DEBUG
   printf("OPEN #%d, filename=<%s>, port=%d ($08x), modus=%c, special=%c\n",number,filename,port,port,modus,special);
#endif

  /*  Sockets  */
  if(special=='C')      filenr[number].dptr=connect_socket(port,modus2,filename);   /* Connect */
  else if(special=='S') filenr[number].dptr=create_socket(port,modus2);             /* serve */
  else if(special=='A') filenr[number].dptr=accept_socket(port,modus2);             /* accept */
  else if(special=='U') filenr[number].dptr=create_udp_socket(port,modus2);         /* UDP socket*/
  else if(special=='Y') filenr[number].dptr=(FILE *) open_USB_device(filename,port);     /* USB devices */
  else                  filenr[number].dptr=fopen(filename,modus2);                 /* Normal File */
    
  if(filenr[number].dptr==NULL) {io_error(errno,"OPEN");return;}

  /*  Nachbereitungen / Einstellungen */
  if(special=='X')      {
    set_terminal_flags(&(filenr[number]),plist->pointer,port); /* Fuer Serielle Devices !  */
    filenr[number].typ=FT_DEV;
  } else if(special=='Y') filenr[number].typ=FT_USB;
  else filenr[number].typ=FT_FILE;
}

void c_link(PARAMETER *plist, int e) {
  int number;
  if(e==2) {
    number=plist[0].integer;
    if(filenr[number].typ) xberror(22,"");  /* File schon geoeffnet  */
    else {
#ifdef WINDOWS
      filenr[number].dptr=(FILE *)LoadLibrary(plist[1].pointer);
      if(filenr[number].dptr==NULL) io_error(GetLastError(),"LINK");
      else  filenr[number].typ=FT_DLL;
#else
#ifdef HAVE_DLOPEN
      filenr[number].dptr=(FILE *)dlopen((char *)plist[1].pointer,RTLD_LAZY);
      if(filenr[number].dptr==NULL) io_error(errno,"LINK");
      else filenr[number].typ=FT_DLL;
#else
      xberror(9,"LINK"); /*Function or command %s not implemented*/
#endif
#endif
    } 
  }
}
void c_send(PARAMETER *plist, int e) {
  FILEINFO fff=get_fileptr(plist->integer);
  if(fff.typ==0) xberror(24,""); /* File nicht geoeffnet */ 
  else if(fff.typ==FT_USB) {
#ifdef HAVE_USB
    int ep_out=fff.ep_out;
    if(e>2) ep_out=plist[2].integer;
    int ret=usb_bulk_write((usb_dev_handle *) filenr[plist->integer].dptr,ep_out,plist[1].pointer,plist[1].integer,TIMEOUT);
    if(ret<0) {
      printf("usb_bulk_write failed with code %i: %s\n", ret, usb_strerror());
      xberror(-1,"SEND"); /* IO-ERROR */
    }
#endif
  } else {
    int sock=fileno(fff.dptr);
    if(e>=4) {
      struct sockaddr_in host_address;

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
  FILEINFO fff=get_fileptr(plist->integer);
  if(fff.typ==0) xberror(24,""); /* File nicht geoeffnet */    
  else if(fff.typ==FT_USB) {
    STRING str;
    int ret;
    if(fff.blk_len<=0) fff.blk_len=64;
    str.pointer=malloc(fff.blk_len+1);
#ifdef HAVE_USB
    ret=usb_bulk_read(fff.dptr,fff.ep_in,str.pointer,fff.blk_len,TIMEOUT);
    if(e>2) varcastint(plist[2].integer,plist[2].pointer,ret);
    if(ret<0) {
      printf("usb_bulk_read failed with code %i: %s\n", ret, usb_strerror());
      xberror(-1,"RECEIVE"); /* IO-ERROR */
      str.len=0;
    } else str.len=ret;
#else
    str.len=fff.blk_len;
#endif
    str.pointer[str.len]=0;
    
    varcaststring(plist[1].integer,plist[1].pointer,str);
    free(str.pointer);
  } else {
    int fdes=fileno(fff.dptr);
    struct	sockaddr_in	host_address;
    socklen_t 	host_address_size;
#if DEBUG
  unsigned	char	*address_holder;
#endif
    STRING str;  
    str.pointer=malloc(1500);
    memset((void*)&host_address,0,sizeof(host_address));
    host_address.sin_family=AF_INET;
    host_address_size=sizeof(host_address);
    if((str.len=recvfrom(fdes,str.pointer,1500,0,(struct sockaddr*)&host_address,
       &host_address_size))<0) {io_error(errno,"recvfrom()");free(str.pointer);return;}
#if DEBUG
address_holder=(unsigned char*)&host_address.sin_addr.s_addr;
    printf("Port  obtained %d Bytes message '%s' from host %d.%d.%d.%d.\n",
    str.len,buffer,address_holder[0],address_holder[1],address_holder[2],
    address_holder[3]);
#endif
    varcaststring(plist[1].integer,plist[1].pointer,str);
    if(e>2) varcastint(plist[2].integer,plist[2].pointer,host_address.sin_addr.s_addr);
    free(str.pointer);
  }
}


/* Schliesse alle Files (und sockets und USB verbindungen). macht kein UNLINK. (evtl. TODO)*/
void close_all_files() {
  int i;
  for(i=0;i<ANZFILENR;i++) {
    if(filenr[i].typ==FT_FILE || filenr[i].typ==FT_SOCKET|| filenr[i].typ==FT_PIPE) {
      if(fclose(filenr[i].dptr)==EOF) io_error(errno,"CLOSE");
      else filenr[i].typ=FT_NONE;
    } else if(filenr[i].typ==FT_DEV) {
      #if defined(TIOCM_DTR) && defined(TIOCMGET)
        RS232_disable(fileno(filenr[i].dptr),TIOCM_DTR|TIOCM_RTS);
      #endif
#ifdef WINDOWS
	CloseHandle(filenr[i].cport);        
#endif

      if(fclose(filenr[i].dptr)==EOF) io_error(errno,"CLOSE");
      else filenr[i].typ=FT_NONE;         
    } else if(filenr[i].typ==FT_USB) {
#ifdef HAVE_USB
        usb_close((struct usb_dev_handle *)filenr[i].dptr);
#endif
    }
  }
}


void c_close(PARAMETER *plist,int e) {
  int i;
  if(e) {
    while(--e>=0) {
      i=plist[e].integer;
      if(filenr[i].typ==FT_FILE) { 
        if(fclose(filenr[i].dptr)==EOF) io_error(errno,"CLOSE");
        else filenr[i].typ=FT_NONE;
      } else if(filenr[i].typ==FT_DEV) {
#if defined(TIOCM_DTR) && defined(TIOCM_RTS)
        RS232_disable(fileno(filenr[i].dptr),TIOCM_DTR|TIOCM_RTS);
#endif
        if(fclose(filenr[i].dptr)==EOF) io_error(errno,"CLOSE");
        else filenr[i].typ=FT_NONE; 
#ifdef WINDOWS
	CloseHandle(filenr[i].cport);        
#endif
      } else if(filenr[i].typ==FT_DLL) { 
#ifdef WINDOWS
        if(FreeLibrary(filenr[i].dptr)==0) io_error(GetLastError(),"UNLINK");
        else filenr[i].typ=FT_NONE;
#elif defined HAVE_DLOPEN
        if(dlclose(filenr[i].dptr)==EOF) io_error(errno,"UNLINK");
        else filenr[i].typ=FT_NONE;
#endif
      } else if(filenr[i].typ==FT_USB) {
#ifdef HAVE_USB
        usb_close((struct usb_dev_handle *)filenr[i].dptr);
#endif
        filenr[i].typ=FT_NONE;
      } else xberror(24,""); /* File nicht geoeffnet...*/
    }
  } else close_all_files();
}

#ifndef HAVE_EXECVPE
int execvpe(const char *program, char **argv, char **envp) {
    char **saved = environ;
    int rc;
    environ = envp;
    rc = execvp(program, argv);
    environ = saved;
    return rc;
}
#endif

/*Exec */
void c_exec(PARAMETER *plist,int e) {
  char *newargv[128]; 
  char *newenviron[128];
  char *a=NULL,*b=NULL;
  int ret,ee,anzargs=0,anzenv=0;
#ifdef ANDROID
  if(strncmp(plist->pointer,"android.",8)==FALSE) {
    if(e>1) a=plist[1].pointer;
    if(e>2) b=plist[2].pointer;
    ANDROID_call_intent(plist->pointer,a,b);
    return;
  }
/* untersuche, ob es sich um ein intent handelt*/
#endif
  if(e>1) {
    newargv[anzargs++]=plist->pointer;
    
    ee=wort_sep_destroy(plist[1].pointer,'\n',0,&a,&b);
    while(ee && anzargs<127) {
      newargv[anzargs++]=a;
      ee=wort_sep_destroy(b,'\n',0,&a,&b);
    }
  }
  
  if(e>2) {
    ee=wort_sep_destroy(plist[2].pointer,'\n',0,&a,&b);
    while(ee && anzenv<127) {
      newenviron[anzenv++]=a;
      ee=wort_sep_destroy(b,'\n',0,&a,&b);
    }
  }
  
  newargv[anzargs]=NULL;
  newenviron[anzenv]=NULL;
 
#ifdef WINDOWS
  _flushall();
  errno = 0;
  ret=spawnv(_P_WAIT,plist->pointer, newargv);
  if (errno) ret=-1;
#else
  if(e>2) ret=execvpe(plist->pointer,newargv,newenviron);
  else ret=execvp(plist->pointer,newargv);
#endif
  if(ret==-1) io_error(errno,"EXEC");
}

/*Diese funktion liefert den Rueckgabewert aus exit zurueck*/

int f_exec(PARAMETER *plist,int e) {
  int statval;

#ifdef ANDROID
  char *a=NULL,*b=NULL;
/* untersuche, ob es sich um ein intent handelt*/
  if(strncmp(plist->pointer,"android.",8)==FALSE) {
    if(e>1) a=plist[1].pointer;
    if(e>2) b=plist[2].pointer;
    ANDROID_call_intent(plist->pointer,a,b);
    return(ANDROID_waitfor_intentresult());
  }
#endif

  
#ifndef WINDOWS
  if(fork() == 0) {
    c_exec(plist,e);
    exit(-1); /*we come here only in case of error */
  } else {
    wait(&statval);
    if(WIFEXITED(statval)) return(WEXITSTATUS(statval));
    else return(-1); // Child did not terminate with exit.
  }
#endif
  return 0;  /* we never come here*/
}

/* Fuehrt Code an Adresse aus */
void c_call(PARAMETER *plist,int e) { f_call(plist,e);}
int f_call(PARAMETER *plist,int e) {
  typedef struct  {int feld[20];} GTT;
  int (*adr)(GTT)=(int (*)())plist->integer;
  // printf("call 0x%x mit %d args.\n",plist->integer,e);
  if(e>20) xberror(45,"CALL"); /* Zu viele Parameter */
  else if(adr==NULL) xberror(29,"CALL"); /* illegal address */
  else {
    int i;
    GTT gtt;
#define w1 ((char *)plist[i].pointer)
    for(i=1;i<e;i++) {
      switch(plist[i].typ) {
      case PL_EVAL:
      //	printf("arg: %s \n",(char *)plist[i].pointer);
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
#undef w1
        break;
      default:
        xberror(32,"CALL"); /* Syntax error */
      }
    }
    return(adr(gtt));
  }
  return(0);
}
void c_bload(PARAMETER *plist,int e) {
  int len=-1;
  if(e>2) len=plist[2].integer;
  if(bload(plist->pointer,(char *)plist[1].integer,len)==-1) io_error(errno,"BLOAD");
}

void c_bsave(PARAMETER *plist,int e) {
  if(bsave(plist->pointer,(char *)plist[1].integer,plist[2].integer)==-1) io_error(errno,"BSAVE");
}
void c_bget(PARAMETER *plist,int e) {
  int i=plist->integer;
  if(filenr[i].typ) {
    e=fread((char *)plist[1].integer,1,plist[2].integer,filenr[i].dptr);
    if(e<plist[2].integer) xberror(26,""); /* Fileende erreicht EOF */
  } else xberror(24,""); /* File nicht geoeffnet */
}
void c_bput(PARAMETER *plist,int e) {
  int i=plist->integer;
  if(filenr[i].typ) fwrite((char *)plist[1].integer,1,plist[2].integer,filenr[i].dptr);
  else xberror(24,""); /* File nicht geoeffnet */
}
void c_bmove(PARAMETER *plist,int e) {   /* Memory copy  BMOVE quelladr%,zieladr%,anzahl%    */
  memmove((char *)plist[1].integer,(char *)plist[0].integer,(size_t)plist[2].integer);
}
void c_pipe(PARAMETER *plist,int e) {
  int i=plist[0].integer;
  int j=plist[1].integer;
  if(filenr[i].typ || filenr[j].typ) xberror(22,"");  /* File schon geoeffnet  */
  else {
    int filedes[2];
#ifndef WINDOWS
    if(pipe(filedes)) io_error(errno,"PIPE");
    else {
        filenr[i].dptr=fdopen(filedes[0],"r");
	if(filenr[i].dptr==NULL) io_error(errno,"PIPE");
	else filenr[i].typ=FT_FILE;
        filenr[j].dptr=fdopen(filedes[1],"w");
	if(filenr[j].dptr==NULL) io_error(errno,"PIPE");
	else filenr[j].typ=FT_FILE;
    }
#else
    io_error(errno,"PIPE");
#endif
  }
}



void c_unget(PARAMETER *plist,int e) {
  FILEINFO fff;
  if(plist->typ==PL_LEER) {fff.dptr=stdin;fff.typ=FT_FILE;}
  else fff=get_fileptr(plist->integer);
  if(fff.typ==0) xberror(24,""); /* File nicht geoeffnet */
  else ungetc(plist[1].integer,fff.dptr);
}

void c_flush(PARAMETER *plist,int e) {
  FILEINFO fff;
  if(e) fff=get_fileptr(plist->integer);
  else {
    fff=get_fileptr(-4);  /*  stdout */
#ifdef ANDROID
    invalidate_screen(); 
#endif
  }
  if(fff.typ==0) {xberror(24,"");return;} /* File nicht geoeffnet */  
  if(fflush(fff.dptr)) io_error(errno,"FLUSH");
}

void c_seek(PARAMETER *plist,int e) {
  int j=0,i=plist[0].integer;
  if(e>1) j=plist[1].integer;
  if(filenr[i].typ) {
    if(fseek(filenr[i].dptr,j,SEEK_SET)) io_error(errno,"SEEK");
  } else xberror(24,""); /* File nicht geoeffnet */
}
void c_relseek(PARAMETER *plist,int e) {
  int i=plist[0].integer;
  if(filenr[i].typ) {
    if(fseek(filenr[i].dptr,plist[1].integer,SEEK_CUR)) io_error(errno,"RELSEEK");
  } else xberror(24,""); /* File nicht geoeffnet */
}

void touch(PARAMETER *plist,int e) {
  if(plist->integer>0) {
    FILEINFO fff=get_fileptr(plist->integer);
    if(fff.typ==0) {xberror(24,"");return;} /* File nicht geoeffnet */ 
    int fp=fileno(fff.dptr);
    struct timespec ts[2];
    ts[0].tv_nsec=UTIME_NOW;
    ts[1].tv_nsec=UTIME_NOW;
#ifndef ATARI
    int ret=futimens(fp, ts);
    if(ret==-1) io_error(errno,"touch");
#endif
  }
}


int inp8(PARAMETER *plist,int e) {
#ifdef WINDOWS
  if(plist->integer==-2) return(getch());
#endif
  unsigned char ergebnis;
  FILEINFO fff=get_fileptr(plist->integer);
  if(fff.typ==0) {xberror(24,"");return(-1);} /* File nicht geoeffnet */  
  if(fread(&ergebnis,1,1,fff.dptr)<1) {xberror(26,"");return(-1);}/* Fileende erreicht EOF */  
  return((int)ergebnis);
}
int inpf(PARAMETER *plist,int e) {
  if(plist->integer==-2) return(kbhit() ? -1 : 0);
  FILEINFO fff=get_fileptr(plist->integer);
  if(fff.typ==0) {xberror(24,"");return(-1);} /* File nicht geoeffnet */  
  fflush(fff.dptr);
#ifndef WINDOWS
  int i;
  ioctl(fileno(fff.dptr), FIONREAD, &i);
  return(i); 
#else    
  return(((feof(fff.dptr)) ? 0 : -1)); 
#endif  
}
int inp16(PARAMETER *plist,int e) {
  unsigned short ergebnis;
  FILEINFO fff=get_fileptr(plist->integer);
  if(fff.typ==0) {xberror(24,"");return(-1);} /* File nicht geoeffnet */  
  if(fread(&ergebnis,sizeof(short),1,fff.dptr)<1)  io_error(errno,"fread");
  return((int)ergebnis);
}
int inp32(PARAMETER *plist,int e) {
  unsigned int ergebnis;
  FILEINFO fff=get_fileptr(plist->integer);
  if(fff.typ==0) {xberror(24,"");return(-1);} /* File nicht geoeffnet */ 
  if(fread(&ergebnis,sizeof(long),1,fff.dptr)<1)  io_error(errno,"fread");
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

#ifdef ANDROID
   int termecho=1;
#endif

void set_input_mode_echo(int onoff) {
#ifdef ANDROID
  termecho=onoff;
#else
#ifndef WINDOWS
  struct termios tattr;
  if(isatty(STDIN_FILENO)) {
    tcgetattr(STDIN_FILENO,&tattr);
    if(onoff) tattr.c_lflag |= ECHO;
    else tattr.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO,TCSAFLUSH,&tattr);
  }
#endif
#endif
}
void reset_input_mode() {
#ifndef WINDOWS
  if(isatty(STDIN_FILENO)) tcsetattr(STDIN_FILENO,TCSANOW,&saved_attributes);
#endif
}

/* Dynamisches Linken von Shared-Object-Files */

int f_symadr(PARAMETER *plist,int e) {
  long adr=0;
  if(filenr[plist->integer].typ==FT_DLL) {
    char *sym=malloc(plist[1].integer+1);
    char *derror;
    memcpy(sym,plist[1].pointer,plist[1].integer);
    sym[plist[1].integer]=0;
    #ifdef WINDOWS
      adr = (long)GetProcAddress(filenr[plist[0].integer].dptr,sym);
      if (adr==0) printf("ERROR: SYM_ADR: %s\n",GetLastError());
    #else
      #ifdef HAVE_DLOPEN
      adr = (long)dlsym(filenr[plist[0].integer].dptr,sym);
      if ((derror = (char *)dlerror()) != NULL) printf("ERROR: SYM_ADR: %s\n",derror);
      #else
        adr=-1;
        xberror(9,"SYM_ADR"); /*Function or command %s not implemented*/
      #endif
    #endif
    free(sym);
  } else xberror(24,""); /* File nicht geoeffnet */
  return(adr);
}


char *terminalname(int fp) {
  char *name="";
  #ifndef WINDOWS
  if(isatty(fp)) name=ttyname(fp);
  return(strdup(name));
  #else
  return(strdup("windows-term"));
  #endif
}

/*  OUT #1,a       */

void c_out(PARAMETER *plist,int e) {
  FILEINFO fff;  
  if(plist->typ==PL_FILENR ||plist->typ==PL_INT) fff=get_fileptr(plist->integer);
  else if(plist->typ==PL_LEER) fff=get_fileptr(-4);  /* stdout */ 
  else {xberror(24,""); /* File nicht geoeffnet */ return;}
  if(fff.typ==0) {xberror(24,""); /* File nicht geoeffnet */ return;}

  // printf("OUT #%d mit %d args.\n",plist->integer,e);
  // dump_parameterlist(plist,e);
  int i;
  unsigned char c;
  for(i=1;i<e;i++) {
    switch(plist[i].typ) {
      case PL_INT:
        c=plist[i].integer;
        fwrite(&c,sizeof(char),1,fff.dptr);
        break;
      case PL_FLOAT:
      case PL_COMPLEX:
        c=(unsigned char)(int)plist[i].real;
        fwrite(&c,sizeof(char),1,fff.dptr);
        break;
      case PL_STRING:
        fwrite(plist[i].pointer,sizeof(char),plist[i].integer,fff.dptr);
        break;
      default:
        dump_parameterlist(plist,e);
        xberror(32,"OUT"); /* Syntax error */
	return;
    }
  }
#ifdef ANDROID
  if(plist->typ==PL_LEER) invalidate_screen();
#endif
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
   while(kbhit() && i<MAXSTRLEN-1) 
#ifdef WINDOWS
     /*getch() unter windows (im Unterschied zu getc()) wartet nicht auf ein 
       return und es gibt kein echo der Zeichen auf die Konsole */
     ik[i++]=getch();
#else
     ik[i++]=getc(stdin);
#endif
   ik[i]=0;
   return(ik);
}


/* Baut einen String aus der Argumenteliste des PRINT-Kommandos zusammen */

STRING print_arg(const char *ausdruck) {
  int e;
  char *a1,w1[strlen(ausdruck)+1],w2[strlen(ausdruck)+1];
  char w3[strlen(ausdruck)+1],w4[strlen(ausdruck)+1];
// printf("print_arg: >%s<\n",ausdruck);
  STRING ergebnis;
  ergebnis.pointer=malloc(4);
  ergebnis.len=0;
  e=arg2(ausdruck,TRUE,w1,w2);
  while(e) {
    a1=indirekt2(w1);
// printf("a1=<%s>\n",a1);
  /*  printf("TEST: <%s> <%s> %d\n",w1,w2,e);*/
    if(strncmp(a1,"AT(",3)==0) {
      a1[strlen(a1)-1]=0;
      wort_sep(a1+3,',',TRUE,w3,w4);
      int x=max(0,(int)parser(w3));
      int y=max(0,(int)parser(w4));
      ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+11);
      sprintf(ergebnis.pointer+ergebnis.len,"\033[%.3d;%.3dH",x,y);
      ergebnis.len+=10;
    } else if(strncmp(a1,"TAB(",4)==0) {
      a1[strlen(a1)-1]=0;
      ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+8);
      int x=max(0,(int)parser(a1+4));
      if(x) {
        sprintf(ergebnis.pointer+ergebnis.len,"\r\033[%.3dC",x);
        ergebnis.len+=7;
      } else {
        sprintf(ergebnis.pointer+ergebnis.len,"\r");
        ergebnis.len+=1;     
      }
    } else if(strncmp(a1,"SPC(",4)==0) {
      a1[strlen(a1)-1]=0;
      int x=max(0,(int)parser(a1+4));
      ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+7);
      if(x) {
        sprintf(ergebnis.pointer+ergebnis.len,"\033[%.3dC",x);
        ergebnis.len+=6;
      }
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
	typ=type(a1);
	
	if(typ&ARRAYTYP) {/* Hier koennte man .... */
	  if((typ&TYPMASK)==STRINGTYP) ;
	  else ;
	  printf("not implemented yet: print array.\n");
	  break;

	
	} else {
	
	switch(typ&TYPMASK) {
	case STRINGTYP:
	  {
          STRING a3=string_parser(a1);
	  if(ee==2) {
	    int i;
	    STRING e2=string_parser(w4);
	    if(e2.len<a3.len) {
	      for(i=0;i<e2.len;i++) e2.pointer[i]='*';
	    } else if(*(e2.pointer)=='<') {
	      for(i=0;i<e2.len;i++) {
	        if(i<a3.len) e2.pointer[i]=a3.pointer[i];
	        else e2.pointer[i]=' ';
	      }	     
	    } else if(*(e2.pointer)=='>') {
	      for(i=0;i<e2.len;i++) {
	        if(i>=e2.len-a3.len) e2.pointer[i]=a3.pointer[i-(e2.len-a3.len)];
	        else e2.pointer[i]=' ';
	      }	     
	    } else {
	      int a=(e2.len-a3.len)>>1;
	      for(i=0;i<e2.len;i++) {
	        if(i>=a && i<a+a3.len) e2.pointer[i]=a3.pointer[i-a];
	        else e2.pointer[i]=' ';
	      }  
	    }
	    ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+e2.len+1);
            memcpy(ergebnis.pointer+ergebnis.len,e2.pointer,e2.len);
	    ergebnis.len+=e2.len;
	    free(e2.pointer);
          } else {
	    ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+a3.len+1);
            memcpy(ergebnis.pointer+ergebnis.len,a3.pointer,a3.len);
	    ergebnis.len+=a3.len;
	  }
	  ergebnis.pointer[ergebnis.len]=0;
	  free(a3.pointer);
	  }
	  break;
	case COMPLEXTYP:
	  {
	  COMPLEX a=complex_parser(a1);
	  STRING b=COMPLEXtoSTRING(a);
	  ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+b.len+1);
	  memcpy(ergebnis.pointer+ergebnis.len,b.pointer,b.len);
	  ergebnis.len+=b.len;
	  free(b.pointer);
	  }
	  break;
	case ARBINTTYP:
	  {
	  ARBINT a;
	  mpz_init(a);
	  arbint_parser(a1,a);
	  char *c=mpz_get_str(NULL,10,a);
	  int l=strlen(c);
	  ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+l+1);
	  memcpy(ergebnis.pointer+ergebnis.len,c,l);
	  ergebnis.len+=l;
	  free(c);
	  mpz_clear(a);
	  }
          break;
	case ARBFLOATTYP:
	case ARBCOMPLEXTYP:
	  printf("not implemented yet: print arbitrary precision number.\n");
	  break;
        default:
	  if(ee==2) {
	    STRING a3=string_parser(w4);
	    STRING e2=do_using(parser(a1),a3);
	    ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+e2.len+1);
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
/* Fuer den Teil nach dem Komma und fuer Exponenten*/
static void xfill(char *p,const char *q,char c, int n) {
  while(*p && n--) {
    if(*p==c) {
      if(*q) *p=*q++;
      else if(c=='#') *p='0';
    }
    p++;
  }
}
/*Fuer den Teil vor dem Komma*/
static void xfillx(char *p,const char *q,int n) {
  char c=0,c2;
  char f=' ';
  while(*p && n--) {
    c2=c;
    c=*q;
    switch(*p) {
    case '$':
      if(c==' ') {
        int i=1;
	while(*(p+i)==',') i++;
        if(*(p+i)=='$' && *(q+1)==' ') *p=f;
        q++;
      } else if(c) {*p=c;q++;}
      p++;
      break;
    case '*':
      if(c==' ') {*p=f='*';q++;}
      else if(c) {*p=c;q++;}
      p++;
      break;
    case '0':
    case '%':
      if(c==' ') {*p=c='0';q++;f=' ';}
      else if(c) {*p=c;q++;}
      p++;
      break;
    case '#':
      if(c) {*p=c;q++;f=' ';}
      p++;
      break;
    case ',':
      if(c2==' ') *p=f;
      p++;
      break;
    default:
      p++;
    }
  }
}

/* print 0 using "+#.###^^^^"   */
STRING do_using(double num,STRING format) {
  STRING dest;
  int a=0,b=0,p,r=0,i,j,ex=0,v=0; 
  int neg;
  char des[32+format.len];

  
//  printf("DO__USING: %13.13g, <%s>\n",num,format.pointer);
  if (*format.pointer=='%') { /* c-style format */
    char b[32];
    sprintf(b,format.pointer,num);
    dest.len=strlen(b);
    dest.pointer=strdup(b);
  } else { /* basic-style format */
    dest=double_string(&format);
    
   /* Zaehle die Rauten vor dem Punkt */
   p=0;
   while((format.pointer)[p] && (format.pointer)[p]!='.') {
     if((format.pointer)[p]=='#' || 
        (format.pointer)[p]=='%' || 
        (format.pointer)[p]=='$' || 
        (format.pointer)[p]=='0' || 
	(format.pointer)[p]=='*') r++;
     p++;
   }
   /* Zaehle die Rauten nach dem Punkt */
   while((format.pointer)[p]) {
     if((format.pointer)[p++]=='#') a++;
   }
   /* Zaehle platzhalter für Exponentialdarstellung */
   p=0;
   while(p<format.len) {
     if((format.pointer)[p++]=='^') ex++;
   }
   /* Zaehle platzhalter für vorzeichen */
   p=0;
   while(p<format.len) {
     if((format.pointer)[p]=='+' || (format.pointer)[p]=='-') v++;
     p++;
   }
   neg=(num<0); 
   num=fabs(num);
 //  printf("Rauten vor Punkt: %d, rauten danach: %d, exponent: %d, vorzeichen: %d\n",r,a,ex,v);

   /* Vorzeichen als erstes: */
   for(i=0;i<dest.len;i++) {
     if(format.pointer[i]=='+') {dest.pointer[i]=(neg ? '-':'+');}
     else if(format.pointer[i]=='-') {dest.pointer[i]=(neg ? '-':' ');}
   }
   //printf("destpointer: <%s>\n",dest.pointer);

   
   if(ex>2) {
     sprintf(des,"%16.16e",num); 
     // printf("preformat: <%s>\n",des);
     j=i=0;
     while(des[i] && des[i]!='e') i++;
     while(dest.pointer[j] && dest.pointer[j]!='^') j++;
     des[i++]=0;
     dest.pointer[j++]='e';   /*e */
     while(dest.pointer[j] && dest.pointer[j]!='^') j++;
     dest.pointer[j++]=des[i++];   /* +*/
     while(dest.pointer[j] && dest.pointer[j]!='^') j++;
     int l=strlen(&des[i])+2;
     while(l<ex) {
       dest.pointer[j++]='0'; 
       while(dest.pointer[j] && dest.pointer[j]!='^') j++;
       l++;
     }
     if(l>ex) {
       for(i=0;i<dest.len;i++) dest.pointer[i]='*';
       return(dest);    
     } else xfill(dest.pointer,&des[i],'^',dest.len);
   } else sprintf(des,"%16.16f",num);
   //  printf("preformat: <%s>\n",des); 
     /*Jetzt muss die Zahl gerundet werden.*/
     num=myatof(des)+pow(0.1,a)*0.5;
     sprintf(des,"%16.16f",num);
   //  printf("preformat2: <%s>\n",des); 
    /*Hierzu brauchen wir die Anzahl der tatsaechlichen STellen vor dem Komma*/
    int count=0;
    i=0;
    while(des[i] && des[i]!='.') {
      if(des[i]>='0' && des[i]<='9') count++;
      i++;
    }
 //   printf("%d Stellen bis Punkt, davon %d signifikant.\n",i,count);
 //   printf("des=<%s>\n",des);
    i=0;
    while(des[i] && des[i]!='.') i++; 
    j=0;
    // printf("destpointer=<%s>\n",dest.pointer);
    while(dest.pointer[j] && dest.pointer[j]!='.') j++;
    if(dest.pointer[j]) {
      if(des[i]) {
        des[i]=0;
        xfill(dest.pointer+j+1,&des[i+1],'#',dest.len-j-1);
      } else xfill(dest.pointer+j+1,"0000000000000000",'#',dest.len-j-1);
    }
    // printf("destpointer=<%s>\n",dest.pointer);
    
    b=0;
    /*Jetzt noch Leerzeichen am Anfang entfernen und ggf minus einfügen.*/
    
    char p[strlen(des)+1+1];
    char *p2=des;
    // printf("des=<%s>\n",des);
    if(neg && !v) p[b++]='-';
    neg=0;
    
    while(*p2 && *p2!='.') {
      if(*p2!=' ') p[b++]=*p2;
      p2++;
    }
    p[b]=0;
    // printf("Verbleiben: <%s> b=%d fuer %d stellen\n",p,b,r);
    if(b==r) xfillx(dest.pointer,p,dest.len);
    else if(b<r) {
      char buf[r+1];
      for(i=0;i<r-b;i++)  buf[i]=' ';
      for(i=r-b;i<r;i++)  buf[i]=p[i-(r-b)];
      buf[r]=0;  
      // printf("buf=<%s>\n",buf);

      xfillx(dest.pointer,buf,dest.len);
    } else {
      for(i=0;i<dest.len;i++) dest.pointer[i]='*';    
    }
  }
  return(dest);
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

/* IOCTL #n,rec[,pointer]   implementation on normal files/sockets and USB devices...*/

int f_ioctl(PARAMETER *plist,int e) {
  int ret=-1;
  FILEINFO fff=get_fileptr(plist->integer);
  if(fff.typ==FT_NONE) {
    xberror(24,""); /* File nicht geoeffnet */    
    return(ret);
  } else if(fff.typ==FT_USB) {
    // printf("IOCTL on USB device: %d\n",plist[1].integer);
#ifdef HAVE_USB
    if(plist[1].integer==0) {  /*USB reset*/
       ret=usb_reset((usb_dev_handle *) filenr[plist->integer].dptr);
       return(ret);
    }
    if(e<=2) return(-1);  /*Ab jetzt bracuhen wir einen zweiten Parameter/Datenpointer*/
    switch(plist[1].integer) {
    case 0: ret=usb_reset((usb_dev_handle *) filenr[plist->integer].dptr); break;  /* USB Reset*/
    case 1:   /* get descriptor data structure*/
      { struct usb_device *a=usb_device((usb_dev_handle *) filenr[plist->integer].dptr);
     	
  	  memcpy((void *)plist[2].integer,a,sizeof(struct usb_device));
          ret=sizeof(struct usb_device);
        break; }
    case 2: /* Set configuration */
      ret=usb_set_configuration((usb_dev_handle *) filenr[plist->integer].dptr, plist[2].integer);
      break;
    case 3: /* Claim Interface */
      ret=usb_claim_interface((usb_dev_handle *) filenr[plist->integer].dptr, plist[2].integer);
      break;
    case 4:
      { int *params=(int *)plist[2].integer;
        void *data=NULL;
	// printf("cm: %d %d %d %d\n",params[0],params[1],params[2],params[3]);
	if(params[5]>0) data=(void *)&params[6];
        ret=usb_control_msg((usb_dev_handle *) filenr[plist->integer].dptr, params[0],params[1],params[2],params[3],data,params[5],params[4]);
      }
      break;
    case 5: filenr[plist->integer].blk_len=plist[2].integer;ret=0; break; /*  Set default blk_len */
    case 6: filenr[plist->integer].ep_in=plist[2].integer;  ret=0; break; /*  Set ep_in */
    case 7: filenr[plist->integer].ep_out=plist[2].integer; ret=0; break; /*  Set ep_out */
    case 12: {  /* get filename+path */
      	struct usb_device *a=usb_device((usb_dev_handle *) filenr[plist->integer].dptr);
    	strncpy((void *)plist[2].integer,a->filename,sizeof(a->filename));
        ret=strlen(a->filename);
      } break;
    case 13: {  /* get manufacturer */
      	struct usb_device *a=usb_device((usb_dev_handle *) filenr[plist->integer].dptr);
        if(a->descriptor.iManufacturer) 
	  ret=usb_get_string_simple((usb_dev_handle *) filenr[plist->integer].dptr, a->descriptor.iManufacturer, (void *)plist[2].integer, 100);
      } break;
    case 14: {  /* get Product name */
      	struct usb_device *a=usb_device((usb_dev_handle *) filenr[plist->integer].dptr);
        if(a->descriptor.iProduct) 
	  ret=usb_get_string_simple((usb_dev_handle *) filenr[plist->integer].dptr, a->descriptor.iProduct, (void *)plist[2].integer, 100);
      } break;
    case 15: {  /* get Serial number */
        struct usb_device *a=usb_device((usb_dev_handle *) filenr[plist->integer].dptr);
        if(a->descriptor.iSerialNumber) 
          ret=usb_get_string_simple((usb_dev_handle *) filenr[plist->integer].dptr, a->descriptor.iSerialNumber, (void *)plist[2].integer, 100);
      } break;
    case 16:   /* get error text */
      strncpy((void *)plist[2].integer,usb_strerror(),100);
      break;
    default:
      ret=-1;
    }
#endif
    return(ret);
  }
  
  int sock=fileno(fff.dptr);
#ifndef WINDOWS
#ifdef ATARI
  if(e==2) ret=ioctl(sock,plist[1].integer,NULL);
#else
  if(e==2) ret=ioctl(sock,plist[1].integer);
#endif
  else ret=ioctl(sock,plist[1].integer,(void *)plist[2].integer);
  if(ret==-1) 
#endif
    io_error(errno,"ioctl");
  return(ret);
}


void c_chdir(PARAMETER *plist,int e) {
  if(chdir(plist->pointer)==-1) io_error(errno,"chdir");
}

void c_mkdir(PARAMETER *plist,int e) {
#ifndef WINDOWS
  mode_t mode=S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
  if(e>1) mode=plist[1].integer;
  if(mkdir(plist->pointer,mode)==-1) io_error(errno,"mkdir");
#else
  if(mkdir(plist->pointer)==-1) io_error(errno,"mkdir");
#endif
}


void c_rmdir(PARAMETER *plist,int e) {
  if(rmdir(plist->pointer)==-1) io_error(errno,plist->pointer);
}


void c_kill(PARAMETER *plist,int e) {
  if(unlink(plist->pointer)==-1) io_error(errno,plist->pointer);
}
void c_rename(PARAMETER *plist,int e) {
  if(rename(plist->pointer,plist[1].pointer)==-1) io_error(errno,plist->pointer);
}
void c_chmod(PARAMETER *plist,int e) {
  if(chmod(plist->pointer,plist[1].integer)==-1) io_error(errno,plist->pointer);
}



static int watch_fd=-2;
void c_watch(PARAMETER *plist, int e) {
#ifdef HAVE_INOTIFY_INIT
  if(watch_fd==-2) watch_fd=inotify_init();
  if(watch_fd<0) io_error(errno,"WATCH");
  else {
    int wd=inotify_add_watch(watch_fd,plist[0].pointer,IN_ALL_EVENTS);
    if(wd<0) io_error(errno,"WATCH");  
    else printf("Watch descriptor for %s is: %d.\n",(char *)plist[0].pointer,wd);
  }
 #else
  if(watch_fd<0) xberror(9,"WATCH"); /* WATCH is not available in this implementation. */
#endif
}
/*If we ever want an UNWATCH function, we have to store a table with 
  filenames associated with watch desciptors.*/

char *fileevent() {
  char *erg=malloc(1);
  erg[0]=0;
  if(watch_fd<0) {
  } else {
#ifdef HAVE_INOTIFY_INIT
  #define EVENT_SIZE  (sizeof(struct inotify_event))
  #define BUF_LEN     (1024*(EVENT_SIZE+16))
  char buf[BUF_LEN];
  int len=read(watch_fd, buf, BUF_LEN);
  struct inotify_event *event;
  int i=0;
  char fields[5]="--- ";
  while(i<len) {
    fields[0]='-';
    fields[1]='-';
    fields[2]='-';
    event=(struct inotify_event *)&buf[i];
    if(event->mask&IN_ISDIR) fields[0]='d';
    if(event->mask&IN_ACCESS) fields[2]='r'; /* ACCESS/r */
    if(event->mask&IN_MODIFY) fields[2]='w'; /* MODIFY */
    if(event->mask&IN_ATTRIB) fields[2]='a'; /* ATTRIB */
    if(event->mask&IN_CLOSE_WRITE)   fields[1]='C'; /*CLOSE_WRITE */
    if(event->mask&IN_CLOSE_NOWRITE) fields[1]='c'; /* CLOSE_NOWRITE */
    if(event->mask&IN_OPEN)        fields[1]='O'; /* OPEN  */
    if(event->mask&IN_CREATE)      fields[1]='X'; /* CREATE */
    if(event->mask&IN_MOVED_FROM)  fields[1]='M'; /* MOVED_FROM */
    if(event->mask&IN_MOVED_TO)    fields[1]='m'; /* MOVED_TO */
    if(event->mask&IN_DELETE)      fields[1]='d'; /* DELETE file */
    if(event->mask&IN_DELETE_SELF) fields[1]='D'; /* DELETE_SELF */

    erg=realloc(erg,strlen(erg)+5+event->len);
    strcat(erg,fields);
    if(event->len) strcat(erg,event->name);
    i+=EVENT_SIZE+event->len;
    if(i<len) strcat(erg," ");
  }
/*Also here it is essential to distinguish between diferent watch descriptors....*/
#endif
  }
  return(erg);
}



/* Spawns a process with redirected standard input and output
   streams. ARGV is the set of arguments for the process,
   terminated by a NULL element. The first element of ARGV
   should be the command to invoke the process.
   Returns a file descriptor that can be used to communicate
   with the process. */

int spawn_shell (char *argv[]) {
  int ret_fd = -1;
  char slavename[80]="";
 #ifndef ANDROID
  struct winsize win={25,80,320,240};
  #endif
  printf("connecting %s\n",argv[0]);

  /* Find out if the intended programme really exists and
     is accessible. */
  struct stat stat_buf;
  if (stat (argv[0], &stat_buf) != 0) {printf("ERROR accessing programme");return -1;}

#ifdef SAVE_STDERR
  /* Save the standard error stream. */
  int saved_stderr = dup (STDERR_FILENO);
  if(saved_stderr < 0) {printf("ERROR saving old STDERR");return -1;}
#endif
  /* Create a pseudo terminal and fork a process attached
     to it. */
  pid_t pid = forkpty (&ret_fd,slavename, NULL, &win);
 // printf("forkpty(%s) --> %d --> %d\n",slavename,ret_fd,pid);
  if (pid == 0) {
    /* Inside the child process. */

#ifndef WINDOWS
    /* Ensure that terminal echo is switched off so that we
       do not get back from the spawned process the same
       messages that we have sent it. */
    struct termios orig_termios;
    if(tcgetattr(STDIN_FILENO,&orig_termios)<0) {printf("ERROR getting current terminal's attributes");return -1;}

    orig_termios.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
    orig_termios.c_oflag &= ~(ONLCR);

    if(tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios) < 0) {printf("ERROR setting current terminal's attributes");return -1;}
#endif
#ifdef SAVE_STDERR
    /* Restore stderr. */
    if(dup2(saved_stderr, STDERR_FILENO)<0) {printf("ERROR restoring STDERR");return -1;}
#endif
    /* This should now already work */
  //  printf("The child is now going to excecute the shell!\n");
    /* Now spawn the intended programme. */
    if (execv (argv[0], argv)) {
      /* execv() should not return. */
      printf("ERROR spawning program\n");
      return -1;
    }
  } else if (pid < 0) {
    io_error(errno,"forkpty");
    if(errno==ENOENT) printf("There are no available ttys.\n");
    printf("slavename=%s",slavename);
    printf("\nERROR spawning program\n");
    return -1;
  }
#ifdef SAVE_STDERR
  else close(saved_stderr);
#endif
  if(ret_fd<0) {
    printf("Cannot spawn shell! ERROR\n");
  } else {
    struct timeval tv;
    fd_set active_fd_set,read_fd_set;
    int DoExit=0;
    int retval;
    FD_ZERO (&active_fd_set);
    FD_SET (STDIN_FILENO, &active_fd_set); 
    FD_SET (ret_fd, &active_fd_set); 
    while (!DoExit) {
      tv.tv_sec =  1;   /* Wait up to one second. */
      tv.tv_usec = 10000;
      read_fd_set = active_fd_set;
      retval = select (FD_SETSIZE, &read_fd_set, NULL, NULL, &tv); 
      if(retval<0) {
        if(errno==EINTR) ; /* This is OK */
        else if(errno==EBADF) { /* This is not OK */
          printf("console: select failed! Connection to shell lost! \n");
	  return(0);
        } else printf("console: select failed!\n");
      } else if(!retval) {
       // printf("++timeout!\n");
      //  DoExit=1;  /* This, probably, will also not work.  */
      } else {
        if(FD_ISSET(ret_fd,&read_fd_set)) {
          char c;
          int cc=read(ret_fd,&c,1);
          if(cc>0) {
	  putchar(c);fflush(stdout);
	  #ifdef ANDROID
  	    invalidate_screen();
	  #endif
	  }
   	  else if(cc==-1) {
	    if(errno==EINTR || errno==EAGAIN) ; /* This is OK */
            else if(errno==EBADF ||errno==EIO) { /* This is not OK */
              printf("Terminal: read failed! Connection to shell lost!\n");
	      DoExit=1;
            } else printf("console: read failed!\n");
	  }
        } 
	if(FD_ISSET(STDIN_FILENO,&read_fd_set)) {
	  char c;
          int cc=read(STDIN_FILENO,&c,1);
	 // printf("read from stdin: %d\n",(int)c);
          if(cc>0) {
	    if(write(ret_fd,&c,1)!=1) printf("Terminal write error.\n");
	    
	    fsync(ret_fd);
          } else if(cc==-1) {
	    if(errno==EINTR || errno==EAGAIN) ; /* This is OK */
            else if(errno==EBADF ||errno==EIO) { /* This is not OK */
              printf("Terminal: read failed! Connection to terminal lost!\n");
	      DoExit=1;
            } else printf("console: read failed!\n");
	  }
        }
      }
    }
  }
  return ret_fd;
}
