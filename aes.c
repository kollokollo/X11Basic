
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
#include "x11basic.h"
#include "graphics.h"
#include "aes.h"
#include "file.h"
#include "window.h"

#ifndef FRAMEBUFFER
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

unsigned int chw=4,chh=8,baseline=7,depth=8;
int gem_colors[16];
const AESRGBCOLOR gem_colordefs[]={
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

ARECT sbox;

/* AES-Nachbildungen (c) Markus Hoffmann     */

#ifdef FRAMEBUFFER
extern int font_behaviour;
#endif

void gem_init() {
  int i;
#ifdef USE_X11
  Window root;
#endif
  unsigned int border;
  /* Screendimensionen bestimmem */
#ifdef FRAMEBUFFER
  FB_get_geometry(&sbox.x,&sbox.y,&sbox.w,&sbox.h,&border,&depth);
#endif
#ifdef USE_X11
    XGetGeometry(display[usewindow],win[usewindow],&root,&sbox.x,&sbox.y,&sbox.w,&sbox.h,&border,&depth);
#if 0
   weiss=WhitePixel(display[usewindow], DefaultScreen(display[usewindow]));
   schwarz=BlackPixel(display[usewindow], DefaultScreen(display[usewindow]));
#endif
#endif
#ifdef USE_SDL
  border=sbox.x=sbox.y=0;
  sbox.w=display[usewindow]->w;
  sbox.h=display[usewindow]->h;
  depth=32;
#endif
 
#ifdef WINDOWS_NATIVE
  RECT interior;

  GetClientRect(win_hwnd[usewindow],&interior);
  sbox.x=interior.left;
  sbox.y=interior.top;
  sbox.w=interior.right-interior.left;
  sbox.h=interior.bottom-interior.top;
#endif
#ifdef DEBUG
  printf("gem_init: usewin=%d\n",usewindow);
  printf("sbox=(%d,%d,%d,%d)\n",sbox.x,sbox.y,sbox.w,sbox.h);
#endif
#ifdef FRAMEBUFFER
  if(font_behaviour==0) { /* Auto */
    if(sbox.w>=1200) load_GEMFONT(FONT_LARGE);
    else if(sbox.w>=640) load_GEMFONT(FONT_BIG);
    else load_GEMFONT(FONT_SMALL);
  } else if(font_behaviour==1) {
    load_GEMFONT(FONT_SMALL);
  } else if(font_behaviour==2) {
    if(sbox.w>=320) load_GEMFONT(FONT_BIG);
    else load_GEMFONT(FONT_SMALL);
  } else if(font_behaviour==3) {
    if(sbox.w>=640) load_GEMFONT(FONT_LARGE);
    else if(sbox.w>=320) load_GEMFONT(FONT_BIG);
    else load_GEMFONT(FONT_SMALL);
  } else load_GEMFONT(FONT_DEFAULT);
#else
  load_GEMFONT(FONT_DEFAULT);
#endif
  for(i=0;i<16;i++)
    gem_colors[i]=get_color(gem_colordefs[i].r,gem_colordefs[i].g,gem_colordefs[i].b);

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
  chw=siz.cx/2;
  chh=siz.cy;
  baseline=chh-2;
#endif
#ifdef FRAMEBUFFER
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
    chw=CharWidth1632;
    chh=CharHeight1632;
    baseline=chh-4;
  } else if(n==FONT_BIG) {
    chw=CharWidth816;
    chh=CharHeight816;
    baseline=chh-2; 
  } else if(n==FONT_SMALL) {
    chw=CharWidth57;
    chh=CharHeight57;
    baseline=chh-0;
  } else {
    chw=CharWidth;
    chh=CharHeight;
    baseline=chh-0;
#ifdef ANDROID
    char t[100];
    sprintf(t,"unbek font #%d",n);
    backlog(t);
#endif
  }
#endif
#ifdef USE_X11
  XGCValues gc_val;
  XFontStruct *fs;
  if(n==FONT_SMALL) fs=XLoadQueryFont(display[usewindow], GEMFONTSMALL);
  else fs=XLoadQueryFont(display[usewindow], GEMFONT);
  if(fs!=NULL)  {
     gc_val.font=fs->fid;
     XChangeGC(display[usewindow], gc[usewindow],  GCFont, &gc_val);
     chw=fs->max_bounds.width;
     chh=fs->max_bounds.ascent+fs->max_bounds.descent;
     baseline=fs->max_bounds.ascent;
  }
#endif
#ifdef USE_SDL
  if(n==FONT_BIG || n==FONT_IBM) {
    chw=8;
    chh=16;
    baseline=chh-2; 
    gfxPrimitivesSetFont(spat_a816,chw,chh); 	
  } else {
    chw=5;
    chh=7;
    baseline=chh-0;
    gfxPrimitivesSetFont(asciiTable,chw,chh); 	
  }
#endif
}
#if 0
void box_center(ARECT *b) {
    b->x=sbox.x+(sbox.w-b->w)/2; /* Koordinaten fuer Bildmitte: */
    b->y=sbox.y+(sbox.h-b->h)/2;
}
#endif
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
/* -------------------------------- AES-Implementationen ------------------*/


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
  TEDINFO tedinfo[32];
  char tmplt[32*80];
  char valid[32*80];
  int tmpltptr=0,validptr=0;
  OBJECT objects[64]={{-1,1,1,G_BOX, 0, OUTLINED, 0x00021100, 0,0,100,100}};
  int objccount=1;
  int x,y,w,h;
