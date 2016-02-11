/* ARRAY.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


ARRAY create_array(int , int ,int *);
ARRAY create_int_array(int dimension, int *dimlist,int value);
ARRAY create_float_array(int dimension, int *dimlist,double value);
ARRAY create_string_array(int dimension, int *dimlist,STRING *value);


ARRAY nullmatrix(int , int ,int *);
ARRAY einheitsmatrix(int , int ,int *);
ARRAY array_const(char *);
ARRAY double_array(ARRAY *);
ARRAY einheitsmatrix(int, int, int *);
ARRAY nullmatrix(int, int, int *);
ARRAY mul_array(ARRAY, ARRAY);
ARRAY trans_array(ARRAY);
ARRAY inv_array(ARRAY);
ARRAY convert_to_floatarray(ARRAY *a);
ARRAY convert_to_intarray(ARRAY *a);
ARRAY get_subarray(ARRAY *arr,int *indexliste);

void array_add(ARRAY a1, ARRAY a2);
void array_sub(ARRAY a1, ARRAY a2);
void array_smul(ARRAY a1, double m);
void feed_array_and_free(int, ARRAY);
void free_array(ARRAY *arr);

/* Operieren auf Variablen */

void fill_string_array (ARRAY *,STRING );
void fill_int_array    (ARRAY *,int);
void fill_float_array  (ARRAY *,double);
int do_dimension       (VARIABLE *v);
int anz_eintraege(ARRAY *a);

void *arrayvarptr(int vnr, char *n,int size);
void *arrayvarptr2(int vnr, int *indexliste,int size);
STRING array_to_string(ARRAY inhalt);
ARRAY string_to_array(STRING in);
int make_indexliste(int dim, char *pos, int *index);
void make_indexliste_plist(int dim, PARAMETER *p, int *index);



