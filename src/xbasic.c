/* XBASIC.C

   (c) Markus Hoffmann


   ##     ##   #   #        ######
    ##   ##   ##  ##        ##   ##                    ##
     ## ##   ### ###        ##   ##
      ###     ##  ## #####  #####     #####    #####  ####   ####
     ## ##    ##  ##        ##   ##       ##  ##       ##   ##  ##
    ##   ##   ##  ##        ##   ##   ######   ####    ##   ##
   ##     ##  ##  ##        ##   ##  ##   ##      ##   ##   ##  ##
   ##     ##  ##  ##        ######    ######  #####   ####   ####



                       VERSION 1.24

            (C) 1997-2016 by Markus Hoffmann
              (kollo@users.sourceforge.net)
            (http://x11-basic.sourceforge.net/)

 **  Erstellt: Aug. 1997   von Markus Hoffmann				   **
 **  letzte Bearbeitung: 08/2015  von Markus Hoffmann		   **
*/

 /* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif


#include <math.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "xbasic.h"
#include "type.h"
#include "parser.h"
#include "parameter.h"
#include "kommandos.h"
#include "gkommandos.h"
#include "io.h"
#include "file.h"
#include "array.h"
#include "x11basic.h"
#include "wort_sep.h"
#include "bytecode.h"
#include "virtual-machine.h"
#include "graphics.h"
#include "window.h"
#include "number.h"
#include "functions.h"


const char libversion[]=VERSION;           /* Programmversion           */
const char libvdate[]=VERSION_DATE;
extern const char version[];
extern const char libvdate[];

#ifdef CONTROL
const char xbasic_name[]="csxbasic";
#elif defined DOOCS
const char xbasic_name[]="doocsxbasic";
#elif defined TINE
const char xbasic_name[]="tinexbasic";
#elif defined USE_SDL
const char xbasic_name[]="sdlxbasic";
#else
const char xbasic_name[]="xbasic";
#endif
int pc=0,sp=0,echoflag=0,batch=0,errcont=0,breakcont=0,everyflag=0;
int errorpc=-1,errorpctype=0,breakpc=-1,breakpctype=0;
int everytime=0,alarmpc=-1,alarmpctype=0;

int stack[STACKSIZE];


/* fuer die Dateiverwaltung     */
FILEINFO filenr[ANZFILENR];
static int *linetable=NULL;   /* for correctly count splitted lines*/


PARAMETER returnvalue;

int param_anzahl;
char **param_argumente=NULL;

int usewindow=DEFAULTWINDOW;

void free_pcode(int l);

static int add_label(char *name,int zeile,int dataptr);
static int add_proc(char *name,char *pars,int zeile,int typ);

static int oldprglen=0;

extern char ifilename[];

/*return the original line (accounting for splitted lines) */
int original_line(int i) {
  if(linetable==NULL || i<0) return i;
  int j;
  int a=i;
  for(j=0;j<i;j++) a+=linetable[j];
  return a;
}


/* Bytecode spezifica. */

int is_bytecode=0;
static char *stringseg;
BYTECODE_SYMBOL *symtab;

static void do_relocation(char *adr,unsigned char *fixup, int l) {
  int i=0;
  long ll;
 // printf("Relocation table:\n");
 // memdump(fixup,l);
  while(i<l) {
    if(fixup[i]==0) break;
    else if(fixup[i]==1) adr+=254;
    else {
      adr+=fixup[i];
      memcpy(&ll,adr,sizeof(long));
      #ifdef ATARI
        LWSWAP((short *)&ll);
      #endif
      ll+=(long)adr;
      #ifdef ATARI
        LWSWAP((short *)&ll);
      #endif
      memcpy(adr,&ll,sizeof(long));
    }
  }
}
/* We assume that the segments are in following order: 
   HEADER
   TEXT
   RODATA
   SDATA
   DATA
   BSS  --- STRINGS  
        --- SYMBOL
	--- RELOCATION
	*/

char *bytecode_init(char *adr) {
  int i,a,typ;
  char *name;
  char *bsseg;
  unsigned char *fixup;   /* Relocation information */
//#ifdef ANDROID
//  char buffer[256];
//#endif

  /* Ueberpruefe ob ein gueltiger Header dabei ist und setze databuffer */
  if(adr[0]==BC_BRAs && adr[1]==sizeof(BYTECODE_HEADER)-2) {
    BYTECODE_HEADER *bytecode=(BYTECODE_HEADER *)adr;

    clear_program();
    free_pcode(prglen);
    prglen=pc=0;
    is_bytecode=1;
//#ifdef ANDROID
//     sprintf(buffer,"Bytecode header found (V.%x)\n",bytecode->version);
//     backlog(buffer);
//#endif
    if(bytecode->version!=BC_VERSION) {
      printf("ERROR: This bytecode was compiled for a different version (V.%04x) of "
      "X11-Basic.\n Please consider to recompile it from the .bas file.\n",bytecode->version);
      return(NULL);
    }
        
    /* Set up the data buffer */
    databuffer=adr+bytecode->textseglen+bytecode->rodataseglen+sizeof(BYTECODE_HEADER);
    databufferlen=bytecode->sdataseglen;
//#ifdef ANDROID
//    sprintf(buffer,"Databuffer $%08x contains: <%s>\n",(unsigned int)databuffer,databuffer);
//    backlog(buffer);
//#endif
    rodata=&adr[sizeof(BYTECODE_HEADER)+bytecode->textseglen];
    bsseg=stringseg=&adr[sizeof(BYTECODE_HEADER)+
                 bytecode->textseglen+
		 bytecode->rodataseglen+
		 bytecode->sdataseglen+
		 bytecode->dataseglen];

    /* Jetzt Variablen anlegen.*/
    symtab=(BYTECODE_SYMBOL *)(adr+sizeof(BYTECODE_HEADER)+
                                   bytecode->textseglen+
		                   bytecode->rodataseglen+
		                   bytecode->sdataseglen+
				   bytecode->dataseglen+
				   bytecode->stringseglen);
    a=bytecode->symbolseglen/sizeof(BYTECODE_SYMBOL);
    if(a>0) {
      for(i=0;i<a;i++) {
	#ifdef ATARI
	  LWSWAP((short *)&symtab[i].name);
	  LWSWAP((short *)&symtab[i].adr);
	#endif
        if(symtab[i].typ==STT_OBJECT) {
	  typ=symtab[i].subtyp;
	  if(symtab[i].name) {
	    name=&stringseg[symtab[i].name];
          } else {
	    name=malloc(32);  /*TODO: Das muss irgendwann wieder freigegeben werden ....*/
	    sprintf(name,"VAR_%x",i);
          }
	  
	  /*Hier erstmal nur int und float im bss ablegen, da noch nicht geklaert ist, 
	  wie wir strings und arrays hier initialisieren koennen ohne die symboltabelle 
	  zu ueberschreiben*/
//#if defined ANDROID
//    sprintf(buffer,"Symbol: <%s> $%04x %08x ",name,typ,symtab[i].adr);
//    backlog(buffer);
//#endif	  
	  if(typ&ARRAYTYP)        add_variable(name,ARRAYTYP,typ&(~ARRAYTYP),V_DYNAMIC,NULL);
	  else if(typ==STRINGTYP) add_variable(name,typ,0,V_DYNAMIC,NULL);
	  else                    add_variable(name,typ,0,V_STATIC,bsseg+symtab[i].adr);
//#ifdef ANDROID	  
//    sprintf(buffer,"BSSSEG auf %08x ",bsseg);
//    backlog(buffer);
//#endif
        }  
      }
    }
//#ifdef ANDROIOD    
//    sprintf(buffer,"%d variables.\n",anzvariablen);
//    backlog(buffer);
//#endif
#if DEBUG
    printf("%d variables.\n",anzvariablen);
    c_dump(NULL,0);
#endif
    fixup=(unsigned char *)(adr+sizeof(BYTECODE_HEADER)+
                       bytecode->textseglen+
		       bytecode->rodataseglen+
		       bytecode->sdataseglen+
		       bytecode->dataseglen+
		       bytecode->stringseglen+
		       bytecode->symbolseglen);
    if((bytecode->flags&EXE_REL)==EXE_REL && bytecode->relseglen>0) 
      do_relocation(adr,fixup,bytecode->relseglen);

   /*Now: clear bss segment. This will probably overwrite symbol table and strings and relocation info*/
    if(bytecode->bssseglen>0) bzero(bsseg,bytecode->bssseglen);
#ifdef ANDROID
    backlog("bytecode_init done.");
#endif
    return(adr);
  } else {
    printf("VM: ERROR, file format not recognized. $%02x $%02x\n",adr[0],adr[1]);
    return(NULL);
  }
}

