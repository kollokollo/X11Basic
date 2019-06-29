/* keywords.c defines some of the X11-Basic keywords (c) Markus Hoffmann             */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keywords.h"

const char *keywords[]={"","ON","OFF","CONT","THEN","MENU","IF","TO","DOWNTO","STEP","USING","ERROR","BREAK","GOTO","GOSUB"};
const int anzkeywords=sizeof(keywords)/sizeof(char *);

int keyword2num(char *t) {
  for(int i=0;i<anzkeywords;i++) {
    if(strcmp(t,keywords[i])==0) {
  //    printf("keyw2num: <%s> --> %x\n",t,i);
      return(i);
    }
  }
  return(KEYW_UNKNOWN);
}
