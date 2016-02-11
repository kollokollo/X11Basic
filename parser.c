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

#include "xbasic.h"

int vergleich(char *w1,char *w2) {
  int e,v;
  e=type2(w1);
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
    if(x==0) v=0;
    else if(x<0) v=-1;
    else v=1;
  }
  /*printf("Vergleich: %d\n",v);*/
  return(v);
}

double parser(char *funktion){
  char *s,*w1,*w2,*pos,*pos2;
  double ergebnis=0.0;
  int e,vnr;
#ifdef DEBUG
  static int depth;
#endif
/* Rekursiver Parser */
  s=malloc(strlen(funktion)+1);w1=malloc(strlen(funktion)+1);w2=malloc(strlen(funktion)+1);
  strcpy(s,funktion);
  xtrim(s,TRUE,s);  /* Leerzeichen vorne und hinten entfernen */
#ifdef DEBUG
  printf("%d:parser: %s \n",depth,funktion);
  depth++;
#endif
  /* Dann Logische Operatoren AND OR NOT ... */ 
       if(wort_sepr2(s," AND ",TRUE,w1,w2)>1)  ergebnis=(double)((int)parser(w1) & (int)parser(w2));    /* von rechts !!  */
  else if(wort_sepr2(s,"&&",TRUE,w1,w2)>1)     ergebnis=(double)((int)parser(w1) & (int)parser(w2));    
  else if(wort_sepr2(s," OR ",TRUE,w1,w2)>1)   ergebnis=(double)((int)parser(w1) | (int)parser(w2));    
  else if(wort_sepr2(s,"||",TRUE,w1,w2)>1)     ergebnis=(double)((int)parser(w1) | (int)parser(w2));    
  else if(wort_sepr2(s,"||",TRUE,w1,w2)>1)     ergebnis=(double)((int)parser(w1) | (int)parser(w2));    
  else if(wort_sepr2(s," NAND ",TRUE,w1,w2)>1) ergebnis=(double)~((int)parser(w1) & (int)parser(w2));    
  else if(wort_sepr2(s," NOR ",TRUE,w1,w2)>1)  ergebnis=(double)~((int)parser(w1) | (int)parser(w2));    
  else if(wort_sepr2(s," XOR ",TRUE,w1,w2)>1)  ergebnis=(double)((int)parser(w1) ^ (int)parser(w2));    
  else if(wort_sepr2(s," EOR ",TRUE,w1,w2)>1)  ergebnis=(double)((int)parser(w1) ^ (int)parser(w2));    
  else if(wort_sepr2(s," EQV ",TRUE,w1,w2)>1)  ergebnis=(double)~((int)parser(w1) ^ (int)parser(w2));    
  else if(wort_sepr2(s," IMP ",TRUE,w1,w2)>1)  ergebnis=(double)(~((int)parser(w1) ^ (int)parser(w2)) | (int)parser(w2));    
  else if(wort_sepr2(s," MOD ",TRUE,w1,w2)>1)  ergebnis=fmod(parser(w1),parser(w2));    
  else if(wort_sepr2(s," DIV ",TRUE,w1,w2)>1) { 
    int nenner=(int)parser(w2);
    if(nenner) ergebnis=(double)((int)parser(w1) / nenner);    
    else error(0,w2); /* Division durch 0 */
 } else if(wort_sepr2(s,"NOT ",TRUE,w1,w2)>1) {        
    if(strlen(w1)==0) {  
      ergebnis=(double)(~(int)parser(w2));    /* von rechts !!  */
    } /* Ansonsten ist NOT Teil eines Variablennamens */
    
  } else if(wort_sep2(s,"==",TRUE,w1,w2)>1) { /* Erst Vergleichsoperatoren mit Wahrheitwert abfangen (lowlevel < Addition)  */
    if(vergleich(w1,w2)==0) ergebnis=-1;
  } else if(wort_sep2(s,"<>",TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)!=0) ergebnis=-1;
  } else if(wort_sep2(s,"><",TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)!=0) ergebnis=-1; 
  } else if(wort_sep2(s,"<=",TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)<=0) ergebnis=-1; 
  } else if(wort_sep2(s,">=",TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)>=0) ergebnis=-1; 
  } else if(wort_sep(s,'=',TRUE,w1,w2)>1) {
    if(vergleich(w1,w2)==0) ergebnis=-1;
  } else if(wort_sep(s,'<',TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)<0) ergebnis=-1;
  } else if(wort_sep(s,'>',TRUE,w1,w2)>1) { 
    if(vergleich(w1,w2)>0) ergebnis=-1;
  } else if(wort_sep(s,'+',TRUE,w1,w2)>1) {           /* Addition/Subtraktion/Vorzeichen  */
    if(strlen(w1)) ergebnis=parser(w1)+parser(w2);
    else ergebnis=parser(w2);   /* war Vorzeichen + */
  } else if(wort_sepr(s,'-',TRUE,w1,w2)>1) {           
    if(strlen(w1)) ergebnis=parser(w1)-parser(w2);    /* von rechts !!  */
    else ergebnis=-parser(w2);   /* war Vorzeichen - */
  } else if(wort_sepr(s,'*',TRUE,w1,w2)>1) {           
    if(strlen(w1)) ergebnis=parser(w1)*parser(w2);    
    else printf("Pointer noch nicht integriert! %s\n",w2);   /* war pointer - */
  } else if(wort_sepr(s,'/',TRUE,w1,w2)>1) {        
    if(strlen(w1)) {
      double nenner;
      nenner=parser(w2); 
      if(nenner!=0.0) ergebnis=parser(w1)/nenner;    /* von rechts !!  */
      else error(0,w2); /* Division durch 0 */
    } else error(51,w2); /* "Parser: Syntax error?! "  */
  } else if(wort_sepr(s,'^',TRUE,w1,w2)>1) {           
    if(strlen(w1)) ergebnis=pow(parser(w1),parser(w2));    /* von rechts !!  */
    else error(51,w2); /* "Parser: Syntax error?! "  */
  
  } else if(s[0]=='(' && s[strlen(s)-1]==')')  { /* Ueberfluessige Klammern entfernen */
    s[strlen(s)-1]=0;
    ergebnis=parser(s+1);
    
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
       
        if(strcmp(s,"ABS")==0)        ergebnis=fabs(parser(pos));
	else if(strcmp(s,"ASIN")==0)  ergebnis=asin(parser(pos));
	else if(strcmp(s,"INT")==0)   ergebnis=(double)(int)parser(pos);
	else if(strcmp(s,"PRED")==0)   ergebnis=ceil(parser(pos)-1);
	else if(strcmp(s,"SUCC")==0)   ergebnis=(double)(int)(parser(pos)+1);
	else if(strcmp(s,"FRAC")==0)  {double d=parser(pos);ergebnis=d-((double)((int)d));}
        else if(strcmp(s,"SGN")==0)   ergebnis=sgn(parser(pos));
        else if(strcmp(s,"ACOS")==0)  ergebnis=acos(parser(pos));
        else if(strcmp(s,"ATAN")==0)  ergebnis=atan(parser(pos));
        else if(strcmp(s,"CEIL")==0)  ergebnis=ceil(parser(pos));
        else if(strcmp(s,"EXP")==0)   ergebnis=exp(parser(pos));
        else if(strcmp(s,"FAK")==0) { 
	  int i,s=1,k;
	  k=(int)parser(pos);
	  for(i=2;i<=k;i++) {s=s*i;} 
	  ergebnis=s;
	}
  	else if(strcmp(s,"FLOOR")==0) ergebnis=floor(parser(pos));
  	else if(strcmp(s,"LN")==0)    ergebnis=log(parser(pos));
  	else if(strcmp(s,"LOG")==0)   ergebnis=log(parser(pos));
  	else if(strcmp(s,"LOG10")==0) ergebnis=log10(parser(pos));
  	else if(strcmp(s,"RAND")==0)  ergebnis=(double)rand();
  	else if(strcmp(s,"RANDOM")==0)ergebnis=(int)((double)rand()/RAND_MAX*parser(pos));
  	else if(strcmp(s,"RND")==0)   ergebnis=(double)rand()/RAND_MAX;
  	else if(strcmp(s,"SRAND")==0) {srand((int)parser(pos));ergebnis=0;}
  	else if(strcmp(s,"SIN")==0)   ergebnis=sin(parser(pos));
  	else if(strcmp(s,"SINH")==0)  ergebnis=sinh(parser(pos));
  	else if(strcmp(s,"COS")==0)   ergebnis=cos(parser(pos));
  	else if(strcmp(s,"COSH")==0)  ergebnis=cosh(parser(pos));
  	else if(strcmp(s,"TAN")==0)   ergebnis=tan(parser(pos));
  	else if(strcmp(s,"TANH")==0)  ergebnis=tanh(parser(pos));
  	else if(strcmp(s,"DEG")==0)   ergebnis=(parser(pos)/2/PI*360);
  	else if(strcmp(s,"RAD")==0)   ergebnis=(parser(pos)/360*2*PI);
  	else if(strcmp(s,"SQR")==0)   ergebnis=sqrt(parser(pos));
  	else if(strcmp(s,"SQRT")==0)  ergebnis=sqrt(parser(pos));   

        else if(strcmp(s,"LEN")==0)  {
	  char *test;
	  test=s_parser(pos);
	  ergebnis=(double)strlen(test);
	  free(test);  
        }else if(strcmp(s,"ASC")==0)  {
	  char *test;
	  test=s_parser(pos);
	  ergebnis=(double)test[0];
	  free(test); 
	}else if(strcmp(s,"DIM?")==0)  {
	   /* printf("DIM?: <%s>\n",pos); */
	    ergebnis=(double)do_dimension(variable_exist_type(pos));
	}else if(strcmp(s,"TYP?")==0)  ergebnis=(double)type2(pos);
	else if(strcmp(s,"LTEXTLEN")==0)  {
	  char *test;
	  test=s_parser(pos);
	  ergebnis=ltextlen(ltextxfaktor,ltextpflg,test);
	  free(test);   
        }else if(strcmp(s,"VAL")==0)  {
	  char *test;
	  test=s_parser(pos);
	  ergebnis=atof(test);
	  free(test);   
        }else if(strcmp(s,"EXIST")==0)  {
	  char *test;
	  test=s_parser(pos);
	  ergebnis=-(double)exist(test);
	  free(test);   
        }else if(strcmp(s,"LOF")==0)  {
	  int i=get_number(pos);
	  if(filenr[i]) ergebnis=(double)lof(dptr[i]);
	  else error(24,""); /* File nicht geoeffnet */
        }else if(strcmp(s,"LOC")==0)  {
	  int i=get_number(pos);
	  if(filenr[i]) ergebnis=(double)ftell(dptr[i]);
	  else error(24,""); /* File nicht geoeffnet */
	}else if(strcmp(s,"EOF")==0)  {
	  int i=get_number(pos);
	  if(i==-2) ergebnis=(double)((eof(stdin)) ? -1 : 0);
	  else if(filenr[i]) ergebnis=(double)((eof(dptr[i])) ? -1 : 0);
	  else error(24,""); /* File nicht geoeffnet */	
	}else if(strcmp(s,"INP")==0)  ergebnis=(double)inp8(pos);
        else if(strcmp(s,"INP?")==0)  ergebnis=(double)inpf(pos);
	else if(strcmp(s,"INP&")==0)  ergebnis=(double)inp16(pos);
	else if(strcmp(s,"INP%")==0)  ergebnis=(double)inp32(pos);
	else if(strcmp(s,"WORT_SEP")==0)  ergebnis=(double)do_wort_sep(pos);

	else if(strcmp(s,"MIN")==0)  {
	  char *w1,*w2;
	  int e;
	  w1=malloc(strlen(pos)+1);w2=malloc(strlen(pos)+1);
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=parser(w1);
	  else if(e==2)	  ergebnis=min(parser(w1),parser(w2));
	  free(w1);free(w2);   
        
	}else if(strcmp(s,"MAX")==0)  {
	  char *w1,*w2;
	  int e;
	  w1=malloc(strlen(pos)+1);w2=malloc(strlen(pos)+1);
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=parser(w1);
	  else if(e==2)	  ergebnis=max(parser(w1),parser(w2));
	  free(w1);free(w2); 
	}else if(strcmp(s,"FORM_ALERT")==0)  {
	  char *w1,*w2;
	  int e;
	  w1=malloc(strlen(pos)+1);w2=malloc(strlen(pos)+1);
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=0;
	  else if(e==2)	 {
	    char *ttt;
	    ttt=s_parser(w2);
	    ergebnis=form_alert((int)parser(w1),ttt);
	    free(ttt);
	  }
	  free(w1);free(w2); 
	}else if(strcmp(s,"POINT")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  e=wort_sep(pos,',',TRUE,w1,w2);
	  ergebnis=get_point((int)parser(w1),(int)parser(w2));
	}else if(strcmp(s,"ADD")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=parser(w1);
	  else if(e==2)	  ergebnis=parser(w1)+parser(w2);
	}else if(strcmp(s,"SUB")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=parser(w1);
	  else if(e==2)	  ergebnis=parser(w1)-parser(w2);
	 }else if(strcmp(s,"MUL")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=parser(w1);
	  else if(e==2)	  ergebnis=parser(w1)*parser(w2);
	 }else if(strcmp(s,"DIV")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=parser(w1);
	  else if(e==2)	  ergebnis=parser(w1)/parser(w2);
	 }else if(strcmp(s,"MOD")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=parser(w1);
	  else if(e==2)	  ergebnis=fmod(parser(w1),parser(w2));
	   
	 }else if(strcmp(s,"ROUND")==0)  {
	  char *w1,*w2;
	  int e;
	  w1=malloc(strlen(pos)+1);w2=malloc(strlen(pos)+1);
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=round(parser(w1));
	  else if(e==2)	{
	    int kommast=-(int)parser(w2);
	    ergebnis=round(parser(w1)/pow(10,kommast))*pow(10,kommast); 
	  }
	  free(w1);free(w2); 
	}else if(strcmp(s,"BCLR")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=parser(w1);
	  else if(e==2)	  ergebnis=(double)((int)parser(w1) & ~ (1 <<((int)parser(w2))));
	}else if(strcmp(s,"BSET")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=parser(w1);
	  else if(e==2)	  ergebnis=(double)((int)parser(w1) | (1 <<((int)parser(w2))));
	}else if(strcmp(s,"BCHG")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=parser(w1);
	  else if(e==2)	  ergebnis=(double)((int)parser(w1) ^ (1 <<((int)parser(w2))));
	}else if(strcmp(s,"BTST")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int e;
	  if((e=wort_sep(pos,',',TRUE,w1,w2))==1) ergebnis=parser(w1);
	  else if(e==2)	  ergebnis=(double) (((int)parser(w1) & (1 <<((int)parser(w2))))==0) ?
	  0 : -1;
	}else if(strcmp(s,"GET_COLOR")==0)  {
	  char w1[strlen(pos)+1],w2[strlen(pos)+1],w3[strlen(pos)+1];
	 
	  wort_sep(pos,',',TRUE,w1,w2);
	  wort_sep(w2,',',TRUE,w2,w3);

	  ergebnis=(double)get_color((int)parser(w1),(int)parser(w2),(int)parser(w3));
	}else if(strcmp(s,"ARRPTR")==0)  ergebnis=(double)arrptr(pos);
	else if(strcmp(s,"VARPTR")==0)  ergebnis=(double)varptr(pos);
	else if(strcmp(s,"MALLOC")==0)  ergebnis=(double)(int)malloc((int)parser(pos));
	else if(strcmp(s,"FREE")==0)  {free((char *)((int)parser(pos)));ergebnis=(double)errno;}
	else if(strcmp(s,"PEEK")==0)  ergebnis=(double)(int)((char *)((int)parser(pos)))[0];
	else if(strcmp(s,"LPEEK")==0)  ergebnis=(double)(int)((int *)((int)parser(pos)))[0];
	else if(strcmp(s,"DPEEK")==0)  ergebnis=(double)(int)((short *)((int)parser(pos)))[0];  
        else if(strcmp(s,"CSPID")==0)  {
	  char *test;
	  test=s_parser(pos);
	  ergebnis=(double)cspid(test);
	  free(test);  
        }else if(strcmp(s,"CSGET")==0)  {
	  char *test;
	  test=s_parser(pos);
	  ergebnis=csget(test);
	  free(test);  
        }else if(strcmp(s,"CSSIZE")==0)  {
	  char *test;
	  test=s_parser(pos);
	  ergebnis=(double)cssize(test);
	  free(test);  
        }else if(strcmp(s,"CSRES")==0)  {
	  char *test;
	  test=s_parser(pos);
	  ergebnis=csres(test);
	  free(test);  
        }else if(strcmp(s,"CSMIN")==0)  {
	  char *test;
	  test=s_parser(pos);
	  ergebnis=csmin(test);
	  free(test);  
        }else if(strcmp(s,"CSMAX")==0)  {
	  char *test;
	  test=s_parser(pos);
	  ergebnis=csmax(test);
	  free(test);  
        } else if(s[0]=='@') ergebnis=do_funktion(s+1,pos);	  
        else if(type2(s) & FLOATTYP) {
          if((vnr=variable_exist(s,FLOATARRAYTYP))!=-1) ergebnis=floatarrayinhalt(vnr,pos);
        } else if(type2(s) & INTTYP) {
	  char *r=varrumpf(s);
	  if((vnr=variable_exist(r,INTARRAYTYP))!=-1) ergebnis=(double)intarrayinhalt(vnr,pos);
	  free(r);
	}
	else error(15,s);  /* Feld nicht dimensioniert  */
      }
    } else {
      /* Dann Systemvariablen und einfache Variablen */
      /* erst integer abfangen (xxx% oder xxx&), dann rest */
      if(strcmp(s,"STIMER")==0) { /* Sekunden-Timer */
        time_t timec;
        timec = time(&timec);
        ergebnis=(double)timec;
      } else if(strcmp(s,"CTIMER")==0) {
	ergebnis=(double)clock()/CLOCKS_PER_SEC;
      } else if(strcmp(s,"TIMER")==0) {
        struct timespec t;
	struct {
               int  tz_minuteswest; /* minutes W of Greenwich */
               int  tz_dsttime;     /* type of dst correction */
       } tz;
	gettimeofday(&t,&tz);
	ergebnis=(double)t.tv_sec+(double)t.tv_nsec/1000000;      
      } else if(strcmp(s,"ERR")==0)    ergebnis=(double)err;
      else if(strcmp(s,"CCSERR")==0)   ergebnis=(double)ccs_err;
      else if(strcmp(s,"CCSAPLID")==0) ergebnis=(double)aplid;
      else if(strcmp(s,"PC")==0)       ergebnis=(double)pc;
      else if(strcmp(s,"SP")==0)       ergebnis=(double)sp;
      else if(strcmp(s,"PI")==0)       ergebnis=PI;
      else if(strcmp(s,"TRUE")==0)     ergebnis=-1;
      else if(strcmp(s,"FALSE")==0)    ergebnis=0;
      else if(strcmp(s,"MOUSEX")==0)   ergebnis=(double)mousex();
      else if(strcmp(s,"MOUSEY")==0)   ergebnis=(double)mousey();
      else if(strcmp(s,"MOUSEK")==0)   ergebnis=(double)mousek();
      else if(strcmp(s,"MOUSES")==0)   ergebnis=(double)mouses();
      else if(s[0]=='@')               ergebnis=do_funktion(s+1,"");
      else if((vnr=variable_exist(s,FLOATTYP))!=-1) ergebnis=variablen[vnr].zahl;
      else ergebnis=atof(s);  /* Jetzt nur noch Zahlen (hex, oct etc ...)*/
    }
  }
  free(s);free(w1);free(w2);
