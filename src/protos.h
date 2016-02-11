/* PROTOS.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#include "vtypes.h"

/* Prototypen */

/* Hilfsfunktionen */


double ltext(int, int, double, double, double , int, char *);
char *inkey();
int wort_sep (char *,char ,int ,char *, char *);
int wort_sepr(char *,char ,int ,char *, char *);
int wort_sep2(char *,char *,int ,char *, char *);
int wort_sepr2(char *,char *,int ,char *, char *);
char *rmemmem(char *,int,char *,int);
int xtrim(char *,int, char *);
char *error_text(char , char *);
char *do_gets (char *); 
char *simple_gets(char *);
void erase_string_array(int);
double floatarrayinhalt(int, char*);
char *indirekt2(char *);
char *print_arg(char *);
void local_vars_loeschen(int);
void clear_parameters();
void clear_labelliste();
void clear_procliste();
char *varrumpf(char *);
STRING f_inputs(char *);
STRING f_lineinputs(char *);
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
ARRAY *array_parser(char *);

int buchstabe(char);

double ltextlen (double, int, char *); 
void cs_init();
void cs_exit();
double csget(char *);
char *csgets(char *);
double do_funktion(char *,char *);
STRING do_sfunktion(char *,char *);

/*Variablen */

ARRAY *array_info(int);
void array_zuweis_and_free(char *, ARRAY *);
void c_dolocal(char *, char *);
void xzuweis(char *, char *);
int xzuweissbuf(char *, char *,int);
void *varptr(char *);
ARRAY *copy_var_array(int);
ARRAY *inv_array(ARRAY *);
ARRAY *mul_array(ARRAY *,ARRAY *);
ARRAY *trans_array(ARRAY *);
ARRAY *create_array(int , int ,int *);
ARRAY *nullmatrix(int , int ,int *);
ARRAY *einheitsmatrix(int , int ,int *);

/* Kommandos */
void bidnm(char *);
void c_poke(char *),c_dpoke(char *),c_lpoke(char *);
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
void c_link(char *),c_exec(char *);
void c_onmenu(char *),c_case(char *),c_select(char *),c_endselect(char *);
void c_onerror(char *),c_system(char *),c_let(char *);
void c_on(char *),c_restore(char *),c_swap(char *);
void handle_window(int),c_bload(char *),c_bsave(char *),c_bmove(char *);
void c_bput(char *),c_bget(char *),c_wort_sep(char *);
void c_addsubmuldiv(char *,int),c_unget(char *);
void c_after(char *),c_every(char *),c_rsrc_load(char *),c_rsrc_free(char *),c_form_do(char *),c_alert_do(char *);
void programmlauf();

/* Grafikroutinen */

void c_alert(char *),c_setfont(char *),c_defmouse(char *),c_allevent(char *);
void c_menu(char *),c_menuset(char *),c_menudef(char *),c_menukill(char *);
void set_graphmode(int),set_font(char *),c_vsync(char *);
void c_copyarea(char *),c_sget(char *),c_sput(char *),c_get(char *),c_put(char *);
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
void c_box(char *),c_pbox(char *),c_line(char *);
void c_polyfill(char *),c_polyline(char *),c_polymark(char *);
void do_menu_draw();
int  do_menu_select();
void do_polygon(int,char *);
 
void activate();

char *rsearchchr(char *, char);
char *rsearchchr2(char *,char,char *);
char *searchchr(char *, char);
 

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
