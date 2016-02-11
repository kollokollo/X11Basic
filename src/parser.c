/* parser.c   Formelparser numerisch und Zeichenketten (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>

#include "config.h"
#include "defs.h"
#include "globals.h"
#include "protos.h"

#ifdef WINDOWS
#undef HAVE_LOGB
#undef HAVE_LOG1P
#undef HAVE_EXPM1
#undef HAVE_FORK
#endif

#ifdef __CYGWIN__  /* David Andersen  11.10.2003*/
#undef HAVE_LOGB
#endif


#ifndef HAVE_LOGB
double logb(double a) {return(log(a)/log(2));}
#endif
#ifndef HAVE_LOG1P
double log1p(double a) {return(log(1+a));}
#endif
#ifndef HAVE_EXPM1
double expm1(double a) {return(exp(a)-1);}
#endif

double f_nop(void *t) {return(0.0);}
STRING vs_error() {
  STRING e;  
  memcpy(e.pointer=malloc(e.len=7),"<ERROR>",7); 
  return(e);
}
#ifndef NOGRAPHICS
int f_point(double v1, double v2) {  return(get_point((int)v1,(int)v2)); }
#endif
int f_bclr(double v1, double v2) { return((int)v1 & ~ (1 <<((int)v2))); }
int f_bset(double v1, double v2) { return((int)v1 | (1 <<((int)v2))); }
int f_bchg(double v1, double v2) { return((int)v1 ^ (1 <<((int)v2))); }
int f_btst(double v1, double v2) { return((((int)v1 & (1 <<((int)v2)))==0) ?  0 : -1); }
int f_shr(double v1, double v2) { return(((int)v1)>>((int)v2)); }
int f_shl(double v1, double v2) { return(((int)v1)<<((int)v2)); }

int f_instr(PARAMETER *,int);
int f_rinstr(PARAMETER *,int);
int f_glob(PARAMETER *,int);
int f_form_alert(PARAMETER *,int);
int f_form_center(PARAMETER *,int);
int f_form_dial(PARAMETER *,int);
int f_form_do(PARAMETER *,int);
int f_objc_draw(PARAMETER *,int);
int f_objc_offset(PARAMETER *,int);
int f_rsrc_gaddr(PARAMETER *,int);
int f_objc_find(PARAMETER *,int);
int f_get_color(PARAMETER *,int);

extern int f_symadr(char *);
extern int f_exec(char *);
extern int shm_malloc(int,int);
extern int shm_attach(int);

int f_int(double b) {return((int)b);}
int f_fix(double b) {if(b>=0) return((int)b);
                      else return(-((int)(-b)));}
double f_pred(double b) {return(ceil(b-1));}
int f_succ(double b) {return((int)(b+1));}
int f_sgn(double b) {return(sgn(b));}
double f_frac(double b) {return(b-((double)((int)b)));}
int f_even(int b) {return(b&1 ? 0:-1);}
int f_odd(int b) {return(b&1 ? -1:0);}
int f_fak(int);
int f_random(double d) {return((int)((double)rand()/RAND_MAX*d));}
double f_rnd(double d) {return((double)rand()/RAND_MAX);}
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
int  f_srand(double d) {srand((int)d);return(0);}
double f_deg(double d) {return(d/PI*180);}
double f_rad(double d) {return(d*PI/180);}
int  f_dimf(char *pos) {return((double)do_dimension(variable_exist_type(pos)));}
int f_asc(STRING n) {  return((int)n.pointer[0]); }
int f_cvi(STRING n) {  return((int)(*((short *)n.pointer))); }
int f_cvl(STRING n) {  return((int)(*((long *)n.pointer))); }
double f_cvd(STRING n) {  return((double)(*((double *)n.pointer))); }
double f_cvf(STRING n) {  return((double)(*((float *)n.pointer))); }
double f_eval(STRING n) {return(parser(n.pointer));}

double f_add(double v1, double v2) {return(v1+v2);}
double f_sub(double v1, double v2) {return(v1-v2);}
double f_mul(double v1, double v2) {return(v1*v2);}
double f_div(double v1, double v2) {return(v1/v2);}
double f_round(PARAMETER *plist,int e) {
  if(e==1) return(round(plist[0].real));
  if(e>=2){
    int kommast=-plist[1].integer;
    return(round(plist[0].real/pow(10,kommast))*pow(10,kommast)); 
  }
}

int f_len(STRING n)    { return(n.len); }
int f_exist(STRING n)  { return(-exist(n.pointer)); }
int f_size(STRING n)   { return(stat_size(n.pointer)); }
int f_nlink(STRING n)  { return(stat_nlink(n.pointer)); }
int f_device(STRING n) { return(stat_device(n.pointer)); }
int f_inode(STRING n)  { return(stat_inode(n.pointer)); }
int f_mode(STRING n)   { return(stat_mode(n.pointer)); }

