/* XBASIC.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


/* P-Code-Definitionen   long */

/* 0000 0000  0000 0000  0000 0000  0000 0000		
   |			 |||| ||||  |||| ||||
   | 			 |||| ||||  ++++-++++--- Befehlsnr, comms.   00, ff reserviert
   | 			 |||| |||+-------------- Lese PC
   |			 |||| |++--------------- 0: Befehl hat keine Parameter
   | 			 |||| | 		 1: Parameter ueber PCODE.argument
   | 			 |||| | 		 2: Befehl ohne Parameter
   | 			 |||| | 		 3: befehl hat Parameterliste
   | 			 |||| +----------------- Befehl ignorieren (nixtun)
   | 			 |||+------------------- Befehl invalid
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
#define PM_COMMS        0x000FF  /* Maske fuer Befehlsnr. */
#define P_NOCMD         0x000ff  /* Falls der Pcode kein Kommando darstellt */

#define P_PREFETCH      0x00100  /* PC lesen */

#define PM_TYP          0x00600  /*  Maske */
#define P_BYTECODE      0x00000  /* */
#define P_ARGUMENT      0x00200  /* Befehl mit Parameter im P_CODE.argument */
#define P_SIMPLE        0x00400  /* Befehl ohne Parameter */
#define P_PLISTE        0x00600  /* Befehl mit Parameterliste */


#define P_IGNORE        0x00800  /* Ist auch Maske, muss 1 Bit sein! */

#define P_INVALID       0x01000
  /* Label:          0x00002000  */
  /* Proc:           0x00004000  */
  
#define P_EVAL          0x08000  /* 1 Bit Befehl geht ueber den Parser */


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
#define P_ELSEIF   (P_LEVELINOUT|P_ARGUMENT|P_IGNORE|P_PREFETCH|P_SPECIAL4)
#define P_ENDIF    (P_LEVELOUT|P_IGNORE|P_SIMPLE|P_SPECIAL0)

#define P_SELECT   (P_LEVELIN |P_PLISTE|P_SPECIAL1)
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
#define P_NEXT     (P_LEVELOUT|P_PLISTE|P_SPECIAL3)

#define P_GOTO     (P_PREFETCH|P_IGNORE)
#define P_GOSUB    (P_ARGUMENT|P_SPECIAL5)


#define P_DATA     (P_IGNORE|P_ARGUMENT|P_SPECIAL4) 
#define P_BREAK    (P_PREFETCH|P_IGNORE|P_SPECIAL4)
#define P_EXITIF   (P_ARGUMENT|P_SPECIAL4)
#define P_REM      (P_IGNORE|P_ARGUMENT)
#define P_CONTINUE (P_PREFETCH|P_IGNORE|P_SIMPLE|P_SPECIAL5)


#define P_LABEL    (P_LEVELINOUT|P_IGNORE|P_NOCMD|0x00002000)
#define P_PROC     (P_LEVELIN|P_NOCMD|0x00004000)
#define P_ENDPROC  (P_LEVELOUT|P_SIMPLE)
#define P_RETURN   (P_ARGUMENT|P_SPECIAL6)
#define P_DEFFN    (P_LEVELINOUT|P_IGNORE|P_NOCMD|0x00004000)

#define P_ZUWEIS    P_SPECIAL7


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
#define F_AQUICK     0x00700
#define F_IGNORE     0x00800

#define F_INVALID    0x01000
#define F_IRET       0x02000
#define F_DRET       0x00000
#define F_CONST      0x08000





/* Extra typen */

#define PE_NONE 0
#define PE_COMMENT 1

/*Procedure - Typen */

#define PROC_PROC 1
#define PROC_FUNC 2
#define PROC_DEFFN 4






typedef struct {
  long opcode;
  int integer;
  short panzahl;
  PARAMETER *ppointer;
  char *argument;
  short etyp;           /* typ of the extra information */
  void *extra;          /* Pointer to extra Information */
} P_CODE;



typedef struct {
  int opcode;
  char name[20];
  ARRAY (*routine)();
  signed char pmin;        /* Mindestanzahl an Parametern */
  signed char pmax;        /* Maximal moegliche Anzahl (-1) = beliebig */
  unsigned short pliste[12];  /* Liste der Kommandoparametertypen mit pmin Eintraegen */
} AFUNCTION;


typedef union {
  ARRAY a;
  STRING str;
  double f;
  int i;
} UNIVAR;
extern UNIVAR returnvalue;


/*Variablen*/

extern const int anzpafuncs;
extern const AFUNCTION pafuncs[];
extern P_CODE *pcode;
extern int errorpc,errorpctype,breakpc,breakpctype;
extern int everytime,alarmpc,alarmpctype;

extern int breakcont;
extern const char vdate[];
extern const char version[];
extern int is_bytecode;

/*  Keywords */

#define KEYW_UNKNOWN 127
#define KEYW_NONE     0
#define KEYW_ON       1
#define KEYW_OFF      2
#define KEYW_CONT     3
#define KEYW_THEN     4
#define KEYW_MENU     5
#define KEYW_IF       6
#define KEYW_TO       7
#define KEYW_DOWNTO   8
#define KEYW_STEP     9
#define KEYW_USING   10
#define KEYW_ERROR   11
#define KEYW_BREAK   12
#define KEYW_GOTO    13
#define KEYW_GOSUB   14


extern int keyword2num(char *t);
extern const int anzkeywords;
extern const char *keywords[];


/* Deklarationen von Hilfsfunktionen */

void clear_program();
int init_program(int prglen);
char *bytecode_init(char *adr);

char *indirekt2(const char *funktion);

void free_pcode(int l);
unsigned int type(const char *ausdruck);


#ifndef ANDROID
   #define invalidate_screen() ;
#endif

#ifdef GERMAN
#define structure_warning(lin,comment)  {printf("Warnung: Programmstruktur fehlerhaft bei Zeile %d : %s.\n",lin,comment); invalidate_screen();}
#else
#define structure_warning(lin,comment)  {printf("WARNING: corrupt program structure at line %d ==> %s.\n",lin,comment); invalidate_screen();}
#endif





/*Schnelle inline Funktionen */

inline static int suchep(int begin, int richtung, int such, int w1, int w2) {
  int i,f=0,o;

  for(i=begin; (i<prglen && i>=0);i+=richtung) {
    o=pcode[i].opcode&PM_SPECIAL;
    if(o==such && f==0) return(i);
    else if(o==w1) f++;
    else if(o==w2) f--;
  }
  return(-1);
}

inline static int procnr(char *n,int typ) {
  register int i=anzprocs;
  while(--i>=0) {
    if((procs[i].typ&typ) && strcmp(procs[i].name,n)==0) return(i);
  }
  return(-1);
}


inline static int labelnr(char *n) {
  register int i=anzlabels;
  while(--i>=0) {
    if(strcmp(labels[i].name,n)==0) return(i);
  }
  return(-1);
}


inline static int labelzeile(char *n) {
  register int i=anzlabels;
  while(--i>=0) {
    if(strcmp(labels[i].name,n)==0) return(labels[i].zeile);
  }
  return(-1);
}

#define clear_labelliste() { \
  while(anzlabels) free(labels[--anzlabels].name); \
}

#define init_pcode(l) { \
  pcode=(P_CODE *)calloc(l,sizeof(P_CODE)); \
}

#define clear_procliste() { \
  while(anzprocs) { \
    anzprocs--; \
    free(procs[anzprocs].name); \
    free(procs[anzprocs].parameterliste); \
  } \
}






int original_line(int);
