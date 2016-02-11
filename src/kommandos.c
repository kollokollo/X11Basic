/* kommandos.c  Sachen fuer die Kommandobearbeitung (c) Markus Hoffmann */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xbasic.h"
#include "protos.h"



/*****************************************/
/* Kommandos zur Programmablaufkontrolle */

void c_gosub(char *n) {
    char *buffer,*pos,*pos2,*pos3;
    int pc2;
    buffer=indirekt2(n);
    pos=searchchr(buffer,'(');
    if(pos!=NULL) {
      pos[0]=0;pos++;
      pos2=pos+strlen(pos)-1;
      if(pos2[0]!=')') printf("Syntax error bei Parameterliste\n");
      else pos2[0]=0;
    } else pos=buffer+strlen(buffer);
    
    pc2=procnr(buffer,1);
    if(pc2==-1)   error(19,buffer); /* Procedure nicht gefunden */
    else {       
	if(do_parameterliste(pos,procs[pc2].parameterliste)) error(42,buffer); /* Zu wenig Parameter */
	else { batch=1;
	  pc2=procs[pc2].zeile;
	  if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
	  else {printf("Stack-Overflow ! PC=%d\n",pc); batch=0;}
	}
    }
    free(buffer);
}
      

void c_local(char *n) {
  /* wenn globalere Variablen mit variablen.local < sp existiert,
     dann lege gleichnamige neu an mit aktuellem sp*/ 
  char *v,*w;
  int p;
  v=malloc(strlen(n)+1);w=malloc(strlen(n)+1);
  strcpy(v,n);
  p=wort_sep(v,',',TRUE,w,v);
    while(p) {
     xtrim(w,TRUE,w);
      c_dolocal(w,""); 
     p=wort_sep(v,',',TRUE,w,v); 
    }
  free(v);free(w);
}

void c_goto(char *n) {
    char *b=indirekt2(n);
    pc=labelnr(b);
    if(pc==-1) {error(20,b);/* Label nicht gefunden */ batch=0;}
    else batch=1;
    free(b);
}


void c_system(char *n) {
    char *buffer;
    buffer=s_parser(n);
    system(buffer);
    free(buffer);
}

void c_after(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,w1,w2);  
  int pc2;
  if(e<2) {
    printf("Syntax-Error bei AFTER. <%s>\n",n);
    return;
  }
  pc2=procnr(w2,1);
  if(pc2==-1)   error(19,w2); /* Procedure nicht gefunden */
  else {
    everyflag=0;
    alarmpc=pc2;
    alarm((int)parser(w1));
  }
}

void c_every(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,w1,w2),pc2;
  if(e<2) {
    printf("Syntax-Error bei AFTER. <%s>\n",n);
    return;
  }
  pc2=procnr(w2,1);
  if(pc2==-1)   error(19,w2); /* Procedure nicht gefunden */
  else {
    everyflag=1;
    alarmpc=pc2;
    everytime=(int)parser(w1);
    alarm(everytime);
  }
}


c_dodim(char *w){
 char *pos,*pos2,*v,*s,*t,*u,*ppp;
 
 int *ooo,i,ndim=0,a=1,vnr,typ;
 char **qqq;
 
  v=malloc(strlen(w)+1);
  strcpy(v,w);
 /*  printf("DODIM: %s\n",w); */

  pos=searchchr(v, '(');
  if(pos==NULL) printf("DIM: Syntax Error.\n");
  else {
    pos2=v+strlen(v)-1;
    pos[0]=0;
    pos++;

    if(pos2[0]!=')') printf("DIM: Syntax Error.\n");
    else {
      /* Typ Bestimmen  */
      pos2[0]=0;
      typ=type2(v);
      if(searchchr("$%#", v[strlen(v)-1])!=NULL) v[strlen(v)-1]=0;

      /* Dimensionen bestimmen   */
      s=malloc(strlen(pos)+1);
      t=malloc(strlen(pos)+1);
     
      strcpy(s,pos);
      i=wort_sep(s,',',TRUE,s,t);
      while(i) {
        xtrim(s,TRUE,s);
        a=a*((int)parser(s)+1);
        ndim++; 
        i=wort_sep(t,',',TRUE,s,t); 
      }
     /* printf("DIM: %s Typ:%d %d %d \n",v,typ,ndim,a);  */
      typ=typ|ARRAYTYP;
      vnr=variable_exist(v,typ);
      if(vnr==-1) {
        vnr=anzvariablen;
	variablen[vnr].name=malloc(strlen(v)+1);
	strcpy(variablen[vnr].name,v);
	variablen[vnr].typ=typ;
	variablen[vnr].local=0;    /* Arrays immer global */
        anzvariablen++;
      } else {
        if(typ==STRINGARRAYTYP)  erase_string_array(vnr); 	
	free(variablen[vnr].pointer);
      }
      variablen[vnr].opcode=ndim;
      if(typ==STRINGARRAYTYP)   {
        variablen[vnr].pointer=malloc(ndim*INTSIZE+a*(sizeof(STRING)));
        ppp=(char *)(variablen[vnr].pointer+ndim*INTSIZE);
        for(i=0;i<a;i++) {
	  ((int *)ppp)[0]=0;   /* Laenge */
	  ppp+=sizeof(int);
	  qqq=(char **)ppp;
	  qqq[0]=malloc(1);
	  ppp+=sizeof(char *);
	}
      } else if(typ==INTARRAYTYP) variablen[vnr].pointer=malloc(ndim*INTSIZE+a*sizeof(int));
      else variablen[vnr].pointer=malloc(ndim*INTSIZE+a*sizeof(double)); 

    /*  printf("----DIM? %d %d \n",ndim,a);*/ 
      strcpy(s,pos);
      ndim=0;
      i=wort_sep(s,',',TRUE,s,t);
      while(i) {
        xtrim(s,TRUE,s);
        ((int *)variablen[vnr].pointer)[ndim]=(int)parser(s)+1;
        ndim++; 
        i=wort_sep(t,',',TRUE,s,t); 
      }
      
      free(s);free(t);
    }
  }
  free(v);
}

