
/* graphics.c      graphics-Routinen (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "defs.h"
#ifdef USE_GEM
  #include <osbind.h>
  #include <gem.h>
#elif defined USE_SDL
  #include <SDL/SDL.h>
  #include <SDL/SDL_gfxPrimitives.h>
#endif
#include "x11basic.h"
#include "graphics.h"
#include "aes.h"
#include "window.h"


#ifdef USE_SDL
#elif defined USE_GEM
#elif defined USE_X11
char *display_name = NULL;  /* NULL: Nimm Argument aus setenv DISPLAY */
#include "bitmaps/bombe_gross.bmp"
#endif

unsigned char marker_typ;
int marker_size=1;
double ltextwinkel=0,ltextxfaktor=0.3,ltextyfaktor=0.5;
int ltextpflg=0;

/* Set default values */
void graphics_setdefaults() {
  marker_size=1;
  ltextwinkel=0,ltextxfaktor=0.3,ltextyfaktor=0.5;
  ltextpflg=0;
#ifdef FRAMEBUFFER
  FB_defaultcontext();
#endif
}


/* Line-Funktion (fuer ltext) */
void line(int x1,int y1,int x2,int y2) {
#ifdef USE_X11
  XDrawLine(window[usewindow].display,window[usewindow].pix,window[usewindow].gc,x1,y1,x2,y2);
#elif defined FRAMEBUFFER
  FB_line(x1,y1,x2,y2);
#elif defined USE_SDL
  lineColor(window[usewindow].display,x1,y1,x2,y2,window[usewindow].fcolor);
#elif defined USE_GEM
  short a[4];
  a[0]=x1;
  a[1]=y1;
  a[2]=x2;
  a[3]=y2;
  v_pline(window[usewindow].vdi_handle,2,a);
#else
  DrawLine(x1,y1,x2,y2);
#endif
}

void pbox(int x1,int y1,int x2, int y2) {
  FillRectangle(min(x1,x2),min(y1,y2),abs(x2-x1)+1,abs(y2-y1)+1); 
}


void mybox(int x1,int y1,int x2, int y2) {
#ifdef USE_GEM
  short a[10];
  a[6]=a[8]=a[0]=x1;
  a[3]=a[9]=a[1]=y1;
  a[4]=a[2]=x2;
  a[7]=a[5]=y2;
  v_pline(window[usewindow].vdi_handle,5,a);
#else
  DrawRectangle(min(x1,x2),min(y1,y2),abs(x2-x1),abs(y2-y1)); 
#endif
}
#if defined USE_SDL
Uint32 sdl_getpixel(SDL_Surface *surface, int x, int y);
#endif
int get_point(int x, int y) {
#if defined WINDOWS_NATIVE
  return(0);  /*  TODO */
#elif defined USE_SDL
  return(sdl_getpixel(window[usewindow].display,x,y));
#elif defined FRAMEBUFFER
  return(FB_point(x,y));
#elif defined USE_X11
    XImage *Image;
    graphics();
    int d=XDefaultDepthOfScreen(XDefaultScreenOfDisplay(window[usewindow].display));
    Image=XGetImage(window[usewindow].display,window[usewindow].win,
                x, y, 1, 1, AllPlanes,(d==1) ?  XYPixmap : ZPixmap);
    int r=XGetPixel(Image, 0, 0);
    XDestroyImage(Image); 
    return(r);
#else
 return(0);
#endif
}


void set_boundary(int n) {
  /* currently has no effect.*/
}


int global_graphmode=GRAPHMD_REPLACE;

