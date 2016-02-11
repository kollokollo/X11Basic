/* functions.c   Sammlung von Funktionen (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "parameter.h"
#include "xbasic.h"
#include "gkommandos.h"
#include "functions.h"
#include "afunctions.h"
#include "array.h"
#include "wort_sep.h"
#include "graphics.h"
#include "number.h"
#include "decode.h"
#include "aes.h"
#include "io.h"
#include "file.h"
#include "window.h"

#ifndef WINDOWS
  #include <fnmatch.h>
#else
  #include "Windows.extension/fnmatch.h"
#endif

#ifndef HAVE_LOGB
static double logb(double a) {return(log(a)/log(2));}
#endif
#ifndef HAVE_LOG1P
static double log1p(double a) {return(log(1+a));}
#endif
#ifndef HAVE_EXPM1
static double expm1(double a) {return(exp(a)-1);}
#endif
extern int shm_malloc(int,int);
extern int shm_attach(int);

static char   *arrptr(PARAMETER *p,int e) {return((char *)variablen[p->integer].pointer.a); }

/*Funktionen (mit f_ prefix )*/

static double f_add(double v1, double v2) {return(v1+v2);}
static int    f_asc(STRING n)   {return((int)n.pointer[0]); }
static int    f_bclr(double v1, double v2) {return((int)v1 & ~ (1 <<((int)v2)));}
static int    f_byte(int a)     {return(a&0xff);}
static int    f_card(int a)     {return(a&0xffff);}
static int    f_word(int a)     {return((signed int)(signed short)(a&0xffff));}
static int    f_swap(int a)     {return(((a&0xffff)<<16)|((a&0xffff0000)>>16));}

static double f_cvd(STRING n)   {return((double)(*((double *)n.pointer))); }
static double f_cvf(STRING n)   {return((double)(*((float *)n.pointer))); }
static int    f_cvi(STRING n)   {return((int)(*((short *)n.pointer))); }
static int    f_cvl(STRING n)   {return((int)(*((long *)n.pointer))); }
static double f_deg(double d)   {return(d/PI*180);}
static int    f_device(STRING n) {return(stat_device(n.pointer)); }
static double f_div(double v1, double v2) {return(v1/v2);}
static int    f_dpeek(int adr)  {return((int)(*(short *)adr));}
static double f_eval(STRING n)  {return(parser(n.pointer));}
static int    f_even(int b)     {return(b&1 ? 0:-1);}
static int    f_exist(STRING n) {return(-exist(n.pointer)); }
static double f_frac(double b)  {return(b-((double)((int)b)));}
static int    f_inode(STRING n) {return(stat_inode(n.pointer)); }
static int    f_len(STRING n)   {return(n.len); }
static int    f_lpeek(int adr)  {return((int)(*(long *)adr));}
static double f_ltextlen(STRING n) {return((double)ltextlen(ltextxfaktor,ltextpflg,n.pointer)); }
static int    f_malloc(int size) {return((int)malloc((size_t)size));}
static int    f_mshrink(int adr,int size) {return(0);} /*always return zero*/
static int    f_mode(STRING n)  {return(stat_mode(n.pointer)); }
static double f_mul(double v1, double v2) {return(v1*v2);}
static int    f_nlink(STRING n) {return(stat_nlink(n.pointer)); }
       double f_nop(void *t)    {return(0.0);}
static int    f_odd(int b)      {return(b&1 ? -1:0);}
static int    f_peek(int adr)   {return((int)(*(char *)adr));}
#ifndef NOGRAPHICS
static int f_point(double v1, double v2) {return(get_point((int)v1,(int)v2));}
#endif
static double f_rad(double d)   {return(d*PI/180);}
static int    f_random(double d) {return((int)((double)rand()/RAND_MAX*d));}
static int    f_realloc(int adr,int size) {return((int)realloc((char *)adr,(size_t)size));}
static double f_rnd(double d)   {return((double)rand()/RAND_MAX);}

static int    f_size(STRING n)  {return(stat_size(n.pointer)); }
static int    f_sgn(double b)   {return(sgn(b));}
static int    f_srand(double d) {srand((int)d);return(0);}
static double f_sub(double v1, double v2) {return(v1-v2);}
static int    f_succ(double b)  {return((int)(b+1));}

static double f_val(STRING n)   {return(myatof(n.pointer)); }
static int    f_valf(STRING n)   {
  if(n.len==0) return(0);
  return(myatofc(n.pointer));
}

