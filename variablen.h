/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#include "vtypes.h"
#include "ptypes.h"

int vartype(char *name);
char *varrumpf(char *n);
char *argument(char *n);
void *varptr(char *n);
void erase_all_variables();
void local_vars_loeschen(int p);
void free_pliste(int anz,PARAMETER *pret);
void free_parameter(PARAMETER p);
void free_string(STRING str);
int variable_exist_type(char *name );
int variable_exist(char *name, int typ);
int variable_exist_or_create(char *name, int typ);
int zuweisbyindex(int vnr,int *indexliste,double wert);
int zuweisibyindex(int vnr,int *indexliste,int wert);
int zuweissbyindex(int vnr,int *indexliste,STRING wert);
int zuweis(char *name, double wert);
void zuweisi(char *name, int wert);
int neue_float_variable(char *name, double wert, int sp);
int neue_int_variable(char *name, int wert, int sp);
int neue_string_variable(char *name, STRING wert, int sp);
int neue_array_variable_and_free(char *name, ARRAY wert, int sp);
void feed_subarray_and_free(int vnr,char *pos, ARRAY wert);
int zuweiss(char *name, char *inhalt);
int zuweissbuf(char *name, char *inhalt,int len);
int zuweis_string(char *name, STRING inhalt);
void string_zuweis_by_vnr(int vnr, STRING inhalt);
void xzuweis(char *name, char *inhalt);
void c_dolocal(char *name, char *inhalt);
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
STRING create_string(char *n);


