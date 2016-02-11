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

#include "defs.h"
#include "globals.h"
#include "protos.h"

double f_nop(void *t) {return(0.0);}
int f_point(double v1, double v2) {  return(get_point((int)v1,(int)v2)); }
int f_bclr(double v1, double v2) { return((int)v1 & ~ (1 <<((int)v2))); }
int f_bset(double v1, double v2) { return((int)v1 | (1 <<((int)v2))); }
int f_bchg(double v1, double v2) { return((int)v1 ^ (1 <<((int)v2))); }
int f_btst(double v1, double v2) { return((((int)v1 & (1 <<((int)v2)))==0) ?  0 : -1); }

int f_instr(PARAMETER *,int);
int f_rinstr(PARAMETER *,int);
int f_glob(PARAMETER *,int);

extern int f_symadr(char *);
extern int f_exec(char *);
int f_int(double b) {return((int)b);}
int f_fix(double b) {if(b>=0) return((int)b);
                      else return(-((int)(-b)));}
double f_pred(double b) {return(ceil(b-1));}
int f_succ(double b) {return((int)(b+1));}
int f_sgn(double b) {return(sgn(b));}
double f_frac(double b) {return(b-((double)((int)b)));}
int f_even(int b) {return(b&1 ? 0:-1);}
int f_odd(int b) {return(b&1 ? -1:0);}
int f_fak(double);
int f_random(double d) {return((int)((double)rand()/RAND_MAX*d));}
double f_rnd(double d) {return((double)rand()/RAND_MAX);}
int  f_srand(double d) {srand((int)d);return(0);}
double f_deg(double d) {return(d/PI*180);}
double f_rad(double d) {return(d*PI/180);}
int  f_dimf(char *pos) {return((double)do_dimension(variable_exist_type(pos)));}
int f_asc(STRING n) {  return((int)n.pointer[0]); }
int f_cvi(STRING n) {  return((int)(*((short *)n.pointer))); }
int f_cvl(STRING n) {  return((int)(*((long *)n.pointer))); }
double f_cvd(STRING n) {  return((double)(*((double *)n.pointer))); }
double f_cvf(STRING n) {  return((double)(*((float *)n.pointer))); }
int f_len(STRING n) { return(n.len); }
int f_exist(STRING n) { return(-exist(n.pointer)); }
double f_val(STRING n) { return((double)atof(n.pointer)); }
double f_ltextlen(STRING n) { return((double)ltextlen(ltextxfaktor,ltextpflg,n.pointer)); }
char *arrptr(char *);
double f_csmax(STRING n) { return(csmax(n.pointer)); }
double f_csmin(STRING n) { return(csmin(n.pointer)); }
double f_csres(STRING n) { return(csres(n.pointer)); }
double f_csget(STRING n) { return(csget(n.pointer)); }
int f_cssize(STRING n) { return(cssize(n.pointer)); }
int f_cspid(STRING n) { return(cspid(n.pointer)); }

