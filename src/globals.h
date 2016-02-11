/* GLOBALS.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


/* Globale Variablen nur definition*/
#include "ptypes.h"
extern const char version[];           /* Programmversion           */
extern const char vdate[];
extern const char xbasic_name[];
extern char ifilename[];
int loadfile,runfile,daemonf;
int pc,sp,prglen,echo,batch,err,errcont,everyflag;
int everytime,alarmpc;
extern int stack[];
char *program[MAXPRGLEN];
P_CODE pcode[MAXPRGLEN];
int  ptimes[MAXPRGLEN];   /* Ausfuehrungszeien der Programmzeilen  */ 

int param_anzahl;             /* fuer PARAM$()*/
char **param_argumente;

char *programbuffer;
int programbufferlen;
int datazeile;              /* fuer Restore, read , DATA */
char *datapos;
char databuffer[MAXSTRLEN];


/* fuer die Dateiverwaltung     */
extern FILE *dptr[];
extern int filenr[];


extern VARIABLE variablen[];
int anzvariablen;

extern COMMAND comms[];
extern LABEL labels[];
extern PROC  procs[];
int anzlabels, anzprocs;
int anzcomms;

union {double f;
       char *str;} returnvalue;

int turtlex,turtley;
extern int usewindow;
double ltextwinkel,ltextxfaktor,ltextyfaktor;
int ltextpflg;


/* Kontrollsystem */

  int aplid;
  int ccs_err;
