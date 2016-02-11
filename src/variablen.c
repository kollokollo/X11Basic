/* variablen.c (c) Markus Hoffmann             */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
  char *ergebnis=malloc(strlen(n)+1);
  char *pos;
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
  char *ergebnis=malloc(strlen(n)+1);
  char *pos;
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
ARRAY *convert_to_floatarray(ARRAY *a) {
  ARRAY *ergeb=create_array(FLOATARRAYTYP,a->dimension,a->pointer);
  double *varptr=(double *)(ergeb->pointer+ergeb->dimension*INTSIZE); 
  int *pp2=(int *)(a->pointer+a->dimension*INTSIZE); 
  int j,anz=1;
  for(j=0;j<a->dimension;j++) anz=anz*(((int *)a->pointer)[j]);

  for(j=0;j<anz;j++) varptr[j]=(double)pp2[j];
  return(ergeb);
}
ARRAY *convert_to_intarray(ARRAY *a) {
  ARRAY *ergeb=create_array(INTARRAYTYP,a->dimension,a->pointer);
  int *varptr=(int *)(ergeb->pointer+ergeb->dimension*INTSIZE); 
  double *pp2=(double *)(a->pointer+a->dimension*INTSIZE); 
  int j,anz=1;
  for(j=0;j<a->dimension;j++) anz=anz*(((int *)a->pointer)[j]);

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



int anz_eintraege(ARRAY *a) {
  int anz=1,j;
  for(j=0;j<a->dimension;j++) anz=anz*((int *)a->pointer)[j];
  return(anz);
}

ARRAY *create_array(int typ, int dimension, int *dimlist) {
  ARRAY *ergebnis=malloc(sizeof(ARRAY));
  int anz=1,j;
  ergebnis->typ=typ;
  ergebnis->dimension=dimension;
  for(j=0;j<dimension;j++) anz=anz*((int *)dimlist)[j];
  
  if(typ & INTTYP) ergebnis->pointer=malloc(dimension*INTSIZE+anz*sizeof(int));
  else if(typ & FLOATTYP) ergebnis->pointer=malloc(dimension*INTSIZE+anz*sizeof(double));
  else {
    int i;
    STRING *varptr;
    ergebnis->pointer=malloc(dimension*INTSIZE+anz*sizeof(STRING));

    varptr=(STRING *)(ergebnis->pointer+dimension*INTSIZE);
    for(i=0;i<anz;i++) {
      varptr[i].len=0;   /* Laenge */
      varptr[i].pointer=malloc(1);
    }
  }
  /* dimlist kopieren */
  for(j=0;j<dimension;j++) ((int *)(ergebnis->pointer))[j]=dimlist[j];
  return(ergebnis);
}

void free_array(ARRAY *arr) {

    if(arr->typ==INTARRAYTYP) free(arr->pointer);
    else if(arr->typ==FLOATARRAYTYP) free(arr->pointer);
    else if(arr->typ==STRINGARRAYTYP) {
       int  j,anz=1;
       STRING *varptr;
      /* Anzahl der Eintraege aus Dimension bestimmen */
	  
	for(j=0;j<arr->dimension;j++) anz=anz*((int *)arr->pointer)[j];

        varptr=(STRING *)(arr->pointer+arr->dimension*INTSIZE); 
        for(j=0;j<anz;j++)  free(varptr[j].pointer); 	
      free(arr->pointer); 
    }
  free(arr);
}

void array_add(ARRAY *a1, ARRAY *a2) {
  int anz=min(anz_eintraege(a1),anz_eintraege(a2)),j;
  
  
  if((a1->typ & FLOATTYP) && (a2->typ & FLOATTYP)) {
    double *pp1=(double *)(a1->pointer+a1->dimension*INTSIZE); 
    double *pp2=(double *)(a2->pointer+a2->dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]+=pp2[j];
  } else if((a1->typ & FLOATTYP) && (a2->typ & INTTYP)) {
    double *pp1=(double *)(a1->pointer+a1->dimension*INTSIZE); 
    int *pp2=(int *)(a2->pointer+a2->dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]+=(double)pp2[j];
  } else if((a1->typ & INTTYP) && (a2->typ & INTTYP)) {
    int *pp1=(int *)(a1->pointer+a1->dimension*INTSIZE); 
    int *pp2=(int *)(a2->pointer+a2->dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]+=pp2[j];
  } else if((a1->typ & INTTYP) && (a2->typ & FLOATTYP)) {
    int *pp1=(int *)(a1->pointer+a1->dimension*INTSIZE); 
    double *pp2=(double *)(a2->pointer+a2->dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]+=(int)pp2[j];
  } else if((a1->typ & STRINGTYP) && (a2->typ & STRINGTYP)) {
    STRING *pp1=(STRING *)(a1->pointer+a1->dimension*INTSIZE); 
    STRING *pp2=(STRING *)(a2->pointer+a2->dimension*INTSIZE); 
    
    for(j=0;j<anz;j++) {
          pp1[j].pointer=realloc(pp1[j].pointer,pp2[j].len+pp1[j].len+1);
	  strcat(pp1[j].pointer,pp2[j].pointer);
	  pp1[j].len+=pp2[j].len; 
    }	
  } else puts("ERROR: inkompatible array type.");
}
void array_sub(ARRAY *a1, ARRAY *a2) {
  int anz=min(anz_eintraege(a1),anz_eintraege(a2)),j;
  
  
  if((a1->typ & FLOATTYP) && (a2->typ & FLOATTYP)) {
    double *pp1=(double *)(a1->pointer+a1->dimension*INTSIZE); 
    double *pp2=(double *)(a2->pointer+a2->dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]-=pp2[j];
  } else if((a1->typ & FLOATTYP) && (a2->typ & INTTYP)) {
    double *pp1=(double *)(a1->pointer+a1->dimension*INTSIZE); 
    int *pp2=(int *)(a2->pointer+a2->dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]-=(double)pp2[j];
  } else if((a1->typ & INTTYP) && (a2->typ & INTTYP)) {
    int *pp1=(int *)(a1->pointer+a1->dimension*INTSIZE); 
    int *pp2=(int *)(a2->pointer+a2->dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]-=pp2[j];
  } else if((a1->typ & INTTYP) && (a2->typ & FLOATTYP)) {
    int *pp1=(int *)(a1->pointer+a1->dimension*INTSIZE); 
    double *pp2=(double *)(a2->pointer+a2->dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]-=(int)pp2[j];
 
  } else puts("ERROR: inkompatible array type.");
}
void array_smul(ARRAY *a1, double m) {
  int anz=anz_eintraege(a1),j;
  if(a1->typ & FLOATTYP)  {
    double *pp1=(double *)(a1->pointer+a1->dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]=m*pp1[j];
  } else if(a1->typ & INTTYP) {
    int *pp1=(int *)(a1->pointer+a1->dimension*INTSIZE); 
    for(j=0;j<anz;j++) pp1[j]=m*pp1[j];
  } else puts("ERROR: inkompatible array type.");
}
ARRAY *inv_array(ARRAY *a) {
  error(9,"INV"); /* Befehl noch nicht moeglich.*/
  return(nullmatrix(a->typ,a->dimension,a->pointer));
}
ARRAY *trans_array(ARRAY *a) {
  error(9,"TRANS"); /* Befehl noch nicht moeglich.*/
  return(nullmatrix(a->typ,a->dimension,a->pointer));
}

ARRAY *mul_array(ARRAY *a1, ARRAY *a2) {
  ARRAY *ergebnis;
  int anz=anz_eintraege(a1),j;
  if(a1->dimension>2 || a1->dimension<1 || a2->dimension>2 || a2->dimension<1) {
    puts("Arrays muessen ein oder zweidimensional sein: Multiplikation nicht moeglich.");
    ergebnis=nullmatrix(a1->typ,a1->dimension,a1->pointer);
  } else {
    if(a1->typ & FLOATTYP)  {
      double *pp1=(double *)(a1->pointer+a1->dimension*INTSIZE); 
      puts("Multiplikation noch nicht möglich.");
      ergebnis=nullmatrix(a1->typ,a1->dimension,a1->pointer);
      
    } else if(a1->typ & INTTYP) {
      int *pp1=(int *)(a1->pointer+a1->dimension*INTSIZE); 
      puts("Multiplikation noch nicht möglich.");
      ergebnis=nullmatrix(a1->typ,a1->dimension,a1->pointer);
    } else {
      puts("Inkompatibler Array-Typ: Multiplikation nicht möglich.");
      ergebnis=nullmatrix(a1->typ,a1->dimension,a1->pointer);
    }
  }
  return(ergebnis);
}
ARRAY *nullmatrix(int typ, int dimension, int *dimlist) {
  ARRAY *ergebnis=malloc(sizeof(ARRAY));
  int anz=1,j;
  ergebnis->typ=typ;
  ergebnis->dimension=dimension;
  for(j=0;j<dimension;j++) anz=anz*((int *)dimlist)[j];
  
  if(typ & INTTYP) {
    int *pp;
    ergebnis->pointer=malloc(dimension*INTSIZE+anz*sizeof(int));
    pp=(int *)(ergebnis->pointer+dimension*INTSIZE);
    for(j=0;j<anz;j++) pp[j]=0;
    
  } else if(typ & FLOATTYP) {
    double *pp;
    ergebnis->pointer=malloc(dimension*INTSIZE+anz*sizeof(double));
    pp=(double *)(ergebnis->pointer+dimension*INTSIZE);
    for(j=0;j<anz;j++) pp[j]=0.0;
 
  } else {
    int i;
    STRING *ppp;
    ergebnis->pointer=malloc(dimension*INTSIZE+anz*(sizeof(int)+sizeof(char *)));

    ppp=(STRING *)(ergebnis->pointer+dimension*INTSIZE);
    for(i=0;i<anz;i++) {
      ppp[i].len=0;   /* Laenge */
      ppp[i].pointer=malloc(1);
    }
  }
  /* dimlist kopieren */
  for(j=0;j<dimension;j++) ((int *)(ergebnis->pointer))[j]=dimlist[j];
  return(ergebnis);
}
ARRAY *einheitsmatrix(int typ, int dimension, int *dimlist) {
  ARRAY *ergebnis=malloc(sizeof(ARRAY));
  int anz=1,i,a=0,j,q;
  ergebnis->typ=typ;
  ergebnis->dimension=dimension;
  for(j=0;j<dimension;j++) anz=anz*((int *)dimlist)[j];
  
  if(typ & INTTYP) {
    int *varptr;
    ergebnis->pointer=malloc(dimension*INTSIZE+anz*sizeof(int));
    varptr=(int *)(ergebnis->pointer+dimension*INTSIZE);
    for(j=0;j<anz;j++) varptr[j]=0;
    q=dimlist[0];
     for(j=0;j<dimension;j++) {if(q>dimlist[j]) q=dimlist[j];}
    for(j=0;j<q;j++) {
      for(i=0;i<dimension;i++) a=j+a*dimlist[i]; 
      varptr[a]=1;
    }
  } else if(typ & FLOATTYP) {
    double *varptr;
    ergebnis->pointer=malloc(dimension*INTSIZE+anz*sizeof(double));
    varptr=(double *)(ergebnis->pointer+dimension*INTSIZE);
    for(j=0;j<anz;j++) varptr[j]=0.0;
    /* Jetzt die Diagonalelemente */
    q=dimlist[0];
    for(j=0;j<dimension;j++) {if(q>dimlist[j]) q=dimlist[j];}
    for(j=0;j<q;j++) {
      for(i=0;i<dimension;i++) a=j+a*dimlist[i]; 
      varptr[a]=1.0;
    }
  } else {
    STRING *ppp;
    ergebnis->pointer=malloc(dimension*INTSIZE+anz*(sizeof(int)+sizeof(char *)));

    ppp=(STRING *)(ergebnis->pointer+dimension*INTSIZE);
    for(i=0;i<anz;i++) {
      ppp[i].len=0;   /* Laenge */
      ppp[i].pointer=malloc(1);
    }
    puts("Einheitsmatrix für Stringfelder nicht definiert !");
  }
  /* dimlist kopieren */
  for(j=0;j<dimension;j++) ((int *)(ergebnis->pointer))[j]=dimlist[j];
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
    } else printf("Variablen-Typ komisch! kann nicht kreieren\n");
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
int neue_array_variable_and_free(char *name, ARRAY *wert, int sp) { 
  /* Uebernimmt ein Array des internen Typs in die Variablenliste */
  /* Es wird nicht kopiert !     */
  
  if(anzvariablen<ANZVARS) {
    variablen[anzvariablen].name=malloc(strlen(name)+1); 
    strcpy(variablen[anzvariablen].name,name);
    variablen[anzvariablen].opcode=wert->dimension;
    variablen[anzvariablen].local=sp;
    variablen[anzvariablen].pointer=wert->pointer;
    variablen[anzvariablen].typ=(wert->typ | ARRAYTYP);
    free(wert);
    anzvariablen++;
    return(anzvariablen-1);
  } else return(-1);
}
void feed_array_and_free(int vnr, ARRAY *wert) { 
  /* Uebernimmt ein Array des internen Typs in die Variablenliste */
  /* Es wird nicht kopiert !     */

  /* erst alten Arrayinhalt freigeben */

  free_array(array_info(vnr));
  /* Dann neuen einklinken       */
  
    variablen[vnr].opcode=wert->dimension;
    variablen[vnr].pointer=wert->pointer;
    free(wert);
}
void feed_subarray_and_free(int vnr,char *pos, ARRAY *wert) { 
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

  if(wert->dimension!=max(variablen[vnr].opcode-rdim,1)) puts("Dimensionen stimmen nicht!");
  
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
	 
    ((double *)(variablen[vnr].pointer+INTSIZE*variablen[vnr].opcode))[anz2]=((double *)(wert->pointer+INTSIZE*wert->dimension))[j];
  }
  free(wert);
}


