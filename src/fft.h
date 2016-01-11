/* fft.h  interface to fftpack from X11-Basic  (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#define FLOAT double
#define TWOPI 6.28318530717958647692528676655900577
#define PIHALF 1.57079632679489661923132169163975
#define SQRT2 1.4142135623730950488016887242097
#define TSQRT2 2.8284271247461900976033774484194

void rffti(int n, FLOAT *wsave, int *ifac);
void rfftf(int n,FLOAT *r,FLOAT *wsave,int *ifac);
void rfftb(int n, FLOAT *r, FLOAT *wsave, int *ifac);
