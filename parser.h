/* parser.h   Formelparser numerisch und Zeichenketten (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


int f_instr      (PARAMETER *,int);
int f_rinstr     (PARAMETER *,int);
int f_glob       (PARAMETER *,int);
int f_form_alert (PARAMETER *,int);
int f_form_center(PARAMETER *,int);
int f_form_dial  (PARAMETER *,int);
int f_form_do    (PARAMETER *,int);
int f_objc_draw  (PARAMETER *,int);
int f_objc_offset(PARAMETER *,int);
int f_rsrc_gaddr (PARAMETER *,int);
int f_objc_find  (PARAMETER *,int);
int f_get_color  (PARAMETER *,int);

int do_parameterliste(char *pos, int *,int);
int vergleich(char *w1,char *w2);
double parser(char *funktion);
ARRAY array_parser(char *funktion);
STRING string_parser(char *);
char *s_parser(char *funktion);
double do_funktion(char *name,char *argumente);
STRING do_sfunktion(char *name,char *argumente);
