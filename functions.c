/* functions.c   Sammlung von Funktionen (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "defs.h"
#include "ptypes.h"
#include "vtypes.h"
#include "protos.h"
#include "gkommandos.h"
#include "functions.h"


double f_nop(void *t) {return(0.0);}
#ifndef HAVE_LOGB
double logb(double a) {return(log(a)/log(2));}
#endif
#ifndef HAVE_LOG1P
double log1p(double a) {return(log(1+a));}
#endif
#ifndef HAVE_EXPM1
double expm1(double a) {return(exp(a)-1);}
#endif
#ifndef NOGRAPHICS
int f_point(double v1, double v2) {return(get_point((int)v1,(int)v2));}
#endif
int f_bclr(double v1, double v2) {return((int)v1 & ~ (1 <<((int)v2)));}
int f_bset(double v1, double v2) {return((int)v1 | (1 <<((int)v2)));}
int f_bchg(double v1, double v2) {return((int)v1 ^ (1 <<((int)v2)));}
int f_btst(double v1, double v2) {return((((int)v1 & (1 <<((int)v2)))==0) ?  0 : -1); }
int f_shr(double v1, double v2)  {return(((int)v1)>>((int)v2));}
int f_shl(double v1, double v2)  {return(((int)v1)<<((int)v2));}
int f_int(double b) {return((int)b);}
int f_fix(double b) {if(b>=0) return((int)b);
                      else return(-((int)(-b)));}
double f_pred(double b) {return(ceil(b-1));}

int f_fak(int k) {
  int i,s=1;
  for(i=2;i<=k;i++) {s=s*i;}
  return(s);
}

int f_combin(PARAMETER *plist,int e) {
  int z=1,n=plist[0].integer,k=plist[1].integer,i;
  double zz=1;
  if(k>n || n<=0 || k<=0) return(0);
  if(k==n) return(1);
  if(n-k>k) {k=n-k;}
  for(i=n-k;i>=1;i--)  zz*=(n-i+1)/(double)i;
  return((int)zz);
}
double f_gasdev(double d) { /* Gaussverteilter Zufall */
  static int flag=1;
  static double gset;
  double fac,rsq,v1,v2;
  flag=(!flag);
  if(flag) return(gset);
  else {
    do {
      v1=2*((double)rand()/RAND_MAX)-1;
      v2=2*((double)rand()/RAND_MAX)-1;
      rsq=v1*v1+v2*v2;
    } while(rsq>=1 || rsq==0);
    fac=sqrt(-2*log(rsq)/rsq);
    gset=v1*fac;
    return(v2*fac);
  }
}
double f_round(PARAMETER *plist,int e) {
  if(e==1) return(round(plist[0].real));
  if(e>=2){
    int kommast=-plist[1].integer;
    return(round(plist[0].real/pow(10,kommast))*pow(10,kommast)); 
  }
}

#define IGREG (15+31L*(10+12L*1582))
int f_julian(STRING n) { /* Julianischer Tag aus time$ */
  char buf[n.len+1],buf2[n.len+1];
  int e;
  int day,mon,jahr;	
  long jul;
  int ja,jy,jm;

  memcpy(buf,n.pointer,n.len);
  buf[n.len]=0;
  e=wort_sep(buf,'.',0,buf2,buf);
  if(e<2) return(-1);
  day=atoi(buf2);
  e=wort_sep(buf,'.',0,buf2,buf);
  if(e<2) return(-1);
  mon=atoi(buf2);
  jy=jahr=atoi(buf);
  if(jy==0) return(-1); /* Jahr 0 gibt es nicht */
  if(jy<0) ++jy;
  if(mon>2) jm=mon+1;
  else { --jy; jm=mon+13; }
  jul=(long)(floor(365.25*jy)+floor(30.6001*jm)+day+1720995);
  if(day+31L*(mon+12L*jahr)>=IGREG) {
    ja=(int)(0.01*jy);
    jul+=2-ja+(int)(0.25*ja);
  }
  return(jul);
}
int f_gray(int n) { /* Gray-Code-Konversion */
  unsigned int i=1,a,d;
  if(n>=0) return(n^(n>>1));
  for(a=-n;;i<<=1) {
    a^=(d=a>>i);
    if(d<=1||i==16) return(a);
  }
}

