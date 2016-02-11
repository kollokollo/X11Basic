/* XBASIC.C

   (c) Markus Hoffmann


   ##     ##   #   #        ######
    ##   ##   ##  ##        ##   ##                    ##
     ## ##   ### ###        ##   ##
      ###     ##  ## #####  #####     #####    #####  ####   ####
     ## ##    ##  ##        ##   ##       ##  ##       ##   ##  ##
    ##   ##   ##  ##        ##   ##   ######   ####    ##   ##
   ##     ##  ##  ##        ##   ##  ##   ##      ##   ##   ##  ##
   ##     ##  ##  ##        ######    ######  #####   ####   ####



                       VERSION 1.19

            (C) 1997-2012 by Markus Hoffmann
              (kollo@users.sourceforge.net)
            (http://x11-basic.sourceforge.net/)

 **  Erstellt: Aug. 1997   von Markus Hoffmann				   **
 **  letzte Bearbeitung: Feb. 2003   von Markus Hoffmann		   **
 **  letzte Bearbeitung: Feb. 2005   von Markus Hoffmann		   **
 **  letzte Bearbeitung: Feb. 2007   von Markus Hoffmann		   **
 **  letzte Bearbeitung: Jan. 2008   von Markus Hoffmann		   **
 **  letzte Bearbeitung: Dez. 2010   von Markus Hoffmann		   **
*/

 /* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
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

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "parameter.h"
#include "xbasic.h"
#include "kommandos.h"
#include "gkommandos.h"
#include "io.h"
#include "file.h"
#include "array.h"
#include "parser.h"
#include "x11basic.h"
#include "wort_sep.h"
#include "bytecode.h"
#include "virtual-machine.h"


const char libversion[]=VERSION;           /* Programmversion           */
const char libvdate[]=VERSION_DATE;
extern const char version[];
extern const char libvdate[];

#ifdef CONTROL
const char xbasic_name[]="csxbasic";
#else
#ifdef DOOCS
const char xbasic_name[]="doocsxbasic";
#else
#ifdef TINE
const char xbasic_name[]="tinexbasic";
#else
#ifdef USE_SDL
const char xbasic_name[]="sdlxbasic";
#else
const char xbasic_name[]="xbasic";
#endif
#endif
#endif
#endif
int pc=0,sp=0,echoflag=0,batch=0,errcont=0,breakcont=0,everyflag=0;
int errorpc=-1,breakpc=-1;
int everytime=0,alarmpc=-1;

int stack[STACKSIZE];

/* fuer die Dateiverwaltung     */
FILE *dptr[ANZFILENR];
int filenr[ANZFILENR];

P_CODE pcode[MAXPRGLEN];

UNIVAR returnvalue;

int param_anzahl;
char **param_argumente;

LABEL labels[ANZLABELS];
int anzlabels=0;
PROCEDURE procs[ANZPROCS];
int anzprocs=0;
int usewindow=DEFAULTWINDOW;

/*  Fuer DATA statements */
char *databuffer=NULL;  /* Hier werden alle DATA-Inhalte gespeichert.*/
int databufferlen=0;
int datapointer=0; 

static void clear_parameters(int l);
static void clear_labelliste();
static void clear_procliste();
static int add_label(char *name,int zeile,int dataptr);
static int add_proc(char *name,char *pars,int zeile,int typ);


/* Kommandoliste: muss alphabetisch sortiert sein !   */

