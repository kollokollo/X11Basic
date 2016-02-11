/* do_gets.c */

/* Benoetigt readline und curses Libraries  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "defs.h"
#include "x11basic.h"

//#undef HAVE_READLINE

#ifdef HAVE_READLINE
#include <readline/readline.h>
#endif
#ifdef ANDROID
  #include "terminal.h"
#endif


/* **************************************************************** */
/*    Liest einen String ein mit Editiermoeglichkeiten              */
/*                                                                  */
/* **************************************************************** */

/* A static variable for holding the line. */
static char *line_read = (char *)NULL;
#ifndef HAVE_READLINE

#ifdef ANDROID
int utf8coding=1;    /* Flag if we should use UTF-8 coding */
#else
static int utf8coding=0;    /* Flag if we should use UTF-8 coding */
#endif

#define MAX_HISTORY 100
static int historyp=-1;
static char *history[MAX_HISTORY];
void add_history(const char *line) {
  int i;
  if(historyp<0) {
    for(i=0;i<MAX_HISTORY;i++) history[i]=NULL;
    historyp=0;  
  }
  if(historyp<MAX_HISTORY) {
    history[historyp++]=strdup(line);
  } else {
    free(history[0]);
    for(i=0;i<MAX_HISTORY-1;i++) history[i]=history[i+1];
    history[historyp-1]=strdup(line);  
  }
 // printf("Add history: %s --> %d\n",line,historyp);
}

static int strlen_utf8(const char *s) {
   int i = 0, j = 0;
   if(!s) return(0);
   if(!utf8coding) return(strlen(s));
   while (s[i]) {
     if ((s[i] & 0xc0) != 0x80) j++;
     i++;
   }
   return j;
}

void out_line(const char *prompt,const char *edittext, const int cursorpos) {
  int i;
  /* ESC sequenz fuer ganze Zeile l"oschen, cursor an*/
#if 0
  putchar(13);
#else
  putchar(27);   /*   resore cursor position */
  putchar('8');
#endif
  putchar(27);
  printf("[K%s%s ",prompt,edittext);
  putchar(8);
  if(strlen_utf8(edittext)>cursorpos) {
    for(i=0;i<strlen_utf8(edittext)-cursorpos;i++) putchar(8);
  }
  #ifdef ANDROID
    invalidate_screen();
  #else
    fflush(stdout);
  #endif
}
char *readline(char *prompt) {
  int i,cur=0,tcur=0,len=0,escflag=0,hip=0,hisav=0;
  int finish=0;
  char a;
  char *line_read=(char *)malloc(MAXLINELEN+1);
  line_read[0]=0;
  putchar(27);   /*   save cursor position */
  putchar('7');

  out_line(prompt,line_read,cur);
#ifdef ANDROID
  cursor_onoff(1);
#endif
  set_input_mode_echo(0);
  while(!finish) {
    a=getchar();
    if(escflag==2) {
      if(a==51) {  /*DEL Taste*/
        if(len>tcur) {
          len--;
	  for(i=tcur;i<len;i++) line_read[i]=line_read[i+1];
	  line_read[len]=0;
	}
      } else if(a==65) {
	if(!hisav) {
            add_history(line_read);
            hisav=1;
	    
	}
        if(historyp-hip-1>0) hip++;
	if(history[historyp-hip-1]) strcpy(line_read,history[historyp-hip-1]);
	tcur=len=strlen(line_read);
	cur=strlen_utf8(line_read);
      } else if(a==66) {
        if(hip>0) hip--;
	if(history[historyp-hip-1]) strcpy(line_read,history[historyp-hip-1]);
	tcur=len=strlen(line_read);
	cur=strlen_utf8(line_read);
      } else if(a==67) {
       if(tcur<len) {
         tcur++;
	 cur++;
	 if(utf8coding) {while(tcur<len && ((line_read[tcur]&0xc0)==0x80)) tcur++;}
       }
      } else if(a==68) {
        if(cur>0) {
	  cur--;
	  tcur--;
	  if(utf8coding) {while(tcur>0 && ((line_read[tcur]&0xc0)==0x80)) tcur--;}
	}
      } else printf("ESC [ %c %d\n",a,a);
#ifndef _WIN32
      out_line(prompt,line_read,cur);
#endif
      escflag=0;
    } else if(escflag==1) {
      if(a=='[') escflag++;
      else printf("ESC %c %d\n",a,a);
    } else {
      if(a==27) escflag=1;
      else if(a==10 || a==13) finish++;
      else if(a==8 || a==127) {
        if(cur>0) {
          int otcur=tcur;
	  int olen=len;
          cur--;
	  len--;
	  tcur--;
	  if(utf8coding) {
	    while(tcur>0 && ((line_read[tcur]&0xc0)==0x80)) {tcur--; len--;}
	  }
	  for(i=otcur;i<olen;i++) line_read[tcur+i-otcur]=line_read[i];
	  line_read[len]=0;
        }
      } else {
        if(len<MAXLINELEN) {
          if(tcur<len) {
            for(i=len;i>tcur;i--) line_read[i]=line_read[i-1];	  
	  }
	  len++;
          line_read[tcur++]=a;
	  cur++;
	  if(utf8coding) {
	    if((a&0xc0)==0x80) cur--;
	  }
	  line_read[len]=0;
	  hisav=0;
        }
      }
#ifndef _WIN32
      out_line(prompt,line_read,cur);
#endif
    }
  }
#ifndef _WIN32
  puts("");
#endif

#ifdef ANDROID
  cursor_onoff(0);
  invalidate_screen();
#endif

  return(line_read);
}
#else
  extern void add_history(const char *line);
#endif




/* Read a string, and return a pointer to it.  Returns NULL on EOF. 
 *
 *
 */

char *do_gets (char *prompt) {
  static int reenter=0;
  while(reenter) usleep(10000);
  reenter=1;
  /* If the buffer has already been allocated, return the memory
     to the free pool. */
  if(line_read!=(char *)NULL) {
      free(line_read);
      line_read=(char *)NULL;
  }
  set_input_mode_echo(1);
  /* Get a line from the user. */
  line_read = readline (prompt);
  set_input_mode_echo(0);
  /* If the line has any text in it, save it on the history. */
  if (line_read && *line_read) add_history (line_read);
  reenter=0;
  return (line_read);
}




/* This is for non terminal stdin*/

char *simple_gets(char *prompt) {
  char *buffer=malloc(MAXLINELEN);  
  if (line_read != (char *)NULL){
      free(line_read);
      line_read = (char *)NULL;
  }
  set_input_mode_echo(0); /* this is the clou in daemon mode*/
  /* Get a line from the user. */
  fputs(prompt,stdout);
  fflush(stdout);
  line_read = fgets(buffer,MAXLINELEN,stdin);
  if(line_read==NULL) free(buffer);
 
  /* If the line has any text in it, save it on the history. */
  if (line_read && *line_read)
    add_history (line_read);
  return (line_read);
}
