/* PTYPES.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

/* Programm-Typen */

#ifndef _PTYPES_
#define _PTYPES_
#include "vtypes.h"

typedef struct {
  char *name;
  int zeile;
} LABEL;

typedef struct {
  char *name;
  int typ;
  int zeile;
  char *parameterliste;
} PROC;


/* P-Code-Definitionen   long */

/* 0000 0000  0000 0000  0000 0000  0000 0000		
   |			 |||| ||||  |||| ||||
   | 			 |||| ||||  ++++-++++--- Befehlsnr, comms.   00, ff reserviert
   | 			 |||| |||+-------------- Lese PC
   |			 |||| |++--------------- 0: Befehl hat keine Parameter
   | 			 |||| | 		 1: Parameter ueber PCODE.argument
   | 			 |||| | 		 2: Parameter ueber PLISTE
   | 			 |||| | 		 3: reserviert (beides)
   | 			 |||| +----------------- Befehl ignorieren (nixtun)
   | 			 |||+------------------- Befehl illegal
   | 			 ||+-------------------- Label
   | 			 |+--------------------- Procedurdef.
   |||| ||||  |||| ||||  +---------------------- Befehl liegt als Zeichenkette vor, muss geparst werden
   |||| ||||  |||| ||++------------------------- Level: 0 Normal
   |||| ||||  |||| ||					1 Einruecken
   |||| ||||  |||| ||					2 Ausruecken
   |||| ||||  |||| ||					3 Einundausruecken
   |||| ||||  ++++------------------------------ Strukturkommando-Labels 0 - f

   |
   +-------------------------------------------- immer 0					    
					       
   
					       */
#define PM_COMMS     0x000FF  /* Maske fuer Befehlsnr. */
#define P_NOCMD      0x000ff  /* Falls der Pcode kein Kommando darstellt */

#define P_PREFETCH   0x00100  /* PC lesen */

#define PM_TYP       0x00600
#define P_ARGUMENT   0x00200  /* Befehl mit Parameter im P_CODE.argument */
#define P_SIMPLE     0x00000  /* Befehl ohne Parameter */
#define P_PLISTE     0x00400
#define P_IGNORE     0x00800
#define P_INVALID    0x01000
#define P_EVAL       0x08000


#define PM_LEVEL     0x00030000
#define P_LEVELIN    0x00010000
#define P_LEVELOUT   0x00020000
#define P_LEVELINOUT 0x00030000

#define PM_SPECIAL   0x00f3ff00 /* Suchmaske */


#define P_SPECIAL0   0x00000000 /* Labels, wonach gesucht werden kann */
#define P_SPECIAL1   0x00100000
#define P_SPECIAL2   0x00200000
#define P_SPECIAL3   0x00300000
#define P_SPECIAL4   0x00400000
#define P_SPECIAL5   0x00500000
#define P_SPECIAL6   0x00600000
#define P_SPECIAL7   0x00700000


#define P_IF       (P_LEVELIN|P_ARGUMENT|P_SPECIAL0)
#define P_ELSE     (P_LEVELINOUT|P_ARGUMENT|P_IGNORE|P_PREFETCH|P_SPECIAL0)
#define P_ENDIF    (P_LEVELOUT|P_IGNORE|P_SIMPLE|P_SPECIAL0)

#define P_SELECT   (P_LEVELIN |P_ARGUMENT|P_SPECIAL1)
#define P_CASE     (P_LEVELINOUT |P_ARGUMENT|P_SPECIAL1)
#define P_DEFAULT  (P_LEVELINOUT|P_SIMPLE|P_SPECIAL1)
#define P_ENDSELECT (P_LEVELOUT|P_IGNORE|P_SIMPLE|P_SPECIAL1)

#define P_DO       (P_LEVELIN|P_SIMPLE|P_IGNORE|P_SPECIAL2)
#define P_REPEAT   (P_LEVELIN|P_SIMPLE|P_IGNORE|P_SPECIAL3)
#define P_WHILE    (P_LEVELIN|P_ARGUMENT|P_SPECIAL2)
#define P_FOR      (P_LEVELIN|P_ARGUMENT|P_SPECIAL3)

#define P_LOOP     (P_LEVELOUT|P_IGNORE|P_SIMPLE|P_SPECIAL2|P_PREFETCH)
#define P_UNTIL    (P_LEVELOUT|P_ARGUMENT|P_SPECIAL2)
#define P_WEND     (P_LEVELOUT|P_IGNORE|P_SIMPLE|P_SPECIAL3|P_PREFETCH)
#define P_NEXT     (P_LEVELOUT|P_ARGUMENT|P_SPECIAL3)


#define P_DATA     (P_IGNORE|P_ARGUMENT|P_SPECIAL4) 
#define P_BREAK    (P_SIMPLE)
#define P_REM      (P_IGNORE|P_ARGUMENT)


