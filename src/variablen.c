/* variablen.c (c) Markus Hoffmann             */

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
#include "xbasic.h"
#include "protos.h"


/* Variablen-Verwaltung   */


int vartype(char *name) {  /* Bestimmt Typ der Variablen */
  char w1[strlen(name)+1];
  char *pos;
  int typ=0;
  
  if(strlen(name)==0) return(NOTYP);
  strcpy(w1,name);
/*  printf("Vartype(%s): ",name);*/
  pos=searchchr(w1+1,'(');
  if(pos!=NULL) {
    if(pos[1]==')') typ=(typ|ARRAYTYP);
    else {   /* jetzt entscheiden, ob Array-element oder sub-array oder Funktion */
      char *ppp=pos+1;
      int i=0,flag=0,sflag=0,count=0;
      while(ppp[i]!=0 && !(ppp[i]==')' && flag==0 && sflag==0)) {
        if(ppp[i]=='(') flag++;
	if(ppp[i]==')') flag--;
	if(ppp[i]=='"') sflag=(!sflag);
	if(ppp[i]==':' && flag==0 && sflag==0) count++;
        i++;
      } 
      if(count) typ=(typ|ARRAYTYP);
    }
  } else pos=&w1[strlen(w1)];
  if(*(pos-1)=='$') typ=(typ|STRINGTYP);
  else if(*(pos-1)=='%') typ=(typ|INTTYP);
  else typ=(typ|FLOATTYP);
/*  printf("%d\n",typ);*/
  return(typ);
}

char *varrumpf(char *n) {  /* Rumpf des Variablennamens */
  char *ergebnis;
  char *pos;
  ergebnis=malloc(strlen(n)+1);
  strcpy(ergebnis,n);
  if((pos=strchr(ergebnis,'('))!=NULL) *pos=0;
  while(strchr("$%()",ergebnis[strlen(ergebnis)-1])!=NULL && strlen(ergebnis)) ergebnis[strlen(ergebnis)-1]=0;
  return(ergebnis);
}

void varcastint(int typ,void *pointer,int val) {
    if(typ&FLOATTYP) *((double *)pointer)=(double)val;
    else if(typ&INTTYP) *((int *)pointer)=val;
}
void varcastfloat(int typ,void *pointer,double val) {
    if(typ&FLOATTYP) *((double *)pointer)=val;
    else if(typ&INTTYP) *((int *)pointer)=(int)val;
}


char *argument(char *n) {
  char *ergebnis;
  char *pos;
  ergebnis=malloc(strlen(n)+1+100);  
  *ergebnis=0;
  if((pos=strchr(n,'('))!=NULL) {
    pos++;
    strcpy(ergebnis,pos);
    ergebnis[strlen(pos)-1]=0;
  }
  return(ergebnis);
}
int do_dimension(int vnr) {  /* liefert Anzahl der Elemente in einem ARRAY */
  int j,a=1;
 /*  printf("DODIM?: vnr=%d \n",vnr); */
  if(vnr==-1) return(0);   /* Variable existiert nicht  */
  if(variablen[vnr].typ & ARRAYTYP) {
    for(j=0;j<variablen[vnr].opcode;j++) a=a*(((int *)variablen[vnr].pointer)[j]);
    return(a);  
  } else return(1);
}

char *arrptr(char *n) {
  int typ=vartype(n);
  char *r=varrumpf(n);
  char *ergebnis=NULL;
  int vnr=variable_exist(r,typ);
  if(vnr==-1) error(15,r); /* Feld nicht dimensioniert */
  else ergebnis=variablen[vnr].pointer;
  return(ergebnis);
}

void *arrayvarptr(int vnr, char *n,int size) {
  char s[strlen(n)+1],t[strlen(n)+1];
  int ndim=0,a=0,i;
  
  if(vnr!=-1)  {
    
    /* Index- Liste aufloesen  */
       
    i=wort_sep(n,',',TRUE,s,t);
    while(i && ndim<variablen[vnr].opcode) {
      xtrim(s,TRUE,s);
      a=(int)parser(s)+a*((int *)variablen[vnr].pointer)[ndim++];
      i=wort_sep(t,',',TRUE,s,t); 
    }
    return(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE+a*size);
  } else return(NULL);
}
void *arrayvarptr2(int vnr, int *indexliste,int size) {
  int ndim=0,a=0,i=0;
  if(vnr!=-1)  {
    /* Index- Liste aufloesen  */
    while(ndim<variablen[vnr].opcode) a=indexliste[ndim]+a*((int *)variablen[vnr].pointer)[ndim++];
    return(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE+a*size);
  } else return(NULL);
}

