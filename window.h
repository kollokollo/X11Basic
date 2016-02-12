/* Window.h  (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#ifdef WINDOWS_NATIVE
  #include <windows.h>
  #include <process.h>
#endif

#include "aes.h"


#define WINDOW_DEFAULT_W 640
#define WINDOW_DEFAULT_H 400
#ifdef SAVE_RAM
  #define MAXWINDOWS 8
#else
  #define MAXWINDOWS 16
#endif


/*Hold information about windows*/

/*Window flags*/
#define WIN_CREATED 1
#define WIN_OPENED  2

typedef struct {
  int x,y;
  unsigned int w,h,b,d;
  unsigned int fcolor,bcolor;
  char *title;
  char *info;
  unsigned long flags;
  unsigned short chw,chh,baseline;
#ifdef WINDOWS_NATIVE
  HDC bitcon;
  HBITMAP backbit;
  HANDLE wthandle; /* handle of win thread */
  HPEN pen;
  HBRUSH brush;
  HFONT font;
  HWND win_hwnd;
#elif defined USE_X11
  Window win;
  Pixmap pix;
  Display *display;
  GC gc;                      /* Im Gc wird Font, Farbe, Linienart, u.s.w.*/
  XSizeHints size_hints;       /* Hinweise fuer den WIndow-Manager..*/
  XWMHints wm_hints;
  XClassHint class_hint;
  XTextProperty win_name, icon_name;
  Pixmap icon_pixmap;
#elif defined USE_SDL
  SDL_Surface *display;
#elif defined FRAMEBUFFER
  G_CONTEXT *screen;
#elif defined USE_GEM
  short vdi_handle,aesvdi_handle;
#endif
  
} WINDOWDEF;

extern WINDOWDEF window[];




/* Function Prototypes */

void handle_window(WINDOWDEF *);
int create_window(char *, char *,unsigned int,unsigned int,unsigned int,unsigned int);
void open_window(WINDOWDEF *);
void close_window(WINDOWDEF *);
void put_bitmap(char *adr,int x,int y,int w, int h);
void graphics();
void activate();
#ifndef USE_GEM
short form_dial(unsigned short fo_diflag,short x1,short y1,short w1,short h1,short x2,short y2,short w2,short h2);
short form_do(OBJECT *tree,short startob);
#endif
void do_menu_open(int);
void do_menu_close();
void do_menu_edraw();
void do_menu_draw();
/* globale Variablen */

extern int usewindow;
#ifdef WINDOWS_NATIVE
/* Event-Typen  */
#define Expose 1
#define ButtonPress 2
#define ButtonRelease 4
#define KeyPress 8
#define KeyRelease 16
#define KeyChar 32
#define MotionNotify 64

extern HINSTANCE hInstance;
WNDCLASSEX win_class;
extern HANDLE keyevent,buttonevent,motionevent;

#endif
#ifdef WINDOWS
int global_mousex,global_mousey,global_mousek,global_mouses;
int global_keycode,global_ks,global_eventtype;
#endif
#ifdef USE_SDL
#define ButtonPress 0

#endif
#ifdef FRAMEBUFFER
extern int global_mousex,global_mousey,global_mousek,global_mouses;
#elif defined USE_X11
#elif defined USE_GEM
short work_in[11],work_out[57];
#endif

typedef struct {
  int flags;
  STRING text;
} MENUENTRY;
extern MENUENTRY menuentry[MAXMENUENTRYS];
extern int anzmenuentry;
extern int menutitle[32];
extern int anzmenutitle;

extern int schubladeff;
extern int schubladenr;
extern int schubladex,schubladey,schubladew,schubladeh;

void do_sizew(WINDOWDEF *,int,int);
void do_movew(int,int,int);

char *fileselector(const char *titel, const char *pfad, const char *sel);

#ifndef USE_GEM
char *fsel_input(const char *,const char *,const char *);
#endif
int lsel_input(const char *titel, STRING *strs,int anzfiles,int sel);

#ifdef USE_SDL
#define XEvent union SDL_Event
#define Expose 0
#elif defined USE_GEM
#define XEvent int

#endif




#if defined USE_X11 || defined FRAMEBUFFER || defined USE_SDL
void handle_event(WINDOWDEF *,XEvent *);
#endif

#if defined USE_X11
  #define do_movew(winnr,x,y) if(winnr) XMoveWindow(window[winnr].display, window[winnr].win, x, y)
#elif defined WINDOWS_NATIVE
  #define do_movew(winnr,x,y) if(winnr) MoveWindow(window[winnr].win_hwnd, x, y,640,400,1)
#else
  #define do_movew(winnr,x,y) 
#endif

						/* keybd states		*/
#define K_RSHIFT 0x0001
#define K_LSHIFT 0x0002
#define K_CTRL 0x0004
#ifndef USE_GEM
#define K_ALT 0x00008
#endif
						/* max string length	*/
#define MAX_LEN 81
						/* max depth of search	*/
						/*   or draw for objects*/
#define MAX_DEPTH 8
						/* inside patterns	*/
#define IP_HOLLOW 0
#define IP_1PATT 1
#define IP_2PATT 2
#define IP_3PATT 3
#define IP_4PATT 4
#define IP_5PATT 5
#define IP_6PATT 6
#define IP_SOLID 7
						/* system foreground and*/
						/*   background rules	*/
						/*   but transparent	*/
#define SYS_FG 0x1100

#define WTS_FG 0x11a1				/* window title selected*/
						/*   using pattern 2 &	*/
						/*   replace mode text 	*/
#define WTN_FG 0x1100				/* window title normal	*/
						/* gsx modes		*/
#define MD_REPLACE 1
#define MD_TRANS 2
#define MD_XOR 3
#define MD_ERASE 4
						/* gsx styles		*/
#define FIS_HOLLOW 0
#define FIS_SOLID 1
#define FIS_PATTERN 2
#define FIS_HATCH 3
#define FIS_USER 4
