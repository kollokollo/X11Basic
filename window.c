
/* window.c      Window-Routinen (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#ifdef WINDOWS
  #include "Windows.extension/fnmatch.h"
#else
  #include <fnmatch.h>
#endif

#include "defs.h"
#include "x11basic.h"
#include "graphics.h"
#include "variablen.h"
#include "file.h"

#include "wort_sep.h"
#include "window.h"
#ifdef FRAMEBUFFER
#include "raw_mouse.h"
#endif


/* globale Variablen */
/* GEM-Globals   */

#ifndef NOGRAPHICS
#ifndef FRAMEBUFFER
static char *wname[MAXWINDOWS];
static char *iname[MAXWINDOWS];
#endif
#ifdef WINDOWS_NATIVE
  HANDLE keyevent=INVALID_HANDLE_VALUE; /* handle for win thread event */
  HANDLE buttonevent=INVALID_HANDLE_VALUE; /* handle for win thread event */
  HANDLE motionevent=INVALID_HANDLE_VALUE; /* handle for win thread event */
  HANDLE tsync=INVALID_HANDLE_VALUE; /* handle for win thread event */
#endif

static int create_window2(int nummer,const char *title, const char* info,int x,int y,unsigned int w,unsigned int h);
static int fetch_rootwindow();
#if 0
int create_window(char *title, char* info,unsigned int x,unsigned int y,unsigned int w,unsigned int h) {
  int nummer=0;
  while(winbesetzt[nummer] && nummer<MAXWINDOWS) nummer++;
  if(nummer>=MAXWINDOWS) {
      printf("No more windows !\n");
      return(-2);
  }
#if DEBUG
  printf("createwindow %d\n",nummer);
#endif
  return(create_window2(nummer,title,info,x,y,w,h));
}
#endif
#ifdef WINDOWS_NATIVE
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  RECT cr; /* client area rectangle */
  int nr=-1,i;
  HDC hdc;
  PAINTSTRUCT ps; /* receives information for painting */
  global_mousex=LOWORD(lParam);
  global_mousey=HIWORD(lParam);
  global_mouses=0;
  if (wParam & MK_CONTROL) global_mouses|=4;
  if (wParam & MK_SHIFT) global_mouses|=1;

  switch(msg) {
  case WM_PAINT:
    if (GetUpdateRect(hwnd,&cr,0)) {
      for(i=0;i<MAXWINDOWS;i++) {
        if(winbesetzt[i]) {
          if(win_hwnd[i]==hwnd) {nr=i;break;}
        }
      }
      if(nr==-1) {
        xberror(99,""); /* Window %s does not exist  */
        nr=1;
      }
      BeginPaint(hwnd,&ps);
      hdc=GetDC(hwnd);

      BitBlt(hdc,cr.left,cr.top,cr.right-cr.left,cr.bottom-cr.top,bitcon[nr],cr.left,cr.top,SRCCOPY);
      ReleaseDC(hwnd,hdc);
      EndPaint(hwnd,&ps);
    }
    global_eventtype=Expose;
    break;
        case WM_CHAR:
	global_keycode=wParam;	
	global_eventtype=KeyChar;
	SetEvent(keyevent);
        break;
    case WM_KEYDOWN:
 	global_keycode=wParam;
	global_eventtype=KeyPress;
	SetEvent(keyevent);
        break;
     case WM_KEYUP:
 	global_keycode=wParam;
	global_eventtype=KeyRelease;
	SetEvent(keyevent);
        break;
     case WM_LBUTTONDOWN:
       global_mousek|=1;
       global_eventtype=ButtonPress;
       SetEvent(buttonevent);
       break;
     case WM_RBUTTONDOWN:
       global_mousek|=2;
       global_eventtype=ButtonPress;
       SetEvent(buttonevent);
       break;
     case WM_RBUTTONUP:
	global_mousek&=(~2);
	global_eventtype=ButtonRelease;
	SetEvent(buttonevent);	
	break;
     case WM_LBUTTONUP:
        global_mousek&=(~1);	
	global_eventtype=ButtonRelease;
	SetEvent(buttonevent);	
	break;
	case WM_CLOSE:    DestroyWindow(hwnd); break;
	case WM_DESTROY:  PostQuitMessage(0);  break;
     default:
	return DefWindowProc(hwnd, msg, wParam, lParam);
  }
  return(0);
}



static DWORD winthread(PWORD par) { /* procedure for WIN95-thread */
  MSG Msg;
  HDC hdc;
  RECT cr;
  int nummer=(int)par;

  winbesetzt[nummer]=1;
  cr.left=0;
  cr.top=0;
  cr.right=WINDOW_DEFAULT_W;
  cr.bottom=WINDOW_DEFAULT_H;
  AdjustWindowRect(&cr,WS_VISIBLE|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,FALSE);

	win_hwnd[nummer] = CreateWindow("X11-Basic",NULL,  /* my style */
		WS_VISIBLE|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,/* window style */
		CW_USEDEFAULT, CW_USEDEFAULT, cr.right-cr.left, cr.bottom-cr.top,
		NULL, NULL, hInstance, NULL);

  SetWindowText(win_hwnd[nummer],"X11-Basic on WINDOWS");
  ShowWindow(win_hwnd[nummer], SW_SHOW);
  UpdateWindow(win_hwnd[nummer]);
  SetForegroundWindow(win_hwnd[nummer]);

  hdc=GetDC(win_hwnd[nummer]);
  bitcon[nummer]=CreateCompatibleDC(hdc);
  backbit[nummer]=CreateCompatibleBitmap(hdc,WINDOW_DEFAULT_W,WINDOW_DEFAULT_H);
  SelectObject(bitcon[nummer],backbit[nummer]);
  SelectClipRgn(bitcon[nummer],NULL);
  IntersectClipRect(bitcon[nummer],0,0,WINDOW_DEFAULT_W,WINDOW_DEFAULT_H);
  pen[nummer]=CreatePen(PS_SOLID,1,RGB(255,255,0));
  brush[nummer]=CreateSolidBrush(RGB(0,127,127));

  SelectObject(bitcon[nummer],pen[nummer]);
  SelectObject(bitcon[nummer],brush[nummer]);
  SetTextColor(bitcon[nummer],RGB(255,255,0)) ;
  SetBkMode(bitcon[nummer],TRANSPARENT);
  SetBkColor(bitcon[nummer],RGB(63,63,63)) ;

#if 0
  font[nummer]=CreateFont(13,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
  DEFAULT_QUALITY,FIXED_PITCH | FF_MODERN,NULL);
  if (font[nummer]!=NULL) {
    SelectObject(bitcon[nummer],font[nummer]);
  } else printf("Could not create font !!\n");
#endif
  ReleaseDC(win_hwnd[nummer],hdc);
  SetEvent(tsync);	

  while(GetMessage(&Msg, NULL, 0, 0) > 0) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }	
  DeleteObject(backbit[nummer]);
#if 0
  DeleteObject(backpen[nummer]);
        DeleteObject(backbrush);
#endif
  DeleteDC(bitcon[nummer]);
  winbesetzt[nummer]=0;

  ExitThread(0);
  return(0);
}
#endif  // Windows native

#ifdef USE_SDL
static int init_sdl() {
  static int done=0;
  if(done) return(0);
  if(SDL_Init(SDL_INIT_VIDEO) < 0 ) return -1;
  atexit(SDL_Quit);
  /* Enable Unicode translation */
  SDL_EnableUNICODE( 1 );

  done=1;
  return(0);
}
#endif

static int create_window2(int nummer,const char *title, const char* info,int x,int y,unsigned int w,unsigned int h) {

#ifndef FRAMEBUFFER
  int screen_num;              /* Ein Server kann mehrere Bildschirme haben */
  unsigned long border=0,foreground,background;
  unsigned int d,b;
#endif
#ifdef WINDOWS
    static class_reg=0;
#endif
  if(winbesetzt[nummer]) {
    printf("X11-Basic: Window %d already open !\n",nummer);
#ifdef WINDOWS_NATIVE
    MessageBox(NULL,"X11-Basic: Window already open !" , "Error!",MB_ICONEXCLAMATION | MB_OK);
#endif			
    return(-1);
  } else {
#ifdef WINDOWS_NATIVE
  if (buttonevent==INVALID_HANDLE_VALUE) buttonevent=CreateEvent(NULL,FALSE,FALSE,NULL);
  if (keyevent==INVALID_HANDLE_VALUE) keyevent=CreateEvent(NULL,FALSE,FALSE,NULL);
  if (motionevent==INVALID_HANDLE_VALUE) buttonevent=CreateEvent(NULL,FALSE,FALSE,NULL);
  if (tsync==INVALID_HANDLE_VALUE) tsync=CreateEvent(NULL,FALSE,FALSE,NULL);
  if(!class_reg) {
   	win_class.cbSize		 = sizeof(WNDCLASSEX);
	win_class.style 	      = 0;
	win_class.lpfnWndProc	      = WndProc;
	win_class.cbClsExtra  = 0;
	win_class.cbWndExtra  = 0;
	win_class.hInstance   = hInstance;
	win_class.hIcon 	      = LoadIcon(NULL, IDI_APPLICATION);
	win_class.hCursor	      = LoadCursor(NULL, IDC_ARROW);
	win_class.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	win_class.lpszMenuName  = NULL;
	win_class.lpszClassName = "X11-Basic";
	win_class.hIconSm	      = LoadIcon(NULL, IDI_APPLICATION);
	if(!RegisterClassEx(&win_class)){
		MessageBox(NULL, "Window Registration Failed!", "Error!",
			MB_ICONEXCLAMATION | MB_OK);
		winbesetzt[nummer]=0;
          return(-1);
	}
       class_reg=1;
    }
    ResetEvent(tsync);
  /* create thread to care for window */
  wthandle[nummer]=_beginthread((LPTHREAD_START_ROUTINE)winthread,0,
    nummer);
  if (wthandle[nummer]==NULL) {
    MessageBox(NULL,"X11-Basic: can't create thread for window" , "Error!",
			MB_ICONEXCLAMATION | MB_OK);
			return(-1);
  }
  winbesetzt[nummer]=1;
  WaitForSingleObject(tsync,INFINITE);
#elif defined USE_VGA
  vga_init();
#elif defined FRAMEBUFFER
  Fb_Open();
  #ifndef ANDROID
    Fb_Mouse_Open();
  #endif
#endif

#if defined USE_X11 || defined USE_SDL
    if(wname[nummer]) free(wname[nummer]);
    wname[nummer]=strdup(title);
    if(iname[nummer]) free(iname[nummer]);
    iname[nummer]=strdup(info);
#endif

#ifdef USE_SDL
    init_sdl();
    if(!(display[nummer]=SDL_SetVideoMode(WINDOW_DEFAULT_W, 
       WINDOW_DEFAULT_H, 32,
    // SDL_FULLSCREEN |
       SDL_HWSURFACE|SDL_SRCALPHA))) {
      printf("cannot open SDL surface \n");
      SDL_Quit();
      return(-1);
    }
    SDL_WM_SetCaption(title,info);
#endif
#ifdef USE_X11
  XGCValues gc_val;            /* */
  Window root;
  XTextProperty win_name, icon_name;
  char *agv[1];
  char *wn=wname[nummer];
  char *in=iname[nummer];

  /* Verbindung zum X-Server aufnehmen. */
  if ( (display[nummer] = XOpenDisplay(display_name)) == NULL) {
    printf("Can't Connect XServer on display %s. Aborted\n",
	    XDisplayName(display_name));
    return(-1);
  }
  /* Welchen Bildschirm nehmen ? */
  screen_num = DefaultScreen(display[nummer]);

  /* Fenster Oeffnen */
  background = BlackPixel(display[nummer], DefaultScreen(display[nummer]));
  foreground = WhitePixel(display[nummer], screen_num);
  win[nummer] = XCreateSimpleWindow(display[nummer], RootWindow(display[nummer], screen_num),
			    x, y, w, h, border, foreground, background);
			   // printf("border=%d\n",border);
  XGetGeometry(display[nummer],win[nummer],&root,&x,&y,&w,&h,&b,&d);
  pix[nummer]=XCreatePixmap(display[nummer],win[nummer],w,h,d);
  fetch_icon_pixmap(nummer);

    /* Dem Window-Manager Hinweise geben, was er mit der Groesse des Fensters
     machen soll. */
  size_hints[nummer].flags = PPosition | PSize | PMinSize;
  size_hints[nummer].min_width = 32;
  size_hints[nummer].min_height = 32;
  wm_hints[nummer].flags = StateHint | InputHint | IconPixmapHint;
  wm_hints[nummer].initial_state = NormalState;
  wm_hints[nummer].input = True;
  wm_hints[nummer].icon_pixmap = icon_pixmap[nummer];
  class_hint[nummer].res_name = "X11-Basic";
  class_hint[nummer].res_class = "Graphics";

    if (!XStringListToTextProperty(&wn, 1, &win_name) ||
      !XStringListToTextProperty(&in, 1, &icon_name)) {
    printf("Couldn't set Name of Window or Icon. Aborted\n");
    return(-1);
  }

  /* Man XSetWMProperties, um zu lesen, was passiert ! */
  XSetWMProperties(display[nummer], win[nummer], &win_name, &icon_name, agv, 0,
		   &size_hints[nummer], &wm_hints[nummer], &class_hint[nummer]);

  /* Auswaehlen, welche Events man von dem Fenster bekommen moechte */
  XSelectInput(display[nummer], win[nummer],
               /*ResizeRedirectMask |*/
	       ExposureMask |
	       ButtonPressMask|
	       ButtonReleaseMask|
	       PointerMotionMask |
	       KeyPressMask|
	       KeyReleaseMask);
  gc[nummer] = XCreateGC(display[nummer], win[nummer], 0, &gc_val);

  
  XSetBackground(display[nummer], gc[nummer], background);
  XSetForeground(display[nummer], gc[nummer], background);
/* Clear window*/

  XFillRectangle(display[nummer],pix[nummer],gc[nummer],0,0,w,h);   
//   XClearWindow(display[nummer], win[nummer]); /*sonst hat man evtl. Müll von vorher */
  XSetForeground(display[nummer], gc[nummer], foreground);

#endif
    winbesetzt[nummer]=1; 
  }
  return(nummer);
}

