/* terminal.c                            (c) Markus Hoffmann  2007-2008
*/

/* This file is part of TTconsole, the TomTom Console interface
 * ======================================================================
 * TTconsole is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdlib.h>
#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <termios.h>
#ifndef __APPLE__
#include <linux/fb.h>
#endif
#include <sys/ioctl.h>

#include "terminal.h"
#include "consolefont.h"
#include "framebuffer.h"
#include "raw_mouse.h"

#define LineLen win.ws_col
#define AnzLine win.ws_row
#define ScreenWidth  (screen.width)
#define ScreenHeight (screen.height)

#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a>b)?a:b)

#define MAXANZNUMBERS 18

int terminal_fd=-1;   /* File descriptor for connection to shell */

#ifndef ANDROID
int utf8coding=1;    /* flag if we use utf8 coding*/
#else
extern int utf8coding;    /* flag if we use utf8 coding*/
#endif

#define WRITE_str_to_TERMINAL(a) {int ret=write(terminal_fd,(a),strlen(a)); \
                              if(ret<0) perror("TERMINAL"); \
                              }
#define WRITE_c_to_TERMINAL(a) { char cc=(char)(a);\
                                 int ret=write(terminal_fd,&cc,1); \
                              if(ret<0) perror("TERMINAL"); \
                              }


extern const unsigned short * unicode_pages[];

struct winsize win={25,80,320,240};

static CINFO *textscreen=NULL;

int col=0,lin=0;
int tcolor=YELLOW,tbcolor=BLACK;
int attributes=AT_DEFAULT;
static int fontnr=0;   /* Number of Charackter set (font) */

extern int CharWidth;
extern int CharHeight;

void set_terminal_fd(int fd) {
  terminal_fd=fd;
}
void init_textscreen() {
   if(textscreen)
   textscreen=realloc(textscreen,sizeof(CINFO)*win.ws_col*win.ws_row);
   else textscreen=calloc(win.ws_col*win.ws_row,sizeof(CINFO));
}

static void textscreen_scroll(int target,int source,int num) {
  int i,j;
  if(target<source) {
    for(i=0;i<num;i++) {
      for(j=0;j<LineLen;j++) textscreen[(target+i)*LineLen+j]=
      textscreen[(source+i)*LineLen+j];
    }
  } else if(target>source) {
    for(i=num-1;i>=0;i--) {
      for(j=0;j<LineLen;j++) textscreen[(target+i)*LineLen+j]=textscreen[(source+i)*LineLen+j];
    }
  }
}
static void textscreen_clear(int line,int num){
  int i,j;
  for(i=0;i<num;i++) {
    for(j=0;j<LineLen;j++) textscreen[(line+i)*LineLen+j].c=0;
  }
}


void textscreen_redraw(int x, int y, int w,int h){
  int i,j;
  for(i=y;i<y+h;i++) {
    for(j=x;j<x+w;j++) {
      if(textscreen[i*LineLen+j].c) Fb_BlitCharacter(j*CharWidth,i*CharHeight,
      textscreen[i*LineLen+j].color,textscreen[i*LineLen+j].bcolor, 
      textscreen[i*LineLen+j].c,textscreen[i*LineLen+j].flags,textscreen[i*LineLen+j].fontnr);
      else Fb_BlitCharacter(j*CharWidth,i*CharHeight,
      textscreen[i*LineLen+j].color,textscreen[i*LineLen+j].bcolor,' ',0,0);
    }
  }
}


static void textscreen_lineclear(int lin,int col,int num) {
  int j;
    for(j=col;j<col+num;j++) textscreen[lin*LineLen+j].c=0;
}

static void g_bell() {
  /* beep some sound */
  #ifdef ANDROID
  ANDROID_beep();
  #endif
}

static void g_linefeed() {
      lin++;
      if((lin+1)>AnzLine){
        lin--;
	textscreen_scroll(0,1,lin);
	textscreen_clear(lin,1);
	
        Fb_Scroll(0, CharHeight,lin*CharHeight);  
        Fb_Clear2(lin*CharHeight,CharHeight,tbcolor);
      } 
}
static void g_insertchar() {
  /* Textscreen move_chars */
  memmove(&textscreen[lin*LineLen+col+1],&textscreen[lin*LineLen+col],(LineLen-(col+1))*sizeof(CINFO));
  FB_copyarea(col*CharWidth,lin*CharHeight,ScreenWidth-(col+1)*CharWidth,CharHeight,(col+1)*CharWidth,lin*CharHeight);
  textscreen_lineclear(lin,col,1);
  FillBox(col*CharWidth,lin*CharHeight,CharWidth*1,CharHeight,tbcolor);
}