void c_run(char *n) {        /* Programmausfuehrung starten und bei 0 beginnen */
  pc=sp=0;
  erase_all_variables();
  batch=1;
  do_restore();
}

void c_chain(char *n){ c_load(n); c_run(n);    }

void c_cont(char *n) {
  if(pc<=prglen) batch=1;
  else error(41,"");     /*CONT nicht moeglich !*/
}

void c_restore(char *n) {
  char *b;
  b=indirekt2(n);
  if(strlen(b)) {
    datazeile=labelnr(b);
    if(datazeile==-1) error(20,b);/* Label nicht gefunden */ 
    else next_data_line();
  } else do_restore();
  free(b);
}

void do_restore() {
  datazeile=0;
  next_data_line();
}
void next_data_line() {
  datazeile=suche(datazeile+1,1,"DATA","BEFEHLGIBTESNICHT","BEFEHLGIBTESNICHT2");
  if(datazeile>=0) {
    int i=0;
    while(!isalnum((program[datazeile])[i])) i++;
    
    strcpy(databuffer,program[datazeile]+i+5);
    
  } else databuffer[0]=0;
}
char *get_next_data_entry() {
  int e;
  char *ergebnis=NULL;
  char t[MAXSTRLEN];
  e=wort_sep(databuffer,',',FALSE,t,databuffer);

   if(e==0) {
     next_data_line();
     e=wort_sep(databuffer,',',FALSE,t,databuffer);
   }
   if(e) {
     ergebnis=malloc(strlen(t)+1);
     strcpy(ergebnis,t);
     return(ergebnis);
   } else return(NULL);
}

void c_read(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int e;
  char *t,*u;
  e=wort_sep(n,',',TRUE,w1,w2);
  while(e>0) {
    t=get_next_data_entry();
    if(t!=NULL) {
      if(type2(w1) & STRINGTYP) {
        if(t[0]=='\"') u=s_parser(t);
        else {
          u=malloc(strlen(t)+1);
	  strcpy(u,t);
        }
        zuweiss(w1,u);
	free(u);
      } else zuweis(w1,parser(t));
      free(t);
    } else error(34,""); /* Zu wenig Data */
    e=wort_sep(w2,',',TRUE,w1,w2);
  }
}

void c_case(char *n) {  /* case und default */
  /*gehe zum naechsten ENDSELECT*/
    pc=suchep(pc,1,P_ENDSELECT,P_SELECT,P_ENDSELECT);
    if(pc==-1) error(36,"CASE"); /*Programmstruktur fehlerhaft !*/ 
    pc++;
}

void c_list(char *n) {
  int i;
  for(i=0;i<prglen;i++) printf("%s\n",program[i]);
}

