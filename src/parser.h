/* parser.h   Formelparser numerisch und Zeichenketten (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */



int do_parameterliste(const char *pos, const int *,int);
int call_sub_with_parameterlist(int procnr,PARAMETER *plist,int anzpar);

double parser(const char *funktion);
ARRAY array_parser(const char *funktion);
STRING string_parser(const char *);
COMPLEX complex_parser(const char *);
void arbint_parser(const char *,ARBINT);
char *s_parser(const char *funktion);

STRING vs_error();
