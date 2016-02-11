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

#include "version.h"
#include "protos.h"
#include "xbasic.h"


const char version[]=VERSION;           /* Programmversion           */
const char vdate[]=VERSION_DATE;
#ifdef CONTROL
const char xbasic_name[]="csxbasic";
#else
const char xbasic_name[]="x11basic";
#endif

int pc=0,sp=0,prglen=0,echo=0,batch=0,errcont=0,everyflag=0;
int everytime=0,alarmpc=-1;
char ifilename[100]="input.bas";       /* Standartfile             */
int loadfile=FALSE;
int stack[STACKSIZE];
/*int param_anzahl; */             /* fuer PARAM$()*/
/*char **param_argumente;*/

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

 { P_ARGUMENT,  "!nulldummy", bidnm ,0,0   ,{0}},
 { P_ARGUMENT,  "?"         , c_print ,0,-1 ,{0}},
    
 { P_ARGUMENT,   "ADD"      , c_add       ,2, 2,{PL_ADD,PL_ADD}},
 { P_ARGUMENT,   "AFTER"    , c_after     ,2, 2 },
 { P_ARGUMENT,   "ALERT"    , c_alert     ,5, 5,{PL_NUMBER,PL_STRING,PL_NUMBER,PL_STRING,PL_NVAR}},
 { P_ARGUMENT,   "ARRAYCOPY", c_arraycopy ,2, 2},
 { P_ARGUMENT,   "ARRAYFILL", c_arrayfill ,2, 2},

 { P_SIMPLE, "BEEP"     , c_beep      ,0, 0},
 { P_SIMPLE, "BELL"     , c_beep      ,0, 0},
 { P_ARGUMENT,   "BGET"     , c_bget      ,1,-1},
 { P_ARGUMENT,   "BLOAD"    , c_bload     ,1,-1},
 { P_ARGUMENT,   "BMOVE"    , c_bmove     ,1,-1}, 
 { P_ARGUMENT,   "BOX"      , c_box       ,4, 4},
 { P_ARGUMENT,   "BPUT"     , c_bput      ,1,-1},
 { P_BREAK,  "BREAK"    , c_break     ,0, 0}, 
 { P_ARGUMENT,   "BSAVE"    , c_bsave     ,1,-1},

 { P_CASE,   "CASE"     , c_case   ,1,1},
 { P_ARGUMENT,   "CHAIN"    , c_chain  ,1,1},
 { P_ARGUMENT,   "CIRCLE"   , c_circle ,3,4},
 { P_ARGUMENT,   "CLEAR"    , c_clear  ,0,-1},
 { P_ARGUMENT,   "CLEARW"   , c_clearw ,0,1},
 { P_ARGUMENT,   "CLOSE"    , c_close  ,0,-1},
 { P_ARGUMENT,   "CLOSEW"   , c_closew ,0,1},
 { P_ARGUMENT,   "CLR"      , c_clr    ,0,-1},
 { P_SIMPLE, "CLS"      , c_cls    ,0,0},
 { P_ARGUMENT,   "COLOR"    , c_color  ,1,1},
 { P_SIMPLE, "CONT"     , c_cont   ,0,0},
/* Kontrollsystembefehle  */
#ifdef CONTROL 

 { P_ARGUMENT,   "CSPUT"    , c_csput ,1,-1},
 { P_SIMPLE, "CSCLEARCALLBACKS"    , c_csclearcallbacks,0,0},
 { P_ARGUMENT,   "CSSET"    , c_csput,1,-1},
 { P_ARGUMENT,   "CSSETCALLBACK", c_cssetcallback,1,-1},
 { P_ARGUMENT,   "CSSWEEP"  , c_cssweep,2,-1},
 { P_ARGUMENT,   "CSVPUT"   , c_csvput,1,-1},