int fix_bytecode_header(BYTECODE_HEADER *bytecode) {
  if(((char *)bytecode)[0]==BC_BRAs && ((char *)bytecode)[1]==sizeof(BYTECODE_HEADER)-2) {
  #if DEBUG 
    printf("Bytecode header found (V.%x)\n",bytecode->version);
  #endif
  #ifdef ATARI
  WSWAP((char *)&bytecode->version);
  #endif
  if(bytecode->version!=BC_VERSION) {
    printf("ERROR: This bytecode was compiled for a different version of "
    "X11-Basic.\n Please consider to recompile it from the .bas file.\n");
    return(-1);
  }
#ifdef ATARI
  LWSWAP((short *)&bytecode->textseglen);
  LWSWAP((short *)&bytecode->rodataseglen);
  LWSWAP((short *)&bytecode->sdataseglen);
  LWSWAP((short *)&bytecode->dataseglen);
  LWSWAP((short *)&bytecode->bssseglen);
  LWSWAP((short *)&bytecode->symbolseglen);
  LWSWAP((short *)&bytecode->stringseglen);
  LWSWAP((short *)&bytecode->relseglen);
  WSWAP((char *)&bytecode->flags);
#endif
  return(0);
  } else return(-1);
}



/* Routine zum Laden eines Programms */

int mergeprg(const char *fname) {
  int i,len;
  char *pos;
  FILE *dptr;
  /* Filelaenge rauskriegen */

  dptr=fopen(fname,"rb"); len=lof(dptr); fclose(dptr);
  programbuffer=realloc(programbuffer,programbufferlen+len+1);
  #ifdef ATARI
    if(programbuffer==NULL) {
      perror("malloc");
      printf("ERROR: Need at least %d Bytes free memory.\n",programbufferlen+len+1);
      sleep(5);
      exit(-1);
    } else printf("MERGE: programbuffer=%p len=%d\n",programbuffer,programbufferlen+len+1);
  #endif
  bload(fname,programbuffer+programbufferlen,len);
  programbufferlen+=len;


/* Hier jetzt pr"ufen, ob es sich um eine bytecode-Datei handelt.
   wenn nicht, versuche sie zu interpretieren...*/

  /* Ueberpruefe ob ein gueltiger Header dabei ist */

  if(programbuffer[0]==BC_BRAs && programbuffer[1]==sizeof(BYTECODE_HEADER)-2) {
    BYTECODE_HEADER *bytecode=(BYTECODE_HEADER *)programbuffer;

    if(fix_bytecode_header((BYTECODE_HEADER *)programbuffer)) return(-1);
    
    /* Sicherstellen, dass der Speicherberiech auch gross genug ist fuer bss segment*/
    if(bytecode->bssseglen>bytecode->relseglen+bytecode->stringseglen+bytecode->symbolseglen) {
      programbufferlen+=bytecode->bssseglen-bytecode->stringseglen-bytecode->symbolseglen;
      programbuffer=realloc(programbuffer,programbufferlen);
    }
    
    if(bytecode_init(programbuffer)) return(0);
    return(-1); /* stimmt was nicht. */
  } else {
    
   /* Zeilenzahl herausbekommen */
    pos=programbuffer;
    oldprglen=prglen;
    i=prglen=0;
    
    /*Erster Durchgang */
    while(i<programbufferlen) {
      if(i<programbufferlen-1 && programbuffer[i]=='\r' && programbuffer[i+1]=='\n') {  /*DOS WINDOWS line ending behandeln.*/
        programbuffer[i]='\n';
	programbuffer[i+1]=2;
      }
      if(i>0 && programbuffer[i]=='\n' && programbuffer[i-1]=='\\') {
        programbuffer[i]=1;   /* Marker */
        programbuffer[i-1]=' ';
      } else if(programbuffer[i]==0 || programbuffer[i]=='\n') {
        programbuffer[i]=0;
	prglen++;
      } else if(programbuffer[i]==9) programbuffer[i]=' '; /* TABs entfernen*/
      i++;
    }
    if(i>0 && programbuffer[i-1]!=0) prglen++;  /*letzte Zeile hatte kein \n*/
    
    program=(char **)realloc(program,prglen*sizeof(char *));  /*Array mit Zeilenpointern*/
    linetable=realloc(linetable,prglen*sizeof(int));
    bzero(linetable,prglen*sizeof(int));
 

    /* Zweiter Durchgang */

    
    prglen=i=0;
    while(i<programbufferlen) {
      if(programbuffer[i]==1) {
        programbuffer[i]=' '; /* Marker entfernen*/
	linetable[prglen]++;
	if(i<programbufferlen+1 &&  programbuffer[i+1]==2) {
	  programbuffer[i+1]=' '; /* Marker entfernen*/
	}
      } else if(programbuffer[i]==2) {
        programbuffer[i]=0; /* Marker entfernen*/
	pos++;  // alternativ: pos=programbuffer+i+1;
      } else if(programbuffer[i]==0) {
        program[prglen++]=pos;
        pos=programbuffer+i+1;
      }
      i++;
    }
    if((pos-programbuffer)<programbufferlen) {
      program[prglen++]=pos;  /* Potenzielle letzte Zeile ohne \n */
      programbuffer[i]=0; /*stelle sicher dass die letzte Zeile durch ein 0 beendet ist*/
    }
    return(init_program(prglen));
  }
}



