/* XBASIC.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include "config.h"
#include "options.h"

#define FALSE    0
#define TRUE     (!FALSE)
#define PI       3.141592654
#define E        2.718281828

#define MAXSTRLEN 4096
#define MAXPRGLEN 100000
#define MAXLINELEN 4096
#define MAXVARNAMLEN 64
#define STACKSIZE 512
#define ANZVARS 1024
#define ANZLABELS 1000
#define ANZPROCS 1000

#define DEFAULTWINDOW 1

#define INTSIZE sizeof(double)

#define round(a) ((int)(a+0.5))
#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a>b)?a:b)
#define sgn(x)   ((x<0)?-1:1)
#define rad(x)   (3.141592654*x/180)
#define deg(x)   (180*x/3.141592654)

#define NOTYP          0
#define INTTYP         1
#define FLOATTYP       2
#define STRINGTYP      4
#define INDIRECTTYP    8
#define ARRAYTYP       16
#define CONSTTYP       32
#define INTARRAYTYP    (INTTYP | ARRAYTYP)
#define FLOATARRAYTYP  (FLOATTYP | ARRAYTYP)
#define STRINGARRAYTYP (STRINGTYP | ARRAYTYP)

typedef struct {
  char *name;
  int typ;
  int local;
  int opcode;
  double zahl;
  char *pointer;
} VARIABLE;

typedef struct {
  int typ;
  int dimension;
  void *pointer;
} ARRAY;

typedef struct {
  int len;
  char *pointer;
} STRING;

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


/* P-Code-Definitionen  */

#define P_EVAL         0
#define P_IGNORE       1
#define P_LEVELIN    0x100
#define P_LEVELOUT   0x200
#define P_LEVELINOUT 0x400


#define P_IF       (P_LEVELIN | 20)
#define P_ELSE     (P_LEVELINOUT | 20)
#define P_ENDIF    (P_LEVELOUT | 20)

#define P_SELECT   (P_LEVELIN | 21)
#define P_CASE     (P_LEVELINOUT | 21)
#define P_DEFAULT  (P_LEVELINOUT | 22)
#define P_ENDSELECT (P_LEVELOUT | 21)

#define P_DO       (P_LEVELIN  | 1)
#define P_REPEAT   (P_LEVELIN  | 2)
#define P_WHILE    (P_LEVELIN  | 3)
#define P_FOR      (P_LEVELIN  | 4)

#define P_LOOP     (P_LEVELOUT | 1)
#define P_UNTIL    (P_LEVELOUT | 2)
#define P_WEND     (P_LEVELOUT | 3)
#define P_NEXT     (P_LEVELOUT | 4)


#define P_TRON         2
#define P_TROFF        3
#define P_END          4
#define P_QUIT         5
#define P_CLS          6
#define P_REM          7
#define P_DATA         8
#define P_PRINT        9
#define P_BELL         10
#define P_STOP         11
#define P_NEW          12
#define P_CONT         13
#define P_BREAK        14


#define P_LABEL    (P_LEVELINOUT | 1)
#define P_PROC     (P_LEVELIN  | 30)
#define P_ENDPROC  (P_LEVELOUT  | 30)


typedef struct {
  int opcode;
  int integer;
  void *pointer;
} P_CODE;

typedef struct {
  int opcode;
  char name[32];
  void (*routine)();
} COMMAND;

/* Definition der globalen Funktionen */

/* Hilfsfunktionen */

double ltext(int, int, double, double, double , int, char *);
char *inkey();
int wort_sep (char *,char ,int ,char *, char *);
int wort_sepr(char *,char ,int ,char *, char *);
int wort_sep2(char *,char *,int ,char *, char *);
int wort_sepr2(char *,char *,int ,char *, char *);
int xtrim(char *,int, char *);
char *error_text(char , char *);
char *do_gets (char *); 
void erase_string_array(int);
double floatarrayinhalt(int, char*);
char *indirekt2(char *);
char *print_arg(char *);
void local_vars_loeschen(int);
void clear_labelliste();
void clear_procliste();
char *varrumpf(char *);
char *inputs(char *);
char *lineinputs(char *);
char *float_to_string(char *);
char *hexoct_to_string(char,char *);
void set_input_mode(int,int);
void set_input_mode_echo(int);
void reset_input_mode();
char *terminalname();

/* parser */


int init_parser();
double parser(char *);
char *s_parser(char *);
ARRAY *array_parser(char *);

int buchstabe(char);

double ltextlen (double, int, char *); 
void cs_init();
void cs_exit();
double csget(char *);
char *csgets(char *);
double do_funktion(char *,char *);
char *do_sfunktion(char *,char *);

/*Variablen */

ARRAY *array_info(int);
void array_zuweis_and_free(char *, ARRAY *);
void c_dolocal(char *, char *);
void xzuweis(char *, char *);
ARRAY *copy_var_array(int);
ARRAY *inv_array(ARRAY *);
ARRAY *mul_array(ARRAY *,ARRAY *);
ARRAY *trans_array(ARRAY *);
ARRAY *create_array(int , int ,int *);
ARRAY *nullmatrix(int , int ,int *);
ARRAY *einheitsmatrix(int , int ,int *);

