/* number.c   Numerische Hilfsfunktionen (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>


#include "defs.h"
#include "options.h"
#include "x11basic.h"
#include "variablen.h"
#include "number.h"


static int atohex(char *n) {
  int value=0;
  while(*n) {
    value<<=4;
    if(v_digit(*n)) value+=(int)(*n-'0');
    else if(*n>='a' && *n<='f') value+=(int)(*n-'a')+10;
    else if(*n>='A' && *n<='F') value+=(int)(*n-'A')+10;
    n++;
  }
  return(value);
}


static int atobin(char *n) {
  int value=0;
  while(*n) {
    value<<=1;
    if(*n!='0') value++;  
    n++;
  }
  return(value);
}
static int atohexc(char *n) {
  int i=0;
  while(*n && (v_digit(*n) || (*n>='a' && *n<='f') || (*n>='A' && *n<='F'))) {i++;n++;}
  return(i);
}
static int atobinc(char *n) {
  int i=0;
  while(*n && (*n=='0' || *n=='1')) {i++;n++;}
  return(i);
}

/* Bestimmt die anzal an Zeichen, welche zu einer Gültigen Zahl
gehören. z.B. für val?()
*/
int myatofc(char *n) {
  if(!n) return(0);
  int i=0;
  while (w_space(*n)) {n++;i++;}  /* Skip leading white space, if any. */
  if(*n=='-' || *n=='+') { n++;i++;} /*  Get sign, if any.  */
   /* try special codings  */
  if(*n=='$') return(i+1+atohexc(++n));
  if(*n=='%') return(i+1+atobinc(++n));
  if(*n=='0' && (n[1]&0xdf)=='X') return(i+2+atohexc(n+2));
  if((*n&0xdf)=='E') return(i);  /*should not happen here*/
  if((*n&0xdf)=='I') return(i);  /*should not happen here*/

  /* Count digits before decimal point or exponent, if any. */
  for(;v_digit(*n); n++) i++;;
  /* Count digits after decimal point, if any. */
  if(*n=='.') {
    n++;i++;
    while(v_digit(*n)) {i++;n++;}
  }
  /* Handle exponent, if any. */
  if((*n&0xdf)=='E') {
    n++;i++;
    /* Get sign of exponent, if any. */
    if(*n=='-' || *n=='+') {i++;n++;} 

    /* Get digits of exponent, if any. */
    for(;v_digit(*n); n++) i++;;
  }
  if((*n&0xdf)=='I') { /*Check if the last digit is an I*/
     // iscomplex=1;
      n++;i++;
  }
  return(i); 
}
/* Bestimmt, ob es sich um eine gültige Zahl handelt und liefert dann
   zurueck:
   1 = INTTYP
   2 = FLOATTYP
   3 = COMPLEXTYP
   5 = ARBINTTYP
   0 = INVALID
   
*/
int myisatof(char *n) {
  if(!n) return(0);
  int i=0;
  int isfloat=0;
  int iscomplex=0;
  int l=strlen(n);
  int mantisse=0;  /*vorauss. Informationsgehalt in bits.*/
  int j;
  while (w_space(*n)) {n++;i++;}  /* Skip leading white space, if any. */
  if(*n=='-' || *n=='+') { n++;i++;} /*  Get sign, if any.  */
   /* try special codings  */
  if(*n=='$') {j=atohexc(++n); mantisse+=j*4; i+=1+j;}
  else if(*n=='%') {j=atobinc(++n); mantisse+=j; i+=1+j;}
  else if(*n=='0' && (n[1]&0xdf)=='X') {j=atohexc(n+2); mantisse+=j*4; i+=2+j;}
  else if((*n&0xdf)=='E') ;  /*should not happen here*/
  else if((*n&0xdf)=='I') ;  /*should not happen here*/
  else { 
    /* Count digits before decimal point or exponent, if any. */
    j=i;
    for(;v_digit(*n); n++) i++;
    j=(i-j)*332/100;  /*Stellen zur Basis 10, 3.32 bits */
    mantisse+=j;
    /* Count digits after decimal point, if any. */
    if(*n=='.') {
      n++;i++;
      isfloat=1;
      while(v_digit(*n)) {i++;n++;}
    }
    /* Handle exponent, if any. */
    if((*n&0xdf)=='E') {
      isfloat=1;
      n++;i++;
      /* Get sign of exponent, if any. */
      if(*n=='-' || *n=='+') {i++;n++;} 

      /* Get digits of exponent, if any. */
      for(;v_digit(*n); n++) i++;;
    }
    if((*n&0xdf)=='I') { /*Check if the last digit is an I*/
      iscomplex=1;
      n++;i++;
    }
  }
  /* Anzahl der Digits in der Mantisse zaehlen, um zu entscheiden ob es
  ARBINT oder ARBFLOAT ist*/
    //  printf("<%s> --> %d bits\n",n-i,mantisse);

  
  if(i!=l) return(NOTYP);
  if(iscomplex) return(COMPLEXTYP);
  if(isfloat) return(FLOATTYP);
  if(mantisse<=sizeof(int)*8) return(INTTYP);
  /*spezialfall im Übergangsbereich der Intzahlen
    1000000000 bis 2147483647
    hier müssen wir die Zahl genauer analysieren ob sie noch in INT passt.
    TODO: */
  return(ARBINTTYP); 
}