static int find_comm_guess(const char *w1,int *guessa,int *guessb) {
  int i=0,a=anzcomms-1,b;
  /* Kommandoliste durchsuchen, moeglichst effektiv ! */
  for(b=0; b<strlen(w1); b++) {
    while(w1[b]>(comms[i].name)[b] && i<a) i++;
    while(w1[b]<(comms[a].name)[b] && a>i) a--;
    if(i==a) break;
  }
  *guessa=i;
  *guessb=a;
  if((i==a && strncmp(w1,comms[i].name,strlen(w1))==0) ||
     (i!=a && strcmp(w1,comms[i].name)==0) ) {
#ifdef DEBUG
      if(b<strlen(w1)) printf("Command %s completed --> %s\n",w1,comms[i].name);
#endif
     return(i);
  }
  return(-1);
}




/*Entfernt ein Programm und alle Strukturen/Variablen aus dem Speicher, 
so dass ein neues Programm (bas oder bytecode) geladen werden kann. */

void clear_program() {

/* Stack aufraumen und Variablen entfernen */

  restore_all_locals();  /* sp=0 */
  remove_all_variables();

  if(is_bytecode) {

  } else {
    if(databuffer) free(databuffer);    
  }
  is_bytecode=0;
  databuffer=NULL;
  databufferlen=0;
  datapointer=0;
  clear_labelliste();
  clear_procliste();
}




/*****************************************************

Programmvorbereitung und precompilation

******************************************************/

