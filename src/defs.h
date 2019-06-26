/* defs.h (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#ifndef __X11BASICDEFS
#define __X11BASICDEFS

#ifdef ATARI
#include "config.h.atari"
#else
#include "config.h"
#endif
#include "options.h"

#ifdef SMALL_RAM
  #define MAXSTRLEN      1024   /* in Bytes */
  #define MAXLINELEN     1024   /* in Bytes */
  #define ANZFILENR        64
  #define MAXSTACKSIZE   1024
  #define STACKINCREMENT   64
  #define MAXANZVARS     1024
  #define VARSINCREMENT    64
  #define MAXANZLABELS    256
  #define LABELSINCREMENT  16
  #define MAXANZPROCS     512
  #define PROCSINCREMENT   16
#else
  #define MAXSTRLEN      4096   /* in Bytes */
  #define MAXLINELEN     4096   /* in Bytes */
  #define ANZFILENR       100
  #define MAXSTACKSIZE (512*16)
  #define STACKINCREMENT  256
  #define MAXANZVARS     4096
  #define VARSINCREMENT    64
  #define MAXANZLABELS   1024
  #define LABELSINCREMENT  32
  #define MAXANZPROCS    1024
  #define PROCSINCREMENT   32
#endif
#define DEFAULTWINDOW 1



/* Hier nehmen wir in jedem Fall 8 Bytes an.*/
#define INTSIZE sizeof(double)

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a>b)?a:b)
#define sgn(x)   ((x<0)?-1:1)
#define rad(x)   (PI*x/180)
#define deg(x)   (180*x/PI)
#ifndef LOBYTE
  #define LOBYTE(x) ((unsigned char) ((x) & 0xff))
#endif
#ifndef HIBYTE
  #define HIBYTE(x) ((unsigned char) ((x) >> 8 & 0xff))
#endif
#ifdef WINDOWS
  #define bzero(p, l) memset(p, 0, l)
#endif
#ifdef ATARI
  void xberror(char errnr, const char *bem);
  static void *mymalloc(size_t d) {
    void *a=malloc(d);
    if(a==0) {
      xberror(8,"malloc"); /* Out of Memory error.... */
      printf("Malloc(%d) --> #%x  failed !\n",(int)d,(int)a);
      a=malloc(1024);
    } 
    return(a);
  }
  #define malloc(a) mymalloc(a)
  
#endif
#endif
