/* GLOBALS.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


/* Globale Variablen nur definition*/
#include "defs.h"
#include "ptypes.h"
extern const char version[];           /* Programmversion           */
extern const char vdate[];
extern const char xbasic_name[];
extern char ifilename[];
extern int pc,sp,prglen,echoflag,batch,globalerr,errcont,breakcont,everyflag;
extern int everytime,alarmpc;
extern int menuaction;

extern int stack[];
extern char *program[];
P_CODE pcode[MAXPRGLEN];
int  ptimes[MAXPRGLEN];   /* Ausfuehrungszeien der Programmzeilen  */ 

int param_anzahl;             /* fuer PARAM$()*/
char **param_argumente;

extern char *programbuffer;
extern int programbufferlen;
int datazeile;              /* fuer Restore, read , DATA */
char *datapos;
char databuffer[MAXSTRLEN];


/* fuer die Dateiverwaltung     */
extern FILE *dptr[];
extern int filenr[];


extern VARIABLE variablen[];
extern int anzvariablen;

extern const COMMAND comms[];
extern LABEL labels[];
extern PROCEDURE  procs[];
extern int anzlabels, anzprocs;
extern const int anzcomms;

union {double f;
       STRING str;} returnvalue;

int turtlex,turtley;
extern int usewindow;
double ltextwinkel,ltextxfaktor,ltextyfaktor;
int ltextpflg;


/* Kontrollsystem */

  int aplid;
  int ccs_err;