int init_program(int prglen) {
  char *expr,*pos2,*pos3,*buffer=NULL,*zeile=NULL;  
  int i,j,len,typ;
  clear_program();
  free_pcode(oldprglen);

  init_pcode(prglen); /*Speicher für pcode allozieren und mit 0 füllen */ 
  
  /* Label-, Procedur- und Variablenliste Erstellen und p_code transformieren*/  
  for(i=0; i<prglen;i++) {
    zeile=realloc(zeile,strlen(program[i])+1);
    buffer=realloc(buffer,strlen(program[i])+1);
    strcpy(zeile, program[i]);
    
/*löschen nicht nötig, da das init_pcode macht.*/
//    code[i].opcode=0;       /*Typ und Kommandonummer*/
//    pcode[i].panzahl=0;       /*Anzahl Parameter*/
//    pcode[i].ppointer=NULL;   /*Zeiger auf Parameterliste*/
//    pcode[i].argument=NULL; /*String als argument*/
//    pcode[i].etyp=PE_NONE;   /* fuer Kommentare */
//    pcode[i].extra=NULL;   /*Extra string fuer Kommentare*/
    pcode[i].integer=-1;

    /* Seitenkommentar behandeln:*/

    wort_sep2(zeile," !",TRUE,zeile,buffer);  /*Kommentare abseparieren*/
    xtrim2(zeile,TRUE,zeile);
    if(strlen(buffer)) {
      pcode[i].etyp=PE_COMMENT;
      pcode[i].extra=strdup(buffer);
    }
    
#if defined DEBUG 
    printf("Zeile %d (%d) : %s\n",i,original_line(i),zeile);
#endif
    if(wort_sep(zeile,' ',TRUE,zeile,buffer)==0) {
      pcode[i].opcode=P_NOTHING;
      continue;
    }
    switch(*zeile) {
    case '\'':
    case '#':
    case '!':
      pcode[i].opcode=P_REM;
      pcode[i].argument=strdup(buffer);
      continue;
    case '@':
      pcode[i].argument=strdup(zeile+1);
      pcode[i].opcode=P_GOSUB|find_comm("GOSUB");
      continue;
    case '~':
      pcode[i].argument=strdup(zeile+1);
      pcode[i].opcode=P_VOID|find_comm("VOID");
      continue;
    }
    len=strlen(zeile);
    if(zeile[len-1]==':') {
      zeile[len-1]=0;
#ifdef DEBUG 
      printf("Label gefunden: %s in Zeile %d (%d)\n",zeile,i,original_line(i));
#endif
      pcode[i].opcode=P_LABEL;
      pcode[i].integer=add_label(zeile,i,(databufferlen?(databufferlen+1):databufferlen));
      continue;
    } 
    if(strcmp(zeile,"DATA")==0) {
      pcode[i].opcode=P_DATA;
#ifdef DEBUG
      printf("DATA Statement found in line %d (%d). <%s>\n",i,original_line(i),buffer);
#endif
      databuffer=realloc(databuffer,databufferlen+strlen(buffer)+2);
      if(databufferlen) databuffer[databufferlen++]=',';
      
      memcpy(databuffer+databufferlen,buffer,strlen(buffer));
      databufferlen+=strlen(buffer);
      databuffer[databufferlen]=0;
   //   printf("databuffer now contains %d Bytes.\n",databufferlen);
   //   printf("databuffer=<%s>\n",databuffer);
      continue;
    }
    typ=(PROC_PROC*(strcmp(zeile,"PROCEDURE")==0) | 
                   (PROC_FUNC*(strcmp(zeile,"FUNCTION")==0)) |
		   (PROC_DEFFN*(strcmp(zeile,"DEFFN")==0)));
    if(typ!=0) {
#ifdef DEBUG
      printf("procedure or function found in line %d (%d). Typ=%d, <%s>\n",i,original_line(i),typ,buffer);
#endif
      if(typ==PROC_DEFFN) {
        pcode[i].opcode=P_DEFFN;
        expr=searchchr2(buffer,'=');
	if(expr==NULL) {
	  printf("WARNING at line %d: ==> Syntax error: DEFFN\n",original_line(i));
	  pcode[i].argument=NULL;
	  pcode[i].opcode|=P_INVALID;
	} else {
	  *expr++=0;
	  pcode[i].argument=strdup(expr);
        }
      } else pcode[i].opcode=P_PROC;
      pos2=searchchr(buffer,'(');
      if(pos2 != NULL) {
          pos2[0]=0;pos2++;
          pos3=pos2+strlen(pos2)-1;
          if(pos3[0]!=')') printf("WARNING at line %d: ==> Syntax error: parameter list\n",original_line(i));
          else *pos3++=0;
      } else pos2=zeile+strlen(zeile);
      pcode[i].integer=add_proc(buffer,pos2,i,typ);
      continue;
    } 
    /* Rest Transformieren    */
    j=-1;
    if(strcmp(zeile,"ON")==0) {
       // TODO: Sonderbehandlung ON  
        char *w1,*w2,w3[strlen(buffer)+1],w4[strlen(buffer)+1];
	wort_sep_destroy(buffer,' ',TRUE,&w1,&w2);
	wort_sep(w2,' ',TRUE,w3,w4);
	// printf("Onspecial: <%s> <%s> --<%s>-- / <%s>\n",w1,w2,w3,w4);
         if(strcmp(w1,"ERROR")==0 || strcmp(w1,"BREAK")==0 || strcmp(w1,"MENU")==0) {
	   if(strcmp(w3,"GOSUB")==0) {
	     j=find_comm("ON B/E/M GOSUB");
	     sprintf(buffer,"%s,%s",w1,w4);
	   } else if(strcmp(w3,"GOTO")==0) {
	     j=find_comm("ON B/E/M GOTO");
	     sprintf(buffer,"%s,%s",w1,w4);
	   } else {
	     j=find_comm("ON B/E/M OTHER");
  	     sprintf(buffer,"%s,%s",w1,w3);
	   }
	  // printf("j=%d, buffer=<%s>\n",j,buffer);
         } else if(strcmp(w3,"GOSUB")==0) {
	   j=find_comm("ON GOSUB");
	   strcat(buffer,",");
	   strcat(buffer,w4);
         } else if(strcmp(w3,"GOTO")==0) {
	   j=find_comm("ON GOTO");
	   strcat(buffer,",");
	   strcat(buffer,w4);
	 } else j=find_comm(zeile);
    } else j=find_comm(zeile);
            
    if(j==-1) { /* Kein Befehl passt... */
      char *buf=malloc(strlen(zeile)+strlen(buffer)+2);
      char *pos,*name;
      strcpy(buf,zeile);
      if(strlen(buffer)) {strcat(buf," ");strcat(buf,buffer);}
      //  printf("EVAL: %s\n",buf);
      if(*buf=='&' || *buf=='+' || *buf=='-' || *buf=='~' || *buf=='@' || isdigit(*buf) ) {
	pcode[i].opcode=P_EVAL|P_NOCMD;
	pcode[i].panzahl=0;
	pcode[i].ppointer=NULL;
	pcode[i].argument=buf;
	continue;
      } 
      pos=searchchr2(buf,'=');
      name=buf;
      if(pos!=NULL) {
        *pos++=0;
	pcode[i].opcode=P_ZUWEIS; /* opcode durch P_ZUWEIS ersetzen, */
	pcode[i].argument=strdup(pos);
	int typ=vartype(name);
	int typ2=pcode[i].atyp=type(pos);   /* Argument typ merken, damit spaeter der richtige parser ausgewählt wird.*/
	char *vname,*argument;
	int e=klammer_sep_destroy(buf,&vname,&argument);
	char *r=varrumpf(name);
	    /*  Jetzt Variable anlegen, wenn noch nicht definiert,
	    	       pcode[i].integer= ist varnummer,
	    */
	if(typ&ARRAYTYP) {
  	      pcode[i].panzahl=0;
	      pcode[i].ppointer=NULL;
	      pcode[i].integer=add_variable(r,ARRAYTYP,typ&(~ARRAYTYP),V_DYNAMIC,NULL);
	      if((pcode[i].atyp&ARRAYTYP)!=ARRAYTYP) printf("WARNING: type mismatch in assignment at line %d.\n",original_line(i));
              /*TODO: Subarray-Zuweisung...*/
        } else {
	      if(pcode[i].atyp&ARRAYTYP) printf("WARNING: type mismatch in assignment at line %d.\n",original_line(i));
              if(e>1) {  /*Idicies sind da */
		pcode[i].integer=add_variable(r,ARRAYTYP,typ,V_DYNAMIC,NULL);
		pcode[i].panzahl=count_parameters(argument);   /* Anzahl indizes z"ahlen*/
		pcode[i].ppointer=calloc(pcode[i].panzahl,sizeof(PARAMETER));
                /*hier die Indizies in einzelne zu evaluierende Ausdruecke
		  separieren*/
		  // printf("makepreparelist: <%s>\n",argument);
		make_preparlist(pcode[i].ppointer,argument);
              } else {
  	        pcode[i].panzahl=0;
	        pcode[i].ppointer=NULL;
	        pcode[i].integer=add_variable(r,typ,0,V_DYNAMIC,NULL);
	      }
	      if((typ&TYPMASK)!=(typ2&TYPMASK) && ((typ&TYPMASK)==STRINGTYP || (typ2&TYPMASK)==STRINGTYP))
	        printf("WARNING: type mismatch in assignment at line %d.\n",original_line(i));
	}
	if(pcode[i].integer==-1) printf("ERROR at line %d: variable could not be created.\n",original_line(i));
	free(r);
	free(buf);
        /* Jetzt noch die rechte Seite behandeln....*/
        pcode[i].rvalue=calloc(1,sizeof(PARAMETER));
	// printf("Rechte Seite <%s> typ=%x\n",pcode[i].argument,pcode[i].atyp);
        make_parameter_stage2(pcode[i].argument,(PL_CONSTGROUP|(pcode[i].atyp&BASETYPMASK)),pcode[i].rvalue);
	// dump_parameterlist(pcode[i].rvalue,1);
	pcode[i].rvalue->panzahl=0;  /* Warum muss das noch initialisiert werden?*/
	continue;
      }
      printf("WARNING at line %d: Syntax error: %s\n",original_line(i),buf);
      pcode[i].opcode=P_INVALID|P_NOCMD;
      pcode[i].panzahl=0;
      pcode[i].ppointer=NULL;
      pcode[i].argument=buf;
      continue;    
    } 
      /* j hat nun die passende Befehlnummer */
    pcode[i].opcode=comms[j].opcode|j;
    xtrim(buffer,TRUE,buffer);
    pcode[i].argument=strdup(buffer);

    if(comms[j].opcode==P_DATA) {
      printf("WARNING at line %d: Something is wrong. Data should have been treated already.\n",original_line(i));
    } else if(comms[j].opcode==P_LOOP) {/*Zugehoeriges DO suchen */
      pcode[i].integer=suchep(i-1,-1,P_DO,P_LOOP,P_DO);
      if(pcode[i].integer==-1)  structure_warning(original_line(i),zeile); /*Programmstruktur fehlerhaft */
    } else if(comms[j].opcode==P_WEND) {/*Zugehoeriges WHILE suchen */
      pcode[i].integer=suchep(i-1,-1,P_WHILE,P_WEND,P_WHILE);
      if(pcode[i].integer==-1)  structure_warning(original_line(i),zeile); /*Programmstruktur fehlerhaft */
    } else if(comms[j].opcode==P_NEXT) {/*Zugehoeriges FOR suchen */
      pcode[i].integer=suchep(i-1,-1,P_FOR,P_NEXT,P_FOR);
      if(pcode[i].integer==-1)  structure_warning(original_line(i),zeile); /*Programmstruktur fehlerhaft */
    } else if(comms[j].opcode==P_UNTIL) {/*Zugehoeriges REPEAT suchen */
      pcode[i].integer=suchep(i-1,-1,P_REPEAT,P_UNTIL,P_REPEAT);
      if(pcode[i].integer==-1)  structure_warning(original_line(i),zeile); /*Programmstruktur fehlerhaft */
    } else if(comms[j].opcode==P_ELSE) {/*Pruefen ob es ELSE IF ist. */
      char w1[strlen(buffer)+1],w2[strlen(buffer)+1];
      wort_sep(buffer,' ',TRUE,w1,w2);
      if(strcmp(w1,"IF")==0) {
        pcode[i].opcode=P_ELSEIF|j;
	strcpy(pcode[i].argument,w2);
        int l=strlen(pcode[i].argument);
        if(l>4) {
    	  if(strcmp(pcode[i].argument+l-5," THEN")==0) {
       // printf("Unnutzes THEN gefunden:\n");
    	    pcode[i].argument[l-5]=0;
    	  }
        }
      }
    } else if(comms[j].opcode==P_IF) {/*Unn"otiges THEN entfernen*/
      int l=strlen(pcode[i].argument);
      if(l>4) {
    	if(strcmp(pcode[i].argument+l-5," THEN")==0) {
       // printf("Unnutzes THEN gefunden:\n");
    	  pcode[i].argument[l-5]=0;
    	}
      }
    } 
    /*Argumente und Argument-Listen vorbereiten*/
    if(comms[j].pmax==0 || (pcode[i].opcode&PM_TYP)==P_SIMPLE) {
      pcode[i].panzahl=0;
      pcode[i].ppointer=NULL;
    } else if((pcode[i].opcode&PM_TYP)==P_ARGUMENT) {
      pcode[i].panzahl=0;
      pcode[i].ppointer=NULL;
    } else if((pcode[i].opcode&PM_TYP)==P_PLISTE) {
      int ii;
      pcode[i].panzahl=ii=count_parameters(pcode[i].argument);
      if((comms[j].pmin>ii && comms[j].pmin!=-1) || (comms[j].pmax<ii && comms[j].pmax!=-1))  
     	     printf("WARNING at line %d: Wrong number of parameters: %s.\n",original_line(i),comms[j].name); /*Programmstruktur fehlerhaft */
      if(ii==0) pcode[i].ppointer=NULL;
    }
     	 /* Einige Befehle noch nachbearbeiten */
    if(strcmp(zeile,"EXIT")==0) { /*Pruefen ob es EXIT IF ist. */
   	 char *w1,*w2;
     	 wort_sep_destroy(buffer,' ',TRUE,&w1,&w2);
     	 if(strcmp(w1,"IF")==0) {
	   pcode[i].opcode=P_EXITIF|j;
	   strcpy(pcode[i].argument,w2);
         } 
    } 
  }  /*  FOR */


#ifdef DEBUG 
  puts("PASS 2:");
#endif
  /* Pass 2, jetzt sind alle Labels und Proceduren bekannt. 
     Sprungmarken werden noch gesetzt, sowie zusaetzliche
     Variablen aus Pliste (CLR,LOCAL,DIM,INC,DEC) noch hinzugefuegt.
  */
  for(i=0; i<prglen;i++) {
    switch(pcode[i].opcode&PM_SPECIAL) {
    case P_ELSE: /* Suche Endif */
      pcode[i].integer=suchep(i+1,1,P_ENDIF,P_IF,P_ENDIF)+1;
      if(pcode[i].integer==0)  structure_warning(original_line(i),"ELSE"); /*Programmstruktur fehlerhaft */
      break;
    case P_ELSEIF: /* Suche Endif */
      pcode[i].integer=suchep(i+1,1,P_ENDIF,P_IF,P_ENDIF)+1;
      if(pcode[i].integer==0)  structure_warning(original_line(i),"ELSE IF"); /*Programmstruktur fehlerhaft */
      break;
    case P_IF: /* Suche Endif */
      pcode[i].integer=suchep(i+1,1,P_ENDIF,P_IF,P_ENDIF)+1;
      if(pcode[i].integer==0)  structure_warning(original_line(i),"IF"); /*Programmstruktur fehlerhaft */
      break;
    case P_WHILE: /* Suche WEND */
      pcode[i].integer=suchep(i+1,1,P_WEND,P_WHILE,P_WEND)+1;
      if(pcode[i].integer==0)  structure_warning(original_line(i),"WHILE"); /*Programmstruktur fehlerhaft */
      break;
    case P_FOR: /* Suche NEXT */
      pcode[i].integer=suchep(i+1,1,P_NEXT,P_FOR,P_NEXT)+1;
      if(pcode[i].integer==0)  structure_warning(original_line(i),"FOR"); /*Programmstruktur fehlerhaft */
      break;
    case P_SELECT:
    case P_CASE:
    case P_DEFAULT:
    case P_CONTINUE: {/* Suche CASE/DEFAULT/ENDSELECT */
      int p1=pcode[i].integer=suchep(i+1,1,P_ENDSELECT,P_SELECT,P_ENDSELECT);
      if(p1<0) {
        structure_warning(original_line(i),"SELECT/ENDSELECT"); /*Programmstruktur fehlerhaft */
      } else {
        int p2=pcode[i].integer=suchep(i+1,1,P_CASE,P_SELECT,P_ENDSELECT);
        int p3=pcode[i].integer=suchep(i+1,1,P_DEFAULT,P_SELECT,P_ENDSELECT);
	if(p2<0 || p2>p1) p2=p3;
	else if(p3<0 || p3>p1) ; 
	else p2=min(p2,p3);
	
	if(p2<0 || p2>p1) pcode[i].integer=p1+1;
	else pcode[i].integer=p2+1;
      }
      } break;
    case P_BREAK:
    case P_EXITIF: { /* Suche ende Schleife*/
      int j,f=0,o=0;
      for(j=i+1; (j<prglen && j>=0);j++) {
        o=pcode[j].opcode&PM_SPECIAL;
        if((o==P_LOOP || o==P_NEXT || o==P_WEND ||  o==P_UNTIL||  o==P_ENDSELECT)  && f<=0) break;
        if(o & P_LEVELIN) f++;
        if(o & P_LEVELOUT) f--;
      }
      if(j==prglen) { 
        structure_warning(original_line(i),"BREAK/EXIT IF"); /*Programmstruktur fehlerhaft */
        pcode[i].integer=-1;
      } else {
        if(o==P_ENDSELECT) pcode[i].integer=j; /* wichtig fuer compiler !*/
        else pcode[i].integer=j+1;
      }
      } break;
    case P_GOSUB: /* Suche Procedure */
      if(*(pcode[i].argument)=='&') pcode[i].integer=-1;  // TODO
      else {
        char buf[strlen(pcode[i].argument)+1];
	char *pos,*pos2;
        strcpy(buf,pcode[i].argument);
        pos=searchchr(buf,'(');
        if(pos!=NULL) {
          *pos=0;pos++;
          pos2=pos+strlen(pos)-1;
          if(*pos2!=')') {
	    printf("ERROR at line %d: Syntax error: GOSUB parameter list\n",original_line(i));
	    structure_warning(original_line(i),"GOSUB"); /*Programmstruktur fehlerhaft */
          } else pos2[0]=0;
        } else pos=buf+strlen(buf);
        pcode[i].integer=procnr(buf,1);
	/* Hier jetzt stage2 parameterliste aus Argument machen. Dabei 
	muss berücksichtigt werden, welche zielvariablen in der Procedur die Werte aufnehmen
	sollen. */
	pcode[i].panzahl=count_parameters(pos);
	if(pcode[i].panzahl!=procs[pcode[i].integer].anzpar) {
	  xberror(56,pcode[i].argument); /* Falsche Anzahl Parameter */
	  structure_warning(original_line(i),"GOSUB"); /*Programmstruktur fehlerhaft */
          pcode[i].opcode=P_INVALID|P_NOCMD;
          pcode[i].panzahl=0;
	}
        if(pcode[i].panzahl) {
          unsigned short ptypliste[pcode[i].panzahl];
	  get_ptypliste(pcode[i].integer,ptypliste,pcode[i].panzahl);
	  make_pliste2(procs[pcode[i].integer].anzpar,procs[pcode[i].integer].anzpar,
	      ptypliste,pos,&(pcode[i].ppointer),pcode[i].panzahl);
        }
      }
      break;
    case P_GOTO: /* Suche Label */
    /*  printf("Goto-Nachbearbeitung, <%s> \n",pcode[i].argument); */
      /* Wenn indirect, dann EVAL */
      if(*(pcode[i].argument)=='&') {
        pcode[i].opcode&=PM_COMMS;
	pcode[i].opcode|=P_EVAL;
      } else {
        pcode[i].integer=labelzeile(pcode[i].argument);
        /* Wenn label nicht gefunden, dann */
        if(pcode[i].integer==-1)  {
	  printf("ERROR at line %d: Label %s not found!\n",original_line(i),pcode[i].argument);
          structure_warning(original_line(i),"GOTO"); /*Programmstruktur fehlerhaft */
	  pcode[i].opcode|=P_INVALID;
        }
      }
      break;
    } /*  SWITCH */
    if((pcode[i].opcode&PM_TYP)==P_PLISTE) { /* Nachbearbeiten */
       int j=pcode[i].opcode&PM_COMMS;
     //  printf("OPS: %s   anz=%d\n",comms[j].name,pcode[i].panzahl);
       make_pliste2(comms[j].pmin,comms[j].pmax,
	(unsigned short *)comms[j].pliste,pcode[i].argument,&(pcode[i].ppointer),pcode[i].panzahl);
    } 
  }  /*  for */
  free(buffer);free(zeile);
  return(0);
}

