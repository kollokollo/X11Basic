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
#include "x11basic.h"
#include "variablen.h"
#include "parameter.h"
#include "xbasic.h"
#include "array.h"
#include "wort_sep.h"
#include "parser.h"

VARIABLE variablen[ANZVARS];
int anzvariablen;

VARIABLE *lvar[STACKSIZE];
int anzlvar[STACKSIZE];

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
  ergebnis=strdup(n);
  if((pos=strchr(ergebnis,'('))!=NULL) *pos=0;
  while(strchr("$%()",ergebnis[strlen(ergebnis)-1])!=NULL && strlen(ergebnis)) ergebnis[strlen(ergebnis)-1]=0;
  return(ergebnis);
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
    vnr=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP));
  } else vnr=add_variable(r,typ,0);

 /* printf("varptr: typ=%d, r=%s, vnr=%d, indize=%d\n",typ,r,vnr,indize);*/
    if(typ & ARRAYTYP) ergebnis=((variablen[vnr].pointer.a)->pointer+(variablen[vnr].pointer.a)->dimension*INTSIZE);
    else if(typ==STRINGTYP) {
      if(indize) ergebnis=((STRING *)(arrayvarptr(vnr,pos,sizeof(STRING))))->pointer;
      else ergebnis=(variablen[vnr].pointer.s)->pointer;
    } else if(typ==INTTYP) {
      if(indize) ergebnis=arrayvarptr(vnr,pos,sizeof(int)); 
      else ergebnis=variablen[vnr].pointer.i;
    } else if(typ==FLOATTYP) {
      if(indize) ergebnis=arrayvarptr(vnr,pos,sizeof(double)); 
      else ergebnis=variablen[vnr].pointer.f;
    }
    else ergebnis=variablen[vnr].pointer.i;
  free(r);
  return(ergebnis);
}

/* entferne Variablen.*/ 

void erase_all_variables() {
  while(anzvariablen) erase_variable(&variablen[--anzvariablen]);
}

void erase_variable(VARIABLE *a) {
  if(a->typ==ARRAYTYP) {
    free_array(a->pointer.a);
  } else if(a->typ==STRINGTYP) {
    free_string(a->pointer.s);
  }
  free(a->pointer.i);
  a->pointer.i=NULL;
}
/*Variablen werden mit 0 oder "" initialisiert. Arrays mit ... 
  entfernt werden die Variablen niemals.*/ 
void clear_all_variables() {
  int i=0;
  while(i<anzvariablen) clear_variable(&variablen[i++]);
}

void clear_variable(VARIABLE *v) {
  STRING inh;
  ARRAY *arr;
  STRING *str;
//printf("clearvar typ=$%x\n",v->typ);
  switch(v->typ) {
  case ARRAYTYP:
    arr=v->pointer.a;
    switch(arr->typ) {
      case STRINGTYP:
        inh=create_string(NULL);
        fill_string_array(arr,inh);
        free_string(&inh);
        break;
      case INTTYP: 
        fill_int_array(arr,0);
        break;
      case FLOATTYP:
        fill_float_array(arr,0);
        break;
    }
    break;
  case STRINGTYP:
    str=v->pointer.s;
    str->len=0;
    *(str->pointer)=0;
    break;
  case INTTYP:
    *(v->pointer.i)=0;
    break;
  case FLOATTYP:
    *(v->pointer.f)=0;
    break;
  }
}


inline void free_string(STRING *str) {  free(str->pointer);str->pointer=NULL;str->len=0; } 

int var_exist(char *name, unsigned char typ,unsigned char subtyp, int l) {
  int i,j;
  VARIABLE *v;
  if(l) {
    v=lvar[l];
    j=anzlvar[l];
  } else  {
    v=variablen;
    j=anzvariablen;
  }
  if(j) {
    if(typ==ARRAYTYP && subtyp) {
      for(i=0;i<j;i++){
        if(v[i].typ==typ && (v[i].pointer.a)->typ==subtyp){
          if(strcmp(name,v[i].name)==0) return(i);
        }
      }
    } else {
      for(i=0;i<j;i++){
        if(v[i].typ==typ){
          if(strcmp(name,v[i].name)==0) return(i);
        }
      }
    }
  }
  return(-1);
}
/*legt neue variable an mit default wert (also 0,"", Arraydim0) und 
  gibt variablennummer zurueck. Der name muss nur der rumpf sein! */