#ifdef DEBUG
  printf("**form_alert:\n");
#endif
memset(tmplt,'_',32*80);
memset(valid,'X',32*80);
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
    objects[0].ob_height=max(objects[0].ob_height+2*max(chh,16),max(chh,16)*2+(anzzeilen+2)*chh);


    for(i=0;i<anzzeilen;i++) maxc=max(maxc,strlen(bzeilen[i]));
    objects[0].ob_width+=chw*maxc;

     /* Buttons  */
    maxc=0;
    for(i=0;i<anzbuttons;i++) maxc=max(maxc,strlen(bbuttons[i]));
    objects[0].ob_width=max(objects[0].ob_width,chw*(4+(maxc+1)*anzbuttons+2*(anzbuttons-1)));

    for(i=0;i<anzbuttons; i++) {
      objects[objccount].ob_x=objects[0].ob_width/2-chw*((maxc+1)*anzbuttons+2*(anzbuttons-1))/2+i*chw*(maxc+3);
      objects[objccount].ob_y=objects[0].ob_height-2*max(chh,16);
      objects[objccount].ob_width=(chw+1)*maxc;
#ifdef FRAMEBUFFER
      objects[objccount].ob_height=max(chh,16)+3;
#else
      objects[objccount].ob_height=chh+3;
#endif
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
	//  printf("Textfeld gefunden. OB=%d vorne=<%s>\n",objccount,objects[objccount-1].ob_spec);
	    
	    objects[objccount].ob_x=textx+chh+chw*j+chw;
            objects[objccount].ob_y=(1+i)*chh;
            objects[objccount-1].ob_width=chw*(strlen((char *)objects[objccount-1].ob_spec));
            objects[objccount].ob_width=chw*(strlen(bzeilen[i]+j+1));
	   // printf("default=<%s>\n",bzeilen[i]+j+1);
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
	    tedinfo[anztedinfo].te_ptmplt=(LONG)&tmplt[tmpltptr];
	    tmpltptr+=strlen(bzeilen[i]+j+1)+1;
	    tmplt[tmpltptr-1]=0;
	    tedinfo[anztedinfo].te_pvalid=(LONG)&valid[validptr];
	    validptr+=strlen(bzeilen[i]+j+1)+1;
	    valid[validptr-1]=0;
	    tedinfo[anztedinfo].te_font=FONT_IBM;
	    tedinfo[anztedinfo].te_just=TE_LEFT;
	    tedinfo[anztedinfo].te_junk1=strlen(bzeilen[i]+j+1);
	    tedinfo[anztedinfo].te_junk2=0;
	    tedinfo[anztedinfo].te_color=0x1100;
	    tedinfo[anztedinfo].te_thickness=1;
	    tedinfo[anztedinfo].te_txtlen=strlen((char *)tedinfo[anztedinfo].te_ptext);
	    tedinfo[anztedinfo].te_tmplen=strlen((char *)tedinfo[anztedinfo].te_ptext);
	    anztedinfo++;
            objccount++;
	  /*  printf("Objcount: %d\n",objccount);*/
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
    objc_draw(objects,0,-1,0,0);
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
    //  printf("tval=<%s>\n",tval);
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
/*
*	Routine to take an unformatted raw string and based on a
*	template string build a formatted string.
*/

void ob_format(short just, char *raw_str, char *tmpl_str, char *fmt_str) {
  char *pfbeg, *ptbeg, *prbeg;
  char *pfend, *ptend, *prend;
  short  inc, ptlen, prlen;
#if DEBUG
printf("ob-format: tmpl=<%s> ",tmpl_str);
printf(" raw =<%s>\n",raw_str);
#endif
  if(*raw_str=='@') *raw_str=0;
  pfbeg=fmt_str;
  ptbeg=tmpl_str;
  prbeg=raw_str;

  ptlen=strlen(tmpl_str);
  prlen=strlen(raw_str);

  if(!ptlen) {
    strcpy(fmt_str,raw_str);
    return;
  }

  inc=1;
  pfbeg[ptlen]=0;
  if(just==TE_RIGHT) {
    inc=-1;
    pfbeg=pfbeg+ptlen-1;
    ptbeg=ptbeg+ptlen-1;
    prbeg=prbeg+prlen-1;
  }

  pfend=pfbeg+(inc*ptlen);
  ptend=ptbeg+(inc*ptlen);
  prend=prbeg+(inc*prlen);

  while(ptbeg!=ptend) {
    if (*ptbeg!='_') *pfbeg=*ptbeg;
    else {
      if(prbeg!=prend) {
  	*pfbeg=*prbeg;
  	prbeg+=inc;
      }
      else *pfbeg='_';
    }
    pfbeg+=inc;
    ptbeg+=inc;
  } /* while */
#if DEBUG
printf("--> fmt=<%s>\n",fmt_str);
#endif
} /* ob_format */

WORD invert_color(WORD color) {
  switch(color) {
  case    BLACK:
  	  return  WHITE;
  case    WHITE:
  	  return  BLACK;
  case    BLUE:
  	  return  LYELLOW;
  case    LYELLOW:
  	  return  BLUE;
  case    RED:
  	  return  LCYAN;
  case    LCYAN:
  	  return  RED;
  case    GREEN:
  	  return  LMAGENTA;
  case    LMAGENTA:
  	  return  GREEN;
  case    CYAN:
  	  return  LRED;
  case    LRED:
  	  return  CYAN;
  case    YELLOW:
  	  return  LBLUE;
  case    LBLUE:
  	  return  YELLOW;
  case    MAGENTA:
  	  return  LGREEN;
  case    LGREEN:
  	  return  MAGENTA;
  case    LWHITE:
  	  return  LBLACK;
  case    LBLACK:
  	  return  LWHITE;
  default:
    return	BLACK;
  }
}

void draw_object(OBJECT *tree,int idx,int rootx,int rooty) {
  signed char randdicke=0;
  char zeichen,opaque=0;
  int fillcolor=BLACK,pattern=8;
  int bgcolor=WHITE;
  int textcolor=BLACK,textmode,framecolor=BLACK;
  int i,drawbg=1;
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
    if(LOBYTE(tree[idx].ob_type)==G_BOXCHAR) bgcolor=invert_color(textcolor);
    else bgcolor=WHITE;
    break;

  case G_IBOX:
    zeichen=(tree[idx].ob_spec & 0xff000000)>>24;
    randdicke=0;
    fillcolor=(tree[idx].ob_spec & 0xf);
    textcolor=(tree[idx].ob_spec & 0xf00)>>8;
    textmode=(tree[idx].ob_spec & 0x80)>>7;
    framecolor=(tree[idx].ob_spec & 0xf000)>>12;
    pattern=(tree[idx].ob_spec & 0x70)>>4;
    bgcolor=WHITE;
    break;

  case G_TEXT:
  case G_FTEXT:
    framecolor=((((TEDINFO *)((long)tree[idx].ob_spec))->te_color)>>12) & 0xf;
    textcolor= ((((TEDINFO *)((long)tree[idx].ob_spec))->te_color)>>8) & 0xf;
    opaque=    ((((TEDINFO *)((long)tree[idx].ob_spec))->te_color)>>7) & 1;
    pattern=   ((((TEDINFO *)((long)tree[idx].ob_spec))->te_color)>>4) & 7;
    fillcolor= (((TEDINFO *) ((long)tree[idx].ob_spec))->te_color) & 0xf;
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
    framecolor=((((TEDINFO *)((long)tree[idx].ob_spec))->te_color)>>12) & 0xf;
    textcolor= ((((TEDINFO *)((long)tree[idx].ob_spec))->te_color)>>8) & 0xf;
    opaque=    ((((TEDINFO *)((long)tree[idx].ob_spec))->te_color)>>7) & 1;
    pattern=   ((((TEDINFO *)((long)tree[idx].ob_spec))->te_color)>>4) & 7;
    fillcolor= (((TEDINFO *) ((long)tree[idx].ob_spec))->te_color) & 0xf;
    randdicke= ((TEDINFO *)  ((long)tree[idx].ob_spec))->te_thickness;
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
    FillRectangle(obx+obw,oby+chh/2,chw,obh);
    FillRectangle(obx+chw,oby+obh,obw,chh/2);
  }


/* Hintergrund zeichnen */
  SetForeground(gem_colors[bgcolor]);
  if(!opaque) FillRectangle(obx,oby,obw,obh);


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
    text=(char *)((long)tree[idx].ob_spec);
    DrawString(obx,oby+baseline,text,strlen(text));
  case G_BOX:
  case G_IBOX:
    break;
  case G_BUTTON:
    text=(char *)((long)tree[idx].ob_spec);
    DrawString(obx+(obw-chw*strlen(text))/2,oby+baseline+(obh-chh)/2,text,strlen(text));
    break;
  case G_BOXCHAR:
    DrawString(obx+(obw-chw)/2,oby+baseline+(obh-chh)/2,&zeichen,1);
    break;
  case G_ALERTTYP:
    chr[0]=tree[idx].ob_spec+4;
    chr[1]=0;
    SetForeground(gem_colors[RED]);
    #if defined USE_X11 || defined FRAMEBUFFER
    XSetLineAttributes(display[usewindow], gc[usewindow], 2, 0,0,0);
    #endif
    ltext(obx,oby,0.5,0.5,0,0,chr);
    if(tree[idx].ob_spec==3) ltext(obx+4,oby+12,0.5/6,0.5/2,0,0,"STOP");
    #if defined USE_X11 || defined FRAMEBUFFER
    XSetLineAttributes(display[usewindow], gc[usewindow], 1, 0,0,0);
    #endif
    break;
  case G_TEXT:
  case G_FTEXT:
  case G_BOXTEXT:
  case G_FBOXTEXT:
    ted=(TEDINFO *)((long)tree[idx].ob_spec);
    
    load_GEMFONT(ted->te_font);      
    text=(char *)malloc(strlen((char *)ted->te_ptext)+1+strlen((char *)ted->te_ptmplt));
    
    ob_format(ted->te_just,(char *)ted->te_ptext,(char *)ted->te_ptmplt,text);

/* j1 ist die cursorposition relativ zum ptext feld, 
   j2 ein offset der darstellung, die maximal darstellbare Laenge ergibt 
   sich aus obw, Maximale laenge des eingabestrings aus der laenge des tmpl
   
   */

//printf("j1=%d, j2=%d\n",ted->te_junk1,ted->te_junk2);
    flen=strlen(text);
    x=fontwidth(ted->te_font); /*x temporarily used*/
    if(flen*x>obw) {
      if((flen-ted->te_junk2)*x>obw) flen=obw/x;
      else flen-=ted->te_junk2;
    } else ted->te_junk2=0;
    
    y=oby+(obh+fontheight(ted->te_font))/2+fontbaseline(ted->te_font)-fontheight(ted->te_font);
    
    if(ted->te_just==TE_LEFT) x=obx;
    else if(ted->te_just==TE_RIGHT) {
      x=obx+obw-fontwidth(ted->te_font)*flen; 
    } else {
      x=obx+(obw-fontwidth(ted->te_font)*flen)/2; 
    }
    
    if(!opaque || 1) { /* AUf dem Atari war edit-text nie durchsichtig!*/
      SetForeground(gem_colors[bgcolor]);
      FillRectangle(x,y-fontbaseline(ted->te_font),
                    fontwidth(ted->te_font)*flen,
		    fontheight(ted->te_font));
      SetForeground(gem_colors[textcolor]);
    }
    DrawString(x,y,text+ted->te_junk2,flen);
    SetForeground(gem_colors[RED]);
    if(strlen(text)-ted->te_junk2>obw)
      DrawString(obx+obw,oby+obh,">",1);
    if(ted->te_junk2)
      DrawString(obx-chw,oby+obh,"<",1);
    load_GEMFONT(FONT_DEFAULT);
    free(text);
    break;
  case G_IMAGE:
    {BITBLK *bit=(BITBLK *)((long)tree[idx].ob_spec);
    unsigned int adr;

    adr=*((LONG *)&(bit->bi_pdata));
    SetForeground(gem_colors[(bit->bi_color) & 0xf]);
    SetBackground(gem_colors[WHITE]);

    put_bitmap((char *)adr,obx,oby,bit->bi_wb*8,bit->bi_hl);
   }
    break;
  case G_ICON:
    {ICONBLK *bit=(ICONBLK *)((long)tree[idx].ob_spec);
    unsigned int adr;
    adr=*(LONG *)&bit->ib_pmask;

    SetForeground(gem_colors[WHITE]);
    put_bitmap((char *)adr,obx,oby,bit->ib_wicon,bit->ib_hicon);
    FillRectangle(obx+bit->ib_xtext,oby+bit->ib_ytext,bit->ib_wtext,bit->ib_htext);
    adr=*(LONG *)&bit->ib_pdata;
    SetBackground(gem_colors[WHITE]);
    SetForeground(gem_colors[BLACK]);
    put_bitmap((char *)adr,obx,oby,bit->ib_wicon,bit->ib_hicon);
    /* Icon-Text */
    load_GEMFONT(FONT_SMALLIBM);
    DrawString(obx+bit->ib_xtext,oby+bit->ib_ytext+bit->ib_htext,(char *)*(LONG *)&bit->ib_ptext,strlen((char *)*(LONG *)&bit->ib_ptext));
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


int objc_draw( OBJECT *tree,int start, int stop,int rootx,int rooty) {
  int idx=start;
#if DEBUG 
  printf("**objc_draw: von %d bis %d\n",start,stop);
#endif 
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
      objc_draw(tree,tree[idx].ob_head,tree[idx].ob_tail,tree[idx].ob_x+rootx,tree[idx].ob_y+rooty);
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
      if(!(tree[idx].ob_flags & HIDETREE)) objc_draw(tree,tree[idx].ob_head,tree[idx].ob_tail,tree[idx].ob_x+rootx,tree[idx].ob_y+rooty );
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


int rsrc_gaddr(int re_gtype, unsigned int re_gindex, char **re_gaddr) {
  if(re_gtype==R_TREE) {
    char **ptreebase;
    if(re_gindex>=rsrc->rsh_ntree) return(0);
    ptreebase = (char **)((unsigned long)rsrc+(unsigned long)rsrc->rsh_trindex);
    *re_gaddr=(char *)(((long)ptreebase[re_gindex]+(long)rsrc));
    return(1);
  } else if(re_gtype==R_FRSTR) {
    char **ptreebase;
    if(re_gindex>=rsrc->rsh_nstring) return(0);
    ptreebase=(char **)((unsigned long)rsrc+(unsigned long)rsrc->rsh_frstr);
    *re_gaddr=(char *)(((long)ptreebase[re_gindex]+(long)rsrc));
    return(1);
  } else return(0);
}

void fix_trindex() {
  int i;
  char **ptreebase;
  int anzahl;
  ptreebase = (char **)((unsigned long)rsrc+(unsigned long)rsrc->rsh_trindex);
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

  ptreebase = (char **)((unsigned long)rsrc+(unsigned long)rsrc->rsh_frstr);
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

  base = (OBJECT *)((unsigned long)rsrc+(unsigned long)rsrc->rsh_object);
  anzahl=rsrc->rsh_nobs;
  if(anzahl) {
    for(i =0; i < anzahl; i++) {
      if(rsrc->rsh_vrsn==0) {
	for(j=0;j<sizeof(OBJECT)/2;j++) {
          WSWAP((char *)((long)&base[i]+2*j));
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
  base = (TEDINFO *)((unsigned long)rsrc+(unsigned long)rsrc->rsh_tedinfo);
  anzahl=rsrc->rsh_nted;
    if(anzahl) {
      for(i =0; i < anzahl; i++) {
	if(rsrc->rsh_vrsn==0) {
	  for(j=0;j<sizeof(TEDINFO)/2;j++) {
            WSWAP((char *)((long)&base[i]+2*j));
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
          WSWAP((char *)((long)&base[i]+2*j));
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
              WSWAP((char *)((long)&base[i]+2*j));
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

int rsrc_load(char *filename) {
  FILE *dptr;
  int i,len;
  if(exist(filename)) {
    dptr=fopen(filename,"r");
    if(dptr==NULL) return(-1);
    len=lof(dptr);
    rsrc=malloc(len);
    if(fread(rsrc,1,len,dptr)==len) {
      WSWAP((char *)((long)rsrc));
      if(rsrc->rsh_vrsn==0 || rsrc->rsh_vrsn==1) {
        if(rsrc->rsh_vrsn==0) {
        for(i=1;i<HDR_LENGTH/2;i++) {
          WSWAP((char *)((long)rsrc+2*i));
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

int objc_offset(OBJECT *tree,int object,int *x,int *y) {
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

int objc_find(OBJECT *tree,int x,int y) {
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
void relobxy(OBJECT *tree,int ndx,int *x, int *y){
  *x=tree[ndx].ob_x;
  *y=tree[ndx].ob_y;
  while((ndx=rootob(tree,ndx))>=0){
    *x+=tree[ndx].ob_x;
    *y+=tree[ndx].ob_y;
  }
}

int ob_curpos(TEDINFO *ted) {
  int n=ted->te_junk1;
  int i=0,j=0;
  char *a=(char *)ted->te_ptmplt;
 // printf("a=<%s>\n",a);
  while(*a) {
    if(*a++=='_') {if(++i>=n) break;}
    else j++;
  }
  return(j+i-ted->te_junk2);
}

void draw_edcursor(OBJECT *tree,int ndx){
     TEDINFO *ted=(TEDINFO *)(tree[ndx].ob_spec);
     int x,y,p,flen,obw;
     int _chw=fontwidth(ted->te_font);
     int _chh=fontheight(ted->te_font);
     
     
     relobxy(tree,ndx,&x,&y);
     SetForeground(gem_colors[RED]);
     p=ob_curpos(ted);    
     obw=tree[ndx].ob_width;
     flen=min(obw,_chw*(ted->te_tmplen-ted->te_junk2));

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






#endif