COMPLEX complex_myatof(char *n) {
  COMPLEX ret;
  ret.i=ret.r=0;
  int l;
  if(n && (l=strlen(n))>0) {
    while(l>0 && w_space(n[l-1]) ) l--;
    if(l>0) {
      if((n[l-1]&0xdf)=='I') ret.i=myatof(n);
      else ret.r=myatof(n);    
    }
  }
  return(ret);
}


/* 
Wandelt einen String mit einer (floating-point) Zahl in einen double 
um.

Diese funktion muss stark Geschwindigkeitsoptimiert sein
TODO: Hier koennte man noch einen Flag zurückliefern, ob es ein real oder imaginaerteil ist.
*/
double myatof(char *n) {
  double sign=1.0;
  while (w_space(*n) ) n++;  /* Skip leading white space, if any. */
  if(*n=='-') { /*  Get sign, if any.  */
    sign=-1.0;
    n++;
  } else if(*n=='+') n++;
  /* try special codings  */
  if(*n=='$') return(sign*(double)atohex(++n));
  if(*n=='%') return(sign*(double)atobin(++n));
  if(*n=='0' && (n[1]&0xdf)=='X') return(sign*(double)atohex(n+2));

  /* Get digits before decimal point or exponent, if any. */
  double value=0.0;
  for(;v_digit(*n); n++) value=value*10.0+(*n-'0');
  /* Get digits after decimal point, if any. */
  if(*n=='.') {
    double pow10 = 10.0;
    n++;
    while(v_digit(*n)) {
      value+=(*n-'0')/pow10;
      pow10*=10.0;
      n++;
    }
  }
  /* Handle exponent, if any. */
  if((*n&0xdf)=='E') {
    int f=0;
    double scale=1.0;
    unsigned int ex=0; 
    n++;

    /* Get sign of exponent, if any. */
    if(*n=='-') {
      f=1;
      n++;
    } else if(*n=='+') n++;
    /* Get digits of exponent, if any. */
    for(;v_digit(*n); n++) ex=ex*10+(*n-'0');
    if(ex>308) ex=308;
    /* Calculate scaling factor. */
    while(ex>= 64) { scale *= 1E64; ex-=64; }
    while(ex>=  8) { scale *= 1E8;  ex-=8; }
    while(ex>   0) { scale *= 10.0; ex--; }

    /* Return signed and scaled floating point result. */
    return sign*(f?(value/scale):(value*scale));
  }
  /* Return signed floating point result. */
  return(sign*value);
}