double f_val(STRING n) { return((double)atof(n.pointer)); }
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
double f_ltextlen(STRING n) { return((double)ltextlen(ltextxfaktor,ltextpflg,n.pointer)); }
int f_gray(int n) { /* Gray-Code-Konversion */
  unsigned int i=1,a,d;
  if(n>=0) return(n^(n>>1));
  for(a=-n;;i<<=1) {
    a^=(d=a>>i);
    if(d<=1||i==16) return(a);
  }
}
char *arrptr(char *);
#ifdef TINE
double f_tinemax(STRING n) { return(tinemax(n.pointer)); }
double f_tinemin(STRING n) { return(tinemin(n.pointer)); }
double f_tineget(STRING n) { return(tineget(n.pointer)); }
int f_tinesize(STRING n) { return(tinesize(n.pointer)); }
int f_tinetyp(STRING n) { return(tinetyp(n.pointer)); }
#endif
#ifdef CONTROL
double f_csmax(STRING n) { return(csmax(n.pointer)); }
double f_csmin(STRING n) { return(csmin(n.pointer)); }
double f_csres(STRING n) { return(csres(n.pointer)); }
double f_csget(STRING n) { return(csget(n.pointer)); }
int f_cssize(STRING n) { return(cssize(n.pointer)); }
int f_cspid(STRING n) { return(cspid(n.pointer)); }
#endif
int f_malloc(int size) {return((int)malloc(size));}
int f_realloc(int adr,int size) {return((int)realloc((char *)adr,size));}
int f_peek(int adr) { return((int)(*(char *)adr));}
int f_dpeek(int adr) { return((int)(*(short *)adr));}
int f_lpeek(int adr) { return((int)(*(long *)adr));}
int f_lof(PARAMETER *plist,int e) {
  if(filenr[plist[0].integer]) return(lof(dptr[plist[0].integer]));
  else { error(24,""); return(0);} /* File nicht geoeffnet */
}
int f_loc(PARAMETER *plist,int e) {
  if(filenr[plist[0].integer]) return(ftell(dptr[plist[0].integer]));
  else { error(24,""); return(0);} /* File nicht geoeffnet */
}
int f_eof(PARAMETER *plist,int e) {
  if(plist[0].integer==-2) return((myeof(stdin))?-1:0);
  else if(filenr[plist[0].integer]) {
    fflush(dptr[plist[0].integer]);
    return(myeof(dptr[plist[0].integer])?-1:0);
  } else { error(24,""); return(0);} /* File nicht geoeffnet */
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
int lsel_input(char *,STRING *,int,int);

int f_listselect(PARAMETER *plist,int e) { 
  int sel=-1;
  if(e==3) sel=plist[2].integer;
  if(e>=2) {
    ARRAY a;
    a.pointer=plist[1].pointer;
    a.dimension=plist[1].integer;
    a.typ=plist[1].typ;
    return(lsel_input(plist[0].pointer,(STRING *)(a.pointer+a.dimension*INTSIZE),anz_eintraege(a),sel));
  } 
}


const FUNCTION pfuncs[]= {  /* alphabetisch !!! */

 { F_ARGUMENT|F_DRET,  "!nulldummy", f_nop ,0,0   ,{0}},
 { F_DQUICK|F_DRET,    "ABS"       , fabs ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "ACOS"      , acos ,1,1     ,{PL_NUMBER}},
#ifndef WINDOWS
 { F_DQUICK|F_DRET,    "ACOSH"      , acosh ,1,1     ,{PL_NUMBER}},
#endif
 { F_DQUICK|F_DRET,    "ADD"     , f_add ,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "ARRPTR"    , arrptr ,1,1     ,{PL_ARRAY}},
 { F_SQUICK|F_IRET,    "ASC"       , f_asc ,1,1   ,{PL_STRING}},
 { F_DQUICK|F_DRET,    "ASIN"      , asin ,1,1     ,{PL_NUMBER}},
#ifndef WINDOWS
 { F_DQUICK|F_DRET,    "ASINH"      , asinh ,1,1     ,{PL_NUMBER}},
#endif
 { F_DQUICK|F_DRET,    "ATAN"      , atan ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "ATAN2"     , atan2 ,2,2     ,{PL_NUMBER,PL_NUMBER}},
#ifndef WINDOWS
 { F_DQUICK|F_DRET,    "ATANH"     , atanh ,1,1     ,{PL_NUMBER}},
#endif
 { F_DQUICK|F_DRET,    "ATN"       , atan ,1,1     ,{PL_NUMBER}},

 { F_DQUICK|F_IRET,    "BCHG"      , f_bchg,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_DQUICK|F_IRET,    "BCLR"      , f_bclr,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_DQUICK|F_IRET,    "BSET"      , f_bset,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_DQUICK|F_IRET,    "BTST"      , f_btst,2,2     ,{PL_NUMBER,PL_NUMBER }},

#ifndef WINDOWS
 { F_DQUICK|F_DRET,    "CBRT"      , cbrt ,1,1     ,{PL_NUMBER}},
#endif
 { F_DQUICK|F_DRET,    "CEIL"      , ceil ,1,1     ,{PL_NUMBER}},
 { F_PLISTE|F_IRET,    "COMBIN"    , f_combin ,2,2     ,{PL_INT,PL_INT}},
 { F_DQUICK|F_DRET,    "COS"       , cos ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "COSH"      , cosh ,1,1     ,{PL_NUMBER}},
#ifdef CONTROL
 { F_SQUICK|F_DRET,  "CSGET"     , f_csget ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSMAX"     , f_csmax ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSMIN"     , f_csmin ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CSPID"     , f_cspid ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSRES"     , f_csres ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CSSIZE"    , f_cssize ,1,1   ,{PL_STRING}},
#endif
 { F_SQUICK|F_DRET,  "CVD"       , f_cvd ,1,1     ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CVF"       , f_cvf ,1,1     ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CVI"       , f_cvi ,1,1     ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CVL"       , f_cvl ,1,1     ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CVS"       , f_cvf ,1,1     ,{PL_STRING}},

 { F_DQUICK|F_DRET,    "DEG"       , f_deg ,1,1     ,{PL_NUMBER}},
 { F_SQUICK|F_IRET,    "DEVICE"    , f_device,1,1   ,{PL_STRING}},
 { F_ARGUMENT|F_IRET,  "DIM?"      , f_dimf ,1,1      ,{PL_ARRAY}},
 { F_DQUICK|F_DRET,    "DIV"       , f_div ,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_IQUICK|F_IRET,    "DPEEK"     , f_dpeek ,1,1     ,{PL_INT}},

 { F_PLISTE|F_IRET,    "EOF"       , f_eof ,1,1     ,{PL_FILENR}},

 { F_SQUICK|F_DRET,  "EVAL"      , f_eval ,1,1      ,{PL_STRING}},
 { F_IQUICK|F_IRET,    "EVEN"       , f_even ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "EXEC"       , f_exec ,1,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_SQUICK|F_IRET,    "EXIST"      , f_exist ,1,1     ,{PL_STRING}},
 { F_DQUICK|F_DRET,    "EXP"       , exp ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "EXPM1"     , expm1 ,1,1     ,{PL_NUMBER}},

 { F_IQUICK|F_IRET,    "FACT"       , f_fak ,1,1     ,{PL_INT}},
 { F_DQUICK|F_IRET,    "FIX"       , f_fix ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "FLOOR"     , floor ,1,1     ,{PL_NUMBER}},
#ifdef HAVE_FORK
 { F_SIMPLE|F_IRET,    "FORK"     , fork ,0,0     },
#endif
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "FORM_ALERT", f_form_alert ,2,2   ,{PL_INT,PL_STRING}},
 { F_PLISTE|F_IRET,    "FORM_CENTER", f_form_center ,5,5   ,{PL_INT,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { F_PLISTE|F_IRET,    "FORM_DIAL", f_form_dial ,9,9   ,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,    "FORM_DO",   f_form_do ,1,1   ,{PL_INT}},
#endif
 { F_DQUICK|F_DRET,    "FRAC"      , f_frac ,1,1     ,{PL_NUMBER}},
 { F_SIMPLE|F_IRET,    "FREEFILE"  , f_freefile ,0,0  },

 { F_DQUICK|F_DRET,    "GASDEV"   , f_gasdev ,1,1     ,{PL_NUMBER}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "GET_COLOR", f_get_color ,3,3   ,{PL_INT,PL_INT,PL_INT}},
#endif
 { F_PLISTE|F_IRET,    "GLOB"     , f_glob ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},
 { F_IQUICK|F_IRET,    "GRAY"     , f_gray ,1,1     ,{PL_INT}},

 { F_DQUICK|F_DRET,    "HYPOT"     , hypot ,2,2     ,{PL_NUMBER,PL_NUMBER}},

 { F_SQUICK|F_IRET,    "INODE"     , f_inode,1,1   ,{PL_STRING}},
 { F_ARGUMENT|F_IRET,  "INP"       , inp8 ,1,1      ,{PL_FILENR}},
 { F_ARGUMENT|F_IRET,  "INP?"      , inpf ,1,1      ,{PL_FILENR}},
 { F_ARGUMENT|F_IRET,  "INP&"      , inp16 ,1,1      ,{PL_FILENR}},
 { F_ARGUMENT|F_IRET,  "INP%"      , inp32 ,1,1      ,{PL_FILENR}},
 { F_PLISTE|F_IRET,  "INSTR"     , f_instr ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},

 { F_DQUICK|F_IRET,    "INT"       , f_int ,1,1     ,{PL_NUMBER}},
 { F_SQUICK|F_IRET,    "JULIAN"    , f_julian ,1,1     ,{PL_STRING}},

 { F_SQUICK|F_IRET,    "LEN"       , f_len ,1,1   ,{PL_STRING}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "LISTSELECT", f_listselect ,2,3   ,{PL_STRING,PL_SARRAY,PL_INT}},
#endif
 { F_DQUICK|F_DRET,    "LN"        , log ,1,1     ,{PL_NUMBER}},

 { F_PLISTE|F_IRET,    "LOC"       , f_loc ,1,1     ,{PL_FILENR}},
 { F_PLISTE|F_IRET,    "LOF"       , f_lof ,1,1     ,{PL_FILENR}},

 { F_DQUICK|F_DRET,    "LOG"       , log ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "LOG10"     , log10 ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "LOG1P"     , log1p ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "LOGB"      , logb  ,1,1     ,{PL_NUMBER}},
 { F_IQUICK|F_IRET,    "LPEEK"    , f_lpeek ,1,1     ,{PL_INT}},
 { F_SQUICK|F_DRET,    "LTEXTLEN"  , f_ltextlen ,1,1   ,{PL_STRING}},

 { F_IQUICK|F_IRET,    "MALLOC"    , f_malloc ,1,1     ,{PL_INT}},
 { F_PLISTE|F_DRET,    "MAX"     , f_max ,1,-1     ,{PL_NUMBER,PL_NUMBER,PL_NUMBER}},
 { F_PLISTE|F_DRET,    "MIN"     , f_min ,1,-1     ,{PL_NUMBER,PL_NUMBER,PL_NUMBER}},
 { F_DQUICK|F_DRET,    "MOD"     , fmod ,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_SQUICK|F_IRET,    "MODE"     , f_mode,1,1   ,{PL_STRING}},
 { F_DQUICK|F_DRET,    "MUL"     , f_mul ,2,2     ,{PL_NUMBER,PL_NUMBER}},

 { F_SQUICK|F_IRET,    "NLINK"     , f_nlink,1,1   ,{PL_STRING}},

#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "OBJC_DRAW", f_objc_draw ,5,5   ,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,    "OBJC_FIND", f_objc_find ,3,3   ,{PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,    "OBJC_OFFSET", f_objc_offset ,4,4,{PL_INT,PL_INT,PL_NVAR,PL_NVAR}},
#endif
 { F_IQUICK|F_IRET,    "ODD"       , f_odd ,1,1     ,{PL_NUMBER}},

 { F_IQUICK|F_IRET,    "PEEK"      , f_peek ,1,1     ,{PL_INT}},
#ifndef NOGRAPHICS
 { F_DQUICK|F_IRET,    "POINT"     , f_point ,2,2     ,{PL_NUMBER, PL_NUMBER }},
#endif
 { F_DQUICK|F_DRET,    "PRED"      , f_pred ,1,1     ,{PL_NUMBER}},
#ifndef NOGRAPHICS
 { F_DQUICK|F_IRET,    "PTST"     , f_point ,2,2     ,{PL_NUMBER, PL_NUMBER }},
#endif

 { F_DQUICK|F_DRET,    "RAD"      , f_rad ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "RAND"      , rand ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "RANDOM"    , f_random ,1,1     ,{PL_NUMBER}},
 { F_IQUICK|F_IRET,    "REALLOC"    , f_realloc ,2,2     ,{PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,    "RINSTR"    , f_rinstr ,2,3  ,{PL_STRING,PL_STRING,PL_INT}},
 { F_DQUICK|F_DRET,    "RND"       , f_rnd ,1,1     ,{PL_NUMBER}},
 { F_PLISTE|F_DRET,    "ROUND"     , f_round ,1,2   ,{PL_NUMBER,PL_INT}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "RSRC_GADDR", f_rsrc_gaddr ,2,2   ,{PL_INT,PL_INT}},
#endif

 { F_DQUICK|F_IRET,    "SGN"       , f_sgn ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "SHL"      , f_shl,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_IQUICK|F_IRET,    "SHM_ATTACH"    , shm_attach ,1,1     ,{PL_INT}},
 { F_IQUICK|F_IRET,    "SHM_MALLOC"    , shm_malloc ,2,2     ,{PL_INT,PL_INT}},
 { F_DQUICK|F_IRET,    "SHR"      , f_shr,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SIN"       , sin ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SINH"      , sinh ,1,1     ,{PL_NUMBER}},
 { F_SQUICK|F_IRET,    "SIZE"     , f_size,1,1   ,{PL_STRING}},
 { F_DQUICK|F_DRET,    "SQR"       , sqrt ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SQRT"      , sqrt ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "SRAND"     , f_srand ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SUB"     , f_sub ,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_DQUICK|F_IRET,    "SUCC"      , f_succ ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "SYM_ADR"   , f_symadr ,2,2     ,{PL_FILENR,PL_STRING}},

 { F_DQUICK|F_DRET,    "TAN"       , tan ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "TANH"       , tanh ,1,1     ,{PL_NUMBER}},
#ifdef TINE
 { F_SQUICK|F_DRET,  "TINEGET"     , f_tineget ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "TINEMAX"     , f_tinemax ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "TINEMIN"     , f_tinemin ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "TINESIZE"    , f_tinesize ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "TINETYP"    , f_tinetyp ,1,1   ,{PL_STRING}},
#endif
 { F_DQUICK|F_DRET,    "TRUNC"     , f_fix ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "TYP?"       , type2 ,1,1     ,{PL_ALL}},

 { F_SQUICK|F_DRET,  "VAL"       , f_val ,1,1     ,{PL_STRING}},
 { F_ARGUMENT|F_IRET,  "VARPTR"    , varptr ,1,1     ,{PL_ALL}},

 { F_ARGUMENT|F_IRET,  "WORT_SEP"    , do_wort_sep ,3,-1     ,{PL_STRING,0}},
 
};
const int anzpfuncs=sizeof(pfuncs)/sizeof(FUNCTION);
   
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
STRING f_trims(STRING n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  xtrim(n.pointer,0,ergebnis.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_xtrims(STRING n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  xtrim(n.pointer,1,ergebnis.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_envs(STRING n) {   
  STRING ergebnis;
  char *ttt=getenv(n.pointer);
  if(ttt==NULL) {
    ergebnis.pointer=malloc(1);
    *ergebnis.pointer=0;
  } else {
    ergebnis.pointer=malloc(strlen(ttt)+1);
    strcpy(ergebnis.pointer,ttt);
  }
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
#ifdef CONTROL
STRING f_csgets(STRING n) {   
  STRING ergebnis;
  ergebnis.pointer=csgets(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_csunits(STRING n) {   
  STRING ergebnis;
  ergebnis.pointer=csunit(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_cspnames(int n) {   
  STRING ergebnis;
  ergebnis.pointer=cspname(n);
  ergebnis.len=strlen(ergebnis.pointer);  
  return(ergebnis);
}
#endif
#ifdef TINE
STRING f_tinegets(STRING n) {   
  STRING ergebnis;
  ergebnis=tinegets(n.pointer);
  return(ergebnis);
}
STRING f_tineunits(STRING n) {   
  STRING ergebnis;
  ergebnis.pointer=tineunit(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_tineinfos(STRING n) {   
  STRING ergebnis;
  ergebnis.pointer=tineinfo(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_tinequerys(PARAMETER *plist,int e) {
  STRING ergebnis;
  if(e>=2) {
    ergebnis=tinequery(plist[0].pointer,plist[1].integer);
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  return(ergebnis);
}
#endif
STRING f_terminalnames(char *n) {   
  STRING ergebnis;
  int i=get_number(n);  
  if(filenr[i]) ergebnis.pointer=terminalname(fileno(dptr[i]));
  else {
    error(24,""); /* File nicht geoeffnet */
    return(vs_error());
  }
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_systems(STRING n) {   
  STRING ergebnis;
  FILE *dptr=popen(n.pointer,"r");

  if (dptr==NULL) {
    io_error(errno,"popen");
    ergebnis.pointer=malloc(38+n.len);
    sprintf(ergebnis.pointer,"couldn't execute '%s'. errno=%d",n.pointer,errno);
  } else {
    int len=0;
    int limit=1024;
    char c;
    ergebnis.pointer=NULL;
    do {
      ergebnis.pointer=realloc(ergebnis.pointer,limit);
     /* printf("Bufferlaenge: %d Bytes.\n",limit); */
      while(len<limit) {
        c=fgetc(dptr);
        if(c==EOF) {
          ergebnis.pointer[len]='\0';
          break;
        }
        ergebnis.pointer[len++]=c;
      }
      limit+=len;
    } while(c!=EOF);
    if(pclose(dptr)==-1) io_error(errno,"pclose");
  }
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_inlines(STRING n) {   
  STRING ergebnis;
  char *pos1=n.pointer;
  char *pos2;
  ergebnis.len=n.len*3/4;
  pos2=ergebnis.pointer=malloc(ergebnis.len);
  while(pos2-ergebnis.pointer<ergebnis.len-2) {
  pos2[0]=(((pos1[0]-36) & 0x3f)<<2)|(((pos1[1]-36) & 0x30)>>4);
  pos2[1]=(((pos1[1]-36) & 0xf)<<4)|(((pos1[2]-36) & 0x3c)>>2);
  pos2[2]=(((pos1[2]-36) & 0x3)<<6)|(((pos1[3]-36) & 0x3f));
  pos2+=3;
  pos1+=4;
  }
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
STRING f_errs(int n) {   
  STRING ergebnis;
  char *test=error_text((char)n,"");
  ergebnis.pointer=malloc(strlen(test)+1);
  ergebnis.len=strlen(test);
  strcpy(ergebnis.pointer,test);
  return(ergebnis);
}
STRING f_prgs(int n) {   
  STRING ergebnis;
  char *test=program[min(prglen-1,max(n,0))];
  ergebnis.pointer=malloc(strlen(test)+1);
  ergebnis.len=strlen(test);
  strcpy(ergebnis.pointer,test);
  if(n>=prglen || n<0) error(16,""); /* Feldindex zu gross*/ 
  return(ergebnis);
}
STRING f_params(int n) {   
  STRING ergebnis;
  char *test=param_argumente[min(param_anzahl-1,max(n,0))];
  ergebnis.pointer=malloc(strlen(test)+1);
  ergebnis.len=strlen(test);
  strcpy(ergebnis.pointer,test);
  if(n>=param_anzahl || n<0) ergebnis.len=0;
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
STRING f_unixdates(int n) {   
  STRING ergebnis;
  struct tm * loctim;
  loctim=localtime((time_t *)(&n));
  ergebnis.pointer=malloc(12);
  sprintf(ergebnis.pointer,"%02d.%02d.%04d",loctim->tm_mday,loctim->tm_mon+1,1900+loctim->tm_year);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_unixtimes(int n) {   
  STRING ergebnis;
  struct tm * loctim;
  loctim=localtime((time_t *)&n);
  ergebnis.pointer=malloc(16);
  sprintf(ergebnis.pointer,"%02d:%02d:%02d",loctim->tm_hour,loctim->tm_min,loctim->tm_sec);
  ergebnis.len=strlen(ergebnis.pointer);
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
STRING f_strs(PARAMETER *plist,int e) {         /* STR$(a[,b[,c[,d]]])     */
  STRING ergebnis;
  int b=-1,c=13,mode=0;
  char formatter[24];
  ergebnis.pointer=malloc(64);
  if(e>=1) {
    if(e>=2) b=min(50,max(0,plist[1].integer));
    if(e>=3) c=min(50,max(0,plist[2].integer));
    if(e>=4) mode=plist[3].integer;
    if(mode==0 && b!=-1) sprintf(formatter,"%%%d.%dg",b,c);
    else if (mode==1 && b!=-1) sprintf(formatter,"%%0%d.%dg",b,c);
    else  sprintf(formatter,"%%.13g");
    sprintf(ergebnis.pointer,formatter,plist[0].real);
  }
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_bins(PARAMETER *plist,int e) {
  STRING ergebnis;
  unsigned int a;
  int j,b=8,i=0;
  ergebnis.pointer=malloc(64);
  if(e>=1) {
    a=plist[0].integer;
    if(e==2) b=min(32,max(0,plist[1].integer));
    for(j=b;j>0;j--) ergebnis.pointer[i++]=((a&(1<<(j-1)))  ? '1':'0');
    ergebnis.pointer[i]=0;
  }
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_hexs(char *pos) {
  STRING ergebnis;
  ergebnis.pointer=hexoct_to_string('x',pos);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_octs(char *pos) {
  STRING ergebnis;
  ergebnis.pointer=hexoct_to_string('o',pos);
  ergebnis.len=strlen(ergebnis.pointer);
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

/* Systemvariablen vom typ String */

int v_false() {return(0);}
int v_true() {return(-1);}
int v_err() { extern int globalerr; return(globalerr);}
int v_ccserr() {return(ccs_err);}
#ifdef CONTROL
int v_ccsaplid() {return(aplid);}
#endif
int v_sp() {return(sp);}
int v_pc() {return(pc);}
double v_timer() {
#ifdef WINDOWS
#if 0
       return((double)GetTickCount()/1000.0);
#else
       return((double)clock()/CLOCKS_PER_SEC);
#endif
#else
        struct timespec t;
	struct {
               int  tz_minuteswest; /* minutes W of Greenwich */
               int  tz_dsttime;     /* type of dst correction */
       } tz;
	gettimeofday(&t,&tz);
	return((double)t.tv_sec+(double)t.tv_nsec/1000000);      
#endif
}
int v_stimer() {   /* Sekunden-Timer */
  time_t timec=time(NULL);
  if(timec==-1) io_error(errno,"TIMER");
  return(timec);
}
extern int getrowcols(int *, int *);

int v_cols() {   /* Anzahl Spalten des Terminals */
  int rows=0,cols=0;
  getrowcols(&rows,&cols);
  return(cols);
}
int v_rows() {   /* Anzahl Zeilen des Terminals */
  int rows=0,cols=0;
  getrowcols(&rows,&cols);
  return(rows);
}

double v_ctimer() {return((double)clock()/CLOCKS_PER_SEC);}
double v_pi() {return(PI);}
extern int mousex(),mousey(), mousek(), mouses();
const SYSVAR sysvars[]= {  /* alphabetisch !!! */
 { PL_LEER,   "!nulldummy", v_false},
#ifdef CONTROL
 { PL_INT,    "CCSAPLID",   v_ccsaplid},
#endif
 { PL_INT,    "CCSERR",     v_ccserr},
 { PL_INT,    "COLS",       v_cols},
 { PL_FLOAT,  "CTIMER",     v_ctimer},
 { PL_INT,    "ERR",        v_err},
 { PL_INT,    "FALSE",      v_false},
#ifndef NOGRAPHICS
 { PL_INT,    "MOUSEK",     mousek},
 { PL_INT,    "MOUSES",     mouses},
 { PL_INT,    "MOUSEX",     mousex},
 { PL_INT,    "MOUSEY",     mousey},
#endif
 { PL_INT,    "PC",         v_pc},
 { PL_FLOAT,  "PI",         v_pi},
 { PL_INT,    "ROWS",       v_rows},
 { PL_INT,    "SP",         v_sp},
 { PL_INT,    "STIMER",     v_stimer},
 { PL_FLOAT,  "TIMER",      v_timer},
 { PL_INT,    "TRUE",       v_true},
#ifdef WINDOWS
 { PL_INT,    "WIN32?",     v_true},
#endif
#ifndef WINDOWS
 { PL_INT,    "UNIX?",      v_true},
#endif
};
const int anzsysvars=sizeof(sysvars)/sizeof(SYSVAR);

STRING vs_date() {
  STRING ergebnis;
  time_t timec;
  struct tm * loctim;
  timec = time(&timec);
  loctim=localtime(&timec);
  ergebnis.pointer=malloc(12);
  sprintf(ergebnis.pointer,"%02d.%02d.%04d",loctim->tm_mday,loctim->tm_mon+1,1900+loctim->tm_year);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING vs_time() {
  STRING ergebnis;
  time_t timec;
  struct tm * loctim;
  timec = time(&timec);
  loctim=localtime(&timec);
  ergebnis.pointer=malloc(9);
  strncpy(ergebnis.pointer,ctime(&timec)+11,8);
  ergebnis.len=8;
  return(ergebnis);
}
STRING vs_trace() {
  STRING ergebnis;
  if(pc>=0 && pc<prglen) {
  ergebnis.pointer=malloc(strlen(program[pc])+1);
  strcpy(ergebnis.pointer,program[pc]);
  xtrim(ergebnis.pointer,TRUE,ergebnis.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  } else ergebnis=vs_error();
  return(ergebnis);
}
STRING vs_terminalname() {
  STRING ergebnis;
  ergebnis.pointer=terminalname(STDIN_FILENO);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING vs_inkey() {
  STRING ergebnis;
  char *t=inkey();
  ergebnis.pointer=malloc(strlen(t)+1);
  strcpy(ergebnis.pointer,t);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}

const SYSSVAR syssvars[]= {  /* alphabetisch !!! */
 { PL_LEER,   "!nulldummy", vs_error},
 { PL_STRING, "DATE$", vs_date},
 { PL_STRING, "INKEY$", vs_inkey},
 { PL_STRING, "TERMINALNAME$", vs_terminalname},
 { PL_STRING, "TIME$", vs_time},
 { PL_STRING, "TRACE$", vs_trace},
};
const int anzsyssvars=sizeof(syssvars)/sizeof(SYSSVAR);

const SFUNCTION psfuncs[]= {  /* alphabetisch !!! */

 { F_ARGUMENT,  "!nulldummy", f_nop ,0,0   ,{0}},
 { F_PLISTE,  "BIN$"    , f_bins ,1,2   ,{PL_INT,PL_INT}},

 { F_IQUICK,    "CHR$"    , f_chrs ,1,1   ,{PL_NUMBER}},
#ifdef CONTROL
 { F_SQUICK,    "CSGET$"    , f_csgets ,1,1   ,{PL_STRING}},
 { F_IQUICK,    "CSPNAME$"  , f_cspnames ,1,1   ,{PL_INT}},
 { F_SQUICK,    "CSUNIT$"   , f_csunits ,1,1   ,{PL_STRING}},
#endif

 { F_SQUICK,    "ENV$"    , f_envs ,1,1   ,{PL_STRING}},
 { F_IQUICK,    "ERR$"    , f_errs ,1,1   ,{PL_NUMBER}},
 { F_ARGUMENT,  "HEX$"    , f_hexs ,1,4   ,{PL_INT,PL_INT,PL_NUMBER,PL_NUMBER}},
 { F_SQUICK,    "INLINE$" , f_inlines ,1,1   ,{PL_STRING}},
 { F_ARGUMENT,  "INPUT$"  , f_inputs ,1,2   ,{PL_FILENR,PL_NUMBER}},
 { F_IQUICK,    "JULDATE$" , f_juldates ,1,1   ,{PL_INT}},

 { F_PLISTE,    "LEFT$" , f_lefts ,1,2   ,{PL_STRING,PL_INT}},
 { F_ARGUMENT,  "LINEINPUT$" , f_lineinputs ,1,1   ,{PL_FILENR}},
 { F_SQUICK,    "LOWER$"    , f_lowers ,1,1   ,{PL_STRING}},

 { F_PLISTE,    "MID$"    , f_mids ,2,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_DQUICK,    "MKD$"    , f_mkds ,1,1   ,{PL_NUMBER}},
 { F_DQUICK,    "MKF$"    , f_mkfs ,1,1   ,{PL_NUMBER}},
 { F_IQUICK,    "MKI$"    , f_mkis ,1,1   ,{PL_INT}},
 { F_IQUICK,    "MKL$"    , f_mkls ,1,1   ,{PL_INT}},
 { F_DQUICK,    "MKS$"    , f_mkfs ,1,1   ,{PL_NUMBER}},
 { F_ARGUMENT,  "OCT$"    , f_octs ,1,4   ,{PL_INT,PL_INT,PL_NUMBER,PL_NUMBER}},
 
 { F_IQUICK,    "PARAM$"  , f_params ,1,1   ,{PL_INT}},
 { F_IQUICK,    "PRG$"    , f_prgs ,1,1   ,{PL_INT}},
 { F_PLISTE,    "REPLACE$"  , f_replaces ,3,3   ,{PL_STRING,PL_STRING,PL_STRING}},
 { F_PLISTE,    "RIGHT$"  , f_rights ,1,2   ,{PL_STRING,PL_INT}},
 { F_IQUICK,    "SPACE$"  , f_spaces ,1,1   ,{PL_INT}},
 { F_PLISTE,  "STR$"    , f_strs ,1,4   ,{PL_NUMBER,PL_INT,PL_INT,PL_INT}},
 { F_PLISTE,  "STRING$" , f_strings ,2,2   ,{PL_INT,PL_STRING}},
 { F_SQUICK,    "SYSTEM$"    , f_systems ,1,1   ,{PL_STRING}},
 { F_SQUICK,    "TERMINALNAME$"    , f_terminalnames ,1,1 ,{PL_FILENR}},
#ifdef TINE
 { F_SQUICK,    "TINEGET$"    , f_tinegets ,1,1   ,{PL_STRING}},
 { F_SQUICK,    "TINEINFO$"   , f_tineinfos ,1,1   ,{PL_STRING}},
 { F_PLISTE,    "TINEQUERY$"  , f_tinequerys ,2,2   ,{PL_STRING,PL_INT}},
 { F_SQUICK,    "TINEUNIT$"   , f_tineunits ,1,1   ,{PL_STRING}},
#endif
 { F_SQUICK,    "TRIM$"   , f_trims ,1,1   ,{PL_STRING}},

 { F_SQUICK,    "UCASE$"    , f_uppers ,1,1   ,{PL_STRING}},
 { F_IQUICK,    "UNIXDATE$" , f_unixdates ,1,1   ,{PL_INT}},
 { F_IQUICK,    "UNIXTIME$" , f_unixtimes ,1,1   ,{PL_INT}},
 { F_SQUICK,    "UPPER$"    , f_uppers ,1,1   ,{PL_STRING}},
 { F_SQUICK,    "XTRIM$"   , f_xtrims ,1,1   ,{PL_STRING}},

};
const int anzpsfuncs=sizeof(psfuncs)/sizeof(SFUNCTION);

int f_fak(int k) {
  int i,s=1;
  for(i=2;i<=k;i++) {s=s*i;} 
  return(s);
}

int vergleich(char *w1,char *w2) {
  int v;
  int e=type2(w1);
  if((e | INTTYP | FLOATTYP)!=(type2(w2) | INTTYP | FLOATTYP )) {
    puts("Typen ungleich bei Vergleich!");
    printf("1: %d    2: %d \n",type2(w1),type2(w2));
    return(-1);
  }
  if(e & ARRAYTYP) { 
    puts("Arrays an dieser Stelle noch nicht möglich.");
    return(0);
  }
  else if(e & STRINGTYP) { 
    STRING a,b;
    a=string_parser(w1);
    b=string_parser(w2);
    v=(a.len-b.len);
    if (v==0) v=memcmp(a.pointer,b.pointer,a.len);
    free(a.pointer);free(b.pointer);
  } else {
    double x=(parser(w1)-parser(w2));
    if(x==0) return(0);
    else if(x<0) return(-1);
    else return(1);
  }
  return(v);
}
int f_instr(PARAMETER *plist,int e) {  
  int start=1;
  char *pos=NULL;
  if(e>=2) {
    if(e==3) start=min(plist[0].integer,max(1,plist[2].integer));
    pos=(char *)memmem(&(((char *)(plist[0].pointer))[start-1]),plist[0].integer-start+1,plist[1].pointer,plist[1].integer);
    if(pos!=NULL) return((int)(pos-(char *)plist[0].pointer)+1);
  } return(0);
}
int f_rinstr(PARAMETER *plist,int e) {
  char *pos=NULL,*n;
  int start;
  if(e>=2) {
    start=plist[0].integer;
    if(e==3) start=min(plist[0].integer,max(1,plist[2].integer));
    pos=rmemmem(plist[0].pointer,start-1,plist[1].pointer,plist[1].integer);
    if(pos!=NULL) return((int)(pos-(char *)plist[0].pointer)+1);
  } return(0);
}
#ifndef WINDOWS
  #include <fnmatch.h>
#else 
  #include "Windows.extension/fnmatch.h"
#endif
int f_glob(PARAMETER *plist,int e) {
  char *pos=NULL,*n;
  int flags=FNM_NOESCAPE;
  if(e>=2) {
    if(e==3) flags^=plist[2].integer;
    flags=fnmatch(plist[1].pointer,plist[0].pointer,flags);
    if(flags==0) return(-1);
  } return(0);
}
#ifndef NOGRAPHICS
int f_form_alert(PARAMETER *plist,int e) {
  if(e==2) return(form_alert(plist[0].integer,plist[1].pointer));
  else return(-1);
}
int f_form_center(PARAMETER *plist,int e) {
  int x,y,w,h;
  graphics();
  gem_init();
  if(e==1) return(form_center(plist[0].integer,&x,&y,&w,&h));
  else if(e==5) {
    e=form_center(plist[0].integer,&x,&y,&w,&h);
    if(plist[1].typ!=PL_LEER) varcastint(plist[1].integer,plist[1].pointer,x);
    if(plist[2].typ!=PL_LEER) varcastint(plist[2].integer,plist[2].pointer,y);
    if(plist[3].typ!=PL_LEER) varcastint(plist[3].integer,plist[3].pointer,w);
    if(plist[4].typ!=PL_LEER) varcastint(plist[4].integer,plist[4].pointer,h);
    return(e);
  } else return(-1);
}
int f_form_dial(PARAMETER *plist,int e) {
  if(e==9) {
    graphics();
    gem_init();
    return(form_dial(plist[0].integer,plist[1].integer,
  plist[2].integer,plist[3].integer,plist[4].integer,plist[5].integer,
  plist[6].integer,plist[7].integer,plist[8].integer));
  } else return(-1);
}
int f_form_do(PARAMETER *plist,int e) {
  if(e==1) {
    graphics();
    gem_init();
    return(form_do((char *)plist[0].integer));
  } else return(-1);
}
int f_objc_draw(PARAMETER *plist,int e) {
  if(e==5) {
    graphics();
    gem_init();
    return(objc_draw((char *)plist[0].integer,plist[1].integer
    ,plist[2].integer,plist[3].integer,plist[4].integer));
  } else return(-1);
}
int f_objc_find(PARAMETER *plist,int e) {
  if(e==3) {
    return(objc_find((char *)plist[0].integer,plist[1].integer
    ,plist[2].integer));
  } else return(-1);
}
int f_objc_offset(PARAMETER *plist,int e) {
  int x,y;
  if(e==4) {
    if(plist[2].integer&FLOATTYP) x=(int)*((double *)plist[2].pointer);
    else if(plist[2].integer&INTTYP) x=*((int *)plist[2].pointer);
    else error(58,""); /* Variable hat falschen Typ */
    if(plist[3].integer&FLOATTYP) y=(int)*((double *)plist[3].pointer);
    else if(plist[3].integer&INTTYP) y=*((int *)plist[3].pointer);
    else error(58,""); /* Variable hat falschen Typ */
    e=objc_offset((char *)plist[0].integer,plist[1].integer,&x,&y);
    if(plist[2].integer&FLOATTYP) *((double *)plist[2].pointer)=(double)x;
    else if(plist[2].integer&INTTYP) *((int *)plist[2].pointer)=x;
     if(plist[3].integer&FLOATTYP) *((double *)plist[3].pointer)=(double)y;
    else if(plist[3].integer&INTTYP) *((int *)plist[3].pointer)=y;
    return(e);
  } else return(-1);
}
int f_get_color(PARAMETER *plist,int e) {
  if(e==3) {
    graphics();
    return(get_color(plist[0].integer,plist[1].integer,plist[2].integer));
  } else return(-1);
}
int f_rsrc_gaddr(PARAMETER *plist,int e) {
  int i;
  char *ptr;
  if(e==2) {
    i=rsrc_gaddr(plist[0].integer,plist[1].integer,&ptr);
    if(i>0) return((int)ptr);
  } return(-1);
}
#endif
double parser(char *funktion){  /* Rekursiver num. Parser */
  char *pos,*pos2;
  char s[strlen(funktion)+1],w1[strlen(funktion)+1],w2[strlen(funktion)+1];
  int e,vnr;

  /* printf("Parser: <%s>\n");*/

  /* Logische Operatoren AND OR NOT ... */
   
  if(searchchr2_multi(funktion,"&|")!=NULL) {
    if(wort_sepr2(s,"&&",TRUE,w1,w2)>1)     return((double)((int)parser(w1) & (int)parser(w2)));    
    if(wort_sepr2(s,"||",TRUE,w1,w2)>1)     return((double)((int)parser(w1) | (int)parser(w2)));    
  }
  xtrim(funktion,TRUE,s);  /* Leerzeichen vorne und hinten entfernen, Grossbuchstaben */
 
if(searchchr2(funktion,' ')!=NULL) {
  if(wort_sepr2(s," AND ",TRUE,w1,w2)>1)  return((double)((int)parser(w1) & (int)parser(w2)));    /* von rechts !!  */
  if(wort_sepr2(s," OR ",TRUE,w1,w2)>1)   return((double)((int)parser(w1) | (int)parser(w2)));    
  if(wort_sepr2(s," NAND ",TRUE,w1,w2)>1) return((double)~((int)parser(w1) & (int)parser(w2)));    
  if(wort_sepr2(s," NOR ",TRUE,w1,w2)>1)  return((double)~((int)parser(w1) | (int)parser(w2)));    
  if(wort_sepr2(s," XOR ",TRUE,w1,w2)>1)  return((double)((int)parser(w1) ^ (int)parser(w2)));	 
  if(wort_sepr2(s," EOR ",TRUE,w1,w2)>1)  return((double)((int)parser(w1) ^ (int)parser(w2)));	 
  if(wort_sepr2(s," EQV ",TRUE,w1,w2)>1)  return((double)~((int)parser(w1) ^ (int)parser(w2)));    
  if(wort_sepr2(s," IMP ",TRUE,w1,w2)>1)  return((double)(~((int)parser(w1) ^ (int)parser(w2)) | (int)parser(w2)));    
  if(wort_sepr2(s," MOD ",TRUE,w1,w2)>1)  return(fmod(parser(w1),parser(w2)));
  if(wort_sepr2(s," DIV ",TRUE,w1,w2)>1) { 
    int nenner=(int)parser(w2);
    if(nenner) return((double)((int)parser(w1) / nenner));    
    else {
      error(0,w2); /* Division durch 0 */
      return(0);
    }
  } 
  if(wort_sepr2(s,"NOT ",TRUE,w1,w2)>1) {        
    if(strlen(w1)==0) return((double)(~(int)parser(w2)));    /* von rechts !!  */
    /* Ansonsten ist NOT Teil eines Variablennamens */
  }  
}

  /* Erst Vergleichsoperatoren mit Wahrheitwert abfangen (lowlevel < Addition)  */
if(searchchr2_multi(s,"<=>")!=NULL) {
  if(wort_sep2(s,"==",TRUE,w1,w2)>1)      return(vergleich(w1,w2)?0:-1);
  if(wort_sep2(s,"<>",TRUE,w1,w2)>1) return(vergleich(w1,w2)?-1:0);
  if(wort_sep2(s,"><",TRUE,w1,w2)>1) return(vergleich(w1,w2)?-1:0);
  if(wort_sep2(s,"<=",TRUE,w1,w2)>1) return((vergleich(w1,w2)<=0)?-1:0);
  if(wort_sep2(s,">=",TRUE,w1,w2)>1) return((vergleich(w1,w2)>=0)?-1:0);
  if(wort_sep(s,'=',TRUE,w1,w2)>1)   return(vergleich(w1,w2)?0:-1); 
  if(wort_sep(s,'<',TRUE,w1,w2)>1)   return((vergleich(w1,w2)<0)?-1:0);
  if(wort_sep(s,'>',TRUE,w1,w2)>1)   return((vergleich(w1,w2)>0)?-1:0);
}
 /* Addition/Subtraktion/Vorzeichen  */
if(searchchr2_multi(s,"+-")!=NULL) {
  if(wort_sep_e(s,'+',TRUE,w1,w2)>1) {
    if(strlen(w1)) return(parser(w1)+parser(w2)); 
    else return(parser(w2));   /* war Vorzeichen + */
  }
  if(wort_sepr_e(s,'-',TRUE,w1,w2)>1) {       /* von rechts !!  */    
    if(strlen(w1)) return(parser(w1)-parser(w2));    
    else return(-parser(w2));   /* war Vorzeichen - */
  } 
}
if(searchchr2_multi(s,"*/^")!=NULL) {
  if(wort_sepr(s,'*',TRUE,w1,w2)>1) {           
    if(strlen(w1)) return(parser(w1)*parser(w2));    
    else {
      printf("Pointer noch nicht integriert! %s\n",w2);   /* war pointer - */
      return(0);
    }
  } 
  if(wort_sepr(s,'/',TRUE,w1,w2)>1) {        
    if(strlen(w1)) {
      double nenner;
      nenner=parser(w2); 
      if(nenner!=0.0) return(parser(w1)/nenner);    /* von rechts !!  */
      else { error(0,w2); return(0);  } /* Division durch 0 */
    } else { error(51,w2); return(0); }/* "Parser: Syntax error?! "  */
  } 
  if(wort_sepr(s,'^',TRUE,w1,w2)>1) {           
    if(strlen(w1)) return(pow(parser(w1),parser(w2)));    /* von rechts !!  */
    else { error(51,w2); return(0); } /* "Parser: Syntax error?! "  */
  } 
}  
  if(*s=='(' && s[strlen(s)-1]==')')  { /* Ueberfluessige Klammern entfernen */
    s[strlen(s)-1]=0;
    return(parser(s+1));
    /* SystemFunktionen Subroutinen und Arrays */
  } else {
    pos=searchchr(s, '(');
    if(pos!=NULL) {
      pos2=s+strlen(s)-1;
      *pos++=0;
    
      if(*pos2!=')') error(51,w2); /* "Parser: Syntax error?! "  */
      else {                         /* $-Funktionen und $-Felder   */
        *pos2=0;

        /* Benutzerdef. Funktionen */
        if(*s=='@') return(do_funktion(s+1,pos));
	else {
	  /* Liste durchgehen */
	  int i=0,a=anzpfuncs-1,b,l=strlen(s);
          for(b=0; b<l; b++) {
            while(s[b]>(pfuncs[i].name)[b] && i<a) i++;
            while(s[b]<(pfuncs[a].name)[b] && a>i) a--;
            if(i==a) break;
          }
          if(strcmp(s,pfuncs[i].name)==0) {
	     /* printf("Funktion %s gefunden. Nr. %d\n",pfuncs[i].name,i); */
	      if((pfuncs[i].opcode&FM_TYP)==F_SIMPLE || pfuncs[i].pmax==0) {
	        if(pfuncs[i].opcode&F_IRET) 
		  return((double)((int (*)())pfuncs[i].routine)());
		else return((pfuncs[i].routine)());
	      } else if((pfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
	      	if(pfuncs[i].opcode&F_IRET) return((double)((int (*)())pfuncs[i].routine)(pos));
		else return((pfuncs[i].routine)(pos));
	      } else if((pfuncs[i].opcode&FM_TYP)==F_PLISTE) {
		 PARAMETER *plist;
                 int e=make_pliste(pfuncs[i].pmin,pfuncs[i].pmax,(short *)pfuncs[i].pliste,pos,&plist);
                 double a;
                 if(pfuncs[i].opcode&F_IRET) a=(double)((int (*)())pfuncs[i].routine)(plist,e);
	         else a=(pfuncs[i].routine)(plist,e);
	         if(e!=-1) free_pliste(e,plist);
	         return(a);
	      } else if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_DQUICK) {      
	      	if(pfuncs[i].opcode&F_IRET) 
		  return((double)((int (*)())pfuncs[i].routine)(parser(pos)));
		else return((pfuncs[i].routine)(parser(pos)));
	      } else if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_IQUICK) {
	      	if(pfuncs[i].opcode&F_IRET) return((double)((int (*)())pfuncs[i].routine)((int)parser(pos)));
		else return((pfuncs[i].routine)((int)parser(pos)));
	      } else if(pfuncs[i].pmax==2 && (pfuncs[i].opcode&FM_TYP)==F_DQUICK) {
	       	 char w1[strlen(pos)+1],w2[strlen(pos)+1];
	         int e;
		 double val1,val2;
	         if((e=wort_sep(pos,',',TRUE,w1,w2))==1) {
		   error(56,""); /* Falsche Anzahl Parameter */
		   val1=parser(w1); val2=0;
	         } else if(e==2) {
	           val1=parser(w1); val2=parser(w2);
	         }
                if(pfuncs[i].opcode&F_IRET) return((double)((int (*)())pfuncs[i].routine)(val1,val2));
		else return((pfuncs[i].routine)(val1,val2));
	      } else if(pfuncs[i].pmax==2 && (pfuncs[i].opcode&FM_TYP)==F_IQUICK) {
	       	 char w1[strlen(pos)+1],w2[strlen(pos)+1];
	         int e;
		 int val1,val2;
	         if((e=wort_sep(pos,',',TRUE,w1,w2))==1) {
		   error(56,""); /* Falsche Anzahl Parameter */
		   val1=(int)parser(w1); val2=0;
	         } else if(e==2) {
	           val1=(int)parser(w1); val2=(int)parser(w2);
	         }
                if(pfuncs[i].opcode&F_IRET) return((double)((int (*)())pfuncs[i].routine)(val1,val2));
		else return((pfuncs[i].routine)(val1,val2));
	       
	      } else if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_SQUICK) {
                STRING test=string_parser(pos);
		double erg;
		test.pointer=realloc(test.pointer,test.len+1);
		test.pointer[test.len]=0;
	        if(pfuncs[i].opcode&F_IRET) erg=(double)((int (*)())pfuncs[i].routine)(test);
		else erg=(pfuncs[i].routine)(test);
		free(test.pointer);
		return(erg);
	      } else printf("Interner ERROR. Funktion nicht korrekt definiert. %s\n",s);
	   /* Nicht in der Liste ? Dann kann es noch ARRAY sein   */   
	      
          } else if(type2(s) & FLOATTYP) {
            if((vnr=variable_exist(s,FLOATARRAYTYP))!=-1) return(floatarrayinhalt(vnr,pos));
	    else { error(15,s); return(0); } /* Feld nicht dimensioniert  */
          } else if(type2(s) & INTTYP) {
	    char *r=varrumpf(s);
	    if((vnr=variable_exist(r,INTARRAYTYP))!=-1) return((double)intarrayinhalt(vnr,pos));
	    else { error(15,s); return(0); }  /* Feld nicht dimensioniert  */
	    free(r);
	  } else { error(15,s); return(0); }  /* Feld nicht dimensioniert  */
        }
      }
    } else {  /* Also keine Klammern */
      /* Dann Systemvariablen und einfache Variablen */

	  /* Liste durchgehen */
	  char c=*s;
	  int i=0,a=anzsysvars-1,b,l=strlen(s);
          for(b=0; b<l; c=s[++b]) {
            while(c>(sysvars[i].name)[b] && i<a) i++;
            while(c<(sysvars[a].name)[b] && a>i) a--;
            if(i==a) break;
          }
          if(strcmp(s,sysvars[i].name)==0) {
	    /*  printf("Sysvar %s gefunden. Nr. %d\n",sysvars[i].name,i);*/ 
	   if((sysvars[i].opcode)==PL_INT) return((double)((int (*)())sysvars[i].routine)());
	   if((sysvars[i].opcode)==PL_FLOAT) return((sysvars[i].routine)());
          }
      /* erst integer abfangen (xxx% oder xxx&), dann rest */

      if(*s=='@')                              return(do_funktion(s+1,""));
      if((vnr=variable_exist(s,FLOATTYP))!=-1) return(variablen[vnr].zahl);
      if(s[strlen(s)-1]=='%') {
        s[strlen(s)-1]=0;
        if((vnr=variable_exist(s,INTTYP))!=-1) return((double)variablen[vnr].opcode);
        return(0);   
      } else return(atof(s));  /* Jetzt nur noch Zahlen (hex, oct etc ...)*/
    }
  }
  error(51,s); /* Syntax error */
  return(0);
}

ARRAY f_smula(PARAMETER *plist, int e) {
	   ARRAY ergeb;
	   ergeb.typ=plist[0].typ;
	   ergeb.dimension=plist[0].integer;
	   ergeb.pointer=plist[0].pointer;
	   ergeb=double_array(ergeb);
	   array_smul(ergeb,plist[1].real);
	   return(ergeb);
}
ARRAY f_nullmat(PARAMETER *plist, int e) {
    int dimlist[2]={plist[0].integer,plist[1].integer};
    return(nullmatrix(FLOATARRAYTYP,e,dimlist));
}
ARRAY f_einsmat(PARAMETER *plist, int e) {
    int dimlist[2]={plist[0].integer,plist[0].integer};
    return(einheitsmatrix(FLOATARRAYTYP,2,dimlist));
}

#ifdef CONTROL
ARRAY f_csvgeta(char *pos) {
  int o=0,nn=0;
  if(e>1) nn=plist[1].integer;
  if(e>2) o=plist[2].integer;
  return(csvget(plist[0].pointer,nn,o));
}
#endif
#ifdef TINE
ARRAY f_tinegeta(PARAMETER *plist, int e) {
  int o=0,nn=0;
  if(e>1) nn=plist[1].integer;
  if(e>2) o=plist[2].integer;
  return(tinevget(plist[0].pointer,nn,o));
}
ARRAY f_tinehistorya(PARAMETER *plist, int e) {
	  return(tinehistory(plist[0].pointer,plist[1].integer,plist[2].integer));
}
#endif
const AFUNCTION pafuncs[]= {  /* alphabetisch !!! */
 { F_ARGUMENT,  "!nulldummy",  f_nop ,0,0   ,{0}},
 { F_PLISTE,    "0"         , f_nullmat ,2,2   ,{PL_INT,PL_INT}},
 { F_PLISTE,    "1"         , f_einsmat ,1,1   ,{PL_INT}},
#ifdef CONTROL
 { F_PLISTE,    "CSGET"     , f_csvgeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_PLISTE,    "CSVGET"    , f_csvgeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
#endif

 { F_AQUICK,  "INV"         , inv_array ,1,1   ,{PL_NARRAY}},
 { F_PLISTE,  "SMUL"        , f_smula ,2,2   ,{PL_ARRAY,PL_FLOAT}},

#ifdef TINE
 { F_PLISTE,    "TINEGET"     , f_tinegeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_PLISTE,    "TINEVGET"    , f_tinegeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_PLISTE,    "TINEHISTORY" , f_tinehistorya ,3,3   ,{PL_STRING,PL_INT,PL_INT}},
#endif
 { F_AQUICK,    "TRANS"     , trans_array ,1,1   ,{PL_ARRAY}},

};
const int anzpafuncs=sizeof(pafuncs)/sizeof(AFUNCTION);

ARRAY array_parser(char *funktion) { /* Array-Parser  */
/* Rekursiv und so, dass dynamische Speicherverwaltung ! */
/* Trenne ersten Token ab, und uebergebe rest derselben Routine */

/* Zum Format:

   a()    ganzes Array
   a(1,2,:,3,:) nur Teil des Arrays. Die angegebenen Indizies werden
                festgehalten. Es resultiert ein 2-dimensionales Array
		*/


  char s[strlen(funktion)+1],w1[strlen(funktion)+1],w2[strlen(funktion)+1];
  char *pos;
  int e;
  strcpy(s,funktion);
  xtrim(s,TRUE,s);  /* Leerzeichen vorne und hinten entfernen */

  if(wort_sep(s,'+',TRUE,w1,w2)>1) {
    if(strlen(w1)) {
      ARRAY zw1=array_parser(w1); 
      ARRAY zw2=array_parser(w2);
      array_add(zw1,zw2);
      free_array(zw2); 
      return(zw1);
    } else return(array_parser(w2)); 
  } else if(wort_sepr(s,'-',TRUE,w1,w2)>1) { 
    if(strlen(w1)) {
      ARRAY zw1=array_parser(w1); 
      ARRAY zw2=array_parser(w2);
      array_sub(zw1,zw2);
      free_array(zw2); 
      return(zw1);
    } else {
      ARRAY zw2=array_parser(w2);
      array_smul(zw2,-1);
      return(zw2);
    } 
  } else if(wort_sepr(s,'*',TRUE,w1,w2)>1) {           
    if(strlen(w1)) {
      if(type2(w1) & ARRAYTYP) {
        ARRAY zw1=array_parser(w1); 
	ARRAY zw2=array_parser(w2);
        ARRAY ergebnis=mul_array(zw1,zw2);
        free_array(zw1); free_array(zw2);
        return(ergebnis);
      } else {    /* Skalarmultiplikation */
        ARRAY zw2=array_parser(w2);
	array_smul(zw2,parser(w1));
	return(zw2);
      }  
    } else error(51,""); /*Syntax Error*/
  } else if(wort_sepr(s,'^',TRUE,w1,w2)>1) {
    ARRAY zw2,zw1=array_parser(w1);
    e=(int)parser(w2);
    if(e<0) error(51,""); /*Syntax Error*/
    else if(e==0) {
      zw2=zw1;
      zw1=einheitsmatrix(zw2.typ,zw2.dimension,zw2.pointer); 
      free_array(zw2);
    } else if(e>1) {
      int i;
      for(i=1;i<e;i++) {
        zw2=mul_array(zw1,zw1);
	free_array(zw1);
	zw1=zw2;
      }
    }
    return(zw1);  
  } else if(s[0]=='(' && s[strlen(s)-1]==')')  { /* Ueberfluessige Klammern entfernen */
    s[strlen(s)-1]=0;
    return(array_parser(s+1));    
  } else {/* SystemFunktionen, Konstanten etc... */
    if(*s=='[' && s[strlen(s)-1]==']') {  /* Konstante */
      s[strlen(s)-1]=0;
      return(array_const(s+1));
    }
    pos=searchchr(s,'(');
    if(pos!=NULL) {
      char *pos2=s+strlen(s)-1;
      *pos++=0;      
      if(*pos2!=')') {
         error(51,w2); /* "Parser: Syntax error?! "  */
      } else {                         /* $-Funktionen und $-Felder   */
	  /* Liste durchgehen */
	  int i=0,a=anzpafuncs-1,b,l=strlen(s);
          *pos2=0;        
          for(b=0; b<l; b++) {
            while(s[b]>(pafuncs[i].name)[b] && i<a) i++;
            while(s[b]<(pafuncs[a].name)[b] && a>i) a--;
            if(i==a) break;
          }
	  if(strcmp(s,pafuncs[i].name)==0) {
	    /*  printf("Funktion %s gefunden. Nr. %d\n",pafuncs[i].name,i); */
	      if((pafuncs[i].opcode&FM_TYP)==F_SIMPLE || pafuncs[i].pmax==0) {
		return((pafuncs[i].routine)());
	      } else if((pafuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
	     	return((pafuncs[i].routine)(pos));
	      } else if((pafuncs[i].opcode&FM_TYP)==F_PLISTE) {
		 PARAMETER *plist;
                 int e=make_pliste(pafuncs[i].pmin,pafuncs[i].pmax,(short *)pafuncs[i].pliste,pos,&plist);
                 ARRAY a=(pafuncs[i].routine)(plist,e);
	         if(e!=-1) free_pliste(e,plist);
	         return(a);
	      } else if(pafuncs[i].pmax==1 && (pafuncs[i].opcode&FM_TYP)==F_AQUICK) {
	        ARRAY ergebnis,a=array_parser(pos);
		ergebnis=(pafuncs[i].routine)(a);
		free_array(a);
	      	return(ergebnis);
	      } else printf("Interner ERROR. Funktion nicht korrekt definiert. %s\n",s);   
          }/* Nicht in der Liste ?    */  
         if(strcmp(s,"INP%")==0) {
	   char w1[strlen(pos)+1],w2[strlen(pos)+1];
	   int i,nn;
	   ARRAY ergeb;
           FILE *fff=stdin;
	   wort_sep(pos,',',TRUE,w1,w2);
	   i=get_number(w1);
	   nn=(int)parser(w2);
	  
           ergeb.typ=INTARRAYTYP;
	   ergeb.dimension=1;
	   ergeb.pointer=malloc(INTSIZE+nn*sizeof(int));
	   ((int *)(ergeb.pointer))[0]=nn;
           if(filenr[i]) {
	     int *varptr=ergeb.pointer+INTSIZE;
             fff=dptr[i];
             fread(varptr,sizeof(int),nn,fff);
             return(ergeb);
           } else error(24,""); /* File nicht geoeffnet */
           return(ergeb);
	 } else {
	    int vnr;
	    char *r=varrumpf(s);
	    ARRAY ergebnis;
	   /* Hier sollten keine Funktionen mehr auftauchen  */
	   /* Jetzt uebergebe spezifiziertes Array, evtl. reduziert*/
	   if(strlen(pos)==0) {
	     if((vnr=variable_exist(r,FLOATARRAYTYP))!=-1)  ergebnis=copy_var_array(vnr);
	     else if((vnr=variable_exist(r,INTARRAYTYP))!=-1)    ergebnis=copy_var_array(vnr);
	     else if((vnr=variable_exist(r,STRINGARRAYTYP))!=-1) ergebnis=copy_var_array(vnr);
             else error(15,s);  /* Feld nicht dimensioniert  */
	   } else {
	     if((vnr=variable_exist(r,FLOATARRAYTYP))!=-1) {
	       char w1[strlen(pos)+1],w2[strlen(pos)+1];
	       int i,e,rdim=0,ndim=0,anz=1,anz2=1,j,k;
	       int indexe[variablen[vnr].opcode];
	       int indexo[variablen[vnr].opcode];
	       int indexa[variablen[vnr].opcode];
	  
	     /* Dimension des reduzierten Arrays bestimmen */
	       ergebnis.typ=FLOATARRAYTYP;
	       e=wort_sep(pos,',',TRUE,w1,w2);
	       while(e) {
	         if(w1[0]!=':' && w1[0]!=0) {
		   indexo[ndim++]=(int)parser(w1);
		   rdim++;
		 } else indexo[ndim++]=-1;
		 
	         e=wort_sep(w2,',',TRUE,w1,w2);
	       }
	       
             /* Dimensionierung uebertragen */
	       ergebnis.dimension=max(variablen[vnr].opcode-rdim,1);
	       ergebnis.pointer=malloc(INTSIZE*ergebnis.dimension);
	       rdim=0;ndim=0;anz=1;
	       e=wort_sep(pos,',',TRUE,w1,w2);
	       while(e) {
	         indexa[rdim]=anz;		 
	         if(w1[0]==':' || w1[0]==0) {
		 
		   ((int *)(ergebnis.pointer))[ndim++]=((int *)(variablen[vnr].pointer))[rdim];
		   anz=anz*(((int *)variablen[vnr].pointer)[rdim]);
		 } 
		 rdim++;
	         e=wort_sep(w2,',',TRUE,w1,w2);
	       }	       

 	       ergebnis.pointer=realloc(ergebnis.pointer,INTSIZE*ergebnis.dimension+anz*sizeof(double));

	      /*Loop fuer die Komprimierung */
               
	       for(j=0;j<anz;j++) {
	         int jj=j;
	         /* Indexliste aus anz machen */
                 for(k=variablen[vnr].opcode-1;k>=0;k--) {
		   if(indexo[k]==-1) {
		     indexe[k]=jj/indexa[k];
		     jj=jj % indexa[k];
		     
		   } else indexe[k]=indexo[k];
		 }
		 if(jj!=0) puts("Rechnung geht nicht auf.");
		 /* Testen ob passt  */
	         /*printf("j=%d : indexe[]=",j);*/
		 anz2=0;
	         for(k=0;k<variablen[vnr].opcode;k++) {
		   /*printf(" %d",indexe[k]);*/
		   anz2=anz2*((int *)variablen[vnr].pointer)[k]+indexe[k];
		 }
	         /*printf("\n");
		 printf("--anz2=%d\n",anz2);*/

		 /* jetzt kopieren */
		 
		 ((double *)(ergebnis.pointer+INTSIZE*ergebnis.dimension))[j]=((double *)(variablen[vnr].pointer+INTSIZE*variablen[vnr].opcode))[anz2]; 
	       }     
	     } else if((vnr=variable_exist(r,INTARRAYTYP))!=-1) {
	       puts("Noch nicht möglich...");
	     }  else if((vnr=variable_exist(s,STRINGARRAYTYP))!=-1) {
	       puts("Noch nicht möglich...");
	     } else {
	       error(15,s);  /* Feld nicht dimensioniert  */
	       e=1;
  	       ergebnis=einheitsmatrix(FLOATARRAYTYP,1,&e);
	     }
	   }
	   free(r);
	   return(ergebnis);
        }
      }
    }
  }
  /* Offenbar war der String leer oder so */
  e=0;
  return(nullmatrix(FLOATARRAYTYP,0,&e));
}

STRING string_parser(char *);

char *s_parser(char *funktion) { /* String-Parser  */
  STRING e=string_parser(funktion);
  e.pointer=realloc(e.pointer,e.len+1);
  (e.pointer)[e.len]=0;
  return(e.pointer);
}
STRING string_parser(char *funktion) {
/* Rekursiv und so, dass dynamische Speicherverwaltung ! */
/* Trenne ersten Token ab, und uebergebe rest derselben Routine */

  STRING ergebnis;
  char v[strlen(funktion)+1],w[strlen(funktion)+1];

  /*printf("S-Parser: <%s>\n",funktion);*/
  if(wort_sep(funktion,'+',TRUE,v,w)>1) {
    STRING t=string_parser(v);
    STRING u=string_parser(w);
    ergebnis.pointer=malloc(t.len+u.len+1);
    memcpy(ergebnis.pointer,t.pointer,t.len);
    memcpy(ergebnis.pointer+t.len,u.pointer,u.len+1);
    ergebnis.len=u.len+t.len;
    free(t.pointer);free(u.pointer); 
    return(ergebnis);
  } else {
    char *pos,*pos2,*inhalt;
    int vnr;
    
    /*printf("s-parser: <%s>\n",funktion);*/
    strcpy(v,funktion);
    pos=searchchr(v, '(');
    if(pos!=NULL) {
      pos2=v+strlen(v)-1;
      *pos++=0;

      if(*pos2!=')') {
        error(51,v); /* "Parser: Syntax error?! "  */
        ergebnis=vs_error();
      } else {                         /* $-Funktionen und $-Felder   */
        *pos2=0;
       	
	if(*v=='@')     /* Funktion oder Array   */
 	  ergebnis=do_sfunktion(v+1,pos);	  
        else {  /* Liste durchgehen */

	  int i=0,a=anzpsfuncs-1,b;
          for(b=0; b<strlen(v); b++) {
            while(v[b]>(psfuncs[i].name)[b] && i<a) i++;
            while(v[b]<(psfuncs[a].name)[b] && a>i) a--;
            if(i==a) break;
          }
          if(strcmp(v,psfuncs[i].name)==0) {
	      if((psfuncs[i].opcode&FM_TYP)==F_SIMPLE || psfuncs[i].pmax==0) 
	        ergebnis=(psfuncs[i].routine)();
	      else if((psfuncs[i].opcode&FM_TYP)==F_ARGUMENT) 
	      	ergebnis=(psfuncs[i].routine)(pos);
	      else if((psfuncs[i].opcode&FM_TYP)==F_PLISTE) {
	        PARAMETER *plist;
                 int e=make_pliste(psfuncs[i].pmin,psfuncs[i].pmax,(short *)psfuncs[i].pliste,pos,&plist);
                 ergebnis=(psfuncs[i].routine)(plist,e);
	         if(e!=-1) free_pliste(e,plist);
	      } else if(psfuncs[i].pmax==1 && (psfuncs[i].opcode&FM_TYP)==F_DQUICK) {
		ergebnis=(psfuncs[i].routine)(parser(pos));
	      } else if(psfuncs[i].pmax==1 && (psfuncs[i].opcode&FM_TYP)==F_IQUICK) {
		ergebnis=(psfuncs[i].routine)((int)parser(pos));
	      } else if(psfuncs[i].pmax==2 && (psfuncs[i].opcode&FM_TYP)==F_DQUICK) {
	       	 char w1[strlen(pos)+1],w2[strlen(pos)+1];
	         int e;
		 double val1,val2;
	         if((e=wort_sep(pos,',',TRUE,w1,w2))==1) {
		   error(56,""); /* Falsche Anzahl Parameter */
		   val1=parser(w1); val2=0;
	         } else if(e==2)  val1=parser(w1); val2=parser(w2);
                 ergebnis=(psfuncs[i].routine)(val1,val2);
	      } else if(psfuncs[i].pmax==1 && (psfuncs[i].opcode&FM_TYP)==F_SQUICK) {
                STRING test=string_parser(pos);
		test.pointer=realloc(test.pointer,test.len+1);
		test.pointer[test.len]=0;
	        ergebnis=(psfuncs[i].routine)(test);
		free(test.pointer);
	      } else printf("Interner ERROR. Funktion nicht korrekt definiert. %s\n",v);
	   /* Nicht in der Liste ? Dann kann es noch ARRAY sein   */   
	    } else {
	     int vnr;
	     v[strlen(v)-1]=0;
	     
             if((vnr=variable_exist(v,STRINGARRAYTYP))==-1) {
	       error(15,v);         /*Feld nicht definiert*/
	       ergebnis.pointer=malloc(1);
	       ergebnis.len=0;
             } else {
	       char *ss,*t;
	       int idxn,i,*bbb,ndim=0,anz=0;
	     
	     /*************/
	     
	     /* Dimensionen bestimmen   */

               ss=malloc(strlen(pos)+1);
               t=malloc(strlen(pos)+1);
               strcpy(ss,pos);
               i=wort_sep(ss,',',TRUE,ss,t);
               if(i==0) puts("Arrayverarbeitung an dieser Stelle nicht möglich!");

               bbb=(int *)variablen[vnr].pointer;
               /*printf("C: %d %s %s %s t: %s\n",vnr,v,pos,ss,t);*/
               while(i) {
                 xtrim(ss,TRUE,ss);
	         if(ndim<variablen[vnr].opcode){
                   idxn=(int)parser(ss);
	           if(idxn>=bbb[ndim]) {error(16,ss); /* Feldindex zu gross*/ break;}
                   else anz=idxn+anz*bbb[ndim];
                 }
                 ndim++; 
                 i=wort_sep(t,',',TRUE,ss,t); 
              } 
	       
              if(ndim!=variablen[vnr].opcode) {
	         error(18,"");  /* Falsche Anzahl Indizies */
		 ergebnis.pointer=malloc(1);
		 ergebnis.len=0;
              } else {
          
	        pos=(char *)(variablen[vnr].pointer+ndim*INTSIZE+anz*(sizeof(int)+sizeof(char *)));
	    
                ergebnis.pointer=malloc(((int *)pos)[0]+1);
		ergebnis.len=((int *)pos)[0];
	        pos+=sizeof(int);
	        memcpy(ergebnis.pointer,((char **)pos)[0],ergebnis.len);
	      }
	      free(ss);free(t);
	    }
	  }
        } 
      }
    } else {
      pos2=v+strlen(v)-1;
      if(*v=='"' && *pos2=='"') {  /* Konstante  */
        ergebnis.pointer=malloc(strlen(v)-2+1);
        ergebnis.len=strlen(v)-2;
        *pos2=0;
        memcpy(ergebnis.pointer,v+1,strlen(v)-2+1);
      } else if(*pos2!='$') {
        error(51,v); /* "Parser: Syntax error?! "  */
        ergebnis=vs_error();
      } else {                      /* einfache Variablen und Systemvariablen */
	/* Liste durchgehen */
	int i=0,a=anzsyssvars,b;
        for(b=0; b<strlen(v); b++) {
          while(v[b]>(syssvars[i].name)[b] && i<a) i++;
          while(v[b]<(syssvars[a].name)[b] && a>i) a--;
          if(i==a) break;
        }
        if(strcmp(v,syssvars[i].name)==0) {
	    /*  printf("Sysvar %s gefunden. Nr. %d\n",syssvars[i].name,i);*/ 
	  return((syssvars[i].routine)());
        }
        *pos2=0;
        if((vnr=variable_exist(v,STRINGTYP))==-1) {
          ergebnis.pointer=malloc(1);
	  ergebnis.len=0;
        } else {
          ergebnis.pointer=malloc(variablen[vnr].opcode+1);
	  ergebnis.len=variablen[vnr].opcode;
	  memcpy(ergebnis.pointer,variablen[vnr].pointer,ergebnis.len);
        }
      }
    }
  }
  return(ergebnis);
}


char *hexoct_to_string(char n,char *pos) {

          /* STR$(a[,b[,c[,d]]])     */
          char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  char formatter[20];
	  int j,e,b=-1,c=13,mode=0,i=0;
	  unsigned int a;
	  char *ergebnis;
          e=wort_sep(pos,',',TRUE,w1,w2); 
	  while(e) {
	    if(strlen(w1)) {
              switch(i) {
	      case 0: {a=(unsigned int)parser(w1);break;}
	      case 1: {b=min(50,max(0,(int)parser(w1)));break;}
	      case 2: {c=min(50,max(0,(int)parser(w1)));break;}
	      case 3: {mode=(int)parser(w1);break;}

              default: break;
              }
            }
	    e=wort_sep(w2,',',TRUE,w1,w2);
            i++;
          }
	  
	  
	  if(mode==0 && b!=-1) sprintf(formatter,"%%%d.%d%c",b,c,n);
	  else if (mode==1 && b!=-1) sprintf(formatter,"%%0%d.%d%c",b,c,n);
	  else  sprintf(formatter,"%%.13%c",n);
	  ergebnis=malloc(20);
          sprintf(ergebnis,formatter,a);
	  return(ergebnis);
}


double do_funktion(char *name,char *argumente) {
 char *buffer,*pos,*pos2,*pos3;
    int pc2; 
    
    buffer=malloc(strlen(name)+1);
    strcpy(buffer,name);
    pos=argumente;
    
    pc2=procnr(buffer,2);
    if(pc2==-1)   error(44,buffer); /* Funktion  nicht definiert */
    else {       
	if(do_parameterliste(pos,procs[pc2].parameterliste)) error(42,buffer); /* Zu wenig Parameter */
	else {     
	  int oldbatch,osp=sp;
	  pc2=procs[pc2].zeile;
	  if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
	  else {printf("Stack-Overflow ! PC=%d\n",pc); batch=0;}
	  oldbatch=batch;batch=1;
	  programmlauf();
	  batch=min(oldbatch,batch);
	  if(osp!=sp) {
	    pc=stack[--sp]; /* wenn error innerhalb der func. */
            puts("Error within FUNCTION.");
	  }
	}
	free(buffer);
	return(returnvalue.f);
      }
     
    free(buffer);
    return(0.0);  
 }
 
 /* loese die Parameterliste auf und weise die Werte auf die neuen lokalen
    Variablen zu */
 
int do_parameterliste(char *pos, char *pos2) {
  char w3[strlen(pos)+1],w4[strlen(pos)+1];
  char w5[strlen(pos2)+1],w6[strlen(pos2)+1];
  int e1,e2;
 /* printf("GOSUB: <%s> <%s>\n",pos,pos2);*/
  e1=wort_sep(pos,',',TRUE,w3,w4);
  e2=wort_sep(pos2,',',TRUE,w5,w6);
  sp++;  /* Uebergabeparameter sind lokal ! */
  while(e1 && e2) {
  /*  printf("ZU: %s=%s\n",w3,w5); */
    c_dolocal(w5,w3);
    e1=wort_sep(w4,',',TRUE,w3,w4);
    e2=wort_sep(w6,',',TRUE,w5,w6);
  }
  sp--;
  return((e1!=e2) ? 1 : 0);
}
 
 
STRING do_sfunktion(char *name,char *argumente) {
  char *buffer,*pos;
  STRING ergebnis;
  int pc2; 
    
  buffer=malloc(strlen(name)+1);
  strcpy(buffer,name);
  pos=argumente;  
  pc2=procnr(buffer,2);
  if(pc2!=-1) {       
	if(do_parameterliste(pos,procs[pc2].parameterliste)) error(42,buffer); /* Zu wenig Parameter */
	else {     
	  int oldbatch,osp=sp;
	  
	  pc2=procs[pc2].zeile;
	  if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
	  else {printf("Stack-Overflow ! PC=%d\n",pc); batch=0;}
	  oldbatch=batch;batch=1;
	  programmlauf();
	  batch=min(oldbatch,batch);
	  if(osp!=sp) {
	    pc=stack[--sp]; /* wenn error innerhalb der func. */
            puts("Error within FUNCTION.");
	  }
	}
    free(buffer);
    return(returnvalue.str);
  }
  error(44,buffer); /* Funktion  nicht definiert */
  free(buffer);
  ergebnis.pointer=malloc(1);
  ergebnis.len=0;
  return(ergebnis);  
}
