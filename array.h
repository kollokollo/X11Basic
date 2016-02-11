/* ARRAY.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


ARRAY create_array(const int , const int ,const int *);
ARRAY recreate_array(const int , const int ,const int *, const ARRAY *);
ARRAY create_int_array(const int dimension,const  int *dimlist,const int value);
ARRAY create_float_array(const int dimension, const int *dimlist,const double value);
ARRAY create_string_array(const int dimension, const int *dimlist,const STRING *value);


ARRAY einheitsmatrix(const int ,const int ,const int *);
ARRAY array_const(const char *);
ARRAY double_array(const ARRAY *);
ARRAY einheitsmatrix(const int, const int, const int *);

ARRAY mul_array(ARRAY, ARRAY);
ARRAY trans_array(ARRAY);
ARRAY inv_array(ARRAY);
ARRAY convert_to_floatarray(const ARRAY *a);
ARRAY convert_to_intarray(const ARRAY *a);
ARRAY get_subarray(ARRAY *arr,int *indexliste);

void array_add(ARRAY a1, ARRAY a2);
void array_sub(ARRAY a1, ARRAY a2);
void array_smul(ARRAY a1, double m);
void feed_array_and_free(int, ARRAY);
void free_array(ARRAY *arr);

/* Operieren auf Variablen */

void fill_string_array (const ARRAY *,const STRING );
void fill_int_array    (const ARRAY *,const int);
void fill_float_array  (const ARRAY *,const double);

STRING array_to_string(ARRAY inhalt);
ARRAY string_to_array(STRING in);
int make_indexliste(int dim, char *pos, int *index);
int check_indexliste(const ARRAY *a, const int *idx);
void anz2idx(int anz,const ARRAY *a,int *idx);


int    int_array_element   (const ARRAY *a, int *idx);
double float_array_element (const ARRAY *a, int *idx);
STRING string_array_element(const ARRAY *a, int *idx);
ARRAY  array_array_element (const ARRAY *a, int *idx);


double array_det(const ARRAY *a);

/*  Kleinere Hilfsfunktionen als inline Makro*/


static inline int anz_eintraege(const ARRAY *a) {/* liefert Anzahl der Elemente in einem ARRAY */
  int anz=1,j;
  if(a->dimension>0 && a->pointer) {
    for(j=0;j<a->dimension;j++) anz=anz*((int *)a->pointer)[j];
  }
  return(anz);
}

static inline int do_dimension(const VARIABLE *v) {  /* liefert Anzahl der Elemente in einem ARRAY */
 /*  printf("DODIM?: vnr=%d \n",vnr); */
  if(v->typ & ARRAYTYP) return(anz_eintraege(v->pointer.a));
  else return(1);
}


static inline ARRAY nullmatrix(const int typ, const int dimension, const int *dimlist) {
  ARRAY ergebnis;
  if(typ & INTTYP)        ergebnis=create_int_array(dimension,dimlist,0);
  else if(typ & FLOATTYP) ergebnis=create_float_array(dimension,dimlist,0);
  else                    ergebnis=create_array(typ,dimension,dimlist);
  return(ergebnis);
}

