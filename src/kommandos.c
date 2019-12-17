/* kommandos.c  Kommandobearbeitung (c) Markus Hoffmann */


/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ===============================================================
 * X11BASIC is free software and comes with NO WARRANTY - 
 * read the file COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#if defined WINDOWS
#include <windows.h>
#include <io.h>
#elif defined ANDROID
#else
  #include <sysexits.h>
#endif

#ifndef EX_OK
  #define EX_OK 0
#endif
#include <ctype.h>
#include <time.h>
#include <errno.h>
#include <fnmatch.h>

#include "defs.h"

#ifdef HAVE_WIRINGPI
#include <wiringPi.h>
#endif
 
#include "x11basic.h"
#include "variablen.h"
#include "keywords.h"
#include "xbasic.h"
#include "memory.h"
#include "type.h"

#include "kommandos.h"
#include "array.h"
#include "sound.h"
#include "parser.h"
#include "parameter.h"
#include "wort_sep.h"
#include "io.h"
#include "file.h"
#include "graphics.h"
#include "aes.h"
#include "window.h"
#include "mathematics.h"
#include "gkommandos.h"
#include "sysVstuff.h"
#include "bytecode.h"
#include "number.h"
#include "ccs.h"

#ifdef ANDROID
#include "android.h"
#endif

#include "virtual-machine.h"

#ifdef DUMMY_LIST
#define c_while NULL
#define c_wave NULL
#define c_version NULL
#define c_tron NULL
#define c_troff NULL
#define c_system NULL
#define c_swap NULL
#define c_sub NULL
#define c_add NULL
#define c_mul NULL
#define c_div NULL
#define c_spawn NULL
#define c_sort NULL
#define c_shm_free NULL
#define c_detach NULL
#define c_shell NULL
#define c_until NULL
#define c_split NULL
#define c_speak NULL
#define c_sound NULL
#define c_detatch NULL
#define c_sensor NULL
#define c_select NULL
#define c_save NULL
#define c_run NULL
#define c_restore NULL
#define c_read NULL
#define c_randomize NULL
#define c_plist NULL
#define c_playsoundfile NULL
#define c_playsound NULL
#define c_ongoto NULL
#define c_ongosub NULL
#define c_poke NULL
#define c_onbreakerrormenuother NULL
#define c_onbreakerrormenugoto NULL
#define c_onbreakerrormenugosub NULL
#define c_on NULL
#define c_next NULL
#define c_merge NULL
#define c_memdump NULL
#define c_lpoke NULL
#define c_local NULL
#define c_load NULL
#define c_list NULL
#define c_let NULL
#define c_inc NULL
#define c_dec NULL
#define c_help NULL
#define c_home NULL
#define c_gpio NULL
#define c_gps NULL
#define c_goto NULL
#define c_gosub NULL
#define c_free NULL
#define c_fit NULL
#define c_fit_linear NULL
#define c_fit_poly NULL
#define c_if NULL
#define c_end NULL
#define c_return NULL
#define c_echo NULL
#define c_dpoke NULL
#define c_lpoke NULL
#define c_dim NULL
#define c_for NULL
#define c_fft NULL
#define c_exit NULL
#define c_every NULL
#define c_getlocation NULL
#define c_eval NULL
#define c_error NULL
#define c_erase NULL
#define c_edit NULL
#define c_pause NULL
#define c_clr NULL
#define c_cls NULL
#define c_chain NULL
#define c_beep NULL
#define c_arrayfill NULL
#define c_arraycopy NULL
#define c_after NULL
#define c_absolute NULL
#define c_case NULL
#define c_clear NULL
#define c_ NULL
#define c_print   NULL
#define c_alert  NULL
#define  c_bget  NULL
#define  c_bload  NULL
#define  c_bmove  NULL
#define  c_bottomw  NULL
#define  c_boundary  NULL
#define  c_box  NULL
#define  c_bput  NULL
#define  c_bsave  NULL
#define  c_call  NULL
#define  c_chdir  NULL
#define  c_chmod  NULL
#define  c_circle  NULL
#define  c_clearw  NULL
#define  c_clip  NULL
#define  c_close  NULL
#define  c_closew  NULL
#define  c_color  NULL
#define  c_connect  NULL
#define  c_cont  NULL
#define  c_copyarea  NULL
#define  c_curve  NULL
#define  c_deffill  NULL
#define  c_defline  NULL
#define  c_defmark  NULL
#define  c_defmouse  NULL
#define  c_deftext  NULL
#define  c_draw  NULL
#define  c_dump  NULL
#define  c_ellipse  NULL
#define  c_allevent  NULL
#define  c_exec  NULL
#define  c_fileselect  NULL
#define c_polyline NULL
#define c_polymark NULL
#define c_prbox NULL
#define c_put NULL
#define c_unget NULL
#define c_put_bitmap NULL
#define c_quit NULL
#define c_rbox NULL
#define c_receive NULL
#define c_relseek NULL
#define c_rename NULL
#define c_rmdir NULL
#define c_rootwindow NULL
#define c_rsrc_free NULL
#define c_rsrc_load NULL
#define c_savescreen NULL
#define c_savewindow NULL
#define c_scope NULL
#define c_screen NULL
#define c_seek NULL
#define c_send NULL
#define c_publish NULL
#define c_broker NULL
#define c_subscribe NULL
#define c_setfont NULL
#define c_setmouse NULL
#define c_sget NULL
#define c_showk NULL
#define c_showm NULL
#define c_vsync NULL
#define c_sizew NULL
#define c_sput NULL
#define c_stop NULL
#define  c_text NULL
#define  c_titlew NULL
#define  c_topw NULL
#define  touch NULL
#define  c_unmap NULL
#define  c_usewindow NULL
#define  c_void NULL
#define  c_vsync NULL
#define  c_watch NULL
#define  c_xload NULL
#define c_xrun NULL
#define  c_fill NULL
#define  c_flush NULL
#define  c_fullw NULL
#define  c_get NULL
#define  c_getgeometry NULL
#define  c_getscreensize NULL
#define  c_gprint NULL
#define  c_graphmode NULL
#define  c_hidem NULL
#define  c_hidek NULL
#define  c_infow NULL
#define  c_input NULL
#define  c_keyevent NULL
#define  c_kill NULL
#define  c_line NULL
#define  c_lineinput NULL
#define  c_link NULL
#define  c_locate NULL
#define  c_ltext NULL
#define  c_menu NULL
#define  c_menudef NULL
#define  c_menukill NULL
#define  c_menuset NULL
#define  c_mkdir NULL
#define  c_mouse NULL
#define  c_mouseevent NULL
#define  c_motionevent NULL
#define  c_movew NULL
#define  c_msync NULL
#define  c_new NULL
#define  c_norootwindow NULL
#define  c_objc_add NULL
#define  c_objc_delete NULL
#define  c_open NULL
#define  c_openw NULL
#define  c_out NULL
#define  c_pbox NULL
#define  c_pcircle NULL
#define  c_pellipse NULL
#define  c_pipe NULL
#define  c_plot NULL
#define  c_polyfill NULL
#else
/*********************/
static int saveprg(const char *fname) {
  char *buf=malloc(programbufferlen);
  int i=0;
  while(i<programbufferlen) {
    if(programbuffer[i]==0 || programbuffer[i]=='\n')
      buf[i]='\n';
    else
      buf[i]=programbuffer[i];
    i++;
  }
  return(bsave(fname,buf,programbufferlen));
}

static void c_memdump(PARAMETER *plist,int e) {
  memdump((unsigned char *)INT2POINTER(plist->integer),plist[1].integer);
}

static void stringdump(const char *s,int l,char *d) {
  int i=0;
  unsigned char a;
  while(i<l && i<60) {
    a=s[i];
    if(isprint(a)) d[i]=a;
    else d[i]='.';
    i++;
  } 
  if(i<l) {
    d[i++]='.';
    d[i++]='.';
    d[i++]='.';
  }
  d[i]=0;
}


/*****************************************/
/* Kommandos zur Programmablaufkontrolle */

void c_stop()  {batch=0;} 

static void c_tron()  {echoflag=1;}
static void c_troff() {echoflag=0;}
static void c_beep()  {putchar('\007');}
 
static void c_clear(PARAMETER *plist,int e){
  clear_all_variables(); 
  graphics_setdefaults();
}

void c_new(const char *n) {
  newprg();
  strcpy(ifilename,"new.bas");
  graphics_setdefaults();
}
static void c_while(PARAMETER *plist, int e) {
  if(pc<=0) {xberror(38,"WHILE"); /* Befehl im Direktmodus nicht moeglich */return;}
  if(plist->integer==0) { 
    int npc=pcode[pc-1].integer;
    if(npc==-1) xberror(36,"WHILE"); /*Programmstruktur fehlerhaft */
    pc=npc;
  } 
}

