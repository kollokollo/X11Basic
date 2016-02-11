/* BYTECODE.C   The X11-basic bytecode compiler         (c) Markus Hoffmann
*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#ifdef WINDOWS
#define EX_OK 0
#else
#include <sysexits.h>
#endif

#include "config.h"
#include "defs.h"
#include "globals.h"
#include "protos.h"
#include "functions.h"
#include "ptypes.h"
#include "bytecode.h"



extern STRING bcpc;
int *relocation;
int anzreloc=0;
const int relocation_offset=sizeof(BYTECODE_HEADER);
int *bc_index;
extern int verbose;

extern int donops;
extern int docomments;

extern int databufferlen;
extern char *databuffer;

/* X11-Basic needs these declar<ations:  */


extern void memdump(unsigned char *,int);

int find_comm(char *); 

#ifdef WINDOWS
#include <windows.h>
HINSTANCE hInstance;
#endif

/* Bestimmt den Typ eines Ausdrucks */

int type3(char *ausdruck) {
  char w1[strlen(ausdruck)+1],w2[strlen(ausdruck)+1];
  char *pos;
  int typ=0;

  wort_sep(ausdruck,'+',TRUE,w1,w2);
  if(strlen(w1)==0) return(type3(w2));  /* war Vorzeichen */
  if(w1[0]=='[') {
    pos=searchchr(w1+1,']');
    if(pos!=NULL) *pos=0;
    pos=searchchr(w1+1,',');
    if(pos!=NULL) *pos=0;
    pos=searchchr(w1+1,' ');
    if(pos!=NULL) *pos=0;
    return(ARRAYTYP|CONSTTYP|type3(w1+1));
  }
  if(w1[0]=='"') return(STRINGTYP|CONSTTYP);
  if(w1[0]=='&') return(INDIRECTTYP);
  if(w1[0]=='(' && w1[strlen(w1)-1]==')') {
    char ttt[strlen(ausdruck)+1];
    strcpy(ttt,ausdruck);
    ttt[strlen(ttt)-1]=0;
    return(type3(ttt+1));
  }
  pos=searchchr(w1+1,'(');
  if(pos!=NULL) {
    if(pos[1]==')') typ=(typ | ARRAYTYP);
    else {   /* jetzt entscheiden, ob Array-element oder sub-array oder Funktion */
      char *ppp=pos+1;
      int i=0,flag=0,sflag=0,count=0;
      while(ppp[i]!=0 && !(ppp[i]==')' && flag==0 && sflag==0)) {
        if(ppp[i]=='(') flag++;
	if(ppp[i]==')') flag--;
	if(ppp[i]=='"') sflag=(!sflag);
	
	if(ppp[i]==':' && flag==0 && sflag==0) count++;
	
        i++;
      }
      if(count) typ=(typ | ARRAYTYP);
    }
    if((pos-1)[0]=='$') typ=(typ | STRINGTYP);
    else if((pos-1)[0]=='%') typ=(typ | INTTYP);
    else typ=(typ | FLOATTYP);
    return(typ);
  } else {
    if(w1[strlen(w1)-1]=='$') return(STRINGTYP);
    else if(w1[strlen(w1)-1]=='%') return(INTTYP);
    else if(w1[0]=='0' && w1[1]=='X') return(INTTYP|CONSTTYP);
    else {
      int i,f=0,g=0;
      pos=searchchr(w1+1,' ');
      if(pos!=NULL) *pos=0;
      if(w1[0]==0) return(NOTYP);
      for(i=0;i<strlen(w1);i++) f|=(strchr("-.1234567890E",w1[i])==NULL);      
      for(i=0;i<strlen(w1);i++) g|=(strchr("1234567890",w1[i])==NULL);
      if(!f && !g) return(INTTYP|CONSTTYP);
      return(FLOATTYP|(f?0:CONSTTYP));
    }  
  }
}

