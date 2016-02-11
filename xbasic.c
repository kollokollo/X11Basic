/* XBASIC.C 

   (c) Markus Hoffmann 
   
 ****************************************************************************
 **									   **
 **				BASIC   				   **
 **									   **
 **									   **
 ** 									   **
 **  Erstellt: Aug. 1997   von Markus Hoffmann				   **
 ** 									   **
 **  Letzte Bearbeitung: 23.Okt. 1998					   **
 **									   **
 ****************************************************************************/

 /* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */  
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#include "xbasic.h"
#include "file.h"
#include "window.h"


const char version[]="1.00";           /* Programmversion           */
const char vdate[]="01.01.1999";
#ifdef CONTROL
const char xbasic_name[]="csxbasic";
#else
const char xbasic_name[]="x11basic";
#endif

int pc=0,sp=0,prglen=0,echo=TRUE,batch=0,errcont=0,everyflag=0;
int everytime=0,alarmpc=-1;
char ifilename[100]="input.bas";       /* Standartfile             */
int loadfile=FALSE;
int stack[STACKSIZE];

/* fuer die Dateiverwaltung     */
FILE *dptr[100];
int filenr[100];


VARIABLE variablen[ANZVARS];
int anzvariablen=0;
LABEL labels[ANZLABELS];
int anzlabels=0;
PROC procs[ANZPROCS];
int anzprocs=0;
int usewindow=DEFAULTWINDOW;

void *obh;       /* old break handler  */
/* Kommandoliste: muss alphabetisch sortiert sein !   */

