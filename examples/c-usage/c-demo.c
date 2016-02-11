
/* Example Program how to link X11-Basic Object codes (produced by xbc)
   with your c-Program                        (c) Markus Hoffmann 2004   */

/* Example Program how to use the x11basic-Library
   with your c-Program                        (c) Markus Hoffmann 2004   */


/* You will have to compile this (testme.c) and link it with the 
   x11basic shared library. Also ohers are required as follos:

gcc -fomit-frame-pointer -o testme -L/usr/X11R6/lib -lx11basic -ldl -lm 
    -lreadline -lncurses  -lasound -lX11 testme.c library.o

*/

#include <stdlib.h>
#include <stdio.h>
#include <x11basic/x11basic.h>

/* X11-Basic needs these declarations:  */

int programbufferlen=0;
char ifilename[]="dummy";     /* Program name.   Put some useful information here */
char *programbuffer=NULL;
const char version[]="dummy"; /* Version Number. Put some useful information here */
const char vdate[]="dummy";   /* Creation date.  Put some useful information here */
char *program[]={"Hallo"};    /* Other comments. Put some useful information here */
int prglen=sizeof(program)/sizeof(char *);
int verbose=0;


/* Here your c program starts */

main(int anzahl, char *argumente[]) {
  double ret;
  /* Initialize the x11basic-library */
  x11basicStartup(); set_input_mode(1,0);
  atexit(reset_input_mode);
  param_anzahl=anzahl;
  param_argumente=argumente;

  /* Here your code can follow */

  /* ... */
 
  /* ... */
  
  /* If you like, you can use single x11basic-commands: */

  kommando("PBOX 10,10,100,100");/* This is for single command execution: */
  kommando("SHOWPAGE");
  kommando("KEYEVENT");
  /* ... */

}
