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
#define FILENRTYP      0x40

/* Flags f"ur Variablen, bei ABSOLUTE und VAR wird static verwendet, ansonsten sind
die Variablen immer dynamic. Bei STATIC, darf der Speicher, auf den pointer zeigt, 
nicht freigegeben werden.*/

#define V_DYNAMIC 0
#define V_STATIC 1


/* Flags fuer Uebergabeart  (bit wert muss groesser ANZVARS sein !)*/

#define V_BY_VALUE       0x00000
#define V_BY_REFERENCE    0x10000



/*  ARRAY typen */
#define NO_ARRAY          0
#define FULL_ARRAY        2
#define SUB_ARRAY         1

/*Prototypen */

void clear_variable(VARIABLE *);
void erase_variable(VARIABLE *);

int var_exist(const char *name, unsigned char typ,unsigned char subtyp, int l);
int add_variable(const char *name, unsigned char  typ, unsigned char subtyp, unsigned int flags, void *adr);
int variable_exist_type(const char *name );

void *varptr_v(VARIABLE *v);
void *varptr(const char *n);
char *varptr_indexliste(VARIABLE *v,int *indexliste,int n);

int vartype(const char *name);
char *varrumpf(const char *n);
void set_var_adr(int vnr,void *adr);
void local_vars_loeschen(int p);
int zuweisbyindex(int vnr,int *indexliste,double wert);
int zuweisibyindex(int vnr,int *indexliste,int,int wert);
int zuweissbyindex(int vnr,int *indexliste,STRING wert);
void zuweispbyindex(int vnr,int *indexliste,int n,PARAMETER *p);
int zuweis(const char *name, double wert);
int izuweis(const char *name, int wert);
void zuweisxbyindex(int vnr,int *indexliste,int,char *ausdruck);
void feed_subarray_and_free(int vnr,char *pos, ARRAY wert);
int zuweiss(char *name, char *inhalt);
int zuweissbuf(char *name, char *inhalt,int len);
int zuweis_string_and_free(const char *name, STRING inhalt);
void string_zuweis_by_vnr(int vnr, STRING inhalt);
void string_zuweis(VARIABLE *, STRING inhalt);
void xzuweis(const char *name, char *inhalt);


void varcastint(int typ,void *pointer,int val);
void varcastfloat(int typ,void *pointer,double val);
void varcaststring(int typ,void *pointer,STRING val);


STRING create_string(const char *n);
void do_local(int vnr,int sp);
void restore_locals(int sp);


STRING double_string(const STRING *a);


char *dump_var(int);
char *dump_arr(int);



/*kleinere Hilfsfunktionen als inline/Makros*/

/* entferne Variablen.*/ 

#define erase_all_variables() { \
  while(anzvariablen) erase_variable(&variablen[--anzvariablen]); \
}
/*Variablen werden mit 0 oder "" initialisiert. Arrays mit ... 
  entfernt werden die Variablen niemals.*/ 
#define clear_all_variables() { \
  int i=0; \
  while(i<anzvariablen) clear_variable(&variablen[i++]); \
}



static inline void free_string(STRING *str) {
  free(str->pointer);str->pointer=NULL;str->len=0; 
} 
#define restore_all_locals() { \
  while(sp>0) restore_locals(sp--); \
  restore_locals(sp); \
}

static inline const int typlaenge(const int typ) {
  switch(typ) {
  case INTTYP: return(sizeof(int));
  case FLOATTYP: return(sizeof(double));
  case STRINGTYP: return(sizeof(STRING));
  case ARRAYTYP: return(sizeof(ARRAY));
  default: return(0);
  }
}

static inline STRING create_string_and_free(char *n,int l) {
  STRING ergeb={l,n};
  return(ergeb);
}

static inline void varcaststring_and_free(int typ,void *pointer,STRING val) {
  free(((STRING *)pointer)->pointer);
  *((STRING *)pointer)=val;
}



#endif
