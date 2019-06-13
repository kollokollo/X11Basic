/* framebuffer.c      Grafik-Routinen fuer Framebuffer-Systeme (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */



#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#ifndef ANDROID
#include <sysexits.h>
#else
#define EX_OSFILE 0
#define EX_IOERR 0
#endif
#include <math.h>

#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/reboot.h>
#include <sys/time.h>
#include <sys/mman.h>
#ifndef ANDROID
#ifndef __APPLE__
#include <linux/fb.h>
#include <linux/ioctl.h>
#endif
#else
#include <android/bitmap.h>
#endif

#include "defs.h"
#include "x11basic.h"

#include "graphics.h"
#include "bitmap.h"
#include "framebuffer.h"
#include "consolefont.h"
#include "raw_mouse.h"
#include "raw_keyboard.h"

#ifndef ANDROID
extern struct fb_var_screeninfo vinfo;
extern struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
int font_behaviour;
#else
extern AndroidBitmapInfo  screen_info;
#endif

int fbfd = -1;
extern int font_behaviour;
G_CONTEXT screen;


void textscreen_redraw(int x, int y, int w,int h);
extern int bigfont;

#ifndef ANDROID
char fbdevname[256]=FB_DEVICENAME;
#endif
void Fb_Open() {
#ifndef ANDROID 
  fbfd=open(fbdevname, O_RDWR);
  if(fbfd<0) {
    printf("ERROR: could not open framebuffer device %s.\n",fbdevname);
    perror(fbdevname);
    exit(EX_OSFILE);
  }
  // Get fixed screen information
  if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo)) {
    perror("ERROR: Could not get fixed screen information.");
    exit(EX_IOERR);
  }
  // Get variable screen information
  if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo)) {
    perror("ERROR: Could not get variable screen information.");
    exit(EX_IOERR);
  }
  screen.bpp=vinfo.bits_per_pixel;
  if(screen.bpp!=16) printf("WARNING: The color depth of the framebuffer should be 16!\n");
  screen.clip_w=screen.width=vinfo.xres;
  screen.clip_h=screen.height=vinfo.yres;
#else
  fbfd=1;
  screen.bpp=16;
  screen.clip_w=screen.width=screen_info.width;
  screen.clip_h=screen.height=screen_info.height;
#endif
  screen.clip_x=screen.clip_y=0;
  if(screen.initialized!=4711) {
    FB_defaultcontext();
  } else {
    screen.mouse_x=min(screen.width,screen.mouse_x);
    screen.mouse_y=min(screen.height,screen.mouse_y);
  }
  // Figure out the size of the screen in bytes
  screen.size = screen.width * screen.height * screen.bpp / 8;
  screen.scanline=screen.width*screen.bpp/8;


  // Map the device to memory
