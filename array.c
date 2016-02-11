/* array.c (c) Markus Hoffmann             */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "parameter.h"
#include "xbasic.h"
#include "wort_sep.h"
#include "array.h"
#include "parser.h"
#include "mathematics.h"

static ARRAY form_array(int, int, int *, char *);







/******************** Array variable routines **************************/

void fill_int_array(ARRAY *arr, int inh) {
  int anz=anz_eintraege(arr); 
  int *pp=(int *)(arr->pointer+arr->dimension*INTSIZE); 
  while(--anz>=0) pp[anz]=inh;
}
void fill_float_array(ARRAY *arr, double inh) {
  int anz=anz_eintraege(arr);
  double *pp=(double *)(arr->pointer+arr->dimension*INTSIZE);
  while(--anz>=0) pp[anz]=inh;
}
void fill_string_array(ARRAY *arr, STRING inh) {
  int anz=anz_eintraege(arr),j; 
  STRING *varptr;
  varptr=(STRING *)(arr->pointer+arr->dimension*INTSIZE); 
  for(j=0;j<anz;j++) {
    varptr[j].pointer=realloc(varptr[j].pointer,inh.len+1);
    memcpy(varptr[j].pointer,inh.pointer,inh.len);
    varptr[j].len=inh.len; 
  }	
}




/********************* ARRAY STructure Routines ************************/
ARRAY create_int_array(int dimension, int *dimlist,int value) {
  ARRAY ergebnis;
  int anz=1,j;
  int *varptr;
  ergebnis.typ=INTTYP;
  ergebnis.dimension=dimension;
  if(dimension>0) {for(j=0;j<dimension;j++) anz=anz*((int *)dimlist)[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*sizeof(int));
  varptr=(int *)(((char *)ergebnis.pointer)+dimension*INTSIZE);
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(int));
  while(--anz>=0) varptr[anz]=value;    
  return(ergebnis);
}
ARRAY create_float_array(int dimension, int *dimlist,double value) {
  ARRAY ergebnis;
  int anz=1,j;
  double *varptr;
  ergebnis.typ=FLOATTYP;
  ergebnis.dimension=dimension;
  if(dimension>0) {for(j=0;j<dimension;j++) anz=anz*((int *)dimlist)[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*sizeof(double));
  varptr=(double *)(((char *)ergebnis.pointer)+dimension*INTSIZE);
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(int));
  while(--anz>=0) varptr[anz]=value;    
  return(ergebnis);
}
ARRAY create_string_array(int dimension, int *dimlist,STRING *value) {
  ARRAY ergebnis;
  int anz=1,j;
  STRING *varptr;
  ergebnis.typ=STRINGTYP;
  ergebnis.dimension=dimension;
  if(dimension>0) {for(j=0;j<dimension;j++) anz=anz*((int *)dimlist)[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*sizeof(STRING));
  varptr=(STRING *)(((char *)ergebnis.pointer)+dimension*INTSIZE);
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(int));
  while(--anz>=0) varptr[anz]=double_string(value);    
  return(ergebnis);
}

ARRAY create_array(int typ, int dimension, int *dimlist) {
  ARRAY ergebnis;
  int anz=1,j;
  ergebnis.typ=typ;
  ergebnis.dimension=dimension;
  if(dimension>0) {for(j=0;j<dimension;j++) anz=anz*((int *)dimlist)[j];}
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*typlaenge(typ));
  
  if(typ==STRINGTYP) {
    STRING *varptr=(STRING *)(ergebnis.pointer+dimension*INTSIZE);
    const STRING a={0,NULL};
    while(--anz>=0) varptr[anz]=a;
  }
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(int));
  return(ergebnis);
}
void free_array(ARRAY *arr) {
 // printf("free_array: %d\n",arr->typ);
  if(arr->typ==NOTYP) return;
  if(arr->typ==STRINGTYP) {
    int j,anz=1;
    STRING *varptr;
    /* Anzahl der Eintraege aus Dimension bestimmen */
    for(j=0;j<arr->dimension;j++) anz=anz*((int *)arr->pointer)[j];
    varptr=(STRING *)(arr->pointer+arr->dimension*INTSIZE); 
    for(j=0;j<anz;j++)  free(varptr[j].pointer); 	  
  } else if(arr->typ==ARRAYTYP) {  /*ARRAY of ARRAYs ....*/
    int j,anz=1;
    ARRAY *varptr;
    /* Anzahl der Eintraege aus Dimension bestimmen */
    for(j=0;j<arr->dimension;j++) anz=anz*((int *)arr->pointer)[j];
    varptr=(ARRAY *)(arr->pointer+arr->dimension*INTSIZE); 
    for(j=0;j<anz;j++)  free_array(&varptr[j]); 	  
  }
  free(arr->pointer);
  arr->pointer=NULL;
  arr->typ=NOTYP;
} 

