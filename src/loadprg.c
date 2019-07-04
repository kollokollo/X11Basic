/* loadprg.c  (c) Markus Hoffmann */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "xbasic.h"
#include "parser.h"
#include "type.h"
#include "parameter.h"
#include "graphics.h"
#include "window.h"

P_CODE *pcode=NULL;     /* Preprocessor code goes into an array of this structure */
int pc=0;               /* Program counter*/
char *databuffer=NULL;  /* for storage of all DATA contents. */
int databufferlen=0;    /* length of the DATA conent area */
int datapointer=0;      /* pointer to the next READ position */

LABEL *labels=NULL;     /* storage for labels */
int labels_size=0;      /* allocated size for labels */
int anzlabels=0;        /* number of used labels */
PROCEDURE *procs=NULL;  /* storage for procedures and functions */
int procs_size=0;       /* allocated size for procedures and functions */
int anzprocs=0;         /* number of used procedures and functions */

PARAMETER returnvalue;  /* general return value for functions */

int *stack=NULL;        /* stack area for return addresses (line numbers) */
int stack_size=0;       /* size of the allocated stack memory */
int sp=0;               /* stack pointer */

/* for global commandline parameters */
int param_anzahl;
char **param_argumente=NULL;

/* for EVERY and AFTER */
int everytime=0,alarmpc=-1,alarmpctype=0;





/*
 * Removes all struktures/variables/labels/procs/data from memory, 
 * including stack clearance, 
 * such that a new program (.bas or bytecode) cn be loaded. 
 */

void clear_program() {
  restore_all_locals();  /* sp=0 */
  remove_all_variables();
  if(!is_bytecode) free(databuffer);
  is_bytecode=0;
  databuffer=NULL;
  databufferlen=0;
  datapointer=0;
  clear_labelliste();
  clear_procliste();
}

/* cleans up the pcode strukture and frees all allocaed memory
 */

void free_pcode(int l) {
  while(l>0) {
    l--;
    if(pcode[l].ppointer!=NULL) {
      free_pliste(pcode[l].panzahl,pcode[l].ppointer);
      pcode[l].ppointer=NULL;
    }
    if(pcode[l].rvalue!=NULL) {
      free_parameter(pcode[l].rvalue);
      free(pcode[l].rvalue);
      pcode[l].rvalue=NULL;
    }
    free(pcode[l].argument);
    free(pcode[l].extra);
    pcode[l].argument=NULL;
    pcode[l].extra=NULL;
    pcode[l].panzahl=0;
  }
  if(pcode) free(pcode);
  pcode=NULL;
}


/* remove program from memory and clear programbuffer */
void newprg() {
#ifndef DUMMY_LIST
  batch=0;             /* Stop a running program */
  pc=0;
#endif
  clear_program();     /* clear stack and databuffer, delete locals, variables, labels, and procs */
  free_pcode(prglen);  /* clear pcode and free all memory assigned to it */
  programbufferlen=prglen=0;
}

/* Load BASIC program from file into memory 
 * Return value:  0 -- everything OK
 *               <0 -- ERRORs detected
 *               >0 -- WARNINGS detected
 */

int loadprg(const char *filename) {
  newprg();
  return(mergeprg(filename));
}



/* gently quit the X11-Baisic interpreter and clean up. 
 */

void quit_x11basic(int c) {
#ifdef ANDROID
  invalidate_screen();
  sleep(1);
#endif
#ifndef NOGRAPHICS
  close_window(&window[usewindow]); 
#endif
  /* cleanup */
  newprg();
  if(programbuffer) free(programbuffer);
 // if(program) free(program); we would not do it, because this causes problems with xbc - compiler
#ifdef CONTROL
  cs_exit();
#endif
#ifdef USE_GEM
// APPL_EXIT
// close VDI workstation
#endif
  exit(c); 
}
