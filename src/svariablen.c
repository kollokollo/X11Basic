/* svariablen.c   system variables handling (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h> 
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "parser.h"
#include "xbasic.h"
#include "type.h"
#include "parameter.h"
#include "svariablen.h"
#include "io.h"
#include "wort_sep.h"

#ifdef DUMMY_LIST
#define v_false NULL
#define v_true NULL
#define v_ccsaplid NULL
#define v_ccserr NULL
#define v_cols NULL
#define v_crscol NULL
#define v_crslin NULL
#define v_ctimer NULL
#define v_err NULL
#define v_gpsalt NULL
#define v_gpslat NULL
#define v_gpslon NULL
#define mousek NULL
#define mouses NULL
#define mousex NULL
#define mousey NULL
#define v_pc NULL
#define v_pi NULL
#define v_rows NULL
#define v_sensorf NULL
#define v_gpiof NULL
#define v_sp NULL
#define v_stimer NULL
#define v_timer NULL
#else

/* system variables with return type <int> */

extern int mousex(),mousey(), mousek(), mouses();

static int v_false() {return(0);}
static int v_true()  {return(-1);}
static int v_err()   {extern int globalerr; return(globalerr);}
static int v_sp()    {return(sp);}
static int v_pc()    {return(pc);}

#ifdef ANDROID
static int v_sensorf() {return(ANZSENSORS);}
#endif
#ifdef HAVE_WIRINGPI
static int v_gpiof() {return(32);}
#endif
static int v_stimer() {   /* seconds timer */
  time_t timec=time(NULL);
  if(timec==-1) io_error(errno,"TIMER");
  return(timec);
}

static int v_cols() {   /* number of colums of the terminal */
  int rows=0,cols=0;
  getrowcols(&rows,&cols);
  return(cols);
}
static int v_rows() {   /* number of rows of the terminal */
  int rows=0,cols=0;
  getrowcols(&rows,&cols);
  return(rows);
}
static int v_crscol() {   /* Terminal cursor position */
  int rows=0,cols=0;
  getcrsrowcol(&rows,&cols);
  return(cols);
}
static int v_crslin() {   /* Terminal cursor position */
  int rows=0,cols=0;
  getcrsrowcol(&rows,&cols);
  return(rows);
}

/* system variables with return type <float> */

#ifdef ANDROID
extern double gps_alt,gps_lat,gps_lon;

static double v_gpsalt() {return(gps_alt);}
static double v_gpslat() {return(gps_lat);}
static double v_gpslon() {return(gps_lon);}
#else
static double v_gpsalt() {return(0);}
static double v_gpslat() {return(-1);}
static double v_gpslon() {return(-1);}
#endif

static double v_timer() {
#ifdef WINDOWS
#if 0
  /* GetTickCount() starts counting at system start/boot */
  return((double)GetTickCount()/1000.0);
#endif
#if 0
  /* clock() starts counting at program start */
  return((double)clock()/CLOCKS_PER_SEC);
#endif
  double fraction=GetTickCount()/1000.0;
  static time_t st_tt=-1;
  static double st_fr=-1;
  if(st_tt==-1 || st_fr>fraction) { /* GetTickCount() overflows after 49 days.*/
    time_t ltime;
    time(&ltime);
    // struct tm* timeinfo = gmtime(&ltime); /* Convert to UTC */
    // ltime = mktime(timeinfo); /* Store as unix timestamp */

    st_tt=ltime;
    st_fr=fraction;
  }
  return((double)st_tt+fraction-st_fr);
#else
  struct timeval t;
  struct timezone tz;
  gettimeofday(&t,&tz);
  return((double)t.tv_sec+(double)t.tv_usec/1000000);
#endif
}
static double v_ctimer() {return((double)clock()/CLOCKS_PER_SEC);}
static double v_pi() {return(PI);}
#endif

