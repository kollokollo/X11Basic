/* parser.c   Formelparser numerisch und Zeichenketten (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif

#include "defs.h"
#include "x11basic.h"
#include "xbasic.h"
#include "type.h"
#include "variablen.h"
#include "parser.h"
#include "parameter.h"
#include "array.h"
#include "number.h"
#include "io.h"
#include "wort_sep.h"
#include "functions.h"


static STRING do_sfunktion(char *name,char *argumente);
static COMPLEX do_cfunktion(char *name,char *argumente);
static double do_funktion(char *name,char *argumente);
static void do_benfunction(const char *name,const char *argumente);


static int vergleich(const char *w1,const char *w2) {
  int e1=type(w1);
  int e2=type(w2);

  if((e1&ARRAYTYP) && (e2&ARRAYTYP)) {
  /* Die Determinante waere ein gutes Mass für die Arrays, 
     falls diese quadratisch und zweidimensional sind.
     Bei eindimensionalen Array die quadratische Summe. 
     Ansonsten kann nur auf übereinstimmung getestet werden.
     TODO: 
     */

    xberror(9,"Compare ARRAY"); /*Function or command %s not implemented*/
    return(0);
  }
  e1&=TYPMASK;
  e2&=TYPMASK;
  if(e1==e2) {
    switch(e1) {
    case INTTYP:
    case FLOATTYP: {
      double x=(parser(w1)-parser(w2));
      if(x==0) return(0);
      if(x<0) return(-1);
      return(1);
    }
    case COMPLEXTYP: {
      COMPLEX a=complex_parser(w1);
      COMPLEX b=complex_parser(w2);
      if(a.r==b.r && a.i==b.i) return(0);
      if(a.r*a.r+a.i*a.i>b.r*b.r+b.i*b.i) return(1);
      return(-1);
    }
    case ARBINTTYP: {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      int i=mpz_cmp(a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      return(i);
    }
    case STRINGTYP: {
      STRING a=string_parser(w1);
      STRING b=string_parser(w2);
     //  printf("String vergleich: <%s> <%s>\n",a.pointer,b.pointer);
      int ret=memcmp(a.pointer,b.pointer,min(a.len,b.len)+1);
      free(a.pointer);free(b.pointer);
      return(ret);
    } 
    default:
      xberror(13,w1);  /* Type mismatch */
      return(-1);
    }
  }
  if(e1==STRINGTYP || e2==STRINGTYP) {
    xberror(13,w1);  /* Type mismatch */
    return(-1);
  }
  if(e1==ARBINTTYP || e2==ARBINTTYP) {
    ARBINT a;
    int b,i;
    mpz_init(a);
    if(e1==ARBINTTYP) {
      arbint_parser(w1,a);
      b=(int)parser(w2);
      i=mpz_cmp_si(a,b);
    } else {
      b=(int)parser(w1);
      arbint_parser(w2,a);
      i=-mpz_cmp_si(a,b);
    }
    mpz_clear(a); 
    return(i);
  }
  
  
/* Jetzt kann nur noch sein:
    e1      e2
    INT     FLOAT
    FLOAT   INT
    */
    double x=(parser(w1)-parser(w2));
    if(x==0) return(0);
    if(x<0) return(-1);
    return(1);
}


/* Hilfsfunktionen f"ur alle Parser */

static int logicoperator_check_int(char *s, int *ret) {
  char *w1,*w2;
  if(searchchr2_multi(s,"&|")!=NULL) {
    if(wort_sepr2_destroy(s,"||",TRUE,&w1,&w2)>1) {*ret=((int)parser(w1) | (int)parser(w2));return(1);}
    if(wort_sepr2_destroy(s,"&&",TRUE,&w1,&w2)>1) {*ret=((int)parser(w1) & (int)parser(w2));return(1);}
  }
  return(0);
}
static int compareoperator_check_int(char *s, int *ret) {
  char *w1,*w2;
  /* Erst Vergleichsoperatoren mit Wahrheitwert abfangen (lowlevel < Addition)  */
  if(searchchr2_multi(s,"<=>")!=NULL) {
    if(wort_sep2_destroy(s,"==",TRUE,&w1,&w2)>1) {*ret=(vergleich(w1,w2)?0:-1);return(1);}
    if(wort_sep2_destroy(s,"<>",TRUE,&w1,&w2)>1) {*ret=(vergleich(w1,w2)?-1:0);return(1);}
    if(wort_sep2_destroy(s,"><",TRUE,&w1,&w2)>1) {*ret=(vergleich(w1,w2)?-1:0);return(1);}
    if(wort_sep2_destroy(s,"<=",TRUE,&w1,&w2)>1) {*ret=((vergleich(w1,w2)<=0)?-1:0);return(1);}
    if(wort_sep2_destroy(s,">=",TRUE,&w1,&w2)>1) {*ret=((vergleich(w1,w2)>=0)?-1:0);return(1);}
    if(wort_sep_destroy(s,'=',TRUE,&w1,&w2)>1)   {*ret=(vergleich(w1,w2)?0:-1);return(1);}
    if(wort_sep_destroy(s,'<',TRUE,&w1,&w2)>1)   {*ret=((vergleich(w1,w2)<0)?-1:0);return(1);}
    if(wort_sep_destroy(s,'>',TRUE,&w1,&w2)>1)   {*ret=((vergleich(w1,w2)>0)?-1:0);return(1);}
  }

  return(0);
}



/*Standard-Parser fuer FLOAT und INT. Soll möglichst schnell sein.
*/

