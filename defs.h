/* defs.h (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */
 
#define FALSE    0
#define TRUE     (!FALSE)

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