static void g_terminal_error(unsigned char a,int escflag) {
  char buffer[100];
  if(escflag==0) {
    sprintf(buffer,"Terminal-emulation: ERROR: %d",a);
  } else if(escflag==1) {
    sprintf(buffer,"Terminal-emulation: ERROR: ESC-%d",a);
  } else if(escflag==2) {
    sprintf(buffer,"Terminal-emulation: ERROR: ESC-[-%d",a);
  } else if(escflag==3) {
    sprintf(buffer,"Terminal-emulation: ERROR: ESC-[-?-%d",a);
  } else {
    sprintf(buffer,"Terminal-emulation: ERROR: CODE=%d, flag=%d",a,escflag);
  }
  Fb_BlitText57(0,0,RED,BLACK,buffer);
}

// For crash debugging
char terminal_last_char=0;
int terminal_last_esc=0;


/* Terminalroutines, (c) Markus Hoffmann, all rights reserved */
static void gg_out(unsigned char a) {
  int chw=CharWidth,chh=CharHeight;
  static int escflag=0;
  static int numbers[MAXANZNUMBERS];
  static int anznumbers;
  static int flags=0;
  static int scroll_region_1=0;
  static int scroll_region_2=30;
  static int cursor_saved_x=0,cursor_saved_y=0,cursor_saved_flags=0;
  static int number=0;
 // static int numlock=0;
  static unsigned char utf8_start=0;
  static int utf8_count=0;
  static unsigned short unicode=0;

  terminal_last_char=a;
  terminal_last_esc=escflag;
  
  if(escflag==1) {
    if(a=='c') {   /* Terminal reset */
      escflag=0;
      cursor_onoff(0);
      col=lin=0;
      textscreen_clear(0,AnzLine);
      scroll_region_2=AnzLine;
      scroll_region_1=0;
      cursor_saved_x=cursor_saved_y=0;
      cursor_saved_flags=FL_NORMAL;
      Fb_Clear2(0,ScreenHeight,tbcolor);
      tcolor=LIGHTGREY;tbcolor=BLACK;flags=FL_NORMAL;attributes=AT_DEFAULT;
      cursor_onoff(1); 
    } else if(a=='7') {/* Save cursor-position and attributes */
      cursor_saved_x=col;cursor_saved_y=lin;cursor_saved_flags=flags;
      escflag=0;
    } else if(a=='8') {/* Set cursor an save-pos. and attributes */
      cursor_onoff(0);
      if(cursor_saved_x<LineLen-1) col=cursor_saved_x;
      if(cursor_saved_y<AnzLine-1) lin=cursor_saved_y;
      flags=cursor_saved_flags;
      cursor_onoff(1);
      escflag=0;
    } else if(a=='E') {/* Next Row (CR LF) */
      cursor_onoff(0);
      col=0;g_linefeed();
      cursor_onoff(1);
      escflag=0;
    } else if(a=='M') {/*  Cursor up - at top of region, scroll down */
      cursor_onoff(0);
      if(lin>scroll_region_1) lin--;
      else {
       	textscreen_scroll(scroll_region_1+1,scroll_region_1,scroll_region_2-scroll_region_1-1);
	textscreen_clear(scroll_region_1,1);
        Fb_Scroll((scroll_region_1+1)*chh,scroll_region_1*chh,(scroll_region_2-scroll_region_1-1)*chh);  
        Fb_Clear2(scroll_region_1*chh,chh,tbcolor);
      }
      cursor_onoff(1);
      escflag=0;  
    } else if(a=='D') {/* Cursor down - at bottom of region, scroll up */
      cursor_onoff(0);
      lin++; 
      if((lin+1)>scroll_region_2){
        lin--;
	textscreen_scroll(scroll_region_1,scroll_region_1+1,scroll_region_2-scroll_region_1-1);
	textscreen_clear(lin,1);
        Fb_Scroll(scroll_region_1*chh,(scroll_region_1+1)*chh,(scroll_region_2-scroll_region_1-1)*chh);  
        Fb_Clear2(lin*chh,chh,tbcolor);
      } 
      cursor_onoff(1);
      escflag=0;
    } else if(a=='Z') {/* DECID:Identify Terminal (ANSI mode)*/
      if(terminal_fd!=-1) {WRITE_c_to_TERMINAL(27);WRITE_str_to_TERMINAL("[?6c");}
      escflag=0;
    } else if(a=='=') {/* Keypad application mode */
      // numlock=1;  /* not used */
      escflag=0;
    } else if(a=='<') {/* Exit VT52 mode  */
      escflag=0;
    } else if(a=='>') {/* Keypad numeric mode */
      // numlock=0;  /* not used */
      escflag=0;
    } else if(a=='[') {/* more parameters follow */
      escflag=2;
      number=anznumbers=0;
    } else {
      g_terminal_error(a,escflag);
      escflag=0;
    }
  } else if(escflag>=2) {
    if(a==';') {
      if(anznumbers<MAXANZNUMBERS) numbers[anznumbers++]=number;
      number=0;
    } else if(a=='?') {
      escflag++;
    } else if(a>='0' && a<='9') {
      number=number*10+(a-'0');
    } else {
      if(anznumbers<MAXANZNUMBERS)  numbers[anznumbers++]=number;
      if(a=='m') {
	  int i,f;
	  for(i=0;i<anznumbers;i++) {
	    f=numbers[i];
	    if(f==0) {flags=0;tcolor=LIGHTGREY;tbcolor=BLACK;}
	    else if(f<10) flags|=(1<<(f-1));
	    else if(f==10) fontnr=0; /*Use default font*/
	    else if(f>10 && f<20) fontnr=(f-10); /* use alternate font */
	    else if(f==20) ; /* Use fraktur */
	    else if(f>20 && f<30) flags&=~(1<<(f-21));
	    else if(f==30) tcolor=BLACK;
	    else if(f==31) tcolor=RED;
	    else if(f==32) tcolor=GREEN;
	    else if(f==33) tcolor=YELLOW;
	    else if(f==34) tcolor=BLUE;
	    else if(f==35) tcolor=MAGENTA;
	    else if(f==36) tcolor=LIGHTBLUE;
	    else if(f==37) tcolor=WHITE;
	    else if(f==39) tcolor=LIGHTGREY;  /* set to default */
	    else if(f==40) tbcolor=BLACK;
	    else if(f==41) tbcolor=RED;
	    else if(f==42) tbcolor=GREEN;
	    else if(f==43) tbcolor=YELLOW;
	    else if(f==44) tbcolor=BLUE;
	    else if(f==45) tbcolor=MAGENTA;
	    else if(f==46) tbcolor=LIGHTBLUE;
	    else if(f==47) tbcolor=WHITE;
	    else if(f==49) tbcolor=BLACK;   /* set to default */
	    else if(f==51) flags|=FL_FRAMED;
	    else if(f==54) flags&=~FL_FRAMED;
	    else g_terminal_error((char)f,5);
	  }
      } else if(a=='z') {
	  int i;
	  unsigned char c=(unsigned char)numbers[0];
	  unsigned char *p=&ext_font816[c*CharHeight816];
	  for(i=1;i<anznumbers;i++) {
	    *p++=(unsigned char)numbers[i];
	  }
      } else if(a=='h') {
          if(escflag==3 && anznumbers>0 && numbers[0]==3) { /*Action: Set terminal to 136 column mode */
            change_fontsize(0);
	  } else {  /*Collect attributes*/
	    int i,f;
	    for(i=0;i<anznumbers;i++) {
 	      f=numbers[i];
	      if(f==0) attributes=AT_DEFAULT; 
              else if(f<30) attributes|=(1<<f);
	      else g_terminal_error((char)f,6);
	    }
	  }
      } else if(a=='l') {
          if(escflag==3 && anznumbers>0 && numbers[0]==3) { /*Action: Set terminal to 80 column mode */
            col=min(col,LineLen-1);
            lin=min(lin,AnzLine-1);
            change_fontsize(1);
	  } else {  /*Collect attributes*/
	    int i,f;
	    for(i=0;i<anznumbers;i++) {
 	      f=numbers[i];
	      if(f==0) attributes=AT_DEFAULT; 
              else if(f<30) attributes&=(~(1<<f));
	      else g_terminal_error((char)f,6);
	    }
          }
      } else if(a=='A') { /* cursor up pn times - stop at top */
        cursor_onoff(0);
	lin=max(0,lin-max(numbers[0],1));
	cursor_onoff(1);
      } else if(a=='B') { /* cursor down pn times - stop at bottom */
        cursor_onoff(0);
	lin=min(ScreenHeight/CharHeight-1,lin+max(numbers[0],1));
	cursor_onoff(1);
      } else if(a=='C') { /* cursor right pn times - stop at far right */
        cursor_onoff(0);
	col=min(ScreenWidth/CharWidth-1,col+max(numbers[0],1));
	cursor_onoff(1);
      } else if(a=='D') { /* cursor left pn times - stop at far left */
        cursor_onoff(0);
	col=max(0,col-max(numbers[0],1));
	cursor_onoff(1);
      } else if(a=='P') { /* delete pn characters  */
        int i=min(max(numbers[0],1),ScreenWidth/CharWidth);
        cursor_onoff(0);
	/* Textscreen move_chars */
        memmove(&textscreen[lin*LineLen+col],&textscreen[lin*LineLen+col+i],(LineLen-(col+i))*sizeof(CINFO));
	FB_copyarea((col+i)*chw,lin*chh,ScreenWidth-(col+i)*chw,chh,col*chw,lin*chh);
	textscreen_lineclear(lin,LineLen-i,i);
	FillBox((LineLen-i)*chw,lin*chh,chw*i,chh,tbcolor);
	cursor_onoff(1);	
      } else if(a=='L') { /* insert pn characters(?)  */
        int i=min(max(numbers[0],1),ScreenWidth/CharWidth);
        cursor_onoff(0);
	/* Textscreen move_chars */
        memmove(&textscreen[lin*LineLen+col+i],&textscreen[lin*LineLen+col],(LineLen-(col+i))*sizeof(CINFO));
	FB_copyarea(col*chw,lin*chh,ScreenWidth-(col+i)*chw,chh,(col+i)*chw,lin*chh);
	textscreen_lineclear(lin,col,i);
	FillBox(col*chw,lin*chh,chw*i,chh,tbcolor);
	cursor_onoff(1);
      } else if(a=='M') {                               /* delete pn lines  */
        int i=min(max(numbers[0],1),ScreenHeight/CharHeight-lin);
        cursor_onoff(0);
	textscreen_scroll(lin,lin+i,ScreenHeight/CharHeight-lin-i);
	Fb_Scroll(lin*chh,(lin+i)*chh,(ScreenHeight/CharHeight-lin-i)*chh);
	Fb_Clear2((ScreenHeight/CharHeight-1)*chh,chh,tbcolor);
	textscreen_clear(ScreenHeight/CharHeight-1,1);
	cursor_onoff(1);
      } else if(a=='s') { /* Speichern der Cursorposition */
        cursor_saved_x=col;
        cursor_saved_y=lin;	
      } else if(a=='u') { /* Cursor auf gespeicherte Position setzen */
        cursor_onoff(0);
        if(cursor_saved_x<LineLen-1) col=cursor_saved_x;
        if(cursor_saved_y<AnzLine-1) lin=cursor_saved_y;
	cursor_onoff(1);		
      } else if(a=='r') { /* set scroll region */
	  if(anznumbers>=2) {
	    int i;
   	    for(i=0;i<=anznumbers;i++) {
	      if(i==0) scroll_region_1=min(max(0,numbers[i]-1),AnzLine-1);
	      if(i==1) scroll_region_2=min(max(numbers[i]-1,scroll_region_1+1),AnzLine);
	    }
	  } else {
	    scroll_region_1=0;
	    scroll_region_2=AnzLine;
	  }   
      } else if(a=='H' ||a=='f' ) {
        cursor_onoff(0);
	if(anznumbers==1 && numbers[0]==0) {col=lin=0;}
	else {
	  int i;
	  for(i=0;i<=anznumbers;i++) {
	    if(i==0) lin=min(max(0,numbers[i]-1),ScreenHeight/CharHeight-1);
	    if(i==1) col=min(max(0,numbers[i]-1),ScreenWidth/CharWidth-1);
	  }
	}
	cursor_onoff(1);
      } else if(a=='G') {  /*set cursor horizontal absolute*/
        cursor_onoff(0);
	if(anznumbers>=1 && numbers[0]!=0) {col=numbers[0];}
	cursor_onoff(1);
      } else if(a=='J') {
        cursor_onoff(0);
        if(numbers[0]==0) {
	  textscreen_clear(lin+1,AnzLine-lin-1);
	  Fb_Clear2((lin+1)*chh,ScreenHeight-(lin+1)*chh,tbcolor);	
	} else if(numbers[0]==1) {
	  textscreen_clear(0,lin);
	  Fb_Clear2(0,lin*chh,tbcolor);	
	} else if(numbers[0]==2) {
	  textscreen_clear(0,ScreenHeight/CharHeight);
	  Fb_Clear2(0,ScreenHeight,tbcolor);
	}
	cursor_onoff(1);
      } else if(a=='K') {
        cursor_onoff(0);
        if(numbers[0]==0) {
	  textscreen_lineclear(lin,col,ScreenWidth/CharWidth-col);
	  FillBox(col*chw,lin*chh,ScreenWidth-chw*col,chh,tbcolor);	
	} else if(numbers[0]==1) {
	  textscreen_lineclear(lin,0,col);
	  FillBox(0,lin*chh,chw*col,chh,tbcolor);	
	} else if(numbers[0]==2) {
	  textscreen_lineclear(lin,0,ScreenWidth/CharWidth);
	  Fb_Clear2(lin*chh,(lin+1)*chh,tbcolor);
	}
	cursor_onoff(1);
      } else if(a=='x') {/* ECREQTPARM: Request Terminal Parameters*/
        if(numbers[0]==1) {
	  if(terminal_fd!=-1) {WRITE_c_to_TERMINAL(27);WRITE_str_to_TERMINAL("[3;;8;9600;9600;9;0x");}
	}
      } else if(a=='n') {/* ECREQTPARM: Request Terminal Parameters*/
        if(numbers[0]==5) {
	  if(terminal_fd!=-1) {WRITE_c_to_TERMINAL(27);WRITE_str_to_TERMINAL("[On");}
	} else if(numbers[0]==6) {
	  if(terminal_fd!=-1) {
	    char buffer[64];
	    sprintf(buffer,"\033[%d;%dR",lin+1,col+1); 
            WRITE_str_to_TERMINAL(buffer);
	  }
	}
      } else {
        g_terminal_error(a,escflag);
      }
      escflag=0;
    }
  } else {
    cursor_onoff(0);
    if(a==0) ;
    else if(a==7) g_bell();
    else if(a==8) {if(col) col--;}
    else if(a==9) col=min(((col+8)>>3)<<3,ScreenWidth/CharWidth-1);
    else if(a>=10 && a<=12) {col=0;g_linefeed();} 
    else if(a==13) {
      col=0;
      if(attributes&AT_AUTOLF) g_linefeed();
    } else if(a==14) fontnr=1;  /* Invoke G1 character set. */
    else if(a==15) fontnr=0;    /* Invoke G0 character set. */
    else if(a==16) fontnr=2;    /* Invoke extended character set. */
    else if(a==24 || a==26) escflag=0;  /* CANCEL */
    else if(a==27) escflag++;
    else if(utf8coding && (a&0xc0)>=0x80) {  /*UTF-8 Decoding*/
      if((a&0xc0)!=0x80) { // Startbyte
         if(utf8_start) {
	   printf("UTF-8 ERROR.\n");
	   utf8_start=0;
	 }
	 if(a==0xc0 || a==0xc1 || a>=0xf5) goto insert;    // printf("UTF-8 ERROR.\n");
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
	//   printf("unistart: %d %02x %04x\n",utf8_count,utf8_start,unicode);
      } else {   // follow byte
         if(utf8_start==0 || utf8_count<=0) goto insert;  // printf("UTF-8 ERROR.\n");
	 unicode=(unicode<<6)|(a&0x3f);
	 if(--utf8_count==0) {
	   // printf("uicode: %04x\n",unicode);
	   if(attributes&AT_INSERT) g_insertchar();
	   Fb_BlitUnicodeCharacter(col*chw,lin*chh,tcolor,tbcolor, unicode,flags,fontnr);
           textscreen[lin*LineLen+col].page=(unicode&0xff00)>>8;
           textscreen[lin*LineLen+col].c=unicode&0xff;
	   goto finish;
	 }
      }
    } else {
insert:
      if(attributes&AT_INSERT) g_insertchar();
      Fb_BlitCharacter(col*chw,lin*chh,tcolor,tbcolor, a,flags,fontnr);      
      textscreen[lin*LineLen+col].page=0;
      textscreen[lin*LineLen+col].c=a;
finish:
      utf8_start=0;
      utf8_count=0;
      textscreen[lin*LineLen+col].color=tcolor;
      textscreen[lin*LineLen+col].bcolor=tbcolor;
      textscreen[lin*LineLen+col].flags=flags;
      textscreen[lin*LineLen+col].fontnr=fontnr;
      col++;
      if(col>=LineLen) {
	if(attributes&AT_LINEWRAP) {col=0; g_linefeed();}
        else col=LineLen-1;
      }
    }
    cursor_onoff(1);
  }
}

void gg_outs(char *t){if(t) {FB_hidex_mouse(); while(*t) gg_out(*t++);}}
