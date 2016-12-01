/* graphics.h  (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include "options.h"
#ifdef FRAMEBUFFER
  #include "framebuffer.h"
#elif defined USE_SDL
  #include <SDL/SDL.h>
  extern SDL_Surface *display[];
  extern const unsigned char spat_a816[];
  extern const unsigned char asciiTable[];
#elif defined USE_X11
  #include <X11/Xlib.h>
  #include <X11/Xutil.h>
  /* X Font definitions */
  #define FONTBIG   "-*-fixed-*-r-normal-*-16-*-iso8859-*"
  #define FONT8x8   "-*-fixed-*-*-*-*-8-*"
  #define FONTSMALL "-*-fixed-*-*-*-*-8-*"
#elif defined USE_VGA
  #include <vga.h>
  #include <vgagl.h>
#endif

#ifdef WINDOWS_NATIVE
  extern HANDLE keyevent,buttonevent,motionevent;
  #define XSetLineAttributes(a,b,c,d,e,f) ; 
#endif



/* Orginal VDI Graphmodi */

#define GRAPHMD_REPLACE 1
#define GRAPHMD_TRANS   2
#define GRAPHMD_XOR     3
#define GRAPHMD_ERASE   4



#ifdef WINDOWS_NATIVE
#define SetForeground(c); { DeleteObject(pen[usewindow]); \
 pen[usewindow]=CreatePen(PS_SOLID,1,c); \
 SelectObject(window[usewindow].bitcon,pen[usewindow]); \
 DeleteObject(brush[usewindow]); \
 brush[usewindow]=CreateSolidBrush(c); \
 SelectObject(window[usewindow].bitcon,brush[usewindow]); \
 SetTextColor(window[usewindow].bitcon,c); }
#define SetBackground(c) SetBkColor(window[usewindow].bitcon,c)
#define FillRectangle(a,b,c,d); { \
  RECT rc; \
  rc.left=a; \
  rc.top=b; \
  rc.right=a+c; \
  rc.bottom=b+d; \
  FillRect(window[usewindow].bitcon,&rc,brush[usewindow]); \
}
#define DrawRectangle(a,b,c,d); { \
  RECT rc; \
  rc.left=a; \
  rc.top=b; \
  rc.right=a+c; \
  rc.bottom=b+d; \
  FrameRect(window[usewindow].bitcon,&rc,brush[usewindow]); \
}
#define DrawLine(a,b,c,d);  {MoveToEx(window[usewindow].bitcon,a,b,NULL); \
  LineTo(window[usewindow].bitcon,c,d);}