void set_graphmode(int n) { 
/*            n=1 copy src
              n=2 src xor dest
              n=3 invert (not dest)
              n=4 src and dest
              n=5 not (src xor dest)
              n<0 uebergibt -n an X-Server
 */  

  global_graphmode=n;
#ifdef USE_X11
  XGCValues gc_val;  
  switch (n) {
    case GRAPHMD_REPLACE:gc_val.function=GXcopy;       break;
    case GRAPHMD_TRANS:gc_val.function=GXxor;        break;
    case GRAPHMD_XOR:gc_val.function=GXinvert;     break;
    case GRAPHMD_ERASE:gc_val.function=GXand;        break;
    case 5:gc_val.function=GXequiv;      break;
    case 6:gc_val.function=GXandInverted;break;
    case 7:gc_val.function=GXor;         break;
    case 8:gc_val.function=GXorInverted; break;
    default:
    if(n>=0) gc_val.function=n;
    else gc_val.function=-n;
    break;
  } 
  XChangeGC(window[usewindow].display, window[usewindow].gc,  GCFunction, &gc_val);
#elif defined USE_SDL
#elif defined FRAMEBUFFER
  FB_setgraphmode(n);
  switch (n) {
    case GRAPHMD_REPLACE:FB_set_alpha(255);break;
    case GRAPHMD_TRANS:break;
    case GRAPHMD_XOR:break;
    case GRAPHMD_ERASE:FB_set_alpha(127);break;
    case 5:break;
    default:
    if(n>=0) FB_set_alpha(n);
    else FB_set_alpha(-n);
    break;
  } 
#endif
}

/* NAME="BIG"
        "SMALL"
        "LARGE"
	"8x16"
	"8x8"
	"5x7"

*/

#ifdef HAVE_FREETYPE
  FT_Library    freetype_library=NULL;
  FT_Face       freetype_face=NULL;
#endif
#if defined USE_SDL
#include <SDL/SDL_ttf.h>
TTF_Font *ttffont = NULL;
int ttffontheight=20;  /* pt @ 100dpi*/
#endif

