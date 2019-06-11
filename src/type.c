/* type.c   Typbestimmung von X11-Basic ausdrücken. (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "number.h"
#include "xbasic.h"
#include "type.h"
#include "parser.h"
#include "parameter.h"
#include "wort_sep.h"



static int find_sysvar(const char *s) {
  /* Liste durchgehen */
  char c=*s;
  int i=0,a=anzsysvars-1,b,l=strlen(s);
  for(b=0; b<l; c=s[++b]) {
            while(c>(sysvars[i].name)[b] && i<a) i++;
            while(c<(sysvars[a].name)[b] && a>i) a--;
            if(i==a) break;
  }
  if(strcmp(s,sysvars[i].name)==0) return(i);
  return(-1);
}




/*Bestimmt den Ergebnistyp einer Binaeren Operation, gegeben durch das
Zeichen c.*/

unsigned int combine_type(unsigned int rtyp,unsigned int ltyp,char c) {
  int typ=0;
  /*könnte man mit einer lookup-Tabelle loesen...*/
    const static unsigned char combtyp[8*8]={
    NOTYP,NOTYP,        NOTYP,         NOTYP,        NOTYP,        NOTYP,        NOTYP,        NOTYP,  /*NOTYP*/
    NOTYP,INTTYP,       FLOATTYP,      ARBINTTYP,    ARBFLOATTYP,  COMPLEXTYP,   ARBCOMPLEXTYP,NOTYP,  /*INTTYP*/
    NOTYP,FLOATTYP,     FLOATTYP,      ARBINTTYP,    ARBFLOATTYP,  COMPLEXTYP,   ARBCOMPLEXTYP,NOTYP,  /*FLOATTYP*/
    NOTYP,ARBINTTYP,    ARBFLOATTYP,   ARBINTTYP,    ARBFLOATTYP,  ARBCOMPLEXTYP,ARBCOMPLEXTYP,NOTYP,  /*ARBINTTYP*/
    NOTYP,ARBFLOATTYP,  ARBFLOATTYP,   ARBFLOATTYP,  ARBFLOATTYP,  ARBCOMPLEXTYP,ARBCOMPLEXTYP,NOTYP,  /*ARBFLOATTYP*/
    NOTYP,COMPLEXTYP,   COMPLEXTYP,    ARBCOMPLEXTYP,ARBCOMPLEXTYP,COMPLEXTYP,   ARBCOMPLEXTYP,NOTYP,  /*COMPLEXTYP*/
    NOTYP,ARBCOMPLEXTYP,ARBCOMPLEXTYP, ARBCOMPLEXTYP,ARBCOMPLEXTYP,ARBCOMPLEXTYP,ARBCOMPLEXTYP,NOTYP,  /*ARBCOMPLEXTYP*/
    NOTYP,NOTYP,        NOTYP,         NOTYP,        NOTYP,        NOTYP,        NOTYP,        STRINGTYP /*STRINGTYP*/
    };
 //  printf("combine-type %x %x %c \n",rtyp,ltyp,c);
  if((ltyp&CONSTTYP) && (rtyp&CONSTTYP)) typ|=CONSTTYP;
  if((ltyp&ARRAYTYP) || (rtyp&ARRAYTYP)) typ|=ARRAYTYP;
  rtyp&=TYPMASK;
  ltyp&=TYPMASK;
  
  if(c=='/') { /*War division, dann kann das Ergebnis nur float,complex oder arbfloat,arbcomplex sein */
    if(rtyp==INTTYP) rtyp=FLOATTYP;
    else if(rtyp==ARBINTTYP) rtyp=ARBFLOATTYP;
    else if(ltyp==INTTYP) ltyp=FLOATTYP;
    else if(ltyp==ARBINTTYP) ltyp=ARBFLOATTYP;
    typ|=combtyp[8*rtyp+ltyp];
  } 
  else if(c=='m') { /* MOD operator */
    int a=combtyp[8*rtyp+ltyp];
    if(rtyp==ARBINTTYP) a=ltyp;
    typ|=a;
  }
  else if(c=='d') { /* DIV operator Ganzzahl, also nur INT oder ARBINT*/
    int a=combtyp[8*rtyp+ltyp];
    if(rtyp==ARBINTTYP) a=ltyp;
    if(a==FLOATTYP || a==COMPLEXTYP) a=INTTYP;
    if(a==ARBFLOATTYP || a==ARBCOMPLEXTYP) a=ARBINTTYP;
    typ|=a;
  }
  else if(c=='=' || c=='<' || c=='>') typ|=INTTYP; /*War Vergleichsoperator, dann kann das Ergebnis nur int sein */
  else if(c=='&') {/*War logic operator, dann kann das Ergebnis nur int oder arbint sein */
    if(rtyp==ARBINTTYP || ltyp==ARBINTTYP) typ|=ARBINTTYP;
    else typ|=INTTYP;
  } else if(c==',') typ|=rtyp;   /* Hm... */
  else  typ|=combtyp[8*rtyp+ltyp];  /*   + - * etc... */
  
  if((typ&TYPMASK)==NOTYP && (typ&ARRAYTYP)!=ARRAYTYP) {
    xberror(51,"combine-type");  /* Parser: Syntax Error */
    printf("WARNING: combine-typ <%s>--<%s>--'%c'--> <%s>\n",type_name(ltyp),type_name(rtyp),c,type_name(typ));
  }
  return(typ);
}


