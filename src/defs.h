/* defs.h (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#ifndef __X11BASICDEFS
#define __X11BASICDEFS

#include "config.h"
#include "options.h"

#ifndef FALSE 
  #define FALSE    0
  #define TRUE     (!FALSE)
#endif
#define PI       3.141592653589793
#define E        2.718281828459

#ifdef SMALL
  #define SAVE_RAM 1
#endif

#ifdef SAVE_RAM
  #define MAXSTRLEN   1024   /* in Bytes */
  #define MAXPRGLEN  64000   /* in Bytes */
  #define MAXLINELEN  1024   /* in Bytes */
  #define MAXVARNAMLEN  64   /* in Bytes */
  #define ANZFILENR     64
  #define STACKSIZE    256
  #define ANZVARS     1024
  #define ANZLABELS    256
  #define ANZPROCS     512
#else
  #define MAXSTRLEN   4096   /* in Bytes */
  #define MAXPRGLEN 100000   /* in Bytes */
  #define MAXLINELEN  4096   /* in Bytes */
  #define MAXVARNAMLEN  64   /* in Bytes */
  #define ANZFILENR    100
  #define STACKSIZE    512
  #define ANZVARS     4096
  #define ANZLABELS   1024
  #define ANZPROCS    1024
#endif
#define DEFAULTWINDOW 1

#define INTSIZE sizeof(double)


#define round(a) ((int)(a+0.5))
#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a>b)?a:b)
#define sgn(x)   ((x<0)?-1:1)
#define rad(x)   (PI*x/180)
#define deg(x)   (180*x/PI)
#ifndef WINDOWS
#ifndef LOBYTE
  #define LOBYTE(x) ((unsigned char) ((x) & 0xff))
#endif
#ifndef HIBYTE
  #define HIBYTE(x) ((unsigned char) ((x) >> 8 & 0xff))
#endif
#endif
#endif
