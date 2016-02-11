/* defs.h (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
#ifndef __X11BASICDEFS
#define __X11BASICDEFS
 
#define FALSE    0
#define TRUE     (!FALSE)
#define PI       3.141592654
#define E        2.718281828


#define MAXSTRLEN   4096   /* in Bytes */
#define MAXPRGLEN 100000   /* in Bytes */
#define MAXLINELEN  4096
#define MAXVARNAMLEN  64
#define STACKSIZE    512
#define ANZVARS     1024
#define ANZLABELS   1000
#define ANZPROCS    1000

#define DEFAULTWINDOW 1

#define INTSIZE sizeof(double)


#define round(a) ((int)(a+0.5))
#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a>b)?a:b)
#define sgn(x)   ((x<0)?-1:1)
#define rad(x)   (3.141592654*x/180)
#define deg(x)   (180*x/3.141592654)

#endif