void set_font(char *name) {
  if(name==NULL || *name==0) return;
  if(!strcmp(name+strlen(name)-4,".ttf") || !strcmp(name+strlen(name)-4,".TTF")) {
    if(exist(name)) {

#ifdef HAVE_FREETYPE
//  FT_Error      error;
  if(!freetype_library) FT_Init_FreeType( &freetype_library ); 
  FT_New_Face( freetype_library, name, 0, &freetype_face );/* create face object */
  /* error handling omitted */
  /* use 50pt at 100dpi */
  FT_Set_Char_Size( freetype_face, 50 * 64, 0,100, 0 );                /* set character size */
  /* error handling omitted */

  /* cmap selection omitted;                                        */

  /* Use defaults */

  window[usewindow].chw=24;
  window[usewindow].chh=48;
  window[usewindow].baseline=window[usewindow].chh-6;
#elif defined USE_SDL
  if(ttffont) {
    TTF_CloseFont(ttffont);
    ttffont=NULL;
  }
  ttffont=TTF_OpenFont(name,20);
  if(ttffont) ttffontheight=TTF_FontHeight(ttffont);
  else ttffontheight=20;

#else
      printf("WARNING: True-type font support not compiled in.\n");
#endif
      return;
    }
  } 
#ifdef HAVE_FREETYPE
   if(freetype_face) FT_Done_Face(freetype_face);
   if(freetype_library) FT_Done_FreeType(freetype_library);
   freetype_face=NULL;
   freetype_library=NULL;
#endif

#ifdef USE_X11
   XGCValues gc_val;  
   XFontStruct *fs;
   if(strcmp(name,"SMALL")==0 || strcmp(name,"5x7")==0) fs=XLoadQueryFont(window[usewindow].display, FONTSMALL);
   else if(strcmp(name,"BIG")==0 || strcmp(name,"8x16")==0) fs=XLoadQueryFont(window[usewindow].display, FONTBIG);
   else if(strcmp(name,"MEDIUM")==0 || strcmp(name,"8x8")==0) fs=XLoadQueryFont(window[usewindow].display, FONT8x8);
   else fs=XLoadQueryFont(window[usewindow].display, name);
   if(fs!=NULL)  {
     gc_val.font=fs->fid;
     XChangeGC(window[usewindow].display, window[usewindow].gc,  GCFont, &gc_val);
     window[usewindow].baseline=fs->ascent;
     window[usewindow].chh=window[usewindow].baseline+fs->descent;
     window[usewindow].chw=fs->max_bounds.width;
   }
#elif defined FRAMEBUFFER
  if(strcmp(name,"BIG")==0 || strcmp(name,"8x16")==0) {
    window[usewindow].chw=CharWidth816;
    window[usewindow].chh=CharHeight816;
    window[usewindow].baseline=window[usewindow].chh-2;   
  } else if(strcmp(name,"HUGE")==0 || strcmp(name,"24x48")==0) {
    window[usewindow].chw=24;
    window[usewindow].chh=48;
    window[usewindow].baseline=window[usewindow].chh-6;
  } else if(strcmp(name,"GIANT")==0 || strcmp(name,"32x64")==0) {
    window[usewindow].chw=32;
    window[usewindow].chh=64;
    window[usewindow].baseline=window[usewindow].chh-8;
  } else if(strcmp(name,"LARGE")==0 || strcmp(name,"16x32")==0) {
    window[usewindow].chw=CharWidth1632;
    window[usewindow].chh=CharHeight1632;
    window[usewindow].baseline=window[usewindow].chh-4;   
  } else if(strcmp(name,"MEDIUM")==0 || strcmp(name,"8x8")==0) {
    window[usewindow].chw=8;
    window[usewindow].chh=8;
    window[usewindow].baseline=window[usewindow].chh-1;
  } else if(strcmp(name,"SMALL")==0 || strcmp(name,"5x7")==0) {
    window[usewindow].chw=CharWidth57;
    window[usewindow].chh=CharHeight57;
    window[usewindow].baseline=window[usewindow].chh-0;   
  } else {
    window[usewindow].chw=CharWidth;
    window[usewindow].chh=CharHeight;
    window[usewindow].baseline=window[usewindow].chh-0;
  }
#elif defined USE_SDL
  ttffont = NULL;
  if(strcmp(name,"BIG")==0 || strcmp(name,"8x16")==0) {
    window[usewindow].chw=8;
    window[usewindow].chh=16;
    window[usewindow].baseline=window[usewindow].chh-2; 
    gfxPrimitivesSetFont(spat_a816,window[usewindow].chw,window[usewindow].chh); 	
  } else if(strcmp(name,"MEDIUM")==0 || strcmp(name,"8x8")==0) {
    window[usewindow].chw=8;
    window[usewindow].chh=8;
    window[usewindow].baseline=window[usewindow].chh-0;
    gfxPrimitivesSetFont(NULL,window[usewindow].chw,window[usewindow].chh); 	
  } else if(strcmp(name,"SMALL")==0 || strcmp(name,"5x7")==0) {
    window[usewindow].chw=5;
    window[usewindow].chh=7;
    window[usewindow].baseline=window[usewindow].chh-0;
    gfxPrimitivesSetFont(asciiTable,window[usewindow].chw,window[usewindow].chh); 	
  }
#endif
}

#ifdef HAVE_FREETYPE
/* Helper function for True Type Font drawing */
static void draw_fontbitmap(FT_Bitmap* bitmap,FT_Int x,FT_Int y) {
  FT_Int i,j,p,q;
  FT_Int x_max=x+bitmap->width;
  FT_Int y_max=y+bitmap->rows;

  /* for simplicity, we assume that `bitmap->pixel_mode' */
  /* is `FT_PIXEL_MODE_GRAY' (i.e., not a bitmap font)   */
  unsigned char a=1;
  for(i=x,p=0;i<x_max;i++,p++) {
    for(j=y,q=0;j<y_max;j++,q++) {
      if(i<0 || j<0 || i>=window[usewindow].w || j>=window[usewindow].h) continue;
      a=bitmap->buffer[q*bitmap->width+p];
      if(a && (double)random()/RAND_MAX*255.0<(double)a) {DrawPoint(i,j);}
      else if(global_graphmode!=GRAPHMD_TRANS)           {DrawBgPoint(i,j);}
    }
  }
}
#endif


/* Draw a TEXT String, using the builtin fonts or TrueType Fonts (.ttf) */