int add_variable(char *name, unsigned char typ, unsigned char subtyp) {
  int vnr=var_exist(name,typ,subtyp,0);
  if(vnr==-1) {
    if(anzvariablen<ANZVARS) { 
      variablen[anzvariablen].name=strdup(name);
      variablen[anzvariablen].typ=typ;
  
      if(typ==ARRAYTYP) {
	variablen[anzvariablen].pointer.a=calloc(1,sizeof(ARRAY));
	*(variablen[anzvariablen].pointer.a)=create_array(subtyp,0,NULL);
      } else if(typ==FLOATTYP) {
        variablen[anzvariablen].pointer.f=calloc(1,sizeof(double));
      } else if(typ==INTTYP) {
        variablen[anzvariablen].pointer.i=calloc(1,sizeof(int));
      } else if(typ==STRINGTYP) {
        variablen[anzvariablen].pointer.s=calloc(1,sizeof(STRING));
	*(variablen[anzvariablen].pointer.s)=create_string(NULL);
      }
      vnr=anzvariablen++;
    } else printf("Zu viele Variablen ! Konnte var nicht anlegen. anz=%d, max. %d\n",anzvariablen,ANZVARS);
  } 
  return(vnr);
}
int add_variable_adr(char *name, unsigned char typ, char *adr) {
  int subtyp=0;  /* Vorlauefig ... ???*/
  int vnr=var_exist(name,typ,subtyp,0);
  if(vnr==-1) {
    if(anzvariablen<ANZVARS) { 
      variablen[anzvariablen].name=strdup(name);
      variablen[anzvariablen].typ=typ;
      variablen[anzvariablen].pointer.i=(int *)adr;
      vnr=anzvariablen++;
   //   printf("add var: $%x\n",adr);
    } else printf("Zu viele Variablen ! Konnte var nicht anlegen. anz=%d, max. %d\n",anzvariablen,ANZVARS);
  } 
  return(vnr);
}


void zuweisxbyindex(int vnr,int *indexliste,int n,char *ausdruck) {
  int typ=variablen[vnr].typ;
  char *varptr=varptr_indexliste(&variablen[vnr],indexliste,n);
  int ia;
   
  if(typ==ARRAYTYP) {
    ia=isarray(indexliste,n);
    if(ia==0) typ=(variablen[vnr].pointer.a)->typ;
    else if(ia==1) {
      printf("ERROR: Subarray geht noch nicht...\n");
    
    }
  }
  // printf("zuw: %x ia=%d typ=$%x\n",varptr,ia,typ);
  if(varptr) {
    ARRAY arr;
    switch(typ) {
    case ARRAYTYP:
      arr=array_parser(ausdruck);
      if(arr.typ==((ARRAY *)varptr)->typ) *((ARRAY *)varptr)=arr;
      else if(((ARRAY *)varptr)->typ==INTTYP) {
        *((ARRAY *)varptr)=convert_to_intarray(&arr);
	free_array(&arr);
      }
      else if(((ARRAY *)varptr)->typ==FLOATTYP) {
        *((ARRAY *)varptr)=convert_to_floatarray(&arr);
	free_array(&arr);
      }
      else xberror(58,""); /* Variable %s has incorrect type*/  
      break;
    case STRINGTYP:
     *((STRING *)varptr)=(STRING)string_parser(ausdruck);
      break;
    case INTTYP:
     *((int *)varptr)=(int)parser(ausdruck);
      break;
    case FLOATTYP:
     *((double *)varptr)=(double)parser(ausdruck);
      break;
    default:
      printf("1Something is wrong. Variable typ=$%x\n",typ); 
    }
  } 
}