const FUNCTION pfuncs[]= {  /* alphabetisch !!! */

 { F_ARGUMENT|F_DRET,  "!nulldummy", f_nop ,0,0   ,{0}},
 { F_DQUICK|F_DRET,    "ABS"       , fabs ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "ACOS"      , acos ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "ACOSH"      , acosh ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "ARRPTR"    , arrptr ,1,1     ,{PL_ARRAY}},
 { F_SQUICK|F_IRET,    "ASC"       , f_asc ,1,1   ,{PL_STRING}},
 { F_DQUICK|F_DRET,    "ASIN"      , asin ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "ASINH"      , asinh ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "ATAN"      , atan ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "ATAN2"     , atan2 ,2,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_DQUICK|F_DRET,    "ATANH"     , atanh ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "ATN"       , atan ,1,1     ,{PL_NUMBER}},

 { F_DQUICK|F_IRET,    "BCHG"      , f_bchg,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_DQUICK|F_IRET,    "BCLR"      , f_bclr,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_DQUICK|F_IRET,    "BSET"      , f_bset,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_DQUICK|F_IRET,    "BTST"      , f_btst,2,2     ,{PL_NUMBER,PL_NUMBER }},

 { F_DQUICK|F_DRET,    "CBRT"      , cbrt ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "CEIL"      , ceil ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "COS"       , cos ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "COSH"      , cosh ,1,1     ,{PL_NUMBER}},
 { F_SQUICK|F_DRET,  "CSGET"     , f_csget ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSMAX"     , f_csmax ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSMIN"     , f_csmin ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CSPID"     , f_cspid ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CSRES"     , f_csres ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CSSIZE"    , f_cssize ,1,1   ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CVD"       , f_cvd ,1,1     ,{PL_STRING}},
 { F_SQUICK|F_DRET,  "CVF"       , f_cvf ,1,1     ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CVI"       , f_cvi ,1,1     ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CVL"       , f_cvl ,1,1     ,{PL_STRING}},
 { F_SQUICK|F_IRET,  "CVS"       , f_cvf ,1,1     ,{PL_STRING}},

 { F_DQUICK|F_DRET,    "DEG"       , f_deg ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "DIM?"      , f_dimf ,1,1      ,{PL_ARRAY}},

 { F_IQUICK|F_IRET,    "EVEN"       , f_even ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "EXEC"       , f_exec ,1,2     ,{PL_NUMBER,PL_NUMBER}},
 { F_SQUICK|F_IRET,    "EXIST"      , f_exist ,1,1     ,{PL_STRING}},
 { F_DQUICK|F_DRET,    "EXP"       , exp ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "EXPM1"     , expm1 ,1,1     ,{PL_NUMBER}},

 { F_DQUICK|F_IRET,    "FAK"       , f_fak ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "FIX"       , f_fix ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "FLOOR"     , floor ,1,1     ,{PL_NUMBER}},
 { F_SIMPLE|F_IRET,    "FORK"     , fork ,0,0     },
 { F_DQUICK|F_DRET,    "FRAC"      , f_frac ,1,1     ,{PL_NUMBER}},

 { F_PLISTE|F_IRET,    "GLOB"     , f_glob ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},

 { F_DQUICK|F_DRET,    "HYPOT"     , hypot ,2,2     ,{PL_NUMBER,PL_NUMBER}},


 { F_ARGUMENT|F_IRET,  "INP"       , inp8 ,1,1      ,{PL_FILENR}},
 { F_ARGUMENT|F_IRET,  "INP?"      , inpf ,1,1      ,{PL_FILENR}},
 { F_ARGUMENT|F_IRET,  "INP&"      , inp16 ,1,1      ,{PL_FILENR}},
 { F_ARGUMENT|F_IRET,  "INP%"      , inp32 ,1,1      ,{PL_FILENR}},
 { F_PLISTE|F_IRET,  "INSTR"     , f_instr ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},

 { F_DQUICK|F_IRET,    "INT"       , f_int ,1,1     ,{PL_NUMBER}},
 
 { F_SQUICK|F_IRET,    "LEN"       , f_len ,1,1   ,{PL_STRING}},
 { F_DQUICK|F_DRET,    "LN"        , log ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "LOG"       , log ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "LOG10"     , log10 ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "LOG1P"     , log1p ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "LOGB"      , logb  ,1,1     ,{PL_NUMBER}},
 { F_SQUICK|F_DRET,    "LTEXTLEN"  , f_ltextlen ,1,1   ,{PL_STRING}},

 { F_DQUICK|F_DRET,    "MOD"       , fmod ,2,2     ,{PL_NUMBER,PL_NUMBER }},
 { F_IQUICK|F_IRET,    "ODD"       , f_odd ,1,1     ,{PL_NUMBER}},

 { F_DQUICK|F_IRET,    "POINT"     , f_point ,2,2     ,{PL_NUMBER, PL_NUMBER }},
 { F_DQUICK|F_DRET,    "PRED"      , f_pred ,1,1     ,{PL_NUMBER}},

 { F_DQUICK|F_DRET,    "RAD"      , f_rad ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "RAND"      , rand ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "RANDOM"    , f_random ,1,1     ,{PL_NUMBER}},
 { F_PLISTE|F_IRET,  "RINSTR"    , f_rinstr ,2,3  ,{PL_STRING,PL_STRING,PL_INT}},
 { F_DQUICK|F_DRET,    "RND"       , f_rnd ,1,1     ,{PL_NUMBER}},

 { F_DQUICK|F_IRET,    "SGN"       , f_sgn ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SIN"       , sin ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SINH"      , sinh ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SQR"       , sqrt ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SQRT"      , sqrt ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "SRAND"     , f_srand ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "SUCC"      , f_succ ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "SYM_ADR"   , f_symadr ,2,2     ,{PL_FILENR,PL_STRING}},


 { F_DQUICK|F_DRET,    "TAN"       , tan ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "TANH"       , tanh ,1,1     ,{PL_NUMBER}},
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
    ergebnis.pointer[0]=0;
  } else {
    ergebnis.pointer=malloc(strlen(ttt)+1);
    strcpy(ergebnis.pointer,ttt);
  }
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_systems(STRING n) {   
  STRING ergebnis;
  FILE *dptr=popen(n.pointer,"r");

  if (dptr==NULL) {
    ergebnis.pointer=malloc(32+n.len);
    sprintf(ergebnis.pointer,"couldn't execute '%s'",n.pointer);
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
    pclose(dptr);
  }
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
STRING f_strings(char *pos) {
  STRING buffer,ergebnis;
  char w1[strlen(pos)+1],w2[strlen(pos)+1];
  int i=0,j;
  wort_sep(pos,',',TRUE,w1,w2);
  j=(int)parser(w1);
  buffer=string_parser(w2);
  ergebnis.pointer=malloc(j*buffer.len+1);
  ergebnis.len=j*buffer.len;
  while(i<j) {memcpy(ergebnis.pointer+i*buffer.len,buffer.pointer,buffer.len); i++;}
  free(buffer.pointer);
  return(ergebnis);
}
STRING f_rights(char *pos) {
  STRING buffer,ergebnis;
  char w1[strlen(pos)+1],w2[strlen(pos)+1];
  int e,j;
  e=wort_sep(pos,',',TRUE,w1,w2);
  buffer=string_parser(w1);
  if(e<2) j=1;
  else j=min(max((int)parser(w2),0),buffer.len); 
  ergebnis.pointer=malloc(j+1);
  ergebnis.len=j;
  memcpy(ergebnis.pointer,buffer.pointer+buffer.len-j,j);
  free(buffer.pointer);
  return(ergebnis);
}

STRING f_lefts(char *pos) {
  STRING ergebnis;
  char w1[strlen(pos)+1],w2[strlen(pos)+1];
  int e,j;
  e=wort_sep(pos,',',TRUE,w1,w2);
  ergebnis=string_parser(w1);
  if(e<2) j=1;
  else j=min(max((int)parser(w2),0),ergebnis.len);
  ergebnis.pointer[j]=0;
  ergebnis.len=j;
  return(ergebnis);
}
STRING f_mids(char *pos) {  
  STRING buffer,ergebnis;
  char w1[strlen(pos)+1],w2[strlen(pos)+1];
  int i=0,e=wort_sep(pos,',',TRUE,w1,w2); 
  int p=1,l=1;
  while(e) {
    if(i==0) buffer=string_parser(w1);
    else if(strlen(w1)) {
      switch(i) {
	case 1: {p=min(buffer.len,max(1,(int)parser(w1)));break;}
	case 2: {l=min(buffer.len-p+1,max(0,(int)parser(w1)));break;}
        default: break;
      }
    }
    e=wort_sep(w2,',',TRUE,w1,w2);
    i++;
  }
  ergebnis.pointer=malloc(l+1),
  ergebnis.len=l;
  if(i) {
    memcpy(ergebnis.pointer,buffer.pointer+p-1,l);
    free(buffer.pointer);
  } else error(42,"MID$()"); /* Zu wenig Parameter */
  return(ergebnis);
}

const SFUNCTION psfuncs[]= {  /* alphabetisch !!! */

 { F_ARGUMENT,  "!nulldummy", f_nop ,0,0   ,{0}},
 { F_PLISTE,  "BIN$"    , f_bins ,1,2   ,{PL_INT,PL_INT}},

 { F_IQUICK,    "CHR$"    , f_chrs ,1,1   ,{PL_NUMBER}},
 { F_SQUICK,    "ENV$"    , f_envs ,1,1   ,{PL_STRING}},
 { F_IQUICK,    "ERR$"    , f_errs ,1,1   ,{PL_NUMBER}},
 { F_ARGUMENT,  "HEX$"    , f_hexs ,1,4   ,{PL_INT,PL_INT,PL_NUMBER,PL_NUMBER}},
 { F_ARGUMENT,  "INPUT$"  , f_inputs ,1,2   ,{PL_FILENR,PL_NUMBER}},

 { F_ARGUMENT,  "LEFT$" , f_lefts ,1,2   ,{PL_STRING,PL_INT}},
 { F_ARGUMENT,  "LINEINPUT$" , f_lineinputs ,1,1   ,{PL_FILENR}},
 { F_SQUICK,    "LOWER$"    , f_lowers ,1,1   ,{PL_STRING}},

 { F_ARGUMENT,  "MID$"    , f_mids ,2,3   ,{PL_STRING,PL_NUMBER,PL_NUMBER}},
 { F_DQUICK,    "MKD$"    , f_mkds ,1,1   ,{PL_NUMBER}},
 { F_DQUICK,    "MKF$"    , f_mkfs ,1,1   ,{PL_NUMBER}},
 { F_IQUICK,    "MKI$"    , f_mkis ,1,1   ,{PL_NUMBER}},
 { F_IQUICK,    "MKL$"    , f_mkls ,1,1   ,{PL_NUMBER}},
 { F_DQUICK,    "MKS$"    , f_mkfs ,1,1   ,{PL_NUMBER}},
 { F_ARGUMENT,  "OCT$"    , f_octs ,1,4   ,{PL_INT,PL_INT,PL_NUMBER,PL_NUMBER}},
 
 { F_IQUICK,    "PARAM$"  , f_params ,1,1   ,{PL_INT}},
 { F_IQUICK,    "PRG$"    , f_prgs ,1,1   ,{PL_INT}},
 { F_ARGUMENT,  "RIGHT$"  , f_rights ,1,2   ,{PL_STRING,PL_INT}},
 { F_IQUICK,    "SPACE$"  , f_spaces ,1,1   ,{PL_INT}},
 { F_PLISTE,  "STR$"    , f_strs ,1,4   ,{PL_NUMBER,PL_INT,PL_INT,PL_INT}},
 { F_ARGUMENT,  "STRING$" , f_strings ,1,2   ,{PL_INT,PL_STRING}},
 { F_SQUICK,    "SYSTEM$"    , f_systems ,1,1   ,{PL_STRING}},
 { F_SQUICK,    "TRIM$"   , f_trims ,1,1   ,{PL_STRING}},

 { F_SQUICK,    "UPPER$"    , f_uppers ,1,1   ,{PL_STRING}},
 { F_SQUICK,    "XTRIM$"   , f_xtrims ,1,1   ,{PL_STRING}},

};
const int anzpsfuncs=sizeof(psfuncs)/sizeof(FUNCTION);

int f_fak(double n) {
  int i,s=1,k=(int)n;
  for(i=2;i<=k;i++) {s=s*i;} 
  return(s);
}

int vergleich(char *w1,char *w2) {
  int v;
  int e=type2(w1);
  if((e | INTTYP | FLOATTYP)!=(type2(w2) | INTTYP | FLOATTYP )) {
    printf("Typen ungleich bei Vergleich..\n");
    printf("1: %d    2: %d \n",type2(w1),type2(w2));
    return(0);
  }
  if(e & ARRAYTYP) { 
    printf("Arrays an dieser Stelle noch nicht moeglich.\n");
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
#include <fnmatch.h>
int f_glob(PARAMETER *plist,int e) {
  char *pos=NULL,*n;
  int flags=FNM_NOESCAPE;
  if(e>=2) {
    if(e==3) flags^=plist[2].integer;
    flags=fnmatch(plist[1].pointer,plist[0].pointer,flags);
    if(flags==0) return(-1);
  } return(0);
}

double parser(char *funktion){  /* Rekursiver num. Parser */
  char *pos,*pos2;
  char s[strlen(funktion)+1],w1[strlen(funktion)+1],w2[strlen(funktion)+1];
  int e,vnr;

  strcpy(s,funktion);
  xtrim(s,TRUE,s);  /* Leerzeichen vorne und hinten entfernen */

  /* printf("Parser: <%s>\n");*/
  /* Dann Logische Operatoren AND OR NOT ... */ 

  if(wort_sepr2(s," AND ",TRUE,w1,w2)>1)  return((double)((int)parser(w1) & (int)parser(w2)));    /* von rechts !!  */
  if(wort_sepr2(s,"&&",TRUE,w1,w2)>1)     return((double)((int)parser(w1) & (int)parser(w2)));    
  if(wort_sepr2(s," OR ",TRUE,w1,w2)>1)   return((double)((int)parser(w1) | (int)parser(w2)));    
  if(wort_sepr2(s,"||",TRUE,w1,w2)>1)     return((double)((int)parser(w1) | (int)parser(w2)));    
  if(wort_sepr2(s,"||",TRUE,w1,w2)>1)     return((double)((int)parser(w1) | (int)parser(w2)));    
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
  if(wort_sep2(s,"==",TRUE,w1,w2)>1) { /* Erst Vergleichsoperatoren mit Wahrheitwert abfangen (lowlevel < Addition)  */
    if(vergleich(w1,w2)==0) return(-1); else return(0);
  } else if(wort_sep2(s,"<>",TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)!=0) return(-1); else return(0);
  } else if(wort_sep2(s,"><",TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)!=0) return(-1);  else return(0);
  } else if(wort_sep2(s,"<=",TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)<=0) return(-1);  else return(0);
  } else if(wort_sep2(s,">=",TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)>=0) return(-1);  else return(0);
  } else if(wort_sep(s,'=',TRUE,w1,w2)>1) {
    if(vergleich(w1,w2)==0) return(-1);  else return(0);
  } else if(wort_sep(s,'<',TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)<0)  return(-1);  else return(0);
  } else if(wort_sep(s,'>',TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)>0)  return(-1);  else return(0);
  } 
  if(wort_sep(s,'+',TRUE,w1,w2)>1) {           /* Addition/Subtraktion/Vorzeichen  */
    if(strlen(w1)) return(parser(w1)+parser(w2));
    else return(parser(w2));   /* war Vorzeichen + */
  } else if(wort_sepr(s,'-',TRUE,w1,w2)>1) {       /* von rechts !!  */    
    if(strlen(w1)) {
      if(strlen(w1)>1 && w1[strlen(w1)-1]=='E') {
        if(strchr("1234567890.",w1[strlen(w1)-2])==NULL)  return(parser(w1)-parser(w2));
        else printf("+++Konflikt ! 1<%s> 2<%s>\n",w1,w2);
      } else return(parser(w1)-parser(w2));    
    } else return(-parser(w2));   /* war Vorzeichen - */
  } else if(wort_sepr(s,'*',TRUE,w1,w2)>1) {           
    if(strlen(w1)) return(parser(w1)*parser(w2));    
    else printf("Pointer noch nicht integriert! %s\n",w2);   /* war pointer - */
  } else if(wort_sepr(s,'/',TRUE,w1,w2)>1) {        
    if(strlen(w1)) {
      double nenner;
      nenner=parser(w2); 
      if(nenner!=0.0) return(parser(w1)/nenner);    /* von rechts !!  */
      else { error(0,w2); return(0);  } /* Division durch 0 */
    } else { error(51,w2); return(0); }/* "Parser: Syntax error?! "  */
  } else if(wort_sepr(s,'^',TRUE,w1,w2)>1) {           
    if(strlen(w1)) return(pow(parser(w1),parser(w2)));    /* von rechts !!  */
    else { error(51,w2); return(0); } /* "Parser: Syntax error?! "  */
  } 
  if(s[0]=='(' && s[strlen(s)-1]==')')  { /* Ueberfluessige Klammern entfernen */
    s[strlen(s)-1]=0;
    return(parser(s+1));
    /* SystemFunktionen Subroutinen und Arrays */
  } else {
    pos=searchchr(s, '(');
    if(pos!=NULL) {
      pos2=s+strlen(s)-1;
      pos[0]=0;
      pos++;
    
      if(pos2[0]!=')') error(51,w2); /* "Parser: Syntax error?! "  */
      else {                         /* $-Funktionen und $-Felder   */
        pos2[0]=0;

        /* Funktionen, die double zurueckliefern */

        if(strcmp(s,"LOF")==0)  {
	  int i=get_number(pos);
	  if(filenr[i]) return((double)lof(dptr[i]));
	  else { error(24,""); return(0);} /* File nicht geoeffnet */
        }else if(strcmp(s,"LOC")==0)  {
	  int i=get_number(pos);
	  if(filenr[i]) return((double)ftell(dptr[i]));
	  else { error(24,""); return(0); } /* File nicht geoeffnet */
	}else if(strcmp(s,"EOF")==0)  {
	  int i=get_number(pos);
	  if(i==-2) return((double)((eof(stdin)) ? -1 : 0));
	  else if(filenr[i]) return((double)((eof(dptr[i])) ? -1 : 0));
	  else { error(24,""); return(0); } /* File nicht geoeffnet */	
	} else if(strcmp(s,"MIN")==0)  {
	  int e;
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) return(parser(w1));
	  else if(e==2)	  return(min(parser(w1),parser(w2)));
	}else if(strcmp(s,"MAX")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) return(parser(w1));
	  else if(e==2)	  return(max(parser(w1),parser(w2)));
	}else if(strcmp(s,"FORM_ALERT")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) return(0);
	  else if(e==2)	 {
	    char *ttt=s_parser(w2);
	    double ergebnis=form_alert((int)parser(w1),ttt);
	    free(ttt);
	    return(ergebnis);
	  }
	}else if(strcmp(s,"ADD")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) return(parser(w1));
	  else if(e==2)	  return(parser(w1)+parser(w2));
	}else if(strcmp(s,"SUB")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) return(parser(w1));
	  else if(e==2)	  return(parser(w1)-parser(w2));
	 }else if(strcmp(s,"MUL")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) return(parser(w1));
	  else if(e==2)	  return(parser(w1)*parser(w2));
	 }else if(strcmp(s,"DIV")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) return(parser(w1));
	  else if(e==2)	  return(parser(w1)/parser(w2));
	 }
	 else if(strcmp(s,"ROUND")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) return(round(parser(w1)));
	  else if(e==2)	{
	    int kommast=-(int)parser(w2);
	    return(round(parser(w1)/pow(10,kommast))*pow(10,kommast)); 
	  }
	  
	}else if(strcmp(s,"GET_COLOR")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1],w3[strlen(pos)+1];
	 
	  wort_sep(pos,',',TRUE,w1,w2);
	  wort_sep(w2,',',TRUE,w2,w3);

	  return((double)get_color((int)parser(w1),(int)parser(w2),(int)parser(w3)));
	}else if(strcmp(s,"MALLOC")==0)  return((double)(int)malloc((int)parser(pos)));
	else if(strcmp(s,"FREE")==0)  {free((char *)((int)parser(pos)));return((double)errno);}
	else if(strcmp(s,"PEEK")==0)  return((double)(int)((char *)((int)parser(pos)))[0]);
	else if(strcmp(s,"LPEEK")==0)  return((double)(int)((int *)((int)parser(pos)))[0]);
	else if(strcmp(s,"DPEEK")==0)  return((double)(int)((short *)((int)parser(pos)))[0]);  


        else 
	if(s[0]=='@') return(do_funktion(s+1,pos));
	else {
	  /* Liste durchgehen */
	  int i=0,oa,a=anzpfuncs,b;
          for(b=0; b<strlen(s); b++) {
            while(s[b]>(pfuncs[i].name)[b] && i<a) i++;
            oa=a;a=i;
            while(s[b]<(pfuncs[a].name)[b]+1 && a<oa) a++;
            if(i==a) break;
          }
          if(i<anzpfuncs && strcmp(s,pfuncs[i].name)==0) {
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
		   printf("Falsche Anzahl Parameter");
		   val1=parser(w1); val2=0;
	         } else if(e==2)	{
	           val1=parser(w1); val2=parser(w2);
	         }
                if(pfuncs[i].opcode&F_IRET) return((double)((int (*)())pfuncs[i].routine)(val1,val2));
		else return((pfuncs[i].routine)(val1,val2));
	       
	      } else if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_SQUICK) {
                STRING test=string_parser(pos);
		double erg;
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
    } else {
      /* Dann Systemvariablen und einfache Variablen */
      /* erst integer abfangen (xxx% oder xxx&), dann rest */
      if(strcmp(s,"STIMER")==0) { /* Sekunden-Timer */
        time_t timec;
        timec = time(&timec);
        return((double)timec);
      } else if(strcmp(s,"CTIMER")==0) {
	return((double)clock()/CLOCKS_PER_SEC);
      } else if(strcmp(s,"TIMER")==0) {
        struct timespec t;
	struct {
               int  tz_minuteswest; /* minutes W of Greenwich */
               int  tz_dsttime;     /* type of dst correction */
       } tz;
	gettimeofday(&t,&tz);
	return((double)t.tv_sec+(double)t.tv_nsec/1000000);      
      } else if(strcmp(s,"ERR")==0)    return((double)err);
      else if(strcmp(s,"CCSERR")==0)   return((double)ccs_err);
      else if(strcmp(s,"CCSAPLID")==0) return((double)aplid);
      else if(strcmp(s,"PC")==0)       return((double)pc);
      else if(strcmp(s,"SP")==0)       return((double)sp);
      else if(strcmp(s,"PI")==0)       return(PI);
      else if(strcmp(s,"TRUE")==0)     return(-1);
      else if(strcmp(s,"FALSE")==0)    return(0);
      else if(strcmp(s,"MOUSEX")==0)   return((double)mousex());
      else if(strcmp(s,"MOUSEY")==0)   return((double)mousey());
      else if(strcmp(s,"MOUSEK")==0)   return((double)mousek());
      else if(strcmp(s,"MOUSES")==0)   return((double)mouses());
      else if(s[0]=='@')               return(do_funktion(s+1,""));
      else if((vnr=variable_exist(s,FLOATTYP))!=-1) return(variablen[vnr].zahl);
      else if(s[strlen(s)-1]=='%') {
        s[strlen(s)-1]=0;
      
        if((vnr=variable_exist(s,INTTYP))!=-1) return((double)variablen[vnr].opcode);
        else return(0);   
     } else return(atof(s));  /* Jetzt nur noch Zahlen (hex, oct etc ...)*/
    }
  }
  error(51,s); /* Syntax error */
  return(0);
}

