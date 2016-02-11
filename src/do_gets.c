/* do_gets.c */

/* Benoetigt readline und curses Libraries  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include "defs.h"
#ifdef HAVE_READLINE
#include <readline/readline.h>
#endif


/* **************************************************************** */
/*    Liest einen String ein mit Editiermoeglichkeiten              */
/*                                                                  */
/* **************************************************************** */

/* A static variable for holding the line. */
static char *line_read = (char *)NULL;

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char *do_gets (char *prompt) {
  /* If the buffer has already been allocated, return the memory
     to the free pool. */
  if(line_read!=(char *)NULL) {
      free(line_read);
      line_read=(char *)NULL;
  }
#ifdef WINDOWS 
  printf(prompt);
  line_read=malloc(MAXLINELEN);
  gets(line_read);
#else
#ifdef HAVE_READLINE
  set_input_mode_echo(1);
  /* Get a line from the user. */
  line_read = readline (prompt);
  set_input_mode_echo(0);

  /* If the line has any text in it, save it on the history. */
  if (line_read && *line_read)
    add_history (line_read);
#else
  set_input_mode_echo(1);
  printf(prompt);
  fflush(stdout);
  line_read=(char *)malloc(MAXLINELEN);
  fgets(line_read,MAXLINELEN,stdin);
  set_input_mode_echo(0);

#endif
#endif
  return (line_read);
}
char *simple_gets(char *prompt) {
  char *buffer=malloc(MAXLINELEN);  
  if (line_read != (char *)NULL){
      free(line_read);
      line_read = (char *)NULL;
  }
  set_input_mode_echo(0);
  /* Get a line from the user. */
  printf(prompt);
  fflush(stdout);
  line_read = fgets(buffer,MAXLINELEN,stdin);
  if(line_read==NULL) free(buffer);
  /*set_input_mode_echo(0);*/
  /* If the line has any text in it, save it on the history. */
#ifdef HAVE_READLINE
  if (line_read && *line_read)
    add_history (line_read);
#endif
  return (line_read);
}