char *plist_zeile(P_CODE *code) {
  char *ergebnis=malloc(MAXLINELEN);
  int i;
  ergebnis[0]=0;
  if(code->opcode&P_INVALID) strcat(ergebnis,"==> ");
  if(code->opcode==P_LABEL) {
    strcat(ergebnis,labels[code->integer].name);
    strcat(ergebnis,":");
  } else if(code->opcode==P_REM) {
    strcat(ergebnis,"' ");
    strcat(ergebnis,code->argument);
  } else if((code->opcode&PM_COMMS)<anzcomms) {
    strcat(ergebnis,comms[(code->opcode&PM_COMMS)].name);
    if(code->panzahl) {
      strcat(ergebnis," ");
      for(i=0;i<code->panzahl;i++) {
        strcat(ergebnis,code->ppointer[i].pointer);
	strcat(ergebnis,",");
      }
      ergebnis[strlen(ergebnis)-1]=0;
    }
  } else if((code->opcode)&P_EVAL) strcpy(ergebnis,code->argument);
  else strcpy(ergebnis,"???");
  
  return(ergebnis);
}
int plist_printzeile(P_CODE *code, int level) {
  int j;
  char *zeile=plist_zeile(code);
  if(code->opcode & P_LEVELOUT) level--;
  for(j=0;j<level;j++) printf("  ");
  printf("%s\n",zeile);
  if(code->opcode & P_LEVELIN) level++;
  free(zeile);
  return(level);
}

void c_plist(char *n) {
  int i,f=0;
  for(i=0;i<prglen;i++) { 
    printf("%4d: $%06x |",i,pcode[i]);
  #if DEBUG
    printf(" (%5d) |",ptimes[i]);
  #endif
    printf(" %3d,%d |",pcode[i].integer,pcode[i].panzahl);
    f=plist_printzeile(&pcode[i],f);
  }
}

void c_load(char *n) { programbufferlen=0;  c_merge(n); }

void c_merge(char *n){
  char *name=s_parser(n);
  if(exist(name)) {
    &name; /* Scheint ein Bug zu sein ... */
    mergeprg(name);
  } else printf("LOAD/MERGE: Datei %s nicht gefunden !\n",name);
  free(name);
}

void c_new(char *n) {
  erase_all_variables();
  batch=0;
}
void c_let(char *n) {  
    char v[strlen(n)+1],w[strlen(n)+1];
    wort_sep(n,'=',TRUE,v,w);
    xzuweis(v,w);
}


void c_quit(char *n) { 
  close_window(usewindow); 
  free(programbuffer); 
#ifdef CONTROL
  cs_exit();
#endif
  exit(0); 
}

void c_fft(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  int isign=1;
  int e=wort_sep(n,',',TRUE,v,w);
  if(e>=1) {
    int typ,vnr;
    char *r;
     /* Typ bestimmem. Ist es Array ? */
 
    typ=type2(v);
    if(typ & ARRAYTYP) {
      r=varrumpf(v);
      vnr=variable_exist(r,typ);
      free(r);
      if(vnr==-1) error(15,v); /* Feld nicht dimensioniert */ 
      else {
        if(typ & FLOATTYP) {
	  int nn=do_dimension(vnr);    
	  double *varptr=(double  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);

	  if(e==2) isign=(int)parser(w);
	  realft(varptr,(nn-1)/2,isign);
        } else printf("FFT: Muss Float-ARRAY sein. \n");
      }
    } else printf("FFT: Kein ARRAY. \n");
  
  } else error(32,"FFT"); /* Syntax error */
}


void c_arraycopy(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  
  if(wort_sep(n,',',TRUE,v,w)==2) {
     int typ1,typ2,vnr1,vnr2;
     char *r1,*r2;
     xtrim(v,TRUE,v);
     xtrim(w,TRUE,w);
     /* Typ bestimmem. Ist es Array ? */
 
     typ1=type2(v);
     typ2=type2(w);
     
    if((typ1 & ARRAYTYP) && (typ2 & ARRAYTYP)) {
      r1=varrumpf(v);
      r2=varrumpf(w);
      
      vnr1=variable_exist(r1,typ1);
      vnr2=variable_exist(r2,typ2);
      
      free(r1);free(r2);
      
      if(vnr1==-1 || vnr2==-1) error(15,""); /* Feld nicht dimensioniert */ 
      else {
        if((typ1 & STRINGTYP) && (typ2 & STRINGTYP)) copy_string_array(vnr1,vnr2); 
        else if((typ1 & INTTYP) && (typ2 & INTTYP))  copy_int_array(vnr1,vnr2); 
        else if((typ1 & FLOATTYP) && (typ2 & FLOATTYP))
	  copy_float_array(vnr1,vnr2);
	else if((typ1 & FLOATTYP) && (typ2 & INTTYP))
	  convert_int_to_float_array(vnr1,vnr2);
	else if((typ1 & INTTYP) && (typ2 & FLOATTYP))
	  convert_float_to_int_array(vnr1,vnr2);
	else printf("ARRAYCOPY: Typen inkompatibel. \n");
      }
    } else printf("ARRAYCOPY: Kein ARRAY. \n");
  } else printf("ARRAYCOPY: Syntax error. \n");

}
void c_arrayfill(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  
  if(wort_sep(n,',',TRUE,v,w)==2) {
     int typ,vnr;
     char *r;
     
     xtrim(v,TRUE,v);
     xtrim(w,TRUE,w);
     /* Typ bestimmem. Ist es Array ? */
 
     typ=type2(v);
    if(typ & ARRAYTYP) {
      r=varrumpf(v);
      vnr=variable_exist(r,typ);
      free(r);
      if(vnr==-1) error(15,v); /* Feld nicht dimensioniert */ 
      else {
        if(typ & STRINGTYP) {
	  STRING str=string_parser(w);
	  fill_string_array(vnr,str);
	  free(str.pointer); 
        } else if(typ & INTTYP) fill_int_array(vnr,(int)parser(w)); 
        else if(typ & FLOATTYP) fill_float_array(vnr,parser(w));
      }
    } else printf("ARRAYFILL: Kein ARRAY. \n");
  } else printf("ARRAYFILL: Syntax error. \n");
}