void open_window(int nr) {
 if(winbesetzt[nr]) {
#ifdef WINDOWS_NATIVE
#endif
#ifndef USE_SDL
    XEvent event;
    /* Das Fensterauf den Screen Mappen */

    XMapWindow(display[nr], win[nr]);

#ifdef USE_X11
    XNextEvent(display[nr], &event);
#endif
    handle_event(nr,&event);
#endif
  }
}

void close_window(int nr) {
#ifdef WINDOWS_NATIVE
  if(winbesetzt[nr]) DestroyWindow(win_hwnd[nr]);
#endif
#ifdef USE_VGA
  vga_setmode(TEXT);
#endif
#ifdef USE_SDL
#endif
#ifdef USE_X11
  XEvent event;
      if(winbesetzt[nr]) {
        XUnmapWindow(display[nr], win[nr]);
        XCheckWindowEvent(display[nr],win[nr],ExposureMask, &event);
      }
#endif
}

#if defined USE_X11 || defined FRAMEBUFFER || defined USE_SDL
void handle_event(int nr,XEvent *event) {
  switch (event->type) {

    /* Das Redraw-Event */

#ifdef USE_X11
    case Expose:
      /* if (event.xexpose.count != 0)    break; */
     XCopyArea(display[nr],pix[nr],event->xexpose.window,gc[nr],
     event->xexpose.x,
     event->xexpose.y,
     event->xexpose.width,
     event->xexpose.height,
     event->xexpose.x,
     event->xexpose.y);
      break;
    case GraphicsExpose:
      // printf("GraphicsExpose: %d\n",event->type);
      break;
    case NoExpose:
      // printf("NoExpose: %d\n",event->type);
      break;
    case KeyPress:
      printf("Key Press: %d\n",event->type);break;
    case KeyRelease:
      printf("Key Release: %d\n",event->type);break;
    case ButtonPress:
      printf("Button Press: %d\n",event->type);break;
    case ButtonRelease:
      printf("Button Release: %d\n",event->type);break;
    case CirculateRequest:
      printf("Circulate Request: %d\n",event->type);break;
    case ConfigureRequest:
      printf("Configure Request: %d\n",event->type);break;
    case ResizeRequest:
      printf("Resize Request: %d\n",event->type);break;
    default:
      printf("Window-Event: %d\n",event->type);
      break;
#endif
#ifdef USE_SDL
/* wahrscheinlich muessen wir gar nix tun...*/
  case SDL_QUIT:
    printf("OOps, window close request. What should I do?\n");
    break;
  case SDL_ACTIVEEVENT: 
    if ( event->active.state & SDL_APPACTIVE ) {
        if ( event->active.gain ) {
            printf("App activated\n");
        } else {
            printf("App iconified\n");
        }
    }
#endif
  }
}
#endif


void handle_window(int winnr) {
  if(winbesetzt[winnr]) {

#ifdef WINDOWS_NATIVE
#endif
#ifdef USE_SDL
  SDL_Event event;
  while(SDL_PollEvent(&event)) handle_event(winnr,&event);
  
#endif
#ifdef USE_X11
   XEvent event;
   while(XCheckWindowEvent(display[winnr],win[winnr],
        ExposureMask, &event)) {
     handle_event(winnr,&event);
   }
#endif
  }
}


void graphics() {
#ifdef DEBUG
  printf("graphics:\n");
#endif
  if(winbesetzt[usewindow]) handle_window(usewindow);
  else {
     if(usewindow==0) fetch_rootwindow();
     else {
       create_window2(usewindow,"X11-Basic","X11-Basic",100,10,WINDOW_DEFAULT_W,WINDOW_DEFAULT_H);
       open_window(usewindow);
     }
  }
}

/* fetch_rootwindow() ersetzt create_window2 und open_window fuer das Root-Fenster 
bzw. Fenster Nr 0 (fullscreen) */

static int fetch_rootwindow() {
#ifdef USE_X11
  char *display_name = NULL;   /* NULL: Nimm Argument aus setenv DISPLAY */
  int x,y;
  unsigned int w,h,b,d;
  XGCValues gc_val;            /* */
  Window root;


  /* Verbindung zum X-Server aufnehmen. */
  if ((display[0]=XOpenDisplay(display_name))==NULL) {
    printf("Can't Connect XServer on display %s. Aborted\n",
	    XDisplayName(display_name));
    return(-1);
  }


  win[0] = RootWindow(display[0],DefaultScreen(display[0]));
  XGetGeometry(display[0],win[0],&root,&x,&y,&w,&h,&b,&d);

  pix[0]=XCreatePixmap(display[0],win[0],w,h,d);


  /* Auswaehlen, welche Events man von dem Fenster bekommen moechte */
 /* XSelectInput(display[0], win[0],
	       ExposureMask |
	       ButtonPressMask|
	       ButtonReleaseMask|
	       PointerMotionMask |
	       KeyPressMask|
	       KeyReleaseMask); */
	
  gc[0] = XCreateGC(display[0], win[0], 0, &gc_val);
  winbesetzt[0]=1;
#endif
#ifdef USE_SDL
    init_sdl();
    if(!(display[0]=SDL_SetVideoMode(WINDOW_DEFAULT_W, 
       WINDOW_DEFAULT_H, 32,
       SDL_FULLSCREEN |
       SDL_HWSURFACE|SDL_SRCALPHA))) {
      printf("cannot open SDL surface \n");
      SDL_Quit();
      return(-1);
    }
    winbesetzt[0]=1;
#endif
#if defined WINDOWS_NATIVE || defined FRAMEBUFFER
  create_window2(0,"X11-Basic","X11-Basic",100,10,WINDOW_DEFAULT_W,WINDOW_DEFAULT_H);
  open_window(0);
#endif
  return(0);
}
#ifdef ANDROID
  extern void invalidate_screen();
#endif

void activate() {
#ifdef ANDROID
  invalidate_screen();
#endif
#ifdef WINDOWS_NATIVE
  HDC hdc;
  RECT interior;
  graphics();
  hdc=GetDC(win_hwnd[usewindow]);
  GetClientRect(win_hwnd[usewindow],&interior);
	
  BitBlt(hdc,interior.left,interior.top,interior.right-interior.left,interior.bottom-interior.top,bitcon[usewindow],0,0,SRCCOPY);

  ReleaseDC(win_hwnd[usewindow],hdc);

#endif
#ifdef USE_SDL
 // if(display[usewindow]->flags & SDL_DOUBLEBUF) 
  SDL_Flip(display[usewindow]); 
#endif
#ifdef USE_X11
   Window root;
   XGCValues gc_val;
   int ox,oy;
   unsigned int ow,oh,ob,d;
   int of;
   graphics();
   XGetGeometry(display[usewindow],win[usewindow],&root,&ox,&oy,&ow,&oh,&ob,&d);
   XGetGCValues(display[usewindow], gc[usewindow],GCFunction , &gc_val);
   of=gc_val.function;
   gc_val.function=GXcopy;

   XChangeGC(display[usewindow], gc[usewindow],  GCFunction, &gc_val);

   XFlush(display[usewindow]);
   XCopyArea(display[usewindow],pix[usewindow],win[usewindow],gc[usewindow],0,0,ow,oh,0,0);
   handle_window(usewindow);
   gc_val.function=of;
   XChangeGC(display[usewindow], gc[usewindow],  GCFunction, &gc_val);
#endif
}



/* AES-Nachbildungen (c) Markus Hoffmann     */

#if defined USE_SDL
static void *memrevcpy(char *dest, const char *src, size_t n) {
  unsigned char a,b;
  int i;
  while(n--) {
    a=src[n];
    b=0;
    for(i=0;i<8;i++) {
      b<<=1;
      b|=(a&1);
      a>>=1;
    }   
    dest[n]=b;
  }
  return(dest);
}
#endif

