 /* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "defs.h"
#include "x11basic.h"
#include "xbasic.h"

P_CODE *pcode=NULL;    /*Preprocessor code goes into an array of this structure */

/*  Fuer DATA statements */
char *databuffer=NULL;  /* Hier werden alle DATA-Inhalte gespeichert.*/
int databufferlen=0;
int datapointer=0; 

VARIABLE variablen[ANZVARS];
int anzvariablen;
LABEL labels[ANZLABELS];
int anzlabels=0;
PROCEDURE procs[ANZPROCS];
int anzprocs=0;





/* Load BASIC program from file into memory 
 * Return value: 0 -- everything OK
 *               <0 -- ERRORs detected
 *               >0 -- WARNINGS detected
 */

int loadprg(const char *filename) {
#ifndef DUMMY_LIST
  batch=0;   /* Stoppen */
  pc=0;
#endif
  clear_program();  /*Stack und databuffer aufräumen, locals, variablen, labels, procs loeschen, */
  free_pcode(prglen);  /*pcode aufraeumen*/
  programbufferlen=prglen=0;
  return(mergeprg(filename));
}