#endif

 { P_DATA,   "DATA"     , c_nop ,0,-1 },
 { P_ARGUMENT,   "DEC"      , c_dec, 1,1},
 { P_DEFAULT,"DEFAULT"  , c_case, 0,0},
 { P_ARGUMENT,   "DEFFILL"  , c_deffill ,1,-1},
 { P_ARGUMENT,   "DEFLINE"  , c_defline ,1,-1},
 { P_ARGUMENT,   "DEFMOUSE" , c_defmouse, 1,1},
 { P_ARGUMENT,   "DEFTEXT"  , c_deftext,1,-1},
 { P_ARGUMENT,   "DIM"      , c_dim ,1,-1},
 { P_ARGUMENT,   "DIV"      , c_div ,2,2},
 { P_DO,     "DO"       , c_do  ,0,0},
 { P_ARGUMENT,   "DRAW"     , c_draw ,2,-1},

 { P_ARGUMENT,   "DUMP"     , c_dump ,0,1},

 { P_ARGUMENT,   "ECHO"     , c_echo ,1,1},
 { P_ARGUMENT,   "ELIPSE"     , c_ellipse,4,5},
 { P_ARGUMENT,   "ELLIPSE"  , c_ellipse,4,5},
 { P_ELSE,   "ELSE"     , bidnm  ,0,2},
 { P_SIMPLE, "END"      , c_end   ,0,0},
 { P_ENDPROC,"ENDFUNC"  , c_return,0,0},
 { P_ENDPROC,"ENDFUNCTION", c_return,0,0},
 { P_ENDIF,  "ENDIF"       , bidnm  ,0,0},
 { P_ENDSELECT,"ENDSELECT" , bidnm  ,0,0},
 { P_ENDSELECT,"ENDSWITCH" , bidnm  ,0,0},
 { P_ARGUMENT,   "ERASE"    , c_erase,0,-1},
 { P_ARGUMENT,   "ERROR"    , c_error,0,1},
 { P_ARGUMENT,   "EVENT"    , c_allevent,0,-1},
 { P_ARGUMENT,   "EVERY"    , c_every,0,-1},
 { P_ARGUMENT,   "EXIT"     , c_exit,0,-1},
