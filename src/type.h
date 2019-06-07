/* type.h Funktionen  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
unsigned int type_(const char *ausdruck);
unsigned int type(const char *ausdruck);
unsigned int combine_type(unsigned int rtyp,unsigned int ltyp,char c);
int returntype(int idx, PARAMETER *p,int n);
int returntype2(int idx, char *n);

char *type_name(unsigned int typ);
