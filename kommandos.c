/* kommandos.c  Sachen fuer die Kommandobearbeitung (c) Markus Hoffmann */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#ifdef WINDOWS
#define EX_OK 0
#else
#include <sysexits.h>
#endif
#include <ctype.h>
#include "defs.h"
#include <time.h>
#include <errno.h>
#include "ptypes.h"
#include "vtypes.h"
#include "globals.h"
#include "protos.h"
#include "options.h"
#include "kommandos.h"
#include "array.h"

#ifdef WINDOWS
#include <windows.h>
#include <string.h>
#include <io.h>
#endif


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
      if(pos2[0]!=')') {
        puts("Syntax error bei Parameterliste");
        xberror(39,buffer); /* Program Error Gosub impossible */
      }	
      else pos2[0]=0;
    } else pos=buffer+strlen(buffer);
    
    pc2=procnr(buffer,1);
    if(pc2==-1)   xberror(19,buffer); /* Procedure nicht gefunden */
    else {       
	if(do_parameterliste(pos,procs[pc2].parameterliste)) xberror(42,buffer); /* Zu wenig Parameter */
	else { batch=1;
	  pc2=procs[pc2].zeile;
	  if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
	  else {
	    printf("Stack-Overflow ! PC=%d\n",pc); 
	    xberror(39,buffer); /* Program Error Gosub impossible */
	  }
	}
    }
    free(buffer);
}
/* Spawn soll eine Routine als separaten thread ausfuehren.
   Derzeit klappt as nur als separater Task. Das bedeutet, dass 
   die beiden Programmteile nicht ueber die Variablen reden koennen.
   Hierzu muesste man die XBASIC-Variablen in Shared-Memory auslagern.
   das waere aehnlich wie EXPORT ....
   */
void c_spawn(char *n) {
  char *buffer,*pos,*pos2,*pos3;
  int pc2;
  
  buffer=indirekt2(n);
  pos=searchchr(buffer,'(');
  if(pos!=NULL) {
    pos[0]=0;pos++;
    pos2=pos+strlen(pos)-1;
    if(pos2[0]!=')') {
      puts("Syntax error bei Parameterliste");
      xberror(39,buffer); /* Program Error Gosub impossible */
    } 
    else pos2[0]=0;
  } else pos=buffer+strlen(buffer);
  
  pc2=procnr(buffer,1);
  if(pc2==-1)	xberror(19,buffer); /* Procedure nicht gefunden */
  else {
    #ifndef WINDOWS
    pid_t forkret=fork();
    if(forkret==-1) io_error(errno,"SPAWN");
    if(forkret==0) {
      if(do_parameterliste(pos,procs[pc2].parameterliste)) xberror(42,buffer); /* Zu wenig Parameter */
      else { 
        batch=1;
        pc2=procs[pc2].zeile;
        if(sp<STACKSIZE) {stack[sp++]=pc;pc=pc2+1;}
        else {
          printf("Stack-Overflow ! PC=%d\n",pc); 
          xberror(39,buffer); /* Program Error Gosub impossible */
        }      
        programmlauf();
        exit(EX_OK);
      }
    }
    #endif
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
  pc=labelzeile(b);
  if(pc==-1) {xberror(20,b);/* Label nicht gefunden */ batch=0;}
  else batch=1;
  free(b);
}


void c_system(PARAMETER *plist,int e) {
    system(plist[0].pointer);
}
void c_edit(char *n) {
    char filename[strlen(ifilename)+8];
    char buffer2[strlen(ifilename)+1];
    char buffer[200];
    
    strcpy(buffer2,ifilename);
    sprintf(filename,"%s.~~~",ifilename);
    saveprg(filename);
    sprintf(buffer,"$EDITOR %s",filename); system(buffer);
    c_new("");  
    strcpy(ifilename,buffer2);
    mergeprg(filename);
    sprintf(buffer,"rm -f %s",filename); system(buffer);
}

void c_after(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1];
  int e=wort_sep(n,',',TRUE,w1,w2);  
  int pc2;
  if(e<2) {
    xberror(42,"AFTER"); /* Zu wenig Parameter  */
    return;
  }
  pc2=procnr(w2,1);
  if(pc2==-1)   xberror(19,w2); /* Procedure nicht gefunden */
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
    xberror(42,"EVERY"); /* Zu wenig Parameter  */
    return;
  }
  pc2=procnr(w2,1);
  if(pc2==-1)   xberror(19,w2); /* Procedure nicht gefunden */
  else {
    everyflag=1;
    alarmpc=pc2;
    everytime=(int)parser(w1);
    alarm(everytime);
  }
}


c_dodim(char *w) {
  char *pos,*pos2,*v,*s,*t,*u,*ppp;
 
  int *ooo,i,ndim=0,a=1,vnr,typ;
  char **qqq;
 
  v=malloc(strlen(w)+1);
  strcpy(v,w);
 /*  printf("DODIM: %s\n",w); */

  pos=searchchr(v, '(');
  if(pos==NULL) xberror(32,"DIM"); /* Syntax nicht Korrekt */
  else {
    pos2=v+strlen(v)-1;
    pos[0]=0;
    pos++;

    if(pos2[0]!=')') xberror(32,"DIM"); /* Syntax nicht Korrekt */
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
  do_restore(0);
}

void c_chain(PARAMETER *plist,int e){ c_load(plist,e); c_run(""); }

void c_cont(char *n) {
  if(pc<=prglen) batch=1;
  else xberror(41,"");     /*CONT nicht moeglich !*/
}

void c_restore(PARAMETER *plist,int e) {
  if(e) {
    do_restore((int)labels[plist[0].integer].datapointer);
  //  printf("RESTORE: %d %s\n",plist[0].integer,labels[plist[0].integer].name);
  } else do_restore(0);
}

void do_restore(int offset) {
  datapointer=offset;
 // printf("DO RESTORE %d\n",offset);
}
char *get_next_data_entry() {
  char *ptr,*ptr2;
  char *ergebnis=NULL;
  if(databufferlen==0 || databuffer==NULL || datapointer>=databufferlen) return(NULL);
  ptr=databuffer+datapointer;
  ptr2=searchchr(ptr,',');
 // printf("Inhalt: k2=%d <%s>\n",ptr2-ptr,ptr);
  if(ptr2==NULL) {
    ergebnis=malloc(databufferlen-datapointer+1);
    strncpy(ergebnis,ptr,databufferlen-datapointer);
    ergebnis[databufferlen-datapointer]=0;
    datapointer=databufferlen;
  } else {
    ergebnis=malloc(ptr2-ptr+1);
    strncpy(ergebnis,ptr,(int)(ptr2-ptr));
    datapointer+=(ptr2-ptr)+1;
    ergebnis[ptr2-ptr]=0;
  } 
//  printf("READ: <%s>\n",ergebnis);
  return(ergebnis);
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
    } else xberror(34,""); /* Zu wenig Data */
    e=wort_sep(w2,',',TRUE,w1,w2);
  }
}