ARRAY convert_to_floatarray(ARRAY *a) {
  ARRAY ergeb=create_array(FLOATTYP,a->dimension,a->pointer);
  double *varptr=(double *)(ergeb.pointer+ergeb.dimension*INTSIZE); 
  int *pp2=(int *)(a->pointer+a->dimension*INTSIZE); 
  int j,anz=1;
  for(j=0;j<a->dimension;j++) anz=anz*(((int *)a->pointer)[j]);
  for(j=0;j<anz;j++) varptr[j]=(double)pp2[j];
  return(ergeb);
}
ARRAY convert_to_intarray(ARRAY *a) {
  ARRAY ergeb=create_array(INTTYP,a->dimension,a->pointer);
  int *varptr=(int *)(ergeb.pointer+ergeb.dimension*INTSIZE); 
  double *pp2=(double *)(a->pointer+a->dimension*INTSIZE); 
  int j,anz=1;
  for(j=0;j<a->dimension;j++) anz=anz*(((int *)a->pointer)[j]);
  for(j=0;j<anz;j++) varptr[j]=(int)pp2[j];
  return(ergeb);
}
/* Kopiert ein Array  */
ARRAY double_array(ARRAY *a) {
  int anz=anz_eintraege(a);
  ARRAY b=*a;
  int size=b.dimension*INTSIZE+anz*typlaenge(b.typ);
  b.pointer=malloc(size);
  memcpy(b.pointer,a->pointer,size);
  if(b.typ==STRINGTYP) {
    STRING *ppp1=(STRING *)(b.pointer+b.dimension*INTSIZE);
    STRING *ppp2=(STRING *)(a->pointer+b.dimension*INTSIZE);

    while(--anz>0) {
      ppp1[anz].pointer=malloc(ppp2[anz].len+1);
      memcpy(ppp1[anz].pointer,ppp2[anz].pointer,ppp2[anz].len);
      (ppp1[anz].pointer)[ppp2[anz].len]=0;
    }
  }
  return(b);
}
ARRAY einheitsmatrix(int typ, int dimension, int *dimlist) {
  ARRAY ergebnis=nullmatrix(typ,dimension,dimlist);
 // int anz=anz_eintraege(ergebnis);
  int i,a=0,j;
  int q=dimlist[0];
  for(j=0;j<dimension;j++) {if(q>dimlist[j]) q=dimlist[j];}
  
  if(typ & INTTYP) {
    int *varptr=(int *)(ergebnis.pointer+dimension*INTSIZE);
    for(j=0;j<q;j++) {
      for(i=0;i<dimension;i++) a=j+a*dimlist[i]; 
      varptr[a]=1;
    }
  } else if(typ & FLOATTYP) {
    double *varptr=(double *)(ergebnis.pointer+dimension*INTSIZE);
    /* Jetzt die Diagonalelemente */
    for(j=0;j<q;j++) {
      a=j;
      for(i=0;i<dimension-1;i++) a+=j*dimlist[i]; 
      varptr[a]=1.0;
    }
  } else puts("Einheitsmatrix nicht definiert !"); 
  return(ergebnis);
}
ARRAY nullmatrix(int typ, int dimension, int *dimlist) {
  ARRAY ergebnis;
  
  if(typ & INTTYP) {
    ergebnis=create_int_array(dimension,dimlist,0);
  } else if(typ & FLOATTYP) {
    ergebnis=create_float_array(dimension,dimlist,0);
  } else {
    ergebnis=create_array(typ,dimension,dimlist);
  }   
  return(ergebnis);
}
/* Uebernimmt einen Speicherbereich in ein Array */

