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

#include "defs.h"
#include "xbasic.h"
#include "protos.h"

double f_nop(void *t) {return(0.0);}
int f_point(double v1, double v2) {  return(get_point((int)v1,(int)v2)); }
int f_bclr(double v1, double v2) { return((int)v1 & ~ (1 <<((int)v2))); }
int f_bset(double v1, double v2) { return((int)v1 | (1 <<((int)v2))); }
int f_bchg(double v1, double v2) { return((int)v1 ^ (1 <<((int)v2))); }
int f_btst(double v1, double v2) { return((((int)v1 & (1 <<((int)v2)))==0) ?  0 : -1); }

int f_len(char *);
int f_asc(char *);
int f_int(double b) {return((int)b);}
double f_pred(double b) {return(ceil(b-1));}
int f_succ(double b) {return((int)(b+1));}
int f_sgn(double b) {return(sgn(b));}
double f_frac(double b) {return(b-((double)((int)b)));}
int f_fak(double);
int f_random(double d) {return((int)((double)rand()/RAND_MAX*d));}
double f_rnd(double d) {return((double)rand()/RAND_MAX);}
int  f_srand(double d) {srand((int)d);return(0);}
double f_deg(double d) {return(d/PI*180);}
double f_rad(double d) {return(d*PI/180);}
int  f_dimf(char *pos) {return((double)do_dimension(variable_exist_type(pos)));}
int f_cssize(char *),f_cspid(char *);
double f_csres(char *),f_csmin(char *),f_csmax(char *),f_csget(char *);
double f_ltextlen(char *),f_val(char *);

