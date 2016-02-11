
/* x11basic.h (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef __X11BASIC
#define __X11BASIC

/*  Allgemeine Konstanten */

#ifndef FALSE
  #define FALSE    0
  #define TRUE     (!FALSE)
#endif
#define PI       3.141592653589793
#define E        2.718281828459

/*Variablen Typen*/

typedef struct {
  int len;
  char *pointer;
} STRING;

typedef struct {
  int dimension;
  void *pointer;
  unsigned short typ;
} ARRAY;

typedef struct {
  unsigned int typ;
  char *name;
  union {
    int *i;
    double *f;
    STRING *s;
    ARRAY *a;
  } pointer;
  int local;
} VARIABLE;

/* X11Basic strukturen, welche f"ur lib aufrufe benoetigt werden.*/

struct PARAMETER {
  unsigned short typ;
  double real;
  int integer;     /*   das kann auch als STRING pointer und ARRAY pointer dienen*/
  void *pointer;
  unsigned short arraytyp; /* reserviert, damit man auch ARRAY pointer machen kann*/
  short panzahl;
  struct PARAMETER *ppointer;   /*Unterparameter wie indizies */
};
typedef struct PARAMETER PARAMETER;

typedef struct {
  long opcode;
  char name[20];
  void (*routine)();
  signed char pmin;        /* Mindestanzahl an Parametern */
  signed char pmax;        /* Maximal moegliche Anzahl (-1) = beliebig */
  unsigned short pliste[12];  /* Liste der Kommandoparametertypen mit pmin Eintraegen */
} COMMAND;

typedef struct {
  long opcode;
  char name[20];
  double (*routine)();
  signed char pmin;        /* Mindestanzahl an Parametern */
  signed char pmax;        /* Maximal moegliche Anzahl (-1) = beliebig */
  unsigned short pliste[12];  /* Liste der Kommandoparametertypen mit pmin Eintraegen */
} FUNCTION;

typedef struct {
  long opcode;
  char name[20];
  STRING (*routine)();
  signed char pmin;        /* Mindestanzahl an Parametern */
  signed char pmax;        /* Maximal moegliche Anzahl (-1) = beliebig */
  unsigned short pliste[12];  /* Liste der Kommandoparametertypen mit pmin Eintraegen */
} SFUNCTION;

typedef struct {
  int opcode;
  char name[20];
  double (*routine)();
} SYSVAR;

typedef struct {
  int opcode;
  char name[20];
  STRING (*routine)();
} SYSSVAR;
 
typedef struct {
  char *name;        /*Name des Labels*/
  int zeile;         /*Programmzeielennummer (fuer Goto) */
  unsigned long int datapointer; /*Pointer zum datenbereich (fuer Restore) */
} LABEL;

typedef struct {
  char *name;       /*Name der Procedure (incl Rueckgabetyp)*/
  int typ;          /* 1=PROC 2=FUNC 4=DEFFN */
  int zeile;
  int anzpar;
  int *parameterliste;  /* Liste mit Variablennummern*/
} PROCEDURE;

/* Globale Variablen */

extern int databufferlen;
extern char *databuffer;
extern const char libversion[];
extern const char libvdate[];
extern int param_anzahl;
extern char **param_argumente;
extern const char xbasic_name[];
extern int pc,sp,err,errcont,everyflag,batch,echoflag;
extern char ifilename[];
extern int stack[];
extern int datapointer;

extern const int anzsysvars;
extern const SYSVAR sysvars[];
extern const int anzsyssvars;
extern const SYSSVAR syssvars[];

extern const int anzcomms;
extern const COMMAND comms[];
extern const int anzpfuncs;
extern const FUNCTION pfuncs[];
extern const int anzpsfuncs;
extern const SFUNCTION psfuncs[];

extern int anzvariablen;
extern VARIABLE variablen[];

extern int anzlabels, anzprocs;
extern PROCEDURE  procs[];
extern LABEL labels[];