static int add_label(char *name,int zeile,int dataptr) {
  labels[anzlabels].name=strdup(name);
  labels[anzlabels].zeile=zeile;
  labels[anzlabels].datapointer=dataptr;
  anzlabels++;
  return(anzlabels-1);
}

/*Raeume pcode struktor auf und gebe Speicherbereiche wieder frei.*/

void free_pcode(int l) {
  while(l>0) {
    l--;
    if(pcode[l].ppointer!=NULL) {
      free_pliste(pcode[l].panzahl,pcode[l].ppointer);
      pcode[l].ppointer=NULL;
    }
    if(pcode[l].rvalue!=NULL) {
      free_parameter(pcode[l].rvalue);
      free(pcode[l].rvalue);
      pcode[l].rvalue=NULL;
    }
    if(pcode[l].argument!=NULL) free(pcode[l].argument);
    if(pcode[l].extra!=NULL) free(pcode[l].extra);
    pcode[l].argument=NULL;
    pcode[l].extra=NULL;
    pcode[l].panzahl=0;
  }
  if(pcode) free(pcode);
  pcode=NULL;
}


/*Bereitet die Variablenliste einer Procedur oder Funktion vor*/

static int make_varliste(char *argument, int *l,int n) {
  char arg[strlen(argument)+1];
  char *w1,*w2;
  int i=0,e;
  char *r;
  int typ,subtyp,vnr;
  int flag;
  strcpy(arg,argument);
  e=wort_sep_destroy(arg,',',TRUE,&w1,&w2);
  while(e && i<n) {
    if(w1[0]=='V' && w1[1]=='A' && w1[2]=='R' && w1[3]==' ') {
      w1+=4;
      flag=1;
    } else flag=0;
    r=varrumpf(w1);
    typ=vartype(w1);
    if(typ&ARRAYTYP) {
      subtyp=typ&(~ARRAYTYP);
      typ=ARRAYTYP;
    } else subtyp=0;
    
    l[i++]=vnr=(add_variable(r,typ,subtyp,V_DYNAMIC,NULL) | flag*V_BY_REFERENCE);
    free(r);
    e=wort_sep_destroy(w2,',',TRUE,&w1,&w2);
  }
  return(i);
}

