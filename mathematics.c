/* mathematics.c */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "defs.h"
#include "x11basic.h"
#include "mathematics.h"
#include "fft.h"


/*Gibt 1 zurueck wenn sich der Punkt (x,y) innerhalb des
Polygonzugs befindet*/

int inpolygon(double x, double y, const double *xp, const double *yp, int npol) {
  int i,j,c=0;
  for(i=0,j=npol-1;i<npol;j=i++) {
    if((((yp[i]<=y) && (y<yp[j])) || ((yp[j]<=y) && (y<yp[i]))) &&
            (x<(xp[j]-xp[i])*(y-yp[i])/(yp[j]-yp[i])+xp[i])) c=!c;
  }
  return c;
}

double determinante(double *a,int n) {
  int i,j,j1,j2;
  double det;
  double *m = NULL;

  if(n<1) det=0;
  else if(n==1) det=*a;
  else if(n==2) det=a[0]*a[3]-a[1]*a[2];
  else {
    det=0;
    for(j1=0;j1<n;j1++) {
      if(a[j1]!=0) {
        m=malloc((n-1)*(n-1)*sizeof(double));
        for(i=1;i<n;i++) {
          j2=0;
          for (j=0;j<n;j++) {
            if(j==j1)  continue;
            m[(i-1)*(n-1)+j2]=a[i*n+j];
            j2++;
          }
        }
	det+=((j1&1)?-1.0:1.0)*a[j1]*determinante(m,n-1);
        free(m);
      }
    }
  }
  return(det);
}






/* Routinen f"uer Fouriertransformation   */

void realft(double *data, int n,int isign) {
  static int oldn=-1;
  static double *wsave=NULL;
  static int ifac[15];
  if(n<=0) return;
  if(n>1 && n!=oldn) {
    wsave=realloc(wsave,(2*n+15)*sizeof(double));
    rffti(n,wsave,ifac);
#ifdef DEBUG
    printf("Initialized for n=%d:\n",n);
    if(n>1 && ifac[1]>0) {
      int i;
      for(i=2;i<2+ifac[1];i++) {
        printf("ifac[%d]=%d\n",i,ifac[i]);
      }
    }
#endif    
    oldn=n;
  }
  if(isign) {
#ifdef DEBUG
    int i;
    for(i=0;i<10;i++) printf("data[%d]=%g\n",i,data[i]);
#endif  
  
    rfftf(n,data,wsave,ifac);
  } else rfftb(n,data,wsave,ifac);

}

#if 0
/* Mithilfe des solvers (obsolete) */
void linear_fit2(double *x, double *y, int anz, double *err, int flag, 
     double *a, double *b, double *aerr, double *berr, double *chi2, double *q) {
  double ergeb[2];
  int i;
  double m[anz*2],v[anz];
  
  if(flag) {
    double weight;
    for(i=0;i<anz;i++) {
      weight=1/sqrt(err[i]);
      m[2*i]=weight;
      m[2*i+1]=x[i]*weight;
      v[i]=y[i]*weight;
    }
  } else {
    for(i=0;i<anz;i++) {
      m[2*i]=1;
      m[2*i+1]=x[i];
      v[i]=y[i];
    }
  }
  solve(v,m,anz,2,ergeb);
  *a=ergeb[0];
  *b=ergeb[1];
}
#endif
/* Die werte sollten noch normiert werden, damit sie bei ca 1 liegen und 
    hoehere Potenzen von x nicht zum overflow fuehren.
    Die routine ist vielleicht nicht die schnellste, aber funktioniert.
    (c) Markus Hoffmann fuer X11-Basic*/


void polynom_fit(double *x, double *y, int anz, double *err, int flag, double *ergeb,int n) {
  int i,j;
  double m[anz*n],v[anz];
  
  if(flag) {
    for(i=0;i<anz;i++) {
      m[n*i]=1/sqrt(err[i]);
      for(j=1;j<n;j++) {
        m[n*i+j]=m[n*i+j-1]*x[i];
      }
      v[i]=y[i]*m[n*i];
    }
  } else {
    for(i=0;i<anz;i++) {
      m[n*i]=1;
      for(j=1;j<n;j++) {
        m[n*i+j]=m[n*i+j-1]*x[i];
      }
      v[i]=y[i];
    }
  }
  solve(v,m,anz,n,ergeb);
}