double parser(const char *funktion){  /* Rekursiver num. Parser */
  char s[strlen(funktion)+1];
  char *w1,*w2;
  int i;
  // printf("Parser: <%s>\n",funktion);
  xtrim(funktion,TRUE,s);  /* Leerzeichen vorne und hinten entfernen, Grossbuchstaben */

  if(*s==0) { 
    xberror(51,funktion);  /* Das ist nicht gut bei RND() s.u. (jetzt OK)*/
    return(0); 
  }


  /* Logische Operatoren AND OR NOT ... */

  if(logicoperator_check_int(s, &i)) return((double)i);
  

  if(searchchr2(s,' ')!=NULL) {
    if(wort_sepr2_destroy(s," OR ",TRUE,&w1,&w2)>1)   return((double)((int)parser(w1) | (int)parser(w2)));
    if(wort_sepr2_destroy(s," AND ",TRUE,&w1,&w2)>1)  return((double)((int)parser(w1) & (int)parser(w2)));    /* von rechts !!  */
    if(wort_sepr2_destroy(s," NAND ",TRUE,&w1,&w2)>1) return((double)~((int)parser(w1) & (int)parser(w2)));
    if(wort_sepr2_destroy(s," NOR ",TRUE,&w1,&w2)>1)  return((double)~((int)parser(w1) | (int)parser(w2)));
    if(wort_sepr2_destroy(s," XOR ",TRUE,&w1,&w2)>1)  return((double)((int)parser(w1) ^ (int)parser(w2)));	
    if(wort_sepr2_destroy(s," EOR ",TRUE,&w1,&w2)>1)  return((double)((int)parser(w1) ^ (int)parser(w2)));	
    if(wort_sepr2_destroy(s," EQV ",TRUE,&w1,&w2)>1)  return((double)~((int)parser(w1) ^ (int)parser(w2)));
    if(wort_sepr2_destroy(s," IMP ",TRUE,&w1,&w2)>1)  {
      int i=(int)parser(w2);
      return((double)(~((int)parser(w1) ^ i) | i));
    }
    if(wort_sepr2_destroy(s," MOD ",TRUE,&w1,&w2)>1)  return(fmod(parser(w1),parser(w2)));
    if(wort_sepr2_destroy(s," DIV ",TRUE,&w1,&w2)>1) {
      int nenner=(int)parser(w2);
      if(nenner) return((double)((int)parser(w1) / nenner));
      else {
        xberror(0,w2); /* Division durch 0 */
        return(0);
      }
    }
    if(wort_sepr2_destroy(s,"NOT ",TRUE,&w1,&w2)>1) {
      if(*w1==0) return((double)(~(int)parser(w2)));    /* von rechts !!  */
      /* Ansonsten ist NOT Teil eines Variablennamens */
    }
  }


  /* Erst Vergleichsoperatoren mit Wahrheitwert abfangen (lowlevel < Addition)  */

  if(compareoperator_check_int(s, &i)) return((double)i);


 /* Addition/Subtraktion/Vorzeichen  */
 
 
  /* Suche eine g"ultige Trennstelle f"ur + oder -  */
  /* Hier muessen wir testen, ob es nicht ein vorzeichen war oder Teil eines Exponenten ...*/

  char *pos=searchchr2_multi_r(s,"+-",s+strlen(s));  /* Finde n"achsten Kandidaten  von rechts*/
  while(pos!=NULL) {
    if(pos==s) {  /*Das +/-  war ganz am Anfang*/
      if(*s=='-') return(-parser(s+1));
      else return(parser(s+1));
    }
    if(pos>s && strchr("*/^",*(pos-1))==NULL && 
                                     !( *(pos-1)=='E' && pos-1>s && vfdigittest(s,pos-1) && v_digit(*(pos+1)))) {
    /* Kandidat war gut.*/
      char c=*pos;
      *pos=0;
      if(c=='-') return(parser(s)-parser(pos+1));
      else return(parser(s)+parser(pos+1));
    }
    pos=searchchr2_multi_r(s,"+-",pos);  /* Finde n"achsten Kandidaten  von rechts*/
  }
  
   char *pos2;
   char w1buf[strlen(funktion)+1],w2buf[strlen(funktion)+1];
   int vnr;
   w1=w1buf;
   w2=w2buf;
 
if(searchchr2_multi(s,"*/^")!=NULL) {
  if(wort_sepr(s,'*',TRUE,w1,w2)>1) {
    if(*w1) return(parser(w1)*parser(w2));
    else {
      printf("Pointer noch nicht integriert! %s\n",w2);   /* war pointer - */
      return(0);
    }
  }
  if(wort_sepr(s,'/',TRUE,w1,w2)>1) {
    if(*w1) {
      double nenner;
      nenner=parser(w2);
      if(nenner!=0.0) return(parser(w1)/nenner);    /* von rechts !!  */
      else { xberror(0,w2); return(0);  } /* Division durch 0 */
    } else { xberror(51,w2); return(0); }/* "Parser: Syntax error?! "  */
  }
  if(wort_sepr(s,'^',TRUE,w1,w2)>1) {
    if(*w1) return(pow(parser(w1),parser(w2)));    /* von rechts !!  */
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
	   //   printf("Funktion %s gefunden. Nr. %d\n",pfuncs[i].name,i);
	   
	      if((pfuncs[i].opcode&FM_TYP)==F_SIMPLE || pfuncs[i].pmax==0) {
		switch(pfuncs[i].opcode&FM_RET) {
	        case F_IRET: return((double)((int (*)())pfuncs[i].routine)());
	        case F_CRET: return(COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)()));
		case F_DRET: return((pfuncs[i].routine)());
		case F_AIRET: {
		  ARBINT b;
		  mpz_init(b);
		  ((void (*)())pfuncs[i].routine)(b);
		  a=mpz_get_d(b);
		  mpz_clear(b);
		  }
		  return(a);
		case F_ANYRET: 
		case F_NRET:
		case F_ANYIRET: {
		     PARAMETER b=((ppfunc)(pfuncs[i].routine))();
		     a=p2float(&b);
		     free_parameter(&b);
		     return(a);
	         }
	        default: xberror(13,s);  /* Type mismatch */
                }
	      } else if((pfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
		switch(pfuncs[i].opcode&FM_RET) {
	        case F_IRET: return((double)((int (*)())pfuncs[i].routine)(pos));
	        case F_CRET: return(COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(pos)));
		case F_DRET: return((pfuncs[i].routine)(pos));
		case F_AIRET: {
		  ARBINT b;
		  mpz_init(b);
		  ((void (*)())pfuncs[i].routine)(b,pos);
		  a=mpz_get_d(b);
		  mpz_clear(b);
		  }
		  return(a);
		case F_ANYRET: 
		case F_NRET:
		case F_ANYIRET: {
		     PARAMETER b;
		     b=((ppfunc)(pfuncs[i].routine))(pos);
		     a=p2float(&b);
		     free_parameter(&b);
		     return(a);
	         }
	        default: xberror(13,s);  /* Type mismatch */
                }
	      } else if((pfuncs[i].opcode&FM_TYP)==F_PLISTE) {
	        PARAMETER *plist;
                int e=make_pliste(pfuncs[i].pmin,pfuncs[i].pmax,(unsigned short *)pfuncs[i].pliste,pos,&plist);
                double a=0;
		// printf("e=%d pmin=%d\n",e,pfuncs[i].pmin);
		if(e<0) xberror(51,s); /* Syntax Error */
		else if(e<pfuncs[i].pmin) xberror(42,s); /* Zu wenig Parameter */
		else {
		   switch(pfuncs[i].opcode&FM_RET) {
		   case F_IRET: a=(double)((int (*)())pfuncs[i].routine)(plist,e); break;
		   case F_CRET: a=COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(plist,e)); break;
		   case F_DRET: a=(pfuncs[i].routine)(plist,e); break;
	  	   case F_AIRET: {
		     ARBINT b;
		     mpz_init(b);
		     ((void (*)())pfuncs[i].routine)(b,plist,e);
		     a=mpz_get_d(b);
		     mpz_clear(b);
		     }
		     return(a);
		   case F_ANYRET: 
		   case F_NRET:
		   case F_ANYIRET: {
		     PARAMETER b;
		     b=((ppfunc)(pfuncs[i].routine))(plist,e);
		     a=p2float(&b);
		     free_parameter(&b);
		   } break;
		   default: xberror(13,s);  /* Type mismatch */
		   }
		}
	        if(e>=0) free_pliste(e,plist);
	        return(a);
              }
	      
	      /*Jetzt können nur noch IRET, CRET oder DRET übrig sein.*/
	      
	      
	      if(pfuncs[i].pmax==1) {
	        switch(pfuncs[i].opcode&(FM_TYP|FM_RET)) {
	        case (F_DQUICK|F_IRET): return((double)((int (*)())pfuncs[i].routine)(parser(pos)));
	        case (F_DQUICK|F_CRET): return(COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(parser(pos))));
	        case (F_DQUICK|F_DRET): return((pfuncs[i].routine)(*pos?parser(pos):0)); /*der Kniff ist für RND()*/
		case (F_CQUICK|F_IRET): return((double)((int (*)())pfuncs[i].routine)(complex_parser(pos)));
		case (F_CQUICK|F_CRET): return(COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(complex_parser(pos))));
		case (F_CQUICK|F_DRET): return((pfuncs[i].routine)(complex_parser(pos)));
		case (F_IQUICK|F_IRET): return((double)((int (*)())pfuncs[i].routine)((int)parser(pos)));
		case (F_IQUICK|F_CRET): return(COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)((int)parser(pos))));
		case (F_IQUICK|F_DRET): return((pfuncs[i].routine)((int)parser(pos)));
	        }
		if((pfuncs[i].opcode&FM_TYP)==F_SQUICK) {
		  STRING test=string_parser(pos);
		  double erg;
		  test.pointer=realloc(test.pointer,test.len+1);  /*TODO: is this still necessary ?*/
		  test.pointer[test.len]=0;
	          if((pfuncs[i].opcode&FM_RET)==F_IRET) erg=(double)((int (*)())pfuncs[i].routine)(test);
	          else if((pfuncs[i].opcode&FM_RET)==F_CRET) erg=COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(test));
		  else erg=(pfuncs[i].routine)(test);
		  free(test.pointer);
		  return(erg);
		}
	      } else if(pfuncs[i].pmax==2) {
	       	char *w1,*w2;
	        int e=wort_sep_destroy(pos,',',TRUE,&w1,&w2);
	        if(e<2) xberror(56,s); /* Falsche Anzahl Parameter */
	        switch(pfuncs[i].opcode&(FM_TYP|FM_RET)) {
 	        case (F_CQUICK|F_IRET): return((double)((int (*)())pfuncs[i].routine)(complex_parser(w1),complex_parser(w2)));
		case (F_CQUICK|F_CRET): return(COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(complex_parser(w1),complex_parser(w2))));
		case (F_CQUICK|F_DRET): return((pfuncs[i].routine)(complex_parser(w1),complex_parser(w2)));
		case (F_DQUICK|F_IRET): return((double)((int (*)())pfuncs[i].routine)(parser(w1),parser(w2)));
		case (F_DQUICK|F_CRET): return(COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(parser(w1),parser(w2))));
		case (F_DQUICK|F_DRET): return((pfuncs[i].routine)(parser(w1),parser(w2)));
		case (F_IQUICK|F_IRET): return((double)((int (*)())pfuncs[i].routine)((int)parser(w1),(int)parser(w2)));
		case (F_IQUICK|F_CRET): return(COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)((int)parser(w1),(int)parser(w2))));
		case (F_IQUICK|F_DRET): return((pfuncs[i].routine)((int)parser(w1),(int)parser(w2)));
		}
	      }
	      
		/* Es sollten alle Möglichkeiten abgefangen sein.*/
	      printf("Interner ERROR. Funktion nicht korrekt definiert. %s\n",s);




	   /* Nicht in der Liste ? Dann kann es noch ARRAY sein   */
	
          } else if(((stype=type(s))&TYPMASK)==FLOATTYP) {
	//  printf("Parser: stype=$%x, <%s>\n",stype,s);
            if((vnr=var_exist(s,ARRAYTYP,FLOATTYP,0))!=-1) {
	  //  printf("vnr=%d \n",vnr);
	  //  c_dump(NULL,0);
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      if(!check_indexliste(a,indexliste)) {
	        xberror(16,""); /* Feldindex zu gross*/
		return(0);
	      } else return(float_array_element(a,indexliste));
	    } else { xberror(15,s); return(0); } /* Feld nicht dimensioniert  */
          } else if((stype&TYPMASK)==INTTYP) {
	    char *r=varrumpf(s);
	//  printf("Parser: stype=$%x, <%s>\n",stype,s);
	    if((vnr=var_exist(r,ARRAYTYP,INTTYP,0))!=-1) {
	      free(r);
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      if(!check_indexliste(a,indexliste)) {
	        xberror(16,""); /* Feldindex zu gross*/
		return(0);
	      } else return((double)int_array_element(a,indexliste));
	    } else { xberror(15,s); free(r); return(0); }  /* Feld nicht dimensioniert  */
          } else if((stype&TYPMASK)==COMPLEXTYP) {
	    char *r=varrumpf(s);
	    if((vnr=var_exist(r,ARRAYTYP,COMPLEXTYP,0))!=-1) {
	      free(r);
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      if(!check_indexliste(a,indexliste)) {
	        xberror(16,""); /* Feldindex zu gross*/
		return(0);
	      } else return(COMPLEX2FLOAT(complex_array_element(a,indexliste)));
	    } else { xberror(15,s); free(r); return(0); }  /* Feld nicht dimensioniert  */

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
            if(i>=a) break;
          }
          if(strcmp(s,sysvars[i].name)==0) {
	     // printf("Sysvar %s gefunden. Nr. %d\n",sysvars[i].name,i);
	   if((sysvars[i].opcode&TYPMASK)==INTTYP) return((double)((int (*)())sysvars[i].routine)());
	   if((sysvars[i].opcode&TYPMASK)==FLOATTYP) return((sysvars[i].routine)());
          }
	  	  
      /* erst integer abfangen (xxx% oder xxx&), dann rest */
      if(*s=='@')                              return(do_funktion(s+1,""));

      switch(s[l-1]) {
      case '%':
        s[l-1]=0;
        if((vnr=var_exist(s,INTTYP,0,0))!=-1) return((double)*variablen[vnr].pointer.i);
        return(0);
      case '#':
        s[l-1]=0;
        if((vnr=var_exist(s,COMPLEXTYP,0,0))!=-1) return(COMPLEX2FLOAT(*variablen[vnr].pointer.c));
        return(0);
      case '&':
        s[l-1]=0;
        if((vnr=var_exist(s,ARBINTTYP,0,0))!=-1) return(mpz_get_d(*variablen[vnr].pointer.ai));
        return(0);
      default:
        if((vnr=var_exist(s,FLOATTYP,0,0))!=-1) return(*variablen[vnr].pointer.f);    
        return(myatof(s));  /* Jetzt nur noch Zahlen (hex, oct etc ...)*/
      }
    }
  }
  xberror(51,s); /* Syntax error */
  return(0);
}


