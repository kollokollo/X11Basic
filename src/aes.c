
/* aes.c      User Interface (c) Markus Hoffmann    */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "defs.h"
#ifdef USE_GEM
#include <osbind.h>
#include <gem.h>
#endif
#include "x11basic.h"
#include "xbasic.h"
#include "graphics.h"
#include "aes.h"
#include "file.h"
#include "window.h"

#ifdef FRAMEBUFFER
#include "framebuffer.h"
#include "raw_mouse.h"
#else
#define CharWidth1632 16
#define CharWidth816 8
#define CharWidth57 5
#define CharHeight1632 32
#define CharHeight816 16
#define CharHeight57 (7+1)
  #define CharWidth    5
  #define CharHeight    (7+1)

#endif

#ifndef NOGRAPHICS
RSHDR *rsrc=NULL;    /* Adresse der Recource im Speicher */

int gem_colors[16];
static const AESRGBCOLOR gem_colordefs[]={
{255,255,255},  /* WHITE */
{0,0,0},/*BLACK  */
{255,0,0},/* RED */
{0,255,0},/* GREEN */
{0,0,255},/* BLUE */
{0,255,255},/* CYAN */
{255,255,0},/* YELLOW */
{255,0,255},/* MAGENTA */
{156,156,156},/* LWHITE */
{78,78,78},/* LBLACK */
{255,127,127},/* LRED */
{127,255,127},/* LGREEN */
{127,127,255},/* LBLUE */
{127,255,255},/* LCYAN  */
{255,255,127},/* LYELLOW */
{255,127,255} /* LMAGENTA */
};


/* AES-Nachbildungen (c) Markus Hoffmann     */

#ifdef FRAMEBUFFER
extern int font_behaviour;
#endif


/*GEM_INIT muss jedesmal aufgr�ufen werden, sobald sich die Bildschirmgroesse
  veraendert.   Screengeometrie, Font und Farbpalette */

void gem_init() {
  static int gem_isinit=0;

/* Passenden Zeichensatz laden.*/
/* Passende Zeichengroesse ermitteln.*/

#ifdef FRAMEBUFFER
  /*Das Ziel ist hier, dass ungefaehrr 60 bis 80 Zeichen in eine Zeile passen*/
  if(font_behaviour==0) { /* Auto */
    if(window[usewindow].w>=1200) load_GEMFONT(FONT_LARGE);
    else if(window[usewindow].w>=640) load_GEMFONT(FONT_BIG);
    else load_GEMFONT(FONT_SMALL);
  } else if(font_behaviour==1) {
    load_GEMFONT(FONT_SMALL);
  } else if(font_behaviour==2) {
    if(window[usewindow].w>=320) load_GEMFONT(FONT_BIG);
    else load_GEMFONT(FONT_SMALL);
  } else if(font_behaviour==3) {
    if(window[usewindow].w>=640) load_GEMFONT(FONT_LARGE);
    else if(window[usewindow].w>=320) load_GEMFONT(FONT_BIG);
    else load_GEMFONT(FONT_SMALL);
  } else load_GEMFONT(FONT_DEFAULT);
#else
  load_GEMFONT(FONT_DEFAULT);
#endif
  #ifdef DEBUG
    printf("chw=%d, chh=%d\n",chw,chh);
  #endif

  /* Farbpalette initialisieren.*/

  if(!gem_isinit) {
    int i;
    /* Farbpalette initialisieren */
    for(i=0;i<16;i++) {
      gem_colors[i]=get_color(gem_colordefs[i].r,gem_colordefs[i].g,gem_colordefs[i].b,255);
      #ifdef DEBUG
        printf("color #%d: r=%d,g=%d,b=%d --> %08x\n",i,gem_colordefs[i].r,gem_colordefs[i].g,gem_colordefs[i].b,
        (unsigned int)gem_colors[i]);
      #endif
    }
  }
  gem_isinit=1;
}

#ifdef USE_SDL
#include "spat-a-fnt.c"
#include "5x7-rev.c"
#endif


static int f1=FONT_SMALL;
static int f2=FONT_BIG;
static int f3=FONT_BIG;


static inline int fontheight(int n) {
  if(n==FONT_IBM) n=f2;
  else if(n==FONT_SMALLIBM) n=f1;
  else if(n==FONT_BIGIBM) n=f3;
  if(n==FONT_LARGE) {
    return(CharHeight1632);
  } else if(n==FONT_BIG) {
    return(CharHeight816);
  } else if(n==FONT_SMALL) {
    return(CharHeight57);
  } else {
    return(CharHeight);
  }
}
static inline int fontwidth(int n) {
  if(n==FONT_IBM) n=f2;
  else if(n==FONT_SMALLIBM) n=f1;
  else if(n==FONT_BIGIBM) n=f3;
  if(n==FONT_LARGE) {
    return(CharWidth1632);
  } else if(n==FONT_BIG) {
    return(CharWidth816);
  } else if(n==FONT_SMALL) {
    return(CharWidth57);
  } else {
    return(CharWidth);
  }
}
static inline int fontbaseline(int n) {
  if(n==FONT_IBM) n=f2;
  else if(n==FONT_SMALLIBM) n=f1;
  else if(n==FONT_BIGIBM) n=f3;
  if(n==FONT_LARGE) {
    return(CharHeight1632-4);
  } else if(n==FONT_BIG) {
    return(CharHeight816);
  } else if(n==FONT_SMALL) {
    return(CharHeight57);
  } else {
    return(CharHeight);
  }
}


