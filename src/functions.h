/* functions.h Funktionen  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 

 
double f_gasdev(double);
double f_round (PARAMETER *,int);
double f_min   (PARAMETER *,int);
double f_max   (PARAMETER *,int);

int f_combin   (PARAMETER *,int);
int f_crc      (PARAMETER *,int);

int f_gray     (int);

int f_julian   (STRING);
int f_ioctl   (PARAMETER *,int);

STRING f_chrs    (int);
STRING f_spaces  (int);
STRING f_juldates(int);
STRING f_mkis    (int);
STRING f_mkls    (int);
STRING f_mkfs    (double);
STRING f_mkds    (double);
STRING f_lowers  (STRING);
STRING f_uppers  (STRING);
STRING f_inlines (STRING);
STRING f_reverses(STRING);
STRING f_rles    (STRING);
STRING f_rlds    (STRING);
STRING f_mtfds   (STRING);
STRING f_mtfes   (STRING);
STRING f_bwtds   (STRING);
STRING f_bwtes   (STRING);

STRING f_replaces(PARAMETER *,int);
STRING f_rights  (PARAMETER *,int);
STRING f_lefts   (PARAMETER *,int);
STRING f_mids    (PARAMETER *,int);
STRING f_strings (PARAMETER *,int);
STRING f_encrypts(PARAMETER *,int);
STRING f_decrypts(PARAMETER *,int);

