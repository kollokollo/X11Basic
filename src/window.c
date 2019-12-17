
/* window.c      Window-Routinen (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fnmatch.h>

#include "defs.h"
#include "x11basic.h"
#include "xbasic.h"
#include "memory.h"
#include "graphics.h"
#include "variablen.h"
#include "file.h"

#include "wort_sep.h"
#include "window.h"
#include "aes.h"
#ifdef FRAMEBUFFER
#include "raw_mouse.h"
#include "raw_keyboard.h"
#elif defined USE_SDL
  #include <SDL/SDL.h>
  #include <SDL/SDL_gfxPrimitives.h>
#endif


/* globale Variablen */

WINDOWDEF window[MAXWINDOWS];
int usewindow=DEFAULTWINDOW;


/* GEM-Globals   */

#if defined NOGRAPHICS 
void activate() {;}
#else
#ifdef FRAMEBUFFER
#elif defined USE_GEM
#include <osbind.h>
#include <gem.h>
#else
#endif
#ifdef WINDOWS_NATIVE
  HANDLE keyevent=INVALID_HANDLE_VALUE; /* handle for win thread event */
  HANDLE buttonevent=INVALID_HANDLE_VALUE; /* handle for win thread event */
  HANDLE motionevent=INVALID_HANDLE_VALUE; /* handle for win thread event */
  HANDLE tsync=INVALID_HANDLE_VALUE; /* handle for win thread event */
#endif

static int create_window2(int nummer,const char *title, const char* info,int x,int y,unsigned int w,unsigned int h);
static int fetch_rootwindow();
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
        if(window[i].flags&WIN_CREATED) {
          if(window[i].win_hwnd==hwnd) {nr=i;break;}
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

  window[nummer].flags=WIN_CREATED;
  cr.left=0;
  cr.top=0;
  cr.right=WINDOW_DEFAULT_W;
  cr.bottom=WINDOW_DEFAULT_H;
  AdjustWindowRect(&cr,WS_VISIBLE|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,FALSE);

	window[nummer].win_hwnd = CreateWindow("X11-Basic",NULL,  /* my style */
		WS_VISIBLE|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,/* window style */
		CW_USEDEFAULT, CW_USEDEFAULT, cr.right-cr.left, cr.bottom-cr.top,
		NULL, NULL, hInstance, NULL);

  SetWindowText(window[nummer].win_hwnd,"X11-Basic on WINDOWS");
  ShowWindow(window[nummer].win_hwnd, SW_SHOW);
  UpdateWindow(window[nummer].win_hwnd);
  SetForegroundWindow(window[nummer].win_hwnd);

  hdc=GetDC(window[nummer].win_hwnd);
  bitcon[nummer]=CreateCompatibleDC(hdc);
  backbit[nummer]=CreateCompatibleBitmap(hdc,WINDOW_DEFAULT_W,WINDOW_DEFAULT_H);
  SelectObject(bitcon[nummer],backbit[nummer]);
  SelectClipRgn(bitcon[nummer],NULL);
  IntersectClipRect(bitcon[nummer],0,0,WINDOW_DEFAULT_W,WINDOW_DEFAULT_H);
  window[nummer].pen=CreatePen(PS_SOLID,1,RGB(255,255,0));
  window[nummer].brush=CreateSolidBrush(RGB(0,127,127));

  SelectObject(bitcon[nummer],pen[nummer]);
  SelectObject(bitcon[nummer],brush[nummer]);
  SetTextColor(bitcon[nummer],RGB(255,255,0)) ;
  SetBkMode(bitcon[nummer],TRANSPARENT);
  SetBkColor(bitcon[nummer],RGB(63,63,63)) ;

  window[nummer].x=0;
  window[nummer].y=0;
  window[nummer].w=WINDOW_DEFAULT_W;
  window[nummer].h=WINDOW_DEFAULT_H;

#if 0
  font[nummer]=CreateFont(13,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
  DEFAULT_QUALITY,FIXED_PITCH | FF_MODERN,NULL);
  if (font[nummer]!=NULL) {
    SelectObject(bitcon[nummer],font[nummer]);
  } else printf("Could not create font !!\n");
#endif
  ReleaseDC(window[nummer].win_hwnd,hdc);
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
  window[nummer].flags=0;    /* Das sollte nicht passieren....*/
  printf("WARNING: Window thread exited. #%d\n",nummer);
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


void fetch_icon_pixmap(WINDOWDEF *,int);


static int create_window2(int nummer,const char *title, const char* info,int x,int y,unsigned int w,unsigned int h) {
#ifdef WINDOWS
    static int class_reg=0;
#endif
  if(window[nummer].flags&WIN_CREATED) {
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
          return(-1);
	}
       class_reg=1;
    }
    ResetEvent(tsync);
  /* create thread to care for window */
  window[nummer].wthandle=_beginthread((LPTHREAD_START_ROUTINE)winthread,0,nummer);
  if (window[nummer].wthandle==NULL) {
    MessageBox(NULL,"X11-Basic: can't create thread for window" , "Error!",
			MB_ICONEXCLAMATION | MB_OK);
			return(-1);
  }
  WaitForSingleObject(tsync,INFINITE);
  
#elif defined USE_VGA
  vga_init();
  /*  TODO: WINDOW groesse etc...*/
#elif defined FRAMEBUFFER
  Fb_Open();
  #ifndef ANDROID
    Fb_Mouse_Open();
    Fb_Keyboard_Open();
  #endif
  window[nummer].x=window[nummer].y=0;
  window[nummer].w=screen.width;
  window[nummer].h=screen.height;
  window[nummer].d=screen.bpp;
  window[nummer].b=0;
  window[nummer].screen=&screen;
  window[nummer].fcolor=screen.fcolor;
  window[nummer].bcolor=screen.bcolor;
#elif defined USE_GEM
  int i;
  int gl_apid=appl_init();
  if(gl_apid<0) exit(0);
  work_in[0]=Getrez()+2;
  for(i=1;i<10;i++) work_in[i]=1;
  work_in[10]=2;
  v_opnvwk(work_in,&window[nummer].vdi_handle,work_out);
  short clip[4];
  window[nummer].x=window[nummer].y=0;
  window[nummer].w=work_out[0];
  window[nummer].h=work_out[1];
  window[nummer].fcolor=1;
  window[nummer].bcolor=0;
  clip[0]=0;
  clip[1]=0;
  clip[2]=work_out[0];
  clip[3]=work_out[1];
  vs_clip(window[nummer].vdi_handle,1,clip);
  unsigned short w,h;
  window[nummer].aesvdi_handle=graf_handle(&window[nummer].chw,&window[nummer].chh,&w,&h);
  /* w und h sind nun die verkleinerten Screengrößen. Wollen wir eigentlich nicht.*/
  gem_init();
#elif defined USE_SDL
    init_sdl();
    if(!(window[nummer].display=SDL_SetVideoMode(WINDOW_DEFAULT_W, 
       WINDOW_DEFAULT_H, 32,
    // SDL_FULLSCREEN |
       SDL_HWSURFACE|SDL_SRCALPHA))) {
      printf("cannot open SDL surface \n");
      SDL_Quit();
      return(-1);
    }
    window[nummer].x=window[nummer].y=0;
    window[nummer].w=window[nummer].display->w;
    window[nummer].h=window[nummer].display->h;
    window[nummer].fcolor=0xffffffff;
    window[nummer].bcolor=0;
    SDL_WM_SetCaption(title,info);
#elif defined USE_X11
  XGCValues gc_val;            /* */
  Window root;
  XTextProperty win_name, icon_name;
  char *agv[1];
  
/* This should be called: */
  XInitThreads(); 
  /* Verbindung zum X-Server aufnehmen. */
  if((window[nummer].display=XOpenDisplay(display_name))==NULL) {
    printf("Can't Connect XServer on display %s. Aborted\n",XDisplayName(display_name));
    return(-1);
  }
  /* Welchen Bildschirm nehmen ? */
  int screen_num = DefaultScreen(window[nummer].display);/* Ein Server kann mehrere Bildschirme haben */
  unsigned int border=0;

  /* Fenster Oeffnen */
  window[nummer].bcolor=BlackPixel(window[nummer].display, DefaultScreen(window[nummer].display));
  window[nummer].fcolor=WhitePixel(window[nummer].display, screen_num);
  window[nummer].win = XCreateSimpleWindow(window[nummer].display, RootWindow(window[nummer].display, screen_num),
			    x, y, w, h, border, window[nummer].fcolor, window[nummer].bcolor);

  XGetGeometry(window[nummer].display,window[nummer].win,&root,&(window[nummer].x),&(window[nummer].y),
                              &(window[nummer].w),&(window[nummer].h),
			      &(window[nummer].b),&(window[nummer].d));
  window[nummer].x=window[nummer].y=0;  /* Funktioniert so besser */
  
  window[nummer].pix=XCreatePixmap(window[nummer].display,window[nummer].win,window[nummer].w,window[nummer].h,window[nummer].d);
  fetch_icon_pixmap(&window[nummer],nummer);




    /* Dem Window-Manager Hinweise geben, was er mit der Groesse des Fensters
     machen soll. */
  window[nummer].size_hints.flags = PPosition | PSize | PMinSize;
  window[nummer].size_hints.min_width = 32;
  window[nummer].size_hints.min_height = 32;
  window[nummer].wm_hints.flags = StateHint | InputHint | IconPixmapHint;
  window[nummer].wm_hints.initial_state = NormalState;
  window[nummer].wm_hints.input = True;
  window[nummer].wm_hints.icon_pixmap = window[nummer].icon_pixmap;
  window[nummer].class_hint.res_name = "X11-Basic";
  window[nummer].class_hint.res_class = "Graphics";

    if (!XStringListToTextProperty(&(window[nummer].title), 1, &win_name) ||
      !XStringListToTextProperty(&(window[nummer].info), 1, &icon_name)) {
    printf("Couldn't set Name of Window or Icon. Aborted\n");
    return(-1);
  }

  /* Man XSetWMProperties, um zu lesen, was passiert ! */
  XSetWMProperties(window[nummer].display, window[nummer].win, &win_name, &icon_name, agv, 0,
		   &window[nummer].size_hints, &window[nummer].wm_hints, 
		   &window[nummer].class_hint);

  /* Auswaehlen, welche Events man von dem Fenster bekommen moechte */
  XSelectInput(window[nummer].display, window[nummer].win,
               /*ResizeRedirectMask |*/ /*sonst geht SIZEW nicht!*/
	       ExposureMask |
	       ButtonPressMask|
	       ButtonReleaseMask|
	       PointerMotionMask |StructureNotifyMask|VisibilityChangeMask|
	       KeyPressMask|
	       KeyReleaseMask);
  window[nummer].gc = XCreateGC(window[nummer].display, window[nummer].win, 0, &gc_val);
  XSetGraphicsExposures(window[nummer].display, window[nummer].gc,0);
  
  XSetBackground(window[nummer].display, window[nummer].gc, window[nummer].bcolor);
  XSetForeground(window[nummer].display, window[nummer].gc, window[nummer].bcolor);
/* Clear window*/

  XFillRectangle(window[nummer].display,window[nummer].pix,window[nummer].gc,0,0,window[nummer].w,window[nummer].h);   
//   XClearWindow(window[nummer].display, window[nummer].win); /*sonst hat man evtl. Müll von vorher */
  XSetForeground(window[nummer].display, window[nummer].gc, window[nummer].fcolor);

#endif
    if(window[nummer].title) free(window[nummer].title);
    window[nummer].title=strdup(title);
    if(window[nummer].info) free(window[nummer].info);
    window[nummer].info=strdup(info); 
    set_font("BIG");  /*Damit wir einen definierten Font haben*/ 
    window[nummer].flags=WIN_CREATED;
    // printf("window: #%d %d %d %d %d %d %d\n",nummer,window[nummer].x,window[nummer].y,window[nummer].w,window[nummer].h,window[nummer].b,window[nummer].d);
  }
  return(nummer);
}

