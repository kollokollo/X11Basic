/* memory.c   helper functions for memory management (c) Markus Hoffmann */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * =====================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "memory.h"


/* This just defines a table of pointers used as 
 * Pointerbase for translating 64bit pointers to 32bit numbers: 
 * You can address up to 8 different memory areas.
 */

#if SIZEOF_VOID_P == 8
void *pointerbase[MAXANZPOINTERBASE]={NULL};
int pointerbasecnt=1;
#endif
