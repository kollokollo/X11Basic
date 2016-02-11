

/* gkommandos.c Grafik-Befehle  (c) Markus Hoffmann     */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
/* SIZEW (WINDOWS) 18.12.2002   M. Hoffmann   */
/* Fehler beim Mouseevent beseitigt 2.6.1999   M. Hoffmann   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"
#include "x11basic.h"
#include "parameter.h"
#include "variablen.h"
#include "xbasic.h"
#include "graphics.h"
#include "kommandos.h"
#include "gkommandos.h"
#include "aes.h"
#include "window.h"
#include "bitmap.h"
#include "file.h"
#include "array.h"
#include "parser.h"
#include "wort_sep.h"
#include "io.h"



#ifndef NOGRAPHICS

#ifdef FRAMEBUFFER
#include "bitmaps/bombe.bmp"
#include "bitmaps/bombe_mask.bmp"
#endif

int menuaction;
int turtlex,turtley;

/*
  Philosophie bei den Windows: 
  Nr 0  ist rootwindow bei X11 und fullscreen bei SDL
  Nr 1-16 normale Fenster


*/
static void do_polygon(int doit,PARAMETER *plist,int e);

void c_rootwindow(char *n){
  usewindow=0; 
  graphics();
}
void c_norootwindow(char *n){
  usewindow=DEFAULTWINDOW; 
  graphics();
}
void c_usewindow(PARAMETER *plist,int e){
  if(e) usewindow=max(0,min(MAXWINDOWS-1,plist[0].integer));
  graphics();
}

void c_vsync(char *n) {activate();}

void c_plot(PARAMETER *plist,int e) {
    graphics();
    DrawPoint(plist[0].integer,plist[1].integer);
}

void c_savescreen(PARAMETER *plist,int e) {
    int x,y,len,i;
    unsigned int b,d,w,h;
    char *data;
    if(e) {
#ifdef USE_X11
    XImage *Image;
    Window root;
    Colormap map;
    XWindowAttributes xwa;
    XColor ppixcolor[256];
#endif
    graphics();
#ifdef FRAMEBUFFER
    FB_get_geometry(&x,&y,&w,&h,&b,&d);
    data=FB_get_image(x,y,w,h,&len);
    bsave(plist[0].pointer,data,len);
    free(data);
#endif
#ifdef USE_X11
    XGetGeometry(display[usewindow],
      RootWindow(display[usewindow],XDefaultScreen(display[usewindow])),
      &root,&x,&y,&w,&h,&b,&d); 
      XGetWindowAttributes(display[usewindow], root, &xwa);
    
    Image=XGetImage(display[usewindow],root,
                x, y, w, h, AllPlanes,(d==1) ?  XYPixmap : ZPixmap);
		
    if(d==8) {
      map =XDefaultColormapOfScreen ( XDefaultScreenOfDisplay(display[usewindow]));
      for(i=0;i<256;i++) ppixcolor[i].pixel=i;
      XQueryColors(display[usewindow], map, ppixcolor,256);
    }
    data=imagetoxwd(Image,xwa.visual,ppixcolor,&len);	
    XDestroyImage(Image);
    bsave(plist[0].pointer,data,len);
    free(data);
#endif
#ifdef USE_SDL
  /* bei SDL ist SAVESCREEN dasselbe wie SAVEWINDOW*/
  SDL_SaveBMP(display[usewindow], plist[0].pointer);
#endif
  }
}
void c_savewindow(PARAMETER *plist,int e) {
  int x,y,len,i;
  unsigned int b,d,w,h;
  char *data;
  if(e) {
#ifdef USE_X11
    XImage *Image;
    Window root;
    Colormap map;
    XColor ppixcolor[256];
    XWindowAttributes xwa;
#endif
    graphics();
#ifdef FRAMEBUFFER
    FB_get_geometry(&x,&y,&w,&h,&b,&d);
    data=FB_get_image(x,y,w,h,&len);
    bsave(plist[0].pointer,data,len);
    free(data);
#endif
#ifdef USE_X11
    XGetGeometry(display[usewindow], win[usewindow], 
	&root,&x,&y,&w,&h,&b,&d); 
    XGetWindowAttributes(display[usewindow], win[usewindow], &xwa);
    
    Image=XGetImage(display[usewindow],pix[usewindow],
                0, 0, w, h, AllPlanes,(d==1) ?  XYPixmap : ZPixmap);
    if(d==8) {
      map =XDefaultColormapOfScreen ( XDefaultScreenOfDisplay(display[usewindow]));
      for(i=0;i<256;i++) ppixcolor[i].pixel=i;
      XQueryColors(display[usewindow], map, ppixcolor,256);
    }
    data=imagetoxwd(Image,xwa.visual,ppixcolor,&len);
    XDestroyImage(Image);
    bsave(plist[0].pointer,data,len);
    free(data);
#endif   
#ifdef USE_SDL
  SDL_SaveBMP(display[usewindow], plist[0].pointer);
#endif
  }
}
void c_get(PARAMETER *plist,int e) {
  int bx,by,len,i;
  unsigned int b,d,bw,bh;
  char *data;

#ifdef USE_X11
  XImage *Image;
  Window root;
  Colormap map;
  XColor ppixcolor[256];
  XWindowAttributes xwa;
#endif
  if(e==5) { 
    graphics();
 
#ifdef FRAMEBUFFER
  data=FB_get_image(plist[0].integer,plist[1].integer,
                    plist[2].integer,plist[3].integer,&len);
  varcaststring_and_free(plist[4].typ,plist[4].pointer,create_string_and_free(data,len));
#endif
#ifdef USE_X11
   XGetGeometry(display[usewindow], win[usewindow], 
	&root,&bx,&by,&bw,&bh,&b,&d); 
    XGetWindowAttributes(display[usewindow], win[usewindow], &xwa);
    Image=XGetImage(display[usewindow],pix[usewindow],
                plist[0].integer,plist[1].integer,plist[2].integer,
		plist[3].integer, AllPlanes,(d==1) ?  XYPixmap : ZPixmap);
    if(d==8) {
      map =XDefaultColormapOfScreen ( XDefaultScreenOfDisplay(display[usewindow]));
      for(i=0;i<256;i++) ppixcolor[i].pixel=i;
      XQueryColors(display[usewindow], map, ppixcolor,256);
    }
    data=imagetoxwd(Image,xwa.visual,ppixcolor,&len);
    varcaststring_and_free(plist[4].typ,plist[4].pointer,create_string_and_free(data,len));
    XDestroyImage(Image);
#endif
  }
}

/* PUT x,y,t$[,scale,transparency,x,y,w,h] */

void c_put(PARAMETER *plist,int e) {  

#ifdef USE_X11
  XImage *ximage;
  XImage *xmask;
  XWindowAttributes xwa;
  GC sgc;
  XGCValues gc_val;
#endif

 if(e>=3) {
    unsigned int x=0,y=0,w,h;
    double scale=1;
    graphics();    
    if(e>=4 && plist[3].typ!=PL_LEER) {
      scale=plist[3].real;
      if(scale<0) scale=-scale;
    } 
#ifdef FRAMEBUFFER
    FB_put_image(plist[2].pointer,plist[0].integer,plist[1].integer);
#endif
#ifdef USE_X11
    XGetWindowAttributes(display[usewindow], win[usewindow], &xwa);
    ximage=xwdtoximage(plist[2].pointer,xwa.visual,xwa.depth,&xmask,0,scale);

    w=ximage->width;
    h=ximage->height;
    if(e>=9) {
      x=min(max(0,plist[5].integer),w);
      y=min(max(0,plist[6].integer),h);
      w=min(max(0,plist[7].integer),w-x);
      h=min(max(0,plist[8].integer),h-y);
    }
  //  printf("y=%d, y=%d, w=%d, h=%d scale=%g\n",x,y,w,h,scale);
    
    if(xwa.depth!=ximage->depth)
      printf("Grafik hat die falsche Farbtiefe %d (must be %d)!\n",ximage->depth,xwa.depth);
    else {
      if(xmask) {
      /*grafik context vorher retten und nachher restoren ... */
        sgc=XCreateGC(display[usewindow], win[usewindow], 0, &gc_val);
        XCopyGC(display[usewindow], gc[usewindow],GCForeground|GCFunction, sgc);
     
       if(XInitImage(xmask)) {
         XSetFunction(display[usewindow], gc[usewindow], GXandInverted);
	 
	 XPutImage(display[usewindow],pix[usewindow],gc[usewindow],
                xmask, x,y,plist[0].integer,plist[1].integer, w,h);
         XSetFunction(display[usewindow], gc[usewindow], GXor); 


       } else xberror(53,"PUT mask"); /* Falsches Grafikformat  */
       XDestroyImage(xmask);
     }

     if(XInitImage(ximage)) {

      XPutImage(display[usewindow],pix[usewindow],gc[usewindow],
                ximage, x,y,plist[0].integer,plist[1].integer, w, h);
     } else xberror(53,""); /* Falsches Grafikformat  */
      if(xmask) {
        XCopyGC(display[usewindow], sgc,GCForeground|GCFunction, gc[usewindow]);
        XFreeGC(display[usewindow],sgc); 
      }
    }
    XDestroyImage(ximage);
#endif
#ifdef USE_SDL
  SDL_Surface *bmpdata;
  SDL_Surface *image;
  bmpdata=bpmtosurface(plist[2].pointer,scale);
  image=SDL_DisplayFormat(bmpdata);
  SDL_FreeSurface(bmpdata);
  SDL_Rect a={0,0,image->w,image->h};
  SDL_Rect b={plist[0].integer,plist[1].integer,0,0};
  SDL_BlitSurface(image, &a,display[usewindow], &b);
  SDL_FreeSurface(image);
#endif
  }
}

