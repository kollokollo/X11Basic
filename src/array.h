/* ARRAY.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include "vtypes.h"

extern double *SVD(double *a, double *w, double *v,int anzzeilen, int anzspalten);
extern double *backsub(double *, double *, double *, double *,int,int);

ARRAY create_array(int , int ,int *);
ARRAY nullmatrix(int , int ,int *);
ARRAY einheitsmatrix(int , int ,int *);
ARRAY array_const(char *);
ARRAY copy_var_array(int);
ARRAY array_info(int);
ARRAY double_array(ARRAY);
ARRAY einheitsmatrix(int, int, int *);
ARRAY nullmatrix(int, int, int *);
ARRAY form_array(int, int, int *, char *);
ARRAY mul_array(ARRAY, ARRAY);
ARRAY trans_array(ARRAY);
ARRAY inv_array(ARRAY);
ARRAY convert_to_floatarray(ARRAY a);
ARRAY convert_to_intarray(ARRAY a);

void free_array(ARRAY arr);
void convert_float_to_int_array(int vnr1, int vnr2);
void convert_int_to_float_array(int vnr1, int vnr2);
void copy_int_array(int vnr1, int vnr2);
void copy_float_array(int vnr1, int vnr2);
void copy_string_array(int vnr1, int vnr2);
void fill_string_array(int vnr, STRING inh);
void erase_string_array(int vnr);

void array_add(ARRAY a1, ARRAY a2);
void array_sub(ARRAY a1, ARRAY a2);
void array_smul(ARRAY a1, double m);
void feed_array_and_free(int, ARRAY);

void fill_int_array(int, int);
void fill_float_array(int, double);
void array_zuweis_and_free(char *, ARRAY);
void free_array(ARRAY);
int anz_eintraege(ARRAY a);
int do_dimension(int vnr);

char *arrptr(char *n);
void *arrayvarptr(int vnr, char *n,int size);
void *arrayvarptr2(int vnr, int *indexliste,int size);
STRING array_to_string(ARRAY inhalt);
ARRAY string_to_array(STRING in);
int make_indexliste(int dim, char *pos, int *index);