void draw_string(int x, int y, char *text,int len) {
#ifdef HAVE_FREETYPE
  if(freetype_face) {
    FT_GlyphSlot slot;
    FT_Matrix    matrix;  /* transformation matrix */
    FT_Vector    pen;     /* untransformed origin  */
    FT_Error     error;
    FT_Set_Char_Size(freetype_face,ltextxfaktor*64,0,100,0); /* set character size use xx pt at 100dpi */
    slot=freetype_face->glyph;
    double angle=(double)ltextwinkel/180.0*PI;
    /* set up matrix */
    matrix.xx=(FT_Fixed)( cos(angle)*0x10000L);
    matrix.xy=(FT_Fixed)(-sin(angle)*0x10000L);
    matrix.yx=(FT_Fixed)( sin(angle)*0x10000L);
    matrix.yy=(FT_Fixed)( cos(angle)*0x10000L);
    /* the pen position starts at */
    pen.x=x*64;
    pen.y=(window[usewindow].h-y)*64;
    int n;
    for(n=0;n<len;n++) {
      FT_Set_Transform(freetype_face,&matrix,&pen);   /* set transformation */
      /* load glyph image into the slot (erase previous one) */
      error=FT_Load_Char(freetype_face,text[n],FT_LOAD_RENDER);
      if(error) continue;                 /* ignore errors */

      /* now, draw to our target surface (convert position) */
      draw_fontbitmap(&slot->bitmap,slot->bitmap_left,window[usewindow].h-slot->bitmap_top);
      pen.x+=slot->advance.x;    /* increment pen position */
      pen.y+=slot->advance.y;
    }
    return;
  }
#endif
#ifdef WINDOWS_NATIVE
  TextOut(bitcon[usewindow],x,(y-window[usewindow].baseline),text,len);
#elif defined FRAMEBUFFER
  FB_DrawString(x,y-window[usewindow].baseline,text,len,window[usewindow].chw,window[usewindow].chh);
#elif defined USE_X11 
  #ifdef X_HAVE_UTF8_STRING 
    // TODO:
    if(global_graphmode==GRAPHMD_REPLACE) XDrawImageString(window[usewindow].display,window[usewindow].pix,window[usewindow].gc,x,y,text,len);
    else XDrawString(window[usewindow].display,window[usewindow].pix,window[usewindow].gc,x,y,text,len);    
  #else
    if(global_graphmode==GRAPHMD_REPLACE) XDrawImageString(window[usewindow].display,window[usewindow].pix,window[usewindow].gc,x,y,text,len);
    else XDrawString(window[usewindow].display,window[usewindow].pix,window[usewindow].gc,x,y,text,len);
  #endif
#elif defined USE_SDL
  char s[len+1];
  memcpy(s,text,len);
  s[len]=0;
  
  /* TODO: implement SDL_TTF with angle here ....*/
  if(ttffont) {
    SDL_Surface *message = NULL;

    unsigned int fgc=window[usewindow].fcolor;
    SDL_Color textColor = { (fgc&0xff000000)>>24, (fgc&0xff0000)>>16, (fgc&0xff00)>>8 };
    message = TTF_RenderUTF8_Blended(ttffont,s, textColor);
    if(message) {
      SDL_Rect a={0,0,message->w,message->h};
      SDL_Rect b={x,y-message->h,message->w,message->h};
      SDL_BlitSurface(message, &a,window[usewindow].display, &b);
      SDL_FreeSurface(message);
      return;
    }
  }
  stringColor(window[usewindow].display,x,y-window[usewindow].baseline+2,s,window[usewindow].fcolor);
#endif
}



