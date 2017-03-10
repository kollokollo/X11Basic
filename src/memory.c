/* memory.c   Hilfsfunktionen fuer Memory-Management (c) Markus Hoffmann */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * =====================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"
#include "memory.h"


/* Pointerbase: Hier können bis zu 8 verschiedene Speicherbereiche addressiert werden.
 */

#if SIZEOF_VOID_P == 8
void *pointerbase[MAXANZPOINTERBASE]={NULL};
int pointerbasecnt=1;
#endif
