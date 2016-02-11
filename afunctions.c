/* afunctions.c   Array functions (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "defs.h"
#include "globals.h"
#include "protos.h"
#include "vtypes.h"
#include "array.h"
#include "afunctions.h"

ARRAY f_smula(PARAMETER *plist, int e) {
  ARRAY ergeb;
  ergeb.typ=plist[0].typ;
  ergeb.dimension=plist[0].integer;
  ergeb.pointer=plist[0].pointer;
  ergeb=double_array(ergeb);
  array_smul(ergeb,plist[1].real);
  return(ergeb);
}
ARRAY f_nullmat(PARAMETER *plist, int e) {
    int dimlist[2]={plist[0].integer,plist[1].integer};
    return(nullmatrix(FLOATARRAYTYP,e,dimlist));
}
ARRAY f_einsmat(PARAMETER *plist, int e) {
    int dimlist[2]={plist[0].integer,plist[0].integer};
    return(einheitsmatrix(FLOATARRAYTYP,2,dimlist));
}

extern double *SVD(double *a, double *w, double *v,int anzzeilen, int anzspalten);
extern double *backsub(double *, double *, double *, double *,int,int);

double *makeSVD(double *v1,double *m1,int anzzeilen, int anzspalten) {
  int i,j;
  double maxsing=0;
  int elim=0;
  int fsing=0;
  double *ergebnis;

  double *u = malloc(sizeof(double)*anzzeilen*anzspalten);
  double *v = malloc(sizeof(double)*anzspalten*anzspalten);
  double *singulars = malloc(sizeof(double)*anzspalten);

  memcpy(u,m1,sizeof(double)*anzzeilen*anzspalten);
  singulars=SVD(u,singulars,v,anzzeilen,anzspalten);

#ifdef DEBUG
  printf("Eigenwerte:\n");
  for(i=0;i<anzspalten;i++) printf("%g\n",singulars[i]);
#endif
/* Groessten Singulaerwert rausfinden */

  for(i=0;i<anzspalten;i++) {
    if(fabs(singulars[i])>maxsing) maxsing=fabs(singulars[i]);
  }

/* Zaehle Anzahl der Singulaeren Werte (d.h. Eigenwerte=0) */
/* Akzeptiere nur Eigenwerte die mindestens 1e-10 vom groessten sind,
   ansonsten setze sie zu 0 */

  for(i=0;i<anzspalten;i++) {
    if(singulars[i]==0) fsing++;
    if(fabs(singulars[i])/maxsing<1e-10 && singulars[i]) {
      printf("** %g\n",singulars[i]);
      singulars[i]=0;
      elim++;
    }
  }
  if(fsing || elim) printf("Found %d singularities and eliminated another %d.\n",fsing,elim);
  ergebnis=backsub(singulars,u,v,v1,anzzeilen,anzspalten);
  free(u);free(v);
  free(singulars);
  return(ergebnis);
}

makeSVD2(double *v1,double *m1,int anzzeilen, int anzspalten, double *ergeb) {
  double *x;
  x=makeSVD(v1,m1,anzzeilen,anzspalten);
  memcpy(ergeb,x,sizeof(double)*anzspalten);
}

/* Gleichungssystem loesen  d=Mx    x()=SOLVE(m(),d())*/
ARRAY f_solvea(PARAMETER *plist, int e) {
  ARRAY ergeb;
  int anzzeilen,anzspalten;
  ergeb.typ=plist[0].typ;
  ergeb.dimension=1;
  if(plist[0].integer!=2) xberror(81,""); /* "Matrizen haben nicht die gleiche Ordnung" */
  if(plist[1].integer!=1) xberror(81,""); /* "Matrizen haben nicht die gleiche Ordnung" */
  anzspalten=*((int *)(plist[0].pointer+sizeof(int)));
  anzzeilen=*((int *)(plist[0].pointer));

  if(anzzeilen!=*((int *)(plist[1].pointer))) xberror(81,""); /* "Matrizen haben nicht die gleiche Ordnung" */

  ergeb.pointer=malloc(INTSIZE+anzspalten*sizeof(double));
  *((int *)ergeb.pointer)=anzspalten;
  makeSVD2((double *)(plist[1].pointer+plist[1].integer*INTSIZE),(double *)(plist[0].pointer+plist[0].integer*INTSIZE),anzzeilen,anzspalten, (double *)(ergeb.pointer+INTSIZE));
  return(ergeb);
}
#ifdef CONTROL
ARRAY f_csvgeta(char *pos) {
  int o=0,nn=0;
  if(e>1) nn=plist[1].integer;
  if(e>2) o=plist[2].integer;
  return(csvget(plist[0].pointer,nn,o));
}
#endif
#ifdef TINE
ARRAY f_tinegeta(PARAMETER *plist, int e) {
  int o=0,nn=0;
  if(e>1) nn=plist[1].integer;
  if(e>2) o=plist[2].integer;
  return(tinevget(plist[0].pointer,nn,o));
}
ARRAY f_tinehistorya(PARAMETER *plist, int e) {
	  return(tinehistory(plist[0].pointer,plist[1].integer,plist[2].integer));
}
#endif
#ifdef DOOCS
ARRAY doocsnames(char *n);
ARRAY f_doocsgeta(PARAMETER *plist, int e) {
  int o=0,nn=0;
  if(e>1) nn=plist[1].integer;
  if(e>2) o=plist[2].integer;
  return(doocsvget(plist[0].pointer,nn,o));
}
ARRAY f_doocsnames(PARAMETER *plist, int e) {
  return(doocsnames(plist[0].pointer));
}
#endif