COMMAND comms[]= {
 { P_PRINT,  "?"        , c_print},
    
 { P_EVAL,   "ADD"      , c_add  },
 { P_EVAL,   "AFTER"    , c_after  },
 { P_EVAL,   "ALERT"    , c_alert  },
 { P_EVAL,   "ARRAYFILL", c_arrayfill},
 { P_EVAL,   "ARRAYCOPY", c_arraycopy},

 { P_BELL,   "BEEP"     , c_beep},
 { P_BELL,   "BELL"     , c_beep},
 { P_EVAL,   "BGET"     , c_bget},
 { P_EVAL,   "BLOAD"    , c_bload},
 { P_EVAL,   "BMOVE"    , c_bmove}, 
 { P_EVAL,   "BPUT"     , c_bput},
 { P_BREAK,  "BREAK"    , c_break}, 
 { P_EVAL,   "BSAVE"    , c_bsave},

 { P_CASE,   "CASE"     , c_case},
 { P_EVAL,   "CHAIN"    , c_chain},
 { P_EVAL,   "CIRCLE"   , c_circle},
 { P_EVAL,   "CLEAR"    , c_clear},
 { P_EVAL,   "CLEARW"   , c_clearw},
 { P_EVAL,   "CLOSE"    , c_close},
 { P_EVAL,   "CLOSEW"   , c_closew},
 { P_EVAL,   "CLR"      , c_clr},
 { P_EVAL,   "CLS"      , c_cls},
 { P_EVAL,   "COLOR"    , c_color},
 { P_CONT,   "CONT"     , c_cont},
/* Kontrollsystembefehle  */
#ifdef CONTROL 

 { P_EVAL,   "CSPUT"    , c_csput},
 { P_EVAL,   "CSCLEARCALLBACKS"    , c_csclearcallbacks},
 { P_EVAL,   "CSSET"    , c_csput},
 { P_EVAL,   "CSSETCALLBACK"    , c_cssetcallback},
 { P_EVAL,   "CSSWEEP"  , c_cssweep},
 { P_EVAL,   "CSVPUT"   , c_csvput},
#endif

 { P_DATA,   "DATA"     , c_nop  },
 { P_EVAL,   "DEC"      , c_dec},
 { P_DEFAULT,"DEFAULT"  , c_case},
 { P_EVAL,   "DEFFILL"  , c_deffill},
 { P_EVAL,   "DEFLINE"  , c_defline},
 { P_EVAL,   "DEFMOUSE" , c_defmouse},
 { P_EVAL,   "DEFTEXT"  , c_deftext},
 { P_EVAL,   "DIM"      , c_dim},
 { P_EVAL,   "DIV"      , c_div},
 { P_DO,     "DO"       , c_do  },
 { P_EVAL,   "DRAW"     , c_draw},

 { P_EVAL,   "DUMP"     , c_dump},


 { P_EVAL,   "ECHO"     , c_echo},
 { P_EVAL,   "ELIPSE"     , c_ellipse},
 { P_EVAL,   "ELLIPSE"     , c_ellipse},
 { P_ELSE,   "ELSE"     , c_else  },
 { P_END,    "END"      , c_end},
 { P_ENDPROC,"ENDFUNC"  , c_return},
 { P_ENDPROC,"ENDFUNCTION", c_return},
 { P_ENDIF,  "ENDIF"    , c_nop  },
 { P_ENDSELECT,"ENDSELECT" , c_nop  },
 { P_ENDSELECT,"ENDSWITCH" , c_nop  },
 { P_EVAL,   "ERASE"    , c_erase},
 { P_EVAL,   "ERROR"    , c_error},
 { P_EVAL,   "EVENT"    , c_allevent},
 { P_EVAL,   "EVERY"    , c_every},
 { P_EVAL,   "EXIT"     , c_exit},

 { P_EVAL,   "FFT"      , c_fft},
 { P_EVAL,   "FILESELECT", c_fileselect},
 { P_EVAL,   "FLUSH"    , c_flush},
 { P_FOR,    "FOR"      , c_for},
 { P_PROC,   "FUNCTION" , c_end},

 { P_EVAL,   "GOSUB"    , c_gosub},
 { P_EVAL,   "GOTO"     , c_goto},
 { P_EVAL,   "GRAPHMODE", c_graphmode},

 { P_EVAL,   "HOME"     , c_home},

 { P_IF,     "IF"       , c_if},
 { P_EVAL,   "INC"      , c_inc},
 { P_EVAL,   "INFOW"    , c_infow},
 { P_EVAL,   "INPUT"    , c_input},

 { P_EVAL,   "KEYEVENT" , c_keyevent},


 { P_EVAL,   "LET"      , c_let},
 { P_EVAL,   "LINEINPUT", c_lineinput},
 
 { P_EVAL,   "LIST"     , c_list},
 { P_EVAL,   "LOAD"     , c_load},
 { P_EVAL,   "LOCAL"    , c_local},
 { P_LOOP,   "LOOP"     , c_loop},
 { P_EVAL,   "LTEXT"    , c_ltext},

 { P_EVAL,   "MERGE"    , c_merge},
 { P_EVAL,   "MOUSE"    , c_mouse},
 { P_EVAL,   "MOUSEEVENT" , c_mouseevent},
 { P_EVAL,   "MOTIONEVENT" , c_motionevent},
 { P_EVAL,   "MOVEW"    , c_movew},
 { P_EVAL,   "MUL"      , c_mul},
 
 { P_NEW,    "NEW"      , c_new},
 { P_NEXT,   "NEXT"     , c_next},
 { P_EVAL,   "NOROOTWINDOW", c_norootwindow},

 { P_EVAL,   "ON"       , c_on},
 { P_EVAL,   "OPEN"     , c_open},
 { P_EVAL,   "OPENW"    , c_openw},
 { P_EVAL,   "OUT"      , c_out},

 { P_EVAL,   "PAUSE"    , c_pause},
 { P_EVAL,   "PCIRCLE"  , c_pcircle},
 { P_EVAL,   "PELIPSE"  , c_pellipse},
 { P_EVAL,   "PLIST"    , c_plist},
 { P_EVAL,   "PLOT"     , c_plot},
 { P_PRINT,  "PRINT"    , c_print},
 { P_PROC,   "PROCEDURE", c_end  },
 { P_IGNORE, "PROGRAM"  , c_nop  },
 { P_EVAL,   "PUTBACK"  , c_unget},

 { P_EVAL,   "QUIT"     , c_quit},


 { P_IGNORE, "RANDOMIZE", c_nop  },
 { P_EVAL,   "READ"     , c_read},
 { P_EVAL,   "RELSEEK"  , c_relseek},
 { P_REM,    "REM"      , c_nop  },
 { P_REPEAT, "REPEAT"   , c_nop  },
 { P_EVAL,   "RESTORE"  , c_restore},
 { P_EVAL,   "RETURN"   , c_return},
 { P_EVAL,   "ROOTWINDOW", c_rootwindow},
 { P_EVAL,   "RUN"      , c_run},

 { P_EVAL,   "SAVESCREEN"    , c_savescreen},
 { P_EVAL,   "SAVEWINDOW"    , c_savewindow},
 { P_EVAL,   "SCOPE"    , c_scope},
 { P_EVAL,   "SEEK"     , c_seek},
 { P_SELECT, "SELECT"   , c_select},
 { P_EVAL,   "SETFONT"  , c_setfont},
 { P_EVAL,   "SETMOUSE" , c_setmouse},
 { P_EVAL,   "SHOWPAGE" , c_vsync},
 { P_EVAL,   "SIZEW"    , c_sizew},
 
 { P_STOP,   "STOP"     , c_stop},
 { P_EVAL,   "SUB"      , c_sub},
 { P_EVAL,   "SWAP"     , c_swap},
 { P_SELECT, "SWITCH"   , c_select},
 { P_EVAL,   "SYSTEM"   , c_system},

 { P_EVAL,   "TEXT"     , c_text},
 { P_EVAL,   "TITLEW"    , c_titlew},
 { P_EVAL,   "TROFF"    , c_troff},
 { P_EVAL,   "TRON"     , c_tron},

 { P_UNTIL,  "UNTIL"    , c_until},
 { P_EVAL,   "USEWINDOW", c_usewindow},

 { P_EVAL,   "VERSION"  , c_version},
 { P_EVAL,   "VOID"     , c_void},
 { P_EVAL,   "VSYNC"    , c_vsync},

 { P_WEND,   "WEND"     , c_wend},
 { P_WHILE,  "WHILE"    , c_while},
 { P_EVAL,   "WORT_SEP" , c_wort_sep},
 
 { P_EVAL,   "XLOAD"    , c_xload},
 { P_EVAL,   "XRUN"     , c_xrun},


};
int anzcomms=sizeof(comms)/sizeof(COMMAND);

