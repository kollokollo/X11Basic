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



ARRAY einheitsmatrix(int , int ,int *);
ARRAY array_const(const char *);
ARRAY double_array(ARRAY *);
ARRAY einheitsmatrix(int, int, int *);

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

STRING array_to_string(ARRAY inhalt);
ARRAY string_to_array(STRING in);
int make_indexliste(int dim, char *pos, int *index);


/*  Kleinere Hilfsfunktionen als inline Makro*/


static inline int anz_eintraege(ARRAY *a) {/* liefert Anzahl der Elemente in einem ARRAY */
  int anz=1,j;
  for(j=0;j<a->dimension;j++) anz=anz*((int *)a->pointer)[j];
  return(anz);
}

static inline int do_dimension(VARIABLE *v) {  /* liefert Anzahl der Elemente in einem ARRAY */
 /*  printf("DODIM?: vnr=%d \n",vnr); */
  if(v->typ & ARRAYTYP) return(anz_eintraege(v->pointer.a));
  else return(1);
}


static inline ARRAY nullmatrix(int typ, int dimension, int *dimlist) {
  ARRAY ergebnis;
  if(typ & INTTYP)        ergebnis=create_int_array(dimension,dimlist,0);
  else if(typ & FLOATTYP) ergebnis=create_float_array(dimension,dimlist,0);
  else                    ergebnis=create_array(typ,dimension,dimlist);
  return(ergebnis);
}

