/* number.c   functions fornumber processing and conversion (c) Markus Hoffmann*/

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

/* Decode hexadecimal number to int  */
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
/* Decode hex-encoded binary data */

STRING inhexs(const char *n) {
  const int l=strlen(n);
  STRING ergebnis;
  ergebnis.len=(l+1)/2;
  ergebnis.pointer=malloc(ergebnis.len+1);  
  unsigned int value=0;
  int i=0;
  while(*n) {
    value<<=4;
    if(v_digit(*n)) value+=(int)(*n-'0');
    else if(*n>='a' && *n<='f') value+=(int)(*n-'a')+10;
    else if(*n>='A' && *n<='F') value+=(int)(*n-'A')+10;
    n++;
    if((i&1)) (ergebnis.pointer)[i>>1]=(value&0xff);
    i++;
  }
  (ergebnis.pointer)[ergebnis.len]=0;
  return(ergebnis);
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

/* count number of hexadecimal digits in string n */
static int atohexc(char *n) {
  int i=0;
  while(*n && (v_digit(*n) || (*n>='a' && *n<='f') || (*n>='A' && *n<='F'))) {i++;n++;}
  return(i);
}

/* count number of binary digits in string n */
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


/* 32 Bit checksum algorithm */

static unsigned long crc_table[256];  /* Table of CRCs of all 8-bit messages. */
static int crc_table_computed = 0;    /* Flag: has the table been computed? Initially false. */

static void make_crc_table(void){    /* Make the table for a fast CRC. */
  unsigned long c;
  for(int n=0;n<256;n++) {
    c=(unsigned long)n;
    for(int k=0;k<8;k++) {
      if(c&1) c=0xedb88320L^(c>>1);
      else c=c>>1;
    }
    crc_table[n]=c;
  }
  crc_table_computed=1;
}


unsigned long update_crc(unsigned long crc, unsigned char *buf, int len) {
  unsigned long c=crc^0xffffffffL;
  if(!crc_table_computed) make_crc_table();
  for(int n=0;n<len;n++) c=crc_table[(c^buf[n])&0xff]^(c>>8);
  return c^0xffffffffL;
}

/* 16 Bit checksum algorithm */

/* CRC16 CCITT X^16 x^12 + x^5 + 1 */

static uint16_t crc16_table [256] =  {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5,	0x60c6, 0x70e7,
	0x8108, 0x9129, 0xa14a, 0xb16b,	0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
	0x1231, 0x0210,	0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
	0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c,	0xf3ff, 0xe3de,
	0x2462, 0x3443, 0x0420, 0x1401,	0x64e6, 0x74c7, 0x44a4, 0x5485,
	0xa56a, 0xb54b,	0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6,	0x5695, 0x46b4,
	0xb75b, 0xa77a, 0x9719, 0x8738,	0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
	0x48c4, 0x58e5,	0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969,	0xa90a, 0xb92b,
	0x5af5, 0x4ad4, 0x7ab7, 0x6a96,	0x1a71, 0x0a50, 0x3a33, 0x2a12,
	0xdbfd, 0xcbdc,	0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
	0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03,	0x0c60, 0x1c41,
	0xedae, 0xfd8f, 0xcdec, 0xddcd,	0xad2a, 0xbd0b, 0x8d68, 0x9d49,
	0x7e97, 0x6eb6,	0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
	0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a,	0x9f59, 0x8f78,
	0x9188, 0x81a9, 0xb1ca, 0xa1eb,	0xd10c, 0xc12d, 0xf14e, 0xe16f,
	0x1080, 0x00a1,	0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c,	0xe37f, 0xf35e,
	0x02b1, 0x1290, 0x22f3, 0x32d2,	0x4235, 0x5214, 0x6277, 0x7256,
	0xb5ea, 0xa5cb,	0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
	0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447,	0x5424, 0x4405,
	0xa7db, 0xb7fa, 0x8799, 0x97b8,	0xe75f, 0xf77e, 0xc71d, 0xd73c,
	0x26d3, 0x36f2,	0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9,	0xb98a, 0xa9ab,
	0x5844, 0x4865, 0x7806, 0x6827,	0x18c0, 0x08e1, 0x3882, 0x28a3,
	0xcb7d, 0xdb5c,	0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
	0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0,	0x2ab3, 0x3a92,
	0xfd2e, 0xed0f, 0xdd6c, 0xcd4d,	0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
	0x7c26, 0x6c07,	0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
	0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba,	0x8fd9, 0x9ff8,
	0x6e17, 0x7e36, 0x4e55, 0x5e74,	0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

uint16_t CRC16CCITT(uint8_t *data, uint16_t length)
{
	uint16_t count;
	uint16_t crc = 0xFFFF;	// Init-Value
	
	uint16_t temp;

	for (count = 0; count < length; ++count)
	{
		temp = (*data++ ^ (crc >> 8)) & 0xFF;
		crc = crc16_table[temp] ^ (crc << 8);
	}

	return (crc);
}


