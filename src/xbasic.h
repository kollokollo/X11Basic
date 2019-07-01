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
#define P_NOCMD         0x000ff  /* Falls der Pcode kein Kommando aus liste darstellt */

#define PM_TYP          0x00700  /*  Maske f"ur Typ des Kommandos, Art der Argumente, Art des Aufrufs*/
#define P_EVAL          0x00000  /* Eval "uber Programmzeile....*/
#define P_ARGUMENT      0x00100  /* Befehl mit Parameter im P_CODE.argument */
#define P_SIMPLE        0x00200  /* Befehl ohne Parameter */
#define P_PLISTE        0x00300  /* Befehl mit Parameterliste */
#define P_IGNORE        0x00400  /*  Befehl ohne effekt...*/
#define P_BYTECODE      0x00500  /*  */

#define PM_FLAGS        0x01800

#define P_PREFETCH      0x00800  /* PC lesen */
#define P_INVALID       0x01000  /*Marker f"ur unverstaendlichen Code*/

  /* Label:          0x00002000  */
  /* Proc:           0x00004000  */
  
#define PM_GROUP        0x0e000
#define P_IFGROUP       0x02000
#define P_SELECTGROUP   0x04000


#define PM_LEVEL     0x00030000
#define P_LEVELIN    0x00010000
#define P_LEVELOUT   0x00020000
#define P_LEVELINOUT 0x00030000

#define PM_SPECIAL   0x00ffff00 /* Suchmaske */


#define P_SPECIAL0   0x00000000 /* Labels, wonach gesucht werden kann */
#define P_SPECIAL1   0x00100000
#define P_SPECIAL2   0x00200000
#define P_SPECIAL3   0x00300000
#define P_SPECIAL4   0x00400000
#define P_SPECIAL5   0x00500000
#define P_SPECIAL6   0x00600000
#define P_SPECIAL7   0x00700000


#define P_IF       (P_LEVELIN|P_PLISTE|P_SPECIAL0)
#define P_ELSE     (P_LEVELINOUT|P_IGNORE|P_PREFETCH|P_SPECIAL0)
#define P_ELSEIF   (P_LEVELINOUT|P_PLISTE|P_PREFETCH|P_SPECIAL4)
#define P_ENDIF    (P_LEVELOUT|P_IGNORE|P_SPECIAL0)

#define P_SELECT   (P_LEVELIN |P_PLISTE|P_SPECIAL1)
#define P_CASE     (P_LEVELINOUT |P_ARGUMENT|P_SPECIAL1)
#define P_DEFAULT  (P_LEVELINOUT|P_SIMPLE|P_SPECIAL1)
#define P_ENDSELECT (P_LEVELOUT|P_IGNORE|P_SPECIAL1)

#define P_DO       (P_LEVELIN|P_IGNORE|P_SPECIAL4)
#define P_REPEAT   (P_LEVELIN|P_IGNORE|P_SPECIAL3)
#define P_WHILE    (P_LEVELIN|P_PLISTE|P_SPECIAL2)
#define P_FOR      (P_LEVELIN|P_ARGUMENT|P_SPECIAL3)

#define P_LOOP     (P_LEVELOUT|P_IGNORE|P_SPECIAL2|P_PREFETCH)
#define P_UNTIL    (P_LEVELOUT|P_PLISTE|P_SPECIAL2)
#define P_WEND     (P_LEVELOUT|P_IGNORE|P_SPECIAL3|P_PREFETCH)
#define P_NEXT     (P_LEVELOUT|P_PLISTE|P_SPECIAL3)

#define P_GOTO     (P_PREFETCH|P_ARGUMENT)
#define P_GOSUB    (P_ARGUMENT|P_SPECIAL5)


#define P_DATA     (P_IGNORE|P_SPECIAL4|P_ARGUMENT) 
#define P_BREAK    (P_PREFETCH|P_IGNORE|P_SPECIAL4)
#define P_EXITIF   (P_PLISTE|P_SPECIAL4)
#define P_REM      (P_IGNORE|P_SPECIAL5|P_ARGUMENT)
#define P_CONTINUE (P_PREFETCH|P_SIMPLE|P_SPECIAL5)
#define P_EXIT     (P_PLISTE|P_SPECIAL6)


#define P_LABEL    (P_LEVELINOUT|P_IGNORE|P_NOCMD|0x00002000)
#define P_PROC     (P_LEVELIN|P_SIMPLE|P_NOCMD|0x00004000)
#define P_ENDPROC  (P_LEVELOUT|P_SIMPLE)
#define P_RETURN   (P_ARGUMENT|P_SPECIAL6)
#define P_DEFFN    (P_LEVELINOUT|P_IGNORE|P_NOCMD|0x00004000)

/*Leerzeile etc...*/
#define P_NOTHING  (P_IGNORE|P_NOCMD)

#define P_ZUWEIS   (P_SPECIAL7|P_NOCMD)
#define P_VOID     (P_SPECIAL7|P_ARGUMENT)


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
#define F_CQUICK     0x00300
#define F_DQUICK     0x00400
#define F_IQUICK     0x00500
#define F_SQUICK     0x00600
#define F_AQUICK     0x00700
#define F_IGNORE     0x00800