double mean(const double *a,int n) {
  double b=0;
  int i;
  for(i=0;i<n;i++) b+=a[i];
  return(b/(double)n);
}
double wmean(const double *a,const double *w,int n) { /*gewichteter Mittelwert*/
  double b=0,ww=0;
  int i;
  for(i=0;i<n;i++) {
    b+=a[i]*w[i];
    ww+=w[i];
  }
  return(b/ww);
}

/* Einfache lineare regression 
   R"uckgabe ist m,b aus y=m*x+b*/
static void linreg(const double *x, const double *y, int n,
            double *b, double *m, double *db, double *dm, double *dy) {
  int i;
  double sx=0,sy=0,sxx=0,sxy=0;
  for(i=0;i<n;i++) {
    sx+=x[i];
    sy+=y[i];
    sxx+=x[i]*x[i];
    sxy+=x[i]*y[i];
  } 
  double delta=n*sxx-sx*sx;
  *m=(n*sxy-sx*sy)/delta;
  *b=(sxx*sy-sx*sxy)/delta;
  double dydy=0;
  for(i=0;i<n;i++) {
    dydy+=(y[i]-*b-*m*x[i])*(y[i]-*b-*m*x[i]);
  }
  if(dm) *dm=sqrt(n/delta);  /* mal Fehler der Einzelwerte dy*/
  if(db) *db=sqrt(sxx/delta); /* mal Fehler der Einzelwerte dy*/
  
  if(n>2) *dy=sqrt(dydy/(n-2)); /*Streuung der Werte um Gerade*/
  else *dy=0;
  printf("linreg\n");
}


/*gewichtete lineare Regression
  R"uckgabe ist m,b aus y=m*x+b*/

static void linregw(const double *x, const double *y, const double *ddy, int n,
            double *b, double *m, double *db, double *dm, 
	    double *chi2) {
  int i;
  double sw=0,swxx=0,swx=0,swy=0,swxy=0,ee;
  for(i=0;i<n;i++) {
    ee=1/ddy[i]/ddy[i];
    sw+=ee;
    swx+=x[i]*ee;
    swy+=y[i]*ee;
    swxx+=x[i]*x[i]*ee;
    swxy+=x[i]*y[i]*ee;
    
  }
  
  double delta=sw*swxx-swx*swx;
  *m=(sw*swxy-swx*swy)/delta;
  *b=(swxx*swy-swx*swxy)/delta;
  if(chi2) {
    double dydy=0;  /* Das ist sowas wie chi2*/
    for(i=0;i<n;i++) dydy+=(y[i]-*b-*m*x[i])*(y[i]-*b-*m*x[i])/ddy[i]/ddy[i];
    *chi2=dydy;
  }
  if(dm) *dm=sqrt(sw/delta);  /* mal Fehler der Einzelwerte dy*/
  if(db) *db=sqrt(swxx/delta); /* mal Fehler der Einzelwerte dy*/
  /*Man koennte noch einen q-Wert bestimmen.... das ist etwa die gewichtete
  Streuung der EInzelwerte um die Gerade */
  // if(n>2) *q=gammq(0.5*(n-2),0.5*(*chi2));  
  /*Unvollstaendige Gammafunktion fehlt hier, dehalb geht es nicht*/

}


#ifdef HAVE_LAPACK
 int dgelss_(int *m, int *n, int *nrhs, 
	double *a, int *lda, double *b, int *ldb, double *
	s, double *rcond, int *rank, double *work, int *lwork,
	 int *info); 


   // LU decomoposition of a general matrix
    void dgetrf_(int* M, int *N, double* A, int* lda, int* IPIV, int* INFO);

    // generate inverse of a matrix given its LU decomposition
    void dgetri_(int* N, double* A, int* lda, int* IPIV, double* WORK, int* lwork, int* INFO);
#endif

/* Berechnet die Inverse zu einer Matrix */