void c_put_bitmap(PARAMETER *plist,int e) {  
    graphics();    
    put_bitmap(plist[0].pointer,plist[1].integer,plist[2].integer,plist[3].integer,plist[4].integer);
}

void c_sget(PARAMETER *plist,int e) {
  STRING str;
  int x,y,i;
  unsigned int w,h,d,b;
#ifdef USE_X11
  XImage *Image;
  Window root;
  Colormap map;
  XColor ppixcolor[256];
  XWindowAttributes xwa;
#endif
  graphics();

#ifdef FRAMEBUFFER
  FB_get_geometry(&x,&y,&w,&h,&b,&d);
  str.pointer=FB_get_image(x,y,w,h,&(str.len));
  varcaststring(plist[0].integer,plist[0].pointer,str);
  free(str.pointer);
#endif
#ifdef USE_X11
    XGetGeometry(display[usewindow], win[usewindow], 
	&root,&x,&y,&w,&h,&b,&d); 
    XGetWindowAttributes(display[usewindow], win[usewindow], &xwa);
    
    Image=XGetImage(display[usewindow],pix[usewindow],
                0, 0, w, h, AllPlanes,(d==1) ?  XYPixmap : ZPixmap);
    if(d==8) {
      map =XDefaultColormapOfScreen ( XDefaultScreenOfDisplay(display[usewindow]));
      for(i=0;i<256;i++) ppixcolor[i].pixel=i;
      XQueryColors(display[usewindow], map, ppixcolor,256);
    }
    str.pointer=imagetoxwd(Image,xwa.visual,ppixcolor,&(str.len));

  varcaststring(plist[0].integer,plist[0].pointer,str);
 XDestroyImage(Image);  
 free(str.pointer);
#endif
}


void c_getgeometry(PARAMETER *plist,int e) {
  int winnr=DEFAULTWINDOW;
  int x,y;
  unsigned int d,b,w,h;
#ifdef USE_X11
  Window root;
#endif
  winnr=plist[0].integer;
  if(winnr<0 || winnr>=MAXWINDOWS) xberror(98,""); /* Illegal Window number %s (0-16) */
  else if(!winbesetzt[winnr]) xberror(99,""); /* Window %s does not exist  */
  else {
    graphics();
#ifdef FRAMEBUFFER
    FB_get_geometry(&x,&y,&w,&h,&b,&d);
#endif
#ifdef USE_X11
    XGetGeometry(display[winnr], win[winnr], &root,&x,&y,&w,&h,&b,&d); 
#endif
#ifdef WINDOWS
#endif
#ifdef USE_SDL
    x=y=0;
    w=display[usewindow]->w;
    h=display[usewindow]->h;
    d=32;
#endif
#if DEBUG
    printf("get_geometry: %d %d %d %d %d %d\n",plist[0].integer,x,y,w,h,d);
#endif
    if(e>6 && plist[6].typ!=PL_LEER) varcastint(plist[6].integer,plist[6].pointer,b);
    if(e>5 && plist[5].typ!=PL_LEER) varcastint(plist[5].integer,plist[5].pointer,d);
    if(e>4 && plist[4].typ!=PL_LEER) varcastint(plist[4].integer,plist[4].pointer,h);
    if(e>3 && plist[3].typ!=PL_LEER) varcastint(plist[3].integer,plist[3].pointer,w);
    if(e>2 && plist[2].typ!=PL_LEER) varcastint(plist[2].integer,plist[2].pointer,y);
    if(e>1 && plist[1].typ!=PL_LEER) varcastint(plist[1].integer,plist[1].pointer,x);
  } 
}

void c_getscreensize(PARAMETER *plist,int e) {
  int x,y;
  unsigned int d,b,w,h;
#ifdef USE_X11
    Window root;
#endif
    graphics();
#ifdef FRAMEBUFFER
    FB_get_geometry(&x,&y,&w,&h,&b,&d);
#endif
#ifdef USE_X11
    XGetGeometry(display[usewindow],
        RootWindow(display[usewindow],XDefaultScreen(display[usewindow]) ),
	&root,&x,&y,&w,&h,&b,&d); 
#endif
#ifdef WINDOWS
#endif
#ifdef USE_SDL
    x=y=0;
    w=display[usewindow]->w;
    h=display[usewindow]->h;
    d=32;
#endif

#if DEBUG
    printf("get_geometry: %d %d %d %d %d %d\n",plist[0].integer,x,y,w,h,d);
#endif
    if(e>4 && plist[4].typ!=PL_LEER) varcastint(plist[4].integer,plist[4].pointer,d);
    if(e>3 && plist[3].typ!=PL_LEER) varcastint(plist[3].integer,plist[3].pointer,h);
    if(e>2 && plist[2].typ!=PL_LEER) varcastint(plist[2].integer,plist[2].pointer,w);
    if(e>1 && plist[1].typ!=PL_LEER) varcastint(plist[1].integer,plist[1].pointer,y);
    if(e>0 && plist[0].typ!=PL_LEER) varcastint(plist[0].integer,plist[0].pointer,x);
}

void c_sput(PARAMETER *plist,int e) {
  if(e) {
#ifdef USE_X11
    XImage *ximage;
    XWindowAttributes xwa;
#endif
    graphics();

#ifdef FRAMEBUFFER
    FB_put_image(plist[0].pointer,0,0);
#endif
#ifdef USE_X11
    XGetWindowAttributes(display[usewindow], win[usewindow], &xwa);

    ximage=xwdtoximage(plist[0].pointer,xwa.visual,xwa.depth,NULL,0,1);
    XPutImage(display[usewindow],pix[usewindow],gc[usewindow],
                ximage, 0,0,0,0, ximage->width, ximage->height);
    XDestroyImage(ximage);
#endif
  }
}
void c_line(PARAMETER *plist,int e) {
  graphics(); 
  line(plist[0].integer,plist[1].integer,plist[2].integer,plist[3].integer);
}
/* Kubische Bezier curve*/
void c_curve(PARAMETER *plist,int e) {
  int granul=3;
  int t=0;
  int x,y,ox,oy;
  #define px0 plist[0].integer
  #define py0 plist[1].integer
  #define px1 plist[2].integer
  #define py1 plist[3].integer
  #define px2 plist[4].integer
  #define py2 plist[5].integer
  #define px3 plist[6].integer
  #define py3 plist[7].integer
  if(e>8) granul=plist[8].integer;
  else granul=max(3,max(abs(px3-px0),abs(py3-px0))/4);
// printf("granul=%d\n",granul);
  graphics();
  while(t<granul) {
    x=px0+(-px0+3*px1-3*px2+px3)*t*t*t/granul/granul/granul+(3*px0-6*px1+3*px2)*t*t/granul/granul+(-3*px0+3*px1)*t/granul;
    y=py0+(-py0+3*py1-3*py2+py3)*t*t*t/granul/granul/granul+(3*py0-6*py1+3*py2)*t*t/granul/granul+(-3*py0+3*py1)*t/granul;
    if(t) line(ox,oy,x,y);
    ox=x;oy=y;
    t++;
  }
  line(x,y,plist[6].integer,plist[7].integer);
  #undef px0 
  #undef px1
  #undef px2 
  #undef px3 
  #undef py0 
  #undef py1 
  #undef py2 
  #undef py3 
}
void c_box(PARAMETER *plist,int e) {
  graphics(); 
  mybox(plist[0].integer,plist[1].integer,plist[2].integer,plist[3].integer);
}
#define RBOX_RADIUS 16
void c_rbox(PARAMETER *plist,int e) {
  graphics(); 
  line(plist[0].integer+RBOX_RADIUS,plist[1].integer,plist[2].integer-RBOX_RADIUS,plist[1].integer);
  line(plist[2].integer,plist[1].integer+RBOX_RADIUS,plist[2].integer,plist[3].integer-RBOX_RADIUS);
  line(plist[2].integer-RBOX_RADIUS,plist[3].integer,plist[0].integer+RBOX_RADIUS,plist[3].integer);
  line(plist[0].integer,plist[3].integer-RBOX_RADIUS,plist[0].integer,plist[1].integer+RBOX_RADIUS);
  XDrawArc(display[usewindow],pix[usewindow],gc[usewindow],plist[0].integer,plist[1].integer,2*RBOX_RADIUS,2*RBOX_RADIUS,90*64,90*64); 
  XDrawArc(display[usewindow],pix[usewindow],gc[usewindow],plist[2].integer-2*RBOX_RADIUS,plist[1].integer,2*RBOX_RADIUS,2*RBOX_RADIUS,0,90*64); 
  XDrawArc(display[usewindow],pix[usewindow],gc[usewindow],plist[2].integer-2*RBOX_RADIUS,plist[3].integer-2*RBOX_RADIUS,2*RBOX_RADIUS,2*RBOX_RADIUS,270*64,90*64); 
  XDrawArc(display[usewindow],pix[usewindow],gc[usewindow],plist[0].integer,plist[3].integer-2*RBOX_RADIUS,2*RBOX_RADIUS,2*RBOX_RADIUS,180*64,90*64); 
}
void c_pbox(PARAMETER *plist,int e) {
  graphics(); 
  pbox(plist[0].integer,plist[1].integer,plist[2].integer,plist[3].integer);
}
void c_prbox(PARAMETER *plist,int e) {
  graphics(); 
  pbox(plist[0].integer+RBOX_RADIUS,plist[1].integer,plist[2].integer-RBOX_RADIUS,plist[3].integer);
  pbox(plist[0].integer,plist[1].integer+RBOX_RADIUS,plist[2].integer,plist[3].integer-RBOX_RADIUS);
  XFillArc(display[usewindow],pix[usewindow],gc[usewindow],plist[0].integer,plist[1].integer,2*RBOX_RADIUS,2*RBOX_RADIUS,90*64,90*64); 
  XFillArc(display[usewindow],pix[usewindow],gc[usewindow],plist[2].integer-2*RBOX_RADIUS,plist[1].integer,2*RBOX_RADIUS,2*RBOX_RADIUS,0,90*64); 
  XFillArc(display[usewindow],pix[usewindow],gc[usewindow],plist[2].integer-2*RBOX_RADIUS,plist[3].integer-2*RBOX_RADIUS,2*RBOX_RADIUS,2*RBOX_RADIUS,270*64,90*64); 
  XFillArc(display[usewindow],pix[usewindow],gc[usewindow],plist[0].integer,plist[3].integer-2*RBOX_RADIUS,2*RBOX_RADIUS,2*RBOX_RADIUS,180*64,90*64); 
}