static ARRAY form_array(int typ, int dimension, int *dimlist, char *inhalt) {
  int j,anz=1,dlen;
  char *pp;
  ARRAY ergebnis;
  ergebnis.typ=typ;
  ergebnis.dimension=dimension;
  
  if(dimension) {
    for(j=0;j<dimension;j++) anz=anz*dimlist[j];
  }
  dlen=typlaenge(typ);
  
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*dlen);
  pp=(char *)(ergebnis.pointer+dimension*INTSIZE);
  /* dimlist kopieren */
  memcpy(ergebnis.pointer,dimlist,dimension*sizeof(int));
  memcpy(pp,inhalt,dlen*anz);
  return(ergebnis);
}



/* Mache ein ARRAY aus Konstante [1,2,3;4,5,6]  */

ARRAY array_const(char *s) {
  ARRAY ergebnis;
  char t[strlen(s)+1],t2[strlen(s)+1],s2[strlen(s)+1];
  int e,i=0,j=0,f,dx=0,dy,anz=1;
  /* Ein oder Zweidimensional ? */
  ergebnis.dimension=wort_sep(s,';',TRUE,t,t2);
  /* Typ Bestimmen */
  wort_sep(t,' ',TRUE,t,t2);
  ergebnis.typ=type(t)&(~CONSTTYP)&(~ARRAYTYP);
  e=wort_sep(s,';',TRUE,t,s2);
  while(e) {
    f=wort_sep(t,',',TRUE,t2,t);
    while(f) {
      j++;
      f=wort_sep(t,',',TRUE,t2,t);
    }
    dx=max(dx,j);
    j=0;
    i++;
    e=wort_sep(s2,';',TRUE,t,s2);
  }
  dy=i;
  anz=dx*dy;
  /*
  printf("ARRAY-Const:  %s\n",s);
  printf("============\n");
  printf("Dimension: %d\n",ergebnis->dimension);
  printf("Typ:       %d\n",ergebnis->typ);
  printf("Dim: %dx%d  \n",dy,dx);
  printf("Anz: %d  \n",anz);
*/
  
  ergebnis.pointer=malloc(ergebnis.dimension*INTSIZE+anz*typlaenge(ergebnis.typ));
  if(ergebnis.typ & STRINGTYP) {
    int i;
    STRING *varptr=(STRING *)(ergebnis.pointer+ergebnis.dimension*INTSIZE);
    for(i=0;i<anz;i++) {
      varptr[i].len=0;   /* Laenge */
      varptr[i].pointer=NULL;
    }
  }
  if(ergebnis.dimension==1) *((int *)(ergebnis.pointer))=dx;
  else if(ergebnis.dimension==2) {
    ((int *)(ergebnis.pointer))[0]=dy;
    ((int *)(ergebnis.pointer))[1]=dx;
  }
  i=j=0;
    e=wort_sep(s,';',TRUE,t,s2);
  while(e) {
    f=wort_sep(t,',',TRUE,t2,t);
    while(f) {
      if(ergebnis.typ & INTTYP) ((int *)(ergebnis.pointer+ergebnis.dimension*INTSIZE))[j]=(int)parser(t2);
      else if(ergebnis.typ & FLOATTYP) ((double *)(ergebnis.pointer+ergebnis.dimension*INTSIZE))[j]=parser(t2);
      else {
        STRING *varptr;
	STRING sss;
        varptr=(STRING *)(ergebnis.pointer+ergebnis.dimension*INTSIZE);
	sss=string_parser(t2);
          varptr[j].len=sss.len;
	  free(varptr[j].pointer);
          varptr[j].pointer=sss.pointer;
      }
      j++;
      f=wort_sep(t,',',TRUE,t2,t);
    }
    i++;
    j=i*dx;
    e=wort_sep(s2,';',TRUE,t,s2);
  }
  return(ergebnis);
}
ARRAY mul_array(ARRAY a1, ARRAY a2) {
  /* Teste ob die Typen fuer eine Multiplikation infrage kommen*/
  if(a1.dimension>2 || a2.dimension>2 || (a1.typ & STRINGTYP) || (a2.typ & STRINGTYP)) {
    xberror(83,""); /*Matrizenprodukt nicht definiert*/
    return(nullmatrix(a1.typ,a1.dimension,a1.pointer));
  } else {
    /* Teste, ob die Dimensionierung fuer eine Multiplikation taugt: */
    int s1,z1,s2,z2;
    void *ar1,*ar2,*ar3;
    if(a1.dimension==0) s1=z1=1;
    else if(a1.dimension==1) {z1=1;s1=((int *)a1.pointer)[0];}
    else {z1=((int *)a1.pointer)[0];s1=((int *)a1.pointer)[1];}
    if(a2.dimension==0) s2=z2=1;
    else if(a2.dimension==1) {z2=1;s2=((int *)a2.pointer)[0];}
    else {z2=((int *)a2.pointer)[0];s2=((int *)a2.pointer)[1];}   
    if(s1!=z2) {
      xberror(81,""); /*Matrizen haben nicht die gleiche Ordnung*/
      return(nullmatrix(a1.typ,a1.dimension,a1.pointer));
    } else {
      ARRAY ergebnis;
      int s3,i,j,k;
      /*Jetzt typen anpassen:*/
      if(a1.typ!=a2.typ) {  /*d.h. einer ist float*/
        if(a1.typ & INTTYP) { /*covert to float*/
           ergebnis=convert_to_floatarray(&a1);
	   free_array(&a1);
	   a1=ergebnis;
	}
        if(a2.typ & INTTYP) { /*covert to float*/
           ergebnis=convert_to_floatarray(&a2);
	   free_array(&a2);
	   a2=ergebnis;
	}
      }
      /*Pointer auf daten bestimmen*/
      ar1=a1.pointer+a1.dimension*INTSIZE;
      ar2=a2.pointer+a2.dimension*INTSIZE;
      /*Dimension des Ergebnisses bestimmen, pointer auf daten des ergebnisses*/
      s3=s1;
      if(s1<=1) ergebnis.dimension=0;
      else ergebnis.dimension=2;
      ergebnis.typ=a1.typ;
      if(a1.typ & FLOATTYP)  { /*floatmultiplikation*/
	ergebnis.pointer=malloc(ergebnis.dimension*INTSIZE+s3*s3*sizeof(double));
        ar3=ergebnis.pointer+ergebnis.dimension*INTSIZE;
        for(i=0;i<ergebnis.dimension;i++) ((int *)ergebnis.pointer)[i]=s3;
        for(i=0;i<s3;i++) {
	  for(j=0;j<s3;j++) {
	    ((double *)ar3)[i*s3+j]=0;
	    for(k=0;k<min(s2,z1);k++) {
	      ((double *)ar3)[i*s3+j]+=((double *)ar1)[k*s1+j]*((double *)ar2)[i*s2+k];
	    }
	  }
	}
      } else {                   /*intmultiplikation*/
	ergebnis.pointer=malloc(ergebnis.dimension*INTSIZE+s3*s3*sizeof(int));
        ar3=ergebnis.pointer+ergebnis.dimension*INTSIZE;
        for(i=0;i<ergebnis.dimension;i++) ((int *)ergebnis.pointer)[i]=s3;
        for(i=0;i<s3;i++) {
	  for(j=0;j<s3;j++) {
  	    ((int *)ar3)[i*s3+j]=0;
	    for(k=0;k<min(s2,z1);k++) ((int *)ar3)[i*s3+j]+=((double *)ar1)[k*s1+j]*((double *)ar2)[i*s2+k];
	  }
	}
      }
      return(ergebnis);
    }
  }
}