void set_fill(int c) {
#include "bitmaps/fill.xbm"
#ifdef USE_X11
  static Pixmap fill_pattern;
  static int fill_alloc=0;
    if(fill_alloc) XFreePixmap(window[usewindow].display,fill_pattern);
    fill_pattern = XCreateBitmapFromData(window[usewindow].display,window[usewindow].win,
                fill_bits+c*fill_width*fill_width/8,fill_width,fill_width);
           /*XSetFillStyle(window[usewindow].display, window[usewindow].gc, FillStippled); */
    fill_alloc=1;   
    XSetStipple(window[usewindow].display, window[usewindow].gc,fill_pattern);
#elif defined FRAMEBUFFER
    FB_setfillpattern(fill_bits+c*16*2);
#endif
}
int mousex() {
#if defined ANDROID || defined FRAMEBUFFER
  graphics(); 
  return(screen.mouse_x);
#elif defined  WINDOWS_NATIVE
  return(global_mousex);
#elif defined USE_X11 || defined USE_SDL
#if defined USE_X11
  Window root_return,child_return;
#endif
  int root_x_return, root_y_return,win_x_return, win_y_return;
  unsigned int mask_return;

  graphics(); 
  XQueryPointer(window[usewindow].display, window[usewindow].win, &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);
  return(win_x_return);
#else
  return(0);
#endif
}
int mousey() {
#if defined ANDROID || defined FRAMEBUFFER
  graphics(); 
  return(screen.mouse_y);
#elif defined  WINDOWS_NATIVE
  return(global_mousey);
#elif defined USE_X11 || defined USE_SDL
#ifdef USE_X11
  Window root_return,child_return;
#endif
  int root_x_return, root_y_return,win_x_return, win_y_return;
  unsigned int mask_return;
  graphics(); 
  XQueryPointer(window[usewindow].display, window[usewindow].win, &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);
  return(win_y_return);
#else
  return(0);
#endif
}
int mousek() {
#if defined ANDROID || defined FRAMEBUFFER
  graphics(); 
  return(screen.mouse_k);
#elif defined WINDOWS_NATIVE 
  return(global_mousek);
#elif defined USE_X11 || defined USE_SDL
#ifdef USE_X11
   Window root_return,child_return;
#endif
   int root_x_return, root_y_return,win_x_return, win_y_return;
   unsigned int mask_return;
   graphics(); 
   XQueryPointer(window[usewindow].display, window[usewindow].win, &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);
#if defined USE_SDL
  return(mask_return&0xff);
#else
   return(mask_return>>8);
#endif
#else
  return(0);
#endif
}
int mouses() {
#if defined WINDOWS_NATIVE
  return(global_mouses);
#elif defined USE_X11
  int root_x_return, root_y_return,win_x_return, win_y_return;
  unsigned int mask_return;
   Window root_return,child_return;
   graphics();
   
   XQueryPointer(window[usewindow].display, window[usewindow].win, &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);
   return(mask_return & 255);
#elif defined USE_SDL
  return(0);
#elif defined FRAMEBUFFER || defined ANDROID
  graphics(); 
  return(screen.mouse_s);
#else
  return(0);
#endif
}


/* Alloziert einen Farbeintrag in der Palette.
   Rueckgabewert ist die Nummer des Farbeintrags.
   Ist kein neuer Eintrag mehr frei, so wird die Nummer des Eintrags
   zurueckgeliefert, der der spezifizierten Farbe am naechsten kommt.
   Diese Routine kann also kein XAllocColor failed mehr produzieren.

   (c) Markus Hoffmann  1998                                   */


unsigned int get_color(unsigned char r, unsigned char g, unsigned char b,unsigned char a) {
#ifdef WINDOWS_NATIVE
  return(RGB(r,g,b));
#elif defined FRAMEBUFFER
  return(FB_get_color(r,g,b));
#elif defined USE_SDL
  return((unsigned int)SDL_MapRGB(window[usewindow].display->format, r, g, b)<<8 |0xff);
#elif defined USE_X11
  Colormap map;
  XColor pixcolor;

  map =XDefaultColormapOfScreen ( XDefaultScreenOfDisplay ( window[usewindow].display ) );

  pixcolor.red=r<<8;
  pixcolor.green=g<<8;
  pixcolor.blue=b<<8;
  if(my_XAllocColor(window[usewindow].display, map, &pixcolor)==0) printf("ERROR: could allocate color.\n");
  return(pixcolor.pixel);
#else
  return(0);
#endif
}



