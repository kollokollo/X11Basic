
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
#include <fnmatch.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <X11/XWDFile.h>

#include "window.h"


/* globale Variablen */

int winbesetzt[MAXWINDOWS];
Window win[MAXWINDOWS];                 
Pixmap pix[MAXWINDOWS];
Display *display[MAXWINDOWS];            
GC gc[MAXWINDOWS];                      /* Im Gc wird Font, Farbe, Linienart, u.s.w.*/

XSizeHints size_hints[MAXWINDOWS];       /* Hinweise fuer den WIndow-Manager..*/
XWMHints wm_hints[MAXWINDOWS];
XClassHint class_hint[MAXWINDOWS];
Pixmap icon_pixmap[MAXWINDOWS];
XTextProperty win_name[MAXWINDOWS], icon_name[MAXWINDOWS];

static char dash_list [] = { 12, 24};
char wname[MAXWINDOWS][80];
char iname[MAXWINDOWS][80];  

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



int create_window(char *title, char* info,unsigned int x,unsigned int y,unsigned int w,unsigned int h) {
  int nummer=0;
  while(winbesetzt[nummer] && nummer<MAXWINDOWS) nummer++;
  if(nummer>=MAXWINDOWS) {
      printf("Keine weiteren Fenster möglich !\n");
      return(-2);
  }
  return(create_window2(nummer,title,info,x,y,w,h));
}
int create_window2(int nummer,char *title, char* info,unsigned int x,unsigned int y,unsigned int w,unsigned int h) {
  
  int screen_num;              /* Ein Server kann mehrere Bildschirme haben */
  unsigned long border=4,foreground,background;
  int i,d,b;
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
  class_hint[nummer].res_name = "XBASIC";
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
    
  return(nummer);
}


void open_window( int nr) {
  XEvent event;
    /* Das Fensterauf den Screen Mappen */
  if(winbesetzt[nr]) {
    XMapWindow(display[nr], win[nr]);
    XNextEvent(display[nr], &event);
    handle_event(nr,&event);
  }
}

void close_window(int nr) { 
  XEvent event;
      if(winbesetzt[nr]) {
        XUnmapWindow(display[nr], win[nr]);
        XCheckWindowEvent(display[nr],win[nr],ExposureMask, &event);
      }
}

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
void handle_window(int winnr) {
  
  if(winbesetzt[winnr]) {
    XEvent event;
   
    while(XCheckWindowEvent(display[winnr],win[winnr] ,
        ExposureMask| 
	ButtonPressMask| 
	PointerMotionMask |
	KeyPressMask, &event)) {
       handle_event(winnr,&event);  
    } 
  }
}


void graphics(){
  
  if(winbesetzt[usewindow]) {handle_window(usewindow);return;}
  else {
     if(usewindow==0) {
       fetch_rootwindow();
     } else {
       create_window2(usewindow,"Grafik","X11BASIC",100,10,640,400);
       open_window(usewindow);
     }
  }
}


int fetch_rootwindow() {

  char *display_name = NULL;   /* NULL: Nimm Argument aus setenv DISPLAY */
  unsigned long border=4,foreground,background;
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
  return(0);
}

void activate() {  
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
}



/*-------------------------------------------------------------------*/
/*               Routine zum Abspeichern von X-Image                 */
/*             Bitmaps.            (c) Markus hoffmann               */
/*-------------------------------------------------------------------*/



/* swap some long ints.  (n is number of BYTES, not number of longs) */
swapdws (bp, n)
  register char *bp;
  register unsigned n;
{
  register char c;
  register char *ep = bp + n;
  register char *sp;

  while (bp<ep) {
    sp=bp+3;
    c=*sp;*sp=*bp; *bp++=c; sp=bp+1;
    c=*sp;*sp=*bp; *bp++=c; bp+=2;
  }
}


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
    if (*(char *) &swaptest)    swapdws(data, sizeof(XWDFileHeader));
    return((char *)data);
}














/* Alloziert einen Farbeintrag in der Palette. 
   Rueckgabewert ist die Nummer des Farbeintrags.
   Ist kein neuer Eintrag mehr frei, so wird die Nummer des Eintrags
   zurueckgeliefert, der der spezifizierten Farbe am naechsten kommt.
   Diese Routine kann also kein XAllocColor failed mehr produzieren.

   (c) markus hoffmann  1998                                   */
 Status my_XAllocColor(Display *,Colormap,XColor *);
   
int get_color(int r, int g, int b) {
  Colormap map;
  XColor pixcolor;

  graphics();  
  map =XDefaultColormapOfScreen ( XDefaultScreenOfDisplay ( display[usewindow] ) );

  pixcolor.red=r;
  pixcolor.green=g;
  pixcolor.blue=b;
  if(my_XAllocColor(display[usewindow], map, &pixcolor)==0) {
    printf("Es konnte partout keine passende Farbe gefunden werden.\n");    
  } 
  return(pixcolor.pixel);
}


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




/* AES-Nachbildungen (c) Markus Hoffmann     */
#define GEMFONT "-*-lucidatypewriter-medium-r-*14*-m-*"
#define GEMFONTSMALL "-*-lucidatypewriter-medium-r-*10*-m-*"

#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a<b)?a:b)




typedef struct {int x,y,w,h;char *t,*p;int shift,cp,maxlen;} ETEXT;
typedef struct {int x,y,w,h,flags,state; char *string;} BUTTON;

/* GEM-Globals   */

int chw=8,chh=16,baseline=14,depth=8,border=4;
RECT sbox;

int weiss,schwarz,rot,grau,gelb;


void load_GEMFONT(int n) {
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
}

void gem_init() {
    Window root;

    /* Screendimensionen bestimmem */
    XGetGeometry(display[usewindow],win[usewindow],&root,&sbox.x,&sbox.y,&sbox.w,&sbox.h,&border,&depth); 
   load_GEMFONT(1);
   
   weiss=WhitePixel(display[usewindow], DefaultScreen(display[usewindow]));
   schwarz=BlackPixel(display[usewindow], DefaultScreen(display[usewindow]));
   grau=get_color(50000,50000,50000);
   rot=get_color(65535,0,0);
   gelb=get_color(65535,65535,0);
}




void box_center(RECT *b) {
  
    b->x=sbox.x+(sbox.w-b->w)/2; /* Koordinaten fuer Bildmitte: */
    b->y=sbox.y+(sbox.h-b->h)/2;

}

void draw_button(BUTTON *b) {
      XSetForeground(display[usewindow],gc[usewindow],schwarz);

      if(b->state & SELECTED)  XSetForeground(display[usewindow],gc[usewindow],schwarz);
      else XSetForeground(display[usewindow],gc[usewindow],weiss);
      
      XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],b->x,b->y,b->w,b->h);
      XSetForeground(display[usewindow],gc[usewindow],schwarz);
      XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],b->x,b->y,b->w,b->h);
  

      if(b->flags & EXIT) XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],b->x+1,b->y+1,b->w-2,b->h-2);
      if(b->state & SELECTED) XSetForeground(display[usewindow],gc[usewindow],weiss);
      XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
        b->x+(b->w-strlen(b->string)*chw)/2,
	b->y+(b->h+baseline)/2,b->string,strlen(b->string));
      XSetForeground(display[usewindow],gc[usewindow],schwarz);
      if(b->flags & DEFAULT) 
        XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],b->x-1,b->y-1,b->w+2,b->h+2);   
}

/* Form-Alert-Routine. Dem GEM nachempfunden.
(c) markus Hoffmann   1998               */

double ltext(int, int, double, double, double , int, char *);