/* Kommandos */
void c_local(char *),c_flush(char *);
void c_nop(char *),c_do(char *),c_else(char *);
void c_arrayfill(char *),c_arraycopy(char *),c_cls(char *),c_clr(char *);
void c_print(char *),c_clear(char *),c_tron(char *),c_troff(char *);
void c_add(char *),c_quit(char *),c_stop(char *),c_beep(char *);
void c_sub(char *),c_erase(char *),c_void(char *);
void c_mul(char *),c_exit(char *),c_pause(char *);
void c_div(char *),c_dec(char *),c_dim(char *);
void c_loop(char *),c_version(char *),c_chain(char *);
void c_while(char *),c_echo(char *),c_end(char *);
void c_return(char *),c_for(char *),c_inc(char *);
void c_if(char *), c_fft(char *),c_error(char *);
void c_load(char *),c_goto(char *),c_gosub(char *);
void c_merge(char *),c_home(char *),c_input(char *);
void c_new(char *),c_read(char *),c_cont(char *);
void c_wend(char *),c_next(char *),c_until(char *),c_break(char *);
void c_run(char *),c_list(char *),c_plist(char *),c_out(char *);
void c_dump(char *),c_open(char *),c_close(char *),c_lineinput(char *);
void c_closew(char *),c_circle(char *),c_clearw(char *);
void do_restore();
void c_dolocal(char *,char *);
void next_data_line();
void c_onbreak(char *),c_seek(char *),c_relseek(char *);
void c_onmenu(char *),c_case(char *),c_select(char *),c_endselect(char *);
void c_onerror(char *),c_system(char *),c_let(char *);
void c_on(char *),c_restore(char *),c_swap(char *);
void handle_window(int),c_bload(char *),c_bsave(char *),c_bmove(char *);
void c_bput(char *),c_bget(char *),c_wort_sep(char *);
void c_addsubmuldiv(char *,int),c_unget(char *);
void c_after(char *),c_every(char *);
void programmlauf();

/* Grafikroutinen */

void c_alert(char *),c_setfont(char *),c_defmouse(char *),c_allevent(char *);
void set_graphmode(int),set_font(char *),c_vsync(char *);
void c_scope(char *), c_color(char *), c_deffill(char *);
void c_defline(char *), c_deftext(char *), c_text(char *),c_setmouse(char *);
void c_rootwindow(char *), c_norootwindow(char *),c_usewindow(char *);
void c_draw(char *), c_ellipse(char *),c_graphmode(char *);
void c_infow(char *), c_keyevent(char *),c_mouse(char *);
void c_ltext(char *), c_mouseevent(char *),c_motionevent(char *);
void c_movew(char *), c_openw(char *),c_pcircle(char *);
void c_pellipse(char *), c_plot(char *),c_sizew(char *),c_titlew(char *);
void c_fileselect(char *),c_xload(char *),c_xrun(char *);
void c_savescreen(char *),c_savewindow(char *);

void activate();
char *rsearchchr(char *, char);
char *rsearchchr2(char *,char,char *);
char *searchchr(char *, char);
 
/* Globale Variablen nur definition*/

extern const char version[];           /* Programmversion           */
extern const char vdate[];
extern const char xbasic_name[];
extern char ifilename[];
int loadfile,runfile;
int pc,sp,prglen,echo,batch,err,errcont,everyflag;
int everytime,alarmpc;
extern int stack[];
char *program[MAXPRGLEN];
P_CODE pcode[MAXPRGLEN];
int  ptimes[MAXPRGLEN];   /* Ausfuehrungszeien der Programmzeilen  */ 

char *programbuffer;
int programbufferlen;
int datazeile;              /* fuer Restore, read , DATA */
char *datapos;
char databuffer[MAXSTRLEN];


/* fuer die Dateiverwaltung     */
extern FILE *dptr[];
extern int filenr[];


extern VARIABLE variablen[];
int anzvariablen;

extern LABEL labels[];
extern PROC  procs[];
int anzlabels, anzprocs;
int anzcomms;

union {double f;
       char *str;} returnvalue;

int turtlex,turtley;
extern int usewindow;
double ltextwinkel,ltextxfaktor,ltextyfaktor;
int ltextpflg;


/* Kontrollsystem */

ARRAY *csvget(char *,int,int);
int cssize(char *);
int cstyp(char *);
int cspid(char *);
char *cspname(int);
char *csunit(char *);
double csget(char *);
double csmin(char *);
double csmax(char *);
double csres(char *);
#ifdef CONTROL
void c_csput(char *);
void c_csvput(char *);
void c_csset(char *);
void c_cssweep(char *);
void c_csclearcallbacks(char *);
void c_cssetcallback(char *);
void sweep_value(int,float,float,float,int);
#endif
  int aplid;
  int ccs_err;
