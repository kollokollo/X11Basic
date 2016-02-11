/* functions.h Funktionen  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#ifdef WINDOWS
#undef HAVE_LOGB
#undef HAVE_FORK
#endif

#ifdef __CYGWIN__  /* David Andersen  11.10.2003*/
#undef HAVE_LOGB
#endif

/* Standard Funktionstyp */


double f_nop(void *t);

int returntype(int idx, PARAMETER *p,int n);
int returntype2(int idx, char *n);
