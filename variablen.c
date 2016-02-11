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

/* Bestimmt Typ der Variablen anhand des Ausdrucks 
   Rückgabe ist: 
   NOTYP, wenn Ausdruck leer.
   STRINGTYP/INTTYP/FLOATTYP wenn keine Klammer im Ausdruclk
   ARRAYTYP|(STRINGTYP/INTTYP/FLOATTYP) wenn () im Ausdruclk
   ARRAYTYP|(STRINGTYP/INTTYP/FLOATTYP) wenn Suarray   
   STRINGTYP/INTTYP/FLOATTYP sonst
*/

int vartype(const char *name) {  
  if(!name || strlen(name)==0) return(NOTYP);
  char *w1=strdup(name);
  char *pos;
  int typ=0;
  
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
  free(w1);
  return(typ);
}

char *varrumpf(const char *n) {  /* Rumpf des Variablennamens */
  char *pos,*ergebnis=strdup(n);
  if((pos=strchr(ergebnis,'('))!=NULL) *pos=0;
  while(strchr("$%()",ergebnis[strlen(ergebnis)-1])!=NULL && strlen(ergebnis)) ergebnis[strlen(ergebnis)-1]=0;
  return(ergebnis);
}

/* Zurueck: 0 -- Arrayelement
            1 -- subarray
	    2 -- ganzes array */

static int isarray(const int *indexliste,int n) {
  if(n==0 || indexliste==NULL) return(FULL_ARRAY);
  while(--n>=0) { if(indexliste[n]==-1) return(SUB_ARRAY); }
  return(NO_ARRAY);
}

/* entferne Variablen.*/ 

void erase_variable(VARIABLE *a) {
  if(a->flags==V_DYNAMIC && a->pointer.i) {
    // printf("erase var: %s %p\n",a->name,a->pointer.i);
    if(a->typ==ARRAYTYP)       free_array(a->pointer.a);
    else if(a->typ==STRINGTYP) free_string(a->pointer.s);
    free(a->pointer.i);
  }
  a->pointer.i=NULL;
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
      case INTTYP:   fill_int_array(arr,0);    break;
      case FLOATTYP: fill_float_array(arr,0);  break;
    }
    break;
  case STRINGTYP:
    str=v->pointer.s;
    str->len=0;
    *(str->pointer)=0;
    break;
  case INTTYP:   *(v->pointer.i)=0;  break;
  case FLOATTYP: *(v->pointer.f)=0;  break;
  }
}

