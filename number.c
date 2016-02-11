/* number.c   Numerische Hilfsfunktionen (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>

#include "number.h"


static int atohex(char *n) {
  int value=0;
  while(*n) {
    value<<=4;
    if(*n>='0' && *n<='9') value+=(int)(*n-'0');
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

double myatof(char *n) {
  if(*n=='$') return((double)atohex(++n));  
  else if(*n=='%') return((double)atobin(++n));  
  else return(atof(n));
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