/*Prozedur in Liste hinzufuegen */

static int add_proc(char *name,char *pars,int zeile,int typ) {
  int ap,i;
  i=procnr(name,typ);
  if(i==-1) {
    procs[anzprocs].name=strdup(name);
    procs[anzprocs].typ=typ;
    procs[anzprocs].rettyp=vartype(name);
    procs[anzprocs].zeile=zeile;
    procs[anzprocs].anzpar=ap=count_parameters(pars);
    if(ap) {
      procs[anzprocs].parameterliste=(int *)malloc(sizeof(int)*ap);
      make_varliste(pars,procs[anzprocs].parameterliste,ap);
    } else procs[anzprocs].parameterliste=NULL;
//printf("ADDPROC: name=%s rettyp=%x\n",procs[anzprocs].name,procs[anzprocs].rettyp);
    anzprocs++;
    return(anzprocs-1);
  } else {
    printf("ERROR: Procedure/Function %s already exists at line %d.\n",name,original_line(procs[i].zeile));
    return(i);
  }
}


char *indirekt2(const char *funktion) {
  if(funktion && *funktion=='&') {
    char *ergebnis=s_parser(funktion+1);
    xtrim(ergebnis,TRUE,ergebnis);
    return(ergebnis);
  } else return(strdup(funktion));
}











