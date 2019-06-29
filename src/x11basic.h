
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

#ifdef HAVE_GMP
  #include <gmp.h>
  #define ARBINT mpz_t
  #define ARBFLOAT mpf_t
  typedef struct {
    ARBFLOAT r;
    ARBFLOAT i;
  }   ARBCOMPLEX;
#elif defined HAVE_MINI_GMP
  /*MINI_GMP ist nur ein schlechter ersatz, weil es viele Funktionen nicht 
    enhaelt. Hierfür wird hier ein Dummy definiert. */
  #include <mini-gmp.h>
  #define ARBINT mpz_t
  #define ARBFLOAT double
  typedef struct {
    ARBFLOAT r;
    ARBFLOAT i;
  } ARBCOMPLEX;
  #define gmp_randstate_t long
  #define gmp_randinit_default(a) srand(a)
  #define gmp_randinit_ui(a,b) srand(a)
  #define gmp_randseed_ui(a,b) srand(a=b)
  #define mpz_urandomm(a,b,c)  mpz_set_si(a,rand())
  #define mpz_div(a,b,c) mpz_set_si(a,mpz_get_si(b)/mpz_get_si(c))
  #define mpz_get_str(a,b,c) strdup("not supported")
  #define gmp_printf printf
  // TODO
#else
#define ARBINT int
#define ARBFLOAT double
#define ARBCOMPLEX COMPLEX
#define mpz_init(a) ;
#define mpz_init_set(a,b) a=b
#define mpz_clear(a) a=0
#define mpz_sub(a,b,c) a=b-(c)
#define mpz_sub_ui(a,b,c) a=b-(c)
#define mpz_add(a,b,c) a=b+c
#define mpz_add_ui(a,b,c) a=b+c
#define mpz_mul(a,b,c) a=(b)*(c)
#define mpz_div(a,b,c) a=(b)/(c)
#define mpz_mod(a,b,c) a=(b)%(c)
#define mpz_bin_uiui(a,b,c) a=0
#define mpz_mul_si(a,b,c) a=(b)*(c)
#define mpz_addmul(a,b,c) a+=(b)*(c)
#define mpz_root(a,b,c) a=0
#define mpz_fac_ui(a,b) a=1
#define mpz_neg(a,b) a=-b
#define mpz_sqrt(a,b) a=(int)sqrt((double)b)
#define mpz_set(a,b) a=b
#define mpz_set_d(a,b) a=(int)b
#define mpz_set_si(a,b) a=b
#define mpz_setbit(a,b) a=0
#define mpz_clrbit(a,b) a=0
#define mpz_combit(a,b) a=0
#define mpz_tstbit(a,b) a==0
#define mpz_get_str(a,b,c) strdup("not supported")
#define mpz_get_d(a) (double)(a)
#define mpz_get_ui(a) (unsigned int)(a)
#define mpz_get_si(a) (int)(a)
#define mpz_odd_p(a) (a&1)
#define mpz_even_p(a) (!(a&1))
#define mpz_set_str(a,b,c) a=atoi(b)
#define gmp_randstate_t long
#define gmp_randinit_default(a) srand(a)
#define gmp_randinit_ui(a,b) srand(a)
#define gmp_randseed_ui(a,b) srand(a=b)
#define mpz_urandomm(a,b,c) a=rand()
#define mpz_eor(a,b,c) a=(b)^(c)
#define mpz_xor(a,b,c) a=(b)^(c)
#define mpz_ior(a,b,c) a=(b)|(c)
#define mpz_and(a,b,c) a=(b)&(c)
#define mpz_pow_ui(a,b,c) a=b
#define mpz_primorial_ui(a,b)  a=0
#define mpz_lucnum_ui(a,b)  a=0
#define mpz_fib_ui(a,b)  a=0
#define mpz_gcd(a,b,c)  a=0
#define mpz_lcm(a,b,c)  a=0
#define mpz_powm(a,b,c,d)  a=0
#define mpz_invert(a,b,c)  a=0
#define mpz_com(a,b) a=~(b)
#define mpz_nextprime(a,b) a=0
#define mpz_cmp_si(a,b) (a-b)
#define mpz_cmp(a,b) (a-b)
#define mpz_abs(a,b) a=abs(b)
#define gmp_printf printf
#endif

/*Variablen Typen*/

typedef struct {
  unsigned int len;
  char *pointer;
} STRING;

typedef struct {
  double r;
  double i;
} MYCOMPLEX;
#define COMPLEX MYCOMPLEX


typedef struct {
  int dimension;
  void *pointer;
  unsigned short typ;
} ARRAY;

typedef struct {
  unsigned int typ; /* can be INTTYP, FLOATTYP, STRINGTYP, ARRAYTYP*/
  unsigned int flags; /* can be DYNAMIC, STATIC*/
  char *name;
  union {
    int *i;
    double *f;
    STRING *s;
    ARRAY *a;
    COMPLEX *c;
    ARBINT *ai;
    ARBFLOAT *af;
    ARBCOMPLEX *ac;
  } pointer;
  int local;
} VARIABLE;

/* X11Basic strukturen, welche f"ur lib aufrufe benoetigt werden.*/

struct PARAMETER {
  unsigned short typ;
  double real;     /*   das kann auch als COMPLEX pointer dienen*/
  double imag;
  int integer;     /*   das kann auch als STRING pointer und ARRAY pointer dienen*/
  void *pointer;   /*  Pointer f"ur STRING ARRAY und ARBINT*/
  unsigned short arraytyp; /* reserviert, damit man auch ARRAY pointer machen kann*/
  short panzahl;
  struct PARAMETER *ppointer;   /*Unterparameter wie indizies */
};
typedef struct PARAMETER PARAMETER;