int form_alert(int dbut,char *n) {
  char *bzeilen[30],*bbuttons[30];
  BUTTON buttons[20];
  int anzzeilen,anzbuttons;
  int symbol,sbut=dbut;
  char *pos;
  char **ein=bzeilen;
  int i=0,j=strlen(n),k=0,l=0;
  
  OBJECT objects[]={{-1,1,1,G_BOX, 0, OUTLINED, 0x00011100, 0,0,100,100},
                    {-1,0,-1,G_BUTTON, SELECTABLE|DEFAULT|EXIT|LASTOB, NORMAL, 0x00011100, 0,0,100,100}};
    
  RECT box;
  
  while(i<j) {
    if(n[i]=='[') {
      pos=&n[i+1];
      k++;
      l=0;
    } else if(n[i]==']') {
      n[i]=0;
      if(k>0) ein[l]=pos;
      l++;
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
  for(i=0;i<anzbuttons;i++) buttons[i].string=bbuttons[i];
  if(anzbuttons) {
    /* Box-Abmessungen bestimmen */
    int textx;
    int maxc=0;
    int exitf=0,bpress=0;
    Pixmap spix; 
    XEvent event;
    GC sgc;
    XGCValues gc_val;

    if(symbol) {box.w=box.h=textx=64;}
    else {box.w=box.h=textx=0;}
    graphics();
    gem_init();

    /*Raender*/
    box.w+=chh*2;
    box.h=max(box.h+2*chh,chh*2+(anzzeilen+2)*chh);
    
    
    for(i=0;i<anzzeilen;i++) maxc=max(maxc,strlen(bzeilen[i]));
    box.w+=chw*maxc;
    
     /* Buttons  */
    maxc=0;
    for(i=0;i<anzbuttons;i++) maxc=max(maxc,strlen(bbuttons[i]));
    box.w=max(box.w,chw*(4+(maxc+1)*anzbuttons+2*(anzbuttons-1)));
   
    box_center(&box);

    for(i=0;i<anzbuttons; i++) {
      buttons[i].w=(chw+1)*maxc;
      buttons[i].h=chh+3;
      buttons[i].y=box.y+box.h-2*chh;
      buttons[i].x=(sbox.w-chw*((maxc+1)*anzbuttons+2*(anzbuttons-1)))/2+i*chw*(maxc+3);
      buttons[i].state=NORMAL;
      buttons[i].flags=(EXIT|SELECTABLE);
    }
    if(dbut>0 && dbut<=anzbuttons) buttons[dbut-1].flags|=(DEFAULT);

    form_dial(0,0,0,0,0,box.x,box.y,box.w,box.h);

    /* Box zeichenen  */
    XSetForeground(display[usewindow],gc[usewindow],weiss);
    XSetLineAttributes(display[usewindow], gc[usewindow], 1, 0,0,0);
    XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],box.x,box.y,box.w,box.h); 
    XSetForeground(display[usewindow],gc[usewindow],schwarz);

    XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],box.x,box.y,box.w-1,box.h-1); 
    XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],box.x+3,box.y+3,box.w-7,box.h-7); 
    XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],box.x+2,box.y+2,box.w-5,box.h-5); 
  
    
     /* Symbol */
     
    if(symbol>=1) {
      char chr[2];
      chr[0]=symbol+4;
      chr[1]=0;
      XSetForeground(display[usewindow],gc[usewindow],rot);
      XSetLineAttributes(display[usewindow], gc[usewindow], 2, 0,0,0);
      ltext(box.x+chh,box.y+chh,0.5,0.5,0,0,chr); 
      if(symbol==3) ltext(box.x+4+chh,box.y+chh+12,0.5/6,0.5/2,0,0,"STOP");
    }
    XSetLineAttributes(display[usewindow], gc[usewindow], 1, 0,0,0);
    XSetForeground(display[usewindow],gc[usewindow],schwarz);

     /* Text  */
    for(i=0;i<anzzeilen;i++) XDrawString(display[usewindow],pix[usewindow],gc[usewindow],box.x+textx+chh,box.y+2*chh+i*chh,bzeilen[i],strlen(bzeilen[i]));

     /* Buttons  */
    for(i=0;i<anzbuttons; i++) draw_button(&buttons[i]);
      
    /* Auf Tasten/Maus reagieren */
    activate();
    while(exitf==0) {
      XWindowEvent(display[usewindow], win[usewindow],KeyPressMask |KeyReleaseMask|ExposureMask |ButtonReleaseMask| ButtonPressMask, &event);
      switch (event.type) {
        char buf[4];
        XComposeStatus status;
        KeySym ks;
    /* Das Redraw-Event */  
      case Expose:
        XCopyArea(display[usewindow],pix[usewindow],win[usewindow],gc[usewindow],
          event.xexpose.x,event.xexpose.y,
          event.xexpose.width,
          event.xexpose.height,
          event.xexpose.x,event.xexpose.y);
        break;
    
    /* Bei Mouse-Taste: Ende */
      case ButtonPress: 
     
        if(event.xbutton.button==1) {
          for(i=0;i<anzbuttons;i++) {
	    if(event.xbutton.x>buttons[i].x && event.xbutton.x<buttons[i].x+buttons[i].w
	     && event.xbutton.y>buttons[i].y && event.xbutton.y<buttons[i].y+buttons[i].h)
            { bpress=1; 
	      sbut=i+1;
              buttons[i].state^=SELECTED;
	      draw_button(&buttons[i]);
	      activate();
	    } 
	  }
        }
        break;
      case ButtonRelease:
        if(bpress) exitf=1;
        break;
      case KeyPress:   /* Return gedrueckt ? */
     
       XLookupString((XKeyEvent *)&event,buf,sizeof(buf),&ks,&status);   
       if((ks & 255)==13) {
         sbut=dbut;
         i=sbut-1;bpress=1;
	 buttons[i].state^=SELECTED;
	 draw_button(&buttons[i]);
	 activate();
       }
       break;
     case KeyRelease:
       if(bpress) exitf=1;
       break;
     }
    }
    form_dial(3,0,0,0,0,box.x,box.y,box.w,box.h);
    return(sbut);
  } else return(0);
}

/* -------------------------------- AES-Implementationen ------------------*/

RSHDR *rsrc;    /* Adresse der Recource im Speicher */

