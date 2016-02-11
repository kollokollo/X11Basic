/* XBASIC.C 

   (c) Markus Hoffmann 


    X     X    1   1        BBBB
    X     X   11  11        B   B                   i
      X X      1   1        B   B            sss
       X       1   1 -----  BBBB    aaaa    s   s   i   cccc
      X X      1   1        B   B  a   a      s     i  c
    X     X    1   1        B   B  a   a    s   s   i  c   
    X     X    1   1        BBBB    aaa a    sss    i   cccc
    
    
    
                       VERSION 1.07

            (C) 1997-2002 by Markus Hoffmann
                 (m.hoffmann@uni-bonn.de)
       (http://www-cip.physik.uni-bonn.de/~hoffmann/)
   
 **  Erstellt: Aug. 1997   von Markus Hoffmann				   **
*/
 /* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ====================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#include "version.h"
#include "defs.h"
#include "options.h"
#include "protos.h"
#include "kommandos.h"
#include "gkommandos.h"
#include "globals.h"


const char libversion[]=VERSION;           /* Programmversion           */
const char libvdate[]=VERSION_DATE;
extern const char version[];
extern const char libvdate[];

#ifdef CONTROL
const char xbasic_name[]="csxbasic";
#else
#ifdef TINE
const char xbasic_name[]="tinexbasic";
#else
const char xbasic_name[]="xbasic";
#endif
#endif
int pc=0,sp=0,echo=0,batch=0,errcont=0,breakcont=0,everyflag=0;
int everytime=0,alarmpc=-1;

int stack[STACKSIZE];

/* fuer die Dateiverwaltung     */
FILE *dptr[ANZFILENR];
int filenr[ANZFILENR];


VARIABLE variablen[ANZVARS];
int anzvariablen=0;
LABEL labels[ANZLABELS];
int anzlabels=0;
PROCEDURE procs[ANZPROCS];
int anzprocs=0;
int usewindow=DEFAULTWINDOW;

/* Kommandoliste: muss alphabetisch sortiert sein !   */

