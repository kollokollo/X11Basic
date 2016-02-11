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
  extern unsigned int fcolor,bcolor;
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
 SelectObject(bitcon[usewindow],pen[usewindow]); \
 DeleteObject(brush[usewindow]); \
 brush[usewindow]=CreateSolidBrush(c); \
 SelectObject(bitcon[usewindow],brush[usewindow]); \
 SetTextColor(bitcon[usewindow],c); }
#define SetBackground(c) SetBkColor(bitcon[usewindow],c)
#define FillRectangle(a,b,c,d); { \
  RECT rc; \
  rc.left=a; \
  rc.top=b; \
  rc.right=a+c; \
  rc.bottom=b+d; \
  FillRect(bitcon[usewindow],&rc,brush[usewindow]); \
}
#define DrawRectangle(a,b,c,d); { \
  RECT rc; \
  rc.left=a; \
  rc.top=b; \
  rc.right=a+c; \
  rc.bottom=b+d; \
  FrameRect(bitcon[usewindow],&rc,brush[usewindow]); \
}
#define DrawLine(a,b,c,d);  {MoveToEx(bitcon[usewindow],a,b,NULL); \
  LineTo(bitcon[usewindow],c,d);}
#define XDrawArc(a,b,c,d,e,f,g,h,i) Arc(bitcon[usewindow],d,e,d+f,d+g,0,0,0,0)
#define XFillArc(a,b,c,d,e,f,g,h,i) Ellipse(bitcon[usewindow],d,e,d+f,d+g)
#define DrawPoint(a,b) SetPixelV(bitcon[usewindow],a,b,global_color)
#define CopyArea(a,b,c,d,e,f) BitBlt(bitcon[usewindow],e,f,c,d,bitcon[usewindow],a,b,SRCCOPY)
#define DrawString(a,b,c,d) TextOut(bitcon[usewindow],a,(b-baseline),c,d)
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
#define DrawString(a,b,c,d) FB_DrawString(a,b-baseline,c,d)
#define DrawLine(a,b,c,d)  FB_line(a,b,c,d)
#define DrawPoint(a,b)  FB_plot(a,b)
#define CopyArea(a,b,c,d,e,f) FB_copyarea(a,b,c,d,e,f)
#elif defined USE_SDL
#define SetFillRule(c)   ;
#define SetFillStyle(c)  ;
#define SetForeground(c) fcolor=c;
#define SetBackground(c) bcolor=c;
#define FillRectangle(a,b,c,d)  boxColor(display[usewindow],a,b,(a)+(c)-1,(b)+(d)-1,fcolor)
#define DrawRectangle(a,b,c,d)  rectangleColor(display[usewindow],a,b,(a)+(c),(b)+(d),fcolor)
#define DrawString(a,b,c,d) {char s[d+1];memcpy(s,c,d);s[d]=0;stringColor(display[usewindow],a,(b)-chh+4,s,fcolor);}
#define DrawLine(a,b,c,d)  lineColor(display[usewindow],a,b,c,d,fcolor)
#define DrawPoint(a,b)  pixelColor(display[usewindow],a,b,fcolor)
#define CopyArea(a,b,c,d,e,f) ;
#define XDrawArc(a,b,c,d,e,f,g,h,i) pieColor(a,d,e,0,100,fcolor)
#define XFillArc(a,b,c,d,e,f,g,h,i) ;
#define XQueryPointer(a,b,c,d,e,f,g,h,i) *(i)=SDL_GetMouseState(g,h)
#else
#define SetFillRule(c)   XSetFillRule(display[usewindow], gc[usewindow],c)
#define SetFillStyle(c)  XSetFillStyle(display[usewindow], gc[usewindow],c)
#define SetForeground(c) XSetForeground(display[usewindow],gc[usewindow],c)
#define SetBackground(c) XSetBackground(display[usewindow],gc[usewindow],c)
#define FillRectangle(a,b,c,d) XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],a,b,c,d)
#define DrawRectangle(a,b,c,d) XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],a,b,c,d)
#define DrawString(a,b,c,d) XDrawString(display[usewindow],pix[usewindow],gc[usewindow],a,b,c,d)
#define DrawLine(a,b,c,d)  XDrawLine(display[usewindow],pix[usewindow],gc[usewindow],a,b,c,d)
#define DrawPoint(a,b)     XDrawPoint(display[usewindow],pix[usewindow],gc[usewindow],a,b)
#define CopyArea(a,b,c,d,e,f) XCopyArea(display[usewindow],pix[usewindow],pix[usewindow],gc[usewindow],a,b,c,d,e,f)
#endif
#endif




#define MAXMENUENTRYS 200
#define MAXMENUTITLES 80




extern unsigned char marker_typ;
extern int marker_size;
extern int boundary;
extern unsigned int baseline,chh,chw;
extern double ltextwinkel,ltextxfaktor,ltextyfaktor;
extern int ltextpflg;

void line(int x1,int y1,int x2,int y2);
void mybox(int x1,int y1,int x2, int y2);
void pbox(int x1,int y1,int x2, int y2);
void draw_string(int x, int y, char *text,int len);
int get_point(int x, int y);
int get_fcolor();
int get_bcolor();
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
void fetch_icon_pixmap(int nummer);
#endif
#define fill_width 16
#define fill_height 624