ARRAY *array_parser(char *funktion) { /* Array-Parser  */
/* Rekursiv und so, dass dynamische Speicherverwaltung ! */
/* Trenne ersten Token ab, und uebergebe rest derselben Routine */

/* Zum Format:

   a()    ganzes Array
   a(1,2,:,3,:) nur Teil des Arrays. Die angegebenen Indizies werden
                festgehalten. Es resultiert ein 2-dimensionales Array
		*/


  char s[strlen(funktion)+1],w1[strlen(funktion)+1],w2[strlen(funktion)+1];
  char *pos,*pos2,*inhalt;
  ARRAY *ergebnis,*zw1,*zw2;
  int e,vnr;
  char *r;

  strcpy(s,funktion);
  xtrim(s,TRUE,s);  /* Leerzeichen vorne und hinten entfernen */
#ifdef DEBUG
  printf("%d:array_parser: %s \n",depth,funktion);
  depth++;
#endif


 if(wort_sep(s,'+',TRUE,w1,w2)>1) {
   if(strlen(w1)) {
     zw1=array_parser(w1); zw2=array_parser(w2);
     array_add(zw1,zw2);
     free_array(zw2); 
     return(zw1);
   } else return(array_parser(w2)); 
 } else if(wort_sepr(s,'-',TRUE,w1,w2)>1) { 
   if(strlen(w1)) {
     zw1=array_parser(w1); zw2=array_parser(w2);
     array_sub(zw1,zw2);
     free_array(zw2); 
     return(zw1);
   } else {
     zw2=array_parser(w2);
     array_smul(zw2,-1);
     return(zw2);
   } 
 } else if(wort_sepr(s,'*',TRUE,w1,w2)>1) {           
    if(strlen(w1)) {
      if(type2(w1) & ARRAYTYP) {
        zw1=array_parser(w1); zw2=array_parser(w2);
        ergebnis=mul_array(zw1,zw2);
        free_array(zw1); free_array(zw2);
        return(ergebnis);
      } else {    /* Skalarmultiplikation */
        zw2=array_parser(w2);
	array_smul(zw2,parser(w1));
	return(zw2);
      }  
    } else error(51,""); /*Syntax Error*/
  } else if(wort_sepr(s,'^',TRUE,w1,w2)>1) {
    zw1=array_parser(w1);
    e=(int)parser(w2);
    if(e<0) error(51,""); /*Syntax Error*/
    else if(e==0) {
      zw2=zw1;
      zw1=einheitsmatrix(zw2->typ,zw2->dimension,zw2->pointer); 
      free_array(zw2);
    } else if(e>1) {
      int i;
      for(i=1;i<e;i++) {
      
      }
    }
    return(zw1);  
  } else if(s[0]=='(' && s[strlen(s)-1]==')')  { /* Ueberfluessige Klammern entfernen */
    s[strlen(s)-1]=0;
    return(array_parser(s+1));
    
    /* SystemFunktionen Subroutinen und Arrays */  
  } else {
    pos=searchchr(s,'(');
    if(pos!=NULL) {
      pos2=s+strlen(s)-1;
      pos[0]=0;
      pos++;      
      if(pos2[0]!=')') {
         error(51,w2); /* "Parser: Syntax error?! "  */
      } else {                         /* $-Funktionen und $-Felder   */
         pos2[0]=0;        
	 if(strcmp(s,"INV")==0)        return(inv_array(array_parser(pos)));
	 else if(strcmp(s,"TRANS")==0) return(trans_array(array_parser(pos)));
	 else if(strcmp(s,"SMUL")==0) {
	   char w1[strlen(pos)+1],w2[strlen(pos)+1];
	   ARRAY *ergeb;
	   wort_sep(pos,',',TRUE,w1,w2);
	   ergeb=array_parser(w1);
	   array_smul(ergeb,parser(w2));
	   return(ergeb);
         }
	 else if(strcmp(s,"CSVGET")==0) {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1],w3[strlen(pos)+1];
	  char *zzz;
	  int o=0,nn=0;
	  ARRAY *ergeb;
	  
	  wort_sep(pos,',',TRUE,w1,w2);
	  e=wort_sep(w2,',',TRUE,w2,w3);
	  nn=(int)parser(w2);
	  if(e==2) o=(int)parser(w3);
	   zzz=s_parser(w1);
	   ergeb=csvget(zzz,nn,o);
	   free(zzz);
	   return(ergeb);
	 } else if(strcmp(s,"INP%")==0) {
	   char w1[strlen(pos)+1],w2[strlen(pos)+1];
	   int i,nn;
	   ARRAY *ergeb=malloc(sizeof(ARRAY));
           FILE *fff=stdin;
	   wort_sep(pos,',',TRUE,w1,w2);
	   i=get_number(w1);
	   nn=(int)parser(w2);
	  
           ergeb->typ=INTARRAYTYP;
	   ergeb->dimension=1;
	   ergeb->pointer=malloc(INTSIZE+nn*sizeof(int));
	   ((int *)(ergeb->pointer))[0]=nn;
           if(filenr[i]) {
	     int *varptr=ergeb->pointer+INTSIZE;
             fff=dptr[i];
             fread(varptr,sizeof(int),nn,fff);
             return(ergeb);
           } else error(24,""); /* File nicht geoeffnet */
           return(ergeb);
	 } else {
	   /* Hier sollten keine Funktionen mehr auftauchen  */
	   /* Jetzt uebergebe spezifiziertes Array, evtl. reduziert*/
	   if(strlen(pos)==0) {
	     r=varrumpf(s);
	     if((vnr=variable_exist(r,FLOATARRAYTYP))!=-1)  ergebnis=copy_var_array(vnr);
	     else if((vnr=variable_exist(r,INTARRAYTYP))!=-1)    ergebnis=copy_var_array(vnr);
	     else if((vnr=variable_exist(s,STRINGARRAYTYP))!=-1) ergebnis=copy_var_array(vnr);
             else error(15,s);  /* Feld nicht dimensioniert  */
	     free(r);
	   } else {
	     r=varrumpf(s);
	     if((vnr=variable_exist(r,FLOATARRAYTYP))!=-1) {
	       char w1[strlen(pos)+1],w2[strlen(pos)+1];
	       int i,e,rdim=0,ndim=0,anz=1,anz2=1,j,k;
	       int indexe[variablen[vnr].opcode];
	       int indexo[variablen[vnr].opcode];
	       int indexa[variablen[vnr].opcode];
	       ergebnis=malloc(sizeof(ARRAY));
	     /* Dimension des reduzierten Arrays bestimmen */
	       ergebnis->typ=FLOATARRAYTYP;
	       e=wort_sep(pos,',',TRUE,w1,w2);
	       while(e) {
	         if(w1[0]!=':' && w1[0]!=0) {
		   indexo[ndim++]=(int)parser(w1);
		   rdim++;
		 } else indexo[ndim++]=-1;
		 
	         e=wort_sep(w2,',',TRUE,w1,w2);
	       }
	       
             /* Dimensionierung uebertragen */
	       ergebnis->dimension=max(variablen[vnr].opcode-rdim,1);
	       ergebnis->pointer=malloc(INTSIZE*ergebnis->dimension);
	       rdim=0;ndim=0;anz=1;
	       e=wort_sep(pos,',',TRUE,w1,w2);
	       while(e) {
	         indexa[rdim]=anz;		 
	         if(w1[0]==':' || w1[0]==0) {
		 
		   ((int *)(ergebnis->pointer))[ndim++]=((int *)(variablen[vnr].pointer))[rdim];
		   anz=anz*(((int *)variablen[vnr].pointer)[rdim]);
		 } 
		 rdim++;
	         e=wort_sep(w2,',',TRUE,w1,w2);
	       }	       

 	       ergebnis->pointer=realloc(ergebnis->pointer,INTSIZE*ergebnis->dimension+anz*sizeof(double));

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
		 if(jj!=0) printf("Rechnung geht nicht auf.\n");
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
		 
		 ((double *)(ergebnis->pointer+INTSIZE*ergebnis->dimension))[j]=((double *)(variablen[vnr].pointer+INTSIZE*variablen[vnr].opcode))[anz2];
		 
	       }
	     
	     } else if((vnr=variable_exist(r,INTARRAYTYP))!=-1) {
	       printf("Noch nicht moeglich...\n");
	     }  else if((vnr=variable_exist(s,STRINGARRAYTYP))!=-1) {
	      printf("Noch nicht moeglich...\n");
	     } else {
	       error(15,s);  /* Feld nicht dimensioniert  */
	       e=1;
  	       ergebnis=einheitsmatrix(FLOATARRAYTYP,1,&e);
	     }
	     free(r);
	   }
	   return(ergebnis);
        }
      }
    }
  }
  printf("Array nicht aufgeloest: %s\n",s);
  e=1;
  ergebnis=einheitsmatrix(FLOATARRAYTYP,1,&e);