/* Transponiere ein 2-d-Array    */

ARRAY trans_array(ARRAY a) {
  ARRAY b=double_array(&a);
  if(a.dimension==1) {
    int anz=1,size,j;
    /* Erweitere die Dimension. */
    for(j=0;j<a.dimension;j++) anz=anz*((int *)a.pointer)[j];
    if(a.typ & INTTYP) size=sizeof(int);
    else if(a.typ & FLOATTYP) size=sizeof(double);
    else size=sizeof(STRING);
    b.dimension++;
    b.pointer=realloc(b.pointer,b.dimension*INTSIZE+anz*size);
    ((int *)b.pointer)[0]=((int *)a.pointer)[0];
    ((int *)b.pointer)[1]=1;
    /* mems may overlap */
    memmove(b.pointer+b.dimension*INTSIZE,a.pointer+a.dimension*INTSIZE,anz*size);
  } else if(a.dimension==2) {
    int size,i,j;
    int sw=((int *)b.pointer)[1];
    ((int *)b.pointer)[1]=((int *)b.pointer)[0];
    ((int *)b.pointer)[0]=sw;
    if(a.typ & INTTYP) size=sizeof(int);
    else if(a.typ & FLOATTYP) size=sizeof(double);
    else size=sizeof(STRING);
    for(i=0;i<((int *)b.pointer)[0];i++) {
      for(j=0;j<((int *)b.pointer)[1];j++) {
         memcpy(b.pointer+b.dimension*INTSIZE+(i*((int *)a.pointer)[0]+j)*size,
	        a.pointer+a.dimension*INTSIZE+(j*((int *)b.pointer)[0]+i)*size,size);
      }
    }
  } else 
    xberror(85,"TRANS"); /* Transposition nur für zweidimensionale Matrizen.*/  
  return(b);
}
ARRAY inv_array(ARRAY a) {
  if(a.dimension==0) {
    double d=*((double *)a.pointer);
    if(d==0) xberror(0,""); /* Division durch Null */
    else d=1/d;
    return(double_array(&a));
  } else if(a.dimension!=2) {
    xberror(89,""); /* Array must be two dimensional */
    return(nullmatrix(a.typ,a.dimension,a.pointer));
  } else {
    if(((int *)a.pointer)[0]!=((int *)a.pointer)[1]) {
      xberror(86,""); /* Matrix nicht quadratisch */
      return(nullmatrix(a.typ,a.dimension,a.pointer));
    } else {
      ARRAY ergeb;
      double maxsing=0;
      int i,j,elim=0;
      int fsing=0;
      int anzzeilen=((int *)a.pointer)[0];
      int anzspalten=((int *)a.pointer)[1];
      double *pp=(double *)(a.pointer+a.dimension*INTSIZE);
      double *u = malloc(sizeof(double)*anzzeilen*anzspalten);
      double *v = malloc(sizeof(double)*anzspalten*anzspalten);
      double *ergebnis = malloc(sizeof(double)*anzspalten*anzspalten);
      double *singulars = malloc(sizeof(double)*anzspalten);
      double *col;
      double *univ = malloc(sizeof(double)*anzspalten);
      memcpy(u,pp,sizeof(double)*anzzeilen*anzspalten);
      singulars=SVD(u,singulars,v,anzzeilen,anzspalten);
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

      /* Jetzt die inverse Matrix ausrechnen   */
      for(i=0;i<anzspalten;i++) {
        for(j=0;j<anzspalten;j++) univ[j]=0;
        univ[i]=1;
        col=backsub(singulars,u,v,univ,anzzeilen,anzspalten);
	for(j=0;j<anzspalten;j++) ergebnis[i*anzspalten+j]=col[j];
        free(col);
      }
      free(u);free(v);
      free(singulars);
      free(univ);
      ergeb=form_array(a.typ,2,a.pointer,(char *)ergebnis);
      free(ergebnis);
      return(ergeb);
    }
  }
}
void array_smul(ARRAY a1, double m) {
  int anz=anz_eintraege(&a1),j;
  if(a1.typ & FLOATTYP)  {
    double *pp1=(double *)(a1.pointer+a1.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]=m*pp1[j];
  } else if(a1.typ & INTTYP) {
    int *pp1=(int *)(a1.pointer+a1.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]=m*pp1[j];
  } else puts("ERROR: inkompatible array type.");
}
void array_add(ARRAY a1, ARRAY a2) {
  int anz=min(anz_eintraege(&a1),anz_eintraege(&a2)),j;
  
  if((a1.typ & FLOATTYP) && (a2.typ & FLOATTYP)) {
    double *pp1=(double *)(a1.pointer+a1.dimension*INTSIZE); 
    double *pp2=(double *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]+=pp2[j];
  } else if((a1.typ & FLOATTYP) && (a2.typ & INTTYP)) {
    double *pp1=(double *)(a1.pointer+a1.dimension*INTSIZE); 
    int *pp2=(int *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]+=(double)pp2[j];
  } else if((a1.typ & INTTYP) && (a2.typ & INTTYP)) {
    int *pp1=(int *)(a1.pointer+a1.dimension*INTSIZE); 
    int *pp2=(int *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]+=pp2[j];
  } else if((a1.typ & INTTYP) && (a2.typ & FLOATTYP)) {
    int *pp1=(int *)(a1.pointer+a1.dimension*INTSIZE); 
    double *pp2=(double *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]+=(int)pp2[j];
  } else if((a1.typ & STRINGTYP) && (a2.typ & STRINGTYP)) {
    STRING *pp1=(STRING *)(a1.pointer+a1.dimension*INTSIZE); 
    STRING *pp2=(STRING *)(a2.pointer+a2.dimension*INTSIZE); 
    
    for(j=0;j<anz;j++) {
      pp1[j].pointer=realloc(pp1[j].pointer,pp2[j].len+pp1[j].len);
      memcpy(pp1[j].pointer+pp1[j].len,pp2[j].pointer,pp2[j].len);
      pp1[j].len+=pp2[j].len; 
    }	
  } else puts("ERROR: inkompatible array type.");
}

