/* VM-API.C (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#include "defs.h"
#include "x11basic.h"

#include "memory.h"
#include "type.h"
#include "parser.h"
#include "variablen.h"
#include "parameter.h"
#include "array.h"



/*
 * These routines allow the access from X11-Basic compiled 
 * functions (in shared libraries etc.) 
 * vm-api.c is normally included in libx11basic.so   
 */

/* Diese Routinen sollten als Interface zur Virtual machine dienen.
   Sie muessen in das .c file integriert werden. Ist nur
   wichtig, wenn man C-Libraries aus X11-Basic Funktionen compilieren
   moechte.
   */


/* Schreibt einen einzelnen Parameter in "C" style als X11-Basic 
 * Parameter auf den X11-Basic Operanden-Stack
 * Der C-Stype parametere wird aus der Liste antfernt.
 */

static void do_pusharg(va_list *arg, unsigned char typ, PARAMETER **sp) {
  ARRAY a;
  STRING s;
  double f;
  int i;
  PARAMETER *opstack=*sp;
  va_list arguments;     /*Patch von Matthias Vogl, 29.12.2012*/
  va_copy(arguments, *arg);
  switch(typ) {
  case 'a': a=va_arg ( arguments, ARRAY );  *((ARRAY *)INT2POINTER(opstack->integer))=double_array(&a); opstack->typ=PL_ARRAY; opstack++; break;
  case 's': s=va_arg ( arguments, STRING ); *((STRING *)INT2POINTER(opstack->integer))=double_string(&s); opstack->typ=PL_STRING; opstack++; break;
  case 'f': f=va_arg ( arguments, double ); opstack->real=f; opstack->typ=PL_FLOAT; opstack++;  break;
  case 'i': i=va_arg ( arguments, int );    opstack->integer=i; opstack->typ=PL_INT; opstack++;    break;
  case '.':   va_arg ( arguments, int );  /*parameter abraeumen*/
  case ' ':
  default:
    opstack->typ=PL_LEER; opstack++;
    break;
  }
  *sp=opstack;
  va_copy(*arg, arguments); /*Patch von Matthias Vogl, 29.12.2012*/
}


/* API for the virtual machine: 
   These are the functions offered: */


/* Push all Arguments in "C" style from the list (va_list) to the
 * X11-Basic parameter stack. The types are defined in a separate list.
 */
int pusharg(PARAMETER **opstack, char *typ,...)   {
  int count=0;
  unsigned char c;
  va_list arguments;    
  va_start ( arguments, typ ); 
  while((c=typ[count++])) do_pusharg(&arguments,c,opstack);
  va_end ( arguments );                  // Cleans up the list
  (*opstack)->integer=count; (*opstack)->typ=PL_INT; (*opstack)++;
  return(count);
}  /* jetzt kann die X11-Basic Funktion aufgerufen werden... */


/* Call an X11-Basic integer function (wrapped in C) with 
 * a list of parameters, types in a separate list. */

int callifunc(PARAMETER **opstack,void (*name)(),char *typ,...) {
  int count=0;
  unsigned char c;
  PARAMETER *osp=*opstack;
  va_list arguments;    
  va_start(arguments,typ); 
  while((c=typ[count++])) do_pusharg(&arguments,c,opstack);
  va_end ( arguments );                  // Cleans up the list
  (*opstack)->integer=count; (*opstack)->typ=PL_INT; (*opstack)++;
  name();
  return(osp->integer);
}
/* Call an X11-Basic function (wrapped in C) with 
 * a list of parameters, types in a separate list. */
double callfunc(PARAMETER **opstack,void (*name)(),char *typ,...) {
  int count=0;
  unsigned char c;
  PARAMETER *osp=*opstack;
  va_list arguments;    
  va_start(arguments,typ); 
  while((c=typ[count++])) do_pusharg(&arguments,c,opstack);
  va_end ( arguments );                  // Cleans up the list
  (*opstack)->integer=count; (*opstack)->typ=PL_INT; (*opstack)++;
  name();
  return(osp->real);
}
/* Call an X11-Basic string function (wrapped in C) with 
 * a list of parameters, types in a separate list. */
STRING callsfunc(PARAMETER **opstack,void (*name)(),char *typ,...) {
  int count=0;
  unsigned char c;
  PARAMETER *osp=*opstack;
  va_list arguments;    
  va_start(arguments,typ); 
  while((c=typ[count++])) do_pusharg(&arguments,c,opstack);
  va_end ( arguments );                  // Cleans up the list
  (*opstack)->integer=count; (*opstack)->typ=PL_INT; (*opstack)++;
  name();
  return(*((STRING *)&(osp->integer)));
}
/* Call an X11-Basic array function (wrapped in C) with 
 * a list of parameters, types in a separate list. */
ARRAY callafunc(PARAMETER **opstack,void (*name)(),char *typ,...) {
  int count=0;
  unsigned char c;
  PARAMETER *osp=*opstack;
  va_list arguments;    
  va_start(arguments,typ); 
  while((c=typ[count++])) do_pusharg(&arguments,c,opstack);
  va_end ( arguments );                  // Cleans up the list
  (*opstack)->integer=count; (*opstack)->typ=PL_INT; (*opstack)++;
  name();
  return(*((ARRAY *)&(osp->integer)));
}
