/* do_gets.c */

/* Benoetigt readline und termcap Libraries  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <readline/readline.h>



/* **************************************************************** */
/*    Liest einen String ein mit editiermoeglichkeiten              */
/*                                                                  */
/* **************************************************************** */

/* A static variable for holding the line. */
static char *line_read = (char *)NULL;

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char *do_gets (char *prompt)
{
  /* If the buffer has already been allocated, return the memory
     to the free pool. */
  if (line_read != (char *)NULL)
    {
      free (line_read);
      line_read = (char *)NULL;
    }
  set_input_mode_echo(1);
  /* Get a line from the user. */
  line_read = readline (prompt);
  set_input_mode_echo(0);

  /* If the line has any text in it, save it on the history. */
  if (line_read && *line_read)
    add_history (line_read);

  return (line_read);
}


/* **************************************************************** */
/*                                                                  */
/*        Writing a Function to be Called by Readline.              */
/*                                                                  */
/* **************************************************************** */

/* Invert the case of the COUNT following characters. */
invert_case_line (count, key)
     int count, key;
{
  register int start, end, direction;

  start = rl_point;

  if (count < 0)
    {
      direction = -1;
      count = -count;
    }
  else
    direction = 1;
      
  /* Find the end of the range to modify. */
  end = start + (count * direction);

  /* Force it to be within range. */
  if (end > rl_end)
    end = rl_end;
  else if (end < 0)
    end = -1;

  if (start > end)
    {
      int temp = start;
      start = end;
      end = temp;
    }

  if (start == end)
    return;

  /* Tell readline that we are modifying the line, so save the undo
     information. */
  rl_modifying (start, end);

  for (; start != end; start += direction)
    {
      if (isupper (rl_line_buffer[start]))
	rl_line_buffer[start] = tolower (rl_line_buffer[start]);
      else if (islower (rl_line_buffer[start]))
	rl_line_buffer[start] = toupper (rl_line_buffer[start]);
    }

  /* Move point to on top of the last character changed. */
  rl_point = end - direction;
}