void open_window(WINDOWDEF *w) {
 if((w->flags&WIN_CREATED) && !(w->flags&WIN_OPENED)) {
#ifdef WINDOWS_NATIVE
#endif
#ifdef USE_SDL


#elif defined USE_GEM
#else
    XEvent event;
    /* Das Fensterauf den Screen Mappen */

    XMapWindow(w->display, w->win);

#ifdef USE_X11
    XNextEvent(w->display, &event);
#endif
    handle_event(w,&event);
#endif
    w->flags|=WIN_OPENED;
  }
}

void close_window(WINDOWDEF *w) {
#ifdef WINDOWS_NATIVE
  if(w->flags&WIN_CREATED) {
    DestroyWindow(w->win_hwnd);
    w->flags=0;
  }
#elif defined USE_VGA
  vga_setmode(TEXT);
  w->flags&=(~WIN_OPENED);
#elif defined USE_SDL
  if(w->flags&WIN_OPENED) {

  }
#elif defined USE_X11
  XEvent event;
  if(w->flags&WIN_OPENED) {
    XUnmapWindow(w->display, w->win);
    XCheckWindowEvent(w->display,w->win,ExposureMask, &event);
    w->flags&=(~WIN_OPENED);
  }
#endif
}

#if defined USE_X11 || defined FRAMEBUFFER || defined USE_SDL
void handle_event(WINDOWDEF *w,XEvent *event) {
  switch (event->type) {
    /* Das Redraw-Event */
#ifdef USE_X11
    case Expose:
      /* if (event.xexpose.count != 0)    break; */
     XCopyArea(w->display,w->pix,event->xexpose.window,w->gc,
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
   case UnmapNotify:
     printf("UnmapNotify type=%d\n",event->type);break;
   case DestroyNotify:
     printf("DestroyNotify type=%d\n",event->type);break;
   case CreateNotify:
     printf("CreateNotify type=%d\n",event->type);break;
   case VisibilityNotify:
     printf("VisibilityNotify type=%d  state=%d\n",event->type,event->xvisibility.state);break;
   case FocusIn:
     printf("FocusIn type=%d\n",event->type);break;
   case FocusOut:
     printf("FocusOut type=%d\n",event->type);break;
   case EnterNotify:
     printf("EnterNotify type=%d\n",event->type);break;
   case LeaveNotify:
     printf("LeaveNotify type=%d\n",event->type);break;
   case ReparentNotify:  /*  21 */
      break;
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
  #ifdef SDL_WINDOWEVENT
  case SDL_WINDOWEVENT:
    switch (event->window.event) {
    case SDL_WINDOWEVENT_SHOWN:
      printf("Window %d shown", event->window.windowID);break;
    case SDL_WINDOWEVENT_HIDDEN:
      printf("Window %d hidden", event->window.windowID);break;
    case SDL_WINDOWEVENT_EXPOSED:
      printf("Window %d exposed", event->window.windowID);break;
    case SDL_WINDOWEVENT_MOVED:
      printf("Window %d moved to %d,%d",event->window.windowID, event->window.data1,event->window.data2);break;
    case SDL_WINDOWEVENT_RESIZED:
      printf("Window %d resized to %dx%d",event->window.windowID, event->window.data1,event->window.data2);break;
    case SDL_WINDOWEVENT_MINIMIZED:
      printf("Window %d minimized", event->window.windowID);break;
    case SDL_WINDOWEVENT_MAXIMIZED:
      printf("Window %d maximized", event->window.windowID);break;
    case SDL_WINDOWEVENT_RESTORED:
      printf("Window %d restored", event->window.windowID);break;
    case SDL_WINDOWEVENT_ENTER:
      printf("Mouse entered window %d",event->window.windowID);break;
    case SDL_WINDOWEVENT_LEAVE:
      printf("Mouse left window %d", event->window.windowID);break;
    case SDL_WINDOWEVENT_FOCUS_GAINED:
      printf("Window %d gained keyboard focus",event->window.windowID);break;
    case SDL_WINDOWEVENT_FOCUS_LOST:
      printf("Window %d lost keyboard focus",event->window.windowID);break;
    case SDL_WINDOWEVENT_CLOSE:
      printf("Window %d closed", event->window.windowID);break;
    default:
      printf("Window %d got unknown event %d",event->window.windowID, event->window.event);break;
    }
  #endif
#endif
  }
}
#endif


void handle_window(WINDOWDEF *w) {
  if(!(w->flags&WIN_CREATED)) return;

#ifdef WINDOWS_NATIVE
#endif
#ifdef USE_SDL
  SDL_Event event;
  while(SDL_PollEvent(&event)) handle_event(w,&event);
#elif defined USE_X11
   XEvent event;
   while(XCheckWindowEvent(w->display,w->win,ExposureMask, &event)) {
     handle_event(w,&event);
   }
#endif
}


void graphics() {
#if defined DEBUG 
  printf("graphics: usewin=%d winbes=%d\n",usewindow,window[usewindow].flags);
#endif
  if(window[usewindow].flags&WIN_CREATED) handle_window(&window[usewindow]);
  else {
     if(usewindow==0) fetch_rootwindow();
     else {
       create_window2(usewindow,"X11-Basic","X11-Basic",100,10,WINDOW_DEFAULT_W,WINDOW_DEFAULT_H);
       open_window(&window[usewindow]);
     }
  }
}

/* fetch_rootwindow() ersetzt create_window2 und open_window fuer das Root-Fenster 
bzw. Fenster Nr 0 (fullscreen) */

static int fetch_rootwindow() {
#ifdef USE_X11
  char *display_name = NULL;   /* NULL: Nimm Argument aus setenv DISPLAY */
  XGCValues gc_val;            /* */
  Window root;
  
  /* This should be called first: */
  XInitThreads(); 

  /* Verbindung zum X-Server aufnehmen. */
  if ((window[0].display=XOpenDisplay(display_name))==NULL) {
    printf("Can't Connect XServer on display %s. Aborted\n",XDisplayName(display_name));
    return(-1);
  }


  window[0].win = RootWindow(window[0].display,DefaultScreen(window[0].display));
  XGetGeometry(window[0].display,window[0].win,&root,&(window[0].x),&(window[0].y),
                                                     &(window[0].w),&(window[0].h),
						     &(window[0].b),&(window[0].d));
  window[0].pix=XCreatePixmap(window[0].display,window[0].win,window[0].w,window[0].h,window[0].d);


  /* Auswaehlen, welche Events man von dem Fenster bekommen moechte */
 /* XSelectInput(window[0].display, window[0].win,
	       ExposureMask |
	       ButtonPressMask|
	       ButtonReleaseMask|
	       PointerMotionMask |
	       KeyPressMask|
	       KeyReleaseMask); */
	
  window[0].gc = XCreateGC(window[0].display, window[0].win, 0, &gc_val);
#elif defined USE_SDL
    init_sdl();
    if(!(window[0].display=SDL_SetVideoMode(WINDOW_DEFAULT_W, 
       WINDOW_DEFAULT_H, 32,
       SDL_FULLSCREEN |
       SDL_HWSURFACE|SDL_SRCALPHA))) {
      printf("cannot open SDL surface \n");
      SDL_Quit();
      return(-1);
    } 
    window[0].x=window[0].y=0;
    window[0].w=window[0].display->w;
    window[0].h=window[0].display->h;
    window[0].fcolor=0xffffffff;
    window[0].bcolor=0;
    
#elif defined USE_VGA
   vga_init();
   /*  TODO: WINDOW groesse etc...*/
#elif defined FRAMEBUFFER
   Fb_Open();
  #ifndef ANDROID
   Fb_Mouse_Open();
  #endif
   window[0].x=window[0].y=0;
   window[0].w=screen.width;
   window[0].h=screen.height;
   window[0].d=screen.bpp;
   window[0].b=0;
   window[0].screen=&screen;
   window[0].fcolor=screen.fcolor;
   window[0].bcolor=screen.bcolor;
#elif defined WINDOWS_NATIVE 
  create_window2(0,"X11-Basic","X11-Basic",100,10,WINDOW_DEFAULT_W,WINDOW_DEFAULT_H);
  open_window(&window[0]);
  return(0);
#elif defined USE_GEM
  printf("TODO: fetch rootwindow:\n");
  
  window[0].fcolor=1;
  window[0].bcolor=0;
 
#endif
  if(window[0].title) free(window[0].title);
  window[0].title=strdup("X11-Basic");
  if(window[0].info) free(window[0].info);
  window[0].info=strdup("root window");
  window[0].flags=WIN_CREATED|WIN_OPENED;
  // printf("Rootwindow: %d %d %d %d %d %d\n",window[0].x,window[0].y,window[0].w,window[0].h,window[0].b,window[0].d);
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
  hdc=GetDC(window[usewindow].win_hwnd);
  GetClientRect(window[usewindow].win_hwnd,&interior);
	
  BitBlt(hdc,interior.left,interior.top,interior.right-interior.left,interior.bottom-interior.top,bitcon[usewindow],0,0,SRCCOPY);

  ReleaseDC(window[usewindow].win_hwnd,hdc);
#elif defined USE_SDL
 // if(window[usewindow].display->flags & SDL_DOUBLEBUF) 
  SDL_Flip(window[usewindow].display); 
#elif defined USE_X11
   Window root;
   XGCValues gc_val;
   int ox,oy;
   unsigned int ow,oh,ob,d;
   int of;
   graphics();
   XGetGeometry(window[usewindow].display,window[usewindow].win,&root,&ox,&oy,&ow,&oh,&ob,&d);
   XGetGCValues(window[usewindow].display, window[usewindow].gc,GCFunction , &gc_val);
   of=gc_val.function;
   gc_val.function=GXcopy;

   XChangeGC(window[usewindow].display, window[usewindow].gc,  GCFunction, &gc_val);

   XFlush(window[usewindow].display);
   XCopyArea(window[usewindow].display,window[usewindow].pix,window[usewindow].win,window[usewindow].gc,0,0,ow,oh,0,0);
   handle_window(&window[usewindow]);
   gc_val.function=of;
   XChangeGC(window[usewindow].display, window[usewindow].gc,  GCFunction, &gc_val);
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

/*Eine einfache Bitmap auf den Screen bringen, hierbei gibt es 
  fordergrundfarbe, hintergrundfarbe und Graphmodi zu berücksichtigen.
  */

void put_bitmap(const char *adr,int x,int y,unsigned int w, unsigned int h) {
#ifdef USE_X11
  Pixmap bitpix=XCreateBitmapFromData(window[usewindow].display,window[usewindow].win,adr,w,h);
  XCopyPlane(window[usewindow].display,bitpix,window[usewindow].pix,window[usewindow].gc,0,0,w,h,x,y,1);
  XFreePixmap(window[usewindow].display,bitpix);
#elif defined USE_SDL
  SDL_Surface *data;
  SDL_Surface *image;
  int bpl=(w+1)>>3;
  data=SDL_CreateRGBSurface(SDL_SWSURFACE,w,h,1, 0,0,0,0);
  memrevcpy(data->pixels,adr,bpl*h);
  data->pitch=bpl;
  if(global_graphmode==GRAPHMD_TRANS) {
    SDL_SetColorKey(data,SDL_SRCCOLORKEY,0);
  }
//  memdump((char *)(data->format),sizeof(SDL_PixelFormat));
//  memdump((char *)(data->format->palette),sizeof(SDL_Palette));
//  memdump((char *)(data->format->palette->colors),2*sizeof(SDL_Color));
  unsigned char *pal=(unsigned char *)(data->format->palette->colors);
  uint32_t col=window[usewindow].bcolor;
  col>>=8;
  pal[2]=(col&0xff); col>>=8;
  pal[1]=(col&0xff); col>>=8;
  pal[0]=(col&0xff);
  col=window[usewindow].fcolor; col>>=8;
  pal[6]=(col&0xff); col>>=8;
  pal[5]=(col&0xff); col>>=8;
  pal[4]=(col&0xff);
   
  image=SDL_DisplayFormat(data);
  SDL_FreeSurface(data);
  SDL_Rect a={0,0,image->w,image->h};
  SDL_Rect b={x,y,image->w,image->h};
//  printf("putbitmap: %dx%d %d %d\n",image->w,image->h,bpl,image->pitch);
  SDL_BlitSurface(image, &a,window[usewindow].display, &b);
  SDL_FreeSurface(image);
#elif defined FRAMEBUFFER

  Fb_BlitBitmap(x,y,w,h,window[usewindow].fcolor, window[usewindow].bcolor, window[usewindow].screen->graphmode, (unsigned char *)adr);

#endif
}

#ifdef FRAMEBUFFER
  unsigned char *spix[30];
#endif
  int sgccount=0;
#ifndef USE_GEM
short form_dial(unsigned short fo_diflag,short x1,short y1,short w1,short h1, 
                                         short x2,short y2,short w2,short h2) {
#ifdef WINDOWS_NATIVE
  static HDC sgc[30];
  static HBITMAP spix[30];
#elif defined USE_X11
  static GC *sgc[30];
  static Pixmap *spix[30];
  XGCValues gc_val;
  GC pgc;
  Pixmap ppix;
#elif defined USE_SDL
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
   FB_hidex_mouse();
   FB_savecontext();
   spix[sgccount]=FB_get_image(x2-3,y2-3,w2+7,h2+7,NULL,0,0);
   // FB_showx_mouse();
#endif
#ifdef USE_X11
    sgc[sgccount]=malloc(sizeof(GC));
    pgc=XCreateGC(window[usewindow].display, window[usewindow].win, 0, &gc_val);

    XCopyGC(window[usewindow].display, window[usewindow].gc,GCForeground|
                              GCFunction |GCLineWidth |GCLineStyle|
			      GCFont, pgc);
    memcpy(sgc[sgccount],&pgc,sizeof(GC));		

    gc_val.function=GXcopy;
    XChangeGC(window[usewindow].display, window[usewindow].gc,  GCFunction, &gc_val);

    /* Hintergrund retten  */
    ppix=XCreatePixmap(window[usewindow].display,window[usewindow].win,w2+8,h2+8,window[usewindow].d);
    XCopyArea(window[usewindow].display, window[usewindow].pix,ppix,window[usewindow].gc,x2-3,y2-3,w2+7,h2+7,0,0);
    spix[sgccount]=malloc(sizeof(Pixmap));
    memcpy(spix[sgccount],&ppix,sizeof(Pixmap));
#endif
#ifdef USE_SDL
  spix[sgccount]=SDL_CreateRGBSurface(SDL_SWSURFACE,a.w,a.h,32,0,0,0,0);
  SDL_BlitSurface(window[usewindow].display, &a,spix[sgccount], &b);
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
    SDL_BlitSurface(spix[sgccount], &b,window[usewindow].display, &a);
    SDL_FreeSurface(spix[sgccount]);
    activate();
#else
#ifdef FRAMEBUFFER
    FB_hidex_mouse();
    FB_put_image(spix[sgccount],x2-3,y2-3);
    FB_restorecontext();
    // FB_showx_mouse();
#endif
#ifdef USE_X11
    XCopyArea(window[usewindow].display, *(spix[sgccount]),window[usewindow].pix,window[usewindow].gc,0,0,w2+7,h2+7,x2-3,y2-3);
    XFreePixmap(window[usewindow].display,*(spix[sgccount]));
    XCopyGC(window[usewindow].display,*sgc[sgccount],GCForeground| GCFunction |GCLineWidth |GCLineStyle| GCFont, window[usewindow].gc);
    XFreeGC(window[usewindow].display,*sgc[sgccount]);
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
#endif
#ifdef WINDOWS_NATIVE
  #define BACKSPACE ((global_keycode&0xff)==8)
  #define TAB       ((global_keycode&0xff)==9)
  #define ESC       ((global_keycode&0xff)==27)
  #define LEFT      (0)
  #define RIGHT     (0)
  #define PAGEUP    (0)
  #define PAGEDOWN  (0)
#elif defined FRAMEBUFFER
  #define BACKSPACE (event.xkey.ks==0xff08) 
  #define TAB       (event.xkey.ks==0xff09) 
  #define ESC       (event.xkey.ks==0xff1b) 
  #define LEFT      (event.xkey.ks==0xff51)
  #define RIGHT     (event.xkey.ks==0xff53)
  #define PAGEUP    (event.xkey.ks==0xff52)
  #define PAGEDOWN  (event.xkey.ks==0xff54)
#elif defined USE_X11
  #define BACKSPACE (ks==0xff08)
  #define TAB       (ks==0xff09)
  #define ESC       (ks==0xff1b)
  #define LEFT      (ks==0xff51)
  #define RIGHT     (ks==0xff53)
  #define PAGEUP    (ks==0xff52)
  #define PAGEDOWN  (ks==0xff54)
#elif defined USE_SDL
  #define BACKSPACE (event.key.keysym.sym==SDLK_BACKSPACE)
  #define TAB       (event.key.keysym.sym==SDLK_TAB)
  #define ESC       (event.key.keysym.sym==SDLK_ESCAPE)
  #define LEFT      (event.key.keysym.sym==SDLK_LEFT)   
  #define RIGHT     (event.key.keysym.sym==SDLK_RIGHT)   
  #define PAGEUP    (event.key.keysym.sym==SDLK_PAGEUP)   
  #define PAGEDOWN  (event.key.keysym.sym==SDLK_PAGEDOWN)   
#endif


#ifndef USE_GEM

short form_do(OBJECT *tree,short startob) {
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
  FB_keyboard_events(1);
  FB_show_mouse();
#endif
    /* erstes editierbare Objekt finden */

  edob=finded(tree,0,0);
 /* objc_draw(tree,0,-1,0,0); */
	
  /* Cursor plazieren */
	
  if(edob>=0) {
    ((TEDINFO *)INT2POINTER(tree[edob].ob_spec.tedinfo))->te_fontid=strlen((char *)INT2POINTER(((TEDINFO *)INT2POINTER(tree[edob].ob_spec.tedinfo))->te_ptext));
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
    XWindowEvent(window[usewindow].display, window[usewindow].win,KeyPressMask |KeyReleaseMask|ExposureMask |ButtonReleaseMask| ButtonPressMask, &event);
#elif defined USE_SDL
    e=SDL_WaitEvent(&event);
    if(e==0) return(sbut);
    while(event.type!=SDL_MOUSEBUTTONDOWN && event.type!=SDL_KEYDOWN &&
        event.type!=SDL_MOUSEBUTTONUP) { 
      handle_event(&window[usewindow],&event);
      e=SDL_WaitEvent(&event);
      if(e==0) return(sbut);
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
      XCopyArea(window[usewindow].display,window[usewindow].pix,window[usewindow].win,window[usewindow].gc,
          event.xexpose.x,event.xexpose.y,
          event.xexpose.width,event.xexpose.height,
          event.xexpose.x,event.xexpose.y);
      break;
#endif
#if defined USE_X11 || defined FRAMEBUFFER
    /* Bei Mouse-Taste: */
    case ButtonPress:
      if(event.xbutton.button==1) {
        sbut=objc_find(tree,0,7,event.xbutton.x,event.xbutton.y);
#endif
#ifdef USE_SDL
    case SDL_MOUSEBUTTONDOWN:
      if(event.button.button==1) {
         sbut=objc_find(tree,0,7,event.button.x,event.button.y);
#endif
#ifdef WINDOWS_NATIVE
    case ButtonPress:
      if(global_mousek==1) {
        sbut=objc_find(tree,0,7,global_mousex,global_mousey);
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
	    objc_draw(tree,0,-1,0,0,0,0);
	    if(edob>=0) draw_edcursor(tree,edob); 
            activate();
	    if(tree[sbut].ob_flags & EXIT) bpress=1;
	  }
	  if(tree[sbut].ob_flags & TOUCHEXIT) exitf=1;
	  if(tree[sbut].ob_flags & EDITABLE) {
	    edob=sbut;
	    ((TEDINFO *)INT2POINTER(tree[edob].ob_spec.tedinfo))->te_fontid=strlen((char *)INT2POINTER(((TEDINFO *)INT2POINTER(tree[edob].ob_spec.tedinfo))->te_ptext));
	    objc_draw(tree,0,-1,0,0,0,0);
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
	    objc_draw(tree,0,-1,0,0,0,0); activate();
	    if(tree[idx].ob_flags & EXIT) bpress=1;
          }
	  if(tree[idx].ob_flags & LASTOB) break;
	  idx++;
	}
      } else if(edob>=0) { /*Ist ein ed-object vorhanden ?*/
         int i;
         TEDINFO *ted=(TEDINFO *)INT2POINTER(tree[edob].ob_spec.tedinfo);

#ifdef FRAMEBUFFER
         if(HIBYTE(event.xkey.ks)) {
#elif defined USE_X11
         if(HIBYTE(ks)) {
#endif
           if BACKSPACE  {          /* BSP */
	     if(ted->te_fontid>0) {
	       int len=strlen((char *)INT2POINTER(ted->te_ptext));
	       i=ted->te_fontid--;
	       char *pt=INT2POINTER(ted->te_ptext);
	       while(i<len) {
	         pt[i-1]=pt[i];
		 i++;
	       }
	       pt[i-1]=0;
	       objc_draw(tree,0,-1,0,0,0,0);draw_edcursor(tree,edob);activate();
	     }
	   } else if LEFT  { /* LEFT */
	     if(ted->te_fontid>0) ted->te_fontid--;
	     objc_draw(tree,0,-1,0,0,0,0);
	     draw_edcursor(tree,edob);activate();
	   } else if RIGHT { /* RIGHT */
	     int len=strlen((char *)INT2POINTER(ted->te_ptext));
	     if(ted->te_fontid<len && ((char *)INT2POINTER(ted->te_ptext))[ted->te_fontid]) ted->te_fontid++;
             objc_draw(tree,0,-1,0,0,0,0);draw_edcursor(tree,edob);activate();
           } else if TAB  {          /* TAB */
	     /* Suche naechstes ED-Feld oder wieder das erste */
	     int cp=ted->te_fontid;
	     i=finded(tree,edob,1);
	     if(i<0) edob=finded(tree,0,0);
	     else edob=i;
	     ted=(TEDINFO *)INT2POINTER(tree[edob].ob_spec.tedinfo);
	     ted->te_fontid=min(cp,strlen((char *)INT2POINTER(ted->te_ptext)));
	     objc_draw(tree,0,-1,0,0,0,0);draw_edcursor(tree,edob);activate();
	   } else if PAGEUP {
	   /* Suche vorangehendes ED-Feld */
	     int cp=ted->te_fontid;
	     i=finded(tree,edob,-1);
	     if(i>=0) {edob=i;ted=(TEDINFO *)INT2POINTER(tree[edob].ob_spec.tedinfo);
	     ted->te_fontid=min(cp,strlen((char *)INT2POINTER(ted->te_ptext)));
	     objc_draw(tree,0,-1,0,0,0,0);draw_edcursor(tree,edob);activate();}
	   } else if PAGEDOWN  { /* Page down */
	     int cp=ted->te_fontid;
	     /* Suche naechstes ED-Feld  */
	     i=finded(tree,edob,1);
	     if(i>=0) {
	       edob=i;
	       ted=(TEDINFO *)INT2POINTER(tree[edob].ob_spec.tedinfo);
	       ted->te_fontid=min(cp,strlen((char *)INT2POINTER(ted->te_ptext)));
	       objc_draw(tree,0,-1,0,0,0,0);
	       draw_edcursor(tree,edob);
	       activate();
	     }
           } else if ESC  {   /* ESC  */
	     ((char *)INT2POINTER(ted->te_ptext))[0]=0;
	     ted->te_fontid=0;
	     objc_draw(tree,0,-1,0,0,0,0);
	     draw_edcursor(tree,edob);
	     activate();
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
	  char *pt=(char *)INT2POINTER(ted->te_ptext);
	  while(i>ted->te_fontid) {pt[i]=pt[i-1];i--;}
	
	  if(ted->te_fontid<ted->te_txtlen) {
#ifdef WINDOWS_NATIVE
            pt[ted->te_fontid]=(char)global_keycode;
#elif defined FRAMEBUFFER
	    pt[ted->te_fontid]=(char)(event.xkey.ks&0xff);
#elif defined USE_X11
	    pt[ted->te_fontid]=(char)ks;
#elif defined USE_SDL
            pt[ted->te_fontid]=(char)event.key.keysym.unicode;
#endif
	    ted->te_fontid++;
	  }	
	  objc_draw(tree,0,-1,0,0,0,0);draw_edcursor(tree,edob);activate();
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
  FB_keyboard_events(0);
#endif
  return(sbut);
}
#endif

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
#elif defined USE_X11 || defined FRAMEBUFFER
  XQueryPointer(window[usewindow].display, window[usewindow].win, &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);
#elif defined USE_SDL
  mask_return=SDL_BUTTON(SDL_GetMouseState(&win_x_return,&win_y_return))&0xffff;
#endif
  for(i=0;i<anzmenutitle;i++) {
    if(menuentry[menutitle[i]].flags) sel=i;
//    printf("%d : %03x\n",i,menutitleflag[i]);
  }

  if(win_y_return<=window[usewindow].y+window[usewindow].chh && win_y_return>=0) { /* Maus in der Titelleiste ? */
  /* Maus auf gueltigem Titel ? */
    textx=window[usewindow].chw;nr=-1;
    for(i=0;i<anzmenutitle;i++) {
      if(win_x_return>textx && win_x_return<textx+window[usewindow].chw*(menuentry[menutitle[i]].text.len+2)) nr=i;
      textx+=window[usewindow].chw*(menuentry[menutitle[i]].text.len+2);
    }
    if(nr>-1) {
     /* Wenn titel noch nicht selektiert, dann */
      if((menuentry[menutitle[nr]].flags & SELECTED)==0) {
        /* unselektiere alle Titel */
        for(i=0;i<anzmenutitle;i++) menuentry[menutitle[i]].flags=NORMAL;
        /* schliesse alle Schubladen */
         do_menu_close();
        /* selektiere Titel */
         menuentry[menutitle[nr]].flags|=SELECTED;
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
      for(i=1;i<menutitle[schubladenr+1]-1-menutitle[schubladenr];i++) {
        if(win_y_return>schubladey+(i-1)*window[usewindow].chh && win_y_return<schubladey+i*window[usewindow].chh) nr=i-1;
      }
      if(nr>-1) {
        if(!(menuentry[menutitle[schubladenr]+nr+1].flags & SELECTED) && !(menuentry[menutitle[schubladenr]+nr+1].flags & DISABLED)) {
        /* unselektiere alles  */
	  for(i=menutitle[schubladenr]+1;i<menutitle[schubladenr+1];i++) menuentry[i].flags&=~SELECTED;

        /* selektiere Eintrag */
	  menuentry[menutitle[schubladenr]+nr+1].flags|=SELECTED;
          do_menu_edraw();
        }
        /* Taste Gedrueckt ? */
        if(mask_return && !(menuentry[menutitle[schubladenr]+nr+1].flags & DISABLED)) {
          /* Menus schliessen, Titel deselektieren und mit nr zurueck */
          for(i=0;i<anzmenutitle;i++) menuentry[menutitle[i]].flags=NORMAL;
          
          for(i=menutitle[schubladenr]+1;i<menutitle[schubladenr+1];i++) menuentry[i].flags&=~SELECTED;
          do_menu_close();
          do_menu_draw();
          return(menutitle[schubladenr]+nr+1);
        }
      }
    } else {
    /* Unselektiere alles  */
      for(i=menutitle[schubladenr]+1;i<menutitle[schubladenr+1];i++) menuentry[i].flags&=~SELECTED;
    }
  }
  /* Taste Gedrueckt, dann schliesse Menu deselektiere Titel */
  if(sel>-1 && mask_return) {
    for(i=0;i<anzmenutitle;i++) menuentry[menutitle[i]].flags=NORMAL;
    do_menu_close();
    do_menu_draw();
  }
  return(-1);
}
#ifdef WINDOWS_NATIVE
HBITMAP schubladepix;
HDC schubladedc;
#elif defined USE_X11
Pixmap schubladepix;
#elif defined USE_SDL
SDL_Surface *schubladepix;
#elif defined FRAMEBUFFER
unsigned char *schubladepix;
#endif
int schubladeff=0;
int schubladenr;
int schubladex,schubladey,schubladew,schubladeh;

void do_menu_open(int nr) {
  int i,textx;
  if(schubladeff) do_menu_close();
  textx=window[usewindow].chw;
  for(i=0;i<nr;i++) textx+=window[usewindow].chw*(menuentry[menutitle[i]].text.len+2);
  schubladex=window[usewindow].x+textx-2;
  schubladey=window[usewindow].y+window[usewindow].chh+1;
  schubladew=10;
  for(i=menutitle[nr];i<menutitle[nr+1];i++) schubladew=max(schubladew,menuentry[i].text.len*window[usewindow].chw);
  
  schubladeh=window[usewindow].chh*(menutitle[nr+1]-1-menutitle[nr]-1)+2;
     /* Hintergrund retten  */
#ifdef WINDOWS_NATIVE
  schubladedc=CreateCompatibleDC(bitcon[usewindow]);
  schubladepix=CreateCompatibleBitmap(bitcon[usewindow],schubladew,schubladeh);
  SelectObject(schubladedc,schubladepix);
  BitBlt(schubladedc,0,0,schubladew,schubladeh,bitcon[usewindow],schubladex,schubladey,SRCCOPY);
#endif
#ifdef USE_X11
  schubladepix=XCreatePixmap(window[usewindow].display,window[usewindow].win,schubladew,schubladeh,window[usewindow].d);
  XCopyArea(window[usewindow].display, window[usewindow].pix,schubladepix,window[usewindow].gc,schubladex,schubladey,schubladew,schubladeh,0,0);
#endif
#ifdef USE_SDL
  SDL_Rect a={schubladex,schubladey,schubladew,schubladeh};
  SDL_Rect b={0,0,schubladew,schubladeh};
  schubladepix=SDL_CreateRGBSurface(SDL_SWSURFACE,a.w,a.h,32,0,0,0,0);
  SDL_BlitSurface(window[usewindow].display, &a,schubladepix, &b);
#endif
  #ifdef FRAMEBUFFER
     FB_hidex_mouse();
     schubladepix=FB_get_image(schubladex,schubladey,schubladew,schubladeh,NULL,0,0);
  #endif
  schubladeff=1;
  schubladenr=nr;
  do_menu_edraw();
  #ifdef FRAMEBUFFER
     FB_showx_mouse();
  #endif
}
void do_menu_edraw() {
  int i; 
  #ifdef FRAMEBUFFER
    FB_savecontext();
  #endif
    SetForeground(gem_colors[WHITE]);
#if defined USE_X11 || defined FRAMEBUFFER
    XSetLineAttributes(window[usewindow].display, window[usewindow].gc, 1, 0,0,0);
#endif
    FillRectangle(schubladex,schubladey,schubladew-1,schubladeh-1);
    for(i=menutitle[schubladenr]+1;i<menutitle[schubladenr+1]-1;i++) {
      if(menuentry[i].flags & SELECTED) {SetForeground(gem_colors[BLACK]);}
      else {SetForeground(gem_colors[WHITE]);}
      FillRectangle(schubladex,schubladey+(i-menutitle[schubladenr]-1)*window[usewindow].chh,schubladew,window[usewindow].chh);
      if(menuentry[i].flags & SELECTED) {SetForeground(gem_colors[WHITE]);SetBackground(gem_colors[BLACK]);}
      else if(menuentry[i].flags & DISABLED) {SetForeground(gem_colors[LWHITE]);SetBackground(gem_colors[WHITE]);}
      else {SetForeground(gem_colors[BLACK]);SetBackground(gem_colors[WHITE]);}
      DrawString(schubladex,schubladey+window[usewindow].baseline+window[usewindow].chh*(i-menutitle[schubladenr]-1),menuentry[i].text.pointer,menuentry[i].text.len);
      if(menuentry[i].flags & CHECKED) {
        DrawLine(schubladex+5,schubladey+window[usewindow].chh-3+window[usewindow].chh*(i-menutitle[schubladenr]-1),schubladex+2,schubladey+window[usewindow].chh-8+window[usewindow].chh*(i-menutitle[schubladenr]-1));
        DrawLine(schubladex+5,schubladey+window[usewindow].chh-3+window[usewindow].chh*(i-menutitle[schubladenr]-1),schubladex+window[usewindow].chw,schubladey+window[usewindow].chh*(i-menutitle[schubladenr]-1));
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
    XCopyArea(window[usewindow].display, schubladepix,window[usewindow].pix,window[usewindow].gc,0,0,schubladew,schubladeh
    ,schubladex,schubladey);
    XFreePixmap(window[usewindow].display,schubladepix);
#endif
#ifdef USE_SDL
  SDL_Rect a={schubladex,schubladey,schubladew,schubladeh};
  SDL_Rect b={0,0,schubladew,schubladeh};
    SDL_BlitSurface(schubladepix, &b,window[usewindow].display, &a);
    SDL_FreeSurface(schubladepix);
#endif
 #ifdef FRAMEBUFFER 
   FB_hidex_mouse();
   FB_put_image(schubladepix,schubladex,schubladey);
   FB_showx_mouse();
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
    XSetLineAttributes(window[usewindow].display, window[usewindow].gc, 1, 0,0,0);
#endif
    FillRectangle(window[usewindow].x,window[usewindow].y,window[usewindow].w,window[usewindow].chh);
    textx=window[usewindow].chw;
    for(i=0;i<anzmenutitle;i++) {
      if(menuentry[menutitle[i]].flags & SELECTED) {SetForeground(gem_colors[BLACK]);}
      else {SetForeground(gem_colors[WHITE]);}
      FillRectangle(window[usewindow].x+textx,window[usewindow].y,window[usewindow].chw*(2+menuentry[menutitle[i]].text.len),window[usewindow].chh);
      if(menuentry[menutitle[i]].flags & SELECTED) {SetForeground(gem_colors[WHITE]);SetBackground(gem_colors[BLACK]);}
      else {SetForeground(gem_colors[BLACK]);SetBackground(gem_colors[WHITE]);}
      DrawString(window[usewindow].x+textx+window[usewindow].chw,window[usewindow].y+window[usewindow].baseline,menuentry[menutitle[i]].text.pointer,menuentry[menutitle[i]].text.len);
      textx+=window[usewindow].chw*(menuentry[menutitle[i]].text.len+2);
    }
    SetForeground(gem_colors[BLACK]);
    DrawLine(window[usewindow].x,window[usewindow].y+window[usewindow].chh,window[usewindow].x+window[usewindow].w,window[usewindow].y+window[usewindow].chh);
 #ifdef FRAMEBUFFER
  FB_restorecontext();
  FB_show_mouse();
 #endif
  activate();
}

#define ANZSHOW 13
#define FWW 34

#ifndef USE_GEM
/* Fileselector-Routine. Dem GEM nachempfunden.
(c) Markus Hoffmann   1998               */
#define FT_NORMAL    0
#define FT_DIR    0x10
#define FT_LINK   0x20


typedef struct fileinfo {
    char *name;       /* The file name. */
    int typ;
    struct stat dstat;
} FINFO;


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
    ted=(TEDINFO *)INT2POINTER(objects[18+2*i].ob_spec.tedinfo);
    objects[18+2*i+1].ob_spec.index&=0x00ffffff;
    if(j<anzfiles) {
      if(filenamensel[j]==1) objects[18+2*i].ob_state=SELECTED;
      else objects[18+2*i].ob_state=NORMAL;
      ted->te_ptext=(CHAR_P)POINTER2INT(filenamen[j].name);
      objects[18+2*i].ob_flags=SELECTABLE|TOUCHEXIT|RBUTTON;
      if(filenamen[j].typ & FT_DIR) objects[18+2*i+1].ob_spec.index|=0x44000000;
      else objects[18+2*i+1].ob_spec.index|=0x20000000;
    } else {
      ted->te_ptext=(CHAR_P)POINTER2INT("");
      objects[18+2*i].ob_state=NORMAL;
      objects[18+2*i].ob_flags=NONE;
      objects[18+2*i+1].ob_spec.index|=0x20000000;
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
  if(!fileinfos || !pfad || !mask) return(0);
  DIR *dp;
  struct dirent *ep;
  int anzfiles=0;
  char filename[256+strlen(pfad)];
#ifdef DEBUG
  printf("Read_dir: %s/%s\n",pfad,mask);
#endif
  snprintf(filename,sizeof(filename),"%s/",pfad);
  dp=opendir(filename);
  if(dp!=NULL) {
    while((ep=readdir(dp)) && (anzfiles<maxentries)) {
      snprintf(filename,sizeof(filename),"%s/%s",pfad,ep->d_name);
      if(stat(filename, &fileinfos[anzfiles].dstat)==0) {
        if(S_ISDIR(fileinfos[anzfiles].dstat.st_mode)) {   /* ist es Directory ? */
	  if(strcmp(ep->d_name,".") && strcmp(ep->d_name,"..")) {
	    fileinfos[anzfiles].name=malloc(128);
	    strncpy(fileinfos[anzfiles].name,ep->d_name,128);
	    fileinfos[anzfiles++].typ=FT_DIR;
	  }
        } else {  /* kein Directory */
	  if (fnmatch(mask,ep->d_name,FNM_PERIOD)==0) {
	    fileinfos[anzfiles].name=malloc(128);
	    strncpy(fileinfos[anzfiles].name,ep->d_name,128);
	    fileinfos[anzfiles++].typ=FT_NORMAL;
	  }
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
#endif
#define FILESELECT_NOBJ (18+2*ANZSHOW)
#define FILESELECT_NTED (4+ANZSHOW)

#define FILESELECT_OK       1
#define FILESELECT_CANCEL   2
#define FILESELECT_HOME     3
#define FILESELECT_BACK     6
#define FILESELECT_UP       7
#define FILESELECT_DOWN     8
#define FILESELECT_INFO     9
#define FILESELECT_MASK    10
#define FILESELECT_SCALERP 15
#define FILESELECT_SCALER  16

char *fileselector(const char *titel, const char *pfad, const char *sel) {
  int sbut=-1;
  int i=0;
  /* Das ist der Speicherbereich f�r Objekt-strukturen. 
  Bei 64bit systemem darf er nicht auf dem Stack liegen.*/
  char *buffer=malloc(FILESELECT_NOBJ*sizeof(OBJECT)+FILESELECT_NTED*sizeof(TEDINFO)+128*5);
  bzero(buffer,FILESELECT_NOBJ*sizeof(OBJECT)+FILESELECT_NTED*sizeof(TEDINFO)+128*5);
  OBJECT  *objects=(OBJECT *)buffer;
  TEDINFO *tedinfo=(TEDINFO *)(buffer+FILESELECT_NOBJ*sizeof(OBJECT));
  char *btitel =(char *)(buffer+FILESELECT_NOBJ*sizeof(OBJECT)+FILESELECT_NTED*sizeof(TEDINFO));
  char *dpfad  =(char *)(buffer+FILESELECT_NOBJ*sizeof(OBJECT)+FILESELECT_NTED*sizeof(TEDINFO)+128);
  char *auswahl=(char *)(buffer+FILESELECT_NOBJ*sizeof(OBJECT)+FILESELECT_NTED*sizeof(TEDINFO)+128*2);
  char *mask   =(char *)(buffer+FILESELECT_NOBJ*sizeof(OBJECT)+FILESELECT_NTED*sizeof(TEDINFO)+128*3);
  char *feld1  =(char *)(buffer+FILESELECT_NOBJ*sizeof(OBJECT)+FILESELECT_NTED*sizeof(TEDINFO)+128*4);
  char *ergebnis;
  strncpy(auswahl,sel,127);
  if(titel!=NULL) strncpy(btitel,titel,60);
  else strcpy(btitel,"FILESELECT");

  /* Change path separator '\' to '/'*/
  while(dpfad[i]) {
    if(dpfad[i]=='\\') dpfad[i]='/';
    i++;
  }
  
  wort_sepr(pfad,'/',0,dpfad, mask);
  
  strcpy(feld1,pfad);
#ifdef USE_GEM
  char a;
  short sh=-1;
  for(i=0;i<127;i++) {
    a=pfad[i];
    if(a==0) break;
    else if(a=='/') a='\\';
    else a=toupper(a);
    dpfad[i]=a;
  }
  dpfad[i]=0;
  fsel_exinput(dpfad,auswahl,&sh,titel);
  sbut=sh;
#else

#if defined USE_X11 || defined FRAMEBUFFER
  XEvent event;
#elif defined USE_SDL
  SDL_Event event;
  int e;
#endif
  FINFO filenamen[MAXANZFILES];

  int filenamensel[MAXANZFILES];

  int anzfiles,showstart=0;


  const TEDINFO t[4]={
    {0,0,0,FONT_BIGIBM,0,TE_CNTR,0x1200,0,0,0,0},
    {0,0,0,FONT_IBM,0,TE_CNTR,0x113a,0,2,0,FWW},
    {0,0,0,FONT_IBM,0,TE_LEFT,0x1100,0,0,128,50},
    {0,0,0,FONT_IBM,0,TE_LEFT,0x1100,0,0,128,20}
  };
  memcpy(tedinfo,t,4*sizeof(TEDINFO));

  tedinfo[0].te_ptext= (CHAR_P)POINTER2INT(btitel);
  tedinfo[1].te_ptext= (CHAR_P)POINTER2INT(mask);
  tedinfo[2].te_ptext= (CHAR_P)POINTER2INT(feld1);
  tedinfo[3].te_ptext= (CHAR_P)POINTER2INT(auswahl);
  tedinfo[0].te_ptmplt=
  tedinfo[0].te_pvalid=
  tedinfo[1].te_ptmplt=
  tedinfo[1].te_pvalid=(CHAR_P)POINTER2INT("");
  tedinfo[2].te_ptmplt=
  tedinfo[3].te_ptmplt=(CHAR_P)POINTER2INT("__________________________________________________");
  tedinfo[2].te_pvalid=
  tedinfo[3].te_pvalid=(CHAR_P)POINTER2INT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
  
  
  const OBJECT o[18]={
/* 0*/  {-1,1,14,G_BOX, NONE, OUTLINED, {0x00021100}, 0,0,54,23},
#ifdef FRAMEBUFFER
/* 1*/  {2,-1,-1,G_BUTTON, SELECTABLE|DEFAULT|EXIT,NORMAL ,{(LONG)POINTER2INT("OK")}, 42,15,9,3},
#else
/* 1*/  {2,-1,-1,G_BUTTON, SELECTABLE|DEFAULT|EXIT,NORMAL ,{(LONG)POINTER2INT("OK")}, 42,18,9,1},
#endif
#ifdef GERMAN
/* 2*/  {3,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, {(LONG)POINTER2INT("ABBRUCH")},   42,20,9,1},
#else
#ifdef FRAMEBUFFER
/* 2*/  {3,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, {(LONG)POINTER2INT("CANCEL")},    42,19,9,3},
#else
/* 2*/  {3,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, {(LONG)POINTER2INT("CANCEL")},    42,20,9,1},
#endif
#endif
#ifdef FRAMEBUFFER
/* 3*/  {4,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL,{ (LONG)POINTER2INT("HOME")},      42,11,9,2},
#else
/* 3*/  {4,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, {(LONG)POINTER2INT("HOME")},      42,12,9,1},
#endif
/* 4*/  {5,-1,-1,G_TEXT,  NONE, NORMAL, {(LONG)POINTER2INT(&tedinfo[0])},      1,1,52,1},
/* 5*/  {11,6,17,G_BOX, NONE, NORMAL, {0x00fe1120}, 2,8,FWW+2,ANZSHOW+1},
/* 6*/  {7,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, {(LONG)POINTER2INT("<")}, 0,0,2,1},
/* 7*/  {8,-1,-1,G_BUTTON, SELECTABLE|TOUCHEXIT, NORMAL, {(LONG)POINTER2INT("^")},         FWW,1,2,1},
/* 8*/  {9,-1,-1,G_BUTTON, SELECTABLE|TOUCHEXIT, NORMAL, {(LONG)POINTER2INT("v")},   FWW,ANZSHOW,2,1},
/* 9*/  {10,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, {(LONG)POINTER2INT("?")},         FWW,0,    2,1},
/*10*/  {15,-1,-1,G_BOXTEXT, SELECTABLE|EXIT, NORMAL, {(LONG)POINTER2INT(&tedinfo[1])},  2,0,FWW-2,1},
#ifdef GERMAN
/*11*/  {12,-1,-1,G_STRING,NONE, NORMAL, {(LONG)POINTER2INT("Pfad:")}, 2,3,2,1},
/*12*/  {13,-1,-1,G_STRING,NONE, NORMAL, {(LONG)POINTER2INT("Auswahl:")}, 2,5,2,1},
#else
/*11*/  {12,-1,-1,G_STRING,NONE, NORMAL, {(LONG)POINTER2INT("Directory:")}, 2,3,2,1},
/*12*/  {13,-1,-1,G_STRING,NONE, NORMAL, {(LONG)POINTER2INT("Selection:")}, 2,5,2,1},
#endif
/*13*/  {14,-1,-1,G_FTEXT,  EDITABLE, NORMAL, {(LONG)POINTER2INT(&tedinfo[2])}, 2,4,50,1},
/*14*/  {0,-1,-1,G_FTEXT,  EDITABLE, NORMAL, {(LONG)POINTER2INT(&tedinfo[3])}, 2,6,50,1},
/*15*/  {17,16,16,G_BOX, TOUCHEXIT, NORMAL, {0x00ff1459}, FWW,2,2,ANZSHOW-2},
/*16*/  {15,-1,-1,G_BOX, TOUCHEXIT, NORMAL, {0x00ff1100}, 0,0,2,2},
/*17*/  {5,18,17+2*ANZSHOW,G_BOX, NONE, NORMAL, {0x00ff1100}, 0,1,FWW,ANZSHOW}
  };
  memcpy(objects,o,18*sizeof(OBJECT));
  
  short x,y,w,h;
  int obx,oby;
  int k;

#ifdef DEBUG
  printf("**2fsel_input: ANZSHOW=%d btitel=%s\n",ANZSHOW,btitel);
#endif

  tedinfo[0].te_txtlen=strlen(btitel);
  tedinfo[0].te_tmplen=0;
  for(i=0;i<ANZSHOW;i++){
    tedinfo[4+i].te_ptext= (CHAR_P)POINTER2INT("________________________________");
    tedinfo[4+i].te_ptmplt=(CHAR_P)POINTER2INT("________________________________");
    tedinfo[4+i].te_pvalid=(CHAR_P)POINTER2INT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
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
    objects[18+2*i].ob_spec.tedinfo=(TEDINFO_P)POINTER2INT(&tedinfo[4+i]);
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
    objects[18+2*i+1].ob_spec.index=0x20000100;
    objects[18+2*i+1].ob_x=0;
    objects[18+2*i+1].ob_y=i;
    objects[18+2*i+1].ob_width=2;
    objects[18+2*i+1].ob_height=1;
  }
  objects[FILESELECT_NOBJ-1].ob_next=17;
  objects[FILESELECT_NOBJ-1].ob_flags|=LASTOB;
  graphics();
  gem_init();  /*Fonts und Farben*/ 
  for(i=0;i<FILESELECT_NOBJ;i++){
    objects[i].ob_x*=window[usewindow].chw;
    objects[i].ob_y*=window[usewindow].chh;
    objects[i].ob_width*=window[usewindow].chw;
    objects[i].ob_height*=window[usewindow].chh;
  }

  anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
  sort_dir(filenamen,anzfiles);


  for(i=0;i<anzfiles;i++) filenamensel[i]=(strcmp(filenamen[i].name,auswahl)==0);


  tedinfo[2].te_fontid=strlen(pfad);
  tedinfo[3].te_fontid=strlen(sel);
  tedinfo[2].te_fontsize=0;
  tedinfo[3].te_fontsize=0;

  make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
  make_scaler(objects,anzfiles,showstart);


  form_center(objects, &x,&y,&w,&h);  /* Objektbaum Zentrieren */
  form_dial(0,0,0,0,0,x,y,w,h);
  form_dial(1,0,0,0,0,x,y,w,h);
  while(sbut!=FILESELECT_OK && sbut!=FILESELECT_CANCEL) {
    objc_draw(objects,0,-1,0,0,0,0);
    sbut=form_do(objects,0);
    if(sbut==FILESELECT_HOME) {    /* HOME */
      char buf[128];
      if(getcwd(buf,128)!=NULL) {

        strcpy(dpfad,buf);
	sprintf(feld1,"%s/%s",dpfad,mask);

	tedinfo[2].te_fontid=strlen(feld1);
	tedinfo[2].te_fontsize=0;

	anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
	for(k=0;k<anzfiles;k++) filenamensel[k]=0;
        sort_dir(filenamen,anzfiles);
	showstart=max(0,min(showstart,anzfiles-ANZSHOW));
	make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
	make_scaler(objects,anzfiles,showstart);

      }
      objects[sbut].ob_state=NORMAL;
    } else if(sbut==FILESELECT_BACK) {    /* < */
      char buf[128];
      wort_sepr(dpfad,'/',0,dpfad, buf);
      sprintf(feld1,"%s/%s",dpfad,mask);
      tedinfo[2].te_fontid=strlen(feld1);
      tedinfo[2].te_fontsize=0;
      anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
      sort_dir(filenamen,anzfiles);
      for(k=0;k<anzfiles;k++) filenamensel[k]=0;
      showstart=max(0,min(showstart,anzfiles-ANZSHOW));
      make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
      make_scaler(objects,anzfiles,showstart);objects[sbut].ob_state=NORMAL;
    } else if(sbut==FILESELECT_UP) {    /* ^ */
      if(showstart) {
        showstart--;
        make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
	make_scaler(objects,anzfiles,showstart);
      }
      objects[sbut].ob_state=NORMAL;
    } else if(sbut==FILESELECT_DOWN) {    /* v */
      if(showstart<anzfiles-ANZSHOW) {
        showstart++;
        make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
        make_scaler(objects,anzfiles,showstart);
      }
      objects[sbut].ob_state=NORMAL;
    } else if(sbut==FILESELECT_INFO) {    /* ? */
      char buf[128];
      snprintf(buf,sizeof(buf),"[1][%d Bytes in %d Files.][ OK ]",dir_bytes(filenamen,anzfiles),anzfiles);
      form_alert(1,buf);
      objects[sbut].ob_state=NORMAL;
    } else if(sbut==FILESELECT_MASK) {    /* MASK */
      wort_sepr((char *)INT2POINTER(tedinfo[2].te_ptext),'/',0,dpfad, mask);
      anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
      sort_dir(filenamen,anzfiles);
      showstart=max(0,min(showstart,anzfiles-ANZSHOW));
      make_filelist(objects,filenamen, filenamensel,anzfiles,showstart);
      make_scaler(objects,anzfiles,showstart);objects[sbut].ob_state=NORMAL;
    } else if(sbut==FILESELECT_SCALERP) {    /* Scalerhintergrund */
#ifdef USE_SDL
      e=SDL_WaitEvent(&event);
      if(e==0) break;
      while(event.type!=SDL_MOUSEBUTTONUP) { 
        handle_event(&window[usewindow],&event);
        e=SDL_WaitEvent(&event);
        if(e==0) break;
      }
      if(e==0) break;
      relobxy(objects,FILESELECT_SCALER,&obx, &oby);
      if(event.button.y<oby) {
	     showstart=max(0,min(showstart-ANZSHOW,anzfiles-ANZSHOW));
             make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
             make_scaler(objects,anzfiles,showstart);
      } else if(event.button.y>oby+objects[FILESELECT_SCALER].ob_height) {
	     showstart=max(0,min(showstart+ANZSHOW,anzfiles-ANZSHOW));
             make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
             make_scaler(objects,anzfiles,showstart);
      }
#elif defined USE_X11 || defined FRAMEBUFFER
      XWindowEvent(window[usewindow].display, window[usewindow].win,
       ButtonReleaseMask|ExposureMask , &event);
      switch (event.type) {

    /* Das Redraw-Event */
      case Expose:
#ifdef USE_X11
        XCopyArea(window[usewindow].display,window[usewindow].pix,window[usewindow].win,window[usewindow].gc,
          event.xexpose.x,event.xexpose.y,
          event.xexpose.width,event.xexpose.height,
          event.xexpose.x,event.xexpose.y);
#endif
        break;
	case ButtonRelease:
	  relobxy(objects,FILESELECT_SCALER,&obx, &oby);
           if(event.xbutton.y<oby) {
	     showstart=max(0,min(showstart-ANZSHOW,anzfiles-ANZSHOW));
             make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
             make_scaler(objects,anzfiles,showstart);
	   } else if(event.xbutton.y>oby+objects[FILESELECT_SCALER].ob_height) {
	     showstart=max(0,min(showstart+ANZSHOW,anzfiles-ANZSHOW));
             make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
             make_scaler(objects,anzfiles,showstart);
	   }
	break;
      }
      #endif
    } else if(sbut==FILESELECT_SCALER) {    /* ScalerSchieber */
      int ex=0,root_x_return,root_y_return,win_x_return,win_y_return;
      unsigned int mask_return;
      int ssold=showstart;
      int sssold=showstart;
      #ifdef USE_X11
      Window root_return,child_return;
      #endif
      #if defined USE_X11 || defined FRAMEBUFFER || defined USE_SDL
       XQueryPointer(window[usewindow].display, window[usewindow].win, &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);
      #endif
      #if defined USE_X11 || defined FRAMEBUFFER 

      while(ex==0) {
       XWindowEvent(window[usewindow].display, window[usewindow].win,
       ButtonReleaseMask|PointerMotionMask|ExposureMask , &event);
      switch (event.type) {

    /* Das Redraw-Event */
      case Expose:
#ifdef USE_X11
        XCopyArea(window[usewindow].display,window[usewindow].pix,window[usewindow].win,window[usewindow].gc,
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
		     (objects[FILESELECT_SCALERP].ob_height*(1-min(1,(float)(ANZSHOW)/anzfiles)))
		     *(anzfiles-ANZSHOW);
	  showstart=max(0,min(showstart,anzfiles-ANZSHOW));
          if(showstart!=sssold) {
            make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
            make_scaler(objects,anzfiles,showstart);
            objc_draw(objects,0,-1,0,0,0,0);
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
          snprintf(buf,sizeof(buf),"%s/%s",dpfad,filenamen[showstart+j].name);
          strcpy(dpfad,buf);
          sprintf(feld1,"%s/%s",dpfad,mask);
	  tedinfo[2].te_fontid=strlen(feld1);
	  tedinfo[2].te_fontsize=0;
          anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
          sort_dir(filenamen,anzfiles);
	  for(k=0;k<anzfiles;k++) filenamensel[k]=0;
          showstart=max(0,min(showstart,anzfiles-ANZSHOW));
	  make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
	  make_scaler(objects,anzfiles,showstart);
        } else {
          for(k=0;k<anzfiles;k++) filenamensel[k]=0;
	  filenamensel[showstart+j]=1;
	  strcpy((char *)INT2POINTER(tedinfo[3].te_ptext),filenamen[showstart+j].name);
	  tedinfo[3].te_fontid=strlen((char *)INT2POINTER(tedinfo[3].te_ptext));
	  tedinfo[3].te_fontsize=0;
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
#endif
#ifdef USE_GEM
  unsigned short pos=0;
  for(i=0;i<strlen(dpfad);i++) {
    a=dpfad[i];
    if(a=='\\') {dpfad[i]=a='/';}
    if(a=='/') pos=i;
  }
  dpfad[pos]=0;
#else
  if(anzfiles) {
    int i;
    for(i=0;i<anzfiles;i++) free(filenamen[i].name);
  }
#endif
  ergebnis=malloc(strlen(dpfad)+strlen(auswahl)+2);
  if(sbut==FILESELECT_OK) sprintf(ergebnis,"%s/%s",dpfad,auswahl);
  else *ergebnis=0;
  free(buffer);
  return(ergebnis);
}
static void make_list(OBJECT *objects,STRING *name,int *filenamensel,int anzfiles,int showstart){
  int i,j;
  TEDINFO *ted;
  for(i=0;i<ANZSHOW;i++) {
    j=showstart+i;
    ted=(TEDINFO *)INT2POINTER(objects[15+2*i].ob_spec.tedinfo);
    objects[15+2*i+1].ob_spec.index&=0x00ffffff;
      if(j<anzfiles) {
        if(filenamensel[j]==1) objects[15+2*i].ob_state=SELECTED;
        else objects[15+2*i].ob_state=NORMAL;
        ted->te_ptext=(CHAR_P)POINTER2INT(name[j].pointer);
	objects[15+2*i].ob_flags=SELECTABLE|TOUCHEXIT|RBUTTON;
	objects[15+2*i+1].ob_spec.index|=0x20000000;
      } else {
        ted->te_ptext=(CHAR_P)POINTER2INT("");
        objects[15+2*i].ob_state=NORMAL;
        objects[15+2*i].ob_flags=NONE;	
        objects[15+2*i+1].ob_spec.index|=0x20000000;
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

#define LISTSELECT_NOBJ (15+2*ANZSHOW)
#define LISTSELECT_NTED (4+ANZSHOW)

int lsel_input(const char *titel, STRING *strs,int anzfiles,int sel) {
  /* Das ist der Speicherbereich f�r Objekt-strukturen. 
  Bei 64bit systemem darf er nicht auf dem Stack liegen.*/
  char *buffer=malloc(LISTSELECT_NOBJ*sizeof(OBJECT)+LISTSELECT_NTED*sizeof(TEDINFO)+128*5);
  bzero(buffer,LISTSELECT_NOBJ*sizeof(OBJECT)+LISTSELECT_NTED*sizeof(TEDINFO)+128*5);
  OBJECT  *objects=(OBJECT *)buffer;
  TEDINFO *tedinfo=(TEDINFO *)(buffer+LISTSELECT_NOBJ*sizeof(OBJECT));
  char *btitel =(char *)(buffer+LISTSELECT_NOBJ*sizeof(OBJECT)+LISTSELECT_NTED*sizeof(TEDINFO));
  if(titel!=NULL) strncpy(btitel,titel,38);
  else strcpy(btitel,"LISTSELCT");

  char *mask=malloc(128);
  char *findmsk=malloc(50);

  int ergebnis=0;
  int i,k;
  int sbut=-1;
#if defined USE_X11 || defined FRAMEBUFFER
  XEvent event;
#endif
  int filenamensel[anzfiles];
  int showstart=0;
  TEDINFO t[4]={
    {(CHAR_P)POINTER2INT(btitel),(CHAR_P)POINTER2INT(""),(CHAR_P)POINTER2INT(""),FONT_IBM,0,TE_CNTR,0x1200,0,0,0,0},
    {(CHAR_P)POINTER2INT(mask),  (CHAR_P)POINTER2INT(""),(CHAR_P)POINTER2INT(""),FONT_IBM,0,TE_CNTR,0x113a,0,2,0,FWW-2},
    {(CHAR_P)POINTER2INT(findmsk),
     (CHAR_P)POINTER2INT("________________________"),
     (CHAR_P)POINTER2INT("XXXXXXXXXXXXXXXXXXXXXXXX"),FONT_IBM,0,TE_LEFT,0x1100,0,0,128,24}
  };
  memcpy(tedinfo,t,4*sizeof(TEDINFO));
  

  OBJECT o[15]={
/* 0*/  {-1,1, 6, G_BOX,     NONE, OUTLINED, {(LONG)0x00021100}, 0,0,40,21},
/* 1*/  {2,-1,-1, G_BUTTON,  SELECTABLE|DEFAULT|EXIT,NORMAL ,{(LONG)POINTER2INT("OK")}, 10,19,9,1},
/* 2*/  {3,-1,-1, G_BUTTON,  SELECTABLE|EXIT,      NORMAL, {(LONG)POINTER2INT("CANCEL")},    24,19,9,1},
/* 3*/  {4,-1,-1, G_TEXT,    NONE,                 NORMAL, {(LONG)POINTER2INT(&tedinfo[0])},      1,1,38,1},
/* 4*/  {5,-1,-1, G_FTEXT,   EDITABLE,             NORMAL, {(LONG)POINTER2INT(&tedinfo[2])}, 12,3,FWW-4-12,1},
/* 5*/  {6,-1,-1, G_STRING,  NONE,                 NORMAL, {(LONG)POINTER2INT("Find:")}, 2,3,2,1},
/* 6*/  {0, 7,14, G_BOX,     NONE,                 NORMAL, {0x00fe1120}, 2,4,FWW+2,ANZSHOW+1},
/* 7*/  {8,-1,-1, G_BUTTON,  SELECTABLE|EXIT,      NORMAL, {(LONG)POINTER2INT("<")}, 0,0,2,1},
/* 8*/  {9,-1,-1, G_BUTTON,  SELECTABLE|TOUCHEXIT, NORMAL, {(LONG)POINTER2INT("^")}, FWW,1,2,1},
/* 9*/  {10,-1,-1,G_BUTTON,  SELECTABLE|TOUCHEXIT, NORMAL, {(LONG)POINTER2INT("v")}, FWW,ANZSHOW,2,1},
/*10*/  {11,-1,-1,G_BUTTON,  SELECTABLE|EXIT,      NORMAL, {(LONG)POINTER2INT("?")},     FWW,0,2,1},
/*11*/  {12,-1,-1,G_BOXTEXT, SELECTABLE|EXIT,      NORMAL, {(LONG)POINTER2INT(&tedinfo[1])},2,0,FWW-2,1},
/*12*/  {14,13,13,G_BOX,     TOUCHEXIT, NORMAL, {0x00ff1459}, FWW,2,2,ANZSHOW-2},
/*13*/  {12,-1,-1,G_BOX,     TOUCHEXIT, NORMAL, {0x00ff1100}, 0,0,2,2},
/*14*/  {6,15,14+2*ANZSHOW,G_BOX, NONE, NORMAL, {0x00ff1100}, 0,1,FWW,ANZSHOW}
  };
  memcpy(objects,o,15*sizeof(OBJECT));
  short x,y,w,h;
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



  strcpy(findmsk,"");
  strcpy(mask,"*");

  tedinfo[0].te_txtlen=strlen(btitel);
  tedinfo[0].te_tmplen=0;
  tedinfo[1].te_fontid=24;
  tedinfo[1].te_fontsize=strlen(mask);
  tedinfo[2].te_txtlen=24;
  tedinfo[2].te_tmplen=24;
  tedinfo[2].te_fontid=24;
  tedinfo[2].te_fontsize=strlen(findmsk);

  for(i=0;i<ANZSHOW;i++){
    tedinfo[4+i].te_ptext =(CHAR_P)POINTER2INT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    tedinfo[4+i].te_ptmplt=(CHAR_P)POINTER2INT("______________________________");
    tedinfo[4+i].te_pvalid=(CHAR_P)POINTER2INT("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
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
    objects[15+2*i].ob_spec.tedinfo=(TEDINFO_P)POINTER2INT(&tedinfo[4+i]);
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
    objects[15+2*i+1].ob_spec.index=0x20000100;
    objects[15+2*i+1].ob_x=0;
    objects[15+2*i+1].ob_y=i;
    objects[15+2*i+1].ob_width=2;
    objects[15+2*i+1].ob_height=1;
  }
  objects[LISTSELECT_NOBJ-1].ob_next=14;
  objects[LISTSELECT_NOBJ-1].ob_flags|=LASTOB;

  graphics();
  gem_init();
  for(i=0;i<LISTSELECT_NOBJ;i++){
    objects[i].ob_x*=window[usewindow].chw;
    objects[i].ob_y*=window[usewindow].chh;
    objects[i].ob_width*=window[usewindow].chw;
    objects[i].ob_height*=window[usewindow].chh;
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
    objc_draw(objects,0,-1,0,0,w,h);
    sbut=form_do(objects,0);
    if(sbut==LISTSELECT_BACK) {    /* < */
//      tedinfo[2].te_fontid=strlen(feld1);
//      tedinfo[2].te_fontsize=0;
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
      snprintf(buf,sizeof(buf),"[1][%d list entrys.][ OK ]",anzfiles);
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
      XWindowEvent(window[usewindow].display, window[usewindow].win,
       ButtonReleaseMask|ExposureMask , &event);
      switch (event.type) {

    /* Das Redraw-Event */
      case Expose:
#ifdef USE_X11
        XCopyArea(window[usewindow].display,window[usewindow].pix,window[usewindow].win,window[usewindow].gc,
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
       XQueryPointer(window[usewindow].display, window[usewindow].win, &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);

      while(ex==0) {
       XWindowEvent(window[usewindow].display, window[usewindow].win,
       ButtonReleaseMask|PointerMotionMask|ExposureMask , &event);
      switch (event.type) {

    /* Das Redraw-Event */
      case Expose:
#ifndef FRAMEBUFFER
        XCopyArea(window[usewindow].display,window[usewindow].pix,window[usewindow].win,window[usewindow].gc,
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
            objc_draw(objects,0,-1,0,0,w,h);
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

  free(mask);
  free(findmsk);
  free(buffer);
  return(ergebnis);
}
#endif /* nographics */



void do_sizew(WINDOWDEF *ww,int w,int h) {
  ww->w=w;
  ww->h=h;
#ifdef WINDOWS_NATIVE
  RECT interior;
  GetClientRect(ww->win_hwnd,&interior);
  MoveWindow(ww->win_hwnd,interior.left,interior.top,w,h,1);
#elif defined USE_X11
  Pixmap pixi;
  Window root;
  int ox,oy;
  unsigned int ow,oh,ob,d;

  XGetGeometry(ww->display,ww->win,&root,&ox,&oy,&ow,&oh,&ob,&d);
  pixi=XCreatePixmap(ww->display, ww->win, w, h, d);
  XResizeWindow(ww->display, ww->win, w, h);
  XCopyArea(ww->display,ww->pix,pixi,ww->gc,0,0,min(w,ow),min(h,oh),0,0);
  XFreePixmap(ww->display,ww->pix); 
  ww->pix=pixi;
  XFlush(ww->display);
#elif defined USE_SDL
  Uint32 flags;
  flags=(ww->display)->flags;
  SDL_FreeSurface(ww->display);
  if(!(ww->display=SDL_SetVideoMode(w, h, 32, flags))) {
    printf("cannot open SDL surface \n");
    SDL_Quit();
    return;
  }
#endif
}