void inverse(double*a, int n) {
#ifdef HAVE_LAPACK
  int lwork = n*n;
  int info,i,j;
  double work[lwork];
  int ipiv[n+1];
  /*Muss umgespeichert werden....*/
  double *m=malloc(sizeof(double)*n*n);

  for(i=0;i<n;i++) { for(j=0;j<n;j++) m[j*n+i]=a[j+i*n]; }
  dgetrf_(&n,&n,m,&n,ipiv,&info);
  dgetri_(&n,m,&n,ipiv,work,&lwork,&info);
  for(i=0;i<n;i++) { for(j=0;j<n;j++) a[j+i*n]=m[j*n+i]; }
  free(m);

  if(info) {
    puts("ERROR: the solver has not been successful.");
    if(info<0) printf("the %d-th argument had an illegal value.\n",-info);
    else printf("the algorithm for computing the %s failed to converge;\n"  
                "%d off-diagonal elements of an intermediate "
		" bidiagonal form did not converge to zero.\n","inverse",info);
  }
  
#else
  printf("The %s function is not implemented \n"
  " in this version of X11-Basic because the LAPACK library \n"
  " was not present at compile time.\n","INVERSE");
#endif

  
}
void linear_fit(double *x, double *y, int anz, double *err, int flag, 
     double *a, double *b, double *aerr, double *berr, double *chi2, double *q) {
  *q=1;  /*kann nicht berechent werden, da gammq fehlt*/
  
  if(flag) linregw(x,y,err,anz,a,b,berr,aerr,chi2);
  else linreg(x,y,anz,a,b,berr,aerr,chi2);
}
void linear_fit_exy(double *x, double *y, int anz, double sigx[], double sigy[],
  double *a, double *b, double *siga, double *sigb, double *chi2, double *q) {
  puts("ERROR: Procedure linear_fit_exy is not (yet) supported.");
}


/* Solves the Gleichungssystem Ax=b with Sigulaerwertzerlegung */


void solve(const double *v1,const double *m1,int anzzeilen, int anzspalten, double *ergeb) {
 int i,j;


#ifdef HAVE_LAPACK
  extern int verbose;
  int nrhs=1,rank,info;
  int ldb=max(anzzeilen,anzspalten);
  int lwork=3*min(anzspalten,anzzeilen) + 
       max(2*min(anzspalten,anzzeilen), ldb)+nrhs+10*anzzeilen;
  double rcond=1e-10;
  double work[lwork];
  double singulars[anzspalten];
  double *zwischenergeb=malloc(ldb*sizeof(double));
  memcpy(zwischenergeb,v1,anzzeilen*sizeof(double));
  double *m=malloc(sizeof(double)*anzspalten*anzzeilen);

  /*Muss umgespeichert werden....*/

  for(i=0;i<anzzeilen;i++) {
    for(j=0;j<anzspalten;j++) {
      m[j*anzzeilen+i]=m1[j+i*anzspalten];
    }
  }
  #if DEBUG
  for(i=0;i<anzzeilen;i++) {
    printf("%d: (",i);
    for(j=0;j<anzspalten;j++) {
      printf("%g",m[j*anzzeilen+i]);
      if(j<anzspalten-1) printf(", ");
    }
    printf(")(x)=(%g)\n",v1[i]);
  }
  #endif

  dgelss_(&anzzeilen,&anzspalten,&nrhs,m,&anzzeilen,zwischenergeb,&ldb,singulars,
     &rcond,&rank,work,&lwork,&info);
  memcpy(ergeb,zwischenergeb,anzspalten*sizeof(double));
  free(zwischenergeb);
  free(m);
  if(info) {
    puts("ERROR: the solver has not been successful.");
    if(info<0) printf("the %d-th argument had an illegal value.\n",-info);
    else printf("the algorithm for computing the %s failed to converge;\n"  
                "%d off-diagonal elements of an intermediate "
                " bidiagonal form did not converge to zero.\n","SVD",info);
    return;
  }

  if(verbose>0) printf("INFO: accuracy: %g%%\n",(double)rank/anzzeilen*100);
  if(verbose>1) printf("lwork=%d optimal: %g\n",lwork,work[0]);


  if(verbose>1) {
    int i;
    printf("First 20 singular values:\n");
    for(i=0;i<min(20,anzspalten);i++) printf("%d: %g\n",i,singulars[i]);
  }
  
  if((double)rank<(double)anzspalten/6) puts("WARNING: The solution will not be very accurate!");
#else
  printf("The %s function is not implemented \n"
  " in this version of X11-Basic because the LAPACK library \n"
  " was not present at compile time.\n","SOLVE");
#endif
#if DEBUG
{ int i;
  for(i=0;i<anzspalten;i++) printf("%d-> %g\n",i,ergeb[i]);
}
#endif
}