extern int filenr[];
extern FILE *dptr[];
extern int ccs_err;
/* Variablen, welche ausserhalb der lib definiert werden muessen */

extern int prglen;
extern int runfile,daemonf;
extern int programbufferlen;
extern char *programbuffer;
extern char *program[];



/* Prototypen  fuer die Libx11basic.s	   */



const char *error_text(unsigned char errnr, char *bem);
void xberror(char errnr, char *bem);
void reset_input_mode();
void x11basicStartup();

void kommando(char *);   /* This is for single command execution: */
void loadprg(char *);
int mergeprg(char *fname);

void programmlauf();

void do_run();
void do_help(char *);
void quit_x11basic(int c);
double parser(char *funktion);

/* Hilfsfunktionen */

int find_comm(char *);
int find_func(char *);
int find_afunc(char *);
int find_sfunc(char *);
int count_parameter(char *);

void break_handler( int);
void doocssig_handler( int);

void fatal_error_handler( int);
void timer_handler( int);
double ltext(int, int, double, double, double , int, char *);
char *do_gets (char *); 
char *simple_gets(char *);
double floatarrayinhalt(int, char*);
double varfloatarrayinhalt(int, int *);
int varintarrayinhalt(int, int *);
char *indirekt2(char *);
void local_vars_loeschen(int);
void clear_parameters();
void clear_labelliste();
void clear_procliste();
char *varrumpf(char *);
int f_freefile();
char *bin_to_string(char *);
void set_input_mode(int,int);
void set_input_mode_echo(int);
void reset_input_mode();
char *terminalname();
int kbhit();


double ltextlen (double, int, char *); 
void cs_init();
void cs_exit();
double csget(char *);
char *csgets(char *);
double do_funktion(char *,char *);


void do_menu_draw();
int  do_menu_select();

void set_fill(int);
 
void activate();
void next_data_line();
void handle_window(int);
void programmlauf();
void set_font(char *);
void set_graphmode(int);

/* Kontrollsystem */

int cssize(char *);
int cstyp(char *);
int cspid(char *);
char *cspname(int);
char *csunit(char *);
double csget(char *);
double csmin(char *);
double csmax(char *);
double csres(char *);
double tineget(char *);
double tinemin(char *);
double tinemax(char *);
#ifdef CONTROL
void c_csput(char *);
void c_csvput(char *);
void c_csset(char *);
void c_cssweep(char *);
void c_csclearcallbacks(char *);
void c_cssetcallback(char *);
void sweep_value(int,float,float,float,int);
#endif
#ifdef TINE
char *tineunit(char *n);
char *tineinfo(char *n);
void c_tinebroadcast(char *);
void c_tinelisten(char *);
void c_tinedeliver(char *);
void c_tineexport(char *);
void c_tinemonitor(PARAMETER *,int);
void c_tineserver(PARAMETER *,int);
void c_tinecycle(char *);
void c_tineput(char *);
STRING tinegets(char *);
STRING tinequery(char *,int );
STRING f_tineunits(STRING n);
#endif
#ifdef DOOCS
ARRAY doocsvget(char *n,int nn, int o);
ARRAY doocshistory(char *n,int start, int stop);
void c_doocscallback(char *);
void c_doocsexport(char *);
void c_doocsput(char *);
void c_doocsserver(PARAMETER *,int);
STRING doocsgets(char *);
STRING doocsinfos(char *);
double doocsget(char *);
double doocstimestamp(char *);
int doocstyp(char *);
int doocssize(char *);
#endif

/* API for The virtual machine */

int pusharg(PARAMETER **opstack, char *typ,...);
int callifunc(PARAMETER **opstack,void (*name)(),char *typ,...);
double callfunc(PARAMETER **opstack,void (*name)(),char *typ,...);
STRING callsfunc(PARAMETER **opstack,void (*name)(),char *typ,...);
ARRAY callafunc(PARAMETER **opstack,void (*name)(),char *typ,...);


#endif


