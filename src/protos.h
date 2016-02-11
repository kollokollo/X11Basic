/* PROTOS.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#include "vtypes.h"
#include "ptypes.h"

/* Prototypen */

/* Hilfsfunktionen */

void break_handler( int);
void memdump(unsigned char *, int);
void fatal_error_handler( int);
void timer_handler( int);
double ltext(int, int, double, double, double , int, char *);
char *inkey();
int make_pliste(int,int,short *,char *,PARAMETER **);
void free_pliste(int,PARAMETER *);
int wort_sep (char *,char ,int ,char *, char *);
int wort_sepr(char *,char ,int ,char *, char *);
int wort_sep2(char *,char *,int ,char *, char *);
int wort_sepr2(char *,char *,int ,char *, char *);
char *searchchr2(char *,char);
char *searchchr2_multi(char *,char *);
char *rmemmem(char *,int,char *,int);
int xtrim(char *,int, char *);
char *error_text(char , char *);
char *do_gets (char *); 
char *simple_gets(char *);
void erase_string_array(int);
double floatarrayinhalt(int, char*);
char *indirekt2(char *);
STRING print_arg(char *);
STRING do_using(double,STRING);
void local_vars_loeschen(int);
void clear_parameters();
void clear_labelliste();
void clear_procliste();
char *varrumpf(char *);
STRING f_inputs(char *);
STRING f_lineinputs(char *);
int f_freefile();
STRING vs_error();
char *float_to_string(char *);
char *hexoct_to_string(char,char *);
char *bin_to_string(char *);
void set_input_mode(int,int);
void set_input_mode_echo(int);
void reset_input_mode();
char *terminalname();
int inp8(char *),inpf(char *),inp16(char *),inp32(char *);
int type2(char *),do_wort_sep(char *),inp16(char *),inp32(char *);


/* parser */


int init_parser();
double parser(char *);
char *s_parser(char *);
STRING string_parser(char *);
ARRAY array_parser(char *);
ARRAY array_const(char *);

int buchstabe(char);

double ltextlen (double, int, char *); 
void cs_init();
void cs_exit();
double csget(char *);
char *csgets(char *);
STRING tinegets(char *);
STRING tinequery(char *,int );
double do_funktion(char *,char *);
STRING do_sfunktion(char *,char *);

/*Variablen */

ARRAY array_info(int);
void array_zuweis_and_free(char *, ARRAY);
void c_dolocal(char *, char *);
void xzuweis(char *, char *);
int xzuweissbuf(char *, char *,int);
void varcastint(int,void *,int);
void varcastfloat(int,void *,double);
void *varptr(char *);
ARRAY double_array(ARRAY);
ARRAY copy_var_array(int);
ARRAY inv_array(ARRAY);
ARRAY mul_array(ARRAY,ARRAY);
ARRAY trans_array(ARRAY);
ARRAY create_array(int , int ,int *);
ARRAY nullmatrix(int , int ,int *);
ARRAY einheitsmatrix(int , int ,int *);

char *argument(char *);
char *varrumpf(char *);

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

char *rsearchchr(char *, char);
char *rsearchchr2(char *,char,char *);
char *searchchr(char *, char);
 

/* Kontrollsystem */

ARRAY csvget(char *,int,int);
ARRAY tinevget(char *,int,int);
ARRAY tinehistory(char *,int,int);
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
void c_tineexport(char *);
void c_tinemonitor(char *);
void c_tineserver(PARAMETER *,int);
void c_tinecycle(char *);
void c_tineput(char *);
#endif
