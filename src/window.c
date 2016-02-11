
/* window.c      Window-Routinen (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
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
  #include <X11/XWDFile.h>
#endif

#include "defs.h"
#include "window.h"


/* globale Variablen */
/* GEM-Globals   */

int chw=4,chh=8,baseline=7,depth=8;
const struct { int r,g,b;} gem_colordefs[]={
{65535,65535,65535},  /* WHITE */
{0,0,0},/*BLACK  */
{65535,0,0},/* RED */
{0,65535,0},/* GREEN */
{0,0,65535},/* BLUE */
{0,65535,65535},/* CYAN */
{65535,65535,0},/* YELLOW */
{65535,0,65535},/* MAGENTA */
{40000,40000,40000},/* LWHITE */
{20000,20000,20000},/* LBLACK */
{65535,32000,32000},/* LRED */
{32000,65535,32000},/* LGREEN */
{32000,32000,65535},/* LBLUE */
{32000,65535,65535},/* LCYAN  */
{65535,65535,32000},/* LYELLOW */
{65535,32000,65535},/* LMAGENTA */
};
int gem_colors[16];

ARECT sbox;

#ifndef NOGRAPHICS
char wname[MAXWINDOWS][80];
char iname[MAXWINDOWS][80];  
#ifdef WINDOWS
  HANDLE keyevent=INVALID_HANDLE_VALUE; /* handle for win thread event */
  HANDLE buttonevent=INVALID_HANDLE_VALUE; /* handle for win thread event */
  HANDLE motionevent=INVALID_HANDLE_VALUE; /* handle for win thread event */
  HANDLE tsync=INVALID_HANDLE_VALUE; /* handle for win thread event */
#else
  char *display_name = NULL;  /* NULL: Nimm Argument aus setenv DISPLAY */
  #include "bitmaps/bombe_gross.bmp"

void fetch_icon_pixmap(int nummer) {
  GC gc;XGCValues gc_val;
  char t[10];
  sprintf(t,"%2d",nummer);
  icon_pixmap[nummer]=XCreateBitmapFromData(display[nummer],win[nummer],
    bombe_gross_bits,bombe_gross_width,bombe_gross_height);  
  gc = XCreateGC(display[nummer], icon_pixmap[nummer], 0, &gc_val);
  XSetForeground(display[nummer], gc, 0);
  XDrawString(display[nummer],icon_pixmap[nummer],gc,9,24,t,strlen(t));
  XFreeGC(display[nummer],gc);
}
#endif  


int create_window(char *title, char* info,unsigned int x,unsigned int y,unsigned int w,unsigned int h) {
  int nummer=0;
  while(winbesetzt[nummer] && nummer<MAXWINDOWS) nummer++;
  if(nummer>=MAXWINDOWS) {
      printf("No more windows !\n");
      return(-2);
  }
  printf("createwindow %d\n",nummer);
  return(create_window2(nummer,title,info,x,y,w,h));
}
#ifdef WINDOWS
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
        printf("ERROR: Window existiert gar nicht !\n");
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
#endif
int create_window2(int nummer,char *title, char* info,unsigned int x,unsigned int y,unsigned int w,unsigned int h) {
  
  int screen_num;              /* Ein Server kann mehrere Bildschirme haben */
  unsigned long border=4,foreground,background;
  int i,d,b;
#ifdef WINDOWS
    static class_reg=0;
#endif
  if(winbesetzt[nummer]) {
    printf("X11-Basic: Window %d already open !\n",nummer);
#ifdef WINDOWS
  MessageBox(NULL,"X11-Basic: Window already open !" , "Error!",
			MB_ICONEXCLAMATION | MB_OK);    
#endif			
 return(-1); 
  
  } else {
#ifdef WINDOWS
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
#else
  XGCValues gc_val;            /* */
  Window root;
  char *wn;
  char *in; 
  XTextProperty win_name, icon_name;
  char *agv[1];


    strcpy(wname[nummer],title);
    strcpy(iname[nummer],info);
    wn=wname[nummer];
    in=iname[nummer];

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

  XSetForeground(display[nummer], gc[nummer], foreground);
  winbesetzt[nummer]=1;
#endif  
  }
  return(nummer);
}


void open_window(int nr) {
 if(winbesetzt[nr]) {
#ifdef WINDOWS
#else
    XEvent event;
    /* Das Fensterauf den Screen Mappen */
  
    XMapWindow(display[nr], win[nr]);
    XNextEvent(display[nr], &event);
    handle_event(nr,&event);
#endif  
  }
}

void close_window(int nr) { 
#ifdef WINDOWS
  if(winbesetzt[nr]) DestroyWindow(win_hwnd[nr]);
#else
  XEvent event;
      if(winbesetzt[nr]) {
        XUnmapWindow(display[nr], win[nr]);
        XCheckWindowEvent(display[nr],win[nr],ExposureMask, &event);
      }
#endif        
}
#ifndef WINDOWS
void handle_event(int nr,XEvent *event) {
  switch (event->type) {

    /* Das Redraw-Event */  
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
#ifdef DEBUG      
    case GraphicsExpose:
      printf("GraphicsExpose: %d\n",event->type);break;
    case NoExpose:
      printf("NoExpose: %d\n",event->type);break;
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
  }
}
#endif
void handle_window(int winnr) {
  
  if(winbesetzt[winnr]) {

#ifdef WINDOWS
#else
   XEvent event;
   
    while(XCheckWindowEvent(display[winnr],win[winnr] ,
        ExposureMask| 
	ButtonPressMask| 
	PointerMotionMask |
	KeyPressMask, &event)) {
       handle_event(winnr,&event);  
    } 
#endif
  }
}


void graphics(){
#ifdef DEBUG
  printf("graphics:\n");
#endif
  if(winbesetzt[usewindow]) {handle_window(usewindow);return;}
  else {
     if(usewindow==0) {
       fetch_rootwindow();
     } else {
       create_window2(usewindow,"X11-Basic","X11-Basic",100,10,WINDOW_DEFAULT_W,WINDOW_DEFAULT_H);
       open_window(usewindow);
     }
  }
}


int fetch_rootwindow() {
#ifndef WINDOWS
  char *display_name = NULL;   /* NULL: Nimm Argument aus setenv DISPLAY */
  unsigned long foreground,background;
  int i,x,y,w,h,b,d;
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
  return(0);
}

