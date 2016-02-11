
/* framebuffer.h      Framebuffer (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef __framebuffer__
#define __framebuffer__

//#define TS_DEVICE_NAME "/dev/ts"
#define FB_DEVICENAME "/dev/fb0"

/* color masks (16 Bit color screen assumed) */

#define R_MASK 0xf800
#define G_MASK 0x07e0
#define B_MASK 0x001f

/* Mouse keys */

#define MOUSE_RIGHT 1
#define MOUSE_LEFT  2
#define MOUSE_MIDDLE 4
#define MOUSE_PRESS 8

  #undef BLACK
  #undef WHITE
  #undef RED
  #undef GREEN
  #undef BLUE     
  #undef YELLOW   
  #undef MAGENTA  
  
/* Some colors (16 Bit color screen assumed) */

  #define BLACK     0x0000
  #define WHITE     0xffff
  #define RED       0xf800
  #define GREEN     0x07e0
  #define BLUE      0x001f
  #define YELLOW    0xffe0
  #define MAGENTA   0xf81f
  #define LIGHTBLUE 0x07ff
  #define GREY      0x7bef
  #define LIGHTGREY 0xadf7


typedef struct {
  int bpp;          /* Bits per pixel */
  int width,height; /* Screen dimensions */
  int scanline;     /* Bytes per Scanline */
  long size;        /* Size in bytes */
  int clip_x,clip_y,clip_w,clip_h; /* Clipping rectangle*/
  char *pixels;     /* Pointer to memory */
  int curor_x,cursor_y;
  int mouse_x,mouse_y;
  unsigned short mouseshown;
  unsigned short *mousepat;
  unsigned char *mousemask;
  unsigned char mouse_ox,mouse_oy; /*Active point in pattern*/
  unsigned short mouse_k,mouse_s;
  unsigned short linewidth;
  unsigned short linestyle;
  unsigned short linecap;
  unsigned short linejoin;
  unsigned int bcolor,fcolor;
  unsigned char alpha;
  unsigned char graphmode;
  unsigned int textmode;
  int textwinkel;
  unsigned short initialized;
  unsigned short fill_rule;
  unsigned short fill_style;
  unsigned short *fill_pat;
  
} G_CONTEXT;

#define CharWidth1632 16
#define CharWidth816 8
#define CharWidth57 5
#define CharHeight1632 32
#define CharHeight816 16
#define CharHeight57 (7+1)

extern const unsigned char asciiTable[];
extern const unsigned char spat_a816[];
extern unsigned char ext_font816[];
extern unsigned char ibm_like816[];

extern G_CONTEXT screen;
extern unsigned short vmousepat[16*16];
extern unsigned char vmousealpha[16*16];
extern const unsigned short mousepat[16*16];
extern const unsigned char mousealpha[16*16];

#define FL_NORMAL        0
#define FL_BOLD          1
#define FL_DIM         (1<<1)
#define FL_ITALIC      (1<<2)
#define FL_UNDERLINE   (1<<3)
#define FL_BLINK       (1<<4)
#define FL_BLINKRAPID  (1<<5)
#define FL_REVERSE     (1<<6)
#define FL_CONCREAL    (1<<7)
#define FL_CROSSED     (1<<8)
#define FL_HIDDEN      (1<<9)
#define FL_DBLUNDERLINE (1<<10)
#define FL_FRAMED       (1<<11)
#define FL_TRANSPARENT  (1<<12)

void Fb_BlitCharacter816_scale(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned char character, 
                               int flags,int fontnr,int charwidth,int charheight);

void Fb_BlitCharacter1632(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned char character,int flags,int);
void Fb_BlitCharacter816(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned char character,int flags,int);
void Fb_BlitCharacter57(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned char character,int flags,int);
void Fb_BlitText3264(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str);
void Fb_BlitText1632(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str);
void Fb_BlitText816(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str);
void Fb_BlitText57(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str);
void Fb_BlitText816_scale(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str,int w,int h);