int var_exist(const char *name, unsigned char typ,unsigned char subtyp, int l) {
  int i,j;
  VARIABLE *v;
//  printf("var exist: <%s> typ=%d, subtyp=%d, l=%d",name,typ,subtyp,l);
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
        if(v[i].typ==typ && (v[i].pointer.a) && (v[i].pointer.a)->typ==subtyp){
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


/* Jetzt syntax check des Variablennamens. 
   1. Darf nicht mit einer Zahl beginnen
   2. Darf kein Leerzeichen enthalten
   3. Systemvariablennamen sind verboten.

Returns:
   0 -- Name is allowed
   -1 -- not allowed
   */
static int varnamecheck(const char *n, int typ) {
//  printf("varnamecheck: %s\n",s);
  if(!n || *n==0 || (*n>='0' && *n<='9')) return(-1);
  char c;
  const char *s=n;
  while((c=*n++)) {
    if(c==' ') return(-1);
  }
  if(typ==FLOATTYP) {
    int i=0,a=anzsysvars-1,b,l=strlen(s);
    c=*s;
    for(b=0; b<l; c=s[++b]) {
      while(c>(sysvars[i].name)[b] && i<a) i++;
      while(c<(sysvars[a].name)[b] && a>i) a--;
      if(i>=a) break;
    }
    if(strcmp(s,sysvars[i].name)==0) {
      printf("ERROR: Assignment to Sysvar %s not allowed.\n",sysvars[i].name);
      return(-1);
    }
  } else if(typ==STRINGTYP) {
     /* Liste durchgehen */
     int i=0,a=anzsyssvars-1,b,l=strlen(s);
     for(b=0; b<l; b++) {
       while(s[b]>(syssvars[i].name)[b] && i<a) i++;
       while(s[b]<(syssvars[a].name)[b] && a>i) a--;
       if(i>=a) break;
     }
	//  printf("i=%d <%s>\n",i,syssvars[i].name);
     if(i==a && l==strlen(syssvars[i].name)-1 && syssvars[i].name[l]=='$' && strncmp(s,syssvars[i].name,l)==0) {
        printf("ERROR: Assignment to Sysvar %s not allowed.\n",syssvars[i].name);
        return(-1);
     }
  }
  return(0);
}


/*legt neue variable an mit default wert (also 0,"", Arraydim0) und 
  gibt variablennummer zurueck. Der name muss nur der rumpf sein! */

int add_variable(const char *name, unsigned char typ, unsigned char subtyp, unsigned int flags, void *adr) {
  int vnr=var_exist(name,typ,subtyp,0);

  if(vnr==-1) {   /*Variable gibt es noch nicht, also neue anlegen. */
  //  printf("Add new variable. %s flags=%x\n",name,flags);
    if(anzvariablen<ANZVARS) { 
      if(varnamecheck(name,typ)) xberror(32,name); /* Syntax error */
      else {
        variablen[anzvariablen].name=strdup(name);
        variablen[anzvariablen].typ=typ;
        variablen[anzvariablen].flags=flags;
        if(flags==V_STATIC) variablen[anzvariablen].pointer.i=adr;
        else variablen[anzvariablen].pointer.i=NULL;
        vnr=anzvariablen++;
      }
    } else printf("WARNING: too many variables! Can not create more than %d in this version of X11-Basic.\n",ANZVARS);
  }
  if(vnr>=0 && variablen[vnr].pointer.i==NULL) {
     // printf("Add variable %d %s\n",vnr,name);
 
      if(typ==ARRAYTYP) {
	variablen[vnr].pointer.a=calloc(1,sizeof(ARRAY));
	*(variablen[vnr].pointer.a)=create_array(subtyp,0,NULL);
      } else if(typ==FLOATTYP) {
        variablen[vnr].pointer.f=calloc(1,sizeof(double));
      } else if(typ==INTTYP) {
        variablen[vnr].pointer.i=calloc(1,sizeof(int));
      } else if(typ==STRINGTYP) {
        variablen[vnr].pointer.s=calloc(1,sizeof(STRING));
	*(variablen[vnr].pointer.s)=create_string(NULL);
      }
  }
  return(vnr);
}


/*Nachtraegliches umwandeln einer dynamischen Variable in eine statische, bei ABSOLUTE und VAR*/
void set_var_adr(int vnr,void *adr) {
  if(vnr<anzvariablen) {
    erase_variable(&variablen[vnr]);
    variablen[vnr].flags=V_STATIC;
    variablen[vnr].pointer.i=adr;
  }
}


void zuweisxbyindex(int vnr,int *indexliste,int n,char *ausdruck) {
  int typ=variablen[vnr].typ;
  char *varptr=varptr_indexliste(&variablen[vnr],indexliste,n);
  int ia;
   
  if(typ==ARRAYTYP) {
    ia=isarray(indexliste,n);
    
    if(ia==NO_ARRAY) typ=(variablen[vnr].pointer.a)->typ;
    else if(ia==SUB_ARRAY) {
      xberror(9,"Subarray"); /* Funktion noch nicht moeglich */
    }
  }
  // printf("zuw: %s: %p ia=%d typ=$%x\n",variablen[vnr].name,varptr,ia,typ);
  if(varptr) {
    ARRAY arr,*zarr;
    switch(typ) {
    case ARRAYTYP:
      arr=array_parser(ausdruck);
    //  zarr=(ARRAY *)varptr;
      zarr=variablen[vnr].pointer.a;
     
      if(arr.typ==zarr->typ) {
        free_array(zarr);
        *zarr=arr;
      } else if(zarr->typ==INTTYP) {
        free_array(zarr);
        *zarr=convert_to_intarray(&arr);
	free_array(&arr);
      } else if(zarr->typ==FLOATTYP) {
        free_array(zarr);
        *zarr=convert_to_floatarray(&arr);
	free_array(&arr);
      }
      else {
          xberror(58,variablen[vnr].name); /* Variable %s has incorrect type*/  
	printf("dest-Typ: %d, ",zarr->typ);
	printf("typ=ARRAY, isarray=%d  ausdruck=<%s>\n",ia,ausdruck);  
	printf("INFO: Typ1=%d, Typ2=%d\n",arr.typ,((ARRAY *)varptr)->typ);
      }
      break;
    case STRINGTYP:
      {
        STRING a=string_parser(ausdruck);
        free_string((STRING *)varptr);
        *((STRING *)varptr)=a;
      }
      break;
    case INTTYP:   *((int *)varptr)=(int)parser(ausdruck);       break;
    case FLOATTYP: *((double *)varptr)=(double)parser(ausdruck); break;
    default:       xberror(13,variablen[vnr].name);  /* Type mismatch */ 
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
	} else xberror(96,variablen[vnr].name); /* Array has wrong type */
      } else {
        switch(variablen[vnr].pointer.a->typ) {
        case STRINGTYP:
	  if(p->typ==PL_STRING) {
            str=double_string((STRING *)&(p->integer));
            free_string((STRING *)varptr);
	    *((STRING *)varptr)=str;
          } else xberror(13,variablen[vnr].name);  /* Type mismatch */
          break;
	case INTTYP:
	  if(p->typ==PL_INT)        *((int *)varptr)=p->integer;
	  else if(p->typ==PL_FLOAT) *((int *)varptr)=(int)p->real;
          else xberror(13,variablen[vnr].name);  /* Type mismatch */
	  break;
	case FLOATTYP: 
	  if(p->typ==PL_FLOAT)    *((double *)varptr)=p->real;
          else if(p->typ==PL_INT) *((double *)varptr)=(double)p->integer;
          else xberror(13,variablen[vnr].name);  /* Type mismatch */
          break;
        }
      }
      break;
    case STRINGTYP:
      if(p->typ==PL_STRING) {
        str=double_string((STRING *)&(p->integer));
        free_string((STRING *)varptr);
        *((STRING *)varptr)=str;
      } else xberror(13,variablen[vnr].name);  /* Type mismatch */
      break;
    case INTTYP:
      if(p->typ==PL_INT)        *((int *)varptr)=p->integer;
      else if(p->typ==PL_FLOAT) *((int *)varptr)=(int)p->real;
      else xberror(13,variablen[vnr].name);  /* Type mismatch */
      break;
    case FLOATTYP:
      if(p->typ==PL_FLOAT)    *((double *)varptr)=p->real;
      else if(p->typ==PL_INT) *((double *)varptr)=(double)p->integer;
      else xberror(13,variablen[vnr].name);  /* Type mismatch */
      break;
    default:
      xberror(13,variablen[vnr].name);  /* Type mismatch */ 
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
   // Typ ist: $%x, das ist kein ARRAYtyp. Trotzdem gibt es %d indizies. 
  //  xberror(18,""); /* Falsche Anzahl Indizes */
    xberror(15,v->name); /* Array not dimensioned */
    return(NULL);
  }
  
  switch(v->typ) {
  case ARRAYTYP:
    {
      int ndim,anz=0;
      int a=isarray(indexliste,n);
   //   printf("n=%d, a=%d\n",n,a);
      if(n>v->pointer.a->dimension) {
        if(v->pointer.a->dimension) xberror(18,"varptr"); /* Falsche Anzahl Indizes */
        else xberror(15,v->name); /* Array not dimensioned */
        return(NULL);
      }
      if(check_feldindex(v,indexliste,n)) { xberror(16,"varptr"); /* Feldindex zu gro� */ return(NULL);}
      if(a==FULL_ARRAY)     varptr=(char *)v->pointer.a;  /* zeigt dann quasi auf ARRAY struktur*/
      else if(a==SUB_ARRAY) varptr=(char  *)(v->pointer.a->pointer+v->pointer.a->dimension*INTSIZE);
      else {                varptr=(char  *)(v->pointer.a->pointer+v->pointer.a->dimension*INTSIZE);
        if(n==0) return(varptr);
        for(ndim=0;ndim<v->pointer.a->dimension;ndim++) 
          anz=indexliste[ndim]+anz*((int *)v->pointer.a->pointer)[ndim];
        switch(v->pointer.a->typ) {
        case STRINGTYP: varptr+=anz*sizeof(STRING); break;
        case INTTYP:    varptr+=anz*sizeof(int);    break;
        case FLOATTYP:  varptr+=anz*sizeof(double); break;
        default:        xberror(13,v->name);  /* Type mismatch */ 
        }
      }
    } 
    break;
  case STRINGTYP: varptr=(char *)v->pointer.s;  break;
  case INTTYP:    varptr=(char *)v->pointer.i;  break;
  case FLOATTYP:  varptr=(char *)v->pointer.f;  break;
  default:        xberror(13,v->name);  /* Type mismatch */ 
  }
  return(varptr);
}



int zuweis(const char *name, double wert) {
  /* Zuweisungen fuer Float-Variablen und Felder    */
  char w1[strlen(name)+1],w2[strlen(name)+1];
  int e=klammer_sep(name,w1,w2);
  char *r=varrumpf(w1);
  int vnr;
  if(e==2) {
    int ii=count_parameters(w2);
    vnr=add_variable(r,ARRAYTYP,FLOATTYP,V_DYNAMIC,NULL);
    if(ii==0) {
      xberror(9,"Array assignment"); /* Funktion noch nicht moeglich */
    } else {
      int indexliste[ii];
      double *varptr;
      make_indexliste(variablen[vnr].pointer.a->dimension,w2,indexliste);
      varptr=(double  *)varptr_indexliste(&variablen[vnr],indexliste,ii);
      if(varptr) *varptr=wert;
    }
  } else {
    vnr=add_variable(r,FLOATTYP,0,V_DYNAMIC,NULL);
    *(variablen[vnr].pointer.f)=wert;
  }
  free(r);
  return(0);
}
int izuweis(const char *name, int wert) {
  /* Zuweisungen fuer Int-Variablen und Felder    */
  char w1[strlen(name)+1],w2[strlen(name)+1];
  int e=klammer_sep(name,w1,w2);
  char *r=varrumpf(w1);
  int vnr;
  if(e==2) {
    int ii=count_parameters(w2);
    vnr=add_variable(r,ARRAYTYP,INTTYP,V_DYNAMIC,NULL);
    if(ii==0) {
      xberror(9,"Array assignment"); /* Funktion noch nicht moeglich */
    } else {
      int indexliste[ii];
      int *varptr;
      make_indexliste(variablen[vnr].pointer.a->dimension,w2,indexliste);
      varptr=(int  *)varptr_indexliste(&variablen[vnr],indexliste,ii);
      if(varptr) *varptr=wert;
    }
  } else {
    vnr=add_variable(r,INTTYP,0,V_DYNAMIC,NULL);
    *(variablen[vnr].pointer.i)=wert;
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

  if(wert.dimension!=max(variablen[vnr].opcode-rdim,1)) xberror(74,variablen[vnr].name); /* Dimensioning mismatch */
  for(j=0;j<anz;j++) {  /*Loop fuer die Komprimierung */
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
		   xberror(70,""); /* Unknown Error */
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
int zuweis_string_and_free(const char *name, STRING inhalt) {
  char w1[strlen(name)+1],w2[strlen(name)+1];
  int e=klammer_sep(name,w1,w2);
  char *r=varrumpf(w1);
  int vnr;
  if(e==2) {
    int ii=count_parameters(w2);
    vnr=add_variable(r,ARRAYTYP,STRINGTYP,V_DYNAMIC,NULL);
    if(ii==0) xberror(9,"Array assignment"); /* Funktion noch nicht moeglich */
    else {
      int indexliste[ii];
      STRING *varptr;
      make_indexliste(variablen[vnr].pointer.a->dimension,w2,indexliste);
      varptr=(STRING  *)varptr_indexliste(&variablen[vnr],indexliste,ii);
      free_string(varptr);
      if(varptr) *varptr=inhalt;
    }
  } else {
    vnr=add_variable(r,STRINGTYP,0,V_DYNAMIC,NULL);
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

void xzuweis(const char *name, char *inhalt) {
  int vnr;
  int dim,ii=0;
  char *vname,*argument;
  int *indexliste=NULL;
  
  char *buffer1=indirekt2(name);
  char *buffer2=indirekt2(inhalt);
  int typ=vartype(buffer1);
  int e=klammer_sep_destroy(buffer1,&vname,&argument);
  char *r=varrumpf(buffer1);
  // printf("xzuweis: <%s> <%s>\n",name,inhalt);
  if(e>1) {
    vnr=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP),V_DYNAMIC,NULL);
    ii=count_parameters(argument);
    dim=variablen[vnr].pointer.a->dimension;
    if(dim<ii) xberror(18,name); /* Falsche Anzahl Indizes */
    indexliste=malloc(sizeof(int)*ii);
    make_indexliste(ii,argument,indexliste);
  } else vnr=add_variable(r,typ,0,V_DYNAMIC,NULL);  

  zuweisxbyindex(vnr,indexliste,ii,buffer2);  
  free(r); free(indexliste);
  free(buffer1);free(buffer2);
}


/*Kopiere Inhalt von VARIABLEN Struktur. Hierbei wird typ unc local einfach Uebernommen, der Pointer zum namen 
  einfach kopiert und nur der Inhalt dupliziert. (neue Pointer und Speicherbereiche werden angelegt.) 
  
  Bei statischen variablen wird der pointer kopiert und auf der Quellvariable eine neue dynamische Struktur angelegt.
  
  alte variable anschliessend in a, neue in b.
  */

static void copy_var(VARIABLE *a,VARIABLE *b) {
  *a=*b;
     switch(b->typ) {
     case STRINGTYP:
       b->pointer.s=malloc(sizeof(STRING));
       *(b->pointer.s)=double_string(a->pointer.s);
       break;
     case ARRAYTYP: 
       b->pointer.a=malloc(sizeof(ARRAY));
       *(b->pointer.a)=double_array(a->pointer.a);
       break;
     case INTTYP:
       b->pointer.i=malloc(sizeof(int));
       *(b->pointer.i)=*(a->pointer.i);
       break;
     case FLOATTYP:
       b->pointer.f=malloc(sizeof(double));
       *(b->pointer.f)=*(a->pointer.f);
       break;
     default:
       xberror(13,a->name);  /* Type mismatch */
     }
     b->flags=V_DYNAMIC;
}

/*restauriere Variable, hierbei werden nur die Inhalte Uebertragen, typ und name bleiben unveraendert.
Ziel-Variable wird vorher freigegeben und dann mit neuem Inhalt überschriebeb. Quell-Variablenspeicherbereich wird
freigegeben.  

Ist die Quelle eine STATIC variable, wird das Ziel auch static.


*/

static void move_var(VARIABLE *a,VARIABLE *b) {
    erase_variable(a);
    *a=*b;
}

void do_local(int vnr,int sp) {
 // printf("Dolocal[%d]: %d <%s> %p --> ",sp,vnr,variablen[vnr].name,variablen[vnr].pointer.i);
  if(anzlvar[sp]==0) lvar[sp]=malloc(sizeof(VARIABLE));
  else lvar[sp]=realloc(lvar[sp],(anzlvar[sp]+1)*sizeof(VARIABLE));
  copy_var(&(lvar[sp])[anzlvar[sp]],&variablen[vnr]);
  (lvar[sp])[anzlvar[sp]].local=vnr;
//  printf("%p\n",variablen[vnr].pointer.i);
  anzlvar[sp]++;
}

void restore_locals(int sp) {
  int i,vnr;
  if(anzlvar[sp]) {
    for(i=0;i<anzlvar[sp];i++) {
      vnr=(lvar[sp])[i].local;
     // printf("Restore-var[%d]: %s %p --> %p\n",sp,variablen[vnr].name,variablen[vnr].pointer.i,(lvar[sp])[i].pointer.i);
      move_var(&variablen[vnr],&(lvar[sp])[i]);
    }
    anzlvar[sp]=0;
    free(lvar[sp]);
  }
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



STRING create_string(const char *n) {
  STRING ergeb;
  if(n) {
    ergeb.len=strlen(n);
    ergeb.pointer=strdup(n);
  } else {
    ergeb.len=0;
    ergeb.pointer=malloc(1);
    ergeb.pointer[0]=0;
  }
  return(ergeb);
}
STRING double_string(const STRING *a) {
  STRING b;
  b.len=a->len;
  b.pointer=malloc(b.len+1);
  memcpy(b.pointer,a->pointer,b.len);
  (b.pointer)[b.len]=0;
  return(b);
}