/*STandard Funktionstyp fuer Parser-Funktionen */

typedef double (*pfunc)();
typedef STRING (*sfunc)();
typedef ARRAY (*afunc)();
typedef COMPLEX (*cfunc)();
typedef PARAMETER (*ppfunc)();

typedef struct {
  uint32_t opcode;
  const char *name;
  void (*routine)();
  signed char pmin;        /* Mindestanzahl an Parametern */
  signed char pmax;        /* Maximal moegliche Anzahl (-1) = beliebig */
  const unsigned short *pliste;  /* Liste der Kommandoparametertypen mit pmin Eintraegen */
} COMMAND;

typedef struct {
  uint32_t opcode;
  const char *name;
  double (*routine)();
  signed char pmin;        /* Mindestanzahl an Parametern */
  signed char pmax;        /* Maximal moegliche Anzahl (-1) = beliebig */
  unsigned short pliste[12];  /* Liste der Kommandoparametertypen mit pmin Eintraegen */
} FUNCTION;

typedef struct {
  uint32_t opcode;
  const char *name;
  COMPLEX (*routine)();
  signed char pmin;        /* Mindestanzahl an Parametern */
  signed char pmax;        /* Maximal moegliche Anzahl (-1) = beliebig */
  unsigned short pliste[12];  /* Liste der Kommandoparametertypen mit pmin Eintraegen */
} CFUNCTION;

typedef struct {
  uint32_t opcode;
  const char *name;
  STRING (*routine)();
  signed char pmin;        /* Mindestanzahl an Parametern */
  signed char pmax;        /* Maximal moegliche Anzahl (-1) = beliebig */
  unsigned short pliste[12];  /* Liste der Kommandoparametertypen mit pmin Eintraegen */
} SFUNCTION;

typedef struct {
  unsigned int opcode;
  const char *name;
  double (*routine)();
} SYSVAR;

typedef struct {
  unsigned int opcode;
  const char *name;
  STRING (*routine)();
} SYSSVAR;
 
typedef struct {
  char *name;        /*Name des Labels*/
  int zeile;         /*Programmzeielennummer (fuer Goto) */
  unsigned long int datapointer; /*Pointer zum datenbereich (fuer Restore) */
} LABEL;

typedef struct {
  char *name;       /*Name der Procedure (incl Rueckgabetyp)*/
  unsigned short typ;          /* 1=PROC 2=FUNC 4=DEFFN */
  unsigned short rettyp;       /* R"uckgabetyp */
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
extern int *stack;
extern int datapointer;

extern const int anzsysvars;
extern const SYSVAR sysvars[];
extern const int anzsyssvars;
extern const SYSSVAR syssvars[];

extern const int anzcomms;
extern const COMMAND comms[];
extern const int anzpfuncs;
extern const FUNCTION pfuncs[];
extern const int anzcfuncs;
extern const CFUNCTION cfuncs[];
extern const int anzpsfuncs;
extern const SFUNCTION psfuncs[];

extern int anzvariablen;
extern VARIABLE *variablen;

extern int anzlabels, anzprocs;
extern PROCEDURE *procs;
extern LABEL *labels;

/* Variablen, welche ausserhalb der lib definiert werden muessen */

extern int prglen;
extern int runfile,daemonf;
extern int programbufferlen;
extern char *programbuffer;
extern char **program;



/* Prototypen  fuer die Libx11basic.s	   */



const char *error_text(unsigned char errnr, const char *bem);
void xberror(char errnr, const char *bem);
void reset_input_mode();
void x11basicStartup();
void libx11basic_init();

void kommando(char *);   /* This is for single command execution: */
int loadprg(const char *);
int mergeprg(const char *fname);

void programmlauf();
void run_bytecode(char *,int);

void do_run();
void do_help(const char *);
void quit_x11basic(int c);
double parser(const char *funktion);

/* Hilfsfunktionen */

int find_comm(const char *);
int find_func(const char *);
int find_afunc(const char *);
int find_sfunc(const char *);
int count_parameter(char *);

void doocssig_handler( int);

double ltext(int, int, double, double, double , int, char *);
char *do_gets (char *); 
char *simple_gets(const char *);
char *indirekt2(const char *);
void local_vars_loeschen(int);
char *varrumpf(const char *);
int f_freefile();
char *bin_to_string(char *);
void set_input_mode(int,int);
void set_input_mode_echo(int);
void reset_input_mode();
int kbhit();


double ltextlen (double, int, char *); 
void cs_init();
void cs_exit();
double csget(char *);
char *csgets(char *);


void do_menu_draw();
int  do_menu_select();

void set_fill(int);
 
void activate();
void next_data_line();
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
int v_ccserr();

#if defined FRAMEBUFFER && !defined ANDROID
extern char fbdevname[];
extern char mousedevname[];
extern char keyboarddevname[];
#endif

/* API for The virtual machine */

int pusharg(PARAMETER **opstack, char *typ,...);
int callifunc(PARAMETER **opstack,void (*name)(),char *typ,...);
double callfunc(PARAMETER **opstack,void (*name)(),char *typ,...);
STRING callsfunc(PARAMETER **opstack,void (*name)(),char *typ,...);
ARRAY callafunc(PARAMETER **opstack,void (*name)(),char *typ,...);

/* important prototypes (repetitions) so that only x11basic.h need to be
   included for normal cases.... 
 */

int wort_sep(const char *t,char c,int klamb ,char *w1, char *w2);
void xtrim(const char *t,int f, char *w);
STRING inhexs(const char *n);
int exist(const char *);
size_t lof(FILE *);
size_t bload(const char *, char *, size_t);

#endif


