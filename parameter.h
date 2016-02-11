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

#define PL_LEER     0x00
#define PL_INT        0x01
#define PL_FLOAT   0x02
#define PL_NUMBER  (PL_INT|PL_FLOAT)
#define PL_STRING  0x04
#define PL_EVAL       0x05
#define PL_KEY        0x06
#define PL_VALUE  (PL_INT|PL_FLOAT|PL_STRING)
#define PL_ARRAY   0x08
#define PL_IARRAY  (PL_ARRAY|PL_INT)    /*9*/
#define PL_FARRAY  (PL_ARRAY|PL_FLOAT)  /*a*/
#define PL_NARRAY   (PL_ARRAY|PL_NUMBER) /*b*/
#define PL_SARRAY  (PL_ARRAY|PL_STRING) /*c*/
#define PL_FILENR 0x0d
#define PL_ANYVALUE 0x0e
#define PL_ANYARRAY   (PL_IARRAY|PL_FARRAY|PL_SARRAY)   /*f*/


#define PL_LABEL   0x10
#define PL_PROC    0x11
#define PL_FUNC    0x12

#define PL_IVAR    0x21
#define PL_FVAR    0x22
#define PL_NVAR   (PL_IVAR|PL_FVAR)
#define PL_SVAR    0x24
#define PL_VAR   (PL_SVAR|PL_NVAR)      /* 0x27 */

#define PL_ARRAYVAR (0x20|PL_ARRAY)  /* 0x28 */

#define PL_IARRAYVAR (0x20|PL_IARRAY)  /* 0x29 */
#define PL_FARRAYVAR (0x20|PL_FARRAY)  /* 0x2a */
#define PL_SARRAYVAR (0x20|PL_SARRAY)  /* 0x2c */
#define PL_ALLVAR (PL_FARRAYVAR|PL_IARRAYVAR|PL_SARRAYVAR)  /* 0x2f */

#define PL_BYTECODE 0x30


/* Zusatztypen */


#define PL_ANYVAR   PL_ALLVAR

#define PL_CONDITION PL_EVAL
#define PL_EXPRESSION PL_EVAL


/*Prototypen */
int count_parameters(char *n);

void free_pliste(int anz,PARAMETER *pret);
void free_parameter(PARAMETER *p);
void dump_parameterlist(PARAMETER *p, int n);



int make_pliste(int pmin,int pmax,unsigned short *pliste,char *n, PARAMETER **pr);
int make_pliste2(int pmin,int pmax,unsigned short *pliste,char *n, PARAMETER **pr,int ii);
int make_pliste3(int pmin,int pmax,unsigned short *pliste,PARAMETER *pin, PARAMETER **pout,int ii);

int make_preparlist(PARAMETER *p,char *n);

#endif

