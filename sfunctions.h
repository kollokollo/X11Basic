/* sfunctions.h String Funktionen  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

STRING f_aries   (STRING n);
STRING f_arids   (STRING n);
STRING f_bins(PARAMETER *plist,int e);
STRING f_bwtds   (STRING);
STRING f_bwtes   (STRING);
STRING f_chrs    (int);
STRING f_compresss(STRING n);
STRING f_csgets  (STRING n);
STRING f_csunits (STRING n);
STRING f_cspnames(int n);
STRING f_decrypts(PARAMETER *,int);
STRING f_doocsgets(STRING n);
STRING f_doocsinfos(STRING n);
STRING f_encrypts(PARAMETER *,int);
STRING f_envs    (STRING n);
STRING f_errs    (int n);
STRING f_hexs(PARAMETER *plist, int e);
STRING f_inlines (STRING);
STRING f_juldates(int);
STRING f_lefts   (PARAMETER *,int);
STRING f_lowers  (STRING);
STRING f_mids    (PARAMETER *,int);
STRING f_mkis    (int);
STRING f_mkls    (int);
STRING f_mkfs    (double);
STRING f_mkds    (double);
STRING f_mtfds   (STRING);
STRING f_mtfes   (STRING);
STRING f_octs(PARAMETER *plist, int e);
STRING f_params  (int n);
STRING f_prgs    (int n);
STRING f_reverses(STRING);
STRING f_replaces(PARAMETER *,int);
STRING f_rights  (PARAMETER *,int);
STRING f_rles    (STRING);
STRING f_rlds    (STRING);
STRING f_spaces  (int);
STRING f_strs    (PARAMETER *plist,int e);
STRING f_strings (PARAMETER *,int);
STRING f_systems (STRING n);
STRING f_terminalnames(PARAMETER *plist,int e);
STRING f_tinegets(STRING n);
STRING f_tineinfos(STRING n);
STRING f_tinequerys(PARAMETER *plist,int e);
STRING f_tineunits(STRING n);
STRING f_trims   (STRING n);
STRING f_uncompresss(STRING n);
STRING f_unixdates(int n);
STRING f_unixtimes(int n);
STRING f_uppers  (STRING);
STRING f_words   (PARAMETER *,int);
STRING f_xtrims  (STRING n);

STRING vs_fileevent();
STRING vs_date();
STRING vs_time();
STRING vs_trace();
STRING vs_terminalname();
STRING vs_inkey();
STRING vs_error();