/* Bestimmt best passendsten einzeltyp aus eine mit Komma oder Semikolon 
   separierten Liste:
   Ist ein Typ eine float zahl, dann ist alles Float
   Ist ein Typ ein String, dann ist alles String
   Ist ein Typ ein Array, dann ist alles Array.
   
   
   */
unsigned int type_list(const char *ausdruck) {
  int e,f,i=0;
  unsigned int typ=CONSTTYP;
  unsigned int temptyp;
  char t[strlen(ausdruck)+1];
  char t2[strlen(ausdruck)+1];
  char t3[strlen(ausdruck)+1];
 // printf("Type-List: <%s>\n",ausdruck);
 // printf("Type() ergibt: %x\n",type(ausdruck));
  e=wort_sep(ausdruck,';',-1,t,t2);
  while(e) {
    f=wort_sep(t,',',-1,t3,t);
    while(f) {
      temptyp=type(t3);
      if(temptyp&ARRAYTYP) typ=(typ|ARRAYTYP);
      if(!(temptyp&CONSTTYP)) typ=(typ&(~CONSTTYP));
      temptyp&=TYPMASK;
      /* Macht aus NOTYP -> INTTYP --> FLOATTYP usw...*/
      if(i) typ=(typ&(~TYPMASK))|combine_type(typ&TYPMASK,temptyp,'+');
      else typ=(typ&(~TYPMASK))|temptyp;
      i++;
      f=wort_sep(t,',',-1,t3,t);
    }
    e=wort_sep(t2,';',TRUE,t,t2);
  }
  // printf("Ergebnis: %x\n",typ);  
  return(typ);
}





/* Bestimmt den Typ eines Ausdrucks, 
   es darf sich nicht um eine Liste von Ausdr"ucken handeln.

*/

/* Typ von Ausdruecken */

char *type_name(unsigned int typ) {
  static int offset=0;
  static char buffer[128];
  if(offset>80) offset=0;
  char *nam=&buffer[offset];
  if(typ==NOTYP) strcpy(nam,"notyp ");
  else {
    *nam=0;
    if(typ&INDIRECTTYP) strcat(nam,"indirect ");
    if(typ&FILENRTYP) strcat(nam,"filenr ");
    if(typ&CONSTTYP) strcat(nam,"const ");
    if(typ&ARRAYTYP) strcat(nam,"array ");

    switch(typ&TYPMASK) {
    case INTTYP:      strcat(nam,"int ");    break;
    case FLOATTYP:    strcat(nam,"float ");  break;
    case COMPLEXTYP:  strcat(nam,"complex ");break;
    case ARBINTTYP:   strcat(nam,"arbint "); break;
    case ARBFLOATTYP: strcat(nam,"arbflt "); break;
    case STRINGTYP:   strcat(nam,"string "); break;
    default:          sprintf(nam+strlen(nam),"unknown[%x] ",typ);
    }
  }
  offset+=strlen(nam)+1;
  return(nam);
}
//#define TYPEDEBUG
#ifdef TYPEDEBUG