void put_bitmap(char *adr,int x,int y,int w, int h) {
#ifdef USE_X11
  Pixmap bitpix=XCreateBitmapFromData(display[usewindow],win[usewindow],adr,w,h);
  XCopyPlane(display[usewindow],bitpix,pix[usewindow],gc[usewindow],0,0,w,h,x,y,1);
  XFreePixmap(display[usewindow],bitpix);
#endif
#ifdef USE_SDL
  SDL_Surface *data;
  SDL_Surface *image;
  int bpl=(w+1)>>3;
  data=SDL_CreateRGBSurface(SDL_SWSURFACE,w,h,1, 0,0,0,0);
  memrevcpy(data->pixels,adr,bpl*h);
  data->pitch=bpl;
  image=SDL_DisplayFormat(data);
  SDL_FreeSurface(data);
  SDL_Rect a={0,0,image->w,image->h};
  SDL_Rect b={x,y,image->w,image->h};
//  printf("putbitmap: %dx%d %d %d\n",image->w,image->h,bpl,image->pitch);
  SDL_BlitSurface(image, &a,display[usewindow], &b);
  SDL_FreeSurface(image);
#endif
}

#ifdef FRAMEBUFFER
  unsigned char *spix[30];
#endif
  int sgccount=0;

int form_dial( int fo_diflag, int x1,int y1, int w1, int h1, int x2, int y2, int w2, int h2 ) {
#ifdef WINDOWS_NATIVE
  static HDC sgc[30];
  static HBITMAP spix[30];
#endif
#ifdef USE_X11
  static GC *sgc[30];
  static Pixmap *spix[30];
  XGCValues gc_val;
  GC pgc;
  Pixmap ppix;
#endif
#ifdef USE_SDL
  static SDL_Surface *spix[30];
  SDL_Rect a={x2-3,y2-3,w2+8,h2+8};
  SDL_Rect b={0,0,w2+8,h2+8};
#endif
#ifdef DEBUG
  printf("**form_dial:\n");
#endif
  switch(fo_diflag){
  case 0:
#ifdef WINDOWS_NATIVE
  sgc[sgccount]=CreateCompatibleDC(bitcon[usewindow]);
  spix[sgccount]=CreateCompatibleBitmap(bitcon[usewindow],w2+7,h2+7);
  SelectObject(sgc[sgccount],spix[sgccount]);
  BitBlt(sgc[sgccount],0,0,w2+7,h2+7,bitcon[usewindow],x2-3,y2-3,SRCCOPY);
#endif
  /* Erst den Graphic-Kontext retten  */
#ifdef FRAMEBUFFER
   FB_hide_mouse();
   FB_savecontext();
   spix[sgccount]=FB_get_image(x2-3,y2-3,w2+7,h2+7,NULL);
   FB_show_mouse();
#endif
#ifdef USE_X11
    sgc[sgccount]=malloc(sizeof(GC));
    pgc=XCreateGC(display[usewindow], win[usewindow], 0, &gc_val);

    XCopyGC(display[usewindow], gc[usewindow],GCForeground|
                              GCFunction |GCLineWidth |GCLineStyle|
			      GCFont, pgc);
    memcpy(sgc[sgccount],&pgc,sizeof(GC));		

    gc_val.function=GXcopy;
    XChangeGC(display[usewindow], gc[usewindow],  GCFunction, &gc_val);

    /* Hintergrund retten  */
    ppix=XCreatePixmap(display[usewindow],win[usewindow],w2+8,h2+8,depth);
    XCopyArea(display[usewindow], pix[usewindow],ppix,gc[usewindow],x2-3,y2-3,w2+7,h2+7,0,0);
    spix[sgccount]=malloc(sizeof(Pixmap));
    memcpy(spix[sgccount],&ppix,sizeof(Pixmap));
#endif
#ifdef USE_SDL
  spix[sgccount]=SDL_CreateRGBSurface(SDL_SWSURFACE,a.w,a.h,32,0,0,0,0);
  SDL_BlitSurface(display[usewindow], &a,spix[sgccount], &b);
#endif
    sgccount++;
   break;
   case 3:
   /* Hintergrund restaurieren  */
   sgccount--;
#ifdef WINDOWS_NATIVE
    BitBlt(bitcon[usewindow],x2-3,y2-3,w2+7,h2+7,sgc[sgccount],0,0,SRCCOPY);
    DeleteObject(spix[sgccount]);
    DeleteDC(sgc[sgccount]);
    activate();
#endif
#ifdef USE_SDL
    SDL_BlitSurface(spix[sgccount], &b,display[usewindow], &a);
    SDL_FreeSurface(spix[sgccount]);
    activate();
#else
#ifdef FRAMEBUFFER
    FB_hide_mouse();
    FB_put_image(spix[sgccount],x2-3,y2-3);
    FB_restorecontext();
    FB_show_mouse();
#endif
#ifdef USE_X11
    XCopyArea(display[usewindow], *(spix[sgccount]),pix[usewindow],gc[usewindow],0,0,w2+7,h2+7,x2-3,y2-3);
    XFreePixmap(display[usewindow],*(spix[sgccount]));
    XCopyGC(display[usewindow],*sgc[sgccount],GCForeground| GCFunction |GCLineWidth |GCLineStyle| GCFont, gc[usewindow]);
    XFreeGC(display[usewindow],*sgc[sgccount]);
    free(sgc[sgccount]);
#endif
    activate();
    free(spix[sgccount]);
#endif
    break;
  default:
    return(-1);
  }
  return(0);
}