void zuweispbyindex(int vnr,int *indexliste,int n,PARAMETER *p) {
  int typ=variablen[vnr].typ;
  char *varptr;
  STRING str;
  if(indexliste==NULL) n=0;
#if 0
  else {
    int i;
    printf("INDEXE: ");
    for(i=0;i<n;i++) printf("%d ",indexliste[i]);
    printf("\n");
  }
#endif
  varptr=varptr_indexliste(&variablen[vnr],indexliste,n);

  if(varptr) {
    switch(typ) {
    case ARRAYTYP:
      if(p->typ==PL_ARRAY) {
        if((variablen[vnr].pointer.a)->typ==p->arraytyp) {
	  ARRAY a=double_array((ARRAY *)&(p->integer));
	  free_array(variablen[vnr].pointer.a);
	  *(variablen[vnr].pointer.a)=a;
	} else printf("zuweis: ARRAYtypen stimmen nicht.\n");
      } else {
        switch(variablen[vnr].pointer.a->typ) {
        case STRINGTYP:
	  if(p->typ==PL_STRING) {
            str=double_string((STRING *)&(p->integer));
            free_string((STRING *)varptr);
	    *((STRING *)varptr)=str;
          } else printf("ERROR: Must be string.\n");
          break;
	case INTTYP:
	  if(p->typ==PL_INT)        *((int *)varptr)=p->integer;
	  else if(p->typ==PL_FLOAT) *((int *)varptr)=(int)p->real;
          else printf("ERROR: Must be int.\n");
	  break;
	case FLOATTYP: 
	  if(p->typ==PL_FLOAT)    *((double *)varptr)=p->real;
          else if(p->typ==PL_INT) *((double *)varptr)=(double)p->integer;
          else printf("ERROR: Must be flt.\n");
          break;
        }
      }
      break;
    case STRINGTYP:
      if(p->typ==PL_STRING) {
        str=double_string((STRING *)&(p->integer));
        free_string((STRING *)varptr);
        *((STRING *)varptr)=str;
      } else printf("ERROR: Must be string.\n");
      break;
    case INTTYP:
      if(p->typ==PL_INT)        *((int *)varptr)=p->integer;
      else if(p->typ==PL_FLOAT) *((int *)varptr)=(int)p->real;
      else printf("ERROR: Must be int.\n");
      break;
    case FLOATTYP:
      if(p->typ==PL_FLOAT)    *((double *)varptr)=p->real;
      else if(p->typ==PL_INT) *((double *)varptr)=(double)p->integer;
      else printf("ERROR: Must be flt.\n");
      break;
    default:
      printf("2Something is wrong. Variable typ=$%x\n",typ); 
    }
  }
}

/* Testet ob in indexliste ein Feldindex zu gross ist.*/

int check_feldindex(VARIABLE *v,int *indexliste,int n) {
  int i=0;
  int *di=(int *)(v->pointer.a->pointer);
  while(i<n) {
    if(indexliste[i]>=di[i]) return(-1);  /*Feldindex zu gross*/
    i++;
  }
  return(0); /*0 = alles OK*/
}


/* Zentrale funktion. Diese wird von allen zuweisoperationen 
   benutzt.
   */

char *varptr_indexliste(VARIABLE *v,int *indexliste,int n) {
  char *varptr=NULL;
  if(v->typ!=ARRAYTYP && n) {
    printf("Typ ist: $%x\n",v->typ);
    xberror(18,""); /* Falsche Anzahl Indizes */
    return(NULL);
  }
  
  // if(indexliste) printf("Index ist: %d\n",*indexliste);
  
  switch(v->typ) {
  case ARRAYTYP:
    {
      int ndim,anz=0;
      int a=isarray(indexliste,n);
     // printf("n=%d, a=%d\n",n,a);
      if(n>v->pointer.a->dimension) {
      printf("variable: dim=%d indizien %d\n",v->pointer.a->dimension,n);
        xberror(18,"varptr"); /* Falsche Anzahl Indizes */
        c_dump("",0);exit(0);
        return(NULL);
      }
      if(check_feldindex(v,indexliste,n)) {
        xberror(16,"varptr"); /* Feldindex zu groß */
        return(NULL);
      }
      if(a==2) {  /*ganzes array*/
        varptr=(char *)&(v->pointer.a);  /* zeigt dann quasi auf ARRAY */
      } else if(a) { /* Subarray */
        varptr=(char  *)(v->pointer.a->pointer+v->pointer.a->dimension*INTSIZE);
      } else {
        varptr=(char  *)(v->pointer.a->pointer+v->pointer.a->dimension*INTSIZE);
        if(n==0) return(varptr);
        for(ndim=0;ndim<v->pointer.a->dimension;ndim++) 
          anz=indexliste[ndim]+anz*((int *)v->pointer.a->pointer)[ndim];
        switch(v->pointer.a->typ) {
        case STRINGTYP: 
          varptr+=anz*sizeof(STRING);
          break;
        case INTTYP: 
          varptr+=anz*sizeof(int);
          break;
        case FLOATTYP: 
          varptr+=anz*sizeof(double);
          break;
        default: 
          printf("varptr: unbekannter typ.\n");
        }
      }
    } 
    break;
  case STRINGTYP: 
    varptr=(char *)v->pointer.s;  
    break;
  case INTTYP:
    varptr=(char *)v->pointer.i;  
    break;
  case FLOATTYP: 
    varptr=(char *)v->pointer.f;  
    break;
  default: 
      printf("varptr: unbekannter typ.\n");
  }
//  printf("v=$%x\n",varptr);
  return(varptr);
}



