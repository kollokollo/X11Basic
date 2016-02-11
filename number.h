
/* number.h   (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

int myisatof(char *n);
int myatofc(char *n);
double myatof(char *n);
int f_gray(int n);
int f_fak(int k);
unsigned long update_crc(unsigned long crc, unsigned char *buf, int len);


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