void array_sub(ARRAY a1, ARRAY a2) {
  int anz=min(anz_eintraege(&a1),anz_eintraege(&a2)),j;
  
  if((a1.typ & FLOATTYP) && (a2.typ & FLOATTYP)) {
    double *pp1=(double *)(a1.pointer+a1.dimension*INTSIZE); 
    double *pp2=(double *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]-=pp2[j];
  } else if((a1.typ & FLOATTYP) && (a2.typ & INTTYP)) {
    double *pp1=(double *)(a1.pointer+a1.dimension*INTSIZE); 
    int *pp2=(int *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]-=(double)pp2[j];
  } else if((a1.typ & INTTYP) && (a2.typ & INTTYP)) {
    int *pp1=(int *)(a1.pointer+a1.dimension*INTSIZE); 
    int *pp2=(int *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]-=pp2[j];
  } else if((a1.typ & INTTYP) && (a2.typ & FLOATTYP)) {
    int *pp1=(int *)(a1.pointer+a1.dimension*INTSIZE); 
    double *pp2=(double *)(a2.pointer+a2.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]-=(int)pp2[j];
  } else puts("ERROR: inkompatible array type.");
}




/* Wandelt ein ganzes Array in einen String um, so dass es auch wieder
   rueckverwandelt werden kann. Nuetzlich zum Speichern und versenden  */


