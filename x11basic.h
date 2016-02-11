
/* x11basic.h (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

/* Prototypen  fuer die Libx11basic.s	   */
const char *error_text(unsigned char errnr, char *bem);
void xberror(char errnr, char *bem);
void reset_input_mode();
void x11basicStartup();

void kommando(char *);   /* This is for single command execution: */
void loadprg(char *);
void programmlauf();

/* Global Variables*/

extern int databufferlen;
extern char *databuffer;
extern const char libversion[];
extern const char libvdate[];
extern int param_anzahl;
extern char **param_argumente;
extern const char xbasic_name[];
extern int pc,sp,err,errcont,everyflag,batch,echoflag;

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
void do_polygon(int,char *);
void do_restore();
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