ARRAY *copy_var_array(int vnr) { 
  /* Uebernimmt ein Array aus Variablenliste und gibt es als ARRAY Struktur
  zurueck */
  /* Es wird kopiert !     */
  int anz=1,j;
  ARRAY *zw=malloc(sizeof(ARRAY));

  zw->typ=variablen[vnr].typ;
  zw->dimension=variablen[vnr].opcode;
  
  for(j=0;j<zw->dimension;j++) anz=anz*((int *)variablen[vnr].pointer)[j];
  
  if(zw->typ & INTTYP) {
    zw->pointer=malloc(zw->dimension*INTSIZE+anz*sizeof(int));
    memcpy(zw->pointer,variablen[vnr].pointer,zw->dimension*INTSIZE+anz*sizeof(int));
  } else if(zw->typ & FLOATTYP) {
    zw->pointer=malloc(zw->dimension*INTSIZE+anz*sizeof(double));
    memcpy(zw->pointer,variablen[vnr].pointer,zw->dimension*INTSIZE+anz*sizeof(double));
  } else {
    STRING *ppp1,*ppp2;
    int i;
    zw->pointer=malloc(zw->dimension*INTSIZE+anz*sizeof(STRING));
    memcpy(zw->pointer,variablen[vnr].pointer,zw->dimension*INTSIZE+anz*sizeof(STRING));

    ppp1=(STRING *)(zw->pointer+zw->dimension*INTSIZE);
    ppp2=(STRING *)(variablen[vnr].pointer+zw->dimension*INTSIZE);

    for(i=0;i<anz;i++) {
      ppp1[i].len=ppp2[i].len;   /* Laenge */
      ppp1[i].pointer=malloc(ppp2[i].len+1);
      memcpy(ppp1[i].pointer,ppp2[i].pointer,ppp2[i].len+1);
    }
  }

  return(zw);
}