/* Return typ */
#define FM_RET       0x0f000

#define F_ANYRET     0x00000
#define F_IRET       0x01000
#define F_DRET       0x02000
#define F_AIRET      0x03000
#define F_AFRET      0x04000
#define F_CRET       0x05000
#define F_ACRET      0x06000
#define F_SRET       0x07000
#define F_ANYIRET    0x08000
#define F_NRET       0x09000
#define F_ARET       0x0a000

#define F_CONST      0x10000





/* Extra typen */

#define PE_NONE 0
#define PE_COMMENT 1

/*Procedure - Typen */

#define PROC_PROC 1
#define PROC_FUNC 2
#define PROC_DEFFN 4



/* Definition einer X11-Basic Zeile*/


typedef struct {
  long opcode;    /* Art des Kommandos P_* */
  int integer;
  short panzahl;  /* Anzahl der Parameter bei ppointer*/
  PARAMETER *ppointer; /*Parameter des kommandos, oder z.B. indizies bei zuweisung*/
  PARAMETER *rvalue;   /*  Bei ZUWEIS, Stage 2 parameter*/
  char *argument;  /*  Argument als nicht aufgelöter Textstring*/
  short atyp;           /* type of the argument */
  short etyp;           /* type of the extra information */
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


extern PARAMETER returnvalue;


/*Variablen*/

extern const int anzpafuncs;
extern const AFUNCTION pafuncs[];
extern P_CODE *pcode;
extern int errorpc,errorpctype,breakpc,breakpctype;
extern int everytime,alarmpc,alarmpctype;

extern int breakcont;
extern int is_bytecode;


/* Deklarationen von Hilfsfunktionen */

void clear_program();
int init_program(int prglen);
char *bytecode_init(char *adr);
// int fix_bytecode_header(BYTECODE_HEADER *bytecode);

char *indirekt2(const char *funktion);
unsigned int type_list(const char *ausdruck);

void free_pcode(int l);
void newprg();

extern int stack_size;
extern VARIABLE **lvar;
extern int *anzlvar;



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
    if(o==w1) f++;
    else if(o==w2) f--;
  }
  return(-1);
}

inline static int sucheloopend(int begin) {
  int i,f=0,o;

  for(i=begin; i<prglen;i++) {
    o=pcode[i].opcode&PM_SPECIAL;
    if((o==P_LOOP || o==P_NEXT || o==P_WEND ||  o==P_UNTIL||  o==P_ENDSELECT)  && f==0)  return(i);
    if(o==P_DO || o==P_FOR || o==P_WHILE ||  o==P_REPEAT||  o==P_SELECT) f++;
    else if(o==P_LOOP || o==P_NEXT || o==P_WEND ||  o==P_UNTIL||  o==P_ENDSELECT) f--;
  }
  return(-1);
}



inline static int procnr(const char *n,int typ) {
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


/* Stack handling functions */

static inline void expand_stack() {
  stack_size+=STACKINCREMENT;
  /* Stack initialisieren */
  stack=realloc(stack,stack_size*sizeof(int));
  anzlvar=realloc(anzlvar,stack_size*sizeof(int));
  for(int i=stack_size-STACKINCREMENT;i<stack_size;i++)
    anzlvar[i]=0; /* Initialisieren */
  lvar=realloc(lvar,stack_size*sizeof(VARIABLE *));
#if DEBUG
  printf("Stacksize changed to: %d (%ld Bytes.)\n",stack_size,
         stack_size*(sizeof(int)*2+sizeof(VARIABLE *)));
#endif
}

/* Makes sure that there is at least one more space for a 
   Stack entry. Otherwise tries to expand the stack.*/

static inline int stack_check(int sp) {
  if(sp<stack_size-1) return(1);
  else if(stack_size<MAXSTACKSIZE) {
    expand_stack();
    return(1);
  }
  return(0);
}
/* Makes sure that there is at least one more space for a 
   procedure entry. Otherwise tries to expand the space.*/

extern int procs_size;
extern PROCEDURE *procs;

static inline void expand_procs() {
  procs_size+=PROCSINCREMENT;
  /* initialisieren */
  procs=realloc(procs,procs_size*sizeof(PROCEDURE));
#if DEBUG
  printf("Procs size changed to: %d (%ld Bytes.)\n",procs_size,
         procs_size*sizeof(PROCEDURE));
#endif
}

static inline int procs_check(int pp) {
  if(pp<procs_size-1) return(1);
  else if(procs_size<MAXANZPROCS) {
    expand_procs();
    return(1);
  }
  return(0);
}

/* Makes sure that there is at least one more space for a 
   label entry. Otherwise tries to expand the space.*/

extern int labels_size;
extern LABEL *labels;

static inline void expand_labels() {
  labels_size+=LABELSINCREMENT;
  /* initialisieren */
  labels=realloc(labels,labels_size*sizeof(LABEL));
#if DEBUG
  printf("Labels size changed to: %d (%ld Bytes.)\n",labels_size,
         labels_size*sizeof(LABEL));
#endif
}

static inline int labels_check(int pp) {
  if(pp<labels_size-1) return(1);
  else if(labels_size<MAXANZLABELS) {
    expand_labels();
    return(1);
  }
  return(0);
}


int original_line(int);
