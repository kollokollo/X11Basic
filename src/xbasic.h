/* XBASIC.H (c) Markus Hoffmann  */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include "defs.h"
#include "ptypes.h"
#include "vtypes.h"
#include "globals.h"

#define PI       3.141592654
#define E        2.718281828


#define round(a) ((int)(a+0.5))
#define min(a,b) ((a<b)?a:b)
#define max(a,b) ((a>b)?a:b)
#define sgn(x)   ((x<0)?-1:1)
#define rad(x)   (3.141592654*x/180)
#define deg(x)   (180*x/3.141592654)