ARRAY *array_info(int vnr) {
  ARRAY *zw=malloc(sizeof(ARRAY));
  zw->typ=variablen[vnr].typ;
  zw->dimension=variablen[vnr].opcode;
  zw->pointer=variablen[vnr].pointer;
  return(zw);
}
ARRAY *array_const(char *s) {
  ARRAY *ergebnis=malloc(sizeof(ARRAY));
  char t[strlen(s)+1],t2[strlen(s)+1],s2[strlen(s)+1];
  int e,i=0,j=0,f,dx=0,dy,anz=1;
  /* Ein oder Zweidimensional ? */
  ergebnis->dimension=wort_sep(s,';',TRUE,t,t2);
  /* Typ Bestimmen */
  wort_sep(t,' ',TRUE,t,t2);
  ergebnis->typ=(type2(t) | ARRAYTYP);
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
  
  
  if(ergebnis->typ & INTTYP) ergebnis->pointer=malloc(ergebnis->dimension*INTSIZE+anz*sizeof(int));
  else if(ergebnis->typ & FLOATTYP) ergebnis->pointer=malloc(ergebnis->dimension*INTSIZE+anz*sizeof(double));
  else {
    int i;
    STRING *varptr;
    ergebnis->pointer=malloc(ergebnis->dimension*INTSIZE+anz*sizeof(STRING));

    varptr=(STRING *)(ergebnis->pointer+ergebnis->dimension*INTSIZE);
    for(i=0;i<anz;i++) {
      varptr[i].len=0;   /* Laenge */
      varptr[i].pointer=malloc(1);
    }
  }
  if(ergebnis->dimension==1) *((int *)(ergebnis->pointer))=dx;
  else if(ergebnis->dimension==2) {
    ((int *)(ergebnis->pointer))[0]=dy;
    ((int *)(ergebnis->pointer))[1]=dx;
  }
  i=j=0;
    e=wort_sep(s,';',TRUE,t,s2);
  while(e) {
    f=wort_sep(t,',',TRUE,t2,t);
    while(f) {
      if(ergebnis->typ & INTTYP) ((int *)(ergebnis->pointer+ergebnis->dimension*INTSIZE))[j]=(int)parser(t2);
      else if(ergebnis->typ & FLOATTYP) ((double *)(ergebnis->pointer+ergebnis->dimension*INTSIZE))[j]=parser(t2);
      else {
        STRING *varptr,sss;
        varptr=(STRING *)(ergebnis->pointer+ergebnis->dimension*INTSIZE);
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
 
    if((i=variable_exist(w,STRINGTYP))!=-1) {
      variablen[i].opcode=inhalt.len;
      variablen[i].pointer=realloc(variablen[i].pointer,inhalt.len);
      memcpy(variablen[i].pointer,inhalt.pointer,inhalt.len);
    } else { 
      if(neue_string_variable(w,inhalt,0)==-1) 
      printf("Zu viele Variablen ! max. %d\n",ANZVARS);
    }
  }
  free(w);
  return(0);
}



void xzuweis(char *name, char *inhalt) {
  char *buffer1, *buffer2;
  int typ;
  
  buffer1=indirekt2(name);
  buffer2=indirekt2(inhalt);
  typ=vartype(buffer1);
  
 /* printf("XZUWEIS: %s \n",buffer1); */

       
  if(typ & ARRAYTYP)  {
     ARRAY *buffer3=array_parser(buffer2);
     array_zuweis_and_free(buffer1, buffer3);
  } else if(typ & STRINGTYP) {
     STRING buffer3=string_parser(buffer2);
     zuweis_string(buffer1,buffer3);
     free(buffer3.pointer);
  } else if(typ & INTTYP) zuweisi(buffer1,parser(buffer2));
  else zuweis(buffer1,parser(buffer2));
  free(buffer1);free(buffer2);
}

void array_zuweis_and_free(char *name, ARRAY *inhalt) {
  int typ,ztyp;
 
  ztyp=vartype(name);
 
  typ=inhalt->typ;
  if(ztyp!=typ) {    
    if((ztyp & FLOATTYP) && (typ & INTTYP)) {
      char *r=varrumpf(name);
      char *a=argument(name);
      int vnr=variable_exist(r,inhalt->typ | ARRAYTYP);
      if(vnr==-1) {
        if(a[0]==0) neue_array_variable_and_free(r,convert_to_floatarray(inhalt), 0);
        else error(15,name); /* Feld nicht dimensioniert */
        free_array(inhalt);
      } else {
        if(a[0]==0) feed_array_and_free(vnr,convert_to_floatarray(inhalt));
        else feed_subarray_and_free(vnr,a,convert_to_floatarray(inhalt));
	free_array(inhalt);
      }
      free(r); free(a);
    } else if((ztyp & INTTYP) && (typ & FLOATTYP)) {
      char *r=varrumpf(name);char *a=argument(name);
      int vnr=variable_exist(r,inhalt->typ | ARRAYTYP);
      if(vnr==-1) {
        if(*a==0) neue_array_variable_and_free(r,convert_to_intarray(inhalt), 0);
        else error(15,name); /* Feld nicht dimensioniert */
        free_array(inhalt);
      } else {
        if(*a==0) feed_array_and_free(vnr,convert_to_intarray(inhalt));
	else feed_subarray_and_free(vnr,a,convert_to_intarray(inhalt));
	free_array(inhalt);
      }
      free(r); free(a);
    } else {
      puts("ERROR: Arrays haben nicht dengleichen Typ!");
      printf("ztyp-typ=%d\n",ztyp-typ);
      printf("ARRAY: Typ=%d\n",inhalt->typ);
      printf("       dim=%d\n",inhalt->dimension);
      printf("       ptr=%d\n",inhalt->pointer);
    }
  } else {
    char *r=varrumpf(name);char *a=argument(name);
    int vnr=variable_exist(r,inhalt->typ | ARRAYTYP);
    
    if(vnr==-1) {
      if(a[0]==0) neue_array_variable_and_free(r, inhalt, sp);
      else error(15,name); /* Feld nicht dimensioniert */
    } else {
      if(a[0]==0) feed_array_and_free(vnr,inhalt);
      else feed_subarray_and_free(vnr,a,inhalt);
    }
    free(r);free(a);
  }
}


void c_dolocal(char *name, char *inhalt) {
  char *buffer1, *buffer3;
  
  buffer1=indirekt2(name);
  
 /*  printf("Do_local: %s=%s\n",buffer1,inhalt); */
   if(strcmp(buffer1+strlen(buffer1)-3,"$()")==0) {
    printf("Stringfelder noch nicht moeglich. %s\n",buffer1);
   } else if(strcmp(buffer1+strlen(buffer1)-2,"()")==0) {
     printf("Feld-Zuweisung noch nicht moeglich. %s\n",buffer1);
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
  return;
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
  int *varptr;
  int ndim,anz=0;

  varptr=(int *)(a->pointer+a->dimension*INTSIZE);
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