#if 0
/*Sucht anhand der Programmtextzeilen
  (anstelle von PCODE)
  obsolet */
static int suche(int begin, int richtung, char *such,char *w1,char *w2) {
  int i,f=0;
  char nbuffer[MAXSTRLEN];
  char zbuffer[MAXSTRLEN];
  char sbuffer[MAXSTRLEN];
  char *zeile,*buffer,*b1,*b2;
  
  for(i=begin; (i<prglen && i>=0);i+=richtung) {
    xtrim(program[i],TRUE,zbuffer);
    wort_sep_destroy(zbuffer,'!',TRUE,&zeile,&buffer);
    wort_sep_destroy(zeile,' ',TRUE,&b1,&b2);
    strcpy(sbuffer+1,such); sbuffer[0]='|';
    strcat(sbuffer,"|");
    strcpy(nbuffer+1,b1); nbuffer[0]='|';
    strcat(nbuffer,"|");
    /*printf("SUCHE: %d <%s> <%s> %d\n",i,buffer,nbuffer,f);*/
    if(strstr(sbuffer,nbuffer)!=NULL && f==0) return(i);
    else if(strcmp(b1,w1)==0) f++;
    else if(strcmp(b1,w2)==0) f--;
  }
  return(-1);
}
#endif


/*  Kommando in Direktmodus auswerten (ohne Strukturhilfe vom PASS 1+2).*/
void kommando(char *cmd) {
  char buffer[strlen(cmd)+1];
  char *w1,*w2,*pos;
  char zeile[strlen(cmd)+1];
  int i,a,b,e,l;
  wort_sep2(cmd," !",TRUE,zeile,buffer);
  xtrim2(zeile,TRUE,zeile);
// printf("KOMMANDO: <%s>\n",zeile);

/*  1. Analysiere erstes Zeichen der Zeile*/

  switch(*zeile) {
  case 0:
  case '\'':
  case '#':
  case '!':
    return;  /* Kommentar oder leerzeile */
  case '@':
    c_gosub(zeile+1);
    return;
  case '~':
    c_void(zeile+1);
    return;
  case '&':
    {
      char *test=s_parser(zeile+1);
      kommando(test);
      free(test);
    }
    return;
  case '(':
  case '-':
  case '+':
     printf("%.13g\n",parser(zeile));
     return;
  }
  if(isdigit(*zeile)) {
     printf("%.13g\n",parser(zeile));
     return;
  }
  
  
  /* 2. Betrachte erstes Wort*/
  
  e=wort_sep_destroy(zeile,' ',TRUE,&w1,&w2);
  l=strlen(w1);
  if(w1[l-1]==':')             return;  /* nixtun, label */
  if(w1[l-1]=='=') {
    w1[l-1]=0;
    xzuweis(w1,w2);
    return;
  }
  if(*w2=='=') {
    xzuweis(w1,++w2);
    return;
  }

  if((pos=searchchr2(w1,'='))!=NULL) {
    *pos++=0;
    if(e==2) w1[l]=' ';
    xzuweis(w1,pos);
    return;
  }

/*  Die PM_FLAGS sind hier irrelevant, also kein P_PREFETCH oder 
    P_INVALID.*/
  

  /* Restliche Befehle */
  i=find_comm_guess(w1,&a,&b);
  if(i!=-1) {
    switch(comms[i].opcode&PM_TYP) {
      case P_IGNORE: xberror(38,w1); /* Befehl im Direktmodus nicht moeglich */return; 
      case P_ARGUMENT: (comms[i].routine)(w2); return;
      case P_SIMPLE: (comms[i].routine)(); return;
      case P_PLISTE: {
        PARAMETER *plist;
        int e=make_pliste(comms[i].pmin,comms[i].pmax,(unsigned short *)comms[i].pliste,w2,&plist);
        if(e>=comms[i].pmin) (comms[i].routine)(plist,e);
	if(e!=-1) free_pliste(e,plist);
	}
	return;
      default: xberror(38,w1); /* Befehl im Direktmodus nicht moeglich */
    }
  } else if(a!=b) {
     printf("Command needs to be more specific ! <%s...%s>\n",comms[a].name,comms[b].name);
  } else xberror(32,w1);  /* Syntax Error */
}




/* programmlauf setzt voraus, dass die Strukturen durch init_program vorbereitet sind*/