void c_dotodraw(char *n) {
  int e2,x,y;
  char *u,*t;
  t=malloc(strlen(n)+1);u=malloc(strlen(n)+1);
  xtrim(n,TRUE,n);
  e2=wort_sep(n,',',TRUE,u,t);
  if(e2<2) xberror(42,"DRAW TO"); /* Zu wenig Parameter */
  else {
    x=(int)parser(u);
    y=(int)parser(t);
    line(turtlex,turtley,x,y); 
    turtlex=x;
    turtley=y;
  }
  free(u);free(t);
  return;
}

void c_draw(char *n) {
  char *v,*t,*u;
  int e,e2;
  v=malloc(strlen(n)+1);t=malloc(strlen(n)+1);u=malloc(strlen(n)+1);
  strcpy(v,n);
  xtrim(v,TRUE,v);
  e=wort_sep2(v,"TO ",TRUE,t,v);  
  xtrim(t,TRUE,t);
  
  if(e>0) {
    graphics();
    if(strlen(t)) {
      e2=wort_sep(t,',',TRUE,u,t);
      
      if(e2<2) xberror(42,"DRAW"); /* Zu wenig Parameter */
      else {
        turtlex=(int)parser(u);
        turtley=(int)parser(t);
      }
    }
    if(e==1) line(turtlex,turtley,turtlex,turtley);
    else if(e==2) {
      e2=wort_sep2(v,"TO ",TRUE,t,v);
      while(e2) {  
        xtrim(t,TRUE,t);
        c_dotodraw(t);
	e2=wort_sep2(v,"TO ",TRUE,t,v);
      }
    }
  }
  free(v);free(t);free(u);
  return;
}

void c_circle(PARAMETER *plist,int e) {
  int r,x,y,a1=0,a2=64*360;
    r=plist[2].integer;
    x=plist[0].integer;
    y=plist[1].integer;
    if(e>=4) a1=plist[3].integer*64;
    if(e>=5) a2=plist[4].integer*64;
    
    graphics(); 
    #ifdef USE_SDL
    circleColor(display[usewindow],x,y,r,fcolor);
    #else
    XDrawArc(display[usewindow],pix[usewindow],gc[usewindow],x-r,y-r,2*r,2*r,a1,a2-a1); 
    #endif
}
void c_pcircle(PARAMETER *plist,int e) {
  int r,x,y,a1=0,a2=64*360;
    r=plist[2].integer;
    x=plist[0].integer;
    y=plist[1].integer;

    if(e>=4) a1=plist[3].integer*64;
    if(e>=5) a2=plist[4].integer*64;
    
    graphics(); 
    #ifdef USE_SDL
    filledCircleColor(display[usewindow],x,y,r,fcolor);
    #else
    XFillArc(display[usewindow],pix[usewindow],gc[usewindow],x-r,y-r,2*r,2*r,a1,a2-a1); 
    #endif
}

void c_ellipse(PARAMETER *plist,int e) {  /*  ellipse x,y,r1,r2,a1,a2    Winkel in 1/64 Grad*/
  int a1=0,a2=64*360;
  int r1=plist[2].integer;
  int r2=plist[3].integer;

  graphics(); 
    #ifdef USE_SDL
      ellipseColor(display[usewindow],plist[0].integer,plist[1].integer,r1,r2,fcolor);
      /* Wir haben nix fuer Ellipsen sektoren ... */
    #else
      if(e>=5) a1=plist[4].integer*64;
      if(e>=6) a2=plist[5].integer*64;
      XDrawArc(display[usewindow],pix[usewindow],gc[usewindow],plist[0].integer-r1,plist[1].integer-r2,2*r1,2*r2,a1,a2-a1);
    #endif
}
void c_pellipse(PARAMETER *plist,int e) {
  int a1=0,a2=64*360;
  int r1=plist[2].integer;
  int r2=plist[3].integer;

  graphics(); 
    #ifdef USE_SDL
      filledEllipseColor(display[usewindow],plist[0].integer,plist[1].integer,r1,r2,fcolor);
      /* Wir haben nix fuer Ellipsen sektoren ... */
    #else
      if(e>=5) a1=plist[4].integer*64;
      if(e>=6) a2=plist[5].integer*64;
      XFillArc(display[usewindow],pix[usewindow],gc[usewindow],plist[0].integer-r1,plist[1].integer-r2,2*r1,2*r2,a1,a2-a1);
    #endif
}

void c_color(PARAMETER *plist,int e) {  
  graphics();
  SetForeground(plist[0].integer);
  if(e==2) {SetBackground(plist[1].integer);}
}

void c_boundary(PARAMETER *plist,int e) { boundary=plist->integer; }

void c_screen(PARAMETER *plist,int e) {
  graphics();
#ifdef USE_VGA
  vga_setmode(plist->integer);
#endif
#ifdef USE_SDL
  
#endif
}

void c_graphmode(PARAMETER *plist,int e) {
  graphics();  set_graphmode(plist->integer);
}
void c_setfont(PARAMETER *plist,int e) {
  graphics();  set_font(plist->pointer);
}