FUNCTION pfuncs[]= {  /* alphabetisch !!! */

 { F_ARGUMENT|F_DRET,  "!nulldummy", f_nop ,0,0   ,{0}},
 { F_DQUICK|F_DRET,    "ABS"       , fabs ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "ACOS"      , acos ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "ACOSH"      , acosh ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "ASC"       , f_asc ,1,1   ,{PL_STRING}},
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
 { F_ARGUMENT|F_DRET,  "CSGET"     , f_csget ,1,1   ,{PL_STRING}},
 { F_ARGUMENT|F_DRET,  "CSMAX"     , f_csmax ,1,1   ,{PL_STRING}},
 { F_ARGUMENT|F_DRET,  "CSMIN"     , f_csmin ,1,1   ,{PL_STRING}},
 { F_ARGUMENT|F_IRET,  "CSPID"     , f_cspid ,1,1   ,{PL_STRING}},
 { F_ARGUMENT|F_DRET,  "CSRES"     , f_csres ,1,1   ,{PL_STRING}},
 { F_ARGUMENT|F_IRET,  "CSSIZE"    , f_cssize ,1,1   ,{PL_STRING}},

 { F_DQUICK|F_DRET,    "DEG"       , f_deg ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "DIM?"      , f_dimf ,1,1      ,{PL_ARRAY}},

 { F_DQUICK|F_DRET,    "EXP"       , exp ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "EXPM1"     , expm1 ,1,1     ,{PL_NUMBER}},

 { F_DQUICK|F_DRET,    "HYPOT"     , hypot ,2,2     ,{PL_NUMBER,PL_NUMBER}},

 { F_DQUICK|F_IRET,    "FAK"       , f_fak ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "FLOOR"     , floor ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "FRAC"       , f_frac ,1,1     ,{PL_NUMBER}},

 { F_ARGUMENT|F_IRET,  "INP"       , inp8 ,1,1      ,{PL_NUMBER|PL_FILENR}},
 { F_ARGUMENT|F_IRET,  "INP?"      , inpf ,1,1      ,{PL_NUMBER|PL_FILENR}},
 { F_ARGUMENT|F_IRET,  "INP&"      , inp16 ,1,1      ,{PL_NUMBER|PL_FILENR}},
 { F_ARGUMENT|F_IRET,  "INP%"      , inp32 ,1,1      ,{PL_NUMBER|PL_FILENR}},
 { F_DQUICK|F_IRET,    "INT"       , f_int ,1,1     ,{PL_NUMBER}},
 
 { F_ARGUMENT|F_IRET,  "LEN"       , f_len ,1,1   ,{PL_STRING}},
 { F_DQUICK|F_DRET,    "LN"        , log ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "LOG"       , log ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "LOG10"     , log10 ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "LOG1P"     , log1p ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "LOGB"      , logb  ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_DRET,  "LTEXTLEN"  , f_ltextlen ,1,1   ,{PL_STRING}},

 { F_DQUICK|F_DRET,    "MOD"       , fmod ,2,2     ,{PL_NUMBER,PL_NUMBER }},

 { F_DQUICK|F_IRET,    "POINT"     , f_point ,2,2     ,{PL_NUMBER, PL_NUMBER }},
 { F_DQUICK|F_DRET,    "PRED"      , f_pred ,1,1     ,{PL_NUMBER}},

 { F_DQUICK|F_DRET,    "RAD"      , f_rad ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "RAND"      , rand ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "RANDOM"    , f_random ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "RND"       , f_rnd ,1,1     ,{PL_NUMBER}},

 { F_DQUICK|F_IRET,    "SGN"       , f_sgn ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SIN"       , sin ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SINH"      , sinh ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SQR"       , sqrt ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "SQRT"      , sqrt ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "SRAND"     , f_srand ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_IRET,    "SUCC"      , f_succ ,1,1     ,{PL_NUMBER}},

 { F_DQUICK|F_DRET,    "TAN"       , tan ,1,1     ,{PL_NUMBER}},
 { F_DQUICK|F_DRET,    "TANH"       , tanh ,1,1     ,{PL_NUMBER}},
 { F_ARGUMENT|F_IRET,  "TYP?"       , type2 ,1,1     ,{PL_ALL}},

 { F_ARGUMENT|F_DRET,  "VAL"       , f_val ,1,1     ,{PL_ALL}},

 { F_ARGUMENT|F_IRET,  "WORT_SEP"    , do_wort_sep ,3,-1     ,{PL_STRING,0}},
 
};
int anzpfuncs=sizeof(pfuncs)/sizeof(FUNCTION);

int f_asc(char *n) {
  char *test=s_parser(n);
  int ergebnis=(int)test[0];
  free(test);  
  return(ergebnis);
}

int f_len(char *n) {
  char *test=s_parser(n);
  int ergebnis=strlen(test);
  free(test);  
  return(ergebnis);
}
double f_ltextlen(char *n) {
  char *test=s_parser(n);
  double ergebnis=ltextlen(ltextxfaktor,ltextpflg,test);
  free(test);  
  return(ergebnis);
}
double f_val(char *n) {
  char *test=s_parser(n);
  double ergebnis=atof(test);
  free(test);  
  return(ergebnis);
}
double f_csmax(char *n) {
  char *test=s_parser(n);
  double ergebnis=csmax(test);
  free(test);  
  return(ergebnis);
}
double f_csmin(char *n) {
  char *test=s_parser(n);
  double ergebnis=csmin(test);
  free(test);  
  return(ergebnis);
}
double f_csres(char *n) {
  char *test=s_parser(n);
  double ergebnis=csres(test);
  free(test);  
  return(ergebnis);
}
double f_csget(char *n) {
  char *test=s_parser(n);
  double ergebnis=csget(test);
  free(test);  
  return(ergebnis);
}
int f_cssize(char *n) {
  char *test=s_parser(n);
  int ergebnis=cssize(test);
  free(test);  
  return(ergebnis);
}
int f_cspid(char *n) {
  char *test=s_parser(n);
  int ergebnis=cspid(test);
  free(test);  
  return(ergebnis);
}

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
    char *a,*b;
    a=s_parser(w1);
    b=s_parser(w2);
    v=strcmp(a,b);
    free(a);free(b);
  } else {
    double x=(parser(w1)-parser(w2));
    if(x==0) return(0);
    else if(x<0) return(-1);
    else return(1);
  }
  return(v);
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

      if(strcmp(s,"EXIST")==0)  {
	  char *test=s_parser(pos);
	  double ergebnis=-(double)exist(test);
	  free(test);   
	  return(ergebnis);
        }else if(strcmp(s,"LOF")==0)  {
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
	}else if(strcmp(s,"ARRPTR")==0)  return((double)arrptr(pos));
	else if(strcmp(s,"VARPTR")==0)  return((double)(int)varptr(pos));
	else if(strcmp(s,"MALLOC")==0)  return((double)(int)malloc((int)parser(pos)));
	else if(strcmp(s,"FREE")==0)  {free((char *)((int)parser(pos)));return((double)errno);}
	else if(strcmp(s,"PEEK")==0)  return((double)(int)((char *)((int)parser(pos)))[0]);
	else if(strcmp(s,"LPEEK")==0)  return((double)(int)((int *)((int)parser(pos)))[0]);
	else if(strcmp(s,"DPEEK")==0)  return((double)(int)((short *)((int)parser(pos)))[0]);  


        else 
	if(s[0]=='@') return(do_funktion(s+1,pos));
	else {
	  /* Liste durchgehen */
	  int i=0,oa,a=anzcomms,b;
          for(b=0; b<strlen(s); b++) {
            while(s[b]>(pfuncs[i].name)[b] && i<a) i++;
            oa=a;a=i;
            while(s[b]<(pfuncs[a].name)[b]+1 && a<oa) a++;
            if(i==a) break;
          }
          if(i<anzpfuncs && strcmp(s,pfuncs[i].name)==0) {
	      
	      
	      if((pfuncs[i].opcode&FM_TYP)==F_SIMPLE || pfuncs[i].pmax==0) {
	        if(pfuncs[i].opcode&F_IRET) return((double)((int (*)())pfuncs[i].routine)(NULL));
		else return((pfuncs[i].routine)(NULL));
	      } 
	      
	      if((pfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
	      	if(pfuncs[i].opcode&F_IRET) return((double)((int (*)())pfuncs[i].routine)(pos));
		else return((pfuncs[i].routine)(pos));
	      } else if((pfuncs[i].opcode&FM_TYP)==F_PLISTE) ; /* Noch nicht moeglich */
	      
	      if(pfuncs[i].pmax==1 && (pfuncs[i].opcode&FM_TYP)==F_DQUICK) {
	        double ue=parser(pos);
		
	      	if(pfuncs[i].opcode&F_IRET) return((double)((int (*)())pfuncs[i].routine)(ue));
		else return((pfuncs[i].routine)(ue));
	
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
	       
	      }
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
  printf("Parser: der angegebene Ausdruck ist Fehlerhaft: <%s>\n",s);
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
    } else printf("Syntax Error.\n");
  } else if(wort_sepr(s,'^',TRUE,w1,w2)>1) {
    zw1=array_parser(w1);
    e=(int)parser(w2);
    if(e<0) printf("Syntax-Error.\n");
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


char *s_parser(char *funktion) { /* String-Parser  */

/* Rekursiv und so, dass dynamische Speicherverwaltung ! */
/* Trenne ersten Token ab, und uebergebe rest derselben Routine */

 char *t,*u,*ergebnis,*pos,*pos2,*inhalt;
 int e,vnr;
 char v[strlen(funktion)+1],w[strlen(funktion)+1];

 /*printf("S-Parser: <%s>\n",funktion);*/
 e=wort_sep(funktion,'+',TRUE,v,w);
 if(e==2) {
   t=s_parser(v);
   u=s_parser(w);
   ergebnis=malloc(strlen(t)+strlen(u)+1);
   strcpy(ergebnis,t);strcat(ergebnis,u);
   free(t);free(u); 
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
       ergebnis=malloc(8);
       strcpy(ergebnis,"<ERROR>");
     } else {                         /* $-Funktionen und $-Felder   */
       pos2[0]=0;
       
       if(strcmp(v,"CHR$")==0) { 
          ergebnis=malloc(2);
          ergebnis[0]=(char)parser(pos);
	  ergebnis[1]=0;
       } else if(strcmp(v,"STR$")==0) ergebnis=float_to_string(pos);   /* STR$(a[,b[,c[,d]]])     */
       else if(strcmp(v,"BIN$")==0)   ergebnis=bin_to_string(pos);
       else if(strcmp(v,"HEX$")==0)   ergebnis=hexoct_to_string('x',pos);
       else if(strcmp(v,"OCT$")==0)   ergebnis=hexoct_to_string('o',pos);
       else if(strcmp(v,"SPACE$")==0) { 
          unsigned int i=0,j; 
	  j=(int)parser(pos);
	  ergebnis=malloc(j+1);
          while(i<j) ergebnis[i++]=' ';
	  ergebnis[i]=0;
       } else if(strcmp(v,"STRING$")==0) {
          char *buffer,*w1,*w2;
	  int i=0,j;
	  w1=malloc(strlen(pos)+1);w2=malloc(strlen(pos)+1);
          wort_sep(pos,',',TRUE,w1,w2); 
          j=(int)parser(w1);
	  free(w1);
	  buffer=s_parser(w2);
	  free(w2);
	  ergebnis=malloc(j*strlen(buffer)+1);
	  ergebnis[0]=0;
          while(i<j) {strcat(ergebnis,buffer); i++;}
	  free(buffer);
       } else if(strcmp(v,"LEFT$")==0) {
          char *w1,*w2;
	  int j,e;
	  w1=malloc(strlen(pos)+1);w2=malloc(strlen(pos)+1);
          e=wort_sep(pos,',',TRUE,w1,w2); 
	  ergebnis=s_parser(w1);
	  free(w1);
	  if(e<2) j=1;
	  else j=min(max((int)parser(w2),0),strlen(ergebnis));
	  free(w2);
	  ergebnis[j]=0;
       } else if(strcmp(v,"RIGHT$")==0) {
	  char *buffer,*w1,*w2;
	  int j,e;
	  w1=malloc(strlen(pos)+1);w2=malloc(strlen(pos)+1);
          e=wort_sep(pos,',',TRUE,w1,w2); 
	  buffer=s_parser(w1);
	  free(w1);
	  if(e<2) j=1;
	  else j=min(max((int)parser(w2),0),strlen(buffer)); 
	  free(w2);
	  j=min(j,strlen(buffer));
	  ergebnis=malloc(j+1);
	  strcpy(ergebnis,buffer+strlen(buffer)-j);
	  free(buffer);
       } else if(strcmp(v,"MID$")==0) {
	  char *buffer,*w1,*w2,*w3;
	  int i,j,e;
	  w1=malloc(strlen(pos)+1);w2=malloc(strlen(pos)+1);w3=malloc(strlen(pos)+1);
          e=wort_sep(pos,',',TRUE,w1,w2); 
	  buffer=s_parser(w1);
	  if(e<2) i=j=1;
	  else {
	    e=wort_sep(w2,',',TRUE,w1,w3); 
	    j=(int)parser(w1);
	    if(e<2) i=1;
	    else i=(int)parser(w3);
	  }
	  free(w1);free(w2);free(w3);
	  if(i==0) {
	    ergebnis=malloc(1);ergebnis[0]=0;
	    free(buffer);
	  } else {
	    j=max(j,1);
	    j=min(j,strlen(buffer));
	    i=min(i,strlen(buffer));
	    ergebnis=malloc(strlen(buffer)+1);
	    strcpy(ergebnis,&buffer[j-1]);
	    ergebnis[i]=0;
	    free(buffer);
	  }
       } else if(strcmp(v,"UPPER$")==0) { 
          int i=0;
          ergebnis=s_parser(pos); 
	  while(ergebnis[i]!=0) {ergebnis[i]=toupper(ergebnis[i]); i++;}
       } else if(strcmp(v,"TRIM$")==0) { 
          ergebnis=s_parser(pos);
          xtrim(ergebnis,0,ergebnis); 
       } else if(strcmp(v,"ERR$")==0) {
          char *test;
	  test=error_text((char)parser(pos),"");
          ergebnis=malloc(strlen(test)+1);
          strcpy(ergebnis,test);
       } else if(strcmp(v,"LINEINPUT$")==0) ergebnis=lineinputs(pos);
        else if(strcmp(v,"INPUT$")==0)      ergebnis=inputs(pos);   
        else if(strcmp(v,"PRG$")==0) {
          char *test;
	  int ind;
	  ind=(int)parser(pos);
	  if(ind<prglen && ind>=0) {
	    test=program[(int)parser(pos)];
            ergebnis=malloc(strlen(test)+1);
            strcpy(ergebnis,test);
	  } else {
	    error(16,pos); /* Feldindex zu gross*/ 
	    ergebnis=malloc(8);
            strcpy(ergebnis,"<ERROR>");
	  }
	} else if(strcmp(v,"ENV$")==0) {
          char *test,*ttt;
	  test=s_parser(pos);
	  ttt=getenv(test);
	  if(ttt==NULL) {
	    ergebnis=malloc(1);
	    ergebnis[0]=0;
	  } else {
            ergebnis=malloc(strlen(ttt)+1);
            strcpy(ergebnis,ttt);
	  }
	  free(test);
	} else if(strcmp(v,"PARAM$")==0) {
          char *ttt;
	  int i=(int)parser(pos);
	  if(i<param_anzahl) ttt=param_argumente[i];
          else ttt=NULL;
	  if(ttt==NULL) {
	    ergebnis=malloc(1);
	    ergebnis[0]=0;
	  } else {
            ergebnis=malloc(strlen(ttt)+1);
            strcpy(ergebnis,ttt);
	  }
       } else if(strcmp(v,"LOWER$")==0) { 
          int i=0;
          ergebnis=s_parser(pos); 
	  while(ergebnis[i]!=0) {ergebnis[i]=tolower(ergebnis[i]); i++;}
	  
       } else if(strcmp(v,"CSGET$")==0) { 
	  char * test;
          test=s_parser(pos);
	  ergebnis=csgets(test);
          free(test);
      } else if(strcmp(v,"CSUNIT$")==0) { 
	  char * test;
          test=s_parser(pos);
	  ergebnis=csunit(test);
          free(test);
      } else if(strcmp(v,"CSPNAME$")==0) { 
	 
	  ergebnis=cspname((int)parser(pos));
    	  	  
       } else {    /* Funktion oder ob Array   */
          int vnr;
           
	   if(v[0]=='@') {
	     ergebnis=do_sfunktion(v+1,pos);
	   } else {
	     v[strlen(v)-1]=0;
	     
             if((vnr=variable_exist(v,STRINGARRAYTYP))==-1) {
	       error(15,v);         /*Feld nicht definiert*/
	       ergebnis=malloc(1);
	       ergebnis[0]=0;
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
		 ergebnis=malloc(1);
	         ergebnis[0]=0;
               } else {
          
	         pos=(char *)(variablen[vnr].pointer+ndim*INTSIZE+anz*(sizeof(int)+sizeof(char *)));
	    
                 ergebnis=malloc(((int *)pos)[0]+1);
	         pos+=sizeof(int);
	         strcpy(ergebnis,((char **)pos)[0]);
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
       ergebnis=malloc(strlen(v)-2+1);
       pos2[0]=0;
       strcpy(ergebnis,v+1);
     } else if(pos2[0]!='$') {
       error(51,v); /* "Parser: Syntax error?! "  */
       ergebnis=malloc(8);
       strcpy(ergebnis,"<ERROR>");
     } else {                      /* einfache Variablen und Systemvariablen */
       time_t timec;
       struct tm * loctim;
       pos2[0]=0;
       if(strcmp(v,"TIME")==0) {
         timec = time(&timec);
         loctim=localtime(&timec);
         ergebnis=malloc(9);
         strncpy(ergebnis,ctime(&timec)+11,8);
         ergebnis[8]=0;
       } else if(strcmp(v,"DATE")==0) {
	 timec = time(&timec);
	 loctim=localtime(&timec);
	 asprintf(&ergebnis,"%2d.%2d.%4d",loctim->tm_mday,loctim->tm_mon+1,1900+loctim->tm_year);
         pos=ergebnis; while(pos[0]!=0) {if(pos[0]==' ') pos[0]='0'; pos++;}
       } else if(strcmp(v,"TRACE")==0) { 
         ergebnis=malloc(strlen(program[pc])+1);
	 strcpy(ergebnis,program[pc]);
	 xtrim(ergebnis,TRUE,ergebnis);
       } else if(strcmp(v,"TERMINALNAME")==0) { 
         ergebnis=terminalname();
       } else if(strcmp(v,"INKEY")==0) { 
         char *t=inkey();
         ergebnis=malloc(strlen(t)+1);
	 strcpy(ergebnis,t);
       } else if((vnr=variable_exist(v,STRINGTYP))==-1) {
         ergebnis=malloc(1);
	 ergebnis[0]=0;
       } else {
         ergebnis=malloc(variablen[vnr].opcode+1);
	 strcpy(ergebnis,variablen[vnr].pointer);
       }
     }
   }
 }
 return(ergebnis);
}


char *float_to_string(char *pos) {

          /* STR$(a[,b[,c[,d]]])     */
          char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  char *formatter;
	  int j,e,b=-1,c=13,mode=0,i=0;
	  double a;
	  char *ergebnis;
	  
          e=wort_sep(pos,',',TRUE,w1,w2); 
	  while(e) {
	    if(strlen(w1)) {
              switch(i) {
	      case 0: {a=parser(w1);break;}
	      case 1: {b=min(50,max(0,(int)parser(w1)));break;}
	      case 2: {c=min(50,max(0,(int)parser(w1)));break;}
	      case 3: {mode=(int)parser(w1);break;}

              default: break;
              }
            }
	    e=wort_sep(w2,',',TRUE,w1,w2);
            i++;
          }
	  
	  
	  if(mode==0 && b!=-1) asprintf(&formatter,"%%%d.%dg",b,c);
	  else if (mode==1 && b!=-1) asprintf(&formatter,"%%0%d.%dg",b,c);
	  else  asprintf(&formatter,"%%.13g");
          asprintf(&ergebnis,formatter,a);
	  free(formatter);
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
          asprintf(&ergebnis,formatter,a);
	  return(ergebnis);
}
char *bin_to_string(char *pos) {
          char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int j,e,b=8,i=0;
	  unsigned int a;
	  char *ergebnis=malloc(64);
          e=wort_sep(pos,',',TRUE,w1,w2); 
	  while(e) {
	    if(strlen(w1)) {
              switch(i) {
	      case 0: {a=(unsigned int)parser(w1);break;}
	      case 1: {b=min(32,max(0,(int)parser(w1)));break;}
              default: break;
              }
            }
	    e=wort_sep(w2,',',TRUE,w1,w2);
            i++;
          }
	  i=0;
	  for(j=b;j>0;j--) {
	    ergebnis[i++]=((a&(1<<(j-1)))  ? '1':'0');
	  }
	  ergebnis[i]=0;
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
 
 
char *do_sfunktion(char *name,char *argumente) {
 char *buffer,*pos,*pos2,*pos3,*ergebnis;
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
    ergebnis=malloc(2);
    ergebnis[0]=0;
    return(ergebnis);  
 }
