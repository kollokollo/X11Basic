/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef __parameter__
#define __parameter__

/* Kommandoparametertypen (unsigned char )*/

/* Basistypen */

#define PL_BASEMASK 0x0f

#define PL_LEER     0x00                /* 0 */
#define PL_INT      0x01                /* 1 */
#define PL_FLOAT    0x02                /* 2 */
#define PL_NUMBER   (PL_INT|PL_FLOAT)   /* 3  ausser array*/
#define PL_STRING   0x04                /* 4 */
#define PL_EVAL     0x05                /* 5 */
#define PL_KEY      0x06                /* 6 */
#define PL_VALUE    (PL_INT|PL_FLOAT|PL_STRING)  /* 7 */
#define PL_ARRAY    0x08                /* 8 */
#define PL_IARRAY   (PL_ARRAY|PL_INT)   /* 9 */
#define PL_FARRAY   (PL_ARRAY|PL_FLOAT) /* a */
#define PL_NARRAY   (PL_ARRAY|PL_NUMBER) /*b */
#define PL_SARRAY   (PL_ARRAY|PL_STRING) /*c */
#define PL_FILENR   0x0d                /* d */
#define PL_ANYVALUE 0x0e                /* e alles ausser array*/
#define PL_ANYARRAY (PL_IARRAY|PL_FARRAY|PL_SARRAY) /* f */

#define PL_LABEL    0x10                /* 0x10 = 16 */
#define PL_PROC     0x11                /* 0x11 = 17 */
#define PL_FUNC     0x12                /* 0x12 = 18 */

#define PL_IVAR     0x21                /* 0x21 = 33 */
#define PL_FVAR     0x22                /* 0x22 = 34 */
#define PL_NVAR     (PL_IVAR|PL_FVAR)   /* 0x23 = 35 */
#define PL_SVAR     0x24                /* 0x24 = 36 */

#define PL_VAR      (PL_SVAR|PL_NVAR)   /* 0x27 */

#define PL_ARRAYVAR (0x20|PL_ARRAY)  /* 0x28 */

#define PL_IARRAYVAR (0x20|PL_IARRAY)  /* 0x29 */
#define PL_FARRAYVAR (0x20|PL_FARRAY)  /* 0x2a */
#define PL_SARRAYVAR (0x20|PL_SARRAY)  /* 0x2c */
#define PL_ALLVAR (PL_FARRAYVAR|PL_IARRAYVAR|PL_SARRAYVAR)  /* 0x2f */

/* Weitere Parameter-Arten */

#define PL_BYTECODE  0x30     /* Zeigt auf auszufuehrenden Bytecode*/
#define PL_SPECIAL   0x31     /*Typ wird noch aufgeloest B:W:D:V:*/
#define PL_PRINTLIST 0x32     /*anytyp mit delimiter*/
#define PL_ANY       0x33     /*anything, typ wird bestimmt, kann float,int,string, array sein*/
#define PL_ANYNUM    0x34     /*anything, typ wird bestimmt, kann float,int, oder num array sein*/


/* abgeleitete Zusatztypen */


#define PL_ANYVAR     PL_ALLVAR
#define PL_CONDITION  PL_EVAL
#define PL_EXPRESSION PL_EVAL


/*Prototypen */
int count_parameters(const char *n);
void free_parameter(PARAMETER *p);
void dump_parameterlist(PARAMETER *p, int n);

void prepare_vvar(char *w1,PARAMETER *p, unsigned int solltyp);


int make_pliste(int pmin,int pmax,unsigned short *pliste,char *n, PARAMETER **pr);
int make_pliste2(int pmin,int pmax,unsigned short *pliste,char *n, PARAMETER **pr,int ii);
int make_pliste3(int pmin,int pmax,unsigned short *pliste,PARAMETER *pin, PARAMETER **pout,int ii);

int make_preparlist(PARAMETER *p,char *n);
int get_indexliste(PARAMETER *p,int *l,int n);

/*kleinere Hilfsfunktionen als inline Makros*/

static inline void free_pliste(int anz,PARAMETER *p) {
  while(anz>0)  free_parameter(&p[--anz]);
  free(p);
}

void make_indexliste_plist(int dim, PARAMETER *p, int *index);

#endif