/*
 { P_ARGUMENT,   "EXPORT"     , c_export,1,2, {PL_ALL, PL_NUMBER}},  
*/
 { P_ARGUMENT,   "FFT"      , c_fft,0,-1},
 { P_ARGUMENT,   "FILESELECT", c_fileselect,3,-1},
 { P_ARGUMENT,   "FLUSH"    , c_flush,0,-1},
 { P_FOR,    "FOR"      , c_for,1,-1},
 { P_PROC,   "FUNCTION" , c_end,1,-1},

 { P_ARGUMENT,   "GOSUB"    , c_gosub,1,1},
 { P_ARGUMENT,   "GOTO"     , c_goto,1,1},
 { P_ARGUMENT,   "GRAPHMODE", c_graphmode,1,1},

 { P_SIMPLE, "HOME"     , c_home,0,0},

 { P_IF,     "IF"       , c_if,1,-1},
 { P_ARGUMENT,   "INC"      , c_inc,1,1},
 { P_ARGUMENT,   "INFOW"    , c_infow,2,2},
 { P_ARGUMENT,   "INPUT"    , c_input,1,-1},

 { P_ARGUMENT,   "KEYEVENT" , c_keyevent,0,-1},


 { P_ARGUMENT,   "LET"      , c_let,1,-1},
 { P_ARGUMENT,   "LINE"     , c_line,4,4},
 { P_ARGUMENT,   "LINEINPUT", c_lineinput,1,2},
 
 { P_SIMPLE, "LIST"     , c_list,0,0},
 { P_ARGUMENT,   "LOAD"     , c_load,1,1},
 { P_ARGUMENT,   "LOCAL"    , c_local,1,-1},
 { P_LOOP,   "LOOP"     , bidnm,0,0},
 { P_ARGUMENT,   "LTEXT"    , c_ltext,0,-1},

 { P_ARGUMENT,   "MERGE"    , c_merge,1,1},
 { P_ARGUMENT,   "MOUSE"    , c_mouse,1,4},
 { P_ARGUMENT,   "MOUSEEVENT" , c_mouseevent,0,-1},
 { P_ARGUMENT,   "MOTIONEVENT" , c_motionevent,0,-1},
 { P_ARGUMENT,   "MOVEW"    , c_movew,0,-1},
 { P_ARGUMENT,   "MUL"      , c_mul,2,2},
 
 { P_SIMPLE, "NEW"      , c_new,0,0},
 { P_NEXT,   "NEXT"     , c_next,0,1},
 { P_IGNORE|P_SIMPLE, "NOOP",         c_nop,         0,0},
 { P_IGNORE|P_SIMPLE, "NOP",          c_nop,         0,0},
 { P_SIMPLE,"NOROOTWINDOW", c_norootwindow,0,0},

 { P_ARGUMENT,   "ON"       , c_on,         1,-1},
 { P_ARGUMENT,   "OPEN"     , c_open,       1,-1},
 { P_ARGUMENT,   "OPENW"    , c_openw,      1,-1},
 { P_ARGUMENT,   "OUT"      , c_out,        2,-1},

 { P_ARGUMENT,   "PAUSE"    , c_pause,      1,1},
 { P_ARGUMENT,   "PBOX"     , c_pbox ,      4,4},
 { P_ARGUMENT,   "PCIRCLE"  , c_pcircle,    3,4},
 { P_ARGUMENT,   "PELLIPSE"  , c_pellipse,   4,5},
 { P_SIMPLE, "PLIST"    , c_plist,      0,0},
 { P_ARGUMENT,   "PLOT"     , c_plot,       2,2},
 { P_ARGUMENT,  "PRINT"    , c_print,       0,-1},
 { P_PROC,   "PROCEDURE", c_end  ,      0,0},
 { P_IGNORE, "PROGRAM"  , c_nop  ,      0,0},
 /* Ausdruck als Message queuen
  { P_ARGUMENT,   "PUBLISH"  , c_publish, 1,2,{PL_ALL,PL_NUMBER}},
 */
 { P_ARGUMENT,   "PUTBACK"  , c_unget,      1,2},

 { P_SIMPLE, "QUIT"     , c_quit,       0,0},

 { P_IGNORE, "RANDOMIZE", c_nop  ,      0,0},
 { P_ARGUMENT,   "READ"     , c_read,       1,-1},
 { P_ARGUMENT,   "RELSEEK"  , c_relseek,    2,2},
 { P_REM,    "REM"      , c_nop  ,      0,0},
 { P_REPEAT, "REPEAT"   , c_nop  ,      0,0},
 { P_ARGUMENT,   "RESTORE"  , c_restore,    1,1},
 { P_ARGUMENT,   "RETURN"   , c_return,     0,1},
 { P_SIMPLE, "ROOTWINDOW", c_rootwindow,0,0},
 { P_SIMPLE, "RUN"      , c_run,        0,0},

 { P_ARGUMENT,   "SAVESCREEN", c_savescreen,1,1},
 { P_ARGUMENT,   "SAVEWINDOW", c_savewindow,1,1},
 { P_ARGUMENT,   "SCOPE"    , c_scope,      1,-1},
 { P_ARGUMENT,   "SEEK"     , c_seek,       1,-1},
 { P_SELECT, "SELECT"   , c_select,     1,1},
 /*
 { P_ARGUMENT,   "SEMGIVE"  , c_semgive, 1,2,{PL_NUMBER,PL_NUMBER}},
 { P_ARGUMENT,   "SEMTAKE"  , c_semtake, 1,2,{PL_NUMBER,PL_NUMBER}},
 */
 { P_ARGUMENT,	"SETFONT"  , c_setfont,    1,1},
 { P_ARGUMENT,	"SETMOUSE" , c_setmouse,   1,-1},
 { P_SIMPLE,	"SHOWPAGE" , c_vsync,      0,0},
 { P_ARGUMENT,	"SIZEW"    , c_sizew,      1,-1},
 { P_SIMPLE,	"STOP"     , c_stop,       0,0},
 { P_ARGUMENT,	"SUB"      , c_sub,        2,2},
 { P_ARGUMENT,	"SWAP"     , c_swap,       2,2},
 { P_SELECT,	"SWITCH"   , c_select,     1,1},
 { P_ARGUMENT,	"SYSTEM"   , c_system,     1,1},

 { P_ARGUMENT,	"TEXT"     , c_text,       3,3},
 { P_ARGUMENT,	"TITLEW"    , c_titlew,    2,2},
 { P_SIMPLE,	"TROFF"    , c_troff,      0,0},
 { P_SIMPLE,	"TRON"     , c_tron,       0,0},

 { P_UNTIL,	"UNTIL"    , c_until,      1,1},
 { P_ARGUMENT,	"USEWINDOW", c_usewindow,  1,1},

 { P_SIMPLE,	"VERSION"  , c_version,    0,0},
 { P_ARGUMENT,	"VOID"     , c_void,       1,1},
 { P_SIMPLE,	"VSYNC"    , c_vsync,      0,0},

 { P_WEND,	"WEND"     , bidnm,       0,0},
 { P_WHILE,	"WHILE"    , c_while,      1,1},
 { P_ARGUMENT,	"WORT_SEP" , c_wort_sep,  2,-1},
 
 { P_SIMPLE,	"XLOAD"    , c_xload,    0,0},
 { P_SIMPLE,	"XRUN"     , c_xrun,     0,0},


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
      printf("Zeile  pc-1   : %s\n",program[pc-2]);
      printf("Zeile: Pc=%d  : %s\n",pc-1,program[pc-1]);
      printf("Zeile  pc+1   : %s\n",program[pc]);

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


