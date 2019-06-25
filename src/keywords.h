/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef __keywords__
#define __keywords__

/*  Keywords */

#define KEYW_UNKNOWN 127
#define KEYW_NONE     0
#define KEYW_ON       1
#define KEYW_OFF      2
#define KEYW_CONT     3
#define KEYW_THEN     4
#define KEYW_MENU     5
#define KEYW_IF       6
#define KEYW_TO       7
#define KEYW_DOWNTO   8
#define KEYW_STEP     9
#define KEYW_USING   10
#define KEYW_ERROR   11
#define KEYW_BREAK   12
#define KEYW_GOTO    13
#define KEYW_GOSUB   14

extern int keyword2num(char *t);
extern const int anzkeywords;
extern const char *keywords[];
#endif