#define XDrawArc(a,b,c,d,e,f,g,h,i) Arc(window[usewindow].bitcon,d,e,d+f,d+g,0,0,0,0)
#define XFillArc(a,b,c,d,e,f,g,h,i) Ellipse(window[usewindow].bitcon,d,e,d+f,d+g)
#define DrawPoint(a,b) SetPixelV(window[usewindow].bitcon,a,b,window[usewindow].fcolor)
#define CopyArea(a,b,c,d,e,f) BitBlt(window[usewindow].bitcon,e,f,c,d,window[usewindow].bitcon,a,b,SRCCOPY)
#define DrawString(a,b,c,d) TextOut(window[usewindow].bitcon,a,(b-window[usewindow].baseline),c,d)
#define SetFillRule(c)   ;
#define SetFillStyle(c)  ;
#else
#ifdef FRAMEBUFFER
#define SetFillRule(c)   FB_setfillrule(c)
#define SetFillStyle(c)  FB_setfillstyle(c)
#define SetForeground(c) FB_set_color(c)
#define SetBackground(c) FB_set_bcolor(c)
#define FillRectangle(a,b,c,d)  FB_pbox(a,b,(a)+(c),(b)+(d))
#define DrawRectangle(a,b,c,d)  FB_box(a,b,(a)+(c),(b)+(d))
#define DrawString(a,b,c,d) FB_DrawString(a,b-window[usewindow].baseline,c,d,window[usewindow].chw,window[usewindow].chh)
#define DrawLine(a,b,c,d)  FB_line(a,b,c,d)
#define DrawPoint(a,b)  FB_plot(a,b)
#define CopyArea(a,b,c,d,e,f) FB_CopyArea(a,b,c,d,e,f)
#elif defined USE_SDL
#define SetFillRule(c)   ;
#define SetFillStyle(c)  ;
#define SetForeground(c) window[usewindow].fcolor=c
#define SetBackground(c) window[usewindow].bcolor=c
#define FillRectangle(a,b,c,d)  boxColor(window[usewindow].display,a,b,(a)+(c)-1,(b)+(d)-1,window[usewindow].fcolor)
#define DrawRectangle(a,b,c,d)  rectangleColor(window[usewindow].display,a,b,(a)+(c),(b)+(d),window[usewindow].fcolor)
#define DrawString(a,b,c,d) {char s[d+1];memcpy(s,c,d);s[d]=0;stringColor(window[usewindow].display,a,(b)-window[usewindow].chh+4,s,window[usewindow].fcolor);}
#define DrawLine(a,b,c,d)  lineColor(window[usewindow].display,a,b,c,d,window[usewindow].fcolor)
#define DrawPoint(a,b)  pixelColor(window[usewindow].display,a,b,window[usewindow].fcolor)
#define CopyArea(a,b,c,d,e,f) ;
#define XDrawArc(a,b,c,d,e,f,g,h,i) pieColor(a,d,e,0,100,window[usewindow].fcolor)
#define XFillArc(a,b,c,d,e,f,g,h,i) ;
#define XQueryPointer(a,b,c,d,e,f,g,h,i) *(i)=SDL_GetMouseState(g,h)
#elif defined USE_GEM
#define SetFillRule(c)   ;
#define SetFillStyle(c)  vsf_style(window[usewindow].vdi_handle,c)
#define SetForeground(c) vsf_color(window[usewindow].vdi_handle,c),vsl_color(window[usewindow].vdi_handle,c),vsm_color(window[usewindow].vdi_handle,c),vst_color(window[usewindow].vdi_handle,c)
#define SetBackground(c) ;
#define FillRectangle(a,b,c,d)  ;
#define DrawRectangle(a,b,c,d)  ;
#define DrawString(a,b,c,d) v_gtext(window[usewindow].vdi_handle,a,b,c)
#define DrawLine(a,b,c,d)  ;
#define DrawPoint(a,b) ;
#define CopyArea(a,b,c,d,e,f) ;
#define XDrawArc(a,b,c,d,e,f,g,h,i) ;
#define XFillArc(a,b,c,d,e,f,g,h,i) ;
#define XQueryPointer(a,b,c,d,e,f,g,h,i) ;
#elif defined USE_X11
#define SetFillRule(c)   XSetFillRule(window[usewindow].display, window[usewindow].gc,c)
#define SetFillStyle(c)  XSetFillStyle(window[usewindow].display, window[usewindow].gc,c)
#define SetForeground(c) XSetForeground(window[usewindow].display,window[usewindow].gc,c),window[usewindow].fcolor=c
#define SetBackground(c) XSetBackground(window[usewindow].display,window[usewindow].gc,c),window[usewindow].bcolor=c
#define FillRectangle(a,b,c,d) XFillRectangle(window[usewindow].display,window[usewindow].pix,window[usewindow].gc,a,b,c,d)
#define DrawRectangle(a,b,c,d) XDrawRectangle(window[usewindow].display,window[usewindow].pix,window[usewindow].gc,a,b,c,d)
#define DrawString(a,b,c,d) XDrawString(window[usewindow].display,window[usewindow].pix,window[usewindow].gc,a,b,c,d)
#define DrawLine(a,b,c,d)  XDrawLine(window[usewindow].display,window[usewindow].pix,window[usewindow].gc,a,b,c,d)
#define DrawPoint(a,b)     XDrawPoint(window[usewindow].display,window[usewindow].pix,window[usewindow].gc,a,b)
#define CopyArea(a,b,c,d,e,f) XCopyArea(window[usewindow].display,window[usewindow].pix,window[usewindow].pix,window[usewindow].gc,a,b,c,d,e,f)
#else
#define SetFillRule(c)   ;
#define SetFillStyle(c)  ;
#define SetForeground(c) ;
#define SetBackground(c) ;
#define FillRectangle(a,b,c,d)  ;
#define DrawRectangle(a,b,c,d)  ;
#define DrawString(a,b,c,d) ;
#define DrawLine(a,b,c,d)  ;
#define DrawPoint(a,b) ;
#define CopyArea(a,b,c,d,e,f) ;
#define XDrawArc(a,b,c,d,e,f,g,h,i) ;
#define XFillArc(a,b,c,d,e,f,g,h,i) ;
#define XQueryPointer(a,b,c,d,e,f,g,h,i) ;
#endif
#endif




#define MAXMENUENTRYS 200
#define MAXMENUTITLES 80


#if defined USE_X11 || defined USE_SDL
extern int global_graphmode;
#endif

extern unsigned char marker_typ;
extern int marker_size;
extern double ltextwinkel,ltextxfaktor,ltextyfaktor;
extern int ltextpflg;

void line(int x1,int y1,int x2,int y2);
void mybox(int x1,int y1,int x2, int y2);
void pbox(int x1,int y1,int x2, int y2);
void draw_string(int x, int y, char *text,int len);
int get_point(int x, int y);
void set_boundary(int n);
void set_graphmode(int n);
void set_font(char *name);
void set_fill(int c);
void ffill(int,int,int,int);
int mousex();
int mousey();
int mousek();
int mouses();

void graphics_setdefaults();

unsigned int get_color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
#ifdef USE_X11
extern char *display_name;  
Status my_XAllocColor(Display *display,Colormap map,XColor *pixcolor);
#endif
#define fill_width 16
#define fill_height 624
