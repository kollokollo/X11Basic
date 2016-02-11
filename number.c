/* number.c   Numerische Hilfsfunktionen (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
  return(i); 
}
/* Bestimmt, ob es sich um eine gültige Zahl handelt und liefert dann
   zurueck:
   1 = INTTYP
   2 = FLOATTYP
   0 = INVALID
   
*/
int myisatof(char *n) {
  if(!n) return(0);
  int i=0;
  int isfloat=0;
  int l=strlen(n);
  while (w_space(*n)) {n++;i++;}  /* Skip leading white space, if any. */
  if(*n=='-' || *n=='+') { n++;i++;} /*  Get sign, if any.  */
   /* try special codings  */
  if(*n=='$') i+=1+atohexc(++n);
  else if(*n=='%') i+=1+atobinc(++n);
  else if(*n=='0' && (n[1]&0xdf)=='X') i+=2+atohexc(n+2);
  else { 
    /* Count digits before decimal point or exponent, if any. */
    for(;v_digit(*n); n++) i++;;
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
  }
  if(i!=l) return(0);
  if(isfloat) return(2);
  return(1); 
}

/* 
Wandelt einen String mit einer (floating-point) Zahl in einen double 
um.

Diese funktion muss stark Geschwindigkeitsoptimiert sein
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

int f_gray(int n) { /* Gray-Code-Konversion */
  unsigned int i=1,a,d;
  if(n>=0) return(n^(n>>1));
  for(a=-n;;i<<=1) {
    a^=(d=a>>i);
    if(d<=1||i==16) return(a);
  }
}
int f_fak(int k) {
  register int i,s=1;
  for(i=2;i<=k;i++) {s=s*i;}
  return(s);
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