#ifdef FRAMEBUFFER
  #define XDrawArc(a,b,c,d,e,f,g,h,i) FB_Arc(d,e,f,g,h,i)
  #define XFillArc(a,b,c,d,e,f,g,h,i) FB_pArc(d,e,f,g,h,i)
  #define XDrawPoints(a,b,c,d,e,f)    FB_points(d,e,f)
  #define XDrawLines(a,b,c,d,e,f)     FB_lines(d,e,f)
  #define XFillPolygon(a,b,c,d,e,f,g) FB_pPolygon(d,e,f,g)
  #define XSetLineAttributes(a,b,c,d,e,f) ; 
  #define XWindowEvent(a,b,c,d) FB_event(c,d)
  #define XCheckWindowEvent(a,b,c,d) FB_check_event(c,d)
  #define XNextEvent(a,b)            FB_next_event(b)
  
  #define XSetFillRule(a,b,c) FB_setfillrule(c)
  #define XSetFillStyle(a,b,c) FB_setfillstyle(c)
  
  #define XQueryPointer(a,b,c,d,e,f,g,h,i) FB_Query_pointer(e,f,g,h,i)
  #define XUnmapWindow(a,b) ;
  #define XMapWindow(a,b) ;

  #define CharWidth    5
  #define CharHeight    (7+1)

  typedef struct {
    int x;
    int y;
  } XPoint;
  typedef struct {
    int type;
    struct {
      int x;
      int y;
      int button;
      int x_root;
      int y_root;
      int state;
    } xbutton;
    struct {
      int x;
      int y;
      int x_root;
      int y_root;
      int state;
    } xmotion;
    struct {
      int x;
      int y;
      int x_root;
      int y_root;
      int state;
      int keycode;
      int ks;
      char buf[4];
    } xkey;
  } XEvent;
  #define ExposureMask 3
  #define NoExpose 1
  #define Expose 2
  #define GraphicsExpose 3

  #define ButtonPressMask   0x08
  #define ButtonReleaseMask   0x04
  #define ButtonPress 8
  #define ButtonRelease 4

  #define PointerMotionMask 0x30
  #define MotionNotify      0x10

  #define KeyPressMask      0x80
  #define KeyPress          0x80
  #define KeyRelease        0x40
  #define KeyReleaseMask    0x40

  #define CirculateRequest 0
  #define ConfigureRequest 0
  #define ResizeRequest    0

  extern int global_mousex,global_mousey,global_mousek,global_mouses;
#endif
void Fb_Open();
void Fb_Close();
void FB_Clear(G_CONTEXT *screen);
void FB_set_color(unsigned short color);
void FB_set_bcolor(unsigned short color);
void FB_set_alpha(unsigned char alpha);
void FB_set_textmode(unsigned int mode);
void FB_set_clip(G_CONTEXT *screen,int x,int y,int w,int h);
void FB_clip_off(G_CONTEXT *screen);
void FB_plot(int x, int y);
unsigned short FB_point(int x, int y);

char *FB_get_image(int x, int y, int w,int h, int *len);
void FB_put_image(char *,int, int);
void FB_get_geometry(int *x, int *y, int *w, int *h, int *b, int *d);
void FB_mtext(int x, int y, char *t);

void FB_DrawString(int x, int y, const char *t,int len);
void FB_DrawLine(int x0, int y0, int x1, int y1,unsigned short color);
void FillBox (int x, int y, int w, int h, unsigned short color);
unsigned short int mix_color(unsigned short int a, unsigned short int b, unsigned char alpha);
void FB_show_mouse();
void FB_hide_mouse();
void FB_draw_sprite(unsigned short *sprite, unsigned char *alpha,int x,int y);
void FB_hide_sprite(int x,int y);
void fill2Poly(unsigned short color,int *point, int num);
void FB_bmp2pixel(const char *s,unsigned short *d,int w, int h, unsigned short color);
void FB_bmp2mask(const char *s,unsigned char *d,int w, int h);

void FB_defaultcontext();



#define FillSolid 0
#define FillStippled 2



void FB_setfillpattern(const char *p);
void FB_setfillstyle(int);
void FB_setfillrule(int);


#endif