#ifdef USE_X11
Status my_XAllocColor(Display *display,Colormap map,XColor *pixcolor) {
  Status rval;
  if((rval=XAllocColor(display, map, pixcolor))==0) {
    int i,r,g,b;
    uint32_t e=0xfffffff,d;
    XColor savecolor;
    XColor ppixcolor[256];
	for(i=0;i<256;i++) ppixcolor[i].pixel=i;
	XQueryColors(display, map, ppixcolor,256);
	
    r=pixcolor->red;
    g=pixcolor->green;
    b=pixcolor->blue;

    savecolor=*pixcolor;
	
   for(i=0;i<256;i++) {
     d=(ppixcolor[i].red-r)/16*(ppixcolor[i].red-r)/16+
       (ppixcolor[i].green-g)/16*(ppixcolor[i].green-g)/16+
       (ppixcolor[i].blue-b)/16*(ppixcolor[i].blue-b)/16;
     if(d<e) {
       e=d;
       savecolor=ppixcolor[i];
     }
   }
   /*
   printf("Gewuenscht: r= %x, g=%x, b=%x.\n",r,g,b);
   printf("Farbe wurde abgewandelt: %d, Close %d.\n",savecolor.pixel, e);
   if(e==0) printf("** Volltreffer: recycled \n");
   printf("Neu: r= %x, g=%x, b=%x.\n",savecolor.red,savecolor.green,savecolor.blue);
   */
   rval=XAllocColor(display, map, &savecolor);
   pixcolor->pixel=savecolor.pixel;
  }
  return(rval);

}
#endif

#ifdef USE_GEM
void  ffill(int x0,int y0,int fill_color, int border_color) {
  v_contourfill(window[usewindow].vdi_handle,x0,y0, border_color);
}
#else
/**** Flood Fill *********************************************************
*	Source code is based on:
*	"Programmer's guide to PC & PS/2 Video Systems"
*/
#define QUEUESIZE   256		/* The size of Queue (400) */

#define FF_UP	    1
#define FF_DN	   -1

#define SCAN_UNTIL  0
#define SCAN_WHILE  1
#define FF_FILLED   (!0)

struct	tagParams	{
		int    xl;     /* leftmost pixel in run */
		int    xr;     /* rightmost pixel in run */
		int y;	    /* y-coordinate of run */
		unsigned char f; /* TRUE if run is filled (blocked) */
};

struct tagQUEUE	{
	struct tagQUEUE	*pQ;     /* pointer to opposite queue */
	int 	    d;	    /* direction (UP or DN) */
	int 	    n;	    /* number of unfilled runs in queue */
	int 	    h;	    /* index of head of queue */
	int 	    t;	    /* index of tail of queue */
	struct tagParams *run;
};

typedef	struct tagQUEUE	FF_QUEUE;

int 	ff_scan_left(int, int, int, int );
int 	ff_scan_right(int,int, int, int );
void    ff_add_queue(FF_QUEUE *,int,int, int, int );
int	ff_next_branch(int,int,int,int,int);
int	ff_in_queue(FF_QUEUE *,int,int,int);


