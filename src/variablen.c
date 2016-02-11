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
#include "protos.h"
#include "globals.h"
#include "array.h"


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





void free_pliste(int anz,PARAMETER *pret){
  int i;
  for(i=0;i<anz;i++)  free_parameter(pret[i]);
  free(pret);
}

void free_parameter(PARAMETER p) {
 /* printf("Free_parameter: %d\n",p.typ); */
  if(p.typ==PL_STRING) free(p.pointer);
  else if(p.typ==PL_KEY) free(p.pointer);
  else if(p.typ&PL_ARRAY) {
    ARRAY a;
    a.typ=p.typ;
    a.dimension=p.integer;
    a.pointer=p.pointer;
    free_array(a);
  }  
}
void free_string(STRING str) {  free(str.pointer); } 

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

int zuweisbyindex(int vnr,int *indexliste,double wert) {
  int ndim,anz=0;
  if(vnr>-1) {
    double *varptr=(double  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
    for(ndim=0;ndim<variablen[vnr].opcode;ndim++) 
      anz=indexliste[ndim]+anz*((int *)variablen[vnr].pointer)[ndim];
    varptr[anz]=wert;
    return(0);
  } else return(-1);
}
int zuweisibyindex(int vnr,int *indexliste,int wert) {
  int ndim,anz=0;
  if(vnr>-1) {
    int *varptr=(int  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
    for(ndim=0;ndim<variablen[vnr].opcode;ndim++) 
      anz=indexliste[ndim]+anz*((int *)variablen[vnr].pointer)[ndim];
    varptr[anz]=wert;
    return(0);
  } else return(-1);
}
int zuweissbyindex(int vnr,int *indexliste,STRING wert) {
  int ndim,anz=0;
  if(vnr>-1) {
    int dim=variablen[vnr].opcode;
    STRING *varptr=(STRING  *)(variablen[vnr].pointer+dim*INTSIZE);
    for(ndim=0;ndim<dim;ndim++) 
      anz=indexliste[ndim]+anz*((int *)variablen[vnr].pointer)[ndim];  

    varptr[anz].len=wert.len;
    varptr[anz].pointer=malloc(wert.len+1);
    memcpy(varptr[anz].pointer,wert.pointer,wert.len);
    return(0);
  } else return(-1);
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
    w[strlen(name)-1]=0;   /* $-Zeichen entfernen */
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

double varfloatarrayinhalt(int vnr, int *indexliste) {
  int ndim,anz=0;
  if(vnr>-1) {
    double *varptr=(double  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
    for(ndim=0;ndim<variablen[vnr].opcode;ndim++) 
      anz=indexliste[ndim]+anz*((int *)variablen[vnr].pointer)[ndim];
    return(varptr[anz]);
  } else return(0);  
}

int intarrayinhalt2(ARRAY *a, int *indexliste) {
  int ndim,anz=0;
  int *varptr=(int *)(a->pointer+a->dimension*INTSIZE);
  for(ndim=0;ndim<a->dimension;ndim++) anz=indexliste[ndim]+anz*((int *)a->pointer)[ndim];
  return(varptr[anz]);
}
int varintarrayinhalt(int vnr, int *indexliste) {
  int ndim,anz=0;
  if(vnr>-1) {
    int *varptr=(int  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);
    for(ndim=0;ndim<variablen[vnr].opcode;ndim++) 
      anz=indexliste[ndim]+anz*((int *)variablen[vnr].pointer)[ndim];
    return(varptr[anz]);
  } else return(0);  
}
STRING varstringarrayinhalt(int vnr, int *indexliste) {
  STRING ergeb;
  int dim=variablen[vnr].opcode;
  int *bbb=(int *)variablen[vnr].pointer;
  int i,anz=0;
  STRING *varptr;
  for(i=0;i<dim;i++){
    if(indexliste[i]>=bbb[i]) {error(16,""); /* Feldindex zu gross*/ break;}
    else anz=indexliste[i]+anz*bbb[i];
  }
  varptr=(STRING *)(variablen[vnr].pointer+dim*INTSIZE);
  ergeb.pointer=malloc(varptr[anz].len+1);
  ergeb.len=varptr[anz].len;
  memcpy(ergeb.pointer,varptr[anz].pointer,ergeb.len);
  return(ergeb);
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


/************************* CASTS *******************************/

void varcastint(int typ,void *pointer,int val) {
    if(typ&FLOATTYP) *((double *)pointer)=(double)val;
    else if(typ&INTTYP) *((int *)pointer)=val;
}
void varcastfloat(int typ,void *pointer,double val) {
    if(typ&FLOATTYP) *((double *)pointer)=val;
    else if(typ&INTTYP) *((int *)pointer)=(int)val;
}




