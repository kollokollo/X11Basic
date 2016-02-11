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

/* Function Prototypes */

void handle_window(int);
int create_window(char *, char *,unsigned int,unsigned int,unsigned int,unsigned int);
void open_window( int);
void close_window(int);
int rsrc_load(char *);
void put_bitmap(char *adr,int x,int y,int w, int h);
int form_dial(int fo_diflag, int x1,int y1,int w1,int h1,int x2,int y2,int w2,int h2);
void graphics();
void activate();
int form_do(OBJECT *tree);

#ifdef USE_X11
void handle_event(int, XEvent *);
char *imagetoxwd(XImage *,Visual *,XColor *, int *);
#endif
void do_menu_open(int);
void do_menu_close();
void do_menu_edraw();
void do_menu_draw();
/* globale Variablen */

extern int usewindow;
int winbesetzt[MAXWINDOWS];
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
HDC bitcon[MAXWINDOWS];
HBITMAP backbit[MAXWINDOWS];
HANDLE wthandle[MAXWINDOWS]; /* handle of win thread */
HPEN pen[MAXWINDOWS];
HBRUSH brush[MAXWINDOWS];
HFONT font[MAXWINDOWS];
WNDCLASSEX win_class;
HWND win_hwnd[MAXWINDOWS];
extern HANDLE keyevent,buttonevent,motionevent;

#endif
#ifdef WINDOWS
int global_mousex,global_mousey,global_mousek,global_mouses;
int global_color,global_keycode,global_ks,global_eventtype;
#endif
#ifdef USE_SDL
#define ButtonPress 0

#endif
#ifdef FRAMEBUFFER
extern int global_mousex,global_mousey,global_mousek,global_mouses;
extern int global_color,global_bcolor;
#endif
#ifdef USE_X11
Window win[MAXWINDOWS];
Pixmap pix[MAXWINDOWS];
Display *display[MAXWINDOWS];
GC gc[MAXWINDOWS];                      /* Im Gc wird Font, Farbe, Linienart, u.s.w.*/
XSizeHints size_hints[MAXWINDOWS];       /* Hinweise fuer den WIndow-Manager..*/
XWMHints wm_hints[MAXWINDOWS];
XClassHint class_hint[MAXWINDOWS];
XTextProperty win_name[MAXWINDOWS], icon_name[MAXWINDOWS];
Pixmap icon_pixmap[MAXWINDOWS];
#endif


extern int menuflags[];
extern char *menuentry[];
extern char *menutitle[];
extern int menutitleslen[];
extern int menuentryslen[];
extern int menutitlesp[];
extern int menutitlelen[];
extern int menutitleflag[];
extern int menuanztitle;
extern int schubladeff;
extern int schubladenr;
extern int schubladex,schubladey,schubladew,schubladeh;

void do_sizew(int,int,int);
void do_movew(int,int,int);


#define ROOT 0
#define NIL -1
						/* keybd states		*/
#define K_RSHIFT 0x0001
#define K_LSHIFT 0x0002
#define K_CTRL 0x0004
#define K_ALT 0x00008
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