STRING array_to_string(ARRAY inhalt) {
  STRING ergebnis;
  char *ppp;
  int len,arraylen;
  len=sizeof(int)+sizeof(int);
  len+=INTSIZE*inhalt.dimension;
  arraylen=anz_eintraege(&inhalt);
  len+=arraylen*typlaenge(inhalt.typ);
  if(inhalt.typ & ARRAYTYP) printf("Array beinhaltet wieder ARRAYs.\n");
  else if(inhalt.typ & STRINGTYP) {
    /* Alle Stringlaengen aufsummieren */
    STRING *a=(STRING *)(inhalt.pointer+INTSIZE*inhalt.dimension);
    int i;
    for(i=0;i<arraylen;i++) {
     /* if(memmem(ppp2,(int)(ppp-ppp2),a[i].pointer,a[i].len)==NULL)*/
        len+=a[i].len;
    }
  }
  ergebnis.pointer=malloc(len);
  ergebnis.len=len;
  
  ((int *)ergebnis.pointer)[0]=inhalt.typ;
  ((int *)ergebnis.pointer)[1]=inhalt.dimension;
  ppp=(char*)ergebnis.pointer+2*sizeof(int);
  memcpy(ppp,inhalt.pointer,INTSIZE*inhalt.dimension);
  ppp+=INTSIZE*inhalt.dimension;
  memcpy(ppp,inhalt.pointer+INTSIZE*inhalt.dimension,
         arraylen*typlaenge(inhalt.typ));
  
  if(inhalt.typ & STRINGTYP) {
    /* Alle Stringpointer relozieren */
    STRING *a=(STRING *)(ppp);
    int i;
    char *ppp2,*found;
    ppp+=arraylen*typlaenge(inhalt.typ);
    ppp2=ppp;
    for(i=0;i<arraylen;i++) {
      /* Schau, ob der eintrag schon vorhanden ist: */
      found=(char *)memmem(ppp2,(int)(ppp-ppp2),a[i].pointer,a[i].len);
      if(found!=NULL) {
        a[i].pointer=(char *)(found-ppp2);
      } else {
        memcpy(ppp,a[i].pointer,a[i].len);
        a[i].pointer=(char *)(ppp-ppp2);
        ppp+=a[i].len;
      }
    }
    ergebnis.len=ppp-ergebnis.pointer; /* String kuerzen */
  }
  return(ergebnis);
}