int rsrc_free() {
  free(rsrc);
  return(0);
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
  char zeichen;
  int fcolor=1;
  int bcolor=0;
  int i;
  int obx=tree[idx].ob_x+rootx;
  int oby=tree[idx].ob_y+rooty;
  int obw=tree[idx].ob_width;
  int obh=tree[idx].ob_height;
  LONG colorspec=0;
  
 /* printf("Drawobjc: %d   head=%d  next=%d tail=%d\n",idx,tree[idx].ob_head, 
  tree[idx].ob_next, tree[idx].ob_tail); */
  switch(tree[idx].ob_type) {
  case G_BOX:
  case G_BOXCHAR:
    zeichen=(tree[idx].ob_spec & 0xff000000)>>24;
    randdicke=(tree[idx].ob_spec & 0xff0000)>>16;
    break;
  case G_IBOX:
  case G_TEXT:
  case G_FTEXT:
   randdicke=0;
  case G_STRING:
  case G_TITLE:
    randdicke=0;break;
  case G_BUTTON:
    randdicke=-1;break;
    
    break;
  
  case G_BOXTEXT:  
  case G_FBOXTEXT:
   randdicke=((TEDINFO *)((int)tree[idx].ob_spec+(int)rsrc))->te_thickness;
   break;
    }
if(tree[idx].ob_flags & EXIT) randdicke--;
if(tree[idx].ob_flags & DEFAULT) randdicke--;

/* Zeichnen  */

  if(tree[idx].ob_state & OUTLINED) {
    XSetForeground(display[usewindow],gc[usewindow],weiss);
    XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],obx-3,oby-3,obw+6,obh+6);
    XSetForeground(display[usewindow],gc[usewindow],schwarz);
    XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],obx-3,oby-3,obw+6,obh+6);
  }
  if(tree[idx].ob_state & SHADOWED) {
    XSetBackground(display[usewindow],gc[usewindow],schwarz);
    XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],obx+obw,oby+chh/2,chw/2,obh);
    XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],obx+chw/2,oby+obh,obw,chh/2);
  }
  
  

  if(bcolor==1) {
    XSetForeground(display[usewindow],gc[usewindow],schwarz);
    XSetBackground(display[usewindow],gc[usewindow],schwarz);}
  else {
  XSetForeground(display[usewindow],gc[usewindow],weiss);
  XSetBackground(display[usewindow],gc[usewindow],weiss);
  }
  switch(tree[idx].ob_type) {
  
  case G_BOXTEXT:  
  case G_FBOXTEXT:
  case G_TEXT:
  case G_FTEXT:
  colorspec=(LONG)((TEDINFO *)((int)tree[idx].ob_spec+(int)rsrc))->te_color;
  break;
  case G_BOX:
  case G_BOXCHAR:
  colorspec=tree[idx].ob_spec;
  break;
   }
  if((colorspec & 0x70)==0x40) XSetForeground(display[usewindow],gc[usewindow],grau);
  else if((colorspec & 0x70)==0x10) XSetForeground(display[usewindow],gc[usewindow],gelb);
  else if((colorspec & 0x70)==0x20) XSetForeground(display[usewindow],gc[usewindow],gelb);
  else if((colorspec & 0x70)==0x30) XSetForeground(display[usewindow],gc[usewindow],gelb);
  else if((colorspec & 0x70)==0x50) XSetForeground(display[usewindow],gc[usewindow],grau);
  else if((colorspec & 0x70)==0x60) XSetForeground(display[usewindow],gc[usewindow],grau);
  else if((colorspec & 0x70)==0x70) XSetForeground(display[usewindow],gc[usewindow],schwarz);
  if(tree[idx].ob_state & SELECTED) {
    XSetForeground(display[usewindow],gc[usewindow],schwarz);
    XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],obx,oby,obw,obh);
    }
  else if(!(colorspec & 0x80)) XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],obx,oby,obw,obh);

  
  if(fcolor==1) XSetForeground(display[usewindow],gc[usewindow],schwarz);
  else XSetForeground(display[usewindow],gc[usewindow],weiss);
  if(randdicke>0) {
    for(i=0;i<randdicke;i++) {
      XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],obx+i,oby+i,obw-2*i,obh-2*i);
    }
  } else if(randdicke<0) {
    for(i=0;i>randdicke;i--) {      
       XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],obx+i,oby+i,obw-2*i,obh-2*i);

      }
    }
    
    
    if(tree[idx].ob_state & DISABLED) XSetForeground(display[usewindow],gc[usewindow],grau);
    else   if(tree[idx].ob_state & SELECTED) XSetForeground(display[usewindow],gc[usewindow],weiss);
  else XSetForeground(display[usewindow],gc[usewindow],schwarz);

    
  if(tree[idx].ob_type==G_STRING || tree[idx].ob_type==G_TITLE) {
  
    char *text=(char *)((int)tree[idx].ob_spec+(int)rsrc);
    
    XDrawString(display[usewindow],pix[usewindow],gc[usewindow],obx,oby+chh-2,text,strlen(text));
  } else if(tree[idx].ob_type==G_BUTTON) {
    char *text=(char *)((int)tree[idx].ob_spec+(int)rsrc);
    XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
    obx+(obw-chw*strlen(text))/2,oby+chh-2+(obh-chh)/2,text,strlen(text));
  } else if(tree[idx].ob_type==G_BOXCHAR) {
    XDrawString(display[usewindow],pix[usewindow],gc[usewindow],obx+(obw-chw)/2,oby+chh-2+(obh-chh)/2,&zeichen,1);
    
  } else if(tree[idx].ob_type==G_TEXT || tree[idx].ob_type==G_FTEXT ||tree[idx].ob_type==G_BOXTEXT || tree[idx].ob_type==G_FBOXTEXT) {
    TEDINFO *ted=(TEDINFO *)((int)tree[idx].ob_spec+(int)rsrc);
    char *text=(char *)(ted->te_ptext);
    int x,y;
    load_GEMFONT(ted->te_font);
    if(ted->te_just==TE_LEFT) {
      x=obx; y=oby+chh-2+(obh-chh)/2;
    } else if(ted->te_just==TE_RIGHT) {
      x=obx+obw-chw*strlen(text); y=oby+chh-2+(obh-chh)/2;
    } else {
      x=obx+(obw-chw*strlen(text))/2; y=oby+chh-2+(obh-chh)/2;
    }
    if(tree[idx].ob_state & DISABLED) XSetForeground(display[usewindow],gc[usewindow],grau);
    else   if(tree[idx].ob_state & SELECTED) XSetForeground(display[usewindow],gc[usewindow],weiss);
    else if((colorspec & 0xf00)==0x000) XSetForeground(display[usewindow],gc[usewindow],weiss);
    else if((colorspec & 0xf00)==0x100) XSetForeground(display[usewindow],gc[usewindow],schwarz);    
    else XSetForeground(display[usewindow],gc[usewindow],rot);

    XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
    x,y,text,strlen(text));
    load_GEMFONT(1);
  
  }else if(tree[idx].ob_type==G_IMAGE) {
    BITBLK *bit=(BITBLK *)((int)tree[idx].ob_spec+(int)rsrc);
    Pixmap bitpix;
    unsigned int adr;
    adr=((bit->dummy)<<16) | bit->bi_pdata;
    printf("***Achtung BITMAP* %x   **\n",adr);
    bitpix=XCreateBitmapFromData(display[usewindow],win[usewindow],
    (char *)adr,bit->bi_wb*8,bit->bi_hl);
    printf("BITMAP liegt vor***\n"); 
    if(bit->bi_color==0) XSetForeground(display[usewindow],gc[usewindow],weiss);
    else if(bit->bi_color==1) XSetForeground(display[usewindow],gc[usewindow],schwarz);
    else XSetForeground(display[usewindow],gc[usewindow],rot);
    
    XCopyPlane(display[usewindow],bitpix,pix[usewindow],gc[usewindow],
     0,0,bit->bi_wb*8,bit->bi_hl,obx,oby,1);
    XFreePixmap(display[usewindow],bitpix);
}
  if(tree[idx].ob_state & CROSSED) { 
    XSetForeground(display[usewindow],gc[usewindow],rot);
    XDrawLine(display[usewindow],pix[usewindow],gc[usewindow],obx,oby,obx+obw,oby+obh); 
    XDrawLine(display[usewindow],pix[usewindow],gc[usewindow],obx+obw,oby,obx,oby+obh); 
  }


}

int objc_draw( OBJECT *tree,int start, int stop,int rootx,int rooty) {
  int idx=start;
  
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
    if(idx==stop) return(1);
    if(tree[idx].ob_head!=-1) {
      if(!(tree[idx].ob_flags & HIDETREE)) objc_draw(tree,tree[idx].ob_head,tree[idx].ob_tail,tree[idx].ob_x+rootx,tree[idx].ob_y+rooty );
    }
  }
}