void activate() {  
#ifdef WINDOWS
  HDC hdc;
  RECT interior;
  graphics();
  hdc=GetDC(win_hwnd[usewindow]);
  GetClientRect(win_hwnd[usewindow],&interior);
	
  BitBlt(hdc,interior.left,interior.top,interior.right-interior.left,interior.bottom-interior.top,bitcon[usewindow],0,0,SRCCOPY);
    
  ReleaseDC(win_hwnd[usewindow],hdc);

#else
   Window root;
   int ox,oy,ow,oh,ob,d;
   XGCValues gc_val;
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



/*-------------------------------------------------------------------*/
/*               Routine zum Abspeichern von X-Image                 */
/*             Bitmaps.            (c) Markus hoffmann               */
/*-------------------------------------------------------------------*/



/* swap some long ints.  (n is number of BYTES, not number of longs) */
swapdws (char *bp, unsigned n) {
  register char c;
  register char *ep = bp + n;
  register char *sp;

  while (bp<ep) {
    sp=bp+3;
    c=*sp;*sp=*bp; *bp++=c; sp=bp+1;
    c=*sp;*sp=*bp; *bp++=c; bp+=2;
  }
}

#ifndef WINDOWS
char *imagetoxwd(XImage *image,Visual *visual,XColor *pixc, int *len) {
    XWDFileHeader *data;
    unsigned long swaptest = 1;
    char image_name[]="X11-BASIC Grafik";
    int header_len=sizeof(XWDFileHeader)+((strlen(image_name)+1+7) & -8);
    int colortable_len,ncolors=0,i;
    XWDColor *color;
    if(image->depth==8) ncolors=256;
    colortable_len=ncolors*sizeof(XWDColor);
    *len=header_len+colortable_len+image->height*image->bytes_per_line;
    data=malloc(header_len+colortable_len+image->height*image->bytes_per_line);
    color=(XWDColor *)((int)data+header_len);
    memcpy((void *)((int)data+header_len+colortable_len),image->data,image->height*image->bytes_per_line);
    memcpy((void *)((int)data+sizeof(XWDFileHeader)),image_name,strlen(image_name));
    data->header_size=(CARD32)header_len;
    data->file_version=(CARD32)XWD_FILE_VERSION;
    data->pixmap_format=(CARD32)image->format;
    data->pixmap_depth=(CARD32)image->depth;
    data->pixmap_width=(CARD32)image->width;
    data->pixmap_height=(CARD32)image->height;
    data->xoffset=(CARD32)image->xoffset;
    data->byte_order=(CARD32)image->byte_order;
    data->bitmap_unit=(CARD32)image->bitmap_unit;
    data->bitmap_bit_order=(CARD32)image->bitmap_bit_order;
    data->bitmap_pad=image->bitmap_pad;
    data->bits_per_pixel=image->bits_per_pixel;
    data->bytes_per_line=image->bytes_per_line;
   
      data->colormap_entries = ncolors;
      data->ncolors          = ncolors;  
   
    data->visual_class   = (CARD32)visual->class;
    data->red_mask       = (CARD32)visual->red_mask;
    data->green_mask     = (CARD32)visual->green_mask;
    data->blue_mask      = (CARD32)visual->blue_mask;
    data->bits_per_rgb   = (CARD32)visual->bits_per_rgb;

    data->window_width=(CARD32)image->width;
    data->window_height=(CARD32)image->height;
    data->window_x        = 0;
    data->window_y        = 0;
    data->window_bdrwidth = 0;
    if(ncolors){  /* Farbtabelle */
      for(i=0;i<ncolors;i++)  {
        color[i].pixel = (CARD32)i;
        color[i].red   = (CARD16)(pixc+i)->red;
        color[i].green = (CARD16)(pixc+i)->green;
        color[i].blue  = (CARD16)(pixc+i)->blue;
        color[i].flags = (CARD8)(DoRed | DoGreen | DoBlue);
        color[i].pad   = (CARD8)0;
      }
    }
    if (*(char *) &swaptest)    swapdws((char *)data, sizeof(XWDFileHeader));
    return((char *)data);
}
XImage *xwdtoximage(char *data,Visual *visual) {
    char *adr;
    unsigned long swaptest = 1;    
    if (*(char *) &swaptest)    swapdws(data, sizeof(XWDFileHeader));
#if DEBUG
    printf("xwd-Name: %s\n",data+sizeof(XWDFileHeader));
    printf("Version: %d\n",((XWDFileHeader *)data)->file_version);
    printf("pixmap_format: %d\n",((XWDFileHeader *)data)->pixmap_format);
    printf("pixmap_depth: %d\n",((XWDFileHeader *)data)->pixmap_depth);
    printf("ncolors: %d\n",((XWDFileHeader *)data)->ncolors);
    printf("xoffset: %d\n",((XWDFileHeader *)data)->xoffset);
    printf("pixmap_width: %d\n",((XWDFileHeader *)data)->pixmap_width);
    printf("pixmap_height: %d\n",((XWDFileHeader *)data)->pixmap_height);
    printf("bitmap_pad: %d\n",((XWDFileHeader *)data)->bitmap_pad);
    printf("bytes_per_line: %d\n",((XWDFileHeader *)data)->bytes_per_line);
#endif
    if(((XWDFileHeader *)data)->file_version!=(CARD32)XWD_FILE_VERSION)
      printf("Achtung: XWD Version: %d\n",((XWDFileHeader *)data)->file_version);

    adr=malloc(((XWDFileHeader *)data)->pixmap_height*((XWDFileHeader *)data)->bytes_per_line);
    memcpy(adr,data+((XWDFileHeader *)data)->header_size+((XWDFileHeader *)data)->ncolors*sizeof(XWDColor),((XWDFileHeader *)data)->pixmap_height*((XWDFileHeader *)data)->bytes_per_line);
    return(XCreateImage(display[usewindow],visual,
    ((XWDFileHeader *)data)->pixmap_depth,
    ((XWDFileHeader *)data)->pixmap_format, 
    ((XWDFileHeader *)data)->xoffset, 
    adr, 
    ((XWDFileHeader *)data)->pixmap_width, 
    ((XWDFileHeader *)data)->pixmap_height, 
    ((XWDFileHeader *)data)->bitmap_pad,
    ((XWDFileHeader *)data)->bytes_per_line));
}

#endif


/* Alloziert einen Farbeintrag in der Palette. 
   Rueckgabewert ist die Nummer des Farbeintrags.
   Ist kein neuer Eintrag mehr frei, so wird die Nummer des Eintrags
   zurueckgeliefert, der der spezifizierten Farbe am naechsten kommt.
   Diese Routine kann also kein XAllocColor failed mehr produzieren.

   (c) markus hoffmann  1998                                   */
#ifndef WINDOWS
 Status my_XAllocColor(Display *,Colormap,XColor *);
#endif   
int get_color(int r, int g, int b) {
#ifdef WINDOWS
  return(RGB(r>>8,g>>8,b>>8));
#else
  Colormap map;
  XColor pixcolor;

  map =XDefaultColormapOfScreen ( XDefaultScreenOfDisplay ( display[usewindow] ) );

  pixcolor.red=r;
  pixcolor.green=g;
  pixcolor.blue=b;
  if(my_XAllocColor(display[usewindow], map, &pixcolor)==0) 
    printf("could not switch to color.\n");    
  
  return(pixcolor.pixel);
#endif
}

#ifndef WINDOWS
Status my_XAllocColor(Display *display,Colormap map,XColor *pixcolor) {
  Status rval;
  if((rval=XAllocColor(display, map, pixcolor))==0) {
    int i,r,g,b;
    unsigned long e=0xfffffff,d;
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


/* AES-Nachbildungen (c) Markus Hoffmann     */



void load_GEMFONT(int n) {
#ifdef WINDOWS
  SIZE siz;
  GetTextExtentPoint(bitcon[usewindow],"Sg", 2,&siz);
  chw=siz.cx/2;
  chh=siz.cy;
  baseline=chh-2;
#else
  XGCValues gc_val;
  XFontStruct *fs;
  if(n==5) fs=XLoadQueryFont(display[usewindow], GEMFONTSMALL);
  else fs=XLoadQueryFont(display[usewindow], GEMFONT);
  if(fs!=NULL)  {
     gc_val.font=fs->fid;
     XChangeGC(display[usewindow], gc[usewindow],  GCFont, &gc_val);
     chw=fs->max_bounds.width,chh=fs->max_bounds.ascent+fs->max_bounds.descent;
     baseline=fs->max_bounds.ascent;
  }
#endif
}

void gem_init() {
  int i;
#ifdef WINDOWS
  RECT interior;

  GetClientRect(win_hwnd[usewindow],&interior);
  sbox.x=interior.left;
  sbox.y=interior.top;
  sbox.w=interior.right-interior.left;
  sbox.h=interior.bottom-interior.top;   
#else
  int border;
  Window root;
#ifdef DEBUG
  printf("gem_init: usewin=%d\n",usewindow);
  printf("sbox=(%d,%d,%d,%d)\n",sbox.x,sbox.y,sbox.w,sbox.h);
#endif

    /* Screendimensionen bestimmem */
    XGetGeometry(display[usewindow],win[usewindow],&root,&sbox.x,&sbox.y,&sbox.w,&sbox.h,&border,&depth); 
#if 0
   weiss=WhitePixel(display[usewindow], DefaultScreen(display[usewindow]));
   schwarz=BlackPixel(display[usewindow], DefaultScreen(display[usewindow]));
#endif
#endif
   load_GEMFONT(1);
   
   for(i=0;i<16;i++) 
   gem_colors[i]=get_color(gem_colordefs[i].r,gem_colordefs[i].g,gem_colordefs[i].b);
}


#if 0
void box_center(ARECT *b) {
    b->x=sbox.x+(sbox.w-b->w)/2; /* Koordinaten fuer Bildmitte: */
    b->y=sbox.y+(sbox.h-b->h)/2;
}
#endif

/* -------------------------------- AES-Implementationen ------------------*/

RSHDR *rsrc=NULL;    /* Adresse der Recource im Speicher */

/* Form-Alert-Routine. Dem GEM nachempfunden.
(c) markus Hoffmann   1998               */

double ltext(int, int, double, double, double , int, char *);

int form_alert(int dbut,char *n) {
  return(form_alert2(dbut,n,NULL));
}
int form_alert2(int dbut,char *n, char *tval) {
  char *bzeilen[30],*bbuttons[30],*buffer[30];
  int anzzeilen=0,anzbuttons=0,anztedinfo=0,anzbuffer=0;
  int symbol,sbut=0;
  char *pos;
  char **ein=bzeilen;
  int i=0,j=strlen(n),k=0,l=0;
  TEDINFO tedinfo[30];
  OBJECT objects[40]={{-1,1,1,G_BOX, 0, OUTLINED, 0x00021100, 0,0,100,100}};
  int objccount=1;
  int x,y,w,h;
#ifdef DEBUG
  printf("**form_alert:\n");
#endif

  while(i<j) {
    if(n[i]=='[') {
      pos=&n[i+1];
      k++;l=0;
    } else if(n[i]==']') {
      n[i]=0;
      if(k>0) ein[l++]=pos;
      if(k==1) {
        symbol=atoi(pos);
	ein=bzeilen;
      } else if(k==2) {
        ein=bbuttons;anzzeilen=l;
      } else if(k==3) anzbuttons=l;
    }
    else if(n[i]=='|') {ein[l]=pos;n[i]=0;pos=n+i+1;l++;};
    i++;
  }
   
  if(anzbuttons) {
    /* Box-Abmessungen bestimmen */
    int textx;
    int maxc=0;

    graphics();
    gem_init();

    if(symbol) {objects[0].ob_width=objects[0].ob_height=textx=64;}
    else {objects[0].ob_width=objects[0].ob_height=textx=0;}
 
     /*Raender*/
    objects[0].ob_width+=chh*2;
    objects[0].ob_height=max(objects[0].ob_height+2*chh,chh*2+(anzzeilen+2)*chh);
    
    
    for(i=0;i<anzzeilen;i++) maxc=max(maxc,strlen(bzeilen[i]));
    objects[0].ob_width+=chw*maxc;
    
     /* Buttons  */
    maxc=0;
    for(i=0;i<anzbuttons;i++) maxc=max(maxc,strlen(bbuttons[i]));
    objects[0].ob_width=max(objects[0].ob_width,chw*(4+(maxc+1)*anzbuttons+2*(anzbuttons-1)));

    for(i=0;i<anzbuttons; i++) {
      objects[objccount].ob_x=objects[0].ob_width/2-chw*((maxc+1)*anzbuttons+2*(anzbuttons-1))/2+i*chw*(maxc+3);
      objects[objccount].ob_y=objects[0].ob_height-2*chh;
      objects[objccount].ob_width=(chw+1)*maxc;
      objects[objccount].ob_height=chh+3;
      objects[objccount].ob_spec=(LONG)bbuttons[i];
      objects[objccount].ob_head=-1;
      objects[objccount].ob_tail=-1;
      objects[objccount].ob_next=objccount+1;
      objects[objccount].ob_type=G_BUTTON;
      objects[objccount].ob_flags=SELECTABLE|EXIT;
      objects[objccount].ob_state=NORMAL;
      objccount++;
    }
    
    if(dbut>0 && dbut<=anzbuttons) {
      objects[dbut].ob_flags|=DEFAULT;
    }    
    
    for(i=0;i<anzzeilen;i++) {
      objects[objccount].ob_x=textx+chh;
      objects[objccount].ob_y=(1+i)*chh;
      objects[objccount].ob_width=chw*strlen(bzeilen[i]);
      objects[objccount].ob_height=chh;
      objects[objccount].ob_spec=(LONG)bzeilen[i];
      objects[objccount].ob_head=-1;
      objects[objccount].ob_tail=-1;
      objects[objccount].ob_next=objccount+1;
      objects[objccount].ob_type=G_STRING;
      objects[objccount].ob_flags=NONE;
      objects[objccount].ob_state=NORMAL;
      objccount++;
      /* Input-Felder finden */
      if(strlen((char *)objects[objccount-1].ob_spec)) {
        for(j=0;j<strlen((char *)objects[objccount-1].ob_spec);j++) {
	  if(((char *)(objects[objccount-1].ob_spec))[j]==27) {
            ((char *)(objects[objccount-1].ob_spec))[j]=0;
	    objects[objccount].ob_x=textx+chh+chw*j+chw;
            objects[objccount].ob_y=(1+i)*chh;
            objects[objccount-1].ob_width=chw*(strlen((char *)objects[objccount-1].ob_spec));
            objects[objccount].ob_width=chw*(strlen(bzeilen[i]+j+1));
            objects[objccount].ob_height=chh;
            objects[objccount].ob_spec=(LONG)&tedinfo[anztedinfo];
            objects[objccount].ob_head=-1;
            objects[objccount].ob_tail=-1;
            objects[objccount].ob_next=objccount+1;
            objects[objccount].ob_type=G_FTEXT;
            objects[objccount].ob_flags=EDITABLE;
            objects[objccount].ob_state=NORMAL;
	    tedinfo[anztedinfo].te_ptext=(LONG)(bzeilen[i]+j+1);
	    buffer[anzbuffer]=malloc(strlen((char *)tedinfo[anztedinfo].te_ptext)+1);
	    tedinfo[anztedinfo].te_ptmplt=(LONG)(buffer[anzbuffer++]);
	    tedinfo[anztedinfo].te_pvalid=(LONG)(bzeilen[i]+j+1);
	    tedinfo[anztedinfo].te_font=FONT_IBM;
	    tedinfo[anztedinfo].te_just=TE_LEFT;
	    tedinfo[anztedinfo].te_junk1=strlen((char *)tedinfo[anztedinfo].te_ptext);
	    tedinfo[anztedinfo].te_junk2=0;
	    tedinfo[anztedinfo].te_color=0x1100;
	    tedinfo[anztedinfo].te_thickness=1;
	    tedinfo[anztedinfo].te_txtlen=strlen((char *)tedinfo[anztedinfo].te_ptext);
	    tedinfo[anztedinfo].te_tmplen=strlen((char *)tedinfo[anztedinfo].te_ptext);
	    anztedinfo++;
            objccount++;
	  }
	}
      }
    }
    if(symbol>=1) {
      objects[objccount].ob_x=2*chw;
      objects[objccount].ob_y=chw;
      objects[objccount].ob_width=50;
      objects[objccount].ob_height=50;
      objects[objccount].ob_spec=symbol;
      objects[objccount].ob_head=-1;
      objects[objccount].ob_tail=-1;
      objects[objccount].ob_next=objccount+1;
      objects[objccount].ob_type=G_ALERTTYP;
      objects[objccount].ob_flags=NONE;
      objects[objccount].ob_state=NORMAL;
      objccount++;
 
    }
    objects[objccount-1].ob_next=0;
    objects[0].ob_tail=objccount-1;
    objects[objccount-1].ob_flags|=LASTOB;
    
      /* Objektbaum Zentrieren */
    form_center(objects, &x,&y,&w,&h);


  /* Erst den Graphic-Kontext retten  */

    form_dial(0,0,0,0,0,x,y,w,h);
    form_dial(1,0,0,0,0,x,y,w,h);

    sbut=form_do(objects);

    form_dial(3,0,0,0,0,x,y,w,h);
    form_dial(2,0,0,0,0,x,y,w,h);

    if(tval!=NULL) { /* Textfelder zusammensuchen  */

      tval[0]=0;
      for(i=0;i<objccount;i++) {

        if(objects[i].ob_flags & EDITABLE) {
	  strcat(tval,(char *)((TEDINFO *)objects[i].ob_spec)->te_ptext);
	  tval[strlen(tval)+1]=0;
	  tval[strlen(tval)]=13;
	}
      }
    }
    while(anzbuffer) {
      free(buffer[--anzbuffer]);
    }
  }
  return(sbut);
}


int rsrc_free() {
  free(rsrc);
  rsrc=NULL;
  return(0);
}
void put_bitmap(char *adr,int x,int y,int w, int h) {
#ifdef WINDOWS
  
#else
  Pixmap bitpix;
  bitpix=XCreateBitmapFromData(display[usewindow],win[usewindow],
    adr,w,h);
    XCopyPlane(display[usewindow],bitpix,pix[usewindow],gc[usewindow],
     0,0,w,w,x,y,1);
    XFreePixmap(display[usewindow],bitpix);
#endif    
}


void WSWAP(char *adr) {
  char a;
  a=adr[0];
  adr[0]=adr[1];
  adr[1]=a;
}
void LSWAP(short *adr) {
  short a;
  a=adr[0];
  adr[0]=adr[1];
  adr[1]=a;
}
void LWSWAP(short *adr) {
  short a;
  WSWAP((char *)&adr[0]);
  WSWAP((char *)&adr[1]);
  a=adr[0];
  adr[0]=adr[1];
  adr[1]=a;
}

int draw_object(OBJECT *tree,int idx,int rootx,int rooty) {
  char randdicke=0;
  char zeichen,opaque=0;
  int fillcolor=BLACK,pattern=9;
  int textcolor=BLACK,textmode,framecolor=BLACK;
  int i,drawbg=1,drawtext=1;
  int obx=tree[idx].ob_x+rootx;
  int oby=tree[idx].ob_y+rooty;
  int obw=tree[idx].ob_width;
  int obh=tree[idx].ob_height;
 
#ifdef DEBUG
  printf("Drawobjc: %d   head=%d  next=%d tail=%d\n",idx,tree[idx].ob_head, 
  tree[idx].ob_next, tree[idx].ob_tail); 
#endif
  switch(LOBYTE(tree[idx].ob_type)) {
  case G_BOX:
  case G_BOXCHAR:
    zeichen=(tree[idx].ob_spec & 0xff000000)>>24;
    randdicke=(tree[idx].ob_spec & 0xff0000)>>16;
    fillcolor=(tree[idx].ob_spec & 0xf);
    textcolor=(tree[idx].ob_spec & 0xf00)>>8;
    textmode=(tree[idx].ob_spec & 0x80)>>7;
    framecolor=(tree[idx].ob_spec & 0xf000)>>12;
    pattern=(tree[idx].ob_spec & 0x70)>>4;
    break;
    
  case G_IBOX:
    zeichen=(tree[idx].ob_spec & 0xff000000)>>24;
    randdicke=0;
    fillcolor=(tree[idx].ob_spec & 0xf);
    textcolor=(tree[idx].ob_spec & 0xf00)>>8;
    textmode=(tree[idx].ob_spec & 0x80)>>7;
    framecolor=(tree[idx].ob_spec & 0xf000)>>12;
    pattern=(tree[idx].ob_spec & 0x70)>>4;
    break;

  case G_TEXT:
  case G_FTEXT:
    framecolor=((((TEDINFO *)((int)tree[idx].ob_spec))->te_color)>>12) & 0xf;
    textcolor=((((TEDINFO *)((int)tree[idx].ob_spec))->te_color)>>8) & 0xf;
    opaque=((((TEDINFO *)((int)tree[idx].ob_spec))->te_color)>>7) & 1;
    pattern=((((TEDINFO *)((int)tree[idx].ob_spec))->te_color)>>4) & 7;
    fillcolor=(((TEDINFO *)((int)tree[idx].ob_spec))->te_color) & 0xf;
    randdicke=0;
    break;
  case G_STRING:
  case G_TITLE:
    randdicke=0;
    if(tree[idx].ob_state & SELECTED) {
       fillcolor=WHITE;
       pattern=9;
     } else {
       drawbg=0;
     }
    break;
  case G_BUTTON:
    randdicke=-1;
    fillcolor=WHITE;
    pattern=9;
    break;
  
  case G_BOXTEXT:  
  case G_FBOXTEXT:
    framecolor=((((TEDINFO *)((int)tree[idx].ob_spec))->te_color)>>12) & 0xf;
    textcolor=((((TEDINFO *)((int)tree[idx].ob_spec))->te_color)>>8) & 0xf;
    opaque=((((TEDINFO *)((int)tree[idx].ob_spec))->te_color)>>7) & 1;
    pattern=((((TEDINFO *)((int)tree[idx].ob_spec))->te_color)>>4) & 7;
    fillcolor=(((TEDINFO *)((int)tree[idx].ob_spec))->te_color) & 0xf;
    randdicke=((TEDINFO *)((int)tree[idx].ob_spec))->te_thickness;
    break;
  default:
    drawbg=0;
    break;
  } 
  
  if(tree[idx].ob_state & SELECTED) {
    fillcolor=fillcolor^1;
    textcolor=textcolor^1;
  }
  if(tree[idx].ob_flags & EXIT) randdicke--;
  if(tree[idx].ob_flags & DEFAULT) randdicke--;

if (drawbg) {

/* Zeichnen  */
  if(tree[idx].ob_state & OUTLINED) {
    SetForeground(gem_colors[WHITE]);
    FillRectangle(obx-3,oby-3,obw+6,obh+6);
    SetForeground(gem_colors[framecolor]);
    DrawRectangle(obx-3,oby-3,obw+6,obh+6);
  }
  if(tree[idx].ob_state & SHADOWED) {
    SetForeground(gem_colors[BLACK]);
    FillRectangle(obx+obw,oby+chh/2,chw,obh);
    FillRectangle(obx+chw,oby+obh,obw,chh/2);
  }  


/* Hintergrund zeichnen */
  SetForeground(gem_colors[WHITE]);
  if(!opaque) {FillRectangle(obx,oby,obw,obh);}

if(pattern) {
  SetForeground(gem_colors[fillcolor]);
  SetFillStyle(FillStippled);
  set_fill(pattern);
  FillRectangle(obx,oby,obw,obh);
  SetFillStyle(FillSolid);
}
}

/* Text zeichnen   */

  SetForeground(gem_colors[textcolor]);

  if(tree[idx].ob_state & DISABLED) {SetForeground(gem_colors[LWHITE]);}

  switch(LOBYTE(tree[idx].ob_type)) {
    char *text;
    char chr[2];
    TEDINFO *ted;
    int x,y,i;
  case G_STRING:
  case G_TITLE:
    text=(char *)((int)tree[idx].ob_spec);
    DrawString(obx,oby+chh-2,text,strlen(text));
  case G_BOX:
  case G_IBOX:  
    break;
  case G_BUTTON:
    text=(char *)((int)tree[idx].ob_spec);
    DrawString(obx+(obw-chw*strlen(text))/2,oby+chh-1+(obh-chh)/2,text,strlen(text));
    break;
  case G_BOXCHAR:
    DrawString(obx+(obw-chw)/2,oby+chh-2+(obh-chh)/2,&zeichen,1);
    break;
  case G_ALERTTYP:
    chr[0]=tree[idx].ob_spec+4;
    chr[1]=0;
    SetForeground(gem_colors[RED]);
    #ifndef WINDOWS
    XSetLineAttributes(display[usewindow], gc[usewindow], 2, 0,0,0);
    #endif
    ltext(obx,oby,0.5,0.5,0,0,chr); 
    if(tree[idx].ob_spec==3) ltext(obx+4,oby+12,0.5/6,0.5/2,0,0,"STOP");
    #ifndef WINDOWS
    XSetLineAttributes(display[usewindow], gc[usewindow], 1, 0,0,0);
    #endif
    break;
  case G_TEXT:
  case G_FTEXT:
  case G_BOXTEXT:
  case G_FBOXTEXT:
    ted=(TEDINFO *)((int)tree[idx].ob_spec);
    text=(char *)(ted->te_ptext);
    load_GEMFONT(ted->te_font);
    if(tree[idx].ob_type==G_FTEXT || tree[idx].ob_type==G_FBOXTEXT){
      if(ted->te_junk1-ted->te_junk2>ted->te_tmplen) ted->te_junk2=ted->te_junk1-ted->te_tmplen;
      if(ted->te_junk1-ted->te_junk2<0) ted->te_junk2=ted->te_junk1;
      for(i=0;i<ted->te_tmplen;i++) {
        if(i<strlen((char *)(ted->te_ptext))) ((char *)(ted->te_ptmplt))[i]=((char *)(ted->te_ptext))[i+ted->te_junk2];
	else ((char *)(ted->te_ptmplt))[i]='_';
      } 
      ((char *)ted->te_ptmplt)[ted->te_tmplen]=0;
      text=(char *)(ted->te_ptmplt);
    }
    if(ted->te_just==TE_LEFT) {
      x=obx; y=oby+chh-2+(obh-chh)/2;
    } else if(ted->te_just==TE_RIGHT) {
      x=obx+obw-chw*strlen(text); y=oby+chh-2+(obh-chh)/2;
    } else {
      x=obx+(obw-chw*strlen(text))/2; y=oby+chh-2+(obh-chh)/2;
    }
    
    DrawString(x,y,text,strlen(text));
    SetForeground(gem_colors[RED]);
    if(strlen((char *)(ted->te_ptext))>ted->te_tmplen+ted->te_junk2)
      DrawString(obx+obw,oby+obh,">",1);
    if(ted->te_junk2)
      DrawString(obx-chw,oby+obh,"<",1);
    load_GEMFONT(1);
    break;
  case G_IMAGE:
    {BITBLK *bit=(BITBLK *)((int)tree[idx].ob_spec);
    unsigned int adr;
    adr=*((LONG *)&(bit->bi_pdata));

    SetForeground(gem_colors[(bit->bi_color) & 0xf]);   
    put_bitmap((char *)adr,obx,oby,bit->bi_wb*8,bit->bi_hl);}
    break;
  case G_ICON:
    {ICONBLK *bit=(ICONBLK *)((int)tree[idx].ob_spec);
    unsigned int adr;
    adr=*(LONG *)&bit->ib_pmask;
    
    SetForeground(gem_colors[WHITE]);   
    put_bitmap((char *)adr,obx,oby,bit->ib_wicon,bit->ib_hicon);
    FillRectangle(obx+bit->ib_xtext,oby+bit->ib_ytext,bit->ib_wtext,bit->ib_htext);
    adr=*(LONG *)&bit->ib_pdata;
    
    SetForeground(gem_colors[BLACK]);   
    put_bitmap((char *)adr,obx,oby,bit->ib_wicon,bit->ib_hicon);
    /* Icon-Text */
    load_GEMFONT(FONT_SMALL);
    DrawString(obx+bit->ib_xtext,oby+bit->ib_ytext+bit->ib_htext,(char *)*(LONG *)&bit->ib_ptext,strlen((char *)*(LONG *)&bit->ib_ptext));
    /* Icon char */
    load_GEMFONT(1);}
    break;
  default:
    printf("Unbekanntes Objekt #%d\n",tree[idx].ob_type);
  }
  
/* Rand zeichnen */
  if(randdicke) {
    SetForeground(gem_colors[framecolor]);
    if(randdicke>0) {
      for(i=0;i<randdicke;i++) DrawRectangle(obx+i,oby+i,obw-2*i,obh-2*i);    
    } else if(randdicke<0) {
      for(i=0;i>randdicke;i--) DrawRectangle(obx+i,oby+i,obw-2*i,obh-2*i);
    }
  }  
  if(tree[idx].ob_state & CROSSED) { 
    SetForeground(gem_colors[RED]);
    DrawLine(obx,oby,obx+obw,oby+obh); 
    DrawLine(obx+obw,oby,obx,oby+obh); 
  }
}


int objc_draw( OBJECT *tree,int start, int stop,int rootx,int rooty) {
  int idx=start;
#ifdef DEBUG
  printf("**objc_draw: von %d bis %d\n",start,stop);
#endif
  draw_object(tree,idx,rootx,rooty);
  if(tree[idx].ob_flags & LASTOB) return(1);
  if(idx==stop) return(1);
  if(tree[idx].ob_head!=-1) {
    if(!(tree[idx].ob_flags & HIDETREE)) {
      objc_draw(tree,tree[idx].ob_head,tree[idx].ob_tail,tree[idx].ob_x+rootx,tree[idx].ob_y+rooty);
    }
  }
  while(tree[idx].ob_next!=-1) {
    idx=tree[idx].ob_next;
    draw_object(tree,idx,rootx,rooty);
    if(tree[idx].ob_flags & LASTOB) return(1);
    if(tree[idx].ob_head!=-1) {
      if(!(tree[idx].ob_flags & HIDETREE)) objc_draw(tree,tree[idx].ob_head,tree[idx].ob_tail,tree[idx].ob_x+rootx,tree[idx].ob_y+rooty );      
    }
    if(idx==stop) return(1);
  }  
}


int rsrc_gaddr(int re_gtype, unsigned int re_gindex, char **re_gaddr) { 
  if(re_gtype==R_TREE) {
    char **ptreebase;
    if(re_gindex>=rsrc->rsh_ntree) return(0);
    ptreebase = (char **)((unsigned int)rsrc+(unsigned int)rsrc->rsh_trindex);
    *re_gaddr=(char *)(((int)ptreebase[re_gindex]+(int)rsrc));
    return(1);
  } else if(re_gtype==R_FRSTR) {
    char **ptreebase;
    if(re_gindex>=rsrc->rsh_nstring) return(0);
    ptreebase=(char **)((unsigned int)rsrc+(unsigned int)rsrc->rsh_frstr);
    *re_gaddr=(char *)(((int)ptreebase[re_gindex]+(int)rsrc));
    return(1);
  } else return(0);
}

void fix_trindex() {
  int i;
  char **ptreebase;
  int anzahl;
  ptreebase = (char **)((unsigned int)rsrc+(unsigned int)rsrc->rsh_trindex);
  anzahl=rsrc->rsh_ntree;
  if(anzahl) {
  if(rsrc->rsh_vrsn==0) {
    for(i = anzahl-1; i >= 0; i--) {
      LWSWAP((short *)(&ptreebase[i]));
    }
  }
  }
}
void fix_frstrindex() {
  int i;
  char **ptreebase;
  int anzahl;
  
  ptreebase = (char **)((unsigned int)rsrc+(unsigned int)rsrc->rsh_frstr);
  anzahl=rsrc->rsh_nstring;
  if(anzahl) {
    if(rsrc->rsh_vrsn==0) {
      for(i = anzahl-1; i >= 0; i--) {
        LWSWAP((short *)(&ptreebase[i]));
      }
    }
  }
}
void fix_objc() {
  int i,j;
  OBJECT *base;
  int anzahl;

  base = (OBJECT *)((unsigned int)rsrc+(unsigned int)rsrc->rsh_object);
  anzahl=rsrc->rsh_nobs;
  if(anzahl) {
    for(i =0; i < anzahl; i++) {
      if(rsrc->rsh_vrsn==0) {
	for(j=0;j<sizeof(OBJECT)/2;j++) {
          WSWAP((char *)((int)&base[i]+2*j));
        }
        LSWAP((short *)&(base[i].ob_spec));
      }
      if(!(base[i].ob_type==G_BOX || base[i].ob_type==G_BOXCHAR|| 
	      base[i].ob_type==G_IBOX || base[i].ob_type==G_ALERTTYP))
	    base[i].ob_spec+=(LONG)rsrc;	
	  
      base[i].ob_x=     LOBYTE(base[i].ob_x)*chw+     HIBYTE(base[i].ob_x);
      base[i].ob_y=     LOBYTE(base[i].ob_y)*chh+     HIBYTE(base[i].ob_y);
      base[i].ob_width= LOBYTE(base[i].ob_width)*chw+ HIBYTE(base[i].ob_width);
      base[i].ob_height=LOBYTE(base[i].ob_height)*chh+HIBYTE(base[i].ob_height);  
    }
  }
}
void fix_tedinfo() {
  int i,j;
  TEDINFO *base;
  int anzahl;
  base = (TEDINFO *)((unsigned int)rsrc+(unsigned int)rsrc->rsh_tedinfo);
  anzahl=rsrc->rsh_nted;
    if(anzahl) {
      for(i =0; i < anzahl; i++) {
	if(rsrc->rsh_vrsn==0) {
	  for(j=0;j<sizeof(TEDINFO)/2;j++) {
            WSWAP((char *)((int)&base[i]+2*j));
          }
          LSWAP((short *)&(base[i].te_ptext));
          LSWAP((short *)&(base[i].te_ptmplt));
          LSWAP((short *)&(base[i].te_pvalid));
        }
      base[i].te_ptext+=(LONG)rsrc;	  
      base[i].te_ptmplt+=(LONG)rsrc;	  
      base[i].te_pvalid+=(LONG)rsrc;	  
    }
  }
}
void fix_bitblk() {
  unsigned int i,j,k,l,m,n=0;
  BITBLK *base;
  int anzahl;
  base = (BITBLK *)((LONG)rsrc+(LONG)rsrc->rsh_bitblk);
  anzahl=rsrc->rsh_nbb;
#if DEBUG
  printf("sizeof: %d\n",sizeof(BITBLK));
#endif
  if(anzahl) {
    for(i =0; i < anzahl; i++) {
      if(rsrc->rsh_vrsn==0) {
	for(j=0;j<sizeof(BITBLK)/2;j++) {
          WSWAP((char *)((int)&base[i]+2*j));
        }
	LSWAP((short *)&(base[i].bi_pdata));
      }
      *((LONG *)&base[i].bi_pdata)+=(LONG)rsrc;
      if(rsrc->rsh_vrsn==0) {
      k=*((LONG *)&base[i].bi_pdata);
#if DEBUG
	  printf("Bitmap #%d at %x\n",i,k); 
	  printf("w=%d h=%d x=%d y=%d c=%d\n",base[i].bi_wb,base[i].bi_hl,base[i].bi_x,base[i].bi_y,base[i].bi_color); 
#endif
	  for(j=0;j<base[i].bi_wb*base[i].bi_hl/2;j++) {
            WSWAP((char *)(k+2*j));
          }
	  for(j=0;j<base[i].bi_wb*base[i].bi_hl/2;j++) {
	    n=0;
            l=((WORD *)(k+2*j))[0];
	    for(m=0;m<16;m++) {
	      n=n<<1;
	      n|=(l & 1);
	      l=l>>1;	    
	    } 
	   *((WORD *)(k+2*j))=n;
        }
      }
    }
  }
}
void fix_iconblk() {
  unsigned int i,j,k,l,m,n=0;
  ICONBLK *base;
  int anzahl;
  base = (ICONBLK *)((LONG)rsrc+(LONG)rsrc->rsh_iconblk);
  anzahl=rsrc->rsh_nib;
  if(anzahl) {
    for(i =0; i < anzahl; i++) {
      if(rsrc->rsh_vrsn==0) {
	    for(j=0;j<sizeof(ICONBLK)/2;j++) {
              WSWAP((char *)((int)&base[i]+2*j));
            }
	    LSWAP((short *)&(base[i].ib_pmask));
	    LSWAP((short *)&(base[i].ib_pdata));
	    LSWAP((short *)&(base[i].ib_ptext));
	  }
	  *(LONG *)&base[i].ib_pmask+=(LONG)rsrc;
	  *(LONG *)&base[i].ib_pdata+=(LONG)rsrc;
	  *(LONG *)&base[i].ib_ptext+=(LONG)rsrc;
#if DEBUG
	  printf("Icon #%d name %s  ",i,*(LONG *)&base[i].ib_ptext); 
	  printf("w=%d h=%d x=%d y=%d c=<%c>\n",base[i].ib_wicon,base[i].ib_hicon,
	  base[i].ib_xicon,base[i].ib_yicon,base[i].ib_char); 
#endif
          if(rsrc->rsh_vrsn==0) {
            k=*(LONG *)&base[i].ib_pmask;
  
	    for(j=0;j<base[i].ib_wicon*base[i].ib_hicon/16;j++) {
              WSWAP((char *)(k+2*j));
            }
	    for(j=0;j<base[i].ib_wicon*base[i].ib_hicon/16;j++) {
	      n=0;
              l=((WORD *)(k+2*j))[0];
	      for(m=0;m<16;m++) {
	        n=n<<1;
	        n|=(l & 1);
	        l=l>>1;
	      } 
	      *((WORD *)(k+2*j))=n;
            }
            k=*(LONG *)&base[i].ib_pdata;
	  
	 for(j=0;j<base[i].ib_wicon*base[i].ib_hicon/16;j++) {
           WSWAP((char *)(k+2*j));
         }
	 for(j=0;j<base[i].ib_wicon*base[i].ib_hicon/16;j++) {
	   n=0;
           l=((WORD *)(k+2*j))[0];
	   for(m=0;m<16;m++) {
	     n=n<<1;
	     n|=(l & 1);
	     l=l>>1;
	   } 
	   *((WORD *)(k+2*j))=n;
        }
      }
    }
  }
}


/* *****************************  */
/*     objc_find                  */




int objc_find(OBJECT *tree,int x,int y) {
  int i=0;
  int sbut=-1;
  int idx=0;
  int stop=-1;
  int flag=0;
  int rootx=0;
  int rooty=0;
  while(1) {
    if(x>=tree[idx].ob_x+rootx && x<tree[idx].ob_x+tree[idx].ob_width+rootx &&
    y>=tree[idx].ob_y+rooty && y<tree[idx].ob_y+tree[idx].ob_height+rooty) {
      sbut=idx;
      if(tree[idx].ob_head!=-1) {
        if(!(tree[idx].ob_flags & HIDETREE)) {
          stop=tree[idx].ob_tail;
          rootx+=tree[idx].ob_x;
          rooty+=tree[idx].ob_y;
          idx=tree[idx].ob_head;
          flag=1;
        }
      } 
    }
    if(flag) flag=0;
    else { 
      if(tree[idx].ob_flags & LASTOB) return(sbut);
      if(idx==stop) return(sbut);
      if(tree[idx].ob_next!=-1) idx=tree[idx].ob_next;
      else return(sbut);
    }
  }
}

int rootob(OBJECT *tree,int onr) {
  int idx=onr;
  int sbut;
  
  while(1) {
    sbut=idx;
    idx=tree[idx].ob_next;
    if(idx==-1) return(-1);    
    if(tree[idx].ob_tail==sbut) return(idx); 
  }
}
int relobxy(OBJECT *tree,int ndx,int *x, int *y){
  *x=tree[ndx].ob_x;
  *y=tree[ndx].ob_y;
  while((ndx=rootob(tree,ndx))>=0){
    *x+=tree[ndx].ob_x;
    *y+=tree[ndx].ob_y;
  }
}
void draw_edcursor(OBJECT *tree,int ndx){
     TEDINFO *ted=(TEDINFO *)(tree[ndx].ob_spec);
     int x,y;
 
     relobxy(tree,ndx,&x,&y);
     SetForeground(gem_colors[RED]);
     DrawLine(x+chw*(ted->te_junk1-ted->te_junk2),y,x+chw*(ted->te_junk1-ted->te_junk2),
     y+chh+4); 
     SetForeground(gem_colors[BLACK]);
} 

int finded(OBJECT *tree,int start, int r) {
    /*  editierbare Objekt finden */
  int idx=start;
  int sbut=-1;
  if(r>0 && !(tree[idx].ob_flags & LASTOB)) idx++;
  else if(r<0 && idx>0) idx--;
  
  while(1) {
    if(tree[idx].ob_flags & EDITABLE) {sbut=idx;break;}
    if(tree[idx].ob_flags & LASTOB) break;
    if(r<0)idx--;
    else idx++;
    if(idx<0) break;
  }
  return(sbut);
}

int form_dial( int fo_diflag, int x1,int y1, int w1, int h1, int x2, int y2, int w2, int h2 ) {
  static sgccount=0;
#ifdef WINDOWS
  static HDC sgc[30];
  static HBITMAP spix[30];
#else
  static GC *sgc[30];
  static Pixmap *spix[30]; 
  XGCValues gc_val;
  GC pgc;
  Pixmap ppix;
#endif
#ifdef DEBUG
  printf("**form_dial:\n");
#endif

  switch(fo_diflag){
  case 0:
#ifdef WINDOWS
  sgc[sgccount]=CreateCompatibleDC(bitcon[usewindow]);
  spix[sgccount]=CreateCompatibleBitmap(bitcon[usewindow],w2+8,h2+8);
  SelectObject(sgc[sgccount],spix[sgccount]);
  BitBlt(sgc[sgccount],0,0,w2+8,h2+8,bitcon[usewindow],x2-3,y2-3,SRCCOPY);
#else
  /* Erst den Graphic-Kontext retten  */
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
    XCopyArea(display[usewindow], pix[usewindow],ppix,gc[usewindow],x2-3,y2-3,w2+8,h2+8,0,0);
    spix[sgccount]=malloc(sizeof(Pixmap));
    memcpy(spix[sgccount],&ppix,sizeof(Pixmap));
#endif
    sgccount++;
   break;
   case 3:    
   /* Hintergrund restaurieren  */    
   sgccount--;
#ifdef WINDOWS
    BitBlt(bitcon[usewindow],x2-3,y2-3,w2+8,h2+8,sgc[sgccount],0,0,SRCCOPY);
    DeleteObject(spix[sgccount]);
    DeleteDC(sgc[sgccount]);
#else
    XCopyArea(display[usewindow], *(spix[sgccount]),pix[usewindow],gc[usewindow],0,0,w2+8,h2+8,x2-3,y2-3);
    XFreePixmap(display[usewindow],*(spix[sgccount]));
    XCopyGC(display[usewindow],*sgc[sgccount],GCForeground| GCFunction |GCLineWidth |GCLineStyle| GCFont, gc[usewindow]);
    XFreeGC(display[usewindow],*sgc[sgccount]); 
    free(sgc[sgccount]);free(spix[sgccount]);
#endif
    activate();
    break;
  default:
    return(-1);
  }
}

int form_center(OBJECT *tree, int *x, int *y, int *w, int *h) {
  /* Objektbaum Zentrieren */
#ifdef DEBUG
  printf("**form_center:\n");
#endif

  tree->ob_x=sbox.x+(sbox.w-tree->ob_width)/2;
  tree->ob_y=sbox.y+(sbox.h-tree->ob_height)/2;    
  *x=tree->ob_x;
  *y=tree->ob_y;
  *w=tree->ob_width;
  *h=tree->ob_height;
  return(0);
}


int form_do(OBJECT *tree) {   
#ifdef WINDOWS
  HANDLE evn[3];
#else
  XEvent event;
  XGCValues gc_val;
#endif
  int exitf=0,bpress=0;
  int sbut,edob=-1,idx;
  int x,y,w,h;
#ifdef DEBUG
  printf("**form_do:\n");
#endif
  

    /* erstes editierbare Objekt finden */
 
  edob=finded(tree,0,0);
  objc_draw(tree,0,-1,0,0); 
	  
  /* Cursor plazieren */
	  
  if(edob>=0) {
    ((TEDINFO *)tree[edob].ob_spec)->te_junk1=strlen((char *)((TEDINFO *)tree[edob].ob_spec)->te_ptext);
    draw_edcursor(tree,edob);
  }	  
  
  /* Auf Tasten/Maus reagieren */
  activate();
#ifdef WINDOWS
  evn[0]=keyevent;
  evn[1]=buttonevent;
  ResetEvent(evn[0]);
  ResetEvent(evn[1]);

#endif
  while(exitf==0) {
#ifdef WINDOWS  
  WaitForMultipleObjects(2,evn,FALSE,INFINITE);
  switch (global_eventtype) {
#else
    XWindowEvent(display[usewindow], win[usewindow],KeyPressMask |KeyReleaseMask|ExposureMask |ButtonReleaseMask| ButtonPressMask, &event);
    switch (event.type) {
      char buf[4];
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
    
    /* Bei Mouse-Taste: */
    case ButtonPress:
#ifdef WINDOWS
      if(global_mousek==1) {
        sbut=objc_find(tree,global_mousex,global_mousey);
#else
      if(event.xbutton.button==1) {
        sbut=objc_find(tree,event.xbutton.x,event.xbutton.y);
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
	    if(edob>=0) draw_edcursor(tree,edob); activate();
          
	    if(tree[sbut].ob_flags & EXIT) {bpress=1;}
	  }
	  if(tree[sbut].ob_flags & TOUCHEXIT) {exitf=1;}
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
    case ButtonRelease:
      if(bpress) exitf=1;
      break;
    
#ifdef WINDOWS
    case KeyChar:   /* Return gedrueckt ? */
    printf("ks=%04x  \n",global_keycode);
    if((global_keycode & 255)==13) {                /* RETURN  */
#else
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
      } else if(edob>=0){
         int i;
         TEDINFO *ted=(TEDINFO *)(tree[edob].ob_spec);
#ifndef WINDOWS
         if(HIBYTE(ks)) {
#endif
#ifdef WINDOWS
           if((global_keycode & 255)==8) {          /* BSP */
#else
	   if(ks==0xff08) {                  /* BACKSPACE   */
#endif
	     if(ted->te_junk1>0) {
	       int len=strlen((char *)ted->te_ptext);
	       i=ted->te_junk1--;
	       while(i<len) ((char *)(ted->te_ptext))[i-1]=((char *)(ted->te_ptext))[i++];
	       ((char *)ted->te_ptext)[i-1]=0;
	       objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate(); 
	     }
#ifndef WINDOWS
	   } else if(ks==0xff51) { /* LEFT */
	     if(ted->te_junk1>0) ted->te_junk1--;
	     objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
	   } else if(ks==0xff53) { /* RIGHT */
	     int len=strlen((char *)ted->te_ptext);
	     if(ted->te_junk1<len && ((char *)ted->te_ptext)[ted->te_junk1]) ted->te_junk1++;
             objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
#endif
#ifdef WINDOWS
           } else if((global_keycode & 255)==9) {          /* TAB */
#else
	   } else if(ks==0xff09) {          /* TAB */
#endif
	     /* Suche naechstes ED-Feld oder wieder das erste */
	     int cp=ted->te_junk1;
	     i=finded(tree,edob,1);
	     if(i<0) edob=finded(tree,0,0);
	     else edob=i;
	     ted=(TEDINFO *)(tree[edob].ob_spec);
	     ted->te_junk1=min(cp,strlen((char *)ted->te_ptext));
	     objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
#ifndef WINDOWS 
	   } else if(ks==0xff52) {
	   /* Suche vorangehendes ED-Feld */
	     int cp=ted->te_junk1;
	     i=finded(tree,edob,-1);
	     if(i>=0) {edob=i;ted=(TEDINFO *)(tree[edob].ob_spec);
	     ted->te_junk1=min(cp,strlen((char *)ted->te_ptext));
	     objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();}
	   } else if(ks==0xff54) { /* Page down */
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
#endif
#ifdef WINDOWS
           } else if((global_keycode & 255)==0x1b) {   /* ESC  */  
#else
	   } else if(ks==0xff1b) {   /* ESC  */               
#endif
	   ((char *)ted->te_ptext)[0]=0;
	   ted->te_junk1=0;
	   objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
#ifndef WINDOWS
	   } else printf("Key: %x\n",ks);
#endif
	} else {
	  i=ted->te_txtlen-1;
	  while(i>ted->te_junk1) {((char *)ted->te_ptext)[i]=((char *)ted->te_ptext)[i-1];i--;}
	   
	  if(ted->te_junk1<ted->te_txtlen) { 
#ifdef WINDOWS
            ((char *)ted->te_ptext)[ted->te_junk1]=(char)global_keycode;
#else
	    ((char *)ted->te_ptext)[ted->te_junk1]=(char)ks;
#endif
	    ted->te_junk1++;
	  }	   
	  objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
        } 
      }       
      break;
    case KeyRelease:
      if(bpress) exitf=1;
      break;
#ifdef WINDOWS
    default:
      ResetEvent(evn[0]);
      ResetEvent(evn[1]);
      break;  
#endif
    }  
  }
  return(sbut);
}

int rsrc_load(char *filename) {
  FILE *dptr;
  char *test;
  OBJECT *tree;
  int i,len;
  if(exist(filename)) {
    dptr=fopen(filename,"r");
    if(dptr==NULL) return(-1);
    len=lof(dptr);
    rsrc=malloc(len);
    if(fread(rsrc,1,len,dptr)==len) {
      WSWAP((char *)((int)rsrc));
      if(rsrc->rsh_vrsn==0 || rsrc->rsh_vrsn==1) {
        if(rsrc->rsh_vrsn==0) {
        for(i=1;i<HDR_LENGTH/2;i++) {
          WSWAP((char *)((int)rsrc+2*i));
        }
        }
#if DEBUG
       printf("RSC loaded: name=<%s> len=%d Bytes\n",filename,len);
       printf("Version: %04x   xlen=%d\n",rsrc->rsh_vrsn,rsrc->rsh_rssize);
       printf("%d Trees and %d FRSTRs \n",rsrc->rsh_ntree,rsrc->rsh_nstring);
       
       printf("OBJC:    %08x  (%d)\n",rsrc->rsh_object,rsrc->rsh_nobs);
       printf("TEDINFO: %08x  (%d)\n",rsrc->rsh_tedinfo,rsrc->rsh_nted);
       printf("ICONBLK: %08x  (%d)\n",rsrc->rsh_iconblk,rsrc->rsh_nib);
       printf("BITBLK:  %08x  (%d)\n",rsrc->rsh_bitblk,rsrc->rsh_nbb);

#endif	
      if(rsrc->rsh_rssize==len || 1) {


        fix_trindex(); 
        fix_frstrindex(); 
	fix_objc();
	fix_tedinfo();
	fix_bitblk();
	fix_iconblk();
	
       	fclose(dptr);
        return(0);
      } else printf("Invalid rsc-Filestructure\n");
      } else printf("Unsupported rsc-Version %d\n",rsrc->rsh_vrsn);
    } 
    fclose(dptr);
    free(rsrc);
    rsrc=NULL;
    return(-1);
  } 
  else return(-1);
}


int do_menu_select() {   
  int nr,i,j,textx,sel=-1;   
  int root_x_return, root_y_return,win_x_return, win_y_return,mask_return;
#ifndef WINDOWS
  Window root_return,child_return;
#endif
  graphics();    
#ifdef WINDOWS
  win_x_return=global_mousex;
  win_y_return=global_mousey;
  mask_return=global_mousek;
#else
  XQueryPointer(display[usewindow], win[usewindow], &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);
#endif
  for(i=0;i<menuanztitle-1;i++) {
   if(menutitleflag[i]) sel=i; 
  }


  /* Maus in der Titelleiste ? */
  if(win_y_return<=sbox.y+chh && win_y_return>=0) {
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
#ifdef WINDOWS
HBITMAP schubladepix; 
HDC schubladedc;
#else
Pixmap schubladepix; 
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
#ifdef WINDOWS
  schubladedc=CreateCompatibleDC(bitcon[usewindow]);
  schubladepix=CreateCompatibleBitmap(bitcon[usewindow],schubladew,schubladeh);
  SelectObject(schubladedc,schubladepix);
  BitBlt(schubladedc,0,0,schubladew,schubladeh,bitcon[usewindow],schubladex,schubladey,SRCCOPY);
#else
  schubladepix=XCreatePixmap(display[usewindow],win[usewindow],schubladew,schubladeh,depth);
  XCopyArea(display[usewindow], pix[usewindow],schubladepix,gc[usewindow],schubladex,schubladey,schubladew,schubladeh,0,0);
#endif
  schubladeff=1;
  schubladenr=nr;
  do_menu_edraw();
}
void do_menu_edraw() {
  int i;
    SetForeground(gem_colors[WHITE]);
#ifndef WINDOWS
    XSetLineAttributes(display[usewindow], gc[usewindow], 1, 0,0,0);
#endif
    FillRectangle(schubladex,schubladey,schubladew-1,schubladeh-1); 
    for(i=0;i<menutitlelen[schubladenr];i++) {      
      if(menuflags[menutitlesp[schubladenr]+i] & SELECTED) {SetForeground(gem_colors[BLACK]);}
      else {SetForeground(gem_colors[WHITE]);}
      FillRectangle(schubladex,schubladey+i*chh,schubladew,chh);
      if(menuflags[menutitlesp[schubladenr]+i] & SELECTED) {SetForeground(gem_colors[WHITE]);}
      else if(menuflags[menutitlesp[schubladenr]+i] & DISABLED) {SetForeground(gem_colors[LWHITE]);}
      else {SetForeground(gem_colors[BLACK]);}
      DrawString(schubladex,schubladey+chh-3+chh*i,menuentry[menutitlesp[schubladenr]+i],menuentryslen[menutitlesp[schubladenr]+i]);
      if(menuflags[menutitlesp[schubladenr]+i] & CHECKED) {
        DrawLine(schubladex+5,schubladey+chh-3+chh*i,schubladex+2,schubladey+chh-8+chh*i);
        DrawLine(schubladex+5,schubladey+chh-3+chh*i,schubladex+chw,schubladey+chh*i);
      }
    }     
    SetForeground(gem_colors[BLACK]);
    DrawRectangle(schubladex,schubladey,schubladew-1,schubladeh-1); 
  activate();
}
void do_menu_close() {
  if(schubladeff) {
#ifdef WINDOWS
    BitBlt(bitcon[usewindow],schubladex,schubladey,schubladew,schubladeh,schubladedc,0,0,SRCCOPY);
    DeleteObject(schubladepix);
    DeleteDC(schubladedc);
#else    
    XCopyArea(display[usewindow], schubladepix,pix[usewindow],gc[usewindow],0,0,schubladew,schubladeh
    ,schubladex,schubladey);
    XFreePixmap(display[usewindow],schubladepix);
#endif
    schubladeff=0;
  }
}
void do_menu_draw() {
  int i,j,textx;
  graphics();
  gem_init();
    
  SetForeground(gem_colors[WHITE]);
#ifndef WINDOWS
    XSetLineAttributes(display[usewindow], gc[usewindow], 1, 0,0,0);
#endif
    FillRectangle(sbox.x,sbox.y,sbox.w,chh); 
    SetForeground(gem_colors[BLACK]);
    DrawLine(sbox.x,sbox.y+chh,sbox.x+sbox.w,sbox.y+chh);
    textx=chw;
    for(i=0;i<menuanztitle-1;i++) {
      if(menutitleflag[i] & SELECTED) {SetForeground(gem_colors[BLACK]);}
      else {SetForeground(gem_colors[WHITE]);}
      FillRectangle(sbox.x+textx,sbox.y,chw*(2+menutitleslen[i]),chh);
      if(menutitleflag[i] & SELECTED) {SetForeground(gem_colors[WHITE]);}
      else {SetForeground(gem_colors[BLACK]);}
      DrawString(sbox.x+textx+chw,sbox.y+chh-3,menutitle[i],menutitleslen[i]);
      textx+=chw*(menutitleslen[i]+2);
    }
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

int dir_bytes(FINFO *dir,int anzfiles) {
  int byt=0,i;
  if(anzfiles){
#ifndef WINDOWS
    for(i=0;i<anzfiles;i++) {
      byt+=dir[i].dstat.st_blocks;
    }
#endif    
    byt*=512;
  }
  return(byt);
}


void make_filelist(OBJECT *objects,FINFO *filenamen,int *filenamensel,int anzfiles,int showstart){
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


int compare_dirs(FINFO *a,FINFO *b) {
  if(a->typ==FT_DIR && b->typ!=FT_DIR) return(-1);
  if(a->typ!=FT_DIR && b->typ==FT_DIR) return(1);
  return(strcmp(a->name,b->name));
}

void sort_dir(FINFO *fileinfos,int anz) {
  qsort((void *)fileinfos,anz,sizeof(FINFO),compare_dirs);
}
#define MAXANZFILES 512
int read_dir(FINFO *fileinfos,int maxentries,char *pfad,char *mask) {
  DIR *dp;
  struct dirent *ep;
  int anzfiles=0;
  struct stat dstat;
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

void make_scaler(OBJECT *objects,int anzfiles,int showstart){
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

char *fsel_input(char *titel, char *pfad, char *sel) {
  char btitel[128],mask[128],dpfad[128],buffer[128];
  char feld1[128],feld2[128],xfeld1[50],xfeld2[20];
  char *ergebnis;
  int i,j,k;
  int sbut=-1;
#ifndef WINDOWS
  XEvent event;
#endif
  FINFO filenamen[MAXANZFILES];

  int filenamensel[MAXANZFILES];
  
  int anzfiles,showstart=0;

  TEDINFO tedinfo[4+ANZSHOW]={
  {(LONG)btitel,(LONG)btitel,(LONG)btitel,FONT_IBM,0,TE_CNTR,0x1200,0,0,0,0},
  {(LONG)mask,(LONG)mask,(LONG)mask,FONT_IBM,0,TE_CNTR,0x113a,0,2,0,FWW},
  {(LONG)feld1,(LONG)xfeld1,(LONG)btitel,FONT_IBM,0,TE_LEFT,0x1100,0,0,128,50},
  {(LONG)feld2,(LONG)xfeld2,(LONG)btitel,FONT_IBM,0,TE_LEFT,0x1100,0,0,128,20}
  };
  int anztedinfo=sizeof(tedinfo)/sizeof(TEDINFO);
  OBJECT objects[18+2*ANZSHOW]={
/* 0*/  {-1,1,14,G_BOX, NONE, OUTLINED, 0x00021100, 0,0,54,23},
/* 1*/  {2,-1,-1,G_BUTTON, SELECTABLE|DEFAULT|EXIT,NORMAL ,(LONG)"OK", 42,18,9,1},
#ifdef GERMAN
/* 2*/  {3,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"ABBRUCH",   42,20,9,1},
#else
/* 2*/  {3,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"CANCEL",    42,20,9,1},
#endif
/* 3*/  {4,-1,-1,G_BUTTON, SELECTABLE|EXIT, NORMAL, (LONG)"HOME",      42,12,9,1},
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
  tedinfo[0].te_tmplen=strlen(btitel);
  for(i=0;i<ANZSHOW;i++){
   tedinfo[4+i].te_ptext=(LONG)"Hallo";
    tedinfo[4+i].te_ptmplt=(LONG)buffer;
    tedinfo[4+i].te_pvalid=(LONG)btitel;
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
#ifdef DEBUG
  printf("**next: ANZSHOW=%d btitel=%s\n",ANZSHOW,btitel);
#endif
  graphics();
  gem_init();
  for(i=0;i<objccount;i++){
    objects[i].ob_x*=chw;
    objects[i].ob_y*=chh;
    objects[i].ob_width*=chw;
    objects[i].ob_height*=chh;
  }
#ifdef DEBUG
  printf("**3fsel_input:\n");
#endif

 
  wort_sepr(pfad,'/',0,dpfad, mask);
  anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
  sort_dir(filenamen,anzfiles);

  strcpy(feld1,pfad);
  strcpy(feld2,sel);

  for(i=0;i<anzfiles;i++) {
    if(strcmp(filenamen[i].name,feld2)==0) {
      filenamensel[i]=1;
    } else {
      filenamensel[i]=0;
    }
    
  }

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
       
      }objects[sbut].ob_state=NORMAL;
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
      }objects[sbut].ob_state=NORMAL;
    } else if(sbut==8) {    /* v */
      if(showstart<anzfiles-ANZSHOW) {
        showstart++;
        make_filelist(objects,filenamen,filenamensel,anzfiles,showstart);
        make_scaler(objects,anzfiles,showstart);
      }objects[sbut].ob_state=NORMAL;
    } else if(sbut==9) {    /* ? */
      char buf[128];
      sprintf(buf,"[1][%d Bytes in %d Files.][ OK ]",dir_bytes(filenamen,anzfiles),anzfiles);
      form_alert(1,buf);objects[sbut].ob_state=NORMAL;
    } else if(sbut==10) {    /* MASK */
      wort_sepr(tedinfo[2].te_ptext,'/',0,dpfad, mask);
      anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
      sort_dir(filenamen,anzfiles);
      showstart=max(0,min(showstart,anzfiles-ANZSHOW));
      make_filelist(objects,filenamen, filenamensel,anzfiles,showstart);
      make_scaler(objects,anzfiles,showstart);objects[sbut].ob_state=NORMAL;
    } else if(sbut==15) {    /* Scalerhintergrund */
      #ifndef WINDOWS
      XWindowEvent(display[usewindow], win[usewindow],
       ButtonReleaseMask|ExposureMask , &event);
      switch (event.type) {
        
    /* Das Redraw-Event */  
      case Expose:
        XCopyArea(display[usewindow],pix[usewindow],win[usewindow],gc[usewindow],
          event.xexpose.x,event.xexpose.y,
          event.xexpose.width,event.xexpose.height,
          event.xexpose.x,event.xexpose.y);
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
      int ex=0,root_x_return,root_y_return,win_x_return,win_y_return,mask_return;   
      int ssold=showstart;
      int sssold=showstart;
      #ifndef WINDOWS
      Window root_return,child_return;
       XQueryPointer(display[usewindow], win[usewindow], &root_return, &child_return,
       &root_x_return, &root_y_return,
       &win_x_return, &win_y_return,&mask_return);

      while(ex==0) {
       XWindowEvent(display[usewindow], win[usewindow],
       ButtonReleaseMask|PointerMotionMask|ExposureMask , &event);
      switch (event.type) {
        
    /* Das Redraw-Event */  
      case Expose:
        XCopyArea(display[usewindow],pix[usewindow],win[usewindow],gc[usewindow],
          event.xexpose.x,event.xexpose.y,
          event.xexpose.width,event.xexpose.height,
          event.xexpose.x,event.xexpose.y);
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
     }}
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
  if(sbut==1) sprintf(ergebnis,"%s/%s",dpfad,tedinfo[3].te_ptext);
  else ergebnis[0]=0;
  return(ergebnis);
}
#endif /* nographics */