/* Standard-Fehlerroutine   */


void error(char err, char *bem) {
  printf("Zeile %d: %s\n",pc-1,error_text(err,bem));
  if(!errcont) batch=0;   
}

void break_handler( int signum) {
  if(batch) {
    printf("** PROGRAM-STOP \n");
    batch=0;
    signal(signum, break_handler);
  } else {
    printf("** X11BASIC-QUIT \n");
    signal(SIGINT, obh);
    raise(signum);
  }
}
void fatal_error_handler( int signum) {
  printf("** Fataler BASIC-Interpreterfehler #%d \n",signum);

  if(batch) {
    if(pc>=0) {
      printf("Zeile  pc-1   : %s\n",program[pc-1]);
      printf("Zeile: Pc=%d  : %s\n",pc,program[pc]);
      printf("Zeile  pc+1   : %s\n",program[pc+1]);

    } else printf("PC negativ !\n");
      printf("Stack-Pointer: SP=%d\n",sp);
      batch=0;
  } else {
    c_dump("");
    printf("Programm-Abbruch...\n");    
    signal(signum,SIG_DFL);
  }
  raise(signum);
}


void timer_handler( int signum) {
  if(alarmpc==-1) printf("** Uninitialisierter Interrupt #%d \n",signum);
  else {
    int oldbatch,osp=sp,pc2;
      pc2=procs[alarmpc].zeile;
      
      if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
      else {printf("Stack-Overflow ! PC=%d\n",pc); batch=0;}
      oldbatch=batch;batch=1;
      programmlauf();
      batch=min(oldbatch,batch);
      if(osp!=sp) {
	pc=stack[--sp]; /* wenn error innerhalb der func. */
        printf("Fehler innerhalb Interrupt-FUNCTION. \n");
      }
      
  }
  signal(signum, timer_handler);
  if(everyflag) alarm(everytime); 
}




