/* parser.h   Formelparser numerisch und Zeichenketten (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */



int do_parameterliste(char *pos, int *,int);
double parser(char *funktion);
ARRAY array_parser(char *funktion);
STRING string_parser(char *);
char *s_parser(char *funktion);


/*STandard Funktionstyp fuer Parser-Funktionen */

typedef double (*pfunc)();
typedef STRING (*sfunc)();
typedef ARRAY (*afunc)();