void c_dump(char *n) {
  int i;
  char kkk=0;
  
  if(strlen(n)) {
    char *kom=s_parser(n);
    kkk=kom[0];
    free(kom);
  }
  

  if(kkk==0 || kkk=='%') {
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==INTTYP) printf("%s%%=%d   [%d]\n",variablen[i].name, 
        variablen[i].opcode,variablen[i].local);
    }
  }
  if(kkk==0) {
    for(i=0;i<anzvariablen;i++) {if(variablen[i].typ==FLOATTYP) printf("%s=%.13g    [%d]\n",variablen[i].name,
         variablen[i].zahl, variablen[i].local);}
  }
  if(kkk==0 || kkk=='$') {
    for(i=0;i<anzvariablen;i++) {if(variablen[i].typ==STRINGTYP) 
    printf("%s$=\"%s\"   [%d]\n",variablen[i].name,variablen[i].pointer,variablen[i].local);}
  }
  if(kkk==0 || kkk=='%' || kkk=='(') {
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==INTARRAYTYP) {
        int j;
        printf("%s%%(",variablen[i].name);
        for(j=0;j<variablen[i].opcode;j++) {
          if(j>0) printf(",%d",((int *)variablen[i].pointer)[j]-1);
	  else  printf("%d",((int *)variablen[i].pointer)[j]-1);
        }
        printf(")\n");
      }
    }
  }
  
  if(kkk==0 || kkk=='(') {
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==FLOATARRAYTYP) {
        int j;
        printf("%s(",variablen[i].name);
        for(j=0;j<variablen[i].opcode;j++) {
          if(j>0) printf(",%d",((int *)variablen[i].pointer)[j]-1);
  	  else  printf("%d",((int *)variablen[i].pointer)[j]-1);
        }
        printf(")  [%d]\n",variablen[i].local);
      }
    }
  }
  if(kkk==0 || kkk=='$' || kkk=='(') {
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==STRINGARRAYTYP) {
        int j;
        printf("%s$(",variablen[i].name);
        for(j=0;j<variablen[i].opcode;j++) {
          if(j>0) printf(",%d",((int *)variablen[i].pointer)[j]-1);
  	  else  printf("%d",((int *)variablen[i].pointer)[j]-1);
        }
        printf(")\n");
      }
    }
  }
  if(kkk==':') {
    for(i=0;i<anzlabels;i++) {
      printf("%s: [%d]\n",labels[i].name,labels[i].zeile);
    }
  }
  if(kkk=='@') {
    for(i=0;i<anzprocs;i++) {
      printf("%d|%s [%d]\n",procs[i].typ,procs[i].name,procs[i].zeile);
    }
  }
  if(kkk=='#') {
    for(i=0;i<100;i++) {
      if(filenr[i]==1) {
        printf("#%d: %s [FILE]\n",i,"");
      } else if(filenr[i]==2) {
        printf("#%d: %s [SHARED OBJECT]\n",i,"");
      }
    }
  }
  
}

void c_end(char *n) {
  batch=0; 
}

