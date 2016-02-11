/* parser.c   Formelparser numerisch und Zeichenketten (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h> 
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
 

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "parameter.h"
#include "xbasic.h"
#include "array.h"
#include "functions.h"
#include "afunctions.h"
#include "sfunctions.h"
#include "kommandos.h"
#include "file.h"
#include "parser.h"
#include "io.h"
#include "wort_sep.h"
#include "graphics.h"
#include "aes.h"
#include "window.h"
#include "number.h"

extern int shm_malloc(int,int);
extern int shm_attach(int);

int f_succ(double b) {return((int)(b+1));}
int f_sgn(double b) {return(sgn(b));}
double f_frac(double b) {return(b-((double)((int)b)));}
int f_even(int b) {return(b&1 ? 0:-1);}
int f_odd(int b) {return(b&1 ? -1:0);}
int f_fak(int);
int f_random(double d) {return((int)((double)rand()/RAND_MAX*d));}
double f_rnd(double d) {return((double)rand()/RAND_MAX);}
int  f_srand(double d) {srand((int)d);return(0);}
double f_deg(double d) {return(d/PI*180);}
double f_rad(double d) {return(d*PI/180);}
int  f_dimf(char *pos) {
  int typ=vartype(pos);
  char *r=varrumpf(pos);
  int subtyp,vnr;
  if(typ&ARRAYTYP) {subtyp=typ&(~ARRAYTYP);typ=ARRAYTYP;}
  else subtyp=0;
  vnr=var_exist(pos,typ,subtyp,0);
  free(r);
  if(vnr==-1) {
    xberror(15,pos); /* Feld nicht dimensioniert */
    return(0);
  }
  return((double)do_dimension(&variablen[vnr]));
}
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

int f_len(STRING n)    { return(n.len); }
int f_exist(STRING n)  { return(-exist(n.pointer)); }
int f_size(STRING n)   { return(stat_size(n.pointer)); }
int f_nlink(STRING n)  { return(stat_nlink(n.pointer)); }
int f_device(STRING n) { return(stat_device(n.pointer)); }
int f_inode(STRING n)  { return(stat_inode(n.pointer)); }
int f_mode(STRING n)   { return(stat_mode(n.pointer)); }

double f_val(STRING n) { return(myatof(n.pointer)); }
double f_ltextlen(STRING n) { return((double)ltextlen(ltextxfaktor,ltextpflg,n.pointer)); }

char *arrptr(PARAMETER *,int);
#ifdef TINE
double f_tinemax(STRING n) { return(tinemax(n.pointer)); }
double f_tinemin(STRING n) { return(tinemin(n.pointer)); }
double f_tineget(STRING n) { return(tineget(n.pointer)); }
int f_tinesize(STRING n) { return(tinesize(n.pointer)); }
int f_tinetyp(STRING n) { return(tinetyp(n.pointer)); }
#endif
#ifdef DOOCS
double f_doocsget(STRING n) { return(doocsget(n.pointer)); }
int f_doocssize(STRING n) { return(doocssize(n.pointer)); }
int f_doocstyp(STRING n) { return(doocstyp(n.pointer)); }
double f_doocstimestamp(STRING n) { return(doocstimestamp(n.pointer)); }
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
// dump_parameterlist(plist,e);
  if(filenr[plist[0].integer]) return(lof(dptr[plist[0].integer]));
  else { xberror(24,""); return(0);} /* File nicht geoeffnet */
}
int f_loc(PARAMETER *plist,int e) {
  if(filenr[plist[0].integer]) return(ftell(dptr[plist[0].integer]));
  else { xberror(24,""); return(0);} /* File nicht geoeffnet */
}
int f_eof(PARAMETER *plist,int e) {
  if(plist[0].integer==-2) return((myeof(stdin))?-1:0);
  else if(filenr[plist[0].integer]) {
    fflush(dptr[plist[0].integer]);
    return(myeof(dptr[plist[0].integer])?-1:0);
  } else { xberror(24,""); return(0);} /* File nicht geoeffnet */
}