void c_gosub(const char *n) {
  char *pos2;
  int pc2;
   
  char *buffer=indirekt2(n);
  char *pos=searchchr(buffer,'(');
    
  if(pos!=NULL) {
    pos[0]=0;pos++;
    pos2=pos+strlen(pos)-1;
    if(pos2[0]!=')') {
      xberror(32,n); /* Syntax error */
      free(buffer);
      return;
    } 
    else pos2[0]=0;
  } else pos=buffer+strlen(buffer);
    
    pc2=procnr(buffer,1);
    if(pc2==-1)   xberror(19,buffer); /* Procedure nicht gefunden */
    else {       
	if(do_parameterliste(pos,procs[pc2].parameterliste,procs[pc2].anzpar)) {
          restore_locals(sp+1);
	  xberror(42,buffer); /* Zu wenig Parameter */
	} else { batch=1;
	  pc2=procs[pc2].zeile;
	  if(stack_check(sp)) {stack[sp++]=pc;pc=pc2+1;}
	  else {
	    printf("Stack overflow! PC=%d\n",pc); 
	    restore_locals(sp+1);
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
   
   Man muesste das als pthread realisieren und aber PC und SP und den 
   Stack duplizieren.
   
   
   */
static void c_spawn(const char *n) {
  char *buffer,*pos,*pos2;
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
    #if defined WINDOWS || defined ATARI
    #else
    pid_t forkret=fork();
    if(forkret==-1) io_error(errno,"SPAWN");
    if(forkret==0) {
      if(do_parameterliste(pos,procs[pc2].parameterliste,procs[pc2].anzpar)) {
        restore_locals(sp+1);
        xberror(42,buffer); /* Zu wenig Parameter */
      } else { 
        batch=1;
        pc2=procs[pc2].zeile;
        if(stack_check(sp)) {stack[sp++]=pc;pc=pc2+1;}
        else {
          printf("Stack overflow! PC=%d\n",pc); 
	  restore_locals(sp+1);
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
      

static void c_local(PARAMETER *plist,int e) {
  if(e) {
    for(int i=0;i<e;i++) do_local(plist[i].integer,sp);
  }
}

static void c_goto(const char *n) {
  char *b=indirekt2(n);
  pc=labelzeile(b);
  if(pc==-1) {xberror(20,b);/* Label nicht gefunden */ batch=0;}
  else batch=1;
  free(b);
}


static void c_system(PARAMETER *plist,int e) {
  if(system(plist->pointer)==-1) io_error(errno,"system");
}


static void c_shell(PARAMETER *plist,int e) {
  char *argv[e+1];
  for(int i=0;i<e;i++) argv[i]=plist[i].pointer;
  argv[e]=NULL;
  if(spawn_shell(argv)==-1) io_error(errno,"shell");
}

static void c_edit(const char *n) {
#ifndef ANDROID
    char filename[strlen(ifilename)+8];
    char buffer[256];
    char *buffer2=strdup(ifilename);
    sprintf(filename,"%s.~~~",ifilename);
    saveprg(filename);
    sprintf(buffer,"$EDITOR %s",filename); 
    if(system(buffer)==-1) io_error(errno,"system");
    c_new(NULL);  
    strcpy(ifilename,buffer2);
    free(buffer2);
    mergeprg(filename);
    sprintf(buffer,"rm -f %s",filename); 
    if(system(buffer)==-1) io_error(errno,"system");
#else
    puts("The EDIT command is not available in the ANDROID version.\n"
    "Please use Menu --> Editor to edit the current program.");
    xberror(9,"EDIT"); /* Funktion nicht implementiert */
#endif
}

static void c_after(PARAMETER *plist,int e) {
  everyflag=0;
  alarmpc=plist[1].integer;    /* Proc nummer */
  alarmpctype=plist[1].arraytyp;
  alarm(plist[0].integer);     /* Zeit in sec */
}

static void c_every(PARAMETER *plist,int e) {
  everyflag=1;
  alarmpc=plist[1].integer;    /* Proc nummer */ 
  alarmpctype=plist[1].arraytyp;
  everytime=plist[0].integer;  /* Zeit in sec */
  alarm(everytime);
}


static inline void do_restore(int offset) {
  datapointer=offset;
}

static void c_run(const char *n) {        /* Programmausfuehrung starten und bei 0 beginnen */
  do_run();
}
void do_run() {
  restore_all_locals(); /* sp=0; von einem vorherigen Abbruch koennten noch locale vars im Zwischenspeicher sein.*/
  clear_all_variables();
  close_all_files();
  pc=0;
  batch=1;
  do_restore(0);
}

void c_cont() {
  if(batch==0) {
    if(prglen>0 && pc>=0 && pc<=prglen) batch=1;
    else xberror(41,"");     /*CONT nicht moeglich !*/
  } else {
    /*hier koennte es ein CONTINUE in einer SWITCH Anweisung sein. */
    /*gehe zum (bzw. hinter) naechsten CASE oder DEFAULT*/
    if(pc<=0) {xberror(38,"SELECT"); /* Befehl im Direktmodus nicht moeglich */return;}
    int j,f=0,o;
    for(j=pc; (j<prglen && j>=0);j++) {
      o=pcode[j].opcode&PM_SPECIAL;
      if((o==P_CASE || o==P_DEFAULT || o==P_ENDSELECT)  && f<=0) break;
      if(o & P_LEVELIN) f++;
      if(o & P_LEVELOUT) f--;
    }
    if(j==prglen) xberror(36,"SELECT/CONTINUE"); /*Programmstruktur fehlerhaft !*/ 
    else pc=j+1;
  }
}
static void c_restore(PARAMETER *plist,int e) {
  if(e) {
    do_restore((int)labels[plist[0].integer].datapointer);
  //  printf("RESTORE: %d %s\n",plist[0].integer,labels[plist[0].integer].name);
  } else do_restore(0);
}

static char *get_next_data_entry() {
  char *ptr,*ptr2;
  char *ergebnis=NULL;
  if(databufferlen==0 || databuffer==NULL || datapointer>=databufferlen) return(NULL);
  ptr=databuffer+datapointer;
  ptr2=searchchr(ptr,',');
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
  return(ergebnis);
}

static void c_read(PARAMETER *plist,int e) {
  char *t;
  for(int i=0;i<e;i++) {
    t=get_next_data_entry();
    if(t==NULL) xberror(34,""); /* Zu wenig Data */
    else {
    switch(plist[i].typ) {
    case PL_ARRAYVAR:
      xberror(9,"READ Array"); /* Funktion noch nicht moeglich */ 
      break;
    case PL_SVAR:
      free_string((STRING *)plist[i].pointer);
      if(*t=='\"') *((STRING *)(plist[i].pointer))=string_parser(t);
      else *((STRING *)(plist[i].pointer))=create_string(t);
      break;
    case PL_IVAR:
      *((int *)(plist[i].pointer))=(int)parser(t);
      break;
    case PL_FVAR:
      *((double *)(plist[i].pointer))=parser(t);
      break;
    case PL_AIVAR:
      mpz_set_str(*(ARBINT *)(plist[i].pointer),t,10);
      break;
    case PL_CVAR:
      *((COMPLEX *)(plist[i].pointer))=complex_parser(t);
      break;
    default:
      xberror(13,"");  /* Type mismatch */
      dump_parameterlist(&plist[i],1);
    }
    free(t);
    }
  }
}

gmp_randstate_t state;
int randstate_isinit=0;

/*RANDOMIZE. Hier k"onnte man noch unterscheiden op der parameter int oder big int
  ist....*/
static void c_randomize(PARAMETER *plist, int e) {
  unsigned int seed;
  if(e) seed=plist->integer;
  else {
    seed=time(NULL);
    if(seed==-1) io_error(errno,"RANDOMIZE");
  }
  srand(seed);
  if(!randstate_isinit) {
    gmp_randinit_default(state);
    randstate_isinit=1;
  } 
  gmp_randseed_ui(state, seed);
}

static void c_list(PARAMETER *plist, int e) {
  int i,a=0,o=prglen;
  if(e==2) {
    a=min(max(plist[0].integer,0),prglen);
    o=max(min(plist[1].integer+1,prglen),0);
  } else if(e==1) {
    a=min(max(plist[0].integer,0),prglen);
    o=a+1;
  }
  if(is_bytecode && programbufferlen>sizeof(BYTECODE_HEADER)-2) {
    BYTECODE_HEADER *h=(BYTECODE_HEADER *)programbuffer;
    printf("Bytecode: %s (%d Bytes)\n",ifilename,programbufferlen);
    print_bytecode_info(h);
  } else {
    if(o<=prglen) for(i=a;i<o;i++) puts(program[i]);
  }
}

char *plist_paramter(PARAMETER *p) {
  static char ergebnis[256];
  switch(p->typ) {
  case PL_EVAL:
  case PL_KEY:     strcpy(ergebnis,p->pointer); break;
  case PL_LEER:    *ergebnis=0; break;
  case PL_FLOAT:
  case PL_CF:
  case PL_NUMBER:  sprintf(ergebnis,"%g",p->real); break;
  case PL_INT:     sprintf(ergebnis,"%d",p->integer); break;
  case PL_ARBINT:  {
    char *buf=mpz_get_str(NULL,10,*(ARBINT *)(p->pointer));
    if(strlen(buf)>200) {
        buf[197]=buf[198]=buf[199]='.';
        buf[200]=0;
    }
    strcpy(ergebnis,buf);
    free(buf);
    } 
    break;
  case PL_COMPLEX: sprintf(ergebnis,"(%g+%gi)",p->real,p->imag);break;
  case PL_FILENR:  sprintf(ergebnis,"#%d",p->integer); break;
  case PL_STRING: { 
    char *buf=malloc(min(p->integer+1,64));
    stringdump(p->pointer,p->integer,buf);
    sprintf(ergebnis,"\"%s\"",buf); 
    break;
  }
  case PL_LABEL:   strcpy(ergebnis,labels[p->integer].name); break;
  case PL_ARRAY:   strcpy(ergebnis,"[..array..]"); break;
  case PL_IVAR:    sprintf(ergebnis,"%s%%",variablen[p->integer].name); break;
  case PL_CVAR:    sprintf(ergebnis,"%s#",variablen[p->integer].name); break;
  case PL_SVAR:    sprintf(ergebnis,"%s$",variablen[p->integer].name); break;
  case PL_AIVAR:   sprintf(ergebnis,"%s&",variablen[p->integer].name); break;
  case PL_FVAR:    strcpy(ergebnis,variablen[p->integer].name); break;
  case PL_NVAR:
  case PL_VAR:
  case PL_ALLVAR:
  case PL_ARRAYVAR:
    switch(variablen[p->integer].typ) {
    case INTTYP:     sprintf(ergebnis,"%s%%",variablen[p->integer].name); break;
    case ARBINTTYP:  sprintf(ergebnis,"%s&",variablen[p->integer].name);  break;
    case FLOATTYP:   sprintf(ergebnis,"%s",variablen[p->integer].name);   break;
    case COMPLEXTYP: sprintf(ergebnis,"%s#",variablen[p->integer].name);  break;
    case STRINGTYP:  sprintf(ergebnis,"%s$",variablen[p->integer].name);  break;
    case ARRAYTYP:
      switch(p->arraytyp) {
      case INTTYP:     sprintf(ergebnis,"%s%%()",variablen[p->integer].name); break;
      case ARBINTTYP:  sprintf(ergebnis,"%s&()",variablen[p->integer].name); break;
      case FLOATTYP:   sprintf(ergebnis,"%s()",variablen[p->integer].name); break;
      case COMPLEXTYP: sprintf(ergebnis,"%s#()",variablen[p->integer].name); break;
      case STRINGTYP:  sprintf(ergebnis,"%s$()",variablen[p->integer].name); break;
      default:         sprintf(ergebnis,"%s?()",variablen[p->integer].name);
      }
      break;
    }
  default: 
    sprintf(ergebnis,"<ups$%x>",p->typ);
  }
  if(p->panzahl) {
    strcat(ergebnis,"(");
    char *buf;
    for(int i=0;i<p->panzahl;i++) {
      buf=plist_paramter(&(p->ppointer[i]));
      strcat(ergebnis,buf);
      free(buf);
      if(i<p->panzahl-1) strcat(ergebnis,",");
    }
    strcat(ergebnis,")");
  }
  return(strdup(ergebnis));
}

static char *plist_zeile(P_CODE *code) {
  char *ergebnis=malloc(MAXLINELEN);
  int i;
  sprintf(ergebnis,"=?=> %d",(int)code->opcode);
  switch(code->opcode) {
  case P_DEFFN: 
    sprintf(ergebnis,"DEFFN %s",procs[code->integer].name); 
    if(procs[code->integer].anzpar) { 
      strcat(ergebnis,"(");
      for(i=0;i<procs[code->integer].anzpar;i++) {
    	strcat(ergebnis,variablen[procs[code->integer].parameterliste[i]&(~V_BY_REFERENCE)].name);
        if(i<procs[code->integer].anzpar-1) strcat(ergebnis,",");
      }
      strcat(ergebnis,")");
    }
    strcat(ergebnis," = ");
    strcat(ergebnis,code->argument); 
    break;
  case P_PROC:
    if(procs[code->integer].typ==PROC_PROC)
         sprintf(ergebnis,"PROCEDURE %s",procs[code->integer].name); 
    else sprintf(ergebnis,"FUNCTION %s",procs[code->integer].name); 
    if(procs[code->integer].anzpar) { 
      strcat(ergebnis,"(");
      for(i=0;i<procs[code->integer].anzpar;i++) {
    	strcat(ergebnis,variablen[procs[code->integer].parameterliste[i]&(~V_BY_REFERENCE)].name);
        if(i<procs[code->integer].anzpar-1) strcat(ergebnis,",");
      }
      strcat(ergebnis,")");
    }
    break;
  case P_REM:
    if(code->integer==(int)'#') sprintf(ergebnis,"#%s",code->argument);
    else if(code->integer==(int)'!') sprintf(ergebnis,"!%s",code->argument);
    else sprintf(ergebnis,"'%s",code->argument);
    break;
  case P_DATA:
    sprintf(ergebnis,"DATA %s",code->argument);
    break;
  case P_LABEL:
    sprintf(ergebnis,"%s:",labels[code->integer].name);
    break;
  case P_NOTHING:  /* Leerzeile */
    *ergebnis=0;
    break;
  case P_ZUWEIS: 
    switch(variablen[code->integer].typ) {
      case INTTYP:    sprintf(ergebnis,"%s%%=",variablen[code->integer].name); break; 
      case STRINGTYP: sprintf(ergebnis,"%s$=",variablen[code->integer].name); break;
      case ARRAYTYP: 
        if(code->panzahl) { 
	  char *buf;
	  int i;
	  if(variablen[code->integer].pointer.a->typ==STRINGTYP) sprintf(ergebnis,"%s$(",variablen[code->integer].name);
	  else if(variablen[code->integer].pointer.a->typ==INTTYP) sprintf(ergebnis,"%s%%(",variablen[code->integer].name);
	  else sprintf(ergebnis,"%s(",variablen[code->integer].name); 
          for(i=0;i<code->panzahl;i++) {
            buf=plist_paramter(&(code->ppointer[i]));
            strcat(ergebnis,buf);
	    free(buf);
	    if(i<code->panzahl-1) strcat(ergebnis,",");
          }
	  sprintf(ergebnis+strlen(ergebnis),")=");
	} else sprintf(ergebnis,"%s()=",variablen[code->integer].name); 
	break;
      case COMPLEXTYP: sprintf(ergebnis,"%s#=",variablen[code->integer].name); break; 
      case ARBINTTYP:  sprintf(ergebnis,"%s&=",variablen[code->integer].name); break; 
      case FLOATTYP:   sprintf(ergebnis,"%s=",variablen[code->integer].name); break;
      default:         sprintf(ergebnis,"zuweis %d %s ---> ",code->integer,variablen[code->integer].name);
    }
    char *buf=plist_paramter(code->rvalue);strcat(ergebnis,buf);free(buf);
    break;
  default:
    if(code->opcode&P_INVALID) sprintf(ergebnis,"==invalid==> %s",code->argument);
    else if((code->opcode&PM_TYP)==P_EVAL) {
      sprintf(ergebnis,"eval ---> %s",code->argument);
    }  else if((code->opcode&PM_COMMS)<anzcomms) {
      sprintf(ergebnis,"%s",comms[(code->opcode&PM_COMMS)].name);
      if((code->opcode&P_ARGUMENT)) {
        strcat(ergebnis," ");
        strcat(ergebnis,code->argument);
      } else if((code->opcode&PM_SPECIAL)==P_PLISTE) {
        if(code->panzahl) {
          char *buf;
          strcat(ergebnis," ");
          for(i=0;i<code->panzahl;i++) {
            buf=plist_paramter(&(code->ppointer[i]));
            strcat(ergebnis,buf);
	    free(buf);
	    if(i<code->panzahl-1) strcat(ergebnis,",");
          }
        }
      }
    }  
  }
 
  /*Anfuegungen an Zeile hinten*/
 
  if(code->etyp==PE_COMMENT) {
    strcat(ergebnis," !");
    strcat(ergebnis,(char *)code->extra);
  }
  return(ergebnis);
}
static int plist_printzeile(FILE *dptr,P_CODE *code, int level) {
  char *zeile=plist_zeile(code);
  if(code->opcode & P_LEVELOUT) level--;
  for(int j=0;j<level;j++) fprintf(dptr,"  ");
  fprintf(dptr,"%s\n",zeile);
  free(zeile);
  if(code->opcode & P_LEVELIN) level++;
  return(level);
}

/*  PLIST [#n] */

static void c_plist(PARAMETER *plist, int e) {
  FILEINFO fff=get_fileptr(-4);  /* stdout */
  if(e>0) {
    fff=get_fileptr(plist->integer);
    if(fff.typ==0) {xberror(24,""); /* File nicht geoeffnet */return;}
  }
  int i,f=0;
  for(i=0;i<prglen;i++) { 
    fprintf(fff.dptr,"%4d: $%06x |",i,(unsigned int)pcode[i].opcode);
  #if DEBUG
    fprintf(fff.dptr," (%5d) |",ptimes[i]);
  #endif
    fprintf(fff.dptr," %3d,%d |",pcode[i].integer,pcode[i].panzahl);
    f=plist_printzeile(fff.dptr,&pcode[i],f);
  }
}


static void c_save(PARAMETER *plist, int e) { 
  if(programbufferlen) {
    char *name;
    if(e) name=plist[0].pointer;
    else name=ifilename;
    if(*name==0) name=ifilename;
    if(exist(name)) {
      char buf[100];
      sprintf(buf,"mv %s %s.bak",name,name);
      if(system(buf)==-1) io_error(errno,"system");
    }
    saveprg(name);
  }
}

/*MERGE: batch und PC bleibt wie es ist. Das Programm kann dann weiterlaufen.*/
static void c_merge(PARAMETER *plist, int e){
  if(exist(plist->pointer)) {
    if(programbufferlen==0) strcpy(ifilename,plist->pointer);
    mergeprg(plist->pointer);
  } else xberror(-33,plist->pointer); /* file not found*/
}

/*LOAD: altes Programm stoppen und loeschen.*/
static void c_load(PARAMETER *plist, int e) { 
  newprg();
  c_merge(plist,e); 
}
static void c_chain(PARAMETER *plist,int e){ c_load(plist,e); do_run(); }

static void c_let(const char *n) {  
  char v[strlen(n)+1],w[strlen(n)+1];
  wort_sep(n,'=',TRUE,v,w);
  xzuweis(v,w);
}


void c_quit(PARAMETER *plist, int e) { 
  int ecode=0;
  if(e) ecode=plist->integer; 
  batch=0;
  pc=0;
  close_all_files();
  quit_x11basic(ecode); 
}

static void c_fit_poly(PARAMETER *plist, int e) {  
  ARRAY *arr=(ARRAY *)&(plist[0].integer);
  int anz=min(plist[3].integer,anz_eintraege(arr));
  double *x=arr->pointer+arr->dimension*INTSIZE;
  arr=(ARRAY *)&(plist[1].integer);
  double *y=arr->pointer+arr->dimension*INTSIZE;
  arr=(ARRAY *)&(plist[2].integer);
  double *er=arr->pointer+arr->dimension*INTSIZE;
  arr=variablen[plist[4].integer].pointer.a;
  int n=anz_eintraege(arr);
  if(plist[5].typ!=PL_LEER) n=min(plist[5].integer,n);
  double *varptr=(double  *)(arr->pointer+arr->dimension*INTSIZE);

  polynom_fit(x, y, anz, er, 1, varptr,n);
}



/* Linearer Fit (regression) optional mit Fehlerbalken in x und y Richtung  */

/* PL_FARRAY,PL_FARRAY,PL_INT,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_FARRAY,PL_FARRAY */

static void c_fit_linear(PARAMETER *plist, int e) {  
  ARRAY *arr=(ARRAY *)&(plist[0].integer);
  int anz=anz_eintraege(arr);
  double *x=arr->pointer+arr->dimension*INTSIZE;
  arr=(ARRAY *)&(plist[1].integer);
  double *y=arr->pointer+arr->dimension*INTSIZE;
  double *dx=NULL,*dy=NULL;
  double a,b,da,db,chi2,q;
  if(plist[2].typ!=PL_LEER) anz=min(plist[2].integer,anz);
  if(e>8) {
    arr=(ARRAY *)&(plist[8].integer);
    dy=arr->pointer+arr->dimension*INTSIZE;
  }
  if(e>9) {
    arr=(ARRAY *)&(plist[9].integer);
    dx=arr->pointer+arr->dimension*INTSIZE;
  }
  if(dx==NULL) linear_fit(x,y,anz,dy,(dy==NULL?0:1),&a,&b,&da,&db,&chi2,&q); 
  else     linear_fit_exy(x,y,anz,dx,dy,&a,&b,&da,&db,&chi2,&q);


  if(e>3 && plist[3].typ!=PL_LEER) varcastfloat(plist[3].integer,plist[3].pointer,a);
  if(e>4 && plist[4].typ!=PL_LEER) varcastfloat(plist[4].integer,plist[4].pointer,b);
  if(e>5 && plist[5].typ!=PL_LEER) varcastfloat(plist[5].integer,plist[5].pointer,da);
  if(e>6 && plist[6].typ!=PL_LEER) varcastfloat(plist[6].integer,plist[6].pointer,db);
  if(e>7 && plist[7].typ!=PL_LEER) varcastfloat(plist[7].integer,plist[7].pointer,chi2);
  if(e>10 && plist[10].typ!=PL_LEER) varcastfloat(plist[10].integer,plist[10].pointer,q);
}

/* Sort-Funktion (wie qsort() ), welche ausserdem noch ein integer-Array mitsortiert */

static void do_sort(void *a, size_t n,size_t size,int(*compar)(const void *, const void *), int *b) {
 // printf("sort: n=%d size=%d b=%p\n",n,size,b);
  if(n<2) return;
  if(b==NULL) qsort(a,n,size,compar);
  else { 
    void *rra=malloc(size);
    unsigned long i,j;
    int index;

    unsigned long l=(n>>1)+1;
    unsigned long ir=n;
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
// memdump(((STRING *)p2)->pointer,((STRING *)p2)->len+1);
  int a=memcmp(((STRING *)p1)->pointer,((STRING *)p2)->pointer,1+min(((STRING *)p1)->len,((STRING *)p2)->len));
 // printf("cmpstring <%s> <%s> --> %d\n",((STRING *)p1)->pointer,((STRING *)p2)->pointer,a);
  return(a);
}
static int cmpdouble(const void *p1, const void *p2) {
  if(*(double *)p1==*(double *)p2) return(0);
  else if(*(double *)p1>*(double *)p2) return(1);
  else return(-1);
}
static int cmpint(const void *p1, const void *p2) {
  return((*(int *)p1)-(*(int *)p2));
}
static int cmparbint(const void *p1, const void *p2) {
  return(mpz_cmp(*(ARBINT *)p1,*(ARBINT *)p2));
}
static int cmpcomplex(const void *p1, const void *p2) {
  /* Hier sollte nach dem Absolutbetrag sortiert werden.*/
  COMPLEX *sqr1=(COMPLEX *)p1;
  COMPLEX *sqr2=(COMPLEX *)p2;
  
  double d1=sqr1->r*sqr1->r+sqr1->i*sqr1->i;
  double d2=sqr2->r*sqr2->r+sqr2->i*sqr2->i;
  
  if(d1==d2) return(0);
  else if(d1>d2) return(1);
  else return(-1);
}

/* Sortierfunktion fuer ARRAYS 

Todo: 
* Umstellen auf pliste.
* Stringsortierung bei unterschiedlicher Laenge ist nicht optimal.
* Indexarray muss INTARRAYTYP sein. Das geht auch flexibler! (mit allarray)

*/


static void c_sort(PARAMETER *plist,int e) {  
  int vnry=-1; 

  int vnrx=plist->integer;
  int ndata=anz_eintraege(variablen[vnrx].pointer.a);

  if(e>=2) ndata=plist[1].integer;
  if(e>=3) vnry=plist[2].integer;
  // int typ=variablen[vnrx].typ;
  int subtyp=variablen[vnrx].pointer.a->typ;
  
//  printf("c_sort vnr=%d ndata=%d vnry=%d\n",vnrx,ndata,vnry);

  switch(subtyp) {
  case STRINGTYP:
    do_sort((void *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE)
      ,ndata,sizeof(STRING),cmpstring,
      (int *)((vnry!=-1)?(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE):NULL));      
    break;
  case INTTYP:
      do_sort((void *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE)
      ,ndata,sizeof(int),cmpint,
      (int *)((vnry!=-1)?(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE):NULL));      
    break;
  case FLOATTYP:
      do_sort((void *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE)
      ,ndata,sizeof(double),cmpdouble,
      (int *)((vnry!=-1)?(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE):NULL));
    break;
  case COMPLEXTYP:
      do_sort((void *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE)
      ,ndata,sizeof(COMPLEX),cmpcomplex,
      (int *)((vnry!=-1)?(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE):NULL));
    break;
  case ARBINTTYP:
    do_sort((void *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE)
      ,ndata,sizeof(ARBINT),cmparbint,
      (int *)((vnry!=-1)?(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE):NULL));
    break;
  default:
    printf("ERROR: cannot sort this type of array.\n");
  }
}

/* Allgemeine Fit-Funktion  mit Fehlerbalken in y Richtung  */

static void c_fit(const char *n) {  
  char w1[strlen(n)+1],w2[strlen(n)+1];                  
  int typ,scip=0,i=0,mtw=0;  
  int vnrx=-1,vnry=-1,vnre=-1,vnre2=-1,ndata=0;
  double a,b,siga,sigb,chi2,q;
  char *r;
  int e=wort_sep(n,',',TRUE,w1,w2);
//  xberror(9,"FIT"); /* Funktion noch nicht moeglich */
  while(e) {
    scip=0;
    if(*w1) {
       switch(i) {
         case 0: { /* Array mit x-Werten */     
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnrx=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnrx==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else xberror(95,w1); /* Parameter must be Array */
	   break;
	   }
	 case 1: {   /* Array mit y-Werten */
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnry=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnry==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	   } else xberror(95,w1); /* Parameter must be Array */
	   break;
	   } 
	 case 2: {   /* Array mit err-Werten */
	 
	 /*TODO: vnre2 ist undefiniert!! */
	 
	 
	   /* Typ bestimmem. Ist es Array ? */
           typ=type(w1)&(~CONSTTYP);
	   if(typ & ARRAYTYP) {
             r=varrumpf(w1);
             vnre=var_exist(r,ARRAYTYP,typ&(~ARRAYTYP),0);
             free(r);
	     if(vnre==-1) xberror(15,w1); /* Feld nicht dimensioniert */
	     else mtw=1;
	   } else {scip=1; mtw=0;}
	   break;
	   } 
	 case 4: 
	   ndata=(int)parser(w1); 
           break;
	 case 5: {   /* Funktion mit Parameterliste */
	   }
	   break;
	 case 6: {   /* Ausdruck, der Angibt, welche Parameter zu fitten sind */  	 
	   if(vnrx!=-1 && vnry!=-1) {
             if(mtw==2 && vnre!=-1 && vnre2!=-1) {
	       linear_fit_exy((double *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE),
		   (double *)(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE),ndata,
		   (double *)(variablen[vnre].pointer.a->pointer+variablen[vnre].pointer.a->dimension*INTSIZE),
		   (double *)(variablen[vnre2].pointer.a->pointer+variablen[vnre2].pointer.a->dimension*INTSIZE),
		   &a,&b,&siga,&sigb,&chi2,&q); 

	     } else {
	       linear_fit((double *)(variablen[vnrx].pointer.a->pointer+variablen[vnrx].pointer.a->dimension*INTSIZE),
		   (double *)(variablen[vnry].pointer.a->pointer+variablen[vnry].pointer.a->dimension*INTSIZE),ndata,(vnre!=-1)?(
		   (double *)(variablen[vnre].pointer.a->pointer+variablen[vnre].pointer.a->dimension*INTSIZE)):NULL,mtw,&a,&b,&siga,&sigb,&chi2,&q); 
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

static void c_fft(PARAMETER *plist,int e) { 
  int vnr=plist->integer;
  ARRAY *arr=variablen[vnr].pointer.a;
  int n=anz_eintraege(arr);
  int isign=0;
  double *varptr=(double  *)(arr->pointer+arr->dimension*INTSIZE);

  if(e>1) isign=plist[1].integer;
  realft(varptr,n,isign);
}


static void c_arraycopy(PARAMETER *plist,int e) {
  int vnr1=plist[0].integer;
  int vnr2=plist[1].integer;
  ARRAY *arr1=variablen[vnr1].pointer.a;
  ARRAY *arr2=variablen[vnr2].pointer.a;
  ARRAY a;
  if(arr1->typ==arr2->typ) a=double_array(arr2);
  else {
    switch(arr1->typ) {
    case INTTYP:
      if(arr2->typ!=STRINGTYP) a=convert_to_intarray(arr2);
      else xberror(96,variablen[vnr2].name); /* Array has wrong type */
      break;
    case FLOATTYP:
      if(arr2->typ!=STRINGTYP) a=convert_to_floatarray(arr2);
      else xberror(96,variablen[vnr2].name); /* Array has wrong type */
      break;
    case COMPLEXTYP:
      if(arr2->typ!=STRINGTYP) a=convert_to_complexarray(arr2);
      else xberror(96,variablen[vnr2].name); /* Array has wrong type */
      break;
    case ARBINTTYP:
      if(arr2->typ!=STRINGTYP) a=convert_to_arbintarray(arr2);
      else xberror(96,variablen[vnr2].name); /* Array has wrong type */
      break;
    case STRINGTYP:
      xberror(96,variablen[vnr2].name); /* Array has wrong type */
      break;
    default:
      xberror(13,"");  /* Type mismatch */
    }
  }
  free_array(arr1);
  *arr1=a;
}

static void c_arrayfill(PARAMETER *plist,int e) {
  int vnr=plist->integer;
//  printf("ARRAYFILL: vnr=%d\n",vnr);
  ARRAY *arr=variablen[vnr].pointer.a;
  ARRAY a;
  
  switch(arr->typ) {
  case INTTYP:
    switch(plist[1].typ) {
    case PL_INT: break;
    case PL_COMPLEX:
    case PL_FLOAT:   plist[1].integer=(int)plist[1].real; break;
    case PL_ARBINT:  plist[1].integer=mpz_get_si(*(ARBINT *)plist[1].pointer); break;
    default: xberror(96,variablen[vnr].name); /* Array has wrong type */
    }
    a=create_int_array(arr->dimension,arr->pointer,plist[1].integer);
    break;
  case FLOATTYP:
    switch(plist[1].typ) {
    case PL_INT:     plist[1].real=(double)plist[1].integer; break;
    case PL_COMPLEX:
    case PL_FLOAT:   break;
    case PL_ARBINT:  plist[1].real=mpz_get_d(*(ARBINT *)plist[1].pointer); break;
    default:   xberror(96,variablen[vnr].name); /* Array has wrong type */
    }
    a=create_float_array(arr->dimension,arr->pointer,plist[1].real);
    break;
  case COMPLEXTYP:
    switch(plist[1].typ) {
    case PL_INT:
      plist[1].real=(double)plist[1].integer;
      plist[1].imag=0;
      break;
    case PL_ARBINT:
      plist[1].real=mpz_get_d(*(ARBINT *)plist[1].pointer); break;
    case PL_FLOAT:
      plist[1].imag=0;
    case PL_COMPLEX:
      break;
    default: xberror(96,variablen[vnr].name); /* Array has wrong type */
    }
    a=create_complex_array(arr->dimension,arr->pointer,(COMPLEX *)&(plist[1].real));
    break;
  case ARBINTTYP: {
    ARBINT b;
    mpz_init(b);
    switch(plist[1].typ) {
    case PL_INT:     mpz_set_si(b,plist[1].integer); break;
    case PL_COMPLEX:
    case PL_FLOAT:   mpz_set_d(b,plist[1].real);     break;
    case PL_ARBINT:  mpz_set(b,*(ARBINT *)plist[1].pointer); break;
    default: xberror(96,variablen[vnr].name); /* Array has wrong type */
    }
    a=create_arbint_array(arr->dimension,arr->pointer,b);
    mpz_clear(b);
    }
    break;
  case STRINGTYP:
    if(plist[1].typ!=PL_STRING) xberror(96,variablen[vnr].name); /* Array has wrong type */
    a=create_string_array(arr->dimension,arr->pointer,(STRING *)&(plist[1].integer));
    break;  
  default:
    xberror(95,variablen[vnr].name); /* Parameter must be Array */
  }
  free_array(arr);
  *arr=a;
}

static char *varinfo(VARIABLE *v) {
  static char info[128];
  char *buf;
  switch(v->typ) {
    case INTTYP:   sprintf(info,"%s%%=%d",v->name,*(v->pointer.i));break;
    case FLOATTYP: sprintf(info,"%s=%.13g",v->name,*(v->pointer.f)); break;
    case COMPLEXTYP: sprintf(info,"%s#=(%.13g+%.13gi)",v->name,(v->pointer.c)->r,(v->pointer.c)->i); break;
    case ARBINTTYP: 
      buf=mpz_get_str(NULL,10,*(v->pointer.ai));
      if(strlen(buf)>60) {
        buf[57]=buf[58]=buf[59]='.';
        buf[60]=0;
      }
      sprintf(info,"%s&=%s",v->name,buf);
      free(buf);
      break;
    case STRINGTYP:
      buf=malloc(min(v->pointer.s->len+1,64));
      stringdump(v->pointer.s->pointer,v->pointer.s->len,buf);
      sprintf(info,"%s$=\"%s\" (len=%d)",v->name,buf,v->pointer.s->len);
      free(buf);
      break;
    default:
      sprintf(info,"?_var_?=?_? ");
  }
  if(v->flags==V_STATIC) strcat(info," (static)");
  return(info);
}

char *dump_var(int typ) {/*  dump variables */
  char *ret=malloc(132*anzvariablen+1);
  *ret=0;
  char *p=ret;
  for(int i=0;i<anzvariablen;i++) {
    if((variablen[i].typ&TYPMASK)==typ) {
      sprintf(p,"%02d: %s\n",i,varinfo(&variablen[i]));
      p+=strlen(p);
    }
  }
  return(ret);
}
char *dump_arr(int typ) {/*  dump arrays */
  int i,j;
  char *ret=malloc(132*anzvariablen+1);
  char suffix[3];
  *ret=0;
  char *p=ret;
  for(i=0;i<anzvariablen;i++) {
    if(variablen[i].typ==ARRAYTYP && (variablen[i].pointer.a->typ&TYPMASK)==typ) {
       switch(variablen[i].pointer.a->typ) {
         case INTTYP:      *suffix='%'; suffix[1]=0; break;
	 case FLOATTYP:    *suffix=0;                break;
	 case COMPLEXTYP:  *suffix='#'; suffix[1]=0; break;
	 case STRINGTYP:   *suffix='$'; suffix[1]=0; break;
	 case ARRAYTYP:    *suffix='('; suffix[1]=')'; suffix[2]=0; break;
	 case ARBINTTYP:     *suffix='&'; suffix[1]=0; suffix[2]=0; break;
	 case ARBFLOATTYP:   *suffix='!'; suffix[1]='!'; suffix[2]=0; break;
	 case ARBCOMPLEXTYP: *suffix='#'; suffix[1]='#'; suffix[2]=0; break;
         default:	   *suffix='?'; suffix[1]=0;
       }
        sprintf(p,"%02d: %s%s(",i,variablen[i].name,suffix);
	p+=strlen(p);
        for(j=0;j<variablen[i].pointer.a->dimension;j++) {
          if(j>0) sprintf(p,",%d",((int *)variablen[i].pointer.a->pointer)[j]);
	  else  sprintf(p,"%d",((int *)variablen[i].pointer.a->pointer)[j]);
	  p+=strlen(p);
        }
	sprintf(p,")  [%d]\n",variablen[i].local);
	p+=strlen(p);
      }
  }
  return(ret);
}


static char *print_pliste(const unsigned short *pl,int pmin, int pmax) {
  int j;
  char *ret=malloc(pmax*32+32);
  char *p=ret;
  *p=0;
  for(j=pmin;j<pmax;j++) {
    switch(pl[j]) {
      case PL_INT:     strcpy(p,"i%"); break;
      case PL_FLOAT:   strcpy(p,"a"); break;
      case PL_COMPLEX: strcpy(p,"z#"); break;
      case PL_ARBINT:  strcpy(p,"i&"); break;
      case PL_FILENR:  strcpy(p,"#n"); break;
      case PL_STRING:  strcpy(p,"t$"); break;
      case PL_ARRAY:   strcpy(p,"arr()"); break;
      case PL_CFAI:
      case PL_CF:
      case PL_NUMBER:   strcpy(p,"num"); break;
      case PL_ANYVALUE: strcpy(p,"value"); break;
      case PL_SVAR: strcpy(p,"var$"); break;
      case PL_IVAR: strcpy(p,"var%"); break;
      case PL_CVAR: strcpy(p,"var#"); break;
      case PL_NVAR: strcpy(p,"nvar"); break;
      case PL_ARRAYVAR:   strcpy(p,"anyarrayvar()"); break;
      case PL_IARRAYVAR:  strcpy(p,"arrayvar%()"); break;
      case PL_FARRAYVAR:  strcpy(p,"arrayvar()"); break;
      case PL_AIARRAYVAR: strcpy(p,"arrayvar&()"); break;
      case PL_CARRAYVAR:  strcpy(p,"arrayvar#()"); break;
      case PL_SARRAYVAR:  strcpy(p,"arrayvar$()"); break;
      case PL_ALLVAR: strcpy(p,"var"); break;
      case PL_KEY:    strcpy(p,"KEY"); break;
      case PL_FARRAY: strcpy(p,"a()"); break;
      case PL_IARRAY: strcpy(p,"h%()"); break;
      case PL_SARRAY: strcpy(p,"f$()"); break;
      case PL_NARRAY: strcpy(p,"n()"); break;
      case PL_CFARRAY: strcpy(p,"c()"); break;
      case PL_LABEL:  strcpy(p,"<label>"); break;
      case PL_PROC:   strcpy(p,"<procedure>"); break;
      case PL_FUNC:   strcpy(p,"<function>"); break;
      default: sprintf(p,"??? -%x-",pl[j]);
    }
    p+=strlen(p);
    if(j<pmax-1) *p++=',';
  }
  return(ret);
}
#define OUTPRINT(a) fputs(a,fff.dptr)

static char *do_explain(int opcode, const char *name,const char *suffix,const char *efix,const unsigned short *pliste, int pmin, int pmax) {
  char *ret=malloc(256);
  char *p=ret;
  switch(opcode&FM_RET) {
  case F_IRET:    strcpy(p,"i%="); break;
  case F_AIRET:   strcpy(p,"i&="); break;
  case F_CRET:    strcpy(p,"c#="); break;
  case F_NRET:    strcpy(p,"n= "); break;
  case F_ANYRET:  strcpy(p,"v= "); break;
  case F_ANYIRET: strcpy(p,"k&="); break;
  case F_DRET:    strcpy(p,"a= "); break;
  case F_SRET:    strcpy(p,"t$="); break;
  case F_ARET:    strcpy(p,"a()="); break;
  default: *p=0;
  }
  p+=strlen(p);
  sprintf(p,"%s%s",name,suffix);
  p+=strlen(p);
  if(pmin) {char *pp=print_pliste(pliste,0,pmin); strcpy(p,pp);free(pp); p+=strlen(p);}
  if(pmin>0 && (pmax>pmin || pmax==-1)) {strcpy(p,"[,");p+=strlen(p);}
  else if(pmax>pmin || pmax==-1) {strcpy(p,"[");p+=strlen(p);}
  if(pmax==-1) {strcpy(p,"...");p+=strlen(p);}
  else {char *pp=print_pliste(pliste,pmin,pmax);strcpy(p,pp);free(pp); p+=strlen(p);}
  if(pmax>pmin || pmax==-1) {strcpy(p,"]");p+=strlen(p);}
  strcpy(p,efix);p+=strlen(p);
  return(ret);
}

/*  DUMP [""][,#n]*/

void c_dump(PARAMETER *plist,int e) {
  int i;
  char kkk=0;
  FILEINFO fff=get_fileptr(-4);  /* stdout */
  if(e>0 && plist->typ!=PL_LEER) kkk=((char *)plist->pointer)[0];
  if(e>1) {
    fff=get_fileptr(plist[1].integer);
    if(fff.typ==0) {xberror(24,""); /* File nicht geoeffnet */return;}
  }


  if(kkk==0 || kkk=='%') {/*  dump ints */
    char *p=dump_var(INTTYP);OUTPRINT(p);free(p);
  }
  if(kkk==0) {/*  dump floats */
    char *p=dump_var(FLOATTYP);OUTPRINT(p);free(p);
  }
  if(kkk==0 || kkk=='$') {/*  dump strings */
    char *p=dump_var(STRINGTYP);OUTPRINT(p);free(p);
  }
  if(kkk==0 || kkk=='#') {/*  dump complex variables */
    char *p=dump_var(COMPLEXTYP);OUTPRINT(p);free(p);
  }
  if(kkk==0 || kkk=='&') {/*  dump arbint variables */
    char *p=dump_var(ARBINTTYP);OUTPRINT(p);free(p);
  }
  if(kkk==0 || kkk=='%' || kkk=='(') {/*  dump int arrays */
    char *p=dump_arr(INTTYP);OUTPRINT(p);free(p);
  }
  if(kkk==0 || kkk=='#' || kkk=='(') {/*  dump complex arrays */
    char *p=dump_arr(COMPLEXTYP);OUTPRINT(p);free(p);
  }
  if(kkk==0 || kkk=='&' || kkk=='(') {/*  dump complex arrays */
    char *p=dump_arr(ARBINTTYP);OUTPRINT(p);free(p);
  }
  
  if(kkk==0 || kkk=='(') {/*  dump arrays */
    char *p=dump_arr(FLOATTYP);OUTPRINT(p);free(p);
  }
  if(kkk==0 || kkk=='$' || kkk=='(') {/*  dump string arrays */
    char *p=dump_arr(STRINGTYP);OUTPRINT(p);free(p);
  }
  if(kkk==':') {/*  dump Labels */
    for(i=0;i<anzlabels;i++) {
      fprintf(fff.dptr,"%s: [%d]\n",labels[i].name,labels[i].zeile);
    }
  }
  if(kkk=='@') {/*  dump Procedures */
    for(i=0;i<anzprocs;i++) {
      fprintf(fff.dptr,"%d|%s [%d]\n",procs[i].typ,procs[i].name,procs[i].zeile);
    }
  }
  if(kkk=='#') {                   /*  dump Channels */
    for(i=0;i<ANZFILENR;i++) {
      if(filenr[i].typ==FT_FILE)        fprintf(fff.dptr,"#%d: %s [%s]\n",i,"","FILE");
      else if(filenr[i].typ==FT_DEV)    fprintf(fff.dptr,"#%d: %s [%s]\n",i,"","DEVICE");
      else if(filenr[i].typ==FT_DLL)    fprintf(fff.dptr,"#%d: %s [%s]\n",i,"","SHARED OBJECT");
      else if(filenr[i].typ==FT_USB)    fprintf(fff.dptr,"#%d: %s [%s]\n",i,"","USB CONNECTION");
      else if(filenr[i].typ==FT_SOCKET) fprintf(fff.dptr,"#%d: %s [%s]\n",i,"","SOCKET");
      else if(filenr[i].typ==FT_PIPE)   fprintf(fff.dptr,"#%d: %s [%s]\n",i,"","PIPE");
      else if(filenr[i].typ==FT_BTRC)   fprintf(fff.dptr,"#%d: %s [%s]\n",i,"","BLUETOOTH RFCOMM");
      else if(filenr[i].typ==FT_BTL2)   fprintf(fff.dptr,"#%d: %s [%s]\n",i,"","BLUETOOTH L2CAP");
      else if(filenr[i].typ)            fprintf(fff.dptr,"#%d: %s [%s]\n",i,"","other");
    }
  }
  if(kkk=='C' || kkk=='K'|| kkk=='c'|| kkk=='k') { /*  dump commands */
    char *p;
    for(i=0;i<anzcomms;i++) {
      p=do_explain(-1,comms[i].name," ","",comms[i].pliste,comms[i].pmin,comms[i].pmax);
      fprintf(fff.dptr,"%3d: [%08x] %s\n",i,(unsigned int)comms[i].opcode,p);
      free(p);
    }
  }
  if(kkk=='F' || kkk=='f') { /*  dump functions */
    char *p;
    for(i=0;i<anzpfuncs;i++) {
      p=do_explain(pfuncs[i].opcode,pfuncs[i].name,"(",")",pfuncs[i].pliste,pfuncs[i].pmin,pfuncs[i].pmax);
      fprintf(fff.dptr,"%3d: [%08x] %s\n",i,(unsigned int) pfuncs[i].opcode,p);
      free(p);
    }    
    for(i=0;i<anzpsfuncs;i++) {
      p=do_explain(psfuncs[i].opcode|F_SRET,psfuncs[i].name,"(",")",psfuncs[i].pliste,psfuncs[i].pmin,psfuncs[i].pmax);
      fprintf(fff.dptr,"%3d: [%08x] %s\n",i,(unsigned int)psfuncs[i].opcode,p);  
      free(p);
    }
    for(i=0;i<anzpafuncs;i++) {
      p=do_explain(pafuncs[i].opcode|F_ARET,pafuncs[i].name,"(",")",pafuncs[i].pliste,pafuncs[i].pmin,pafuncs[i].pmax);
      fprintf(fff.dptr,"%3d: [%08x] %s\n",i,(unsigned int)pafuncs[i].opcode,p);  
      free(p);
    }
  }
}

static void c_end() { batch=0; pc=0; close_all_files();}


/* Handles ON ERROR  {GOTO|GOSUB|CONT} proc/label
           ON BREAK  {GOTO|GOSUB|CONT} proc/label
	   ON MENU   [{GOSUB} proc/label]
 */

static void c_onbreakerrormenugosub(PARAMETER *plist,int e) {
  switch(plist->arraytyp) {
  case KEYW_BREAK: 
    if(e==1) { breakcont=0; return; }  /* ON BREAK */
    breakpc=plist[1].integer;   /* Proc nummer*/
    breakpctype=8|plist[1].arraytyp;
    breakcont=1;
    return;
  case KEYW_ERROR:
    if(e==1) { errcont=0; return; } 
    errorpc=plist[1].integer; /*Proc nummer*/
    errorpctype=8|plist[1].arraytyp;
    errcont=1;
    return;
#ifndef NOGRAPHICS
  case KEYW_MENU:
    if(e==1) { c_menu(""); return;}
    menuaction=plist[1].integer; /*Proc nummer*/
    menuactiontype=plist[1].arraytyp;
    return;
#endif
  default:
    xberror(32,"ON ??"); /* Syntax error */
  }
}
static void c_onbreakerrormenugoto(PARAMETER *plist,int e) {
  switch(plist->arraytyp) {
  case KEYW_BREAK: 
    if(e==1) { breakcont=0; return; }
    breakpc=plist[1].integer; /*Label */
    breakpctype=16|plist[1].arraytyp;
    breakcont=1;
    return;
  case KEYW_ERROR:
    if(e==1) { errcont=0; return; } 
    errorpc=plist[1].integer; /*Label */
    errorpctype=16|plist[1].arraytyp;
    errcont=1;
    return;
#ifndef NOGRAPHICS
  case KEYW_MENU:
    if(e==1) { c_menu(""); return;}
    xberror(32,"ON MENU"); /* Syntax error */
    return;
#endif
  default:
    xberror(32,"ON ??"); /* Syntax error */
  }
}
static void c_onbreakerrormenuother(PARAMETER *plist,int e) {
  if(e==2 && plist[1].typ==PL_LEER) e=1;
  if(e==2 && plist[1].arraytyp==KEYW_NONE) e=1;
  switch(plist->arraytyp) {
  case KEYW_BREAK: 
    if(e==1) { breakcont=0; return; }
    if(plist[1].arraytyp==KEYW_CONT) {
      breakcont=1;
      breakpc=-1;
    } else xberror(32,"ON BREAK"); /* Syntax error */
    return;
  case KEYW_ERROR:
    if(e==1) { errcont=0; return; }
    if(plist[1].arraytyp==KEYW_CONT) {
      errcont=1;
      errorpc=-1;
    } else xberror(32,"ON ERROR"); /* Syntax error */
    return;
#ifndef NOGRAPHICS
  case KEYW_MENU:
    if(e==1) { c_menu(""); return;} 
    xberror(32,"ON MENU"); /* Syntax error */
    return;
#endif
  default:
    xberror(32,"ON ??"); /* Syntax error */
  }
}

static void gosubproc(int pc2,int type) {
  if(pc2!=-1) {
    if(type==0) {
      int oldbatch,osp=sp;
      pc2=procs[pc2].zeile;
      if(stack_check(sp)) {stack[sp++]=pc;pc=pc2+1;}
      else xberror(75,""); /* Stack Overflow! */
        oldbatch=batch;batch=1;
        programmlauf();
        batch=min(oldbatch,batch);
        if(osp!=sp) {
  	  pc=stack[--sp]; /* wenn error innerhalb der func. */
        }
    } else if(type==1) {
      int n;
      batch=1;
      STRING bcpc;
      PARAMETER par[1];
      bcpc.pointer=programbuffer;
      bcpc.len=programbufferlen;
      par[0].integer=0;
      par[0].typ=PL_INT;
      par[0].panzahl=0;
      par[0].ppointer=NULL;
      if(stack_check(sp)) {
        sp++;
        BYTECODE_HEADER *bh=(BYTECODE_HEADER *)programbuffer;
	char *rodata=programbuffer+sizeof(BYTECODE_HEADER)+bh->textseglen;
        stack[sp]=bcpc.len;  /*Return wird diesen Wert holen, dann virt machine beenden.*/
        virtual_machine(bcpc,pc2, &n,par,1,rodata);
        sp--;
      } xberror(75,""); /* Stack Overflow! */
    } else {
      void (*func)();
      func=(void *)INT2POINTER(pc2);
      func();
    }
  }
}
static void gotolabel(int pc2,int type) {
  if(pc2!=-1) {
    if(type==0) {
      batch=1;
      pc=labels[pc2].zeile;      
    } else if(type==1) {
      batch=1;
      // TODO:
    } else {
#ifndef __APPLE__
      void *func=(void *)INT2POINTER(pc2);
      goto *func;
#else
      printf("ERROR/Macintosh MacOS X\nSee Bug 18658 Bugzilla.\nGOTO not possible.\n");
#endif
    }
  }
}

static void c_ongosub(PARAMETER *plist,int e) {
  if(e>1 && plist[0].integer>0 && plist[0].integer<e) {
    gosubproc(plist[plist[0].integer].integer,plist[plist[0].integer].arraytyp);
  }
}
static void c_ongoto(PARAMETER *plist,int e) {
  if(e>1 && plist[0].integer>0 && plist[0].integer<e) {
    gotolabel(plist[plist[0].integer].integer,plist[plist[0].integer].arraytyp);
  }
}

/* Allgemeiner ON Befehl. Sollte nur vom Kommandomodus aus 
   aufgerufen werden....*/

static void c_on(const char *n) {
  char w1[strlen(n)+1],w2[strlen(n)+1],w3[strlen(n)+1];
  int e=wort_sep(n,' ',TRUE,w1,w2);
  int mode=0;
  if(e==0) {
    xberror(32,"ON"); /* Syntax error */
    return;
  }
  wort_sep(w2,' ',TRUE,w2,w3);

  if(!strcmp(w2,"CONT")) mode=1;
  else if(!strcmp(w2,"GOTO")) mode=2;
  else if(!strcmp(w2,"GOSUB")) mode=3;
    
  if(!strcmp(w1,"ERROR")) {
    errcont=(mode>0);
    if(mode==1) errorpctype=0;
    else if(mode==2) {
      errorpctype=16;
      errorpc=labelzeile(w3);
    } else if(mode==3) {
      errorpc=procnr(w3,1);
      if(errorpc!=-1) errorpc=procs[errorpc].zeile;
      errorpctype=8;      
    }
    return;
  } 
  if(strcmp(w1,"BREAK")==0) {
    breakcont=(mode>0);
    if(mode==2) breakpc=labelzeile(w3);
    else if(mode==3) {
      breakpc=procnr(w3,1);
      if(breakpc!=-1) breakpc=procs[breakpc].zeile;
    }
    return;
  }
#ifndef NOGRAPHICS 
  if(strcmp(w1,"MENU")==0) {
    if(mode==0)  c_menu("");  
    else if(mode==3) {
      int pc2=procnr(w3,1);
      if(pc2==-1) xberror(19,w3); /* Procedure nicht gefunden */
      else menuaction=pc2;
    } else  xberror(32,w1);  /* Syntax Error */
    return;
  }
#endif
  /* on n goto/gosub ...  */
  if(mode<2) xberror(32,w1);  /* Syntax Error */
  else {
    int gi=max(0,(int)parser(w1));
    if(gi) {
      while(gi) { e=wort_sep(w3,',',TRUE,w2,w3); gi--;}
      if(e) {
  	if(mode==3) c_gosub(w2);
    	else if(mode==2) c_goto(w2);
      }
    }
  }
}
static void c_absolute(PARAMETER *plist,int e) {
  int vnr=plist->integer;
  erase_variable(&variablen[vnr]);
  variablen[vnr].flags=V_STATIC;
  variablen[vnr].pointer.i=(int *)INT2POINTER(plist[1].integer);
}



static void c_add(PARAMETER *plist,int e) {
  const int vnr=plist->integer;
  const char *varptr=plist->pointer;
  int typ=variablen[vnr].typ;
  if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
  switch(typ) {
  case INTTYP:     *((int *)varptr)+=p2int(&plist[1]); break;
  case FLOATTYP:   *((double *)varptr)+=p2float(&plist[1]); break;
  case COMPLEXTYP: *((COMPLEX *)varptr)=complex_add(*((COMPLEX *)varptr),p2complex(&plist[1])); break;
  case ARBINTTYP: {
    ARBINT a;
    mpz_init(a);
    p2arbint(&plist[1],a);
    mpz_add(*((ARBINT *)varptr),*((ARBINT *)varptr),a);
    mpz_clear(a); 
    }
    break;
  case STRINGTYP:
    if(plist[1].typ==PL_STRING) {
      STRING *s=(STRING *)varptr;
      s->pointer=realloc(s->pointer,s->len+plist[1].integer+1);
      memcpy(s->pointer+s->len,plist[1].pointer,plist[1].integer);
      s->len+=plist[1].integer;
      (s->pointer)[s->len]=0;
    } else xberror(47,""); /*  Parameter %s falsch, kein String */
    break;
  default:
    xberror(32,""); /* Syntax error */
  }
}
static void c_sub(PARAMETER *plist,int e) {
  const int vnr=plist->integer;
  const char *varptr=plist->pointer;
  int typ=variablen[vnr].typ;
  if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
  switch(typ) {
  case INTTYP:  *((int *)varptr)-=p2int(&plist[1]); break;
  case FLOATTYP:*((double *)varptr)-=p2float(&plist[1]); break;
  case COMPLEXTYP: *((COMPLEX *)varptr)=complex_sub(*((COMPLEX *)varptr),p2complex(&plist[1])); break;
  case ARBINTTYP: {
    ARBINT a;
    mpz_init(a);
    p2arbint(&plist[1],a);
    mpz_sub(*((ARBINT *)varptr),*((ARBINT *)varptr),a);
    mpz_clear(a); 
    }
    break;
  default:  xberror(32,""); /* Syntax error */
  }
}
static void c_mul(PARAMETER *plist,int e) {
  const int vnr=plist->integer;
  const char *varptr=plist->pointer;
  int typ=variablen[vnr].typ;
  if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
  switch(typ) {
  case INTTYP:  *((int *)varptr)*=p2int(&plist[1]); break;
  case FLOATTYP:*((double *)varptr)*=p2float(&plist[1]); break;
  case COMPLEXTYP: *((COMPLEX *)varptr)=complex_mul(*((COMPLEX *)varptr),p2complex(&plist[1])); break;
  case ARBINTTYP: {
    ARBINT a;
    mpz_init(a);
    p2arbint(&plist[1],a);
    mpz_mul(*((ARBINT *)varptr),*((ARBINT *)varptr),a);
    mpz_clear(a); 
    }
    break;
  default: xberror(32,""); /* Syntax error */
  }
}
static void c_div(PARAMETER *plist,int e) {
  const int vnr=plist->integer;
  const char *varptr=plist->pointer;
  int typ=variablen[vnr].typ;
  if(typ==ARRAYTYP) typ=variablen[vnr].pointer.a->typ;
  switch(typ) {
  case INTTYP:  *((int *)varptr)/=p2int(&plist[1]); break;
  case FLOATTYP:*((double *)varptr)/=p2float(&plist[1]); break;
  case COMPLEXTYP: *((COMPLEX *)varptr)=complex_div(*((COMPLEX *)varptr),p2complex(&plist[1])); break;
  case ARBINTTYP: {
    ARBINT a;
    mpz_init(a);
    p2arbint(&plist[1],a);
    mpz_div(*((ARBINT *)varptr),*((ARBINT *)varptr),a);
    mpz_clear(a); 
    }
    break;
  default: xberror(32,""); /* Syntax error */
  }
}

/* Vertausche den Inhalt von zwei Variablen*/

static void c_swap(PARAMETER *plist,int e) {
  int vnr1=plist->integer;
  int vnr2=plist[1].integer;
  if(vnr1==vnr2 && plist[0].pointer==plist[1].pointer) return; // nix zu tun
  if(plist[0].typ!=plist[1].typ) {
     xberror(58,""); /* Variable %s ist vom falschen Typ */
     return;
   }
   /* Die p->pointer enthalten das ergebnis von varptr_indexliste() */
   switch(plist->typ) {
   case PL_IVAR:
     { int tmp=*(int *)plist[1].pointer;
       *(int *)plist[1].pointer=*(int *)plist->pointer;
       *(int *)plist->pointer=tmp;
     }
     break;
   case PL_FVAR:
     { double tmp=*(double *)plist[1].pointer;
       *(double *)plist[1].pointer=*(double *)plist[0].pointer;
       *(double *)plist[0].pointer=tmp;
     }
     break;
   case PL_SVAR:
     { STRING tmp=*(STRING *)plist[1].pointer;
       *(STRING *)plist[1].pointer=*(STRING *)plist[0].pointer;
       *(STRING *)plist[0].pointer=tmp;
     }
     break;
   case PL_CVAR:
     { COMPLEX tmp=*(COMPLEX *)plist[1].pointer;
       *(COMPLEX *)plist[1].pointer=*(COMPLEX *)plist->pointer;
       *(COMPLEX *)plist[0].pointer=tmp;
     }
     break;
   case PL_AIVAR:
     { ARBINT tmp;
       mpz_init(tmp);
       mpz_set(tmp,*(ARBINT *)plist[1].pointer);
       mpz_set(*(ARBINT *)plist[1].pointer,*(ARBINT *)plist->pointer);
       mpz_set(*(ARBINT *)plist[0].pointer,tmp);
       mpz_clear(tmp);
     }
     break;
   case PL_FARRAYVAR:
   case PL_IARRAYVAR:
   case PL_CARRAYVAR:
   case PL_AIARRAYVAR:
   case PL_SARRAYVAR:
     { ARRAY tmp=*(ARRAY *)plist[1].pointer;
       *(ARRAY *)plist[1].pointer=*(ARRAY *)plist[0].pointer;
       *(ARRAY *)plist[0].pointer=tmp;
     }
     break;
   default:
     printf("SWAP: vnr=%d %d ",vnr1,vnr2);
     printf("pointer=%p %p ",plist[0].pointer,plist[1].pointer);
     printf("Typ=%x\n",plist->typ);
     xberror(58,""); /* Variable %s ist vom falschen Typ */
   }
} 

#if 0
static void c_do(const char *n) {   /* wird normalerweise ignoriert */
  if(*n==0) ;
  else if(!strncmp(n,"WHILE",5)) c_while(n+6);
  else if(!strncmp(n,"UNTIL",5)) ;
  else xberror(32,n); /*Syntax nicht korrekt*/
}
#endif

/* DIM should not evaluate expressions on runtime, 
 * better have that resolved before (for the compiler).
 * Now: DIM takes only a special Parameter with PL_DIMARG
 * The dimension and dimlist is already be resolved.
 */

static void c_dim(PARAMETER *plist,int e) {
  for(int i=0;i<e;i++) {
    switch(plist[i].typ) {
#if 0
    case PL_EVAL:  /* eigentlich obsolete */
    {
    printf("DIM %d : eval <%s>\n",i,(char *)plist[i].pointer);
      PARAMETER par;
      PARAMETER *p=&par;
      p->pointer=NULL;
      p->integer=-1;
      p->typ=NOTYP; /* Falls type mismatch auftritt, definiertes Ergebnis */

      char w1[strlen(plist[i].pointer)+1],w2[strlen(plist[i].pointer)+1];
      int e=klammer_sep(plist[i].pointer,w1,w2);
 
      if(e==0) { /* String war leer oder falsch formatiert */
          xberror(32,"DIM"); /* Syntax nicht Korrekt */
      } else if(e==1 || *w2==0) {
          xberror(32,"DIM"); /* Syntax nicht Korrekt */
      } else if(e==2) {
        char *r=varrumpf(w1);
        int typ=type(w1)&(~CONSTTYP);  /* Typ Bestimmen  */
        p->integer=add_variable(r,ARRAYTYP,typ,V_DYNAMIC,NULL);  /*  vnr */
    
        int ndim=count_parameters(w2);
        uint32_t dimlist[ndim];
        p->typ=(PL_VARGROUP|typ);
        p->panzahl=ndim;   /* Anzahl indizes z"ahlen*/
        p->ppointer=malloc(sizeof(PARAMETER)*p->panzahl);

        make_preparlist(p->ppointer,w2);
    
        // dump_parameterlist(p,1);
        if(p->integer<0) xberror(76,w1);   /* illegal variable name */
          else if(p->typ==NOTYP) xberror(13,w1);  /* Type mismatch */

         /* Dimlist machen   */
        if(p->panzahl>0) {
          for(int i=0;i<ndim;i++) {
            dimlist[i]=p2int(&(p->ppointer[i]));
          // printf("idx[%d]=%d\n",i,dimlist[i]);
          }
        }

     
   //  printf("DIM: <%s>: dim=%d typ=$%x\n",r,ndim,typ);

          if(p->integer>=0) { /* vnr */
            ARRAY arr=*(variablen[p->integer].pointer.a);
            *(variablen[p->integer].pointer.a)=recreate_array(typ,ndim,dimlist,&arr);
            free_array(&arr); /*alten Inhalt freigeben*/
          }
          free(r);
        }
        if(p->integer<0) xberror(76,w1);   /* illegal variable name */
        else if(p->typ==NOTYP) xberror(13,w1);  /* Type mismatch */
        free_parameter(p);
      }
      break;
#endif
    case PL_DIMARG:
      if(plist[i].integer>=0) { /* vnr */
        //printf("dimlist vnr=%d (%d):\n",plist[i].integer,plist[i].arraytyp);
        //for(int j=0;j<plist[i].arraytyp;j++)
        //  printf("%d:%d\n",j,((int *)plist[i].pointer)[j]); 
	ARRAY arr=*(variablen[plist[i].integer].pointer.a);
	unsigned int typ=arr.typ;
	int ndim=plist[i].arraytyp;
	uint32_t *dimlist=(uint32_t *)plist[i].pointer;
	//printf("typ=%x %s\n",typ,type_name(typ));
	*(variablen[plist[i].integer].pointer.a)=recreate_array(typ,ndim,dimlist,&arr);
	free_array(&arr); /*alten Inhalt freigeben*/
	   
      } else xberror(76,"");   /* illegal variable name */
      break;
    default: 
      xberror(32,"DIM"); /* Syntax error */
      return;
    }
  }
}
static void c_erase(PARAMETER *plist,int e) {
  while(e) erase_variable(&variablen[plist[--e].integer]);
}


/*  Versucht, eine Unterroutine ohne r"ueckgabe zu verlassen.
Liefert 1 wenn das geklappt hat, 0 sonst.
*/
static int do_return() {
  if(sp>0) {
    if(returnvalue.typ!=PL_LEER) free_parameter(&returnvalue);
    restore_locals(sp);
    pc=stack[--sp];
    return(1);
  }
  return(0);
}

static void c_return(const char *n) {
  if(sp>0) {
    if(n && *n) {
      PARAMETER ret;
      bzero(&ret,sizeof(PARAMETER));
      int t=type(n)&(~CONSTTYP);      
      switch(t&TYPMASK) {
      case STRINGTYP:
        *(STRING *)&(ret.integer)=string_parser(n);
        break;
      case COMPLEXTYP:
        *(COMPLEX *)&(ret.real)=complex_parser(n); 
	break;
      case INTTYP:
        ret.integer=(int)parser(n);
	break;
      case FLOATTYP:
        ret.real=parser(n);
	break;
      case ARBINTTYP:
        ret.pointer=malloc(sizeof(ARBINT));
	mpz_init(*((ARBINT *)ret.pointer));
        arbint_parser(n,*((ARBINT *)ret.pointer));
	break;
      default: xberror(13,n);  /* Type mismatch */
      }
      ret.typ=(PL_CONSTGROUP|t);
      if(returnvalue.typ!=PL_LEER) free_parameter(&returnvalue);
      returnvalue=ret;
    }
    restore_locals(sp);
    pc=stack[--sp];
  } else xberror(93,""); /*Stack-Error !*/
}

void c_void(const char *n) {
  int t=type(n)&(~CONSTTYP);
  if(t&ARRAYTYP) {
    ARRAY erg=array_parser(n);
    free_array(&erg);
    return;
  }
  switch(t) {
  case STRINGTYP: 
    free(s_parser(n));
    return;
  case COMPLEXTYP:
    complex_parser(n);
    return;
  case ARBINTTYP: {
    ARBINT a;
    mpz_init(a);
    arbint_parser(n,a);
    mpz_clear(a);
    }
    return;
  case INTTYP:
  case FLOATTYP:
  default: parser(n);
  }
}

static void c_inc(PARAMETER *plist,int e) {
  int typ=variablen[plist->integer].typ;
  if(typ==ARRAYTYP) typ=variablen[plist->integer].pointer.a->typ;
  switch(typ) {
  case FLOATTYP:
  case COMPLEXTYP: (*((double *)plist->pointer))++; return;
  case INTTYP:     (*((int *)plist->pointer))++;    return;
  case ARBINTTYP:  
    mpz_add_ui(*((ARBINT *)plist->pointer),*((ARBINT *)plist->pointer),1);
    return;
  default: xberror(13,variablen[plist->integer].name);  /* Type mismatch */
  }
}

static void c_dec(PARAMETER *plist,int e) { 
  int typ=variablen[plist->integer].typ;
  if(typ==ARRAYTYP)   typ=variablen[plist->integer].pointer.a->typ;
  switch(typ) {
  case FLOATTYP:
  case COMPLEXTYP: (*((double *)plist->pointer))--; return;
  case INTTYP:     (*((int *)plist->pointer))--;    return;
  case ARBINTTYP:  
    mpz_sub_ui(*((ARBINT *)plist->pointer),*((ARBINT *)plist->pointer),1);
    return;
  default: xberror(13,variablen[plist->integer].name);  /* Type mismatch */
  }
}
static void c_cls() { 
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
  FillConsoleOutputCharacter(ConsoleOutput,' ',LINES*COLS,coord,&written);
  FillConsoleOutputAttribute(ConsoleOutput,
    FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE,LINES*COLS,
    coord,&written);

  SetConsoleCursorPosition(ConsoleOutput,coord);
#else
  printf("\033[2J\033[H");
#endif
}
static void c_home() { 
#ifdef WINDOWS
  COORD coord;
  HANDLE ConsoleOutput; /* handle for console output */
  ConsoleOutput=GetStdHandle(STD_OUTPUT_HANDLE); 
  coord.X=0;
  coord.Y=0;
  SetConsoleCursorPosition(ConsoleOutput,coord);
#else
  printf("\033[H");
#endif
}

static void c_version() { puts("X11-BASIC Version: " VERSION " " __DATE__ " " __TIME__);}

static void c_help(PARAMETER *plist,int e) {
  if(e==0 || plist->typ==PL_LEER) puts("HELP [topic]");
  else do_help(plist->pointer);
}  
void do_help(const char *w) {
  int i;
  char *p=NULL;
  // printf("HELP: <%s>\n",w);
  for(i=0;i<anzcomms;i++) {
    // printf("<%s> -> %d\n",comms[i].name,fnmatch(w,comms[i].name,FNM_NOESCAPE));
    if(fnmatch(w,comms[i].name,FNM_NOESCAPE)==0) {
      p=do_explain(-1,comms[i].name," ","",comms[i].pliste,comms[i].pmin,comms[i].pmax);
      printf("Command: %s\n",p);
      free(p);
    }
  }
  for(i=0;i<anzpfuncs;i++) {
    if(fnmatch(w,pfuncs[i].name,FNM_NOESCAPE)==0) {
      p=do_explain(pfuncs[i].opcode,pfuncs[i].name,"(",")",pfuncs[i].pliste,pfuncs[i].pmin,pfuncs[i].pmax);
      printf("Function: %s\n",p);
      free(p);
    }
  }
  for(i=0;i<anzpsfuncs;i++) {
    if(fnmatch(w,psfuncs[i].name,FNM_NOESCAPE)==0) {
      do_explain(psfuncs[i].opcode|F_SRET,psfuncs[i].name,"(",")",psfuncs[i].pliste,psfuncs[i].pmin,psfuncs[i].pmax);
      printf("Function: %s\n",p);
      free(p);
    }
  }
  for(i=0;i<anzsysvars;i++) {
    if(fnmatch(w,sysvars[i].name,FNM_NOESCAPE)==0) {
      printf("Sysvar: ");
      if((sysvars[i].opcode&TYPMASK)==INTTYP) printf("int ");
      else if((sysvars[i].opcode&TYPMASK)==FLOATTYP) printf("flt ");
      else if((sysvars[i].opcode&TYPMASK)==COMPLEXTYP) printf("cpx ");
      else printf("??? ");
      printf("%s\n",sysvars[i].name);	       
    }
  }
  for(i=0;i<anzsyssvars;i++) {
    if(fnmatch(w,syssvars[i].name,FNM_NOESCAPE)==0) {
      printf("Sysvar: %s\n",syssvars[i].name);  	
    }
  }
}
static void c_error(PARAMETER *plist,int e) {xberror(plist->integer,"");}
static void c_free(PARAMETER *plist,int e)  {free((char *)INT2POINTER(plist->integer));}
static void c_detatch(PARAMETER *plist,int e) {
  int r=shm_detatch(INT2POINTER(plist->integer));
  if(r!=0) io_error(r,"DETATCH");
}
static void c_shm_free(PARAMETER *plist,int e) {shm_free(plist->integer);}
static void c_pause(PARAMETER *plist,int e) {
  double zeit=plist->real;
  if(zeit<0) return;
#ifdef WINDOWS
  Sleep((int)(1000*zeit));
#else
  int i=(int)zeit;
  if(i) sleep(i);
  zeit=zeit-(double)i;
  if(zeit>0) usleep((int)(1000000*zeit));
#endif
}

static void c_echo(PARAMETER *plist,int e) {
  int f=plist->arraytyp;
  if(f==KEYW_ON)       echoflag=TRUE; 
  else if(f==KEYW_OFF) echoflag=FALSE;
  else  echoflag=(int)parser(plist->pointer);
}
static void c_gps(PARAMETER *plist,int e) {
  char *n=plist->pointer;
  int f=plist->arraytyp;

#ifdef ANDROID
  if(f==KEYW_ON)       do_gpsonoff(1);  
  else if(f==KEYW_OFF) do_gpsonoff(0);
  else do_gpsonoff((int)parser(n));
#else
  printf("GPS <%s> %x\n",n,f);
#endif
}

#ifdef HAVE_WIRINGPI
extern int wiringpiissetup;
#endif

/* Sets the GPIO of the Raspberry Pi, using the wirig Pi pin numbering
   n=0 -->GPIO 17 
   1           18
   2           27
   3           22
   4           23
   5           24
   6           25
   7           4
   8           2
   9           3
   10          8
   11          7
   12          10
   13           9
   14          11
   15          14
   16          15
   
   21          5
   22          6
   23         13
   24         19
   25         26
   26         12
   27         16
   28         20
   29         21
   30         0
   31         1
*/

static void c_gpio(PARAMETER *plist,int e) {
#ifdef HAVE_WIRINGPI
  int n=plist->integer;
  int f=plist[1].integer;
   if(n<0 || n>31) printf("ERROR: Wrong pin number [0-16, 21-31].\n");
   else {
     if(!wiringpiissetup) {
       if(wiringPiSetup()==-1) {
         printf("Error with wiring Pi setup!\n");
       } else wiringpiissetup=1;
     }
     pinMode(n,OUTPUT);
     digitalWrite(n,f);
  }
#endif
}


static void c_sensor(PARAMETER *plist,int e) {
#ifdef ANDROID
  int f=plist->arraytyp;
  if(f==KEYW_ON) do_sensoronoff(1); 
  else if(f==KEYW_OFF) do_sensoronoff(0); 
  else  do_sensoronoff((int)parser(plist[0].pointer));
#endif
}


static void c_clr(PARAMETER *plist,int e) {
  while(--e>=0) {
   //   dump_parameterlist(&plist[e],1);
    switch(plist[e].typ) {
    case PL_ALLVAR:
    case PL_ARRAYVAR: 
    case PL_AIARRAYVAR: 
    case PL_IARRAYVAR: 
    case PL_FARRAYVAR: 
    case PL_CARRAYVAR: 
    case PL_SARRAYVAR: 
      clear_variable(&variablen[plist[e].integer]);
      break;
    case PL_SVAR:
      *(((STRING *)(plist[e].pointer))->pointer)=0;
      ((STRING *)(plist[e].pointer))->len=0;
      break;
    case PL_IVAR:
      *((int *)(plist[e].pointer))=0;
      break;
    case PL_CVAR:
      ((COMPLEX *)(plist[e].pointer))->r=0;
      ((COMPLEX *)(plist[e].pointer))->i=0;
      break;
    case PL_AIVAR:
      mpz_clear(*((ARBINT *)(plist[e].pointer)));
      mpz_init(*((ARBINT *)(plist[e].pointer)));
      break;
    case PL_FVAR:
      *((double *)(plist[e].pointer))=0;
      break;
    default:
      xberror(13,"CLR");  /* Type mismatch */
    }
  }
}

/*  Liefert 1 zurueck, wenn es geklappt hat, sonst 0*/
static int do_break() {
  if(pc<=0) return(0);
  int i=pcode[pc-1].integer;
  if(i==-1) {
    i=sucheloopend(pc);
    if(i<0) return(0);
    pc=i+1;
  } else pc=i;
  return(1);
}


/*  EXIT                 --- same as return or quit
    EXIT IF <expression>  */

static void c_exit(PARAMETER *plist,int e) {
  if(e==0) {
    if(do_break()) return;
    if(do_return()) return;
    c_quit(NULL,0);   /*  Im Direktmodus und ausserhalb von Schleifen und proceduren 
                          f"uhrt das zum Beenden des Interpreters.*/
  }
  /*  EXIT IF ... */
  if(plist->integer) {
    if(do_break()==0) xberror(36,"EXIT IF"); /*Programmstruktur fehlerhaft */
  }
}


static void c_if(PARAMETER *plist,int e) {
// printf("c_if: %d  pc=%d\n",e,pc);
// dump_parameterlist(plist,e);

  if(plist->integer==0) {
    int i,f=0,o=0;
    for(i=pc; (i<prglen && i>=0);i++) {
      o=pcode[i].opcode&PM_SPECIAL;
      if((o==P_ENDIF || o==P_ELSE|| o==P_ELSEIF)  && f==0) break;
      else if(o==P_IF) f++;
      else if(o==P_ENDIF) f--;
    }
    
    if(i==prglen) { xberror(36,"IF"); /*Programmstruktur fehlerhaft */ return;}
    pc=i+1;  /*hinter(!) ELSE oder ENDIF oder ELSEIF fortfahren.*/
    if(o==P_ELSEIF) {
      /* ELSEIF wird nicht ausgewertet, wenn pc drauflaeuft, da P_PREFETCH.
       * ALSO muessen wir es hier explizit auswerten... 
       * PARAMETERLISTE sollte vorbereitet sein...
       *
       */
      PARAMETER *plist;
      int j=pcode[i].opcode&PM_COMMS;
      int e=make_pliste3(comms[j].pmin,comms[j].pmax,(unsigned short *)comms[j].pliste,
	     pcode[i].ppointer,&plist,pcode[i].panzahl);
          if(e>=0) (comms[j].routine)(plist,e); /* führt c_elseif aus */
	  free_pliste(e,plist);
    } 
  }
}

static void c_select(PARAMETER *plist,int e) {
  if(pc<=0) {xberror(38,"SELECT"); /* Befehl im Direktmodus nicht moeglich */return;}
  int npc=pcode[pc-1].integer;
  if(npc==-1) {
    xberror(36,"SELECT"); /*Programmstruktur fehlerhaft */
    return;
  }
  int wert=plist->integer;
  int wert2=-1;
  char *w1=NULL,*w2,*w3;  
  
  /* Case-Anweisungen finden */
  for(;;) {
    pc=npc;
     if((pcode[pc-1].opcode&PM_SPECIAL)==P_CASE) {
       free(w1);
       w1=strdup(pcode[pc-1].argument);
       e=wort_sep_destroy(w1,',',TRUE,&w2,&w3);
       while(e) {
         wert2=parser(w2);
	 if(wert==wert2) break;
	 e=wort_sep_destroy(w3,',',TRUE,&w2,&w3);
       }
       if(wert==wert2) break;
       else npc=pcode[pc-1].integer;
     } else break;
  } 
  free(w1);
}


  /* case und default 
   * --> gehe zum naechsten ENDSELECT
   */
static void c_case(const char *n) {
  pc=suchep(pc,1,P_ENDSELECT,P_SELECT,P_ENDSELECT);/*gehe zum naechsten ENDSELECT*/
  if(pc==-1) xberror(36,"CASE"); /*Programmstruktur fehlerhaft !*/ 
  pc++;
}



/*Diese routine kann stark verbessert werden, wenn 
  Variablen-typ sowie DOWNTO flag schon beim laden in pass 1 bestimmt wird.*/

static void c_next(PARAMETER *plist,int e) {
  if(pc<=0) {xberror(38,"NEXT"); /* Befehl im Direktmodus nicht moeglich */ return;}
  int hpc=pc;
  pc=pcode[pc-1].integer; /*Hier sind wir beim FOR*/
  if(pc==-1) {xberror(36,"NEXT"); /*Programmstruktur fehlerhaft */return;}

  char *w2,*w3,*var;
  double step, limit,varwert;
  int ss,f=0,type=NOTYP;

  // printf("c_next: das for befindet sich bei pc=%d\n",pc);
  // printf("Argument dort ist: <%s>\n",pcode[pc].argument);

  char *buf=strdup(pcode[pc].argument);
  char *w1=buf;
  int to_or_downto=0;
  
  for(;;) {
    w1=find_next_word(w1,' ',TRUE);
    if(!w1 || !(*w1)) { /* Hinter Ausdruck kommt nix mehr oder Ausdruck war leer*/
      xberror(32,buf); /* Syntax error */
      free(buf);
      return;
    }
    // printf("w1 zeigt auf: <%s>\n",w1);
    if(w1[0]=='T' && w1[1]=='O' && w1[2]==' ') {
      to_or_downto=0;
      break;
    } else if(!strncmp(w1,"DOWNTO ",7)) {
      to_or_downto=1;
      break;
    } else { /* Kein TO oder DOWNTO, also gehört das noch zu dem Ausdruck davor*/
       *(w1-1)=' ';
    }
  }

 
  /* Variable bestimmem */
  if((var=searchchr(buf,'='))!=NULL) {
     *var++=0;
     var=buf;
     type=vartype(var);
     if(type!=INTTYP && type!=FLOATTYP) {
       printf("Syntax Error: FOR %s, illegal variable type.\n",buf);
       xberror(32,buf); /* Syntax error */
       free(buf);
       return;
     }
   } else {
     xberror(32,buf); /* Syntax error */
     free(buf);
     return;
   }
   
   if(!to_or_downto)    {ss=1;  w2=w1+3;} /* TO */ 
   else                 {ss=-1; w2=w1+7;} /* DOWNTO */

   // printf("w2 zeigt auf: <%s>\n",w2);

   w3=w2;
   for(;;) {
     /* Limit bestimmem  */
     w3=find_next_word(w3,' ',TRUE);
     // printf("w2 zeigt auf: <%s>\n",w2);
     if(!w3) { /* Hinter Ausdruck kommt nix mehr oder Ausdruck war leer*/
       if(!(*w2)) { /* Ausdruck war leer */
         xberror(32,w2); /* Syntax error */
         free(buf);
         return;
       }
       step=1;
       limit=parser(w2);
       break;
     } 

     // printf("w3 zeigt auf: <%s>\n",w3);
     if(!strncmp(w3,"STEP ",5)) {
       limit=parser(w2);
       step=parser(w3+5);
       break;
     } else { /* Kein STEP, also gehört das noch zu dem Ausdruck davor*/
       *(w3-1)=' ';
     }
   }

   // printf("Limit=%g step=%g\n",limit,step);

   
   step*=ss;
   varwert=parser(var)+step;
 //  printf("var=<%s>\n",var);
   if(type==FLOATTYP) zuweis(var,varwert);
   else if(type==INTTYP) izuweis(var,(int)varwert);
   
   if(step<0) ss=-1;
   else ss=1;
   /* Schleifenende ueberpruefen    */
   if(ss>0) {
     if(varwert>limit) f=TRUE;
   } else {
     if(varwert<limit) f=TRUE;
   } 
   if(f)  pc=hpc;          /* Schleifenende, gehe hinter NEXT */
   else pc++;
   free(buf);
}

/* FOR
 * -- erledigt nur die erste Zuweiseung der Variable und betritt dann den Schleifenblock 
 */

static void c_for(const char *n) {
  char *buf=strdup(n);
  char *w1=buf;

  for(;;) {
    w1=find_next_word(w1,' ',TRUE);
    if(!w1 || !(*w1)) { /* Hinter Ausdruck kommt nix mehr oder Ausdruck war leer*/
      xberror(32,buf); /* Syntax error */
      free(buf);
      return;
    }
    /* Ist da auch TO oder DOWNTO ?*/
    if((w1[0]=='T' && w1[1]=='O' && w1[2]==' ') || !strncmp(w1,"DOWNTO ",7)) {
      break;
    } else { /* Kein TO oder DOWNTO, also gehört das noch zu dem Ausdruck davor*/
       *(w1-1)=' ';
    }
  }
    
  if((w1=searchchr(buf,'='))!=NULL) {
    *w1++=0;
    xzuweis(buf,w1);
  } else xberror(32,n); /* Syntax error */
  free(buf);
}
static void c_until(PARAMETER *plist, int e) {
  if(pc<=0) {xberror(38,"UNTIL"); /* Befehl im Direktmodus nicht moeglich */ return;}
  if(plist->integer==0) {
    int npc=pcode[pc-1].integer;
    if(npc==-1) xberror(36,"UNTIL"); /*Programmstruktur fehlerhaft */
    else pc=npc+1;
  }
}


/* Dies ist eine modifizierte wort_sep2 routine, welche auch 
   0-Bytes erlaubt. */

static int s_wort_sep2(const STRING *t,const STRING *c,int klamb ,STRING *w1, STRING *w2)    {
  int f=0, klam=0, i=0;
  char a;
 // printf("s_wort_sep2: t(%d), c(%d)\n",t->len,c->len);
  if(!t || t->len==0) {
    *w1=create_string(NULL);
    *w2=create_string(NULL);
    return(0);   /* hier gibts nix zu trennen */
  }
  *w1=double_string(t);
  if(!c || c->len==0 || t->len<=c->len) {
    *w2=create_string(NULL);
    return(1);
  }
//  printf("t=<%s> c=<%s>\n",t->pointer,c->pointer);
  for(;;) {
/* suche erstes Vorkommen von c */
    while(i<t->len && ((t->pointer)[i]!=(c->pointer)[0] || f || klam>0)) {
      a=(t->pointer)[i];
      if(a=='"') f=!f;
      else if(!f && (((klamb&1) && a=='(') || ((klamb&2) && a=='[') || ((klamb&4) && a=='{'))) klam++;
      else if(!f && (((klamb&1) && a==')') || ((klamb&2) && a==']') || ((klamb&4) && a=='}'))) klam--;
      i++;
    }
    if(i==t->len) { /* schon am ende ? */
      *w2=create_string(NULL);
      return(1);
    } else {     /* ueberpruefe, ob auch der Rest von c vorkommt */
  
      if(memcmp(&(t->pointer)[i],c->pointer,c->len)==0) {
        (w1->pointer)[i]=0;
	w1->len=i;
	i+=c->len;
	w2->pointer=malloc(t->len-i+1);
	w2->len=t->len-i;
	memcpy(w2->pointer,&(t->pointer)[i],w2->len);
       (w2->pointer)[w2->len]=0;
        return(2);
      } else {
        a=t->pointer[i];
        if(a=='"') f=!f;
        else if(!f && (((klamb&1) && a=='(') || ((klamb&2) && a=='[') || ((klamb&4) && a=='{'))) klam++;
        else if(!f && (((klamb&1) && a==')') || ((klamb&2) && a==']') || ((klamb&4) && a=='}'))) klam--;
        i++;
      }     /* ansonsten weitersuchen */
    }
  }
}



/* Bei split wollen wir den optionalen int parameter ans ende setzen.
   ist aber noch nicht wegen kompatibilitaet.*/
static void c_split(PARAMETER *plist,int e) {
  STRING str1,str2;
  
  s_wort_sep2((STRING *)&(plist[0].integer),(STRING *)&(plist[1].integer),plist[2].integer,&str1,&str2);
  varcaststring_and_free(plist[3].pointer,str1);  
  if(e>4)  varcaststring_and_free((STRING *)plist[4].pointer,str2);
  else free_string(&str2);
}

/* GET_LOCATION lat,lon,alt,res,speed,....*/
/* globale veriablen, welche die GPS Informationen aufnehmen.*/
double gps_alt,gps_lat=-1,gps_lon=-1;
double gps_bearing,gps_accuracy,gps_speed;
double gps_time;
char *gps_provider;

static void c_getlocation(PARAMETER *plist,int e) {
#ifdef ANDROID
  /* mae sure, that the values get updated */
  ANDROID_get_location();
#endif
  if(e>0 && plist[0].typ!=PL_LEER) varcastfloat(plist[0].integer,plist[0].pointer,gps_lat);
  if(e>1 && plist[1].typ!=PL_LEER) varcastfloat(plist[1].integer,plist[1].pointer,gps_lon);
  if(e>2 && plist[2].typ!=PL_LEER) varcastfloat(plist[2].integer,plist[2].pointer,gps_alt);
  if(e>3 && plist[3].typ!=PL_LEER) varcastfloat(plist[3].integer,plist[3].pointer,gps_bearing);
  if(e>4 && plist[4].typ!=PL_LEER) varcastfloat(plist[4].integer,plist[4].pointer,gps_accuracy);
  if(e>5 && plist[5].typ!=PL_LEER) varcastfloat(plist[5].integer,plist[5].pointer,gps_speed);
  if(e>6 && plist[6].typ!=PL_LEER) varcastfloat(plist[6].integer,plist[6].pointer,gps_time);
  if(e>7 && plist[7].typ!=PL_LEER) {
    STRING a;
    a.pointer=gps_provider;
    if(a.pointer) a.len=strlen(a.pointer);
    else a.len=0;
    varcaststring(plist[7].integer,plist[7].pointer,a);
  }
}


static void c_poke(PARAMETER *plist,int e) {
  char *adr=(char *)INT2POINTER(plist->integer);
  *adr=(char)plist[1].integer;
}
static void c_dpoke(PARAMETER *plist,int e) {
  short *adr=(short *)INT2POINTER(plist->integer);
  *adr=(short)plist[1].integer;
}
static void c_lpoke(PARAMETER *plist,int e) {
  uint32_t *adr=(uint32_t *)INT2POINTER(plist->integer);
  *adr=(uint32_t)plist[1].integer;
}

/* SOUND channel,frequency [Hz],volume (0-1),duration (s)*/

static void c_sound(PARAMETER *plist,int e) { 
  double duration=-1;
  int c=-1;
  double frequency=-1;
  double volume=-1;
  if(plist[0].typ!=PL_LEER) c=plist[0].integer;
  if(e>=2 && plist[1].typ!=PL_LEER) frequency=plist[1].real;
  if(e>=3 && plist[2].typ!=PL_LEER) volume=plist[2].real;
  if(e>=4) duration=plist[3].real;
  sound_activate();
  do_sound(c,frequency,volume,duration);
}

/* PLAYSOUND channel,data$[,pitch,volume] */

static void c_playsound(PARAMETER *plist,int e) { 
  int pitch=0x100,volume=0xffff;
  int c=-1;
  sound_activate();
  if(plist[0].typ!=PL_LEER) c=plist[0].integer;
  if(e>=3) pitch= (int)(plist[2].real*0x100);
  if(e>=4) volume= (int)(plist[3].real*0xffff);
  do_playsound(c,plist[1].pointer,plist[1].integer,pitch,volume,0);
}

static void c_playsoundfile(PARAMETER *plist,int e) {
  if(exist(plist[0].pointer)) {
#ifdef ANDROID
  ANDROID_playsoundfile(plist[0].pointer);
#else
  char buffer[256];
  sprintf(buffer,"ogg123 %s &",(char *)plist[0].pointer); 
  if(system(buffer)==-1) io_error(errno,"system");
#endif    
  } else xberror(-33,plist[0].pointer); /* file not found*/
}


/* WAVE channel,...*/

static void c_wave(PARAMETER *plist,int e) { 
  int c=-1;
  int form=-1;
  double attack=-1;
  double decay=-1;
  double sustain=-1;
  double release=-1;
  
  if(plist[0].typ!=PL_LEER) c=plist[0].integer;
  if(e>=2 && plist[1].typ!=PL_LEER) form=plist[1].integer;
  if(e>=3 && plist[2].typ!=PL_LEER) attack=plist[2].real;
  if(e>=4 && plist[3].typ!=PL_LEER) decay=plist[3].real;
  if(e>=5 && plist[4].typ!=PL_LEER) sustain=plist[4].real;
  if(e>=6 && plist[5].typ!=PL_LEER) release=plist[5].real;
  sound_activate();
  do_wave(c,form,attack,decay,sustain,release);
}

#ifdef ANDROID
  extern void ANDROID_speek(char *,double,double,char *);
#endif

static void c_speak(PARAMETER *plist,int e) { 
#ifdef ANDROID
  double pitch=-1,rate=-1;
  char *enc=NULL;
  if(e>=2) pitch= plist[1].real;
  if(e>=3) rate=plist[2].real;
  if(e>=4) enc=plist[3].pointer;
  ANDROID_speek(plist[0].pointer,pitch,rate,enc);
#endif
}

static void c_eval(PARAMETER *plist,int e) { kommando(plist->pointer); }

#endif
/* Kommandoliste: muss alphabetisch sortiert sein !   */

const COMMAND comms[]= {

 { P_IGNORE,    " nulldummy", NULL        , 0, 0},
 { P_REM,       "!"         , NULL        , 0,-1,(unsigned short []){PL_EVAL}},
 { P_PLISTE,    "?"         , c_print     , 0,-1,(unsigned short []){PL_EVAL}},

 { P_PLISTE,    "ABSOLUTE"  , c_absolute  , 2, 2,(unsigned short []){PL_ANYVAR,PL_INT}},
 { P_PLISTE,    "ADD"       , c_add       , 2, 2,(unsigned short []){PL_ANYVAR,PL_ANYVALUE}},
 { P_PLISTE,    "AFTER"     , c_after     , 2, 2,(unsigned short []){PL_INT,PL_PROC}},
#ifndef NOGRAPHICS
 { P_PLISTE,    "ALERT"     , c_alert     , 5, 6,(unsigned short []){PL_INT,PL_STRING,PL_INT,PL_STRING,PL_NVAR,PL_SVAR}},
#endif
 { P_PLISTE,    "ARRAYCOPY" , c_arraycopy , 2, 2,(unsigned short []){PL_ARRAYVAR,PL_ARRAYVAR}}, /*zweiter parameter muesste "PL_ARRAY sein, nicht ARRAYVAR*/
 { P_PLISTE,    "ARRAYFILL" , c_arrayfill , 2, 2,(unsigned short []){PL_ARRAYVAR,PL_ANYVALUE}},

 { P_SIMPLE,     "BEEP"     , c_beep      ,0, 0},
 { P_SIMPLE,     "BELL"     , c_beep      ,0, 0},
 { P_PLISTE,     "BGET"     , c_bget      ,3,  3,(unsigned short []){PL_FILENR,PL_INT,PL_INT}},
 { P_PLISTE,     "BLOAD"    , c_bload     ,2,  3,(unsigned short []){PL_STRING,PL_INT,PL_INT}},
 { P_PLISTE,     "BMOVE"    , c_bmove     ,3,  3,(unsigned short []){PL_INT,PL_INT,PL_INT} },
#ifndef NOGRAPHICS
 { P_PLISTE,     "BOTTOMW"  , c_bottomw,   0,  1,(unsigned short []){PL_FILENR}},
 { P_PLISTE,     "BOUNDARY" , c_boundary  ,1,  1,(unsigned short []){PL_INT}},
 { P_PLISTE,     "BOX"      , c_box       ,4,  4,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "BPUT"     , c_bput      ,3,  3,(unsigned short []){PL_FILENR,PL_INT,PL_INT}},
 { P_BREAK,      "BREAK"    , NULL        ,0,  0},
 { P_PLISTE,     "BROKER"  , c_broker,    1,4, (unsigned short [])  {PL_STRING,PL_STRING,PL_STRING,PL_NUMBER}},
 { P_PLISTE,     "BSAVE"    , c_bsave     ,3,  3,(unsigned short []){PL_STRING,PL_INT,PL_INT}},

 { P_PLISTE,     "CALL"     , c_call      ,1, -1,(unsigned short []){PL_INT,PL_EVAL}},
 { P_CASE,       "CASE"     , c_case      ,1,  1,(unsigned short []){PL_FLOAT}},
 { P_PLISTE,     "CHAIN"    , c_chain     ,1,  1,(unsigned short []){PL_STRING}},
 { P_PLISTE,     "CHDIR"    , c_chdir     ,1,  1,(unsigned short []){PL_STRING}},
 { P_PLISTE,     "CHMOD"    , c_chmod     ,2,  2,(unsigned short []){PL_STRING,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "CIRCLE"   , c_circle    ,3,  5,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "CLEAR"    , c_clear     ,0, -1,(unsigned short []){PL_ALLVAR}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "CLEARW"   , c_clearw    ,0,  1,(unsigned short []){PL_FILENR}},
 { P_PLISTE,     "CLIP"     , c_clip      ,4,  6,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "CLOSE"    , c_close     ,0, -1,(unsigned short []){PL_FILENR}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "CLOSEW"   , c_closew    ,0,  1,(unsigned short []){PL_FILENR}},
#endif
 { P_PLISTE,     "CLR"      , c_clr       ,1, -1,(unsigned short []){PL_ALLVAR,PL_ALLVAR}},
 { P_SIMPLE,     "CLS"      , c_cls       ,0,  0},
#ifndef NOGRAPHICS
 { P_PLISTE,     "COLOR"    , c_color     ,1,  2,(unsigned short []){PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "CONNECT"  , c_connect   ,2,  3,(unsigned short []){PL_FILENR,PL_STRING,PL_INT}},
 { P_CONTINUE,   "CONTINUE" , c_cont      ,0,  0},
#ifndef NOGRAPHICS
 { P_PLISTE,     "COPYAREA" , c_copyarea  ,6,  6,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
/* Kontrollsystembefehle  */
#ifdef CONTROL
 { P_ARGUMENT,   "CSPUT"    , c_csput     ,2, -1,(unsigned short []){PL_STRING,PL_VALUE}},
 { P_SIMPLE,     "CSCLEARCALLBACKS", c_csclearcallbacks,0,0},
 { P_ARGUMENT,   "CSSET"    , c_csput     ,2, -1,(unsigned short []){PL_STRING,PL_VALUE}},
 { P_ARGUMENT,   "CSSETCALLBACK", c_cssetcallback,2,-1},
 { P_ARGUMENT,   "CSSWEEP"  , c_cssweep   ,2, -1},
 { P_ARGUMENT,   "CSVPUT"   , c_csvput    ,2, -1},
#endif
#ifndef NOGRAPHICS
 { P_PLISTE,     "CURVE"    , c_curve     ,8,  9,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_DATA,       "DATA"     , NULL        ,0, -1,(unsigned short []){PL_EVAL}},
 { P_PLISTE,     "DEC"      , c_dec       ,1,  1,(unsigned short []){PL_NVAR}},
 { P_DEFAULT,    "DEFAULT"  , c_case      ,0,  0},
#ifndef NOGRAPHICS
 { P_PLISTE,     "DEFFILL"  , c_deffill   ,1,  3,(unsigned short []){PL_INT,PL_INT,PL_INT}},
#endif
 { P_DEFFN,      "DEFFN"    , NULL        ,0,  0},
#ifndef NOGRAPHICS
 { P_PLISTE,     "DEFLINE"  , c_defline   ,1,  4,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "DEFMARK"  , c_defmark   ,1,  3,(unsigned short []){PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "DEFMOUSE" , c_defmouse  ,1,  1,(unsigned short []){PL_INT}},
 { P_PLISTE,     "DEFTEXT"  , c_deftext   ,1,  4,(unsigned short []){PL_INT,PL_FLOAT,PL_FLOAT,PL_FLOAT}},
#endif
 { P_PLISTE,     "DELAY"    , c_pause     ,1,  1,(unsigned short []){PL_FLOAT}},
 { P_PLISTE,     "DIM"      , c_dim       ,1, -1,(unsigned short []){PL_DIMARG,PL_DIMARG}},
 { P_PLISTE,     "DIV"      , c_div       ,2,  2,(unsigned short []){PL_NVAR,PL_NUMBER}},
 { P_DO,         "DO"       , NULL        ,0,  0},
#ifdef DOOCS
/* { P_ARGUMENT,   "TINEBROADCAST", c_tinebroadcast,1,-1,{PL_STRING}},
 { P_SIMPLE,     "TINECYCLE", c_tinecycle,0,0},
 { P_ARGUMENT,   "TINEDELIVER", c_tinedeliver,1,-1},   */
 { P_ARGUMENT,   "DOOCSCALLBACK", c_doocscallback,2,3, (unsigned short []){PL_VAR,PL_PROC,PL_PROC}},
 { P_ARGUMENT,   "DOOCSEXPORT", c_doocsexport,1,-1},
/* { P_ARGUMENT,   "TINELISTEN", c_tinelisten,1,-1,{PL_STRING}},
 { P_PLISTE,     "TINEMONITOR", c_tinemonitor,2,3,{PL_STRING,PL_PROC,PL_INT}},*/
 { P_ARGUMENT,   "DOOCSPUT"    , c_doocsput ,2,-1,(unsigned short []){PL_STRING}},
 { P_PLISTE,     "DOOCSSERVER" , c_doocsserver,0,2,(unsigned short []){PL_STRING,PL_INT}},
 { P_ARGUMENT,   "DOOCSSET"    , c_doocsput ,2,-1,(unsigned short []){PL_STRING}},
#endif
 { P_PLISTE,     "DPOKE"    , c_dpoke,       2,2,(unsigned short []){PL_INT,PL_INT}},
#ifndef NOGRAPHICS
 { P_ARGUMENT,   "DRAW"     , c_draw ,2,-1,(unsigned short []){PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "DUMP"     , c_dump ,0,2,(unsigned short []){PL_STRING,PL_FILENR}},

 { P_PLISTE,     "ECHO"     , c_echo ,1,1,(unsigned short []){PL_KEY}},
 { P_SIMPLE,     "EDIT"     , c_edit ,0,0},
#ifndef NOGRAPHICS
 { P_PLISTE,     "ELLIPSE"  , c_ellipse,4,6,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_ELSE,       "ELSE"     , c_if  ,1,1,(unsigned short []){PL_INT}}, /* ELSE IF something*/
 { P_SIMPLE,     "END"      , c_end   ,0,0},
 { P_ENDPROC,    "ENDFUNCTION", c_return,0,0},
 { P_ENDIF,      "ENDIF"    , NULL  ,0,0},
 { P_ENDPROC,    "ENDPROCEDURE", c_return,0,0},
 { P_ENDSELECT,  "ENDSELECT", NULL  ,0,0},
 { P_PLISTE,     "ERASE"    , c_erase,1,-1,(unsigned short []){PL_ARRAYVAR,PL_ARRAYVAR}},
 { P_PLISTE,     "ERROR"    , c_error,1,1,(unsigned short []){PL_INT}},
 { P_PLISTE,     "EVAL"     , c_eval,1,1,(unsigned short []){PL_STRING}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "EVENT"    , c_allevent,0,10,(unsigned short []){PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_SVAR,PL_NVAR}},
#endif
 { P_PLISTE,     "EVERY"    , c_every,2,2,(unsigned short []){PL_INT,PL_PROC}},
 { P_PLISTE,     "EXEC"     , c_exec,1,3,(unsigned short []){PL_STRING,PL_STRING,PL_STRING}},
 { P_PLISTE,     "EXIT"     , c_exit,0,1,(unsigned short []){PL_INT}}, /* EXIT IF something*/
/*
 { P_ARGUMENT,   "EXPORT"   , c_export,1,2, {PL_ALLVAR, PL_NUMBER}},
*/
 { P_PLISTE,     "FFT"      , c_fft,1,2,(unsigned short []){PL_FARRAYVAR,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "FILESELECT", c_fileselect,4, 4,(unsigned short []){PL_STRING,PL_STRING,PL_STRING,PL_SVAR}},
 { P_PLISTE,     "FILL",       c_fill,      2, 3,(unsigned short []){PL_INT,PL_INT,PL_INT}},
#endif
 { P_ARGUMENT,   "FIT",        c_fit,       4,10,(unsigned short []){PL_FARRAY,PL_FARRAY}},
 { P_PLISTE,     "FIT_LINEAR", c_fit_linear,5,11,(unsigned short []){PL_FARRAY,PL_FARRAY,PL_INT,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_FARRAY,PL_FARRAY,PL_NVAR}},
 { P_PLISTE,     "FIT_POLY",   c_fit_poly,  6, 6,(unsigned short []){PL_FARRAY,PL_FARRAY,PL_FARRAY,PL_INT,PL_FARRAYVAR,PL_INT}},
 { P_PLISTE,     "FLUSH",      c_flush,     0, 1,(unsigned short []){PL_FILENR}},
 { P_FOR,        "FOR",        c_for,       1,-1,(unsigned short []){PL_EVAL,PL_KEY,PL_NUMBER,PL_KEY,PL_NUMBER}},
 { P_PLISTE,     "FREE",       c_free,      1, 1,(unsigned short []){PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "FULLW",      c_fullw,     0, 1,(unsigned short []){PL_FILENR}},
#endif
 { P_PROC,       "FUNCTION" , c_end,0,0},
#ifndef NOGRAPHICS
 { P_PLISTE,     "GET"      , c_get,5,6,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT,PL_SVAR,PL_INT}},
 { P_PLISTE,     "GET_GEOMETRY" , c_getgeometry,2,7,(unsigned short []){PL_FILENR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { P_PLISTE,     "GET_LOCATION" , c_getlocation,2,8,(unsigned short []){PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_SVAR}},
 
 { P_PLISTE,     "GET_SCREENSIZE" , c_getscreensize,1,5,(unsigned short []){PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
#endif
 { P_GOSUB,      "GOSUB"    , c_gosub,1,1,(unsigned short []){PL_PROC}},
 { P_GOTO,       "GOTO"     , c_goto,1,1,(unsigned short []){PL_LABEL}},
 { P_PLISTE,     "GPIO"     , c_gpio ,2,2,(unsigned short []){PL_INT,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "GPRINT"    , c_gprint,       0,-1,(unsigned short []){PL_EVAL}},
#endif
 { P_PLISTE,     "GPS"     , c_gps ,1,1,(unsigned short []){PL_KEY}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "GRAPHMODE", c_graphmode,1,1,(unsigned short []){PL_INT}},
#endif
 { P_PLISTE,     "HELP"    , c_help,0,1,(unsigned short []){PL_KEY}},
#ifndef NOGRAPHICS
 { P_SIMPLE,     "HIDEK"     , c_hidek,0,0},
 { P_SIMPLE,     "HIDEM"     , c_hidem,0,0},
#endif
 { P_SIMPLE,     "HOME"     , c_home,0,0},

 { P_IF,         "IF"       , c_if,1,1,(unsigned short []){PL_INT}},
 { P_PLISTE,     "INC"      , c_inc,1,1,(unsigned short []){PL_NVAR}},
#ifndef NOGRAPHICS
 { P_PLISTE,	 "INFOW"    , c_infow,    2,2,(unsigned short []){PL_FILENR,PL_STRING}},
#endif
 { P_ARGUMENT,   "INPUT"    , c_input,1,-1,(unsigned short []){PL_ALLVAR}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "KEYEVENT" , c_keyevent,0,8,(unsigned short []){PL_NVAR,PL_NVAR,PL_SVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
#endif
 { P_PLISTE,     "KILL"    , c_kill     ,1, 1,(unsigned short []){PL_STRING}},


 { P_ARGUMENT,   "LET"      , c_let,1,-1,(unsigned short []){PL_KEY}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "LINE"     , c_line,4,4,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_ARGUMENT,   "LINEINPUT", c_lineinput,1,2, (unsigned short []){PL_FILENR,PL_STRING}},
 { P_PLISTE,     "LINK"     , c_link,       2,2,(unsigned short []){PL_FILENR,PL_STRING}},

 { P_PLISTE,     "LIST"     , c_list,0,2,(unsigned short []){PL_INT,PL_INT}},
 { P_PLISTE,     "LOAD"     , c_load,1,1,(unsigned short []){PL_STRING}},
 { P_PLISTE,     "LOCAL"    , c_local,1,-1,(unsigned short []){PL_ALLVAR,PL_ALLVAR}},
 { P_PLISTE,     "LOCATE"    , c_locate,2,2,(unsigned short []){PL_INT,PL_INT}},
 { P_LOOP,       "LOOP"     , NULL,0,0},
 { P_PLISTE,     "LPOKE"    , c_lpoke,       2,2,(unsigned short []){PL_INT,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "LTEXT"     , c_ltext,3,3,(unsigned short []){PL_INT,PL_INT,PL_STRING}},
#endif

 { P_PLISTE,     "MEMDUMP"    , c_memdump,2,2,(unsigned short []){PL_INT,PL_INT}},
#ifndef NOGRAPHICS
 { P_SIMPLE,     "MENU"     , c_menu,0,0},
 { P_PLISTE,     "MENUDEF"  , c_menudef,1,2,(unsigned short []){PL_SARRAY,PL_PROC}},
 { P_SIMPLE,     "MENUKILL" , c_menukill,0,0},
 { P_PLISTE,     "MENUSET"  , c_menuset,2,2,(unsigned short []){PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "MERGE"    , c_merge,1,1,(unsigned short []){PL_STRING}},
 { P_PLISTE,     "MFREE"      , c_free,1,1,(unsigned short []){PL_INT}},
 { P_PLISTE,     "MKDIR"    , c_mkdir     ,1, 2,(unsigned short []){PL_STRING,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "MOUSE"    , c_mouse,1,5,(unsigned short []){PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { P_PLISTE,     "MOUSEEVENT" , c_mouseevent,0,6,(unsigned short []){PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { P_PLISTE,     "MOTIONEVENT" , c_motionevent,0,6,(unsigned short []){PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR,PL_NVAR}},
 { P_PLISTE,     "MOVEW"    , c_movew,3,3, (unsigned short []){PL_FILENR,PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "MSYNC"     , c_msync  ,2,2,(unsigned short []){PL_INT, PL_INT}},
 { P_PLISTE,     "MUL"      , c_mul,2,2,(unsigned short []){PL_NVAR,PL_NUMBER}},

 { P_SIMPLE,     "NEW"      , c_new,0,0},
 { P_NEXT,       "NEXT"     , c_next,0,1,(unsigned short []){PL_NVAR}},
 { P_IGNORE,     "NOOP",         NULL,         0,0},
 { P_IGNORE,     "NOP",          NULL,         0,0},
#ifndef NOGRAPHICS
 { P_SIMPLE,     "NOROOTWINDOW", c_norootwindow,0,0},
 { P_PLISTE,     "OBJC_ADD"    , c_objc_add,      3,3,(unsigned short []){PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "OBJC_DELETE"    , c_objc_delete,      2,2,(unsigned short []){PL_INT,PL_INT}},
#endif
 { P_ARGUMENT,   "ON"       , c_on,         1,-1,(unsigned short []){PL_KEY}},
 { P_PLISTE,     "ON B/E/M GOSUB" , c_onbreakerrormenugosub,    1,2,(unsigned short []){PL_KEY,PL_PROC}},
 { P_PLISTE,     "ON B/E/M GOTO" , c_onbreakerrormenugoto,    1,2,(unsigned short []){PL_KEY,PL_LABEL}},
 { P_PLISTE,     "ON B/E/M OTHER" , c_onbreakerrormenuother,    1,2,(unsigned short []){PL_KEY,PL_KEY}},
 { P_PLISTE,     "ON GOSUB" , c_ongosub,    2,-1,(unsigned short []){PL_INT,PL_PROC,PL_PROC}},
 { P_PLISTE,     "ON GOTO"  , c_ongoto,     2,-1,(unsigned short []){PL_INT,PL_LABEL,PL_LABEL}},
 { P_PLISTE,     "OPEN"     , c_open,       3,4,(unsigned short []){PL_STRING,PL_FILENR,PL_STRING,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "OPENW"    , c_openw,      1,1,(unsigned short []){PL_FILENR}},
#endif
 { P_PLISTE,     "OUT"      , c_out,        2,-1,(unsigned short []){PL_FILENR,PL_ANYVALUE,PL_ANYVALUE}},

 { P_PLISTE,     "PAUSE"    , c_pause,      1,1,(unsigned short []){PL_FLOAT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "PBOX"     , c_pbox ,      4,4,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "PCIRCLE"  , c_pcircle,    3,5,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "PELLIPSE" , c_pellipse,   4,6,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "PIPE" , c_pipe,   2,2,(unsigned short []){PL_FILENR,PL_FILENR}},
 { P_PLISTE,     "PLAYSOUND",     c_playsound, 2,4,(unsigned short []){PL_INT,PL_STRING,PL_FLOAT,PL_FLOAT}},
 { P_PLISTE,     "PLAYSOUNDFILE",     c_playsoundfile, 1,1,(unsigned short []){PL_STRING}},
 { P_PLISTE,     "PLIST"    , c_plist,      0,1,(unsigned short []){PL_FILENR}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "PLOT"     , c_plot,       2,2,(unsigned short []){PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "POKE"     , c_poke,       2,2,(unsigned short []){PL_INT,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "POLYFILL" , c_polyfill,         3,7,(unsigned short []){PL_INT,PL_IARRAY,PL_IARRAY,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "POLYLINE" , c_polyline,       3,7,(unsigned short []){PL_INT,PL_IARRAY,PL_IARRAY,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "POLYMARK" , c_polymark,   3,7,(unsigned short []){PL_INT,PL_IARRAY,PL_IARRAY,PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,     "PRBOX"    , c_prbox ,      4,4,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "PRINT"    , c_print,       0,-1,(unsigned short []){PL_EVAL}},
 { P_PROC,       "PROCEDURE", c_end  ,      0,0},
 { P_IGNORE,     "PROGRAM"  , NULL  ,      0,0},
 /* Ausdruck als Message queuen oder per MQTT pushen */
 { P_PLISTE,     "PUBLISH"  , c_publish,   2,4, (unsigned short [])  {PL_STRING,PL_STRING,PL_NUMBER,PL_NUMBER}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "PUT"  , c_put,      3,10,(unsigned short []){PL_INT,PL_INT,PL_STRING,PL_FLOAT,PL_INT,PL_INT,PL_INT,PL_INT,PL_INT,PL_FLOAT}},
#endif
 { P_PLISTE,     "PUTBACK"  , c_unget,      2,2,(unsigned short []){PL_FILENR,PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "PUT_BITMAP"  , c_put_bitmap, 5,5,(unsigned short []){PL_STRING,PL_INT,PL_INT,PL_INT,PL_INT}},
#endif

 { P_PLISTE,     "QUIT"     , c_quit,       0,1,(unsigned short []){PL_INT}},

 { P_PLISTE,     "RANDOMIZE", c_randomize  ,      0,1,(unsigned short []){PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "RBOX"      , c_rbox       ,4, 4,(unsigned short []) {PL_INT,PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "READ"     , c_read,       1,-1,(unsigned short []){PL_ALLVAR,PL_ALLVAR}},
 { P_PLISTE,     "RECEIVE"  , c_receive,    2,3,(unsigned short []){PL_FILENR,PL_SVAR,PL_NVAR}},
 { P_PLISTE,     "RELSEEK"  , c_relseek,    2,2,(unsigned short []){PL_FILENR,PL_INT}},
 { P_REM,        "REM"      , NULL  ,      0,-1,(unsigned short []){PL_EVAL}},
 { P_PLISTE,     "RENAME"     , c_rename,2,2,(unsigned short []){PL_STRING,PL_STRING}},
 { P_REPEAT,     "REPEAT"   , NULL  ,      0,0},
 { P_PLISTE,     "RESTORE"  , c_restore,    0,1,(unsigned short []){PL_LABEL}},
 { P_RETURN,     "RETURN"   , c_return,     0,1,(unsigned short []){PL_VALUE}},
 { P_PLISTE,     "RMDIR"    , c_rmdir     ,1, 1,(unsigned short []){PL_STRING}},
#ifndef NOGRAPHICS
 { P_SIMPLE,     "ROOTWINDOW", c_rootwindow,0,0},
 { P_SIMPLE,     "RSRC_FREE", c_rsrc_free,0,0},
 { P_PLISTE,     "RSRC_LOAD", c_rsrc_load,1,1,(unsigned short []){PL_STRING}},
#endif

 { P_SIMPLE,     "RUN"      , c_run,        0,0},

 { P_PLISTE,     "SAVE"     , c_save,0,1,(unsigned short []){PL_STRING}},
#ifndef NOGRAPHICS
 { P_PLISTE,     "SAVESCREEN", c_savescreen,1,1,(unsigned short []){PL_STRING}},
 { P_PLISTE,     "SAVEWINDOW", c_savewindow,1,1,(unsigned short []){PL_STRING}},
 { P_ARGUMENT,   "SCOPE"    , c_scope,      1,6,(unsigned short []){PL_FARRAY,PL_ANYVALUE,PL_FLOAT,PL_FLOAT,PL_FLOAT,PL_FLOAT}},
 { P_PLISTE,     "SCREEN"    , c_screen,      1,3,(unsigned short []){PL_INT,PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "SEEK"     , c_seek,       1,2,(unsigned short []){PL_FILENR,PL_INT}},
 { P_SELECT,     "SELECT"   , c_select,     1,1,(unsigned short []){PL_INT}},
 /*
 { P_ARGUMENT,   "SEMGIVE"  , c_semgive, 1,2,{PL_NUMBER,PL_NUMBER}},
 { P_ARGUMENT,   "SEMTAKE"  , c_semtake, 1,2,{PL_NUMBER,PL_NUMBER}},
 */
 { P_PLISTE,     "SEND"   , c_send,     2,4,(unsigned short []){PL_FILENR,PL_STRING,PL_INT,PL_INT}},
 { P_PLISTE,     "SENSOR" , c_sensor ,1,1,(unsigned short []){PL_KEY}},
#ifndef NOGRAPHICS
 { P_PLISTE,	 "SETFONT"  , c_setfont,    1,1,(unsigned short []){PL_STRING}},
 { P_PLISTE,	 "SETMOUSE" , c_setmouse,   2,4,(unsigned short []){PL_INT,PL_INT,PL_INT,PL_INT}},
 { P_PLISTE,	 "SGET" , c_sget,   1,1,(unsigned short []){PL_SVAR}},
#endif
 { P_PLISTE,	 "SHELL"   , c_shell,     1,-1,(unsigned short []){PL_STRING}},
 { P_PLISTE,     "SHM_DETACH"      , c_detatch,1,1,(unsigned short []){PL_INT}},
 { P_PLISTE,     "SHM_FREE" , c_shm_free,1,1,(unsigned short []){PL_INT}},
#ifndef NOGRAPHICS
 { P_SIMPLE,     "SHOWK"     , c_showk,0,0},
 { P_SIMPLE,     "SHOWM"     , c_showm,0,0},
 { P_SIMPLE,	 "SHOWPAGE" , c_vsync,      0,0},
 { P_PLISTE,	 "SIZEW"    , c_sizew,      3,3,(unsigned short []){PL_FILENR,PL_INT,PL_INT}},
#endif
 { P_PLISTE,     "SORT",      c_sort,        1,3,(unsigned short []){PL_ARRAYVAR,PL_INT,PL_IARRAYVAR}},
 { P_PLISTE,     "SOUND",     c_sound,        2,4,(unsigned short []){PL_INT,PL_FLOAT,PL_FLOAT,PL_FLOAT}},

 { P_GOSUB,      "SPAWN"    , c_spawn,1,1,(unsigned short []){PL_PROC}},
 { P_PLISTE,     "SPEAK",     c_speak, 1,4,(unsigned short []){PL_STRING,PL_FLOAT,PL_FLOAT,PL_STRING}},

 { P_PLISTE,	 "SPLIT"    , c_split,  4,5,(unsigned short []){PL_STRING,PL_STRING,PL_INT,PL_SVAR,PL_SVAR}},
#ifndef NOGRAPHICS
 { P_PLISTE,	 "SPUT"     , c_sput,      1,1,(unsigned short []){PL_STRING}},
#endif
 { P_SIMPLE,	 "STOP"     , c_stop,       0,0},
 { P_PLISTE,	 "SUB"      , c_sub,        2,2,(unsigned short []){PL_NVAR,PL_NUMBER}},
 /* MQTT subscribe */
 { P_PLISTE,     "SUBSCRIBE", c_subscribe,  2,4,(unsigned short []){PL_STRING,PL_SVAR,PL_PROC,PL_NUMBER}},
 { P_PLISTE,	 "SWAP"     , c_swap,       2,2,(unsigned short []){PL_ALLVAR,PL_ALLVAR}},
 { P_PLISTE,	 "SYSTEM"   , c_system,     1,1,(unsigned short []){PL_STRING}},

#ifndef NOGRAPHICS
 { P_PLISTE,	 "TEXT"     , c_text,       3,3,(unsigned short []){PL_INT,PL_INT,PL_STRING}},
#endif
#ifdef TINE
 { P_ARGUMENT,   "TINEBROADCAST", c_tinebroadcast,1,-1,(unsigned short []){PL_STRING}},
 { P_SIMPLE,     "TINECYCLE", c_tinecycle,0,0},
 { P_ARGUMENT,   "TINEDELIVER", c_tinedeliver,1,-1},
 { P_ARGUMENT,   "TINEEXPORT", c_tineexport,1,-1},
 { P_ARGUMENT,   "TINELISTEN", c_tinelisten,1,-1,(unsigned short []){PL_STRING}},
 { P_PLISTE,     "TINEMONITOR", c_tinemonitor,2,3,(unsigned short []){PL_STRING,PL_PROC,PL_INT}},
 { P_ARGUMENT,   "TINEPUT"    , c_tineput ,2,-1,(unsigned short []){PL_STRING}},
 { P_PLISTE,     "TINESERVER" , c_tineserver,0,2,(unsigned short []){PL_STRING,PL_INT}},
 { P_ARGUMENT,   "TINESET"    , c_tineput ,2,-1,(unsigned short []){PL_STRING}},
#endif
#ifndef NOGRAPHICS
 { P_PLISTE,	 "TITLEW"   , c_titlew,     2,2,(unsigned short []){PL_FILENR,PL_STRING}},
 { P_PLISTE,     "TOPW"     , c_topw,       0,1,  (unsigned short []) {PL_FILENR}},
#endif
 { P_PLISTE,     "TOUCH"    , touch,1,1,(unsigned short []){PL_FILENR}},
 { P_SIMPLE,	 "TROFF"    , c_troff,      0,0},
 { P_SIMPLE,	 "TRON"     , c_tron,       0,0},

 { P_PLISTE,     "UNLINK"   , c_close  ,     1,-1,(unsigned short []){PL_FILENR,PL_FILENR}},
 { P_PLISTE,     "UNMAP"    , c_unmap      ,2,2,(unsigned short []){PL_INT, PL_INT}},
 { P_UNTIL,	 "UNTIL"    , c_until,      1,1,(unsigned short []){PL_INT}},
#ifndef NOGRAPHICS
 { P_PLISTE,	 "USEWINDOW", c_usewindow,  1,1,(unsigned short []){PL_FILENR}},
#endif

 { P_SIMPLE,	 "VERSION"  , c_version,    0,0},
 { P_ARGUMENT,	 "VOID"     , c_void,       1,1,(unsigned short []){PL_EVAL}},
#ifndef NOGRAPHICS
 { P_SIMPLE,	 "VSYNC"    , c_vsync,      0,0},
#endif
 { P_PLISTE,     "WATCH"    , c_watch      ,1,1,(unsigned short []){PL_STRING}},
 { P_PLISTE,     "WAVE"     , c_wave       ,2,6,(unsigned short []){PL_INT,PL_INT,PL_FLOAT,PL_FLOAT,PL_FLOAT,PL_FLOAT}},

 { P_WEND,       "WEND"     , NULL,       0,0},
 { P_WHILE,	 "WHILE"    , c_while,    1,1,(unsigned short []){PL_INT}},
 { P_PLISTE,	 "WORT_SEP" , c_split,    4,5,(unsigned short []){PL_STRING,PL_STRING,PL_INT,PL_SVAR,PL_SVAR}},
#ifndef NOGRAPHICS
 { P_SIMPLE,	 "XLOAD"    , c_xload,    0,0},
 { P_SIMPLE,	 "XRUN"     , c_xrun,     0,0},
#endif

};
const int anzcomms=sizeof(comms)/sizeof(COMMAND);