#ifdef TINE
static double f_tinemax(STRING n) { return(tinemax(n.pointer)); }
static double f_tinemin(STRING n) { return(tinemin(n.pointer)); }
static double f_tineget(STRING n) { return(tineget(n.pointer)); }
static int f_tinesize(STRING n) {return(tinesize(n.pointer)); }
static int f_tinetyp(STRING n)  {return(tinetyp(n.pointer)); }
#endif
#ifdef DOOCS
static double f_doocsget(STRING n) { return(doocsget(n.pointer)); }
static int f_doocssize(STRING n) {return(doocssize(n.pointer)); }
static int f_doocstyp(STRING n)  {return(doocstyp(n.pointer)); }
static double f_doocstimestamp(STRING n) { return(doocstimestamp(n.pointer)); }
#endif
#ifdef CONTROL
static double f_csmax(STRING n)  {return(csmax(n.pointer)); }
static double f_csmin(STRING n)  {return(csmin(n.pointer)); }
static double f_csres(STRING n)  {return(csres(n.pointer)); }
static double f_csget(STRING n)  {return(csget(n.pointer)); }
static int f_cssize(STRING n)    {return(cssize(n.pointer)); }
static int f_cspid(STRING n)     {return(cspid(n.pointer)); }
#endif



static int f_and(int v1, int v2) {return(v1 & v2);}
static int f_or( int v1, int v2) {return(v1 | v2);}
static int f_xor(int v1, int v2) {return(v1 ^ v2);}

static int f_not(int v1)         {return(v1?0:-1);}



static int f_bset(double v1, double v2) {return((int)v1 | (1 <<((int)v2)));}
static int f_bchg(double v1, double v2) {return((int)v1 ^ (1 <<((int)v2)));}
static int f_btst(double v1, double v2) {return((((int)v1 & (1 <<((int)v2)))==0) ?  0 : -1); }
static int f_shr(double v1, double v2)  {return(((int)v1)>>((int)v2));}
static int f_shl(double v1, double v2)  {return(((int)v1)<<((int)v2));}
static int f_int(double b) {return((int)b);}
static int f_cint(double b) {return((int)round(b));}
static double f_pred(double b) {return(ceil(b-1));}

static int  f_dimf(PARAMETER *plist,int e) {
  return((double)do_dimension(&variablen[plist->integer]));
}