void c_on(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1],w3[strlen(n)+1];
  int e=wort_sep(n,' ',TRUE,w1,w2);
  int mode=0;
  if(e==0) error(32,"ON"); /* Syntax error */
  else {
    wort_sep(w2,' ',TRUE,w2,w3);
    if(strcmp(w2,"CONT")==0) mode=1;
    else if(strcmp(w2,"GOTO")==0) mode=2;
    else if(strcmp(w2,"GOSUB")==0) mode=3;
    else mode=0;
    
    if(strcmp(w1,"ERROR")==0) {
      errcont=(mode==1);
      
    } else if(strcmp(w1,"BREAK")==0) {
      breakcont=(mode==1);
    } else if(strcmp(w1,"MENU")==0) {
      if(mode==0)  c_menu("");  
      else if(mode==3) {
       int pc2=procnr(w3,1);
       if(pc2==-1) error(19,w3); /* Procedure nicht gefunden */
       else menuaction=pc2;
      } else  printf("Unbekannter Befehl: ON <%s> <%s>\n",w1,w2);  
    } else { /* on n goto ...  */
      if(mode<2) printf("Unbekannter Befehl: ON <%s> <%s>\n",w1,w2);
      else {
        int i,gi=max(0,(int)parser(w1));
	if(gi) {
	  while(gi) { e=wort_sep(w3,',',TRUE,w2,w3); gi--;}
	  if(e) {
            if(mode==3) c_gosub(w2);
	    else if(mode==2) c_goto(w2);
          }
	}
      }
    }
  }
}

void c_add(char *n) {c_addsubmuldiv(n,1);}
void c_sub(char *n) {c_addsubmuldiv(n,2);}
void c_mul(char *n) {c_addsubmuldiv(n,3);}
void c_div(char *n) {c_addsubmuldiv(n,4);}

void c_addsubmuldiv(char *n,int z) {
  char var[strlen(n)+1],t[strlen(n)+1];
  int e;
  
  e=wort_sep(n,',',TRUE,var,t);
  if(e<2) error(56,"");  /* Zuwenig Parameter ! */
  else {
    if(z==1)      zuweis(var,parser(var)+parser(t));
    else if(z==2) zuweis(var,parser(var)-parser(t));
    else if(z==3) zuweis(var,parser(var)*parser(t));
    else if(z==4) zuweis(var,parser(var)/parser(t));
    else printf("Unbekannter Befehl !\n");
  }
}

void c_swap(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  int e,vnr1,vnr2;
  e=wort_sep(n,',',TRUE,v,w);
  if(e<2) error(56,"");  /* Zuwenig Parameter ! */
  else {
     char *r1,*r2;
     int typ;
     typ=type2(v);
     if(typ!=type2(w)) printf("Variablen haben nicht den gleichen typ !\n");
     else {
       r1=varrumpf(v);
       r2=varrumpf(w);
       vnr1=variable_exist(r1,typ);
       vnr2=variable_exist(r2,typ);
       if(vnr1==-1 || vnr2==-1) printf("Variable existiert nicht !\n");
       else if(vnr1!=vnr2) {    
           char *zb=variablen[vnr1].name;
	   variablen[vnr1].name=variablen[vnr2].name;
           variablen[vnr2].name=zb;
	   /* evtl noch Plaetze Tauschen, damit sich auch die varnr aendern */
       }
       free(r1);free(r2);
     }
  }
} 

void c_do(char *n) {   /* wird normalerweise ignoriert */
  char w1[strlen(n)+1],w2[strlen(n)+1];
  
  wort_sep(n,' ',TRUE,w1,w2);
  if(strcmp(w1,"")==0) ; 
  else if(strcmp(w1,"WHILE")==0) c_while(w2);
  else if(strcmp(w1,"UNTIL")==0) ;
  else  printf("Unbekannter Befehl: DO <%s> <%s>\n",w1,w2);
}

void c_dim(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  int p=wort_sep(n,',',TRUE,w,v);
  while(p) {
     xtrim(w,TRUE,w);
     c_dodim(w);
     p=wort_sep(v,',',TRUE,w,v); 
  }
}
void c_erase(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  int p=wort_sep(n,',',TRUE,w,v);
  while(p) {
     xtrim(w,TRUE,w);
     c_doerase(w);
     p=wort_sep(v,',',TRUE,w,v); 
  }
}