ARRAY string_to_array(STRING in) {
  ARRAY out;
  int len,arraylen;
  
  out.typ=((int *)in.pointer)[0];
  out.dimension=((int *)in.pointer)[1];
  len=INTSIZE*out.dimension;
  out.pointer=in.pointer+2*sizeof(int);
  arraylen=anz_eintraege(&out);
  len+=arraylen*typlaenge(out.typ);
  
  out.pointer=malloc(len);
  
  memcpy(out.pointer,in.pointer+2*sizeof(int),INTSIZE*out.dimension);
  memcpy(out.pointer+INTSIZE*out.dimension,
         in.pointer+2*sizeof(int)+INTSIZE*out.dimension,
	 arraylen*typlaenge(out.typ));

  if(out.typ & STRINGTYP) {
    char *ppp=in.pointer+2*sizeof(int)+arraylen*typlaenge(out.typ);
    STRING *a=(STRING *)(out.pointer+INTSIZE*out.dimension);
    /* Alle Strings anlegen und pointer relozieren */
    unsigned int i;
    unsigned long offset;
    for(i=0;i<arraylen;i++) {
      offset=(unsigned long)a[i].pointer;
      a[i].pointer=malloc(a[i].len);
      memcpy(a[i].pointer,ppp+offset,a[i].len);
    }
  }
  return(out);
}


/********************* Hilfsfunktionen ********************************/

/* Get subarry (ARRAY *a,indexliste) zurueck: ARRAY b. */