const COMMAND comms[]= {

 { P_ARGUMENT,  "!nulldummy", bidnm ,0,0   ,{0}},
 { P_ARGUMENT,  "?"         , c_print ,0,-1 ,{0}},
    
 { P_ARGUMENT,   "ADD"      , c_add       ,2, 2,{PL_ADD,PL_ADD}},
 { P_ARGUMENT,   "AFTER"    , c_after     ,2, 2,{PL_NUMBER,PL_PROC}},
 { P_PLISTE,     "ALERT"    , c_alert     ,5, 6,{PL_INT,PL_STRING,PL_INT,PL_STRING,PL_NVAR,PL_SVAR}},
 { P_ARGUMENT,   "ARRAYCOPY", c_arraycopy ,2, 2,{PL_ARRAY,PL_ARRAY}},
 { P_ARGUMENT,   "ARRAYFILL", c_arrayfill ,2, 2,{PL_ARRAY,PL_VALUE}},

 { P_SIMPLE,     "BEEP"     , c_beep      ,0, 0},
 { P_SIMPLE,     "BELL"     , c_beep      ,0, 0},
 { P_ARGUMENT,   "BGET"     , c_bget      ,3, 3,{PL_FILENR,PL_INT,PL_INT}},
 { P_ARGUMENT,   "BLOAD"    , c_bload     ,2, 3,{PL_STRING,PL_INT,PL_INT}},
 { P_ARGUMENT,   "BMOVE"    , c_bmove     ,3, 3, {PL_INT,PL_INT,PL_INT} }, 
 { P_PLISTE,     "BOX"      , c_box       ,4, 4, {PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_ARGUMENT,   "BPUT"     , c_bput      ,3, 3,{PL_FILENR,PL_INT,PL_INT}},
 { P_BREAK,      "BREAK"    , c_break     ,0, 0}, 
 { P_ARGUMENT,   "BSAVE"    , c_bsave     ,3, 3,{PL_STRING,PL_INT,PL_INT}},

 { P_ARGUMENT,   "CALL"     , c_exec,1,-1,{PL_INT}},
 { P_CASE,       "CASE"     , c_case   ,1,1,{PL_NUMBER}},
 { P_ARGUMENT,   "CHAIN"    , c_chain  ,1,1,{PL_STRING}},
 { P_PLISTE,     "CIRCLE"   , c_circle ,3,5,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_ARGUMENT,   "CLEAR"    , c_clear  ,0,-1},
 { P_ARGUMENT,   "CLEARW"   , c_clearw ,0,1,{PL_INT}},
 { P_ARGUMENT,   "CLOSE"    , c_close  ,0,-1,{PL_FILENR}},
 { P_ARGUMENT,   "CLOSEW"   , c_closew ,0,1,{PL_INT}},
 { P_ARGUMENT,   "CLR"      , c_clr    ,0,-1,{PL_ALLVAR}},
 { P_SIMPLE, "CLS"      , c_cls    ,0,0},
 { P_PLISTE,   "COLOR"    , c_color  ,1,2,{PL_INT,PL_INT}},
 { P_SIMPLE, "CONT"     , c_cont   ,0,0},
 { P_PLISTE, "COPYAREA"     , c_copyarea   ,6,6,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
/* Kontrollsystembefehle  */
#ifdef CONTROL 
 { P_ARGUMENT,   "CSPUT"    , c_csput ,2,-1,{PL_STRING,PL_VALUE}},
 { P_SIMPLE, "CSCLEARCALLBACKS"    , c_csclearcallbacks,0,0},
 { P_ARGUMENT,   "CSSET"    , c_csput,2,-1,{PL_STRING,PL_VALUE}},
 { P_ARGUMENT,   "CSSETCALLBACK", c_cssetcallback,2,-1},
 { P_ARGUMENT,   "CSSWEEP"  , c_cssweep,2,-1},
 { P_ARGUMENT,   "CSVPUT"   , c_csvput,2,-1},
#endif

 { P_DATA,     "DATA"     , c_nop ,0,-1 },
 { P_ARGUMENT, "DEC"      , c_dec, 1,1,{PL_NVAR}},
 { P_DEFAULT,  "DEFAULT"  , c_case, 0,0},
 { P_PLISTE,   "DEFFILL"  , c_deffill ,1,3,{PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,   "DEFLINE"  , c_defline ,1,4,{PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_ARGUMENT,   "DEFMOUSE" , c_defmouse, 1,1,{PL_INT}},
 { P_PLISTE,   "DEFTEXT"  , c_deftext,1,4,{PL_INT,PL_NUMBER,PL_NUMBER,PL_NUMBER}},
 { P_ARGUMENT,   "DIM"      , c_dim ,1,-1},
 { P_ARGUMENT,   "DIV"      , c_div ,2,2,{PL_NVAR,PL_NUMBER}},
 { P_DO,     "DO"       , c_do  ,0,0}, 
 { P_ARGUMENT,   "DPOKE"    , c_dpoke,       2,2,{PL_INT,PL_INT}},
 { P_ARGUMENT,   "DRAW"     , c_draw ,2,-1,{PL_INT,PL_INT}},
 { P_ARGUMENT,   "DUMP"     , c_dump ,0,1,{PL_STRING}},

 { P_ARGUMENT,   "ECHO"     , c_echo ,1,1,{PL_KEY}},
 { P_SIMPLE,   "EDIT"     , c_edit ,0,0},
 { P_PLISTE,   "ELIPSE"   , c_ellipse,4,6,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,   "ELLIPSE"  , c_ellipse,4,6,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_ELSE,   "ELSE"     , bidnm  ,0,2,{PL_KEY,PL_CONDITION}},
 { P_SIMPLE, "END"      , c_end   ,0,0},
 { P_ENDPROC,"ENDFUNC"  , c_return,0,0},
 { P_ENDPROC,"ENDFUNCTION", c_return,0,0},
 { P_ENDIF,  "ENDIF"       , bidnm  ,0,0},
 { P_ENDSELECT,"ENDSELECT" , bidnm  ,0,0},
 { P_ENDSELECT,"ENDSWITCH" , bidnm  ,0,0},
 { P_ARGUMENT,   "ERASE"    , c_erase,0,-1},
 { P_ARGUMENT,   "ERROR"    , c_error,0,1},
 { P_ARGUMENT,   "EVENT"    , c_allevent,0,9},
 { P_ARGUMENT,   "EVERY"    , c_every,0,-1},
 { P_ARGUMENT,   "EXEC"     , c_exec,1,-1,{PL_INT}},
 { P_ARGUMENT,   "EXIT"     , c_exit,0,-1},
/*
 { P_ARGUMENT,   "EXPORT"     , c_export,1,2, {PL_ALL, PL_NUMBER}},  
*/
 { P_ARGUMENT,   "FFT"      , c_fft,1,-1},
 { P_PLISTE,   "FILESELECT", c_fileselect,4,4,{PL_STRING,PL_STRING,PL_STRING,PL_SVAR}},
 { P_ARGUMENT,   "FIT",        c_fit,4,10,{PL_FARRAY,PL_FARRAY}},
 { P_ARGUMENT,   "FIT_LINEAR", c_fit_linear,4,10,{PL_FARRAY,PL_FARRAY}},
 { P_ARGUMENT,   "FLUSH"    , c_flush,0,1,{PL_FILENR}},
 { P_FOR,    "FOR"      , c_for,1,-1,{PL_EXPRESSION,PL_KEY,PL_NUMBER,PL_KEY,PL_NUMBER}},
 { P_ARGUMENT,    "FREE"      , c_free,1,1,{PL_INT}},
 { P_PROC,   "FUNCTION" , c_end,1,-1,{PL_EXPRESSION}},

 { P_PLISTE,   "GET"      , c_get,5,5,{PL_INT,PL_INT,PL_INT,PL_INT,PL_SVAR}},
 { P_ARGUMENT,   "GOSUB"    , c_gosub,1,1,{PL_PROC}},
 { P_ARGUMENT,   "GOTO"     , c_goto,1,1,{PL_LABEL}},
 { P_ARGUMENT,   "GPRINT"    , c_gprint,       0,-1},
 { P_ARGUMENT,   "GRAPHMODE", c_graphmode,1,1,{PL_INT}},

 { P_ARGUMENT,   "HELP"    , c_help,0,1,{PL_EXPRESSION}},
 { P_SIMPLE, "HOME"     , c_home,0,0},

 { P_IF,     "IF"       , c_if,1,-1,{PL_CONDITION}},
 { P_ARGUMENT,   "INC"      , c_inc,1,1,{PL_NVAR}},
 { P_ARGUMENT,   "INFOW"    , c_infow,2,2,{PL_INT,PL_STRING}},
 { P_ARGUMENT,   "INPUT"    , c_input,1,-1},

 { P_ARGUMENT,   "KEYEVENT" , c_keyevent,0,8},


 { P_ARGUMENT,   "LET"      , c_let,1,-1,{PL_EXPRESSION}},
 { P_PLISTE,     "LINE"     , c_line,4,4,{PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_ARGUMENT,   "LINEINPUT", c_lineinput,1,2},
 { P_PLISTE,     "LINK"     , c_link,       2,2,{PL_FILENR,PL_STRING}},
 
 { P_SIMPLE, "LIST"     , c_list,0,0},
 { P_ARGUMENT,   "LOAD"     , c_load,1,1,{PL_STRING}},
 { P_ARGUMENT,   "LOCAL"    , c_local,1,-1},
 { P_LOOP,   "LOOP"     , bidnm,0,0}, 
 { P_ARGUMENT,   "LPOKE"    , c_lpoke,       2,2,{PL_INT,PL_INT}},

 { P_ARGUMENT,   "LTEXT"    , c_ltext,0,-1},

 { P_ARGUMENT,   "MERGE"    , c_merge,1,1,{PL_STRING}},
 { P_SIMPLE,     "MENU"    , c_menu,0,0},
 { P_ARGUMENT,   "MENUDEF"  , c_menudef,1,2},
 { P_SIMPLE,     "MENUKILL" , c_menukill,0,0},
 { P_ARGUMENT,   "MENUSET"  , c_menuset,2,2,{PL_INT,PL_INT}},
 { P_PLISTE,   "MOUSE"    , c_mouse,1,5,{PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { P_ARGUMENT,   "MOUSEEVENT" , c_mouseevent,0,6},
 { P_ARGUMENT,   "MOTIONEVENT" , c_motionevent,0,5},
 { P_ARGUMENT,   "MOVEW"    , c_movew,0,-1},
 { P_ARGUMENT,   "MUL"      , c_mul,2,2,{PL_NVAR,PL_NUMBER}},
 
 { P_SIMPLE, "NEW"      , c_new,0,0},
 { P_NEXT,   "NEXT"     , c_next,0,1},
 { P_IGNORE|P_SIMPLE, "NOOP",         c_nop,         0,0},
 { P_IGNORE|P_SIMPLE, "NOP",          c_nop,         0,0},
 { P_SIMPLE,"NOROOTWINDOW", c_norootwindow,0,0},

 { P_ARGUMENT,   "ON"       , c_on,         1,-1},
 { P_ARGUMENT,   "OPEN"     , c_open,       3,4,{PL_STRING,PL_FILENR,PL_STRING,PL_INT}},
 { P_ARGUMENT,   "OPENW"    , c_openw,      1,-1,{PL_INT}},
 { P_ARGUMENT,   "OUT"      , c_out,        2,-1,{PL_FILENR,PL_INT}},

 { P_ARGUMENT,   "PAUSE"    , c_pause,      1,1,{PL_NUMBER}},
 { P_PLISTE,     "PBOX"     , c_pbox ,      4,4,{PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "PCIRCLE"  , c_pcircle,    3,5,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "PELIPSE"  , c_pellipse,   4,6,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "PELLIPSE" , c_pellipse,   4,6,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_SIMPLE,     "PLIST"    , c_plist,      0,0},
 { P_PLISTE,     "PLOT"     , c_plot,       2,2,{PL_INT,PL_INT}},
 { P_ARGUMENT,   "POKE"     , c_poke,       2,2,{PL_INT,PL_INT}},
 { P_ARGUMENT,   "POLYFILL" , c_polyfill,   3,7},
 { P_ARGUMENT,   "POLYLINE" , c_polyline,   3,6},
 { P_ARGUMENT,   "POLYMARK" , c_polymark,   3,6},
 { P_PLISTE,     "PRBOX"    , c_prbox ,      4,4,{PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_ARGUMENT,  "PRINT"    , c_print,       0,-1},
 { P_PROC,   "PROCEDURE", c_end  ,      0,0},
 { P_IGNORE, "PROGRAM"  , c_nop  ,      0,0},
 /* Ausdruck als Message queuen
  { P_ARGUMENT,   "PUBLISH"  , c_publish, 1,2,{PL_ALL,PL_NUMBER}},
 */
 { P_PLISTE,   "PUT"  , c_put,      3,4,{PL_INT,PL_INT,PL_STRING,PL_INT}},
 { P_ARGUMENT,   "PUTBACK"  , c_unget,      1,-1},
 { P_PLISTE,   "PUT_BITMAP"  , c_put_bitmap, 5,5,{PL_STRING,PL_INT,PL_INT,PL_INT,PL_INT}},

 { P_SIMPLE, "QUIT"     , c_quit,       0,0},

 { P_PLISTE, "RANDOMIZE", c_randomize  ,      0,1,{PL_INT}},
 { P_PLISTE,     "RBOX"      , c_rbox       ,4, 4, {PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_ARGUMENT,   "READ"     , c_read,       1,-1,{PL_ALLVAR}},
 { P_ARGUMENT,   "RELSEEK"  , c_relseek,    2,2,{PL_FILENR,PL_INT}},
 { P_REM,    "REM"      , c_nop  ,      0,0},
 { P_REPEAT, "REPEAT"   , c_nop  ,      0,0},
 { P_ARGUMENT,   "RESTORE"  , c_restore,    1,1,{PL_LABEL}},
 { P_ARGUMENT,   "RETURN"   , c_return,     0,1},
 { P_SIMPLE, "ROOTWINDOW", c_rootwindow,0,0},
 { P_SIMPLE, "RSRC_FREE", c_rsrc_free,0,0},
 { P_ARGUMENT, "RSRC_LOAD", c_rsrc_load,1,1,{PL_STRING}},

 { P_SIMPLE, "RUN"      , c_run,        0,0},

 { P_ARGUMENT,   "SAVE"     , c_save,0,1,{PL_STRING}},
 { P_ARGUMENT,   "SAVESCREEN", c_savescreen,1,1,{PL_STRING}},
 { P_ARGUMENT,   "SAVEWINDOW", c_savewindow,1,1,{PL_STRING}},
 { P_ARGUMENT,   "SCOPE"    , c_scope,      1,-1},
 { P_ARGUMENT,   "SEEK"     , c_seek,       1,2,{PL_FILENR,PL_INT}},
 { P_SELECT, "SELECT"   , c_select,     1,1},
 /*
 { P_ARGUMENT,   "SEMGIVE"  , c_semgive, 1,2,{PL_NUMBER,PL_NUMBER}},
 { P_ARGUMENT,   "SEMTAKE"  , c_semtake, 1,2,{PL_NUMBER,PL_NUMBER}},
 */
 { P_ARGUMENT,	"SETFONT"  , c_setfont,    1,1,{PL_STRING}},
 { P_PLISTE,	"SETMOUSE" , c_setmouse,   2,3,{PL_INT,PL_INT,PL_INT}},
 { P_ARGUMENT,	"SGET" , c_sget,   1,1,{PL_STRING}},
 { P_ARGUMENT,  "SHM_DETACH"      , c_detatch,1,1,{PL_INT}},
 { P_ARGUMENT,  "SHM_FREE" , c_shm_free,1,1,{PL_INT}},
 { P_SIMPLE,	"SHOWPAGE" , c_vsync,      0,0},
 { P_ARGUMENT,	"SIZEW"    , c_sizew,      3,3,{PL_INT,PL_INT,PL_INT}},
 { P_ARGUMENT,  "SORT",     c_sort,        2,3,{PL_ARRAY,PL_INT,PL_IARRAY}},

 { P_ARGUMENT,	"SPLIT"    , c_wort_sep,  4,5,{PL_STRING,PL_STRING,PL_INT,PL_SVAR,PL_SVAR}},
 { P_ARGUMENT,	"SPUT"     , c_sput,      1,1,{PL_STRING}},
 { P_SIMPLE,	"STOP"     , c_stop,       0,0},
 { P_ARGUMENT,	"SUB"      , c_sub,        2,2,{PL_NVAR,PL_NUMBER}},
 { P_ARGUMENT,	"SWAP"     , c_swap,       2,2,{PL_VAR,PL_VAR}},
 { P_SELECT,	"SWITCH"   , c_select,     1,1,{PL_CONDITION}},
 { P_ARGUMENT,	"SYSTEM"   , c_system,     1,1,{PL_STRING}},

 { P_ARGUMENT,	"TEXT"     , c_text,       3,3,{PL_INT,PL_INT,PL_STRING}},
#ifdef TINE
 { P_ARGUMENT,   "TINEMONITOR", c_tinemonitor,2,-1},
 { P_ARGUMENT,   "TINEPUT"    , c_tineput ,2,-1,{PL_STRING}},
 { P_ARGUMENT,   "TINESET"    , c_tineput ,2,-1,{PL_STRING}},
#endif
 { P_ARGUMENT,	"TITLEW"    , c_titlew,    2,2,{PL_INT,PL_STRING}},
 { P_SIMPLE,	"TROFF"    , c_troff,      0,0},
 { P_SIMPLE,	"TRON"     , c_tron,       0,0},

 { P_ARGUMENT,  "UNLINK"   , c_close  ,1,-1,{PL_FILENR}},
 { P_UNTIL,	"UNTIL"    , c_until,      1,1,{PL_CONDITION}},
 { P_ARGUMENT,	"USEWINDOW", c_usewindow,  1,1,{PL_INT}},

 { P_SIMPLE,	"VERSION"  , c_version,    0,0},
 { P_ARGUMENT,	"VOID"     , c_void,       1,1,{PL_CONDITION}},
 { P_SIMPLE,	"VSYNC"    , c_vsync,      0,0},

 { P_WEND,	"WEND"     , bidnm,       0,0},
 { P_WHILE,	"WHILE"    , c_while,      1,1,{PL_CONDITION}},
 { P_ARGUMENT,	"WORT_SEP" , c_wort_sep,  4,5,{PL_STRING,PL_STRING,PL_INT,PL_SVAR,PL_SVAR}},
 
 { P_SIMPLE,	"XLOAD"    , c_xload,    0,0},
 { P_SIMPLE,	"XRUN"     , c_xrun,     0,0},

};
const int anzcomms=sizeof(comms)/sizeof(COMMAND);

#if 0
char *keys[]={"ON","OFF","IF","TO","DOWNTO","STEP","USING"};
const int anzkeys=sizeof(keys)/sizeof(char *);
#endif



int make_pliste(int pmin,int pmax,short *pliste,char *n, PARAMETER **pr){
  char w1[strlen(n)+1],w2[strlen(n)+1];
  PARAMETER *pret;
  int i=0,e=wort_sep(n,',',TRUE,w1,w2);
  int typ;
  if(pmax==-1) pret=malloc(sizeof(PARAMETER)*12);
  else pret=malloc(sizeof(PARAMETER)*pmax);
  *pr=pret;
  while(e && i<pmax) {
    pret[i].pointer=NULL; /* Default is NULL */
    pret[i].integer=0;
    pret[i].real=0.0;
      if(strlen(w1)){
        if(pliste[i]==PL_LABEL) {
          pret[i].integer=labelnr(w1);
          if(pret[i].integer==-1) {
	    error(20,w1);/* Label nicht gefunden */
            free_pliste(i,pret);
            return(-1);
	  }
	} else if(pliste[i]==PL_FILENR) {
           pret[i].integer=get_number(w1);
	   if(pret[i].integer>99 || pret[i].integer<1) {
	     error(23,"");  /* File # falsch  */
	     free_pliste(i,pret);
             return(-1);
           }
	} else if(pliste[i]==PL_PROC) {
	  char *pos2,*pos=searchchr(w1,'(');
          if(pos!=NULL) {
            pos[0]=0;pos++;
            pos2=pos+strlen(pos)-1;
            if(pos2[0]!=')') {
	      error(32,w1); /* Syntax error */
	      free_pliste(i,pret);
              return(-1);
            }
            else pos2[0]=0;
          } else pos=w1+strlen(w1);
          pret[i].integer=procnr(w1,1);
          if(pret[i].integer==-1) {
            error(19,w1); /* Procedure nicht gefunden */
	    free_pliste(i,pret);
            return(-1);
          }
	} else if(pliste[i]==PL_LEER) { 
	  pret[i].typ=PL_LEER; /* Nixtun */
	} else if(pliste[i] & PL_FLOAT) {  /* Float oder Nuber */
          pret[i].real=parser(w1);
	} else if(pliste[i]==PL_INT) {  /* Integer */
	  pret[i].integer=(int)parser(w1);
	} else if(pliste[i]==PL_STRING) {  /* String */
          STRING str=string_parser(w1);
	  pret[i].integer=str.len;
	  pret[i].pointer=str.pointer;
	} else if((pliste[i] & PL_VAR) || (pliste[i] & PL_KEY)) { ; /* Varname */
	  pret[i].pointer=malloc(strlen(w1)+1);
	  strcpy(pret[i].pointer,w1);
	  pret[i].integer=strlen(w1);
        } else puts("unknown parameter type.");
      } else pret[i].typ=PL_LEER;
      
      e=wort_sep(w2,',',TRUE,w1,w2);
     i++;
  }
  if(i<pmin) {
    error(42,""); /* Zu wenig Parameter  */
    free_pliste(i,pret);
    return(-1);
  } else if(i==pmax && e) {
    error(45,""); /* Zu viele Parameter  */
  } 
  return(i);
}
void free_pliste(int anz,PARAMETER *pret){
  int i;
  for(i=0;i<anz;i++) {
    if(pret[i].pointer!=NULL) free(pret[i].pointer);
  }
  if(pret!=NULL) free(pret);
}

void loadprg(char *filename) {
  programbufferlen=prglen=pc=sp=0;
  mergeprg(filename);
}

extern char ifilename[];

int saveprg(char *fname) {
  char *buf=malloc(programbufferlen);
  int i=0;
  while(i<programbufferlen) {
    if(programbuffer[i]==0 || programbuffer[i]=='\n') 
      buf[i]='\n';
    else   
      buf[i]=programbuffer[i];
    i++;
  }
  bsave(fname,buf,programbufferlen);

  return(0);
}

int mergeprg(char *fname) {
  int i,len;
  char *pos;  
  FILE *dptr;

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
  return(init_program());
}
void structure_warning(char *comment) {
#ifdef GERMAN
  printf("Warnung: Programmstruktur fehlerhaft bei %s.\n",comment);
#else
  printf("Warning: corrupt program structure ==> %s.\n",comment);
#endif
}
int init_program() {
  char *pos,*pos2,*pos3,*buffer=NULL,*zeile=NULL;  int i,typ;

  clear_parameters();
  clear_labelliste();
  clear_procliste();
   
  /* Label- und Procedurliste Erstellen und p_code transformieren*/
  
  for(i=0; i<prglen;i++) {
    zeile=realloc(zeile,strlen(program[i])+1);
    buffer=realloc(buffer,strlen(program[i])+1);
 
    
    strcpy(zeile, program[i]);
    pcode[i].panzahl=0;
    pcode[i].ppointer=NULL;
    pcode[i].argument=NULL;

    wort_sep2(zeile," !",TRUE,zeile,buffer);  /*Kommentare abseparieren*/
    xtrim(zeile,TRUE,zeile);
#ifdef DEBUG
    printf("Zeile %d : %s\n",i,zeile);
#endif
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
          if(pos3[0]!=')') puts("Syntax error ==> Parameterliste");
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
              if(pcode[i].integer==-1)  structure_warning(zeile); /*Programmstruktur fehlerhaft */ 
	    } else  if(strcmp(zeile,"WEND")==0) { /*Zugehoeriges WHILE suchen */
              pcode[i].integer=suchep(i-1,-1,P_WHILE,P_WEND,P_WHILE); 
              if(pcode[i].integer==-1)  structure_warning(zeile); /*Programmstruktur fehlerhaft */ 
	    } else  if(strcmp(zeile,"NEXT")==0) { /*Zugehoeriges FOR suchen */
              pcode[i].integer=suchep(i-1,-1,P_FOR,P_NEXT,P_FOR); 
              if(pcode[i].integer==-1)  structure_warning(zeile); /*Programmstruktur fehlerhaft */ 
	    } else  if(strcmp(zeile,"UNTIL")==0) { /*Zugehoeriges REPEAT suchen */
              pcode[i].integer=suchep(i-1,-1,P_REPEAT,P_UNTIL,P_REPEAT); 
              if(pcode[i].integer==-1)  structure_warning(zeile); /*Programmstruktur fehlerhaft */ 
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
#ifdef DEBUG
  puts("PASS 2:");
#endif
  /* Pass 2 */
  for(i=0; i<prglen;i++) {
  
    if((pcode[i].opcode&PM_SPECIAL)==P_ELSE) { /* Suche Endif */
      pcode[i].integer=suchep(i+1,1,P_ENDIF,P_IF,P_ENDIF)+1; 
      if(pcode[i].integer==0)  structure_warning("ELSE"); /*Programmstruktur fehlerhaft */ 
    } else if((pcode[i].opcode&PM_SPECIAL)==P_IF) { /* Suche Endif */
      pcode[i].integer=suchep(i+1,1,P_ENDIF,P_IF,P_ENDIF)+1; 
      if(pcode[i].integer==0)  structure_warning("IF"); /*Programmstruktur fehlerhaft */ 
    } else if((pcode[i].opcode&PM_SPECIAL)==P_WHILE) { /* Suche WEND */
      pcode[i].integer=suchep(i+1,1,P_WEND,P_WHILE,P_WEND)+1; 
      if(pcode[i].integer==0)  structure_warning("WHILE"); /*Programmstruktur fehlerhaft */ 
    } else if((pcode[i].opcode&PM_SPECIAL)==P_FOR) { /* Suche NEXT */
      pcode[i].integer=suchep(i+1,1,P_NEXT,P_FOR,P_NEXT)+1; 
      if(pcode[i].integer==0)  structure_warning("FOR"); /*Programmstruktur fehlerhaft */ 
    }
  }
  free(buffer);free(zeile); 
  return(0);
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
   if(isdigit(w1[0]) || w1[0]=='(') {
     printf("%.13g\n",parser(zeile));
     return;  
  } else if(w1[0]=='&') {
    char *test,*test2;
    test=indirekt2(w1);
    test2=malloc(strlen(test)+1+1+strlen(w2));
    strcpy(test2,test);
    free(test);
    strcat(test2," ");
    strcat(test2,w2);  
    kommando(test2);
    free(test2);
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
    if(strcmp(w1,comms[i].name)==0) {
      if(comms[i].opcode & P_IGNORE) return;
      if(comms[i].opcode==P_ARGUMENT) (comms[i].routine)(w2);
      else if(comms[i].opcode==P_SIMPLE) (comms[i].routine)();
      else if(comms[i].opcode==P_PLISTE) {
        PARAMETER *plist;
        int e=make_pliste(comms[i].pmin,comms[i].pmax,(short *)comms[i].pliste,w2,&plist);
        (comms[i].routine)(plist,e);
	if(e!=-1) free_pliste(e,plist);
      } else error(38,w1); /* Befehl im Direktmodus nicht moeglich */
      return;
    } 
  } 
#ifdef GERMAN
  printf("Unbekannter Befehl: <%s> <%s>\n",w1,w2);  
#else
  printf("Syntax error! unknown command <%s> <%s>\n",w1,w2);  
#endif
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
      } else if(pcode[opc].opcode&P_INVALID) error(32,program[opc]); /*Syntax nicht korrekt*/
      else if((pcode[opc].opcode&PM_COMMS)>=anzcomms) {
        puts("Precompiler error...");
        kommando(program[opc]);
      } else {
        if(pcode[opc].opcode&P_ARGUMENT)
          (comms[pcode[opc].opcode&PM_COMMS].routine)(pcode[opc].argument);
        else if(pcode[opc].opcode&P_PLISTE) {
	  PARAMETER *plist;
	  int i=pcode[opc].opcode&PM_COMMS;
          int e=make_pliste(comms[i].pmin,comms[i].pmax,(short *)comms[i].pliste,pcode[opc].argument,&plist);
          (comms[i].routine)(plist,e);
	  if(e!=-1) free_pliste(e,plist);
        } else printf("Was denn noch ?: %s\n",program[opc]);
      }
#ifdef DEBUG
      ptimes[opc]=(int)((clock()-timer)/1000);  /* evaluiert die
                                                   Ausfuehrungszeit der Programmzeile */
#endif
    }
}
