/* xb2c.C   The X11-basic to C translator   (c) Markus Hoffmann 2010-2011
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

#include "config.h"
#include "defs.h"
#include "globals.h"
#include "protos.h"
#include "functions.h"
#include "ptypes.h"
//#include "xb2c.h"


char ifilename[100]="new.bas";       /* Standard inputfile  */
char ofilename[100]="11.c";       /* Standard outputfile     */
int loadfile=FALSE;
int verbose=1;

extern int databufferlen;
extern char *databuffer;

/* X11-Basic needs these declar<ations:  */

extern void reset_input_mode(),x11basicStartup();
extern int param_anzahl;
extern char **param_argumente;
int programbufferlen=0;
char *programbuffer=NULL;
const char version[]="dummy"; /* Version Number. Put some useful information here */
const char vdate[]="dummy";   /* Creation date.  Put some useful information here */
extern const char libversion[];
extern const char libvdate[];
char *program[MAXPRGLEN];
int prglen=0;

extern void memdump(unsigned char *,int);

int find_comm(char *); 

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


#if 0

int expression_parser(char *funktion){  /* Rekursiver Parser */
  char *pos,*pos2;
  char s[strlen(funktion)+1],w1[strlen(funktion)+1],w2[strlen(funktion)+1];
  int a,b,i,e,l,vnr;

  static int bcerror;

  /* Erst der Komma-Operator */
  
  if(searchchr2(funktion,',')!=NULL){
    if(wort_sep(funktion,',',TRUE,w1,w2)>1) {
      expression_parser(w1);
      printf(",");
      expression_parser(w2);return;
    }
  }

  /* Logische Operatoren AND OR NOT ... */
  xtrim(funktion,TRUE,s);  /* Leerzeichen vorne und hinten entfernen, Grossbuchstaben */
  if(strlen(s)==0) {return(bcerror);}
  if(searchchr2_multi(funktion,"&|")!=NULL) {
    if(wort_sepr2(s,"&&",TRUE,w1,w2)>1) {expression_parser(w1);printf(" && ");expression_parser(w2);return(bcerror);}
    if(wort_sepr2(s,"||",TRUE,w1,w2)>1) {
      expression_parser(w1);
      printf(" || ");
      expression_parser(w2);
      return(bcerror);}    
  }

  if(searchchr2(funktion,' ')!=NULL) {
    if(wort_sepr2(s," AND ",TRUE,w1,w2)>1)  {expression_parser(w1);printf(" & ");expression_parser(w2);return(bcerror);}    /* von rechts !!  */
    if(wort_sepr2(s," OR ",TRUE,w1,w2)>1)   {expression_parser(w1);printf(" | ");expression_parser(w2);return(bcerror);}
    if(wort_sepr2(s," NAND ",TRUE,w1,w2)>1) {printf("~(");expression_parser(w1);printf(" & ");expression_parser(w2);printf(")");return(bcerror);}
    if(wort_sepr2(s," NOR ",TRUE,w1,w2)>1)  {printf("~(");expression_parser(w1);printf(" | ");expression_parser(w2);printf(")");return(bcerror);}
    if(wort_sepr2(s," XOR ",TRUE,w1,w2)>1)  {expression_parser(w1);printf(" ^ ");expression_parser(w2);return(bcerror);}
    if(wort_sepr2(s," EOR ",TRUE,w1,w2)>1)  {expression_parser(w1);printf(" ^ ");expression_parser(w2);return(bcerror);}  
    if(wort_sepr2(s," EQV ",TRUE,w1,w2)>1)  {printf("~(");expression_parser(w1);printf(" ^ ");expression_parser(w2);printf(")");return(bcerror);}
    if(wort_sepr2(s," IMP ",TRUE,w1,w2)>1)  {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_XOR;bcpc.pointer[bcpc.len++]=BC_NOT;bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_OR;return(bcerror);}
    if(wort_sepr2(s," MOD ",TRUE,w1,w2)>1)  {bc_parser(w1);bc_parser(w2);bcpc.pointer[bcpc.len++]=BC_MOD;return(bcerror);}
    if(wort_sepr2(s," DIV ",TRUE,w1,w2)>1) {expression_parser(w1);printf(" / ");expression_parser(w2);return(bcerror);}
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
      if(strlen(w1)) {expression_parser(w1);printf("+");expression_parser(w2);return(bcerror);}
      else {expression_parser(w2);return(bcerror);}   /* war Vorzeichen + */
    }
    if(wort_sepr_e(s,'-',TRUE,w1,w2)>1) {       /* von rechts !!  */
      if(strlen(w1)) {expression_parser(w1);printf("-");expression_parser(w2);return(bcerror);}
      else {printf("-");expression_parser(w2);return(bcerror);}   /* war Vorzeichen - */
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
    /* Benutzerdef. Funktionen */
    if(*s=='@') {
      int pc2;
      /* Anzahl Parameter ermitteln */
      bc_parser(pos);
      /* Funktionsnr finden */
      pc2=procnr(s+1,2);
      if(pc2==-1) { 
        xberror(44,s+1); /* Funktion  nicht definiert */
        return(bcerror);
      }
      bcpc.pointer[bcpc.len++]=BC_PUSHUSERFUNC;        
      memcpy(&bcpc.pointer[bcpc.len],&pc2,sizeof(int));
      bcpc.len+=sizeof(int);
      bcpc.pointer[bcpc.len++]=count_parameter(pos); 
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
        bc_parser(pos);
		bcpc.pointer[bcpc.len++]=BC_PUSHFUNC;
		bcpc.pointer[bcpc.len++]=i;
		bcpc.pointer[bcpc.len++]=count_parameter(pos); 
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
    pc2=procnr(s+1,2);
    if(pc2==-1) { 
      xberror(44,s+1); /* Funktion  nicht definiert */
      return(bcerror);
    }
    bcpc.pointer[bcpc.len++]=BC_PUSHUSERFUNC;        
    memcpy(&bcpc.pointer[bcpc.len],&pc2,sizeof(int));
    bcpc.len+=sizeof(int);
    bcpc.pointer[bcpc.len++]=0;
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
  
  
  /* Testen ob und was fuer eine Konstante das ist. */
  e=type3(s);
  if(e&CONSTTYP) { 
    if (e&INTTYP) {
      return(bcerror);
    } else if (e&FLOATTYP) {  
      double d;
      return(bcerror);
    } else if (e&STRINGTYP) {
      return(bcerror);
    }  
  } else {
    return(bcerror);
  }  /* Jetzt nur noch Zahlen (hex, oct etc ...)*/
  }
  xberror(51,s); /* Syntax error */
  bcerror=51;
  return(bcerror);
}

#endif

void intro(){
  puts("***************************************************************");
  printf("*           X11-Basic bytecode compilter                    *\n");
  puts("*                    by Markus Hoffmann 1997-2008 (c)         *");
  printf("* library V. %s date:   %30s      *\n",libversion,libvdate);
  puts("***************************************************************");
  puts("");
}
void usage(){
  puts("\n Usage:\n ------ \n");
  printf(" %s [-o <outputfile> -h] [<filename>] --- compile program [%s]\n\n","bytecode",ifilename);
  printf("-o <outputfile>     --- put result in file [%s]\n",ofilename);
  puts("-h --help           --- Usage");
}

void kommandozeile(int anzahl, char *argumente[]) {
  int count;

  /* Kommandozeile bearbeiten   */
  for(count=1;count<anzahl;count++) {
    if (strcmp(argumente[count],"-o")==FALSE) {
      strcpy(ofilename,argumente[++count]);
    } else if (strcmp(argumente[count],"-h")==FALSE) {
      intro();
      usage();
    } else if (strcmp(argumente[count],"--help")==FALSE) {
      intro();
      usage();
    } else {
      if(!loadfile) {
        loadfile=TRUE;
        strcpy(ifilename,argumente[count]);
      }
    }
   }
}



void compile() {
  int i,a,b;
  printf("/* produced with xb2c \n\n");
  printf("%d Zeilen.\n",prglen);
  printf("%d Procedures.\n",anzprocs);
  printf("%d Labels.\n",anzlabels);
  printf("    */\n");
  printf("databuffer=\"%s\";",databuffer);
  printf("databufferlen=%d;",databufferlen);
  printf("main() {\n");
  for(i=0;i<prglen;i++) {
    printf("%3d:$%08x %x_[%08xx%d](%d)%s \t |",i,0,pcode[i].etyp,pcode[i].opcode,pcode[i].integer,
    pcode[i].panzahl,program[i]);
/* Sonderbehandlungen fuer ... */
    if((pcode[i].opcode&PM_COMMS)==find_comm("INC")) {
        /* Variable */
        printf("%s++;  /* INC */\n",pcode[i].argument);
    } else if((pcode[i].opcode&PM_COMMS)==find_comm("DEC")) {
        printf("%s--;  /* DEC */\n",pcode[i].argument);
    }
/* Jetzt behandlungen nach Pcode */
    else if((pcode[i].opcode&PM_SPECIAL)==P_DATA) {
      /*Erzeugt keinen Code...*/
    }
    else if((pcode[i].opcode&PM_COMMS)==find_comm("RESTORE")) {
      if(strlen(pcode[i].argument)) {
        int j;
        j=labelnr(pcode[i].argument);
        if(j==-1) xberror(20,pcode[i].argument);/* Label nicht gefunden */
        else printf("datapointer=%d;  /*RESTORE %s */\n",(int)labels[j].datapointer,labels[j].name);
      } else {
        printf("datapointer=0;  /* RESTORE */\n");
      }
    } else if((pcode[i].opcode&PM_SPECIAL)==P_EXITIF) {
      printf("if(%s) break; /* EXIT IF %s */\n",pcode[i].argument,pcode[i].argument);
    } else if((pcode[i].opcode&PM_SPECIAL)==P_IF) {
      printf("if(%s) { /* IF %s */\n",pcode[i].argument,pcode[i].argument);
    } else if((pcode[i].opcode&PM_SPECIAL)==P_ELSEIF) {
      printf("} else if(%s) { /* ELSEIF %s */\n",pcode[i].argument,pcode[i].argument);
    } else if((pcode[i].opcode&PM_SPECIAL)==P_GOSUB) {
      printf("%s /* GOSUB */\n");
      if(pcode[i].integer==-1) printf("Something is wrong.\n");
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
//	    if(anzpar) bc_parser(pos);  /* Parameter auf den Stack */
	  }
        } else pos=buf+strlen(buf);
      }
    } else if((pcode[i].opcode&(PM_SPECIAL|P_NOCMD))==P_PROC) {
      int e;
      char w1[strlen(procs[pcode[i].integer].parameterliste)+1];
      char w2[strlen(procs[pcode[i].integer].parameterliste)+1];

      printf(" PROC_%d %s ",pcode[i].integer,procs[pcode[i].integer].parameterliste);
      e=count_parameter(procs[pcode[i].integer].parameterliste);
      /* Wir muessen darauf vertrauen, dass die anzahl Parameter stimmt */
      /* Jetzt von Liste von hinten aufloesen */
      e=wort_sepr(procs[pcode[i].integer].parameterliste,',',TRUE,w1,w2);
      while(e==2) {
        printf("<%s>",w2);      
      }  
      if(e) {	
        printf("<%s>",w1);   
      }    
    } else if((pcode[i].opcode&PM_SPECIAL)==P_RETURN) {
      printf(" RETURN %s ",pcode[i].argument);
      if(pcode[i].argument && strlen(pcode[i].argument)) {
     //   bc_parser(pcode[i].argument);
      }      
    } else if((pcode[i].opcode&PM_SPECIAL)==P_WHILE) {
      int j,f=0,o;
    //  bc_parser(pcode[i].argument);
      printf(" WHILE ");
    } else if((pcode[i].opcode&PM_SPECIAL)==P_UNTIL) {
      int j,f=0,o;
    //  bc_parser(pcode[i].argument);
      printf(" UNTIL ");
    } else if((pcode[i].opcode&PM_SPECIAL)==P_FOR) {
      char w1[strlen(pcode[i].argument)+1],w2[strlen(pcode[i].argument)+1];
      wort_sep(pcode[i].argument,' ',TRUE,w1,w2);
      wort_sep(w1,'=',TRUE,w1,w2);
      printf(" FOR ");
    //  bc_parser(w2);
    } else if((pcode[i].opcode&PM_SPECIAL)==P_NEXT) {
      printf("}\n /* NEXT */");
    }
    else if(pcode[i].opcode&P_INVALID) xberror(32,program[i]); /*Syntax nicht korrekt*/
    else if(pcode[i].opcode&P_EVAL)  /*bc_kommando(i) */;
    else if((pcode[i].opcode&PM_COMMS)>=anzcomms) {
      puts("Precompiler error...");
      //bc_kommando(i);
    } else if((pcode[i].opcode&PM_TYP)==P_SIMPLE) {
        printf(" SIMPLE_COMM ");
      } else {
      if((pcode[i].opcode&PM_TYP)==P_ARGUMENT) {
        printf(" ARGUMENT_COMM ");
      }
      else if((pcode[i].opcode&PM_TYP)==P_PLISTE) {
        printf(" PLISTE(%d) ",pcode[i].panzahl);
        if(pcode[i].panzahl) /*bc_parser(pcode[i].argument)*/;  /* Parameter auf den Stack packen */
      }
      else /*bc_kommando(i)*/;
    }
    
    if(pcode[i].opcode&P_PREFETCH) {
     // bc_jumpto(i,pcode[i].integer,0);
    }
     printf("\n");
  }  
  printf("Info:\n");
}





main(int anzahl, char *argumente[]) {
  /* Initialize data segment buffer */
  if(anzahl<2) {    /* Kommandomodus */
    intro();
    batch=0;
  } else {
    kommandozeile(anzahl, argumente);    /* Kommandozeile bearbeiten */
    if(loadfile) {
      if(exist(ifilename)) {
        loadprg(ifilename);
	compile();
      } else printf("ERROR: %s not found !\n",ifilename);
    }
  }
}