P_CODE make_pcode(char *n) {
  int oa,i,a,b;
  char w1[strlen(n)+1],w2[strlen(n)+1];

  /* Kommandoliste durchsuchen, moeglichst effektiv ! */

  i=0;a=anzcomms;
  for(b=0; b<strlen(w1); b++) {
    while(w1[b]>(comms[i].name)[b] && i<a) i++;
    oa=a;a=i;
    while(w1[b]<(comms[a].name)[b]+1 && a<oa) a++;
    if(i==a) break;
  }
  if(i<anzcomms) {
    if(strcmp(w1,comms[i].name)==0) {(comms[i].routine)(w2);return;}
  }   
  

}

int mergeprg(char *fname) {
  char *pos,*pos2,*pos3,*buffer=NULL,*zeile=NULL;  int i,len,typ;  FILE *dptr;

  /* Filelaenge rauskriegen */

  dptr=fopen(fname,"r"); len=lof(dptr); fclose(dptr);
  programbuffer=realloc(programbuffer,programbufferlen+len+1);
  bload(fname,programbuffer+programbufferlen,len);
  programbufferlen+=len;
  clear_parameters();

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
    pcode[i].panzahl=0;
    pcode[i].ppointer=NULL;
    pcode[i].argument=NULL;

    wort_sep2(zeile," !",TRUE,zeile,buffer);  /*Kommentare abseparieren*/
    xtrim(zeile,TRUE,zeile);
    if(wort_sep(zeile,' ',TRUE,zeile,buffer)==0) pcode[i].opcode=P_IGNORE|P_NOCMD;	      
    else if(zeile[0]=='\'' || zeile[0]=='#') {
      pcode[i].opcode=P_REM;
      pcode[i].argument=malloc(strlen(buffer)+1);
      strcpy(pcode[i].argument,buffer);
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
       
        for(j=0;j<anzcomms; j++) {
	  if(strcmp(zeile,comms[j].name)==0) {
	    pcode[i].opcode=comms[j].opcode|j;
	    if(comms[j].pmax==0 || (pcode[i].opcode & P_SIMPLE)) {
	      pcode[i].panzahl=0;
	      pcode[i].ppointer=NULL;
	    } else {
	      int e,ii=0;
	      char w1[strlen(buffer)+1],w2[strlen(buffer)+1];
	      
	      xtrim(buffer,TRUE,buffer); /* hier Parameter abseparieren */
	      pcode[i].argument=malloc(strlen(buffer)+1);
	      strcpy(pcode[i].argument,buffer);
	      
	      /* Parameter finden */
	      e=wort_sep(buffer,',',TRUE,w1,w2);
              while(e) {
 	        e=wort_sep(w2,',',TRUE,w1,w2);
                ii++;
              }
	      if((comms[j].pmin>ii && comms[j].pmin!=-1) || (comms[j].pmax<ii && comms[j].pmax!=-1))  printf("Warnung: Z.%d Falsche Anzahl Parameter.\n",i); /*Programmstruktur fehlerhaft */ 
	      pcode[i].panzahl=ii;
	      if(ii==0) {
	        pcode[i].ppointer=NULL;
	      } else {
	        pcode[i].ppointer=malloc(sizeof(PARAMETER)*ii);
		
                e=wort_sep(buffer,',',TRUE,w1,w2); ii=0;
		while(e) {
		  
	          if(strlen(w1)) {
                    pcode[i].ppointer[ii].typ=PL_EVAL; 
		    pcode[i].ppointer[ii].pointer=malloc(strlen(w1)+1);
		    strcpy(pcode[i].ppointer[ii].pointer,w1);

     		  } else  {
		    pcode[i].ppointer[ii].typ=PL_LEER;
		    pcode[i].ppointer[ii].pointer=NULL;
		  }
		  e=wort_sep(w2,',',TRUE,w1,w2);
                  ii++;
                }
		
	      }

	       
	    }
	    /* Einige Befehle noch nachbearbeiten */
	    if(strcmp(zeile,"LOOP")==0) { /*Zugehoeriges Do suchen */
	      pcode[i].integer=suchep(i-1,-1,P_DO,P_LOOP,P_DO); 
              if(pcode[i].integer==-1)  printf("Warnung: LOOP: Programmstruktur fehlerhaft.\n"); /*Programmstruktur fehlerhaft */ 
	    } else  if(strcmp(zeile,"WEND")==0) { /*Zugehoeriges WHILE suchen */
              pcode[i].integer=suchep(i-1,-1,P_WHILE,P_WEND,P_WHILE); 
              if(pcode[i].integer==-1)  printf("Warnung: WEND: Programmstruktur fehlerhaft.\n"); /*Programmstruktur fehlerhaft */ 
	    } else  if(strcmp(zeile,"NEXT")==0) { /*Zugehoeriges FOR suchen */
              pcode[i].integer=suchep(i-1,-1,P_FOR,P_NEXT,P_FOR); 
              if(pcode[i].integer==-1)  printf("Warnung: NEXT: Programmstruktur fehlerhaft.\n"); /*Programmstruktur fehlerhaft */ 
	    } else  if(strcmp(zeile,"UNTIL")==0) { /*Zugehoeriges REPEAT suchen */
              pcode[i].integer=suchep(i-1,-1,P_REPEAT,P_UNTIL,P_REPEAT); 
              if(pcode[i].integer==-1)  printf("Warnung: UNTIL: Programmstruktur fehlerhaft.\n"); /*Programmstruktur fehlerhaft */ 
	    }
	    break;
	  }
	}
	if(j==anzcomms) { /* Kein Befehl passt... */
	  pcode[i].opcode=P_EVAL|P_NOCMD;
	  pcode[i].panzahl=0;
	  pcode[i].ppointer=NULL;
	  pcode[i].argument=malloc(strlen(buffer)+1);
	  strcpy(pcode[i].argument,buffer);
	 /* printf("Warnung: Zeile %d Unbek. Befehl: <%s>\n",i,zeile);*/
	}
      }
    }
    
  } 
  /* Pass 2 */
  for(i=0; i<prglen;i++) {
  
    if((pcode[i].opcode&PM_SPECIAL)==P_ELSE) { /* Suche Endif */
      pcode[i].integer=suchep(i+1,1,P_ENDIF,P_IF,P_ENDIF)+1; 
      if(pcode[i].integer==0)  printf("Warnung: ELSE: Programmstruktur fehlerhaft.\n"); /*Programmstruktur fehlerhaft */ 
    } else if((pcode[i].opcode&PM_SPECIAL)==P_IF) { /* Suche Endif */
      pcode[i].integer=suchep(i+1,1,P_ENDIF,P_IF,P_ENDIF)+1; 
      if(pcode[i].integer==0)  printf("Warnung: IF: Programmstruktur fehlerhaft.\n"); /*Programmstruktur fehlerhaft */ 
    } else if((pcode[i].opcode&PM_SPECIAL)==P_WHILE) { /* Suche WEND */
      pcode[i].integer=suchep(i+1,1,P_WEND,P_WHILE,P_WEND)+1; 
      if(pcode[i].integer==0)  printf("Warnung: WHILE: Programmstruktur fehlerhaft.\n"); /*Programmstruktur fehlerhaft */ 
    } else if((pcode[i].opcode&PM_SPECIAL)==P_FOR) { /* Suche NEXT */
      pcode[i].integer=suchep(i+1,1,P_NEXT,P_FOR,P_NEXT)+1; 
      if(pcode[i].integer==0)  printf("Warnung: FOR: Programmstruktur fehlerhaft.\n"); /*Programmstruktur fehlerhaft */ 
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
void clear_parameters() {
  int i;
  if(prglen) {
    for(i=0;i<prglen;i++) {
      if(pcode[i].ppointer!=NULL) {
        free(pcode[i].ppointer->pointer);
        free(pcode[i].ppointer);
      }
      free(pcode[i].argument);
    }
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
    o=pcode[i].opcode&PM_SPECIAL;
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
  ergebnis=malloc(4);
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
  } else  printf("Unbekannter Befehl: <%s> <%s>\n",w1,w2);
}


void programmlauf(){
    int isp,ipc,opc;
#ifdef DEBUG
    int timer;
#endif    
    isp=sp;ipc=pc;
    while(batch && pc<prglen && pc>=0 && sp>=isp)  {
      if(echo) printf("%s\n",program[pc]);
#ifdef DEBUG      
      timer=clock();
#endif 
      opc=pc;  
      if(pcode[opc].opcode&P_PREFETCH) pc=pcode[opc].integer;
      else pc++;
      if(pcode[opc].opcode&P_IGNORE) ;
      else if(pcode[opc].opcode&P_EVAL)  kommando(program[opc]);
      else if((pcode[opc].opcode&PM_TYP)==P_SIMPLE) {
        (comms[pcode[opc].opcode&PM_COMMS].routine)(NULL);      
      } else if(pcode[opc].opcode&P_INVALID) {
        printf("Zeile %d: Syntax nicht korrekt: %s\n",ipc,program[opc]);
	batch=0;
      }
      else if((pcode[opc].opcode&PM_COMMS)>=anzcomms) {
        printf("Precompiler error...\n");
        kommando(program[opc]);
      } else {
        if(pcode[opc].opcode&P_ARGUMENT)
          (comms[pcode[opc].opcode&PM_COMMS].routine)(pcode[opc].argument);
        else if(pcode[opc].opcode&P_PLISTE)
	 (comms[pcode[opc].opcode&PM_COMMS].routine)(pcode[opc].ppointer);
        else printf("Was denn noch ?: %s\n",program[opc]);
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
  param_anzahl=anzahl;
  param_argumente=argumente;
  
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
    if(daemonf) zw=simple_gets("");
    else zw=do_gets("> ");
    if(zw==NULL) {
      c_quit("");
    } else {
      strcpy(buffer,zw);  
      kommando(buffer);
    }
  }
}