#ifdef DEBUG
  depth--;
#endif
  return(ergebnis);
}

STRING string_parser(char *);

char *s_parser(char *funktion) { /* String-Parser  */
  STRING e=string_parser(funktion);
  return(e.pointer);
}
STRING string_parser(char *funktion) {
/* Rekursiv und so, dass dynamische Speicherverwaltung ! */
/* Trenne ersten Token ab, und uebergebe rest derselben Routine */

 char *pos,*pos2,*inhalt;
 STRING ergebnis;
 STRING t,u;
 int e,vnr;
 char v[strlen(funktion)+1],w[strlen(funktion)+1];

 /*printf("S-Parser: <%s>\n",funktion);*/
 e=wort_sep(funktion,'+',TRUE,v,w);
 if(e==2) {
   t=string_parser(v);
   u=string_parser(w);
   ergebnis.pointer=malloc(t.len+u.len+1);
   memcpy(ergebnis.pointer,t.pointer,t.len);
   memcpy(ergebnis.pointer+t.len,u.pointer,u.len+1);
   ergebnis.len=u.len+t.len;
   free(t.pointer);free(u.pointer); 
 } else {
   /*printf("s-parser: <%s>\n",funktion);*/
   strcpy(v,funktion);
   pos=searchchr(v, '(');
   if(pos!=NULL) {
     pos2=v+strlen(v)-1;
     pos[0]=0;
     pos++;

     if(pos2[0]!=')') {
       error(51,v); /* "Parser: Syntax error?! "  */
       ergebnis.pointer=malloc(8);
       strcpy(ergebnis.pointer,"<ERROR>");
       ergebnis.len=strlen(ergebnis.pointer);
     } else {                         /* $-Funktionen und $-Felder   */
       pos2[0]=0;
       
      if(strcmp(v,"CSGET$")==0) { 
	  STRING test;
          test=string_parser(pos);
	  ergebnis.pointer=csgets(test.pointer);
          free(test.pointer);
	  ergebnis.len=strlen(ergebnis.pointer);
      } else if(strcmp(v,"CSUNIT$")==0) { 
	  STRING test;
          test=string_parser(pos);
	  ergebnis.pointer=csunit(test.pointer);
          free(test.pointer);
	  ergebnis.len=strlen(ergebnis.pointer);
      } else if(strcmp(v,"CSPNAME$")==0) { 
	  ergebnis.pointer=cspname((int)parser(pos));
    	  ergebnis.len=strlen(ergebnis.pointer);  
      } else if(v[0]=='@')     /* Funktion oder Array   */
 	  ergebnis=do_sfunktion(v+1,pos);	  
      else {  /* Liste durchgehen */

	  int i=0,oa,a=anzpsfuncs,b;
          for(b=0; b<strlen(v); b++) {
            while(v[b]>(psfuncs[i].name)[b] && i<a) i++;
            oa=a;a=i;
            while(v[b]<(psfuncs[a].name)[b]+1 && a<oa) a++;
            if(i==a) break;
          }
          if(i<anzpsfuncs && strcmp(v,psfuncs[i].name)==0) {
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
		   printf("Falsche Anzahl Parameter");
		   val1=parser(w1); val2=0;
	         } else if(e==2)  val1=parser(w1); val2=parser(w2);
                 ergebnis=(psfuncs[i].routine)(val1,val2);
	      } else if(psfuncs[i].pmax==1 && (psfuncs[i].opcode&FM_TYP)==F_SQUICK) {
                STRING test=string_parser(pos);
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
	       ergebnis.pointer[0]=0;
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
               if(i==0) printf("Arrayverarbeitung an dieser Stelle nicht moeglich ! \n");

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
	         ergebnis.pointer[0]=0;
		 ergebnis.len=0;
               } else {
          
	         pos=(char *)(variablen[vnr].pointer+ndim*INTSIZE+anz*(sizeof(int)+sizeof(char *)));
	    
                 ergebnis.pointer=malloc(((int *)pos)[0]+1);
		 ergebnis.len=((int *)pos)[0];
	         pos+=sizeof(int);
	         memcpy(ergebnis.pointer,((char **)pos)[0],ergebnis.len);
	       }
	       free(ss);free(t);
	       /****************/
	     
	     }
	  }
       } 
     }
   } else {
     pos2=v+strlen(v)-1;
     if(v[0]=='"' && pos2[0]=='"') {  /* Konstante  */
       ergebnis.pointer=malloc(strlen(v)-2+1);
       ergebnis.len=strlen(v)-2;
       pos2[0]=0;
       memcpy(ergebnis.pointer,v+1,strlen(v)-2+1);
       
     } else if(pos2[0]!='$') {
       error(51,v); /* "Parser: Syntax error?! "  */
       ergebnis.pointer=malloc(8);
       strcpy(ergebnis.pointer,"<ERROR>");
       ergebnis.len=strlen(ergebnis.pointer);
     } else {                      /* einfache Variablen und Systemvariablen */
       time_t timec;
       struct tm * loctim;
       pos2[0]=0;
       if(strcmp(v,"TIME")==0) {
         timec = time(&timec);
         loctim=localtime(&timec);
         ergebnis.pointer=malloc(9);
         strncpy(ergebnis.pointer,ctime(&timec)+11,8);
         ergebnis.pointer[8]=0;
	 ergebnis.len=strlen(ergebnis.pointer);
       } else if(strcmp(v,"DATE")==0) {
	 timec = time(&timec);
	 loctim=localtime(&timec);
	 ergebnis.pointer=malloc(12);
	 sprintf(ergebnis.pointer,"%2d.%2d.%4d",loctim->tm_mday,loctim->tm_mon+1,1900+loctim->tm_year);
         pos=ergebnis.pointer; while(pos[0]!=0) {if(pos[0]==' ') pos[0]='0'; pos++;}
         ergebnis.len=strlen(ergebnis.pointer);
       } else if(strcmp(v,"TRACE")==0) { 
         ergebnis.pointer=malloc(strlen(program[pc])+1);
	 strcpy(ergebnis.pointer,program[pc]);
	 xtrim(ergebnis.pointer,TRUE,ergebnis.pointer);
	 ergebnis.len=strlen(ergebnis.pointer);
       } else if(strcmp(v,"TERMINALNAME")==0) { 
         ergebnis.pointer=terminalname();
	 ergebnis.len=strlen(ergebnis.pointer);
       } else if(strcmp(v,"INKEY")==0) { 
         char *t=inkey();
         ergebnis.pointer=malloc(strlen(t)+1);
	 strcpy(ergebnis.pointer,t);
	 ergebnis.len=strlen(ergebnis.pointer);
       } else if((vnr=variable_exist(v,STRINGTYP))==-1) {
         ergebnis.pointer=malloc(1);
	 ergebnis.pointer[0]=0;
	 ergebnis.len=0;
       } else {
         ergebnis.pointer=malloc(variablen[vnr].opcode+1);
	 ergebnis.len=variablen[vnr].opcode;
	 memcpy(ergebnis.pointer,variablen[vnr].pointer,ergebnis.len);
       }
     }
   }
 }
 ergebnis.pointer[ergebnis.len]=0;
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
            printf("Fehler innerhalb FUNCTION. \n");
	  }
	}
	free(buffer);
	return(returnvalue.f);
      }
     
    free(buffer);
    return(0.0);  
 }
 
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
 char *buffer,*pos,*pos2,*pos3;
 STRING ergebnis;
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
            printf("Fehler innerhalb FUNCTION. \n");
	  }
	}
	free(buffer);
	return(returnvalue.str);
      }
    error(44,buffer); /* Funktion  nicht definiert */
    free(buffer);
    ergebnis.pointer=malloc(1);
    ergebnis.pointer[0]=0;
    ergebnis.len=0;
    return(ergebnis);  
}
