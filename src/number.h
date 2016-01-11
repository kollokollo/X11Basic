
/* number.h   (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

int myisatof(char *n);
int myatofc(char *n);
double myatof(char *n);
COMPLEX complex_myatof(char *n);
void arbint_myatof(char *n,ARBINT);
int f_gray(int n);
unsigned long update_crc(unsigned long crc, unsigned char *buf, int len);


COMPLEX complex_add(COMPLEX a, COMPLEX b);
COMPLEX complex_sub(COMPLEX a, COMPLEX b);
COMPLEX complex_mul(COMPLEX a, COMPLEX b);
COMPLEX complex_div(COMPLEX a, COMPLEX b);
COMPLEX complex_pow(COMPLEX a, COMPLEX b);
COMPLEX complex_neg(COMPLEX a);
double complex_real(COMPLEX a);
double complex_imag(COMPLEX a);


STRING INTtoSTRING(int n);
STRING FLOATtoSTRING(double a);
STRING COMPLEXtoSTRING(COMPLEX a);

static inline int COMPLEX2INT(COMPLEX a) {return((int)a.r);}
static inline double COMPLEX2FLOAT(COMPLEX a) {return(a.r);}
static inline COMPLEX FLOAT2COMPLEX(double a) {COMPLEX ret;ret.r=a;ret.i=0;return(ret);}
static inline COMPLEX INT2COMPLEX(int a) {COMPLEX ret;ret.r=(double)a;ret.i=0;return(ret);}



#define w_space(c) ((c) == ' ' || (c) == '\t')
#define v_digit(c) ((c) >= '0' && (c) <= '9')
#define vf_digit(c) (((c) >= '0' && (c) <= '9') || (c)=='.')
#define v_var(c) (((c) >= 'A' && (c) <= 'Z') || (c)=='_')


/*Testet ob im String ab s, vor der Position pos nur Zahlen
  (oder dezimalpunkt) vorkommen.*/

static inline int vfdigittest(char *s,char *pos) {
  while(--pos>=s && vf_digit(*pos)) ;
  return(pos<s || !v_var(*pos));
}