void load_GEMFONT(int n) {
#ifdef WINDOWS_NATIVE
  SIZE siz;
  GetTextExtentPoint(bitcon[usewindow],"Sg", 2,&siz);
  window[usewindow].chw=siz.cx/2;
  window[usewindow].chh=siz.cy;
  window[usewindow].baseline=window[usewindow].chh-2;
#elif defined FRAMEBUFFER
  if(n==FONT_IBM) n=f2;
  else if(n==FONT_SMALLIBM) n=f1;
  else if(n==FONT_BIGIBM) n=f3;
  else if(n==FONT_BIG) {
    f2=n;
    f1=FONT_SMALL;
    f3=FONT_LARGE;
  } else if(n==FONT_LARGE) {
    f3=f2=n;
    f1=FONT_BIG;
  } else if(n==FONT_SMALL) {
    f2=f1=n;
    f3=FONT_BIG;
  }
  if(n==FONT_LARGE) {
    window[usewindow].chw=CharWidth1632;
    window[usewindow].chh=CharHeight1632;
    window[usewindow].baseline=window[usewindow].chh-4;
  } else if(n==FONT_BIG) {
    window[usewindow].chw=CharWidth816;
    window[usewindow].chh=CharHeight816;
    window[usewindow].baseline=window[usewindow].chh-2; 
  } else if(n==FONT_SMALL) {
    window[usewindow].chw=CharWidth57;
    window[usewindow].chh=CharHeight57;
    window[usewindow].baseline=window[usewindow].chh-0;
  } else {
    window[usewindow].chw=CharWidth;
    window[usewindow].chh=CharHeight;
    window[usewindow].baseline=window[usewindow].chh-0;
#ifdef ANDROID
    char t[100];
    sprintf(t,"unbek font #%d",n);
    backlog(t);
#endif
  }
#elif defined USE_X11
  XGCValues gc_val;
  XFontStruct *fs;
//  printf("load Gemfont %d\n",n);
  if(n==FONT_SMALL || n==FONT_SMALLIBM) fs=XLoadQueryFont(window[usewindow].display, GEMFONTSMALL);
  else fs=XLoadQueryFont(window[usewindow].display, GEMFONT);
  if(fs!=NULL)  {
     gc_val.font=fs->fid;
     XChangeGC(window[usewindow].display, window[usewindow].gc,  GCFont, &gc_val);
     window[usewindow].chw=fs->max_bounds.width;
     window[usewindow].chh=fs->max_bounds.ascent+fs->max_bounds.descent;
     window[usewindow].baseline=fs->max_bounds.ascent;
     //printf("font %dx%d-%d\n",chw,chh,baseline);
  }
#elif defined USE_SDL
  if(n==FONT_BIG || n==FONT_IBM || n==FONT_LARGE|| n==FONT_BIGIBM) {
    window[usewindow].chw=8;
    window[usewindow].chh=16;
    window[usewindow].baseline=window[usewindow].chh-2; 
    gfxPrimitivesSetFont(spat_a816,window[usewindow].chw,window[usewindow].chh); 	
  } else {
    window[usewindow].chw=5;
    window[usewindow].chh=7;
    window[usewindow].baseline=window[usewindow].chh-0;
    gfxPrimitivesSetFont(asciiTable,window[usewindow].chw,window[usewindow].chh); 	
  }
#endif
}
/* -------------------------------- AES-Implementationen ------------------*/


/* Form-Alert-Routine. Dem GEM nachempfunden.
(c) markus Hoffmann   1998               */

double ltext(int, int, double, double, double , int, char *);

#ifndef USE_GEM
short form_alert(short dbut,char *n) {return(form_alert2(dbut,n,strlen(n),NULL));}
#endif

/* Diese Funktion kann ja auch Input-Felder annehmen. Es können
   0-bytes in n sein.*/