double f_min(PARAMETER *plist,int e) {
  if(e==1) return(plist[0].real);
  else if(e==2) return(min(plist[0].real,plist[1].real));
  else {
    int i=e-1;
    double ret=plist[i].real;
    while((--i)>=0) {
      ret=min(ret,plist[i].real);
    }
    return(ret);
  }
}
double f_max(PARAMETER *plist,int e) {
  if(e==1) return(plist[0].real);
  else if(e==2) return(max(plist[0].real,plist[1].real));
  else {
    int i=e-1;
    double ret=plist[i].real;
    while((--i)>=0) {
      ret=max(ret,plist[i].real);
    }
    return(ret);
  }
}



/* 32 Bit Checksumme */


unsigned long crc_table[256];  /* Table of CRCs of all 8-bit messages. */
int crc_table_computed = 0;    /* Flag: has the table been computed? Initially false. */

void make_crc_table(void){    /* Make the table for a fast CRC. */
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

int f_crc(PARAMETER *plist,int e) { 
  if(e==1)  return(update_crc(0L, plist[0].pointer, plist[0].integer));
  if(e>=1)  return(update_crc(plist[1].integer, plist[0].pointer, plist[0].integer));
}

STRING f_errs(int n) { return(create_string(error_text((char)n,NULL))); }

STRING f_lowers(STRING n) {   
  int i=0;
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  ergebnis.len=n.len;
  while(i<n.len) {ergebnis.pointer[i]=tolower(n.pointer[i]); i++;}
  return(ergebnis);
}
STRING f_uppers(STRING n) {   
  int i=0;
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  ergebnis.len=n.len;
  while(i<n.len) {ergebnis.pointer[i]=toupper(n.pointer[i]); i++;}
  return(ergebnis);
}
STRING f_replaces(PARAMETER *plist,int e) {  /* MH 10.02.2004 */
  STRING ergebnis;
  char *pos;
  int i=0;
  int start=0;
  ergebnis.len=0;
  ergebnis.pointer=malloc(1);
  if(e==3) {
    pos=(char *)memmem(&(((char *)(plist[0].pointer))[start]),plist[0].integer-start,
                       plist[1].pointer,plist[1].integer);
    while(pos!=NULL) {         
      i=((int)(pos-(char *)plist[0].pointer))-start;     
      ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+i+plist[2].integer);
      memcpy((char *)ergebnis.pointer+ergebnis.len,(char *)plist[0].pointer+start,i);
      memcpy((char *)ergebnis.pointer+ergebnis.len+i,(char *)plist[2].pointer,plist[2].integer);
      ergebnis.len+=i+plist[2].integer;
      start+=i+plist[1].integer;
      pos=(char *)memmem(&(((char *)(plist[0].pointer))[start]),plist[0].integer-start,
                       plist[1].pointer,plist[1].integer);
    }		       
    ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+(plist[0].integer-start));
    memcpy((char *)ergebnis.pointer+ergebnis.len,(char *)plist[0].pointer+start,plist[0].integer-start);
    ergebnis.len+=(plist[0].integer-start);
  }
  return(ergebnis);
}
STRING f_mids(PARAMETER *plist,int e) {  
  STRING buffer,ergebnis;
  int p,l;
  if(e>=2) {
    buffer.len=plist[0].integer;
    buffer.pointer=plist[0].pointer;
    p=min(max(plist[1].integer,1),buffer.len);
    if(e<3) l=1;
    else l=min(max(plist[2].integer,0),buffer.len-p+1); 
    ergebnis.pointer=malloc(l+1);
    ergebnis.len=l;
    memcpy(ergebnis.pointer,buffer.pointer+p-1,l);  
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  return(ergebnis);
}
STRING f_lefts(PARAMETER *plist,int e) {
  STRING ergebnis;
  if(e>=1) {
    ergebnis.len=plist[0].integer;
    ergebnis.pointer=malloc(ergebnis.len+1);
    memcpy(ergebnis.pointer,plist[0].pointer,ergebnis.len);
    if(e<2) ergebnis.len=1;
    else ergebnis.len=min(max(plist[1].integer,0),ergebnis.len);
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  return(ergebnis);
}

/* Gibt das i te Wort aus Zeichenkette zurueck (mit Blank getrennt, Teile in
Anfuehrungszeichen werden als ein Wort behandelt. (c) Markus Hoffmann 2010)*/
STRING f_words(PARAMETER *plist,int e) {
  STRING ergebnis;
  int i=0,j=0,k;
  int c=1;
  int l=1;
  int f=0;
  char *p;
  if(e>=2) l=plist[1].integer;
  if(e>=1) {
    
    k=ergebnis.len=plist[0].integer;
    ergebnis.pointer=malloc(ergebnis.len+1);

    p=plist[0].pointer;
    while(i<k) {
      if(p[i]==' ' && !f) c++;
      else if(p[i]=='"') f=!f;
      else if(c==l) ergebnis.pointer[j++]=p[i];
      i++; 
    }
    ergebnis.pointer[j]=0;
    ergebnis.len=j;
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  return(ergebnis);
}
STRING f_rights(PARAMETER *plist,int e) {
  STRING buffer,ergebnis;
  int j;
  if(e>=1) {
    buffer.len=plist[0].integer;
    buffer.pointer=plist[0].pointer;
    if(e<2) j=1;
    else j=min(max(plist[1].integer,0),buffer.len); 
    ergebnis.pointer=malloc(j+1);
    ergebnis.len=j;
    memcpy(ergebnis.pointer,buffer.pointer+buffer.len-j,j);
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  return(ergebnis);
}
STRING f_strings(PARAMETER *plist,int e) {
  STRING buffer,ergebnis;
  int i=0,j;
  if(e>=2) {
    j=plist[0].integer;
    buffer.len=plist[1].integer;
    buffer.pointer=plist[1].pointer;
    ergebnis.pointer=malloc(j*buffer.len+1);
    ergebnis.len=j*buffer.len;
    while(i<j) {memcpy(ergebnis.pointer+i*buffer.len,buffer.pointer,buffer.len); i++;}
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  return(ergebnis);
}
STRING f_spaces(int n) {   
  STRING ergebnis;
  int i=0;
  ergebnis.pointer=malloc(n+1);
  ergebnis.len=n;
  while(i<n) ergebnis.pointer[i++]=' ';
  return(ergebnis);
}
#undef IGREG
#define IGREG 2299161
STRING f_juldates(int n) {   
  STRING ergebnis;
  long ja,jalpha,jb;
  long day,mon,jahr;
  ergebnis.pointer=malloc(16);
  if(n>=IGREG) {
    jalpha=(long)(((float)(n-1867216)-0.25)/36524.25);
    ja=n+1+jalpha-(long)(0.25*jalpha);
  } else ja=n;
  jb=ja+1524;
  jahr=(long)(6680.0+((float)(jb-2439870)-122.1)/365.25);
  day=(long)(365*jahr+(0.25*jahr));
  mon=(long)((jb-day)/30.6001);
  day=jb-day-(long)(30.6001*mon);
  mon--;
  if(mon>12) mon-=12;
  jahr-=4715;
  if(mon>2) --(jahr);
  if(jahr<=0) --(jahr);
  sprintf(ergebnis.pointer,"%02d.%02d.%04d",day,mon,jahr);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_chrs(int n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(2);
  ergebnis.len=1;
  *ergebnis.pointer=(char)n;
  return(ergebnis);
}
STRING f_mkis(int n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(sizeof(short)+1);
  ergebnis.len=sizeof(short);
  *((short *)ergebnis.pointer)=(short)n;
  return(ergebnis);
}
STRING f_mkls(int n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(sizeof(long)+1);
  ergebnis.len=sizeof(long);
  *((long *)ergebnis.pointer)=(long)n;
  return(ergebnis);
}
STRING f_mkfs(double n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(sizeof(float)+1);
  ergebnis.len=sizeof(float);
  *((float *)ergebnis.pointer)=(float)n;
  return(ergebnis);
}
STRING f_mkds(double n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(sizeof(double)+1);
  ergebnis.len=sizeof(double);
  *((double *)ergebnis.pointer)=n;
  return(ergebnis);
}
STRING f_inlines(STRING n) {   
  STRING ergebnis;
  char *pos1=n.pointer;
  char *pos2;
  ergebnis.len=n.len*3/4;
  pos2=ergebnis.pointer=malloc(ergebnis.len);
  while(pos2-ergebnis.pointer<ergebnis.len-2) {
  *pos2=(((pos1[0]-36) & 0x3f)<<2)|(((pos1[1]-36) & 0x30)>>4);
  pos2[1]=(((pos1[1]-36) & 0xf)<<4)|(((pos1[2]-36) & 0x3c)>>2);
  pos2[2]=(((pos1[2]-36) & 0x3)<<6)|(((pos1[3]-36) & 0x3f));
  pos2+=3;
  pos1+=4;
  }
  return(ergebnis);
}
STRING f_reverses(STRING n) {   
  int i=0;
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  ergebnis.len=n.len;
  while(i<n.len) {ergebnis.pointer[i]=n.pointer[n.len-i-1]; i++;}
  return(ergebnis);
}


/* ************ Code Engines   ************************ */
/* *********  RLE -- Run Length Encoding/Decoding ***** */

/*  In the output stream, any two consecutive
    characters with the same value flag a run.  A byte following
    those two characters gives the count of *additional*
    repeat characters, which can be anything from 0 to 255.  */

STRING f_rles(STRING n) {  /*Run Length Encoding*/
  int j=0,i=0;
  unsigned char c,count,last=0;
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+(int)(n.len/2)+1);
  while(i<n.len) {
    c=n.pointer[i++];
    ergebnis.pointer[j++]=c;
    if(c==last) {
      count=0;
      while(count<255 && i<n.len) {
        c=n.pointer[i++];
        if(c==last) count++;
        else break;
      }
      ergebnis.pointer[j++]=count;
      if(count!=255 && c!=last) ergebnis.pointer[j++]=c;
    }
    last=c;
  }
  ergebnis.len=j;
  return(ergebnis);
}
STRING f_rlds(STRING n) {  /*Run Length Decoding*/
  int j=0,i=0;
  unsigned char c,count,last=0;
  STRING ergebnis;
 
  while(i<n.len) {        /* Laenge bestimmen : */
      c=n.pointer[i++];
      j++;
      if(c==last) {
            count=n.pointer[i++];
            j+=(count&0xff);
      }
      last=c;
  }
  last=0;
  ergebnis.pointer=malloc(j+1);
  i=j=0;
  while(i<n.len) {        /* Decodieren */
      c=n.pointer[i++];
      ergebnis.pointer[j++]=c;
      if(c==last) {
        count=n.pointer[i++];
        while(count-->0) ergebnis.pointer[j++]=c;
      }
      last=c;
  }
  ergebnis.len=j;
  return(ergebnis);
}


/*  An MTF encoder encodes each character using the
    count of distinct previous characters seen since the characters
    last appearance.  This is implemented by keeping an array of
    characters.  Each new input character is encoded with its
    current position in the array.  The character is then moved to
    position 0 in the array, and all the higher order characters
    are moved down by one position to make roon.
  
    Both the encoder and decoder have to start with the order array
    initialized to the same values.   */

STRING f_mtfes(STRING n) {  /* Move To Front Encoding*/
    unsigned char order[256];
    unsigned int i,j,k;
    unsigned char c;
    STRING ergebnis;
    ergebnis.pointer=malloc(n.len+1);
    for(i=0;i<256;i++) order[i]=(unsigned char)i;
    i=0;
    while(i<n.len) {
      c=n.pointer[i];
      /* Find the char, and output it  */
     for(j=0;j<256;j++)
       if(order[j]==(c&0xff)) break;
      ergebnis.pointer[i++]=j;
          /* Now shuffle the order array  */
      for(k=j;k>0;k--) order[k]=order[k-1];
      order[0]=c;
    }  
  ergebnis.len=n.len;
  return(ergebnis);
}
STRING f_mtfds(STRING n) {  /* Move To Front Decoding*/
    unsigned char order[256];
    unsigned int i,j,k;
    unsigned char c;
    STRING ergebnis;
    ergebnis.pointer=malloc(n.len+1);
    for(i=0;i<256;i++) order[i]=(unsigned char)i;
    i=0;
    while(i<n.len) {
      j=n.pointer[i] & 0xff;
      c=order[j];
      ergebnis.pointer[i++]=c;
          /* Now shuffle the order array  */
      for(k=j;k>0;k--) order[k]=order[k-1];
      order[0]=c;
    }  
  ergebnis.len=n.len;
  return(ergebnis);
}

/**************** Burrows-Wheeler Transformation ***************/

 /* Hilfsfunktionen fuer Burrows-Wheeler transform*/

unsigned char *rotlexcmp_buf = NULL;
int rottexcmp_bufsize = 0;

int rotlexcmp(const void *l, const void *r) {
  int li=*(const int*)l,ri=*(const int*)r,ac=rottexcmp_bufsize;
  while(rotlexcmp_buf[li]==rotlexcmp_buf[ri]) {
        if(++li==rottexcmp_bufsize) li=0;
        if(++ri==rottexcmp_bufsize) ri=0;
        if(!--ac) return 0;
  }
  if(rotlexcmp_buf[li]>rotlexcmp_buf[ri]) return 1;
  else return -1;
}



STRING f_bwtes(STRING n) {  /* Burrows-Wheeler transform*/
  STRING ergebnis;
  int indices[n.len];
  int i;
  ergebnis.pointer=malloc(n.len+1+sizeof(int));
  for(i=0;i<n.len;i++) indices[i]=i;
  rotlexcmp_buf=n.pointer;
  rottexcmp_bufsize=n.len;
  qsort(indices,n.len,sizeof(int),rotlexcmp);
  for(i=0;i<n.len;i++)
        ergebnis.pointer[i+sizeof(int)]=n.pointer[(indices[i]+n.len-1)%n.len];

  for(i=0;i<n.len;i++) {
    if(indices[i]==1) {
      *((int *)ergebnis.pointer)=i;
      break;
    }
  }
  ergebnis.len=n.len+sizeof(int);
  return(ergebnis);
}
STRING f_bwtds(STRING n) {  /* inverse Burrows-Wheeler transform */
  STRING ergebnis;
  int primary_index;
  int size;   
  if(n.len>=sizeof(int)) {
    size=n.len-sizeof(int);
    primary_index=*((int *)n.pointer);
  } else {
    size=0;
    primary_index=0;
  }
  ergebnis.pointer=malloc(size+1);
  {
    unsigned char F[size];
    int buckets[256];
    int i,j,k;
    int indices[size];

    for(i=0;i<256;i++) buckets[i]=0;
    for(i=0;i<size;i++) buckets[n.pointer[sizeof(int)+i]&0xff]++;
    for(i=0,k=0;i<256;i++)
      for(j=0;j<buckets[i];j++) F[k++]=i;
   
    for(i=0,j=0;i<256;i++) {
      while(i>F[j] && j<size) j++;
      buckets[i]=j;     // it will get fake values if there is no i in F, but
                        // that won't bring us any problems
    }

    for(i=0;i<size;i++) indices[buckets[n.pointer[sizeof(int)+i]&0xff]++]=i;
    for(i=0,j=primary_index;i<size;i++) {
      ergebnis.pointer[i]=n.pointer[j+sizeof(int)];
      j=indices[j];
    }
  }
  ergebnis.len=size;
  return(ergebnis);
}
#ifdef USE_BLOWFISH
#include "blowfish.h"
#endif
STRING f_encrypts(PARAMETER *plist,int e) {
  STRING ergebnis;
  int len=plist[0].integer>>3;
  int i;  
  unsigned long *L,*R;
  if(plist[0].integer&7) len++;
  len<<=3;
  ergebnis.pointer=malloc(len+1);
  ergebnis.len=len;
  memcpy(ergebnis.pointer,plist[0].pointer,plist[0].integer);
#ifdef USE_BLOWFISH 
  if(e==2) {
    BLOWFISH_CTX ctx;
    Blowfish_Init(&ctx, (unsigned char*)plist[1].pointer,plist[1].integer);
    len>>=3;
    for(i=0;i<len;i++) {
      L=(unsigned long *)&(ergebnis.pointer[i*8]);
      R=(unsigned long *)&(ergebnis.pointer[i*8+4]);
      Blowfish_Encrypt(&ctx,L,R);
    }
  } 
#else
#ifdef HAVE_MEMFROB
  memfrob(ergebnis.pointer,len);  
#endif
#endif  
  return(ergebnis);
}
STRING f_decrypts(PARAMETER *plist,int e) {
  STRING ergebnis;
  int len=plist[0].integer>>3;
  int i;  
  unsigned long *L,*R;
  if(plist[0].integer&7) len++;
  len<<=3;
  ergebnis.pointer=malloc(len+1);
  ergebnis.len=len;
  memcpy(ergebnis.pointer,plist[0].pointer,plist[0].integer);
#ifdef USE_BLOWFISH 
  if(e==2) {
    BLOWFISH_CTX ctx;
    Blowfish_Init(&ctx, (unsigned char*)plist[1].pointer,plist[1].integer);
    len>>=3;
    for(i=0;i<len;i++) {
      L=(unsigned long *)&(ergebnis.pointer[i*8]);
      R=(unsigned long *)&(ergebnis.pointer[i*8+4]);
      Blowfish_Decrypt(&ctx,L,R);
    }
  } 
#else
 #ifdef HAVE_MEMFROB 
   memfrob(ergebnis.pointer,len);
 #endif 
#endif  
  return(ergebnis);
}