int zuweis(char *name, double wert) {
  /* Zuweisungen fuer Float-Variablen und Felder    */
  char w1[strlen(name)+1],w2[strlen(name)+1];
  int e=klammer_sep(name,w1,w2);
  char *r=varrumpf(w1);
  int vnr;
  if(e==2) {
    int ii=count_parameters(w2);
    vnr=add_variable(r,ARRAYTYP,FLOATTYP);
    if(ii==0) puts("ARRAY-Zuweisung noch nicht möglich.");
    else {
      int indexliste[ii];
      double *varptr;
      make_indexliste(variablen[vnr].pointer.a->dimension,w2,indexliste);
      varptr=(double  *)varptr_indexliste(&variablen[vnr],indexliste,ii);
      *varptr=wert;
    }
  } else {
    vnr=add_variable(r,FLOATTYP,0);
    *(variablen[vnr].pointer.f)=wert;
  }
  free(r);
  return(0);
}


#if 0
void feed_subarray_and_free(int vnr,char *pos, ARRAY wert) { 
  char w1[strlen(pos)+1],w2[strlen(pos)+1];
  int e,rdim=0,ndim=0,anz=1,anz2=1,j,k;
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
#endif

/* Weist einer $-Variable eine Zeichenkette zu */

/*Achtung: String inhalt wird nicht dupliziert !*/
int zuweis_string_and_free(char *name, STRING inhalt) {
  char w1[strlen(name)+1],w2[strlen(name)+1];
  int e=klammer_sep(name,w1,w2);
  char *r=varrumpf(w1);
  int vnr;
  if(e==2) {
    int ii=count_parameters(w2);
    vnr=add_variable(r,ARRAYTYP,STRINGTYP);
    if(ii==0) puts("ARRAY-Zuweisung noch nicht möglich.");
    else {
      int indexliste[ii];
      STRING *varptr;
      make_indexliste(variablen[vnr].pointer.a->dimension,w2,indexliste);
      varptr=(STRING  *)varptr_indexliste(&variablen[vnr],indexliste,ii);
      free_string(varptr);
      *varptr=inhalt;
    }
  } else {
    vnr=add_variable(r,STRINGTYP,0);
    free_string(variablen[vnr].pointer.s);
    *(variablen[vnr].pointer.s)=inhalt;
  }
  free(r);
  return(0);
}

void string_zuweis(VARIABLE *v, STRING inhalt) {
  v->pointer.s->len=inhalt.len;
  v->pointer.s->pointer=realloc(v->pointer.s->pointer,inhalt.len+1);
  memcpy(v->pointer.s->pointer,inhalt.pointer,inhalt.len);
  v->pointer.s->pointer[inhalt.len]=0;
}

void xzuweis(char *name, char *inhalt) {
  char *buffer1, *buffer2;
  int typ;
  char *r;
  int vnr;
  int dim,ii=0,e;
  char *vname,*argument;
  int *indexliste=NULL;
  
  buffer1=indirekt2(name);
  buffer2=indirekt2(inhalt);
  typ=vartype(buffer1);
  e=klammer_sep_destroy(buffer1,&vname,&argument);
  r=varrumpf(buffer1);
  if(e>1) {
    vnr=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP));
    ii=count_parameters(argument);
    dim=variablen[vnr].pointer.a->dimension;
    if(dim<ii) xberror(18,name); /* Falsche Anzahl Indizes */
    indexliste=malloc(sizeof(int)*ii);
    make_indexliste(ii,argument,indexliste);
  } else vnr=add_variable(r,typ,0);  

  zuweisxbyindex(vnr,indexliste,ii,buffer2);  
  free(r); free(indexliste);
  free(buffer1);free(buffer2);
}

void copy_var(VARIABLE *a,VARIABLE *b) {
  switch(b->typ) {
  case STRINGTYP:
   // free_string(a->pointer.s);
    *a=*b;
    a->pointer.s=malloc(sizeof(STRING));
    *(a->pointer.s)=double_string(b->pointer.s);
    break;
  case ARRAYTYP: 
   // free_array(a->pointer.a);
    *a=*b;
    a->pointer.a=malloc(sizeof(ARRAY));
    *(a->pointer.a)=double_array(b->pointer.a);
    break;
  case INTTYP:
 // printf("umf %x $%x\n",(int)a,a->typ);
    *a=*b;
    a->pointer.i=malloc(sizeof(int));
    *(a->pointer.i)=*(b->pointer.i);
    break;
  
  case FLOATTYP:
 // printf("umf %x $%x\n",(int)a,a->typ);
    *a=*b;
    a->pointer.f=malloc(sizeof(double));
    *(a->pointer.f)=*(b->pointer.f);
    break;
  default:
    *a=*b;
    printf("Falsche Typen: $%x\n",a->typ);
  }
}