int f_wort_sep(PARAMETER *plist,int e) {
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




#ifndef NOGRAPHICS

int lsel_input(char *,STRING *,int,int);

int f_listselect(PARAMETER *plist,int e) {
  int sel=-1;
  if(e>2) sel=plist[2].integer;
  if(e>1) {
    ARRAY a;
    a=*(ARRAY *)&(plist[1].integer);
    return(lsel_input(plist[0].pointer,(STRING *)(a.pointer+a.dimension*INTSIZE),anz_eintraege(&a),sel));
  }
  return(0);
}
#endif

/*F_CONST fuer die Funktionen, welche bei constantem input imemr das gleiche 
  output liefern.
  */

const FUNCTION pfuncs[]= {  /* alphabetisch !!! */
 { F_CONST|F_ARGUMENT|F_DRET,  "!nulldummy", f_nop ,0,0   ,{0}},
 { F_CONST|F_DQUICK|F_DRET,    "ABS"       , fabs ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ACOS"      , acos ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ACOSH"      , acosh ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ADD"     , f_add ,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_PLISTE|F_IRET,  "ARRPTR"    , arrptr ,1,1     ,{PL_ARRAY}},
 { F_CONST|F_SQUICK|F_IRET,    "ASC"       , f_asc ,1,1   ,{PL_STRING}},
 { F_CONST|F_DQUICK|F_DRET,    "ASIN"      , asin ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ASINH"      , asinh ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ATAN"      , atan ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ATAN2"     , atan2 ,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ATANH"     , atanh ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "ATN"       , atan ,1,1     ,{PL_NUMBER}},

 { F_CONST|F_DQUICK|F_IRET,    "BCHG"      , f_bchg,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_CONST|F_DQUICK|F_IRET,    "BCLR"      , f_bclr,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_CONST|F_DQUICK|F_IRET,    "BSET"      , f_bset,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_CONST|F_DQUICK|F_IRET,    "BTST"      , f_btst,2,2     ,{PL_NUMBER,PL_NUMBER }},

 { F_CONST|F_DQUICK|F_DRET,    "CBRT"      , cbrt ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "CEIL"      , ceil ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_PLISTE|F_IRET,    "COMBIN"    , f_combin ,2,2     ,{PL_INT,PL_INT}},
 { F_CONST|F_DQUICK|F_DRET,    "COS"       , cos ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "COSH"      , cosh ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_PLISTE|F_IRET,    "CRC"       , f_crc ,1,2     ,{PL_STRING, PL_INT}},
#ifdef CONTROL
 { F_SQUICK|F_DRET,  "CSGET"     , f_csget ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSMAX"     , f_csmax ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSMIN"     , f_csmin ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CSPID"     , f_cspid ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSRES"     , f_csres ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CSSIZE"    , f_cssize ,1,1   ,{PL_STRING}},
#endif
 { F_CONST|F_SQUICK|F_DRET,  "CVD"       , f_cvd ,1,1     ,{PL_STRING}},
 { F_CONST|F_SQUICK|F_DRET,  "CVF"       , f_cvf ,1,1     ,{PL_STRING}},
 { F_CONST|F_SQUICK|F_IRET,  "CVI"       , f_cvi ,1,1     ,{PL_STRING}},
 { F_CONST|F_SQUICK|F_IRET,  "CVL"       , f_cvl ,1,1     ,{PL_STRING}},
 { F_CONST|F_SQUICK|F_IRET,  "CVS"       , f_cvf ,1,1     ,{PL_STRING}},

 { F_CONST|F_DQUICK|F_DRET,    "DEG"       , f_deg ,1,1     ,{PL_NUMBER}},
 { F_SQUICK|F_IRET,    "DEVICE"    , f_device,1,1   ,{PL_STRING}},
 { F_CONST|F_ARGUMENT|F_IRET,  "DIM?"      , f_dimf ,1,1      ,{PL_ARRAY}},
 { F_CONST|F_DQUICK|F_DRET,    "DIV"       , f_div ,2,2     ,{PL_NUMBER,PL_NUMBER}},
#ifdef DOOCS
 { F_SQUICK|F_DRET,  "DOOCSGET"     , f_doocsget ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "DOOCSSIZE"    , f_doocssize ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "DOOCSTIMESTAMP"    , f_doocstimestamp ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "DOOCSTYP"    , f_doocstyp ,1,1   ,{PL_STRING}},
#endif
 { F_IQUICK|F_IRET,    "DPEEK"     , f_dpeek ,1,1     ,{PL_INT}},

 { F_PLISTE|F_IRET,    "EOF"       , f_eof ,1,1     ,{PL_FILENR}},

 { F_CONST|F_SQUICK|F_DRET,  "EVAL"      , f_eval ,1,1      ,{PL_STRING}},
 { F_CONST|F_IQUICK|F_IRET,    "EVEN"       , f_even ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "EXEC"       , f_exec ,1,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_SQUICK|F_IRET,    "EXIST"      , f_exist ,1,1     ,{PL_STRING}},
 { F_CONST|F_DQUICK|F_DRET,    "EXP"       , exp ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "EXPM1"     , expm1 ,1,1     ,{PL_NUMBER}},

 { F_CONST|F_IQUICK|F_IRET,    "FACT"       , f_fak ,1,1     ,{PL_INT}},
 { F_CONST|F_DQUICK|F_IRET,    "FIX"       , f_fix ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "FLOOR"     , floor ,1,1     ,{PL_NUMBER}},
#ifdef HAVE_FORK
 { F_SIMPLE|F_IRET,    "FORK"     , fork ,0,0     },
#else
 { F_SIMPLE|F_IRET,    "FORK"     , f_nop ,0,0     },
#endif
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "FORM_ALERT", f_form_alert ,2,2   ,{PL_INT,PL_STRING}},
 { F_PLISTE|F_IRET,    "FORM_CENTER", f_form_center ,1,5   ,{PL_INT,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { F_PLISTE|F_IRET,    "FORM_DIAL", f_form_dial ,9,9   ,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,    "FORM_DO",   f_form_do ,1,1   ,{PL_INT}},
#endif
 { F_CONST|F_DQUICK|F_DRET,    "FRAC"      , f_frac ,1,1     ,{PL_NUMBER}},
 { F_SIMPLE|F_IRET,    "FREEFILE"  , f_freefile ,0,0  },

 { F_DQUICK|F_DRET,    "GASDEV"   , f_gasdev ,1,1     ,{PL_NUMBER}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "GET_COLOR", f_get_color ,3,3   ,{PL_INT,PL_INT,PL_INT}},
#endif
 { F_CONST|F_PLISTE|F_IRET,    "GLOB"     , f_glob ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},
 { F_CONST|F_IQUICK|F_IRET,    "GRAY"     , f_gray ,1,1     ,{PL_INT}},

 { F_CONST|F_DQUICK|F_DRET,    "HYPOT"     , hypot ,2,2     ,{PL_NUMBER,PL_NUMBER}},

 { F_SQUICK|F_IRET,    "INODE"     , f_inode,1,1   ,{PL_STRING}},
 { F_PLISTE|F_IRET,  "INP"       , inp8 ,1,1      ,{PL_FILENR}},
 { F_PLISTE|F_IRET,  "INP?"      , inpf ,1,1      ,{PL_FILENR}},
 { F_PLISTE|F_IRET,  "INP&"      , inp16 ,1,1      ,{PL_FILENR}},
 { F_PLISTE|F_IRET,  "INP%"      , inp32 ,1,1      ,{PL_FILENR}},
 { F_CONST|F_PLISTE|F_IRET,  "INSTR"     , f_instr ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},

 { F_CONST|F_DQUICK|F_IRET,    "INT"       , f_int ,1,1     ,{PL_NUMBER}},
 { F_PLISTE|F_IRET,    "IOCTL"     , f_ioctl ,2,3     ,{PL_FILENR,PL_INT,PL_INT}},
 { F_CONST|F_SQUICK|F_IRET,    "JULIAN"    , f_julian ,1,1     ,{PL_STRING}},

 { F_CONST|F_SQUICK|F_IRET,    "LEN"       , f_len ,1,1   ,{PL_STRING}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "LISTSELECT", f_listselect ,2,3   ,{PL_STRING,PL_SARRAY,PL_INT}},
#endif
 { F_CONST|F_DQUICK|F_DRET,    "LN"        , log ,1,1     ,{PL_NUMBER}},

 { F_PLISTE|F_IRET,    "LOC"       , f_loc ,1,1     ,{PL_FILENR}},
 { F_PLISTE|F_IRET,    "LOF"       , f_lof ,1,1     ,{PL_FILENR}},

 { F_CONST|F_DQUICK|F_DRET,    "LOG"       , log ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "LOG10"     , log10 ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "LOG1P"     , log1p ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "LOGB"      , logb  ,1,1     ,{PL_NUMBER}},
 { F_IQUICK|F_IRET,    "LPEEK"    , f_lpeek ,1,1     ,{PL_INT}},
 { F_SQUICK|F_DRET,    "LTEXTLEN"  , f_ltextlen ,1,1   ,{PL_STRING}},

 { F_IQUICK|F_IRET,    "MALLOC"    , f_malloc ,1,1     ,{PL_INT}},
 { F_CONST|F_PLISTE|F_DRET,    "MAX"     , f_max ,1,-1     ,{PL_NUMBER,PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_PLISTE|F_DRET,    "MIN"     , f_min ,1,-1     ,{PL_NUMBER,PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "MOD"     , fmod ,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_SQUICK|F_IRET,    "MODE"     , f_mode,1,1   ,{PL_STRING}},
 { F_CONST|F_DQUICK|F_DRET,    "MUL"     , f_mul ,2,2     ,{PL_NUMBER,PL_NUMBER}},

 { F_SQUICK|F_IRET,    "NLINK"     , f_nlink,1,1   ,{PL_STRING}},

#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "OBJC_DRAW", f_objc_draw ,5,5   ,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,    "OBJC_FIND", f_objc_find ,3,3   ,{PL_INT,PL_INT,PL_INT}},
 { F_PLISTE|F_IRET,    "OBJC_OFFSET", f_objc_offset ,4,4,{PL_INT,PL_INT,PL_NVAR,PL_NVAR}},
#endif
 { F_CONST|F_IQUICK|F_IRET,    "ODD"       , f_odd ,1,1     ,{PL_NUMBER}},

 { F_IQUICK|F_IRET,    "PEEK"      , f_peek ,1,1     ,{PL_INT}},
#ifndef NOGRAPHICS
 { F_DQUICK|F_IRET,    "POINT"     , f_point ,2,2     ,{PL_NUMBER, PL_NUMBER }},
#endif
 { F_CONST|F_DQUICK|F_DRET,    "PRED"      , f_pred ,1,1     ,{PL_NUMBER}},
#ifndef NOGRAPHICS
 { F_DQUICK|F_IRET,    "PTST"     , f_point ,2,2     ,{PL_NUMBER, PL_NUMBER }},
#endif

 { F_CONST|F_DQUICK|F_DRET,    "RAD"      , f_rad ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "RAND"      , rand ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "RANDOM"    , f_random ,1,1     ,{PL_NUMBER}},
 { F_IQUICK|F_IRET,    "REALLOC"    , f_realloc ,2,2     ,{PL_INT,PL_INT}},
 { F_CONST|F_PLISTE|F_IRET,    "RINSTR"    , f_rinstr ,2,3  ,{PL_STRING,PL_STRING,PL_INT}},
 { F_DQUICK|F_DRET,    "RND"       , f_rnd ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_PLISTE|F_DRET,    "ROUND"     , f_round ,1,2   ,{PL_NUMBER,PL_INT}},
#ifndef NOGRAPHICS
 { F_PLISTE|F_IRET,    "RSRC_GADDR", f_rsrc_gaddr ,2,2   ,{PL_INT,PL_INT}},
#endif

 { F_CONST|F_DQUICK|F_IRET,    "SGN"       , f_sgn ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_IRET,    "SHL"      , f_shl,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_IQUICK|F_IRET,    "SHM_ATTACH"    , shm_attach ,1,1     ,{PL_INT}},
 { F_IQUICK|F_IRET,    "SHM_MALLOC"    , shm_malloc ,2,2     ,{PL_INT,PL_INT}},
 { F_CONST|F_DQUICK|F_IRET,    "SHR"      , f_shr,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "SIN"       , sin ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "SINH"      , sinh ,1,1     ,{PL_NUMBER}},
 { F_SQUICK|F_IRET,    "SIZE"     , f_size,1,1   ,{PL_STRING}},
 { F_CONST|F_DQUICK|F_DRET,    "SQR"       , sqrt ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "SQRT"      , sqrt ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "SRAND"     , f_srand ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "SUB"     , f_sub ,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_CONST|F_DQUICK|F_IRET,    "SUCC"      , f_succ ,1,1     ,{PL_NUMBER}},
 { F_PLISTE|F_IRET,    "SYM_ADR"   , f_symadr ,2,2   ,{PL_FILENR,PL_STRING}},

 { F_CONST|F_DQUICK|F_DRET,    "TAN"       , tan ,1,1     ,{PL_NUMBER}},
 { F_CONST|F_DQUICK|F_DRET,    "TANH"       , tanh ,1,1     ,{PL_NUMBER}},
#ifdef TINE
 { F_SQUICK|F_DRET,  "TINEGET"     , f_tineget ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "TINEMAX"     , f_tinemax ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "TINEMIN"     , f_tinemin ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "TINESIZE"    , f_tinesize ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "TINETYP"    , f_tinetyp ,1,1   ,{PL_STRING}},
#endif
 { F_CONST|F_DQUICK|F_DRET,    "TRUNC"     , f_fix ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "TYP?"       , type ,1,1     ,{PL_ALLVAR}},

 { F_CONST|F_SQUICK|F_DRET,  "VAL"       , f_val ,1,1     ,{PL_STRING}},
 { F_ARGUMENT|F_IRET,  "VARPTR"    , varptr ,1,1     ,{PL_ALLVAR}},

 { F_PLISTE|F_IRET,  "WORT_SEP" , f_wort_sep ,3,5 ,{PL_STRING,PL_STRING,PL_INT,PL_SVAR,PL_SVAR}},

};
const int anzpfuncs=sizeof(pfuncs)/sizeof(FUNCTION);

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
        struct timeval t;
        struct timezone tz;
	gettimeofday(&t,&tz);
	return((double)t.tv_sec+(double)t.tv_usec/1000000);
#endif
}

int v_stimer() {   /* Sekunden-Timer */
  time_t timec=time(NULL);
  if(timec==-1) io_error(errno,"TIMER");
  return(timec);
}

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
 { NOTYP,   "!nulldummy", v_false},
#ifdef CONTROL
 { INTTYP,    "CCSAPLID",   v_ccsaplid},
#endif
 { INTTYP,    "CCSERR",     v_ccserr},
 { INTTYP,    "COLS",       v_cols},
 { FLOATTYP,  "CTIMER",     v_ctimer},
 { INTTYP,    "ERR",        v_err},
 { CONSTTYP|INTTYP,    "FALSE",      v_false},
#ifndef NOGRAPHICS
 { INTTYP,    "MOUSEK",     mousek},
 { INTTYP,    "MOUSES",     mouses},
 { INTTYP,    "MOUSEX",     mousex},
 { INTTYP,    "MOUSEY",     mousey},
#endif
 { INTTYP,    "PC",         v_pc},
 { CONSTTYP|FLOATTYP,  "PI",         v_pi},
 { INTTYP,    "ROWS",	    v_rows},
 { INTTYP,    "SP",	    v_sp},
 { INTTYP,    "STIMER",     v_stimer},
 { FLOATTYP,  "TIMER",      v_timer},
 { CONSTTYP|INTTYP,    "TRUE",       v_true},
#ifdef WINDOWS
 { CONSTTYP|INTTYP,    "WIN32?",     v_true},
#else
 { CONSTTYP|INTTYP,    "WIN32?",     v_false},
#endif
#ifndef WINDOWS
 { CONSTTYP|INTTYP,    "UNIX?",      v_true},
#else
 { CONSTTYP|INTTYP,    "UNIX?",      v_false},
#endif
};
const int anzsysvars=sizeof(sysvars)/sizeof(SYSVAR);


const SYSSVAR syssvars[]= {  /* alphabetisch !!! */
 { PL_LEER,   "!nulldummy", vs_error},
 { PL_STRING, "DATE$", vs_date},
 { PL_STRING, "FILEEVENT$", vs_fileevent},
 { PL_STRING, "INKEY$", vs_inkey},
 { PL_STRING, "TERMINALNAME$", vs_terminalname},
 { PL_STRING, "TIME$", vs_time},
 { PL_STRING, "TRACE$", vs_trace},
};
const int anzsyssvars=sizeof(syssvars)/sizeof(SYSSVAR);

const SFUNCTION psfuncs[]= {  /* alphabetisch !!! */

 { F_CONST|F_ARGUMENT,  "!nulldummy", f_nop ,0,0   ,{0}},
 { F_CONST|F_SQUICK,    "ARID$"     , f_arids ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "ARIE$"     , f_aries ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE,    "BIN$"      , f_bins ,1,2   ,{PL_INT,PL_INT}},
 { F_CONST|F_SQUICK,    "BWTD$"     , f_bwtds ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "BWTE$"     , f_bwtes ,1,1   ,{PL_STRING}},

 { F_CONST|F_IQUICK,    "CHR$"      , f_chrs ,1,1   ,{PL_INT}},
 { F_CONST|F_SQUICK,    "COMPRESS$" , f_compresss ,1,1   ,{PL_STRING}},
#ifdef CONTROL
 { F_SQUICK,    "CSGET$"    , f_csgets ,1,1   ,{PL_STRING}},
 { F_IQUICK,    "CSPNAME$"  , f_cspnames ,1,1   ,{PL_INT}},
 { F_SQUICK,    "CSUNIT$"   , f_csunits ,1,1   ,{PL_STRING}},
#endif
 { F_CONST|F_PLISTE,    "DECRYPT$", f_decrypts ,2,2   ,{PL_STRING,PL_STRING}},
#ifdef DOOCS
 { F_SQUICK,    "DOOCSGET$"    , f_doocsgets ,1,1   ,{PL_STRING}},
 { F_SQUICK,    "DOOCSINFO$"    , f_doocsinfos ,1,1   ,{PL_STRING}},
#endif

 { F_CONST|F_PLISTE,    "ENCRYPT$", f_encrypts ,2,2   ,{PL_STRING,PL_STRING}},
 { F_SQUICK,    "ENV$"    , f_envs ,1,1   ,{PL_STRING}},
 { F_CONST|F_IQUICK,    "ERR$"    , f_errs ,1,1   ,{PL_NUMBER}},
 { F_CONST|F_PLISTE,  "HEX$"    , f_hexs ,1,4   ,{PL_INT,PL_INT,PL_INT,PL_INT}},
 { F_CONST|F_SQUICK,    "INLINE$" , f_inlines ,1,1   ,{PL_STRING}},
 { F_ARGUMENT,  "INPUT$"  , f_inputs ,1,2   ,{PL_FILENR,PL_INT}},
 { F_CONST|F_IQUICK,    "JULDATE$" , f_juldates ,1,1   ,{PL_INT}},

 { F_CONST|F_PLISTE,    "LEFT$" , f_lefts ,1,2   ,{PL_STRING,PL_INT}},
 { F_PLISTE,    "LINEINPUT$" , f_lineinputs ,1,1   ,{PL_FILENR}},
 { F_CONST|F_SQUICK,    "LOWER$"    , f_lowers ,1,1   ,{PL_STRING}},

 { F_CONST|F_PLISTE,    "MID$"    , f_mids ,2,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_CONST|F_AQUICK,    "MKA$"    , array_to_string ,1,1   ,{PL_ARRAY}},
 { F_CONST|F_DQUICK,    "MKD$"    , f_mkds ,1,1   ,{PL_NUMBER}},
 { F_CONST|F_DQUICK,    "MKF$"    , f_mkfs ,1,1   ,{PL_NUMBER}},
 { F_CONST|F_IQUICK,    "MKI$"    , f_mkis ,1,1   ,{PL_INT}},
 { F_CONST|F_IQUICK,    "MKL$"    , f_mkls ,1,1   ,{PL_INT}},
 { F_CONST|F_DQUICK,    "MKS$"    , f_mkfs ,1,1   ,{PL_NUMBER}},
 { F_CONST|F_SQUICK,    "MTFD$"  , f_mtfds ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "MTFE$"  , f_mtfes ,1,1   ,{PL_STRING}},

 { F_CONST|F_PLISTE,  "OCT$"    , f_octs ,1,4   ,{PL_INT,PL_INT,PL_INT,PL_INT}},

 { F_IQUICK,    "PARAM$"  , f_params ,1,1   ,{PL_INT}},
 { F_CONST|F_IQUICK,    "PRG$"    , f_prgs ,1,1   ,{PL_INT}},
 { F_CONST|F_PLISTE,    "REPLACE$"  , f_replaces ,3,3   ,{PL_STRING,PL_STRING,PL_STRING}},
 { F_CONST|F_SQUICK,    "REVERSE$"  , f_reverses ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE,    "RIGHT$"  , f_rights ,1,2   ,{PL_STRING,PL_INT}},
 { F_CONST|F_SQUICK,    "RLD$"  , f_rlds ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "RLE$"  , f_rles ,1,1   ,{PL_STRING}},

 { F_CONST|F_IQUICK,    "SPACE$"  , f_spaces ,1,1   ,{PL_INT}},
 { F_CONST|F_PLISTE,  "STR$"    , f_strs ,1,4   ,{PL_NUMBER,PL_INT,PL_INT,PL_INT}},
 { F_CONST|F_PLISTE,  "STRING$" , f_strings ,2,2   ,{PL_INT,PL_STRING}},
 { F_SQUICK,    "SYSTEM$"    , f_systems ,1,1   ,{PL_STRING}},
 { F_PLISTE,    "TERMINALNAME$"    , f_terminalnames ,1,1 ,{PL_FILENR}},
#ifdef TINE
 { F_SQUICK,    "TINEGET$"    , f_tinegets ,1,1   ,{PL_STRING}},
 { F_SQUICK,    "TINEINFO$"   , f_tineinfos ,1,1   ,{PL_STRING}},
 { F_PLISTE,    "TINEQUERY$"  , f_tinequerys ,2,2   ,{PL_STRING,PL_INT}},
 { F_SQUICK,    "TINEUNIT$"   , f_tineunits ,1,1   ,{PL_STRING}},
#endif
 { F_CONST|F_SQUICK,    "TRIM$"   , f_trims ,1,1   ,{PL_STRING}},

 { F_CONST|F_SQUICK,    "UCASE$"    , f_uppers ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "UNCOMPRESS$" , f_uncompresss ,1,1   ,{PL_STRING}},
 { F_CONST|F_IQUICK,    "UNIXDATE$" , f_unixdates ,1,1   ,{PL_INT}},
 { F_CONST|F_IQUICK,    "UNIXTIME$" , f_unixtimes ,1,1   ,{PL_INT}},
 { F_CONST|F_SQUICK,    "UPPER$"    , f_uppers ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE,    "WORD$"    , f_words ,2,2   ,{PL_STRING,PL_INT}},
 { F_CONST|F_SQUICK,    "XTRIM$"   , f_xtrims ,1,1   ,{PL_STRING}},

};
const int anzpsfuncs=sizeof(psfuncs)/sizeof(SFUNCTION);


int vergleich(char *w1,char *w2) {
  int v;
  int e=type(w1)&(~CONSTTYP);
  if((e | INTTYP | FLOATTYP)!=((type(w2)&(~CONSTTYP)) | INTTYP | FLOATTYP )) {
    puts("Typen ungleich bei Vergleich!");
    printf("1: %d    2: %d \n",type(w1)&(~CONSTTYP),type(w2)&(~CONSTTYP));
    return(-1);
  }
  if(e & ARRAYTYP) {
    puts("Arrays/vergleich an dieser Stelle noch nicht möglich.");
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
  char *pos=NULL;
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
  int flags=FNM_NOESCAPE;
  if(e>=2) {
    if(e==3) flags^=plist[2].integer;
    flags=fnmatch(plist[1].pointer,plist[0].pointer,flags);
    if(flags==0) return(-1);
  } return(0);
}
#ifndef NOGRAPHICS
int f_form_alert(PARAMETER *plist,int e) {
  return(form_alert(plist[0].integer,plist[1].pointer));
}
int f_form_center(PARAMETER *plist,int e) {
  int x,y,w,h,ret;
  graphics();
  gem_init();
  if(e) ret=form_center((OBJECT *)plist[0].integer,&x,&y,&w,&h);
  if(e>4) {
    if(plist[1].typ!=PL_LEER) varcastint(plist[1].integer,plist[1].pointer,x);
    if(plist[2].typ!=PL_LEER) varcastint(plist[2].integer,plist[2].pointer,y);
    if(plist[3].typ!=PL_LEER) varcastint(plist[3].integer,plist[3].pointer,w);
    if(plist[4].typ!=PL_LEER) varcastint(plist[4].integer,plist[4].pointer,h);
  }
  return(ret);
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
    return(form_do((OBJECT *)plist[0].integer));
  } else return(-1);
}
int f_objc_draw(PARAMETER *plist,int e) {
  if(e==5) {
    graphics();
    gem_init();
    return(objc_draw((OBJECT *)plist[0].integer,plist[1].integer
    ,plist[2].integer,plist[3].integer,plist[4].integer));
  } else return(-1);
}
int f_objc_find(PARAMETER *plist,int e) {
    return(objc_find((OBJECT *)plist[0].integer,plist[1].integer,plist[2].integer));
}
int f_objc_offset(PARAMETER *plist,int e) {
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
int f_get_color(PARAMETER *plist,int e) {
  graphics();
  return(get_color(plist[0].integer,plist[1].integer,plist[2].integer));
}
int f_rsrc_gaddr(PARAMETER *plist,int e) {
  int i;
  char *ptr;
  i=rsrc_gaddr(plist[0].integer,plist[1].integer,&ptr);
  if(i>0) return((int)ptr);
  else return(0);
}
#endif



double parser(char *funktion){  /* Rekursiver num. Parser */
  char *pos,*pos2;
  char s[strlen(funktion)+1],w1buf[strlen(funktion)+1],w2buf[strlen(funktion)+1];
  int vnr;
  char *w1=w1buf;
  char *w2=w2buf;

//   printf("Parser: <%s>\n",funktion);

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
      xberror(0,w2); /* Division durch 0 */
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
  if(wort_sep_destroy(s,'=',TRUE,&w1,&w2)>1)   return(vergleich(w1,w2)?0:-1);
  if(wort_sep_destroy(s,'<',TRUE,&w1,&w2)>1)   return((vergleich(w1,w2)<0)?-1:0);
  if(wort_sep_destroy(s,'>',TRUE,&w1,&w2)>1)   return((vergleich(w1,w2)>0)?-1:0);
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
      else { xberror(0,w2); return(0);  } /* Division durch 0 */
    } else { xberror(51,w2); return(0); }/* "Parser: Syntax error?! "  */
  }
  if(wort_sepr(s,'^',TRUE,w1,w2)>1) {
    if(strlen(w1)) return(pow(parser(w1),parser(w2)));    /* von rechts !!  */
    else { xberror(51,w2); return(0); } /* "Parser: Syntax error?! "  */
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

      if(*pos2!=')') xberror(51,w2); /* "Parser: Syntax error?! "  */
      else {                         /* $-Funktionen und $-Felder   */
        *pos2=0;

        /* Benutzerdef. Funktionen */
        if(*s=='@') return(do_funktion(s+1,pos));
	else {
	  /* Liste durchgehen */
	  int i=0,stype,a=anzpfuncs-1,b,l=strlen(s);
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
		 double val1=0,val2=0;
	         if((e=wort_sep(pos,',',TRUE,w1,w2))==1) {
		   xberror(56,""); /* Falsche Anzahl Parameter */
		   val1=parser(w1); 
	         } else if(e==2) {
	           val1=parser(w1); 
		   val2=parser(w2);
	         }
                if(pfuncs[i].opcode&F_IRET) return((double)((int (*)())pfuncs[i].routine)(val1,val2));
		else return((pfuncs[i].routine)(val1,val2));
	      } else if(pfuncs[i].pmax==2 && (pfuncs[i].opcode&FM_TYP)==F_IQUICK) {
	       	 char w1[strlen(pos)+1],w2[strlen(pos)+1];
	         int e;
		 int val1=0,val2=0;
	         if((e=wort_sep(pos,',',TRUE,w1,w2))==1) {
		   xberror(56,""); /* Falsche Anzahl Parameter */
		   val1=(int)parser(w1); 
	         } else if(e==2) {
	           val1=(int)parser(w1); 
		   val2=(int)parser(w2);
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
	
          } else if((stype=type(s)) & FLOATTYP) {
	 // printf("Parser: stype=$%x, <%s>\n",stype,s);
            if((vnr=var_exist(s,ARRAYTYP,FLOATTYP,0))!=-1) {
	  //  printf("vnr=%d \n",vnr);
	  //  c_dump(NULL,0);
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      return(floatarrayinhalt2(a,indexliste));
	    } else { xberror(15,s); return(0); } /* Feld nicht dimensioniert  */
          } else if(stype & INTTYP) {
	    char *r=varrumpf(s);
	    if((vnr=var_exist(r,ARRAYTYP,INTTYP,0))!=-1) {
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      return((double)intarrayinhalt2(a,indexliste));
	    } else { xberror(15,s); return(0); }  /* Feld nicht dimensioniert  */
	    free(r);
	  } else { xberror(15,s); return(0); }  /* Feld nicht dimensioniert  */
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
	   if(sysvars[i].opcode&INTTYP) return((double)((int (*)())sysvars[i].routine)());
	   if(sysvars[i].opcode&FLOATTYP) return((sysvars[i].routine)());
          }
      /* erst integer abfangen (xxx% oder xxx&), dann rest */

      if(*s=='@')                              return(do_funktion(s+1,""));
      if((vnr=var_exist(s,FLOATTYP,0,0))!=-1) return(*variablen[vnr].pointer.f);
      if(s[strlen(s)-1]=='%') {
        s[strlen(s)-1]=0;
        if((vnr=var_exist(s,INTTYP,0,0))!=-1) return((double)*variablen[vnr].pointer.i);
        return(0);
      } else return(myatof(s));  /* Jetzt nur noch Zahlen (hex, oct etc ...)*/
    }
  }
  xberror(51,s); /* Syntax error */
  return(0);
}

const AFUNCTION pafuncs[]= {  /* alphabetisch !!! */
 { F_CONST|F_ARGUMENT,  "!nulldummy",  f_nop ,0,0   ,{0}},
 { F_CONST|F_PLISTE,    "0"         , f_nullmat ,2,2   ,{PL_INT,PL_INT}},
 { F_CONST|F_PLISTE,    "1"         , f_einsmat ,1,1   ,{PL_INT}},
#ifdef CONTROL
 { F_PLISTE,    "CSGET"     , f_csvgeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_PLISTE,    "CSVGET"    , f_csvgeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
#endif
 { F_CONST|F_SQUICK,    "CVA"       , string_to_array ,1,1   ,{PL_STRING}},
#ifdef DOOCS
 { F_PLISTE,    "DOOCSGET"     , f_doocsgeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_PLISTE,    "DOOCSNAMES"     , f_doocsnames ,1,1   ,{PL_STRING}},
 { F_PLISTE,    "DOOCSVGET"    , f_doocsgeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
#endif

 { F_CONST|F_AQUICK,  "INV"         , inv_array ,1,1   ,{PL_NARRAY}},
 { F_CONST|F_PLISTE,  "SMUL"        , f_smula ,2,2   ,{PL_ARRAY,PL_FLOAT}},
 { F_CONST|F_PLISTE,  "SOLVE"       , f_solvea ,2,2   ,{PL_ARRAY,PL_ARRAY}},

#ifdef TINE
 { F_PLISTE,    "TINEGET"     , f_tinegeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_PLISTE,    "TINEVGET"    , f_tinegeta ,1,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_PLISTE,    "TINEHISTORY" , f_tinehistorya ,3,3   ,{PL_STRING,PL_INT,PL_INT}},
#endif
 { F_CONST|F_AQUICK,    "TRANS"     , trans_array ,1,1   ,{PL_ARRAY}},

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
//  printf("ARRAY_PARSER: \n");
  if(wort_sep(s,'+',TRUE,w1,w2)>1) {
    if(strlen(w1)) {
      ARRAY zw1=array_parser(w1);
      ARRAY zw2=array_parser(w2);
      array_add(zw1,zw2);
      free_array(&zw2);
      return(zw1);
    } else return(array_parser(w2));
  } else if(wort_sepr(s,'-',TRUE,w1,w2)>1) {
    if(strlen(w1)) {
      ARRAY zw1=array_parser(w1);
      ARRAY zw2=array_parser(w2);
      array_sub(zw1,zw2);
      free_array(&zw2);
      return(zw1);
    } else {
      ARRAY zw2=array_parser(w2);
      array_smul(zw2,-1);
      return(zw2);
    }
  } else if(wort_sepr(s,'*',TRUE,w1,w2)>1) {
    if(strlen(w1)) {
      if(type(w1) & ARRAYTYP) {
        ARRAY zw1=array_parser(w1);
	ARRAY zw2=array_parser(w2);
        ARRAY ergebnis=mul_array(zw1,zw2);
        free_array(&zw1); free_array(&zw2);
        return(ergebnis);
      } else {    /* Skalarmultiplikation */
        ARRAY zw2=array_parser(w2);
	array_smul(zw2,parser(w1));
	return(zw2);
      }
    } else xberror(51,""); /*Syntax Error*/
  } else if(wort_sepr(s,'^',TRUE,w1,w2)>1) {
    ARRAY zw2,zw1=array_parser(w1);
    e=(int)parser(w2);
    if(e<0) xberror(51,""); /*Syntax Error*/
    else if(e==0) {
      zw2=zw1;
      zw1=einheitsmatrix(zw2.typ,zw2.dimension,zw2.pointer);
      free_array(&zw2);
    } else if(e>1) {
      int i;
      for(i=1;i<e;i++) {
        zw2=mul_array(zw1,zw1);
	free_array(&zw1);
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
         xberror(51,w2); /* "Parser: Syntax error?! "  */
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
		free_array(&a);
	      	return(ergebnis);
	      } else if(pafuncs[i].pmax==1 && (pafuncs[i].opcode&FM_TYP)==F_SQUICK) {
	        ARRAY ergebnis;
		STRING a=string_parser(pos);
		ergebnis=(pafuncs[i].routine)(a);
		free_string(&a);
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
	
           ergeb.typ=INTTYP;
	   ergeb.dimension=1;
	   ergeb.pointer=malloc(INTSIZE+nn*sizeof(int));
	   ((int *)(ergeb.pointer))[0]=nn;
           if(filenr[i]) {
	     int *varptr=ergeb.pointer+INTSIZE;
             if(i=fread(varptr,sizeof(int),nn,dptr[i])<nn) {
	       io_error(errno,"fread");
	     }
             return(ergeb);
           } else xberror(24,""); /* File nicht geoeffnet */
           return(ergeb);
	 } else {
	    int vnr;
	    char *r=varrumpf(s);
	    ARRAY ergebnis;
	   /* Hier sollten keine Funktionen mehr auftauchen  */
	   /* Jetzt uebergebe spezifiziertes Array, evtl. reduziert*/
	   if(strlen(pos)==0) {
	     if((vnr=var_exist(r,ARRAYTYP,0,0))!=-1) ergebnis=double_array(variablen[vnr].pointer.a);
             else xberror(15,s);  /* Feld nicht dimensioniert  */
	   } else {
	     if((vnr=var_exist(r,ARRAYTYP,0,0))!=-1) {
	       int indexliste[variablen[vnr].pointer.a->dimension];
	       int ii=count_parameters(pos);
	       make_indexliste(ii,pos,indexliste);
	       ergebnis=get_subarray(variablen[vnr].pointer.a,indexliste);	
	     } else {
	       xberror(15,s);  /* Feld nicht dimensioniert  */
	       e=1;
  	       ergebnis=einheitsmatrix(FLOATTYP,1,&e);
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
  return(nullmatrix(FLOATTYP,0,&e));
}


char *s_parser(char *funktion) { /* String-Parser  */
  STRING e=string_parser(funktion);
  return(e.pointer);
}
STRING string_parser(char *funktion) {
/* Rekursiv und so, dass dynamische Speicherverwaltung ! */
/* Trenne ersten Token ab, und uebergebe rest derselben Routine */

  STRING ergebnis;
  char v[strlen(funktion)+1],w[strlen(funktion)+1];

 // printf("S-Parser: <%s>\n",funktion);
  if(wort_sep(funktion,'+',TRUE,v,w)>1) {
    STRING t=string_parser(v);
    STRING u=string_parser(w);
    ergebnis.pointer=malloc(t.len+u.len+1);
    memcpy(ergebnis.pointer,t.pointer,t.len);
    memcpy(ergebnis.pointer+t.len,u.pointer,u.len+1);
    ergebnis.len=u.len+t.len;
    ergebnis.pointer[ergebnis.len]=0;
    free(t.pointer);free(u.pointer);
    return(ergebnis);
  } else {
    char *pos,*pos2;
    int vnr;

  //  printf("s-parser: <%s>\n",funktion);
    strcpy(v,funktion);
    pos=searchchr(v, '(');
    if(pos!=NULL) {
      pos2=v+strlen(v)-1;
      *pos++=0;

      if(*pos2!=')') {
        xberror(51,v); /* "Parser: Syntax error?! "  */
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
//          printf("s-parser: <%s>\n",funktion);
  
          if(strcmp(v,psfuncs[i].name)==0) {
//          printf("Funktion gefunden <%s>\n",psfuncs[i].name);
//          printf("opcode=%d pmax=%d  AQUICK=%d\n",psfuncs[i].opcode&FM_TYP,psfuncs[i].pmax,F_AQUICK);
	  
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
		 double val1=0,val2=0;
	         if((e=wort_sep(pos,',',TRUE,w1,w2))==1) {
		   xberror(56,""); /* Falsche Anzahl Parameter */
		   val1=parser(w1); 
	         } else if(e==2) { 
		   val1=parser(w1); 
		   val2=parser(w2);
		 }
                 ergebnis=(psfuncs[i].routine)(val1,val2);
	      } else if(psfuncs[i].pmax==1 && (psfuncs[i].opcode&FM_TYP)==F_SQUICK) {
                STRING test=string_parser(pos);
		test.pointer=realloc(test.pointer,test.len+1);
		test.pointer[test.len]=0;
	        ergebnis=(psfuncs[i].routine)(test);
		free(test.pointer);
	      } else if(psfuncs[i].pmax==1 && (psfuncs[i].opcode&FM_TYP)==F_AQUICK) {
                ARRAY test=array_parser(pos);
	        ergebnis=(psfuncs[i].routine)(test);
		free_array(&test);
	      } else printf("Interner ERROR. Funktion nicht korrekt definiert. %s\n",v);
	    } else {/* Nicht in der Liste ? Dann kann es noch ARRAY sein   */
	     int vnr;
	     v[strlen(v)-1]=0;
	
             if((vnr=var_exist(v,ARRAYTYP,STRINGTYP,0))==-1) {
	       xberror(15,v);         /*Feld nicht definiert*/
	       ergebnis=create_string(NULL);
             } else {
	       int dim=variablen[vnr].pointer.a->dimension;
	       int indexliste[dim];
	       
	       if(make_indexliste(dim,pos,indexliste)==0)
	         ergebnis=varstringarrayinhalt(vnr,indexliste);
	       else ergebnis=create_string(NULL);
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
	ergebnis.pointer[ergebnis.len]=0;
      } else if(*pos2!='$') {
        xberror(51,v); /* "Parser: Syntax error?! "  */
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
        if((vnr=var_exist(v,STRINGTYP,0,0))==-1) ergebnis=create_string(NULL);
        else ergebnis=double_string(variablen[vnr].pointer.s);
      }
    }
  }
  return(ergebnis);
}



double do_funktion(char *name,char *argumente) {
  int oldbatch,osp=sp;
  int pc2=procnr(name,2|4);  /*FUNCTION und DEFFN*/
  int typ;
//printf("do function <%s> <%s>\n",name,argumente);
  if(pc2==-1) {
    xberror(44,name); /* Funktion  nicht definiert */
    return(0.0);
  } 
  typ=procs[pc2].typ;
  
  if(do_parameterliste(argumente,procs[pc2].parameterliste,procs[pc2].anzpar)) {
    xberror(42,name); /* Zu wenig Parameter */
    return(0.0);
  }
  pc2=procs[pc2].zeile;

  if(typ==4) {
//    printf("Auszuwerten: <%s>\n",pcode[pc2].argument);
    returnvalue.f=parser(pcode[pc2].argument);
    restore_locals(sp+1);
  } else {
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
  return(returnvalue.f);
}

 /* loese die Parameterliste auf und weise die Werte auf die neuen lokalen
    Variablen zu */

int do_parameterliste(char *pos, int *l,int n) {
  char buf[strlen(pos)+1];
  char *w1,*w2;
  int e;
  int i=0;
  strcpy(buf,pos);
  e=wort_sep_destroy(buf,',',TRUE,&w1,&w2);

  sp++;  /* Uebergabeparameter sind lokal ! */
  while(e && i<n) {
  /*  printf("ZU: %s=%s\n",w3,w5); */
    c_dolocal(l[i++],w1);
    e=wort_sep_destroy(w2,',',TRUE,&w1,&w2);
  }
  sp--;
  if(e || i<n) xberror(56,pos); /* Falsche Anzahl Parameter */
  return((i!=n) ? 1 : 0);
}


STRING do_sfunktion(char *name,char *argumente) {
  int oldbatch,osp=sp;
  int pc2=procnr(name,2|4);  /*FUNCTION und DEFFN*/
  int typ;
  if(pc2==-1) {
    xberror(44,name); /* Funktion  nicht definiert */
    return(create_string(NULL));
  } 
  typ=procs[pc2].typ;
  if(do_parameterliste(argumente,procs[pc2].parameterliste,procs[pc2].anzpar)) {
    xberror(42,name); /* Zu wenig Parameter */
    return(create_string(NULL));
  }
  pc2=procs[pc2].zeile;

  if(typ==4) {
//    printf("Auszuwerten: <%s>\n",pcode[pc2].argument);
    returnvalue.str=string_parser(pcode[pc2].argument);
    restore_locals(sp+1);
  } else {
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
  return(returnvalue.str);
}
