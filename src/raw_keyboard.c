/* raw_keyboard.c     Routinen fuer Event-Driver (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#ifndef ANDROID
#include <pthread.h>
#include <linux/input.h>
#endif

#include "framebuffer.h"
#include "raw_keyboard.h"

static int do_keyboard_events=0;

void FB_keyboard_events(int onoff) {do_keyboard_events=onoff;}


#ifndef ANDROID
char keyboarddevname[256]=KEYBOARD_DEVICE;

static int keyboard_fd=-1;
static pthread_t keyboard_thread;

static int read_keyboard_event(struct input_event *keye) {
  if(keyboard_fd>0) {
        int bytes=read(keyboard_fd, keye, sizeof(struct input_event));
        if(bytes==-1) perror("Error reading keyboard device");
        else if(bytes==sizeof(struct input_event)) return 1;
  }
  return 0;
}

static void *keyboard_handler(void *ptr) {
  // printf("keyboard_thread running...\n");
  fd_set aset,rset;
  FD_ZERO(&aset);
  FD_SET(keyboard_fd, &aset);
  struct input_event m;
  XEvent e;
  unsigned char state=0;
  int rc;
  while(1) {
    rset=aset;
    rc=select(keyboard_fd+1, &rset, NULL, NULL, NULL);
    if(rc==0) break;  /*TIMEOUT should not happen*/
    else if(rc<0) printf("select failed: errno=%d\n",errno);
    else {
      if(FD_ISSET(keyboard_fd,&rset)) {
	if(read_keyboard_event(&m)) {
	  if(do_keyboard_events && m.type==EV_KEY) {
	    e.xkey.time=(int)m.time.tv_sec; /* evtl könnte man noch die tv_usec verarbeiten */
	    e.xkey.keycode=m.code;
	    if(m.value) e.type=KeyPress;
	    else e.type=KeyRelease;
	    e.xkey.buf[1]=0;
	     
	    /*TODO: hier muesste der Buchstabe rausgefunden 
	        werden....*/

	    switch(m.code) {
	      case 0x01: e.xkey.buf[0]=27;e.xkey.ks=0xff1b;break;
	      case 0x0f: e.xkey.buf[0]=9;e.xkey.ks=0xff09;break;
	      case 0x1c: e.xkey.buf[0]=13;e.xkey.ks=(state<<8)|13;break;
	      case 0x39: e.xkey.buf[0]=' ';e.xkey.ks=(state<<8)|32;break;
	      case 0x67: e.xkey.buf[0]=0;e.xkey.ks=0xff00;break;  /* UP */
	      case 0x68: e.xkey.buf[0]=0;e.xkey.ks=0xff52;break; /* Page UP*/
	      case 0x69: e.xkey.buf[0]=0;e.xkey.ks=0xff00;break; /* LEFT*/
	      case 0x6a: e.xkey.buf[0]=0;e.xkey.ks=0xff00;break; /* RIGHT*/
	      case 0x6c: e.xkey.buf[0]=0;e.xkey.ks=0xff00;break; /* DOWN*/
	      case 0x6d: e.xkey.buf[0]=0;e.xkey.ks=0xff54;break; /* Page DOWN*/
	      default: e.xkey.buf[0]=0;e.xkey.ks=0;break;
	    }
	    FB_put_event(&e);
	  }
#if DEBUG
	  if(m.type==EV_KEY) {
	    printf("EVENT: time=%d.%d type=%04x code=%04x value=%d ",(int)m.time.tv_sec,(int)m.time.tv_usec,m.type,m.code,m.value);
            switch(m.type) {
	      case EV_KEY: printf("EV_KEY \n"); break;
	      case EV_REL: printf("EV_REL \n"); break;
	      case EV_ABS: printf("EV_ABS \n"); break;
	      case EV_SYN: printf("EV_SYN \n"); break;
	      case EV_MSC: printf("EV_MSC \n"); break;
	      default:     printf("unknown\n"); break;
	    }
	  }
#endif
        }
      }
    }
  }
  printf("keyboard_thread finished.\n");
  return(NULL);
}

void FB_close_keyboard() {
  if(keyboard_fd>0) {
    pthread_cancel(keyboard_thread);
    pthread_join(keyboard_thread, NULL);
    close(keyboard_fd);
    keyboard_fd = -1;
  }
}
int Fb_Keyboard_Open() {
  FB_close_keyboard();
  keyboard_fd=open(keyboarddevname, O_RDONLY);
/* Jetzt einen Interrupthandler / thread starten, welcher die
Event-Ereignisse verarbeitet.*/
  if(keyboard_fd>0) {
    pthread_create(&keyboard_thread, NULL,keyboard_handler, (void*) NULL);
  } else perror(keyboarddevname);
  return keyboard_fd;
}
#endif