void c_case(char *n) {  /* case und default */
  /*gehe zum naechsten ENDSELECT*/
    pc=suchep(pc,1,P_ENDSELECT,P_SELECT,P_ENDSELECT);
    if(pc==-1) xberror(36,"CASE"); /*Programmstruktur fehlerhaft !*/ 
    pc++;
}

void c_randomize(PARAMETER *plist, int e) {
  unsigned int seed;
  if(e) seed=plist[0].integer;
  else {
    seed=time(NULL);
    if(seed==-1) io_error(errno,"RANDOMIZE");
  }
  srand(seed);
}

void c_list(PARAMETER *plist, int e) {
  int i,a=0,o=prglen;
  if(e==2) {
    a=min(max(plist[0].integer,0),prglen);
    o=max(min(plist[1].integer+1,prglen),0);
  } else if(e==1) {
    a=min(max(plist[0].integer,0),prglen);
    o=a+1;
  }
  for(i=a;i<o;i++) printf("%s\n",program[i]);
}

char *plist_zeile(P_CODE *code) {
  char *ergebnis=malloc(MAXLINELEN);
  int i;
  ergebnis[0]=0;
  if(code->opcode&P_INVALID) strcat(ergebnis,"==> ");
  if(code->opcode==P_REM) {
    strcat(ergebnis,"' ");
    strcat(ergebnis,code->argument);
    return(ergebnis);
  }
  if(code->opcode==(P_IGNORE|P_NOCMD)) return(ergebnis);
  
  if(code->opcode==P_LABEL) {
    strcat(ergebnis,labels[code->integer].name);
    strcat(ergebnis,":");
  } else if((code->opcode)&P_EVAL) {
    strcat(ergebnis,"eval ---> ");
 
    strcat(ergebnis,code->argument);
  }  
  else if((code->opcode&PM_COMMS)<anzcomms) {
    strcat(ergebnis,comms[(code->opcode&PM_COMMS)].name);
    if(code->panzahl) {
      strcat(ergebnis," ");
      for(i=0;i<code->panzahl;i++) {
        strcat(ergebnis,code->ppointer[i].pointer);
	strcat(ergebnis,",");
      }
      ergebnis[strlen(ergebnis)-1]=0;
    }
  } else sprintf(ergebnis,"=?=> %d",code->opcode);
  if(code->etyp==PE_COMMENT) {
    strcat(ergebnis," !");
    strcat(ergebnis,(char *)code->extra);
  }
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

void c_load(PARAMETER *plist, int e) { 
  programbufferlen=prglen=pc=sp=0;
  c_merge(plist,e); 
}
void c_save(PARAMETER *plist, int e) { 
  if(programbufferlen) {
    char *name;
    if(e) name=plist[0].pointer;
    else name=ifilename;
    if(strlen(name)==0) name=ifilename;
    if(exist(name)) {
      char buf[100];
      sprintf(buf,"mv %s %s.bak",name,name);
      system(buf);
    }
    saveprg(name);
  }
}

void c_merge(PARAMETER *plist, int e){
  if(exist(plist[0].pointer)) {
    if(programbufferlen==0) strcpy(ifilename,plist[0].pointer);
    mergeprg(plist[0].pointer);
  } else printf("LOAD/MERGE: Datei %s nicht gefunden !\n",plist[0].pointer);
}

void c_new(char *n) {
  erase_all_variables();
  batch=0;
  programbufferlen=prglen=pc=sp=0;
  strcpy(ifilename,"new.bas");
}
void c_let(char *n) {  
    char v[strlen(n)+1],w[strlen(n)+1];
    wort_sep(n,'=',TRUE,v,w);
    xzuweis(v,w);
}


void c_quit(PARAMETER *plist, int e) { 
  int ecode=0;
  if(e) ecode=plist[0].integer; 
#ifndef NOGRAPHICS
  close_window(usewindow); 
#endif

  free(programbuffer);
  
#ifdef CONTROL
  cs_exit();
#endif
  exit(ecode); 
}

/* Linearer Fit (regression) optional mit Fehlerbalken in x und y Richtung  */

void c_fit_linear(char *n) {  
  char w1[strlen(n)+1],w2[strlen(n)+1];                  
  int e,typ,scip=0,i=0,mtw=0;  
  int vnrx,vnry,vnre,vnre2,ndata; 
  double a,b,siga,sigb,chi2,q;
  char *r;
  e=wort_sep(n,',',TRUE,w1,w2);
  while(e) {
    scip=0;
    if(strlen(w1)) {
       switch(i) {
         case 0: { /* Array mit x-Werten */     
	   /* Typ bestimmem. Ist es Array ? */
           typ=type2(w1);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnrx=variable_exist(r,typ);
             free(r);
	     if(vnrx==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else puts("FIT: Kein ARRAY.");
	   break;
	   }
	 case 1: {   /* Array mit y-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type2(w1);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnry=variable_exist(r,typ);
             free(r);
	     if(vnry==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else puts("FIT: Kein ARRAY.");
	   break;
	   } 
	 case 2: {   /* Array mit err-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type2(w1);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnre=variable_exist(r,typ);
             free(r);
	     if(vnre==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	     else mtw=1;
	   } else {scip=1; mtw=0;}
	   break;
	   } 
	 case 3: {   /* Array mit err-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type2(w1);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnre2=variable_exist(r,typ);
             free(r);
	     if(vnre2==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	     else mtw=2;
	   } else {scip=1;}
	   break;
	   } 
	 case 4: {
	   ndata=(int)parser(w1); 
	   if(vnrx!=-1 && vnry!=-1) {
             if(mtw==2 && vnre!=-1 && vnre2!=-1) {
	       linear_fit_exy(variablen[vnrx].pointer+variablen[vnrx].opcode*INTSIZE,
		   variablen[vnry].pointer+variablen[vnry].opcode*INTSIZE,ndata,
		   variablen[vnre].pointer+variablen[vnre].opcode*INTSIZE,
		   variablen[vnre2].pointer+variablen[vnre2].opcode*INTSIZE
		   ,&a,&b,&siga,&sigb,&chi2,&q); 

	     } else {
	       linear_fit(variablen[vnrx].pointer+variablen[vnrx].opcode*INTSIZE,
		   variablen[vnry].pointer+variablen[vnry].opcode*INTSIZE,ndata,(mtw)?(
		   variablen[vnre].pointer+variablen[vnre].opcode*INTSIZE):NULL,mtw,&a,&b,&siga,&sigb,&chi2,&q); 
             }
	   }
	   break; 
	 } 
	 case 5: { zuweis(w1,a); break; } 
	 case 6: { zuweis(w1,b); break;} 
	 case 7: { zuweis(w1,siga); break;} 
	 case 8: { zuweis(w1,sigb);  break;} 
	 case 9: { zuweis(w1,chi2);  break;} 
	 case 10: { zuweis(w1,q);  break;} 
	   
         default: break;
       }
    }
    if(scip==0) e=wort_sep(w2,',',TRUE,w1,w2);
    i++;
  }
}

/* Sort-Funktion (wie qsort() ), welche ausserdem noch ein integer-Array mitsortiert */

void do_sort(void *a, size_t n,size_t size,int(*compar)(const void *, const void *), int *b) {
 // printf("sort: n=%d size=%d\n",n,size);
  if (n<2) return;
  if(b==NULL) qsort(a,n,size,compar);
  else { 
    void *rra=malloc(size);
    unsigned long i,ir,j,l;
    int index;

  l=(n>>1)+1;
  ir=n;
  for(;;) {
    if(l>1) {
      memcpy(rra,a+size*(l-2),size);
      l--;
      index=b[l-1];
    } else {
      memcpy(rra,a+size*(ir-1),size);
      index=b[ir-1];
      memcpy(a+size*(ir-1),a+size*(1-1),size);
      b[ir-1]=b[1-1];
      if (--ir==1) {
        memcpy(a,rra,size);
        *b=index;
        break;
      }
    }
    i=l;j=l+l;
    while(j<=ir) {
      if(j<ir && compar(a+size*(j-1),a+size*j)<0) j++;
      if(compar(rra,a+size*(j-1))<0) {
	memcpy(a+size*(i-1),a+size*(j-1),size); 
	b[i-1]=b[j-1];
	i=j;
	j<<=1;
      } else j=ir+1;
    }
    memcpy(a+size*(i-1),rra,size);
    b[i-1]=index;
  }
  free(rra);
  }
}


/*The sort functions for all variable types */
static int cmpstring(const void *p1, const void *p2) {
 // printf("cmpstring\n");
  return(memcmp(((STRING *)p1)->pointer,((STRING *)p2)->pointer,min(((STRING *)p1)->len,((STRING *)p2)->len)));
}
static int cmpdouble(const void *p1, const void *p2) {
  if(*(double *)p1==*(double *)p2) return(0);
  else if(*(double *)p1>*(double *)p2) return(1);
  else return(-1);
}
static int cmpint(const void *p1, const void *p2) {
  if((*(int *)p1)==(*(int *)p2)) return(0);
  else if((*(int *)p1)>(*(int *)p2)) return(1);
  else return(-1);
}

/* Sortierfunktion fuer ARRAYS 

Todo: 
* Umstellen auf pliste.
* Stringsortierung bei unterschiedlicher Laenge ist nicht optimal.
* Indexarray muss INTARRAYTYP sein. Das geht auch flexibler! (mit allarray)

*/



void c_sort(char *n) {  
  char w1[strlen(n)+1],w2[strlen(n)+1];                  
  int e,typ,typi,scip=0,i=0,size;  
  int vnrx=-1,vnry=-1,ndata=0; 
  char *r;
 // printf("c_sort\n");
  e=wort_sep(n,',',TRUE,w1,w2);
  while(e) {
    scip=0;
    if(strlen(w1)) {
      switch(i) {
        case 0: { /* Array  */     
	  /* Typ bestimmem. Ist es Array ? */
          typ=type2(w1);
	  if(typ & ARRAYTYP) {
            r=varrumpf(w1);
            vnrx=variable_exist(r,typ);
            free(r);
	    if(vnrx==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	    else {
	      ARRAY dummy=array_info(vnrx);
	      ndata=anz_eintraege(dummy);
             // printf("Anz-Eintraege liefert: %d\n",ndata);
	    }
	  } else puts("SORT: Kein ARRAY.");
	  break;
        }
	case 1: 
	  ndata=(int)parser(w1);
	  break;
	case 2: {   /* Array mit index-Tabelle */
          /* Typ bestimmem. Ist es Array ? */
          typi=type2(w1);
	  if(typi==INTARRAYTYP) {
            r=varrumpf(w1);
            vnry=variable_exist(r,typi);
            free(r);
	    if(vnry==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	  } else puts("SORT: Kein INT-ARRAY.");
	  break;
	} 
        default: break;
      }
    }
    if(scip==0) e=wort_sep(w2,',',TRUE,w1,w2);
    i++;
  }
 // printf("typ= %x %x\n",typ,FLOATARRAYTYP);
  if(i>=1 && ndata>1) {
    if(typ==STRINGARRAYTYP) 
      do_sort((void *)(variablen[vnrx].pointer+variablen[vnrx].opcode*INTSIZE)
      ,ndata,sizeof(STRING),cmpstring,
      (int *)((vnry!=-1)?(variablen[vnry].pointer+variablen[vnry].opcode*INTSIZE):NULL));      
    else if(typ==INTARRAYTYP) 
      do_sort((void *)(variablen[vnrx].pointer+variablen[vnrx].opcode*INTSIZE)
      ,ndata,sizeof(int),cmpint,
      (int *)((vnry!=-1)?(variablen[vnry].pointer+variablen[vnry].opcode*INTSIZE):NULL));      
    else if(typ==FLOATARRAYTYP)  
      do_sort((void *)(variablen[vnrx].pointer+variablen[vnrx].opcode*INTSIZE)
      ,ndata,sizeof(double),cmpdouble,
      (int *)((vnry!=-1)?(variablen[vnry].pointer+variablen[vnry].opcode*INTSIZE):NULL));
  }
}

/* Allgemeine Fit-Funktion  mit Fehlerbalken in y Richtung  */

void c_fit(char *n) {  
  char w1[strlen(n)+1],w2[strlen(n)+1];                  
  int e,typ,scip=0,i=0,mtw=0;  
  int vnrx,vnry,vnre,vnre2,ndata; 
  double a,b,siga,sigb,chi2,q;
  char *r;
  e=wort_sep(n,',',TRUE,w1,w2);
  xberror(9,"FIT"); /* Funktion noch nicht moeglich */
  while(e) {
    scip=0;
    if(strlen(w1)) {
       switch(i) {
         case 0: { /* Array mit x-Werten */     
	   /* Typ bestimmem. Ist es Array ? */
           typ=type2(w1);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnrx=variable_exist(r,typ);
             free(r);
	     if(vnrx==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else puts("FIT: Kein ARRAY.");
	   break;
	   }
	 case 1: {   /* Array mit y-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type2(w1);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnry=variable_exist(r,typ);
             free(r);
	     if(vnry==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else puts("FIT: no ARRAY.");
	   break;
	   } 
	 case 2: {   /* Array mit err-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type2(w1);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnre=variable_exist(r,typ);
             free(r);
	     if(vnre==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	     else mtw=1;
	   } else {scip=1; mtw=0;}
	   break;
	   } 
	 case 4: {
	   ndata=(int)parser(w1); 
           break;
	   }
	 case 5: {   /* Funktion mit Parameterliste */
	   }
	 case 6: {   /* Ausdruck, der Angibt, welche Parameter zu fitten sind */  	 
	   if(vnrx!=-1 && vnry!=-1) {
             if(mtw==2 && vnre!=-1 && vnre2!=-1) {
	       linear_fit_exy(variablen[vnrx].pointer+variablen[vnrx].opcode*INTSIZE,
		   variablen[vnry].pointer+variablen[vnry].opcode*INTSIZE,ndata,
		   variablen[vnre].pointer+variablen[vnre].opcode*INTSIZE,
		   variablen[vnre2].pointer+variablen[vnre2].opcode*INTSIZE
		   ,&a,&b,&siga,&sigb,&chi2,&q); 

	     } else {
	       linear_fit(variablen[vnrx].pointer+variablen[vnrx].opcode*INTSIZE,
		   variablen[vnry].pointer+variablen[vnry].opcode*INTSIZE,ndata,(vnre!=-1)?(
		   variablen[vnre].pointer+variablen[vnre].opcode*INTSIZE):NULL,mtw,&a,&b,&siga,&sigb,&chi2,&q); 
             }
	   }
	   break; 
	 } 
	 case 7: { zuweis(w1,chi2); break; } 
	 case 8: { zuweis(w1,b); break;} 
	 case 9: { zuweis(w1,siga); break;} 
	 case 10: { zuweis(w1,sigb);  break;} 
	 case 11: { zuweis(w1,chi2);  break;} 
	 case 12: { zuweis(w1,q);  break;}	   
         default: break;
       }
    }
    if(scip==0) e=wort_sep(w2,',',TRUE,w1,w2);
    i++;
  }
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
      if(vnr==-1) xberror(15,v); /* Feld nicht dimensioniert */ 
      else {
        if(typ & FLOATTYP) {
	  int nn=do_dimension(vnr);    
	  double *varptr=(double  *)(variablen[vnr].pointer+variablen[vnr].opcode*INTSIZE);

	  if(e==2) isign=(int)parser(w);
	  realft(varptr,(nn-1)/2,isign);
        } else puts("FFT: Muss Float-ARRAY sein.");
      }
    } else puts("FFT: Kein ARRAY.");
  
  } else xberror(32,"FFT"); /* Syntax error */
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
      
      if(vnr1==-1 || vnr2==-1) xberror(15,""); /* Feld nicht dimensioniert */ 
      else {
        if((typ1 & STRINGTYP) && (typ2 & STRINGTYP)) copy_string_array(vnr1,vnr2); 
        else if((typ1 & INTTYP) && (typ2 & INTTYP))  copy_int_array(vnr1,vnr2); 
        else if((typ1 & FLOATTYP) && (typ2 & FLOATTYP))
	  copy_float_array(vnr1,vnr2);
	else if((typ1 & FLOATTYP) && (typ2 & INTTYP))
	  convert_int_to_float_array(vnr1,vnr2);
	else if((typ1 & INTTYP) && (typ2 & FLOATTYP))
	  convert_float_to_int_array(vnr1,vnr2);
	else puts("ARRAYCOPY: inkompatible types.");
      }
    } else puts("ARRAYCOPY: not an ARRAY.");
  } else xberror(32,"ARRAYCOPY"); /* Syntax error */

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
      if(vnr==-1) xberror(15,v); /* Feld nicht dimensioniert */ 
      else {
        if(typ & STRINGTYP) {
	  STRING str=string_parser(w);
	  fill_string_array(vnr,str);
	  free(str.pointer); 
        } else if(typ & INTTYP) fill_int_array(vnr,(int)parser(w)); 
        else if(typ & FLOATTYP) fill_float_array(vnr,parser(w));
      }
    } else xberror(32,"ARRAYFILL: not an ARRAY.");
  } else xberror(32,"ARRAYFILL"); /* Syntax error */
}
void c_memdump(PARAMETER *plist,int e) {
  memdump((char *)plist[0].integer,plist[1].integer);
}
void c_dump(PARAMETER *plist,int e) {
  int i;
  char kkk=0;
  
  if(e) kkk=((char *)plist[0].pointer)[0];

  if(kkk==0 || kkk=='%') {
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==INTTYP) printf("%s%%=%d   [%d]\n",variablen[i].name, 
        variablen[i].opcode,variablen[i].local);
    }
  }
  if(kkk==0) {
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==FLOATTYP) 
        printf("%s=%.13g    [%d]\n",variablen[i].name,variablen[i].zahl, variablen[i].local);
    }
  }
  if(kkk==0 || kkk=='$') {
    char *buf;
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==STRINGTYP) {
        buf=malloc(variablen[i].opcode+1);
	memcpy(buf,variablen[i].pointer,variablen[i].opcode);
	(buf)[variablen[i].opcode]=0;
        printf("%s$=\"%s\"   [%d]\n",variablen[i].name,buf,
        variablen[i].local);
        free(buf);
      }
    }
  }
  if(kkk==0 || kkk=='%' || kkk=='(') {
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==INTARRAYTYP) {
        int j;
        printf("%s%%(",variablen[i].name);
        for(j=0;j<variablen[i].opcode;j++) {
          if(j>0) printf(",%d",((int *)variablen[i].pointer)[j]);
	  else  printf("%d",((int *)variablen[i].pointer)[j]);
        }
        puts(")");
      }
    }
  }
  
  if(kkk==0 || kkk=='(') {
    for(i=0;i<anzvariablen;i++) {
      if(variablen[i].typ==FLOATARRAYTYP) {
        int j;
        printf("%s(",variablen[i].name);
        for(j=0;j<variablen[i].opcode;j++) {
          if(j>0) printf(",%d",((int *)variablen[i].pointer)[j]);
  	  else  printf("%d",((int *)variablen[i].pointer)[j]);
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
          if(j>0) printf(",%d",((int *)variablen[i].pointer)[j]);
  	  else  printf("%d",((int *)variablen[i].pointer)[j]);
        }
        puts(")");
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
    for(i=0;i<ANZFILENR;i++) {
      if(filenr[i]==1) {
        printf("#%d: %s [FILE]\n",i,"");
      } else if(filenr[i]==2) {
        printf("#%d: %s [SHARED OBJECT]\n",i,"");
      }
    }
  }
  if(kkk=='C' || kkk=='K'|| kkk=='c'|| kkk=='k') {
    int j;
    for(i=0;i<anzcomms;i++) {
      printf("%3d: [%08x] %s ",i,comms[i].opcode,comms[i].name);  
      if(comms[i].pmin) {
        for(j=0;j<comms[i].pmin;j++) {
	  switch(comms[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<comms[i].pmin-1) printf(",");
	}
      }
      if(comms[i].pmax>comms[i].pmin || comms[i].pmax==-1) printf("[,");
      if(comms[i].pmax==-1) printf("...");
      else {
      for(j=comms[i].pmin;j<comms[i].pmax;j++) {
	  switch(comms[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<comms[i].pmax-1) printf(",");
	}
      }
      if(comms[i].pmax>comms[i].pmin || comms[i].pmax==-1) printf("]");
      printf("\n");
    }
  }
  if(kkk=='F' || kkk=='f') {
    int j;
    for(i=0;i<anzpfuncs;i++) {
      printf("%3d: [%08x] %s(",i,pfuncs[i].opcode,pfuncs[i].name);  
      if(pfuncs[i].pmin) {
        for(j=0;j<pfuncs[i].pmin;j++) {
	  switch(pfuncs[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<pfuncs[i].pmin-1) printf(",");
	}
      }
      if(pfuncs[i].pmax>pfuncs[i].pmin || pfuncs[i].pmax==-1) printf("[,");
      if(pfuncs[i].pmax==-1) printf("...");
      else {
      for(j=pfuncs[i].pmin;j<pfuncs[i].pmax;j++) {
	  switch(pfuncs[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<pfuncs[i].pmax-1) printf(",");
	}
      }
      if(pfuncs[i].pmax>pfuncs[i].pmin || pfuncs[i].pmax==-1) printf("]");
      printf(")\n");
    }    
    for(i=0;i<anzpsfuncs;i++) {
      printf("%3d: [%08x] %s(",i,psfuncs[i].opcode,psfuncs[i].name);  
      if(psfuncs[i].pmin) {
        for(j=0;j<psfuncs[i].pmin;j++) {
	  switch(psfuncs[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<psfuncs[i].pmin-1) printf(",");
	}
      }
      if(psfuncs[i].pmax>psfuncs[i].pmin || psfuncs[i].pmax==-1) printf("[,");
      if(psfuncs[i].pmax==-1) printf("...");
      else {
      for(j=psfuncs[i].pmin;j<psfuncs[i].pmax;j++) {
	  switch(psfuncs[i].pliste[j]) {
	    case PL_INT: printf("i%%"); break;
	    case PL_FILENR: printf("#n"); break;
	    case PL_STRING: printf("t$"); break;
	    case PL_NUMBER: printf("num"); break;
	    case PL_SVAR: printf("var$"); break;
	    case PL_NVAR: printf("var"); break;
	    case PL_KEY: printf("KEY"); break;
	    default: printf("???");
	  }
	  if(j<psfuncs[i].pmax-1) printf(",");
	}
      }
      if(psfuncs[i].pmax>psfuncs[i].pmin || psfuncs[i].pmax==-1) printf("]");
      printf(")\n");
    }
  }
}

void c_end(char *n) { batch=0; }

void c_on(char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1],w3[strlen(n)+1];
  int e=wort_sep(n,' ',TRUE,w1,w2);
  int mode=0;
  if(e==0) xberror(32,"ON"); /* Syntax error */
  else {
    wort_sep(w2,' ',TRUE,w2,w3);
    if(strcmp(w2,"CONT")==0) mode=1;
    else if(strcmp(w2,"GOTO")==0) mode=2;
    else if(strcmp(w2,"GOSUB")==0) mode=3;
    else mode=0;
    
    if(strcmp(w1,"ERROR")==0) {
      errcont=(mode>0);
      if(mode==2) errorpc=labelzeile(w3);
      else if(mode==3) {
        errorpc=procnr(w3,1);
	if(errorpc!=-1) errorpc=procs[errorpc].zeile;      
      }
    } else if(strcmp(w1,"BREAK")==0) {
      breakcont=(mode>0);
      if(mode==2) breakpc=labelzeile(w3);
      else if(mode==3) {
        breakpc=procnr(w3,1);
	if(breakpc!=-1) breakpc=procs[breakpc].zeile;
      }
#ifndef NOGRAPHICS 
    } else if(strcmp(w1,"MENU")==0) {
      if(mode==0)  c_menu("");  
      else if(mode==3) {
       int pc2=procnr(w3,1);
       if(pc2==-1) xberror(19,w3); /* Procedure nicht gefunden */
       else menuaction=pc2;
      } else  printf("Unbekannter Befehl: ON <%s> <%s>\n",w1,w2);  
#endif
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
  if(e<2) xberror(42,"");  /* Zuwenig Parameter ! */
  else {
    if(z==1)      zuweis(var,parser(var)+parser(t));
    else if(z==2) zuweis(var,parser(var)-parser(t));
    else if(z==3) zuweis(var,parser(var)*parser(t));
    else if(z==4) zuweis(var,parser(var)/parser(t));
    else xberror(32,""); /* Syntax error */
  }
}

void c_swap(char *n) {
  char v[strlen(n)+1],w[strlen(n)+1];
  int e,vnr1,vnr2;
  e=wort_sep(n,',',TRUE,v,w);
  if(e<2) xberror(42,"");  /* Zuwenig Parameter ! */
  else {
     char *r1,*r2;
     int typ;
     typ=type2(v);
     if(typ!=type2(w)) xberror(58,w); /* Variable %s ist vom falschen Typ */
     else {
       r1=varrumpf(v);
       r2=varrumpf(w);
       vnr1=variable_exist(r1,typ);
       vnr2=variable_exist(r2,typ);
       if(vnr1==-1 || vnr2==-1) xberror(57,w); /* Variable %s noch nicht initialisiert */
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
  if(pos==NULL) xberror(32,"ERASE"); /* Syntax error */
  else {
    pos2=v+strlen(v)-1;
    pos[0]=0;
    pos++;

    if(pos2[0]!=')') xberror(32,"ERASE"); /* Syntax error */
    else {
      /* Typ Bestimmen  */
      pos2[0]=0;
      typ=type2(v);
      if(searchchr("$%#", v[strlen(v)-1])!=NULL) v[strlen(v)-1]=0;
      
      
      typ=typ|ARRAYTYP;
      vnr=variable_exist(v,typ);
      if(vnr==-1) xberror(15,v); /* Feld nicht dimensioniert */ 
      else {
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
      } else returnvalue.f=parser(n);
    }
    local_vars_loeschen(sp-1);
    pc=stack[--sp];
  }
  else xberror(93,""); /*Stack-Error !*/
}

void c_void(char *n) { 
  if(type2(n) & STRINGTYP) {
    char *erg=s_parser(n);
    free(erg);
  } else parser(n);
}
void c_nop(char *n) { return; }

void c_inc(PARAMETER *plist,int e) {
  if(e) {
    if(plist[0].integer&FLOATTYP) 
      *((double *)plist[0].pointer)=*((double *)plist[0].pointer)+1;
    else if(plist[0].integer&INTTYP)
//#ifdef __hpux
//      *((int *)plist[0].pointer)=*((int *)plist[0].pointer)+1; 
//#else
      (*((int *)plist[0].pointer))++;
//#endif
  }
}

void c_dec(PARAMETER *plist,int e) { 
  if(e) {
    if(plist[0].integer&FLOATTYP) 
      *((double *)plist[0].pointer)=*((double *)plist[0].pointer)-1;
    else if(plist[0].integer&INTTYP) 
// #ifdef __hpux
//    *((int *)plist[0].pointer)=*((int *)plist[0].pointer)-1;
// #else
    (*((int *)plist[0].pointer))--;
// #endif
  }
}
void c_cls(char *n) { 
#ifdef WINDOWS
  DWORD written; /* number of chars actually written */
  COORD coord; /* coordinates to start writing */
  CONSOLE_SCREEN_BUFFER_INFO coninfo; /* receives console size */ 
  HANDLE ConsoleOutput; /* handle for console output */
  ConsoleOutput=GetStdHandle(STD_OUTPUT_HANDLE); 
  GetConsoleScreenBufferInfo(ConsoleOutput,&coninfo);
#define  COLS coninfo.dwSize.X
#define  LINES coninfo.dwSize.Y
  coord.X=0;
  coord.Y=0;
  FillConsoleOutputCharacter(ConsoleOutput,' ',LINES*COLS,
    coord,&written);
  FillConsoleOutputAttribute(ConsoleOutput,
    FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE,LINES*COLS,
    coord,&written);

  SetConsoleCursorPosition(ConsoleOutput,coord);
#else
  puts("\033[2J\033[H");
#endif
}
void c_home(char *n) { 
#ifdef WINDOWS
  COORD coord;
  HANDLE ConsoleOutput; /* handle for console output */
  ConsoleOutput=GetStdHandle(STD_OUTPUT_HANDLE); 
  coord.X=0;
  coord.Y=0;
  SetConsoleCursorPosition(ConsoleOutput,coord);
#else
  puts("\033[H");
#endif
}
void c_version(char *n) { printf("X11-BASIC Version: %s %s\n",version,vdate);}

#ifndef WINDOWS
#include <fnmatch.h>
#else
#include "Windows.extension/fnmatch.h"
#endif
void c_help(char *w) {
  if(strlen(w)==0) puts("HELP [topic]");
  else {
    int j,i;
    for(i=0;i<anzcomms;i++) {
    
      if(fnmatch(w,comms[i].name,FNM_NOESCAPE)==0) {
        printf("%s ",comms[i].name);  
        if(comms[i].pmin) {
          for(j=0;j<comms[i].pmin;j++) {
	    switch(comms[i].pliste[j]) {
	      case PL_INT: printf("i%%"); break;
	      case PL_FILENR: printf("#n"); break;
	      case PL_STRING: printf("t$"); break;
	      case PL_NUMBER: printf("num"); break;
	      case PL_SVAR: printf("var$"); break;
	      case PL_NVAR: printf("var"); break;
	      case PL_KEY: printf("KEY"); break;
	      case PL_FARRAY: printf("a()"); break;
	      case PL_IARRAY: printf("h%()"); break;
	      case PL_SARRAY: printf("f$()"); break;
	      case PL_LABEL: printf("<label>"); break;
	      case PL_PROC: printf("<procedure>"); break;
	      case PL_FUNC: printf("<function>"); break;
	      default: printf("???");
	    }
	    if(j<comms[i].pmin-1) printf(",");
	  }
        }
        if(comms[i].pmax>comms[i].pmin || comms[i].pmax==-1) printf("[,");
        if(comms[i].pmax==-1) printf("...");
        else {
        for(j=comms[i].pmin;j<comms[i].pmax;j++) {
	    switch(comms[i].pliste[j]) {
	      case PL_INT: printf("i%%"); break;
	      case PL_FILENR: printf("#n"); break;
	      case PL_STRING: printf("t$"); break;
	      case PL_NUMBER: printf("num"); break;
	      case PL_SVAR: printf("var$"); break;
	      case PL_NVAR: printf("var"); break;
	      case PL_KEY: printf("KEY"); break;
	      case PL_FARRAY: printf("a()"); break;
	      case PL_IARRAY: printf("h%()"); break;
	      case PL_SARRAY: printf("f$()"); break;
	      case PL_LABEL: printf("<label>"); break;
	      case PL_PROC: printf("<procedure>"); break;
	      case PL_FUNC: printf("<function>"); break;
	      default: printf("???");
	    }
	    if(j<comms[i].pmax-1) printf(",");
	  }
        }
        if(comms[i].pmax>comms[i].pmin || comms[i].pmax==-1) printf("]");
        puts("");
      }
    }
    for(i=0;i<anzpfuncs;i++) {
      if(fnmatch(w,pfuncs[i].name,FNM_NOESCAPE)==0) {
        printf("%s(",pfuncs[i].name);
	if(pfuncs[i].pmin) printf("%d",pfuncs[i].pmin);
	if(pfuncs[i].pmax-pfuncs[i].pmin) printf("-%d",pfuncs[i].pmax);
        puts(")");
      }
    }
     for(i=0;i<anzpsfuncs;i++) {
      if(fnmatch(w,psfuncs[i].name,FNM_NOESCAPE)==0) {
        printf("%s(",psfuncs[i].name);  
	if(psfuncs[i].pmin) printf("%d",psfuncs[i].pmin);
	if(psfuncs[i].pmax-psfuncs[i].pmin) printf("-%d",psfuncs[i].pmax);
        puts(")");
      }
    }
     for(i=0;i<anzsysvars;i++) {
      if(fnmatch(w,sysvars[i].name,FNM_NOESCAPE)==0) {
        if(sysvars[i].opcode==PL_INT) printf("int ");
	else if(sysvars[i].opcode==PL_FLOAT) printf("flt ");
	else printf("??? ");
        printf("%s\n",sysvars[i].name);          
      }
    }
      for(i=0;i<anzsyssvars;i++) {
      if(fnmatch(w,syssvars[i].name,FNM_NOESCAPE)==0) {
        printf("%s\n",syssvars[i].name);          
      }
    }
  
  }
}
void c_error(PARAMETER *plist,int e) {
  xberror(plist[0].integer,"");
}
void c_free(PARAMETER *plist,int e) {
  free((char *)plist[0].integer);
}
void c_detatch(PARAMETER *plist,int e) {
  int r=shm_detatch(plist[0].integer);
  if(r!=0) io_error(r,"DETATCH");
}
void c_shm_free(PARAMETER *plist,int e) {
  shm_free((char *)plist[0].integer);
}
void c_pause(PARAMETER *plist,int e) {
#ifdef WINDOWS
  Sleep((int)(1000*plist[0].real));
#else
  double zeit=plist[0].real;
  int i=(int)zeit;
  if(i) sleep(i);
  zeit=zeit-(double)i;
  if(zeit>0) usleep((int)(1000000*zeit));
#endif
}

void c_echo(PARAMETER *plist,int e) {
  char *n=plist[0].pointer;
  if(strcmp(n,"ON")==0) echoflag=TRUE; 
  else if(strcmp(n,"OFF")==0) echoflag=FALSE;
  else  echoflag=(int)parser(n);
}

void c_stop()  {batch=0;} 
void c_tron()  {echoflag=1;}
void c_troff() {echoflag=0;}
void c_beep()  {printf("\007");}
 
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

c_doclr(char *v) {
  char *r,*pos;
  int vnr,typ; 
 
 /* Typ bestimmem. Ist es Array ? */
 
  typ=type2(v);
  if(typ & CONSTTYP) {xberror(32,"CLR"); return;} /* Syntax error */
  r=varrumpf(v);
  vnr=variable_exist(r,typ);

  if(typ & ARRAYTYP) { /* ganzes Array  */
    if(vnr==-1) xberror(15,r); /* Feld nicht dimensioniert */ 
    else {
      if(typ & STRINGTYP) {
        STRING a=create_string("");
        fill_string_array(vnr,a);
	free_string(a); 
      } else if(typ & INTTYP) fill_int_array(vnr,0); 
      else if(typ & FLOATTYP) fill_float_array(vnr,0.0);
    }
  } else {
    int indize=((pos=strchr(v,'('))!=NULL); 
    if(indize) vnr=variable_exist(r,typ | ARRAYTYP);
    if(vnr==-1) {   /* Variable existiert gar nicht */
        if(indize) xberror(15,r); /* Feld nicht dimensioniert */ 
	else {
	  if(typ & STRINGTYP) neue_string_variable(r,(STRING){0,""},0); 
	  else if(typ & INTTYP) neue_int_variable(r,0,0); 
	  else if(typ & FLOATTYP) neue_float_variable(r,0.0,0);
        }
      } else {      /* Variable existiert */
      
        /* Hier wird Feld/indexe abgefangen  */
	if(!indize) {
          if(typ & STRINGTYP) {
	     variablen[vnr].pointer=realloc(variablen[vnr].pointer,1);
	     *(variablen[vnr].pointer)=0;
	     variablen[vnr].opcode=0; 
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
  i=pcode[pc-1].integer;
  if(i==-1) {
    for(i=pc; (i<prglen && i>=0);i++) {
      o=pcode[i].opcode&PM_SPECIAL;
      if((o==P_LOOP || o==P_NEXT || o==P_WEND ||  o==P_UNTIL)  && f<=0) break;
      if(o & P_LEVELIN) f++;
      if(o & P_LEVELOUT) f--;
     }
    if(i==prglen) { xberror(36,"BREAK/EXIT IF"); /*Programmstruktur fehlerhaft */return;}
    pc=i+1;
  } else pc=i;
}

void c_if(char *n) {
  char w1[MAXSTRLEN],w2[MAXSTRLEN],w3[MAXSTRLEN];
  
  if((int)parser(n)==0) { 
    int i,f=0,o;
  
    for(i=pc; (i<prglen && i>=0);i++) {
      o=pcode[i].opcode&PM_SPECIAL;
      if((o==P_ENDIF || o==P_ELSE|| o==P_ELSEIF)  && f==0) break;
      else if(o==P_IF) f++;
      else if(o==P_ENDIF) f--;
    }
    
    if(i==prglen) { xberror(36,"IF"); /*Programmstruktur fehlerhaft */return;}
    pc=i+1;
    if(o==P_ELSEIF) {
      xtrim(program[i],TRUE,w1);
      wort_sep(w1,' ',TRUE,w2,w3);
      wort_sep(w3,' ',TRUE,w1,w2);
      if(strcmp(w1,"IF")==0) c_if(w2); 
    } 
  }
}

void c_select(char *n) {
  int i,wert,wert2,f=0,o;
  char w1[MAXSTRLEN],w2[MAXSTRLEN],w3[MAXSTRLEN];
  
  wert=(int)parser(n);
  
  /* Case-Anweisungen finden */
  while(1) {
    for(i=pc; (i<prglen && i>=0);i++) {
      o=pcode[i].opcode&PM_SPECIAL;
      if((o==P_DEFAULT || o==P_CASE)  && f==0) break;
      else if(o==P_SELECT) f++;
      else if(o==P_ENDSELECT) f--;
    }
    
     if(i==prglen) { xberror(36,"SELECT"); /*Programmstruktur fehlerhaft */return;}
     pc=i;
     if(pcode[i].opcode&PM_SPECIAL==P_CASE) {
       xtrim(program[pc],TRUE,w1);
       wort_sep(w1,' ',TRUE,w2,w3);
       wert2=parser(w3);
       if(wert==wert2) break;
     } else break;
  }  
  pc++;  
}

void bidnm(char *n) {
  xberror(38,n); /* Befehl im Direktmodus nicht moeglich */
}

void c_next(char *n) {
  char w1[MAXSTRLEN],w2[MAXSTRLEN],w3[MAXSTRLEN],var[MAXSTRLEN];
  double step, limit;
  int ss,e,f=0,hpc=pc;

   pc=pcode[pc-1].integer; 
   if(pc==-1) {xberror(36,"NEXT"); /*Programmstruktur fehlerhaft */return;}

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
  int typ;
  wort_sep(n,' ',TRUE,w1,w2);
  if(searchchr(w1,'=')!=NULL) {
    wort_sep(w1,'=',TRUE,w2,w3);
    typ=vartype(w2);
    if(typ & INTTYP) zuweisi(w2,parser(w3));
    else zuweis(w2,parser(w3));
  } else {printf("Syntax Error ! FOR %s\n",n); batch=0;}
}
void c_until(char *n) {
  if(parser(n)==0) {
    int npc=pcode[pc-1].integer;
    if(npc==-1) xberror(36,"UNTIL"); /*Programmstruktur fehlerhaft */
    else pc=npc+1;
  }
}

void c_while(char *n) {
  if(parser(n)==0) {
    int npc=pcode[pc-1].integer;
    if(npc==-1) xberror(36,"WHILE"); /*Programmstruktur fehlerhaft */
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
    xberror(32,"WORT_SEP"); /* Syntax Error */
    return(0);
  } else return(ergeb);
}
void c_poke(PARAMETER *plist,int e) {
  char *adr=(char *)(plist[0].integer);
  *adr=(char)plist[1].integer;
}
void c_dpoke(PARAMETER *plist,int e) {
  short *adr=(short *)(plist[0].integer);
  *adr=(short)plist[1].integer;
}
void c_lpoke(PARAMETER *plist,int e) {
  long *adr=(long *)plist[0].integer;
  *adr=(long)plist[1].integer;
}

void c_sound(PARAMETER *plist,int e) { speaker(plist[0].integer); }
void c_eval(PARAMETER *plist,int e) { kommando(plist[0].pointer); }