static int f_combin(PARAMETER *plist,int e) {
  int n=plist[0].integer,k=plist[1].integer,i;
  double zz=1;
  if(k>n || n<=0 || k<=0) return(0);
  if(k==n) return(1);
  if(n-k>k) {k=n-k;}
  for(i=n-k;i>=1;i--)  zz*=(n-i+1)/(double)i;
  return((int)zz);
}
static int f_variat(PARAMETER *plist,int e) {
  int n=plist[0].integer,k=plist[1].integer,i;
  double zz=1;
  if(k>n || n<=0 || k<=0) return(0);
  if(k==n) return(1);
  for(i=n-k+1;i<=n;i++)  zz*=i;
  return((int)zz);
}
static double f_gasdev(double d) { /* Gaussverteilter Zufall */
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
static double f_round(PARAMETER *plist,int e) {
  if(e==1) return(round(plist->real));
  if(e>=2){
    int kommast=-plist[1].integer;
    return(round(plist[0].real/pow(10,kommast))*pow(10,kommast)); 
  }
  return(0);
}

#define IGREG (15+31L*(10+12L*1582))
static int f_julian(STRING n) { /* Julianischer Tag aus time$ */
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
#undef IGREG

static double f_min(PARAMETER *plist,int e) {
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
static double f_max(PARAMETER *plist,int e) {
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


static int f_crc(PARAMETER *plist,int e) { 
  if(e>1) return(update_crc(plist[1].integer, plist->pointer, plist->integer));
  return(update_crc(0L, plist->pointer, plist->integer));
}


static double f_sensor(int n) {
 //  printf("Sensor #%d\n",n);
  if(n>=0 && n<ANZSENSORS) return(sensordata[n]);
  else return(-1);
}


static int f_lof(PARAMETER *plist,int e) {
// dump_parameterlist(plist,e);
  if(filenr[plist[0].integer].typ) return(lof(filenr[plist[0].integer].dptr));
  else { xberror(24,""); return(0);} /* File nicht geoeffnet */
}
static int f_loc(PARAMETER *plist,int e) {
  if(filenr[plist[0].integer].typ) return(ftell(filenr[plist[0].integer].dptr));
  else { xberror(24,""); return(0);} /* File nicht geoeffnet */
}
static int f_eof(PARAMETER *plist,int e) {
  if(plist[0].integer==-2) return((myeof(stdin))?-1:0);
  else if(filenr[plist[0].integer].typ) {
    // fflush(filenr[plist[0].integer].dptr);
    // dieses fflush hat bei der WINDOWS version dazu geführt, dass
    // ans ende des files gesprungen wurde. Folgendes EOF war immer true
    return(myeof(filenr[plist[0].integer].dptr)?-1:0);
  } else { xberror(24,""); return(0);} /* File nicht geoeffnet */
}

static int f_wort_sep(PARAMETER *plist,int e) {
  STRING str1,str2;
  int ret;
  str1.pointer=malloc(plist->integer+1);
  str2.pointer=malloc(plist->integer+1);
  
  ret=wort_sep2(plist[0].pointer,plist[1].pointer,plist[2].integer,str1.pointer,str2.pointer);
  str1.len=strlen(str1.pointer);
  str2.len=strlen(str2.pointer);
  if(e>3)  varcaststring_and_free(plist[3].typ,(STRING *)plist[3].pointer,str1);  
  else free_string(&str1);
  if(e>4)  varcaststring_and_free(plist[4].typ,(STRING *)plist[4].pointer,str2);
  else free_string(&str2);
  return(ret);
}

static int f_instr(PARAMETER *plist,int e) {
  int start=0;
  if(plist[1].integer>plist->integer) return(0);
  if(e>2) start=plist[2].integer-1;
  if(start<0) start=0;
  if(start>=plist->integer) return(0);  
  char *pos=(char *)memmem(&(((char *)(plist->pointer))[start]),plist->integer-start,plist[1].pointer,plist[1].integer);
  if(pos!=NULL) return((int)(pos-(char *)plist->pointer)+1);
  return(0);
}
static int f_rinstr(PARAMETER *plist,int e) {
  if(plist[1].integer>plist->integer) return(0);
  int start=plist->integer-1;
  if(e>2) start=plist[2].integer-1;
  if(start<0) start=0;
  if(start>=plist->integer) return(0);  
  char *pos=rmemmem(plist->pointer,start,plist[1].pointer,plist[1].integer);
  if(pos!=NULL) return((int)(pos-(char *)plist->pointer)+1);
  return(0);
}
static int f_tally(PARAMETER *plist,int e) {
  int start=0;
  if(e>2) start=plist[2].integer-1;
  if(start<0) start=0;
  if(start>=plist->integer) return(0);  
  if(plist->integer<start+plist[1].integer) return(0);
  int i=start;
  char *t=plist->pointer;
  char *c=plist[1].pointer;
  int j=0;
  for(;;) {
/* suche erstes Vorkommen von c */
    while(i<plist->integer && t[i]!=*c) i++;
    if(i>=plist->integer) return(j); /* schon am ende ? */
    else {     /* ueberpruefe, ob auch der Rest von c vorkommt */
      if(memcmp(t+i,c,plist[1].integer)==0) {
        j++;
	i+=plist[1].integer;
      } else i++;     /* ansonsten weitersuchen */
    }
  }
}

static int f_glob(PARAMETER *plist,int e) {
  int flags=FNM_NOESCAPE;
  if(e>2) flags^=plist[2].integer;
  flags=fnmatch(plist[1].pointer,plist->pointer,flags);
  if(flags==0) return(-1);
  return(0);
}

#ifndef NOGRAPHICS
static int f_form_alert(PARAMETER *plist,int e) {
  return(form_alert(plist[0].integer,plist[1].pointer));
}
static int f_form_center(PARAMETER *plist,int e) {
  int x,y,w,h,ret;
  graphics();
  gem_init();
  ret=form_center((OBJECT *)plist->integer,&x,&y,&w,&h);
  if(e>4) {
    if(plist[1].typ!=PL_LEER) varcastint(plist[1].integer,plist[1].pointer,x);
    if(plist[2].typ!=PL_LEER) varcastint(plist[2].integer,plist[2].pointer,y);
    if(plist[3].typ!=PL_LEER) varcastint(plist[3].integer,plist[3].pointer,w);
    if(plist[4].typ!=PL_LEER) varcastint(plist[4].integer,plist[4].pointer,h);
  }
  return(ret);
}
static int f_form_dial(PARAMETER *plist,int e) {
    graphics();
    gem_init();
    return(form_dial(plist->integer,plist[1].integer,
      plist[2].integer,plist[3].integer,plist[4].integer,plist[5].integer,
      plist[6].integer,plist[7].integer,plist[8].integer));
}
static int f_form_do(PARAMETER *plist,int e) {
  graphics();
  gem_init();
  return(form_do((OBJECT *)plist->integer));
}
static int f_objc_draw(PARAMETER *plist,int e) {
  graphics();
  gem_init();
  return(objc_draw((OBJECT *)plist->integer,plist[1].integer
    ,plist[2].integer,plist[3].integer,plist[4].integer));
}
static int f_objc_find(PARAMETER *plist,int e) {
    return(objc_find((OBJECT *)plist[0].integer,plist[1].integer,plist[2].integer));
}
static int f_objc_offset(PARAMETER *plist,int e) {
  int x,y,ret;

  if(e>2 && plist[2].typ!=PL_LEER) {
    int typ;
    if((typ=variablen[plist[2].integer].typ)==ARRAYTYP) 
      typ=variablen[plist[2].integer].pointer.a->typ;
    if(typ==INTTYP) x=*((int *)plist[2].pointer);
    else if(typ==FLOATTYP) x=(int)*((double *)plist[2].pointer);
  }
  if(e>3 && plist[3].typ!=PL_LEER) {
    int typ;
    if((typ=variablen[plist[3].integer].typ)==ARRAYTYP) 
      typ=variablen[plist[3].integer].pointer.a->typ;
    if(typ==INTTYP) y=*((int *)plist[3].pointer);
    else if(typ==FLOATTYP) y=(int)*((double *)plist[3].pointer);
  }
  ret=objc_offset((OBJECT *)plist[0].integer,plist[1].integer,&x,&y);

  if(e>2 && plist[2].typ!=PL_LEER) varcastint(plist[2].integer,plist[2].pointer,x);
  if(e>3 && plist[3].typ!=PL_LEER) varcastint(plist[3].integer,plist[3].pointer,y);

  return(ret);
}
static int f_get_color(PARAMETER *plist,int e) {
  graphics();
  return(get_color(plist->integer>>8,plist[1].integer>>8,plist[2].integer>>8,255));
}
static int f_color_rgb(PARAMETER *plist,int e) {
  double a=1;
  if(e>3) a=plist[3].real;
  graphics();
  return(get_color((unsigned char)(plist[0].real*255.0),(unsigned char)(plist[1].real*255.0),
                   (unsigned char)(plist[2].real*255.0),(unsigned char)(a*255.0)));
}
static int f_rsrc_gaddr(PARAMETER *plist,int e) {
  char *ptr;
  int i=rsrc_gaddr(plist->integer,plist[1].integer,&ptr);
  if(i>0) return((int)ptr);
  else return(-1);
}


static int f_listselect(PARAMETER *plist,int e) {
  int sel=-1;
  if(e>2) sel=plist[2].integer;
  ARRAY a;
  a=*(ARRAY *)&(plist[1].integer);
  return(lsel_input(plist->pointer,(STRING *)(a.pointer+a.dimension*INTSIZE),anz_eintraege(&a),sel));
}
#endif




static int f_varptr(PARAMETER *p,int e) {
  switch(p->typ) {
  case PL_IVAR:
  case PL_FVAR:
  case PL_NVAR:
  case PL_IARRAYVAR: /* Variable */
  case PL_FARRAYVAR: /* Variable */
    return((int)p->pointer);
  case PL_SVAR:
    return((int)((STRING *)p->pointer)->pointer);
  case PL_SARRAYVAR: /* Variable */
    if(p->panzahl>0) return((int)((STRING *)p->pointer)->pointer);
    else return((int)p->pointer);
  case PL_VAR:   /* Variable */
  case PL_ARRAYVAR: /* Variable */
  case PL_ALLVAR:  /* Varname */
  default:
  {
    int typ=variablen[p->integer].typ;
    if(typ==ARRAYTYP) typ=variablen[p->integer].pointer.a->typ;
    if(typ==STRINGTYP) return((int)((STRING *)p->pointer)->pointer);
    else if(typ==ARRAYTYP) return((int)((ARRAY *)p->pointer)->pointer);
    else return((int)p->pointer);
    }
  }  
}



/*F_CONST fuer die Funktionen, welche bei constantem input imemr das gleiche 
  output liefern.
  */
  
  
  /* Folgende funktionen sollten noch implemntiert werden
  
  MEAN(ARRAY)
  RMS(ARRAY)
  STDDEV(ARRAY)
  int=MEDIAN(ARRAY)
  also
  AMAX(ARRAY)
  AMIN(ARRAY)
  NORM(ARRAY)
  
  */
  
const FUNCTION pfuncs[]= {  /* alphabetisch !!! */
 { F_CONST|F_ARGUMENT|F_DRET,  "!nulldummy", f_nop ,0,0   ,{0}},
 { F_CONST|F_DQUICK|F_DRET,    "ABS"       , fabs ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ACOS"      , acos ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ACOSH"      , acosh ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ADD"     , f_add ,2,2     ,{PL_NUMBER,PL_NUMBER}},

 { F_CONST|F_IQUICK|F_IRET,    "AND"      ,(pfunc) f_and,2,2     ,{PL_NUMBER,PL_NUMBER}},


 { F_PLISTE|F_IRET,  "ARRPTR"    , (pfunc)arrptr ,1,1     ,{PL_ARRAYVAR}},
 { F_CONST|F_SQUICK|F_IRET,    "ASC"       ,(pfunc) f_asc ,1,1   ,{PL_STRING}},
 { F_CONST|F_DQUICK|F_DRET,    "ASIN"      , asin ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ASINH"      , asinh ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ATAN"      , atan ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ATAN2"     , atan2 ,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ATANH"     , atanh ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ATN"       , atan ,1,1     ,{PL_NUMBER}},

 { F_CONST|F_DQUICK|F_IRET,    "BCHG"      , (pfunc)f_bchg,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_CONST|F_DQUICK|F_IRET,    "BCLR"      ,(pfunc) f_bclr,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_CONST|F_DQUICK|F_IRET,    "BSET"      , (pfunc)f_bset,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_CONST|F_DQUICK|F_IRET,    "BTST"      ,(pfunc) f_btst,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_CONST|F_IQUICK|F_IRET,    "BYTE"     , (pfunc)f_byte ,1,1     ,{PL_INT}},

 { F_PLISTE|F_IRET,            "CALL"       , (pfunc)f_call ,1,-1     ,{PL_INT,PL_EVAL}},
 { F_CONST|F_IQUICK|F_IRET,    "CARD"     , (pfunc)f_card ,1,1     ,{PL_INT}},
 { F_CONST|F_DQUICK|F_DRET,    "CBRT"      , cbrt ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "CEIL"      , ceil ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_IRET,    "CINT"      ,(pfunc) f_cint ,1,1     ,{PL_NUMBER}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,            "COLOR_RGB", (pfunc)f_color_rgb ,3,4   ,{PL_NUMBER,PL_NUMBER,PL_NUMBER,PL_NUMBER}},
#endif
 { F_CONST|F_PLISTE|F_IRET,    "COMBIN"    , (pfunc)f_combin ,2,2     ,{PL_INT,PL_INT}},
 { F_CONST|F_DQUICK|F_DRET,    "COS"       , cos ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "COSH"      , cosh ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_PLISTE|F_IRET,    "CRC"       ,(pfunc) f_crc ,1,2     ,{PL_STRING, PL_INT}},
#ifdef CONTROL
 { F_SQUICK|F_DRET,  "CSGET"     , f_csget ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSMAX"     , f_csmax ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSMIN"     , f_csmin ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CSPID"     , (pfunc)f_cspid ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSRES"     , f_csres ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CSSIZE"    , (pfunc)f_cssize ,1,1   ,{PL_STRING}},
#endif
 { F_CONST|F_SQUICK|F_DRET,  "CVD"       , f_cvd ,1,1     ,{PL_STRING}},
 { F_CONST|F_SQUICK|F_DRET,  "CVF"       , f_cvf ,1,1     ,{PL_STRING}},
 { F_CONST|F_SQUICK|F_IRET,  "CVI"       ,(pfunc) f_cvi ,1,1     ,{PL_STRING}},
 { F_CONST|F_SQUICK|F_IRET,  "CVL"       , (pfunc)f_cvl ,1,1     ,{PL_STRING}},
 { F_CONST|F_SQUICK|F_IRET,  "CVS"       , (pfunc)f_cvf ,1,1     ,{PL_STRING}},

 { F_CONST|F_DQUICK|F_DRET,    "DEG"       , f_deg ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_PLISTE|F_DRET,    "DET"       , f_det ,1,1     ,{PL_FARRAY}},
 { F_SQUICK|F_IRET,    "DEVICE"    , (pfunc)f_device,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE|F_IRET,  "DIM?"      , (pfunc)f_dimf ,1,1      ,{PL_ARRAYVAR}},
 { F_CONST|F_DQUICK|F_DRET,    "DIV"       , f_div ,2,2     ,{PL_NUMBER,PL_NUMBER}},
#ifdef DOOCS
 { F_SQUICK|F_DRET,  "DOOCSGET"     , f_doocsget ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "DOOCSSIZE"    ,(pfunc) f_doocssize ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "DOOCSTIMESTAMP"    , f_doocstimestamp ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "DOOCSTYP"    , (pfunc)f_doocstyp ,1,1   ,{PL_STRING}},
#endif
 { F_IQUICK|F_IRET,    "DPEEK"     , (pfunc)f_dpeek ,1,1     ,{PL_INT}},

 { F_PLISTE|F_IRET,    "EOF"       ,(pfunc) f_eof ,1,1     ,{PL_FILENR}},

 { F_CONST|F_SQUICK|F_DRET,  "EVAL"      , f_eval ,1,1      ,{PL_STRING}},
 { F_CONST|F_IQUICK|F_IRET,    "EVEN"       , (pfunc)f_even ,1,1     ,{PL_NUMBER}},
 { F_IQUICK|F_IRET,    "EVENT?"      , (pfunc)f_eventf ,1,1     ,{PL_INT}},

 { F_ARGUMENT|F_IRET,  "EXEC"       , (pfunc)f_exec ,1,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_SQUICK|F_IRET,    "EXIST"      , (pfunc)f_exist ,1,1     ,{PL_STRING}},
 { F_CONST|F_DQUICK|F_DRET,    "EXP"       , exp ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "EXPM1"     , expm1 ,1,1     ,{PL_NUMBER}},

 { F_CONST|F_IQUICK|F_IRET,    "FACT"       , (pfunc)f_fak ,1,1     ,{PL_INT}},
 { F_CONST|F_DQUICK|F_DRET,    "FIX"       , trunc ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "FLOOR"     , floor ,1,1     ,{PL_NUMBER}},
#ifdef HAVE_FORK
 { F_SIMPLE|F_IRET,    "FORK"     , (pfunc)fork ,0,0     },
#else
 { F_SIMPLE|F_IRET,    "FORK"     , (pfunc)f_nop ,0,0     },
#endif
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "FORM_ALERT", (pfunc)f_form_alert ,2,2   ,{PL_INT,PL_STRING}},
 { F_PLISTE|F_IRET,    "FORM_CENTER", (pfunc)f_form_center ,1,5   ,{PL_INT,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { F_PLISTE|F_IRET,    "FORM_DIAL", (pfunc)f_form_dial ,9,9   ,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,    "FORM_DO",   (pfunc)f_form_do ,1,1   ,{PL_INT}},
#endif
 { F_CONST|F_DQUICK|F_DRET,    "FRAC"      , f_frac ,1,1     ,{PL_NUMBER}},
 { F_SIMPLE|F_IRET,    "FREEFILE"  , (pfunc)f_freefile ,0,0  },

 { F_CONST|F_DQUICK|F_DRET,    "GAMMA"       , tgamma ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "GASDEV"   , f_gasdev ,0,1     ,{PL_NUMBER}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "GET_COLOR", (pfunc)f_get_color ,3,3   ,{PL_INT,PL_INT,PL_INT}},
#endif
 { F_CONST|F_PLISTE|F_IRET,    "GLOB"     ,(pfunc) f_glob ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},
 { F_CONST|F_IQUICK|F_IRET,    "GRAY"     ,(pfunc) f_gray ,1,1     ,{PL_INT}},

 { F_CONST|F_DQUICK|F_DRET,    "HYPOT"     , hypot ,2,2     ,{PL_NUMBER,PL_NUMBER}},

 { F_SQUICK|F_IRET,    "INODE"     ,(pfunc) f_inode,1,1   ,{PL_STRING}},
 { F_PLISTE|F_IRET,  "INP"       , (pfunc)inp8 ,1,1      ,{PL_FILENR}},
 { F_PLISTE|F_IRET,  "INP%"      , (pfunc)inp32 ,1,1      ,{PL_FILENR}},
 { F_PLISTE|F_IRET,  "INP&"      , (pfunc)inp16 ,1,1      ,{PL_FILENR}},
 { F_PLISTE|F_IRET,  "INP?"      ,(pfunc) inpf ,1,1      ,{PL_FILENR}},
 { F_CONST|F_PLISTE|F_IRET,  "INSTR"     ,(pfunc) f_instr ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},

 { F_CONST|F_DQUICK|F_IRET,    "INT"       ,(pfunc) f_int ,1,1     ,{PL_NUMBER}},
 { F_PLISTE|F_IRET,    "IOCTL"     , (pfunc)f_ioctl ,2,3     ,{PL_FILENR,PL_INT,PL_INT}},
 { F_CONST|F_SQUICK|F_IRET,    "JULIAN"    ,(pfunc) f_julian ,1,1     ,{PL_STRING}},

 { F_CONST|F_SQUICK|F_IRET,    "LEN"       ,(pfunc) f_len ,1,1   ,{PL_STRING}},
 { F_CONST|F_DQUICK|F_DRET,    "LGAMMA"       , lgamma ,1,1     ,{PL_NUMBER}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "LISTSELECT", (pfunc)f_listselect ,2,3   ,{PL_STRING,PL_SARRAY,PL_INT}},
#endif
 { F_CONST|F_DQUICK|F_DRET,    "LN"        , log ,1,1     ,{PL_NUMBER}},

 { F_PLISTE|F_IRET,    "LOC"       , (pfunc)f_loc ,1,1     ,{PL_FILENR}},
 { F_PLISTE|F_IRET,    "LOF"       , (pfunc)f_lof ,1,1     ,{PL_FILENR}},

 { F_CONST|F_DQUICK|F_DRET,    "LOG"       , log ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "LOG10"     , log10 ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "LOG1P"     , log1p ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "LOGB"      , logb  ,1,1     ,{PL_NUMBER}},
 { F_IQUICK|F_IRET,    "LPEEK"    , (pfunc)f_lpeek ,1,1     ,{PL_INT}},
 { F_SQUICK|F_DRET,    "LTEXTLEN"  , f_ltextlen ,1,1   ,{PL_STRING}},

 { F_IQUICK|F_IRET,    "MALLOC"    ,(pfunc) f_malloc ,1,1     ,{PL_INT}},
 { F_CONST|F_PLISTE|F_DRET,    "MAX"     , f_max ,1,-1     ,{PL_NUMBER,PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_PLISTE|F_DRET,    "MIN"     , f_min ,1,-1     ,{PL_NUMBER,PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "MOD"     , fmod ,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_SQUICK|F_IRET,    "MODE"     , (pfunc)f_mode,1,1   ,{PL_STRING}},
 { F_IQUICK|F_IRET,    "MSHRINK"    , (pfunc)f_mshrink ,2,2     ,{PL_INT,PL_INT}},
 { F_CONST|F_DQUICK|F_DRET,    "MUL"     , f_mul ,2,2     ,{PL_NUMBER,PL_NUMBER}},

 { F_SQUICK|F_IRET,    "NLINK"     , (pfunc)f_nlink,1,1   ,{PL_STRING}},
 { F_CONST|F_IQUICK|F_IRET,    "NOT"      , (pfunc)f_not,1,1     ,{PL_NUMBER}},

#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "OBJC_DRAW",(pfunc) f_objc_draw ,5,5   ,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,    "OBJC_FIND", (pfunc)f_objc_find ,3,3   ,{PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,    "OBJC_OFFSET", (pfunc)f_objc_offset ,4,4,{PL_INT,PL_INT,PL_NVAR,PL_NVAR}},
#endif
 { F_CONST|F_IQUICK|F_IRET,    "ODD"       ,(pfunc) f_odd ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_IQUICK|F_IRET,    "OR"      ,(pfunc) f_or,2,2     ,{PL_NUMBER,PL_NUMBER}},

 { F_IQUICK|F_IRET,    "PEEK"      , (pfunc)f_peek ,1,1     ,{PL_INT}},
#ifndef NOGRAPHICS
 { F_DQUICK|F_IRET,    "POINT"     , (pfunc)f_point ,2,2     ,{PL_NUMBER, PL_NUMBER }},
#endif
 { F_CONST|F_DQUICK|F_DRET,    "PRED"      , f_pred ,1,1     ,{PL_NUMBER}},
#ifndef NOGRAPHICS
 { F_DQUICK|F_IRET,    "PTST"     , (pfunc)f_point ,2,2     ,{PL_NUMBER, PL_NUMBER }},
#endif

 { F_CONST|F_DQUICK|F_DRET,    "RAD"      , f_rad ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "RAND"      ,(pfunc) rand ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "RANDOM"    , (pfunc)f_random ,1,1     ,{PL_NUMBER}},
 { F_IQUICK|F_IRET,    "REALLOC"    , (pfunc)f_realloc ,2,2     ,{PL_INT,PL_INT}},
 { F_CONST|F_PLISTE|F_IRET,    "RINSTR"    , (pfunc)f_rinstr ,2,3  ,{PL_STRING,PL_STRING,PL_INT}},
 { F_DQUICK|F_DRET,    "RND"       , f_rnd ,0,1     ,{PL_NUMBER}},
 { F_CONST|F_PLISTE|F_DRET,    "ROUND"     , f_round ,1,2   ,{PL_NUMBER,PL_INT}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "RSRC_GADDR", (pfunc)f_rsrc_gaddr ,2,2   ,{PL_INT,PL_INT}},
#endif

 { F_IQUICK|F_DRET,    "SENSOR"       ,(pfunc) f_sensor ,1,1     ,{PL_INT}},
 { F_CONST|F_DQUICK|F_IRET,    "SGN"       , (pfunc)f_sgn ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_IRET,    "SHL"      , (pfunc)f_shl,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_IQUICK|F_IRET,    "SHM_ATTACH"    , (pfunc)shm_attach ,1,1     ,{PL_INT}},
 { F_IQUICK|F_IRET,    "SHM_MALLOC"    , (pfunc)shm_malloc ,2,2     ,{PL_INT,PL_INT}},
 { F_CONST|F_DQUICK|F_IRET,    "SHR"      , (pfunc)f_shr,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "SIN"       , sin ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "SINH"      , sinh ,1,1     ,{PL_NUMBER}},
 { F_SQUICK|F_IRET,    "SIZE"     , (pfunc)f_size,1,1   ,{PL_STRING}},
 { F_CONST|F_DQUICK|F_DRET,    "SQR"       , sqrt ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "SQRT"      , sqrt ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "SRAND"     ,(pfunc) f_srand ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "SUB"     , f_sub ,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_DQUICK|F_IRET,    "SUCC"      ,(pfunc) f_succ ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_IQUICK|F_IRET,    "SWAP"     , (pfunc)f_swap ,1,1     ,{PL_INT}},
 { F_PLISTE|F_IRET,    "SYM_ADR"   ,(pfunc) f_symadr ,2,2   ,{PL_FILENR,PL_STRING}},

 { F_CONST|F_PLISTE|F_IRET,    "TALLY"     ,(pfunc) f_tally ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},
 { F_CONST|F_DQUICK|F_DRET,    "TAN"       , tan ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "TANH"       , tanh ,1,1     ,{PL_NUMBER}},
#ifdef TINE
 { F_SQUICK|F_DRET,  "TINEGET"     , f_tineget ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "TINEMAX"     , f_tinemax ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "TINEMIN"     , f_tinemin ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "TINESIZE"    , (pfunc)f_tinesize ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "TINETYP"    , (pfunc)f_tinetyp ,1,1   ,{PL_STRING}},
#endif
 { F_CONST|F_DQUICK|F_DRET, "TRUNC"     , trunc ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,       "TYP?"       , (pfunc)type ,1,1     ,{PL_ALLVAR}},

 { F_CONST|F_SQUICK|F_DRET, "VAL"       , f_val ,1,1     ,{PL_STRING}},
 { F_CONST|F_SQUICK|F_IRET, "VAL?"       ,(pfunc) f_valf ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE|F_IRET, "VARIAT"    , (pfunc)f_variat ,2,2     ,{PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,         "VARPTR"    , (pfunc)f_varptr ,1,1     ,{PL_ALLVAR}},

 { F_CONST|F_IQUICK|F_IRET, "WORD"     , (pfunc)f_word ,1,1     ,{PL_INT}},
 { F_PLISTE|F_IRET,         "WORT_SEP" , (pfunc)f_wort_sep ,3,5 ,{PL_STRING,PL_STRING,PL_INT,PL_SVAR,PL_SVAR}},

 { F_CONST|F_IQUICK|F_IRET, "XOR"      , (pfunc) f_xor,2,2     ,{PL_NUMBER,PL_NUMBER}},

};
const int anzpfuncs=sizeof(pfuncs)/sizeof(FUNCTION);