#ifdef DEBUG
  depth--;
#endif
  return(ergebnis);
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

 char *t,*u,*v,*w,*ergebnis,*pos,*pos2,*inhalt;
 int e,vnr;

 v=malloc(strlen(funktion)+1);w=malloc(strlen(funktion)+1);
 
 e=wort_sep(funktion,'+',TRUE,v,w);
 if(e==2) {
   t=s_parser(v);
   u=s_parser(w);
   ergebnis=malloc(strlen(t)+strlen(u)+1);
   strcpy(ergebnis,t);strcat(ergebnis,u);
   free(t);free(u); 
 } else {
   /*printf("parser: <%s>\n",funktion);*/
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
	  else j=min(max((int)parser(w2),0),strlen(ergebnis));
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
	    strcpy(ergebnis,buffer+j-1);
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
         ergebnis=malloc(11);
	 timec = time(&timec);
	 loctim=localtime(&timec);
	 sprintf(ergebnis,"%2d.%2d.%4d",loctim->tm_mday,loctim->tm_mon+1,1900+loctim->tm_year);
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
 free(v);free(w);
 return(ergebnis);
}


char *float_to_string(char *pos) {

          /* STR$(a[,b[,c[,d]]])     */
          char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  char formatter[20];
	  int j,e,b=-1,c=13,mode=0,i=0;
	  double a;
	  char *ergebnis=malloc(50);
	  
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
	  
	  
	  if(mode==0 && b!=-1) sprintf(formatter,"%%%d.%dg",b,c);
	  else if (mode==1 && b!=-1) sprintf(formatter,"%%0%d.%dg",b,c);
	  else  sprintf(formatter,"%%.13g");
          sprintf(ergebnis,formatter,a);
	  return(ergebnis);
}
char *hexoct_to_string(char n,char *pos) {

          /* STR$(a[,b[,c[,d]]])     */
          char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  char formatter[20];
	  int j,e,b=-1,c=13,mode=0,i=0;
	  unsigned int a;
	  char *ergebnis=malloc(50);
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
    ergebnis=malloc(1);
    ergebnis[0]=0;
    return(ergebnis);  
 }