int mergeprg(char *fname) {
  char *pos,*pos2,*pos3,*buffer=NULL,*zeile=NULL;  int i,len,typ;  FILE *dptr;

  /* Filelaenge rauskriegen */

  dptr=fopen(fname,"r"); len=lof(dptr); fclose(dptr);
  programbuffer=realloc(programbuffer,programbufferlen+len+1);
  bload(fname,programbuffer+programbufferlen,len);
  programbufferlen+=len;

  /* Zeilenzahl herausbekommen */
  pos=programbuffer;
  i=prglen=0;
  while(i<programbufferlen) {
    if(programbuffer[i]==0 || programbuffer[i]=='\n') {
      programbuffer[i]=0;
      program[prglen++]=pos;
      pos=programbuffer+i+1;
    }
    i++;
  }
  
  /* Label- und Procedurliste Erstellen und p_code transformieren*/

  clear_labelliste();
  clear_procliste();
  
  for(i=0; i<prglen;i++) {
    zeile=realloc(zeile,strlen(program[i])+1);
    buffer=realloc(buffer,strlen(program[i])+1);
 
    
    strcpy(zeile, program[i]);

    wort_sep2(zeile," ! ",TRUE,zeile,buffer);  /*Kommentare abseparieren*/
    xtrim(zeile,TRUE,zeile);
    if(wort_sep(zeile,' ',TRUE,zeile,buffer)==0) {
      pcode[i].opcode=P_IGNORE;
    } else if(zeile[0]=='\'') {
      pcode[i].opcode=P_REM;
    } else if(zeile[strlen(zeile)-1]==':') {
      zeile[strlen(zeile)-1]=0;
#ifdef DEBUG
      printf("Label gefunden: %s in Zeile %d  \n",zeile,i);
#endif
      labels[anzlabels].name=malloc(strlen(zeile)+1);
      strcpy(labels[anzlabels].name,zeile);
      labels[anzlabels].zeile=i;
      pcode[i].opcode=P_LABEL;
      pcode[i].integer=anzlabels;
      anzlabels++;
    } else {
      typ=(strcmp(zeile,"PROCEDURE")==0 | 2*(strcmp(zeile,"FUNCTION")==0));
      if(typ) {
        pos2=searchchr(buffer,'(');
        if(pos2 != NULL) {
          pos2[0]=0;pos2++;
          pos3=pos2+strlen(pos2)-1;
          if(pos3[0]!=')') printf("Syntax error bei Parameterliste\n");
          else pos3[0]=0;
        } else pos2=zeile+strlen(zeile);
        procs[anzprocs].name=malloc(strlen(buffer)+1);
        strcpy(procs[anzprocs].name,buffer);
        procs[anzprocs].parameterliste=malloc(strlen(pos2)+1);
 
        strcpy(procs[anzprocs].parameterliste,pos2);
        procs[anzprocs].typ=typ;
      
        procs[anzprocs].zeile=i;
	pcode[i].opcode=P_PROC;
        pcode[i].integer=anzprocs;
        anzprocs++;
      } else {
        int j;
	
      /* Rest Transformieren    */
        pcode[i].opcode=P_EVAL;
        pcode[i].pointer=program[i];
        for(j=0;j<anzcomms; j++) {
	  if(strcmp(zeile,comms[j].name)==0) {
	    pcode[i].opcode=comms[j].opcode;
	    break;
	  }
	}
      }
    }
    
  } 
  free(buffer);free(zeile); 
}

void clear_labelliste() {
  int i;
  if(anzlabels) {
    for(i=0;i<anzlabels;i++) free(labels[i].name);
    anzlabels=0;
  }
}

int labelnr(char *n) {
  int i;
  if(anzlabels) {
    for(i=0;i<anzlabels;i++) {
      if(strcmp(labels[i].name,n)==0) return(labels[i].zeile);
    }
  }
  return(-1);
}

void clear_procliste() {
  int i;
  if(anzprocs) {
    for(i=0;i<anzprocs;i++) {
      free(procs[i].name);
      free(procs[i].parameterliste);
    }
    anzprocs=0;
  }
}

int procnr(char *n,int typ) {
  int i;
  if(anzprocs) {
    for(i=0;i<anzprocs;i++) {
      if(strcmp(procs[i].name,n)==0 && procs[i].typ==typ) return(i);
    }
  }
  return(-1);
}


char *indirekt2(char *funktion) {
   char *ergebnis;
  if(funktion[0]=='&') {
    ergebnis=s_parser(funktion+1);
    xtrim(ergebnis,TRUE,ergebnis);
  } else {
    ergebnis=malloc(strlen(funktion)+1);
    strcpy(ergebnis,funktion);
  }
  return(ergebnis);
}



char *rsearchchr2(char *start,char c,char *end) {
  int f=0;
  while((start[0]!=c || f) && start>=end) {
    if(start[0]=='\"') f= !f;
    start--;
  }
  return(start);
}

int type2(char *ausdruck) {
  char w1[strlen(ausdruck)+1],w2[strlen(ausdruck)+1];
  char *pos;
  int typ=0;
  
  wort_sep(ausdruck,'+',TRUE,w1,w2);
  if(strlen(w1)==0) return(FLOATTYP);  /* war Vorzeichen */
  if(w1[0]=='"') return(STRINGTYP);
  if(w1[0]=='&') return(INDIRECTTYP);
  if(w1[0]=='(' && w1[strlen(w1)-1]==')') {
    char ttt[strlen(ausdruck)+1];
    strcpy(ttt,ausdruck);
    ttt[strlen(ttt)-1]=0;
    return(type2(ttt+1));
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
    else return(FLOATTYP);
  }
}



