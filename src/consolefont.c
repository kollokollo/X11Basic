/* consolefont.c                            (c) Markus Hoffmann  2007-2008
*/

/* This file is part of TTconsole, the TomTom Console interface
 * ======================================================================
 * TTconsole is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include "terminal.h"
#include "consolefont.h"
#include "framebuffer.h"

#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a>b)?a:b)

static int bigfont=0;  /* Default use the small font
                          0 = 5x7
			  1 = 8x16
			  2 = 16x32
			  4 = 32x64
			  */

int CharWidth=CharWidth57;
int CharHeight=CharHeight57;

extern struct winsize win;
extern unsigned char *fontlist816[];
extern unsigned char *fontlist57[];

void Fb_BlitCharacter(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned char character, int flags, int fontnr){
  if(bigfont>2) Fb_BlitCharacter816_scale(x,y,aColor,aBackColor,character,flags,fontnr,8*bigfont,16*bigfont);
  else if(bigfont>1) Fb_BlitCharacter1632(x,y,aColor,aBackColor,character,flags,fontnr);
  else if(bigfont) Fb_BlitCharacter816(x,y,aColor,aBackColor,character,flags,fontnr);
  else Fb_BlitCharacter57(x,y,aColor,aBackColor,character,flags,fontnr);
}
void Fb_BlitUnicodeCharacter(int x, int y, unsigned short aColor, unsigned short aBackColor, unsigned int unicode, int flags, int fontnr) {
  if(unicode<0x80) {
    Fb_BlitCharacter(x, y,aColor,aBackColor, unicode&0xff, flags, fontnr);
    return;
  }
  if(bigfont>2) Fb_BlitCharacter816_scale_raw(x,y,aColor,aBackColor,flags,8*bigfont,16*bigfont,unicode2glyph816(unicode));
  else if(bigfont>1) Fb_BlitCharacter1632_raw(x,y,aColor,aBackColor,flags,unicode2glyph816(unicode));
  else if(bigfont) Fb_BlitCharacter816_raw(x,y,aColor,aBackColor,flags,unicode2glyph816(unicode));
  else Fb_BlitCharacter57_raw(x,y,aColor,aBackColor,flags,unicode2glyph57(unicode));
}


void Fb_BlitText57(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str) {
  unsigned char a;
  unsigned short unicode;
  while((a=*str++)) {
    if(utf8(a,&unicode)) {
      Fb_BlitCharacter57_raw(x, y, aColor, aBackColor, 0, unicode2glyph57(unicode));
      x+=CharWidth57;
    }
  }
}
void Fb_BlitText816(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str) {
  unsigned char a;
  unsigned short unicode;
  while((a=*str++)) {
    if(utf8(a,&unicode)) {
      Fb_BlitCharacter816_raw(x, y, aColor, aBackColor,0,unicode2glyph816(unicode));
      x+=CharWidth816;
    }
  }
}
void Fb_BlitText1632(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str) {
  unsigned char a;
  unsigned short unicode;
  while((a=*str++)) {
    if(utf8(a,&unicode)) {
      Fb_BlitCharacter1632_raw(x, y, aColor, aBackColor, 0,unicode2glyph816(unicode));
      x+=CharWidth1632;
    }
  }
}
void Fb_BlitText3264(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str) {
  unsigned char a;
  unsigned short unicode;
  while((a=*str++)) {
    if(utf8(a,&unicode)) {
      Fb_BlitCharacter816_scale_raw(x, y, aColor, aBackColor,0,32,64,unicode2glyph816(unicode));
      x+=32;
    }
  }
}
void Fb_BlitText816_scale(int x, int y, unsigned short aColor, unsigned short aBackColor, const char *str,int w,int h) {
  unsigned char a;
  unsigned short unicode;
  while((a=*str++)) {
    if(utf8(a,&unicode)) {
      Fb_BlitCharacter816_scale_raw(x, y, aColor, aBackColor,0,w,h,unicode2glyph816(unicode));
      x+=w;
    }
  }
}

void Fb_BlitText(int x, int y, unsigned short aColor, unsigned short aBackColor, char *str) {
  if(bigfont>2) Fb_BlitText816_scale(x,y,aColor, aBackColor,str,8*bigfont,16*bigfont);
  else if(bigfont>1) Fb_BlitText1632(x,y,aColor, aBackColor,str);
  else if(bigfont) Fb_BlitText816(x,y,aColor, aBackColor,str);
  else Fb_BlitText57(x,y,aColor, aBackColor,str);
}


