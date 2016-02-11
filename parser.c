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
#include <unistd.h>
#include <math.h>
#include <errno.h>

#include "defs.h"
#include "x11basic.h"
#include "xbasic.h"
#include "variablen.h"
#include "parameter.h"
#include "array.h"
#include "number.h"
#include "io.h"
#include "wort_sep.h"
#include "parser.h"


static STRING do_sfunktion(char *name,char *argumente);
static double do_funktion(char *name,char *argumente);


static int vergleich(char *w1,char *w2) {
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



double parser(const char *funktion){  /* Rekursiver num. Parser */
  char *pos,*pos2;
  char s[strlen(funktion)+1],w1buf[strlen(funktion)+1],w2buf[strlen(funktion)+1];
  int vnr;
  char *w1=w1buf;
  char *w2=w2buf;

  // printf("Parser: <%s>\n",funktion);

  /* Logische Operatoren AND OR NOT ... */

  if(searchchr2_multi(funktion,"&|")!=NULL) {
    if(wort_sepr2(funktion,"&&",TRUE,w1,w2)>1)     return((double)((int)parser(w1) & (int)parser(w2)));
    if(wort_sepr2(funktion,"||",TRUE,w1,w2)>1)     return((double)((int)parser(w1) | (int)parser(w2)));
  }
  xtrim(funktion,TRUE,s);  /* Leerzeichen vorne und hinten entfernen, Grossbuchstaben */

if(searchchr2(s,' ')!=NULL) {
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
                 int e=make_pliste(pfuncs[i].pmin,pfuncs[i].pmax,(unsigned short *)pfuncs[i].pliste,pos,&plist);
                 double a;
		 if(e>=pfuncs[i].pmin) {
                   if(pfuncs[i].opcode&F_IRET) a=(double)((int (*)())pfuncs[i].routine)(plist,e);
	           else a=(pfuncs[i].routine)(plist,e);
		 } else a=0;
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
            if(i>=a) break;
          }
          if(strcmp(s,sysvars[i].name)==0) {
	     // printf("Sysvar %s gefunden. Nr. %d\n",sysvars[i].name,i);
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
      } else {
        return(myatof(s));  /* Jetzt nur noch Zahlen (hex, oct etc ...)*/
      }
    }
  }
  xberror(51,s); /* Syntax error */
  return(0);
}


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
	   int nn;
	   ARRAY ergeb;

	   wort_sep(pos,',',TRUE,w1,w2);
	   FILE *fff=get_fileptr(get_number(w1));
	   
	   
	   nn=(int)parser(w2);
	
           ergeb.typ=INTTYP;
	   ergeb.dimension=1;
	   ergeb.pointer=malloc(INTSIZE+nn*sizeof(int));
	   ((int *)(ergeb.pointer))[0]=nn;
           if(fff) {
	     int *varptr=ergeb.pointer+INTSIZE;
             if((i=fread(varptr,sizeof(int),nn,fff))<nn) io_error(errno,"fread");
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


char *s_parser(const char *funktion) { /* String-Parser  */
  STRING e=string_parser(funktion);
  return(e.pointer);
}
STRING string_parser(const char *funktion) {
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
            if(i>=a) break;
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
                 int e=make_pliste(psfuncs[i].pmin,psfuncs[i].pmax,(unsigned short *)psfuncs[i].pliste,pos,&plist);
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
	int i=0,a=anzsyssvars-1,b;
        for(b=0; b<strlen(v); b++) {
          while(v[b]>(syssvars[i].name)[b] && i<a) i++;
          while(v[b]<(syssvars[a].name)[b] && a>i) a--;
          if(i>=a) break;
        }
        if(i==a && strcmp(v,syssvars[i].name)==0) {
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



static double do_funktion(char *name,char *argumente) {
  int oldbatch,osp=sp;
  int pc2=procnr(name,PROC_FUNC|PROC_DEFFN);  /*FUNCTION und DEFFN*/
  int typ;
  if(pc2==-1) {
    xberror(44,name); /* Funktion  nicht definiert */
    return(0.0);
  } 
  typ=procs[pc2].typ;
  
  if(do_parameterliste(argumente,procs[pc2].parameterliste,procs[pc2].anzpar)) {
    restore_locals(sp+1);
    xberror(42,name); /* Zu wenig Parameter */
    return(0.0);
  }
  pc2=procs[pc2].zeile;

  if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
  else {printf("Stack-Overflow ! PC=%d\n",pc); restore_locals(sp+1);batch=0;return(0.0);}

  if(typ==PROC_DEFFN) {
    returnvalue.f=parser(pcode[pc2].argument);
    restore_locals(sp);
    pc=stack[--sp];
  } else {
    oldbatch=batch;batch=1;
    programmlauf();
    batch=min(oldbatch,batch);
    /* restore_locals(sp); wird im normalfall von RETURN gemacht:  Lokale parameter wieder freigeben ! */
    if(osp!=sp) {  /* Hier wurde die Funktion offenbar nicht durch ein RETURN beendet.*/
      /*Funktion wird vorzeitig verlassen. Ein CONT ist nicht moeglich. lokale vars sind nicht mehr debugbar.*/
      restore_locals(sp);
      pc=stack[--sp]; /* wenn error innerhalb der func. Meistens ist es bei Abbruch (CTRL-c oder error.*/
     
      printf("Error within FUNCTION. pc=%d, sp=%d, FATAL!\n",pc,sp);
    }
  }
  return(returnvalue.f);
}

 /* loese die Parameterliste auf und weise die Werte auf die neuen lokalen
    Variablen zu. Lege die locals schon an (mit sp+1) */

int do_parameterliste(char *pos, int *l,int n) {
  char buf[strlen(pos)+1];
  char *w1,*w2;
  int e;
  int i=0;
  strcpy(buf,pos);
  e=wort_sep_destroy(buf,',',TRUE,&w1,&w2);

  while(e && i<n) {
  /*  printf("ZU: %s=%s\n",w3,w5); */
    do_local(l[i],sp+1);    /* Uebergabeparameter sind lokal ! */
    zuweisxbyindex(l[i++],NULL,0,w1);
    e=wort_sep_destroy(w2,',',TRUE,&w1,&w2);
  }
  if(e || i<n) xberror(56,pos); /* Falsche Anzahl Parameter */
  return((i!=n) ? 1 : 0);
}


static STRING do_sfunktion(char *name,char *argumente) {
  int oldbatch,osp=sp;
  int pc2=procnr(name,PROC_FUNC|PROC_DEFFN);  /*FUNCTION und DEFFN*/
  int typ;
  if(pc2==-1) { xberror(44,name); /* Funktion  nicht definiert */ return(create_string(NULL)); } 
  if(do_parameterliste(argumente,procs[pc2].parameterliste,procs[pc2].anzpar)) {
    restore_locals(sp+1);
    xberror(42,name); /* Zu wenig Parameter */
    return(create_string(NULL));
  }
  typ=procs[pc2].typ;
  pc2=procs[pc2].zeile;
  if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
  else {printf("Stack-Overflow ! PC=%d\n",pc); batch=0;restore_locals(sp+1);return(create_string(NULL));}

  if(typ==PROC_DEFFN) {
    returnvalue.str=string_parser(pcode[pc2].argument);
    restore_locals(sp);
    pc=stack[--sp];
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
      returnvalue.str=create_string(NULL);
    }
  }
  return(returnvalue.str);
}
