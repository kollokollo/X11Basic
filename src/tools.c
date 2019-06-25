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

/* Find Numer des Kommandos w1 aus Liste. 
   Wenn w1 nur ein Teil des kommandos ist, wird es trotzdem gefunden, 
   sofern es eindeutig zuordnenbar ist. 
  Wenn nicht gefunden, -1 zurueck.
*/

int find_comm(const char *w1) {
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


int find_func(const char *w1) {
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

int find_afunc(const char *w1) {
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

int find_sfunc(const char *w1) {
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