int rsrc_gaddr(int re_gtype, unsigned int re_gindex, char **re_gaddr) {
  char **ptreebase;
    if(re_gtype==R_TREE) {
    if(re_gindex>=rsrc->rsh_ntree) return(0);
    ptreebase = (char **)((unsigned int)rsrc+(unsigned int)rsrc->rsh_trindex);
    *re_gaddr=(char *)(((int)ptreebase[re_gindex]+(int)rsrc));
    return(1);
  } else if(re_gtype==R_FRSTR) {
    if(re_gindex>=rsrc->rsh_nstring) return(0);
    ptreebase = (char **)((unsigned int)rsrc+(unsigned int)rsrc->rsh_frstr);
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
  for(i = anzahl-1; i >= 0; i--) {
    LWSWAP((short *)(&ptreebase[i]));
  }
}
void fix_frstrindex() {
  int i;
  char **ptreebase;
  int anzahl;
  
  ptreebase = (char **)((unsigned int)rsrc+(unsigned int)rsrc->rsh_frstr);
  anzahl=rsrc->rsh_nstring;
  if(anzahl) {
    for(i = anzahl-1; i >= 0; i--) {
      LWSWAP((short *)(&ptreebase[i]));
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
	  for(j=0;j<sizeof(OBJECT)/2;j++) {
            WSWAP((char *)((int)&base[i]+2*j));
          }
	  LSWAP((short *)&(base[i].ob_spec));
	  
	  base[i].ob_x=(base[i].ob_x & 0xff)*chw+((base[i].ob_x & 0xff00)>>8);
	  base[i].ob_y=(base[i].ob_y & 0xff)*chh+((base[i].ob_y & 0xff00)>>8);
	  base[i].ob_width=(base[i].ob_width & 0xff)*chw+((base[i].ob_width & 0xff00)>>8);
	  base[i].ob_height=(base[i].ob_height & 0xff)*chh+((base[i].ob_height & 0xff00)>>8);
	  
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
	  for(j=0;j<sizeof(TEDINFO)/2;j++) {
            WSWAP((char *)((int)&base[i]+2*j));
          }
	  LSWAP((short *)&(base[i].te_ptext));
	  LSWAP((short *)&(base[i].te_ptmplt));
	  LSWAP((short *)&(base[i].te_pvalid));
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
	printf("sizeof: %d\n",sizeof(BITBLK));
	if(anzahl) {
	for(i =0; i < anzahl; i++) {
	  for(j=0;j<sizeof(BITBLK)/2;j++) {
            WSWAP((char *)((int)&base[i]+2*j));
          }
	  LSWAP((short *)&(base[i].bi_pdata));
	  k=((base[i].dummy)<<16) | base[i].bi_pdata;
	  printf("Bitmap #%d at %x\n",i,k); 
	  printf("w=%d h=%d x=%d y=%d c=%d\n",base[i].bi_wb,base[i].bi_hl,base[i].bi_x,base[i].bi_y,base[i].bi_color); 
	  k+=(int)rsrc;
	  base[i].bi_pdata=(k & 0xffff);
	  base[i].dummy=((k & 0xffff0000)>>16);
	  
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
     TEDINFO *ted=(TEDINFO *)(tree[ndx].ob_spec+(LONG)rsrc);
     int x,y;
 
     relobxy(tree,ndx,&x,&y);
     
     XSetForeground(display[usewindow],gc[usewindow],rot);
     XDrawLine(display[usewindow],pix[usewindow],gc[usewindow],
     x+chw*(ted->te_junk1-ted->te_junk2),y,x+chw*(ted->te_junk1-ted->te_junk2),
     y+chh+4); 
     XSetForeground(display[usewindow],gc[usewindow],schwarz);
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
  static GC sgc;
  static Pixmap spix; 
  XGCValues gc_val;
  
  graphics();
  switch(fo_diflag){
   case 0:
   gem_init();
     /* Erst den Graphic-Kontext retten  */
  sgc=XCreateGC(display[usewindow], win[usewindow], 0, &gc_val);
  XCopyGC(display[usewindow], gc[usewindow],GCForeground|
                              GCFunction |GCLineWidth |GCLineStyle|
			      GCFont, sgc);
  set_graphmode(GXcopy);
    
    /* Hintergrund retten  */
  spix=XCreatePixmap(display[usewindow],win[usewindow],w2+8,h2+8,depth);
  XCopyArea(display[usewindow], pix[usewindow],spix,gc[usewindow],x2-3,y2-3,w2+8,h2+8,0,0);
   break;
   case 3:    
   /* Hintergrund restaurieren  */
    XCopyArea(display[usewindow], spix,pix[usewindow],gc[usewindow],0,0,w2+8,
    h2+8,x2-3,y2-3);
    XFreePixmap(display[usewindow],spix);
    XCopyGC(display[usewindow], sgc,GCForeground| GCFunction |GCLineWidth |GCLineStyle| GCFont, gc[usewindow]);
    XFreeGC(display[usewindow],sgc); 
    activate();
   break;
   default:
     return(-1);
  
  }
}

int form_center(OBJECT *tree, int *x, int *y, int *w, int *h) {
  graphics();
  gem_init();

  /* Objektbaum Zentrieren */
  tree->ob_x=sbox.x+(sbox.w-tree->ob_width)/2;
  tree->ob_y=sbox.y+(sbox.h-tree->ob_height)/2;    
  *x=tree->ob_x;
  *y=tree->ob_y;
  *w=tree->ob_width;
  *h=tree->ob_height;
  return(0);
}


int form_do(OBJECT *tree) {   
  XEvent event;
  int exitf=0,bpress=0;
  int sbut,edob=-1,idx;
  XGCValues gc_val;
  int x,y,w,h;
  

  /* Objektbaum Zentrieren */
  form_center(tree, &x,&y,&w,&h);
  
  /* Erst den Graphic-Kontext retten  */

  form_dial(0,0,0,0,0,x,y,w,h);
  form_dial(1,0,0,0,0,x,y,w,h);


    /* erstet editierbare Objekt finden */
 
  edob=finded(tree,0,0);
  objc_draw(tree,0,-1,0,0); 
	  
  /* Cursor plazieren */
	  
  if(edob>=0) draw_edcursor(tree,edob);
	  
  /* Auf Tasten/Maus reagieren */
  activate();
  while(exitf==0) {
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
    
    /* Bei Mouse-Taste: */
    case ButtonPress:
      if(event.xbutton.button==1) {
        sbut=objc_find(tree,event.xbutton.x,event.xbutton.y);
        if(sbut!=-1) {
	if((tree[sbut].ob_flags & SELECTABLE) && 
	!(tree[sbut].ob_state & DISABLED)) {
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
	  if(tree[sbut].ob_flags & EDITABLE) {edob=sbut;objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();}
	}
      } else bpress=1;
      break;
    case ButtonRelease:
      if(bpress) exitf=1;
      break;
    case KeyPress:   /* Return gedrueckt ? */
      XLookupString((XKeyEvent *)&event,buf,sizeof(buf),&ks,&status);   
      if((ks & 255)==13) {                /* RETURN  */
        int idx=0;
	while(1) {
	  if(tree[idx].ob_flags & DEFAULT) {
	    tree[idx].ob_state^=SELECTED;
	    objc_draw(tree,0,-1,0,0); activate();
	    if(tree[idx].ob_flags & EXIT) {bpress=1;}
          }
	  if(tree[idx].ob_flags & LASTOB) break;
	  idx++;
	}
      } else if(edob>=0){
         int i;
         TEDINFO *ted=(TEDINFO *)(tree[edob].ob_spec+(LONG)rsrc);
         if((ks & 0xff00)) {	   
	   if(ks==0xff08) {                  /* BACKSPACE   */
	     if(ted->te_junk1>0) {
	       int len=strlen((char *)ted->te_ptext);
	       i=ted->te_junk1--;
	       while(i<len) ((char *)(ted->te_ptext))[i-1]=((char *)(ted->te_ptext))[i++];
	       ((char *)ted->te_ptext)[i-1]=0;
	       objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate(); 
	     }
	   } else if(ks==0xff51) {
	     if(ted->te_junk1>0) ted->te_junk1--;
	     objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
	   } else if(ks==0xff53) {
	     int len=strlen((char *)ted->te_ptext);
	     if(ted->te_junk1<len && ((char *)ted->te_ptext)[ted->te_junk1]) ted->te_junk1++;
             objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
	   } else if(ks==0xff09) {          /* TAB */
	     /* Suche naechstes ED-Feld oder wieder das erste */
	     int cp=ted->te_junk1;
	     i=finded(tree,edob,1);
	     if(i<0) edob=finded(tree,0,0);
	     else edob=i;
	     ted=(TEDINFO *)(tree[edob].ob_spec+(LONG)rsrc);
	     ted->te_junk1=min(cp,strlen((char *)ted->te_ptext));
	     objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
	   
	   } else if(ks==0xff52) {
	   /* Suche vorangehendes ED-Feld */
	     int cp=ted->te_junk1;
	     i=finded(tree,edob,-1);
	     if(i>=0) {edob=i;ted=(TEDINFO *)(tree[edob].ob_spec+(LONG)rsrc);
	     ted->te_junk1=min(cp,strlen((char *)ted->te_ptext));
	     objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();}
	   } else if(ks==0xff54) {
	     int cp=ted->te_junk1;
	     /* Page down */
	     /* Suche naechstes ED-Feld  */
	     i=finded(tree,edob,1);
	     if(i>=0) {edob=i;
	     ted=(TEDINFO *)(tree[edob].ob_spec+(LONG)rsrc);
	     ted->te_junk1=min(cp,strlen((char *)ted->te_ptext));
	     objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();}
	   } else if(ks==0xff1b) {                 
	   /* ESC  */ 
	   ((char *)ted->te_ptext)[0]=0;
	   ted->te_junk1=0;
	   objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
	   } else printf("Key: %x\n",ks);
	 } else {
	   i=ted->te_tmplen-2;
	   while(i>ted->te_junk1) {((char *)ted->te_ptext)[i]=((char *)ted->te_ptext)[i-1];i--;}
	   ((char *)ted->te_ptext)[ted->te_junk1]=(char)ks;
	   if(ted->te_junk1<ted->te_tmplen-2) ted->te_junk1++;
	   objc_draw(tree,0,-1,0,0);draw_edcursor(tree,edob);activate();
	 }
	 
       }
       
       break;
     case KeyRelease:
       if(bpress) exitf=1;
       break;
     }
    }
    form_dial(3,0,0,0,0,x,y,w,h);
    form_dial(2,0,0,0,0,x,y,w,h);
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
      for(i=0;i<HDR_LENGTH/2;i++) {
        WSWAP((char *)((int)rsrc+2*i));
      }
      if(rsrc->rsh_rssize==len) {
        fix_trindex(); 
        fix_frstrindex(); 
	fix_objc();
	fix_tedinfo();
	fix_bitblk();
       	fclose(dptr);
        return(0);
      }
    } 
    fclose(dptr);
    free(rsrc);
    return(-1);
  } 
  else return(-1);
}




/* Fileselector-Routine. Dem GEM nachempfunden.
(c) markus Hoffmann   1998               */




void draw_ecursor(ETEXT *e){
     XSetForeground(display[usewindow],gc[usewindow],rot);
     XDrawLine(display[usewindow],pix[usewindow],gc[usewindow],
     e->x+chw*(e->cp-e->shift),e->y,e->x+chw*(e->cp-e->shift),e->y+chh+4); 

     XSetForeground(display[usewindow],gc[usewindow],schwarz);
} 

void draw_efeld(ETEXT *e){      
      int i;
      int len=strlen(e->p);
      
      if(e->cp-e->shift > e->maxlen) e->shift=e->cp-e->maxlen;
      if(e->cp-e->shift<0) e->shift=e->cp;
      for(i=0;i<e->maxlen;i++) {
        if(i<strlen(e->p)) e->t[i]=e->p[i+e->shift];
	else e->t[i]='_';
      } 
      e->t[e->maxlen]=0;

       XSetForeground(display[usewindow],gc[usewindow],weiss);
       XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],
       e->x-chw,e->y,e->w+2*chw,e->h); 
       XSetForeground(display[usewindow],gc[usewindow],rot);
       if(strlen(e->p)>e->maxlen+e->shift) 
         XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
         e->x+e->w,e->y+chh,">",1);
       
      if(e->shift) 
        XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
         e->x-chw,e->y+chh,"<",1);
      
       XSetForeground(display[usewindow],gc[usewindow],schwarz);
       XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
       e->x,e->y+chh,e->t,strlen(e->t));

}


