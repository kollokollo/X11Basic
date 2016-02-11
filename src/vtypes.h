/* VTYPES.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

/* Variablentypen */

#ifndef __vtypes__
#define __vtypes__

#define NOTYP          0
#define INTTYP         1
#define FLOATTYP       2
#define BOOLTYP        3
#define STRINGTYP      4
#define INDIRECTTYP    8
#define ARRAYTYP       16
#define CONSTTYP       32
#define INTARRAYTYP    (INTTYP | ARRAYTYP)
#define FLOATARRAYTYP  (FLOATTYP | ARRAYTYP)
#define STRINGARRAYTYP (STRINGTYP | ARRAYTYP)

typedef struct {
  char *name;
  unsigned int typ;
  int local;
  int opcode;
  double zahl;
  char *pointer;
} VARIABLE;

typedef struct {
  unsigned int typ;
  int dimension;
  void *pointer;
} ARRAY;

typedef struct {
  int len;
  char *pointer;
} STRING;

#endif
