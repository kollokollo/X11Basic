/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

/* Variablentypen */

#ifndef __variablen__
#define __variablen__

/* Variablen Typen (unsigned char)*/

#define NOTYP             0
#define INTTYP            1
#define FLOATTYP          2
#define STRINGTYP         4
#define INDIRECTTYP       8
#define ARRAYTYP       0x10
#define CONSTTYP       0x20
#define FILENRTYP       0x40


/*Prototypen */

void clear_variable(VARIABLE *);
void clear_all_variables();
void erase_variable(VARIABLE *);
void erase_all_variables();

int var_exist(char *name, unsigned char typ,unsigned char subtyp, int l);
int add_variable(char *name, unsigned char  typ, unsigned char subtyp);
int add_variable_adr(char *name, unsigned char  typ, char *adr);
int variable_exist_type(char *name );

void *varptr_v(VARIABLE *v);
void *varptr(char *n);
char *varptr_indexliste(VARIABLE *v,int *indexliste,int n);

int vartype(char *name);
char *varrumpf(char *n);
char *argument(char *n);

void local_vars_loeschen(int p);
inline void free_string(STRING *str);
int zuweisbyindex(int vnr,int *indexliste,double wert);
int zuweisibyindex(int vnr,int *indexliste,int,int wert);
int zuweissbyindex(int vnr,int *indexliste,STRING wert);
int zuweis(char *name, double wert);
void zuweisxbyindex(int vnr,int *indexliste,int,char *ausdruck);
void feed_subarray_and_free(int vnr,char *pos, ARRAY wert);
int zuweiss(char *name, char *inhalt);
int zuweissbuf(char *name, char *inhalt,int len);
int zuweis_string_and_free(char *name, STRING inhalt);
void string_zuweis_by_vnr(int vnr, STRING inhalt);
void string_zuweis(VARIABLE *, STRING inhalt);
void xzuweis(char *name, char *inhalt);
void c_dolocal(int vnr, char *inhalt);
double arrayinhalt(char *name, char* index);
double floatarrayinhalt2(ARRAY *a, int *indexliste);
double varfloatarrayinhalt(int vnr, int *indexliste);
int intarrayinhalt2(ARRAY *a, int *indexliste);
int varintarrayinhalt(int vnr, int *indexliste);
STRING varstringarrayinhalt(int vnr, int *indexliste);

double floatarrayinhalt(int vnr, char* index);
int intarrayinhalt(int vnr, char* index);

int typlaenge(int typ);
void varcastint(int typ,void *pointer,int val);
void varcastfloat(int typ,void *pointer,double val);
void varcaststring(int typ,void *pointer,STRING val);
void varcaststring_and_free(int typ,void *pointer,STRING val);

STRING create_string(char *n);
void do_local(int vnr,int sp);
void restore_locals(int sp);
STRING create_string_and_free(char *n,int l);
STRING double_string(STRING *a);

#endif