#define FT_NORMAL    0
#define FT_DIR    0x10
#define FT_LINK   0x20


typedef struct fileinfo
  {
    char name[128];       /* The file name. */
    int typ;
  } FINFO;
#define ANZSHOW 13
#define FWW 34
void draw_filelist(FINFO *filenamen,int *filenamensel,int anzfiles,int showstart, RECT box) {

  int i,j;
 /* Directory */
 
  XSetForeground(display[usewindow],gc[usewindow],weiss);
  XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+2*chw,box.y+9*chh,chw*(FWW-2),chh*ANZSHOW);    
  XSetForeground(display[usewindow],gc[usewindow],schwarz);    
  XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+2*chw,box.y+9*chh,chw*(FWW-2),chh*ANZSHOW);   

    for(i=0;i<ANZSHOW;i++) {
      j=showstart+i;
      if(j<anzfiles) {
      
        if(filenamensel[j]==1)  XSetForeground(display[usewindow],gc[usewindow],schwarz);
        else XSetForeground(display[usewindow],gc[usewindow],weiss);

        XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],
	box.x+chw*2+1,box.y+(9+i)*chh+1,chw*(FWW-2)-1,chh-1);      
	
	
	if(filenamensel[j]==1) XSetForeground(display[usewindow],gc[usewindow],weiss);

        else XSetForeground(display[usewindow],gc[usewindow],schwarz);
        
	


        XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
          box.x+chw*5,box.y+(10+i)*chh-2,filenamen[j].name,min(strlen(filenamen[j].name),FWW-6));
	if(filenamen[j].typ & FT_DIR)   
	  XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],
          box.x+3*chw,box.y+(10+i)*chh-chh/2-2,5,6);   

          XSetForeground(display[usewindow],gc[usewindow],schwarz);


      } else XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
       box.x+chw*5,box.y+(10+i)*chh," ",1);
      
    }

}
int compare_dirs(FINFO *a,FINFO *b) {
  if(a->typ==FT_DIR && b->typ!=FT_DIR) return(-1);
  if(a->typ!=FT_DIR && b->typ==FT_DIR) return(1);
  return(strcmp(a->name,b->name));
}