c_doerase(char *w){
 char *pos,*pos2,*v,*u,*ppp;
 
 int *ooo,i,vnr,typ;
 char **qqq;
 
  v=malloc(strlen(w)+1);
  strcpy(v,w); 
 
 
  pos=searchchr(v, '(');
  if(pos==NULL) printf("ERASE: Syntax Error.\n");
  else {
    pos2=v+strlen(v)-1;
    pos[0]=0;
    pos++;

    if(pos2[0]!=')') printf("ERASE: Syntax Error.\n");
    else {
      /* Typ Bestimmen  */
      pos2[0]=0;
      typ=type2(v);
      if(searchchr("$%#", v[strlen(v)-1])!=NULL) v[strlen(v)-1]=0;
      
      
      typ=typ|ARRAYTYP;
      vnr=variable_exist(v,typ);
      if(vnr==-1) {
	error(15,v); /* Feld nicht dimensioniert */ 
      } else {
        if(typ==STRINGARRAYTYP)  erase_string_array(vnr);
	free(variablen[vnr].pointer);
      
        /* Jetzt noch aus Variablenliste entfernen  */
        
	if(vnr<anzvariablen-1) variablen[vnr]=variablen[anzvariablen-1];
	anzvariablen--;
      }
    }
  }
  free(v);
}


void c_return(char *n) {
  if(sp>0) {
    if(strlen(n)) {
      if(type2(n) & STRINGTYP) {
        returnvalue.str=string_parser(n); /* eigentlich muss auch der Funktionstyp 
                                       	 abgefragt werden */
      } else {
        returnvalue.f=parser(n);
      }
    }
    local_vars_loeschen(sp-1);
    pc=stack[--sp];
  }
  else error(93,""); /*Stack-Error !*/
}

void c_void(char *n) { 
  if(type2(n) & STRINGTYP) {
    char *erg=s_parser(n);
    free(erg);
  } else parser(n);
}
void c_nop(char *n) { return; }

void c_inc(char *n) {
  int typ=type2(n);
  void *v;
  v=varptr(n);
  if(v==NULL) printf("Fehler bei varptr.\n");
  else {
    if(typ&FLOATTYP) *((double *)v)=*((double *)v)+1;
    else if(typ&INTTYP) *((int *)v)++;
    else error(58,n); /* Variable hat falschen Typ */
  }
}

void c_dec(char *n) { 
  int typ=type2(n);
  void *v;
  v=varptr(n);
  if(v==NULL) printf("Fehler bei varptr.\n");
  else {
    if(typ&FLOATTYP) *((double *)v)=*((double *)v)-1;
    else if(typ&INTTYP) *((int *)v)--;
    else error(58,n); /* Variable hat falschen Typ */
  }
}
void c_cls(char *n) { printf("\033[2J\033[H");}
void c_home(char *n) { printf("\033[H");}
void c_version(char *n) { printf("XBASIC Version: %s vom %s \n",version,vdate);}

void c_error(char *w) {
  error((char)parser(w),"");
}
void c_pause(char *w) {
  int dummy,i=0;
  dummy=(int)(1000000*parser(w));
  while(dummy>=1000000) {i++;dummy-=1000000;}
  if(i) sleep(i);
  if(dummy>0) usleep(dummy);
}

void c_echo(char *n) {
  if(strcmp(n,"ON")==0) echo=TRUE; 
  else if(strcmp(n,"OFF")==0) echo=FALSE;
  else  echo=(int)parser(n);
}
void c_stop(char *n) { batch=0;} 
void c_tron(char *n) {  echo=1;}
void c_troff(char *n) { echo=0;}
void c_beep(char *n) { printf("\007");}
 
void c_clear(char *w){  erase_all_variables(); }

void c_clr(char *n){  
 char *v,*w;
  int p;
  v=malloc(strlen(n)+1);w=malloc(strlen(n)+1);
  strcpy(v,n);
  p=wort_sep(v,',',TRUE,w,v);
    while(p) {
     xtrim(w,TRUE,w);
     c_doclr(w);
     p=wort_sep(v,',',TRUE,w,v); 
    }
  free(v);free(w);
}

c_doclr(char *v){
  
 char *r,*pos;
 
 int vnr,typ; 
 
 
 /* Typ bestimmem. Ist es Array ? */
 
  typ=type2(v);
  if(typ & CONSTTYP) {printf("CLR: Syntax error..\n"); return;}
  r=varrumpf(v);
  vnr=variable_exist(r,typ);

  if(typ & ARRAYTYP) { /* ganzes Array  */
    if(vnr==-1) error(15,r); /* Feld nicht dimensioniert */ 
    else {
      if(typ & STRINGTYP) fill_string_array(vnr,""); 
      else if(typ & INTTYP) fill_int_array(vnr,0); 
      else if(typ & FLOATTYP) fill_float_array(vnr,0.0);
    }
  } else {
    int indize=((pos=strchr(v,'('))!=NULL); 
    if(indize) vnr=variable_exist(r,typ | ARRAYTYP);
    if(vnr==-1) {   /* Variable existiert gar nicht */
        if(indize) error(15,r); /* Feld nicht dimensioniert */ 
	else {
	  if(typ & STRINGTYP) neue_string_variable(r,"",0); 
	  else if(typ & INTTYP) neue_int_variable(r,0,0); 
	  else if(typ & FLOATTYP) neue_float_variable(r,0.0,0);
        }
      } else {      /* Variable existiert */
      
        /* Hier wird Feld/indexe abgefangen  */
	if(!indize) {
          if(typ & STRINGTYP) {
	     variablen[vnr].pointer=realloc(variablen[vnr].pointer,1);
	     (variablen[vnr].pointer)[0]=0;
	     variablen[vnr].opcode=1; 
	  } else {
	    variablen[vnr].opcode=0;
	    variablen[vnr].zahl=0.0;
	  } 
	} else {
	  if(typ & STRINGTYP) zuweiss(v,""); 
	  else zuweis(v,0.0);
	}
      } 	
  }
  free(r);
}