static void set_fontsize(int big) {
  /* Fontgroesse etc anpassen */
  if(big>2) {
    CharWidth=CharWidth816*big;
    CharHeight=CharHeight816*big; 
  } else if(big>1) {
    CharWidth=CharWidth1632;
    CharHeight=CharHeight1632;
  } else if(big) {
    CharWidth=CharWidth816;
    CharHeight=CharHeight816;
  } else {
    CharWidth=CharWidth57;
    CharHeight=CharHeight57;
  }
  bigfont=big;
  /* Setze information fuer terminals */
  win.ws_row=screen.height/CharHeight;
  win.ws_col=screen.width/CharWidth;
  col=min(col,win.ws_col-1);
  lin=min(lin,win.ws_row-1);
  init_textscreen();
}
void change_fontsize(int big) {
  char buffer[100];
    set_fontsize(big);
    if(terminal_fd!=-1) {
//      pty_change_window_size(terminal_fd, win.ws_row, win.ws_col, win.ws_xpixel, win.ws_ypixel);
      ioctl(terminal_fd, TIOCSWINSZ, &win); /*Announce new screen size*/
    }
//    textscreen_redraw(0,0,win.ws_col,win.ws_row);
    sprintf(buffer,"%dx%d characters. ",win.ws_col,win.ws_row);
    Fb_BlitText57(screen.width-strlen(buffer)*5,0,GREEN,BLACK,buffer);
   // putchar(27);printf("[8;1H");
   // if(big==0) printf("Small font activated. ");
  //  else printf("Big font activated. ");
  //  printf("%dx%d characters. \n",win.ws_col,win.ws_row);
}

void cursor_onoff(int onoff) {
  static int cursor=0;
  static int ox,oy,ow;
  int chw=CharWidth,chh=CharHeight;
  if(!cursor && !onoff) ; /*nixtun*/
  else if(!cursor && onoff && (attributes&AT_CURSORON)) { /*Einschalten */
    if(attributes&AT_INSERT) ow=CharWidth/2;
    else ow=CharWidth;
      ox=col;
      oy=lin;
      Fb_inverse(ox*chw,oy*chh,ow,CharHeight);
      cursor=onoff;
  } else if(cursor && !onoff) { /* Ausschalten */
     Fb_inverse(ox*chw,oy*chh,ow,CharHeight);
     cursor=onoff;
  } else if(ox!=col || oy!=lin) {  /* Umpositionieren */
     Fb_inverse(ox*chw,oy*chh,ow,CharHeight);
      ox=col;
      oy=lin;
      Fb_inverse(ox*chw,oy*chh,ow,CharHeight);
  }
}
int utf8(unsigned char a,unsigned short *runicode) {
  static unsigned char utf8_start=0;
  static int utf8_count=0;
  static unsigned short unicode=0;

  if(a>=0x80) {
    if((a&0xc0)!=0x80) { // Startbyte
      if(utf8_start) {
	utf8_start=0;
	*runicode=0x3f;
	return(1);
      }
      if(a==0xc0 || a==0xc1 || a>=0xf5) { *runicode=0x3f;return(1);}
      else {
        utf8_start=a;
	unicode=0;
	if(utf8_start<0xe0) {
	  utf8_count=1;
	  unicode=(utf8_start&0x1f);
	} else if(utf8_start<0xf0) {
	  utf8_count=2;
	  unicode=(utf8_start&0xf);
	} else {
	  utf8_count=3;
	  unicode=(utf8_start&0x7);
	}
      }
    } else {   // follow byte
      if(utf8_start==0 || utf8_count<=0)  {*runicode=0x3f;return(0);}
      else {
        unicode=(unicode<<6)|(a&0x3f);
	if(--utf8_count==0) {
	  *runicode=unicode;
          unicode=0;
          utf8_start=0;
          utf8_count=0; 
	  return(1);         
	}
      }
    }
  } else {
    unicode=0;
    utf8_start=0;
    utf8_count=0;
    *runicode=(unsigned short)a;
    return(1);
  }
  return(0);
}

const unsigned char *unicode2glyph816(unsigned short unicode) {
  if(unicode<0x80) return(&((fontlist816[0])[unicode*CharHeight816]));  
  unsigned int page=(unicode&0xff00)>>8;
  const unsigned short *table=unicode_pages[page];
  if(table==NULL) return(&((fontlist816[0])['?'*CharHeight816]));
  if(table[unicode&0xff]<0x100) return(&((fontlist816[0])[table[unicode&0xff]*CharHeight816]));  
  return(&unicode_font[(table[unicode&0xff]-0x100)*16]);
}
const unsigned char *unicode2glyph57(unsigned short unicode) {
  if(unicode<0x80) return(&((fontlist57[0])[unicode*5]));  
  unsigned int page=(unicode&0xff00)>>8;
  const unsigned short *table=unicode_pages57[page];
  if(table==NULL) return(&((fontlist57[0])['?'*5]));
  if(table[unicode&0xff]<0x100) return(&((fontlist57[0])[table[unicode&0xff]*5]));  
  return(&unicode_font57[(table[unicode&0xff]-0x100)*5]);
}