void sort_dir(FINFO *fileinfos,int anz) {
  
  qsort(fileinfos,anz,sizeof(FINFO),compare_dirs);
}
#define MAXANZFILES 256
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
      if(stat(filename, &dstat)==0) {
        if(S_ISDIR(dstat.st_mode)) {
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
  } else printf("Directory %s konnte nicht geoeffnet werden.\n",pfad);
  return(anzfiles);
}
void draw_mask(char *mask, RECT box){

  XSetForeground(display[usewindow],gc[usewindow],grau);
  XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+4*chw,box.y+8*chh,chw*(FWW-4),chh);    
  XSetForeground(display[usewindow],gc[usewindow],schwarz);    
  XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+4*chw+4,box.y+8*chh+4,chw*(FWW-4)-8,chh-8);
     
  XSetForeground(display[usewindow],gc[usewindow],weiss);
  XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+4*chw+((FWW-4)*chw)/2-((strlen(mask)+1)*chw)/2,box.y+8*chh,chw*(strlen(mask)+1),chh);    
  XSetForeground(display[usewindow],gc[usewindow],schwarz);
  XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+4*chw,box.y+8*chh,chw*(FWW-4),chh);
 
  XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+4*chw+((FWW-4)*chw)/2-(strlen(mask)*chw)/2,box.y+9*chh-3,mask,strlen(mask));
}
void draw_scaler(int anzfiles,int showstart, RECT box){
  int hoehe,xpos;

  XSetForeground(display[usewindow],gc[usewindow],grau);
  XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+(FWW)*chw,box.y+10*chh,chw*2,chh*(ANZSHOW-2));
  XSetForeground(display[usewindow],gc[usewindow],weiss);
  if(anzfiles>ANZSHOW){
    hoehe=chh*(ANZSHOW-2)*min(1,(float)ANZSHOW/anzfiles);
    xpos=chh*(ANZSHOW-2)*(1-min(1,(float)(ANZSHOW)/anzfiles))*showstart/(anzfiles-ANZSHOW);
  } else {
    hoehe=chh*(ANZSHOW-2);
    xpos=0;
  }
  XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+(FWW)*chw,box.y+10*chh+xpos,chw*2,hoehe);    
  XSetForeground(display[usewindow],gc[usewindow],schwarz);
  XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+(FWW)*chw,box.y+10*chh+xpos,chw*2,hoehe);  
  XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+(FWW)*chw,box.y+10*chh,chw*2,chh*(ANZSHOW-2));
}