void c_exit(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  
  wort_sep(n,' ',TRUE,w1,w2);
  if(strcmp(w1,"")==0) c_return; 
  else if(strcmp(w1,"IF")==0) {
    if(parser(w2)==-1) c_break("");
  } else  printf("Unbekannter Befehl: EXIT <%s> <%s>\n",w1,w2);
}

void c_break(char *n) {
  int i,f=0,o;
  for(i=pc; (i<prglen && i>=0);i++) {
    o=pcode[i].opcode&PM_SPECIAL;
    if((o==P_LOOP || o==P_NEXT || o==P_WEND ||  o==P_UNTIL)  && f<=0) break;
    if(o & P_LEVELIN) f++;
    if(o & P_LEVELOUT) f--;
   }
  if(i==prglen) { error(36,"BREAK/EXIT IF"); /*Programmstruktur fehlerhaft */return;}
  pc=i+1;
}

void c_if(char *n) {
  char w1[MAXSTRLEN],w2[MAXSTRLEN],w3[MAXSTRLEN];
  
  if((int)parser(n)==0) { 
    int i,f=0,o;
  
    for(i=pc; (i<prglen && i>=0);i++) {
      o=pcode[i].opcode&PM_SPECIAL;
      if((o==P_ENDIF || o==P_ELSE)  && f==0) break;
      else if(o==P_IF) f++;
      else if(o==P_ENDIF) f--;
    }
    
    if(i==prglen) { error(36,"IF"); /*Programmstruktur fehlerhaft */return;}
    pc=i+1;
    if((pcode[i].opcode&PM_SPECIAL)==P_ELSE) {
      xtrim(program[i],TRUE,w1);
      wort_sep(w1,' ',TRUE,w2,w3);
      wort_sep(w3,' ',TRUE,w1,w2);
      if(strcmp(w1,"IF")==0) c_if(w2); 
    } 
  }
}

void c_select(char *n) {
  int i,wert,f=0,o;
  char w1[MAXSTRLEN],w2[MAXSTRLEN],w3[MAXSTRLEN];
  
  wert=(int)parser(n);
  
  /* Case-Anweisungen finden */
  
  for(i=pc; (i<prglen && i>=0);i++) {
    o=pcode[i].opcode&PM_SPECIAL;
    if((o==P_DEFAULT || o==P_CASE)  && f==0) break;
    else if(o==P_SELECT) f++;
    else if(o==P_ENDSELECT) f--;
  }
    
   if(i==prglen) { error(36,"SELECT"); /*Programmstruktur fehlerhaft */return;}
   pc=i;
   xtrim(program[pc],TRUE,w1);
   wort_sep(w1,' ',TRUE,w2,w3);
   pc++;
   if((pcode[i].opcode&PM_SPECIAL)==P_ELSE){
     wort_sep(w3,' ',TRUE,w1,w2);
     if(strcmp(w1,"IF")==0) c_if(w2);
   }
  
}

void bidnm(char *n) {
  error(38,n); /* Befehl im Direktmodus nicht moeglich */
}

