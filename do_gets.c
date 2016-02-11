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
#include "defs.h"

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
void out_line(const char *prompt,const char *edittext, const int cursorpos) {
  int i;
  /* ESC sequenz fuer ganze Zeile l"oschen, cursor an*/
  putchar(13);
  putchar(27);
  printf("[K%s%s ",prompt,edittext);
  putchar(8);
  if(strlen(edittext)>cursorpos) {
    for(i=0;i<strlen(edittext)-cursorpos;i++) putchar(8);
  }
  #ifdef ANDROID
    invalidate_screen();
  #else
    fflush(stdout);
  #endif
}
char *readline(char *prompt) {
  int i,cur=0,len=0,escflag=0,hip=0,hisav=0;
  int finish=0;
  char a;
  char *line_read=(char *)malloc(MAXLINELEN+1);
  line_read[0]=0;

  out_line(prompt,line_read,cur);
#ifdef ANDROID
  cursor_onoff(1);
#endif
  set_input_mode_echo(0);
  while(!finish) {
    a=getchar();
    if(escflag==2) {
      if(a==51) {  /*DEL Taste*/
        if(len>cur) {
          len--;
	  for(i=cur;i<len;i++) line_read[i]=line_read[i+1];
	  line_read[len]=0;
	  out_line(prompt,line_read,cur);
	}
      } else if(a==65) {
	if(!hisav) {
            add_history(line_read);
            hisav=1;
	    
	}
        if(historyp-hip-1>0) hip++;
	if(history[historyp-hip-1]) strcpy(line_read,history[historyp-hip-1]);
	len=strlen(line_read);
	cur=len;
	out_line(prompt,line_read,cur);
      } else if(a==66) {
        if(hip>0) hip--;
	if(history[historyp-hip-1]) strcpy(line_read,history[historyp-hip-1]);
	len=strlen(line_read);
	cur=len;
	out_line(prompt,line_read,cur);
	
      } else if(a==67) {
       if(cur<len) cur++;
      } else if(a==68) {
        if(cur>0) cur--;
      } else printf("ESC [ %c %d\n",a,a);
      escflag=0;
    } else if(escflag==1) {
      if(a==91) escflag++;
      else printf("ESC %c %d\n",a,a);
    } else {
    
    if(a==27) {
      escflag=1;
    } else if(a==10) {
      finish++;
    } else if(a==8 || a==127) {
      if(cur>0) {
        cur--;
	len--;
	for(i=cur;i<len;i++) line_read[i]=line_read[i+1];
	line_read[len]=0;
      }
    } else if(isprint(a)){
      if(len<MAXLINELEN) {
        if(cur<len) {
          for(i=len;i>cur;i--) line_read[i]=line_read[i-1];	  
	}
	len++;
        line_read[cur++]=a;
	line_read[len]=0;
	hisav=0;
      }
    } else {
      printf("non printable char: %c %d\n",a,a);
    }
    
    out_line(prompt,line_read,cur);
    }
  }
  printf("\n");

#ifdef ANDROID
  cursor_onoff(0);
  invalidate_screen();
#endif

  return(line_read);
}
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