void  ffill(int x0,int y0,int fill_color, int border_color) {
  int y,x,xl,xr,xln,xrn;
  int qp,bcd;
  int scan_type;
  struct tagParams ff_buf1[QUEUESIZE];
  struct tagParams ff_buf2[QUEUESIZE];
  FF_QUEUE Qup,Qdn,*Q;
  /* Check that we are inside the screen 
     maybe better: return if not inside.
  */
  if(x0<window[usewindow].x) x0=window[usewindow].x;
  if(y0<window[usewindow].y) y0=window[usewindow].y;
  if(x0>window[usewindow].x+window[usewindow].w) x0=window[usewindow].x+window[usewindow].w-1;
  if(y0>window[usewindow].y+window[usewindow].h) y0=window[usewindow].y+window[usewindow].h-1;

  xr=xl=x=x0;y=y0;

   /* do nothing if the seed pixel is a border pixel */
  if(border_color==-1)  {
    border_color=get_point(x0,y0);
    scan_type=SCAN_WHILE;
  } else  scan_type=SCAN_UNTIL;

  if(scan_type==SCAN_UNTIL) {
    if(get_point(x0,y0)==border_color) return;
  } else {
    if(get_point(x0,y0)==fill_color) return;
  }
  Qup.run = ff_buf1;
  Qdn.run = ff_buf2;
  Qup.pQ = &Qdn;		/* pointer to opposite queue */
  Qup.d = FF_UP; 		/* direction for queue */
  Qup.h = -1;
  Qup.t = 0;
  Qup.n = 0;

  Qdn.pQ = &Qup;
  Qdn.d = FF_DN;
  Qdn.h = -1;
  Qdn.t = 0;
  Qdn.n = 0;
  /* put the seed run in the up queue */
  Q = &Qup;
  ff_add_queue(Q,ff_scan_left(x0,y0,border_color,scan_type),
		ff_scan_right(x0,y0,border_color,scan_type),y0,!FF_FILLED);

  for(;;) {
    if(Q->n==0) {
      Q=Q->pQ;
      if(Q->n==0) break;
    }
    qp = Q->h;
    while(qp>=Q->t) {
      if(!Q->run[qp].f) {
	y=Q->run[qp].y;
	xl=Q->run[qp].xl;
	xr=Q->run[qp].xr;
	line(xl,y,xr+1,y);      /* fill the run */
	activate();
	Q->run[qp].f=FF_FILLED;
	Q->n--;
	break;
      } else qp-- ;
    }
/*    printf("Queue(%d) [%d,%d]:  n=%d\n",Q->d,Q->h,Q->t,Q->n);*/
    if(Q->d==FF_UP) {
      while((Q->h>qp)&&(Q->run[Q->h].y<(y-1))) Q->h--;
    } else {
      while((Q->h>qp)&&(Q->run[Q->h].y>(y+1))) Q->h--;
    }
    xln=ff_next_branch(xl,xr,y+Q->d,border_color,scan_type);
    while(xln>=0) {
      x=(xln>xl)?xln:xl;
      xrn=ff_scan_right(x,y+Q->d,border_color,scan_type);
      if(!ff_in_queue(Q,xln,xrn,y+Q->d)) ff_add_queue(Q,xln,xrn,y+Q->d,!FF_FILLED);
      if(xrn>(xr-2)) break;
      else {
        x=xrn+2;
	xln=ff_next_branch(x,xr,y+Q->d,border_color,scan_type);
      }
    }
    bcd=0;
    xln=ff_next_branch(xl,xr,y-Q->d,border_color,scan_type);
    while(xln>=0) {
      x=(xln>xl)?xln:xl;
      xrn=ff_scan_right(x,y-Q->d,border_color,scan_type);
      if(!ff_in_queue(Q,xln,xrn,y-Q->d)) {
        ff_add_queue( Q->pQ, xln, xrn, y-Q->d,!FF_FILLED);
        bcd=1;
      }
      if(xrn>(xr-2)) break;
      else {
        x=xrn+2;
	xln=ff_next_branch(x,xr,y-Q->d,border_color,scan_type);
      }
    }
    if(bcd) {
      Q=Q->pQ;
      ff_add_queue(Q,xl,xr,y,FF_FILLED);
    }
  }
}
int ff_scan_left(int xl,int y, int ucPixel, int f ) {
/*  printf("Scan-left: x=%d,y=%d  ",xl,y);*/
#ifndef NOGRAPHICS
  if(f==SCAN_UNTIL)  {
    if(get_point(xl,y)==ucPixel) return -1;
    do {
      if(--xl<window[usewindow].x) break;
    } while(get_point(xl,y)!=ucPixel);
  }  else {
    if(get_point(xl,y)!=ucPixel) return -1;
    do {
      if(--xl<window[usewindow].x) break;
    } while (get_point(xl,y)==ucPixel);
  }
#endif
  return ++xl;
}


