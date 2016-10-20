/* raw_mouse.c     Routinen fuer Maus-Driver (c) Markus Hoffmann    */


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
#endif

#include "framebuffer.h"
#include "raw_mouse.h"
static int mouse_mapped=0;  /*if mouse is actually visible*/
#ifdef ANDROID
#define pthread_mutex_lock(a) ;
#define pthread_mutex_unlock(a) ;
#else 
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
static int mouse_fd = -1;
static pthread_t mouse_thread;
static int do_mouse_events=0;
typedef struct MOUSE_EVENT {
  unsigned char but;
  signed char dx;
  signed char dy;
} MOUSE_EVENT;
#endif

#include "bitmaps/mauspfeil.bmp"
#include "bitmaps/mauspfeil_mask.bmp"
#include "bitmaps/biene.bmp"
#include "bitmaps/biene_mask.bmp"
#include "bitmaps/hand.bmp"
#include "bitmaps/hand_mask.bmp"
#include "bitmaps/zeigehand.bmp"
#include "bitmaps/zeigehand_mask.bmp"
#include "bitmaps/bombe.bmp"
#include "bitmaps/bombe_mask.bmp"

static unsigned short vmousepat[16*16];
static unsigned char vmousealpha[16*16];

const unsigned char mousealpha[16*16]={
  0,0,0,0,0,0,127,127,127,127,0,0,0,0,0,0,
  0,0,0,0,0,0,127,255,255,127,0,0,0,0,0,0,
  0,0,0,0,0,0,127,255,255,127,0,0,0,0,0,0,
  0,0,0,0,0,0,127,255,255,127,0,0,0,0,0,0,
  0,0,0,0,0,0,127,255,255,127,0,0,0,0,0,0,
  0,0,0,0,0,0,127,255,255,127,0,0,0,0,0,0,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
127,255,255,255,255,255,255,  0,  0,255,255,255,255,255,255,127,
127,255,255,255,255,255,255,  0,  0,255,255,255,255,255,255,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
  0,0,0,0,0,0,127,255,255,127,0,0,0,0,0,0,
  0,0,0,0,0,0,127,255,255,127,0,0,0,0,0,0,
  0,0,0,0,0,0,127,255,255,127,0,0,0,0,0,0,
  0,0,0,0,0,0,127,255,255,127,0,0,0,0,0,0,
  0,0,0,0,0,0,127,255,255,127,0,0,0,0,0,0,
  0,0,0,0,0,0,127,127,127,127,0,0,0,0,0,0};


const unsigned short mousepat[16*16]={
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0,
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0,
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0,
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0,
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0,
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0,
BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,WHITE,WHITE,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,
WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,WHITE,
BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,WHITE,WHITE,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,BLACK,
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0,
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0,
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0,
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0,
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0,
  0,0,0,0,0,0,BLACK,WHITE,WHITE,BLACK,0,0,0,0,0,0};

static void FB_show_mouse_() {
  pthread_mutex_lock( &mutex1 );
  if(screen.mouseshown && !mouse_mapped) {
    mouse_mapped++;
    FB_draw_sprite(screen.mousepat,screen.mousemask,screen.mouse_x-screen.mouse_ox,screen.mouse_y-screen.mouse_oy);
  }
  pthread_mutex_unlock( &mutex1 );
}
static void FB_hide_mouse_() {
  pthread_mutex_lock( &mutex1 );
  if(screen.mouseshown && mouse_mapped) {
    mouse_mapped=0;
    FB_hide_sprite(screen.mouse_x-screen.mouse_ox,screen.mouse_y-screen.mouse_oy);
  }
  pthread_mutex_unlock( &mutex1 );
}
void FB_show_mouse() {
  screen.mouseshown=1;
  FB_show_mouse_();
}
void FB_hide_mouse() {
  FB_hide_mouse_();
  screen.mouseshown=0;
}

void FB_defmouse(int form) {
  FB_hide_mouse();
  if(form==0 || form==7) {
    screen.mousemask=(unsigned char *)mousealpha;
    screen.mousepat=(unsigned short *)mousepat;
  } else if(form==1) {
    screen.mousemask=vmousealpha;
    screen.mousepat=vmousepat;
    FB_bmp2pixel(mauspfeil_bits,vmousepat,biene_width,biene_height,BLACK);
    FB_bmp2mask(mauspfeil_mask_bits,vmousealpha,biene_mask_width,biene_mask_height);
    screen.mouse_ox=screen.mouse_oy=0;
  } else if(form==2) {
    screen.mousemask=vmousealpha;
    screen.mousepat=vmousepat;
    FB_bmp2pixel(biene_bits,vmousepat,biene_width,biene_height,BLACK);
    FB_bmp2mask(biene_mask_bits,vmousealpha,biene_mask_width,biene_mask_height);
    screen.mouse_ox=screen.mouse_oy=8;
} else if(form==3) {
  screen.mousemask=vmousealpha;
  screen.mousepat=vmousepat;
  FB_bmp2pixel(zeigehand_bits,vmousepat,zeigehand_width,zeigehand_height,BLACK);
  FB_bmp2mask(zeigehand_mask_bits,vmousealpha,zeigehand_mask_width,zeigehand_mask_height);
  screen.mouse_ox=screen.mouse_oy=0;
} else if(form==4) {
  screen.mousemask=vmousealpha;
  screen.mousepat=vmousepat;
  FB_bmp2pixel(hand_bits,vmousepat,hand_width,hand_height,BLACK);
  FB_bmp2mask(hand_mask_bits,vmousealpha,hand_mask_width,hand_mask_height);
  screen.mouse_ox=screen.mouse_oy=8;
} else if(form==5) {
  screen.mousemask=vmousealpha;
  screen.mousepat=vmousepat;
  FB_bmp2pixel(bombe_bits,vmousepat,bombe_width,bombe_height,RED);
  FB_bmp2mask(bombe_mask_bits,vmousealpha,bombe_mask_width,bombe_mask_height);
  screen.mouse_ox=screen.mouse_oy=8;
}



  FB_show_mouse();
}