void programmlauf(){
  if(is_bytecode) {  
    PARAMETER *p;
    int n;
  #if DEBUG
    printf("Virtual Machine: %d bytes.\n",programbufferlen);
  #endif
    STRING bcpc;  /* Bytecode holder */
    bcpc.pointer=programbuffer;
    bcpc.len=programbufferlen;
    p=virtual_machine(bcpc,0,&n,NULL,0);
    dump_parameterlist(p,n);  
    free_pliste(n,p);
    return;
  }
  int opc;
  int isp=sp;
#ifdef DEBUG
  int timer;
#endif
  while(batch && pc<prglen && pc>=0 && sp>=isp)  {
    if(echoflag) printf("%s\n",program[pc]);
#ifdef DEBUG
    timer=clock();
#endif
    opc=pc;
      
    // printf("OPCODE=$%x/$%x $%x",pcode[opc].opcode,P_DEFFN,P_PROC);
      
    /*  Erst flags auswerten: */
    
    /*  Prefetch kommt meist zusammen mit P_IGNORE vor, wenn es mit 
    P_ARGUMENT (z.B. bei GOTO) vorkommt, kann abgekürzt werden. Wenn es mit P_SIMPLE
    vorkommt (bei P_CONTINUE), kann auch abgekürzt werden, wenn nicht im direktmodus. */
    
    if((pcode[opc].opcode&P_PREFETCH)==P_PREFETCH) {
      pc=pcode[opc].integer;
      continue;
    } else pc++;
      
    if((pcode[opc].opcode&P_INVALID)==P_INVALID)  {xberror(32,program[opc]); /* Syntax error */break;}
    // printf("programmlauf: %x\n",pcode[opc].opcode);
      
    /*  Jetzt Spezialanweisungen, zuerst die mit P_NOCMD */
    switch(pcode[opc].opcode) {
    case P_ZUWEIS: {
      int vnr=pcode[opc].integer;
      int typ=variablen[vnr].typ;
      int ii=pcode[opc].panzahl;
      // printf("ZUWEIS: <%s> --> %s\n",pcode[opc].argument,variablen[vnr].name);
      if(ii) {   /* Mit Index ....*/
        if(typ==ARRAYTYP) {
	  int dim=variablen[vnr].pointer.a->dimension;
  	  int *indexliste=malloc(ii*sizeof(int));
	  get_indexliste(pcode[opc].ppointer,indexliste,ii);
	  if(ii!=dim) xberror(18,"");  /* Falsche Anzahl Indizies */
	  if(pcode[opc].rvalue) {
	     PARAMETER *par=calloc(1,sizeof(PARAMETER));
  	     make_parameter_stage3(pcode[opc].rvalue,(PL_CONSTGROUP|variablen[vnr].pointer.a->typ),par);
             zuweispbyindex(vnr,indexliste,ii,par);
	     free_parameter(par);
	     free(par);
	  } else {  /* Kann entfernt werden, da nicht mehr benötigt....*/
	    printf("Something is wrong: //zuweisxbyindex");
	  //  zuweisxbyindex(vnr,indexliste,ii,pcode[opc].argument,pcode[opc].atyp);
	  }
	  free(indexliste);
        } else xberror(18,"");  /* Falsche Anzahl Indizies */
      } else {   /* Ohne indizies  ...*/
        if(pcode[opc].rvalue) {
	  PARAMETER *par=calloc(1,sizeof(PARAMETER));
	  int ap=variablen[vnr].typ;
	  if(ap==ARRAYTYP) ap|=variablen[vnr].pointer.a->typ;
  	  make_parameter_stage3(pcode[opc].rvalue,(PL_CONSTGROUP|ap),par);
          zuweis_v_parameter(&variablen[vnr],par);
	  free_parameter(par);free(par);
	} else {
	  printf("Something is wrong: //zuweisxbyindex");
	//  zuweisxbyindex(vnr,NULL,0,pcode[opc].argument,pcode[opc].atyp);
	}
      }	  
    }
    case P_LABEL:
    case P_NOTHING: break;
    case (P_EVAL|P_NOCMD):  kommando(program[opc]); break;
    case P_PROC:
      xberror(36,program[opc]); /*Programmstruktur fehlerhaft !*/
      printf("END missing?\n");
      break;
    default: 
      if((pcode[opc].opcode&PM_COMMS)>=anzcomms) {
	printf("ERROR: invalid command #%d/%d (%x)\n",(int)(pcode[opc].opcode&PM_COMMS),anzcomms,(int)pcode[opc].opcode);
	xberror(36,program[opc]); /*Programmstruktur fehlerhaft !*/
	break;
      }
      /*Jetzt noch Sonderfälle abfangen: */
      switch(pcode[opc].opcode&PM_SPECIAL) {
      case P_GOSUB: {
	int procnr=pcode[opc].integer;
	if(procnr==-1)   xberror(19,pcode[opc].argument); /* Procedure nicht gefunden */
        else {
	  unsigned short ptypliste[pcode[opc].panzahl];
	  get_ptypliste(procnr,ptypliste,pcode[opc].panzahl);
	  
  	  PARAMETER *plist;
	  int e=make_pliste3(pcode[opc].panzahl,pcode[opc].panzahl,ptypliste,
             pcode[opc].ppointer,&plist,pcode[opc].panzahl);

	  call_sub_with_parameterlist(pcode[opc].integer,plist,pcode[opc].panzahl);
          if(e!=-1) free_pliste(e,plist);
        }

	} break;
      case P_VOID:
        c_void(pcode[opc].argument);
        break;
      default:
        switch(pcode[opc].opcode&PM_TYP) {
        case P_EVAL:   kommando(program[opc]);
        case P_IGNORE: break;
        case P_SIMPLE: (comms[pcode[opc].opcode&PM_COMMS].routine)(NULL); break;
        case P_ARGUMENT: (comms[pcode[opc].opcode&PM_COMMS].routine)(pcode[opc].argument); break;
        case P_PLISTE: {
          /*Hier ist ja schon das meiste vorbereitet !*/
          PARAMETER *plist;
          int i=pcode[opc].opcode&PM_COMMS;
          int e=make_pliste3(comms[i].pmin,comms[i].pmax,(unsigned short *)comms[i].pliste,
             pcode[opc].ppointer,&plist,pcode[opc].panzahl);
          (comms[i].routine)(plist,e);
          if(e!=-1) free_pliste(e,plist);
        } break;
        default:
	    printf("something is wrong: %x %s\n",(int)pcode[opc].opcode,program[opc]);
        }
      }
    }
#ifdef DEBUG
    ptimes[opc]=(int)((clock()-timer)/1000);  /* evaluiert die Ausfuehrungszeit der Programmzeile */
#endif
  }
}

/* Programm beenden und Aufr"aumen. */


void quit_x11basic(int c) {
#ifdef ANDROID
  invalidate_screen();
  sleep(1);
#endif
#ifndef NOGRAPHICS
  close_window(&window[usewindow]); 
#endif
  /* Aufr"aumen */
  clear_program();
  free_pcode(prglen);
  if(programbuffer) free(programbuffer);
 // if(program) free(program); machen wir nicht, gibt aerger beim xbc - compiler
#ifdef CONTROL
  cs_exit();
#endif
#ifdef USE_GEM
// APPL_EXIT
// close VDI workstation
#endif
  exit(c); 
}