int suche(int begin, int richtung, char *such,char *w1,char *w2) {
  int i,f=0;
  char buffer[MAXSTRLEN],nbuffer[MAXSTRLEN],b1[MAXSTRLEN],b2[MAXSTRLEN],zeile[MAXSTRLEN];

  for(i=begin; (i<prglen && i>=0);i+=richtung) {
    strcpy(zeile, program[i]);
    xtrim(zeile,TRUE,zeile);
    wort_sep(zeile,'!',TRUE,zeile,buffer);
    wort_sep(zeile,' ',TRUE,b1,b2);
    strcpy(buffer+1,such); buffer[0]='|';
    strcat(buffer,"|");
    strcpy(nbuffer+1,b1); nbuffer[0]='|';
    strcat(nbuffer,"|");
    /*printf("SUCHE: %d <%s> <%s> %d\n",i,buffer,nbuffer,f);*/
    if(strstr(buffer,nbuffer)!=NULL && f==0) return(i);
    else if(strcmp(b1,w1)==0) f++;
    else if(strcmp(b1,w2)==0) f--;
  }
  return(-1);
}
int suchep(int begin, int richtung, int such, int w1, int w2) {
  int i,f=0,o;

  for(i=begin; (i<prglen && i>=0);i+=richtung) {
    o=pcode[i].opcode;
    if(o==such && f==0) return(i);
    else if(o==w1) f++;
    else if(o==w2) f--;
  }
  return(-1);
}


char *print_arg(char *ausdruck) {
  int e;
  char *ergebnis;
  char *a1,w1[strlen(ausdruck)+1],w2[strlen(ausdruck)+1];
  char w3[strlen(ausdruck)+1],w4[strlen(ausdruck)+1];
  
  /*printf("print_arg: %s\n",ausdruck);*/
  ergebnis=malloc(1);
  ergebnis[0]=0;
  e=arg2(ausdruck,TRUE,w1,w2);
  while(e!=0) {
    a1=indirekt2(w1);
  /*  printf("TEST: <%s> <%s> %d\n",w1,w2,e);*/
    if(strncmp(a1,"AT(",3)==0) {
      a1[strlen(a1)-1]=0;
      wort_sep(a1+3,',',TRUE,w3,w4);
      ergebnis=realloc(ergebnis,strlen(ergebnis)+1+10);
      sprintf(ergebnis+strlen(ergebnis),"\033[%.3d;%.3dH",(int)parser(w3),(int)parser(w4));
    } else if(strncmp(a1,"TAB(",4)==0) {
      a1[strlen(a1)-1]=0;
      ergebnis=realloc(ergebnis,strlen(ergebnis)+1+6);
      sprintf(ergebnis+strlen(ergebnis),"\033[%.3dC",(int)parser(a1+4));
    } else {
      if(strlen(a1)) {    
      /*printf("TEST2: <%s> <%s> %d\n",a1,a2,e);*/
        int typ=type2(a1);
	if(typ & ARRAYTYP) {
	  if(typ & STRINGTYP) ;
	  else ;
	} else if(typ & STRINGTYP) {
	  /*printf("TEST3: <%s> <%s> %d\n",a1,a2,e);*/

          char *a3=s_parser(a1);
	  ergebnis=realloc(ergebnis,strlen(ergebnis)+1+strlen(a3));
          strcat(ergebnis,a3); 
	  free(a3);
        } else {
	  ergebnis=realloc(ergebnis,strlen(ergebnis)+1+13);
	  sprintf(ergebnis+strlen(ergebnis),"%.13g",parser(a1));
        }
      }
    }
    ergebnis=realloc(ergebnis,strlen(ergebnis)+1+1);
    if(e==2) ;
    else if(e==3) strcat(ergebnis,"\011");
    else if(e==4) strcat(ergebnis," ");
    free(a1);
    e=arg2(w2,TRUE,w1,w2);
  }
  return(ergebnis);
}