char *fsel_input(char *titel, char *pfad, char *sel) {
  char *btitel=malloc(128);
  char *mask=malloc(128);
  char *dpfad=malloc(128);
  RECT box;
  #define ANZBUTTONS 7
  #define ANZEFELDER 2
  char *ergebnis;
  int i=0,cy=1,j,k;
  
  BUTTON buttons[ANZBUTTONS]; 
  ETEXT efeld[ANZEFELDER];
  char *feld1=malloc(128);
  char *feld2=malloc(128);
  char *xfeld1=malloc(128);
  char *xfeld2=malloc(128);
  int sbut;
  int maxc=0;
  int exitf=0,bpress=0; 
  XEvent event;
  XGCValues gc_val;
  
  FINFO filenamen[MAXANZFILES];
  int filenamensel[MAXANZFILES];
  
  int anzfiles,showstart=0;

  OBJECT objects[]={{-1,1,1,G_BOX, 0, OUTLINED, 0x00011100, 0,0,100,100},
                    {-1,0,-1,G_BUTTON, SELECTABLE|DEFAULT|EXIT|LASTOB, NORMAL, 0x00011100, 0,0,100,100}};


  if(titel!=NULL) strncpy(btitel,titel,60);
  else strcpy(btitel,"FILESELCT");
 
  wort_sepr(pfad,'/',0,dpfad, mask);
  anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
  sort_dir(filenamen,anzfiles);
  graphics();
  gem_init();
  
  box.w=chw*54;box.h=chh*24;  
  box_center(&box);
  



    buttons[0].w=buttons[1].w=buttons[2].w=(chw+1)*9;
    buttons[0].h=buttons[1].h=buttons[2].h=chh+3;
    buttons[0].y=box.y+box.h-6*chh;
    buttons[1].y=box.y+box.h-4*chh;
    buttons[2].y=box.y+box.h-12*chh;
    buttons[3].y=box.y+8*chh;
    buttons[3].x=box.x+2*chw;
    buttons[3].w=2*chw;
    buttons[3].h=1*chh;
   
    buttons[0].x=buttons[1].x=buttons[2].x=box.x+box.w-12*chw;
    buttons[0].flags=(DEFAULT|SELECTABLE|EXIT);
    buttons[1].flags=(SELECTABLE|EXIT);
    buttons[2].flags=buttons[3].flags=(TOUCHEXIT);
    
    buttons[0].state=buttons[1].state=buttons[2].state=buttons[3].state=(NORMAL);
   
    buttons[0].string="OK";
    buttons[1].string="ABBRUCH";
    buttons[2].string="HOME";
    buttons[3].string="<";
    buttons[4].string="^";
    buttons[4].state=(NORMAL);
    buttons[4].flags=(TOUCHEXIT);
    buttons[4].x=box.x+chw*(FWW);
    buttons[4].y=box.y+chh*9;
    buttons[4].w=chw*2;
    buttons[4].h=chh;
    buttons[5].string="v";
    buttons[5].state=(NORMAL);
    buttons[5].flags=(TOUCHEXIT);
    buttons[5].x=box.x+chw*(FWW);
    buttons[5].y=box.y+chh*(8+ANZSHOW);
    buttons[5].w=chw*2;
    buttons[5].h=chh;
    buttons[6].string="?";
    buttons[6].state=(NORMAL);
    buttons[6].flags=(TOUCHEXIT);
    buttons[6].x=box.x+chw*(FWW);
    buttons[6].y=box.y+chh*(8);
    buttons[6].w=chw*2;
    buttons[6].h=chh;
    
    strcpy(xfeld1,pfad);
    strcpy(xfeld2,sel);

    efeld[0].t=feld1;
    efeld[1].t=feld2;
    efeld[0].p=xfeld1;
    efeld[1].p=xfeld2;
    efeld[0].maxlen=50;
    efeld[1].maxlen=20;
    efeld[0].cp=strlen(pfad);
    efeld[1].cp=strlen(sel);
    efeld[0].h=efeld[1].h=chh+3;
    efeld[0].w=chw*50;
    efeld[1].w=chw*20;
    efeld[0].x=box.x+chw*2;
    efeld[1].x=box.x+chw*32;
    efeld[0].y=box.y+chh*4-4;
    efeld[1].y=box.y+chh*6;
    efeld[0].shift=efeld[1].shift=0;

    form_dial(0,0,0,0,0,box.x,box.y,box.w,box.h);
    form_dial(1,0,0,0,0,box.x,box.y,box.w,box.h);



    /* Box zeichenen  */
    XSetForeground(display[usewindow],gc[usewindow],weiss);
    XSetLineAttributes(display[usewindow], gc[usewindow], 1, 0,0,0);
    XFillRectangle(display[usewindow],pix[usewindow],gc[usewindow],box.x,box.y,box.w,box.h); 
    XSetForeground(display[usewindow],gc[usewindow],schwarz);

    XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],box.x,box.y,box.w-1,box.h-1); 
    XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],box.x+3,box.y+3,box.w-7,box.h-7); 
    XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],box.x+2,box.y+2,box.w-5,box.h-5); 
  
    
 
     /* Text  */

    XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
       box.x+box.w/2-(strlen(btitel)*chw)/2,box.y+2*chh,btitel,strlen(btitel));
    XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
       box.x+chw*2,box.y+3*chh+4,"Directory:",strlen("Directory:"));
    XDrawString(display[usewindow],pix[usewindow],gc[usewindow],
       box.x+chw*32,box.y+6*chh,"Auswahl:",strlen("Auswahl:"));

     /* Efelder */

     for(i=0;i<ANZEFELDER; i++) draw_efeld(&efeld[i]);
     draw_ecursor(&efeld[cy]);
     
    /* Buttons  */

    for(i=0;i<ANZBUTTONS; i++)  draw_button(&buttons[i]);

    XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+2*chw-1,box.y+8*chh-1,chw*FWW+2,chh*(ANZSHOW+1)+2);   
    XDrawRectangle(display[usewindow],pix[usewindow],gc[usewindow],
    box.x+2*chw,box.y+8*chh,chw*2,chh*1);   

    /* Directory */
    draw_filelist(filenamen,filenamensel,anzfiles,showstart,box);
    draw_mask(mask, box);
    draw_scaler(anzfiles,showstart,box);

    /* Auf Tasten/Maus reagieren */
    activate();
    while(exitf==0) {
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
    
    /* Bei Mouse-Taste: */
      case ButtonPress: 
     
        if(event.xbutton.button==1) {
          for(i=0;i<ANZBUTTONS;i++) {
	    if(event.xbutton.x>buttons[i].x && event.xbutton.x<buttons[i].x+buttons[i].w
	     && event.xbutton.y>buttons[i].y && event.xbutton.y<buttons[i].y+buttons[i].h)
            { if(buttons[i].flags & SELECTABLE) buttons[i].state^=SELECTED;
	      draw_button(&buttons[i]);
	      if(buttons[i].flags & EXIT) {sbut=i+1;bpress=1;}
	      if(buttons[i].flags & TOUCHEXIT){
	      #ifdef DEBUG
	        printf("BUTTON--->%d\n",i);
	      #endif
	        if(i==2) {
		   char buf[128];
		   if(getcwd(buf,128)!=NULL) {
		     strcpy(dpfad,buf);
		     sprintf(xfeld1,"%s/%s",dpfad,mask);
		     efeld[0].p=xfeld1;
		     efeld[0].cp=strlen(xfeld1);efeld[0].shift=0;
		     draw_efeld(&efeld[0]);
	             if(cy==0) draw_ecursor(&efeld[cy]);
		     anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
		     for(k=0;k<anzfiles;k++) filenamensel[k]=0;
                     sort_dir(filenamen,anzfiles);
                     draw_mask(mask,box);
	             showstart=max(0,min(showstart,anzfiles-ANZSHOW));
	             draw_filelist(filenamen,filenamensel,anzfiles,showstart,box);
	             draw_scaler(anzfiles,showstart,box);
		     activate();
		   }
		} else if(i==3) {
		   char buf[128];
		   wort_sepr(dpfad,'/',0,dpfad, buf);
		   sprintf(xfeld1,"%s/%s",dpfad,mask);
		   efeld[0].p=xfeld1;
		   efeld[0].cp=strlen(xfeld1);efeld[0].shift=0;
		   draw_efeld(&efeld[0]);
	           if(cy==0) draw_ecursor(&efeld[cy]);
		   anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
                   sort_dir(filenamen,anzfiles);
		   for(k=0;k<anzfiles;k++) filenamensel[k]=0;
                   draw_mask(mask,box);
	           showstart=max(0,min(showstart,anzfiles-ANZSHOW));
	           draw_filelist(filenamen,filenamensel,anzfiles,showstart,box);
	           draw_scaler(anzfiles,showstart,box);
		   activate();
		   
		} else if(i==6) {
		  char buf[128];
		  sprintf(buf,"[1][%d Files.][ OK ]",anzfiles);
		  form_alert(1,buf);
		} else if(i==5) {
		  if(showstart<anzfiles-ANZSHOW) {
	            showstart++;
	            draw_filelist(filenamen,filenamensel,anzfiles,showstart,box);
	            draw_scaler(anzfiles,showstart,box);
	            activate();
	          }
		} else if(i==4) {
		  if(showstart) {
	            showstart--;
	            draw_filelist(filenamen,filenamensel,anzfiles,showstart,box);
	            draw_scaler(anzfiles,showstart,box);
	            activate();
	          }
		}
	      }
	      activate();
	    } 
	  }
	  for(i=0;i<ANZEFELDER;i++) {
	    if(event.xbutton.x>efeld[i].x &&
	    event.xbutton.x<efeld[i].x+efeld[i].w
	     && event.xbutton.y>efeld[i].y && event.xbutton.y<efeld[i].y+efeld[i].h)
            { cy=i; 
	      for(j=0;j<ANZEFELDER; j++) draw_efeld(&efeld[j]);               
	      draw_ecursor(&efeld[cy]);
	      activate();
	    } 
	  }
	  if(event.xbutton.x>box.x+4*chw &&
	    event.xbutton.x<box.x+FWW*chw
	     && event.xbutton.y>box.y+8*chh && event.xbutton.y<box.y+9*chh)
            {
	        wort_sepr(efeld[0].p,'/',0,dpfad, mask);
                anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
                sort_dir(filenamen,anzfiles);
                draw_mask(mask,box);
	       showstart=max(0,min(showstart,anzfiles-ANZSHOW));
	       draw_filelist(filenamen, filenamensel,anzfiles,showstart,box);
	       draw_scaler(anzfiles,showstart, box);
	       activate();
	  } else if(event.xbutton.x>box.x+FWW*chw &&
	    event.xbutton.x<box.x+(FWW+2)*chw
	     && event.xbutton.y>box.y+10*chh && event.xbutton.y<box.y+(8+ANZSHOW)*chh)
            {
	      int hoehe,xpos,ypos=event.xbutton.y-(box.y+10*chh);  
	       if(anzfiles>ANZSHOW){
                 hoehe=chh*(ANZSHOW-2)*min(1,(float)ANZSHOW/anzfiles);
                 xpos=chh*(ANZSHOW-2)*(1-min(1,(float)(ANZSHOW)/anzfiles))*showstart/(anzfiles-ANZSHOW);
               } else {
                 hoehe=chh*(ANZSHOW-2);
                 xpos=0;
               }
	       if(ypos<xpos) {
	         if(showstart) {
	           showstart=max(0,min(showstart-ANZSHOW,anzfiles-ANZSHOW));
	         }  
	       } else if(ypos>xpos+hoehe) {
	         if(showstart<anzfiles-ANZSHOW) {
	           showstart=max(0,min(showstart+ANZSHOW,anzfiles-ANZSHOW));
	         }
	       } else {
	         int ex=0;
		 int ssold=showstart;
		 XEvent mevent;
	         
		 while(ex==0) {
                   XWindowEvent(display[usewindow], win[usewindow],
		   ButtonReleaseMask| PointerMotionMask|ExposureMask , &mevent);
                   switch (mevent.type) {
		   case Expose:
                     XCopyArea(display[usewindow],pix[usewindow],win[usewindow],gc[usewindow],
                       mevent.xexpose.x,mevent.xexpose.y,
                       mevent.xexpose.width,mevent.xexpose.height,
                       mevent.xexpose.x,mevent.xexpose.y);
                     break;
		   case ButtonRelease:
                     ex=1;
                     break;
		   case MotionNotify:
		     #ifdef DEBUG
		     printf("Motion %d\n",mevent.xmotion.y-event.xbutton.y);
		     #endif
		     showstart=ssold+(mevent.xmotion.y-event.xbutton.y)/
		     (chh*(ANZSHOW-2)*(1-min(1,(float)(ANZSHOW)/anzfiles)))
		     *(anzfiles-ANZSHOW);
		     showstart=max(0,min(showstart,anzfiles-ANZSHOW));
	             draw_filelist(filenamen,filenamensel,anzfiles,showstart,box);
	             draw_scaler(anzfiles,showstart,box);
	             activate();
		     break; 
		   }
		 }
	       }
	       showstart=max(0,min(showstart,anzfiles-ANZSHOW));
	       draw_filelist(filenamen,filenamensel,anzfiles,showstart,box);
	       draw_scaler(anzfiles,showstart, box);
	       activate();
	  } else if(event.xbutton.x>box.x+2*chw && event.xbutton.x<box.x+FWW*chw
	     && event.xbutton.y>box.y+9*chh && event.xbutton.y<box.y+(9+ANZSHOW)*chh)
            {
	       int j=(event.xbutton.y-(box.y+9*chh))/chh;
	       char buf[128];
	       
	       if(showstart+j<anzfiles) {
	         #ifdef DEBUG
	         printf("--->%s\n",filenamen[showstart+j].name);
		 #endif
		 if(filenamen[showstart+j].typ & FT_DIR) {
		   sprintf(buf,"%s/%s",dpfad,filenamen[showstart+j].name);
		   strcpy(dpfad,buf);
		   sprintf(xfeld1,"%s/%s",dpfad,mask);
		   efeld[0].p=xfeld1;
		   efeld[0].cp=strlen(xfeld1);efeld[0].shift=0;
		   draw_efeld(&efeld[0]);
	           if(cy==0) draw_ecursor(&efeld[cy]);
		   anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
                   sort_dir(filenamen,anzfiles);
		   for(k=0;k<anzfiles;k++) filenamensel[k]=0;
                   draw_mask(mask,box);
	           showstart=max(0,min(showstart,anzfiles-ANZSHOW));
	           draw_filelist(filenamen,filenamensel,anzfiles,showstart,box);
	           draw_scaler(anzfiles,showstart, box);
		 } else {
		   for(k=0;k<anzfiles;k++) filenamensel[k]=0;
		   filenamensel[showstart+j]=1;
		   draw_filelist(filenamen,filenamensel,anzfiles,showstart,box);
		   strcpy(efeld[1].p,filenamen[showstart+j].name);
		   efeld[1].cp=strlen(efeld[1].p);efeld[1].shift=0;
		   draw_efeld(&efeld[1]);
	           if(cy==1) draw_ecursor(&efeld[cy]);
		   
		 }
	         activate();
	       }       
	  }
        }
        break;
      case ButtonRelease:
        if(bpress) exitf=1;
        break;
      case KeyPress:   /* Return gedrueckt ? */
     
       XLookupString((XKeyEvent *)&event,buf,sizeof(buf),&ks,&status);   
       if((ks & 255)==13) {                /* RETURN  */
         for(i=0;i<ANZBUTTONS;i++) {
	   if(buttons[i].flags & DEFAULT){
	     if(buttons[i].flags & SELECTABLE) buttons[i].state^=SELECTED;
	     draw_button(&buttons[i]);
	     if(buttons[i].flags & EXIT) {sbut=i+1;bpress=1;}
	     activate();
	   }
	 }
       } else {
         
         if((ks & 0xff00)) {
	   if(ks==0xff08) {                  /* BACKSPACE   */
	     if(efeld[cy].cp>0) {
	       int len=strlen(efeld[cy].p);
	       i=efeld[cy].cp--;
	       while(i<len) efeld[cy].p[i-1]=efeld[cy].p[i++];
	       efeld[cy].p[i-1]=0;
	       draw_efeld(&efeld[cy]);
	       draw_ecursor(&efeld[cy]);
	       activate();
	     }
	   } else if(ks==0xff51) {
	     if(efeld[cy].cp>0) efeld[cy].cp--;
	     draw_efeld(&efeld[cy]);
	     draw_ecursor(&efeld[cy]);
	     activate();
	   } else if(ks==0xff53) {
	     int len=strlen(efeld[cy].p);
	     if(efeld[cy].cp<len && efeld[cy].p[efeld[cy].cp]) efeld[cy].cp++;

	     draw_efeld(&efeld[cy]);
	     draw_ecursor(&efeld[cy]);
	     activate();
	   } else if(ks==0xff97) {           /* NUM UP */
	     if(cy>0) cy--;
	     for(j=0;j<ANZEFELDER; j++) draw_efeld(&efeld[j]);
	     draw_ecursor(&efeld[cy]);
	     activate();
	   } else if(ks==0xff09) {          /* TAB */
	     if(cy<ANZEFELDER-1) cy++;
	     else cy=0;
	     for(j=0;j<ANZEFELDER; j++) draw_efeld(&efeld[j]);
	     draw_ecursor(&efeld[cy]);
	     activate();
	   } else if(ks==0xff99) {          /* NUm DOWN */
	     if(cy<ANZEFELDER-1) cy++;
	     for(j=0;j<ANZEFELDER; j++) draw_efeld(&efeld[j]);
	     draw_ecursor(&efeld[cy]);
	     activate();
	   } else if(ks==0xff54) {
	     if(showstart<anzfiles-ANZSHOW) {
	       showstart++;
	       draw_filelist(filenamen,filenamensel,anzfiles,showstart, box);
	       draw_scaler(anzfiles,showstart, box);
	       activate();
	     }
	   } else if(ks==0xff52) {
	     if(showstart) {
	       showstart--;
	       draw_filelist(filenamen,filenamensel,anzfiles,showstart, box);
	       draw_scaler(anzfiles,showstart, box);
	       activate();
	     }
	   } else if(ks==0xff56) {
	     if(showstart<anzfiles-ANZSHOW) {
	       showstart=max(0,min(showstart+ANZSHOW,anzfiles-ANZSHOW));
	       draw_filelist(filenamen,filenamensel,anzfiles,showstart,box);
	       draw_scaler(anzfiles,showstart,box);
	       activate();
	     }
	   } else if(ks==0xff55) {
	     if(showstart) {
	       showstart=max(0,min(showstart-ANZSHOW,anzfiles-ANZSHOW));
	       draw_filelist(filenamen,filenamensel,anzfiles,showstart, box);
	       draw_scaler(anzfiles,showstart, box);
	       activate();
	     }  
	   } else if(ks==0xff1b) {                 /* ESC  */
	     
	        wort_sepr(efeld[0].p,'/',0,dpfad, mask);
                anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
                sort_dir(filenamen,anzfiles);
                draw_mask(mask, box);
	       showstart=max(0,min(showstart,anzfiles-ANZSHOW));
	       draw_filelist(filenamen,filenamensel,anzfiles,showstart,box);
	       draw_scaler(anzfiles,showstart, box);
	       activate();
	     
	   } else printf("Key: %x\n",ks);
	 } else {
	   i=128;
	   while(i>efeld[cy].cp) {efeld[cy].p[i]=efeld[cy].p[i-1];i--;}
	   efeld[cy].p[efeld[cy].cp]=(char)ks;
	   if(efeld[cy].cp<127) efeld[cy].cp++;
	   draw_efeld(&efeld[cy]);	     
	   draw_ecursor(&efeld[cy]);
	   activate();
	 }
	 
       }
       break;
     case KeyRelease:
       if(bpress) exitf=1;
       break;
     }
    }

    form_dial(3,0,0,0,0,box.x,box.y,box.w,box.h);
    form_dial(2,0,0,0,0,box.x,box.y,box.w,box.h);

    ergebnis=malloc(strlen(dpfad)+strlen(efeld[1].p)+2);
    if(sbut==1) sprintf(ergebnis,"%s/%s",dpfad,efeld[1].p);
    else ergebnis[0]=0;
    free(btitel);free(mask);free(dpfad);free(xfeld1);free(xfeld2);free(feld1);free(feld2);
    return(ergebnis);
}