int form_alert2(int dbut,char *n,int size, char *tval) {
  char *bzeilen[30],*bbuttons[30],*buffer[30];
  int bzeilenlen[30],bbuttonslen[30];
  int anzzeilen=0,anzbuttons=0,anztedinfo=0,anzbuffer=0;
  int symbol=0,sbut=0;
  char *pos=n;
  char **ein=bzeilen;
  int *len=bzeilenlen;
  int i=0,j=size,k=0,l=0;
  TEDINFO tedinfo[32];
  char tmplt[32*80],valid[32*80];
  int tmpltptr=0,validptr=0;
  OBJECT objects[64]={{-1,1,1,G_BOX, 0, OUTLINED, { 0x00021100}, 0,0,100,100}};
  int objccount=1;
  short x,y,w,h;
#ifdef DEBUG
  printf("**form_alert:\n");
#endif
  memset(tmplt,'_',sizeof(tmplt));
  memset(valid,'X',sizeof(valid));
  while(i<j) {
    if(n[i]=='[') {
      pos=&n[i+1];
      k++;l=0;
    } else if(n[i]==']') {
      n[i]=0;
      if(k>0) {
        ein[l]=pos;
	len[l]=&n[i]-pos;
	l++;
      }
      if(k==1) {
        symbol=atoi(pos);
	ein=bzeilen;
	len=bzeilenlen;
      } else if(k==2) {
        ein=bbuttons;anzzeilen=l;
	len=bbuttonslen;
      } else if(k==3) anzbuttons=l;
    } else if(n[i]=='|') {
      ein[l]=pos;
      len[l]=&n[i]-pos;
      n[i]=0;
      l++;
      pos=n+i+1;
    };
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
    objects[0].ob_width+=window[usewindow].chh*2;
    objects[0].ob_height=max(objects[0].ob_height+2*max(window[usewindow].chh,16),max(window[usewindow].chh,16)*2+(anzzeilen+2)*window[usewindow].chh);


    for(i=0;i<anzzeilen;i++) maxc=max(maxc,bzeilenlen[i]); /*Auch textfelder mit längeren default berücksichtigen*/
    objects[0].ob_width+=window[usewindow].chw*maxc;

     /* Buttons  */
    maxc=0;
    for(i=0;i<anzbuttons;i++) maxc=max(maxc,strlen(bbuttons[i]));
    objects[0].ob_width=max(objects[0].ob_width,window[usewindow].chw*(4+(maxc+1)*anzbuttons+2*(anzbuttons-1)));

    for(i=0;i<anzbuttons; i++) {
      objects[objccount].ob_x=objects[0].ob_width/2-window[usewindow].chw*((maxc+1)*anzbuttons+2*(anzbuttons-1))/2+i*window[usewindow].chw*(maxc+3);
      objects[objccount].ob_y=objects[0].ob_height-2*max(window[usewindow].chh,16);
      objects[objccount].ob_width=(window[usewindow].chw+1)*maxc;
#ifdef FRAMEBUFFER
      objects[objccount].ob_height=max(window[usewindow].chh,16)+3;
#else
      objects[objccount].ob_height=window[usewindow].chh+3;
#endif
      objects[objccount].ob_spec.free_string=bbuttons[i];
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
      objects[objccount].ob_x=textx+window[usewindow].chh;
      objects[objccount].ob_y=(1+i)*window[usewindow].chh;
      objects[objccount].ob_width=window[usewindow].chw*bzeilenlen[i];
      objects[objccount].ob_height=window[usewindow].chh;
      objects[objccount].ob_spec.free_string=bzeilen[i];
      objects[objccount].ob_head=-1;
      objects[objccount].ob_tail=-1;
      objects[objccount].ob_next=objccount+1;
      objects[objccount].ob_type=G_STRING;
      objects[objccount].ob_flags=NONE;
      objects[objccount].ob_state=NORMAL;
      objccount++;
      /* Input-Felder finden */
      if(strlen(objects[objccount-1].ob_spec.free_string)) {
        for(j=0;j<strlen(objects[objccount-1].ob_spec.free_string);j++) {
	  if((objects[objccount-1].ob_spec.free_string)[j]==27) {
            (objects[objccount-1].ob_spec.free_string)[j]=0;
	     // printf("Textfeld gefunden. OB=%d vorne=<%s>\n",objccount,objects[objccount-1].ob_spec);
	    
	    objects[objccount].ob_x=textx+window[usewindow].chh+window[usewindow].chw*j+window[usewindow].chw;
            objects[objccount].ob_y=(1+i)*window[usewindow].chh;
            objects[objccount-1].ob_width=window[usewindow].chw*(strlen(objects[objccount-1].ob_spec.free_string));
            objects[objccount].ob_width=window[usewindow].chw*(bzeilenlen[i]-j-1);
	    // printf("default=<%s> len=%d\n",bzeilen[i]+j+1,bzeilenlen[i]-j-1);
            objects[objccount].ob_height=window[usewindow].chh;
            objects[objccount].ob_spec.tedinfo=&tedinfo[anztedinfo];
            objects[objccount].ob_head=-1;
            objects[objccount].ob_tail=-1;
            objects[objccount].ob_next=objccount+1;
            objects[objccount].ob_type=G_FTEXT;
            objects[objccount].ob_flags=EDITABLE;
            objects[objccount].ob_state=NORMAL;
	    tedinfo[anztedinfo].te_ptext=(char *)(bzeilen[i]+j+1);
	    buffer[anzbuffer]=malloc(bzeilenlen[i]-j-1+1);
	    tedinfo[anztedinfo].te_ptmplt=(char *)&tmplt[tmpltptr];
	    tmpltptr+=bzeilenlen[i]-j-1+1;
	    tmplt[tmpltptr-1]=0;
	    tedinfo[anztedinfo].te_pvalid=(char *)&valid[validptr];
	    validptr+=bzeilenlen[i]-j-1+1;
	    valid[validptr-1]=0;
	    tedinfo[anztedinfo].te_font=FONT_IBM;
	    tedinfo[anztedinfo].te_just=TE_LEFT;
	    tedinfo[anztedinfo].te_fontid=strlen(bzeilen[i]+j+1);  /*Cursor-Position*/
	    tedinfo[anztedinfo].te_fontsize=0;
	    tedinfo[anztedinfo].te_color=0x1100;
	    tedinfo[anztedinfo].te_thickness=1;
	    tedinfo[anztedinfo].te_txtlen=bzeilenlen[i]-j-1;
	    tedinfo[anztedinfo].te_tmplen=bzeilenlen[i]-j-1;
	    // printf("ptext=<%s>\n",tedinfo[anztedinfo].te_ptext);
	    // printf("tmplt=<%s>\n",tedinfo[anztedinfo].te_ptmplt);
	    // printf("valid=<%s>\n",tedinfo[anztedinfo].te_pvalid);
	    anztedinfo++;
            objccount++;
	  /*  printf("Objcount: %d\n",objccount);*/
	  }
	}
      }
    }
    if(symbol>=1) {
      objects[objccount].ob_x=2*window[usewindow].chw;
      objects[objccount].ob_y=window[usewindow].chw;
      objects[objccount].ob_width=50;
      objects[objccount].ob_height=50;
      objects[objccount].ob_spec.index=symbol;
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
    objc_draw(objects,0,-1,0,0,w,h);
    sbut=form_do(objects,0);

    form_dial(3,0,0,0,0,x,y,w,h);
    form_dial(2,0,0,0,0,x,y,w,h);

    if(tval!=NULL) { /* Textfelder zusammensuchen  */
      tval[0]=0;
      for(i=0;i<objccount;i++) {
        if(objects[i].ob_flags & EDITABLE) {
	  strcat(tval,(char *)(objects[i].ob_spec.tedinfo)->te_ptext);
	  tval[strlen(tval)+1]=0;
	  tval[strlen(tval)]=13;
	}
      }
    //  printf("tval=<%s>\n",tval);
    }
    while(anzbuffer) free(buffer[--anzbuffer]);
  }
  return(sbut);
}

#ifndef USE_GEM
short rsrc_free() {
  free(rsrc);
  rsrc=NULL;
  return(0);
}

/*
*	Routine to take an unformatted raw string and based on a
*	template string build a formatted string.
*/

static void ob_format(short just, const char *raw_str, const char *tmpl_str, char *fmt_str) {
#if DEBUG
  printf("ob-format: tmpl=<%s> raw =<%s>\n",tmpl_str,raw_str);
#endif
  if(!fmt_str) return;
  if(!raw_str && !tmpl_str){*fmt_str=0; return;}
  if(!tmpl_str || *tmpl_str==0) {strcpy(fmt_str,raw_str);return;}
  if(!raw_str || *raw_str=='@' ||*raw_str==0) {strcpy(fmt_str,tmpl_str);return;}

  if(just==TE_RIGHT) {
    int ptlen=strlen(tmpl_str);
    const char *ptbeg=tmpl_str+ptlen-1;
    char *pfbeg=fmt_str+ptlen-1;
    const char *prbeg=raw_str+strlen(raw_str)-1;
    fmt_str[ptlen]=0;
    while(ptbeg>=tmpl_str) {
      if (*ptbeg!='_') *pfbeg--=*ptbeg;
      else {
        if(prbeg>=raw_str) {
  	  *pfbeg--=*prbeg;
    	  prbeg--;
        } else *pfbeg--='_';
      }
      ptbeg--;
    } /* while */
  
  } else {
    while(*tmpl_str) {
      if (*tmpl_str!='_') *fmt_str++=*tmpl_str;
      else {
        if(*raw_str) *fmt_str++=*raw_str++;
        else *fmt_str++='_';
      }
      tmpl_str++;
    } 
    *fmt_str=0;
  }
} /* ob_format */

static WORD invert_color(WORD color) {
  switch(color) {
  case    BLACK:    return  WHITE;
  case    WHITE:    return  BLACK;
  case    BLUE:     return  LYELLOW;
  case    LYELLOW:  return  BLUE;
  case    RED:      return  LCYAN;
  case    LCYAN:    return  RED;
  case    GREEN:    return  LMAGENTA;
  case    LMAGENTA: return  GREEN;
  case    CYAN:     return  LRED;
  case    LRED:     return  CYAN;
  case    YELLOW:   return  LBLUE;
  case    LBLUE:    return  YELLOW;
  case    MAGENTA:  return  LGREEN;
  case    LGREEN:   return  MAGENTA;
  case    LWHITE:   return  LBLACK;
  case    LBLACK:   return  LWHITE;
  default:          return  BLACK;
  }
}

static void draw_object(OBJECT *tree,int idx,int rootx,int rooty) {
  signed char randdicke=0;
  char zeichen,opaque=0;
  int fillcolor=BLACK,pattern=8;
  int bgcolor=WHITE;
  int textcolor=BLACK,framecolor=BLACK;
  int i,drawbg=1;
  int obx=tree[idx].ob_x+rootx;
  int oby=tree[idx].ob_y+rooty;
  int obw=tree[idx].ob_width;
  int obh=tree[idx].ob_height;
  int textmode=0;  // Text transparent (0) or opaque (1)
#ifdef DEBUG
  printf("Drawobjc: %d   head=%d  next=%d tail=%d\n",idx,tree[idx].ob_head,
  tree[idx].ob_next, tree[idx].ob_tail);
#endif
  switch(LOBYTE(tree[idx].ob_type)) {
  case G_BOX:
  case G_BOXCHAR:
    zeichen=(tree[idx].ob_spec.index & 0xff000000)>>24;
    randdicke=(tree[idx].ob_spec.index & 0xff0000)>>16;
    // printf("Randdicke:%d\n",randdicke);
    fillcolor=(tree[idx].ob_spec.index & 0xf);
    textcolor=(tree[idx].ob_spec.index & 0xf00)>>8;
    textmode=(tree[idx].ob_spec.index & 0x80)>>7;
    framecolor=(tree[idx].ob_spec.index & 0xf000)>>12;
    pattern=(tree[idx].ob_spec.index & 0x70)>>4;
    if(LOBYTE(tree[idx].ob_type)==G_BOXCHAR) bgcolor=invert_color(textcolor);
    else bgcolor=WHITE;
    break;

  case G_IBOX:
    zeichen=(tree[idx].ob_spec.index & 0xff000000)>>24;
    randdicke=0;
    fillcolor=(tree[idx].ob_spec.index & 0xf);
    textcolor=(tree[idx].ob_spec.index & 0xf00)>>8;
    textmode=(tree[idx].ob_spec.index & 0x80)>>7;
    framecolor=(tree[idx].ob_spec.index & 0xf000)>>12;
    pattern=(tree[idx].ob_spec.index & 0x70)>>4;
    bgcolor=WHITE;
    break;

  case G_TEXT:
  case G_FTEXT:
    framecolor=(((tree[idx].ob_spec.tedinfo)->te_color)>>12) & 0xf;
    textcolor= (((tree[idx].ob_spec.tedinfo)->te_color)>>8) & 0xf;
    opaque=    (((tree[idx].ob_spec.tedinfo)->te_color)>>7) & 1;
    pattern=   (((tree[idx].ob_spec.tedinfo)->te_color)>>4) & 7;
    fillcolor=  ((tree[idx].ob_spec.tedinfo)->te_color) & 0xf;
    randdicke=0;
    bgcolor=invert_color(textcolor);
    break;
  case G_STRING:
  case G_TITLE:
    randdicke=0;
    if(tree[idx].ob_state & SELECTED) {
       fillcolor=WHITE;
       textcolor=BLACK;
       bgcolor=WHITE;
       pattern=8;
     } else {
       drawbg=0;
     }
    break;
  case G_BUTTON:
    randdicke=-1;
    fillcolor=WHITE;
    bgcolor=WHITE;
    textcolor=BLACK;
    pattern=8;
    break;

  case G_BOXTEXT:
  case G_FBOXTEXT:
    framecolor=(((tree[idx].ob_spec.tedinfo)->te_color)>>12) & 0xf;
    textcolor= (((tree[idx].ob_spec.tedinfo)->te_color)>>8) & 0xf;
    opaque=    (((tree[idx].ob_spec.tedinfo)->te_color)>>7) & 1;
    pattern=   (((tree[idx].ob_spec.tedinfo)->te_color)>>4) & 7;
    fillcolor=  ((tree[idx].ob_spec.tedinfo)->te_color) & 0xf;
    randdicke=   (tree[idx].ob_spec.tedinfo)->te_thickness;
    bgcolor=invert_color(textcolor);
    break;
  default:
    drawbg=0;
    break;
  }


  if(tree[idx].ob_state & SELECTED) {
    fillcolor=invert_color(fillcolor);
    textcolor=invert_color(textcolor);
    bgcolor=invert_color(bgcolor);
  }
  if(tree[idx].ob_flags & EXIT) randdicke--;
  if(tree[idx].ob_flags & DEFAULT) randdicke--;



if (drawbg) {

/* Zeichnen  */
  if(tree[idx].ob_state & OUTLINED) {
    SetForeground(gem_colors[bgcolor]);
    FillRectangle(obx-3,oby-3,obw+6,obh+6);
    SetForeground(gem_colors[framecolor]);
    DrawRectangle(obx-3,oby-3,obw+6,obh+6);
  }
  if(tree[idx].ob_state & SHADOWED) {
    SetForeground(gem_colors[framecolor]);
    FillRectangle(obx+obw,oby+window[usewindow].chh/2,window[usewindow].chw,obh);
    FillRectangle(obx+window[usewindow].chw,oby+obh,obw,window[usewindow].chh/2);
  }


/* Hintergrund zeichnen */
  SetForeground(gem_colors[bgcolor]);
  if(!opaque) FillRectangle(obx+1,oby+1,obw-1,obh-1);


  if(pattern) {
    SetForeground(gem_colors[fillcolor]);
    SetFillStyle(FillStippled);
    set_fill(pattern+1);
    FillRectangle(obx,oby,obw,obh);
    SetFillStyle(FillSolid);
  }
}

/* Text zeichnen   */
  if(tree[idx].ob_state & DISABLED) textcolor=LWHITE;

  SetForeground(gem_colors[textcolor]);
  SetBackground(gem_colors[bgcolor]);
  

  switch(LOBYTE(tree[idx].ob_type)) {
    char *text;
    char chr[2];
    TEDINFO *ted;
    int x,y,flen;
  case G_STRING:
  case G_TITLE:
    text=tree[idx].ob_spec.free_string;
   // printf("Title: <%s> y=%d baseline=%d\n",text,oby,baseline);
    DrawString(obx,oby+window[usewindow].baseline,text,strlen(text));
  case G_BOX:
  case G_IBOX:
    break;
  case G_BUTTON:
    text=tree[idx].ob_spec.free_string;
  //  printf("Title: <%s> y=%d baseline=%d h=%d\n",text,oby,window[usewindow].baseline+(obh-window[usewindow].chh)/2,obh);
    DrawString(obx+(obw-window[usewindow].chw*strlen(text))/2,oby+window[usewindow].baseline+(obh-window[usewindow].chh)/2,text,strlen(text));
    break;
  case G_BOXCHAR:
    if(textmode) {
  SetForeground(gem_colors[bgcolor]);
      FillRectangle(obx+(obw-window[usewindow].chw)/2,oby+window[usewindow].baseline+(obh-window[usewindow].chh)/2,window[usewindow].chw,window[usewindow].chh);
  SetForeground(gem_colors[textcolor]);   
    }
    DrawString(obx+(obw-window[usewindow].chw)/2,oby+window[usewindow].baseline+(obh-window[usewindow].chh)/2,&zeichen,1);
    break;
  case G_ALERTTYP:
    chr[0]=tree[idx].ob_spec.index+4;
    chr[1]=0;
    SetForeground(gem_colors[RED]);
    #if defined USE_X11 || defined FRAMEBUFFER
    XSetLineAttributes(window[usewindow].display, window[usewindow].gc, 2, 0,0,0);
    #endif
    ltext(obx,oby,0.5,0.5,0,0,chr);
    if(tree[idx].ob_spec.index==3) ltext(obx+4,oby+12,0.5/6,0.5/2,0,0,"STOP");
    #if defined USE_X11 || defined FRAMEBUFFER
    XSetLineAttributes(window[usewindow].display, window[usewindow].gc, 1, 0,0,0);
    #endif
    break;
  case G_TEXT:
  case G_FTEXT:
  case G_BOXTEXT:
  case G_FBOXTEXT:
    ted=tree[idx].ob_spec.tedinfo;
    
    load_GEMFONT(ted->te_font);      
    text=(char *)malloc(ted->te_txtlen+ted->te_tmplen+1);
    
    ob_format(ted->te_just,ted->te_ptext,ted->te_ptmplt,text);
/* ted->te_fontid ist die cursorposition relativ zum ptext feld, 
   ted->te_fontsize ein offset der darstellung, die maximal darstellbare Laenge ergibt 
   sich aus obw, Maximale laenge des eingabestrings aus der laenge des tmpl
   
   */
    flen=strlen(text);
// printf("Zusammengesetzter text: <%s> len=%d ",text,flen);
// printf("cursorpos=%d, offset=%d\n",ted->te_fontid,ted->te_fontsize);
    x=fontwidth(ted->te_font); /*x temporarily used*/
    if(flen*x>obw) {
      if((flen-ted->te_fontsize)*x>obw) flen=obw/x;
      else flen-=ted->te_fontsize;
    } else ted->te_fontsize=0;
    
    y=oby+(obh-fontheight(ted->te_font))/2+fontbaseline(ted->te_font)-2;
    
    if(ted->te_just==TE_LEFT) x=obx;
    else if(ted->te_just==TE_RIGHT) {
      x=obx+obw-fontwidth(ted->te_font)*flen; 
    } else {
      x=obx+(obw-fontwidth(ted->te_font)*flen)/2; 
    }
    
    if(!opaque || 1) { /* AUf dem Atari war edit-text nie durchsichtig!*/
      SetForeground(gem_colors[bgcolor]);
      FillRectangle(x+1,y-fontbaseline(ted->te_font)+3,
                    fontwidth(ted->te_font)*flen-1,
		    fontheight(ted->te_font)-1);
      SetForeground(gem_colors[textcolor]);
    }
    DrawString(x,y,text+ted->te_fontsize,flen);
    // printf("DrawString: <%s> l=%d\n",text+ted->te_fontsize,flen);
    SetForeground(gem_colors[RED]);
    if(strlen(text)-ted->te_fontsize>obw)
      DrawString(obx+obw,oby+obh,">",1);
    if(ted->te_fontsize)
      DrawString(obx-window[usewindow].chw,oby+obh,"<",1);
    load_GEMFONT(FONT_DEFAULT);
    free(text);
    break;
  case G_IMAGE:
    {BITBLK *bit=tree[idx].ob_spec.bitblk;
    char *helper=(char *)&(bit->bi_pdata);
    unsigned int adr=*((LONG *)helper);
    SetForeground(gem_colors[(bit->bi_color) & 0xf]);
    SetBackground(gem_colors[WHITE]);

    put_bitmap((char *)INT2POINTER(adr),obx,oby,bit->bi_wb*8,bit->bi_hl);
   }
    break;
  case G_ICON:
    {ICONBLK *bit=tree[idx].ob_spec.iconblk;
    char *helper=(char *)&bit->ib_pmask;
    unsigned int adr=*((LONG *)helper);
    char *maskdata=(char *)INT2POINTER(adr);
    helper=(char *)&bit->ib_pdata;
    adr=*((LONG *)helper);
    char *bitdata=(char *)INT2POINTER(adr);
    helper=(char *)&bit->ib_ptext;
    char *text=(char *)INT2POINTER(adr);
    SetForeground(gem_colors[WHITE]);
    put_bitmap(maskdata,obx,oby,bit->ib_wicon,bit->ib_hicon);
    FillRectangle(obx+bit->ib_xtext,oby+bit->ib_ytext,bit->ib_wtext,bit->ib_htext);
    SetBackground(gem_colors[WHITE]);
    SetForeground(gem_colors[BLACK]);
    put_bitmap(bitdata,obx,oby,bit->ib_wicon,bit->ib_hicon);
    /* Icon-Text */
    load_GEMFONT(FONT_SMALLIBM);
    DrawString(obx+bit->ib_xtext,oby+bit->ib_ytext+bit->ib_htext,text,strlen(text));
    /* Icon char */
    load_GEMFONT(FONT_DEFAULT);}
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

short objc_draw(OBJECT *tree,short start, short stop,short rootx,short rooty,short clipw,short cliph) {
  int idx=start;
#if DEBUG 
  printf("**objc_draw: von %d bis %d\n",start,stop);
#endif
// TODO: 
// if(clipw<=0 || cliph<=0) return(0);
#ifdef FRAMEBUFFER
  FB_hide_mouse();
#endif
  draw_object(tree,idx,rootx,rooty);
  if(tree[idx].ob_flags & LASTOB) {
#ifdef FRAMEBUFFER
    FB_show_mouse();
#endif 
    return(1);
  }
  if(tree[idx].ob_head!=-1) {
    if(!(tree[idx].ob_flags & HIDETREE)) {
      objc_draw(tree,tree[idx].ob_head,tree[idx].ob_tail,tree[idx].ob_x+rootx,tree[idx].ob_y+rooty,clipw,cliph);
    }
  }
  if(idx==stop) {
#ifdef FRAMEBUFFER
    FB_show_mouse();
#endif 
    return(1);
  }
  while(tree[idx].ob_next!=-1) {
    idx=tree[idx].ob_next;
    draw_object(tree,idx,rootx,rooty);
    if(tree[idx].ob_flags & LASTOB) {
 #ifdef FRAMEBUFFER
      FB_show_mouse();
#endif  
      return(1);
    }
    if(tree[idx].ob_head!=-1) {
      if(!(tree[idx].ob_flags & HIDETREE)) objc_draw(tree,tree[idx].ob_head,tree[idx].ob_tail,tree[idx].ob_x+rootx,tree[idx].ob_y+rooty,clipw,cliph);
    }
    if(idx==stop) {
#ifdef FRAMEBUFFER
      FB_show_mouse();
#endif     
      return(1);
    }
  }
#ifdef FRAMEBUFFER
      FB_show_mouse();
#endif 
  return(0);
}

short rsrc_gaddr(short re_gtype, unsigned short re_gindex, char **re_gaddr) {
  if(re_gtype==R_TREE) {
    char **ptreebase;
    if(re_gindex>=rsrc->rsh_ntree) return(0);
    ptreebase = (char **)((unsigned long)rsrc+(unsigned long)rsrc->rsh_trindex);
    *re_gaddr=(char *)((long)rsrc+(long)ptreebase[re_gindex]);
    return(1);
  } else if(re_gtype==R_FRSTR) {
    char **ptreebase;
    if(re_gindex>=rsrc->rsh_nstring) return(0);
    ptreebase=(char **)((unsigned long)rsrc+(unsigned long)rsrc->rsh_frstr);
    *re_gaddr=(char *)(((long)ptreebase[re_gindex]+(long)rsrc));
    return(1);
  } else return(0);
}
static void fix_trindex() {
  int anzahl=rsrc->rsh_ntree;
  if(anzahl) {
    char **ptreebase = (char **)((unsigned long)rsrc+(unsigned long)rsrc->rsh_trindex);
    int i; for(i=anzahl-1;i>=0;i--) {LWSWAP((short *)(&ptreebase[i]));}
  }
}
static void fix_frstrindex() {
  int anzahl=rsrc->rsh_nstring;
  if(anzahl) {
    char **ptreebase = (char **)((unsigned long)rsrc+(unsigned long)rsrc->rsh_frstr);
    int i; for(i=anzahl-1;i>= 0;i--) {LWSWAP((short *)(&ptreebase[i]));}
  }
}
static void fix_objc(int chw,int chh) {
  int anzahl=rsrc->rsh_nobs;
  if(anzahl) {
    OBJECT *base = (OBJECT *)((unsigned long)rsrc+(unsigned long)rsrc->rsh_object);
    int i,j;
    for(i =0; i < anzahl; i++) {
      for(j=0;j<sizeof(OBJECT)/2;j++) {WSWAP((char *)((long)&base[i]+2*j));}
      LSWAP((short *)&(base[i].ob_spec));
      if(!(base[i].ob_type==G_BOX || base[i].ob_type==G_BOXCHAR||
	      base[i].ob_type==G_IBOX || base[i].ob_type==G_ALERTTYP))
	    base[i].ob_spec.index+=(LONG)rsrc;	
	
      base[i].ob_x=     LOBYTE(base[i].ob_x)*chw+     HIBYTE(base[i].ob_x);
      base[i].ob_y=     LOBYTE(base[i].ob_y)*chh+     HIBYTE(base[i].ob_y);
      base[i].ob_width= LOBYTE(base[i].ob_width)*chw+ HIBYTE(base[i].ob_width);
      base[i].ob_height=LOBYTE(base[i].ob_height)*chh+HIBYTE(base[i].ob_height);
    }
  }
}
static void fix_tedinfo() {
  if(rsrc->rsh_nted) {
    TEDINFO *base = (TEDINFO *)((unsigned long)rsrc+(unsigned long)rsrc->rsh_tedinfo);
    int i,j;
    for(i =0; i < rsrc->rsh_nted; i++) {
      for(j=0;j<sizeof(TEDINFO)/2;j++) {WSWAP((char *)((long)&base[i]+2*j));}
      LSWAP((short *)&(base[i].te_ptext));
      LSWAP((short *)&(base[i].te_ptmplt));
      LSWAP((short *)&(base[i].te_pvalid));
      base[i].te_ptext+=(LONG)rsrc;	
      base[i].te_ptmplt+=(LONG)rsrc;	
      base[i].te_pvalid+=(LONG)rsrc;	
    }
  }
}
static void fix_bitblk() {
  unsigned int i,j,k,l,m,n=0;
  LONG *helper;
  BITBLK *base = (BITBLK *)((LONG)rsrc+(LONG)rsrc->rsh_bitblk);
  int anzahl=rsrc->rsh_nbb;
  if(anzahl) {
   // printf("FIX_BITBLK: Anzahl=$%04x\n",anzahl);
    for(i=0; i<anzahl; i++) {
      // if(rsrc->rsh_vrsn==0) {
	for(j=0;j<sizeof(BITBLK)/2;j++) {
          WSWAP((char *)((long)&base[i]+2*j));
        }
	LSWAP((short *)&(base[i].bi_pdata));
      // }
      helper=(LONG *)&base[i].bi_pdata;
      *helper+=(LONG)rsrc;
   //   if(rsrc->rsh_vrsn==0) {
        k=*helper;
#if DEBUG
	  printf("Bitmap #%d at %x\n",i,k);
	  printf("w=%d h=%d x=%d y=%d c=%d\n",base[i].bi_wb,base[i].bi_hl,base[i].bi_x,base[i].bi_y,base[i].bi_color);
#endif
	  for(j=0;j<base[i].bi_wb*base[i].bi_hl/2;j++) {
            WSWAP((char *)INT2POINTER(k+2*j));
          }
	  for(j=0;j<base[i].bi_wb*base[i].bi_hl/2;j++) {
	    n=0;
            l=((WORD *)INT2POINTER(k+2*j))[0];
	    for(m=0;m<16;m++) {
	      n=n<<1;
	      n|=(l & 1);
	      l=l>>1;	
	    }
	   *((WORD *)INT2POINTER(k+2*j))=n;
        }
      // }
    }
  }
}
static void fix_iconblk() {
  unsigned int i,j,k,l,m,n=0;
  LONG *helper,*helper2;
  ICONBLK *base = (ICONBLK *)((LONG)rsrc+(LONG)rsrc->rsh_iconblk);
  int anzahl=rsrc->rsh_nib;
  if(anzahl) {
    for(i =0; i < anzahl; i++) {
    //  if(rsrc->rsh_vrsn==0) {
	    for(j=0;j<sizeof(ICONBLK)/2;j++) {
              WSWAP((char *)INT2POINTER((long)&base[i]+2*j));
            }
	    LSWAP((short *)&(base[i].ib_pmask));
	    LSWAP((short *)&(base[i].ib_pdata));
	    LSWAP((short *)&(base[i].ib_ptext));
	//  }
	  helper2=(LONG *)&base[i].ib_pdata;
	  *helper2+=(LONG)rsrc;
	  helper=(LONG *)&base[i].ib_ptext;
	  *helper+=(LONG)rsrc;
	  helper=(LONG *)&base[i].ib_pmask;
	  *helper+=(LONG)rsrc;
#if DEBUG
	  printf("Icon #%d name %s  ",i,*(LONG *)&base[i].ib_ptext);
	  printf("w=%d h=%d x=%d y=%d c=<%c>\n",base[i].ib_wicon,base[i].ib_hicon,
	  base[i].ib_xicon,base[i].ib_yicon,base[i].ib_char);
#endif
        //  if(rsrc->rsh_vrsn==0) {
            k=*helper;  /*(LONG *)&base[i].ib_pmask*/

	    for(j=0;j<base[i].ib_wicon*base[i].ib_hicon/16;j++) {
              WSWAP((char *)INT2POINTER(k+2*j));
            }
	    for(j=0;j<base[i].ib_wicon*base[i].ib_hicon/16;j++) {
	      n=0;
              l=((WORD *)INT2POINTER(k+2*j))[0];
	      for(m=0;m<16;m++) {
	        n=n<<1;
	        n|=(l & 1);
	        l=l>>1;
	      }
	      *((WORD *)INT2POINTER(k+2*j))=n;
            }
            k=*helper2;   /*(LONG *)&base[i].ib_pdata;*/
	
	 for(j=0;j<base[i].ib_wicon*base[i].ib_hicon/16;j++) {
           WSWAP((char *)INT2POINTER(k+2*j));
         }
	 for(j=0;j<base[i].ib_wicon*base[i].ib_hicon/16;j++) {
	   n=0;
           l=((WORD *)INT2POINTER(k+2*j))[0];
	   for(m=0;m<16;m++) {
	     n=n<<1;
	     n|=(l & 1);
	     l=l>>1;
	   }
	   *((WORD *)INT2POINTER(k+2*j))=n;
        }
      //}
    }
  }
}
void memdump    (const unsigned char *adr,int l);	    
short rsrc_load(const char *filename) {
  int i;
  if(exist(filename)) {
    FILE *dptr=fopen(filename,"rb");
    if(dptr==NULL) return(-1);
    int len=lof(dptr);
    rsrc=malloc(len);
    int a;
    if((a=fread(rsrc,1,len,dptr))==len) {
      WSWAP((char *)((long)rsrc));
      if(rsrc->rsh_vrsn==0 || rsrc->rsh_vrsn==1) {
//        if(rsrc->rsh_vrsn==0) {
        for(i=1;i<HDR_LENGTH/2;i++) {
          WSWAP((char *)((long)rsrc+2*i));
        }
//        }
#if DEBUG 
        printf("RSC loaded: name=<%s> len=%d Bytes\n",filename,len);
        printf("Version: %04x   xlen=%d\n",rsrc->rsh_vrsn,rsrc->rsh_rssize);
        printf("%d Trees and %d FRSTRs \n",rsrc->rsh_ntree,rsrc->rsh_nstring);

        printf("OBJC:    %08x  (%d)\n",rsrc->rsh_object,rsrc->rsh_nobs);
        printf("TEDINFO: %08x  (%d)\n",rsrc->rsh_tedinfo,rsrc->rsh_nted);
        printf("ICONBLK: %08x  (%d)\n",rsrc->rsh_iconblk,rsrc->rsh_nib);
        printf("BITBLK:  %08x  (%d)\n",rsrc->rsh_bitblk,rsrc->rsh_nbb);

#endif	
        if(rsrc->rsh_rssize<=len) {
	  if(rsrc->rsh_rssize<len) {
	    printf("Warning: %d extra Bytes detected at end of RSC file.\n",len-rsrc->rsh_rssize);
memdump((unsigned char *)rsrc,len-rsrc->rsh_rssize);
	  }
	
          fix_trindex();
          fix_frstrindex();
	  fix_objc(window[usewindow].chw,window[usewindow].chh);
	  fix_tedinfo();
	  fix_bitblk();
	  fix_iconblk();
	
       	  fclose(dptr);
          return(0);
        } else printf("Invalid RSC file structure. len: %d<>%d\n",len,rsrc->rsh_rssize);
      } else {
        printf("Unsupported RSC version %d.\n",rsrc->rsh_vrsn);
	memdump((unsigned char *)rsrc,64);
	}
    } else {
      printf("fread failed. %d  %d \n",a,len);
    
    }
    fclose(dptr);
    free(rsrc);
    rsrc=NULL;
  } else xberror(-33,filename); /* File not found*/
  return(-1);
}

void objc_add(OBJECT *tree,int p,int c) {
  if(tree[p].ob_tail<0) {
    tree[p].ob_head = c;
    tree[p].ob_tail = c;
    tree[c].ob_next = p;
  } else {
    tree[c].ob_next = p;
    tree[tree[p].ob_tail].ob_next = c;
    tree[p].ob_tail = c;
  }
}

void objc_delete(OBJECT *tree,int object) {
  int i=0;
  int prev=-1;
  int next=tree[object].ob_next;	
  if(next!=-1) {
    if(tree[next].ob_tail==object) next=-1;
  }
  while(1) {	
    if((tree[i].ob_next==object) && (tree[object].ob_tail!=i)) {
      prev=i;
      tree[i].ob_next=tree[object].ob_next;
      break;
    }
    if(tree[i].ob_flags & LASTOB) break;
    i++;
  }
  i=0;
  while(1) {	
    if(tree[i].ob_head==object) tree[i].ob_head=next;
    if(tree[i].ob_tail == object) tree[i].ob_tail=prev;
    if(tree[i].ob_flags & LASTOB) break;
    i++;
  }
}

/* *****************************  */
/*     objc_offset                  */

short objc_offset(OBJECT *tree,short object,short *x,short *y) {
  if((tree == NULL)) return(0);
  *x=*y=0;
  while(1) {
    int last;	
    *x+=tree[object].ob_x;
    *y+=tree[object].ob_y;
    if((tree[object].ob_next<0) || (object==0)) break;		
    do {
      last=object;
      object=tree[object].ob_next;
    } while(last!=tree[object].ob_tail);	
  }
  if(object==0) return 1;
  else return 0;
}




/* *****************************  */
/*     objc_find                  */

short objc_find(OBJECT *tree,short startob, short depth,short x,short y) {
  int sbut=-1;
  int idx=startob;
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
void relobxy(OBJECT *tree,int ndx,int *x, int *y){
  *x=tree[ndx].ob_x;
  *y=tree[ndx].ob_y;
  while((ndx=rootob(tree,ndx))>=0){
    *x+=tree[ndx].ob_x;
    *y+=tree[ndx].ob_y;
  }
}

static int ob_curpos(TEDINFO *ted) {
  int n=ted->te_fontid;
  int i=0,j=0;
  char *a=(char *)ted->te_ptmplt;
 // printf("a=<%s>\n",a);
  while(*a) {
    if(*a++=='_') {if(++i>=n) break;}
    else j++;
  }
  return(j+i-ted->te_fontsize);
}

void draw_edcursor(OBJECT *tree,int ndx){
     TEDINFO *ted=tree[ndx].ob_spec.tedinfo;
     int x,y,p,flen,obw;
     int _chw=fontwidth(ted->te_font);
     int _chh=fontheight(ted->te_font);
     
     
     relobxy(tree,ndx,&x,&y);
     SetForeground(gem_colors[RED]);
     p=ob_curpos(ted);    
     obw=tree[ndx].ob_width;
     flen=min(obw,_chw*(ted->te_tmplen-ted->te_fontsize));

     if(ted->te_just==TE_LEFT) {
      ; 
     } else if(ted->te_just==TE_RIGHT) {
       x+=obw-flen; 
     } else {
       x+=(obw-flen)/2; 
     }
     DrawLine(x+_chw*p,y,x+_chw*p,y+_chh+4);
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

short form_center(OBJECT *tree,short *x,short *y,short *w,short *h) {
  /* Objektbaum Zentrieren */
#ifdef DEBUG
  printf("**form_center:\n");
#endif

  tree->ob_x=window[usewindow].x+(window[usewindow].w-tree->ob_width)/2;
  tree->ob_y=window[usewindow].y+(window[usewindow].h-tree->ob_height)/2;
  *x=tree->ob_x;
  *y=tree->ob_y;
  *w=tree->ob_width;
  *h=tree->ob_height;
  return(0);
}
#endif
#endif