#define P_LABEL    (P_LEVELINOUT|P_IGNORE|P_NOCMD|0x00002000)
#define P_PROC     (P_LEVELIN|P_NOCMD|0x00004000)
#define P_ENDPROC  (P_LEVELOUT|P_SIMPLE)


/* F-Code-Definitionen   long */

/* 0000 0000  0000 0000  0000 0000  0000 0000		
   |			 |||| ||||  |||| ||||
   | 			 |||| ||||  ++++-++++--- Funktionsnr, pfunc.   00, ff reserviert
   |			 |||| ++++-------------- 0: Funktion hat keine Parameter
   | 			 ||||   		 1: nimmt Parameter als Argument-Zeichenkette
   | 			 ||||   		 2: nimmt Parameter als PLISTE
   | 			 ||||   		 3: reserviert
   | 			 ||||   		 4: Alle Parameter sind DOUBLE
   | 			 ||||   		 5: Alle Parameter sind INT
   | 			 ||||   		 6: Alle Parameter sind STRING
   | 			 ||||   		 7: reserviert
   | 			 ||||                    8: Funktion ignorieren (nixtun)
   | 			 |||+------------------- Funktion illegal
   |||| ||||  |||| ||||  ||+-------------------- 1: return int, sonst double
   |+++-++++--++++-++++--+++-------------------- reserviert 
   +-------------------------------------------- immer 0					    
					       
   
					       */
#define FM_COMMS     0x000FF  /* Maske fuer Befehlsnr. */
#define F_NOCMD      0x000ff  /* Falls der Pcode kein Kommando darstellt */


#define FM_TYP       0x00f00
#define F_SIMPLE     0x00000  /* Befehl ohne Parameter */
#define F_ARGUMENT   0x00100  /* Befehl mit Parameter im P_CODE.argument */
#define F_PLISTE     0x00200
#define F_DQUICK     0x00400
#define F_IQUICK     0x00500
#define F_SQUICK     0x00600

#define F_IGNORE     0x00800
#define F_INVALID    0x01000
#define F_IRET       0x02000
#define F_DRET       0x00000



typedef struct {
  short typ;
  int integer;
  double real;
  void *pointer;
} PARAMETER;

typedef struct {
  long opcode;
  int integer;
  short panzahl;
  PARAMETER *ppointer;
  char *argument;
} P_CODE;

typedef struct {
  long opcode;
  char name[20];
  void (*routine)();
  char pmin;        /* Mindestanzahl an Parametern */
  char pmax;        /* Maximal moegliche Anzahl (-1) = beliebig */
  short pliste[12];  /* Liste der Kommandoparametertypen mit pmin Eintraegen */
} COMMAND;

typedef struct {
  long opcode;
  char name[20];
  double (*routine)();
  char pmin;        /* Mindestanzahl an Parametern */
  char pmax;        /* Maximal moegliche Anzahl (-1) = beliebig */
  short pliste[12];  /* Liste der Kommandoparametertypen mit pmin Eintraegen */
} FUNCTION;

typedef struct {
  int opcode;
  char name[20];
  STRING (*routine)();
  char pmin;        /* Mindestanzahl an Parametern */
  char pmax;        /* Maximal moegliche Anzahl (-1) = beliebig */
  short pliste[12];  /* Liste der Kommandoparametertypen mit pmin Eintraegen */
} SFUNCTION;


/* Kommandoparametertypen */

#define PL_EVAL     0
#define PL_LEER     1
#define PL_OPTIONAL 1

#define PL_KEY     2
#define PL_INT     4
#define PL_FLOAT   8
#define PL_NUMBER  (PL_INT|PL_FLOAT)
#define PL_STRING  0x10
#define PL_IARRAY  0x20 
#define PL_FARRAY  0x40
#define PL_SARRAY  0x80
#define PL_NARRAY   (PL_IARRAY|PL_FARRAY)
#define PL_ARRAY   (PL_IARRAY|PL_FARRAY|PL_SARRAY)
#define PL_ADD     (PL_ARRAY|PL_NUMBER|PL_STRING)
#define PL_SUB     (PL_NARRAY|PL_NUMBER)
#define PL_FILENR  0x0100

#define PL_LABEL   0x0200
#define PL_PROC    0x0400
#define PL_FUNC    0x0800
#define PL_IVAR    0x1000
#define PL_FVAR    0x2000
#define PL_SVAR    0x4000
#define PL_NVAR   (PL_IVAR|PL_FVAR)
#define PL_AVAR    0x8000
#define PL_VAR   (PL_SVAR|PL_NVAR)
#define PL_ALL   (PL_VAR|PL_ADD)

#endif