ARRAY get_subarray(ARRAY *arr,int *indexliste) {
  ARRAY ergebnis=*arr;
  int *aindex=(int *)arr->pointer;
  int bindex[arr->dimension];
  int adim,dim,dim2=0,i,j,jj,k,anz=1,anz2;

  adim=arr->dimension;
  if(adim) {
  for(i=0;i<adim;i++) {
    if(indexliste[i]==-1) {
      bindex[dim2++]=aindex[i];
      anz=anz*aindex[i];
    }
  }
  }
  ergebnis.dimension=dim2;
  ergebnis.pointer=malloc(ergebnis.dimension*INTSIZE+anz*typlaenge(ergebnis.typ));
   /* dimlist kopieren */
  memcpy(ergebnis.pointer,bindex,dim2*sizeof(int));
 
  /*Loop fuer die Komprimierung */
  for(j=0;j<anz;j++) {
    jj=j;
    /* Indexliste aus anz machen */
    for(k=arr->dimension-1;k>=0;k--) {
      if(indexliste[k]==-1) {
        bindex[k]=jj/aindex[k];
	jj=jj % aindex[k];	
      } else bindex[k]=indexliste[k];
    }
    if(jj!=0) puts("Rechnung geht nicht auf."); /* Testen ob passt  */
    anz2=0;
    for(k=0;k<arr->dimension;k++)  anz2=anz2*aindex[k]+bindex[k];

 /* jetzt kopieren */

    if(ergebnis.typ==INTTYP) {
      ((int *)(ergebnis.pointer+INTSIZE*ergebnis.dimension))[j]=((int *)(arr->pointer+INTSIZE*arr->dimension))[anz2];   
    } else if(ergebnis.typ==FLOATTYP) {
      ((double *)(ergebnis.pointer+INTSIZE*ergebnis.dimension))[j]=((double *)(arr->pointer+INTSIZE*arr->dimension))[anz2];   
    } else if(ergebnis.typ==STRINGTYP) {
      ((STRING *)(ergebnis.pointer+INTSIZE*ergebnis.dimension))[j]=double_string(&((STRING *)(arr->pointer+INTSIZE*arr->dimension))[anz2]);
    } else printf("ERROR: subarray, wrong typ\n");
  }
  return(ergebnis);
}


int anz_eintraege(ARRAY *a) {/* liefert Anzahl der Elemente in einem ARRAY */
  int anz=1,j;
  for(j=0;j<a->dimension;j++) anz=anz*((int *)a->pointer)[j];
  return(anz);
}

int do_dimension(VARIABLE *v) {  /* liefert Anzahl der Elemente in einem ARRAY */
 /*  printf("DODIM?: vnr=%d \n",vnr); */
  if(v->typ & ARRAYTYP) {
    return(anz_eintraege(v->pointer.a));
  } else return(1);
}


void *arrayvarptr(int vnr, char *n,int size) {  
  if(vnr!=-1)  {
    int dim=variablen[vnr].pointer.a->dimension;
    int indexliste[dim];
    /* Index- Liste aufloesen  */
    make_indexliste(dim,n,indexliste);
    return(arrayvarptr2(vnr,indexliste,size));
  } else return(NULL);
}
void *arrayvarptr2(int vnr, int *indexliste,int size) {
  int a=0,i=0;
  if(vnr!=-1)  {
    int dim=variablen[vnr].pointer.a->dimension;
    /* Index- Liste aufloesen  */
    for(i=0;i<dim;i++) {
      if(indexliste[i]==-1) a=a*((int *)variablen[vnr].pointer.a->pointer)[i];
      else a=indexliste[i]+a*((int *)variablen[vnr].pointer.a->pointer)[i];
    }  
    return(variablen[vnr].pointer.a->pointer+dim*INTSIZE+a*size);
  } else return(NULL);
}


int make_indexliste(int dim, char *pos, int *index) {
  char w1[strlen(pos)+1],w2[strlen(pos)+1];
  int i=0,e,flag=0;
// printf("Make indexliste: dim=%d <%s>\n",dim,pos);
  e=wort_sep(pos,',',TRUE,w1,w2);
  for(i=0;i<dim;i++) {
    if(e==0) {
      xberror(18,"");  /* Falsche Anzahl Indizies */
      flag=1;
      break;
    }
    if(w1[0]==':' || w1[0]==0) {index[i]=-1;flag=1;}	
    else index[i]=(int)parser(w1);
    e=wort_sep(w2,',',TRUE,w1,w2);
  }
  return(flag);
}

void make_indexliste_plist(int dim, PARAMETER *p, int *index) {
  while(--dim>=0) {
      if(p[dim].typ==PL_INT) index[dim]=p[dim].integer;
      else if(p[dim].typ==PL_FLOAT) index[dim]=(int)p[dim].real;
      else printf("ERROR: Kein int!");    
  }
}