int ff_scan_right(int xr,int y,int ucPixel,int f) {
/*  printf("Scan-right: x=%d,y=%d  ",xr,y); */
#ifndef NOGRAPHICS
  if(f==SCAN_UNTIL) {
    if(get_point(xr,y)==ucPixel) return -1;
    do {
      if(++xr>=window[usewindow].x+window[usewindow].w) break;
    } while(get_point(xr,y)!=ucPixel);
  } else {
    if(get_point(xr,y)!=ucPixel) return -1;
    do  {
      if(++xr>=window[usewindow].x+window[usewindow].w) break;
    } while(get_point(xr,y)==ucPixel);
  }
#endif
  return --xr;
}

void  ff_add_queue(FF_QUEUE *Q,int xl,int xr,int y,int f) {
  int   qp,i;
/*    printf("Add-queue: x=%d..%d,y=%d\n",xl,xr,y); */
  if(Q->d==FF_UP) {
    for ( qp = Q->t; qp <= Q->h; qp++ )
      if ( Q->run[qp].y <= y ) break;
  } else {
    for ( qp = Q->t; qp <= Q->h; qp++ )
      if ( Q->run[qp].y >= y ) break;
  }
  if(qp<=Q->h) {
    Q->h++;
    for(i=Q->h;i>qp;--i) {
      Q->run[i].xl=Q->run[i-1].xl;
      Q->run[i].xr=Q->run[i-1].xr;
      Q->run[i].y=Q->run[i-1].y;
      Q->run[i].f=Q->run[i-1].f;
    }
  } else Q->h++;
  Q->run[qp].xl=xl;
  Q->run[qp].xr=xr;
  Q->run[qp].y=y;
  Q->run[qp].f=f;
  if(!f) Q->n++ ;
}

int ff_next_branch(int xl,int xr,int y,int border_color,int scan_type) {
  int xln=-1;
/*  printf("Next Branch: x=[%d,%d] y=%d   %d %d\n",xl,xr,y,window[usewindow].y,window[usewindow].h);*/
#ifndef NOGRAPHICS
  if((y<window[usewindow].y)||(y>=window[usewindow].y+window[usewindow].h)) return(-1);
  xln=ff_scan_left(xl,y,border_color,scan_type);
  if(xln==-1) {
    xln=ff_scan_right(xl,y,border_color,(scan_type==SCAN_WHILE)?SCAN_UNTIL:SCAN_WHILE);
    if(xln<xr) xln++ ;
    else xln=-1;
  }
#endif
  return xln;
}
int ff_in_queue(FF_QUEUE *Q, int xl, int xr, int y) {
  int  qp;
  if(Q->d==FF_UP) {
    for(qp=Q->h;qp>=0;--qp) {
      if( Q->run[qp].y>y ) break;
      if((Q->run[qp].y==y) && (Q->run[qp].xl==xl) && (Q->run[qp].xr==xr))
	return(1);
    }
  } else {
    for(qp=Q->h;qp>=0;--qp) {
      if(Q->run[qp].y<y) break;
      if((Q->run[qp].y==y) && (Q->run[qp].xl==xl) && (Q->run[qp].xr==xr))
	return(1);
    }
  }
  return(0);
}
#endif
/**** End ******************* Flood Fill ********************************/

#ifdef USE_X11

/* Bereite die Pixmap mit dem Bomben-Icon und der Fensternummer
   vor. */

void fetch_icon_pixmap(WINDOWDEF *w,int nummer) {
  GC gc; XGCValues gc_val;
  char t[10]; sprintf(t,"%2d",nummer);
  w->icon_pixmap=XCreateBitmapFromData(w->display,w->win,(char *)bombe_gross_bits,bombe_gross_width,bombe_gross_height);
  gc = XCreateGC(w->display, w->icon_pixmap, 0, &gc_val);
  XSetForeground(w->display, gc, 0);
  XDrawString(w->display,w->icon_pixmap,gc,9,24,t,strlen(t));
  XFreeGC(w->display,gc);
}
#endif