/* Langsamerer Parser für die Komplexen Funktionen. Noch nicht vollstaendig...*/




COMPLEX complex_parser(const char *funktion) {
  char s[strlen(funktion)+1];
  char *w1,*w2;
  COMPLEX ret;
  ret.i=ret.r=0;
  int i;

 // printf("Complex-Parser: <%s>\n",funktion);
  xtrim(funktion,TRUE,s);  /* Leerzeichen vorne und hinten entfernen, Grossbuchstaben */
  if(*s==0) { xberror(51,funktion); return(ret); }

  /* Logische Operatoren AND OR NOT ... */

  if(searchchr2_multi(s,"&|")!=NULL) {
    if(wort_sepr2_destroy(s,"||",TRUE,&w1,&w2)>1) return(INT2COMPLEX(COMPLEX2INT(complex_parser(w1)) | COMPLEX2INT(complex_parser(w2))));
    if(wort_sepr2_destroy(s,"&&",TRUE,&w1,&w2)>1) return(INT2COMPLEX(COMPLEX2INT(complex_parser(w1)) & COMPLEX2INT(complex_parser(w2))));
  }
  if(searchchr2(s,' ')!=NULL) {
    if(wort_sepr2_destroy(s," OR ",TRUE,&w1,&w2)>1)   return(INT2COMPLEX(COMPLEX2INT(complex_parser(w1)) | COMPLEX2INT(complex_parser(w2))));
    if(wort_sepr2_destroy(s," AND ",TRUE,&w1,&w2)>1)  return(INT2COMPLEX(COMPLEX2INT(complex_parser(w1)) & COMPLEX2INT(complex_parser(w2))));
    if(wort_sepr2_destroy(s," NAND ",TRUE,&w1,&w2)>1) return(INT2COMPLEX(~(COMPLEX2INT(complex_parser(w1)) & COMPLEX2INT(complex_parser(w2)))));
    if(wort_sepr2_destroy(s," NOR ",TRUE,&w1,&w2)>1)  return(INT2COMPLEX(~(COMPLEX2INT(complex_parser(w1)) | COMPLEX2INT(complex_parser(w2)))));
    if(wort_sepr2_destroy(s," XOR ",TRUE,&w1,&w2)>1)  return(INT2COMPLEX(COMPLEX2INT(complex_parser(w1)) ^ COMPLEX2INT(complex_parser(w2))));
    if(wort_sepr2_destroy(s," EOR ",TRUE,&w1,&w2)>1)  return(INT2COMPLEX(COMPLEX2INT(complex_parser(w1)) ^ COMPLEX2INT(complex_parser(w2))));
    if(wort_sepr2_destroy(s," EQV ",TRUE,&w1,&w2)>1)  return(INT2COMPLEX(~(COMPLEX2INT(complex_parser(w1)) ^ COMPLEX2INT(complex_parser(w2)))));
    if(wort_sepr2_destroy(s," IMP ",TRUE,&w1,&w2)>1)  {
      int i=COMPLEX2INT(complex_parser(w2));
      return(INT2COMPLEX(~(COMPLEX2INT(complex_parser(w1)) ^ i) | i));
    }
    if(wort_sepr2_destroy(s," MOD ",TRUE,&w1,&w2)>1)  return(FLOAT2COMPLEX(fmod(COMPLEX2FLOAT(complex_parser(w1)),COMPLEX2FLOAT(complex_parser(w2)))));
    if(wort_sepr2_destroy(s," DIV ",TRUE,&w1,&w2)>1) {
      int nenner=COMPLEX2INT(complex_parser(w2));
      if(nenner) return(INT2COMPLEX(COMPLEX2INT(complex_parser(w1)) / nenner));
      else {
        xberror(0,w2); /* Division durch 0 */
        return(ret);
      }
    }
    if(wort_sepr2_destroy(s,"NOT ",TRUE,&w1,&w2)>1) {
      if(*w1==0) return(INT2COMPLEX(~COMPLEX2INT(complex_parser(w2))));    /* von rechts !!  */
      /* Ansonsten ist NOT Teil eines Variablennamens */
    }
  }

  /* Erst Vergleichsoperatoren mit Wahrheitwert abfangen (lowlevel < Addition)  */


  if(compareoperator_check_int(s, &i)) return(INT2COMPLEX(i));



 /* Addition/Subtraktion/Vorzeichen  */
 
 
  /* Suche eine g"ultige Trennstelle f"ur + oder -  */
  /* Hier muessen wir testen, ob es nicht ein vorzeichen war oder Teil eines Exponenten ...*/
  char *pos=searchchr2_multi_r(s,"+-",s+strlen(s));  /* Finde n"achsten Kandidaten  von rechts*/
  while(pos!=NULL) {
    if(pos==s) {  /*Das +/-  war ganz am Anfang*/
      if(*s=='-') return(complex_neg(complex_parser(s+1)));
      else return(complex_parser(s+1));
    }
    if(pos>s && strchr("*/^",*(pos-1))==NULL && 
                                     !( *(pos-1)=='E' && pos-1>s && vfdigittest(s,pos-1) && v_digit(*(pos+1)))) {
    /* Kandidat war gut.*/
      char c=*pos;
      *pos=0;
      if(c=='-') return(complex_sub(complex_parser(s),complex_parser(pos+1)));
      else return(complex_add(complex_parser(s),complex_parser(pos+1)));
    }
    pos=searchchr2_multi_r(s,"+-",pos);  /* Finde n"achsten Kandidaten  von rechts*/
  }
  char *pos2;
  char w1buf[strlen(funktion)+1],w2buf[strlen(funktion)+1];
  int vnr;
  w1=w1buf;
  w2=w2buf;
  if(searchchr2_multi(s,"*/^")!=NULL) {
    if(wort_sepr(s,'*',TRUE,w1,w2)>1) {
      if(*w1) return(complex_mul(complex_parser(w1),complex_parser(w2)));
      else {
        printf("Pointer not yet implemented! %s\n",w2);   /* war pointer - */
        return(ret);
      }
    }
    if(wort_sepr(s,'/',TRUE,w1,w2)>1) {
      if(*w1) {
        COMPLEX nenner=complex_parser(w2);
        if(nenner.r||nenner.i) return(complex_div(complex_parser(w1),nenner));    /* von rechts !!  */
        else { xberror(0,w2); return(ret);  } /* Division durch 0 */
      } else { xberror(51,w2); return(ret); }/* "Parser: Syntax error?! "  */
    }
    if(wort_sepr(s,'^',TRUE,w1,w2)>1) {
      if(*w1) return(complex_pow(complex_parser(w1),complex_parser(w2)));    /* von rechts !!  */
      else { xberror(51,w2); return(ret); } /* "Parser: Syntax error?! "  */
    }
  }
  if(*s=='(' && s[strlen(s)-1]==')')  { /* Ueberfluessige Klammern entfernen */
    s[strlen(s)-1]=0;
    return(complex_parser(s+1));
    /* SystemFunktionen Subroutinen und Arrays */
  } 
  pos=searchchr(s, '(');
  if(pos!=NULL) {
    pos2=s+strlen(s)-1;
    *pos++=0;
    if(*pos2!=')') xberror(51,w2); /* "Parser: Syntax error?! "  */
    else {                         /* $-Funktionen und $-Felder   */
      *pos2=0;
      /* Benutzerdef. Funktionen */
      if(*s=='@') return(do_cfunktion(s+1,pos));
      else {
	
	  /* Dann liste mit funktionen */
	  int i=0;
	  int a=anzpfuncs-1,b,stype,l=strlen(s);
          for(b=0; b<l; b++) {
            while(s[b]>(pfuncs[i].name)[b] && i<a) i++;
            while(s[b]<(pfuncs[a].name)[b] && a>i) a--;
            if(i==a) break;
          }
          if(strcmp(s,pfuncs[i].name)==0) {
 /* printf("Funktion %s gefunden. Nr. %d\n",pfuncs[i].name,i); */
	      if((pfuncs[i].opcode&FM_TYP)==F_SIMPLE || pfuncs[i].pmax==0) {
	        switch(pfuncs[i].opcode&FM_RET) {
	        case F_IRET: return(INT2COMPLEX(((int (*)())pfuncs[i].routine)()));
	        case F_CRET: return(((COMPLEX (*)())pfuncs[i].routine)());
		case F_DRET: return(FLOAT2COMPLEX((pfuncs[i].routine)()));
		case F_AIRET: {
		  ARBINT b;
		  mpz_init(b);
		  ((void (*)())pfuncs[i].routine)(b);
		  ret=FLOAT2COMPLEX(mpz_get_d(b));
		  mpz_clear(b);
		  }
		  return(ret);
		case F_ANYRET: 
		case F_NRET: 
		case F_ANYIRET: {
		     PARAMETER b=((ppfunc)(pfuncs[i].routine))();
		     ret=p2complex(&b);
		     free_parameter(&b);
		     return(ret);
	         }
	        default: xberror(13,s);  /* Type mismatch */
                }
	      } else if((pfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
	        switch(pfuncs[i].opcode&FM_RET) {
	        case F_IRET: return(INT2COMPLEX(((int (*)())pfuncs[i].routine)(pos)));
	        case F_CRET: return(((COMPLEX (*)())pfuncs[i].routine)(pos));
		case F_DRET: return(FLOAT2COMPLEX((pfuncs[i].routine)(pos)));
		case F_AIRET: {
		  ARBINT b;
		  mpz_init(b);
		  ((void (*)())pfuncs[i].routine)(b,pos);
		  ret=FLOAT2COMPLEX(mpz_get_d(b));
		  mpz_clear(b);
		  }
		  return(ret);
		case F_ANYRET: 
		case F_NRET:
		case F_ANYIRET: {
		     PARAMETER b;
		     b=((ppfunc)(pfuncs[i].routine))(pos);
		     ret=p2complex(&b);
		     free_parameter(&b);
		     return(ret);
                }
	        default: xberror(13,s);  /* Type mismatch */
		}
              } else if((pfuncs[i].opcode&FM_TYP)==F_PLISTE) {
	        PARAMETER *plist;
                int e=make_pliste(pfuncs[i].pmin,pfuncs[i].pmax,(unsigned short *)pfuncs[i].pliste,pos,&plist);
		// printf("e=%d pmin=%d\n",e,pfuncs[i].pmin);
		if(e<0)                   xberror(51,s); /* Syntax Error */
		else if(e<pfuncs[i].pmin) xberror(42,s); /* Zu wenig Parameter */
		else {
		   switch(pfuncs[i].opcode&FM_RET) {
		   case F_IRET: ret=INT2COMPLEX(((int (*)())pfuncs[i].routine)(plist,e)); break;
		   case F_CRET: ret=((COMPLEX (*)())pfuncs[i].routine)(plist,e); break;
		   case F_DRET: ret=FLOAT2COMPLEX((pfuncs[i].routine)(plist,e)); break;
		   case F_AIRET: {
		     ARBINT b;
		     mpz_init(b);
		     ((void (*)())pfuncs[i].routine)(b,plist,e);
		     ret=FLOAT2COMPLEX(mpz_get_d(b));
		     mpz_clear(b);
		     }
		     return(ret);
		   case F_ANYRET: 
		   case F_NRET:
		   case F_ANYIRET: {
		     PARAMETER b;
		     b=((ppfunc)(pfuncs[i].routine))(plist,e);
		     ret=p2complex(&b);
		     free_parameter(&b);
		   } break;
		   default: xberror(13,s);  /* Type mismatch */
		   }
		} 
	        if(e>=0) free_pliste(e,plist);
	        return(ret);
              }

	      /*Jetzt können nur noch IRET, CRET oder DRET übrig sein.*/

	      if(pfuncs[i].pmax==1) {
	        switch(pfuncs[i].opcode&(FM_TYP|FM_RET)) {
	        case (F_DQUICK|F_IRET): return(INT2COMPLEX(((int (*)())pfuncs[i].routine)(parser(pos))));
	        case (F_DQUICK|F_CRET): return(((COMPLEX (*)())pfuncs[i].routine)(parser(pos)));
	        case (F_DQUICK|F_DRET): return(FLOAT2COMPLEX((pfuncs[i].routine)(parser(pos))));
		case (F_CQUICK|F_IRET): return(INT2COMPLEX(((int (*)())pfuncs[i].routine)(complex_parser(pos))));
		case (F_CQUICK|F_CRET): return(((COMPLEX (*)())pfuncs[i].routine)(complex_parser(pos)));
		case (F_CQUICK|F_DRET): return(FLOAT2COMPLEX((pfuncs[i].routine)(complex_parser(pos))));
		case (F_IQUICK|F_IRET): return(INT2COMPLEX(((int (*)())pfuncs[i].routine)((int)parser(pos))));
		case (F_IQUICK|F_CRET): return(((COMPLEX (*)())pfuncs[i].routine)((int)parser(pos)));
		case (F_IQUICK|F_DRET): return(FLOAT2COMPLEX((pfuncs[i].routine)((int)parser(pos))));
	        }
		if((pfuncs[i].opcode&FM_TYP)==F_SQUICK) {
		  STRING test=string_parser(pos);
		  test.pointer=realloc(test.pointer,test.len+1);  /*TODO: is this still necessary ?*/
		  test.pointer[test.len]=0;
	          if((pfuncs[i].opcode&FM_RET)==F_IRET) ret=INT2COMPLEX(((int (*)())pfuncs[i].routine)(test));
	          else if((pfuncs[i].opcode&FM_RET)==F_CRET) ret=((COMPLEX (*)())pfuncs[i].routine)(test);
		  else ret=FLOAT2COMPLEX((pfuncs[i].routine)(test));
		  free(test.pointer);
		  return(ret);
		}
	      } 
	      else if(pfuncs[i].pmax==2) {
	       	char *w1,*w2;
	        int e=wort_sep_destroy(pos,',',TRUE,&w1,&w2);
	        if(e<2) xberror(56,s); /* Falsche Anzahl Parameter */
	        switch(pfuncs[i].opcode&(FM_TYP|FM_RET)) {
 	        case (F_CQUICK|F_IRET): return(INT2COMPLEX(((int (*)())pfuncs[i].routine)(complex_parser(w1),complex_parser(w2))));
		case (F_CQUICK|F_CRET): return(((COMPLEX (*)())pfuncs[i].routine)(complex_parser(w1),complex_parser(w2)));
		case (F_CQUICK|F_DRET): return(FLOAT2COMPLEX((pfuncs[i].routine)(complex_parser(w1),complex_parser(w2))));
		case (F_DQUICK|F_IRET): return(INT2COMPLEX(((int (*)())pfuncs[i].routine)(parser(w1),parser(w2))));
		case (F_DQUICK|F_CRET): return(((COMPLEX (*)())pfuncs[i].routine)(parser(w1),parser(w2)));
		case (F_DQUICK|F_DRET): return(FLOAT2COMPLEX((pfuncs[i].routine)(parser(w1),parser(w2))));
		case (F_IQUICK|F_IRET): return(INT2COMPLEX(((int (*)())pfuncs[i].routine)((int)parser(w1),(int)parser(w2))));
		case (F_IQUICK|F_CRET): return(((COMPLEX (*)())pfuncs[i].routine)((int)parser(w1),(int)parser(w2)));
		case (F_IQUICK|F_DRET): return(FLOAT2COMPLEX((pfuncs[i].routine)((int)parser(w1),(int)parser(w2))));
		}
	      }
	      
		/* Es sollten alle Möglichkeiten abgefangen sein.*/
	      printf("Interner ERROR. Funktion nicht korrekt definiert. %s\n",s);

	 
	   /* Nicht in der Liste ? Dann kann es noch ARRAY sein   */

          } else if(((stype=type(s))&TYPMASK)==FLOATTYP) {
	 // printf("Parser: stype=$%x, <%s>\n",stype,s);
            if((vnr=var_exist(s,ARRAYTYP,FLOATTYP,0))!=-1) {
	  //  printf("vnr=%d \n",vnr);
	  //  c_dump(NULL,0);
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      if(!check_indexliste(a,indexliste)) {
	        xberror(16,""); /* Feldindex zu gross*/
		return(ret);
	      } else return(FLOAT2COMPLEX(float_array_element(a,indexliste)));
	    } else { xberror(15,s); return(ret); } /* Feld nicht dimensioniert  */
          } else if((stype&TYPMASK)==INTTYP) {
	    char *r=varrumpf(s);
	    if((vnr=var_exist(r,ARRAYTYP,INTTYP,0))!=-1) {
	      free(r);
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      if(!check_indexliste(a,indexliste)) {
	        xberror(16,""); /* Feldindex zu gross*/
		return(ret);
	      } else return(INT2COMPLEX(int_array_element(a,indexliste)));
	    } else { xberror(15,s);free(r); return(ret); }  /* Feld nicht dimensioniert  */
          } else if((stype&TYPMASK)==COMPLEXTYP) {
	    char *r=varrumpf(s);
	    if((vnr=var_exist(r,ARRAYTYP,COMPLEXTYP,0))!=-1) {
	      free(r);
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      if(!check_indexliste(a,indexliste)) {
	        xberror(16,""); /* Feldindex zu gross*/
		return(ret);
	      } else return(complex_array_element(a,indexliste));
	    } else { xberror(15,s); free(r); return(ret); }  /* Feld nicht dimensioniert  */
	} else { xberror(15,s); return(ret); }  /* Feld nicht dimensioniert  */
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
      if(i>=a) break;
    }
    if(strcmp(s,sysvars[i].name)==0) {
       // printf("Sysvar %s gefunden. Nr. %d\n",sysvars[i].name,i);
      if((sysvars[i].opcode&TYPMASK)==INTTYP) return(INT2COMPLEX(((int (*)())sysvars[i].routine)()));
      if((sysvars[i].opcode&TYPMASK)==FLOATTYP) return(FLOAT2COMPLEX((sysvars[i].routine)()));
    }
    /* erst integer abfangen (xxx% oder xxx&), dann rest */
    if(*s=='@') 			     return(do_cfunktion(s+1,"")); 
    switch(s[l-1]) {
    case '%':
      s[l-1]=0;
      if((vnr=var_exist(s,INTTYP,0,0))!=-1) return(INT2COMPLEX(*variablen[vnr].pointer.i));
      return(ret);
    case '#':
      s[l-1]=0;
      if((vnr=var_exist(s,COMPLEXTYP,0,0))!=-1) return(*variablen[vnr].pointer.c);
      return(ret);
    case '&':
      s[l-1]=0;
      if((vnr=var_exist(s,ARBINTTYP,0,0))!=-1) return(FLOAT2COMPLEX(mpz_get_d(*variablen[vnr].pointer.ai)));
      return(ret);
    default:
      if((vnr=var_exist(s,FLOATTYP,0,0))!=-1) return(FLOAT2COMPLEX(*variablen[vnr].pointer.f));
      return(complex_myatof(s));  /* Jetzt nur noch Zahlen (hex, oct etc ...)*/
    } 
  }
  xberror(51,s); /* Syntax error */
  return(ret);
}

/*Arbintparser: Noch nicht ganz fertig. Einige Extrafunktionen.*/


void arbint_parser(const char *funktion, ARBINT ret) {
  char s[strlen(funktion)+1];
  char *w1,*w2;

  // printf("Arbint-Parser: <%s>\n",funktion);
  xtrim(funktion,TRUE,s);  /* Leerzeichen vorne und hinten entfernen, Grossbuchstaben */
  if(*s==0) { xberror(51,funktion); return; }

  /* Logische Operatoren AND OR NOT ... */
  if(searchchr2_multi(s,"&|")!=NULL) {
    if(wort_sepr2_destroy(s,"||",TRUE,&w1,&w2)>1) {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      mpz_ior(ret,a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      return;
    }
    if(wort_sepr2_destroy(s,"&&",TRUE,&w1,&w2)>1) {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      mpz_and(ret,a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      return;
    }
  }
  if(searchchr2(s,' ')!=NULL) {
    if(wort_sepr2_destroy(s," OR ",TRUE,&w1,&w2)>1)   {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      mpz_ior(ret,a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      return;
    }
    if(wort_sepr2_destroy(s," AND ",TRUE,&w1,&w2)>1)  {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      mpz_and(ret,a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      return;
    }
    if(wort_sepr2_destroy(s," NAND ",TRUE,&w1,&w2)>1) {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      mpz_and(ret,a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      mpz_com(ret,ret);
      return;
    }
    if(wort_sepr2_destroy(s," NOR ",TRUE,&w1,&w2)>1)  {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      mpz_ior(ret,a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      mpz_com(ret,ret);
      return;
    }
    if(wort_sepr2_destroy(s," XOR ",TRUE,&w1,&w2)>1)  {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      mpz_xor(ret,a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      return;
    }
    if(wort_sepr2_destroy(s," EOR ",TRUE,&w1,&w2)>1)  {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      mpz_xor(ret,a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      return;
    }
    if(wort_sepr2_destroy(s," EQV ",TRUE,&w1,&w2)>1)  {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      mpz_xor(ret,a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      mpz_com(ret,ret);
      return;
    }
    if(wort_sepr2_destroy(s," IMP ",TRUE,&w1,&w2)>1)  {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      mpz_xor(ret,a1,a2);
      mpz_com(ret,ret);
      mpz_ior(ret,ret,a2);
      mpz_clear(a1);mpz_clear(a2);
      return;
    }
    if(wort_sepr2_destroy(s," MOD ",TRUE,&w1,&w2)>1)  {
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(w1,a1);
      mpz_init(a2); arbint_parser(w2,a2);
      mpz_mod(ret,a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      return;
    }
    if(wort_sepr2_destroy(s," DIV ",TRUE,&w1,&w2)>1) {
        ARBINT a1,a2;
        mpz_init(a2); arbint_parser(w2,a2);
        if(mpz_cmp_si(a2,0)) {
          mpz_init(a1); arbint_parser(w1,a1);         
          mpz_div(ret,a1,a2);
          mpz_clear(a1);
        } else xberror(0,w2);  /* Division durch 0 */
       mpz_clear(a2);
       return;
    }
    if(wort_sepr2_destroy(s,"NOT ",TRUE,&w1,&w2)>1) {
      if(*w1==0) {
        arbint_parser(w2,ret);
	mpz_com(ret,ret);
        return;   
      } /* Ansonsten ist NOT Teil eines Variablennamens */
    }
  }

  /* Erst Vergleichsoperatoren mit Wahrheitwert abfangen (lowlevel < Addition)  */
  int i;
  if(compareoperator_check_int(s, &i)) {mpz_set_si(ret,i); return;}

 /* Addition/Subtraktion/Vorzeichen  */
 
  /* Suche eine g"ultige Trennstelle f"ur + oder -  */
  /* Hier muessen wir testen, ob es nicht ein vorzeichen war oder Teil eines Exponenten ...*/
  char *pos=searchchr2_multi_r(s,"+-",s+strlen(s));  /* Finde n"achsten Kandidaten  von rechts*/
  while(pos!=NULL) {
    if(pos==s) {  /*Das +/-  war ganz am Anfang*/
      if(*s=='-') {
	arbint_parser(s+1,ret);
	mpz_neg(ret,ret);
        return;
      } else {
        arbint_parser(s+1,ret);
        return;
      }
    }
    if(pos>s && strchr("*/^",*(pos-1))==NULL && 
                                     !( *(pos-1)=='E' && pos-1>s && vfdigittest(s,pos-1) && v_digit(*(pos+1)))) {
    /* Kandidat war gut.*/
      char c=*pos;
      *pos=0;
      ARBINT a1,a2;
      mpz_init(a1); arbint_parser(s,a1);
      mpz_init(a2); arbint_parser(pos+1,a2);
      if(c=='-') mpz_sub(ret,a1,a2);
      else       mpz_add(ret,a1,a2);
      mpz_clear(a1);mpz_clear(a2);
      return;
    }
    pos=searchchr2_multi_r(s,"+-",pos);  /* Finde n"achsten Kandidaten  von rechts*/
  }
  
  char *pos2;
  char w1buf[strlen(funktion)+1],w2buf[strlen(funktion)+1];
  int vnr;
  w1=w1buf;
  w2=w2buf;
 
  if(searchchr2_multi(s,"*/^")!=NULL) {
    if(wort_sepr(s,'*',TRUE,w1,w2)>1) {
      if(*w1) {
        ARBINT a1,a2;
        mpz_init(a1); arbint_parser(w1,a1);
        mpz_init(a2); arbint_parser(w2,a2);
        mpz_mul(ret,a1,a2);
        mpz_clear(a1);mpz_clear(a2);
        return;
      } else {
        printf("Pointer not yet implemented! %s\n",w2);   /* war pointer - */
        return;
      }
    }
    if(wort_sepr(s,'/',TRUE,w1,w2)>1) {
      if(*w1) {
        ARBINT a1,a2;
        mpz_init(a2); arbint_parser(w2,a2);
        if(mpz_cmp_si(a2,0)) {
          mpz_init(a1); arbint_parser(w1,a1);         
          mpz_div(ret,a1,a2);
          mpz_clear(a1);mpz_clear(a2);
          return;
        } else { xberror(0,w2); mpz_clear(a2); return;  } /* Division durch 0 */
	
      } else { xberror(51,w2); return; }/* "Parser: Syntax error?! "  */
    }
    if(wort_sepr(s,'^',TRUE,w1,w2)>1) {
      if(*w1) {
        ARBINT a1,a2;
        mpz_init(a1); arbint_parser(w1,a1);
        mpz_init(a2); arbint_parser(w2,a2);
        mpz_pow_ui(ret,a1,mpz_get_ui(a2));
        mpz_clear(a1);mpz_clear(a2);
        return;
      } else { xberror(51,w2); return; } /* "Parser: Syntax error?! "  */
    }
  }
  if(*s=='(' && s[strlen(s)-1]==')')  { /* Ueberfluessige Klammern entfernen */
    s[strlen(s)-1]=0;
    arbint_parser(s+1,ret);
    return;
    /* SystemFunktionen Subroutinen und Arrays */
  } 
  pos=searchchr(s, '(');
  if(pos!=NULL) {
    pos2=s+strlen(s)-1;
    *pos++=0;
    if(*pos2!=')') xberror(51,w2); /* "Parser: Syntax error?! "  */
    else {                         /* $-Funktionen und $-Felder   */
      *pos2=0;
      /* Benutzerdef. Funktionen */
      if(*s=='@') {
        do_benfunction(s+1,pos);
        p2arbint(&returnvalue,ret);
        return;
      } 
      /* Hier jetzt spezielle arbint funktionen einf"ugen/abfangen 
      (gehen dann nicht im compiler !) */
      if(strcmp(s,"PRIMORIAL")==0) {
      #ifdef HAVE_PRIMORIAL_UI
        int n=(int)parser(pos);
        mpz_primorial_ui(ret,n);
      #else
        xberror(9,"PRIMORIAL"); /*Function or command %s not implemented*/
      #endif
        return;
      }
      if(strcmp(s,"FIB")==0) {
        int n=(int)parser(pos);
        mpz_fib_ui(ret,n);
        return;
      }
      if(strcmp(s,"LUCNUM")==0) {
        int n=(int)parser(pos);
        mpz_lucnum_ui(ret,n);
        return;
      }
      /* Dann liste mit funktionen */
      int i=0;
      int a=anzpfuncs-1,b,stype,l=strlen(s);
      for(b=0; b<l; b++) {
        while(s[b]>(pfuncs[i].name)[b] && i<a) i++;
        while(s[b]<(pfuncs[a].name)[b] && a>i) a--;
        if(i==a) break;
      }
      if(strcmp(s,pfuncs[i].name)==0) {
        // printf("Funktion %s gefunden. Nr. %d\n",pfuncs[i].name,i); 
	if((pfuncs[i].opcode&FM_TYP)==F_SIMPLE || pfuncs[i].pmax==0) {
	  switch(pfuncs[i].opcode&FM_RET) {
	  case F_IRET: mpz_set_si(ret,((int (*)())pfuncs[i].routine)());return;
	  case F_CRET: mpz_set_d(ret,COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)()));return;
	  case F_DRET: mpz_set_d(ret,(pfuncs[i].routine)());return;
          case F_AIRET: 
	    ((void (*)())pfuncs[i].routine)(ret);		  
	       return;
	  case F_ANYRET: 
	  case F_NRET: 
	  case F_ANYIRET: {
	       PARAMETER b=((ppfunc)(pfuncs[i].routine))();
	       p2arbint(&b,ret);
	       free_parameter(&b);
	       return;
	   }
	  default: xberror(13,s);  /* Type mismatch */
          }
	} else if((pfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
	  switch(pfuncs[i].opcode&FM_RET) {
	  case F_IRET: mpz_set_si(ret,(((int (*)())pfuncs[i].routine)(pos)));return;
	  case F_CRET: mpz_set_d(ret,COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(pos)));return;
  	  case F_DRET: mpz_set_d(ret,((pfuncs[i].routine)(pos)));return;
          case F_AIRET: 
		  ((void (*)())pfuncs[i].routine)(ret,pos);
		  return;
	  case F_ANYRET: 
	  case F_NRET:
	  case F_ANYIRET: {
	       PARAMETER b=((ppfunc)(pfuncs[i].routine))(pos);
	       p2arbint(&b,ret);
	       free_parameter(&b);
	       return;
	   }
	  default: xberror(13,s);  /* Type mismatch */
	  }
	} else if((pfuncs[i].opcode&FM_TYP)==F_PLISTE) {
	  PARAMETER *plist;
          int e=make_pliste(pfuncs[i].pmin,pfuncs[i].pmax,(unsigned short *)pfuncs[i].pliste,pos,&plist);
          if(e<0)                   xberror(51,s); /* Syntax Error */
	  else if(e<pfuncs[i].pmin) xberror(42,s); /* Zu wenig Parameter */
	  else {
	     switch(pfuncs[i].opcode&FM_RET) {
	     case F_IRET: mpz_set_si(ret,(((int (*)())pfuncs[i].routine)(plist,e))); break;
	     case F_CRET: mpz_set_d(ret,COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(plist,e))); break;
	     case F_DRET: mpz_set_d(ret,((pfuncs[i].routine)(plist,e)));
	     case F_AIRET: 
	       ((void (*)())pfuncs[i].routine)(ret,plist,e);
	       return;
	     case F_ANYRET: 
	     case F_NRET: 
	     case F_ANYIRET: {
	       PARAMETER b=((ppfunc)(pfuncs[i].routine))(plist,e);
	       p2arbint(&b,ret);
	       free_parameter(&b);
	       return;
	     }
	     default: xberror(13,s);  /* Type mismatch */
	     }
	  } 
	  if(e>=0) free_pliste(e,plist);
	  return;
        }

/*
       Hier haben wir noch die Schwierigkeit Funktionen einzufügen, welche normalerweise IRET oder DRET sind, aber
       durchaus BIGINT zurückliefern können. 
       --> ANYRET, ANYIRET, muss also noch umgestrickt werden (macht es langsamer!): 
       
       Also z.B. ABS(), 
        VAL() VARIAT() XOR() 
*/


	      /*Jetzt können nur noch IRET, CRET oder DRET übrig sein.*/

	      if(pfuncs[i].pmax==1) {
	        switch(pfuncs[i].opcode&(FM_TYP|FM_RET)) {
	        case (F_DQUICK|F_IRET): mpz_set_si(ret,((int (*)())pfuncs[i].routine)(parser(pos)));return;
	        case (F_DQUICK|F_CRET): mpz_set_d(ret,COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(parser(pos))));return;
	        case (F_DQUICK|F_DRET): mpz_set_d(ret,(pfuncs[i].routine)(parser(pos)));return;
		case (F_CQUICK|F_IRET): mpz_set_si(ret,((int (*)())pfuncs[i].routine)(complex_parser(pos)));return;
		case (F_CQUICK|F_CRET): mpz_set_d(ret,COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(complex_parser(pos))));return;
		case (F_CQUICK|F_DRET): mpz_set_d(ret,(pfuncs[i].routine)(complex_parser(pos)));return;
		case (F_IQUICK|F_IRET): mpz_set_si(ret,((int (*)())pfuncs[i].routine)((int)parser(pos)));return;
		case (F_IQUICK|F_CRET): mpz_set_d(ret,COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)((int)parser(pos))));return;
		case (F_IQUICK|F_DRET): mpz_set_d(ret,(pfuncs[i].routine)((int)parser(pos)));return;
	        }
		if((pfuncs[i].opcode&FM_TYP)==F_SQUICK) {
		  STRING test=string_parser(pos);
	          if((pfuncs[i].opcode&FM_RET)==F_IRET)      mpz_set_si(ret,((int (*)())pfuncs[i].routine)(test));
	          else if((pfuncs[i].opcode&FM_RET)==F_CRET) mpz_set_d (ret,COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(test)));
		  else mpz_set_d(ret,(pfuncs[i].routine)(test));
		  free(test.pointer);
		  return;
		}
	      } else if(pfuncs[i].pmax==2) {
	       	char *w1,*w2;
	        int e=wort_sep_destroy(pos,',',TRUE,&w1,&w2);
	        if(e<2) xberror(56,s); /* Falsche Anzahl Parameter */
	        switch(pfuncs[i].opcode&(FM_TYP|FM_RET)) {
 	        case (F_CQUICK|F_IRET): mpz_set_si(ret,((int (*)())pfuncs[i].routine)(complex_parser(w1),complex_parser(w2)));return;
		case (F_CQUICK|F_CRET): mpz_set_d(ret,COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(complex_parser(w1),complex_parser(w2))));return;
		case (F_CQUICK|F_DRET): mpz_set_d(ret,(pfuncs[i].routine)(complex_parser(w1),complex_parser(w2)));return;
		case (F_DQUICK|F_IRET): mpz_set_si(ret,((int (*)())pfuncs[i].routine)(parser(w1),parser(w2)));return;
		case (F_DQUICK|F_CRET): mpz_set_d(ret,COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)(parser(w1),parser(w2))));return;
		case (F_DQUICK|F_DRET): mpz_set_d(ret,(pfuncs[i].routine)(parser(w1),parser(w2)));return;
		case (F_IQUICK|F_IRET): mpz_set_si(ret,((int (*)())pfuncs[i].routine)((int)parser(w1),(int)parser(w2)));return;
		case (F_IQUICK|F_CRET): mpz_set_d(ret,COMPLEX2FLOAT(((COMPLEX (*)())pfuncs[i].routine)((int)parser(w1),(int)parser(w2))));return;
		case (F_IQUICK|F_DRET): mpz_set_d(ret,(pfuncs[i].routine)((int)parser(w1),(int)parser(w2)));return;
		}
	      }
		/* Es sollten alle Möglichkeiten abgefangen sein.*/
	      printf("ArbIntParser: Interner ERROR. Funktion %s nicht korrekt definiert.\n",s);

	 
	   /* Nicht in der Liste ? Dann kann es noch ARRAY sein   */

          } else if(((stype=type(s))&TYPMASK)==FLOATTYP) {
	 // printf("Parser: stype=$%x, <%s>\n",stype,s);
            if((vnr=var_exist(s,ARRAYTYP,FLOATTYP,0))!=-1) {
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      if(!check_indexliste(a,indexliste)) {
	        xberror(16,""); /* Feldindex zu gross*/
		return;
	      } else {
	        mpz_set_d(ret,float_array_element(a,indexliste));
		return;
              }
	    } else { xberror(15,s); return; } /* Feld nicht dimensioniert  */
          } else if((stype&TYPMASK)==INTTYP) {
	    char *r=varrumpf(s);
	    if((vnr=var_exist(r,ARRAYTYP,INTTYP,0))!=-1) {
	      free(r);
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      if(!check_indexliste(a,indexliste)) {
	        xberror(16,""); /* Feldindex zu gross*/
		return;
	      } else {
	        mpz_set_si(ret,int_array_element(a,indexliste));
		return;
	      }
	    } else { xberror(15,s);free(r); return; }  /* Feld nicht dimensioniert  */
          } else if((stype&TYPMASK)==COMPLEXTYP) {
	    char *r=varrumpf(s);
	    if((vnr=var_exist(r,ARRAYTYP,COMPLEXTYP,0))!=-1) {
	      free(r);
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      if(!check_indexliste(a,indexliste)) {
	        xberror(16,""); /* Feldindex zu gross*/
		return;
	      } else {
	        mpz_set_d(ret,COMPLEX2FLOAT(complex_array_element(a,indexliste)));
		return;
	      }
	    } else { xberror(15,s); free(r); return; }  /* Feld nicht dimensioniert  */
          } else if((stype&TYPMASK)==ARBINTTYP) {
	    char *r=varrumpf(s);
	    if((vnr=var_exist(r,ARRAYTYP,ARBINTTYP,0))!=-1) {
	      free(r);
              ARRAY *a=variablen[vnr].pointer.a;
	      int indexliste[a->dimension];
	      make_indexliste(a->dimension,pos,indexliste);
	      if(!check_indexliste(a,indexliste)) {
	        xberror(16,""); /* Feldindex zu gross*/
		return;
	      } else {
	        arbint_array_element(a,indexliste,ret);
		return;
	      }
	    } else { xberror(15,s); free(r); return; }  /* Feld nicht dimensioniert  */
      } else { xberror(15,s); return; }  /* Feld nicht dimensioniert  */
    }
  } else {  /* Also keine Klammern */
    /* Dann Systemvariablen und einfache Variablen */
    /* Liste durchgehen */
    char c=*s;
    int i=0,a=anzsysvars-1,b,l=strlen(s);
    for(b=0; b<l; c=s[++b]) {
      while(c>(sysvars[i].name)[b] && i<a) i++;
      while(c<(sysvars[a].name)[b] && a>i) a--;
      if(i>=a) break;
    }
    if(strcmp(s,sysvars[i].name)==0) {
       // printf("Sysvar %s gefunden. Nr. %d\n",sysvars[i].name,i);
      if((sysvars[i].opcode&TYPMASK)==INTTYP) {
        mpz_set_si(ret,((int (*)())sysvars[i].routine)());
        return;
      }
      if((sysvars[i].opcode&TYPMASK)==FLOATTYP) {
        mpz_set_d(ret,(sysvars[i].routine)());
        return;
      }
    }

    /* erst integer abfangen (xxx% oder xxx&), dann rest */
    if(*s=='@') {
      do_benfunction(s+1,"");
      p2arbint(&returnvalue,ret);
      return;
    }
    switch(s[l-1]) {
    case '%':
      s[l-1]=0;
      if((vnr=var_exist(s,INTTYP,0,0))!=-1) mpz_set_si(ret,*variablen[vnr].pointer.i);
      return;
    case '#':
      s[l-1]=0;
      if((vnr=var_exist(s,COMPLEXTYP,0,0))!=-1) mpz_set_d(ret,variablen[vnr].pointer.c->r);
      return;
    case '&':
      s[l-1]=0;
      if((vnr=var_exist(s,ARBINTTYP,0,0))!=-1) mpz_set(ret,*variablen[vnr].pointer.ai);
      return;
    default:
      if((vnr=var_exist(s,FLOATTYP,0,0))!=-1) {
        mpz_set_d(ret,*variablen[vnr].pointer.f);
        return;
      }
      arbint_myatof(s,ret); /* Jetzt nur noch Zahlen (hex, oct etc ...)*/ 
      return; 
    } 

  }
  xberror(51,s); /* Syntax error */
}

ARRAY array_parser(const char *funktion) { /* Array-Parser  */
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
  xtrim(funktion,TRUE,s);  /* Leerzeichen vorne und hinten entfernen */
  // printf("ARRAY_PARSER: <%s>\n",s);
  if(*s==0) { 
    xberror(51,funktion);
    e=0;
    return(nullmatrix(FLOATTYP,0,(uint32_t *)&e));
  }
  if(wort_sep(s,'+',3,w1,w2)>1) {
    if(*w1) {
      ARRAY zw1=array_parser(w1);
      ARRAY zw2=array_parser(w2);
      array_add(zw1,zw2);
      free_array(&zw2);
      return(zw1);
    } else return(array_parser(w2));
  } else if(wort_sepr(s,'-',3,w1,w2)>1) {
    if(*w1) {
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
  } else if(wort_sepr(s,'*',3,w1,w2)>1) {
    if(*w1) {
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
  } else if(wort_sepr(s,'^',3,w1,w2)>1) {
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
            if(i>=a) break;
          }
	  if(strcmp(s,pafuncs[i].name)==0) {
	    /*  printf("Funktion %s gefunden. Nr. %d\n",pafuncs[i].name,i); */
	      if((pafuncs[i].opcode&FM_TYP)==F_SIMPLE || pafuncs[i].pmax==0) {
		return((pafuncs[i].routine)());
	      } else if((pafuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
	     	return((pafuncs[i].routine)(pos));
	      } else if((pafuncs[i].opcode&FM_TYP)==F_PLISTE) {
		 PARAMETER *plist;
                 int e=make_pliste(pafuncs[i].pmin,pafuncs[i].pmax,(unsigned short *)pafuncs[i].pliste,pos,&plist);
		 if(e>=0) {
                   ARRAY a=(pafuncs[i].routine)(plist,e);
	           free_pliste(e,plist);
	           return(a);
		 } else {  /* ERROR has occured in parameterlist */
		   e=0;
		   return(nullmatrix(FLOATTYP,0,(uint32_t *)&e));
		 }
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
	   int nn;
	   ARRAY ergeb;

	   wort_sep(pos,',',TRUE,w1,w2);
	   FILEINFO fff=get_fileptr(get_number(w1));
	   
	   
	   nn=(int)parser(w2);
	
           ergeb.typ=INTTYP;
	   ergeb.dimension=1;
	   ergeb.pointer=malloc(INTSIZE+nn*sizeof(int));
	   ((int *)(ergeb.pointer))[0]=nn;
           if(fff.typ) {
	     int *varptr=ergeb.pointer+INTSIZE;
             if((i=fread(varptr,sizeof(int),nn,fff.dptr))<nn) io_error(errno,"fread");
             return(ergeb);
           } else xberror(24,""); /* File nicht geoeffnet */
           return(ergeb);
	 } else {
	    int vnr;
	    char *r=varrumpf(s);
	    ARRAY ergebnis;
	   /* Hier sollten keine Funktionen mehr auftauchen  */
	   /* Jetzt uebergebe spezifiziertes Array, evtl. reduziert*/
	   if(*pos==0) {
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
  	       ergebnis=einheitsmatrix(FLOATTYP,1,(uint32_t *)&e);
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
  return(nullmatrix(FLOATTYP,0,(uint32_t *)&e));
}


char *s_parser(const char *funktion) { /* String-Parser  */
  STRING e=string_parser(funktion);
  return(e.pointer);
}
STRING string_parser(const char *funktion) {
/* Rekursiv und so, dass dynamische Speicherverwaltung ! */
/* Trenne ersten Token ab, und uebergebe rest derselben Routine */

  STRING ergebnis;
  char s[strlen(funktion)+1],*v,*w;

  //printf("S-Parser: <%s>\n",funktion);
  xtrim(funktion,TRUE,s);  /* Leerzeichen vorne und hinten entfernen */
  if(*s==0) {
    xberror(51,funktion); /* "Parser: Syntax error?! " */
    return(create_string(NULL)); /* Leerer Ausdruck (eigentlich syntay error...*/
  }
  if(wort_sep_destroy(s,'+',TRUE,&v,&w)>1) {
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
    
    pos=searchchr(s,'(');
    if(pos!=NULL) {
      pos2=s+strlen(s)-1;
      *pos++=0;
      if(*pos2!=')') {
        xberror(51,s); /* "Parser: Syntax error?! "  */
        ergebnis=vs_error();
      } else {                         /* $-Funktionen und $-Felder   */
        *pos2=0;
	if(*s=='@')     /* Funktion oder Array   */
 	  ergebnis=do_sfunktion(s+1,pos);
        else {  /* Liste durchgehen */
	  int i=0,a=anzpsfuncs-1,b;    

          for(b=0; b<strlen(s); b++) {
            while(s[b]>(psfuncs[i].name)[b] && i<a) i++;
            while(s[b]<(psfuncs[a].name)[b] && a>i) a--;
            if(i>=a) break;
          }
          if(strcmp(s,psfuncs[i].name)==0) {
//          printf("Funktion gefunden <%s>\n",psfuncs[i].name);
	      if((psfuncs[i].opcode&FM_TYP)==F_SIMPLE || psfuncs[i].pmax==0)
	        ergebnis=(psfuncs[i].routine)();
	      else if((psfuncs[i].opcode&FM_TYP)==F_ARGUMENT)
	      	ergebnis=(psfuncs[i].routine)(pos);
	      else if((psfuncs[i].opcode&FM_TYP)==F_PLISTE) {
	        PARAMETER *plist;
                 int e=make_pliste(psfuncs[i].pmin,psfuncs[i].pmax,(unsigned short *)psfuncs[i].pliste,pos,&plist);
		 if(e<0) {
		   xberror(51,s); /* Syntax Error */
		   ergebnis=create_string(NULL);
		 } else {
		   ergebnis=(psfuncs[i].routine)(plist,e);
	           free_pliste(e,plist);
		 }
	      } else if(psfuncs[i].pmax==1 && (psfuncs[i].opcode&FM_TYP)==F_DQUICK) {
		ergebnis=(psfuncs[i].routine)(parser(pos));
	      } else if(psfuncs[i].pmax==1 && (psfuncs[i].opcode&FM_TYP)==F_IQUICK) {
		ergebnis=(psfuncs[i].routine)(*pos?(int)parser(pos):0);
	      } else if(psfuncs[i].pmax==2 && (psfuncs[i].opcode&FM_TYP)==F_DQUICK) {
	       	 char *w1,*w2;
	         int e;
		 double val1=0,val2=0;
	         if((e=wort_sep_destroy(pos,',',TRUE,&w1,&w2))==1) {
		   xberror(56,s); /* Falsche Anzahl Parameter */
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
	      } else printf("Interner ERROR. Funktion nicht korrekt definiert. %s\n",s);
	    } else {/* Nicht in der Liste ? Dann kann es noch ARRAY sein   */
	     int vnr=strlen(s)-1;
	     if(s[vnr]=='$') s[vnr]=0;  /*Entferne dollar zeichen.*/
	
             if((vnr=var_exist(s,ARRAYTYP,STRINGTYP,0))==-1) {
	       xberror(15,s);         /*Feld nicht definiert*/
	       ergebnis=create_string(NULL);
             } else {
	       int dim=variablen[vnr].pointer.a->dimension;
	       int indexliste[dim];
	       
	       if(make_indexliste(dim,pos,indexliste)==0) { 
	          if(check_indexliste(variablen[vnr].pointer.a,indexliste))
		    ergebnis=string_array_element(variablen[vnr].pointer.a,indexliste);
		  else {
		    xberror(16,s); /* Feldindex zu gross*/
		    ergebnis=create_string(NULL);
		  }
	       } else ergebnis=create_string(NULL);
	    }
	  }
        }
      }
    } else {   /* Keine Klammer auf gefunden*/
      if(*s=='@') ergebnis=do_sfunktion(s+1,"");  /* Funktion oder Array   */
      else {
        pos2=s+strlen(s)-1;
        if(*s=='"' && *pos2=='"') {  /* Konstante  */
	  *pos2=0;
          ergebnis.pointer=strdup(s+1);
          ergebnis.len=strlen(ergebnis.pointer);
        } else if(*pos2!='$') {
          xberror(51,s); /* "Parser: Syntax error?! "  */
          ergebnis=vs_error();
        } else {                      /* einfache Variablen und Systemvariablen */
	  /* Liste durchgehen */
	  int i=0,a=anzsyssvars-1,b;
          for(b=0; b<strlen(s); b++) {
            while(s[b]>(syssvars[i].name)[b] && i<a) i++;
            while(s[b]<(syssvars[a].name)[b] && a>i) a--;
            if(i>=a) break;
          }
          if(i==a && strcmp(s,syssvars[i].name)==0) {
	    /*  printf("Sysvar %s gefunden. Nr. %d\n",syssvars[i].name,i);*/
	    return((syssvars[i].routine)());
          }
          *pos2=0;
          if((vnr=var_exist(s,STRINGTYP,0,0))==-1) ergebnis=create_string(NULL);
          else ergebnis=double_string(variablen[vnr].pointer.s);
        }
      }
    }
  }
  return(ergebnis);
}


/* Ruft PROCEDURE oder FUNCTION auf und erstellt lokale Übergabeparameter der
   procedur. und füllt diese mit Ihnalten aus der Parameterliste.
   Returns: 0 everything OK, 
            1 error
	   */

int call_sub_with_parameterlist(int procnr,PARAMETER *plist,int anzpar) {
  if(anzpar!=procs[procnr].anzpar) {
    xberror(56,procs[procnr].name); /* Falsche Anzahl Parameter */
    return 1;
  }
  if(!stack_check(sp)) {
    printf("Stack overflow! PC=%d SP=%d/%d\n",pc,sp,stack_size);
    xberror(39,procs[procnr].name); /* Program Error Gosub impossible */ 
    return 1;
  }
  /* Jetzt Übergabe der Parameter an lokale variablen. Es ist hier essentiell, dass die
     Information, ob ein Parameter by reference übbergeben wird, 
     schon im Parameter enthalten ist.*/
     
  if(anzpar) {
    int vnr;
    int *l=procs[procnr].parameterliste;
    
    // dump_parameterlist(plist,anzpar);
    
    
    for(int i=0;i<anzpar;i++) {
      vnr=(l[i]&(~V_BY_REFERENCE)); /* vnr der lokalen variable, auf die Zugewiesen werden soll.*/
      // printf("par[%d]: Ziel-vnr=%d\n",i,vnr);
      if((l[i]&V_BY_REFERENCE)==V_BY_REFERENCE) {
        /* VARPTR sollte schon in PARAMETER eingetragen sein.*/
        // printf("Wir haben schon folgende adresse: %p\n",plist[i].pointer);
        void *pointer=plist[i].pointer;    /*  Pointer der zugewiesenen var merken */
        if(!pointer) {
	  printf("ERROR: Something is wrong in call_sub_with_parameterlist.\n");
          pointer=variablen[plist[i].integer].pointer.i; /*  mehme pointer der zugewiesenen var */
        }
	if(stack_check(sp)) {
          sp++;
          do_local(vnr,sp);    /* Uebergabeparameter sind lokal ! */
          /*lokale variable wird nun statisch mit referenz zur Uebergabvar*/
          set_var_adr(vnr,pointer);
	  // printf("VAR: Variablen-adr gesetzt auf: adr=0x%p\n",pointer);
          sp--;
        } else {xberror(75,""); /* Stack Overflow! */ return(1);}
      } else {
        if(stack_check(sp)) {
          sp++;
          do_local(vnr,sp);    /* Uebergabeparameter sind lokal ! */
          zuweis_v_parameter(&variablen[vnr],&plist[i]);
          sp--;
	} else {xberror(75,""); /* Stack Overflow! */ return(1);}
      }
    }
  }

  /*Rücksprungzeile auf den Stack und PC auf anfang der Routine setzen.*/
  if(stack_check(sp)) {stack[sp++]=pc;pc=procs[procnr].zeile+1;} 
  else {xberror(75,""); /* Stack Overflow! */ return 1;}
  return(0);
}


 /* loese die Parameterliste auf und weise die Werte auf die neuen lokalen
    Variablen zu. Lege die locals schon an (mit sp+1) */

int do_parameterliste(const char *pos, const int *l,int n) {
  if(n==0 ||!pos ||*pos==0) return(0);
  char *buf=strdup(pos);
  char *w1,*w2;
  int i=0;
  int vnr;
  PARAMETER p;
//  printf("do Parameterliste: <%s>(%d)\n",pos,n);
  int e=wort_sep_destroy(buf,',',TRUE,&w1,&w2);
  p.panzahl=0;
  p.pointer=NULL;
  while(e && i<n) {
  /*  printf("ZU: %s=%s\n",w3,w5); */
    vnr=(l[i]&(~V_BY_REFERENCE));
    if((l[i]&V_BY_REFERENCE)==V_BY_REFERENCE) {
       int typ=variablen[vnr].typ;
       if(typ==ARRAYTYP) typ|=variablen[vnr].pointer.a->typ;
       if(prepare_vvar(w1,&p,(PL_VARGROUP|typ))>=0) {
         void *pointer=variablen[p.integer].pointer.i; /*Pointer merken*/
         if(stack_check(sp)) {
	   sp++;
           do_local(vnr,sp);    /* Variable sichern... ! */
       
           /*lokale variable wird nun statisch mit referenz zur Uebergabvar*/
           set_var_adr(vnr,pointer);
           free_parameter(&p);
           sp--;
	 } else xberror(75,""); /* Stack Overflow! */
       }
    } else {
      if(stack_check(sp)) {
        sp++;
        do_local(vnr,sp);    /* Uebergabeparameter sind lokal ! */
        zuweisxbyindex(vnr,NULL,0,w1,variablen[vnr].typ); /*Todo: Ausdruck kann andere Typen enthalten !*/
        sp--;
      } else xberror(75,""); /* Stack Overflow! */
    }
    i++;
    e=wort_sep_destroy(w2,',',TRUE,&w1,&w2);
  }
  free(buf);
  if(e || i<n) xberror(56,pos); /* Falsche Anzahl Parameter */
  return((i!=n) ? 1 : 0);
}



static void do_benfunction(const char *name,const char *argumente) {
//  printf("Do benutzerdef function: %s  <-- %s\n",name,argumente);
  int oldbatch,osp=sp;
  int pc2=procnr(name,PROC_FUNC|PROC_DEFFN);  /*FUNCTION und DEFFN*/
  if(pc2==-1) { xberror(44,name); /* Funktion  nicht definiert */ return; } 
  if(do_parameterliste(argumente,procs[pc2].parameterliste,procs[pc2].anzpar)) {
    restore_locals(sp+1);
    xberror(42,name); /* Zu wenig Parameter */
    return;
  }
  int proctyp=procs[pc2].typ;
  int typ=vartype(name);
  pc2=procs[pc2].zeile;
  if(stack_check(sp)) {stack[sp++]=pc;pc=pc2+1;}
  else {xberror(75,""); /* Stack Overflow! */restore_locals(sp+1);return;}
  if(proctyp==PROC_DEFFN) {
    PARAMETER localpar;  /* Localer parameter, da die Parseraufrufe returnvalue wiederum veraendern.*/
    bzero(&localpar,sizeof(PARAMETER));
    localpar.typ=typ;
    switch(typ) {
    case INTTYP:   localpar.integer=(int)parser(pcode[pc2].argument); break;
    case FLOATTYP: localpar.real=parser(pcode[pc2].argument);         break;
    case COMPLEXTYP:
      *(COMPLEX *)&localpar.real=complex_parser(pcode[pc2].argument);
      break;
    case ARBINTTYP:
      localpar.pointer=malloc(sizeof(ARBINT));
      mpz_init(*((ARBINT *)localpar.pointer));
      arbint_parser(pcode[pc2].argument,*((ARBINT *)localpar.pointer));
      break;
    case STRINGTYP:
      *((STRING *)&localpar.integer)=string_parser(pcode[pc2].argument);
      break;
    case ARRAYTYP:
    case (ARRAYTYP|INTTYP):
    case (ARRAYTYP|FLOATTYP):
    case (ARRAYTYP|COMPLEXTYP):
    case (ARRAYTYP|ARBINTTYP):
    case (ARRAYTYP|STRINGTYP):
    /*  TODO */
    default: printf("ERROR: function type not determined.\n");
    }
    restore_locals(sp);
    pc=stack[--sp];
    free_parameter(&returnvalue);
    returnvalue=localpar;
  } else {
    oldbatch=batch;batch=1;
    programmlauf();
    batch=min(oldbatch,batch);
    /* restore_locals(sp); wird im normalfall von RETURN gemacht:  Lokale parameter wieder freigeben ! */
    if(osp!=sp) { /* Hier wurde die Funktion offenbar nicht durch ein RETURN beendet.*/
      /*Funktion wird vorzeitig verlassen. Ein CONT ist nicht moeglich. lokale vars sind nicht mehr debugbar.*/
      restore_locals(sp);
      pc=stack[--sp]; /* wenn error innerhalb der func. */
      printf("Error within FUNCTION. pc=%d, sp=%d, FATAL!\n",pc,sp);
    }
  }
}

static COMPLEX do_cfunktion(char *name,char *argumente) {
  do_benfunction(name,argumente);
  return(p2complex(&returnvalue));
}
static double do_funktion(char *name,char *argumente) {
  do_benfunction(name,argumente);
  return(p2float(&returnvalue));
}
static STRING do_sfunktion(char *name,char *argumente) {
  do_benfunction(name,argumente);
  return(p2string(&returnvalue));
}
