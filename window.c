
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

#define max(a,b) ((a>b)?a:b)
#define min(a,b) ((a<b)?a:b)

/* object types */
#define G_BOX      20
#define G_TEXT     21
#define G_BOXTEXT  22
#define G_IMAGE    23
#define G_USERDEF  24
#define G_PROGDEF  G_USERDEF
#define G_IBOX     25
#define G_BUTTON   26
#define G_BOXCHAR  27
#define G_STRING   28
#define G_FTEXT    29
#define G_FBOXTEXT 30
#define G_ICON     31
#define G_TITLE    32
#define G_CICON    33
/* object flags */
#define NONE       0x0000
#define SELECTABLE 0x0001
#define DEFAULT    0x0002
#define EXIT       0x0004
#define EDITABLE   0x0008
#define RBUTTON    0x0010
#define LASTOB     0x0020
#define TOUCHEXIT  0x0040
#define HIDETREE   0x0080
#define INDIRECT   0x0100
#define FL3DIND    0x0200
#define FL3DBAK    0x0400
#define FL3DACT    0x0600
#define SUBMENU    0x0800
/* Object states */
#define NORMAL     0x0000
#define SELECTED   0x0001
#define CROSSED    0x0002
#define CHECKED    0x0004
#define DISABLED   0x0008
#define OUTLINED   0x0010
#define SHADOWED   0x0020
#define WHITEBAK   0x0080
#define FLD3DANY   0x0600


typedef struct {int x,y,w,h;char *t,*p;int shift,cp,maxlen;} ETEXT;
typedef struct {int x,y,w,h,flags,state; char *string;} BUTTON;

/* GEM-Globals   */

int chw=8,chh=16,baseline=14,depth=8,border=4;
RECT sbox;

int weiss,schwarz,rot,grau;

void gem_init() {
    Window root;
    XGCValues gc_val;
    XFontStruct *fs;

    /* Screendimensionen bestimmem */
    XGetGeometry(display[usewindow],win[usewindow],&root,&sbox.x,&sbox.y,&sbox.w,&sbox.h,&border,&depth); 

   fs=XLoadQueryFont(display[usewindow], GEMFONT);
   if(fs!=NULL)  {
     gc_val.font=fs->fid;
     XChangeGC(display[usewindow], gc[usewindow],  GCFont, &gc_val);
     chw=fs->max_bounds.width,chh=fs->max_bounds.ascent+fs->max_bounds.descent;
     baseline=fs->max_bounds.ascent;
   }
   weiss=WhitePixel(display[usewindow], DefaultScreen(display[usewindow]));
   schwarz=BlackPixel(display[usewindow], DefaultScreen(display[usewindow]));
   grau=get_color(50000,50000,50000);
   rot=get_color(65535,0,0);
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
    graphics();

    /* Erst den Graphic-Kontext retten  */
    sgc=XCreateGC(display[usewindow], win[usewindow], 0, &gc_val);
    XCopyGC(display[usewindow], gc[usewindow],GCForeground|
                              GCFunction |GCLineWidth |GCLineStyle|
			      GCFont, sgc);
    
    gem_init();
   
    if(symbol) {box.w=box.h=textx=64;}
    else {box.w=box.h=textx=0;}
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

    set_graphmode(GXcopy);
    
    /* Hintergrund retten  */
    spix=XCreatePixmap(display[usewindow],win[usewindow],box.w,box.h,depth);
    XCopyArea(display[usewindow], pix[usewindow],spix,gc[usewindow],box.x,box.y,box.w,box.h,0,0);


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
    
    /* Hintergrund restaurieren  */
    XCopyArea(display[usewindow], spix,pix[usewindow],gc[usewindow],0,0,box.w,box.h,box.x,box.y);
    XFreePixmap(display[usewindow],spix);
    XCopyGC(display[usewindow], sgc,GCForeground| GCFunction |GCLineWidth |GCLineStyle| GCFont, gc[usewindow]);
    XFreeGC(display[usewindow],sgc); 
    activate();
    return(sbut);
  } else return(0);
}

/* -------------------------------- AES-Implementationen ------------------*/




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
  Pixmap spix; 
  XEvent event;
  GC sgc;
  XGCValues gc_val;
  
  FINFO filenamen[MAXANZFILES];
  int filenamensel[MAXANZFILES];
  
  int anzfiles,showstart=0;


  if(titel!=NULL) strncpy(btitel,titel,60);
  else strcpy(btitel,"FILESELCT");
 
  wort_sepr(pfad,'/',0,dpfad, mask);
  anzfiles=read_dir(filenamen,MAXANZFILES,dpfad,mask);
  sort_dir(filenamen,anzfiles);
  graphics();
    /* Erst den Graphic-Kontext retten  */
  sgc=XCreateGC(display[usewindow], win[usewindow], 0, &gc_val);
  XCopyGC(display[usewindow], gc[usewindow],GCForeground|
                              GCFunction |GCLineWidth |GCLineStyle|GCFont, sgc);

  
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
    
    set_graphmode(GXcopy);
    
    /* Hintergrund retten  */
    spix=XCreatePixmap(display[usewindow],win[usewindow],box.w,box.h,depth);
    XCopyArea(display[usewindow], pix[usewindow],spix,gc[usewindow],box.x,box.y,box.w,box.h,0,0);


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

    /* Hintergrund restaurieren  */
    XCopyArea(display[usewindow], spix,pix[usewindow],gc[usewindow],0,0,box.w,box.h,box.x,box.y);
    
    XFreePixmap(display[usewindow],spix);
    XCopyGC(display[usewindow], sgc,GCForeground|
                              GCFunction |GCLineWidth |GCLineStyle|
			      GCFont, gc[usewindow]);
    XFreeGC(display[usewindow],sgc); 
    activate();
    ergebnis=malloc(strlen(dpfad)+strlen(efeld[1].p)+2);
    if(sbut==1) sprintf(ergebnis,"%s/%s",dpfad,efeld[1].p);
    else ergebnis[0]=0;
    free(btitel);free(mask);free(dpfad);free(xfeld1);free(xfeld2);free(feld1);free(feld2);
    return(ergebnis);
}
