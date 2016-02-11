/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

void four1(double *data,int nn,int isign);
void realft(double *data,int n,int isign);
void linear_fit(double *x, double *y, int anz, double *err, int flag, 
  double *a, double *b, double *aerr, double *berr, double *chi2, double *q);
void linear_fit_exy(double *x, double *y, int anz, double sigx[], double sigy[],
  double *a, double *b, double *siga, double *sigb, double *chi2, double *q);
double *SVD(double *a, double *w, double *v,int anzzeilen, int anzspalten);
double *backsub(double *singulars, double *u, double *v, double *b, int n, int m);