static void dump_type(unsigned int typ) {
  printf(type_name(typ));
}
unsigned int type_(const char *ausdruck);
unsigned int type(const char *ausdruck) {
//#if 0
  int i;
  static int level=0;
  static int count=0;
  level++;
  unsigned int a=type_(ausdruck);
  level--;
  printf("%4d: ",count);
  for(i=0;i<level*2;i++) putchar(' ');
  printf("TYPE <%s> --> %x ",ausdruck,a);
  dump_type(a);
  printf("\n");
  // if(count==59) *((char *)count)=1;
  count++;
  return(a);
}
unsigned int type_(const char *ausdruck) {
#else 
unsigned int type(const char *ausdruck) {
#endif
  if(!ausdruck) return(NOTYP);  
  while (w_space(*ausdruck)) ausdruck++;  /* Skip leading white space, if any. */

  // printf("TYPE: <%s> \n",ausdruck);
  switch(*ausdruck) {
  case 0: return(NOTYP);
  case '+':
  case '-': return(type(ausdruck+1));  /* war Vorzeichen */
  case '&':
    if(ausdruck[1]=='"') return(INDIRECTTYP|CONSTTYP); 
    else return(INDIRECTTYP);
  case '[': {
    char s[strlen(ausdruck)+1];
    xtrim(ausdruck,TRUE,s);  /* Leerzeichen vorne und hinten entfernen, Grossbuchstaben */
    char *pos=searchchr2(s+1,']');
    if(pos!=NULL) *pos=0;
    return(ARRAYTYP|type(s+1));  
    }
  }
  char s[strlen(ausdruck)+1];
  char *pos;
  
  xtrim(ausdruck,TRUE,s);  /* Leerzeichen vorne und hinten entfernen, Grossbuchstaben */
  if((pos=searchchr3(s,';'))!=NULL) { /* Erst der Semikolon-Operator */
    *pos++=0;
    return(combine_type(type(pos),type(s),';'));
  }  
  
  if((pos=searchchr3(s,','))!=NULL) {  /* Dann der Komma-Operator */
    *pos++=0;
    return(combine_type(type(pos),type(s),','));
  }  

/* jetzt = < > */

  pos=searchchr2_multi(s,"=<>");
  
  if(pos!=NULL) {
    char a=*pos;
    *pos++=0;
    while(*pos=='=' ||*pos=='<' ||*pos=='>') pos++; /* faengt die operatoen <>,==,<=,>= ab.*/
    return(combine_type(type(pos),type(s),a));
  }

  if((pos=searchchr3(s,'|'))!=NULL) {  /* Dann || operatoren */
    if(*pos=='|' && pos[1]=='|') {
      *pos++=0;
      return(combine_type(type(++pos),type(s),'&'));
    }
  }  

  char *w1,*w2;
/*  jetzt OR NOR XOR | */
  if(wort_sepr2_destroy(s," OR ",TRUE,&w1,&w2)>1)  return(combine_type(type(w2),type(w1),'&'));
  if(wort_sepr2_destroy(s," NOR ",TRUE,&w1,&w2)>1)  return(combine_type(type(w2),type(w1),'&'));
  if(wort_sepr2_destroy(s," XOR ",TRUE,&w1,&w2)>1)  return(combine_type(type(w2),type(w1),'&'));


  if((pos=searchchr3(s,'&'))!=NULL) {  /* Dann &&  operatoren */
    if(*pos=='&' && pos[1]=='&') {
      *pos++=0;
      return(combine_type(type(++pos),type(s),'&'));
    }
  }  
  
  if(wort_sepr2_destroy(s," AND ",TRUE,&w1,&w2)>1)  return(combine_type(type(w2),type(w1),'&'));
  if(wort_sepr2_destroy(s," NAND ",TRUE,&w1,&w2)>1)  return(combine_type(type(w2),type(w1),'&'));
  if(wort_sepr2_destroy(s," EQR ",TRUE,&w1,&w2)>1)  return(combine_type(type(w2),type(w1),'&'));
  if(wort_sepr2_destroy(s," EQV ",TRUE,&w1,&w2)>1)  return(combine_type(type(w2),type(w1),'&'));
  if(wort_sepr2_destroy(s," IMP ",TRUE,&w1,&w2)>1)  return(combine_type(type(w2),type(w1),'&'));
  if(wort_sepr2_destroy(s," MOD ",TRUE,&w1,&w2)>1)  return(combine_type(type(w2),type(w1),'m'));
  if(wort_sepr2_destroy(s," DIV ",TRUE,&w1,&w2)>1)  return(combine_type(type(w2),type(w1),'d'));
  if(wort_sepr2_destroy(s,"NOT ",TRUE,&w1,&w2)>1) {
      if(*w1==0) return(type(w2));    /* von rechts !!  */
      /* Ansonsten ist NOT Teil eines Variablennamens */
  }
  
    /* Addition/Subtraktion/Vorzeichen  */

    /* Suche eine g"ultige Trennstelle f"ur + oder -  */
  /* Hier muessen wir testen, ob es nicht ein vorzeichen war oder Teil eines Exponenten ...*/

  pos=searchchr2_multi_r(s,"+-",s+strlen(s));  /* Finde n"achsten Kandidaten  von rechts*/
  while(pos!=NULL) {
    if(pos==s) return(type(s+1));/*Das +/-  war ganz am Anfang*/
    if(pos>s && strchr("*/^",*(pos-1))==NULL && 
                                     !( *(pos-1)=='E' && pos-1>s && vfdigittest(s,pos-1) && v_digit(*(pos+1)))) {
    /* Kandidat war gut.*/
      char c=*pos;
      *pos=0;
      return(combine_type(type(pos+1),type(s),c));
    } 
    pos=searchchr2_multi_r(s,"+-",pos);  /* Finde n"achsten Kandidaten  von rechts*/
  }

  /* File-Type */
  if(*s=='#') return(FILENRTYP|type(s+1));
  
  /* Restliche Trennzeichen= ''', ' ', "* /^()|<>=&!" */

  pos=searchchr2_multi(s,"'*/^");
  
  if(pos!=NULL) {
    char a=*pos;
    *pos++=0;
    return(combine_type(type(pos),type(s),a));
  }

  
  /*  hier ist nun nurnoch ein operand zu untersuchen */

  if(*s=='(' && s[strlen(s)-1]==')') {
    s[strlen(s)-1]=0;
    return(type(s+1));
  }
  int typ=0;
  int i=0;
  pos=searchchr(s,'(');
  if(pos!=NULL) {       /* Erste Klammer finden*/
    if(*s!='@') {
      if(pos==s) printf("WARNING: Syntax-error in expression: parenthesis? <%s>\n",s);
      /* jetzt entscheiden, ob Array-element oder sub-array oder Funktion */
      char *ppp=pos+1;
      int i=0,flag=0,sflag=0,count=0;
      *pos=0;
      if((i=find_func(s))!=-1) { /* Koennte funktion sein: */
 	if(ppp[strlen(ppp)-1]==')') ppp[strlen(ppp)-1]=0;
	else printf("WARNING: Syntay-error in expression: closing parenthesis?. <%s>\n",ppp);
        typ=returntype2(i,ppp);
	if(pfuncs[i].opcode&F_CONST) {
          if(type(ppp)&CONSTTYP) typ|=CONSTTYP;
        }
	return(typ);
      } else if((i=find_sfunc(s))!=-1) { /* Koennte $-funktion sein: */
        typ=STRINGTYP;
	if(psfuncs[i].opcode&F_CONST) {
	  if(ppp[strlen(ppp)-1]==')') ppp[strlen(ppp)-1]=0;
	  else printf("WARNING: Syntay-error in expression: closing parenthesis?. <%s>\n",ppp);
          if(type(ppp)&CONSTTYP) typ|=CONSTTYP;
        }
	return(typ);
      } else if((i=find_afunc(s))!=-1) { /* Koennte Array-funktion sein: */
        typ=ARRAYTYP;
	// printf("Array-Funktion %s gefunden.\n",s);
	if(psfuncs[i].opcode&F_CONST) {
	  if(ppp[strlen(ppp)-1]==')') ppp[strlen(ppp)-1]=0;
	  else printf("WARNING: Syntay-error in expression: closing parenthesis?. <%s>\n",ppp);
          if(type(ppp)&CONSTTYP) typ|=CONSTTYP;
	}
        return(typ);
      } else {                   /* wird wohl Array sein.*/
        if(pos[1]==')') typ=(typ | ARRAYTYP);
        else {
          while(ppp[i]!=0 && !(ppp[i]==')' && flag==0 && sflag==0)) { /*Fine doppelpunkte in Argumentliste*/
            if(ppp[i]=='(') flag++;
	    if(ppp[i]==')') flag--;
	    if(ppp[i]=='"') sflag=(!sflag);
	    if(ppp[i]==':' && flag==0 && sflag==0) count++;
            i++;
          }
          if(count) typ=(typ | ARRAYTYP);
        }
      }
    } /* Kann auch @-Funktionsergebnis sein */
    if((pos-1)[0]=='$') typ=(typ | STRINGTYP);
    else if((pos-1)[0]=='%') typ=(typ | INTTYP);
    else if((pos-1)[0]=='&') typ=(typ | ARBINTTYP);
    else if((pos-1)[0]=='#') typ=(typ | COMPLEXTYP);
    else typ=(typ | FLOATTYP);
    return(typ);
  } 
  /* Keine Klammer*/
  switch(*s) {
  case 0:   typ=NOTYP; break;
  case '"': typ=CONSTTYP|STRINGTYP;break;
  case '$':				  /*HEX Zahl*/
  case '%': typ=INTTYP|CONSTTYP;break;    /*BIN Zahl*/
  default:
    if((i=find_sysvar(s))!=-1) {  /*Es koennte sysvar sein. Dann testen, ob ggf constant.*/
          typ=sysvars[i].opcode;
    } else { /* Normale Variablen und Konstanten*/
      int l=strlen(s);
      if(s[l-1]=='$') typ=STRINGTYP;
      else if(s[l-1]=='%') typ=INTTYP;
      else if(s[l-1]=='&') typ=ARBINTTYP;
      else if(s[l-1]=='#') typ=COMPLEXTYP;
      else { /*Unterscheide nun noch zwischen ZahlenKonstanten und float variablen*/
        int i=myisatof(s);
//	printf("myisatof <%s> --> %x\n",s,i);
        if(i==INTTYP) typ=INTTYP|CONSTTYP;
	else if(i==FLOATTYP) typ=FLOATTYP|CONSTTYP;
	else if(i==COMPLEXTYP) typ=COMPLEXTYP|CONSTTYP;
	else if(i==ARBINTTYP) typ=ARBINTTYP|CONSTTYP;
	else typ=FLOATTYP;
      }
   //  printf("type---> <%s> = %x\n",s,typ);
    }
  }
  return(typ);
}


/*  Gibt returntyp einer Funktion zurück undter beachtung des Typs der
    Eingangsparameter (Nur der erste PL_NUMBER oder PL_ANYVALUE ist relevant)
    */
int returntype(int idx, PARAMETER *p,int n) {
  int  g=(pfuncs[idx].opcode&FM_RET);
  switch(g) {
  case F_IRET: return(INTTYP);
  case F_DRET: return(FLOATTYP);
  case F_CRET: return(COMPLEXTYP);
  case F_AIRET: return(ARBINTTYP);
  case F_AFRET: return(ARBFLOATTYP);
  case F_ACRET: return(ARBCOMPLEXTYP);
  case F_SRET: return(STRINGTYP);
  case F_ANYIRET:
  case F_NRET:
  case F_ANYRET:
    if(!p ||n==0 || pfuncs[idx].pmax==0) return(NOTYP);
    
  /* Jetzt eingangstypen der Funktion bis n durchgehen, ersten 
     PL_NUMBER oder PL_ANYVALUE(?) finden, danach den Returntyp
     bestimmen. */
  
    if(n==1 && (pfuncs[idx].opcode&FM_TYP)==F_ARGUMENT && (p->typ==PL_KEY || p->typ==PL_EVAL)) {
      int typ;
      if(p->typ==PL_EVAL && p->arraytyp) typ=(p->arraytyp&TYPMASK);
      else typ=(type(p->pointer)&TYPMASK);
      switch(typ) {
        case INTTYP:
        case ARBINTTYP: return(typ);
        case FLOATTYP:
        case COMPLEXTYP:
        case ARBFLOATTYP:
        case ARBCOMPLEXTYP:
          if(g==F_NRET || g==F_ANYRET) return(typ);
          else if(g==F_ANYIRET) return(ARBINTTYP);
          break;
        case STRINGTYP:
          if(g==F_ANYRET) return(typ);
          break;
      }
      printf("ERROR: something is wrong! returntype\n");
      return(typ);
    }
      int i,typ=NOTYP,typ2=NOTYP;
      for(i=0;i<n;i++) {
        if((pfuncs[idx].pmax>-1) && i>pfuncs[idx].pmax) break;
        if((pfuncs[idx].pmax==-1) && i>pfuncs[idx].pmin) break;
      /* Erstmal die Parameterlistendef durchschauen....*/
        switch(pfuncs[idx].pliste[i]) {
        case PL_INT:     typ2=INTTYP; break;
        case PL_FLOAT:   typ2=FLOATTYP; break;
        case PL_COMPLEX: typ2=COMPLEXTYP; break;
        case PL_ARBINT:  typ2=ARBINTTYP; break;
	case PL_STRING:  typ2=STRINGTYP; break;
	/* Jetzt die Mehdeutigen*/
	case PL_CF:
	  typ2=p[i].typ&PL_BASEMASK;
	  if(typ2==INTTYP) typ2=FLOATTYP;
	  else if(typ2==ARBINTTYP) typ2=FLOATTYP;
	  break;
	case PL_CFAI:
	  typ2=p[i].typ&PL_BASEMASK;
	  if(typ2==INTTYP) typ2=FLOATTYP;
	  break;
        case PL_NUMBER:
	case PL_ANYVALUE:
	  typ2=p[i].typ&PL_BASEMASK;
          break;
        case PL_ARRAY:
        case PL_IARRAY:
        case PL_FARRAY:
        case PL_CARRAY:
        case PL_AIARRAY:
	case PL_NARRAY:
	case PL_CFARRAY:
	case PL_SARRAY:
	  return(p[i].typ&PL_BASEMASK&TYPMASK);/* Ausstieg mit Abkürzung...*/
	default: 
	 printf("ERROR: something is wrong! unerkannter Typ. %x\n",pfuncs[idx].pliste[i]);
      }
      if(i==0) typ=typ2;
      else typ=combine_type(typ,typ2,'+');
    }
    return(typ);
  default: printf("ERROR: something is wrong! x324\n");
  }
  return(NOTYP);
}

/*Liefert zur"uck: den Typ des Returnwertes einer (numerischen) Funktion anhand
  von der Funktionsdefinition und ggf. auch von den Typen der für die Funktion vorgesehenen und auch der 
  tatsächlich gegebenen Parameter.
  
  Dieser wird nach folgenden Regeln bestimmt: 
  1. Ist dieser explizit angegeben, nimm diesen
  2. Ist es ANYIRET, ANYRET oder NRET:
     Bestimme den Typ aus der Argumentliste durch kombinieren mit '+'
     und zwar: 
     a) Ist irgendwo ein ARRAY, nimm den Typ des ersten Arrays (kommt nicht vor!)
     b) PL_ANYVALUE und PL_NUMBER nimmt den jeweiligen Typ des Arguments
     c) PL_CFAI oder PL_CF: liefere FLOAT statt INT 
     d) PL_CF: nimm auch FLOAT statt ARBINT
      
*/
int returntype2(int idx, char *n) {
  int  g=(pfuncs[idx].opcode&FM_RET);  /* Anhand des explizit gegebenen Returntyps*/
  switch(g) {
  case F_IRET: return(INTTYP);
  case F_DRET: return(FLOATTYP);
  case F_CRET: return(COMPLEXTYP);
  case F_AIRET: return(ARBINTTYP);
  case F_AFRET: return(ARBFLOATTYP);
  case F_ACRET: return(ARBCOMPLEXTYP);
  case F_SRET: return(STRINGTYP);
  case F_ANYIRET:
  case F_NRET:
  case F_ANYRET: { /* Hier die möglichen Parameter heranziehen bzw die Argumente.... */
    if(!n ||*n==0 || pfuncs[idx].pmax==0) return(NOTYP);
    char w1[strlen(n)+1],w2[strlen(n)+1];
    int e=wort_sep(n,',',TRUE,w1,w2);
    int i=0;
    int typ=NOTYP,typ2=NOTYP;
    // printf("returntype2: n=<%s> -->",n);
    while(e) {
      if((pfuncs[idx].pmax>-1) && i>pfuncs[idx].pmax) break;
      if((pfuncs[idx].pmax==-1) && i>pfuncs[idx].pmin) break;
      /* Erstmal die Parameterlistendef durchschauen....*/
      switch(pfuncs[idx].pliste[i]) {
        case PL_INT:     typ2=INTTYP; break;
        case PL_FLOAT:   typ2=FLOATTYP; break;
        case PL_COMPLEX: typ2=COMPLEXTYP; break;
        case PL_ARBINT:  typ2=ARBINTTYP; break;
	case PL_STRING:  typ2=STRINGTYP; break;
	/* Jetzt die Mehdeutigen*/
	case PL_CF:
	  typ2=type(w1)&(~CONSTTYP);
	  if(typ2==INTTYP) typ2=FLOATTYP;
	  else if(typ2==ARBINTTYP) typ2=FLOATTYP;
	  break;
	case PL_CFAI:
	  typ2=type(w1)&(~CONSTTYP);
	  if(typ2==INTTYP) typ2=FLOATTYP;
	  break;
        case PL_NUMBER:
	case PL_ANYVALUE:
	  typ2=type(w1)&(~CONSTTYP);
          break;
        case PL_IARRAY:  return(INTTYP);
        case PL_FARRAY:  return(FLOATTYP);
        case PL_CARRAY:  return(COMPLEXTYP);
        case PL_AIARRAY: return(ARBINTTYP);

        case PL_ARRAY:	
	case PL_NARRAY:
	case PL_CFARRAY:
	  typ2=type(w1)&(~CONSTTYP);
	  return(typ2&TYPMASK);          /* Ausstieg mit Abkürzung...*/
	default: 
	 printf("ERROR: something is wrong! unerkannter Typ. %x\n",pfuncs[idx].pliste[i]);
      }
      if(i==0) typ=typ2;
      else typ=combine_type(typ,typ2,'+');
      i++;
      e=wort_sep(w2,',',TRUE,w1,w2);
    }
    // printf("%x\n",typ);
    return(typ);    
    }
  default: printf("ERROR: something is wrong! x324-2 %s\n",pfuncs[idx].name);
  }
  return(NOTYP);
}