int form_do(OBJECT *tree) {
#ifdef WINDOWS_NATIVE
  HANDLE evn[3];
#endif
#if defined FRAMEBUFFER || defined USE_X11|| defined USE_SDL
  XEvent event;
#endif
#ifdef USE_SDL
  int e;
#endif
  int exitf=0,bpress=0;
  int sbut=-1,edob=-1,idx;
#ifdef DEBUG
  printf("**form_do:\n");
#endif
#if defined FRAMEBUFFER
  FB_clear_events();
  FB_mouse_events(1);
#endif
    /* erstes editierbare Objekt finden */

  edob=finded(tree,0,0);
 /* objc_draw(tree,0,-1,0,0); */
	
  /* Cursor plazieren */
	
  if(edob>=0) {
    ((TEDINFO *)tree[edob].ob_spec)->te_junk1=strlen((char *)((TEDINFO *)tree[edob].ob_spec)->te_ptext);
    draw_edcursor(tree,edob);
  }	

  /* Auf Tasten/Maus reagieren */
  activate();
#ifdef WINDOWS_NATIVE
  evn[0]=keyevent;
  evn[1]=buttonevent;
  ResetEvent(evn[0]);
  ResetEvent(evn[1]);
#endif
  while(exitf==0) {
#ifdef WINDOWS_NATIVE
    WaitForMultipleObjects(2,evn,FALSE,INFINITE);
    switch (global_eventtype) {
#endif
#if defined USE_X11 || defined FRAMEBUFFER
    XWindowEvent(display[usewindow], win[usewindow],KeyPressMask |KeyReleaseMask|ExposureMask |ButtonReleaseMask| ButtonPressMask, &event);
#endif
#if defined USE_SDL
    e=SDL_WaitEvent(&event);
    if(e==0) return;
    while(event.type!=SDL_MOUSEBUTTONDOWN && event.type!=SDL_KEYDOWN &&
        event.type!=SDL_MOUSEBUTTONUP) { 
     handle_event(usewindow,&event);
     e=SDL_WaitEvent(&event);
     if(e==0) return;
    }
#endif
#if defined USE_X11 || defined FRAMEBUFFER || defined USE_SDL
    switch (event.type) {
#ifndef FRAMEBUFFER
      char buf[4];
#endif
#endif
#ifdef USE_X11
      XComposeStatus status;
      KeySym ks;
    /* Das Redraw-Event */
    case Expose:
      XCopyArea(display[usewindow],pix[usewindow],win[usewindow],gc[usewindow],
          event.xexpose.x,event.xexpose.y,
          event.xexpose.width,event.xexpose.height,
          event.xexpose.x,event.xexpose.y);
      break;
#endif
#if defined USE_X11 || defined FRAMEBUFFER
    /* Bei Mouse-Taste: */
    case ButtonPress:
      if(event.xbutton.button==1) {
        sbut=objc_find(tree,event.xbutton.x,event.xbutton.y);
#endif
#ifdef USE_SDL
    case SDL_MOUSEBUTTONDOWN:
      if(event.button.button==1) {
         sbut=objc_find(tree,event.button.x,event.button.y);
#endif
#ifdef WINDOWS_NATIVE
    case ButtonPress:
      if(global_mousek==1) {
        sbut=objc_find(tree,global_mousex,global_mousey);
#endif
        if(sbut!=-1) {
	if((tree[sbut].ob_flags & SELECTABLE) && !(tree[sbut].ob_state & DISABLED)) {
          if(tree[sbut].ob_flags & RBUTTON) {
            idx=rootob(tree,sbut);
            if(idx>=0) {
	      int start=tree[idx].ob_head;
	      int stop=tree[idx].ob_tail;
	      if(start>=0) {
		idx=start;
		while(1) {
		  if(tree[idx].ob_flags & RBUTTON) tree[idx].ob_state=tree[idx].ob_state & (~SELECTED);
		  if(idx==stop) break;
	          idx=tree[idx].ob_next;
		}
	      }
            }
          }
	
	    tree[sbut].ob_state^=SELECTED;
	    objc_draw(tree,0,-1,0,0);
	    if(edob>=0) draw_edcursor(tree,edob); 
            activate();
	    if(tree[sbut].ob_flags & EXIT) bpress=1;
	  }
	  if(tree[sbut].ob_flags & TOUCHEXIT) exitf=1;
	  if(tree[sbut].ob_flags & EDITABLE) {
	    edob=sbut;
	    ((TEDINFO *)tree[edob].ob_spec)->te_junk1=strlen((char *)((TEDINFO *)tree[edob].ob_spec)->te_ptext);
	    objc_draw(tree,0,-1,0,0);
	    draw_edcursor(tree,edob);
	    activate();
	  }
	}
      } else bpress=1;
      break;
#ifdef USE_SDL
    case SDL_MOUSEBUTTONUP:
#else
    case ButtonRelease:
#endif
      if(bpress) exitf=1;
      break;
#ifdef WINDOWS_NATIVE
    case KeyChar:   /* Return gedrueckt ? */
    printf("ks=%04x  \n",global_keycode);
    if((global_keycode & 255)==13) {                /* RETURN  */
#endif
#ifdef USE_SDL
    case SDL_KEYDOWN:   /* Return gedrueckt ? */
#if DEBUG 
      printf("Keydown: %d\n",event.key.keysym.sym);
      /* Print the hardware scancode first */
      printf( "Scancode: 0x%02X", event.key.keysym.scancode );
      /* Print the name of the key */
      printf( ", Name: %s", SDL_GetKeyName( event.key.keysym.sym ) );
      printf(", Unicode: " );
      if( event.key.keysym.unicode < 0x80 && event.key.keysym.unicode > 0 ){
        printf( "%c (0x%04X)", (char)event.key.keysym.unicode,
             event.key.keysym.unicode );
      } else {
        printf( "? (0x%04X)", event.key.keysym.unicode );
      }
      printf("\n");
#endif     
      if(event.key.keysym.sym==SDLK_RETURN) {                /* RETURN  */
#endif
#ifdef FRAMEBUFFER
    case KeyPress:   /* Return gedrueckt ? */
      if((event.xkey.ks & 255)==13 || (event.xkey.ks & 255)==10) {                /* RETURN  */
#endif
#ifdef USE_X11
    case KeyPress:   /* Return gedrueckt ? */
      XLookupString((XKeyEvent *)&event,buf,sizeof(buf),&ks,&status);

      if((ks & 255)==13) {                /* RETURN  */  
#endif

        int idx=0;
	while(1) {
	  if(tree[idx].ob_flags & DEFAULT) {
	    tree[idx].ob_state^=SELECTED;
	    sbut=idx;
	    objc_draw(tree,0,-1,0,0); activate();
	    if(tree[idx].ob_flags & EXIT) bpress=1;
          }
	  if(tree[idx].ob_flags & LASTOB) break;
	  idx++;
	}
      } else if(edob>=0) { /*Ist ein ed-object vorhanden ?*/
         int i;
         TEDINFO *ted=(TEDINFO *)(tree[edob].ob_spec);

#ifdef FRAMEBUFFER
         if(HIBYTE(event.xkey.ks)) {
#elif defined USE_X11
         if(HIBYTE(ks)) {
#endif
#ifdef WINDOWS_NATIVE
           if((global_keycode & 255)==8) {          /* BSP */
#endif
#ifdef FRAMEBUFFER
	   if(event.xkey.ks==0xff08) {                  /* BACKSPACE   */
#endif
#ifdef USE_X11
	   if(ks==0xff08) {                  /* BACKSPACE   */
#endif
#ifdef USE_SDL
           if(event.key.keysym.sym==SDLK_BACKSPACE) {    /* BACKSPACE */
#endif

	     if(ted->te_junk1>0) {
	       int len=strlen((char *)ted->te_ptext);
	       i=ted->te_junk1--;
	       while(i<len) {
	         ((char *)(ted->te_ptext))[i-1]=((char *)(ted->te_ptext))[i];
		 i++;
	       }
	       ((char *)ted->te_ptext)[i-1]=0;
	       objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
	     }

#ifdef FRAMEBUFFER
	   } else if(event.xkey.ks==0xff51) { /* LEFT */
#endif
#ifdef USE_X11
	   } else if(ks==0xff51) { /* LEFT */
#endif
#ifdef USE_SDL
           } else if(event.key.keysym.sym==SDLK_LEFT) {   /* LEFT */
#endif
	     if(ted->te_junk1>0) ted->te_junk1--;
	     objc_draw(tree,0,-1,0,0);
	     draw_edcursor(tree,edob);activate();
#ifdef FRAMEBUFFER
	   } else if(event.xkey.ks==0xff53) { /* RIGHT */
#elif defined USE_X11
	   } else if(ks==0xff53) { /* RIGHT */
#elif defined USE_SDL
           } else if(event.key.keysym.sym==SDLK_RIGHT) {   /* RIGHT */
#endif  


	     int len=strlen((char *)ted->te_ptext);
	     if(ted->te_junk1<len && ((char *)ted->te_ptext)[ted->te_junk1]) ted->te_junk1++;
             objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();

#ifdef WINDOWS_NATIVE
           } else if((global_keycode & 255)==9) {          /* TAB */
#endif
#ifdef FRAMEBUFFER
	   } else if(event.xkey.ks==0xff09) {          /* TAB */
#endif
#ifdef USE_X11
	   } else if(ks==0xff09) {          /* TAB */
#endif
#ifdef USE_SDL
           } else if(event.key.keysym.sym==SDLK_TAB) {   /* TAB */
#endif

	     /* Suche naechstes ED-Feld oder wieder das erste */
	     int cp=ted->te_junk1;
	     i=finded(tree,edob,1);
	     if(i<0) edob=finded(tree,0,0);
	     else edob=i;
	     ted=(TEDINFO *)(tree[edob].ob_spec);
	     ted->te_junk1=min(cp,strlen((char *)ted->te_ptext));
	     objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();

#ifdef FRAMEBUFFER
	   } else if(event.xkey.ks==0xff52) {
#elif defined USE_X11
	   } else if(ks==0xff52) {
#elif defined USE_SDL
           } else if(event.key.keysym.sym==SDLK_PAGEUP) {   
#endif
	   /* Suche vorangehendes ED-Feld */
	     int cp=ted->te_junk1;
	     i=finded(tree,edob,-1);
	     if(i>=0) {edob=i;ted=(TEDINFO *)(tree[edob].ob_spec);
	     ted->te_junk1=min(cp,strlen((char *)ted->te_ptext));
	     objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();}
#ifdef FRAMEBUFFER
	   } else if(event.xkey.ks==0xff54) { /* Page down */
#elif defined USE_X11
	   } else if(ks==0xff54) { /* Page down */
#elif defined USE_SDL
           } else if(event.key.keysym.sym==SDLK_PAGEDOWN) {   
#endif
	     int cp=ted->te_junk1;
	     /* Suche naechstes ED-Feld  */
	     i=finded(tree,edob,1);
	     if(i>=0) {
	       edob=i;
	       ted=(TEDINFO *)(tree[edob].ob_spec);
	       ted->te_junk1=min(cp,strlen((char *)ted->te_ptext));
	       objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);
	       activate();
	     }
#ifdef WINDOWS_NATIVE
           } else if((global_keycode & 255)==0x1b) {   /* ESC  */
#endif
#ifdef FRAMEBUFFER
	   } else if(event.xkey.ks==0xff1b) {   /* ESC  */
#endif
#ifdef USE_X11
	   } else if(ks==0xff1b) {   /* ESC  */
#endif
#ifdef USE_SDL
           } else if(event.key.keysym.sym==SDLK_ESCAPE) {    /* ESC  */
#endif


	   ((char *)ted->te_ptext)[0]=0;
	   ted->te_junk1=0;
	   objc_draw(tree,0,-1,0,0);
	   draw_edcursor(tree,edob);activate();


#ifdef FRAMEBUFFER
	   } else printf("Key: %x\n",event.xkey.ks);
#endif
#ifdef USE_X11
	   } else printf("Key: %x\n",(unsigned int)ks);
#endif
#ifdef USE_SDL
           } else {
      printf("Keydown: %d\n",event.key.keysym.sym);
      /* Print the hardware scancode first */
      printf( "Scancode: 0x%02X", event.key.keysym.scancode );
      /* Print the name of the key */
      printf( ", Name: %s", SDL_GetKeyName( event.key.keysym.sym ) );
      printf(", Unicode: " );
      if( event.key.keysym.unicode < 0x80 && event.key.keysym.unicode > 0 ) {
        printf( "%c (0x%04X)", (char)event.key.keysym.unicode,
             event.key.keysym.unicode );
      } else {
        printf( "? (0x%04X)", event.key.keysym.unicode );
      }
      printf("\n");
      if(event.key.keysym.unicode < 0x80 && event.key.keysym.unicode > 0 ) {
#endif
#if defined USE_X11 || defined FRAMEBUFFER
	} else { /*HIBYTE*/
#endif
	  i=ted->te_txtlen-1;
	  while(i>ted->te_junk1) {((char *)ted->te_ptext)[i]=((char *)ted->te_ptext)[i-1];i--;}
	
	  if(ted->te_junk1<ted->te_txtlen) {
#ifdef WINDOWS_NATIVE
            ((char *)ted->te_ptext)[ted->te_junk1]=(char)global_keycode;
#endif
#ifdef FRAMEBUFFER
	    ((char *)ted->te_ptext)[ted->te_junk1]=(char)(event.xkey.ks&0xff);
#endif
#ifdef USE_X11
	    ((char *)ted->te_ptext)[ted->te_junk1]=(char)ks;
#endif
#ifdef USE_SDL
            ((char *)ted->te_ptext)[ted->te_junk1]=(char)event.key.keysym.unicode;
#endif
	    ted->te_junk1++;
	  }	
	  objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
#ifdef USE_SDL
      } /* unicode*/ 
#endif
      } /* if HIBYTE*/
      } /* if edob*/
      break;
#ifdef USE_SDL
    case SDL_KEYUP:
#else
    case KeyRelease:
#endif
      if(bpress) exitf=1;
      break;
#ifdef WINDOWS_NATIVE
    default:
      ResetEvent(evn[0]);
      ResetEvent(evn[1]);
      break;
#endif
    } /* switch */
  } /* while */
#ifdef FRAMEBUFFER
  FB_mouse_events(0);
#endif
  return(sbut);
}


int do_menu_select() {
  int nr,i,textx,sel=-1;
  int root_x_return, root_y_return,win_x_return, win_y_return;
  unsigned int mask_return;
#ifdef USE_X11
  Window root_return,child_return;
#endif
  graphics();
#ifdef WINDOWS_NATIVE
  win_x_return=global_mousex;
  win_y_return=global_mousey;
  mask_return=global_mousek;
#endif
#if defined USE_X11 || defined FRAMEBUFFER
  XQueryPointer(display[usewindow], win[usewindow], &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);
#endif
#ifdef USE_SDL
  mask_return=SDL_BUTTON(SDL_GetMouseState(&win_x_return,&win_y_return))&0xffff;
#endif
  for(i=0;i<menuanztitle-1;i++) {
    if(menutitleflag[i]) sel=i;
//    printf("%d : %03x\n",i,menutitleflag[i]);
  }

  if(win_y_return<=sbox.y+chh && win_y_return>=0) { /* Maus in der Titelleiste ? */
  /* Maus auf gueltigem Titel ? */
    textx=chw;nr=-1;
    for(i=0;i<menuanztitle-1;i++) {
      if(win_x_return>textx && win_x_return<textx+chw*(menutitleslen[i]+2)) nr=i;
      textx+=chw*(menutitleslen[i]+2);
    }
    if(nr>-1) {
     /* Wenn titel noch nicht selektiert, dann */
      if((menutitleflag[nr] & SELECTED)==0) {
        /* unselektiere alle Titel */
        for(i=0;i<menuanztitle-1;i++) menutitleflag[i]=NORMAL;
        /* schliesse alle Schubladen */
         do_menu_close();
        /* selektiere Titel */
         menutitleflag[nr]|=SELECTED;
	 sel=nr;
         do_menu_draw();
        /* oeffne schublade */
         do_menu_open(nr);
      }
    }
  }

  /* Schublade geoeffnet ? */
  if(sel>-1 && schubladeff) {
  /* Maus auf gueltigem Eintrag, der selektierbar ist  ?*/
    if(win_x_return>schubladex && win_x_return<schubladex+schubladew &&
    win_y_return>schubladey && win_y_return<schubladey+schubladeh) {
      nr=-1;
      for(i=0;i<menutitlelen[schubladenr];i++) {
        if(win_y_return>schubladey+i*chh && win_y_return<schubladey+(i+1)*chh) nr=i;
      }
      if(nr>-1) {
        if(!(menuflags[menutitlesp[schubladenr]+nr] & SELECTED) && !(menuflags[menutitlesp[schubladenr]+nr] & DISABLED)) {
        /* unselektiere alles  */
	  for(i=0;i<menutitlelen[schubladenr];i++) {
            menuflags[menutitlesp[schubladenr]+i]&=~SELECTED;
          }

        /* selektiere Eintrag */
	  menuflags[menutitlesp[schubladenr]+nr]|=SELECTED;
          do_menu_edraw();
        }
        /* Taste Gedrueckt ? */
        if(mask_return && !(menuflags[menutitlesp[schubladenr]+nr] & DISABLED)) {
          /* Menus schliessen, Titel deselektieren und mit nr zurueck */
          for(i=0;i<menuanztitle-1;i++) {
            menutitleflag[i]=0;
          }
          for(i=0;i<menutitlelen[schubladenr];i++) {
            menuflags[menutitlesp[schubladenr]+i]&=~SELECTED;
          }
          do_menu_close();
          do_menu_draw();
          return(menutitlesp[schubladenr]+nr);
        }
      }
    } else {
    /* Unselektiere alles  */
      for(i=0;i<menutitlelen[schubladenr];i++) {
        menuflags[menutitlesp[schubladenr]+i]&=~SELECTED;
      }
    }
  }
  /* Taste Gedrueckt, dann schliesse Menu deselektiere Titel */
  if(sel>-1 && mask_return) {
    for(i=0;i<menuanztitle-1;i++) menutitleflag[i]=0;
    do_menu_close();
    do_menu_draw();
  }
  return(-1);
}
#ifdef WINDOWS_NATIVE
HBITMAP schubladepix;
HDC schubladedc;
#endif
#ifdef USE_X11
Pixmap schubladepix;
#endif
#ifdef USE_SDL
SDL_Surface *schubladepix;
#endif
#ifdef FRAMEBUFFER
unsigned char *schubladepix;
#endif
int schubladeff=0;
int schubladenr;
int schubladex,schubladey,schubladew,schubladeh;

void do_menu_open(int nr) {
  int i,textx;
  if(schubladeff) do_menu_close();
  textx=chw;
  for(i=0;i<nr;i++) {
    textx+=chw*(menutitleslen[i]+2);
  }
  schubladex=sbox.x+textx-2;
  schubladey=sbox.y+chh+1;
  schubladew=10;
  for(i=0;i<menutitlelen[nr];i++) {
    schubladew=max(schubladew,menuentryslen[menutitlesp[nr]+i]*chw);
  }
  schubladeh=chh*menutitlelen[nr]+2;
     /* Hintergrund retten  */
#ifdef WINDOWS_NATIVE
  schubladedc=CreateCompatibleDC(bitcon[usewindow]);
  schubladepix=CreateCompatibleBitmap(bitcon[usewindow],schubladew,schubladeh);
  SelectObject(schubladedc,schubladepix);
  BitBlt(schubladedc,0,0,schubladew,schubladeh,bitcon[usewindow],schubladex,schubladey,SRCCOPY);
#endif
#ifdef USE_X11
  schubladepix=XCreatePixmap(display[usewindow],win[usewindow],schubladew,schubladeh,depth);
  XCopyArea(display[usewindow], pix[usewindow],schubladepix,gc[usewindow],schubladex,schubladey,schubladew,schubladeh,0,0);
#endif
#ifdef USE_SDL
  SDL_Rect a={schubladex,schubladey,schubladew,schubladeh};
  SDL_Rect b={0,0,schubladew,schubladeh};
  schubladepix=SDL_CreateRGBSurface(SDL_SWSURFACE,a.w,a.h,32,0,0,0,0);
  SDL_BlitSurface(display[usewindow], &a,schubladepix, &b);
#endif
  #ifdef FRAMEBUFFER
     FB_hide_mouse();
     schubladepix=FB_get_image(schubladex,schubladey,schubladew,schubladeh,NULL);
  #endif
  schubladeff=1;
  schubladenr=nr;
  do_menu_edraw();
  #ifdef FRAMEBUFFER
     FB_show_mouse();
  #endif
}
void do_menu_edraw() {
  int i; 
  #ifdef FRAMEBUFFER
    FB_savecontext();
  #endif
    SetForeground(gem_colors[WHITE]);
#if defined USE_X11 || defined FRAMEBUFFER
    XSetLineAttributes(display[usewindow], gc[usewindow], 1, 0,0,0);
#endif
    FillRectangle(schubladex,schubladey,schubladew-1,schubladeh-1);
    for(i=0;i<menutitlelen[schubladenr];i++) {
      if(menuflags[menutitlesp[schubladenr]+i] & SELECTED) {SetForeground(gem_colors[BLACK]);}
      else {SetForeground(gem_colors[WHITE]);}
      FillRectangle(schubladex,schubladey+i*chh,schubladew,chh);
      if(menuflags[menutitlesp[schubladenr]+i] & SELECTED) {SetForeground(gem_colors[WHITE]);SetBackground(gem_colors[BLACK]);}
      else if(menuflags[menutitlesp[schubladenr]+i] & DISABLED) {SetForeground(gem_colors[LWHITE]);SetBackground(gem_colors[WHITE]);}
      else {SetForeground(gem_colors[BLACK]);SetBackground(gem_colors[WHITE]);}
      DrawString(schubladex,schubladey+baseline+chh*i,menuentry[menutitlesp[schubladenr]+i],menuentryslen[menutitlesp[schubladenr]+i]);
      if(menuflags[menutitlesp[schubladenr]+i] & CHECKED) {
        DrawLine(schubladex+5,schubladey+chh-3+chh*i,schubladex+2,schubladey+chh-8+chh*i);
        DrawLine(schubladex+5,schubladey+chh-3+chh*i,schubladex+chw,schubladey+chh*i);
      }
    }
    SetForeground(gem_colors[BLACK]);
    DrawRectangle(schubladex,schubladey,schubladew-1,schubladeh-1);
 #ifdef FRAMEBUFFER 
   FB_restorecontext();
 #endif
  activate();

}
void do_menu_close() {
  if(schubladeff) {
#ifdef WINDOWS_NATIVE
    BitBlt(bitcon[usewindow],schubladex,schubladey,schubladew,schubladeh,schubladedc,0,0,SRCCOPY);
    DeleteObject(schubladepix);
    DeleteDC(schubladedc);
#endif
#ifdef USE_X11
    XCopyArea(display[usewindow], schubladepix,pix[usewindow],gc[usewindow],0,0,schubladew,schubladeh
    ,schubladex,schubladey);
    XFreePixmap(display[usewindow],schubladepix);
#endif
#ifdef USE_SDL
  SDL_Rect a={schubladex,schubladey,schubladew,schubladeh};
  SDL_Rect b={0,0,schubladew,schubladeh};
    SDL_BlitSurface(schubladepix, &b,display[usewindow], &a);
    SDL_FreeSurface(schubladepix);
#endif
 #ifdef FRAMEBUFFER 
   FB_hide_mouse();
   FB_put_image(schubladepix,schubladex,schubladey);
   FB_show_mouse();
 #endif
    schubladeff=0;
  }
}
void do_menu_draw() {
  int i,textx;
  graphics();
  gem_init();
 #ifdef FRAMEBUFFER
 FB_savecontext();
 FB_hide_mouse();
 #endif
  SetForeground(gem_colors[WHITE]);
#if defined USE_X11 || defined FRAMEBUFFER
    XSetLineAttributes(display[usewindow], gc[usewindow], 1, 0,0,0);
#endif
    FillRectangle(sbox.x,sbox.y,sbox.w,chh);
    textx=chw;
    for(i=0;i<menuanztitle-1;i++) {
      if(menutitleflag[i] & SELECTED) {SetForeground(gem_colors[BLACK]);}
      else {SetForeground(gem_colors[WHITE]);}
      FillRectangle(sbox.x+textx,sbox.y,chw*(2+menutitleslen[i]),chh);
      if(menutitleflag[i] & SELECTED) {SetForeground(gem_colors[WHITE]);SetBackground(gem_colors[BLACK]);}
      else {SetForeground(gem_colors[BLACK]);SetBackground(gem_colors[WHITE]);}
      DrawString(sbox.x+textx+chw,sbox.y+baseline,menutitle[i],menutitleslen[i]);
      textx+=chw*(menutitleslen[i]+2);
    }
    SetForeground(gem_colors[BLACK]);
    DrawLine(sbox.x,sbox.y+chh,sbox.x+sbox.w,sbox.y+chh);
 #ifdef FRAMEBUFFER
 FB_restorecontext();
 FB_show_mouse();
 #endif
  activate();
}


/* Fileselector-Routine. Dem GEM nachempfunden.
(c) Markus Hoffmann   1998               */



#define FT_NORMAL    0
#define FT_DIR    0x10
#define FT_LINK   0x20


typedef struct fileinfo {
    char name[128];       /* The file name. */
    int typ;
    struct stat dstat;
} FINFO;
#define ANZSHOW 13
#define FWW 34


#ifdef WINDOWS
  #define dir_bytes(dir,anzfiles) 0
#else
static int dir_bytes(FINFO *dir,int anzfiles) {
  int byt=0,i;
  if(anzfiles){
    for(i=0;i<anzfiles;i++) byt+=dir[i].dstat.st_blocks;
    byt*=512;
  }
  return(byt);
}
#endif
static void make_filelist(OBJECT *objects,FINFO *filenamen,int *filenamensel,int anzfiles,int showstart){
  int i,j;
  TEDINFO *ted;
  for(i=0;i<ANZSHOW;i++) {
    j=showstart+i;
    ted=(TEDINFO *)objects[18+2*i].ob_spec;
    objects[18+2*i+1].ob_spec&=0x00ffffff;
      if(j<anzfiles) {
        if(filenamensel[j]==1) objects[18+2*i].ob_state=SELECTED;
        else objects[18+2*i].ob_state=NORMAL;
        ted->te_ptext=(LONG)filenamen[j].name;
	objects[18+2*i].ob_flags=SELECTABLE|TOUCHEXIT|RBUTTON;
	if(filenamen[j].typ & FT_DIR) objects[18+2*i+1].ob_spec|=0x44000000;
	else objects[18+2*i+1].ob_spec|=0x20000000;
      } else {
        ted->te_ptext=(LONG)"";
        objects[18+2*i].ob_state=NORMAL;
        objects[18+2*i].ob_flags=NONE;
        objects[18+2*i+1].ob_spec|=0x20000000;
    }
  }
}

static int compare_dirs(FINFO *a,FINFO *b) {
  if(a->typ==FT_DIR && b->typ!=FT_DIR) return(-1);
  if(a->typ!=FT_DIR && b->typ==FT_DIR) return(1);
  return(strcmp(a->name,b->name));
}

static inline void sort_dir(FINFO *fileinfos,int anz) {
  qsort((void *)fileinfos,anz,sizeof(FINFO),(int(*)(const void *,const void*))compare_dirs);
}
#define MAXANZFILES 512
static int read_dir(FINFO *fileinfos,int maxentries,const char *pfad,const char *mask) {
  DIR *dp;
  struct dirent *ep;
  int anzfiles=0;
  char filename[128];
#ifdef DEBUG
  printf("Read_dir: %s/%s\n",pfad,mask);
#endif
  sprintf(filename,"%s/",pfad);
  dp=opendir(filename);
  if(dp!=NULL) {

    while((ep=readdir(dp)) && (anzfiles<maxentries)) {
      /* ist es Directory ? */
      sprintf(filename,"%s/%s",pfad,ep->d_name);
      if(stat(filename, &fileinfos[anzfiles].dstat)==0) {
        if(S_ISDIR(fileinfos[anzfiles].dstat.st_mode)) {
	  if(strcmp(ep->d_name,".") && strcmp(ep->d_name,"..")) {
	    strncpy(fileinfos[anzfiles].name,ep->d_name,128);
	    fileinfos[anzfiles++].typ=FT_DIR;
	  }
	  #ifdef DEBUG
	  printf("**%s %d\n",ep->d_name,(int)ep->d_type);
	  #endif
        } else {
	#ifdef DEBUG
	  printf("%s ",ep->d_name);
	  #endif
	  if (fnmatch(mask,ep->d_name,FNM_PERIOD)==0) {
	  #ifdef DEBUG
	    printf("****");
	  #endif
	    strncpy(fileinfos[anzfiles].name,ep->d_name,128);
	    fileinfos[anzfiles++].typ=FT_NORMAL;
	  }
	  #ifdef DEBUG
	  printf("\n");
	  #endif
	}
      }
    }
    closedir(dp);
  } else printf("Could not open directory %s.\n",pfad);
  return(anzfiles);
}

static void make_scaler(OBJECT *objects,int anzfiles,int showstart){
  float hoehe,xpos;

  if(anzfiles>ANZSHOW){
    hoehe=min(1.0,max(0.1,(float)ANZSHOW/anzfiles));
    xpos=(float)showstart/(anzfiles-ANZSHOW);
  } else {
    hoehe=1;
    xpos=0;
  }
  objects[16].ob_height=(int)(hoehe*objects[15].ob_height);
  objects[16].ob_y=(int)(xpos*(objects[15].ob_height-objects[16].ob_height));
}

char *fsel_input(const char *titel, const char *pfad, const char *sel) {
  char btitel[128],mask[128],dpfad[128];
  char feld1[128],feld2[128];
  char *ergebnis;
  int i,k;
  int sbut=-1;
#if defined USE_X11 || defined FRAMEBUFFER
  XEvent event;
#endif
#ifdef USE_SDL
  SDL_Event event;
  int e;
#endif
  FINFO filenamen[MAXANZFILES];

  int filenamensel[MAXANZFILES];

  int anzfiles,showstart=0;

  TEDINFO tedinfo[4+ANZSHOW]={
  {(LONG)btitel,(LONG)"",(LONG)"",FONT_BIGIBM,0,TE_CNTR,0x1200,0,0,0,0},
  {(LONG)mask,(LONG)"",(LONG)"",FONT_IBM,0,TE_CNTR,0x113a,0,2,0,FWW},
  {(LONG)feld1,(LONG)"__________________________________________________",(LONG)"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",FONT_IBM,0,TE_LEFT,0x1100,0,0,128,50},
  {(LONG)feld2,(LONG)"____________________",(LONG)"XXXXXXXXXXXXXXXXXXXX",FONT_IBM,0,TE_LEFT,0x1100,0,0,128,20}
  };
 // int anztedinfo=sizeof(tedinfo)/sizeof(TEDINFO);
  OBJECT objects[18+2*ANZSHOW]={
/* 0*/  {-1,1,14,G_BOX, NONE, OUTLINED, 0x00021100, 0,0,54,23},
#ifdef FRAMEBUFFER
/* 1*/  {2,-1,-1,G_BUTTON, SELECTABLE|DEFAULT|EXIT,NORMAL ,(LONG)"OK", 42,15,9,3},
#else
/* 1*/  {2,-1,-1,G_BUTTON, SELECTABLE|DEFAULT|EXIT,NORMAL ,(LONG)"OK", 42,18,9,1},
#endif
#ifdef GERMAN
/* 2*/  {3,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"ABBRUCH",   42,20,9,1},
#else
#ifdef FRAMEBUFFER
/* 2*/  {3,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"CANCEL",    42,19,9,3},
#else
/* 2*/  {3,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"CANCEL",    42,20,9,1},
#endif
#endif
#ifdef FRAMEBUFFER
/* 3*/  {4,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"HOME",      42,11,9,2},
#else
/* 3*/  {4,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"HOME",      42,12,9,1},
#endif
/* 4*/  {5,-1,-1,G_TEXT,  NONE, NORMAL, (LONG)&tedinfo[0],      1,1,52,1},
/* 5*/  {11,6,17,G_BOX, NONE, NORMAL, 0x00fe1120, 2,8,FWW+2,ANZSHOW+1},
/* 6*/  {7,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"<", 0,0,2,1},
/* 7*/  {8,-1,-1,G_BUTTON, SELECTABLE|TOUCHEXIT, NORMAL, (LONG)"^",    FWW,1,2,1},
/* 8*/  {9,-1,-1,G_BUTTON, SELECTABLE|TOUCHEXIT, NORMAL, (LONG)"v", FWW,ANZSHOW,2,1},
/* 9*/  {10,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"?",         FWW,0,2,1},
/*10*/  {15,-1,-1,G_BOXTEXT, SELECTABLE|EXIT, NORMAL, (LONG)&tedinfo[1],       2,0,FWW-2,1},
#ifdef GERMAN
/*11*/  {12,-1,-1,G_STRING,NONE, NORMAL, (LONG)"Pfad:", 2,3,2,1},
/*12*/  {13,-1,-1,G_STRING,NONE, NORMAL, (LONG)"Auswahl:", 32,5,2,1},
#else
/*11*/  {12,-1,-1,G_STRING,NONE, NORMAL, (LONG)"Directory:", 2,3,2,1},
/*12*/  {13,-1,-1,G_STRING,NONE, NORMAL, (LONG)"Selection:", 32,5,2,1},
#endif
/*13*/  {14,-1,-1,G_FTEXT,  EDITABLE, NORMAL, (LONG)&tedinfo[2],       2,4,50,1},
/*14*/  {0,-1,-1,G_FTEXT,  EDITABLE, NORMAL, (LONG)&tedinfo[3],      32,6,20,1},
/*15*/  {17,16,16,G_BOX, TOUCHEXIT, NORMAL, 0x00ff1459, FWW,2,2,ANZSHOW-2},
/*16*/  {15,-1,-1,G_BOX, TOUCHEXIT, NORMAL, 0x00ff1100, 0,0,2,2},
/*17*/  {5,18,17+2*ANZSHOW,G_BOX, NONE, NORMAL, 0x00ff1100, 0,1,FWW,ANZSHOW}
  };
  int objccount=sizeof(objects)/sizeof(OBJECT);
  int x,y,w,h;
  int obx,oby;
#ifdef DEBUG
  printf("**fsel_input:\n");
#endif

  if(titel!=NULL) strncpy(btitel,titel,60);
  else strcpy(btitel,"FILESELCT");
#ifdef DEBUG
  printf("**2fsel_input: ANZSHOW=%d btitel=%s\n",ANZSHOW,btitel);
#endif

  tedinfo[0].te_txtlen=strlen(btitel);
  tedinfo[0].te_tmplen=0;
  for(i=0;i<ANZSHOW;i++){
    tedinfo[4+i].te_ptext=(LONG)"________________________________";
    tedinfo[4+i].te_ptmplt=(LONG)"________________________________";
    tedinfo[4+i].te_pvalid=(LONG)"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    tedinfo[4+i].te_font=FONT_IBM;
    tedinfo[4+i].te_just=TE_LEFT;
    tedinfo[4+i].te_color=0x1110;
    tedinfo[4+i].te_txtlen=128;
    tedinfo[4+i].te_tmplen=FWW-4;
    objects[18+2*i].ob_next=18+2*i+1;
    objects[18+2*i].ob_head=-1;
    objects[18+2*i].ob_tail=-1;
    objects[18+2*i].ob_type=G_FTEXT;
    objects[18+2*i].ob_flags=SELECTABLE|RBUTTON|TOUCHEXIT;
    objects[18+2*i].ob_state=NORMAL;
    objects[18+2*i].ob_spec=(LONG)&tedinfo[4+i];
    objects[18+2*i].ob_x=2;
    objects[18+2*i].ob_y=i;
    objects[18+2*i].ob_width=FWW-3;
    objects[18+2*i].ob_height=1;
    objects[18+2*i+1].ob_next=18+2*i+2;
    objects[18+2*i+1].ob_head=-1;
    objects[18+2*i+1].ob_tail=-1;
    objects[18+2*i+1].ob_type=G_BOXCHAR;
    objects[18+2*i+1].ob_flags=HIDETREE;
    objects[18+2*i+1].ob_state=NORMAL;
    objects[18+2*i+1].ob_spec=0x20000100;
    objects[18+2*i+1].ob_x=0;
    objects[18+2*i+1].ob_y=i;
    objects[18+2*i+1].ob_width=2;
    objects[18+2*i+1].ob_height=1;

  }
  objects[18+2*ANZSHOW-1].ob_next=17;
  objects[objccount-1].ob_flags|=LASTOB;
  graphics();
  gem_init();
  for(i=0;i<objccount;i++){
    objects[i].ob_x*=chw;
    objects[i].ob_y*=chh;
    objects[i].ob_width*=chw;
    objects[i].ob_height*=chh;
  }

  wort_sepr(pfad,'/',0,dpfad, mask);
  anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
  sort_dir(filenamen,anzfiles);

  strcpy(feld1,pfad);
  strcpy(feld2,sel);

  for(i=0;i<anzfiles;i++) filenamensel[i]=(strcmp(filenamen[i].name,feld2)==0);


  tedinfo[2].te_junk1=strlen(pfad);
  tedinfo[3].te_junk1=strlen(sel);
  tedinfo[2].te_junk2=0;
  tedinfo[3].te_junk2=0;

  make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
  make_scaler(objects,anzfiles,showstart);


  form_center(objects, &x,&y,&w,&h);  /* Objektbaum Zentrieren */
  form_dial(0,0,0,0,0,x,y,w,h);
  form_dial(1,0,0,0,0,x,y,w,h);
  while(sbut!=1 && sbut!=2) {
    objc_draw(objects,0,-1,0,0);
    sbut=form_do(objects);
    if(sbut==3) {    /* HOME */
      char buf[128];
      if(getcwd(buf,128)!=NULL) {

        strcpy(dpfad,buf);
	sprintf(feld1,"%s/%s",dpfad,mask);

	tedinfo[2].te_junk1=strlen(feld1);
	tedinfo[2].te_junk2=0;

	anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
	for(k=0;k<anzfiles;k++) filenamensel[k]=0;
        sort_dir(filenamen,anzfiles);
	showstart=max(0,min(showstart,anzfiles-ANZSHOW));
	make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
	make_scaler(objects,anzfiles,showstart);

      }
      objects[sbut].ob_state=NORMAL;
    } else if(sbut==6) {    /* < */
      char buf[128];
      wort_sepr(dpfad,'/',0,dpfad, buf);
      sprintf(feld1,"%s/%s",dpfad,mask);
      tedinfo[2].te_junk1=strlen(feld1);
      tedinfo[2].te_junk2=0;
      anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
      sort_dir(filenamen,anzfiles);
      for(k=0;k<anzfiles;k++) filenamensel[k]=0;
      showstart=max(0,min(showstart,anzfiles-ANZSHOW));
      make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
      make_scaler(objects,anzfiles,showstart);objects[sbut].ob_state=NORMAL;
    } else if(sbut==7) {    /* ^ */
      if(showstart) {
        showstart--;
        make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
	make_scaler(objects,anzfiles,showstart);
      }
      objects[sbut].ob_state=NORMAL;
    } else if(sbut==8) {    /* v */
      if(showstart<anzfiles-ANZSHOW) {
        showstart++;
        make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
        make_scaler(objects,anzfiles,showstart);
      }
      objects[sbut].ob_state=NORMAL;
    } else if(sbut==9) {    /* ? */
      char buf[128];
      sprintf(buf,"[1][%d Bytes in %d Files.][ OK ]",dir_bytes(filenamen,anzfiles),anzfiles);
      form_alert(1,buf);
      objects[sbut].ob_state=NORMAL;
    } else if(sbut==10) {    /* MASK */
      wort_sepr((char *)tedinfo[2].te_ptext,'/',0,dpfad, mask);
      anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
      sort_dir(filenamen,anzfiles);
      showstart=max(0,min(showstart,anzfiles-ANZSHOW));
      make_filelist(objects,filenamen, filenamensel,anzfiles,showstart);
      make_scaler(objects,anzfiles,showstart);objects[sbut].ob_state=NORMAL;
    } else if(sbut==15) {    /* Scalerhintergrund */
#ifdef USE_SDL
      e=SDL_WaitEvent(&event);
      if(e==0) return;
      while(event.type!=SDL_MOUSEBUTTONUP) { 
        handle_event(usewindow,&event);
        e=SDL_WaitEvent(&event);
        if(e==0) return;
      }
      relobxy(objects,16,&obx, &oby);
      if(event.button.y<oby) {
	     showstart=max(0,min(showstart-ANZSHOW,anzfiles-ANZSHOW));
             make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
             make_scaler(objects,anzfiles,showstart);
      } else if(event.button.y>oby+objects[16].ob_height) {
	     showstart=max(0,min(showstart+ANZSHOW,anzfiles-ANZSHOW));
             make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
             make_scaler(objects,anzfiles,showstart);
      }

#endif

      #if defined USE_X11 || defined FRAMEBUFFER
      XWindowEvent(display[usewindow], win[usewindow],
       ButtonReleaseMask|ExposureMask , &event);
      switch (event.type) {

    /* Das Redraw-Event */
      case Expose:
#ifdef USE_X11
        XCopyArea(display[usewindow],pix[usewindow],win[usewindow],gc[usewindow],
          event.xexpose.x,event.xexpose.y,
          event.xexpose.width,event.xexpose.height,
          event.xexpose.x,event.xexpose.y);
#endif
        break;
	case ButtonRelease:
	  relobxy(objects,16,&obx, &oby);
           if(event.xbutton.y<oby) {
	     showstart=max(0,min(showstart-ANZSHOW,anzfiles-ANZSHOW));
             make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
             make_scaler(objects,anzfiles,showstart);
	   } else if(event.xbutton.y>oby+objects[16].ob_height) {
	     showstart=max(0,min(showstart+ANZSHOW,anzfiles-ANZSHOW));
             make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
             make_scaler(objects,anzfiles,showstart);
	   }
	break;
      }
      #endif
    } else if(sbut==16) {    /* ScalerSchieber */
      int ex=0,root_x_return,root_y_return,win_x_return,win_y_return;
      unsigned int mask_return;
      int ssold=showstart;
      int sssold=showstart;
      #ifdef USE_X11
      Window root_return,child_return;
      #endif
      #if defined USE_X11 || defined FRAMEBUFFER || defined USE_SDL
       XQueryPointer(display[usewindow], win[usewindow], &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);
      #endif
      #if defined USE_X11 || defined FRAMEBUFFER 

      while(ex==0) {
       XWindowEvent(display[usewindow], win[usewindow],
       ButtonReleaseMask|PointerMotionMask|ExposureMask , &event);
      switch (event.type) {

    /* Das Redraw-Event */
      case Expose:
#ifdef USE_X11
        XCopyArea(display[usewindow],pix[usewindow],win[usewindow],gc[usewindow],
          event.xexpose.x,event.xexpose.y,
          event.xexpose.width,event.xexpose.height,
          event.xexpose.x,event.xexpose.y);
#endif
        break;
	case ButtonRelease:
          ex=1;
	break;
          case MotionNotify:
         /* printf("Motion %d\n",event.xmotion.y-win_y_return);*/
          showstart=ssold+(event.xmotion.y-win_y_return)/
		     (objects[15].ob_height*(1-min(1,(float)(ANZSHOW)/anzfiles)))
		     *(anzfiles-ANZSHOW);
	  showstart=max(0,min(showstart,anzfiles-ANZSHOW));
          if(showstart!=sssold) {
            make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
            make_scaler(objects,anzfiles,showstart);
            objc_draw(objects,0,-1,0,0);
	    activate();
	    sssold=showstart;
	  }
          break;
     }
     }
     #endif
    } else if(sbut>=18 && sbut<ANZSHOW*2+18) {    /* Auswahlliste */
      int j=(sbut-18)/2;
      char buf[128];
	
      if(showstart+j<anzfiles) {	
      /*  printf("--->%s\n",filenamen[showstart+j].name);*/	
        if(filenamen[showstart+j].typ & FT_DIR) {
          sprintf(buf,"%s/%s",dpfad,filenamen[showstart+j].name);
          strcpy(dpfad,buf);
          sprintf(feld1,"%s/%s",dpfad,mask);
	  tedinfo[2].te_junk1=strlen(feld1);
	  tedinfo[2].te_junk2=0;
          anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
          sort_dir(filenamen,anzfiles);
	  for(k=0;k<anzfiles;k++) filenamensel[k]=0;
          showstart=max(0,min(showstart,anzfiles-ANZSHOW));
	  make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
	  make_scaler(objects,anzfiles,showstart);
        } else {
          for(k=0;k<anzfiles;k++) filenamensel[k]=0;
	  filenamensel[showstart+j]=1;
	  strcpy((char *)tedinfo[3].te_ptext,filenamen[showstart+j].name);
	  tedinfo[3].te_junk1=strlen((char *)tedinfo[3].te_ptext);
	  tedinfo[3].te_junk2=0;
        }
      }
#if DEBUG
    } else {
      printf("BUTTON: %d\n",sbut);
#endif
    }
  }
  form_dial(3,0,0,0,0,x,y,w,h);
  form_dial(2,0,0,0,0,x,y,w,h);
  ergebnis=malloc(strlen(dpfad)+strlen((char *)tedinfo[3].te_ptext)+2);
  if(sbut==1) sprintf(ergebnis,"%s/%s",dpfad,(char *)tedinfo[3].te_ptext);
  else ergebnis[0]=0;
  return(ergebnis);
}
static void make_list(OBJECT *objects,STRING *name,int *filenamensel,int anzfiles,int showstart){
  int i,j;
  TEDINFO *ted;
  for(i=0;i<ANZSHOW;i++) {
    j=showstart+i;
    ted=(TEDINFO *)objects[15+2*i].ob_spec;
    objects[15+2*i+1].ob_spec&=0x00ffffff;
      if(j<anzfiles) {
        if(filenamensel[j]==1) objects[15+2*i].ob_state=SELECTED;
        else objects[15+2*i].ob_state=NORMAL;
        ted->te_ptext=(LONG)name[j].pointer;
	objects[15+2*i].ob_flags=SELECTABLE|TOUCHEXIT|RBUTTON;
	objects[15+2*i+1].ob_spec|=0x20000000;
      } else {
        ted->te_ptext=(LONG)"";
        objects[15+2*i].ob_state=NORMAL;
        objects[15+2*i].ob_flags=NONE;	
        objects[15+2*i+1].ob_spec|=0x20000000;
    }
  }
}
static void make_scaler2(OBJECT *objects,int anzfiles,int showstart){
  float hoehe,xpos;

  if(anzfiles>ANZSHOW){
    hoehe=min(1.0,max(0.1,(float)ANZSHOW/anzfiles));
    xpos=(float)showstart/(anzfiles-ANZSHOW);
  } else {
    hoehe=1;
    xpos=0;
  }
  objects[13].ob_height=(int)(hoehe*objects[12].ob_height);
  objects[13].ob_y=(int)(xpos*(objects[12].ob_height-objects[13].ob_height));
}

/* List-Select */

int lsel_input(const char *titel, STRING *strs,int anzfiles,int sel) {
  char btitel[128],mask[128];
  char findmsk[50];
  int ergebnis;
  int i,k;
  int sbut=-1;
#if defined USE_X11 || defined FRAMEBUFFER
  XEvent event;
#endif
  int filenamensel[anzfiles];
  int showstart=0;
  TEDINFO tedinfo[4+ANZSHOW]={
  {(LONG)btitel,(LONG)"",(LONG)"",FONT_IBM,0,TE_CNTR,0x1200,0,0,0,0},
  {(LONG)mask,(LONG)"",(LONG)"",FONT_IBM,0,TE_CNTR,0x113a,0,2,0,FWW-2},
  {(LONG)findmsk,(LONG)"________________________",(LONG)"XXXXXXXXXXXXXXXXXXXXXXXX",FONT_IBM,0,TE_LEFT,0x1100,0,0,128,24}
  };
 // int anztedinfo=sizeof(tedinfo)/sizeof(TEDINFO);
  OBJECT objects[15+2*ANZSHOW]={
/* 0*/  {-1,1, 6,G_BOX, NONE, OUTLINED, 0x00021100, 0,0,40,21},
/* 1*/  {2,-1,-1,G_BUTTON, SELECTABLE|DEFAULT|EXIT,NORMAL ,(LONG)"OK", 10,19,9,1},
/* 2*/  {3,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"CANCEL",    24,19,9,1},
/* 3*/  {4,-1,-1,G_TEXT,  NONE, NORMAL, (LONG)&tedinfo[0],      1,1,38,1},
/* 4*/  {5,-1,-1,G_FTEXT,  EDITABLE, NORMAL, (LONG)&tedinfo[2], 12,3,FWW-4-12,1},
/* 5*/  {6,-1,-1,G_STRING,NONE, NORMAL, (LONG)"Find:", 2,3,2,1},

/* 6*/  {0,7,14,G_BOX, NONE, NORMAL, 0x00fe1120, 2,4,FWW+2,ANZSHOW+1},
/* 7*/  {8,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"<", 0,0,2,1},
/* 8*/  {9,-1,-1,G_BUTTON, SELECTABLE|TOUCHEXIT, NORMAL, (LONG)"^", FWW,1,2,1},
/* 9*/  {10,-1,-1,G_BUTTON, SELECTABLE|TOUCHEXIT, NORMAL, (LONG)"v", FWW,ANZSHOW,2,1},
/*10*/  {11,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"?",     FWW,0,2,1},
/*11*/  {12,-1,-1,G_BOXTEXT, SELECTABLE|EXIT, NORMAL, (LONG)&tedinfo[1],2,0,FWW-2,1},
/*12*/  {14,13,13,G_BOX, TOUCHEXIT, NORMAL, 0x00ff1459, FWW,2,2,ANZSHOW-2},
/*13*/  {12,-1,-1,G_BOX, TOUCHEXIT, NORMAL, 0x00ff1100, 0,0,2,2},
/*14*/  {6,15,14+2*ANZSHOW,G_BOX, NONE, NORMAL, 0x00ff1100, 0,1,FWW,ANZSHOW}
  };
  int objccount=sizeof(objects)/sizeof(OBJECT);
  int x,y,w,h;
  int obx,oby;

#define LISTSELECT_OK       1
#define LISTSELECT_CANCEL   2
#define LISTSELECT_BACK     7
#define LISTSELECT_UP       8
#define LISTSELECT_DOWN     9
#define LISTSELECT_INFO    10
#define LISTSELECT_MASK    11
#define LISTSELECT_SCALERP 12
#define LISTSELECT_SCALER  13


  if(titel!=NULL) strncpy(btitel,titel,38);
  else strcpy(btitel,"LISTSELCT");

  strcpy(findmsk,"");
  strcpy(mask,"*");

  tedinfo[0].te_txtlen=strlen(btitel);
  tedinfo[0].te_tmplen=0;
  tedinfo[1].te_junk1=24;
  tedinfo[1].te_junk2=strlen(mask);
  tedinfo[2].te_txtlen=24;
  tedinfo[2].te_tmplen=24;
  tedinfo[2].te_junk1=24;
  tedinfo[2].te_junk2=strlen(findmsk);

  for(i=0;i<ANZSHOW;i++){
    tedinfo[4+i].te_ptext=(LONG)"Hallo";
    tedinfo[4+i].te_ptmplt=(LONG)"______________________________";
    tedinfo[4+i].te_pvalid=(LONG)"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
    tedinfo[4+i].te_font=FONT_IBM;
    tedinfo[4+i].te_just=TE_LEFT;
    tedinfo[4+i].te_color=0x1110;
    tedinfo[4+i].te_txtlen=128;
    tedinfo[4+i].te_tmplen=FWW-4;
    objects[15+2*i].ob_next=15+2*i+1;
    objects[15+2*i].ob_head=-1;
    objects[15+2*i].ob_tail=-1;
    objects[15+2*i].ob_type=G_FTEXT;
    objects[15+2*i].ob_flags=SELECTABLE|RBUTTON|TOUCHEXIT;
    objects[15+2*i].ob_state=NORMAL;
    objects[15+2*i].ob_spec=(LONG)&tedinfo[4+i];
    objects[15+2*i].ob_x=2;
    objects[15+2*i].ob_y=i;
    objects[15+2*i].ob_width=FWW-3;
    objects[15+2*i].ob_height=1;
    objects[15+2*i+1].ob_next=15+2*i+2;
    objects[15+2*i+1].ob_head=-1;
    objects[15+2*i+1].ob_tail=-1;
    objects[15+2*i+1].ob_type=G_BOXCHAR;
    objects[15+2*i+1].ob_flags=HIDETREE;
    objects[15+2*i+1].ob_state=NORMAL;
    objects[15+2*i+1].ob_spec=0x20000100;
    objects[15+2*i+1].ob_x=0;
    objects[15+2*i+1].ob_y=i;
    objects[15+2*i+1].ob_width=2;
    objects[15+2*i+1].ob_height=1;
  }
  objects[15+2*ANZSHOW-1].ob_next=14;
  objects[objccount-1].ob_flags|=LASTOB;

  graphics();
  gem_init();
  for(i=0;i<objccount;i++){
    objects[i].ob_x*=chw;
    objects[i].ob_y*=chh;
    objects[i].ob_width*=chw;
    objects[i].ob_height*=chh;
  }
  if(sel>=anzfiles) sel=-1;
  for(i=0;i<anzfiles;i++) filenamensel[i]=0;
  if(sel>=0) filenamensel[sel]=1;

  make_list(objects,strs,filenamensel,anzfiles,showstart);
  make_scaler2(objects,anzfiles,showstart);
  form_center(objects, &x,&y,&w,&h);  /* Objektbaum Zentrieren */
  form_dial(0,0,0,0,0,x,y,w,h);
  form_dial(1,0,0,0,0,x,y,w,h);
  while(sbut!=LISTSELECT_OK && sbut!=LISTSELECT_CANCEL) {
    objc_draw(objects,0,-1,0,0);
    sbut=form_do(objects);
    if(sbut==LISTSELECT_BACK) {    /* < */
//      tedinfo[2].te_junk1=strlen(feld1);
//      tedinfo[2].te_junk2=0;
      showstart=max(0,min(showstart,anzfiles-ANZSHOW));
//      make_list(objects,filenamen,filenamensel,anzfiles,showstart);
      make_scaler2(objects,anzfiles,showstart);
      objects[sbut].ob_state=NORMAL;
    } else if(sbut==LISTSELECT_UP) {    /* ^ */
      if(showstart) {
        showstart--;
        make_list(objects,strs,filenamensel,anzfiles,showstart);
	make_scaler2(objects,anzfiles,showstart);
      }objects[sbut].ob_state=NORMAL;
    } else if(sbut==LISTSELECT_DOWN) {    /* v */
      if(showstart<anzfiles-ANZSHOW) {
        showstart++;
        make_list(objects,strs,filenamensel,anzfiles,showstart);
        make_scaler2(objects,anzfiles,showstart);
      }objects[sbut].ob_state=NORMAL;
    } else if(sbut==LISTSELECT_INFO) {    /* ? */
      char buf[128];
      sprintf(buf,"[1][%d list entrys.][ OK ]",anzfiles);
      form_alert(1,buf);objects[sbut].ob_state=NORMAL;
    } else if(sbut==LISTSELECT_MASK) {    /* MASK */
     /* printf("Hier jetzt suchen nach %s \n",findmsk); */
      strcpy(mask,"*");
      strcat(mask,findmsk);
      strcat(mask,"*");
      sel=-1;
      for(k=0;k<anzfiles;k++) {
        if(fnmatch(mask,strs[k].pointer,FNM_NOESCAPE)==0) sel=k;
        filenamensel[k]=0;
      }
      if(sel!=-1) {
        filenamensel[sel]=1;
        if(sel<showstart || sel>=showstart+ANZSHOW) showstart=sel;
      }

      showstart=max(0,min(showstart,anzfiles-ANZSHOW));
      make_list(objects,strs, filenamensel,anzfiles,showstart);
      make_scaler2(objects,anzfiles,showstart);objects[sbut].ob_state=NORMAL;
    } else if(sbut==LISTSELECT_SCALERP) {    /* Scalerhintergrund */
      #if defined FRAMEBUFFER || defined USE_X11
      XWindowEvent(display[usewindow], win[usewindow],
       ButtonReleaseMask|ExposureMask , &event);
      switch (event.type) {

    /* Das Redraw-Event */
      case Expose:
#ifdef USE_X11
        XCopyArea(display[usewindow],pix[usewindow],win[usewindow],gc[usewindow],
          event.xexpose.x,event.xexpose.y,
          event.xexpose.width,event.xexpose.height,
          event.xexpose.x,event.xexpose.y);
#endif
        break;
	case ButtonRelease:
	  relobxy(objects,LISTSELECT_SCALER,&obx, &oby);
           if(event.xbutton.y<oby) {
	     showstart=max(0,min(showstart-ANZSHOW,anzfiles-ANZSHOW));
             make_list(objects,strs,filenamensel,anzfiles,showstart);
             make_scaler2(objects,anzfiles,showstart);
	   } else if(event.xbutton.y>oby+objects[LISTSELECT_SCALER].ob_height) {
	     showstart=max(0,min(showstart+ANZSHOW,anzfiles-ANZSHOW));
             make_list(objects,strs,filenamensel,anzfiles,showstart);
             make_scaler2(objects,anzfiles,showstart);
	   }
	break;
      }
      #endif
    } else if(sbut==LISTSELECT_SCALER) {    /* ScalerSchieber */
      int ex=0,root_x_return,root_y_return,win_x_return,win_y_return;
      unsigned int mask_return;
      int ssold=showstart;
      int sssold=showstart;
      #if defined USE_X11 || defined FRAMEBUFFER
#ifdef USE_X11
      Window root_return,child_return;
#endif
       XQueryPointer(display[usewindow], win[usewindow], &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);

      while(ex==0) {
       XWindowEvent(display[usewindow], win[usewindow],
       ButtonReleaseMask|PointerMotionMask|ExposureMask , &event);
      switch (event.type) {

    /* Das Redraw-Event */
      case Expose:
#ifndef FRAMEBUFFER
        XCopyArea(display[usewindow],pix[usewindow],win[usewindow],gc[usewindow],
          event.xexpose.x,event.xexpose.y,
          event.xexpose.width,event.xexpose.height,
          event.xexpose.x,event.xexpose.y);
#endif
        break;
	case ButtonRelease:
          ex=1;
	break;
          case MotionNotify:
         /* printf("Motion %d\n",event.xmotion.y-win_y_return);*/
          showstart=ssold+(event.xmotion.y-win_y_return)/
		     (objects[LISTSELECT_SCALERP].ob_height*(1-min(1,(float)(ANZSHOW)/anzfiles)))
		     *(anzfiles-ANZSHOW);
	  showstart=max(0,min(showstart,anzfiles-ANZSHOW));
          if(showstart!=sssold) {
            make_list(objects,strs,filenamensel,anzfiles,showstart);
            make_scaler2(objects,anzfiles,showstart);
            objc_draw(objects,0,-1,0,0);
	    activate();
	    sssold=showstart;
	  }
          break;
     }
     }
     #endif
    } else if(sbut>=15 && sbut<ANZSHOW*2+15) {    /* Auswahlliste */
      int j=(sbut-15)/2;
      if(showstart+j<anzfiles) {	
          for(k=0;k<anzfiles;k++) filenamensel[k]=0;
	  sel=showstart+j;
	  filenamensel[sel]=1;
      } else {objects[sbut].ob_state=NORMAL;sel=-1;}
    } else {
      if(sbut==LISTSELECT_OK) ergebnis=sel;
      else ergebnis=-1;
    }
  }
  form_dial(3,0,0,0,0,x,y,w,h);
  form_dial(2,0,0,0,0,x,y,w,h);
  return(ergebnis);
}
#endif /* nographics */



void do_sizew(int winnr,int w,int h) {
#ifdef WINDOWS_NATIVE
  RECT interior;
  GetClientRect(win_hwnd[winnr],&interior);
  MoveWindow(win_hwnd[winnr],interior.left,interior.top,w,h,1);
#elif defined USE_X11
  Pixmap pixi;
  Window root;
  int ox,oy;
  unsigned int ow,oh,ob,d;

  XGetGeometry(display[winnr],win[winnr],&root,&ox,&oy,&ow,&oh,&ob,&d);
  pixi=XCreatePixmap(display[winnr], win[winnr], w, h, d);
  XResizeWindow(display[winnr], win[winnr], w, h);
  XCopyArea(display[winnr],pix[winnr],pixi,gc[winnr],0,0,min(w,ow),min(h,oh),0,0);
  XFreePixmap(display[winnr],pix[winnr]); 
  pix[winnr]=pixi;
  XFlush(display[winnr]);
#elif defined USE_SDL
  Uint32 flags;
  flags=display[winnr]->flags;
  SDL_FreeSurface(display[winnr]);
  if(!(display[winnr]=SDL_SetVideoMode(w, h, 32, flags))) {
    printf("cannot open SDL surface \n");
    SDL_Quit();
    return;
  }
#endif
}