void kommando(char *cmd) {
  char buffer[strlen(cmd)+1],w1[strlen(cmd)+1],w2[strlen(cmd)+1],zeile[strlen(cmd)+1];
  char *pos;
  int i,a,b,oa;
 
  xtrim(cmd,TRUE,zeile);
  wort_sep2(zeile," !",TRUE,zeile,buffer);
  xtrim(zeile,TRUE,zeile);
  if(wort_sep(zeile,' ',TRUE,w1,w2)==0) return;  /* Leerzeile */
  if(w1[0]=='\'')                       return;  /* Kommentar */         
  if(w1[strlen(w1)-1]==':')             return;  /* nixtun, label */
  if(w1[0]=='@') {c_gosub(w1+1);return;}  
  if(w1[0]=='~') {c_void(w1+1);return;}  
  if(searchchr(w1,'=')!=NULL) {
    wort_sep(zeile,'=',TRUE,w1,w2);
    xzuweis(w1,w2);
    return;
  }
  
  
  /* Kommandoliste durchsuchen, moeglichst effektiv ! */


  i=0;a=anzcomms;
  for(b=0; b<strlen(w1); b++) {
    while(w1[b]>(comms[i].name)[b] && i<a) i++;
    oa=a;a=i;
    while(w1[b]<(comms[a].name)[b]+1 && a<oa) a++;
#ifdef DEBUG    
    printf("%c:%d,%d: %s <--> %s \n",w1[b],i,a,w1,comms[i].name);
#endif
    if(i==a) break;
  }
  if(i<anzcomms) {
    if(strcmp(w1,comms[i].name)==0) {(comms[i].routine)(w2);return;}
  }   
  
  if(isdigit(w1[0])) printf("%.13g\n",parser(zeile));  
  else if(w1[0]=='&') {
    char *test,*test2;
    test=indirekt2(w1);
    test2=malloc(strlen(test)+1+1+strlen(w2));
    strcpy(test2,test);
    free(test);
    strcat(test2," ");
    strcat(test2,w2);  
    kommando(test2);
    free(test2);
  }
  
  /* Jetzt die restlichen Grafik-Routinen  */
   else if(strcmp(w1,"BOX")==0) c_x4(w2,1);
   else if(strcmp(w1,"LINE")==0) c_x4(w2,0);
   else if(strcmp(w1,"PBOX")==0) c_x4(w2,2);
   else  {
    printf("Unbekannter Befehl: <%s> <%s>\n",w1,w2);

    }
}


void programmlauf(){
    int isp,ipc;
#ifdef DEBUG
    int opc,timer;
#endif    
    isp=sp;ipc=pc;
    while(batch && pc<prglen && pc>=0 && sp>=isp)  {
      if(echo) printf("%s\n",program[pc]);
#ifdef DEBUG      
      timer=clock();opc=pc;
#endif   
      switch(pcode[pc].opcode) {
        case P_IGNORE:
	case P_REM: 
	case P_ENDIF:
	case P_ENDSELECT:
	case P_REPEAT:
	case P_LABEL:
	case P_DATA:
	  pc++;break;
	case P_PROC: pc++;c_end("");break;
	case P_WEND: pc++;c_wend("");break;  
	case P_STOP: pc++;c_stop("");break;  
        default: kommando(program[pc++]);
      }
#ifdef DEBUG
      ptimes[opc]=(int)((clock()-timer)/1000);  /* evaluiert die
                                                   Ausfuehrungszeit der Programmzeile */
#endif
    }
}


main(int anzahl, char *argumente[]) {
  char buffer[MAXSTRLEN],*zw; 
 
   programbuffer=NULL;
  
#ifdef CONTROL  
  cs_init();        /* Kontrollsystem anmelden */
#endif
  /* Signal- und Interrupt-Handler installieren  */
  obh=signal(SIGINT, break_handler);
  signal(SIGILL, fatal_error_handler);
  signal(SIGSEGV, fatal_error_handler);
  signal(SIGBUS, fatal_error_handler);
  signal(SIGALRM, timer_handler);
  set_input_mode(1,0);  /* Terminalmode auf noncanonical, no echo */
  atexit(reset_input_mode);
  
  if(anzahl<2) {    /* Kommandomodus */
    intro();
    usage();
    batch=0;
  } else {
    kommandozeile(anzahl, argumente);    /* Kommandozeile bearbeiten */
    if(loadfile) {
      if(exist(ifilename)) {
        loadprg(ifilename);
	if (runfile) c_run("");     
      } else printf("ERROR: %s nicht gefunden !\n",ifilename);
    
    }
  } 
  
  /* Programmablaufkontrolle  */
  for(;;) {
    programmlauf();
    echo=0;batch=0;
    zw=do_gets("> ");
    if(zw==NULL) {
      c_quit("");
    } else {
      strcpy(buffer,zw);  
      kommando(buffer);
    }
  }
}