const SYSVAR sysvars[]= {  /* alphabetical order is required !!! */
 { NOTYP,           "!nulldummy", (pfunc)v_false},
#ifdef ANDROID
 { CONSTTYP|INTTYP, "ANDROID?",   (pfunc)v_true},
#else
 { CONSTTYP|INTTYP, "ANDROID?",   (pfunc)v_false},
#endif
#ifdef CONTROL
 { INTTYP,          "CCSAPLID",   (pfunc)v_ccsaplid},
#endif
#if defined CONTROL || defined TINE
 { INTTYP,          "CCSERR",     (pfunc)v_ccserr},
#endif
 { INTTYP,          "COLS",       (pfunc)v_cols},
 { INTTYP,          "CRSCOL",     (pfunc)v_crscol},
 { INTTYP,          "CRSLIN",     (pfunc)v_crslin},
 { FLOATTYP,        "CTIMER",      v_ctimer},
 { INTTYP,          "ERR",        (pfunc)v_err},
 { CONSTTYP|INTTYP, "FALSE",      (pfunc)v_false},
#ifdef HAVE_WIRINGPI
 { CONSTTYP|INTTYP, "GPIO?",      (pfunc)v_gpiof},
#else
 { CONSTTYP|INTTYP, "GPIO?",      (pfunc)v_false},
#endif
#ifdef ANDROID
 { CONSTTYP|INTTYP, "GPS?",       (pfunc)v_true},
#else
 { CONSTTYP|INTTYP, "GPS?",       (pfunc)v_false},
#endif
 { FLOATTYP,        "GPS_ALT",     v_gpsalt}, /* Altitude in meters*/
 { FLOATTYP,        "GPS_LAT",     v_gpslat}, /* Lattitude in degrees*/
 { FLOATTYP,        "GPS_LON",     v_gpslon}, /* Longitude in degrees*/
#ifndef NOGRAPHICS
 { INTTYP,          "MOUSEK",     (pfunc)mousek},
 { INTTYP,          "MOUSES",     (pfunc)mouses},
 { INTTYP,          "MOUSEX",     (pfunc)mousex},
 { INTTYP,          "MOUSEY",     (pfunc)mousey},
#endif
 { INTTYP,          "PC",         (pfunc)v_pc},
 { CONSTTYP|FLOATTYP, "PI",        v_pi},
 { INTTYP,          "ROWS",	  (pfunc)v_rows},
#ifdef ANDROID
 { CONSTTYP|INTTYP, "SENSOR?",    (pfunc)v_sensorf},
#else
 { CONSTTYP|INTTYP, "SENSOR?",    (pfunc)v_false},
#endif
 { INTTYP,          "SP",	  (pfunc)v_sp},
 { INTTYP,          "STIMER",     (pfunc)v_stimer},
 { FLOATTYP,        "TIMER",       v_timer},
 { CONSTTYP|INTTYP, "TRUE",       (pfunc)v_true},
#ifdef WINDOWS
 { CONSTTYP|INTTYP, "WIN32?",     (pfunc)v_true},
#else
 { CONSTTYP|INTTYP, "WIN32?",     (pfunc)v_false},
#endif
#ifndef WINDOWS
 { CONSTTYP|INTTYP, "UNIX?",      (pfunc)v_true}
#else
 { CONSTTYP|INTTYP, "UNIX?",      (pfunc)v_false}
#endif
};
const int anzsysvars=sizeof(sysvars)/sizeof(SYSVAR);


/* system variables with return type <string> */

#ifdef DUMMY_LIST
#define vs_error        NULL
#define vs_date         NULL
#define vs_fileevent    NULL
#define vs_inkey        NULL
#define vs_terminalname NULL
#define vs_time         NULL
#define vs_trace        NULL
#else
STRING vs_error()       { return(create_string("<ERROR>")); }
static STRING vs_inkey(){ return(create_string(inkey())); }
static STRING vs_date() {
  STRING ergebnis;
  time_t timec;
  struct tm * loctim;
  timec = time(&timec);
  loctim=localtime(&timec);
  ergebnis.pointer=malloc(12);
  sprintf(ergebnis.pointer,"%02d.%02d.%04d",loctim->tm_mday,loctim->tm_mon+1,1900+loctim->tm_year);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}

static STRING vs_time() {
  STRING ergebnis;
  time_t timec;
  timec = time(&timec);
  ergebnis.pointer=malloc(9);
  strncpy(ergebnis.pointer,ctime(&timec)+11,8);
  ergebnis.len=8;
  ergebnis.pointer[8]=0;
  return(ergebnis);
}
static STRING vs_trace() {
  if(pc>=0 && pc<prglen) {
    STRING ergebnis=create_string(program[pc]);
    xtrim(ergebnis.pointer,TRUE,ergebnis.pointer);
    ergebnis.len=strlen(ergebnis.pointer);
    return(ergebnis);
  } else return(vs_error());
}
static STRING vs_terminalname() {
  STRING ergebnis;
  ergebnis.pointer=terminalname(STDIN_FILENO);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
static STRING vs_fileevent() {
  STRING ergebnis;
  ergebnis.pointer=fileevent();
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
#endif

const SYSSVAR syssvars[]= {  /* alphabetical order required !!! */
 { PL_LEER,   "!nulldummy",    vs_error},
 { PL_STRING, "DATE$",         vs_date},
 { PL_STRING, "FILEEVENT$",    vs_fileevent},
 { PL_STRING, "INKEY$",        vs_inkey},
 { PL_STRING, "TERMINALNAME$", vs_terminalname},
 { PL_STRING, "TIME$",         vs_time},
 { PL_STRING, "TRACE$",        vs_trace}
};
const int anzsyssvars=sizeof(syssvars)/sizeof(SYSSVAR);