void *varptr(char *n) {
  int typ=vartype(n);
  char *r=varrumpf(n);
  void *ergebnis=NULL;
  char *pos=strchr(n,'(');
  int vnr,indize=0;

  if(pos!=NULL) {
    *pos++=0; 
    indize=1; 
    vnr=variable_exist(r,typ|ARRAYTYP);
  } 
  else vnr=variable_exist_or_create(r,typ);
 /* printf("varptr: typ=%d, r=%s, vnr=%d, indize=%d\n",typ,r,vnr,indize);*/
  if(vnr==-1) error(57,n); /* Variable existiert nicht */
  else {
    if(typ & ARRAYTYP) ergebnis=(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
    else if(typ==STRINGTYP) {
      if(indize) ergebnis=((STRING *)(arrayvarptr(vnr,pos,sizeof(STRING))))->pointer;
      else ergebnis=variablen[vnr].pointer;
    } else if(typ==INTTYP) {
      if(indize) ergebnis=arrayvarptr(vnr,pos,sizeof(int)); 
      else ergebnis=(void *)&variablen[vnr].opcode;
    } else if(typ==FLOATTYP) {
      if(indize) ergebnis=arrayvarptr(vnr,pos,sizeof(double)); 
      else ergebnis=(void *)&variablen[vnr].zahl;
    }
    else ergebnis=variablen[vnr].pointer;
  }
  return(ergebnis);
}
void erase_all_variables() {
  int i;
  for(i=0;i<anzvariablen;i++) {
    free(variablen[i].name);
    if(variablen[i].typ==INTTYP) ;
    else if(variablen[i].typ==FLOATTYP) ;
    else if(variablen[i].typ==STRINGTYP) free(variablen[i].pointer);
    else if(variablen[i].typ==INTARRAYTYP) free(variablen[i].pointer);
    else if(variablen[i].typ==FLOATARRAYTYP) free(variablen[i].pointer);
    else if(variablen[i].typ==STRINGARRAYTYP) {
      erase_string_array(i);
      free(variablen[i].pointer); 
    }
  }
  anzvariablen=0;
  /*init_parser();*/
}

void local_vars_loeschen(int p) {
  int i;
  for(i=0;i<anzvariablen;i++) {
    if(variablen[i].local>p) {
     
      free(variablen[i].name);
      if(variablen[i].typ==INTTYP) ;
      else if(variablen[i].typ==FLOATTYP) ;
      else if(variablen[i].typ==STRINGTYP) free(variablen[i].pointer);
      else if(variablen[i].typ==INTARRAYTYP) free(variablen[i].pointer);
      else if(variablen[i].typ==FLOATARRAYTYP) free(variablen[i].pointer);
      else if(variablen[i].typ==STRINGARRAYTYP) {
        erase_string_array(i);
        free(variablen[i].pointer); 
      }
      variablen[i]=variablen[anzvariablen-1];      
      anzvariablen--;i--;
    }
  }
}

void erase_string_array(int vnr) {
  int anz=1,j;  
  STRING *varptr;
  /* Anzahl der Eintraege aus Dimension bestimmen */
	  
  for(j=0;j<variablen[vnr].opcode;j++) anz=anz*((int *)variablen[vnr].pointer)[j];
  varptr=(STRING *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE); 
  for(j=0;j<anz;j++)  free(varptr[j].pointer); 
}

void fill_string_array(int vnr, STRING inh) {
  int anz=do_dimension(vnr),j; 
  STRING *varptr;
  varptr=(STRING *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE); 
  for(j=0;j<anz;j++) {
    varptr[j].pointer=realloc(varptr[j].pointer,inh.len+1);
    memcpy(varptr[j].pointer,inh.pointer,inh.len);
    varptr[j].len=inh.len; 
  }	
}
void copy_string_array(int vnr1, int vnr2) {
  int anz1=do_dimension(vnr1),j;
  int anz2=do_dimension(vnr2);
  int anz=min(anz1,anz2); 
  STRING *varptr1,*varptr2;
       
  varptr1=(STRING *)(variablen[vnr1].pointer+variablen[vnr1].opcode*INTSIZE); 
  varptr2=(STRING *)(variablen[vnr2].pointer+variablen[vnr2].opcode*INTSIZE); 

  for(j=0;j<anz;j++) {
    varptr1[j].pointer=realloc(varptr1[j].pointer,varptr2[j].len+1);
    memcpy(varptr1[j].pointer,varptr2[j].pointer,varptr2[j].len+1);
    varptr1[j].len=varptr2[j].len; 
  }	
}

void fill_int_array(int vnr, int inh) {
  int anz=do_dimension(vnr),j=0; 
  int *pp=(int *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE); 
  while(j<anz) pp[j++]=inh;	 
}
void fill_float_array(int vnr, double inh) {
  int anz=do_dimension(vnr),j=0; 
  double *pp=(double *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE); 
  while(j<anz) pp[j++]=inh;	 
}
void copy_float_array(int vnr1, int vnr2) {
  int anz1=do_dimension(vnr1),j;
  int anz2=do_dimension(vnr2);
  int anz=min(anz1,anz2);

  double *pp1=(double *)(variablen[vnr1].pointer+variablen[vnr1].opcode*INTSIZE); 
  double *pp2=(double *)(variablen[vnr2].pointer+variablen[vnr2].opcode*INTSIZE); 

  for(j=0;j<anz;j++) pp1[j]=pp2[j];	 
}
void copy_int_array(int vnr1, int vnr2) {
  int anz1=do_dimension(vnr1),j;
  int anz2=do_dimension(vnr2);
  int anz=min(anz1,anz2);

  int *pp1=(int *)(variablen[vnr1].pointer+variablen[vnr1].opcode*INTSIZE); 
  int *pp2=(int *)(variablen[vnr2].pointer+variablen[vnr2].opcode*INTSIZE); 

  for(j=0;j<anz;j++) pp1[j]=pp2[j];	 
}
void convert_int_to_float_array(int vnr1, int vnr2) {
  int anz1=do_dimension(vnr1),j;
  int anz2=do_dimension(vnr2);
  int anz=min(anz1,anz2);

  double *pp1=(double *)(variablen[vnr1].pointer+variablen[vnr1].opcode*INTSIZE); 
  int *pp2=(int *)(variablen[vnr2].pointer+variablen[vnr2].opcode*INTSIZE); 

  for(j=0;j<anz;j++) pp1[j]=(double)pp2[j];	 
}
ARRAY convert_to_floatarray(ARRAY a) {
  ARRAY ergeb=create_array(FLOATTYP,a.dimension,a.pointer);
  double *varptr=(double *)(ergeb.pointer+ergeb.dimension*INTSIZE); 
  int *pp2=(int *)(a.pointer+a.dimension*INTSIZE); 
  int j,anz=1;
  for(j=0;j<a.dimension;j++) anz=anz*(((int *)a.pointer)[j]);

  for(j=0;j<anz;j++) varptr[j]=(double)pp2[j];
  return(ergeb);
}
ARRAY convert_to_intarray(ARRAY a) {
  ARRAY ergeb=create_array(INTTYP,a.dimension,a.pointer);
  int *varptr=(int *)(ergeb.pointer+ergeb.dimension*INTSIZE); 
  double *pp2=(double *)(a.pointer+a.dimension*INTSIZE); 
  int j,anz=1;
  for(j=0;j<a.dimension;j++) anz=anz*(((int *)a.pointer)[j]);

  for(j=0;j<anz;j++) varptr[j]=(int)pp2[j];
  return(ergeb);
}
void convert_float_to_int_array(int vnr1, int vnr2) {
  int anz1=do_dimension(vnr1),j;
  int anz2=do_dimension(vnr2);
  int anz=min(anz1,anz2);

  int *pp1=(int *)(variablen[vnr1].pointer+variablen[vnr1].opcode*INTSIZE); 
  double *pp2=(double *)(variablen[vnr2].pointer+variablen[vnr2].opcode*INTSIZE); 

  for(j=0;j<anz;j++) pp1[j]= (int) pp2[j];	 
}

int anz_eintraege(ARRAY a) {/* liefert Anzahl der Elemente in einem ARRAY */
  int anz=1,j;
  for(j=0;j<a.dimension;j++) anz=anz*((int *)a.pointer)[j];
  return(anz);
}

ARRAY create_array(int typ, int dimension, int *dimlist) {
  ARRAY ergebnis;
  int anz=1,j;
  ergebnis.typ=typ;
  ergebnis.dimension=dimension;
  for(j=0;j<dimension;j++) anz=anz*((int *)dimlist)[j];
  ergebnis.pointer=malloc(dimension*INTSIZE+anz*typlaenge(typ));
  
  if(typ & STRINGTYP) {
    int i;
    STRING *varptr;

    varptr=(STRING *)(ergebnis.pointer+dimension*INTSIZE);
    for(i=0;i<anz;i++) {
      varptr[i].len=0;   /* Laenge */
      varptr[i].pointer=NULL;
    }
  }
  /* dimlist kopieren */
  for(j=0;j<dimension;j++) ((int *)(ergebnis.pointer))[j]=dimlist[j];
  return(ergebnis);
}
void free_string(STRING str) {  free(str.pointer); } 

void free_array(ARRAY arr) {
  if(arr.typ & STRINGTYP) {
    int j,anz=1;
    STRING *varptr;
    /* Anzahl der Eintraege aus Dimension bestimmen */
    for(j=0;j<arr.dimension;j++) anz=anz*((int *)arr.pointer)[j];
    varptr=(STRING *)(arr.pointer+arr.dimension*INTSIZE); 
    for(j=0;j<anz;j++)  free(varptr[j].pointer); 	  
  }
  free(arr.pointer); 
} 

void array_add(ARRAY a1, ARRAY a2) {
  int anz=min(anz_eintraege(a1),anz_eintraege(a2)),j;
  
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
  int anz=min(anz_eintraege(a1),anz_eintraege(a2)),j;
  
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
void array_smul(ARRAY a1, double m) {
  int anz=anz_eintraege(a1),j;
  if(a1.typ & FLOATTYP)  {
    double *pp1=(double *)(a1.pointer+a1.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]=m*pp1[j];
  } else if(a1.typ & INTTYP) {
    int *pp1=(int *)(a1.pointer+a1.dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]=m*pp1[j];
  } else puts("ERROR: inkompatible array type.");
}

extern double *SVD(double *a, double *w, double *v,int anzzeilen, int anzspalten);
extern double *backsub(double *, double *, double *, double *,int,int);

ARRAY inv_array(ARRAY a) {
  if(a.dimension==0) {
    double d=*((double *)a.pointer);
    if(d==0) error(0,""); /* Division durch Null */
    else d=1/d;
    return(form_array(a.typ,0,NULL,(char *)&d));
  } else if(a.dimension!=2) {
    error(89,""); /* Array must be two dimensional */
    return(nullmatrix(a.typ,a.dimension,a.pointer));
  } else {
    if(((int *)a.pointer)[0]!=((int *)a.pointer)[1]) {
      error(86,""); /* Matrix nicht quadratisch */
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

/* Transponiere ein 2-d-Array    */

ARRAY trans_array(ARRAY a) {
  ARRAY b=double_array(a);
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
    error(85,"TRANS"); /* Transposition nur für zweidimensionale Matrizen.*/  
  return(b);
}

ARRAY mul_array(ARRAY a1, ARRAY a2) {
  /* Teste ob die Typen fuer eine Multiplikation infrage kommen*/
  if(a1.dimension>2 || a2.dimension>2 || (a1.typ & STRINGTYP) || (a2.typ & STRINGTYP)) {
    error(83,""); /*Matrizenprodukt nicht definiert*/
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
      error(81,""); /*Matrizen haben nicht die gleiche Ordnung*/
      return(nullmatrix(a1.typ,a1.dimension,a1.pointer));
    } else {
      ARRAY ergebnis;
      int s3,i,j,k;
      /*Jetzt typen anpassen:*/
      if(a1.typ!=a2.typ) {  /*d.h. einer ist float*/
        if(a1.typ & INTTYP) { /*covert to float*/
           ergebnis=convert_to_floatarray(a1);
	   free_array(a1);
	   a1=ergebnis;
	}
        if(a2.typ & INTTYP) { /*covert to float*/
           ergebnis=convert_to_floatarray(a2);
	   free_array(a2);
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

/* Uebernimmt einen Speicherbereich in ein Array */

ARRAY form_array(int typ, int dimension, int *dimlist, char *inhalt) {
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
  if(dimension) {
    for(j=0;j<dimension;j++) ((int *)(ergebnis.pointer))[j]=dimlist[j];
  }
  memcpy(pp,inhalt,dlen*anz);
  return(ergebnis);
}

ARRAY nullmatrix(int typ, int dimension, int *dimlist) {
  ARRAY ergebnis=create_array(typ,dimension,dimlist);
  int anz=anz_eintraege(ergebnis),j;
  
  if(typ & INTTYP) {
    int *pp=(int *)(ergebnis.pointer+ergebnis.dimension*INTSIZE);
    for(j=0;j<anz;j++) pp[j]=0;    
  } else if(typ & FLOATTYP) {
    double *pp=(double *)(ergebnis.pointer+dimension*INTSIZE);
    for(j=0;j<anz;j++) pp[j]=0.0;
  }   
  return(ergebnis);
}
ARRAY einheitsmatrix(int typ, int dimension, int *dimlist) {
  ARRAY ergebnis=nullmatrix(typ,dimension,dimlist);
  int anz=anz_eintraege(ergebnis),i,a=0,j;
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


int variable_exist_type(char *name ){
  int typ=vartype(name);
  char w1[strlen(name)+1];
  strcpy(w1,name);
  while(strchr("$%()",w1[strlen(w1)-1])!=NULL && strlen(w1)) w1[strlen(w1)-1]=0;
  return(variable_exist(w1,typ));
}
/* Diese Routine sollte geschwindigkeitsoptimiert sein ! */
int variable_exist(char *name, int typ) {
  int i,vnr=-1,ltyp=-1;
  for(i=0;i<anzvariablen;i++){
    if(variablen[i].typ==typ && variablen[i].local>ltyp){
      if(strcmp(name,variablen[i].name)==0) {
        vnr=i;ltyp=variablen[i].local;	
      }
    }
  }
  return(vnr);
}
int variable_exist_or_create(char *name, int typ) {
  int vnr=variable_exist(name,typ);
  if(vnr==-1) {
    if(typ&FLOATTYP) {
      if((vnr=neue_float_variable(name,0.0,0))==-1) printf("Zu viele Variablen ! max. %d\n",ANZVARS);
    } else if(typ&INTTYP) {
      if((vnr=neue_int_variable(name,0,0))==-1) printf("Zu viele Variablen ! max. %d\n",ANZVARS);
    } else if(typ&STRINGTYP) {
      STRING e;  
      *(e.pointer=malloc(e.len=1))=0;
      if((vnr=neue_string_variable(name,e,0))==-1) printf("Zu viele Variablen ! max. %d\n",ANZVARS);
      free(e.pointer);
    } else printf("Variablen-Typ (%x) komisch! kann nicht kreieren\n",typ);
  } 
  return(vnr);
}
int zuweis(char *name, double wert) {
  /* Zuweisungen fuer Float-Variablen und Felder    */

  int i,s=0,vnr=-1,a=0,ndim;
  if(strchr(name,'(')!=NULL) {
   char *pos, *pos2;
   
   int j;

   pos=strchr(name,'(');
   pos[0]=0;pos++;
   vnr=variable_exist(name,FLOATARRAYTYP);
   if(vnr==-1) error(15,name); /* Feld nicht dimensioniert */
   else {
	pos2=searchchr2(pos,')');
	if(pos2==NULL) puts("ARRAY-Zuweisung noch nicht möglich.");
	else {
	  double *varptr;
	  char *s,*t;
	  int *bbb;
	  int idxn;
	  
	  *pos2=0;
#if DEBUG
	   printf("Zugriff auf Element: %s \n",pos); 
#endif
	  varptr=(double  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
	  /* Index- Liste aufloesen  */
          ndim=0;
	  s=malloc(strlen(pos)+1);t=malloc(strlen(pos)+1);
          i=wort_sep(pos,',',TRUE,s,t);
	  bbb=(int *)variablen[vnr].pointer;
          while(i && ndim<variablen[vnr].opcode) {
            xtrim(s,TRUE,s);
	    idxn=(int)parser(s);
	    if(idxn>bbb[ndim]) error(16,s); /* Feldindex zu gross */
            else a=idxn+a*((int *)variablen[vnr].pointer)[ndim];
            ndim++; 
            i=wort_sep(t,',',TRUE,s,t); 
          }
          free(s);free(t);
          /* printf("Speicherstelle: %d \n",a);*/
          varptr[a]=wert;
        }

   }
  } else {
  /* ist variable schon vorhanden ? */

    if((i=variable_exist(name,FLOATTYP))!=-1)  variablen[i].zahl=wert;
    else {
       if(neue_float_variable(name,wert,0)==-1) 
         printf("Zu viele Variablen ! max. %d\n",ANZVARS);
    }
  }
  return(0);
}

  /* Zuweisungen fuer Int-Variablen und Felder-Eintraege    */

void zuweisi(char *name, int wert) {
  int i,s=0,vnr=-1,a=0,ndim;
  char *r=varrumpf(name);

  if(strchr(name,'(')!=NULL) {
    char *pos, *pos2;
    int j;
   
    pos=strchr(name,'(');
    *pos=0;pos++;
    vnr=variable_exist(r,INTARRAYTYP);
    if(vnr==-1) error(15,name); /* Feld nicht dimensioniert */
    else {
	pos2=searchchr2(pos,')');
	if(pos2==NULL) puts("ARRAY-Zuweisung an dieser Stelle nicht möglich.");
	else {
	  int *varptr;
	  char *s,*t;
	  int *bbb;
	  int idxn;
	  
	  *pos2=0;
	  /* printf("Zugriff auf Element: %s \n",pos); */
	  varptr=(int *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
	  /* Index- Liste aufloesen  */
          ndim=0;
	  s=malloc(strlen(pos)+1);t=malloc(strlen(pos)+1);
          i=wort_sep(pos,',',TRUE,s,t);
	  bbb=(int *)variablen[vnr].pointer;
          while(i && ndim<variablen[vnr].opcode) {
            xtrim(s,TRUE,s);
	    idxn=(int)parser(s);
	    if(idxn>bbb[ndim]) error(16,s); /* Feldindex zu gross */
            else a=idxn+a*((int *)variablen[vnr].pointer)[ndim];
            ndim++; 
            i=wort_sep(t,',',TRUE,s,t); 
          }
          free(s);free(t);
          /* printf("Speicherstelle: %d \n",a);*/
          varptr[a]=wert;
        }
    }
  } else {
  /* ist variable schon vorhanden ? */
    if((i=variable_exist(r,INTTYP))!=-1)  variablen[i].opcode=wert;
    else {
      if(neue_int_variable(r,wert,0)==-1) 
        printf("Zu viele Variablen ! max. %d\n",ANZVARS);
    }
  }
  free(r);
}

int neue_float_variable(char *name, double wert, int sp) {
  if(anzvariablen<ANZVARS) {
    variablen[anzvariablen].name=malloc(strlen(name)+1); 
    strcpy(variablen[anzvariablen].name,name);
    variablen[anzvariablen].zahl=wert;
    variablen[anzvariablen].local=sp;
    variablen[anzvariablen].typ=FLOATTYP;
    anzvariablen++;
    return(anzvariablen-1);
  } else return(-1);
}
int neue_int_variable(char *name, int wert, int sp) {
  if(anzvariablen<ANZVARS) {
    variablen[anzvariablen].name=malloc(strlen(name)+1); 
    strcpy(variablen[anzvariablen].name,name);
    variablen[anzvariablen].opcode=wert;
    variablen[anzvariablen].local=sp;
    variablen[anzvariablen].typ=INTTYP;
    anzvariablen++;
    return(anzvariablen-1);
  } else return(-1);
}

int neue_string_variable(char *name, STRING wert, int sp) { 
  if(anzvariablen<ANZVARS) {
    variablen[anzvariablen].name=malloc(strlen(name)+1); 
    strcpy(variablen[anzvariablen].name,name);
    variablen[anzvariablen].opcode=wert.len;
    variablen[anzvariablen].local=sp;
    variablen[anzvariablen].pointer=malloc(wert.len);
    memcpy(variablen[anzvariablen].pointer,wert.pointer,wert.len);
    variablen[anzvariablen].typ=STRINGTYP;
    anzvariablen++;
    return(anzvariablen-1);
  } else return(-1);
}
int neue_array_variable_and_free(char *name, ARRAY wert, int sp) { 
  /* Uebernimmt ein Array des internen Typs in die Variablenliste */
  /* Es wird nicht kopiert !     */
  if(anzvariablen<ANZVARS) {
    variablen[anzvariablen].name=malloc(strlen(name)+1); 
    strcpy(variablen[anzvariablen].name,name);
    variablen[anzvariablen].opcode=wert.dimension;
    variablen[anzvariablen].local=sp;
    variablen[anzvariablen].pointer=wert.pointer;
    variablen[anzvariablen].typ=(wert.typ | ARRAYTYP);
    anzvariablen++;
    return(anzvariablen-1);
  } else return(-1);
}
void feed_array_and_free(int vnr, ARRAY wert) { 
  /* Uebernimmt ein Array des internen Typs in die Variablenliste */
  /* Es wird nicht kopiert !     */
  /* erst alten Arrayinhalt freigeben */
  free_array(array_info(vnr));
  /* Dann neuen einklinken       */
  variablen[vnr].opcode=wert.dimension;
  variablen[vnr].pointer=wert.pointer;
  variablen[vnr].typ=(wert.typ | ARRAYTYP);
}
void feed_subarray_and_free(int vnr,char *pos, ARRAY wert) { 
  char w1[strlen(pos)+1],w2[strlen(pos)+1];
  int i,e,rdim=0,ndim=0,anz=1,anz2=1,j,k;
  int indexe[variablen[vnr].opcode];
  int indexo[variablen[vnr].opcode];
  int indexa[variablen[vnr].opcode];   

  /* Dimension des reduzierten Arrays bestimmen */
	       
  e=wort_sep(pos,',',TRUE,w1,w2);
  while(e) {
    indexa[ndim]=anz;
    if(*w1!=':' && *w1!=0) {
      indexo[ndim]=(int)parser(w1);
      rdim++;
    } else {
      anz=anz*(((int *)variablen[vnr].pointer)[ndim]);
    /*  printf("dim(vnr)=%d, dim(wert)=%d\n",((int *)variablen[vnr].pointer)[ndim],((int *)wert->pointer)[rdim]);
      do_gets("");*/
      indexo[ndim]=-1;
    }	 
    ndim++;
    e=wort_sep(w2,',',TRUE,w1,w2);
  }
	       
     /* Dimensionierung uebertragen */

  if(wert.dimension!=max(variablen[vnr].opcode-rdim,1)) puts("Dimensionen stimmen nicht!");
  
      /*Loop fuer die Komprimierung */
               
	       for(j=0;j<anz;j++) {
	         int jj=j;
	         /* Indexliste aus anz machen */
                 for(k=variablen[vnr].opcode-1;k>=0;k--) {
		   if(indexo[k]==-1) {
		     indexe[k]=jj/indexa[k];
		     jj=jj % indexa[k];
		     
		   } else indexe[k]=indexo[k];
		 }
		 /* Testen ob passt  */
	         
		 anz2=0;
	         for(k=0;k<variablen[vnr].opcode;k++) 
		   anz2=anz2*((int *)variablen[vnr].pointer)[k]+indexe[k];
	 	 
		 
		 
		 
		 if(jj!=0) {
		   printf("INTERNAL ERROR: %d: Rechnung geht nicht auf. <%s>\n",jj,pos);
#ifdef DEBUG
		   printf("anz=%d\n",anz);
		   printf("--anz2=%d\n",anz2);
		   printf("ARRAY wert: dim=%d (",wert->dimension);
		   for(i=0;i<wert->dimension;i++) printf("%d ",((int *)wert->pointer)[i]);
		   puts(")");
		   printf("ARRAY vnr: dim=%d (",variablen[vnr].opcode);
		   for(i=0;i<variablen[vnr].opcode;i++) printf("%d ",((int *)variablen[vnr].pointer)[i]);
		   puts(")");
		   printf("INDEXO: [");
		   for(i=0;i<variablen[vnr].opcode;i++) printf("%d ",indexo[i]);
		   puts("]");
		   printf("INDEXE: [");
		   for(i=0;i<variablen[vnr].opcode;i++) printf("%d ",indexe[i]);
		   puts("]");
		   printf("INDEXA: [");
		   for(i=0;i<variablen[vnr].opcode;i++) printf("%d ",indexa[i]);
		   puts("]");
		   do_gets("Press RETURN");
#endif
		 }
		   
		 
                 
 /* jetzt kopieren */
	 
    ((double *)(variablen[vnr].pointer+INTSIZE*variablen[vnr].opcode))[anz2]=((double *)(wert.pointer+INTSIZE*wert.dimension))[j];
  }
}

/* Kopiert ein Array  */
ARRAY double_array(ARRAY a) {
  int anz=1,j;
  ARRAY b=a;
  int size;
  for(j=0;j<a.dimension;j++) anz=anz*((int *)a.pointer)[j];
  size=a.dimension*INTSIZE+anz*typlaenge(a.typ);
  b.pointer=malloc(size);
  memcpy(b.pointer,a.pointer,size);
  if(a.typ & STRINGTYP) {
    STRING *ppp1,*ppp2;
    int i;

    ppp1=(STRING *)(b.pointer+a.dimension*INTSIZE);
    ppp2=(STRING *)(a.pointer+a.dimension*INTSIZE);

    for(i=0;i<anz;i++) {
 //     ppp1[i].len=ppp2[i].len;   /* Laenge */
      ppp1[i].pointer=malloc(ppp2[i].len);
      memcpy(ppp1[i].pointer,ppp2[i].pointer,ppp2[i].len);
    }
  }
  return(b);
}

ARRAY copy_var_array(int vnr) { 
  /* Uebernimmt ein Array aus Variablenliste und gibt es als ARRAY Struktur
  zurueck */
  /* Es wird kopiert !     */
  ARRAY zw;

  zw.typ=variablen[vnr].typ&(~ARRAYTYP);
  zw.dimension=variablen[vnr].opcode;
  zw.pointer=variablen[vnr].pointer;
  return(double_array(zw));
}
/* Dasselbe wie copy_var_array, es wird aber nicht kopiert !  */
ARRAY array_info(int vnr) {
  ARRAY zw;
  zw.typ=variablen[vnr].typ&(~ARRAYTYP);
  zw.dimension=variablen[vnr].opcode;
  zw.pointer=variablen[vnr].pointer;
  return(zw);
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
  ergebnis.typ=type2(t);
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
        STRING *varptr,sss;
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



/* Weist einer $-Variable eine Zeichenkette zu */

int zuweiss(char *name, char *inhalt) {
   return(zuweissbuf(name,inhalt,strlen(inhalt)));
}
int zuweissbuf(char *name, char *inhalt,int len) {
   STRING e;
   e.len=len;
   e.pointer=inhalt;
   return(zuweis_string(name,e));
}

int zuweis_string(char *name, STRING inhalt) {
  int i,s=0;
  char *ss,*t,*v,*pos,*pos2,*w;
  
  
  w=malloc(strlen(name)+1);
  strcpy(w,name);
  
  if((pos=strchr(w,'('))!=NULL) {
    int idxn,ndim=0,vnr,anz=0;
    int *bbb;
    
    
    pos2=w+strlen(w)-1;
    *pos++=0;
   
    if(*pos2!=')') puts("Syntax error. Fehlende schliessende Klammer.");
    else *pos2=0;
    w[strlen(w)-1]=0; /* $-Zeichen entfernen */

    vnr=variable_exist(w,STRINGARRAYTYP);
    if(vnr==-1) error(15,w);  /* Feld nicht dimensioniert */
    else {

      /* Dimensionen bestimmen   */

      ss=malloc(strlen(pos)+1);
      t=malloc(strlen(pos)+1);
     
      strcpy(ss,pos);
      
      i=wort_sep(ss,',',TRUE,ss,t);
      if(i==0) puts("Arrayzuweisung an dieser Stelle nicht möglich!");

      bbb=(int *)variablen[vnr].pointer;

      while(i) {
        xtrim(ss,TRUE,ss);
	if(ndim<variablen[vnr].opcode){
          idxn=(int)parser(ss);
	  if(idxn>=bbb[ndim]) {error(16,ss); /* Feldindex zu gross*/ break;}
          else anz=idxn+anz*bbb[ndim];
        }
        ndim++; 
        i=wort_sep(t,',',TRUE,ss,t); 
      } 
      if(ndim!=variablen[vnr].opcode) error(18,"");  /* Falsche Anzahl Indizies */
      else {
          
	pos=(char *)(variablen[vnr].pointer+ndim*INTSIZE+anz*(sizeof(int)+sizeof(char *)));
	    
            
   /* printf("Zuweisung: %s dim:%d anz:%d ll:%d,%d \n",w,ndim,anz,((int
   *)pos)[0],((int *)(pos+2))[0]);  */
      
          ((int *)pos)[0]=inhalt.len;
	  pos+=sizeof(int);
	  ((char **)pos)[0]=realloc(((char **)pos)[0],inhalt.len);
	  memcpy(((char **)pos)[0],inhalt.pointer,inhalt.len);
	 }
	 free(ss);free(t);
      }
  } else { /* Kein Feld */
    w[strlen(name)-1]=0;
  
  /* ist variable schon vorhanden ? */
 
    if((i=variable_exist(w,STRINGTYP))!=-1) string_zuweis_by_vnr(i,inhalt);
    else { 
      if(neue_string_variable(w,inhalt,0)==-1) 
      printf("Zu viele Variablen ! max. %d\n",ANZVARS);
    }
  }
  free(w);
  return(0);
}

void string_zuweis_by_vnr(int vnr, STRING inhalt) {
  if(vnr==-1) return;
  variablen[vnr].opcode=inhalt.len;
  variablen[vnr].pointer=realloc(variablen[vnr].pointer,inhalt.len);
  memcpy(variablen[vnr].pointer,inhalt.pointer,inhalt.len);
}

void xzuweis(char *name, char *inhalt) {
  char *buffer1, *buffer2;
  int typ;
  
  buffer1=indirekt2(name);
  buffer2=indirekt2(inhalt);
  typ=vartype(buffer1);
  if(typ & ARRAYTYP) array_zuweis_and_free(buffer1,array_parser(buffer2));
  else if(typ & STRINGTYP) {
     STRING buffer3=string_parser(buffer2);
     zuweis_string(buffer1,buffer3);
     free(buffer3.pointer);
  } else if(typ & INTTYP) zuweisi(buffer1,parser(buffer2));
  else zuweis(buffer1,parser(buffer2));
  free(buffer1);free(buffer2);
}

void array_zuweis_and_free(char *name, ARRAY inhalt) {
  int typ,vnr;
  char *r=varrumpf(name),*a=argument(name);
  char *nam;
  int ztyp=vartype(name);
  
  vnr=variable_exist(r,inhalt.typ | ARRAYTYP);
  typ=inhalt.typ;
  if(ztyp!=(typ|ARRAYTYP)) {
    if((ztyp & FLOATTYP) && (typ & INTTYP)) {
      
      if(vnr==-1) {
        if(a[0]==0) neue_array_variable_and_free(r,convert_to_floatarray(inhalt), 0);
        else error(15,name); /* Feld nicht dimensioniert */
        free_array(inhalt);
      } else {
        if(a[0]==0) feed_array_and_free(vnr,convert_to_floatarray(inhalt));
        else feed_subarray_and_free(vnr,a,convert_to_floatarray(inhalt));
	free_array(inhalt);
      }
    } else if((ztyp & INTTYP) && (typ & FLOATTYP)) {
      
      if(vnr==-1) {
        if(*a==0) neue_array_variable_and_free(r,convert_to_intarray(inhalt), 0);
        else error(15,name); /* Feld nicht dimensioniert */
        free_array(inhalt);
      } else {
        if(*a==0) feed_array_and_free(vnr,convert_to_intarray(inhalt));
	else feed_subarray_and_free(vnr,a,convert_to_intarray(inhalt));
	free_array(inhalt);
      }
    } else {
      puts("ERROR: Arrays haben nicht dengleichen Typ!");
      printf("ztyp=%d typ=%d\n",ztyp,typ);
      printf("ARRAY: Typ=%d\n",inhalt.typ);
      printf("       dim=%d\n",inhalt.dimension);
      printf("       ptr=%d\n",inhalt.pointer);
    }
  } else {
    if(vnr==-1) {
      if(a[0]==0) neue_array_variable_and_free(r, inhalt, sp);
      else error(15,name); /* Feld nicht dimensioniert */
    } else {
      if(a[0]==0) feed_array_and_free(vnr,inhalt);
      else feed_subarray_and_free(vnr,a,inhalt);
    }
  }
  free(r);free(a);
}


void c_dolocal(char *name, char *inhalt) {
  char *buffer3;
  char *buffer1=indirekt2(name);
  
 /*  printf("Do_local: %s=%s\n",buffer1,inhalt); */
   if(type2(name) & ARRAYTYP) {
     char *nam=varrumpf(buffer1);
     neue_array_variable_and_free(nam,array_parser(inhalt),sp);
     free(nam);
   } else if(type2(name) & STRINGTYP) {
     char *buffer4=malloc(strlen(buffer1)+1);
     STRING e;
     strcpy(buffer4,buffer1);
     buffer4[strlen(buffer4)-1]=0;
     if(strlen(inhalt)) {
       e=string_parser(inhalt);
       
       neue_string_variable(buffer4,e,sp);
       free(e.pointer);
     } else {
       e.len=0;
       e.pointer=buffer4;
       neue_string_variable(buffer4,e,sp);
     }
     free(buffer4);
  } else if(type2(name) & INTTYP) {
    if(strlen(inhalt)) neue_int_variable(buffer1,(int)parser(inhalt),sp);
    else neue_int_variable(buffer1,0,sp);
  } else {
    if(strlen(inhalt)) neue_float_variable(buffer1,parser(inhalt),sp);
    else neue_float_variable(buffer1,0,sp);
  }
  free(buffer1);
}

double arrayinhalt(char *name, char* index) {
  double *varptr;
  char *s,*t;
  int vnr,ndim,a=0,i;
/*  printf("ARRAYINHALT: %s %s\n",name,index);*/
  vnr=variable_exist(name,FLOATARRAYTYP);
  if(vnr==-1) error(15,name); /* Feld nicht dimensioniert */
  else {
    varptr=(double  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
    /* Index- Liste aufloesen  */
    ndim=0;
    s=malloc(strlen(index)+1);t=malloc(strlen(index)+1);
    i=wort_sep(index,',',TRUE,s,t);
    while(i && ndim<variablen[vnr].opcode) {
      xtrim(s,TRUE,s);
      a=(int)parser(s)+a*((int *)variablen[vnr].pointer)[ndim];
      ndim++; 
      i=wort_sep(t,',',TRUE,s,t); 
    }
    free(s);free(t);
    return(varptr[a]);
  }
  return(0.0);
}


double floatarrayinhalt2(ARRAY *a, int *indexliste) {
  int ndim,anz=0;
  double *varptr=(double  *)(a->pointer+a->dimension*INTSIZE);
  for(ndim=0;ndim<a->dimension;ndim++) anz=indexliste[ndim]+anz*((int *)a->pointer)[ndim];
  return(varptr[anz]);
}

int intarrayinhalt2(ARRAY *a, int *indexliste) {
  int ndim,anz=0;
  int *varptr=(int *)(a->pointer+a->dimension*INTSIZE);
  for(ndim=0;ndim<a->dimension;ndim++) anz=indexliste[ndim]+anz*((int *)a->pointer)[ndim];
  return(varptr[anz]);
}
double floatarrayinhalt(int vnr, char* index) {
  double *varptr;
  char s[strlen(index)+1],t[strlen(index)+1];
  
  int ndim=0,a=0,i;
    varptr=(double  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
    /* Index- Liste aufloesen  */
    
    i=wort_sep(index,',',TRUE,s,t);
    while(i && ndim<variablen[vnr].opcode) {
      xtrim(s,TRUE,s);
      a=(int)parser(s)+a*((int *)variablen[vnr].pointer)[ndim];
      ndim++; 
      i=wort_sep(t,',',TRUE,s,t); 
    }
    return(varptr[a]);
}

int intarrayinhalt(int vnr, char* index) {
  int *varptr;
  char s[strlen(index)+1],t[strlen(index)+1];
  int ndim=0,a=0,i;
  varptr=(int  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
    /* Index- Liste aufloesen  */
    i=wort_sep(index,',',TRUE,s,t);
    while(i && ndim<variablen[vnr].opcode) {
      xtrim(s,TRUE,s);
      a=(int)parser(s)+a*((int *)variablen[vnr].pointer)[ndim];
      ndim++; 
      i=wort_sep(t,',',TRUE,s,t); 
    }
    return(varptr[a]);
}
int typlaenge(int typ) {
  if(typ & INTTYP) return(sizeof(int));
  else if(typ & FLOATTYP) return(sizeof(double));
  else if(typ & STRINGTYP) return(sizeof(STRING));
  else if(typ & ARRAYTYP) return(sizeof(ARRAY));
}

/* Wandelt ein ganzes Array in einen String um, so dass es auch wieder
   rueckverwandelt werden kann. Nuetzlich zum Speichern und versenden  */

STRING array_to_string(ARRAY inhalt) {
  STRING ergebnis;
  char *ppp;
  int len,arraylen;
  len=sizeof(int)+sizeof(int);
  len+=INTSIZE*inhalt.dimension;
  arraylen=anz_eintraege(inhalt);
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
  arraylen=anz_eintraege(out);
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
    unsigned int i,offset;
    for(i=0;i<arraylen;i++) {
      offset=(unsigned int)a[i].pointer;
      a[i].pointer=malloc(a[i].len);
      memcpy(a[i].pointer,ppp+offset,a[i].len);
    }
  }
  return(out);
}