void c_next(char *n) {
  char w1[MAXSTRLEN],w2[MAXSTRLEN],w3[MAXSTRLEN],var[MAXSTRLEN];
  double step, limit;
  int ss,e,f=0,hpc=pc;

   pc=pcode[pc-1].integer; 
   if(pc==-1) {error(36,"NEXT"); /*Programmstruktur fehlerhaft */return;}

   strcpy(w1,pcode[pc].ppointer->pointer);
   wort_sep(w1,' ',TRUE,w2,w3);
 
   /* Variable bestimmem */
   if(searchchr(w2,'=')!=NULL) {
    wort_sep(w2,'=',TRUE,var,w1);
   } else {printf("Syntax Error ! FOR %s\n",w2); batch=0;return;}
   wort_sep(w3,' ',TRUE,w1,w2);
   
   if(strcmp(w1,"TO")==0) ss=1; 
   else if(strcmp(w1,"DOWNTO")==0) ss=-1; 
   else {printf("Syntax Error ! FOR %s\n",w1); batch=0;return;}

   /* Limit bestimmem  */
   e=wort_sep(w2,' ',TRUE,w1,w3);
   if(e==0) {printf("Syntax Error ! FOR %s\n",w2); batch=0;return;}
   else {
     limit=parser(w1);
     if(e==1) step=(double)ss;
     else {
       /* Step-Anweisung auswerten  */
       wort_sep(w3,' ',TRUE,w1,w2);
       if(strcmp(w1,"STEP")==0)
         step=parser(w2);
       else {printf("Syntax Error ! FOR %s\n",w1); batch=0;return;}
     }
   }
   zuweis(var,parser(var)+step);
   ss=(int)((step>0)-(step<0));
   /* Schleifenende ueberpruefen    */
   if(ss>0) {
     if(parser(var)>limit) f=TRUE;
   } else {
     if(parser(var)<limit) f=TRUE;
   } 
   if(f)  pc=hpc;          /* Schleifenende, gehe hinter NEXT */
   else pc++;
}
void c_for(char *n) {
  /* erledigt nur die erste Zuweisung  */
  char w1[strlen(n)+1],w2[strlen(n)+1],w3[strlen(n)+1];
  
  wort_sep(n,' ',TRUE,w1,w2);
  if(searchchr(w1,'=')!=NULL) {
    wort_sep(w1,'=',TRUE,w2,w3);
    zuweis(w2,parser(w3));
  } else {printf("Syntax Error ! FOR %s\n",n); batch=0;}
}
void c_until(char *n) {
  if(parser(n)==0) {
    int npc=pcode[pc-1].integer;
    if(npc==-1) error(36,"UNTIL"); /*Programmstruktur fehlerhaft */
    else pc=npc+1;
  }
}

void c_while(char *n) {
  if(parser(n)==0) {
    int npc=pcode[pc-1].integer;
    if(npc==-1) error(36,"WHILE"); /*Programmstruktur fehlerhaft */
    pc=npc;
  } 
}


void c_wort_sep(char *n) {
  do_wort_sep(n);
}

int do_wort_sep(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,w1,w2),ergeb=-1;
  if(e==2) {
    char *quelle=s_parser(w1);
    e=wort_sep(w2,',',TRUE,w1,w2);
    if(e==2) {
      char *deliminator=s_parser(w1);
      e=wort_sep(w2,',',TRUE,w1,w2);
      if(e==2) {
        int flag=(int)parser(w1);
	e=wort_sep(w2,',',TRUE,w1,w2);
        if(e==2) {
	  char *var1=indirekt2(w1);
	  char *var2=indirekt2(w2);
	  char *buffer1=malloc(strlen(quelle)+1);
	  char *buffer2=malloc(strlen(quelle)+1);
	  ergeb=wort_sep2(quelle,deliminator,flag,buffer1,buffer2);
	  zuweiss(var1,buffer1);
	  free(buffer1); free(var1);
	  zuweiss(var2,buffer2);
	  free(buffer2); free(var2);
        } 
      } 
      free(deliminator);
    } 
    free(quelle);
  } 
  if(ergeb==-1) {
    error(32,"WORT_SEP"); /*Syntax Error */
    return(0);
  } else return(ergeb);
}
void c_poke(char *n) {
  char v[strlen(n)+1],t[strlen(n)+1];
  int e;
  char *adr;
  e=wort_sep(n,',',TRUE,v,t);
  if(e<2) printf("Zuwenig Parameter !\n");
  else {
    adr=(char *)((int)parser(v));
    *adr=(char)parser(t);
  }
}
void c_dpoke(char *n) {
  char v[strlen(n)+1],t[strlen(n)+1];
  int e;
  short *adr;
  e=wort_sep(n,',',TRUE,v,t);
  if(e<2) printf("Zuwenig Parameter !\n");
  else {
    adr=(short *)((int)parser(v));
    *adr=(short)parser(t);
  }
}
void c_lpoke(char *n) {
  char v[strlen(n)+1],t[strlen(n)+1];
  int e;
  long *adr;
  e=wort_sep(n,',',TRUE,v,t);
  if(e<2) printf("Zuwenig Parameter !\n");
  else {
    adr=(long *)((int)parser(v));
    *adr=(long)parser(t);
  }
}