/*restauriere Variable */

void move_var(VARIABLE *a,VARIABLE *b) {
//  printf("mov a: ");
//  memdump(a,sizeof(VARIABLE));
//  printf("mov b: ");
//  memdump(b,sizeof(VARIABLE));
  switch(a->typ) {
  case INTTYP:
    *(a->pointer.i)=*(b->pointer.i);
    break;
  case FLOATTYP:
    *(a->pointer.f)=*(b->pointer.f);
    break;
  case STRINGTYP:
    *(a->pointer.s)=*(b->pointer.s);
    break;
  case ARRAYTYP:
    *(a->pointer.a)=*(b->pointer.a);
    break;
  default:
    *a=*b;
  }  
  free(b->pointer.a);
}

void do_local(int vnr,int sp) {
//  printf("Dolocal: %d <%s> %d\n",vnr,variablen[vnr].name,sp);
  if(anzlvar[sp]==0) lvar[sp]=malloc(sizeof(VARIABLE));
  else lvar[sp]=realloc(lvar[sp],(anzlvar[sp]+1)*sizeof(VARIABLE));
  copy_var(&(lvar[sp])[anzlvar[sp]],&variablen[vnr]);
  (lvar[sp])[anzlvar[sp]].local=vnr;
  anzlvar[sp]++;
}

void restore_locals(int sp) {
  int i,vnr;
  if(anzlvar[sp]) {
    for(i=0;i<anzlvar[sp];i++) {
      vnr=(lvar[sp])[i].local;
  //    printf("Restore-var: %d\n",vnr);
      move_var(&variablen[vnr],&(lvar[sp])[i]);
    }
    anzlvar[sp]=0;
    free(lvar[sp]);
  }
}

void c_dolocal(int vnr, char *inhalt) {
  do_local(vnr,sp);
  zuweisxbyindex(vnr,NULL,0,inhalt);
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

STRING varstringarrayinhalt(int vnr, int *indexliste) {
  int dim=variablen[vnr].pointer.a->dimension;
  int *bbb=(int *)variablen[vnr].pointer.a->pointer;
  int i,anz=0;
  STRING *varptr;
  for(i=0;i<dim;i++){
    if(indexliste[i]>=bbb[i]) {xberror(16,""); /* Feldindex zu gross*/ break;}
    else anz=indexliste[i]+anz*bbb[i];
  }
  varptr=(STRING *)(variablen[vnr].pointer.a->pointer+dim*INTSIZE);
  return(double_string(&varptr[anz]));
}

int typlaenge(int typ) {
  switch(typ) {
  case INTTYP: return(sizeof(int));
  case FLOATTYP: return(sizeof(double));
  case STRINGTYP: return(sizeof(STRING));
  case ARRAYTYP: return(sizeof(ARRAY));
  }
  return(0);
}

/************************* CASTS *******************************/

void varcastint(int vnr,void *pointer,int val) {
    int typ=variablen[vnr].typ;
    if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
    if(typ==FLOATTYP) *((double *)pointer)=(double)val;
    else if(typ==INTTYP) *((int *)pointer)=val;
}
void varcastfloat(int vnr,void *pointer,double val) {
    int typ=variablen[vnr].typ;
    if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
    if(typ==FLOATTYP) *((double *)pointer)=val;
    else if(typ==INTTYP) *((int *)pointer)=(int)val;
}

void varcaststring(int vnr,void *pointer,STRING val) {
  int typ=variablen[vnr].typ;
  if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
  if(typ==STRINGTYP) {
    STRING *s=(STRING *)pointer;
    free(s->pointer);
    *s=double_string(&val);
  }
}

void varcaststring_and_free(int typ,void *pointer,STRING val) {
    STRING *s=(STRING *)pointer;
    free(s->pointer);
    *s=val;
}


STRING create_string(char *n) {
  STRING ergeb;
  if(n) ergeb.len=strlen(n);
  else ergeb.len=0;
  ergeb.pointer=malloc(ergeb.len+1);
  if(n) memcpy(ergeb.pointer,n,ergeb.len+1);
  else *(ergeb.pointer)=0;
  return(ergeb);
}
STRING create_string_and_free(char *n,int l) {
  STRING ergeb;
  ergeb.pointer=n;
  ergeb.len=l;
  return(ergeb);
}
STRING double_string(STRING *a) {
  STRING b;
  b.len=a->len;
  b.pointer=malloc(b.len+1);
  memcpy(b.pointer,a->pointer,b.len);
  (b.pointer)[b.len]=0;
  return(b);
}