const COMMAND comms[]= {

 { P_ARGUMENT,  " nulldummy", bidnm       ,0, 0,{0}},
 { P_REM,       "!"      , c_nop  ,      0,0},
 { P_PLISTE,    "?"         , c_print     ,0,-1,{PL_EVAL}},

 { P_PLISTE,   "ADD"      , c_add       ,2, 2,{PL_ANYVAR,PL_ANYVALUE}},
 { P_PLISTE,   "AFTER"    , c_after     ,2, 2,{PL_INT,PL_PROC}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "ALERT"    , c_alert     ,5, 6,{PL_INT,PL_STRING,PL_INT,PL_STRING,PL_NVAR,PL_SVAR}},
#endif
 { P_PLISTE,   "ARRAYCOPY", c_arraycopy ,2, 2,{PL_ARRAYVAR,PL_ARRAYVAR}}, /*zweiter parameter muesste "PL_ARRAY sein, nicht ARRAYVAR*/
 { P_PLISTE,   "ARRAYFILL", c_arrayfill ,2, 2,{PL_ARRAYVAR,PL_ANYVALUE}},

 { P_SIMPLE,     "BEEP"     , c_beep      ,0, 0},
 { P_SIMPLE,     "BELL"     , c_beep      ,0, 0},
 { P_PLISTE,     "BGET"     , c_bget      ,3, 3,{PL_FILENR,PL_INT,PL_INT}},
 { P_PLISTE,     "BLOAD"    , c_bload     ,2, 3,{PL_STRING,PL_INT,PL_INT}},
 { P_PLISTE,     "BMOVE"    , c_bmove     ,3, 3,{PL_INT,PL_INT,PL_INT} },
#ifndef NOGRAPHICS
 { P_PLISTE,     "BOTTOMW"  , c_bottomw,   0, 1,{PL_FILENR}},
 { P_PLISTE,     "BOUNDARY" , c_boundary  ,1, 1,{PL_INT}},
 { P_PLISTE,     "BOX"      , c_box       ,4, 4,{PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "BPUT"     , c_bput      ,3, 3,{PL_FILENR,PL_INT,PL_INT}},
 { P_BREAK,      "BREAK"    , c_break     ,0, 0},
 { P_PLISTE,     "BSAVE"    , c_bsave     ,3, 3,{PL_STRING,PL_INT,PL_INT}},

 { P_ARGUMENT,   "CALL"     , c_exec      ,1,-1,{PL_INT}},
 { P_CASE,       "CASE"     , c_case      ,1, 1,{PL_NUMBER}},
 { P_PLISTE,     "CHAIN"    , c_chain     ,1, 1,{PL_STRING}},
 { P_PLISTE,     "CHDIR"    , c_chdir     ,1, 1,{PL_STRING}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "CIRCLE"   , c_circle    ,3, 5,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,   "CLEAR"    , c_clear     ,0,-1,{PL_ALLVAR}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "CLEARW"   , c_clearw      ,0, 1,{PL_FILENR}},
 { P_PLISTE,   "CLIP"     , c_clip        ,4, 6,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,   "CLOSE"    , c_close     ,0,-1,{PL_FILENR}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "CLOSEW"   , c_closew    ,0, 1,{PL_FILENR}},
#endif
 { P_PLISTE,   "CLR"      , c_clr       ,1,-1,{PL_ALLVAR,PL_ALLVAR}},
 { P_SIMPLE,     "CLS"      , c_cls       ,0, 0},
#ifndef NOGRAPHICS
 { P_PLISTE,     "COLOR"    , c_color     ,1,2,{PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "CONNECT"  , c_connect   ,2,3,{PL_FILENR,PL_STRING,PL_INT}},
 { P_CONTINUE,     "CONTINUE" , c_cont      ,0,0},
#ifndef NOGRAPHICS
 { P_PLISTE,     "COPYAREA"     , c_copyarea   ,6,6,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
/* Kontrollsystembefehle  */
#ifdef CONTROL
 { P_ARGUMENT,   "CSPUT"    , c_csput ,2,-1,{PL_STRING,PL_VALUE}},
 { P_SIMPLE, "CSCLEARCALLBACKS"    , c_csclearcallbacks,0,0},
 { P_ARGUMENT,   "CSSET"    , c_csput,2,-1,{PL_STRING,PL_VALUE}},
 { P_ARGUMENT,   "CSSETCALLBACK", c_cssetcallback,2,-1},
 { P_ARGUMENT,   "CSSWEEP"  , c_cssweep,2,-1},
 { P_ARGUMENT,   "CSVPUT"   , c_csvput,2,-1},
#endif
 { P_PLISTE,     "CURVE"     , c_curve,8,9,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},

 { P_DATA,     "DATA"     , c_nop ,0,-1 },
 { P_PLISTE, "DEC"      , c_dec, 1,1,{PL_NVAR}},
 { P_DEFAULT,  "DEFAULT"  , c_case, 0,0},
#ifndef NOGRAPHICS
 { P_PLISTE,   "DEFFILL"  , c_deffill ,1,3,{PL_INT,PL_INT,PL_INT}},
#endif
 { P_DEFFN,    "DEFFN"     , bidnm  ,0,0},
#ifndef NOGRAPHICS
 { P_PLISTE,   "DEFLINE"  , c_defline ,1,4,{PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,   "DEFMARK"  , c_defmark,1,3,{PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,   "DEFMOUSE" , c_defmouse, 1,1,{PL_INT}},
 { P_PLISTE,   "DEFTEXT"  , c_deftext,1,4,{PL_INT,PL_NUMBER,PL_NUMBER,PL_NUMBER}},
#endif
 { P_PLISTE,   "DELAY"    , c_pause,      1,1,{PL_NUMBER}},
 { P_ARGUMENT,   "DIM"      , c_dim ,1,-1,{PL_ARRAYVAR,PL_ARRAYVAR}},
 { P_PLISTE,   "DIV"      , c_div ,2,2,{PL_NVAR,PL_NUMBER}},
 { P_DO,     "DO"       , c_do  ,0,0},
#ifdef DOOCS
/* { P_ARGUMENT,   "TINEBROADCAST", c_tinebroadcast,1,-1,{PL_STRING}},
 { P_SIMPLE,     "TINECYCLE", c_tinecycle,0,0},
 { P_ARGUMENT,   "TINEDELIVER", c_tinedeliver,1,-1},   */
 { P_ARGUMENT,   "DOOCSCALLBACK", c_doocscallback,2,3, {PL_VAR,PL_PROC,PL_PROC}},
 { P_ARGUMENT,   "DOOCSEXPORT", c_doocsexport,1,-1},
/* { P_ARGUMENT,   "TINELISTEN", c_tinelisten,1,-1,{PL_STRING}},
 { P_PLISTE,     "TINEMONITOR", c_tinemonitor,2,3,{PL_STRING,PL_PROC,PL_INT}},*/
 { P_ARGUMENT,   "DOOCSPUT"    , c_doocsput ,2,-1,{PL_STRING}},
 { P_PLISTE,     "DOOCSSERVER" , c_doocsserver,0,2,{PL_STRING,PL_INT}},
 { P_ARGUMENT,   "DOOCSSET"    , c_doocsput ,2,-1,{PL_STRING}},
#endif
 { P_PLISTE,   "DPOKE"    , c_dpoke,       2,2,{PL_INT,PL_INT}},
#ifndef NOGRAPHICS
 { P_ARGUMENT,   "DRAW"     , c_draw ,2,-1,{PL_INT,PL_INT}},
#endif
 { P_PLISTE,   "DUMP"     , c_dump ,0,1,{PL_STRING}},

 { P_PLISTE,   "ECHO"     , c_echo ,1,1,{PL_KEY}},
 { P_SIMPLE,   "EDIT"     , c_edit ,0,0},
#ifndef NOGRAPHICS
 { P_PLISTE,   "ELLIPSE"  , c_ellipse,4,6,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_ELSE,   "ELSE"     , bidnm  ,0,2,{PL_KEY,PL_CONDITION}},
 { P_SIMPLE, "END"      , c_end   ,0,0},
 { P_ENDPROC,"ENDFUNCTION", c_return,0,0},
 { P_ENDIF,  "ENDIF"       , bidnm  ,0,0},
 { P_ENDSELECT,"ENDSELECT" , bidnm  ,0,0},
 { P_PLISTE,   "ERASE"    , c_erase,1,-1,{PL_ARRAYVAR,PL_ARRAYVAR}},
 { P_PLISTE,   "ERROR"    , c_error,1,1,{PL_INT}},
 { P_PLISTE,   "EVAL"     , c_eval,1,1,{PL_STRING}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "EVENT"    , c_allevent,0,9,{PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_SVAR}},
#endif
 { P_PLISTE,   "EVERY"    , c_every,2,2,{PL_INT,PL_PROC}},
 { P_ARGUMENT,   "EXEC"     , c_exec,1,-1,{PL_INT}},
 { P_ARGUMENT,   "EXIT"     , c_exit,0,-1},
/*
 { P_ARGUMENT,   "EXPORT"     , c_export,1,2, {PL_ALLVAR, PL_NUMBER}},
*/
 { P_ARGUMENT,   "FFT"      , c_fft,1,-1},
#ifndef NOGRAPHICS
 { P_PLISTE,   "FILESELECT", c_fileselect,4,4,{PL_STRING,PL_STRING,PL_STRING,PL_SVAR}},
 { P_PLISTE,   "FILL"     , c_fill,2,3,{PL_INT,PL_INT,PL_INT}},
#endif
 { P_ARGUMENT,   "FIT",        c_fit,4,10,{PL_FARRAY,PL_FARRAY}},
 { P_ARGUMENT,   "FIT_LINEAR", c_fit_linear,4,10,{PL_FARRAY,PL_FARRAY}},
 { P_PLISTE,   "FLUSH"    , c_flush,0,1,{PL_FILENR}},
 { P_FOR,    "FOR"      , c_for,1,-1,{PL_EXPRESSION,PL_KEY,PL_NUMBER,PL_KEY,PL_NUMBER}},
 { P_PLISTE,    "FREE"      , c_free,1,1,{PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "FULLW"    , c_fullw,0,1, {PL_FILENR}},
#endif
 { P_PROC,   "FUNCTION" , c_end,1,-1,{PL_EXPRESSION}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "GET"      , c_get,5,5,{PL_INT,PL_INT,PL_INT,PL_INT,PL_SVAR}},
 { P_PLISTE,   "GET_GEOMETRY" , c_getgeometry,2,7,{PL_FILENR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { P_PLISTE,   "GET_LOCATION" , c_getlocation,2,8,{PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_SVAR}},
 
 { P_PLISTE,   "GET_SCREENSIZE" , c_getscreensize,1,5,{PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
#endif
 { P_GOSUB,     "GOSUB"    , c_gosub,1,1,{PL_PROC}},
 { P_GOTO,       "GOTO"     , c_goto,1,1,{PL_LABEL}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "GPRINT"    , c_gprint,       0,-1,{PL_EVAL}},
#endif
 { P_PLISTE,   "GPS"     , c_gps ,1,1,{PL_KEY}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "GRAPHMODE", c_graphmode,1,1,{PL_INT}},
#endif
 { P_PLISTE,   "HELP"    , c_help,0,1,{PL_KEY}},
 { P_SIMPLE,     "HOME"     , c_home,0,0},

 { P_IF,         "IF"       , c_if,1,-1,{PL_CONDITION}},
 { P_PLISTE,   "INC"      , c_inc,1,1,{PL_NVAR}},
#ifndef NOGRAPHICS
 { P_PLISTE,	 "INFOW"    , c_infow,    2,2,{PL_FILENR,PL_STRING}},
#endif
 { P_ARGUMENT,   "INPUT"    , c_input,1,-1},
#ifndef NOGRAPHICS
 { P_PLISTE,   "KEYEVENT" , c_keyevent,0,8,{PL_NVAR,PL_NVAR,PL_SVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
#endif


 { P_ARGUMENT,   "LET"      , c_let,1,-1},
#ifndef NOGRAPHICS
 { P_PLISTE,     "LINE"     , c_line,4,4,{PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_ARGUMENT,   "LINEINPUT", c_lineinput,1,2, {PL_FILENR,PL_STRING}},
 { P_PLISTE,     "LINK"     , c_link,       2,2,{PL_FILENR,PL_STRING}},

 { P_PLISTE,     "LIST"     , c_list,0,2,{PL_INT,PL_INT}},
 { P_PLISTE,     "LOAD"     , c_load,1,1,{PL_STRING}},
 { P_PLISTE,     "LOCAL"    , c_local,1,-1,{PL_ALLVAR,PL_ALLVAR}},
 { P_PLISTE,     "LOCATE"    , c_locate,2,2,{PL_INT,PL_INT}},
 { P_LOOP,       "LOOP"     , bidnm,0,0},
 { P_PLISTE,     "LPOKE"    , c_lpoke,       2,2,{PL_INT,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "LTEXT"     , c_ltext,3,3,{PL_INT,PL_INT,PL_STRING}},
#endif

 { P_PLISTE,     "MEMDUMP"    , c_memdump,2,2,{PL_INT,PL_INT}},
#ifndef NOGRAPHICS
 { P_SIMPLE,     "MENU"    , c_menu,0,0},
 { P_ARGUMENT,   "MENUDEF"  , c_menudef,1,2},
 { P_SIMPLE,     "MENUKILL" , c_menukill,0,0},
 { P_PLISTE,     "MENUSET"  , c_menuset,2,2,{PL_INT,PL_INT}},
#endif
 { P_ARGUMENT,   "MERGE"    , c_merge,1,1,{PL_STRING}},
 { P_PLISTE,    "MFREE"      , c_free,1,1,{PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "MOUSE"    , c_mouse,1,5,{PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { P_PLISTE,   "MOUSEEVENT" , c_mouseevent,0,6,{PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { P_PLISTE,   "MOTIONEVENT" , c_motionevent,0,6,{PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { P_PLISTE,   "MOVEW"    , c_movew,3,3, {PL_FILENR,PL_INT,PL_INT}},
#endif
 { P_PLISTE,  "MSYNC"     , c_msync  ,2,2,{PL_INT, PL_INT}},
 { P_PLISTE,   "MUL"      , c_mul,2,2,{PL_NVAR,PL_NUMBER}},

 { P_SIMPLE, "NEW"      , c_new,0,0},
 { P_NEXT,   "NEXT"     , c_next,0,1,{PL_NVAR}},
 { P_IGNORE|P_SIMPLE, "NOOP",         c_nop,         0,0},
 { P_IGNORE|P_SIMPLE, "NOP",          c_nop,         0,0},
#ifndef NOGRAPHICS
 { P_SIMPLE,"NOROOTWINDOW", c_norootwindow,0,0},
 { P_PLISTE,   "OBJC_ADD"    , c_objc_add,      3,3,{PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,   "OBJC_DELETE"    , c_objc_delete,      2,2,{PL_INT,PL_INT}},
#endif
 { P_ARGUMENT,   "ON"       , c_on,         1,-1},
 { P_PLISTE,     "OPEN"     , c_open,       3,4,{PL_STRING,PL_FILENR,PL_STRING,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "OPENW"    , c_openw,      1,1,{PL_FILENR}},
#endif
 { P_ARGUMENT,   "OUT"      , c_out,        2,-1,{PL_FILENR,PL_INT,PL_INT}},

 { P_PLISTE,   "PAUSE"    , c_pause,      1,1,{PL_NUMBER}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "PBOX"     , c_pbox ,      4,4,{PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "PCIRCLE"  , c_pcircle,    3,5,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "PELLIPSE" , c_pellipse,   4,6,{PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "PIPE" , c_pipe,   2,2,{PL_FILENR,PL_FILENR}},
 { P_PLISTE,     "PLAYSOUND",     c_playsound, 2,4,{PL_INT,PL_STRING,PL_NUMBER,PL_NUMBER}},
 { P_PLISTE,     "PLAYSOUNDFILE",     c_playsoundfile, 1,1,{PL_STRING}},
 { P_SIMPLE,     "PLIST"    , c_plist,      0,0},
#ifndef NOGRAPHICS
 { P_PLISTE,     "PLOT"     , c_plot,       2,2,{PL_INT,PL_INT}},
#endif
 { P_PLISTE,   "POKE"     , c_poke,       2,2,{PL_INT,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "POLYFILL" , c_polyfill,         3,7,{PL_INT,PL_IARRAY,PL_IARRAY,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,   "POLYLINE" , c_polyline,       3,7,{PL_INT,PL_IARRAY,PL_IARRAY,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,   "POLYMARK" , c_polymark,   3,7,{PL_INT,PL_IARRAY,PL_IARRAY,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,   "PRBOX"    , c_prbox ,      4,4,{PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,  "PRINT"    , c_print,       0,-1,{PL_EVAL}},
 { P_PROC,   "PROCEDURE", c_end  ,      0,0},
 { P_IGNORE, "PROGRAM"  , c_nop  ,      0,0},
 /* Ausdruck als Message queuen
  { P_ARGUMENT,   "PUBLISH"  , c_publish, 1,2,{PL_ALLVAR,PL_NUMBER}},
 */
#ifndef NOGRAPHICS
 { P_PLISTE,   "PUT"  , c_put,      3,4,{PL_INT,PL_INT,PL_STRING,PL_NUMBER}},
#endif
 { P_PLISTE,   "PUTBACK"  , c_unget,      2,2,{PL_FILENR,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "PUT_BITMAP"  , c_put_bitmap, 5,5,{PL_STRING,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif

 { P_PLISTE, "QUIT"     , c_quit,       0,1,{PL_INT}},

 { P_PLISTE, "RANDOMIZE", c_randomize  ,      0,1,{PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "RBOX"      , c_rbox       ,4, 4, {PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,   "READ"     , c_read,       1,-1,{PL_ALLVAR,PL_ALLVAR}},
 { P_PLISTE,     "RECEIVE"  , c_receive,    2,3,{PL_FILENR,PL_SVAR,PL_NVAR}},
 { P_PLISTE,     "RELSEEK"  , c_relseek,    2,2,{PL_FILENR,PL_INT}},
 { P_REM,    "REM"      , c_nop  ,      0,0},
 { P_REPEAT, "REPEAT"   , c_nop  ,      0,0},
 { P_PLISTE,   "RESTORE"  , c_restore,    0,1,{PL_LABEL}},
 { P_RETURN,   "RETURN"   , c_return,     0,1},
#ifndef NOGRAPHICS
 { P_SIMPLE, "ROOTWINDOW", c_rootwindow,0,0},
 { P_SIMPLE, "RSRC_FREE", c_rsrc_free,0,0},
 { P_PLISTE, "RSRC_LOAD", c_rsrc_load,1,1,{PL_STRING}},
#endif

 { P_SIMPLE, "RUN"      , c_run,        0,0},

 { P_PLISTE,   "SAVE"     , c_save,0,1,{PL_STRING}},
#ifndef NOGRAPHICS
 { P_PLISTE,   "SAVESCREEN", c_savescreen,1,1,{PL_STRING}},
 { P_PLISTE,   "SAVEWINDOW", c_savewindow,1,1,{PL_STRING}},
 { P_ARGUMENT,   "SCOPE"    , c_scope,      1,6,{PL_NARRAY,PL_ANYVALUE,PL_NUMBER,PL_NUMBER,PL_NUMBER,PL_NUMBER}},
#endif
 { P_PLISTE,   "SCREEN"    , c_screen,      1,1,{PL_INT}},
 { P_PLISTE,   "SEEK"     , c_seek,       1,2,{PL_FILENR,PL_INT}},
 { P_SELECT, "SELECT"   , c_select,     1,1,{PL_INT}},
 /*
 { P_ARGUMENT,   "SEMGIVE"  , c_semgive, 1,2,{PL_NUMBER,PL_NUMBER}},
 { P_ARGUMENT,   "SEMTAKE"  , c_semtake, 1,2,{PL_NUMBER,PL_NUMBER}},
 */
 { P_PLISTE, "SEND"   , c_send,     2,4,{PL_FILENR,PL_STRING,PL_INT,PL_INT}},
 { P_PLISTE, "SENSOR" , c_sensor ,1,1,{PL_KEY}},
#ifndef NOGRAPHICS
 { P_PLISTE,	"SETFONT"  , c_setfont,    1,1,{PL_STRING}},
 { P_PLISTE,	"SETMOUSE" , c_setmouse,   2,3,{PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,	"SGET" , c_sget,   1,1,{PL_SVAR}},
#endif
 { P_PLISTE,  "SHM_DETACH"      , c_detatch,1,1,{PL_INT}},
 { P_PLISTE,    "SHM_FREE" , c_shm_free,1,1,{PL_INT}},
#ifndef NOGRAPHICS
 { P_SIMPLE,	"SHOWPAGE" , c_vsync,      0,0},
 { P_PLISTE,	"SIZEW"    , c_sizew,      3,3,{PL_FILENR,PL_INT,PL_INT}},
#endif
 { P_PLISTE,    "SORT",      c_sort,        1,3,{PL_ARRAYVAR,PL_INT,PL_IARRAYVAR}},
 { P_PLISTE,    "SOUND",     c_sound,        2,4,{PL_INT,PL_NUMBER,PL_NUMBER,PL_NUMBER}},

 { P_GOSUB,     "SPAWN"    , c_spawn,1,1,{PL_PROC}},
 { P_PLISTE,    "SPEAK",     c_speak, 1,4,{PL_STRING,PL_NUMBER,PL_NUMBER,PL_STRING}},

 { P_PLISTE,	"SPLIT"    , c_split,  4,5,{PL_STRING,PL_STRING,PL_INT,PL_SVAR,PL_SVAR}},
#ifndef NOGRAPHICS
 { P_PLISTE,	"SPUT"     , c_sput,      1,1,{PL_STRING}},
#endif
 { P_SIMPLE,	"STOP"     , c_stop,       0,0},
 { P_PLISTE,	"SUB"      , c_sub,        2,2,{PL_NVAR,PL_NUMBER}},
 { P_PLISTE,	"SWAP"     , c_swap,       2,2,{PL_ALLVAR,PL_ALLVAR}},
 { P_PLISTE,	"SYSTEM"   , c_system,     1,1,{PL_STRING}},

#ifndef NOGRAPHICS
 { P_PLISTE,	"TEXT"     , c_text,       3,3,{PL_INT,PL_INT,PL_STRING}},
#endif
#ifdef TINE
 { P_ARGUMENT,   "TINEBROADCAST", c_tinebroadcast,1,-1,{PL_STRING}},
 { P_SIMPLE,     "TINECYCLE", c_tinecycle,0,0},
 { P_ARGUMENT,   "TINEDELIVER", c_tinedeliver,1,-1},
 { P_ARGUMENT,   "TINEEXPORT", c_tineexport,1,-1},
 { P_ARGUMENT,   "TINELISTEN", c_tinelisten,1,-1,{PL_STRING}},
 { P_PLISTE,     "TINEMONITOR", c_tinemonitor,2,3,{PL_STRING,PL_PROC,PL_INT}},
 { P_ARGUMENT,   "TINEPUT"    , c_tineput ,2,-1,{PL_STRING}},
 { P_PLISTE,     "TINESERVER" , c_tineserver,0,2,{PL_STRING,PL_INT}},
 { P_ARGUMENT,   "TINESET"    , c_tineput ,2,-1,{PL_STRING}},
#endif
#ifndef NOGRAPHICS
 { P_PLISTE,	"TITLEW"   , c_titlew,     2,2,{PL_FILENR,PL_STRING}},
 { P_PLISTE,    "TOPW"     , c_topw,       0,1,   {PL_FILENR}},
#endif
 { P_SIMPLE,	"TROFF"    , c_troff,      0,0},
 { P_SIMPLE,	"TRON"     , c_tron,       0,0},

 { P_PLISTE,  "UNLINK"   , c_close  ,1,-1,{PL_FILENR,PL_FILENR}},
 { P_PLISTE,    "UNMAP"    , c_unmap  ,2,2,{PL_INT, PL_INT}},
 { P_UNTIL,	"UNTIL"    , c_until,      1,1,{PL_CONDITION}},
#ifndef NOGRAPHICS
 { P_PLISTE,	"USEWINDOW", c_usewindow,  1,1,{PL_FILENR}},
#endif

 { P_SIMPLE,	"VERSION"  , c_version,    0,0},
 { P_ARGUMENT,	"VOID"     , c_void,       1,1,{PL_EVAL}},
#ifndef NOGRAPHICS
 { P_SIMPLE,	"VSYNC"    , c_vsync,      0,0},
#endif
 { P_PLISTE,     "WATCH"     , c_watch,1,1,{PL_STRING}},
 { P_PLISTE,    "WAVE",     c_wave,        2,6,{PL_INT,PL_INT,PL_NUMBER,PL_NUMBER,PL_NUMBER,PL_NUMBER}},

 { P_WEND,	"WEND"     , bidnm,       0,0},
 { P_WHILE,	"WHILE"    , c_while,      1,1,{PL_CONDITION}},
 { P_PLISTE,	"WORT_SEP" , c_wort_sep,  4,5,{PL_STRING,PL_STRING,PL_INT,PL_SVAR,PL_SVAR}},
#ifndef NOGRAPHICS
 { P_SIMPLE,	"XLOAD"    , c_xload,    0,0},
 { P_SIMPLE,	"XRUN"     , c_xrun,     0,0},
#endif

};
const int anzcomms=sizeof(comms)/sizeof(COMMAND);

#if 0
char *keys[]={"ON","OFF","IF","TO","DOWNTO","STEP","USING"};
const int anzkeys=sizeof(keys)/sizeof(char *);
#endif

static int oldprglen=0;

void loadprg(char *filename) {
  batch=programbufferlen=pc=sp=0;
  mergeprg(filename);
}

extern char ifilename[];


/* Bytecode spezifica. */

int is_bytecode=0;
static char *stringseg;
BYTECODE_SYMBOL *symtab;

static void do_relocation(char *adr,unsigned char *fixup, int l) {
  int i=0;
  long ll;
  while(i<l) {
    if(fixup[i]==0) break;
    else if(fixup[i]==1) adr+=254;
    else {
      adr+=fixup[i];
      memcpy(&ll,adr,sizeof(long));
      ll+=(long)adr;
      memcpy(adr,&ll,sizeof(long));
    }
  
  } 
}
/* We assume that the segments are in following order: 
   HEADER
   TEXT
   RODATA
   SDATA
   DATA
   BSS  --- STRINGS  
        --- SYMBOL
	--- RELOCATION
	*/

char *bytecode_init(char *adr) {
  int i,a,vnr,vnr2,typ;
  char *name;
  char *bsseg;
  char *fixup;   /* Relocation information */
#ifdef ANDROID
  char buffer[100];
#endif

  erase_all_variables();
  clear_labelliste();
  clear_procliste();

  /* Ueberpruefe ob ein gueltiger Header dabei ist und setze databuffer */
  if(adr[0]==BC_BRAs && adr[1]==sizeof(BYTECODE_HEADER)-2) {
    BYTECODE_HEADER *bytecode=(BYTECODE_HEADER *)adr;
    clear_parameters(prglen);
    prglen=pc=sp=0;
    is_bytecode=1;
#ifdef ANDROID
     sprintf(buffer,"Bytecode header found (V.%x)\n",bytecode->version);
     backlog(buffer);
#endif
    if(bytecode->version!=BC_VERSION) {
      printf("ERROR: This Bytecode was compiled for a different version of "
      "X11-Basic.\n Please consider to recompile it.\n");
      return(NULL);
    }
        
    /* Set up the data buffer */
    databuffer=adr+bytecode->textseglen+bytecode->rodataseglen+sizeof(BYTECODE_HEADER);
    databufferlen=bytecode->sdataseglen;
#ifdef ANDROID
      sprintf(buffer,"Databuffer $%08x contains: <%s>\n",(unsigned int)databuffer,databuffer);
    backlog(buffer);
#endif
    rodata=&adr[sizeof(BYTECODE_HEADER)+bytecode->textseglen];
    bsseg=stringseg=&adr[sizeof(BYTECODE_HEADER)+
                 bytecode->textseglen+
		 bytecode->rodataseglen+
		 bytecode->sdataseglen+
		 bytecode->dataseglen];

    /* Jetzt Variablen anlegen.*/
    symtab=(BYTECODE_SYMBOL *)(adr+sizeof(BYTECODE_HEADER)+
                                   bytecode->textseglen+
		                   bytecode->rodataseglen+
		                   bytecode->sdataseglen+
				   bytecode->dataseglen+
				   bytecode->stringseglen);
    a=bytecode->symbolseglen/sizeof(BYTECODE_SYMBOL);
    if(a>0) {
      for(i=0;i<a;i++) {
        if(symtab[i].typ==STT_OBJECT) {
	  typ=symtab[i].subtyp;
	  if(symtab[i].name) name=&stringseg[symtab[i].name];
          else {
	    name=malloc(64);
	    sprintf(name,"VAR_%x",vnr);
          }
	  /*Hier erstmal nur int und float im bss ablegen, da noch nicht geklaert ist, 
	  wie wir strings und arrays hier initialisieren koennen ohne die symboltabelle 
	  zu ueberschreiben*/
#ifdef ANDROID
    sprintf(buffer,"Symbol: <%s> $%04x %08x ",name,typ,symtab[i].adr);
    backlog(buffer);
#endif	  
	  if(typ&ARRAYTYP) vnr2=add_variable(name,ARRAYTYP,typ&(~ARRAYTYP));
	  else if(typ==STRINGTYP) vnr2=add_variable(name,typ,0);
	  else vnr2=add_variable_adr(name,typ,bsseg+symtab[i].adr);
#ifdef ANDROID	  
    sprintf(buffer,"BSSSEG auf %08x ",bsseg);
    backlog(buffer);
#endif
        }  
      }
    }
#ifdef ANDROIOD    
    sprintf(buffer,"%d variables.\n",anzvariablen);
    backlog(buffer);
#endif
    #if DEBUG
    printf("%d variables.\n",anzvariablen);
    c_dump(NULL,0);
    #endif
    fixup=(char *)(adr+sizeof(BYTECODE_HEADER)+
                       bytecode->textseglen+
		       bytecode->rodataseglen+
		       bytecode->sdataseglen+
		       bytecode->dataseglen+
		       bytecode->stringseglen+
		       bytecode->symbolseglen);
    if((bytecode->flags&EXE_REL)==EXE_REL && bytecode->relseglen>0) 
      do_relocation(adr,fixup,bytecode->relseglen);

   /*Now: clear bss segment. This will probably overwrite symbol table and strings and relocation info*/
    if(bytecode->bssseglen>0) bzero(bsseg,bytecode->bssseglen);
#ifdef ANDROID
    backlog("bytecode_init done.");
#endif
    return(adr);
  } else {
    printf("VM: ERROR, file format not recognized. $%02x $%02x\n",adr[0],adr[1]);
    return(NULL);
  }
}





/* Routine zum Laden eines Programms */

int mergeprg(char *fname) {
  int i,len;
  char *pos;
  FILE *dptr;
  /* Filelaenge rauskriegen */

  dptr=fopen(fname,"rb"); len=lof(dptr); fclose(dptr);
  programbuffer=realloc(programbuffer,programbufferlen+len+1);
  bload(fname,programbuffer+programbufferlen,len);
  programbufferlen+=len;


/* Hier jetzt pr"ufen, ob es sich um eine bytecode-Datei handelt.
   wenn nicht, versuche sie zu interpretieren...*/

  /* Ueberpruefe ob ein gueltiger Header dabei ist */

  if(programbuffer[0]==BC_BRAs && programbuffer[1]==sizeof(BYTECODE_HEADER)-2) {
    BYTECODE_HEADER *bytecode=(BYTECODE_HEADER *)programbuffer;

     #if DEBUG 
       printf("Bytecode header found (V.%x)\n",bytecode->version);
     #endif
     if(bytecode->version!=BC_VERSION) {
       printf("ERROR: This Bytecode was compiled for a different version of "
       "X11-Basic.\n Please consider to recompile it.\n");
       return(-1);
    }
    
    /* Sicherstellen, dass der Speicherberiech auch gross genug ist fuer bss segment*/
    if(bytecode->bssseglen>bytecode->relseglen+bytecode->stringseglen+bytecode->symbolseglen) {
      programbufferlen+=bytecode->bssseglen-bytecode->stringseglen-bytecode->symbolseglen;
      programbuffer=realloc(programbuffer,programbufferlen);
    }
    
    if(bytecode_init(programbuffer)) return(0);
    return(-1); /* stimmt was nicht. */
  } else {
    
   /* Zeilenzahl herausbekommen */
    pos=programbuffer;
    oldprglen=prglen;
    i=prglen=0;
    while(i<programbufferlen) {
      if(i>0 && programbuffer[i]=='\n' && programbuffer[i-1]=='\\') {
        programbuffer[i]=' ';
        programbuffer[i-1]=' ';
      } else if(programbuffer[i]==0 || programbuffer[i]=='\n') {
        programbuffer[i]=0;
        program[prglen++]=pos;
	
        pos=programbuffer+i+1;
      } else if(programbuffer[i]==9) programbuffer[i]=' '; /* TABs entfernen*/
      i++;
    }
    return(init_program());
  }
}


/* Find Numer des Kommandos w1 aus Liste. 
   Wenn w1 nur ein Teil des kommandos ist, wird es trotzdem gefunden, 
   sofern es eindeutig zuordnenbar ist. 
  Wenn nicht gefunden, -1 zurueck.
*/

int find_comm(char *w1) {
  int i=0,a=anzcomms-1,b;
  /* Kommandoliste durchsuchen, moeglichst effektiv ! */
  for(b=0; b<strlen(w1); b++) {
    while(w1[b]>(comms[i].name)[b] && i<a) i++;
    while(w1[b]<(comms[a].name)[b] && a>i) a--;
    if(i==a) break;
  }
  if((i==a && strncmp(w1,comms[i].name,strlen(w1))==0) ||
     (i!=a && strcmp(w1,comms[i].name)==0) ) {
#ifdef DEBUG
      if(b<strlen(w1)) printf("Command %s completed --> %s\n",w1,comms[i].name);
#endif
     return(i);
  }
  return(-1);
}

static int find_comm_guess(char *w1,int *guessa,int *guessb) {
  int i=0,a=anzcomms-1,b;
  /* Kommandoliste durchsuchen, moeglichst effektiv ! */
  for(b=0; b<strlen(w1); b++) {
    while(w1[b]>(comms[i].name)[b] && i<a) i++;
    while(w1[b]<(comms[a].name)[b] && a>i) a--;
    if(i==a) break;
  }
  *guessa=i;
  *guessb=a;
  if((i==a && strncmp(w1,comms[i].name,strlen(w1))==0) ||
     (i!=a && strcmp(w1,comms[i].name)==0) ) {
#ifdef DEBUG
      if(b<strlen(w1)) printf("Command %s completed --> %s\n",w1,comms[i].name);
#endif
     return(i);
  }
  return(-1);
}

int find_func(char *w1) {
  int i=0,a=anzpfuncs-1,b;
  /* Funktionsliste durchsuchen, moeglichst effektiv ! */
  for(b=0; b<strlen(w1); b++) {
      while(w1[b]>(pfuncs[i].name)[b] && i<a) i++;
      while(w1[b]<(pfuncs[a].name)[b] && a>i) a--;
      if(i==a) break;
  }
  if(strcmp(w1,pfuncs[i].name)==0) return(i);
  return(-1);
}

int find_afunc(char *w1) {
  int i=0,a=anzpafuncs-1,b;
  /* Funktionsliste durchsuchen, moeglichst effektiv ! */
  for(b=0; b<strlen(w1); b++) {
      while(w1[b]>(pafuncs[i].name)[b] && i<a) i++;
      while(w1[b]<(pafuncs[a].name)[b] && a>i) a--;
      if(i==a) break;
  }
  if(strcmp(w1,pafuncs[i].name)==0) return(i);
  return(-1);
}

int find_sfunc(char *w1) {
  int i=0,a=anzpsfuncs-1,b;
  /* Funktionsliste durchsuchen, moeglichst effektiv ! */
  for(b=0; b<strlen(w1); b++) {
      while(w1[b]>(psfuncs[i].name)[b] && i<a) i++;
      while(w1[b]<(psfuncs[a].name)[b] && a>i) a--;
      if(i==a) break;
  }
  if(strcmp(w1,psfuncs[i].name)==0) return(i);
  return(-1);
}

static int find_sysvar(char *s) {
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


/*****************************************************

Programmvorbereitung und precompilation


******************************************************/

int init_program() {
  char *expr,*pos2,*pos3,*buffer=NULL,*zeile=NULL;  
  int i,typ;
  erase_all_variables();
  clear_labelliste();
  clear_procliste();
  databufferlen=0;
  clear_parameters(oldprglen);

  is_bytecode=0;
  /* Label-, Procedur- und Variablenliste Erstellen und p_code transformieren*/
  
  for(i=0; i<prglen;i++) {
    zeile=realloc(zeile,strlen(program[i])+1);
    buffer=realloc(buffer,strlen(program[i])+1);
    strcpy(zeile, program[i]);
    
//    code[i].opcode=0;       /*Typ und Kommandonummer*/
    pcode[i].panzahl=0;       /*Anzahl Parameter*/
    pcode[i].ppointer=NULL;   /*Zeiger auf Parameterliste*/
//    pcode[i].argument=NULL; /*String als argument*/
    pcode[i].etyp=PE_NONE;   /* fuer Kommentare */
//    pcode[i].extra=NULL;   /*Extra string fuer Kommentare*/
    pcode[i].integer=-1;

    wort_sep2(zeile," !",TRUE,zeile,buffer);  /*Kommentare abseparieren*/
    xtrim2(zeile,TRUE,zeile);
    if(strlen(buffer)) {
      pcode[i].etyp=PE_COMMENT;
      pcode[i].extra=strdup(buffer);
    }
#ifdef DEBUG
    printf("Zeile %d : %s\n",i,zeile);
#endif
    if(wort_sep(zeile,' ',TRUE,zeile,buffer)==0) 
      pcode[i].opcode=P_IGNORE|P_NOCMD;	
    else if(zeile[0]=='\'' || zeile[0]=='#') {
      pcode[i].opcode=P_REM;
      pcode[i].argument=strdup(buffer);
    } else if(zeile[0]=='@') {
      pcode[i].argument=strdup(zeile+1);
      pcode[i].opcode=P_GOSUB|find_comm("GOSUB");  /* Hier fehlt die comms nr */
    } else if(zeile[strlen(zeile)-1]==':') {
      zeile[strlen(zeile)-1]=0;
#ifdef DEBUG 
      printf("Label gefunden: %s in Zeile %d  \n",zeile,i);
#endif
      pcode[i].opcode=P_LABEL;
      pcode[i].integer=add_label(zeile,i,(databufferlen?(databufferlen+1):databufferlen));
    } else if(strcmp(zeile,"DATA")==0) {
      pcode[i].opcode=P_DATA;
#ifdef DEBUG
      printf("DATA Statement found in line %d. <%s>\n",i,buffer);
#endif
      databuffer=realloc(databuffer,databufferlen+strlen(buffer)+2);
      if(databufferlen) databuffer[databufferlen++]=',';
      
      memcpy(databuffer+databufferlen,buffer,strlen(buffer));
      databufferlen+=strlen(buffer);
      databuffer[databufferlen]=0;
   //   printf("databuffer now contains %d Bytes.\n",databufferlen);
   //   printf("databuffer=<%s>\n",databuffer);
    } else if((typ=((strcmp(zeile,"PROCEDURE")==0) | 
                 (2*(strcmp(zeile,"FUNCTION")==0)) |
		 (4*(strcmp(zeile,"DEFFN")==0))))!=0) {
#ifdef DEBUG
      printf("procedure or function found in line %d. Typ=%d, <%s>\n",i,typ,buffer);
#endif
      if(typ==4) {
        expr=searchchr2(buffer,'=');
	if(expr==NULL) {
	  puts("Syntax error ==> DEFFN");
	  pcode[i].argument=NULL;
	} else {
	  *expr++=0;
	  pcode[i].argument=strdup(expr);
        }
	pcode[i].opcode=P_DEFFN;
      } else pcode[i].opcode=P_PROC;
      pos2=searchchr(buffer,'(');
      if(pos2 != NULL) {
          pos2[0]=0;pos2++;
          pos3=pos2+strlen(pos2)-1;
          if(pos3[0]!=')') puts("Syntax error ==> Parameterliste");
          else *pos3++=0;
      } else pos2=zeile+strlen(zeile);
      pcode[i].integer=add_proc(buffer,pos2,i,typ);
    } else { /* Rest Transformieren    */
      int j=find_comm(zeile);
      if(j==-1) { /* Kein Befehl passt... */
        char *buf=malloc(strlen(zeile)+strlen(buffer)+2);
        char *pos,*name;
	strcpy(buf,zeile);
	if(strlen(buffer)) {strcat(buf," ");strcat(buf,buffer);}
	//  printf("EVAL: %s\n",buf);
	if(*buf=='&' || *buf=='+' || *buf=='-' || *buf=='~' || *buf=='@' 
	             || isdigit(*buf) ) {
	  pcode[i].opcode=P_EVAL|P_NOCMD;
	  pcode[i].panzahl=0;
	  pcode[i].ppointer=NULL;
	  pcode[i].argument=buf;
	} else {
	  int typ;
	  char *r;
	  pos=searchchr2(buf,'=');
	  name=buf;
	  if(pos!=NULL) {
 	    char *vname,*argument;
	    int e;
	    *pos++=0;
	    pcode[i].opcode=P_ZUWEIS;
	    pcode[i].argument=strdup(pos);
	    typ=vartype(name);
	    e=klammer_sep_destroy(buf,&vname,&argument);
	    /* opcode durch P_ZUWEIS ersetzen, 
	       Variable anlegen, wenn noch nicht definiert,
	       pcode[i].integer= ist varnummer,
	       (Was machen wir mit idizes?) 
	       argument durch Ausdruck ersetzen.*/
	    
	    r=varrumpf(name);
            	     
	    if(typ & ARRAYTYP) { 
  	      pcode[i].panzahl=0;
	      pcode[i].ppointer=NULL;
	      pcode[i].integer=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP));
            } else {
              if(e>1) {
		pcode[i].integer=add_variable(r,ARRAYTYP,typ);
		pcode[i].panzahl=count_parameters(argument);   /* Anzahl indizes z"ahlen*/
		pcode[i].ppointer=calloc(pcode[i].panzahl,sizeof(PARAMETER));
                /*hier die Indizies in einzelne zu evaluierende Ausdruecke
		  separieren*/
		make_preparlist(pcode[i].ppointer,argument);
              } else {
  	        pcode[i].panzahl=0;
	        pcode[i].ppointer=NULL;
	        pcode[i].integer=add_variable(r,typ,0);
	      }
	    }
	    if(pcode[i].integer==-1) printf("ERROR: at line %d: Variable could not be created.\n",i);
	    free(r);
	    free(buf);	       
          } else {
	    printf("WARNING: Syntax error at line %d: <%s>\n",i,buf);
	    pcode[i].opcode=P_EVAL|P_NOCMD;
	    pcode[i].panzahl=0;
	    pcode[i].ppointer=NULL;
	    pcode[i].argument=buf;
	  }
        }
      } else {  /* j hat nun die passende Befehlnummer */
	pcode[i].opcode=comms[j].opcode|j;
	xtrim(buffer,TRUE,buffer);
	pcode[i].argument=strdup(buffer);
	
	if(comms[j].opcode==P_DATA) {
	  printf("WARNING: Something is wrong at line %d. Data should have been treated already.\n",i);
	} else if(comms[j].opcode==P_LOOP) {/*Zugehoeriges DO suchen */
	  pcode[i].integer=suchep(i-1,-1,P_DO,P_LOOP,P_DO);
          if(pcode[i].integer==-1)  structure_warning(i,zeile); /*Programmstruktur fehlerhaft */
	} else if(comms[j].opcode==P_WEND) {/*Zugehoeriges WHILE suchen */
          pcode[i].integer=suchep(i-1,-1,P_WHILE,P_WEND,P_WHILE);
          if(pcode[i].integer==-1)  structure_warning(i,zeile); /*Programmstruktur fehlerhaft */
	} else if(comms[j].opcode==P_NEXT) {/*Zugehoeriges FOR suchen */
          pcode[i].integer=suchep(i-1,-1,P_FOR,P_NEXT,P_FOR);
          if(pcode[i].integer==-1)  structure_warning(i,zeile); /*Programmstruktur fehlerhaft */
	} else if(comms[j].opcode==P_UNTIL) {/*Zugehoeriges REPEAT suchen */
	  pcode[i].integer=suchep(i-1,-1,P_REPEAT,P_UNTIL,P_REPEAT);
          if(pcode[i].integer==-1)  structure_warning(i,zeile); /*Programmstruktur fehlerhaft */
	} else if(comms[j].opcode==P_ELSE) {/*Pruefen ob es ELSE IF ist. */
          char w1[strlen(buffer)+1],w2[strlen(buffer)+1];
	  wort_sep(buffer,' ',TRUE,w1,w2);
	  if(strcmp(w1,"IF")==0) pcode[i].opcode=P_ELSEIF|j;
	} 
	/*Argumente und Argument-Listen vorbereiten*/
	if(comms[j].pmax==0 || (comms[j].opcode&PM_TYP)==P_SIMPLE) {
	  pcode[i].panzahl=0;
	  pcode[i].ppointer=NULL;
	} else if((comms[j].opcode&PM_TYP)==P_ARGUMENT) {
	  pcode[i].panzahl=0;
	  pcode[i].ppointer=NULL;
	} else if((comms[j].opcode&PM_TYP)==P_PLISTE) {
	  int ii;
	  pcode[i].panzahl=ii=count_parameters(buffer);
	  if((comms[j].pmin>ii && comms[j].pmin!=-1) || (comms[j].pmax<ii && comms[j].pmax!=-1))  
	        printf("Warnung: Z.%d Falsche Anzahl Parameter bei %s.\n",i,comms[j].name); /*Programmstruktur fehlerhaft */
	  if(ii==0) pcode[i].ppointer=NULL;
	}
	    /* Einige Befehle noch nachbearbeiten */
        if(strcmp(zeile,"EXIT")==0) { /*Pruefen ob es EXIT IF ist. */
            char *w1,*w2;
	    wort_sep_destroy(buffer,' ',TRUE,&w1,&w2);
	    if(strcmp(w1,"IF")==0) pcode[i].opcode=P_EXITIF|j; 
	} 
      }
    }
  } 


#ifdef DEBUG
  puts("PASS 2:");
#endif
  /* Pass 2, jetzt sind alle Labels und Proceduren bekannt. 
     Sprungmarken werden noch gesetzt, sowie zusaetzliche
     Variablen aus Pliste (CLR,LOCAL,DIM,INC,DEC) noch hinzugefuegt.
  */
  for(i=0; i<prglen;i++) {
    if((pcode[i].opcode&PM_SPECIAL)==P_ELSE) { /* Suche Endif */
      pcode[i].integer=suchep(i+1,1,P_ENDIF,P_IF,P_ENDIF)+1;
      if(pcode[i].integer==0)  structure_warning(i,"ELSE"); /*Programmstruktur fehlerhaft */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_ELSEIF) { /* Suche Endif */
      pcode[i].integer=suchep(i+1,1,P_ENDIF,P_IF,P_ENDIF)+1;
      if(pcode[i].integer==0)  structure_warning(i,"ELSE IF"); /*Programmstruktur fehlerhaft */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_IF) { /* Suche Endif */
      pcode[i].integer=suchep(i+1,1,P_ENDIF,P_IF,P_ENDIF)+1;
      if(pcode[i].integer==0)  structure_warning(i,"IF"); /*Programmstruktur fehlerhaft */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_WHILE) { /* Suche WEND */
      pcode[i].integer=suchep(i+1,1,P_WEND,P_WHILE,P_WEND)+1;
      if(pcode[i].integer==0)  structure_warning(i,"WHILE"); /*Programmstruktur fehlerhaft */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_FOR) { /* Suche NEXT */
      pcode[i].integer=suchep(i+1,1,P_NEXT,P_FOR,P_NEXT)+1;
      if(pcode[i].integer==0)  structure_warning(i,"FOR"); /*Programmstruktur fehlerhaft */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_SELECT) { /* Suche CASE/DEFAULT/ENDSELECT */
      pcode[i].integer=suchep(i+1,1,P_CASE,P_SELECT,P_ENDSELECT)+1;
      if(pcode[i].integer==0)  {
        pcode[i].integer=suchep(i+1,1,P_DEFAULT,P_SELECT,P_ENDSELECT)+1;
        if(pcode[i].integer==0)  pcode[i].integer=suchep(i+1,1,P_ENDSELECT,P_SELECT,P_ENDSELECT)+1;
        if(pcode[i].integer==0)  structure_warning(i,"SELECT/CASE"); /*Programmstruktur fehlerhaft */
      }
    } else if((pcode[i].opcode&PM_SPECIAL)==P_CASE) { /* Suche CASE/DEFAULT/ENDSELECT */
      pcode[i].integer=suchep(i+1,1,P_CASE,P_SELECT,P_ENDSELECT)+1;
      if(pcode[i].integer==0)  pcode[i].integer=suchep(i+1,1,P_DEFAULT,P_SELECT,P_ENDSELECT)+1;
      if(pcode[i].integer==0)  pcode[i].integer=suchep(i+1,1,P_ENDSELECT,P_SELECT,P_ENDSELECT)+1;
      if(pcode[i].integer==0)  structure_warning(i,"SELECT/CASE"); /*Programmstruktur fehlerhaft */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_DEFAULT) { /* Suche CASE/DEFAULT/ENDSELECT */
      pcode[i].integer=suchep(i+1,1,P_CASE,P_SELECT,P_ENDSELECT)+1;
      if(pcode[i].integer==0)  pcode[i].integer=suchep(i+1,1,P_DEFAULT,P_SELECT,P_ENDSELECT)+1;
      if(pcode[i].integer==0)  pcode[i].integer=suchep(i+1,1,P_ENDSELECT,P_SELECT,P_ENDSELECT)+1;
      if(pcode[i].integer==0)  structure_warning(i,"SELECT/DEFAULT"); /*Programmstruktur fehlerhaft */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_CONTINUE) { /* Suche CASE/DEFAULT/ENDSELECT */
      pcode[i].integer=suchep(i+1,1,P_CASE,P_SELECT,P_ENDSELECT)+1;
      if(pcode[i].integer==0)  pcode[i].integer=suchep(i+1,1,P_DEFAULT,P_SELECT,P_ENDSELECT)+1;
      if(pcode[i].integer==0)  pcode[i].integer=suchep(i+1,1,P_ENDSELECT,P_SELECT,P_ENDSELECT)+1;
      if(pcode[i].integer==0)  structure_warning(i,"SELECT/CONTINUE"); /*Programmstruktur fehlerhaft */
    } else if((pcode[i].opcode&PM_SPECIAL)==P_BREAK ||
              (pcode[i].opcode&PM_SPECIAL)==P_EXITIF) { /* Suche ende Schleife*/
      int j,f=0,o;
      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_LOOP || o==P_NEXT || o==P_WEND ||  o==P_UNTIL||  o==P_ENDSELECT)  && f<=0) break;
        if(o & P_LEVELIN) f++;
        if(o & P_LEVELOUT) f--;
      }
      if(j==prglen) { 
        structure_warning(i,"BREAK/EXIT IF"); /*Programmstruktur fehlerhaft */
        pcode[i].integer=-1;
      } else {
        if(o==P_ENDSELECT) pcode[i].integer=j; /* wichtig fuer compiler !*/
        else pcode[i].integer=j+1;
      }
    } else if((pcode[i].opcode&PM_SPECIAL)==P_GOSUB) { /* Suche Procedure */
      if(*(pcode[i].argument)=='&') pcode[i].integer=-1;
      else {
        char buf[strlen(pcode[i].argument)+1];
	char *pos,*pos2;
        strcpy(buf,pcode[i].argument);
        pos=searchchr(buf,'(');
        if(pos!=NULL) {
          pos[0]=0;pos++;
          pos2=pos+strlen(pos)-1;
          if(pos2[0]!=')') {
	    puts("GOSUB: Syntax error: parameter list");
	    structure_warning(i,"GOSUB"); /*Programmstruktur fehlerhaft */
          } else pos2[0]=0;
        } else pos=buf+strlen(buf);
        pcode[i].integer=procnr(buf,1);
      }
    } else if((pcode[i].opcode&PM_SPECIAL)==P_GOTO) { /* Suche Label */
    /*  printf("Goto-Nachbearbeitung, <%s> \n",pcode[i].argument); */
      /* Wenn indirect, dann PREFETCH und IGNORE aufheben */
      if(*(pcode[i].argument)=='&') pcode[i].opcode&=~(P_PREFETCH|P_IGNORE);
      else {
        pcode[i].integer=labelzeile(pcode[i].argument);
        /* Wenn label nicht gefunden, dann PREFETCH und IGNORE aufheben */
        if(pcode[i].integer==-1)  {
	  printf("Label %s not found at line %d!\n",pcode[i].argument,i);
          structure_warning(i,"GOTO"); /*Programmstruktur fehlerhaft */
	  pcode[i].opcode&=~(P_PREFETCH|P_IGNORE);
        }
      }
    } 
    if((pcode[i].opcode&PM_TYP)==P_PLISTE) { /* Nachbearbeiten */
       int j=pcode[i].opcode&PM_COMMS;
       make_pliste2(comms[j].pmin,comms[j].pmax,
	(unsigned short *)comms[j].pliste,pcode[i].argument,&(pcode[i].ppointer),pcode[i].panzahl);
    } 
  }
  free(buffer);free(zeile);
 

  return(0);
}

static int add_label(char *name,int zeile,int dataptr) {
  labels[anzlabels].name=strdup(name);
  labels[anzlabels].zeile=zeile;
  labels[anzlabels].datapointer=dataptr;
  anzlabels++;
  return(anzlabels-1);
}

static void clear_labelliste() {
  int i;
  if(anzlabels) {
    for(i=0;i<anzlabels;i++) free(labels[i].name);
    anzlabels=0;
  }
}
static void clear_parameters(int l) {
  int i;
  if(l && !is_bytecode) {
    for(i=0;i<l;i++) {
    
    
      if(pcode[i].ppointer!=NULL) {

/*Hier ist was falsch, denn nicht alle pointer koennen ge free t werden
bei Arrays sind es z.b. varptr pointer*/

 //       if(pcode[i].ppointer->pointer!=NULL) free(pcode[i].ppointer->pointer);

        free(pcode[i].ppointer);
	pcode[i].ppointer=NULL;
      }
      if(pcode[i].argument!=NULL) free(pcode[i].argument);
      if(pcode[i].extra!=NULL) free(pcode[i].extra);
      pcode[i].argument=NULL;
      pcode[i].extra=NULL;
    }
  }
}


static void clear_procliste() {
  while(anzprocs) {
    anzprocs--;
    free(procs[anzprocs].name);
    free(procs[anzprocs].parameterliste);
  }
}


static int make_varliste(char *argument, int *l,int n) {
  char arg[strlen(argument)+1];
  char *w1,*w2;
  int i=0,e;
  char *r;
  int typ,subtyp,vnr;
  strcpy(arg,argument);
  e=wort_sep_destroy(arg,',',TRUE,&w1,&w2);
  while(e && i<n) {
    r=varrumpf(w1);
    typ=vartype(w1);
    if(typ&ARRAYTYP) {
      subtyp=typ&(~ARRAYTYP);
      typ=ARRAYTYP;
    } else subtyp=0;
    
    l[i++]=vnr=add_variable(r,typ,subtyp);
    free(r);
    e=wort_sep_destroy(w2,',',TRUE,&w1,&w2);
  }
  return(i);
}

/*Prozedurin Liste hinzufuegen */

static int add_proc(char *name,char *pars,int zeile,int typ) {
  int ap,i;
  i=procnr(name,typ);
  if(i==-1) {
    procs[anzprocs].name=strdup(name);
    procs[anzprocs].typ=typ;
    procs[anzprocs].zeile=zeile;
    procs[anzprocs].anzpar=ap=count_parameters(pars);
    if(ap) {
      procs[anzprocs].parameterliste=(int *)malloc(sizeof(int)*ap);
      make_varliste(pars,procs[anzprocs].parameterliste,ap);
    } else procs[anzprocs].parameterliste=NULL;
    anzprocs++;
    return(anzprocs-1);
  } else {
    printf("Error. Procedure/Function %s already exists! --> line %d\n",name,procs[i].zeile);
    return(i);
  }
}


char *indirekt2(char *funktion) {
   char *ergebnis;
  if(funktion[0]=='&') {
    ergebnis=s_parser(funktion+1);
    xtrim(ergebnis,TRUE,ergebnis);
  } else ergebnis=strdup(funktion);
  return(ergebnis);
}


#if DEBUG 

/* Typ von Ausdruecken */

static void dump_type(unsigned int typ) {
  if(typ==NOTYP) printf("notyp ");
  else {
    if(typ&INDIRECTTYP) printf("indirect ");
    if(typ&FILENRTYP) printf("filenr ");
   if(typ&CONSTTYP) printf("const ");
    if(typ&ARRAYTYP) printf("array ");

    if(typ&FLOATTYP) printf("float ");
    if(typ&INTTYP)      printf("int ");
    if(typ&STRINGTYP) printf("string ");
  }
}

/* Bestimmt den Typ eines Ausdrucks */
int type3(char *ausdruck) {
  int ret;
  ret=type(ausdruck);
  printf("TYPE3: <%s> --> ",ausdruck);
  dump_type(ret);
  printf("\n");
  return(ret);
}
#endif



/* Bestimmt den Typ eines Ausdrucks 

*/

unsigned int type(char *ausdruck) {
  char *pos;
  unsigned int typ=0,ltyp=0;
  int i;

/*Hier erstes Trennzeichen suchen (ausserhalb "" und Klammern()[]), dann type getrennt fuer beide teile.
  Trennzeichen=',', ';', ''', ' ', "+-* / ^()|<>=&!" 
  Trennzeichen sind aber auch alle Operatoren wie AND, OR, ....
  
  + und - am Anfang einfach ignorieren
  blank am Anfang ignorieren
  wenn eines kein Konsttyp ist, dann konsttyp loeschen
  */
 // printf("type: <%s>\n",ausdruck);
  if(ausdruck==NULL) return(NOTYP);
  switch(*ausdruck) {
    case 0: 
      return(NOTYP);
    case '+':
    case '-': 
      return(type(ausdruck+1));  /* war Vorzeichen */
    case '&':
      if(ausdruck[1]=='"') return(INDIRECTTYP|CONSTTYP); 
      else return(INDIRECTTYP);
    case '#':
      return(FILENRTYP|type(ausdruck+1));
    default: ; /*Was machen mit ~ und @  ?*/
  }
  {
    char w1[strlen(ausdruck)+1]; /*Ab hier arbeiten wir mit einer Kopie des Ausdrucks*/
    memcpy(w1,ausdruck,strlen(ausdruck)+1);
    if(*w1=='[') {   /*Array const def.*/
      pos=searchchr2(w1+1,']');
      if(pos!=NULL) *pos=0;
      return(ARRAYTYP|type(w1+1));
    }

    pos=searchchr2_multi(w1,",;+- '*/^=<>");
    if(pos!=NULL) {
      char a=*pos;
      // printf("Wir haben folgendes im Ausdruck gefunden: <%s>\n",pos);
      *pos++=0;
      if(a=='=' || a=='<' || a=='>') {
        if(*pos=='=' ||*pos=='<' ||*pos=='>') { /* faengt die operatoen <>,==,<=,>= ab.*/
	  a=*pos++;
	}
      } else if(a==' ') {
        ; /*Hier koennte man die Operatoren OR; AND etc bearbeiten....*/
      }
      
      int rtyp=type(pos);  /*Rechte Seite auswerten */
      int ltyp=type(w1);   /*Linke Seite auswerten */
    //  if(ltyp&CONSTTYP) printf("rechts ist const.\n");
      typ=0;
      if((ltyp&CONSTTYP) && (rtyp&CONSTTYP)) typ|=CONSTTYP;
      if((ltyp&ARRAYTYP) || (rtyp&ARRAYTYP)) typ|=ARRAYTYP;
      if(a=='/') { /*War division, dann kann das Ergebnis nur float sein */
        typ|=FLOATTYP;
      } else if(a=='=' || a=='<' || a=='>') { /*War Vergleichsoperator, dann kann das Ergebnis nur int sein */
        typ|=INTTYP;
      } else {
        if(rtyp&INTTYP) typ|=INTTYP;
	else if(rtyp&FLOATTYP) typ|=FLOATTYP;
	else if(rtyp&STRINGTYP) typ|=STRINGTYP;
	else printf("something is wrong: %x\n",rtyp);
      } 
      return(typ); 
    }

/*  hier ist nun nurnoch ein operand zu untersuchen */
    if(*w1=='(' && w1[strlen(w1)-1]==')') {
      w1[strlen(w1)-1]=0;
      typ=type(w1+1);
    } else {
      pos=searchchr(w1,'(');
      if(pos!=NULL) {
        if(*w1!='@') {
          if(pos==w1) printf("Syntax-error at line %d: Parenthesis? <%s>\n",i,w1);
      /* jetzt entscheiden, ob Array-element oder sub-array oder Funktion */
          char *ppp=pos+1;
          int i=0,flag=0,sflag=0,count=0;
	  *pos=0;
	  if((i=find_func(w1))!=-1) { /* Koennte funktion sein: */
	    if((pfuncs[i].opcode&F_IRET)==F_IRET) typ=INTTYP;
	    else  typ=FLOATTYP;
	    if(pfuncs[i].opcode&F_CONST) {
	      if(ppp[strlen(ppp)-1]==')') ppp[strlen(ppp)-1]=0;
	      else printf("WARNING: Syntay-error at line %d: fehlende schliessende Klammer. <%s>\n",i,ppp);
              if(type(ppp)&CONSTTYP) typ|=CONSTTYP;
            }
	    goto binfertig;
	  } else if((i=find_sfunc(w1))!=-1) { /* Koennte $-funktion sein: */
	    typ=STRINGTYP;
	    if(psfuncs[i].opcode&F_CONST) {
	      if(ppp[strlen(ppp)-1]==')') ppp[strlen(ppp)-1]=0;
	      else printf("WARNING: Syntay-error at line %d: fehlende schliessende Klammer. <%s>\n",i,ppp);
              if(type(ppp)&CONSTTYP) typ|=CONSTTYP;
            }
	    goto binfertig;
	    
	  } else {
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
        
      }
      if((pos-1)[0]=='$') typ=(typ | STRINGTYP);
      else if((pos-1)[0]=='%') typ=(typ | INTTYP);
      else typ=(typ | FLOATTYP);
    } else {
    
      switch(*w1) {
      case 0:   typ=NOTYP; break;
      case '"': typ=CONSTTYP|STRINGTYP;break;
      case '$': typ=INTTYP|CONSTTYP;break;
      default:
        if((i=find_sysvar(w1))!=-1) {  /*Es koennte sysvar sein. Dann testen, ob ggf constant.*/
          typ=sysvars[i].opcode;
        } else {
            if(w1[strlen(w1)-1]=='$') typ=STRINGTYP;
            else if(w1[strlen(w1)-1]=='%') typ=INTTYP;
            else if(*w1=='0' && w1[1]=='X') typ=INTTYP|CONSTTYP;
            else {
              int i,f=0,g=0;
              for(i=0;i<strlen(w1);i++) f|=(strchr("-.1234567890E",w1[i])==NULL);      
              for(i=0;i<strlen(w1);i++) g|=(strchr("1234567890",w1[i])==NULL);
              if(!f && !g) typ=INTTYP|CONSTTYP;
              else typ=FLOATTYP|(f?0:CONSTTYP);
            } 
          }
        }
      }
    }
    binfertig:
    if(ltyp) {
      if((ltyp&CONSTTYP)==0) typ&=(~CONSTTYP);
      if((typ&STRINGTYP)==0 && ltyp&FLOATTYP) {
        typ&=(~INTTYP);
        typ|=FLOATTYP;
      } 
      if((ltyp&INTTYP) && typ&FLOATTYP) {
        ltyp&=(~INTTYP);
        ltyp|=FLOATTYP;
      } 
      #if 0
      if((typ&(STRINGTYP|FLOATTYP|INTTYP|ARRAYTYP))!=
       (ltyp&(STRINGTYP|FLOATTYP|INTTYP|ARRAYTYP))) 
       printf("WARNING: Syntax-error? <%s> $%x $%x\n",ausdruck,typ,ltyp);
     #endif
    }
    return(typ);
  }
}


static int suche(int begin, int richtung, char *such,char *w1,char *w2) {
  int i,f=0;
  char nbuffer[MAXSTRLEN];
  char zbuffer[MAXSTRLEN];
  char sbuffer[MAXSTRLEN];
  char *zeile,*buffer,*b1,*b2;
  
  for(i=begin; (i<prglen && i>=0);i+=richtung) {
    xtrim(program[i],TRUE,zbuffer);
    wort_sep_destroy(zbuffer,'!',TRUE,&zeile,&buffer);
    wort_sep_destroy(zeile,' ',TRUE,&b1,&b2);
    strcpy(sbuffer+1,such); sbuffer[0]='|';
    strcat(sbuffer,"|");
    strcpy(nbuffer+1,b1); nbuffer[0]='|';
    strcat(nbuffer,"|");
    /*printf("SUCHE: %d <%s> <%s> %d\n",i,buffer,nbuffer,f);*/
    if(strstr(sbuffer,nbuffer)!=NULL && f==0) return(i);
    else if(strcmp(b1,w1)==0) f++;
    else if(strcmp(b1,w2)==0) f--;
  }
  return(-1);
}




void kommando(char *cmd) {
  char buffer[strlen(cmd)+1];
  char *w1,*w2;
  char zeile[strlen(cmd)+1];
  int i,a,b;

  wort_sep2(cmd," !",TRUE,zeile,buffer);
  xtrim2(zeile,TRUE,zeile);
  if(wort_sep_destroy(zeile,' ',TRUE,&w1,&w2)==0) return;  /* Leerzeile */
  switch(*w1) {
  case '\'':
    return;  /* Kommentar */
  case '@':
    c_gosub(w1+1);
    return;
  case '~':
    c_void(w1+1);
    return;
  case '&':
    {
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
    return;
  case '(':
  case '-':
     printf("%.13g\n",parser(zeile));
     return;
  }
  
  if(w1[strlen(w1)-1]==':')             return;  /* nixtun, label */

  if(searchchr2(w1,'=')!=NULL) {
    wort_sep_destroy(zeile,'=',TRUE,&w1,&w2);
    xzuweis(w1,w2);
    return;
  }
   if(isdigit(w1[0])) {
     printf("%.13g\n",parser(zeile));
     return;
  } 

  i=find_comm_guess(w1,&a,&b);
  if(i!=-1) {
      if((comms[i].opcode&PM_TYP)==P_IGNORE) return; /*?? das geht nicht*/
      if((comms[i].opcode&PM_TYP)==P_ARGUMENT) (comms[i].routine)(w2);
      else if((comms[i].opcode&PM_TYP)==P_SIMPLE) (comms[i].routine)();
      else if((comms[i].opcode&PM_TYP)==P_PLISTE) {
        PARAMETER *plist;
        int e=make_pliste(comms[i].pmin,comms[i].pmax,(unsigned short *)comms[i].pliste,w2,&plist);
        if(e>=comms[i].pmin) (comms[i].routine)(plist,e);
	if(e!=-1) free_pliste(e,plist);
      } else xberror(38,w1); /* Befehl im Direktmodus nicht moeglich */
    } else if(a!=b) {
       printf("Command needs to be more specific ! <%s...%s>\n",comms[a].name,comms[b].name);
    }  else xberror(32,w1);  /* Syntax Error */
}




/* programmlauf setzt voraus, dass die Strukturen durch init_program vorbereitet sind*/

void programmlauf(){
    int isp,ipc,opc;
#ifdef DEBUG
    int timer;
#endif

  if(is_bytecode) {  
    PARAMETER *p;
    int n;
  #if DEBUG
    printf("Virtual Machine: %d bytes.\n",programbufferlen);
  #endif
    STRING bcpc;  /* Bytecode holder */
    bcpc.pointer=programbuffer;
    bcpc.len=programbufferlen;
    p=virtual_machine(bcpc,&n);
    dump_parameterlist(p,n);  
    free_pliste(n,p);
    return;
  }

    isp=sp;ipc=pc;
    while(batch && pc<prglen && pc>=0 && sp>=isp)  {
      if(echoflag) printf("%s\n",program[pc]);
#ifdef DEBUG
      timer=clock();
#endif
      opc=pc;
      
  //    printf("OPCODE=$%x/$%x $%x",pcode[opc].opcode,P_DEFFN,P_PROC);
      
      
      if(pcode[opc].opcode&P_PREFETCH) pc=pcode[opc].integer;
      else pc++;
      if(pcode[opc].opcode&P_IGNORE) ;
      else if(pcode[opc].opcode&P_EVAL)  kommando(program[opc]);
      else if((pcode[opc].opcode&P_ZUWEIS)==P_ZUWEIS) {
        int *indexliste=NULL;
	int vnr=pcode[opc].integer;
	int typ=variablen[vnr].typ;
	int ii=pcode[opc].panzahl;
	if(ii) {
	  if(typ==ARRAYTYP) {
            int dim=variablen[vnr].pointer.a->dimension;
  	    indexliste=malloc(ii*sizeof(int));
	    get_indexliste(pcode[opc].ppointer,indexliste,ii);
	    if(ii!=dim) xberror(18,"");  /* Falsche Anzahl Indizies */
          } else xberror(18,"");  /* Falsche Anzahl Indizies */
	}
	zuweisxbyindex(vnr,indexliste,ii,pcode[opc].argument);
	free(indexliste);
      }
      else if((pcode[opc].opcode&PM_TYP)==P_SIMPLE) {
        (comms[pcode[opc].opcode&PM_COMMS].routine)(NULL);
      } else if(pcode[opc].opcode&P_INVALID) xberror(32,program[opc]); /*Syntax nicht korrekt*/
      else if((pcode[opc].opcode&PM_COMMS)>=anzcomms) {
	xberror(36,program[opc]); /*Programmstruktur fehlerhaft !*/
   //     kommando(program[opc]);
      } else {
        if((pcode[opc].opcode&PM_TYP)==P_ARGUMENT)
          (comms[pcode[opc].opcode&PM_COMMS].routine)(pcode[opc].argument);
        else if((pcode[opc].opcode&PM_TYP)==P_PLISTE) {

/*Hier ist ja schon das meiste vorbereitet !*/

	  PARAMETER *plist;
	  int i=pcode[opc].opcode&PM_COMMS;
          int e=make_pliste3(comms[i].pmin,comms[i].pmax,(unsigned short *)comms[i].pliste,
	     pcode[opc].ppointer,&plist,pcode[opc].panzahl);
          (comms[i].routine)(plist,e);
	  if(e!=-1) free_pliste(e,plist);
        } else printf("something is wrong: %x %s\n",pcode[opc].opcode,program[opc]);
      }
#ifdef DEBUG
      ptimes[opc]=(int)((clock()-timer)/1000);  /* evaluiert die
                                                   Ausfuehrungszeit der Programmzeile */
#endif
    }
}
