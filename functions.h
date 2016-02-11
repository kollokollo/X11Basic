/* functions.h Funktionen  (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#ifdef WINDOWS
#undef HAVE_LOGB
#undef HAVE_LOG1P
#undef HAVE_EXPM1
#undef HAVE_FORK
#endif

#ifdef __CYGWIN__  /* David Andersen  11.10.2003*/
#undef HAVE_LOGB
#endif

double f_nop(void *t);
double f_gasdev(double);

double logb(double);
double log1p(double a);
double expm1(double a);

int f_point(double v1, double v2);
int f_bclr(double v1, double v2);
int f_bset(double v1, double v2);
int f_bchg(double v1, double v2);
int f_btst(double v1, double v2);
int f_shr(double v1, double v2);
int f_shl(double v1, double v2);
int f_int(double b);
int f_fix(double b);
int f_fak(int k);

int f_and(int v1, int v2);
int f_or( int v1, int v2);
int f_xor(int v1, int v2);
int f_not(int v1);

double f_pred(double b);

double f_round (PARAMETER *,int);
double f_min   (PARAMETER *,int);
double f_max   (PARAMETER *,int);

int f_combin   (PARAMETER *,int);
int f_crc      (PARAMETER *,int);

int f_gray     (int);

int f_julian   (STRING);
int f_ioctl   (PARAMETER *,int);

double f_sensor (int);

