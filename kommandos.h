/* kommandos.h Kommandos  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


void bidnm(char *);
void c_addsubmuldiv(char *,int);
void c_add      (char *);
void c_after    (char *);
void c_arraycopy(char *);
void c_arrayfill(char *);
void c_beep     ();
void c_bload    (PARAMETER *,int);
void c_bget     (PARAMETER *,int);
void c_bmove    (PARAMETER *,int);
void c_bput     (PARAMETER *,int);
void c_bsave    (PARAMETER *,int);
void c_chain    (PARAMETER *,int);
void c_chdir    (PARAMETER *,int);
void c_clear    (char *);
void c_clr      (char *);
void c_cls      (char *);
void c_connect  (PARAMETER *,int);
void c_dec      (PARAMETER *,int);
void c_detatch  (PARAMETER *,int);
void c_dim      (char *);
void c_div      (char *);
void c_do       (char *);
void c_dpoke    (PARAMETER *,int);
void c_dump     (PARAMETER *,int);
void c_echo     (PARAMETER *,int);
void c_edit     (char *);
void c_end      (char *);
void c_erase    (char *);
void c_error    (PARAMETER *,int);
void c_eval     (PARAMETER *,int);
void c_every    (char *);
void c_exit     (char *);
void c_flush    (PARAMETER *,int);
void c_for(char *);
void c_free     (PARAMETER *,int);
void c_help     (char *);
void c_inc      (PARAMETER *,int);
void c_load     (PARAMETER *,int);
void c_local    (char *);
void c_locate   (PARAMETER *,int);
void c_loop     (char *);
void c_lpoke    (PARAMETER *,int);
void c_memdump  (PARAMETER *,int);
void c_merge    (PARAMETER *,int);
void c_mul      (char *);
void c_nop      (char *);
void c_objc_add   (PARAMETER *,int);
void c_objc_delete (PARAMETER *,int);
void c_on(char *);
void c_onbreak(char *);
void c_pause    (PARAMETER *,int);
void c_pipe     (PARAMETER *,int);
void c_poke     (PARAMETER *,int);
void c_print    (char *);
void c_quit     (PARAMETER *,int);
void c_receive  (PARAMETER *,int);
void c_relseek  (PARAMETER *,int);
void c_return   (char *);
void c_rsrc_free(char *);
void c_rsrc_load(PARAMETER *,int);
void c_save     (PARAMETER *,int);
void c_seek     (PARAMETER *,int);
void c_send     (PARAMETER *,int);
void c_shm_free (PARAMETER *,int);
void c_sound    (PARAMETER *,int);
void c_sort(char *);
void c_stop     ();
void c_system   (PARAMETER *,int);
void c_sub      (char *);
void c_swap     (char *);
void c_troff    ();
void c_tron     ();
void c_unget    (char *);
void c_version  (char *);
void c_void     (char *);
void c_while    (char *);
void c_wort_sep (char *);

void c_if(char *);
void c_fft(char *);
void c_fit(char *);
void c_fit_linear(char *);
void c_goto(char *);
void c_gosub(char *);
void c_home(char *);
void c_input(char *);
void c_new(char *),c_read(char *),c_cont(char *);
void c_spawn(char *);
void c_wend(char *),c_next(char *),c_until(char *),c_break(char *);
void c_run(char *),c_list(PARAMETER *,int),c_plist(char *),c_out(char *);

void c_close(char *),c_lineinput(char *);
void c_dolocal(char *,char *);

void c_link(PARAMETER *,int);
void c_randomize(PARAMETER *,int);
void c_unmap(PARAMETER *,int);
void c_msync(PARAMETER *,int);
void c_exec(char *);
void c_onmenu(char *),c_case(char *),c_select(char *),c_endselect(char *);
void c_onerror(char *);
void c_let(char *);
void c_restore(PARAMETER *,int);