/************************** Mouse Handler Thread ***************************/



/*Bei der normalen framebuffer-Version (also nicht Android) sollen
  keyboard und mouse-events gesammelt werden. 
  Evtl. auch von stdin?*/

#ifndef ANDROID
static int read_mouse_event(struct MOUSE_EVENT *mousee) {
  if(mouse_fd>0) {
        int bytes=read(mouse_fd, mousee, sizeof(struct MOUSE_EVENT));
        if(bytes==-1) perror("Error reading mouse device ");
        else if(bytes==sizeof(struct MOUSE_EVENT)) return 1;
  }
  return 0;
}

static void *mouse_handler(void *ptr) {
 // printf("mouse_thread running...\n");
  fd_set aset,rset;
  FD_ZERO(&aset);
  FD_SET(mouse_fd, &aset);
  struct MOUSE_EVENT m;
  XEvent e;

  int rc;
  while(1) {
    rset=aset;
    rc=select(mouse_fd+1, &rset, NULL, NULL, NULL);
    if(rc==0) break;  /*TIMEOUT should not happen*/
    else if(rc<0) printf("select failed: errno=%d\n",errno);
    else {
      if(FD_ISSET(mouse_fd,&rset)) {
	if(read_mouse_event(&m)) {
	// printf("MOUSE: %02x/ %d %d \n",m.but,m.dx,m.dy);
  	  if(m.dx || m.dy) {
	    FB_hide_mouse_();
	    screen.mouse_x+=m.dx;
	    screen.mouse_y-=m.dy;
	    if(screen.mouse_x<0) screen.mouse_x=0;
	    else if(screen.mouse_x>screen.width) screen.mouse_x=screen.width;
	    if(screen.mouse_y<0) screen.mouse_y=0;
	    else if(screen.mouse_y>screen.height) screen.mouse_y=screen.height;
            if(do_mouse_events) {
              e.type=MotionNotify;
              e.xmotion.x=screen.mouse_x;
              e.xmotion.y=screen.mouse_y;
              e.xmotion.x_root=screen.mouse_x;
              e.xmotion.y_root=screen.mouse_y;
              e.xmotion.state=screen.mouse_s;
              FB_put_event(&e);
	    }
            FB_show_mouse_();
          }
	  if((m.but&7)!=screen.mouse_k) {
            if(do_mouse_events) {
              if((m.but&7)!=0) {
                e.type=ButtonPress;
	        e.xbutton.x=screen.mouse_x;
                e.xbutton.y=screen.mouse_y;
                e.xbutton.button=(m.but&7);
                e.xbutton.state=screen.mouse_s;
                e.xbutton.x_root=screen.mouse_x;
                e.xbutton.y_root=screen.mouse_y;
	        FB_put_event(&e);
	      } else  {
                e.type=ButtonRelease;
	        e.xbutton.x=screen.mouse_x;
                e.xbutton.y=screen.mouse_y;
                e.xbutton.button=screen.mouse_k;
                e.xbutton.state=screen.mouse_s;
                e.xbutton.x_root=screen.mouse_x;
                e.xbutton.y_root=screen.mouse_y;
	        FB_put_event(&e);
	      }
	    }
	  }
	  screen.mouse_k=(m.but&7);
        }
      }
    }
  }
  printf("mouse_thread finished.\n");
  return(NULL);
}



void FB_close_mouse() {
  if(mouse_fd>0) {
    pthread_cancel(mouse_thread);
    pthread_join(mouse_thread, NULL);
    close(mouse_fd);
    mouse_fd = -1;
  }
}

char mousedevname[256]=MOUSE_DEVICE;

int Fb_Mouse_Open() {
  FB_close_mouse();
  mouse_fd=open(mousedevname, O_RDONLY );
/* Jetzt einen Interrupthandler / thread starten, welcher die
Maus-Ereignisse verarbeitet.*/
  if(mouse_fd>0) {
    pthread_create(&mouse_thread, NULL,mouse_handler, (void*) NULL);
  } else perror(mousedevname);
  return mouse_fd;
}

/*Routine teilt TTconsole mit, dass nun mousebewegungen über stdin gemeldet
  werden sollen....
  TODO: macht keinen sinn in normaler framebufferconsole. 
  
  */
void FB_mouse_events(int onoff) {
#ifdef TOMTOM
  static int mousecount=0;
  if(onoff) {
    printf("\033[?10h");
    mousecount++;
  } else {
    mousecount--;
    if(mousecount==0) printf("\033[?10l");
  }  
  fflush(stdout);
#endif
  do_mouse_events=onoff;
}

#endif



void FB_Query_pointer(int *rx,int *ry,int *x,int *y,unsigned int *k) {
  *rx=*x=screen.mouse_x;
  *ry=*y=screen.mouse_y;
  *k=screen.mouse_k;
}




