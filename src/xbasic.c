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



                       VERSION 1.28

            (C) 1997-2021 by Markus Hoffmann

(https://x11-basic.codeberg.page/ http://x11-basic.sourceforge.net/)

*/

 /* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <strings.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif

#ifdef ATARI
char *strdup(const char *s);
#endif

#include <math.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "xbasic.h"
#include "memory.h"
#include "type.h"
#include "parser.h"
#include "parameter.h"
#include "kommandos.h"
#include "gkommandos.h"
#include "file.h"
#include "array.h"
#include "x11basic.h"
#include "wort_sep.h"
#include "bytecode.h"
#include "virtual-machine.h"
#include "number.h"
#include "functions.h"
#include "io.h"


const char libversion[]=VERSION;           /* Programmversion           */
const char libvdate[]=VERSION_DATE;

#ifdef CONTROL
const char xbasic_name[]="csxbasic";
#elif defined DOOCS
const char xbasic_name[]="doocsxbasic";
#elif defined TINE
const char xbasic_name[]="tinexbasic";
#elif defined USE_SDL
const char xbasic_name[]="sdlxbasic";
#elif defined FRAMEBUFFER
const char xbasic_name[]="fbxbasic";
#else
const char xbasic_name[]="xbasic";
#endif
int echoflag=0,batch=0,errcont=0,breakcont=0,everyflag=0;
int errorpc=-1,errorpctype=0,breakpc=-1,breakpctype=0;


static int *linetable=NULL;   /* for correctly count splitted lines*/

static int add_label(char *name,int zeile,int dataptr);
static int add_proc(char *name,char *pars,int zeile,int typ);

static int oldprglen=0;

extern int verbose;

/*return the original line (accounting for splitted lines) */
int original_line(int i) {
  if(linetable==NULL || i<0) return i;
  int a=i;
  for(int j=0;j<i;j++) a+=linetable[j];
  return a;
}


/* Bytecode spezifica. */

int is_bytecode=0;


static void do_relocation(char *adr,unsigned char *fixup, int l) {
  int i=0;
  uint32_t ll;
 // printf("Relocation table:\n");
 // memdump(fixup,l);
  while(i<l) {
    if(fixup[i]==0) break;
    else if(fixup[i]==1) adr+=254;
    else {
      adr+=fixup[i];
      memcpy(&ll,adr,sizeof(uint32_t));
      #ifdef IS_BIG_ENDIAN
        LWSWAP((char *)&ll);
      #endif
      ll+=POINTER2INT(adr);
      #ifdef IS_BIG_ENDIAN
        LWSWAP((char *)&ll);
      #endif
      memcpy(adr,&ll,sizeof(uint32_t));
    }
  }
}

void print_bytecode_info(BYTECODE_HEADER *bytecode) {
  printf("Info: bytecode version: %04x\n"
  	 "  Size of   Text-Segment: %d\n"
  	 "  Size of roData-Segment: %d\n"
  	 "  Size of   Data-Segment: %d\n",bytecode->version,(int)bytecode->textseglen,
    					(int)bytecode->rodataseglen,(int)bytecode->sdataseglen);
  printf("  Size of    bss-Segment: %d\n"
  	 "  Size of String-Segment: %d\n",(int)bytecode->bssseglen,(int)bytecode->stringseglen);
  printf("  Size of Symbol-Segment: %d (%d symbols)\n",(int)bytecode->symbolseglen,(int)(bytecode->symbolseglen/sizeof(BYTECODE_SYMBOL)));
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

COMPILE_BLOCK *bytecode_init(char *adr) {
  int i,a,typ;
  char *name;
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
        
    /* Set up the data buffers */
    
    /* Initialize compile block: */

    COMPILE_BLOCK *cb=calloc(sizeof(COMPILE_BLOCK),1);
    cb->bc_version=bytecode->version;
    cb->status=1; /*cannot free rodataseg and textseg*/
    cb->textseg=adr+sizeof(BYTECODE_HEADER);
    cb->textseglen=bytecode->textseglen;
    cb->rodataseg=&adr[sizeof(BYTECODE_HEADER)+bytecode->textseglen];
    cb->rodataseglen=bytecode->rodataseglen;
    cb->bsseglen=bytecode->bssseglen;
    cb->dataseg=adr+bytecode->textseglen+bytecode->rodataseglen+sizeof(BYTECODE_HEADER);
    cb->dataseglen=bytecode->sdataseglen+bytecode->dataseglen;
    databuffer=cb->dataseg;
    databufferlen=bytecode->sdataseglen;
    
    cb->bsseg=cb->stringseg=&adr[sizeof(BYTECODE_HEADER)+
                 bytecode->textseglen+
		 bytecode->rodataseglen+
		 bytecode->sdataseglen+
		 bytecode->dataseglen];

    /* Now create all variables from symbol table. Afterwards the
       symbol table and the string segment is not needed anymore.
       */
    cb->symtab=(BYTECODE_SYMBOL *)(adr+sizeof(BYTECODE_HEADER)+
                                   bytecode->textseglen+
		                   bytecode->rodataseglen+
		                   bytecode->sdataseglen+
				   bytecode->dataseglen+
				   bytecode->stringseglen);
    a=bytecode->symbolseglen/sizeof(BYTECODE_SYMBOL);
    if(a>0) {
      for(i=0;i<a;i++) {
	#ifdef IS_BIG_ENDIAN
	  LWSWAP((char *)&cb->symtab[i].name);
	  LWSWAP((char *)&cb->symtab[i].adr);
	#endif
        if(cb->symtab[i].typ==STT_OBJECT) {
	  typ=cb->symtab[i].subtyp;
	  if(cb->symtab[i].name) {
	    name=&cb->stringseg[cb->symtab[i].name];
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
	  else                    add_variable(name,typ,0,V_STATIC,cb->bsseg+cb->symtab[i].adr);
//#ifdef ANDROID	  
//    sprintf(buffer,"BSSSEG auf %08x ",cb->bsseg);
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
    if(verbose>1) print_bytecode_info(bytecode);
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
    if(bytecode->bssseglen>0) bzero(cb->bsseg,cb->bsseglen);
#ifdef ANDROID
    backlog("bytecode_init done.");
#endif
    return(cb);
  } else {
    printf("VM: ERROR, file format not recognized. $%02x $%02x\n",adr[0],adr[1]);
    return(NULL);
  }
}


/*
 * Check if this is a valid bytecode and if so, fix the values (according to 
 * Endianess).
 * Return: 0 -- OK
 *        -1 -- no bytecode
 *        -2 -- wrong bytecode version
 */

int fix_bytecode_header(BYTECODE_HEADER *bytecode) {
  if(((char *)bytecode)[0]==BC_BRAs && ((char *)bytecode)[1]==sizeof(BYTECODE_HEADER)-2) {
    if(verbose>0) printf("Bytecode header found (V.%x)\n",bytecode->version);
    #ifdef IS_BIG_ENDIAN
    WSWAP((char *)&bytecode->version);
    #endif
    if(bytecode->version!=BC_VERSION) {
      printf("ERROR: This bytecode was compiled for a different version of "
      "X11-Basic.\n Please consider to recompile it from the .bas file.\n");
      return(-1);
    }
#ifdef IS_BIG_ENDIAN
    LWSWAP((char *)&bytecode->textseglen);
    LWSWAP((char *)&bytecode->rodataseglen);
    LWSWAP((char *)&bytecode->sdataseglen);
    LWSWAP((char *)&bytecode->dataseglen);
    LWSWAP((char *)&bytecode->bssseglen);
    LWSWAP((char *)&bytecode->symbolseglen);
    LWSWAP((char *)&bytecode->stringseglen);
    LWSWAP((char *)&bytecode->relseglen);
    WSWAP((char *)&bytecode->flags);
#endif
    return(0);
  } else return(-1);
}

/* make sure that the memory area allocated is large enough
 * for bss segment.
 * reallocates programbuffer, returns new progambufferlen.
 */
int bytecode_make_bss(BYTECODE_HEADER *bytecode,char **adr,int len) {
  if(bytecode->bssseglen>bytecode->relseglen+bytecode->stringseglen+bytecode->symbolseglen) {
    len+=bytecode->bssseglen-bytecode->stringseglen-bytecode->symbolseglen;
    *adr=realloc(*adr,len);
  #ifdef ATARI
    if(*adr==NULL) {
      perror("malloc");
      sleep(5);
    }
  #endif
  }
  return(len);
}


/* Routine zum Laden eines Programms */

int mergeprg(const char *fname) {
  /* Filelaenge rauskriegen */

  FILE *dptr=fopen(fname,"rb"); 
  int len=lof(dptr); 
  fclose(dptr);
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
    programbufferlen=bytecode_make_bss(bytecode,&programbuffer,programbufferlen);
    COMPILE_BLOCK *cb=bytecode_init(programbuffer); 
    if(!cb) return(-1); /* something is wrong... */
    free(cb);  /* We do not need this here */
    return(0);
  } else {
    
   /* Zeilenzahl herausbekommen */
    char *pos=programbuffer;
    oldprglen=prglen;
    int i=prglen=0;
    
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

/* Durchsucht die Kommandoliste (möglichst effektiv) und 
   findet den Bereich in der alphabetisch sortierten Liste 
   welcher auf das Suchwort passt.
   Rückgabe: 
   -1 : Kein eindeutiges match gefunden, 
        alles zwischen guessa und geuessb ist möglich. 
	Wenn guessa==guessb, dann wurde nix gefunden.
 */

static int find_comm_guess(const char *suchwort,int *guessa,int *guessb) {
  int i=0,a=anzcomms-1,b;
  for(b=0; b<strlen(suchwort); b++) {
    while(suchwort[b]>(comms[i].name)[b] && i<a) i++;
    while(suchwort[b]<(comms[a].name)[b] && a>i) a--;
    if(i==a) break;
  }
  *guessa=i;
  *guessb=a;
  if((i==a && !strncmp(suchwort,comms[i].name,strlen(suchwort))) ||
     (i!=a && !strcmp(suchwort,comms[i].name)) ) {
#ifdef DEBUG
      if(b<strlen(suchwort)) printf("Command %s completed --> %s\n",suchwort,comms[i].name);
#endif
     return(i);
  }
  return(-1);
}





/*****************************************************

Programmvorbereitung und precompilation
Rückgabewert: 
0 -- OK
-1 -- Error, programm kann nicht ausgef�ührt werden
>0 -- Warnings, aber man kann loslaufen.

******************************************************/

int init_program(int prglen) {
  char *expr,*pos2,*pos3,*buffer=NULL,*zeile=NULL;  
  char *p;
  int i,j;
  int len;
  int return_value=0;
  clear_program();
  free_pcode(oldprglen);

  init_pcode(prglen); /*Speicher für pcode allozieren und mit 0 füllen */ 
  
  /* Label-, Procedur- und Variablenliste Erstellen und p_code transformieren*/  
  for(i=0; i<prglen;i++) {
    zeile=realloc(zeile,strlen(program[i])+1);
    buffer=realloc(buffer,strlen(program[i])+1);
/*löschen nicht nötig, da das init_pcode macht.*/
//    code[i].opcode=0;       /*Typ und Kommandonummer*/
//    pcode[i].panzahl=0;       /*Anzahl Parameter*/
//    pcode[i].ppointer=NULL;   /*Zeiger auf Parameterliste*/
//    pcode[i].argument=NULL; /*String als argument*/
//    pcode[i].etyp=PE_NONE;   /* fuer Kommentare */
//    pcode[i].extra=NULL;   /*Extra string fuer Kommentare*/
    pcode[i].integer=-1;
    /* Leerzeichen am Anfang der Zeile überspringen:*/
    p=program[i];
    while(*p==' ' || *p==9) p++;
    
    /* Seitenkommentar behandeln:*/
    if(wort_sep2(p," !",TRUE,zeile,buffer)==2) {  /* Kommentare abseparieren*/
      pcode[i].etyp=PE_COMMENT;
      pcode[i].extra=strdup(buffer);
    }
#if defined DEBUG 
    printf("Zeile %d (%d) : %s\n",i,original_line(i),zeile);
#endif
    switch(*zeile) {
    case 0:     /* Leere Zeile */
      pcode[i].opcode=P_NOTHING;
      continue;    
    case '\'':
    case '#':
    case '!':
      pcode[i].opcode=P_REM;
      pcode[i].integer=(int)*zeile;  /* Welche Art rem ?*/
      pcode[i].argument=strdup(zeile+1);
      continue;
    }


    /* Ab hier brauchen wir Gro�bucstaben und Formatierung.*/
    xtrim(zeile,TRUE,zeile);
    /* Zeile hat jetzt keine Blanks mehr vorne und hinten, 
       keine doppelten Blanks mehr und alles is Grossbuchstaben
       ausser in Anführungszeichen. Ausserdem sind Kommentare am 
       Ende der Zeile abgetrennt.
     */

    /* Labels finden*/
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

    /* Kommandos REM, HELP und DATA */

    if(!strncmp(zeile,"DATA ",5)) {
      pcode[i].opcode=P_DATA;
      char *content=zeile+5;
      
#ifdef DEBUG
      printf("DATA Statement found in line %d (%d). <%s>\n",i,original_line(i),content);
#endif
      databuffer=realloc(databuffer,databufferlen+strlen(content)+2);
      if(databufferlen) databuffer[databufferlen++]=',';
      pcode[i].integer=strlen(content);
      pcode[i].argument=strdup(content);
      memcpy(databuffer+databufferlen,content,pcode[i].integer);
      
      databufferlen+=pcode[i].integer;
      databuffer[databufferlen]=0;
      continue;
    } else if(!strncmp(zeile,"REM ",4)) {
      ;
    } else if(!strncmp(zeile,"HELP ",5)) {
      ; 
    } else {
      xtrim2(zeile,TRUE,zeile);  /* Unnötige blanks vor Operatoren entfernen */
    }


    /* Erstes Wort abtrennen: 
       Wie soll man eine Zuweisung erkennen, wenn nach der
       Variable und vor dem Gleichzeichen ein Blank ist?
       Problematisch sind z.B. :
       REM =test=1
       HELP *h*
       DATA ==========
       REM (hallo)=4
    
     */
  
    if(wort_sep(zeile,' ',TRUE,zeile,buffer)==0) {
      pcode[i].opcode=P_NOTHING;
      continue;
    }
    
    switch(*zeile) {
    case '@':
      pcode[i].argument=strdup(zeile+1);
      pcode[i].opcode=P_GOSUB|find_comm("GOSUB");
      continue;
    case '~':
      pcode[i].argument=strdup(zeile+1);
      pcode[i].opcode=P_VOID|find_comm("VOID");
      continue;
    }

    int typ=(PROC_PROC*(strcmp(zeile,"PROCEDURE")==0) | 
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
	  return_value|=1;
	} else {
	  *expr++=0;
	  pcode[i].argument=strdup(expr);
        }
      } else pcode[i].opcode=P_PROC;
      pos2=searchchr(buffer,'(');
      if(pos2 != NULL) {
        pos2[0]=0;pos2++;
        pos3=pos2+strlen(pos2)-1;
        if(pos3[0]!=')') {
	  printf("WARNING at line %d: ==> Syntax error: parameter list\n",original_line(i));
	  return_value|=1;
        } else *pos3++=0;
      } else pos2=zeile+strlen(zeile);
      pcode[i].integer=add_proc(buffer,pos2,i,typ);
      continue;
    } 
    /* Rest Transformieren    */
    j=-1;
    if(!strcmp(zeile,"ON")) {
       // TODO: Sonderbehandlung ON  
        char *w1,*w2,w3[strlen(buffer)+1],w4[strlen(buffer)+1];
	wort_sep_destroy(buffer,' ',TRUE,&w1,&w2);
	wort_sep(w2,' ',TRUE,w3,w4);
	// printf("Onspecial: <%s> <%s> --<%s>-- / <%s>\n",w1,w2,w3,w4);
         if(!strcmp(w1,"ERROR") || !strcmp(w1,"BREAK") || !strcmp(w1,"MENU")) {
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
      if(pos!=NULL) { /* Es ist eine Zuweisung */
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
	      if(*argument) {  /*Idicies sind da */
                /* Subarray-Zuweisung..., */
		
		/*  Erstmal muss das Argument in eine Indexliste verwandelt werden, die muss dann in 
		pcode abgelegt werden.
		*/
		pcode[i].panzahl=count_parameters(argument);   /* Anzahl indizes z"ahlen*/
                if(pcode[i].panzahl>0) { 
  		  pcode[i].ppointer=calloc(pcode[i].panzahl,sizeof(PARAMETER));
		  make_preparlist(pcode[i].ppointer,argument);
		  // dump_parameterlist(pcode[i].ppointer,pcode[i].panzahl);  
                }
		// xberror(9,zeile); /*not implemented*/
		/* Das einzige Problem ist hier, wie unterscheidet man nun die Subarray-Zuweisung
		von einer einfachen Zuweisung zum Array-Element? Es gibt keinen Typ-Parameter 
		for den lvalue im P_CODE. Ein hinweis kann höchstens pcode[i].atyp geben.*/
	      }
        } else {
	  if(pcode[i].atyp&ARRAYTYP) printf("WARNING: type mismatch in assignment at line %d.\n",original_line(i));
          if(e>1) {  /*Idicies sind da */
	    pcode[i].integer=add_variable(r,ARRAYTYP,typ,V_DYNAMIC,NULL);
	    pcode[i].panzahl=count_parameters(argument);   /* Anzahl indizes z"ahlen*/
	    pcode[i].ppointer=calloc(pcode[i].panzahl,sizeof(PARAMETER));
	
            /*hier die Indizies in einzelne zu evaluierende Ausdruecke
	      separieren*/
	      
	    make_preparlist(pcode[i].ppointer,argument);
          } else if(e==1) {
  	    pcode[i].panzahl=0;
	    pcode[i].ppointer=NULL;
	    pcode[i].integer=add_variable(r,typ,0,V_DYNAMIC,NULL);
	  } else { /* e=0 also Syntaxfehler */
  	    pcode[i].panzahl=0;
	    pcode[i].ppointer=NULL;
	    pcode[i].integer=-1;
	  }
	  if((typ&TYPMASK)!=(typ2&TYPMASK) && ((typ&TYPMASK)==STRINGTYP || (typ2&TYPMASK)==STRINGTYP)) {
	    printf("WARNING: type mismatch in assignment at line %d.\n",original_line(i));
	    return_value|=1;
          }
	}
	if(pcode[i].integer==-1) {
	  printf("ERROR at line %d: variable could not be created.\n",original_line(i));
	  return_value|=-1;
	}
	free(r);
	free(buf);
        /* Jetzt noch die rechte Seite behandeln....*/
        pcode[i].rvalue=calloc(1,sizeof(PARAMETER));
	// printf("Rechte Seite <%s> typ=%x\n",pcode[i].argument,pcode[i].atyp);
        e=make_parameter_stage2(pcode[i].argument,PL_CONSTGROUP|(pcode[i].atyp&BASETYPMASK),pcode[i].rvalue);
	if(e<0) {
	  printf("make_parameter_stage2 failed. <%s>\n",pcode[i].argument);
	  return_value|=-1;
        }
	pcode[i].rvalue->panzahl=0;  /* Warum muss das noch initialisiert werden?*/
	continue;
      }
      printf("WARNING at line %d: Syntax error: %s\n",original_line(i),buf);
      return_value|=1;
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
      return_value|=1;
    } else if(comms[j].opcode==P_LOOP) {       /* Zugehoeriges DO suchen */
      pcode[i].integer=suchep(i-1,-1,P_DO,P_LOOP,P_DO);
      if(pcode[i].integer==-1) {
        structure_warning(original_line(i),zeile); /*Programmstruktur fehlerhaft */
	return_value|=-1;
      }
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
      if(!strcmp(w1,"IF")) {
        pcode[i].opcode=P_ELSEIF|j;
	strcpy(pcode[i].argument,w2);
        int l=strlen(pcode[i].argument);
        if(l>4) {
    	  if(!strcmp(pcode[i].argument+l-5," THEN")) { /* Unnutzes THEN gefunden  */
    	    pcode[i].argument[l-5]=0;
    	  }
        }
      }
    } else if(comms[j].opcode==P_IF) { /*Unnötiges THEN entfernen*/
      int l=strlen(pcode[i].argument);
      if(l>4) {
    	if(!strcmp(pcode[i].argument+l-5," THEN")) { /* Unnutzes THEN gefunden  */
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
      if((comms[j].pmin>ii && comms[j].pmin!=-1) || (comms[j].pmax<ii && comms[j].pmax!=-1))  {
        printf("WARNING at line %d: Wrong number of parameters: %s.\n",original_line(i),comms[j].name); /*Programmstruktur fehlerhaft */
        return_value|=1;
      }
      if(ii==0) pcode[i].ppointer=NULL;
    }
     	 /* Einige Befehle noch nachbearbeiten */
    if(!strcmp(zeile,"EXIT")) { /* Pruefen ob es EXIT IF ist. */
      char *w1,*w2;
      wort_sep_destroy(buffer,' ',TRUE,&w1,&w2);
      if(!strcmp(w1,"IF")) {
        pcode[i].opcode=P_EXITIF|j;
        strcpy(pcode[i].argument,w2);
      } else {
      /* 
        hier prüfen ob es innerhalb einer Schleife ist, dann P_BREAK
        wenn nicht, dann normal EXIT kommando ausführen lassen.
        Die Prüfung geht erst im pass 2, also hier nur vormerken.
        */
        pcode[i].opcode=P_EXIT|j;
      }
    } 
  }  /*  FOR */

  /* PASS 2, jetzt sind alle Labels und Proceduren bekannt. 
     Sprungmarken werden noch gesetzt, sowie zusaetzliche
     Variablen aus Pliste (CLR,LOCAL,DIM,INC,DEC) noch hinzugefuegt.
  */
  for(i=0; i<prglen;i++) {
    switch(pcode[i].opcode&PM_SPECIAL) {
    case P_IF:     /* Suche Endif */
    case P_ELSEIF: /* Suche Endif */
    case P_ELSE:   /* Suche Endif */
      pcode[i].integer=suchep(i+1,1,P_ENDIF,P_IF,P_ENDIF)+1;
      if(pcode[i].integer==0)  {
        structure_warning(original_line(i),"IF/ELSE/ELSE IF"); /* Programmstruktur fehlerhaft */
        return_value|=-1;
      }
      break;
    case P_WHILE: /* Suche WEND */
      pcode[i].integer=suchep(i+1,1,P_WEND,P_WHILE,P_WEND)+1;
      if(pcode[i].integer==0) {
        structure_warning(original_line(i),"WHILE"); /*Programmstruktur fehlerhaft */
        return_value|=-1;
      }
      break;
    case P_FOR: /* Suche NEXT */
      pcode[i].integer=suchep(i+1,1,P_NEXT,P_FOR,P_NEXT)+1;
      if(pcode[i].integer==0) {
        structure_warning(original_line(i),"FOR"); /*Programmstruktur fehlerhaft */
        return_value|=-1;
      }
      break;
    case P_SELECT:
    case P_CASE:
    case P_DEFAULT:
    case P_CONTINUE: {/* Suche CASE/DEFAULT/ENDSELECT */
      int p1=pcode[i].integer=suchep(i+1,1,P_ENDSELECT,P_SELECT,P_ENDSELECT);
      if(p1<0) {
        structure_warning(original_line(i),"SELECT/CASE/DEFAULT/ENDSELECT"); /* Programmstruktur fehlerhaft */
	return_value|=-1;
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
    case P_EXIT:
    case P_EXITIF: { /* Suche ende Schleife*/
      int j=sucheloopend(i+1);
      if(j<0) { 
        if((pcode[i].opcode&PM_SPECIAL)==P_EXIT) {
          /* EXIT ohne Parameter dann als normales Kommando aufrufen.*/
          pcode[i].opcode=P_PLISTE|(pcode[i].opcode&PM_COMMS);
          pcode[i].panzahl=0;
          pcode[i].ppointer=NULL;
        } else {
          structure_warning(original_line(i),"BREAK/EXIT IF"); /*Programmstruktur fehlerhaft */
          pcode[i].integer=-1;
	  return_value|=-1;
        }
      } else {
        /* Ansonsten EXIT ohne Parameter wie BREAK behandeln */
        if((pcode[i].opcode&PM_SPECIAL)==P_EXIT) pcode[i].opcode=P_BREAK|(pcode[i].opcode&PM_COMMS); 
        if((pcode[j].opcode&PM_SPECIAL)==P_ENDSELECT) pcode[i].integer=j; /* wichtig fuer compiler !*/
        else pcode[i].integer=j+1;
      }
      } break;
    case P_GOSUB: /* Suche Procedure */
      if(*(pcode[i].argument)=='&') pcode[i].integer=-1;  // TODO
      else {
        char buf[strlen(pcode[i].argument)+1];
	char *pos2;
        strcpy(buf,pcode[i].argument);
        char *pos=searchchr(buf,'(');
        if(pos!=NULL) {
          *pos=0;pos++;
          pos2=pos+strlen(pos)-1;
          if(*pos2!=')') {
	    printf("ERROR at line %d: Syntax error: GOSUB parameter list\n",original_line(i));
	    structure_warning(original_line(i),"GOSUB"); /*Programmstruktur fehlerhaft */
	    return_value|=-1;
          } else pos2[0]=0;
        } else pos=buf+strlen(buf);
        pcode[i].integer=procnr(buf,1);
	/* Hier jetzt stage2 parameterliste aus Argument machen. Dabei 
	muss berücksichtigt werden, welche zielvariablen in der Procedur die Werte aufnehmen
	sollen. */
	pcode[i].panzahl=count_parameters(pos);
        if(pcode[i].integer>=0) {  /* Es kann ja sein, dass die Procedure nicht definiert ist. */
          if(pcode[i].panzahl!=procs[pcode[i].integer].anzpar) {
	    xberror(56,pcode[i].argument); /* Falsche Anzahl Parameter */
	    structure_warning(original_line(i),"GOSUB"); /*Programmstruktur fehlerhaft */
            pcode[i].opcode=P_INVALID|P_NOCMD;
            pcode[i].panzahl=0;
	    return_value|=-1;
	  }
          if(pcode[i].panzahl) {
            unsigned short ptypliste[pcode[i].panzahl];
	    get_ptypliste(pcode[i].integer,ptypliste,pcode[i].panzahl);
	    int e=make_pliste2(procs[pcode[i].integer].anzpar,procs[pcode[i].integer].anzpar,
	        ptypliste,pos,&(pcode[i].ppointer),pcode[i].panzahl);
	    if(e<0) {
	      printf("ERROR at line %d: Parameterliste konnte nicht vorbereitet werden.\n",original_line(i));
	      return_value|=-1;
	    }
          }
        } else {
          /* Procedure nicht gefunden, aber vielleicht wird sie spaeter per merge hinzugefügt ....*/
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
      //  if(pcode[i].integer==-1)  {
      //  printf("ERROR at line %d: Label %s not found!\n",original_line(i),pcode[i].argument);
      //  structure_warning(original_line(i),"GOTO"); /*Programmstruktur fehlerhaft */
      //  pcode[i].opcode|=P_INVALID;
      //  }
      /*  Es kann ja sein, dass das label erst spaeter durch MERGE hinzugefuegt wird.*/
      }
      break;
    } /*  SWITCH */
    if((pcode[i].opcode&PM_TYP)==P_PLISTE) { /* Nachbearbeiten */
       int j=pcode[i].opcode&PM_COMMS;
       int e=make_pliste2(comms[j].pmin,comms[j].pmax,
	(unsigned short *)comms[j].pliste,pcode[i].argument,&(pcode[i].ppointer),pcode[i].panzahl);
       if(e<0) {
         return_value|=-1;
         printf("ERROR at line %d: invalid parameter list!\n",original_line(i));
       }
    } 
  }  /*  for */
  free(buffer);free(zeile);
  return(return_value);
}

static int add_label(char *name,int zeile,int dataptr) {
  if(procs_check(anzlabels)) {
    labels[anzlabels].name=strdup(name);
    labels[anzlabels].zeile=zeile;
    labels[anzlabels].datapointer=dataptr;
    anzlabels++;
    return(anzlabels-1);
  } else {
    printf("ERROR: Cannot create more than %d labels in this version of X11-Basic.\n",MAXANZLABELS);
    return(-1);
  }
}



/* Bereitet die Variablenliste einer Procedur oder Funktion vor */

static int make_varliste(char *argument, int *l,int n) {
  char arg[strlen(argument)+1];
  char *w1,*w2;
  int i=0;
  char *r;
  int typ,subtyp,vnr;
  int flag;
  strcpy(arg,argument);
  int e=wort_sep_destroy(arg,',',TRUE,&w1,&w2);
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
    // printf("  r=<%s> typ=%x subtyp=%x\n",r,typ,subtyp);
    l[i++]=vnr=(add_variable(r,typ,subtyp,V_DYNAMIC,NULL) | flag*V_BY_REFERENCE);
    free(r);
    e=wort_sep_destroy(w2,',',TRUE,&w1,&w2);
  }
  return(i);
}

/* Prozedur in Liste hinzufuegen */

static int add_proc(char *name,char *pars,int zeile,int typ) {
  int i=procnr(name,typ);
  if(i==-1) {
    if(procs_check(anzprocs)) {
      int ap;
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
      printf("ERROR: Cannot create more than %d procedures in this version of X11-Basic.\n",MAXANZPROCS);
      return(-1);
    }
  } else {
    printf("ERROR: Procedure/Function %s already exists at line %d.\n",name,original_line(procs[i].zeile));
    return(i);
  }
}


/* Rückgabewert ist eine Kopie (strdup) von funktion, oder bei indirekt (&)
 * ein per s_parser und xtrim erzeugter string.
 */

char *indirekt2(const char *funktion) {
  if(funktion && *funktion=='&') {
    char *ergebnis=s_parser(funktion+1);
    xtrim(ergebnis,TRUE,ergebnis);
    return(ergebnis);
  } else return(strdup(funktion));
}


/*  Kommando in Direktmodus auswerten 
 *  (ohne Strukturhilfe vom PASS 1+2).
 */
void kommando(char *cmd) {

  /* Leerzeichen vorne ignorieren */
  while(*cmd==' ' || *cmd==9) cmd++;

/*  1. Analysiere erstes Zeichen der Zeile*/

  switch(*cmd) {
  case 0:
  case '\'':
  case '#':
  case '!':
    return;  /* Kommentar oder Leerzeile? dann fertig. */
  }
  char zeile[strlen(cmd)+1];
  char buffer[strlen(cmd)+1];
    
  wort_sep2(cmd," !",TRUE,zeile,buffer);

  /* doppelte Leerzeichen entfernen und Grossbuchstaben */

  xtrim(zeile,TRUE,zeile);

  /*REM, DATA und HELP abfangen */

  if(!strncmp(zeile,"DATA ",5)) {
    ; /* ignorieren, bzw. spaeter wird Fehlermeldung ausgegeben. */
  } else if(!strncmp(zeile,"REM ",4)) {
    return; /* ignorieren */
  } else if(!strncmp(zeile,"HELP ",5)) {
    ; /* Keine Blanks entfernen und alles so lassen...*/
  } else {
    xtrim2(zeile,TRUE,zeile); /* Leerzeichen vor und nach operatoren enfernen*/
  }

  switch(*zeile) {
  case '@': c_gosub(zeile+1); return;
  case '~': c_void(zeile+1); return;
  case '&': {  /* Indirektes Kommando bearbeiten */
      static int level=0;
      if(level>100) {
        printf("ERROR: indirect recursion: Stack overflow. %d\n",level);
        return;
      }
      level++;
      char *test=s_parser(zeile+1);
      kommando(test);
      free(test);
      level--;
    }
    return;
  case '(':  /* Vereinfachte Taschenrechner-Funktion */
  case '-':
  case '+':
     { double result=parser(zeile);
       printf("%.13g\n",result);
       zuweis("ANS",result);     /* Ergebnis wird in variable "ANS" gespeichert.*/
       return;
     }
  }
  if(isdigit(*zeile)) {
     double result=parser(zeile);
     printf("%.13g\n",result);
     zuweis("ANS",result);   /* Ergebnis wird in variable "ANS" gespeichert.*/
     return;
  }
  char *w1,*w2;
  
  /* 2. Betrachte erstes Wort*/
  int e;
  if((e=wort_sep_destroy(zeile,' ',TRUE,&w1,&w2))==0) return;

  int l=strlen(w1);
  if(w1[l-1]==':') return;  /* nixtun, label */
  if(w1[l-1]=='=') {
    w1[l-1]=0;
    xzuweis(w1,w2);
    return;
  }
  if(*w2=='=') {
    xzuweis(w1,++w2);
    return;
  }
  char *pos;
  if((pos=searchchr2(w1,'='))!=NULL) {
    *pos++=0;
    if(e==2) w1[l]=' ';
    xzuweis(w1,pos);
    return;
  }

/*  Die PM_FLAGS sind hier irrelevant, also kein P_PREFETCH oder 
    P_INVALID.*/
  

  /* Restliche Befehle */
  int a,b;
  int i=find_comm_guess(w1,&a,&b);
  if(i!=-1) {
    switch(comms[i].opcode&PM_TYP) {
      case P_IGNORE: xberror(38,w1); /* Befehl im Direktmodus nicht moeglich */return; 
      case P_ARGUMENT: (comms[i].routine)(w2); return;
      case P_SIMPLE: (comms[i].routine)(); return;
      case P_PLISTE: {
        PARAMETER *plist;
        int e=make_pliste(comms[i].pmin,comms[i].pmax,(unsigned short *)comms[i].pliste,w2,&plist);
        if(e<0) {
	  xberror(51,w1); /*Syntax Error*/
	  return;
	}
        if(e>=comms[i].pmin) (comms[i].routine)(plist,e);
	free_pliste(e,plist);
	}
	return;
      default: xberror(38,w1); /* Befehl im Direktmodus nicht moeglich */
    }
  } else if(a!=b) {
     printf("Command needs to be more specific ! <%s...%s>\n",comms[a].name,comms[b].name);
  } else xberror(32,w1);  /* Syntax Error */
}

/* Started eine virtuelle machine. 
 * Vorausgesetzt, adr zeigt auf eine BYTECODE_HEADER struktur
 */

void run_bytecode(char *adr,int len) {
  STRING bcpc;  /* Bytecode holder */
  bcpc.pointer=adr;
  bcpc.len=len;
  BYTECODE_HEADER *bh=(BYTECODE_HEADER *)adr;
  if(verbose>0) printf("Virtual machine: %d bytes.\n",bcpc.len);
  if(verbose>1) memdump((unsigned char *)bcpc.pointer,bcpc.len);
  int n;
  char *rodata=adr+sizeof(BYTECODE_HEADER)+bh->textseglen;
  
  PARAMETER *p=virtual_machine(bcpc,0,&n,NULL,0,rodata);
  dump_parameterlist(p,n);  
  free_pliste(n,p);
}

/* programmlauf setzt voraus, dass die Strukturen durch init_program vorbereitet sind.
 * Startet entweder eine virtuelle machschine oder hangelt sich Zeilenweise
 * durch das Programm.
 */

void programmlauf(){
  if(is_bytecode) {
    run_bytecode(programbuffer,programbufferlen);
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
      if(pc<0) {
        pc=opc+1;
        xberror(20,pcode[opc].argument); /* Label not found*/
      }
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
	  if(ii!=dim) xberror(18,"");  /* Falsche Anzahl Indizies */
	  int *indexliste=malloc(ii*sizeof(int));
	  get_indexliste(pcode[opc].ppointer,indexliste,ii);
	  PARAMETER *par=calloc(1,sizeof(PARAMETER));
	  if(pcode[opc].rvalue) {
	    /* Das kann jetzt entweder eine Zuweisung zum Array-Element sein oder
	       Zuweisung eines Subarrays.*/
	    if((pcode[opc].atyp&ARRAYTYP)==ARRAYTYP) {
  	      make_parameter_stage3(pcode[opc].rvalue,PL_ARRAY,par);
	      feed_subarray_and_free(vnr,indexliste,ii,(ARRAY *)&(par->integer));
	      free_parameter(par);   
	    } else { /* Einfaches Element wird zugewiesen.*/
  	      make_parameter_stage3(pcode[opc].rvalue,(PL_CONSTGROUP|variablen[vnr].pointer.a->typ),par);
              zuweispbyindex(vnr,indexliste,ii,par);
	      free_parameter(par);
	    }
	  } else printf("Something is wrong: //zuweisxbyindex");
	  free(par);
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
    case P_DEFFN:
    case P_REM:
    case P_DATA:
    case P_NOTHING: break;
    case (P_EVAL|P_NOCMD):  kommando(program[opc]); break;
    case P_PROC:
      xberror(36,program[opc]); /* Programmstruktur fehlerhaft !*/
      printf("END missing?\n");
      break;
    default: 
      if((pcode[opc].opcode&PM_COMMS)>=anzcomms) {
	printf("ERROR: invalid command #%d/%d (%x)\n",(int)(pcode[opc].opcode&PM_COMMS),anzcomms,(int)pcode[opc].opcode);
	xberror(36,program[opc]); /* Programmstruktur fehlerhaft !*/
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
	  if(e>=0) call_sub_with_parameterlist(pcode[opc].integer,plist,pcode[opc].panzahl);
          free_pliste(e,plist);
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
          if(e>=0) (comms[i].routine)(plist,e);
          free_pliste(e,plist);
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