void bc_push_integer(int i) {
      if(i==0) bcpc.pointer[bcpc.len++]=BC_PUSH0;
      else if(i==1) bcpc.pointer[bcpc.len++]=BC_PUSH1;
      else if(i==2) bcpc.pointer[bcpc.len++]=BC_PUSH2;
      else if(i==-1) bcpc.pointer[bcpc.len++]=BC_PUSHM1;
      else if(i<128 && i>0) {
        bcpc.pointer[bcpc.len++]=BC_PUSHB;
        bcpc.pointer[bcpc.len++]=i;
      } else {
        bcpc.pointer[bcpc.len++]=BC_PUSHI;
        memcpy(&bcpc.pointer[bcpc.len],&i,sizeof(int));
        bcpc.len+=sizeof(int);
      }
}
int bc_parser(char *funktion){  /* Rekursiver Parser */
  char *pos,*pos2;
  char s[strlen(funktion)+1],w1[strlen(funktion)+1],w2[strlen(funktion)+1];
  int a,b,i,e,l,vnr;

  static int bcerror;

  /* printf("Bytecode Parser: <%s>\n");*/

  /* Erst der Komma-Operator */
  
  if(searchchr2(funktion,',')!=NULL){
    if(wort_sep(funktion,',',TRUE,w1,w2)>1) {
      bc_parser(w1);bc_parser(w2);return;
    }
  }
  /* Logische Operatoren AND OR NOT ... */
  xtrim(funktion,TRUE,s);  /* Leerzeichen vorne und hinten entfernen, Grossbuchstaben */
  if(strlen(s)==0) {bcpc.pointer[bcpc.len++]=BC_PUSHLEER;return(bcerror);}
  if(searchchr2_multi(funktion,"&|")!=NULL) {
    if(wort_sepr2(s,"&&",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_AND;return(bcerror);}
    if(wort_sepr2(s,"||",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_OR;return(bcerror);}    
  }

  if(searchchr2(funktion,' ')!=NULL) {
    if(wort_sepr2(s," AND ",TRUE,w1,w2)>1)  {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_AND;return(bcerror);}    /* von rechts !!  */
    if(wort_sepr2(s," OR ",TRUE,w1,w2)>1)   {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_OR;return(bcerror);}
    if(wort_sepr2(s," NAND ",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_AND;bcpc.pointer[bcpc.len++]=BC_NOT;return(bcerror);}
    if(wort_sepr2(s," NOR ",TRUE,w1,w2)>1)  {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_OR;bcpc.pointer[bcpc.len++]=BC_NOT;return(bcerror);}
    if(wort_sepr2(s," XOR ",TRUE,w1,w2)>1)  {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_XOR;return(bcerror);}
    if(wort_sepr2(s," EOR ",TRUE,w1,w2)>1)  {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_XOR;return(bcerror);}  
    if(wort_sepr2(s," EQV ",TRUE,w1,w2)>1)  {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_XOR;bcpc.pointer[bcpc.len++]=BC_NOT;return(bcerror);}
    if(wort_sepr2(s," IMP ",TRUE,w1,w2)>1)  {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_XOR;bcpc.pointer[bcpc.len++]=BC_NOT;bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_OR;return(bcerror);}
    if(wort_sepr2(s," MOD ",TRUE,w1,w2)>1)  {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_MOD;return(bcerror);}
    if(wort_sepr2(s," DIV ",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_DIV;return(bcerror);}
    if(wort_sepr2(s,"NOT ",TRUE,w1,w2)>1) {
      if(strlen(w1)==0) {bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_NOT;return;}    /* von rechts !!  */
      /* Ansonsten ist NOT Teil eines Variablennamens */
    }
  }

  /* Erst Vergleichsoperatoren mit Wahrheitwert abfangen (lowlevel < Addition)  */
  if(searchchr2_multi(s,"<=>")!=NULL) {
    if(wort_sep2(s,"==",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_EQUAL;return(bcerror);}
    if(wort_sep2(s,"<>",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_EQUAL;bcpc.pointer[bcpc.len++]=BC_NOT;return(bcerror);}
    if(wort_sep2(s,"><",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_EQUAL;bcpc.pointer[bcpc.len++]=BC_NOT;return(bcerror);}
    if(wort_sep2(s,"<=",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_GREATER;bcpc.pointer[bcpc.len++]=BC_NOT;return(bcerror);}
    if(wort_sep2(s,">=",TRUE,w1,w2)>1) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_LESS;bcpc.pointer[bcpc.len++]=BC_NOT;return(bcerror);}
    if(wort_sep(s,'=',TRUE,w1,w2)>1)   {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_EQUAL;return(bcerror);}
    if(wort_sep(s,'<',TRUE,w1,w2)>1)   {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_LESS;return(bcerror);}
    if(wort_sep(s,'>',TRUE,w1,w2)>1)   {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_GREATER;return(bcerror);}
  }
  /* Addition/Subtraktion/Vorzeichen  */
  if(searchchr2_multi(s,"+-")!=NULL) {
    if(wort_sep_e(s,'+',TRUE,w1,w2)>1) {
      if(strlen(w1)) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_ADD;return(bcerror);}
      else {bc_parser(w2);return(bcerror);}   /* war Vorzeichen + */
    }
    if(wort_sepr_e(s,'-',TRUE,w1,w2)>1) {       /* von rechts !!  */
      if(strlen(w1)) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_SUB;return(bcerror);}
      else {bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_NEG;return(bcerror);}   /* war Vorzeichen - */
    }
  }
  if(searchchr2_multi(s,"*/^")!=NULL) {
    if(wort_sepr(s,'*',TRUE,w1,w2)>1) {
      if(strlen(w1)) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_MUL;return(bcerror);}
      else {
        printf("Pointer noch nicht integriert! %s\n",w2);   /* war pointer - */
        return(bcerror);
      }
    }
    if(wort_sepr(s,'/',TRUE,w1,w2)>1) {
      if(strlen(w1)) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_DIV;return(bcerror);}
      else { xberror(51,w2); return(bcerror); }/* "Parser: Syntax error?! "  */
    }
    if(wort_sepr(s,'^',TRUE,w1,w2)>1) {
      if(strlen(w1)) {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_POW;return(bcerror);}    /* von rechts !!  */
      else { xberror(51,w2); return(bcerror); } /* "Parser: Syntax error?! "  */
    }
  }
  if(*s=='(' && s[strlen(s)-1]==')')  { /* Ueberfluessige Klammern entfernen */
    s[strlen(s)-1]=0;
    bc_parser(s+1);
    return(bcerror);
  } 
  /* SystemFunktionen Subroutinen und Arrays */
  pos=searchchr(s, '(');
  if(pos!=NULL) {
    pos2=s+strlen(s)-1;
    *pos++=0;
    if(*pos2!=')') {
      printf("Closing bracket is missing!\n");
      xberror(51,w2); /* "Parser: Syntax error?! "  */
      return(bcerror);
    }
    /* $-Funktionen und $-Felder   */
    *pos2=0;
    /* Benutzerdef. Funktionen mit parameter*/
    if(*s=='@') {
      int pc2;
      int anzpar=0;

      /* Anzahl Parameter ermitteln */
      anzpar=count_parameter(pos);
      printf("Funktionsaufruf <%s>(%s)\n",s+1,pos);

      /* Bei Benutzerdef Funktionen muessen wir den Parametertyp erraten.*/

      if(anzpar) bc_parser(pos);  /* Parameter auf den Stack */

      bc_push_integer(anzpar);
      /* Funktionsnr finden */
      pc2=procnr(s+1,2);
      if(pc2==-1) { 
        xberror(44,s+1); /* Funktion  nicht definiert */
        return(bcerror);
      }
      printf("Funktion procnr=%d, anzpar=%d\n",pc2,anzpar);
      bc_jumptosr2(procs[pc2].zeile);
      /*Der Rueckgabewert sollte nach rueckkehr auf dem Stack liegen.*/
      return(bcerror);
    }
    if((i=find_func(s))!=-1) {
      /* printf("Funktion %s gefunden. Nr. %d\n",pfuncs[i].name,i); */
      /* Jetzt erst die Parameter auf den Stack packen */
      
      if((pfuncs[i].opcode&FM_TYP)==F_SIMPLE || pfuncs[i].pmax==0) {
        bcpc.pointer[bcpc.len++]=BC_PUSHFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=0;
        return(bcerror);
      } else if((pfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(pos);
        memcpy(&bcpc.pointer[bcpc.len],pos,strlen(pos));
	bcpc.len+=strlen(pos);
        bcpc.pointer[bcpc.len++]=BC_PUSHFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=1;
        return(bcerror);
      } else if((pfuncs[i].opcode&FM_TYP)==F_PLISTE) {                
	int anz;
	anz=count_parameter(pos);
	if((pfuncs[i].pmin>anz && pfuncs[i].pmin!=-1) || 
	   (pfuncs[i].pmax<anz && pfuncs[i].pmax!=-1))  
	     printf("Warnung: Funktion %d: Falsche Anzahl Parameter (%d).\n",i,anz); /*Programmstruktur fehlerhaft */
	if(anz) {
          PARAMETER *par=(PARAMETER *)malloc(sizeof(PARAMETER)*anz);
  	  char w1[strlen(pos)+1],w2[strlen(pos)+1];
	  int ii=0;
          e=wort_sep(pos,',',TRUE,w1,w2);
		while(e) {
	          if(strlen(w1)) {
                    par[ii].typ=PL_EVAL;
		    par[ii].pointer=malloc(strlen(w1)+1);
		    strcpy(par[ii].pointer,w1);
     		  } else  {
		    par[ii].typ=PL_LEER;
		    par[ii].pointer=NULL;
		  }
		  e=wort_sep(w2,',',TRUE,w1,w2);
                  ii++;
                }
          plist_to_stack(par,(short *)pfuncs[i].pliste,anz,pfuncs[i].pmin,pfuncs[i].pmax);
	  for(ii=0;ii<anz;ii++) free(par[ii].pointer);
          free(par);
	}
	bcpc.pointer[bcpc.len++]=BC_PUSHFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=anz; 
	return(bcerror);
      } else if(pfuncs[i].pmax==1 && ((pfuncs[i].opcode&FM_TYP)==F_SQUICK || 
                                      (pfuncs[i].opcode&FM_TYP)==F_IQUICK ||
                                      (pfuncs[i].opcode&FM_TYP)==F_DQUICK
                                     )) {
        bc_parser(pos);
	bcpc.pointer[bcpc.len++]=BC_PUSHFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=1;
        return(bcerror);
      } else if(pfuncs[i].pmax==2 && (pfuncs[i].opcode&FM_TYP)==F_DQUICK) {
        bc_parser(pos);
	bcpc.pointer[bcpc.len++]=BC_PUSHFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=2;
        return(bcerror);
      } else if(pfuncs[i].pmax==2 && (pfuncs[i].opcode&FM_TYP)==F_IQUICK) {
        bc_parser(pos);
	bcpc.pointer[bcpc.len++]=BC_PUSHFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=2;
        return(bcerror);
      } else {
        printf("Interner ERROR. Funktion nicht korrekt definiert. %s\n",s);
	return(bcerror);
      }
    } else if((i=find_afunc(s))!=-1) {
      if((pafuncs[i].opcode&FM_TYP)==F_SIMPLE || pafuncs[i].pmax==0) {
        bcpc.pointer[bcpc.len++]=BC_PUSHAFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=0;
        return(bcerror);
      } else if((pafuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(pos);
        memcpy(&bcpc.pointer[bcpc.len],pos,strlen(pos));
	bcpc.len+=strlen(pos);
        bcpc.pointer[bcpc.len++]=BC_PUSHAFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=1;
        return(bcerror);
      } else if((pafuncs[i].opcode&FM_TYP)==F_PLISTE) {                
        bc_parser(pos);
	bcpc.pointer[bcpc.len++]=BC_PUSHAFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=count_parameter(pos); 
	return(bcerror);
      } else if(pafuncs[i].pmax==1 && (pafuncs[i].opcode&FM_TYP)==F_AQUICK) {
        bc_parser(pos);
 	bcpc.pointer[bcpc.len++]=BC_PUSHAFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=1; 
	return(bcerror);
      } else if(pafuncs[i].pmax==1 && (pafuncs[i].opcode&FM_TYP)==F_SQUICK) {
        bc_parser(pos);
 	bcpc.pointer[bcpc.len++]=BC_PUSHAFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=1; 
	return(bcerror);
      }
    } else if((i=find_sfunc(s))!=-1) {
      if((psfuncs[i].opcode&FM_TYP)==F_SIMPLE || psfuncs[i].pmax==0) {
        bcpc.pointer[bcpc.len++]=BC_PUSHSFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=0;
        return(bcerror);
      } else if((psfuncs[i].opcode&FM_TYP)==F_ARGUMENT) {
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(pos);
        memcpy(&bcpc.pointer[bcpc.len],pos,strlen(pos));
	bcpc.len+=strlen(pos);
        bcpc.pointer[bcpc.len++]=BC_PUSHSFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=1;
        return(bcerror);
      } else if((psfuncs[i].opcode&FM_TYP)==F_PLISTE) {                
        bc_parser(pos);
	bcpc.pointer[bcpc.len++]=BC_PUSHSFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=count_parameter(pos); 
	return(bcerror);
      } else if(psfuncs[i].pmax==2 && (psfuncs[i].opcode&FM_TYP)==F_DQUICK) {
        bc_parser(pos);
	bcpc.pointer[bcpc.len++]=BC_PUSHSFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=2; 
        return(bcerror);
      } else if(psfuncs[i].pmax==1 && ((psfuncs[i].opcode&FM_TYP)==F_SQUICK ||
                                       (psfuncs[i].opcode&FM_TYP)==F_AQUICK ||
                                       (psfuncs[i].opcode&FM_TYP)==F_IQUICK ||
                                       (psfuncs[i].opcode&FM_TYP)==F_DQUICK
				      )) {
        bc_parser(pos);
	bcpc.pointer[bcpc.len++]=BC_PUSHSFUNC;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=1; 
        return(bcerror);
      }
    } else {
       int anzp=count_parameter(pos);
       bc_parser(pos);    
       bcpc.pointer[bcpc.len++]=BC_PUSHX;
       bcpc.pointer[bcpc.len++]=strlen(s);
       memcpy(&bcpc.pointer[bcpc.len],s,strlen(s));
       bcpc.len+=strlen(s);
       bcpc.pointer[bcpc.len++]=BC_PUSHARRAYELEM;
       bcpc.pointer[bcpc.len++]=anzp;
  //     printf("ARRAY_%s(%s) %d parameter ",s,pos,count_parameter(pos));
       return(bcerror);
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
    /* Erstmal PI, TRUE und FALSE abfangen, das ist schneller */
    
    if(strcmp(s,"PI")==0) {
      double d=PI;
      bcpc.pointer[bcpc.len++]=BC_PUSHF;
      memcpy(&bcpc.pointer[bcpc.len],&d,sizeof(double));
      bcpc.len+=sizeof(double);
    } else if(strcmp(s,"FALSE")==0) {
      bc_push_integer(0);
    } else if(strcmp(s,"TRUE")==0) {
      bc_push_integer(-1);
    } else {
      /*  printf("Sysvar %s gefunden. Nr. %d\n",sysvars[i].name,i);*/
      bcpc.pointer[bcpc.len++]=BC_PUSHSYS;
      memcpy(&bcpc.pointer[bcpc.len],&i,sizeof(int));
      bcpc.len+=sizeof(int);
    }
    return(bcerror);
  } 
  /* Stringsysvars */
  /* Liste durchgehen */
  i=0;
  c=*s;
  a=anzsyssvars;
  l=strlen(s);
  for(b=0; b<l; c=s[++b]) {
    while(c>(syssvars[i].name)[b] && i<a) i++;
    while(c<(syssvars[a].name)[b] && a>i) a--;
    if(i==a) break;
  }
  if(strcmp(s,syssvars[i].name)==0) {
	    /*  printf("Sysvar %s gefunden. Nr. %d\n",syssvars[i].name,i);*/
    bcpc.pointer[bcpc.len++]=BC_PUSHSSYS;
    memcpy(&bcpc.pointer[bcpc.len],&i,sizeof(int));
    bcpc.len+=sizeof(int);
    return(bcerror);
  }

  /* Arraysysvars  */
   
  /* erst integer abfangen (xxx% oder xxx&), dann rest */
  if(*s=='@')   {
    int pc2;
    /* Funktionsnr finden */
     printf("Funktionsaufruf <%s>\n",s+1);
     pc2=procnr(s+1,2);
    if(pc2==-1) { 
      xberror(44,s+1); /* Funktion  nicht definiert */
      return(bcerror);
    }
      printf("Funktion procnr=%d\n",pc2);
      bc_push_integer(0); /*Keine Parameter*/
       bc_jumptosr2(procs[pc2].zeile);
      /*Der Rueckgabewert sollte nach Rueckkehr auf dem Stack liegen.*/
    return(bcerror);
  }
  
#if 0
// Geht so nicht. wir brauchen die Namen   
  if((vnr=variable_exist(s,FLOATTYP))!=-1) {
    bcpc.pointer[bcpc.len++]=BC_PUSHV;
    memcpy(&bcpc.pointer[bcpc.len],&vnr,sizeof(int));
    bcpc.len+=sizeof(int);
    return(bcerror);
  }
  if(s[strlen(s)-1]=='%') {
    s[strlen(s)-1]=0;
    if((vnr=variable_exist(s,INTTYP))!=-1) {
      bcpc.pointer[bcpc.len++]=BC_PUSHV;
      memcpy(&bcpc.pointer[bcpc.len],&vnr,sizeof(int));
      bcpc.len+=sizeof(int);
      return(bcerror);
    }
  }
  if(s[strlen(s)-1]=='$') {
  }  
#endif
  
  if(s[0]=='#') { /* Filenummer ?*/
    int i=0;
    printf("Filenummer: %s\n",s);
    while(s[++i]) s[i-1]=s[i];
    s[i-1]=s[i];
  }
  /* Testen ob und was fuer eine Konstante das ist. */
  e=type3(s);
  if(e&CONSTTYP) {
    if (e&INTTYP) {
      bc_push_integer(atoi(s));
      return(bcerror);
    } else if (e&FLOATTYP) {  
      double d;
      bcpc.pointer[bcpc.len++]=BC_PUSHF;
      d=atof(s);
      memcpy(&bcpc.pointer[bcpc.len],&d,sizeof(double));
      bcpc.len+=sizeof(double);
      return(bcerror);
    } else if (e&STRINGTYP) {
      bcpc.pointer[bcpc.len++]=BC_PUSHS;
      i=strlen(s)-2;
      memcpy(&bcpc.pointer[bcpc.len],&i,sizeof(int));
      bcpc.len+=sizeof(int);
      memcpy(&bcpc.pointer[bcpc.len],&s[1],i);
      bcpc.len+=i;
      return(bcerror);
    }  
  } else {
    bcpc.pointer[bcpc.len++]=BC_PUSHX;
    bcpc.pointer[bcpc.len++]=strlen(s);
    memcpy(&bcpc.pointer[bcpc.len],s,strlen(s));bcpc.len+=strlen(s);
    bcpc.pointer[bcpc.len++]=BC_PUSHV;
    return(bcerror);
  }  /* Jetzt nur noch Zahlen (hex, oct etc ...)*/
  }
  xberror(51,s); /* Syntax error */
  bcerror=51;
  return(bcerror);
}


void bc_kommando(int idx) {
  char buffer[strlen(program[idx])+1];
  char w1[strlen(program[idx])+1];
  char w2[strlen(program[idx])+1],zeile[strlen(program[idx])+1];
  char *pos;
  int i,a,b;

  xtrim(program[idx],TRUE,zeile);
  wort_sep2(zeile," !",TRUE,zeile,buffer);
  xtrim(zeile,TRUE,zeile);
  if(wort_sep(zeile,' ',TRUE,w1,w2)==0) {
    if(donops) {
      bcpc.pointer[bcpc.len++]=BC_NOOP;
      printf(" bc_noop ");
    }
    return;
  }  /* Leerzeile */
  if(w1[0]=='\'') {
    if(docomments) {
      bcpc.pointer[bcpc.len++]=BC_COMMENT;    
      bcpc.pointer[bcpc.len++]=strlen(program[idx]);
      memcpy(&bcpc.pointer[bcpc.len],program[idx],strlen(program[idx]));
      bcpc.len+=strlen(program[idx]);
      printf(" bc_comment <%s> ",program[idx]);
    }
    return;
  }  /* Kommentar */
  if(w1[strlen(w1)-1]==':') {
    if(donops) {
      bcpc.pointer[bcpc.len++]=BC_NOOP;
      printf(" bc_noop ");
    }
    return;
  }  /* nixtun, label */
  if(w1[0]=='@') {
    /* Wenn Parameter, */
    /* Parameter auf den Stack*/
    /* Anzahl Parameter ermitteln */
    /* Zieladresse ermitteln */
    /* BC_BSR  */
    return;
  }
  if(w1[0]=='&') {
    bc_parser(w1+1);
    bcpc.pointer[bcpc.len++]=BC_EVAL;
    printf(" bc_eval <%s> ",w1+1);
    return;
  }
  if(w1[0]=='~') {
    bc_parser(zeile+1);
    /* einmal POP, um den Rueckgabewert zu vernichten */
    bcpc.pointer[bcpc.len++]=BC_POP;
    return;
  }
  if(searchchr(w1,'=')!=NULL) {
    wort_sep(zeile,'=',TRUE,w1,w2);
    bc_parser(w2);
    if(strchr(w1,'(')!=NULL) {
      char *pos,*pos2;
      pos=strchr(w1,'(');
      pos[0]=0;pos++;
      pos2=searchchr2(pos,')');
      if(pos2==NULL) puts("fehlende Schliessende Klammer !");
      if(pos2==pos) {  /* Array-Zuweisung */
        --pos;pos[0]='(';
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(w1);
        memcpy(&bcpc.pointer[bcpc.len],w1,strlen(w1));
        bcpc.len+=strlen(w1);
        bcpc.pointer[bcpc.len++]=BC_ZUWEIS;
	printf("Zuweis ARRAY %s ",w1);
      } else {
        pos2[0]=0;
	bc_parser(pos);
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(w1);
        memcpy(&bcpc.pointer[bcpc.len],w1,strlen(w1));
        bcpc.len+=strlen(w1);
        bcpc.pointer[bcpc.len++]=BC_ZUWEISINDEX;
	bcpc.pointer[bcpc.len++]=count_parameter(pos); 
	printf("Zuweis_index %s %d %s ",w1,count_parameter(pos),pos);
      }
    } else {
      bcpc.pointer[bcpc.len++]=BC_PUSHX;
      bcpc.pointer[bcpc.len++]=strlen(w1);
      memcpy(&bcpc.pointer[bcpc.len],w1,strlen(w1));
      bcpc.len+=strlen(w1);
      bcpc.pointer[bcpc.len++]=BC_ZUWEIS;
      printf("Zuweis %s ",w1);
    }
    return;
  }
  if(isdigit(w1[0]) || w1[0]=='(') {
     bc_parser(w1);
     /* Hm, der Kram landet dann auf dem Stack...*/
     return;
  }
  /* Kommandoliste durchsuchen, moeglichst effektiv ! */

  i=0;a=anzcomms-1;
  for(b=0; b<strlen(w1); b++) {
    while(w1[b]>(comms[i].name)[b] && i<a) i++;
    while(w1[b]<(comms[a].name)[b] && a>i) a--;
#ifdef DEBUG
    printf("%c:%d,%d: %s: %s <--> %s \n",w1[b],i,a,w1,comms[i].name,comms[a].name);
#endif
    if(i==a) break;
  }
  if((i==a && strncmp(w1,comms[i].name,strlen(w1))==0) ||
     (i!=a && strcmp(w1,comms[i].name)==0) ) {
#ifdef DEBUG
      if(b<strlen(w1)) printf("Befehl %s vervollstaendigt --> %s\n",w1,comms[i].name);
#endif
      if(comms[i].opcode&P_IGNORE) {bcpc.pointer[bcpc.len++]=BC_NOOP;return;}
      if((comms[i].opcode&PM_TYP)==P_ARGUMENT){
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(w2);
        memcpy(&bcpc.pointer[bcpc.len],w2,strlen(w2));
	bcpc.len+=strlen(w2);
        bcpc.pointer[bcpc.len++]=BC_PUSHCOMM;
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=1;
        return;
      } 
      if((comms[i].opcode&PM_TYP)==P_SIMPLE) {
        bcpc.pointer[bcpc.len++]=BC_PUSHCOMM;	
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=0;
        return;
      }
      if((comms[i].opcode&PM_TYP)==P_PLISTE) {
        if(strlen(w2)) bc_parser(w2);
	/* Anzahl Prameter herausfinden */
        /* Warnen wenn zuwenig oder zuviel parameter */
        bcpc.pointer[bcpc.len++]=BC_PUSHCOMM;	
	bcpc.pointer[bcpc.len++]=i;
	bcpc.pointer[bcpc.len++]=count_parameter(w2); 
        return;
      } else xberror(38,w1); /* Befehl im Direktmodus nicht moeglich */
    } else if(i!=a) {
       printf("Command uneindeutig ! <%s...%s>\n",comms[i].name,comms[a].name);
    }  else xberror(32,w1);  /* Syntax Error */
}

void bc_restore(int offset) {
  printf("offset=%d ",offset);
  bcpc.pointer[bcpc.len++]=BC_RESTORE;
  memcpy(&bcpc.pointer[bcpc.len],&offset,sizeof(int));
  bcpc.len+=sizeof(int);        
}

void bc_jumpto(int from, int ziel, int eqflag) {
       /* Hier jetzt ein JUMP realisieren */
      if(ziel<=from) { /* Dann ist das Ziel schon bekannt */
        int a,b;
        a=bc_index[ziel];
	b=a-bcpc.len;
	printf("Delta=%d ",b);
        if(b<=127 && b>=-128) {
          printf(" %s.s ",(eqflag?"BEQ":"BRA"));
	  bcpc.pointer[bcpc.len++]=(eqflag?BC_BEQs:BC_BRAs);
	  bcpc.pointer[bcpc.len++]=b;
	} else if(b<=0x7fff && b>=-32768) {
	  short o=b;
          printf(" %s ",(eqflag?"BEQ":"BRA"));
	  bcpc.pointer[bcpc.len++]=(eqflag?BC_BEQ:BC_BRA);
          memcpy(&bcpc.pointer[bcpc.len],&o,sizeof(short));
          bcpc.len+=sizeof(short);        
	} else {
          printf(" %s ",(eqflag?"JEQ":"JMP"));
          bcpc.pointer[bcpc.len++]=(eqflag?BC_JEQ:BC_JMP);
          memcpy(&bcpc.pointer[bcpc.len],&a,sizeof(int));
          bcpc.len+=sizeof(int);        
	}
	printf("---> $%x ",a);
      } else {
        printf(" %s ",(eqflag?"JEQ":"JMP"));
        bcpc.pointer[bcpc.len++]=(eqflag?BC_JEQ:BC_JMP);
        memcpy(&bcpc.pointer[bcpc.len],&ziel,sizeof(int));
        relocation[anzreloc++]=bcpc.len;
        bcpc.len+=sizeof(int);      
      }
}
void bc_jumptosr(int from, int ziel) {
       /* Hier jetzt ein JUMP realisieren */
      if(ziel<=from) { /* Dann ist das Ziel schon bekannt */
        int a,b;
        a=bc_index[ziel];
	b=a-bcpc.len;
	printf("Delta=%d ",b);
        if(b<=0x7fff && b>=-32768) {
	  short o=b;
          printf(" BSR ");
	  bcpc.pointer[bcpc.len++]=BC_BSR;
          memcpy(&bcpc.pointer[bcpc.len],&o,sizeof(short));
          bcpc.len+=sizeof(short);        
	} else {
          printf(" JSR ");
          bcpc.pointer[bcpc.len++]=BC_JSR;
          memcpy(&bcpc.pointer[bcpc.len],&a,sizeof(int));
          bcpc.len+=sizeof(int);        
	}
      } else {
        printf(" JSR ");
        bcpc.pointer[bcpc.len++]=BC_JSR;
        memcpy(&bcpc.pointer[bcpc.len],&ziel,sizeof(int));
        relocation[anzreloc++]=bcpc.len;
        bcpc.len+=sizeof(int);      
      }
}

void bc_jumptosr2(int ziel) {
       /* Hier jetzt ein JUMP realisieren */
        printf(" JSR ");
        bcpc.pointer[bcpc.len++]=BC_JSR;
        memcpy(&bcpc.pointer[bcpc.len],&ziel,sizeof(int));
        relocation[anzreloc++]=bcpc.len;
        bcpc.len+=sizeof(int);      
}

void plist_to_stack(PARAMETER *pp, short *pliste, int anz, int pmin, int pmax) {
  int ii;
  if(anz) {
    for(ii=0;ii<anz;ii++) {
      if(pp[ii].typ==PL_EVAL) {
            if(ii<pmax && (pliste[ii]==PL_NUMBER ||pliste[ii]==PL_INT 
	       ||pliste[ii]==PL_STRING)) {
  	      printf("%d:<%s>  / %x\n",ii,(char *)pp[ii].pointer,pliste[ii]);
	      bc_parser(pp[ii].pointer);  /* Parameter auf den Stack packen */
            } else if(ii<pmax && pliste[ii]==PL_FILENR) {
              if(*((char *)pp[ii].pointer)=='#') bc_parser(pp[ii].pointer+1);
	      else bc_parser(pp[ii].pointer);  /* Parameter auf den Stack packen */
              printf("%d:#<%s>  / %x\n",ii,(char *)pp[ii].pointer,pliste[ii]);
            } else if(ii<pmax && (pliste[ii]==PL_IVAR || 
	           pliste[ii]==PL_FVAR || pliste[ii]==PL_NVAR)) {
              bcpc.pointer[bcpc.len++]=BC_PUSHX;
              bcpc.pointer[bcpc.len++]=strlen(pp[ii].pointer);
              memcpy(&bcpc.pointer[bcpc.len],pp[ii].pointer,strlen(pp[ii].pointer));
	      bcpc.len+=strlen(pp[ii].pointer);
              bcpc.pointer[bcpc.len++]=BC_PUSHVV;
	      printf("%d:VAR<%s>  / %x\n",ii,(char *)pp[ii].pointer,pliste[ii]);
	    } else {
	      printf("Weiss nicht, was mit Parameter anzustellen ist !\n");
  	      printf("%d:<%s>  / %x\n",ii,(char *)pp[ii].pointer,pliste[ii]);
	      bc_parser(pp[ii].pointer);  /* Parameter auf den Stack packen */
            }
        
      } else if(pp[ii].typ==PL_LEER){  /*Dann ist es leer*/
        bcpc.pointer[bcpc.len++]=BC_PUSHLEER;
        printf("%d: empty \n",ii);
      } else {  /*?*/
        printf("Hier stimmt was nicht....\n");
      }
    }
  }
}

void compile() {
  int i,a,b;

  bc_index=malloc(prglen*sizeof(int));
  relocation=malloc(max(1000,prglen)*sizeof(int));
  anzreloc=0;
  bcpc.len=0;
  printf("%d Zeilen.\n",prglen);
  printf("%d Procedures.\n",anzprocs);
  printf("%d Labels.\n",anzlabels);
  printf("PASS A:\n");
  for(i=0;i<prglen;i++) {
    bc_index[i]=bcpc.len;
    printf("%3d:$%08x %x_[%08xx%d](%d)%s \t |",i,bcpc.len,pcode[i].etyp,(unsigned int)pcode[i].opcode,pcode[i].integer,
    pcode[i].panzahl,program[i]);
/* Sonderbehandlungen fuer ... */
    find_comm("PRINT");
//    if((pcode[i].opcode&PM_COMMS)==find_comm("PRINT") ||
//      (pcode[i].opcode&PM_COMMS)==find_comm("?")) {
//    } else
    if((pcode[i].opcode&PM_COMMS)==find_comm("INC")) {
      printf(" INC ");
      bcpc.pointer[bcpc.len++]=BC_PUSH1;
        /* Variable */
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(pcode[i].argument);
        memcpy(&bcpc.pointer[bcpc.len],pcode[i].argument,strlen(pcode[i].argument));
	bcpc.len+=strlen(pcode[i].argument);
        bcpc.pointer[bcpc.len++]=BC_PUSHV;
   
        bcpc.pointer[bcpc.len++]=BC_ADD;
	
        /* Zuweisung */
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(pcode[i].argument);
        memcpy(&bcpc.pointer[bcpc.len],pcode[i].argument,strlen(pcode[i].argument));
	bcpc.len+=strlen(pcode[i].argument);
        bcpc.pointer[bcpc.len++]=BC_ZUWEIS;
    } else if((pcode[i].opcode&PM_COMMS)==find_comm("DEC")) {
      printf(" DEC ");
      bcpc.pointer[bcpc.len++]=BC_PUSH1;
        /* Variable */
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(pcode[i].argument);
        memcpy(&bcpc.pointer[bcpc.len],pcode[i].argument,strlen(pcode[i].argument));
	bcpc.len+=strlen(pcode[i].argument);
        bcpc.pointer[bcpc.len++]=BC_PUSHV;
   
        bcpc.pointer[bcpc.len++]=BC_SUB;
	
        /* Zuweisung */
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(pcode[i].argument);
        memcpy(&bcpc.pointer[bcpc.len],pcode[i].argument,strlen(pcode[i].argument));
	bcpc.len+=strlen(pcode[i].argument);
        bcpc.pointer[bcpc.len++]=BC_ZUWEIS;
    }
/* Jetzt behandlungen nach Pcode */
    else if((pcode[i].opcode&PM_SPECIAL)==P_DATA) {
      /*Erzeugt keinen Code...*/
    }
    else if((pcode[i].opcode&PM_COMMS)==find_comm("RESTORE")) {
      int j;
      printf(" RESTORE ");
      if(strlen(pcode[i].argument)) {
        j=labelnr(pcode[i].argument);
        if(j==-1) xberror(20,pcode[i].argument);/* Label nicht gefunden */
        else {
          bc_restore((int)labels[j].datapointer);
          printf(" %d %s\n",j,labels[j].name);
        }
      } else {
        bc_restore(0);
	printf(" \n");
      }
    } else if((pcode[i].opcode&PM_SPECIAL)==P_EXITIF) {
      char w1[strlen(pcode[i].argument)+1];
      char w2[strlen(pcode[i].argument)+1];
      printf(" EXIT IF ");
      wort_sep(pcode[i].argument,' ',TRUE,w1,w2);
      printf(" <%s> <%s> ",w1,w2);
      bc_parser(w2);
      bcpc.pointer[bcpc.len++]=BC_NOT;
      bc_jumpto(i,pcode[i].integer,1);
    } else if((pcode[i].opcode&PM_SPECIAL)==P_IF) {
      int j,f=0,o;
      bc_parser(pcode[i].argument);
      printf(" IF ");
     /*  a=pcode[i].integer; zeigt auf ENDIF */
      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_ENDIF || o==P_ELSE|| o==P_ELSEIF)  && f==0) break;
        else if(o==P_IF) f++;
        else if(o==P_ENDIF) f--;
      }
      if(j==prglen) xberror(36,"IF"); /*Programmstruktur fehlerhaft */
      if(o==P_ENDIF || o==P_ELSE) bc_jumpto(i,j+1,1);
      else if(o==P_ELSEIF) {
        bc_jumpto(i,j,1); /* Die elseif muss ausgewertet werden*/
      } else bcpc.pointer[bcpc.len++]=BC_POP; /* Stack korrigieren und weitermachen */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_ELSEIF) {
      int j,f=0,o;
      char w1[strlen(pcode[i].argument)+1];
      char w2[strlen(pcode[i].argument)+1];
      pcode[i].opcode&=~P_PREFETCH;
      bc_jumpto(i,pcode[i].integer,0); /* von oben draufgelaufen, gehe zum endif */
      /* Korrigiere bc_index */
      bc_index[i]=bcpc.len;
      /* Wenn wir vom Sprung kommen, landen wir hier */
      printf(" ELSE IF (corr=$%x) ",bcpc.len);

      /* Bedingung separieren */
      wort_sep(pcode[i].argument,' ',TRUE,w1,w2);
      
      /* Bedingung auswerten */
      bc_parser(w2);
            
      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_ENDIF || o==P_ELSE|| o==P_ELSEIF)  && f==0) break;
        else if(o==P_IF) f++;
        else if(o==P_ENDIF) f--;
      }
      if(j==prglen) xberror(36,"ELSE IF"); /*Programmstruktur fehlerhaft */
      if(o==P_ENDIF || o==P_ELSE) bc_jumpto(i,j+1,1);
      else if(o==P_ELSEIF) {
        bc_jumpto(i,j,1); /* Die elseif muss ausgewertet werden*/
      } else bcpc.pointer[bcpc.len++]=BC_POP; /* Stack korrigieren und weitermachen */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_GOSUB) {
      if(pcode[i].integer==-1) bc_kommando(i);
      else {
        char buf[strlen(pcode[i].argument)+1];
	char *pos,*pos2;
	int anzpar=0;
        printf(" GOSUB %d:%d ",pcode[i].integer,procs[pcode[i].integer].zeile);
        strcpy(buf,pcode[i].argument);
        pos=searchchr(buf,'(');
        if(pos!=NULL) {
          pos[0]=0;pos++;
          pos2=pos+strlen(pos)-1;
          if(pos2[0]!=')') {
	    puts("GOSUB: Syntax error bei Parameterliste");
	    structure_warning("GOSUB"); /*Programmstruktur fehlerhaft */
          } else {
	    pos2[0]=0;
	    anzpar=count_parameter(pos);
	    if(anzpar) bc_parser(pos);  /* Parameter auf den Stack */
	  }
        } else pos=buf+strlen(buf);
	bc_push_integer(anzpar);
	bc_jumptosr(i,procs[pcode[i].integer].zeile);
      }
    } else if((pcode[i].opcode&(PM_SPECIAL|P_NOCMD))==P_PROC) {
      int e;
      char w1[strlen(procs[pcode[i].integer].parameterliste)+1];
      char w2[strlen(procs[pcode[i].integer].parameterliste)+1];

      printf(" PROC_%d %s ",pcode[i].integer,procs[pcode[i].integer].parameterliste);
      e=count_parameter(procs[pcode[i].integer].parameterliste);
      bcpc.pointer[bcpc.len++]=BC_POP;
      /* Wir muessen darauf vertrauen, dass die anzahl Parameter stimmt */
      /* Jetzt von Liste von hinten aufloesen */
      e=wort_sepr(procs[pcode[i].integer].parameterliste,',',TRUE,w1,w2);
      while(e==2) {
        printf("<%s>",w2);      
	bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(w2);
        memcpy(&bcpc.pointer[bcpc.len],w2,strlen(w2));
        bcpc.len+=strlen(w2);
        bcpc.pointer[bcpc.len++]=BC_ZUWEIS;
        e=wort_sepr(w1,',',TRUE,w1,w2);
      }  
      if(e) {	
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(w1);
        memcpy(&bcpc.pointer[bcpc.len],w1,strlen(w1));
        bcpc.len+=strlen(w1);
        bcpc.pointer[bcpc.len++]=BC_ZUWEIS;
        printf("<%s>",w1);   
      }    
    } else if((pcode[i].opcode&PM_SPECIAL)==P_RETURN) {
      printf(" RETURN %s ",pcode[i].argument);
      if(pcode[i].argument && strlen(pcode[i].argument)) {
        bc_parser(pcode[i].argument);
      }      
      bcpc.pointer[bcpc.len++]=BC_RTS;
    } else if((pcode[i].opcode&PM_SPECIAL)==P_WHILE) {
      int j,f=0,o;
      bc_parser(pcode[i].argument);
      printf(" WHILE ");
      bc_jumpto(i,pcode[i].integer,1);      
    } else if((pcode[i].opcode&PM_SPECIAL)==P_UNTIL) {
      int j,f=0,o;
      bc_parser(pcode[i].argument);
      printf(" UNTIL ");
      bc_jumpto(i,pcode[i].integer,1);      
    } else if((pcode[i].opcode&PM_SPECIAL)==P_FOR) {
      char w1[strlen(pcode[i].argument)+1],w2[strlen(pcode[i].argument)+1];
      wort_sep(pcode[i].argument,' ',TRUE,w1,w2);
      wort_sep(w1,'=',TRUE,w1,w2);
      printf(" FOR ");
      bc_parser(w2);
      bcpc.pointer[bcpc.len++]=BC_PUSHX;
      bcpc.pointer[bcpc.len++]=strlen(w1);
      memcpy(&bcpc.pointer[bcpc.len],w1,strlen(w1));
      bcpc.len+=strlen(w1);
      bcpc.pointer[bcpc.len++]=BC_ZUWEIS;
    } else if((pcode[i].opcode&PM_SPECIAL)==P_NEXT) {
      int pp=pcode[i].integer;  /* Zeile it dem zugehoerigen For */
      if(pp==-1) xberror(36,"NEXT"); /*Programmstruktur fehlerhaft */
      else {
        char *w1,*w2,*w3,*var,*limit,*step;
	int ss,e,st=0;
        w1=malloc(strlen(pcode[pp].argument)+1); 
        w2=malloc(strlen(pcode[pp].argument)+1); 
        w3=malloc(strlen(pcode[pp].argument)+1); 
        var=malloc(strlen(pcode[pp].argument)+1); 
        step=malloc(strlen(pcode[pp].argument)+1); 
        limit=malloc(strlen(pcode[pp].argument)+1); 
        strcpy(w1,pcode[pp].ppointer->pointer);
        wort_sep(w1,' ',TRUE,w2,w3);
        /* Variable bestimmem */
        if(searchchr(w2,'=')!=NULL) {
          wort_sep(w2,'=',TRUE,var,w1);
        } else printf("Syntax Error ! FOR %s\n",w2); 
        wort_sep(w3,' ',TRUE,w1,w2);
   
        if(strcmp(w1,"TO")==0) ss=0; 
        else if(strcmp(w1,"DOWNTO")==0) ss=1; 
        else printf("Syntax Error ! FOR %s\n",w1);

        /* Limit bestimmem  */
        e=wort_sep(w2,' ',TRUE,limit,w3);
        if(e==0) printf("Syntax Error ! FOR %s\n",w2);
        else {
          if(e==1) ;
          else {
            /* Step-Anweisung auswerten  */
            wort_sep(w3,' ',TRUE,w1,step);
            if(strcmp(w1,"STEP")==0) st=1;
            else printf("Syntax Error ! FOR %s\n",w1); 
          }
        }
        /* Jetzt den Inkrement realisieren a=a+step*/
	if(st) {
	  bc_parser(step);
	} else {
	  bcpc.pointer[bcpc.len++]=BC_PUSH1;
	}
        /* Bei downto negativ */
	if(ss) bcpc.pointer[bcpc.len++]=BC_NEG;

        /* Variable */
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(var);
        memcpy(&bcpc.pointer[bcpc.len],var,strlen(var));bcpc.len+=strlen(var);
        bcpc.pointer[bcpc.len++]=BC_PUSHV;
   
        bcpc.pointer[bcpc.len++]=BC_ADD;

        /* Ergebnis duplizieren fuer den folgenden Test */
        bcpc.pointer[bcpc.len++]=BC_DUP;
	
        /* Zuweisung */
        bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(var);
        memcpy(&bcpc.pointer[bcpc.len],var,strlen(var));bcpc.len+=strlen(var);
        bcpc.pointer[bcpc.len++]=BC_ZUWEIS;

        /* Jetzt auf Schleifenende testen */
        bc_parser(limit);
	if(ss) bcpc.pointer[bcpc.len++]=BC_LESS;
	else bcpc.pointer[bcpc.len++]=BC_GREATER;

        free(w1);free(w2);free(w3);free(var);free(step);free(limit);
        bc_jumpto(i,pp+1,1);
      }	
    } 
    else if((pcode[i].opcode&P_ENDPROC)==P_ENDPROC) printf(" * ");
    else if(pcode[i].opcode&P_IGNORE) printf(" * ");
    else if(pcode[i].opcode&P_INVALID) xberror(32,program[i]); /*Syntax nicht korrekt*/
    else if(pcode[i].opcode&P_EVAL)  {
      printf(" EVAL ");
      bc_kommando(i);
    } else if((pcode[i].opcode&PM_COMMS)>=anzcomms) {
      puts("Precompiler error...");
      bc_kommando(i);
    } else if((pcode[i].opcode&PM_TYP)==P_SIMPLE) {
        printf(" SIMPLE_COMM ");
        bcpc.pointer[bcpc.len++]=BC_PUSHCOMM;
        bcpc.pointer[bcpc.len++]=pcode[i].opcode&PM_COMMS;
        bcpc.pointer[bcpc.len++]=0;
      } else {
      if((pcode[i].opcode&PM_TYP)==P_ARGUMENT) {
        printf(" ARGUMENT_COMM ");
	bcpc.pointer[bcpc.len++]=BC_PUSHX;
        bcpc.pointer[bcpc.len++]=strlen(pcode[i].argument);
        memcpy(&bcpc.pointer[bcpc.len],pcode[i].argument,strlen(pcode[i].argument));
	bcpc.len+=strlen(pcode[i].argument);
        bcpc.pointer[bcpc.len++]=BC_PUSHCOMM;
        bcpc.pointer[bcpc.len++]=pcode[i].opcode&PM_COMMS;	
        bcpc.pointer[bcpc.len++]=1;
      }
      else if((pcode[i].opcode&PM_TYP)==P_PLISTE) {
        int j=(pcode[i].opcode&PM_COMMS);
        printf(" PLISTE(%d) ",pcode[i].panzahl);
	printf("COMMS=%d(%d)\n",j,comms[j].pmax);

	plist_to_stack((PARAMETER *)pcode[i].ppointer,(short *)comms[j].pliste,pcode[i].panzahl,comms[j].pmin,comms[j].pmax);

        bcpc.pointer[bcpc.len++]=BC_PUSHCOMM;
        bcpc.pointer[bcpc.len++]=pcode[i].opcode&PM_COMMS;
        bcpc.pointer[bcpc.len++]=pcode[i].panzahl;
	if(pcode[i].panzahl<comms[j].pmin)  xberror(42,""); /* Zu wenig Parameter  */
        if(comms[j].pmax!=-1 && (pcode[i].panzahl>comms[j].pmax)) xberror(45,""); /* Zu viele Parameter  */
      }
      else bc_kommando(i);
    }
    if(pcode[i].opcode&P_PREFETCH) {
      bc_jumpto(i,pcode[i].integer,0);
    }
     printf("\n");
  }  
  printf("Info:\n");
  printf("  Size of Text-Segment: %d\n",bcpc.len);
  printf("  Size of Data-Segment: %d\n",databufferlen);

  printf("PASS B: %d relocations\n",anzreloc);
  for(i=0;i<anzreloc;i++) {
    b=relocation[i];
    memcpy(&a,&bcpc.pointer[relocation[i]],sizeof(int));
    printf("%d:$%08x ziel=%d ---> $%x\n",i,b,a,bc_index[a]);

    /* Wenn a negativ, dann zeigt er in das data-Segment */

    if(a>=0) a=bc_index[a]+relocation_offset;
    else a=bcpc.len-a;
    
    memcpy(&bcpc.pointer[relocation[i]],&a,sizeof(int));
  }
  free(relocation);
  free(bc_index);
}