void c_scope(char *n) {                                      /* SCOPE y()[,sy[,oy,[,mod]]]   */
  char w1[strlen(n)+1],w2[strlen(n)+1];                      /* oder                         */
  int e,i=0,scip=0;			                     /* SCOPE y(),x(),sy,oy,sx,ox    */
  int vnrx=-1,vnry=-1,typ,mode=0,xoffset=0,yoffset=0;
  char *r;
  double yscale=1,xscale=1;
  
  e=wort_sep(n,',',TRUE,w1,w2);
  while(e) {
     scip=0;
     if(strlen(w1)) {
       switch(i) {
         case 0: { /* Array mit y-Werten */     
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnry=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnry==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else printf("SCOPE: no ARRAY.\n");
	   break;
	   }
	 case 1: {   /* Array mit x-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnrx=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnrx==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else scip=1;
	   break;
	   } 
	 
	 case 2: { mode=(int)parser(w1); break; } 
	 case 3: { yscale=parser(w1); break; } 
	 case 4: {  yoffset=(int)parser(w1); break;} 
	 case 5: {  xscale=parser(w1); break;} 
	 case 6: {  xoffset=(int)parser(w1); break;} 
	   
         default: break;
       }
     }
     if(scip==0) e=wort_sep(w2,',',TRUE,w1,w2);
     i++;
  }
  if(vnry!=-1) {
    int nn=do_dimension(&variablen[vnry]);
    int x1,x2,y1,y2;
    int typ=variablen[vnry].typ;
    if(typ==ARRAYTYP) typ=variablen[vnry].pointer.a->typ;
    
    if(typ==FLOATTYP) {   
      double *varptry=(double  *)(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE);
      double *varptrx;
      if(vnrx!=-1) {
        nn=min(do_dimension(&variablen[vnrx]),nn);
        varptrx=(double  *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE);
      }
    
      graphics();

      for(i=0;i<nn-1;i++) {
        y1=(int)(varptry[i]*yscale)+yoffset;
        y2=(int)(varptry[i+1]*yscale)+yoffset;
        if(vnrx!=-1) {
          x1=(int)(varptrx[i]*xscale)+xoffset;
          x2=(int)(varptrx[i+1]*xscale)+xoffset;
        } else {x1=i*xscale+xoffset;x2=(i+1)*xscale+xoffset;}
        if(mode==0) {DrawLine(x1,y1,x2,y2);}
	else if(mode==1 && vnrx!=-1) {DrawPoint(x1,y1);}
        else DrawLine(x1,yoffset,x1,y2);
      }
    } else {
      printf("ERROR: Please only use float arrays for SCOPE.\n");
    }
  }
}

void c_polymark(PARAMETER *plist,int e) { do_polygon(0,plist,e);}
void c_polyline(PARAMETER *plist,int e) { do_polygon(1,plist,e);}
void c_polyfill(PARAMETER *plist,int e) { do_polygon(2,plist,e);}

static void do_polygon(int doit,PARAMETER *plist,int e) {
  int i=0,xoffset=0,yoffset=0;
#ifdef USE_X11
  int mode=CoordModeOrigin,shape=Nonconvex;
#else
  int mode,shape;
#endif
  int anz=max(0,plist[0].integer);
  ARRAY *arrx=(ARRAY *)&(plist[1].integer);
  ARRAY *arry=(ARRAY *)&(plist[2].integer);
  anz=min(anz,anz_eintraege(arrx));
  anz=min(anz,anz_eintraege(arry));
  if(e>3 && plist[3].typ!=PL_LEER) xoffset=plist[3].integer;
  if(e>4 && plist[4].typ!=PL_LEER) yoffset=plist[4].integer;
#ifdef USE_X11
  if(e>5 && plist[5].typ!=PL_LEER) mode=((plist[5].integer)&1) ?CoordModePrevious:CoordModeOrigin; 
#endif
  if(e>6) shape=plist[6].integer;

  if(anz>0) {
#ifndef USE_X11
       typedef struct {short x, y;} XPoint;
#endif
      XPoint points[anz];
      int *varptry=(int  *)(arry->pointer+arry->dimension*INTSIZE);
      int *varptrx=(int  *)(arrx->pointer+arrx->dimension*INTSIZE);
      for(i=0;i<anz;i++) {
        points[i].x=varptrx[i]+xoffset;
        points[i].y=varptry[i]+yoffset;
      }
      graphics();
      if(doit==0) {
        switch(marker_typ) {
	case 1:
	  for(i=0;i<anz;i++) {
            XDrawArc(display[usewindow],pix[usewindow],gc[usewindow],points[i].x-marker_size,points[i].y-marker_size,2*marker_size,2*marker_size,0,64*360); 
	  }
	  break;
        case 2:
	  for(i=0;i<anz;i++) {
	    line(points[i].x-marker_size,points[i].y,points[i].x+marker_size,points[i].y);
	    line(points[i].x,points[i].y-marker_size,points[i].x,points[i].y+marker_size);
	  }
	  break;
	case 3:
	  for(i=0;i<anz;i++) {
	    line(points[i].x-marker_size,points[i].y,points[i].x+marker_size,points[i].y);
	    line(points[i].x,points[i].y-marker_size,points[i].x,points[i].y+marker_size);
	    line(points[i].x-marker_size,points[i].y-marker_size,points[i].x+marker_size,points[i].y+marker_size);
	    line(points[i].x+marker_size,points[i].y-marker_size,points[i].x-marker_size,points[i].y+marker_size);
	  }
	  break;
	case 4:
	  for(i=0;i<anz;i++) mybox(points[i].x-marker_size,points[i].y-marker_size,points[i].x+marker_size,points[i].y+marker_size);
	  break;
	case 5:
	  for(i=0;i<anz;i++) {
	    line(points[i].x-marker_size,points[i].y-marker_size,points[i].x+marker_size,points[i].y+marker_size);
	    line(points[i].x+marker_size,points[i].y-marker_size,points[i].x-marker_size,points[i].y+marker_size);
	  }
	  break;
	case 8:
	  for(i=0;i<anz;i++) {
            XFillArc(display[usewindow],pix[usewindow],gc[usewindow],points[i].x-marker_size,points[i].y-marker_size,2*marker_size,2*marker_size,0,64*360); 
	  }
	  break;
	case 9:
	  for(i=0;i<anz;i++) pbox(points[i].x-marker_size,points[i].y-marker_size,points[i].x+marker_size,points[i].y+marker_size);
	  break;
	default:
	#ifdef USE_X11
	  XDrawPoints(display[usewindow],pix[usewindow],gc[usewindow],points,anz,mode);
        #else
	  { int i; for(i=0;i<anz;i++) DrawPoint(points[i].x,points[i].y);}
        #endif
      } /* switch*/
    } /* if doit */
      #ifdef USE_SDL
      else if(doit==1) {
        Sint16 vx[anz],vy[anz];
	int i;
	for(i=0;i<anz;i++) {
	  vx[i]=points[i].x;
	  vy[i]=points[i].y;
	}
        polygonColor(display[usewindow],&vx,&vy,anz,fcolor);
      } else if(doit==2) {
        Sint16 vx[anz],vy[anz];
	int i;
 	for(i=0;i<anz;i++) {
	  vx[i]=points[i].x;
	  vy[i]=points[i].y;
	}       
        filledPolygonColor(display[usewindow],&vx,&vy,anz,fcolor);
      }
     #endif
     #ifdef USE_X11
      else if(doit==1) XDrawLines(display[usewindow],pix[usewindow],gc[usewindow],points,anz,mode);
      else if(doit==2) XFillPolygon(display[usewindow],pix[usewindow],gc[usewindow],points,anz,shape,mode);    
     #endif
  } /* if anz  */
}



void c_deffill(PARAMETER *plist,int e) {
  graphics();
#ifdef USE_X11
  if(e>=1 && plist[0].typ!=PL_LEER){
    int fs=plist[0].integer;
    if(fs>=0 && fs<2) XSetFillRule(display[usewindow], gc[usewindow], fs);
  } 
  if(e>=2 && plist[1].typ!=PL_LEER){
    int fs=plist[1].integer;
    if(fs>=0 && fs<4) XSetFillStyle(display[usewindow], gc[usewindow], fs);
  }
  if(e>=3 && plist[2].typ!=PL_LEER){
    int pa=plist[2].integer;
    pa=max(0,min(fill_height/fill_width,pa));
    set_fill(pa+1);
  }
#endif  
}
void c_defmark(PARAMETER *plist,int e) {
  graphics();

  if(e>=1 && plist[0].typ!=PL_LEER){
    /* Marker color not supported */
  }
  if(e>=2 && plist[1].typ!=PL_LEER) marker_typ=plist[1].integer;
  if(e>=3 && plist[2].typ!=PL_LEER) marker_size=max(0,plist[2].integer);
}


void c_fill(PARAMETER *plist,int e) {
  int bc=-1;
  graphics();
  if(e>2) bc=plist[2].integer;
  ffill(plist[0].integer,plist[1].integer,get_fcolor(),bc);
}
void c_clip(PARAMETER *plist,int e) {
  graphics();
#ifdef USE_X11
  if(e==1) XSetClipMask(display[usewindow], gc[usewindow], None); /*CLIP OFF */
  else if(e>3) {
    XRectangle rectangle;
    int ox=0,oy=0;
    rectangle.x=plist[0].integer;
    rectangle.y=plist[1].integer;
    rectangle.width=plist[2].integer;
    rectangle.height=plist[3].integer;
    if(e>4) ox=plist[4].integer;
    if(e>5) oy=plist[5].integer;
    XSetClipRectangles(display[usewindow], gc[usewindow], ox, oy, &rectangle, 1,Unsorted);
  }
#endif
#ifdef FRAMEBUFFER
  if(e==1) FB_clip_off(&screen); /*CLIP OFF */
  else if(e>3) FB_set_clip(&screen,plist[0].integer,plist[1].integer,plist[2].integer,plist[3].integer);
#endif
#ifdef USE_SDL
  if(e==1) SDL_SetClipRect(display[usewindow], NULL); /*CLIP OFF */
  else if(e>3) {
    SDL_Rect a={plist[0].integer,plist[1].integer,plist[2].integer,plist[3].integer};
    SDL_SetClipRect(display[usewindow], &a);
  }
#endif
}
void c_defline(PARAMETER *plist,int e) { 
  graphics();
#ifdef FRAMEBUFFER
  if(e>=1 && plist[0].typ!=PL_LEER) screen.linestyle=plist[0].integer;
  if(e>=2 && plist[1].typ!=PL_LEER) screen.linewidth=plist[1].integer;
  if(e>=3 && plist[2].typ!=PL_LEER)   screen.linecap=plist[2].integer;
  if(e>=4 && plist[3].typ!=PL_LEER)  screen.linejoin=plist[3].integer;
#endif
#ifdef USE_X11
  static int style=0,width=0,cap=0,join=0;
  if(e>=1 && plist[0].typ!=PL_LEER) style=plist[0].integer;
  if(e>=2 && plist[1].typ!=PL_LEER) width=plist[1].integer;
  if(e>=3 && plist[2].typ!=PL_LEER)   cap=plist[2].integer;
  if(e>=4 && plist[3].typ!=PL_LEER)  join=plist[3].integer;
  XSetLineAttributes(display[usewindow], gc[usewindow],width,style,cap,join);
#endif
}
void c_copyarea(PARAMETER *plist,int e) {
  if(e==6) {
    graphics();
    #ifdef USE_SDL
      SDL_Rect a={plist[0].integer,plist[1].integer,plist[2].integer,plist[3].integer};
      SDL_Rect b={plist[4].integer,plist[5].integer,0,0};
      SDL_BlitSurface(display[usewindow], &a,display[usewindow], &b);
    #else
      CopyArea(plist[0].integer,plist[1].integer,plist[2].integer,
        plist[3].integer,plist[4].integer,plist[5].integer);
    #endif
  }
}


void c_deftext(PARAMETER *plist,int e) {
  if(e>=1 && plist[0].typ!=PL_LEER)    ltextpflg=plist[0].integer;
  if(e>=2 && plist[1].typ!=PL_LEER) ltextxfaktor=plist[1].real;
  if(e>=3 && plist[2].typ!=PL_LEER) ltextyfaktor=plist[2].real;
  if(e>=4 && plist[3].typ!=PL_LEER)  ltextwinkel=plist[3].real;
}


void c_mouse(PARAMETER *plist,int e) {
#ifdef WINDOWS
  if(e>0 && plist[0].typ!=PL_LEER) varcastint(plist[0].integer,plist[0].pointer,global_mousex);
  if(e>1 && plist[1].typ!=PL_LEER) varcastint(plist[1].integer,plist[1].pointer,global_mousey);
  if(e>2 && plist[2].typ!=PL_LEER) varcastint(plist[2].integer,plist[2].pointer,(global_mousek|(global_mouses<<8)));
#else
   int root_x_return, root_y_return,win_x_return, win_y_return;
   unsigned int mask_return;
#ifdef USE_X11
   Window root_return,child_return;
#endif
#ifdef FRAMEBUFFER
   FB_mouse(1);
#endif
   graphics();
#if defined USE_X11
   XQueryPointer(display[usewindow], win[usewindow], &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);
#endif
#ifdef USE_SDL
  mask_return=SDL_BUTTON(SDL_GetMouseState(&win_x_return,&win_y_return));
#endif
#if defined USE_X11 || defined USE_SDL   
  if(e>0 && plist[0].typ!=PL_LEER) varcastint(plist[0].integer,plist[0].pointer,win_x_return);
  if(e>1 && plist[1].typ!=PL_LEER) varcastint(plist[1].integer,plist[1].pointer,win_y_return);
  if(e>2 && plist[2].typ!=PL_LEER) 
#ifdef USE_X11
    varcastint(plist[2].integer,plist[2].pointer,(((mask_return>>8)|(mask_return<<8)) & 0xffff));
#else
    varcastint(plist[2].integer,plist[2].pointer,mask_return&0xffff );
#endif
  if(e>3 && plist[3].typ!=PL_LEER) varcastint(plist[3].integer,plist[3].pointer,root_x_return);
  if(e>4 && plist[4].typ!=PL_LEER) varcastint(plist[4].integer,plist[4].pointer,root_y_return);
#endif
#endif
#ifdef DEBUG
  printf("Mouse: x=%d y=%d m=%d\n",win_x_return,win_y_return,mask_return);
#endif
#ifdef FRAMEBUFFER
   FB_mouse(0);
#endif
}

void c_setmouse(PARAMETER *plist,int e) {
  int mode=0;

    if(e>2) mode=plist[2].integer;
    graphics();
#ifdef FRAMEBUFFER
  screen.mouse_x=plist[0].integer;
  screen.mouse_y=plist[1].integer;
  if(e>2) screen.mouse_k=plist[2].integer;
#else
#ifdef USE_X11
    if(mode==0) XWarpPointer(display[usewindow], None, win[usewindow], 0, 0,0,0,plist[0].integer,plist[1].integer);
    else if(mode==1) XWarpPointer(display[usewindow], None, None, 0, 0,0,0,plist[0].integer,plist[1].integer);
#endif
#endif
}


void c_mouseevent(PARAMETER *plist,int e) { /*MOUSEEVENT x,y,k,rx,ry,s */

#ifdef WINDOWS_NATIVE
  graphics();
  ResetEvent(buttonevent);
  WaitForSingleObject(buttonevent,INFINITE);
  if(e>0 && plist[0].typ!=PL_LEER)  varcastint(plist[0].integer,plist[0].pointer,global_mousex);
  if(e>1 && plist[1].typ!=PL_LEER)  varcastint(plist[1].integer,plist[1].pointer,global_mousey);
  if(e>2 && plist[2].typ!=PL_LEER)  varcastint(plist[2].integer,plist[2].pointer,global_mousek);
  if(e>5 && plist[5].typ!=PL_LEER)  varcastint(plist[5].integer,plist[5].pointer,global_mouses);
#endif
#ifdef FRAMEBUFFER
   FB_mouse(1);
#endif
#if defined USE_X11 || defined FRAMEBUFFER
  XEvent event;
  graphics();
    
  XWindowEvent(display[usewindow], win[usewindow],ButtonPressMask|ExposureMask, &event);
  while(event.type!=ButtonPress) { 
     handle_event(usewindow,&event);
     XWindowEvent(display[usewindow], win[usewindow],ButtonPressMask|ExposureMask, &event);
  }
   if(event.type==ButtonPress) {
  if(e>0 && plist[0].typ!=PL_LEER)  varcastint(plist[0].integer,plist[0].pointer,event.xbutton.x);
  if(e>1 && plist[1].typ!=PL_LEER)  varcastint(plist[1].integer,plist[1].pointer,event.xbutton.y);
  if(e>2 && plist[2].typ!=PL_LEER)  varcastint(plist[2].integer,plist[2].pointer,event.xbutton.button);
  if(e>3 && plist[3].typ!=PL_LEER)  varcastint(plist[3].integer,plist[3].pointer,event.xbutton.x_root);
  if(e>4 && plist[4].typ!=PL_LEER)  varcastint(plist[4].integer,plist[4].pointer,event.xbutton.y_root);
  if(e>5 && plist[5].typ!=PL_LEER)  varcastint(plist[5].integer,plist[5].pointer,event.xbutton.state);
  }
#endif
#ifdef USE_SDL
  SDL_Event event;
  if(SDL_WaitEvent(&event)==0) return;
  while(event.type!=SDL_MOUSEBUTTONDOWN && event.type!=SDL_MOUSEBUTTONUP) { 
     handle_event(usewindow,&event);
     if(SDL_WaitEvent(&event)==0) return;
  }
  if(e>0 && plist[0].typ!=PL_LEER)  varcastint(plist[0].integer,plist[0].pointer,event.button.x);
  if(e>1 && plist[1].typ!=PL_LEER)  varcastint(plist[1].integer,plist[1].pointer,event.button.y);
  if(e>2 && plist[2].typ!=PL_LEER)  varcastint(plist[2].integer,plist[2].pointer,event.button.button);
  if(e>3 && plist[3].typ!=PL_LEER)  varcastint(plist[3].integer,plist[3].pointer,0);
  if(e>4 && plist[4].typ!=PL_LEER)  varcastint(plist[4].integer,plist[4].pointer,0);
  if(e>5 && plist[5].typ!=PL_LEER)  varcastint(plist[5].integer,plist[5].pointer,event.button.state);
#endif
#ifdef FRAMEBUFFER
  FB_mouse(0);
#endif  
}

void c_motionevent(PARAMETER *plist,int e) {  /* x,y,b,rx,ry,s   */
#ifdef WINDOWS_NATIVE
  graphics();
  ResetEvent(motionevent);
  WaitForSingleObject(motionevent,INFINITE);
  if(e>0 && plist[0].typ!=PL_LEER)  varcastint(plist[0].integer,plist[0].pointer,global_mousex);
  if(e>1 && plist[1].typ!=PL_LEER)  varcastint(plist[1].integer,plist[1].pointer,global_mousey);
  if(e>2 && plist[2].typ!=PL_LEER)  varcastint(plist[2].integer,plist[2].pointer,global_mousek);
  if(e>5 && plist[5].typ!=PL_LEER)  varcastint(plist[5].integer,plist[5].pointer,global_mouses);
#endif
#if defined USE_X11 || defined FRAMEBUFFER
   XEvent event;   
   graphics();
#ifdef FRAMEBUFFER
   FB_mouse(1);
#endif
    
   XWindowEvent(display[usewindow], win[usewindow],PointerMotionMask|ExposureMask, &event);
   while(event.type!=MotionNotify) { 
     handle_event(usewindow,&event);
     XWindowEvent(display[usewindow], win[usewindow],PointerMotionMask|ExposureMask, &event);
   }   
   if(event.type==MotionNotify) {
  if(e>0 && plist[0].typ!=PL_LEER)  varcastint(plist[0].integer,plist[0].pointer,event.xmotion.x);
  if(e>1 && plist[1].typ!=PL_LEER)  varcastint(plist[1].integer,plist[1].pointer,event.xmotion.y);
  if(e>2 && plist[2].typ!=PL_LEER)  varcastint(plist[2].integer,plist[2].pointer,event.xmotion.state>>8);
  if(e>3 && plist[3].typ!=PL_LEER)  varcastint(plist[3].integer,plist[3].pointer,event.xmotion.x_root);
  if(e>4 && plist[4].typ!=PL_LEER)  varcastint(plist[4].integer,plist[4].pointer,event.xmotion.y_root);
  if(e>5 && plist[5].typ!=PL_LEER)  varcastint(plist[5].integer,plist[5].pointer,event.xmotion.state&0xff);
  }
#endif
#ifdef USE_SDL
  SDL_Event event;
  if(SDL_WaitEvent(&event)==0) return;
  while(event.type!=SDL_MOUSEMOTION) { 
     handle_event(usewindow,&event);
     if(SDL_WaitEvent(&event)==0) return;
  }
  if(e>0 && plist[0].typ!=PL_LEER)  varcastint(plist[0].integer,plist[0].pointer,event.motion.x);
  if(e>1 && plist[1].typ!=PL_LEER)  varcastint(plist[1].integer,plist[1].pointer,event.motion.y);
  if(e>2 && plist[2].typ!=PL_LEER)  varcastint(plist[2].integer,plist[2].pointer,event.motion.state);
  if(e>3 && plist[3].typ!=PL_LEER)  varcastint(plist[3].integer,plist[3].pointer,event.motion.xrel);
  if(e>4 && plist[4].typ!=PL_LEER)  varcastint(plist[4].integer,plist[4].pointer,event.motion.yrel);
  if(e>5 && plist[5].typ!=PL_LEER)  varcastint(plist[5].integer,plist[5].pointer,0);
#endif
#ifdef FRAMEBUFFER
   FB_mouse(0);
#endif
}

void c_keyevent(PARAMETER *plist,int e) {
#ifdef WINDOWS_NATIVE
  graphics();
  ResetEvent(keyevent);
  WaitForSingleObject(keyevent,INFINITE);
  while(global_eventtype!=KeyChar) {
    ResetEvent(keyevent);
    WaitForSingleObject(keyevent,INFINITE); 
  }
  if(e>0 && plist[0].typ!=PL_LEER)  varcastint(plist[0].integer,plist[0].pointer,global_keycode);
  if(e>1 && plist[1].typ!=PL_LEER)  varcastint(plist[1].integer,plist[1].pointer,global_ks);
  if(e>3 && plist[3].typ!=PL_LEER)  varcastint(plist[3].integer,plist[3].pointer,global_mouses);
  if(e>4 && plist[4].typ!=PL_LEER)  varcastint(plist[4].integer,plist[4].pointer,global_mousex);
  if(e>5 && plist[5].typ!=PL_LEER)  varcastint(plist[5].integer,plist[5].pointer,global_mousey);
#endif
#if defined USE_X11 || defined FRAMEBUFFER
   XEvent event;   
   graphics();
    
   XWindowEvent(display[usewindow], win[usewindow],KeyPressMask|ExposureMask, &event);
   while(event.type!=KeyPress) { 
     handle_event(usewindow,&event);
     XWindowEvent(display[usewindow], win[usewindow],KeyPressMask|ExposureMask, &event);
   }   
   if(event.type==KeyPress) {
     STRING str;
#ifndef FRAMEBUFFER
     char buf[4];
     XComposeStatus status;
     KeySym ks;
     
     XLookupString((XKeyEvent *)&event,buf,sizeof(buf),&ks,&status);   
#endif
  if(e>0 && plist[0].typ!=PL_LEER)  varcastint(plist[0].integer,plist[0].pointer,event.xkey.keycode);
#ifdef FRAMEBUFFER
  if(e>1 && plist[1].typ!=PL_LEER)  varcastint(plist[1].integer,plist[1].pointer,event.xkey.ks);
  str.pointer=event.xkey.buf;
  str.len=strlen(event.xkey.buf);
  if(e>2 && plist[2].typ!=PL_LEER)  varcaststring(plist[2].integer,plist[2].pointer,str);
#else
  if(e>1 && plist[1].typ!=PL_LEER)  varcastint(plist[1].integer,plist[1].pointer,ks);
  str.pointer=buf;
  str.len=strlen(buf);
  if(e>2 && plist[2].typ!=PL_LEER)  varcaststring(plist[2].integer,plist[2].pointer,str);
#endif

  if(e>3 && plist[3].typ!=PL_LEER)  varcastint(plist[3].integer,plist[3].pointer,event.xkey.state);
  if(e>4 && plist[4].typ!=PL_LEER)  varcastint(plist[4].integer,plist[4].pointer,event.xkey.x);
  if(e>5 && plist[5].typ!=PL_LEER)  varcastint(plist[5].integer,plist[5].pointer,event.xkey.y);
  if(e>6 && plist[6].typ!=PL_LEER)  varcastint(plist[6].integer,plist[6].pointer,event.xkey.x_root);
  if(e>7 && plist[7].typ!=PL_LEER)  varcastint(plist[7].integer,plist[7].pointer,event.xkey.y_root);
  }
#endif
#ifdef USE_SDL
  SDL_Event event;
  STRING str;
  char buf[4];
  graphics();
  if(SDL_WaitEvent(&event)==0) return;
  while(event.type!=SDL_KEYDOWN /* && event.type!=SDL_KEYUP */ ) { 
     handle_event(usewindow,&event);
     if(SDL_WaitEvent(&event)==0) return;
  }
  buf[0]=event.key.keysym.sym;

  str.pointer=buf;
  str.len=1;

  if(e>0 && plist[0].typ!=PL_LEER)  varcastint(plist[0].integer,plist[0].pointer,event.key.keysym.scancode);
  if(e>1 && plist[1].typ!=PL_LEER)  varcastint(plist[1].integer,plist[1].pointer,event.key.keysym.sym);
  if(e>2 && plist[2].typ!=PL_LEER)  varcaststring(plist[2].integer,plist[2].pointer,str);
  if(e>3 && plist[3].typ!=PL_LEER)  varcastint(plist[3].integer,plist[3].pointer,event.key.keysym.mod);
  if(e>4 && plist[4].typ!=PL_LEER)  varcastint(plist[4].integer,plist[4].pointer,event.key.state);
#endif
}
void c_allevent(PARAMETER *plist,int e) {
#ifdef WINDOWS_NATIVE
  HANDLE evn[3];
  graphics();
  evn[0]=keyevent;
  evn[1]=motionevent;
  evn[2]=buttonevent;
  ResetEvent(keyevent);
  ResetEvent(motionevent);
  ResetEvent(buttonevent);
  WaitForMultipleObjects(3,evn,FALSE,INFINITE);
  if(e>0 && plist[0].typ!=PL_LEER)  varcastint(plist[0].integer,plist[0].pointer,global_eventtype);
  if(e>1 && plist[1].typ!=PL_LEER)  varcastint(plist[1].integer,plist[1].pointer,global_mousex);
  if(e>2 && plist[2].typ!=PL_LEER)  varcastint(plist[2].integer,plist[2].pointer,global_mousey);
  if(e>5 && plist[5].typ!=PL_LEER)  varcastint(plist[5].integer,plist[5].pointer,global_mouses);
  if(e>6 && plist[6].typ!=PL_LEER)  {
    if(global_eventtype==MotionNotify) varcastint(plist[6].integer,plist[6].pointer,0);
    else if(global_eventtype==ButtonPress) varcastint(plist[6].integer,plist[6].pointer,global_mousek);
    else if(global_eventtype==KeyPress) varcastint(plist[6].integer,plist[6].pointer,global_keycode);
  }
  if(e>7 && plist[7].typ!=PL_LEER)  varcastint(plist[7].integer,plist[7].pointer,global_ks);
#endif
#if defined USE_X11 || defined FRAMEBUFFER
   XEvent event;   
   graphics();
#ifdef FRAMEBUFFER
   FB_mouse(1);
#endif
    
 //   printf("EVENT e=%d vnr=%d pointer=%x\n",e,plist->integer,plist->pointer);
 //   dump_parameterlist(plist,e);
    
   XWindowEvent(display[usewindow], win[usewindow],
         KeyPressMask|ButtonPressMask|PointerMotionMask|ExposureMask, &event);
   while(event.type==Expose) { 
     handle_event(usewindow,&event);
     XWindowEvent(display[usewindow], win[usewindow],KeyPressMask|ButtonPressMask|PointerMotionMask|ExposureMask, &event);
   }   
   if(e>0 && plist[0].typ!=PL_LEER)  varcastint(plist[0].integer,plist[0].pointer,event.type);
   if(e>1 && plist[1].typ!=PL_LEER) {
    	   if(event.type==MotionNotify)     varcastint(plist[1].integer,plist[1].pointer,event.xmotion.x);
	   else if(event.type==ButtonPress) varcastint(plist[1].integer,plist[1].pointer,event.xbutton.x);
	   else if(event.type==KeyPress)    varcastint(plist[1].integer,plist[1].pointer,event.xkey.x);
   }
   if(e>2 && plist[2].typ!=PL_LEER) {
    	   if(event.type==MotionNotify)        varcastint(plist[2].integer,plist[2].pointer,event.xmotion.y);
	   else if(event.type==ButtonPress) varcastint(plist[2].integer,plist[2].pointer,event.xbutton.y);
	   else if(event.type==KeyPress)     varcastint(plist[2].integer,plist[2].pointer,event.xkey.y);
   }
   if(e>3 && plist[3].typ!=PL_LEER) {
    	   if(event.type==MotionNotify)        varcastint(plist[3].integer,plist[3].pointer,event.xmotion.x_root);
	   else if(event.type==ButtonPress) varcastint(plist[3].integer,plist[3].pointer,event.xbutton.x_root);
	   else if(event.type==KeyPress)     varcastint(plist[3].integer,plist[3].pointer,event.xkey.x_root);
   }
   if(e>4 && plist[4].typ!=PL_LEER) {
    	   if(event.type==MotionNotify)        varcastint(plist[4].integer,plist[4].pointer,event.xmotion.y_root);
	   else if(event.type==ButtonPress) varcastint(plist[4].integer,plist[4].pointer,event.xbutton.y_root);
	   else if(event.type==KeyPress)     varcastint(plist[4].integer,plist[4].pointer,event.xkey.y_root);
   }
  if(e>5 && plist[5].typ!=PL_LEER) {
    	   if(event.type==MotionNotify)        varcastint(plist[5].integer,plist[5].pointer,event.xmotion.state &255);
	   else if(event.type==ButtonPress) varcastint(plist[5].integer,plist[5].pointer,event.xbutton.state);
	   else if(event.type==KeyPress)     varcastint(plist[5].integer,plist[5].pointer,event.xkey.state);
   }
   if(e>6 && plist[6].typ!=PL_LEER) {
    	   if(event.type==MotionNotify)        varcastint(plist[6].integer,plist[6].pointer,event.xmotion.state>>8);
	   else if(event.type==ButtonPress) varcastint(plist[6].integer,plist[6].pointer,event.xbutton.button);
	   else if(event.type==KeyPress)     varcastint(plist[6].integer,plist[6].pointer,event.xkey.keycode);
   }
  
  if(e>7 && plist[7].typ!=PL_LEER)  {
     if(event.type==MotionNotify)     ;     
     else if(event.type==ButtonPress) ;
     else if(event.type==KeyPress)  {
 #ifdef FRAMEBUFFER
         varcastint(plist[7].integer,plist[7].pointer,event.xkey.ks);
#else
             char buf[4];
             XComposeStatus status;
             KeySym ks;
     
             XLookupString((XKeyEvent *)&event,buf,sizeof(buf),&ks,&status);   
             varcastint(plist[7].integer,plist[7].pointer,ks);    
#endif
    }
  }
  if(e>8 && plist[8].typ!=PL_LEER)  {
     if(event.type==MotionNotify)     ;     
     else if(event.type==ButtonPress) ;
     else if(event.type==KeyPress)  {
       STRING str;
#ifdef FRAMEBUFFER
       str.pointer=event.xkey.buf;
       str.len=strlen(str.pointer);
      varcaststring(plist[8].integer,plist[8].pointer,str);
 #else
      char buf[4];
             XComposeStatus status;
             KeySym ks;
     
             XLookupString((XKeyEvent *)&event,buf,sizeof(buf),&ks,&status);   
	     str.pointer=buf;
#endif   
      str.len=strlen(str.pointer);
      varcaststring(plist[8].integer,plist[8].pointer,str);
    }
  }
  
#ifdef USE_SDL
  SDL_Event event;
  char buf[4];
  graphics();
  if(SDL_WaitEvent(&event)==0) return;
/*  ... to be completed ....*/
#endif
#ifdef FRAMEBUFFER
   FB_mouse(0);
#endif
#endif
}
void c_titlew(PARAMETER *plist,int e) {
  int winnr=DEFAULTWINDOW;
  if(plist->typ!=PL_LEER) winnr=plist->integer;
  if(winnr>0 && winnr<MAXWINDOWS) {
    graphics();
#ifdef WINDOWS_NATIVE
  SetWindowText(win_hwnd[winnr],plist[1].pointer);
#endif
#ifdef USE_X11
    if (!XStringListToTextProperty((char **)&(plist[1].pointer), 1, &win_name[winnr]))    
      printf("Couldn't set Name of Window.\n");
    XSetWMName(display[winnr], win[winnr], &win_name[winnr]);
#endif
#ifdef USE_SDL
  SDL_WM_SetCaption(plist[1].pointer,NULL);
#endif
  } else xberror(98,""); /* Illegal Window number %s (0-16) */
}
void c_infow(PARAMETER *plist,int e) {  /* Set the Icon Name */
  int winnr=usewindow;
  if(plist->typ!=PL_LEER) winnr=plist->integer;
  if(winnr>0 && winnr<MAXWINDOWS) {
#ifdef USE_X11
    graphics();
    XSetIconName(display[winnr], win[winnr],plist[1].pointer);
#endif
#ifdef USE_SDL
  SDL_WM_SetCaption(NULL,plist[1].pointer);
#endif
  } else xberror(98,""); /* Illegal Window number %s (0-16) */
}
void c_clearw(PARAMETER *plist,int e) {
  int winnr=usewindow,x,y;
  unsigned int w,h,b,d;
  if(e) winnr=max(0,plist[0].integer);
  if(winnr<MAXWINDOWS) {
    graphics();
    if(winbesetzt[winnr]) {  	
#ifdef WINDOWS_NATIVE
      RECT interior;

      GetClientRect(win_hwnd[winnr],&interior);
      FillRect(bitcon[winnr],&interior,(HBRUSH)COLOR_WINDOW);

#endif
#ifdef USE_X11
      Window root;
      GC sgc;   
      XGCValues gc_val;  
       /* Erst den Graphic-Kontext retten  */
      sgc=XCreateGC(display[winnr], win[winnr], 0, &gc_val);
      XCopyGC(display[winnr], gc[winnr],GCForeground , sgc); 
      XGetGeometry(display[winnr],win[winnr],&root,&x,&y,&w,&h,&b,&d); 
      XSetForeground(display[winnr],gc[winnr],get_bcolor());
      XFillRectangle(display[winnr],pix[winnr],gc[winnr],x,y,w,h); 

      /* XClearWindow(display[winnr],win[winnr]); */
      
      XCopyGC(display[winnr], sgc,GCForeground, gc[winnr]);
      XFreeGC(display[winnr],sgc); 
#endif
#ifdef USE_SDL
  SDL_FillRect(display[winnr],NULL,0);
#endif
#ifdef FRAMEBUFFER
     FB_Clear(&screen);
#endif
    }
  }
}
void c_closew(PARAMETER *plist,int e) {
  int winnr=usewindow;
  if(e) winnr=plist[0].integer;
  if(winnr>0 && winnr<MAXWINDOWS) {
    if(winbesetzt[winnr]) {
      graphics();
      close_window(winnr);
    } else  xberror(99,""); /* Window %s does not exist  */
  } else xberror(98,""); /* Illegal Window number %s (0-16) */
}
void c_openw(PARAMETER *plist,int e) {
  int winnr=DEFAULTWINDOW;
  if(e) winnr=plist[0].integer;
  if(winnr>0 && winnr<MAXWINDOWS) {
    if(!winbesetzt[winnr]) {
      graphics();
      open_window(winnr);
    }
  } else xberror(98,""); /* Illegal Window number %s (0-16) */
}

void c_sizew(PARAMETER *plist,int e) {
  int winnr;
  if(plist->typ!=PL_LEER) winnr=plist->integer;
  else winnr=usewindow;
  
  if(winnr>0 && winnr<MAXWINDOWS) {
    graphics();
    if(winbesetzt[winnr]) {do_sizew(winnr,plist[1].integer,plist[2].integer);}
    else  xberror(99,""); /* Window %s does not exist  */
  } else if(winnr==0) xberror(97,"SIZEW"); /* This operation %s is not allowed for root window  */
  else xberror(98,""); /* Illegal Window number %s (0-16) */
}
void c_movew(PARAMETER *plist,int e) {
  int winnr;
  if(plist->typ!=PL_LEER) winnr=plist->integer;
  else winnr=usewindow;
  
  if(winnr>0 && winnr<MAXWINDOWS) {
    graphics();
    if(winbesetzt[winnr]) {do_movew(winnr,plist[1].integer,plist[2].integer);}
    else  xberror(99,""); /* Window %s does not exist  */
  } else if(winnr==0) xberror(97,"MOVEW"); /* This operation %s is not allowed for root window  */
  else xberror(98,""); /* Illegal Window number %s (0-16) */
}

void c_fullw(PARAMETER *plist,int e) {
  int winnr=usewindow;
  if(e) winnr=plist->integer;
  if(winnr>0 && winnr<MAXWINDOWS) {
    if(winbesetzt[winnr]) {
#ifdef USE_X11
      Window root;
      int ox,oy;
      unsigned int ow,oh,ob;
      unsigned int d;
      graphics();
      do_movew(winnr,0,0);
      XGetGeometry(display[winnr],RootWindow(display[winnr],
        DefaultScreen(display[winnr])),&root,&ox,&oy,&ow,&oh,&ob,&d);
      do_sizew(winnr,ow,oh);
#endif
#ifdef USE_SDL
/*Hier haben wir die chance, den fullscreenmodus zu aktivieren...*/
#endif
    } else  xberror(99,""); /* Window %s does not exist  */
  } else if(winnr==0) xberror(97,"FULLW"); /* This operation %s is not allowed for root window  */
  else xberror(98,""); /* Illegal Window number %s (0-16) */
}

void c_topw(PARAMETER *plist,int e) {
  int winnr=usewindow;
  if(e) winnr=plist->integer;
  if(winnr==0) xberror(97,"TOPW"); /* This operation %s is not allowed for root window  */
  else if(winnr>0 && winnr<MAXWINDOWS) {
    if(winbesetzt[winnr]) {
      graphics();
#ifdef USE_X11
      XRaiseWindow(display[winnr], win[winnr]);
#endif
    } else  xberror(99,""); /* Window %s does not exist  */
  } else xberror(98,""); /* Illegal Window number %s (0-16) */
}
void c_bottomw(PARAMETER *plist,int e) {
  int winnr=usewindow;
  if(e) winnr=plist->integer;
  if(winnr==0) xberror(97,"BOTTOMW"); /* This operation %s is not allowed for root window  */
  else if(winnr>0 && winnr<MAXWINDOWS) {
    if(winbesetzt[winnr]) {
#ifdef USE_X11
      graphics();
      XLowerWindow(display[winnr], win[winnr]);
#endif
    } else  xberror(99,""); /* Window %s does not exist  */
  } else xberror(98,""); /* Illegal Window number %s (0-16) */
}

#include "bitmaps/biene.bmp"
#include "bitmaps/biene_mask.bmp"
#include "bitmaps/hand.bmp"
#include "bitmaps/hand_mask.bmp"
#include "bitmaps/zeigehand.bmp"
#include "bitmaps/zeigehand_mask.bmp"


void c_defmouse(PARAMETER *plist,int e) {
  int form=plist[0].integer,formt;

#ifdef FRAMEBUFFER
FB_hide_mouse();
if(form==0) {
  screen.mousemask=mousealpha;
  screen.mousepat=mousepat;
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
#endif
#ifdef USE_X11
  Cursor maus;
 
  if(form<0) formt=-form;   
  else if(form==0) formt=68;     /* Pfeil  */
  else if(form==1) formt=152;    /* Doppelklammer  */
  else if(form==2) formt=150;    /* Biene  bzw. Sanduhr*/
  else if(form==3) formt=60;     /* Hand mit Zeigefinger  */
  else if(form==4) formt=24;     /* offene Hand */
  else if(form==5) formt=34;     /* Fadenkreuz fein */
  else if(form==6) formt=90;     /* Fadenkreuz grob */
  else if(form==7) formt=30;     /* Fadenkreuz weiss mit Umrandung */
  else if(form>=8) formt=(form-8)*2;
  if(form==2 || form==3 || form==4) {
    Pixmap mausp,mausm;
    XColor f,b;
    int hotx=8,hoty=8;
    graphics();
    if(form==2) {
      mausp=XCreateBitmapFromData(display[usewindow],win[usewindow],
                (char *)biene_bits,biene_width,biene_height);
      mausm=XCreateBitmapFromData(display[usewindow],win[usewindow],
                (char *)biene_mask_bits,biene_mask_width,biene_mask_height);
    } else if(form==3) {
      hotx=0;hoty=0;
      mausp=XCreateBitmapFromData(display[usewindow],win[usewindow],
                (char *)zeigehand_bits,zeigehand_width,zeigehand_height);
      mausm=XCreateBitmapFromData(display[usewindow],win[usewindow],
                (char *)zeigehand_mask_bits,zeigehand_mask_width,zeigehand_mask_height);
    } else if(form==4) {
      mausp=XCreateBitmapFromData(display[usewindow],win[usewindow],
                (char *)hand_bits,hand_width,hand_height);
      mausm=XCreateBitmapFromData(display[usewindow],win[usewindow],
                (char *)hand_mask_bits,hand_mask_width,hand_mask_height);

    }
    f.pixel=1;b.pixel=0;
    f.red=0;b.red=65535;    f.green=0;b.green=65535;f.blue=0;b.blue=65535;
    
    maus=XCreatePixmapCursor(display[usewindow],mausp,mausm,&f,&b,hotx,hoty);
    XDefineCursor(display[usewindow], win[usewindow], maus);
    XFreePixmap(display[usewindow],mausp);XFreePixmap(display[usewindow],mausm);
  } else if(formt<153 && formt>=0) {
    graphics();
    maus=XCreateFontCursor(display[usewindow],formt);
    XDefineCursor(display[usewindow], win[usewindow], maus);
  }
#endif
#ifdef USE_SDL
  static SDL_Cursor *cursor=NULL;
  static SDL_Cursor defcursor;

  char *data,*mask;
  char d2[32],m2[32];
  int hot_x,hot_y;
  int i,j;
  unsigned char a,b;
  graphics();
  if(!cursor) defcursor=*SDL_GetCursor();
  switch(form) {
  case 2:
    data=(char *)biene_bits;
    mask=(char *)biene_mask_bits;
    hot_x=hot_y=8;
    break;
  case 3:
    data=(char *)zeigehand_bits;
    mask=(char *)zeigehand_mask_bits;
    hot_x=hot_y=0;
    break;
  case 4:
    data=(char *)hand_bits;
    mask=(char *)hand_mask_bits;
    hot_x=hot_y=8;
    break;
  default:
      hot_x=hot_y=0;
      data=NULL;
    }
    if(data) {
    for(i=0;i<32;i++) {
      a=data[i];
      b=0;
      for(j=0;j<8;j++) {
        b<<=1;
        if((a>>j)&1) b=b|1;
      }
      d2[i]=b;
    }
    for(i=0;i<32;i++) {
      a=mask[i];
      b=0;
      for(j=0;j<8;j++) {
        b<<=1;
        if((a>>j)&1) b=b|1;
      }
      m2[i]=b;
    }
    SDL_FreeCursor(cursor);
    cursor=SDL_CreateCursor(d2,m2,16,16,hot_x,hot_y);
    SDL_SetCursor(cursor);
  } else SDL_SetCursor(&defcursor);
#endif
}
void g_outs(STRING t);
void g_out(char a) {
  static int lin=0,col=0;
  extern ARECT sbox;
  int bbb;
  switch(a) {
  case 0: break;
  case 7: printf("\007");break;
  case 8: if(col) col--; break;
  case 10: lin++; col=0; 
    if(lin*chh>=sbox.h){
      lin--;
      CopyArea(0,chh,sbox.w,sbox.h-chh,0,0);
      bbb=get_fcolor();
      SetForeground(get_bcolor());
      FillRectangle(0,chh*lin,sbox.w,chh);
      SetForeground(bbb);
    } 
    break;
  case 13: col=0; break;
  default:
    DrawString(col*chw,lin*chh+chh,&a,1);
    col++;
    if(col*chw>=sbox.w) {col=0; lin++;
      if(lin*chh>=sbox.h) {
        lin--;
        CopyArea(0,chh,sbox.w,sbox.h-chh,0,0);
        bbb=get_fcolor();
        SetForeground(get_bcolor());
        FillRectangle(0,chh*lin,sbox.w,chh);
        SetForeground(bbb);
      } 
    }
  } 
}
void g_outs(STRING t) {
  int i;
  if(t.len) {
    for(i=0;i<t.len;i++) g_out(t.pointer[i]);
  }
}


void c_text(PARAMETER *plist,int e) {
  graphics();
  draw_string(plist[0].integer,plist[1].integer,plist[2].pointer,plist[2].integer);
}

void c_gprint(PARAMETER *plist,int e) {
  if(e) {
    int i;
    char *v;

    for(i=0;i<e;i++) {
      if(plist[i].typ==PL_EVAL || plist[i].typ==PL_KEY) {
          v=plist[i].pointer;
          if(strlen(v)) {
	    STRING buffer=print_arg(v);
	    g_outs(buffer);
            free(buffer.pointer);
	    if(i!=e-1) g_out('\011');
	    else {
              if(v[strlen(v)-1]!=';' && v[strlen(v)-1]!='\'') 
	      g_out('\n');
            }
	  } else {
	    if(i!=e-1) g_out('\011');
	  }
      } else if(plist[i].typ==PL_LEER) {
        if(i!=e-1) g_out('\011');
      } else printf("GPRINT: Falscher typ. $%x\n",plist[i].typ);
    }
  } else g_out('\n');
}


void c_ltext(PARAMETER *plist,int e) {
  graphics();
  ltext(plist[0].integer,plist[1].integer,ltextxfaktor,ltextyfaktor,ltextwinkel,ltextpflg,plist[2].pointer); 
}

void c_alert(PARAMETER *plist,int e) {
  /* setzt nur das Format in einen FORM_ALERT Aufruf um */
  STRING str;
  char buffer[MAXSTRLEN];
  str.pointer=malloc(MAXSTRLEN);
  if(e>4) {
    sprintf(buffer,"[%d][%s][%s]",plist[0].integer,(char *)plist[1].pointer,(char *)plist[3].pointer);
    varcastint(plist[4].integer,plist[4].pointer,form_alert2(plist[2].integer,buffer,str.pointer));
    str.len=strlen(str.pointer);
  }
  if(e>5) varcaststring(plist[5].integer,plist[5].pointer,str);
  free(str.pointer);
}

char *fsel_input(char *,char *,char *);

void c_fileselect(PARAMETER *plist,int e) {
  STRING str;
  str.pointer=fsel_input(plist[0].pointer,plist[1].pointer,plist[2].pointer);
  str.len=strlen(str.pointer);
  varcaststring(plist[3].integer,plist[3].pointer,str);
  free(str.pointer);
}


int menuaction=-1;
int menuflags[MAXMENUENTRYS];
char *menuentry[MAXMENUENTRYS];
int menuentryslen[MAXMENUENTRYS];
char *menutitle[MAXMENUTITLES];
int menutitleslen[MAXMENUTITLES];
int menutitlesp[MAXMENUTITLES];
int menutitlelen[MAXMENUTITLES];
int menutitleflag[MAXMENUTITLES];
int menuanztitle=0;

void c_menu(char *n) {
  int sel;
  int pc2;
  char pos[20];
  if(menuaction!=-1) {
    sel=do_menu_select();
    if(sel>=0) {
      sprintf(pos,"%d",sel);
      if(do_parameterliste(pos,procs[menuaction].parameterliste,procs[menuaction].anzpar)) xberror(42,pos); /* Zu wenig Parameter */
      else {
        batch=1;
        pc2=procs[menuaction].zeile;
        if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
        else {printf("Stack-Overflow ! PC=%d\n",pc); batch=0;}
      }
    }
  }
}
void c_menudef(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int i=0,e;
  int count=0,nn,vnr=-1,typ,pc2=-1;
  char *r;
  STRING *varptr;
  e=wort_sep(n,',',TRUE,w1,w2);
  while(e) {
       if(strlen(w1)) {
       switch(i) {
         case 0: {
	   typ=type(w1)&(~CONSTTYP);
	   if((typ & (STRINGTYP|ARRAYTYP))==(STRINGTYP|ARRAYTYP)) {
             r=varrumpf(w1);
             vnr=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnr==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else printf("MENUDEF: no STRINGARRAY.\n");
	   break;
	   }
	 case 1: {   
	     pc2=procnr(w1,1);
	 break;
	   } 	 
         default: break;
       }
     }
     e=wort_sep(w2,',',TRUE,w1,w2);
     i++;
  }
  if(i<2) xberror(42,""); /* Zu wenig Parameter  */
  else {
    if(pc2==-1)   xberror(19,w2); /* Procedure nicht gefunden */
    else {
      if(vnr>-1) {
 	nn=do_dimension(&variablen[vnr]);
	varptr=(STRING *)(variablen[vnr].pointer.a->pointer+variablen[vnr].pointer.a->dimension*INTSIZE);
	menuanztitle=0;
	count=0;
	for(i=0;i<nn;i++) {
	  menuentry[i]=varptr[i].pointer;
	  menuentryslen[i]=varptr[i].len;
	  if(count==0 && varptr[i].len) {
	    menutitle[menuanztitle]=varptr[i].pointer;
	    menutitleslen[menuanztitle]=varptr[i].len;
	    menutitlesp[menuanztitle]=i+1;
	    menutitleflag[menuanztitle]=NORMAL;
	    count++;
	  } else if(varptr[i].len) {
	    if(*(varptr[i].pointer)=='-') menuflags[i]=DISABLED; else menuflags[i]=NORMAL;
	    count++;
	  } else if(count) {
	    menutitlelen[menuanztitle]=count-1;
	    menuanztitle++;
	    count=0;
	  }
	}
	menutitlelen[menuanztitle]=count;
	menuanztitle++;
        menuaction=pc2;
        do_menu_draw(); 
      }
    }
  }
}
void c_menuset(PARAMETER *plist, int e) {
  if(e==2) {
    int nr=plist[0].integer;
    if(nr<MAXMENUENTRYS && nr>0) menuflags[nr]=plist[1].integer;    
    else printf("Nr. des Menueintrags zu gross. Max %d.\n",MAXMENUENTRYS);
  }
}
void c_menukill(char *n) {
  if(menuaction!=-1) {
    menuaction=-1;
  }
}


/**************  RSRC-Library *******************************************/


void c_rsrc_load(PARAMETER *plist,int e) {
  if(rsrc_load(plist[0].pointer)) xberror(72,"");  /*  Fehler bei RSRC_LOAD*/
}
void c_rsrc_free(char *n) {
  if(rsrc_free()) xberror(73,"");  /*  Fehler bei RSRC_FREE*/
}
void c_objc_add(PARAMETER *plist,int e) {
  objc_add((OBJECT *)plist[0].integer,plist[1].integer,plist[2].integer);
}
void c_objc_delete(PARAMETER *plist,int e) {
  objc_delete((OBJECT *)plist[0].integer,plist[1].integer);
}
void c_xload(char *n) {
  char *name=fsel_input("Load program:    ","./*.bas","");
  if(strlen(name)) {
    if(exist(name)) {
      programbufferlen=0; 
      mergeprg(name);
    } else printf("LOAD/MERGE: File %s not found!\n",name);
  }
  free(name);
}
void c_xrun(char *n) {
  c_xload(n);
  do_run();
}
#endif /* NOGRAPHICS */