#ifndef ANDROID
  screen.pixels = (unsigned char *)mmap(0, screen.size, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
  if(screen.pixels==MAP_FAILED) {
    perror("ERROR: Could not map framebuffer device to memory.");
    exit(EX_IOERR);
  }
#else
  extern void *screen_pixels;
  screen.pixels = (char *)screen_pixels;
#endif

  if(font_behaviour==0) { /* This means: auto  */
    if(screen.width<200) change_fontsize(0);  /*5x7 font*/
    else if(screen.width<=480) change_fontsize(1); /*8x16 font*/
    else if(screen.width<=960) change_fontsize(2); /*16x32 font*/
    else if(screen.width<=1200) change_fontsize(3); /*24x48 font*/
    else change_fontsize(3);  /*32x64 font*/
  } else change_fontsize(font_behaviour-1);

  /* Now set the padding to zero, otherwise it can happen that nothing is visible
    if the pading was set to the second page...*/

#ifndef ANDROID
  vinfo.xoffset=0;
  vinfo.yoffset=0;
  ioctl(fbfd,FBIOPAN_DISPLAY,&vinfo);
#endif
#if DEBUG
  printf("%dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel );
#endif
  // printf("Screen size: %d x %d\n",screen.width, screen.height);
 FB_show_mouse();
 screen.initialized=4711;
}

void Fb_Close() {
#ifndef ANDROID
  if(fbfd > 0) {
    munmap(screen.pixels, screen.size);
    close(fbfd);
  }
#endif
  FB_hidex_mouse();
#ifndef ANDROID
  FB_close_mouse();
  FB_close_keyboard();
#endif
  fbfd = -1;
}

static G_CONTEXT backup;

void FB_savecontext() {
   backup=screen;
}
void FB_restorecontext() {
  screen.fcolor=backup.fcolor;
  screen.bcolor=backup.bcolor;
  screen.graphmode=backup.graphmode;
  screen.textmode=backup.textmode;
  screen.linewidth=backup.linewidth;
  screen.fill_rule=backup.fill_rule;
  screen.fill_style=backup.fill_style;
  screen.fill_pat=backup.fill_pat;
  
}
void FB_defaultcontext() {
  screen.fcolor=YELLOW;
  screen.bcolor=BLACK;
  screen.mouse_ox=8;
  screen.mouse_oy=8;
  screen.mouse_x=screen.width/2;
  screen.mouse_y=screen.height/2;
  screen.mousemask=(unsigned char *)mousealpha;
  screen.mousepat=(unsigned short *)mousepat;
  screen.alpha=255;
  screen.graphmode=1;
  screen.linewidth=0;  /* 0 means thin lines */
  screen.fill_rule=0;
  screen.fill_style=0;
  screen.fill_pat=NULL;
}


void FB_setgraphmode(int n) {
  screen.graphmode=n;
}


/* Hier folgen nun die Grafik-Routinen für primitive.

   Es gibt immer zwei Versionen der Routinen, 
   1. Eine schnelle ohne Maus-Abschaltung (interne verwendung oder terminal emu)
   2. Eine normale, bei der Mauszeiger nicht übermalt wird.
   TODO:
   Ist noch nicht vollstaendig aussortiert....
   */


/* This is a low-level Function, need to be fast, but does noch check 
   clipping */

inline static void FB_PutPixel_noclip(int x, int y, unsigned short color) {
  unsigned short *ptr  = (unsigned short*)(screen.pixels+x*2+y*screen.scanline);
  *ptr = color;
}
inline static void FB_PutPixel_noclip_alpha(int x, int y, unsigned short color, unsigned char alpha) {
  unsigned short *ptr  = (unsigned short*)(screen.pixels+x*2+y*screen.scanline);
  *ptr = mix_color(color,*ptr,alpha);
}

inline static void FB_PutPixel(int x, int y, unsigned short color) {
  if(x<screen.clip_x) return;
  if(y<screen.clip_y) return;
  if(x>=screen.clip_x+screen.clip_w) return;
  if(y>=screen.clip_y+screen.clip_h) return;
  
  unsigned short *ptr  = (unsigned short*)(screen.pixels+x*2+y*screen.scanline);
  if(screen.alpha==255)  *ptr = color;
  else *ptr = mix_color(color,*ptr,screen.alpha);
}



void Fb_Scroll(int target_y, int source_y, int height) {
  memmove(screen.pixels+target_y*screen.scanline,
          screen.pixels+source_y*screen.scanline,height*screen.scanline);
}

void FB_set_color(unsigned short color)  {screen.fcolor=color; }
void FB_set_bcolor(unsigned short color) {screen.bcolor=color; }
void FB_set_alpha(unsigned char alpha)   {screen.alpha=alpha;  }
void FB_set_textmode(unsigned int mode)  {screen.textmode=mode;}

void FB_set_clip(G_CONTEXT *screen,int x,int y,int w,int h) {
  screen->clip_x=x;
  screen->clip_y=y;
  screen->clip_w=w;
  screen->clip_h=h;
}
void FB_clip_off(G_CONTEXT *screen) {
  screen->clip_x=0;
  screen->clip_y=0;
  screen->clip_w=screen->width;
  screen->clip_h=screen->height;
}

void FB_plot(int x, int y) {
  FB_hidex_mouse();
  FB_PutPixel(x,y,screen.fcolor);
 // FB_showx_mouse(); muss nicht sein (nur langsam) Mousebewegung aktiviert es wieder
}
unsigned short FB_point(int x, int y) {
  if(x<0 || y<0 || x>=screen.width || y>=screen.height) return(0);
  unsigned short *ptr  = (unsigned short*)(screen.pixels+x*2+y*screen.scanline);
  return(*ptr);
}
static void DrawHorizontalLine(int X, int Y, int width, unsigned short color) {
  register int w = width; 

  if (Y<screen.clip_y) return;
  if (Y>=screen.clip_y+screen.clip_h) return;

  if (X<screen.clip_x)      // clip left margin
    { w-=(screen.clip_x-X); X=screen.clip_x; }
  if (w>screen.clip_x+screen.clip_w-X)    // clip right margin
    w=screen.clip_x+screen.clip_w-X;

  if(screen.alpha==255) {
    while(w-->=0) FB_PutPixel_noclip(X++,Y,color);
  } else {
    while(w-->=0) FB_PutPixel_noclip_alpha(X++,Y,color,screen.alpha);
  }
}






void FB_setfillpattern(const char *p) { screen.fill_pat=(unsigned short *)p; }
void FB_setfillstyle  (int c)         { screen.fill_style=c; }
void FB_setfillrule   (int c)         { screen.fill_rule=c;  }


/*Draw a horizontal line and use pattern, transparent*/

static void DrawHorizontalLinePatt(int X, int Y, int width, unsigned short color, unsigned short pat) {
  if (Y<screen.clip_y) return;
  if (Y>=screen.clip_y+screen.clip_h) return;
  register int w = width; 

  if (Y<screen.clip_y) return;
  if (Y>=screen.clip_y+screen.clip_h) return;

  if (X<screen.clip_x)      // clip left margin
    { w-=(screen.clip_x-X); X=screen.clip_x; }
  if (w>screen.clip_x+screen.clip_w-X)    // clip right margin
    w=screen.clip_x+screen.clip_w-X;

  if(screen.alpha==255) {
    while(w-->=0) {
      if((pat>>(X&0xf))&1) FB_PutPixel_noclip(X,Y,color);
      X++;
    }
  } else {
    while(w-->=0) {
      if((pat>>(X&0xf))&1) FB_PutPixel_noclip_alpha(X,Y,color,screen.alpha);
      X++;
    }
  }
}

static void DrawHorizontalLinePattBg(int X, int Y, int width, unsigned short color,unsigned short bgcolor, unsigned short pat) {
  if (Y<screen.clip_y) return;
  if (Y>=screen.clip_y+screen.clip_h) return;
  register int w = width; 

  if (Y<screen.clip_y) return;
  if (Y>=screen.clip_y+screen.clip_h) return;

  if (X<screen.clip_x)      // clip left margin
    { w-=(screen.clip_x-X); X=screen.clip_x; }
  if (w>screen.clip_x+screen.clip_w-X)    // clip right margin
    w=screen.clip_x+screen.clip_w-X;

  if(screen.alpha==255) {
    while(w-->=0) {
      if((pat>>(X&0xf))&1) FB_PutPixel_noclip(X,Y,color);
      else FB_PutPixel_noclip(X,Y,bgcolor);
      X++;
    }
  } else {
    while(w-->=0) {
      if((pat>>(X&0xf))&1) FB_PutPixel_noclip_alpha(X,Y,color,screen.alpha);
      X++;
    }
  }
}




static void DrawVerticalLine(int X, int Y, int height, unsigned short color) {
  register int h = height;  // in pixels

  if (X<screen.clip_x) return;
  if (X>=screen.clip_x+screen.clip_w) return;

  if (screen.clip_y>Y) { 
    h-=(screen.clip_y-Y); 
    Y=screen.clip_y; 
  }
  if(Y+h>screen.clip_y+screen.clip_h) h=screen.clip_y+screen.clip_h-X;
  if(screen.alpha==255) {
    while(h-->=0) FB_PutPixel_noclip(X,Y++,color);
  } else {
    while(h-->=0) FB_PutPixel_noclip_alpha(X,Y++,color,screen.alpha);
  }
}



static void fillLine(int x1,int x2,int y,unsigned short color) {
  if(screen.fill_pat && screen.fill_style==FillStippled) {
    if(screen.graphmode<=1) {
      if(x2>=x1) DrawHorizontalLinePattBg(x1,y,x2-x1,color,screen.bcolor,*((unsigned short *)(screen.fill_pat+(y&0xf))));
      else DrawHorizontalLinePattBg(x1,y,x1-x2,color,screen.bcolor,*((unsigned short *)(screen.fill_pat+(y&0xf))));
    } else {
      if(x2>=x1) DrawHorizontalLinePatt(x1,y,x2-x1,color,*((unsigned short *)(screen.fill_pat+(y&0xf))));
      else DrawHorizontalLinePatt(x1,y,x1-x2,color,*((unsigned short *)(screen.fill_pat+(y&0xf))));
   
    }
  } else {
    if(x2>=x1) DrawHorizontalLine(x1,y,x2-x1,color);
    else DrawHorizontalLine(x1,y,x1-x2,color);
  }
}

/* Bresenham's line drawing algorithm single with */

void FB_DrawLine(int x0, int y0, int x1, int y1,unsigned short color) {
  int dy=y1-y0;
  int dx=x1-x0;
  int stepx,stepy;

  if(dy==0) {
    if(dx==0) {FB_PutPixel(x0, y0,color); return;}
    /* Swap x1, x2 if required */
    if(x0>x1) {int xtmp=x0;x0=x1;x1=xtmp;}
    DrawHorizontalLine(x0, y0,x1-x0,color); return;
  } 
  if(dy<0) {dy=-dy; stepy=-1;} else stepy=1;
  if(dx<0) {dx=-dx; stepx=-1;} else stepx=1;
  dy<<=1;	// dy is now 2*dy
  dx<<=1;	// dx is now 2*dx

  FB_PutPixel(x0, y0,color);

  if(dx>dy) {
    int fraction=dy-(dx>>1);     // same as 2*dy - dx
    while(x0!=x1) {
	if(fraction>=0) {
	  y0+=stepy;
	  fraction-=dx;	    // same as fraction -= 2*dx
	}
	x0+=stepx;
	fraction+=dy;	    // same as fraction -= 2*dy
	FB_PutPixel(x0,y0,color);
    }
  } else {
    int fraction=dx-(dy>>1);
    while(y0!=y1) {
      if(fraction>=0) {
        x0 += stepx;
        fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
      FB_PutPixel(x0, y0,color);
    }
  }
}
#define HYPOT(x,y) sqrt((double)(x)*(double)(x)+(double)(y)*(double)(y)) 

void FB_DrawThickLine(int x0, int y0, int x1, int y1,int width, unsigned short color) {
  int dy=y1-y0;
  int dx=x1-x0;
  int stepx,stepy;

  int ddx1,ddy1;
  int ddx2,ddy2;
  int w2=width>>1;
  
  if(dx==0) {
    int i;
    /* Swap y1, y2 if required */
    if(y0>y1) {int ytmp=y0;y0=y1;y1=ytmp;}
    for (i=y0; i<=y1; i++) {
      DrawHorizontalLine(x0-w2, i, width, color);
    }  
    return;
  }
  if(dy==0) {
    int i;
    /* Swap x1, x2 if required */
    if(x0>x1) {int xtmp=x0;x0=x1;x1=xtmp;}
    for (i=0; i<width; i++) {
      DrawHorizontalLine(x0, y0-w2+i,x1-x0, color);
    }  
    return;
  }


  if(dy<0) {dy=-dy; stepy=-1;} else stepy=1;
  if(dx<0) {dx=-dx; stepx=-1;} else stepx=1;

  width--;

  ddx1=(int)(dy/HYPOT(dx,dy)*width/2);
  ddy1=(int)(dx/HYPOT(dx,dy)*width/2);
  if(width&1) {
    ddx2=(int)(dy/HYPOT(dx,dy)*(width+1)/2);
    ddy2=(int)(dx/HYPOT(dx,dy)*(width+1)/2);
  } else {
    ddx2=ddx1;
    ddy2=ddy1;
  }
  // printf("(%d/%d) (%d/%d) %d\n",ddx1,ddy1,ddx2,ddy2,width);
  dy<<=1;	// dy is now 2*dy
  dx<<=1;	// dx is now 2*dx


  FB_DrawLine(x0-ddx1*stepx,y0+ddy1*stepy,x0+ddx2*stepx,y0-ddy2*stepy,color);

  if(dx>dy) {
    int fraction=dy-(dx>>1);     // same as 2*dy - dx
    while(x0!=x1) {
	if(fraction>=0) {
	  y0+=stepy;
  FB_DrawLine(x0-ddx1*stepx,y0+ddy1*stepy,x0+ddx2*stepx,y0-ddy2*stepy,color);
	  fraction-=dx;	    // same as fraction -= 2*dx
	}
	x0+=stepx;
	fraction+=dy;	    // same as fraction -= 2*dy
  FB_DrawLine(x0-ddx1*stepx,y0+ddy1*stepy,x0+ddx2*stepx,y0-ddy2*stepy,color);

    }
  } else {
    int fraction=dx-(dy>>1);
    while(y0!=y1) {
      if(fraction>=0) {
        x0 += stepx;
  FB_DrawLine(x0-ddx1*stepx,y0+ddy1*stepy,x0+ddx2*stepx,y0-ddy2*stepy,color);

        fraction -= dy;
      }
      y0 += stepy;
      fraction += dx;
  FB_DrawLine(x0-ddx1*stepx,y0+ddy1*stepy,x0+ddx2*stepx,y0-ddy2*stepy,color);
    }
  }
}

static void FB_doline(int x1,int y1,int x2,int y2) {
  if(screen.linewidth) FB_DrawThickLine(x1,y1,x2,y2,screen.linewidth, screen.fcolor);
  else {
    if(y1==y2) {
      if(x2>=x1) DrawHorizontalLine(x1,y1,x2-x1,screen.fcolor);
      else DrawHorizontalLine(x2,y1,x1-x2,screen.fcolor);
    } else FB_DrawLine(x1,y1,x2,y2,screen.fcolor);
  }
}
void FB_line(int x1,int y1,int x2,int y2) {
  FB_hidex_mouse();
  FB_doline(x1,y1,x2,y2);
  // FB_showx_mouse();
}

void FB_lines(XPoint *points, int n, int mode) {
  int i;
  FB_hidex_mouse();
  if(n>1) {
    for(i=1;i<n;i++)
      FB_doline(points[i-1].x,points[i-1].y,points[i].x,points[i].y);
  }
  // FB_showx_mouse();
}
void FB_points(XPoint *points, int n, int mode) {
  int i;
  FB_hidex_mouse();
  if(n) {
    for(i=0;i<n;i++) FB_PutPixel(points[i].x,points[i].y,screen.fcolor);
  }
  // FB_showx_mouse();
}


void FB_box(int x1,int y1,int x2,int y2) {
  int w,h;
  FB_hidex_mouse();
  /* Swap x1, x2 if required */
  if(x1>x2) {int xtmp=x1;x1=x2;x2=xtmp;}
  /* Swap y1, y2 if required */
  if(y1>y2) {int ytmp=y1;y1=y2;y2=ytmp;}
  w=x2-x1;
  h=y2-y1;
  if(x1==x2) {
    if(y1==y2) FB_PutPixel(x1,y1,screen.fcolor);
    else {
      DrawVerticalLine(x1,y1,h,screen.fcolor);
    } 
  } else {
    if(y1==y2) DrawHorizontalLine(x1,y1,w,screen.fcolor);
    else {
      DrawHorizontalLine(x1,y1,w,screen.fcolor);
      DrawHorizontalLine(x1,y2,w,screen.fcolor);
      if(h>1) {
        DrawVerticalLine(x1,y1+1,h-1,screen.fcolor);
        DrawVerticalLine(x2,y1+1,h-1,screen.fcolor);
      }
    }
  }
  // FB_showx_mouse();
}


void FillBox (int x, int y, int w, int h, unsigned short color) {
  int i;
 // printf("fillbox w=%d\n",w);
  if(w<=0 || h<=0) return;
  for (i=y; i<y+h; i++) {
    if(screen.fill_pat && screen.fill_style==FillStippled) {
      if(screen.graphmode<=1) DrawHorizontalLinePattBg(x,i,w,color,screen.bcolor,*((unsigned short *)(screen.fill_pat+(i&0xf))));
      else DrawHorizontalLinePatt(x,i,w,color,*((unsigned short *)(screen.fill_pat+(i&0xf))));
    } else DrawHorizontalLine(x, i, w, color);
  }  
}

void FB_pbox(int x1, int y1, int x2, int y2) {
  /* Swap x1, x2 if required */
  if(x1>x2) {int xtmp=x1;x1=x2;x2=xtmp;}
  /* Swap y1, y2 if required */
  if(y1>y2) {int ytmp=y1;y1=y2;y2=ytmp;}
  FB_hidex_mouse();
  FillBox(x1,y1,x2-x1,y2-y1,screen.fcolor);
  //FB_showx_mouse();
}


/* Clear whole screen */
void FB_Clear(G_CONTEXT *screen) {
  unsigned short *ptr=(unsigned short *)screen->pixels;
  int n=screen->size>>1;
  int i;
  FB_hidex_mouse();
  for(i=0; i<n; i++) *ptr++=screen->bcolor;
  //FB_showx_mouse();
}

/* Wird von Terminal Emulation verwendet*/
void Fb_Clear2(int y, int h, unsigned short color) {
  if (y<0|| y+h>screen.height) return;
  unsigned short *ptr  = (unsigned short*)(screen.pixels+y*screen.scanline);
  unsigned short *endp = ptr + h*screen.scanline/sizeof(short);
  while (ptr < endp) *ptr++=color;
}


void Fb_inverse(int x, int y,int w,int h){
  if(w<=0||h<=0) return;

// printf("%d %d %d %d : %d %d %d %d \r",screen.clip_x,screen.clip_y,screen.clip_w,screen.clip_h,x,y,w,h);

  if(x<screen.clip_x||y<screen.clip_y|| x+w>screen.clip_x+screen.clip_w|| y+h>screen.clip_y+screen.clip_h) return;

  register unsigned short *ptr  = (unsigned short*)(screen.pixels+y*screen.scanline);
  ptr+=x;
  register unsigned short *endp  = ptr+h*screen.width;
  register int i;
  while (ptr<endp) {
    for(i=0;i<w;i++) {*ptr=~*ptr;ptr++;}
    ptr+=screen.width-w;
  }
}
unsigned short int mean_color(unsigned short int a, unsigned short int b) {
  return(
  (((((a>>11)&0x1f)+((b>>11)&0x1f))>>2)<<11)|
  (((((a>>5)&0x3f)+((b>>5)&0x3f))>>2)<<5)|
  (((a&0x1f)+(b&0x1f))>>2));
}
unsigned short int mix_color(unsigned short int a, unsigned short int b, unsigned char alpha) {
/*...*/
  if(alpha==255) return a;
  if(alpha==0) return b;
  unsigned long  R, G, B;
  R = ((b & R_MASK) + (((a & R_MASK) - (b & R_MASK)) * alpha >> 8)) & R_MASK;
  G = ((b & G_MASK) + (((a & G_MASK) - (b & G_MASK)) * alpha >> 8)) & G_MASK;
  B = ((b & B_MASK) + (((a & B_MASK) - (b & B_MASK)) * alpha >> 8)) & B_MASK;
  return(R | G | B);
}

void FB_copyarea(int x,int y,int w, int h, int tx,int ty) {
  if(x<0||y<0||w<=0||h<=0|| x+w>screen.width|| y+h>screen.height||
  tx<0||ty<0||tx+w>screen.width|| ty+h>screen.height) return;
  register unsigned short *ptr1  = (unsigned short*)(screen.pixels+y*screen.scanline);
  register unsigned short *ptr2  = (unsigned short*)(screen.pixels+ty*screen.scanline);
  ptr1+=x;
  ptr2+=tx;

  register int i;
  if(y>ty) {
    for(i=0;i<h;i++) {
       memmove(ptr2,ptr1,w*sizeof(short));
       ptr1+=screen.width;
       ptr2+=screen.width;
    }
  } else {
    for(i=h-1;i>=0;i--) {
      memmove(&ptr2[i*screen.width],&ptr1[i*screen.width],w*sizeof(short));
    }
  }
}
void FB_CopyArea(int x,int y,int w, int h, int tx,int ty) {
  FB_hidex_mouse();
  FB_copyarea(x,y,w,h,tx,ty);
  //FB_showx_mouse();
}




/* Draw 16x16 Sprite */

unsigned short spritebg[16*16];  /*store the background*/

void FB_draw_sprite(unsigned short *sprite, unsigned char *alpha,int x,int y) {
  register unsigned short *ptr=(unsigned short*)(screen.pixels);
  int i,j;
  for(j=0;j<16;j++) {
    if(y+j>=screen.clip_y && y+j<screen.clip_y+screen.clip_h) {
      for(i=0;i<16;i++) {
        if(x+i>=screen.clip_x && x+i<screen.clip_x+screen.clip_w) {
          spritebg[16*j+i]=ptr[(y+j)*(screen.scanline>>1)+x+i];
	  ptr[(y+j)*(screen.scanline>>1)+x+i]=mix_color(sprite[16*j+i],
	    ptr[(y+j)*(screen.scanline>>1)+x+i],alpha[16*j+i]);
        }
      }
    }
  }
}
void FB_hide_sprite(int x,int y) {
  register unsigned short *ptr=(unsigned short*)(screen.pixels);
  int i,j;
  for(j=0;j<16;j++) {
    if(y+j>=screen.clip_y && y+j<screen.clip_y+screen.clip_h) {
      for(i=0;i<16;i++) {
        if(x+i>=screen.clip_x && x+i<screen.clip_x+screen.clip_w)
          ptr[(y+j)*(screen.scanline>>1)+x+i]=spritebg[16*j+i];
      }
    }
  }
}

int FB_get_color(unsigned char r, unsigned char g, unsigned char b) {
  return((((r>>3)&0x1f)<<11)|(((g>>2)&0x3f)<<5)|(((b>>3)&0x1f)));
}


#ifdef ANDROID
unsigned char *fontlist57[10]={
(unsigned char *)asciiTable,(unsigned char *)asciiTable,
(unsigned char *)asciiTable,(unsigned char *)asciiTable,
(unsigned char *)asciiTable,(unsigned char *)asciiTable,
(unsigned char *)asciiTable,(unsigned char *)asciiTable,
(unsigned char *)asciiTable,(unsigned char *)asciiTable};

unsigned char *fontlist816[10]={
(unsigned char *)spat_a816,(unsigned char *)ibm_like816,
(unsigned char *)ext_font816,(unsigned char *)spat_a816,
(unsigned char *)spat_a816,(unsigned char *)spat_a816,
(unsigned char *)spat_a816,(unsigned char *)spat_a816,
(unsigned char *)spat_a816,(unsigned char *)spat_a816};
#else
unsigned char *fontlist816[1]={(unsigned char *)spat_a816};
unsigned char *fontlist57[1] ={(unsigned char *)asciiTable};
#endif



/*Draw a 1-Bit bitmap in fgcolor and bgcolor*/
void Fb_BlitBitmap(int x, int y,unsigned int w, unsigned int h,unsigned short aColor, 
                   unsigned short aBackColor, unsigned short graphmode, const unsigned char *bdata) {
  if (x<0||y<0||w==0||h==0||x>screen.width-w || y>screen.height-h || bdata==NULL) return;
  register unsigned short *ptr  = (unsigned short*)(screen.pixels+y*screen.scanline);
  ptr+=x;
  register unsigned short *endp  = ptr+h*screen.width;
  register const unsigned char *dptr=bdata;
  unsigned char d=0;
  while(ptr<endp) {
  
    for(int i=0;i<w;i++) {
      if((i%8)==0) d=*dptr++;
      if(graphmode==GRAPHMD_TRANS) {    /* Transparent */
        if(d & 1) *ptr= aColor;  ptr++; d >>= 1; 
      } else if(graphmode==GRAPHMD_XOR) {    /* EOR */
        if(d & 1) *ptr^=aColor; else *ptr^=aBackColor; ptr++; d >>= 1;   
      } else {
        if(d & 1) *ptr++=aColor; else *ptr++ = aBackColor; d >>= 1;    
      }
    }
    ptr+=screen.width-w;
  }
}



void Fb_BlitCharacter57_raw(int x, int y, unsigned short aColor, unsigned short aBackColor,int flags, const unsigned char *chrdata){
  unsigned char data0,data1,data2,data3,data4;

  if (x<0||y<0|| x>screen.width-CharWidth57 || y>screen.height-CharHeight57 || chrdata==NULL) return;
//  if ((character < Fontmin) || (character > Fontmax)) character = '.';

  const unsigned char *aptr = chrdata;
  data0 = *aptr++;
  data1 = *aptr++;
  data2 = *aptr++;
  data3 = *aptr++;
  data4 = *aptr;

  register unsigned short *ptr  = (unsigned short*)(screen.pixels+y*screen.scanline);
  ptr+=x;
  register unsigned short *endp  = ptr+CharHeight57*screen.width;

  if(flags&FL_REVERSE) { /* reverse */
    unsigned short t=aBackColor;
    aBackColor=aColor;
    aColor=t;
  }
#if 0
  /* Sieht nicht gut aus (unlesbar) deshalb ignorieren*/
  if(flags&FL_BOLD) {/* bold */
    data0|=data1;
    data1|=data2;
    data2|=data3;
    data3|=data4;
  }
#endif  
  if(flags&FL_UNDERLINE || flags&FL_DBLUNDERLINE) {/* underline */
    data0|=0x80;
    data1|=0x80;
    data2|=0x80;
    data3|=0x80;
    data4|=0x80;
  }
  if(flags&FL_HIDDEN) {
    data0=data1=data2=data3=data4=0;
  }
  if(flags&FL_TRANSPARENT) {
    while (ptr<endp) {
      if (data0 & 1) *ptr= aColor;  ptr++; data0 >>= 1;    
      if (data1 & 1) *ptr= aColor;  ptr++; data1 >>= 1;
      if (data2 & 1) *ptr= aColor;  ptr++; data2 >>= 1;
      if (data3 & 1) *ptr= aColor;  ptr++; data3 >>= 1;
      if (data4 & 1) *ptr= aColor;  ptr++; data4 >>= 1;
      ptr+=screen.width-CharWidth57;
    }
  } else {
    while (ptr<endp) {
      if (data0 & 1) *ptr++ = aColor; else *ptr++ = aBackColor; data0 >>= 1;    
      if (data1 & 1) *ptr++ = aColor; else *ptr++ = aBackColor; data1 >>= 1;
      if (data2 & 1) *ptr++ = aColor; else *ptr++ = aBackColor; data2 >>= 1;
      if (data3 & 1) *ptr++ = aColor; else *ptr++ = aBackColor; data3 >>= 1;
      if (data4 & 1) *ptr++ = aColor; else *ptr++ = aBackColor; data4 >>= 1;
      ptr+=screen.width-CharWidth57;
    }
  }
}
void Fb_BlitCharacter57(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned char character, int flags,int fontnr){
  const unsigned char *aptr = &((fontlist57[fontnr])[character*5]);
  Fb_BlitCharacter57_raw(x,y,aColor,aBackColor,flags, aptr);
}

void Fb_BlitCharacter816_raw(int x, int y, unsigned short aColor, unsigned short aBackColor, int flags, const unsigned char *chrdata) {
  char charackter[CharHeight816];
  int i,d;
  if (x<0||y<0|| x>screen.width-CharWidth816 || y>screen.height-CharHeight816 || chrdata==NULL) return;

  memcpy(charackter,chrdata,CharHeight816);

  register unsigned short *ptr  = (unsigned short*)(screen.pixels+y*screen.scanline);

  if(flags&FL_REVERSE) { /* reverse */
    unsigned short t=aBackColor;
    aBackColor=aColor;
    aColor=t;
  }  
  if(flags&FL_FRAMED) {/* Framed */
    charackter[0]=0xff;
    for(i=1;i<16;i++) charackter[i]|=1;
  }
  if(flags&FL_UNDERLINE) {/* underline */
    charackter[15]=0xff;
    charackter[14]=0xff;
  } else if(flags&FL_DBLUNDERLINE) {/* double underline */
    charackter[15]=0xff;
    charackter[13]=0xff;
  }
  if(flags&FL_DIM) {
    for(i=0;i<16;i++) {
      charackter[i++]&=0xaa;
      charackter[i]  &=0x55;
    }
  } else if(flags&FL_CONCREAL) {
    for(i=0;i<16;i++) {
      charackter[i++]|=0xaa;
      charackter[i]  |=0x55;
    }
  }  
  ptr+=x;
  if(flags&FL_TRANSPARENT) {/* transparent */
    for(i=0;i<16;i++) {
      d=charackter[i];
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      ptr+=screen.width-CharWidth816;
    }
  } else {
    for(i=0;i<16;i++) {
      d=charackter[i];
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      ptr+=screen.width-CharWidth816;
    }
  }
  if(flags&FL_BOLD) {/* bold */
    ptr  = (unsigned short*)(screen.pixels+y*screen.scanline);
    ptr+=x+1;
    for(i=0;i<16;i++) {
      d=charackter[i];
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      ptr+=screen.width-CharWidth816;
    }
  }
}




void Fb_BlitCharacter816(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned char character, int flags,int fontnr){
  const unsigned char *aptr = &((fontlist816[fontnr])[character*CharHeight816]);
  Fb_BlitCharacter816_raw(x,y,aColor,aBackColor,flags, aptr);
}

void Fb_BlitCharacter1632(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned char character, int flags,int fontnr){
  const unsigned char *aptr = &((fontlist816[fontnr])[character*CharHeight816]);
  Fb_BlitCharacter1632_raw(x,y,aColor,aBackColor,flags,aptr);
}
void Fb_BlitCharacter1632_raw(int x, int y, unsigned short aColor, unsigned short aBackColor, int flags,const unsigned char *chrdata){
  char charackter[CharHeight816];
  int i,d;
  if (x<0||y<0|| x>screen.width-CharWidth816 || y>screen.height-CharHeight816 || chrdata==NULL) return;
  memcpy(charackter,chrdata,CharHeight816);
  register unsigned short *ptr  = (unsigned short*)(screen.pixels+y*screen.scanline);

  if(flags&FL_REVERSE) { /* reverse */
    unsigned short t=aBackColor;
    aBackColor=aColor;
    aColor=t;
  }  
  if(flags&FL_FRAMED) {/* Framed */
    charackter[0]=0xff;
    for(i=1;i<16;i++) charackter[i]|=1;
  }
  if(flags&FL_UNDERLINE) {/* underline */
    charackter[15]=0xff;
    charackter[14]=0xff;
  } else if(flags&FL_DBLUNDERLINE) {/* double underline */
    charackter[15]=0xff;
    charackter[13]=0xff;
  }
  if(flags&FL_DIM) {
    for(i=0;i<16;i++) {
      charackter[i++]&=0xaa;
      charackter[i]  &=0x55;
    }
  } else if(flags&FL_CONCREAL) {
    for(i=0;i<16;i++) {
      charackter[i++]|=0xaa;
      charackter[i]  |=0x55;
    }
  }  
  ptr+=x;
  if(flags&FL_TRANSPARENT) {/* transparent */
    for(i=0;i<16;i++) {
      d=charackter[i];
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      ptr+=screen.width-CharWidth1632;
      d=charackter[i];
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      if (d&0x80) *ptr= aColor; ptr++; 
      if (d&0x80) *ptr= aColor; ptr++; d<<=1;
      ptr+=screen.width-CharWidth1632;
    }
  } else {
    for(i=0;i<16;i++) {
      d=charackter[i];
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      ptr+=screen.width-CharWidth1632;
      d=charackter[i];
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; 
      if (d&0x80) *ptr++ = aColor; else *ptr++ = aBackColor; d<<=1;
      ptr+=screen.width-CharWidth1632;
    }
  }
  if(flags&FL_BOLD) {/* bold */
    ptr  = (unsigned short*)(screen.pixels+y*screen.scanline);
    ptr+=x+1;
    for(i=0;i<16;i++) {
      d=charackter[i];
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      ptr+=screen.width-CharWidth1632;
      d=charackter[i];
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      if(d&0x80) *ptr= aColor; ptr++; 
      if(d&0x80) *ptr= aColor; ptr++; d<<=1;
      ptr+=screen.width-CharWidth1632;
    }
  }
}

/*Langsame Routine zum scalieren des 8x16 Fonts auf beliebige Groessen*/
void Fb_BlitCharacter816_scale(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned char character, 
                               int flags,int fontnr,int charwidth,int charheight ) {
  const unsigned char *aptr = &((fontlist816[fontnr])[character*CharHeight816]);
  Fb_BlitCharacter816_scale_raw(x,y,aColor,aBackColor,flags,charwidth,charheight, aptr);
}
void Fb_BlitCharacter816_scale_raw(int x, int y, unsigned short aColor, unsigned short aBackColor, 
                               int flags,int charwidth,int charheight, const unsigned char *chrdata) {
  char charackter[CharHeight816];
  int i,d,j,dd;
  if (x<0||y<0|| x>screen.width-charwidth || y>screen.height-charheight || chrdata==NULL) return;
  memcpy(charackter,chrdata,CharHeight816);
  register unsigned short *ptr  = (unsigned short*)(screen.pixels+y*screen.scanline);

  if(flags&FL_REVERSE) { /* reverse */
    unsigned short t=aBackColor;
    aBackColor=aColor;
    aColor=t;
  }  
  if(flags&FL_FRAMED) {/* Framed */
    charackter[0]=0xff;
    for(i=1;i<16;i++) charackter[i]|=1;
  }
  if(flags&FL_UNDERLINE) {/* underline */
    charackter[15]=0xff;
    charackter[14]=0xff;
  } else if(flags&FL_DBLUNDERLINE) {/* double underline */
    charackter[15]=0xff;
    charackter[13]=0xff;
  }
  if(flags&FL_DIM) {
    for(i=0;i<16;i++) {
      charackter[i++]&=0xaa;
      charackter[i]  &=0x55;
    }
  } else if(flags&FL_CONCREAL) {
    for(i=0;i<16;i++) {
      charackter[i++]|=0xaa;
      charackter[i]  |=0x55;
    }
  }  
  ptr+=x;
  if(flags&FL_TRANSPARENT) {/* transparent */
    for(i=0;i<charheight;i++) {
      d=charackter[i*CharHeight816/charheight];
      for(j=0;j<charwidth;j++) {
        dd=j*CharWidth816/charwidth;
        if((d<<dd)&0x80) *ptr= aColor; 
	ptr++; 
      }
      ptr+=screen.width-charwidth;
    }
  } else {
    for(i=0;i<charheight;i++) {
      d=charackter[i*CharHeight816/charheight];
      for(j=0;j<charwidth;j++) {
        dd=j*CharWidth816/charwidth;
        if((d<<dd)&0x80) *ptr++= aColor; 
	else *ptr++ = aBackColor;
      }
      ptr+=screen.width-charwidth;
    }
  }
  if(flags&FL_BOLD) {/* bold */
    ptr  = (unsigned short*)(screen.pixels+y*screen.scanline);
    ptr+=x+charheight/CharHeight816;
    for(i=0;i<charheight;i++) {
      d=charackter[i*CharHeight816/charheight];
      for(j=0;j<charwidth;j++) {
        dd=j*CharWidth816/charwidth;
        if((d<<dd)&0x80) *ptr= aColor; 
	ptr++; 
      }
      ptr+=screen.width-charwidth;
    }
  }
}


extern int ltextpflg;

void FB_DrawString(int x, int y, const char *t,int len,unsigned short chw, unsigned short chh) {
  if(len>0) {
    char buf[len+1];
    FB_hidex_mouse();
    utf8(0,(unsigned short *)&buf);   /*Startbedingungen herstellen*/
    memcpy(buf,t,len);
    buf[len]=0;
    if(ltextpflg==1 || (chh==16 && chw==8))  Fb_BlitText816(x,y,screen.fcolor, screen.bcolor,buf);
    else if(chh==32 && chw==16) Fb_BlitText1632(x,y,screen.fcolor, screen.bcolor,buf);
    else if(chh<8 || chw<8) Fb_BlitText57(x,y,screen.fcolor,screen.bcolor,buf);
    else Fb_BlitText816_scale(x,y,screen.fcolor, screen.bcolor,buf,chw,chh);
    //FB_showx_mouse();
  }
}



void FB_get_geometry(int *x, int *y, unsigned int *w, unsigned int *h, int *b, unsigned int *d) {
  *b=*x=*y=0;
  *w=screen.width;
  *h=screen.height;
  *d=screen.bpp;
}

static STANDARDBITMAP framebuffertostdbm(unsigned short *ptr1,int w,int h,int line,int usealpha, unsigned short bcolor) {
  STANDARDBITMAP ret;
  ret.w=w;
  ret.h=h;
  ret.image=malloc(ret.w*ret.h*4);
  int i,j;
  unsigned char r,g,b,a;
  unsigned short p;
  unsigned char *buf=ret.image;
  for(i=0;i<h;i++) {
    for(j=0;j<w;j++) {
      p=ptr1[j+i*line];
      if(usealpha && bcolor==p) {r=b=g=a=0;}
      else {
        a=255;
	r=((p>>11)& 0x1f)<<3;
        g=((p>>5)& 0x3f)<<2;
        b=(p & 0x1f)<<3;
      }
      *buf++=r;
      *buf++=g;
      *buf++=b;
      *buf++=a;
    }    
  }
  return(ret);
}


/* This produces data which conforms to a WINDOWS .bmp file */

unsigned char *FB_get_image(int x, int y, int w,int h, int *len,int usealpha,int bcolor) {
  if(x<0) x=0;
  if(y<0) y=0;
  if(w<0 || h<0) {if(len) *len=0; return(NULL);}
  if(x+w>screen.width) w=screen.width-x;
  if(y+h>screen.height) h=screen.height-y;
  unsigned short *ptr1  = (unsigned short*)(screen.pixels+y*screen.scanline);
  ptr1+=x;
  
  STANDARDBITMAP bitmap=framebuffertostdbm(ptr1,w,h,screen.scanline/2,usealpha,(unsigned short)bcolor);
  unsigned char *buf=stdbmtobmp(bitmap,len);
  free(bitmap.image);
  return(buf);
}

/*Put standard Bitmap to framebuffer 
  bm -- bitmap
  x,y -- destination coordinates on framebuffer
  sx,sy -- source corrdinates in bm
  sw,sw -- width and height of the portion.
  
  */

static void stdbmtoframebuffer(STANDARDBITMAP bm,int x,int y,int sx,int sy,unsigned int sw,unsigned int sh) {
  unsigned short *ptr=(unsigned short *)(screen.pixels+y*screen.scanline);
  ptr+=x;
  if(sw<=0 || sh<=0) return;
  if(sx<0) sx=0;
  if(sy<0) sy=0;
  sx=min(bm.w-1,sx);
  sy=min(bm.h-1,sy);
  sw=min(bm.w-sx,sw);
  sh=min(bm.h-sy,sh);
  int i,j;
  unsigned char r,g,b,a;
  unsigned char *buf;
  unsigned short *ptr2;
  for(j=0;j<sh;j++) {
    buf=bm.image+(sx+(sy+j)*bm.w)*4;
    ptr2=ptr+j*screen.scanline/2;
    for(i=0;i<sw;i++) {
      r=*buf++;
      g=*buf++;
      b=*buf++;
      a=*buf++;
      *ptr2=mix_color(((((r>>3)&0x1f)<<11)|(((g>>2)&0x3f)<<5)|((b>>3)&0x1f)),*ptr2,a);
      ptr2++;
    }
  }
}

void FB_put_image(unsigned char *data,int x, int y) {
  if(x>screen.width||y>screen.height) return;
  if(x<screen.clip_x||y<screen.clip_y) return;
  STANDARDBITMAP bm=bmp2stdbm(data);
  stdbmtoframebuffer(bm,x,y,0,0,999999,999999);
  free(bm.image);
}
void FB_put_image_scale(const unsigned char *data,int x, int y,double scale,int sx,int sy,unsigned int sw, unsigned int sh) {
  if(x>screen.width||y>screen.height) return;
  if(x<screen.clip_x||y<screen.clip_y) return;
  STANDARDBITMAP bm=bmp2stdbm(data);
  if(scale!=1 && scale>0) {
    STANDARDBITMAP bm2;
    bm2.w=(int)((double)bm.w*scale);
    bm2.h=(int)((double)bm.h*scale);
    bm2.image=malloc(4*bm2.w*bm2.h);
    bitmap_scale(bm.image,32,bm.w,bm.h,bm2.image,bm2.w,bm2.h);
    free(bm.image);
    bm=bm2;
  }
  stdbmtoframebuffer(bm,x,y,sx,sy,sw,sh);
  free(bm.image);
}



void FB_bmp2pixel(const unsigned char *s,unsigned short *d,int w, int h, unsigned short color) {
  int i,j;
  unsigned char a;
  for(j=0;j<h;j++) {
  a=*s++;
  for(i=0;i<8;i++) {
    if((a>>i)&1) *d=color;
    else *d=WHITE;
    d++;
    #if DEBUG
    if((a>>i)&1) printf("##");
    else printf("..");
#endif
  }
  a=*s++;
  for(i=0;i<8;i++) {
     if((a>>i)&1) *d=color;
    else *d=WHITE;
    d++;
#if DEBUG
    if((a>>i)&1) printf("##");
    else printf("..");
#endif
  }
#if DEBUG
  printf("\n");
#endif
  }
}
void FB_bmp2mask(const unsigned char *s,unsigned char *d,int w, int h) {
  int i,j;
  unsigned char a;
  for(j=0;j<h;j++) {
  a=*s++;
  for(i=0;i<8;i++) {
    if((a>>i)&1) *d=255;
    else *d=0;
    d++;
#if DEBUG
    if((a>>i)&1) printf("##");
    else printf("..");
#endif
  }
  a=*s++;
  for(i=0;i<8;i++) {
     if((a>>i)&1) *d=255;
    else *d=0;
    d++;
#if DEBUG
    if((a>>i)&1) printf("##");
    else printf("..");
#endif
  }
#if DEBUG
  printf("\n");
#endif
  }
}

/* Zeichne vollen Kreis: Mittelpunkt (x,y), Radius r [pixel]*/



static void DrawCircle(int x, int y, int r, unsigned short color) {
  int row, col, px, py;
  long int sum;
  if(r==0) {
    FB_PutPixel(x,y,color);
    return;
  }

  py = r<<1;
  px = 0;
  sum = 0;
  while (px <= py) {
    if (!(px & 1)) {
      col = x + (px>>1);
      row = y + (py>>1);FB_PutPixel(col,row,color);
      row = y - (py>>1);FB_PutPixel(col,row,color);
      col = x - (px>>1);FB_PutPixel(col,row,color);
      row = y + (py>>1);FB_PutPixel(col,row,color);
      col = x + (py>>1);
      row = y + (px>>1);FB_PutPixel(col,row,color);
      row = y - (px>>1);FB_PutPixel(col,row,color);
      col = x - (py>>1);FB_PutPixel(col,row,color);
      row = y + (px>>1);FB_PutPixel(col,row,color);
    }
    sum+=px++;
    sum += px;
    if (sum >= 0) {
	sum-=py--;
	sum-=py;
    }
  }
}

#if 0
/*  Routine mit Problemen / overflow in int, need to use 64bit long long*/

static void DrawEllipse(int x0, int y0, int rx,int ry, unsigned short color) {
  long long hh=ry*ry;
  long long ww=rx*rx;
  long long hhww=hh*ww;
  int x00=rx;
  int dx=0;
  int y,x1,oldx=0,oldy=0;

  /* do the horizontal diameter */
  FB_PutPixel(x0-rx,y0,color);
  FB_PutPixel(x0+rx,y0,color);

  // now do both halves at the same time, away from the diameter
  for(y=1;y<=ry;y++) {
    x1=x00-(dx-1);          /* try slopes of dx - 1 or more  */
    for ( ; x1 > 0; x1--) {
        if(x1*x1*hh+y*y*ww<=hhww) break;
    }
    dx=x00-x1;  // current approximation of the slope
    x00=x1;
    while(oldx-x00>0) {
      FB_PutPixel(x0-oldx,y0+oldy,color);
      FB_PutPixel(x0+oldx,y0+oldy,color);
      FB_PutPixel(x0-oldx,y0-oldy,color);
      FB_PutPixel(x0+oldx,y0-oldy,color);
      oldx--;
    }
    FB_PutPixel(x0-x00,y0+y,color);
    FB_PutPixel(x0-x00,y0-y,color);
    FB_PutPixel(x0+x00,y0+y,color);
    FB_PutPixel(x0+x00,y0-y,color);
    oldx=x00;
    oldy=y;
  }
}
#else

/* Besenhams algorithm  for drawing ellipses.... 
   I think this is the better one.*/

static void DrawEllipse(int x0, int y0, int rx,int ry, unsigned short color) {
   int x1=x0+rx;
   int y1=y0+ry;
   x0-=rx;
   y0-=ry;
   /*  Durchmesser berechnen */
   long long a=abs(rx<<1);
   long long b=abs(ry<<1), b1 = b&1; /* Ungerade pixel... */
   /*  Fehler-Vektor */
   long long dx=4*(1-a)*b*b;
   long long dy=4*(b1+1)*a*a; 
   /* Anfangsfehler, erster Schritt */
   long long err=dx+dy+b1*a*a, e2;

   if(x0>x1) {x0=x1;x1+=a;} /* if called with swapped points */
   if(y0>y1) y0=y1; /* .. exchange them */
   y0+=(b+1)/2; /* starting pixel */
   y1=y0-b1;    /* starting pixel */
   a*=(a<<3); 
   b1=8*b*b;

   do {
       FB_PutPixel(x1, y0,color); /*   1. Quadrant */
       FB_PutPixel(x0, y0,color); /*   2. Quadrant */
       FB_PutPixel(x0, y1,color); /*   3. Quadrant */
       FB_PutPixel(x1, y1,color); /*   4. Quadrant */
       e2=2*err;
       if(e2<=dy) {y0++;y1--;err+=dy+=a;}  /* y step */ 
       if(e2>=dx || 2*err>dy) {x0++;x1--;err+=dx+=b1; } /* x step */
   } while(x0<=x1);
   
   while(y0-y1<b) {  /* too early stop of flat ellipses a=1 */
       FB_PutPixel(x0-1, y0  ,color); /* -> finish tip of ellipse */
       FB_PutPixel(x1+1, y0++,color); 
       FB_PutPixel(x0-1, y1  ,color);
       FB_PutPixel(x1+1, y1--,color); 
   }
}
#endif

#include <math.h>

void FB_Arc(int x1, int y1, int w, int h, int a1, int da) {
  // printf("ARC:%d:%d:%d:%d->%d:%d\n",x1,y1,w,h,a1,da);


  int r1=w/2;
  int r2=h/2;
  int x=x1+w/2;
  int y=y1+h/2;
  int a2=a1+da;


  if(r1==0 && r2==0) {
    FB_PutPixel(x,y,screen.fcolor);
    return;
  } else if(da==64*360) {
    if(r1==r2) DrawCircle(x,y,r1,screen.fcolor);
    else DrawEllipse(x,y,r1,r2,screen.fcolor); 
    return;
  } else {
    int i;
    int ox=x;
    int oy=y;
    int dx,dy;

    dx=(double)r1*cos((double)a1/180*3.14159/64);
    dy=-(double)r2*sin((double)a1/180*3.14159/64);
    // FB_line(x,y,x+dx,y+dy);
    for(i=a1/64+1;i<=a2/64;i++) {
      ox=dx;oy=dy;
      dx=(double)r1*cos((double)i/180*3.14159);
      dy=-(double)r2*sin((double)i/180*3.14159);
      FB_line(x+ox,y+oy,x+dx,y+dy);
    }
   // FB_line(x,y,x+dx,y+dy);
  }
}


static void FillEllipse(int x0, int y0, int rx,int ry, unsigned short color) {
  long long hh=ry*ry;
  long long ww=rx*rx;
  long long hhww=hh*ww;
  int x00 = rx;
  int dx = 0;
  int y,x1;

  fillLine(x0-rx, x0+rx, y0, color);/* do the horizontal diameter */

  // now do both halves at the same time, away from the diameter
  for(y=1;y<=ry;y++) {
    x1=x00-(dx-1);  // try slopes of dx - 1 or more
    for ( ; x1>0; x1--)
        if(x1*x1*hh+y*y*ww<=hhww) break;
    dx=x00-x1;  // current approximation of the slope
    x00=x1;
    fillLine(x0-x00, x0+x00, y0+y, color);
    fillLine(x0-x00, x0+x00, y0-y, color);
  }
}


void FillCircle(int x, int y, int r, unsigned short color) {
  int row, col_start, col_end, px, py;
  long int sum;
  
  py = r<<1;
  px = 0;
  sum = 0;
  while (px <= py) {
    col_start=x-(px>>1);
    col_end=x+(px>>1);
    row =y+(py>>1);
    fillLine(col_start, col_end, row, color);
    col_start =x - (py>>1);
    col_end =x + (py>>1);
    row =y+(px>>1);
    fillLine(col_start, col_end, row, color);

    col_start = x - (px>>1);
    col_end = x + (px>>1);
    row =y - (py>>1);
    fillLine(col_start, col_end, row, color);

    col_start = x - (py>>1);
    col_end = x + (py>>1);
    row =y - (px>>1);
    fillLine(col_start, col_end, row, color);

    sum +=px++;
    sum += px;
    if (sum >= 0) {
       sum-=py--;
       sum -=py;
    }
  }
}
void FB_pArc(int x1, int y1, int w, int h, int a1, int da) {
  int r1=w/2;
  int r2=h/2;
  int x=x1+w/2;
  int y=y1+h/2;
  int a2=a1+da;
/* muss man mit polygonen machen.... */
  if(r1==0 && r2==0) {
    FB_PutPixel(x,y,screen.fcolor);
    return;
  } else if(da==360*64) {
    if(r1==r2) FillCircle(x,y,r1,screen.fcolor);
    else FillEllipse(x,y,r1,r2,screen.fcolor);
    return;
  } else {
    int point[((a2-a1)/64+2)*2];
    int i;
    int count=0;
    // printf("Draw-Arc: a1=%d,a2=%d\n",a1,a2);
    point[count++]=x;
    point[count++]=y;
    point[count++]=x+(int)((double)r1*cos((double)a1/180*3.14159/64));
    point[count++]=y+(int)(-(double)r2*sin((double)a1/180*3.14159/64));
    for(i=a1/64+1;i<=a2/64;i++) {
      point[count++]=x+(int)((double)r1*cos((double)i/180*3.14159));
      point[count++]=y+(int)(-(double)r2*sin((double)i/180*3.14159));
    }
    fill2Poly(screen.fcolor,point,count/2);
  }
}

/*
  +---------------------------------------------------------+
  |                                                         |
  |                                                         |
  | fil2Poly() = fills a polygon in specified color by      |
  |              filling in boundaries resulting from       |
  |              connecting specified points in the         |
  |              order given and then connecting last       |
  |              point to first.  Uses an array to          |
  |              store coordinates.                         |
  |                                                         |
  +---------------------------------------------------------+
*/
static int sort_function (const long int *a, const long int *b) {
	if (*a < *b)  return(-1);
	// else if (*a == *b) return(0);
	else if (*a > *b)  return(1);
	return(0);
}


void fill2Poly(unsigned short color,int *point, int num) {
  #define sign(x) ((x) > 0 ? 1:  ((x) == 0 ? 0:  (-1)))

  int dx, dy, dxabs, dyabs, i=0, index=0, j, k, px, py, sdx, sdy, x, y,
      xs,xe,ys,ye,toggle=0, old_sdy, sy0;
  long int coord[4000];

// backlog("fill2poly");sleep(1);

  if (num<3) return;
  i=2*num+1;
  px = point[0];
  py = point[1];
  if (point[1] == point[3]) {
    coord[index++] = px | (long)py << 16;
  }
  for (j=0; j<=i-3; j+=2) {
    xs = point[j];
    ys = point[j+1];
    if ((j == (i-3)) || (j == (i-4))) {
    	    xe = point[0];
    	    ye = point[1];
    } else {
    	    xe = point[j+2];
    	    ye = point[j+3];
    }
    dx = xe - xs;
    dy = ye - ys;
    sdx = sign(dx);
    sdy = sign(dy);
    if (j==0) {
      old_sdy = sdy;
      sy0 = sdy;
    }
    dxabs = abs(dx);
    dyabs = abs(dy);
    x = 0;
    y = 0;
    if (dxabs >= dyabs) {
      for (k=0; k<dxabs; k++) {
	y += dyabs;
	if (y>=dxabs) {
	  y -= dxabs;
	  py += sdy;
	  if (old_sdy != sdy) {
	    old_sdy = sdy;
	    index--;
	  }
	  coord[index++] = (px + sdx) | (long)py << 16;
	}
	px += sdx;
	FB_PutPixel(px,py,color);
      }
    } else {
      for (k=0; k<dyabs; k++) {
	x += dxabs;
	if (x>=dyabs) {
	  x -= dyabs;
	  px += sdx;
	}
	py += sdy;
	if (old_sdy != sdy) {
	  old_sdy = sdy;
	  if (sdy != 0) index--;
        }
	FB_PutPixel(px,py,color);
	coord[index] = px | (long)py << 16;
	index++;
      }
    }
  }
  if(sy0+sdy==0) index--;
  qsort(coord,index,sizeof(coord[0]),(int(*)
		(const void *, const void *))sort_function);
  for (i=0; i<index; i++) {
    xs=min(screen.width-1,(max(0,(int)((signed short)(coord[i])))));
    xe=min(screen.width-1,(max(0,(int)((signed short)(coord[i + 1])))));
    ys=min(screen.height-1,(max(0,(int)((signed short)(coord[i] >> 16)))));
    ye=min(screen.height-1,(max(0,(int)((signed short)(coord[i + 1] >> 16)))));
    if((ys==ye) && (toggle==0)) {
      fillLine(xs, xe, ys, color);
      toggle=1;
    } else toggle=0;
  }
}


void FB_pPolygon(int *points, int n,int shape, int mode) {
  FB_hidex_mouse();
  fill2Poly(screen.fcolor,(int *)points,n);
  //FB_showx_mouse();
}


#define MAXQUEUELEN 1024

XEvent eque[MAXQUEUELEN];

volatile int queueptr=0;
volatile int queueptrlow=0;
int number=0;

#ifndef ANDROID
int escflag=0;
int numbers[16];
int anznumbers;

static void process_char(int a) {
  a&=0xff;
  
  if(escflag>=2) {
    if(a==';') {
      if(anznumbers<15) numbers[anznumbers++]=number;
      number=0;
    } else if(a=='?') escflag++;
    else if(a>='0' && a<='9') number=number*10+(a-'0');
    else {      
      escflag=0;
      if(anznumbers<15)  numbers[anznumbers++]=number;
      if(a=='o') {
        XEvent e;
        e.type=MotionNotify;
        e.xmotion.x=numbers[0];
        e.xmotion.y=numbers[1];
        e.xmotion.x_root=numbers[0];
        e.xmotion.y_root=numbers[1];
        e.xmotion.state=numbers[2];
#if DEBUG
	printf("Mausmotion: %d %d %d \n",numbers[0],numbers[1],numbers[2]);
#endif
        FB_put_event(&e);
      } else if(a=='M') {
        XEvent e;
        if(anznumbers>=3 ||numbers[2]>0) e.type=ButtonPress;
        else e.type=ButtonRelease;
        e.xbutton.x=numbers[0];
        e.xbutton.y=numbers[1];
        e.xbutton.button=1;
        e.xbutton.state=numbers[2];
        e.xbutton.x_root=numbers[0];
        e.xbutton.y_root=numbers[1];
#if DEBUG
	printf("Mausklick: %d %d %d \n",numbers[0],numbers[1],numbers[2]);
#endif
	FB_put_event(&e);
	
	if(eque[queueptr-1].type==ButtonPress) {
	  e.type=ButtonRelease;
          e.xbutton.x=numbers[0];
          e.xbutton.y=numbers[1];
          e.xbutton.button=1;
          e.xbutton.state=numbers[2];
          e.xbutton.x_root=numbers[0];
          e.xbutton.y_root=numbers[1];
#if DEBUG
	  printf("Mausrelease: %d %d %d \n",numbers[0],numbers[1],numbers[2]);
#endif
	  FB_put_event(&e);
	}
      } else if(a>='A' && a<='D') {
        XEvent e;
        if(a=='A') a=0x52;
	else if(a=='B') a=0x54;
	else if(a=='C') a=0x53;
	else if(a=='D') a=0x51;
	
        e.type=KeyPress;
        e.xkey.keycode=(char)a;
        e.xkey.ks=(a&255)|0xff00;
        e.xkey.buf[0]=(char)a;
        e.xkey.buf[1]=0;
        FB_put_event(&e);
        e.xkey.keycode=(char)a;
        e.xkey.ks=(a&255)|0xff00;
        e.type=KeyRelease;
        e.xkey.buf[0]=(char)a;
        e.xkey.buf[1]=0;
        FB_put_event(&e);
      } else printf("Unknown ESC-Code: %d\n",a);
    }    
  } else if(escflag==1) {
    if(a=='[') {escflag=2;number=anznumbers=0;}
    else escflag=0;
  } else {
    XEvent e;
    if(a==27) escflag=1;
    else if(a==127||a==10||a==8||a==9||a==27) {
      if(a==10) a=13;
      else if(a==127) a=8;
      e.type=KeyPress;
      e.xkey.keycode=(char)a;
      e.xkey.ks=((a&255)|0xff00);
      e.xkey.buf[0]=(char)a;
      e.xkey.buf[1]=0;
      FB_put_event(&e);
      e.xkey.keycode=(char)a;
      e.xkey.ks=((a&255)|0xff00);
      e.type=KeyRelease;
      e.xkey.buf[0]=(char)a;
      e.xkey.buf[1]=0;
      FB_put_event(&e);
    } else {
      e.xkey.keycode=(char)a;
      e.xkey.ks=(a&255);
      e.type=KeyPress;
      e.xkey.buf[0]=(char)a;
      e.xkey.buf[1]=0;
      FB_put_event(&e);
      e.xkey.keycode=(char)a;
      e.xkey.ks=(a&255);
      e.type=KeyRelease;
      e.xkey.buf[0]=(char)a;
      e.xkey.buf[1]=0;
      FB_put_event(&e);
    }
  }
}

#endif


static void collect_events()  {
#ifndef ANDROID
  fd_set aset,rset;
  int a,rc;
#ifdef TIMEVAL_WORKAROUND
  struct { int  tv_sec; 
           int  tv_usec; } tv;
#else 
    struct timeval tv;
#endif
   /* memset(&tv, 0, sizeof(tv));  */   

    FD_ZERO(&aset);
    FD_SET(0, &aset);

  while(1) {
    tv.tv_sec=0; tv.tv_usec=100;
    rset=aset;
    rc=select(FD_SETSIZE, &rset, NULL, NULL, &tv);
    if(rc==0) return;  /*TIMEOUT*/
    else if (rc<0) printf("select failed: errno=%d\n",errno);
    else {
      if(FD_ISSET(0,&rset)) {
        a=getc(stdin);
        process_char(a);
      } 
#if DEBUG
      printf("PC: %d, %d Events pending.\n",a,queueptr);
#endif
    }
  }
#endif
}


static void wait_events() {
#ifndef ANDROID
  fd_set set;
  int rc=0;
#ifdef TIMEVAL_WORKAROUND
  struct { int  tv_sec; 
           int  tv_usec; } tv;
#else 
    struct timeval tv;
#endif
   /* memset(&tv, 0, sizeof(tv));  */   
  while(queueptr==queueptrlow) {
    tv.tv_sec=0; tv.tv_usec=100;
    FD_ZERO(&set);
    FD_SET(0, &set);
    rc=select(1, &set, 0, 0, &tv);
    if (rc<0) printf("select failed: errno=%d\n",errno);
  }
#else
// backlog("waitevents");
while(queueptr==queueptrlow) usleep(10000);
// backlog("waitevents done.");


#endif
}


/* l"osche einen Event an der STelle r in der Schlange und lasse den rest aufr"ucken.*/

static void remove_event(int r) {
  int e;
 /*Event entfernen*/
  while(r!=queueptrlow) {
    e=r-1;
    if(e<0) e+=MAXQUEUELEN;
    eque[r]=eque[e];
    r=e;
  }
  queueptrlow++;
  if(queueptrlow>=MAXQUEUELEN) queueptrlow=0;
}

/*Event am Ende der Schlange anfügen*/

void FB_put_event(XEvent *event) {
  eque[queueptr++]=*event;
  if(queueptr>=MAXQUEUELEN) queueptr=0;
  if(queueptrlow==queueptr) {
   //  backlog("Event-queue is full!");
    /*"altestes Event "uberschreiben.*/
    queueptrlow++;
    if(queueptrlow>=MAXQUEUELEN) queueptrlow=0;
  }
}

/*Event am Anfang der Schlange einfügen*/

void FB_putback_event(XEvent *event) {
  queueptrlow--;
  if(queueptrlow<0) queueptrlow+=MAXQUEUELEN;
  if(queueptrlow!=queueptr) eque[queueptrlow]=*event;
  // else backlog("Event-queue is full!\n");
}
int FB_check_event(int mask, XEvent *event) {
  int e,i,r=-1;
  collect_events();
 // backlog("check events...");
  if(queueptr!=queueptrlow) {
    if(queueptr==0) e=MAXQUEUELEN;
    else e=queueptr;
    i=queueptrlow;    

    while(i!=e) {
      if((eque[i].type&mask)==eque[i].type) {
         r=i;
	
	 break;
      }
      i++;
      if(i>=MAXQUEUELEN) i=0;
    }
  }
  
  if(r<0) {
    queueptrlow=queueptr=0; /*alle events loeschen*/
    return(0);
  } 
  *event=eque[r];  
  queueptrlow=r; /*remove all other events*/
  remove_event(r);
  return(1);
}


void FB_event(int mask, XEvent *event) {
  int r=0;
  while(r==0) {
    r=FB_check_event(mask,event);
    if(r==0) {
      usleep(10000);
      wait_events();
    }
  }
}
void FB_clear_events()            { queueptrlow=queueptr=0; }
void FB_next_event(XEvent *event) { FB_event(0xffffffff, event); }