/*Arbitrary int number lesen... */
void arbint_myatof(char *n, ARBINT ret) {
  int sign=1;
// printf("AIMYT: <%s>\n",n);
  while (w_space(*n) ) n++;  /* Skip leading white space, if any. */
  if(*n=='-') { /*  Get sign, if any.  */
    sign=-1;
    n++;
  } else if(*n=='+') n++;
  
  /* try special codings  */
  if(*n=='$') mpz_set_str(ret,++n,16);
  else if(*n=='%') mpz_set_str(ret,++n,2);
  else if(*n=='0' && (n[1]&0xdf)=='X') mpz_set_str(ret,n+2,16);
  else mpz_set_str(ret,n,10);
  if(sign==-1) mpz_neg(ret,ret);
  return;
}
int f_gray(int n) { /* Gray-Code-Konversion */
  unsigned int i=1,a,d;
  if(n>=0) return(n^(n>>1));
  for(a=-n;;i<<=1) {
    a^=(d=a>>i);
    if(d<=1||i==16) return(a);
  }
}
#if 0
int f_fak(int k) {
  register int i,s=1;
  for(i=2;i<=k;i++) {s=s*i;}
  return(s);
}
#endif
/* Operationen zum COMPLEX Datentyp */

COMPLEX complex_add(COMPLEX a, COMPLEX b) {
  COMPLEX c;
  c.r=a.r+b.r;
  c.i=a.i+b.i;
  return(c);
}
COMPLEX complex_sub(COMPLEX a, COMPLEX b) {
  COMPLEX c;
  c.r=a.r-b.r;
  c.i=a.i-b.i;
  return(c);
}
COMPLEX complex_neg(COMPLEX a) {
  a.r=-a.r;
  a.i=-a.i;
  return(a);
}
double complex_real(COMPLEX a) {return(a.r);}
double complex_imag(COMPLEX a) {return(a.i);}

COMPLEX complex_mul(COMPLEX a, COMPLEX b) {
  COMPLEX c;
  c.r=a.r*b.r-a.i*b.i;
  c.i=a.i*b.r+a.r*b.i;
  return(c);
}
COMPLEX complex_div(COMPLEX a, COMPLEX b) {
  COMPLEX c;
  double tmp=b.r*b.r+b.i*b.i;
  c.r=(a.r*b.r+a.i*b.i)/tmp;
  c.i=(a.i*b.r-a.r*b.i)/tmp;
  return(c);
}
COMPLEX complex_pow(COMPLEX a, COMPLEX b) {
  COMPLEX c;
  double r=sqrt(a.r*a.r+a.i*a.i);
  double t=atan2(a.i,a.r);
  double p=pow(r,b.r)*exp(-b.i*t);
  c.r=p*cos(b.r*t+b.i*log(r));
  c.i=p*sin(b.r*t+b.i*log(r));
  return(c);
}



/*Datentyp zu STRING funktionen */

STRING INTtoSTRING(int n) {
  STRING ret;
  ret.pointer=malloc(100);
  sprintf(ret.pointer,"%d",n);
  ret.len=strlen(ret.pointer);
  return(ret);
}
STRING FLOATtoSTRING(double a) {
  STRING ret;
  ret.pointer=malloc(100);
  sprintf(ret.pointer,"%.13g",a);
  ret.len=strlen(ret.pointer);
  return(ret);
}
STRING COMPLEXtoSTRING(COMPLEX a) {
  STRING ret;
  ret.pointer=malloc(100);
  if(a.i>=0) sprintf(ret.pointer,"(%.13g+%.13gi)",a.r,a.i);
  else sprintf(ret.pointer,"(%.13g%.13gi)",a.r,a.i);
  ret.len=strlen(ret.pointer);
  return(ret);
}




/* 32 Bit Checksumme */

static unsigned long crc_table[256];  /* Table of CRCs of all 8-bit messages. */
static int crc_table_computed = 0;    /* Flag: has the table been computed? Initially false. */

static void make_crc_table(void){    /* Make the table for a fast CRC. */
  unsigned long c;
  int n, k;
  
  for(n=0;n<256;n++) {
    c=(unsigned long)n;
    for(k=0;k<8;k++) {
      if(c&1) c=0xedb88320L^(c>>1);
      else c=c>>1;
    }
    crc_table[n]=c;
  }
  crc_table_computed=1;
}


unsigned long update_crc(unsigned long crc, unsigned char *buf, int len) {
  unsigned long c=crc^0xffffffffL;
  int n;

  if(!crc_table_computed) make_crc_table();
  for(n=0;n<len;n++) c=crc_table[(c^buf[n])&0xff]^(c>>8);
  return c^0xffffffffL;
}





