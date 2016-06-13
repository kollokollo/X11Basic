/* xb2c.C   The X11-basic to C translator   (c) Markus Hoffmann 2010-2016
*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ======================================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#if defined WINDOWS || defined ANDROID
#define EX_CANTCREAT	73	/* can't create (user) output file */
#define EX_NOINPUT	66	/* cannot open input */
#define EX_OK 0
#else
#include <sysexits.h>
#endif

#include "defs.h"
#include "x11basic.h"
#include "xbasic.h"
#include "bytecode.h"
#include "variablen.h"
#include "file.h"
#include "virtual-machine.h"
//#include "xb2c.h"


char ifilename[128]="b.b";       /* Standard inputfile  */
char ofilename[128]="11.c";       /* Standard outputfile     */
int loadfile=FALSE;
#ifdef ATARI
int verbose=1;
#else
int verbose=0;
#endif
int nomain=0;

BYTECODE_HEADER *bytecode;

BYTECODE_SYMBOL *symtab;
char *strings;
unsigned char *datasec;
char *rodata;


FILE *optr;

/* X11-Basic needs these declar<ations:  */
int prglen=0;
const char version[]="1.24";        /* Programmversion*/
const char vdate[]="2016-01-01";
char *programbuffer=NULL;
char **program=NULL;
int programbufferlen=0;

extern void memdump(unsigned char *,int);

#ifdef DUMMY_LIST
/*Kurzform der Error-Routine, tritt hier nur bei IO-Errors auf.*/
void xberror(char errnr, const char *bem) {
  printf("ERROR: #%d %s\n",errnr,bem);
  perror(strerror(errno));
}
#else 
void xberror(char errnr, const char *bem);
#endif

static void intro(){
  printf("********************************************************\n"
         "*     X11-Basic bytecode to C translator               *\n"
         "*                  by Markus Hoffmann 1997-2016 (c)    *\n"
         "* V.%s/%04x    date: %30s  *\n"
         "********************************************************\n",version,BC_VERSION,vdate);
}
static void usage(){
  printf("\n Usage:\n ------ \n"
         " %s [-o <outputfile> -h] [<filename>] --- translate program [%s]\n\n"
         "-o <outputfile>\t--- put result in file [%s]\n"
         "-h --help\t--- Usage\n"
         "-l       \t--- suppress the main function\n"
         "-v\t\t--- be more verbose\n"
	 ,"xb2c",ifilename,ofilename);
}

static void kommandozeile(int anzahl, char *argumente[]) {
  int count;

  /* Kommandozeile bearbeiten   */
  for(count=1;count<anzahl;count++) {
    if (strcmp(argumente[count],"-o")==FALSE) {
      strcpy(ofilename,argumente[++count]);
    } else if (strcmp(argumente[count],"-h")==FALSE || strcmp(argumente[count],"--help")==FALSE) {
      intro();usage();
    } else if (strcmp(argumente[count],"-v")==FALSE) verbose++;
    else if(strcmp(argumente[count],"-q")==FALSE)   verbose--;
    else if(strcmp(argumente[count],"-l")==FALSE)   nomain=1;
    else {
      if(!loadfile) {
        loadfile=TRUE;
        strcpy(ifilename,argumente[count]);
      }
    }
  }
}

static int havesymbol(int adr,int typ) {
  int i,c=bytecode->symbolseglen/sizeof(BYTECODE_SYMBOL);
  for(i=0;i<c;i++) {
    if(symtab[i].adr==adr && symtab[i].typ==typ) return(i);
  }
  return(-1);
}
static int frishmemcpy(unsigned char *d,unsigned char *s,unsigned int n) {
  int i,j=0;
  for(i=0;i<n;i++) {
    if(s[i]=='\"' || s[i]=='\\') {
      d[j++]='\\';
      d[j++]=s[i];  
    } else if(!isprint(s[i])) {
      sprintf((char *)&d[j],"\\x%02x\" \"",s[i]);
     /* printf("sonderzeichen %d %02x <%s>\n",s[i],s[i],(char *)&d[j]); */
      j+=7;
    } else d[j++]=s[i];
  }
  return(j);  
}
static void data_section() {
  int i=0,c;
  int count=0;
  if(verbose) {
    printf("Make data section ...");
    fflush(stdout);
  }
  fprintf(optr,"/* Data section compiled by xb2c. */\n\n");
  if(bytecode->sdataseglen) {
    fprintf(optr,"static const char datasec[%d]=\n",(int)bytecode->sdataseglen);
    fprintf(optr,"\"");
    while(i<bytecode->sdataseglen) {
      if(havesymbol(i,STT_DATAPTR)>=0) {
        fprintf(optr,"\"\n    \"");
	count=0;
      }
      if(datasec[i]=='\"') {
        fprintf(optr,"\\\"");
	count+=2;
       
      } else if(isprint(datasec[i])) {
        fprintf(optr,"%c",datasec[i]);
	count++;
      } else {
        fprintf(optr,"\\x%02x",datasec[i]);
        count+=4;
      }
      if(count>64) {
        fprintf(optr,"\"\n    \"");
	count=0;
      }
      i++;
    }
    fprintf(optr,"\";\n\n");
    c=bytecode->symbolseglen/sizeof(BYTECODE_SYMBOL);
    for(i=0;i<c;i++) {
      if(symtab[i].typ==STT_DATAPTR) {
        if(symtab[i].name)
           fprintf(optr,"#define DTA_%s 0x%x\n",&strings[symtab[i].name],(unsigned int)symtab[i].adr);
        else  fprintf(optr,"#define DTA_%x 0x%x\n",(unsigned int)symtab[i].adr,(unsigned int)symtab[i].adr);
      }
    }

  }
  if(verbose) printf(" (done.)\n");
}

static void translate() {
  unsigned char *buf;
  
  signed char c;
  int i,n,b,redo;
  short ss,ss2;
  double d;
  char *bcpc=(char *)bytecode+sizeof(BYTECODE_HEADER);
  unsigned char cmd;
  int pcount=0;

  int a=bytecode->symbolseglen/sizeof(BYTECODE_SYMBOL);
  int vidx[a];
  if(verbose) {
    printf("Translate ... ");
    fflush(stdout);
  }
  
  if(a>0) { 
    if(verbose) {printf("protos ... ");fflush(stdout);}
    fprintf(optr,"\n/* Function prototypes */\n");
    for(i=0;i<a;i++) {
      if(symtab[i].typ==STT_FUNC) {
        if(symtab[i].name)
           fprintf(optr,"void proc_%s(); \t/* $%x */\n",&strings[symtab[i].name],(unsigned int)symtab[i].adr);
        else  fprintf(optr,"void FUNC_%x(); \t/* 0x%x */\n",(unsigned int)symtab[i].adr,(unsigned int)symtab[i].adr);
      }
    }
  }
  
  /*Jetzt Variablen:*/
  if(a>0) { 
    if(verbose) {printf("variables ... ");fflush(stdout);}
    int count=0;
    fprintf(optr,"\n/* Variables */\n");
    for(i=0;i<a;i++) {
      if(symtab[i].typ==STT_OBJECT) {
        int typ,subtyp;
	typ=symtab[i].subtyp;
	if(typ&ARRAYTYP) {
  	  subtyp=typ&(~ARRAYTYP);
	  typ=ARRAYTYP;
        } else subtyp=0;
	if(typ==INTTYP)         fprintf(optr,"int    VARi_");
	else if(typ==FLOATTYP)  fprintf(optr,"double VARf_");
	else if(typ==ARBINTTYP) fprintf(optr,"ARBINT VARai_");
	else if(typ==COMPLEXTYP)fprintf(optr,"COMPLEX VARc_");
	else if(typ==STRINGTYP) fprintf(optr,"STRING VARs_");
	else if(typ==ARRAYTYP)  fprintf(optr,"ARRAY  VARa_");
        vidx[count]=i;
        if(symtab[i].name)
              fprintf(optr,"%s; \t/* typ=0x%x, subtyp=0x%x; vnr=%d sym=%d $%04x: */\n",&strings[symtab[i].name],typ,subtyp,count,i,(unsigned int)symtab[i].adr);
        else  fprintf(optr,"_%d; \t/* typ=0x%x, subtyp=0x%x;        sym=%d $%04x */\n",count,typ,subtyp,vidx[count],(unsigned int)symtab[i].adr);
        count++;
      }
      fflush(optr);
    }
  }
  if(verbose) {printf("init ... ");fflush(stdout);}
  
  if(nomain) {
    char nn[256];
    int i=0;
    int j=0;
    while(ifilename[i]) {
      nn[j++]=ifilename[i];
      if(ifilename[i]=='/' || ifilename[i]=='\\') j=0;
      if(ifilename[i]=='.') {j--;break;}
      i++;
    }
    nn[j]=0;
    
    fprintf(optr,"\nvoid lib_%s_init() {\n",nn);
  } else {
    fprintf(optr,"\nmain(int anzahl, char *argumente[]) {\n"
               "  MAIN_INIT;\n"
               "  databufferlen=%d;\n",(int)bytecode->dataseglen);
    if(bytecode->dataseglen) fprintf(optr,"  databuffer=datasec;\n");
  }
  
  if(a>0) { 
    int count=0;
    for(i=0;i<a;i++) {
      if(symtab[i].typ==STT_OBJECT) {
        int typ,subtyp;
	char vnam[256];
	typ=symtab[i].subtyp;
	if(typ&ARRAYTYP) {
  	  subtyp=typ&(~ARRAYTYP);
	  typ=ARRAYTYP;
        } else subtyp=0;
        vidx[count]=i;
        
	if(typ==INTTYP)         sprintf(vnam,"VARi_");
	else if(typ==FLOATTYP)  sprintf(vnam,"VARf_");
	else if(typ==COMPLEXTYP)sprintf(vnam,"VARc_");
	else if(typ==ARBINTTYP) sprintf(vnam,"VARai_");
	else if(typ==STRINGTYP) sprintf(vnam,"VARs_");
	else if(typ==ARRAYTYP)  sprintf(vnam,"VARa_");
	
#if 0	
/*Irgendwas ist hier noch faul....*/
          if(symtab[i].name)
                fprintf(optr,"  add_variable(\"%s\",0x%x,0,V_STATIC,(char *)&%s%s); \t/*%d $%02x: */\n",&strings[symtab[i].name],typ,vnam,&strings[symtab[i].name],count,(unsigned int)symtab[i].adr);
          else  fprintf(optr,"  add_variable(\"VAR_%d\",0x%x,0,V_STATIC,(char *)&%s_%d); \t/*%d $%04x */\n",count,typ,vnam,count,vidx[count],(unsigned int)symtab[i].adr);
 if(typ==STRINGTYP) {
          if(symtab[i].name) fprintf(optr,"  %s%s=create_string(NULL);\n",vnam,&strings[symtab[i].name]);
	  else               fprintf(optr,"  %s_%d=create_string(NULL);\n",vnam,count);
 } else if(typ==ARRAYTYP) {
          if(symtab[i].name) fprintf(optr,"  %s%s=create_array(0x%x,0,NULL);\n",vnam,&strings[symtab[i].name],subtyp);
	  else               fprintf(optr,"  %s_%d=create_array(0x%x,0,NULL);\n",vnam,count,subtyp);
 
 }
 #else
 	if(typ==INTTYP || typ==FLOATTYP|| typ==COMPLEXTYP) {
          if(symtab[i].name)
                fprintf(optr,"  add_variable(\"%s\",0x%x,0,V_STATIC,(char *)&%s%s); \t/*%d $%02x: */\n",&strings[symtab[i].name],typ,vnam,&strings[symtab[i].name],count,(unsigned int)symtab[i].adr);
          else  fprintf(optr,"  add_variable(\"VAR_%d\",0x%x,0,V_STATIC,(char *)&%s_%d); \t/*%d $%04x */\n",count,typ,vnam,count,vidx[count],(unsigned int)symtab[i].adr);
	} else {
          if(symtab[i].name)
                fprintf(optr,"  add_variable(\"%s\",0x%x,0x%x,V_DYNAMIC,NULL); \t/*%d $%02x: */\n",&strings[symtab[i].name],typ,subtyp,count,(unsigned int)symtab[i].adr);
          else  fprintf(optr,"  add_variable(\"VAR_%d\",0x%x,0x%x,V_DYNAMIC,NULL); \t/*%d $%04x */\n",count,typ,subtyp,vidx[count],(unsigned int)symtab[i].adr);
	} 
 #endif
        count++;
      }
      fflush(optr);
    }
  }
  if(verbose) {printf("TEXT ... ");fflush(stdout);}
  i=0;
  while((cmd=bcpc[i]) && i<bytecode->textseglen) {
    fflush(optr);
    
    redo=0;
    fprintf(optr,"/* %02x / %02x */",i,i+(unsigned int)sizeof(BYTECODE_HEADER));
    if((b=havesymbol(i,STT_LABEL))>=0) 
      fprintf(optr,"%s:  ",&strings[symtab[b].name]);
    else if((b=havesymbol(i,0))>=0) {
      if(symtab[b].name)
        fprintf(optr,"%s:  ",&strings[symtab[b].name]);
      else fprintf(optr,"LBL_%x:  ",(unsigned int)symtab[b].adr);
    } else fprintf(optr,"    ");
       
    
    if((b=havesymbol(i,STT_FUNC))>=0) {
      redo=1;
      if(pcount==0)
        fprintf(optr,";\n}\nvoid proc_%s() {\n",&strings[symtab[b].name]);
      else
        fprintf(optr,"exit(-1);\n}\nvoid proc_%s() {\n",&strings[symtab[b].name]);
      pcount++;
    } 
    if(redo && (b=havesymbol(i,STT_LABEL))>=0) {
      fprintf(optr,"/* %02x */",i);
      fprintf(optr,"%s:  ",&strings[symtab[b].name]);
    } else if(redo && (b=havesymbol(i,0))>=0) {
      fprintf(optr,"/* %02x */",i);
      if(symtab[b].name)
        fprintf(optr,"%s:  ",&strings[symtab[b].name]);
      else fprintf(optr,"LBL_%x:  ",(unsigned int)symtab[b].adr);
    } else if(redo) fprintf(optr,"    ");
    i++;
     switch(cmd) {
    case BC_NOOP:
      fprintf(optr,"NOOP;\n");
      break;
    case BC_JSR:
      CP4(&a,&bcpc[i],i);
      a-=sizeof(BYTECODE_HEADER);
      if((b=havesymbol(a,STT_FUNC))>=0) {
        fprintf(optr,"proc_%s();",&strings[symtab[b].name]);
        fprintf(optr,"\t/* JSR(0x%x);*/\n",a);
      } else fprintf(optr,"JSR(0x%x);\n",a);
      break;
    case BC_JMP:
      CP4(&a,&bcpc[i],i);
      a-=sizeof(BYTECODE_HEADER);
      if((b=havesymbol(a,STT_LABEL))>=0) 
        fprintf(optr,"goto %s;\t/* JMP(0x%x); */\n",&strings[symtab[b].name],a);
      else if((b=havesymbol(a,0))>=0) 
        fprintf(optr,"goto LBL_%x;\t/* JMP(0x%x); */\n",a,a);
      else fprintf(optr,"JMP(0x%x);\n",a);
      break;
    case BC_JEQ:
      CP4(&a,&bcpc[i],i);
      a-=sizeof(BYTECODE_HEADER);
      if((b=havesymbol(a,STT_LABEL))>=0) 
        fprintf(optr,"JUMPIFZERO %s\t/* JEQ(0x%x); */\n",&strings[symtab[b].name],a);
      else if((b=havesymbol(a,0))>=0) 
        fprintf(optr,"JUMPIFZERO LBL_%x;\t/* JEQ(0x%x); */\n",a,a);
      else fprintf(optr,"JEQ(0x%x);\n",a);
      break;
    case BC_BRA:
      CP2(&ss,&bcpc[i],i);
     if((b=havesymbol(i+ss,STT_LABEL))>=0) 
        fprintf(optr,"goto %s;\t/*BRA(%d); $%x */\n",&strings[symtab[b].name],ss,i+ss);
      else if((b=havesymbol(i+ss,0))>=0) 
        fprintf(optr,"goto LBL_%x;\t/* BRA(%d); */\n",i+ss,ss);
      else fprintf(optr,"BRA(%d);\t/* $%x */\n",ss,i+ss); 
      break;
    case BC_BEQ:
      CP2(&ss,&bcpc[i],i);
      if((b=havesymbol(i+ss,STT_LABEL))>=0) 
        fprintf(optr,"JUMPIFZERO %s;\t/*BEQ(%d); $%x */\n",&strings[symtab[b].name],ss,i+ss);
      else if((b=havesymbol(i+ss,0))>=0) 
        fprintf(optr,"JUMPIFZERO LBL_%x;\t/* BEQ(%d); */\n",i+ss,ss);
      else fprintf(optr,"BEQ(%d);\t/* $%x */\n",ss,i+ss); 
      break;
    case BC_BSR:
      CP2(&ss,&bcpc[i],i);
      if((b=havesymbol(i+ss,STT_FUNC))>=0) {
        fprintf(optr,"proc_%s();",&strings[symtab[b].name]);
        fprintf(optr,"\t/* BSR(%d); $%x */\n",ss,i+ss);
      } else fprintf(optr,"BSR(%d);\n",ss);
      break;
    case BC_BLKEND:
      fprintf(optr,"restore_locals(sp--);\n");
      break;
    case BC_BLKSTART:
      fprintf(optr,"sp++;\n");
      break;
    case BC_RTS:
      fprintf(optr,"return;\n");
      break;
    case BC_BRAs:
      c=bcpc[i++];
      if((b=havesymbol(i+c,STT_LABEL))>=0) 
        fprintf(optr,"goto %s;\t/*BRA_s(%d); $%x */\n",&strings[symtab[b].name],c,i+c);
      else if((b=havesymbol(i+c,0))>=0) 
        fprintf(optr,"goto LBL_%x;\t/* BRA_s(%d); */\n",i+c,c);
      else fprintf(optr,"BRA_s(%d);\t/* $%x */\n",c,i+c); 
      break;
    case BC_BEQs:
      c=bcpc[i++];
      if((b=havesymbol(i+c,STT_LABEL))>=0) 
        fprintf(optr,"JUMPIFZERO %s;\t/*BEQ_s(%d); $%x */\n",&strings[symtab[b].name],c,i+c);
      else if((b=havesymbol(i+c,0))>=0) 
        fprintf(optr,"JUMPIFZERO LBL_%x;\t/* BEQ_s(%d); */\n",i+c,c);
      else fprintf(optr,"BEQ_s(%d);\t/* $%x */\n",c,i+c); 
      break;
    case BC_LOADi:
      CP4(&a,&bcpc[i],i);
      fprintf(optr,"LOADi(0x%x);\n",a);
      break;
    case BC_LOADf:
      CP4(&a,&bcpc[i],i);
      fprintf(optr,"LOADf(0x%x);\n",a);
      break;
    case BC_LOADs:
      CP4(&a,&bcpc[i],i);
      fprintf(optr,"LOADs(0x%x);\n",a);
      break;
    case BC_LOADa:
      CP4(&a,&bcpc[i],i);
      fprintf(optr,"LOADa(0x%x);\n",a);
      break;
    case BC_SAVEi:
      CP4(&a,&bcpc[i],i);
      fprintf(optr,"SAVEi(0x%x);\n",a);
      break;
    case BC_SAVEf:
      CP4(&a,&bcpc[i],i);
      fprintf(optr,"SAVEf(0x%x);\n",a);
      break;
    case BC_SAVEs:
      CP4(&a,&bcpc[i],i);
      fprintf(optr,"SAVEs(0x%x);\n",a);
      break;
    case BC_SAVEa:
      CP4(&a,&bcpc[i],i);
      fprintf(optr,"SAVEa(0x%x);\n",a);
      break;
    case BC_PUSHF:
      CP8(&d,&bcpc[i],i);
      fprintf(optr,"PUSHF(%.13g);\n",d);
      break;
    case BC_PUSHC:
      CP8(&d,&bcpc[i],i);
      fprintf(optr,"PUSHC(%.13g,",d);
      CP8(&d,&bcpc[i],i);
      fprintf(optr,"%.13g);\n",d);
      break;
    case BC_PUSHI:
      CP4(&a,&bcpc[i],i);
      fprintf(optr,"PUSHI(%d);\n",a);
      break;
    case BC_PUSHW:
      CP2(&ss,&bcpc[i],i);
      fprintf(optr,"PUSHW(%d);\n",ss);
      break;
    case BC_PUSHB:
      fprintf(optr,"PUSHB(%d);\n",bcpc[i++]);
      break;
    case BC_PUSHLEER: fprintf(optr,"PUSHLEER;\n"); break;
    case BC_PUSH0:    fprintf(optr,"PUSH0;\n");    break;
    case BC_PUSH1:    fprintf(optr,"PUSH1;\n");    break;
    case BC_PUSH2:    fprintf(optr,"PUSH2;\n");    break;
    case BC_PUSHM1:   fprintf(optr,"PUSHM1;\n");   break;
    case BC_PUSHFUNC:
      a=bcpc[i++]&0xff;
      n=bcpc[i++]&0xff;
      if(a==find_func("ABS") || a==find_func("ACOS") || a==find_func("ACOSH") || a==find_func("ASIN") || a==find_func("ASINH") ||
         a==find_func("ATAN") || a==find_func("ATN") || a==find_func("ATANH")) 
        fprintf(optr,"PFUNC_%s; /* %s() */\n",pfuncs[a].name,pfuncs[a].name);
      else if(a==find_func("CBRT") || a==find_func("CEIL") || a==find_func("COS") || a==find_func("COSH") || a==find_func("EXP") ||
         a==find_func("EXPM1") || a==find_func("FLOOR") || a==find_func("HYPOT")) 
        fprintf(optr,"PFUNC_%s; /* %s() */\n",pfuncs[a].name,pfuncs[a].name);
       else if(a==find_func("LN") || a==find_func("LOG") || a==find_func("LOG10") || a==find_func("LOG1P") || a==find_func("LOGB") ||
         a==find_func("RAND") || a==find_func("SIN") || a==find_func("SINH")) 
        fprintf(optr,"PFUNC_%s; /* %s() */\n",pfuncs[a].name,pfuncs[a].name);
       else if(a==find_func("SQR") || a==find_func("SQRT") || a==find_func("TAN") || a==find_func("TANH")) 
        fprintf(optr,"PFUNC_%s; /* %s() */\n",pfuncs[a].name,pfuncs[a].name);
      else fprintf(optr,"PUSHFUNC(%d,%d); /* %s */\n",a,n,pfuncs[a].name);
      break;
    case BC_PUSHSFUNC:
      a=bcpc[i++]&0xff;
      n=bcpc[i++]&0xff;
      fprintf(optr,"PUSHSFUNC(%d,%d); /* %s */\n",a,n,psfuncs[a].name);
      break;
    case BC_PUSHCOMM:
      a=bcpc[i++]&0xff;
      n=bcpc[i++]&0xff;
      if(a==find_comm("VSYNC") || a==find_comm("SHOWPAGE") || a==find_comm("BEEP") || a==find_comm("BELL") || a==find_comm("END") || 
                  a==find_comm("INC") || a==find_comm("DEC") ) {
         fprintf(optr,"COMM_%s; /* %d %d %s */\n",comms[a].name,a,n,comms[a].name);
      }
      else
      fprintf(optr,"PUSHCOMM(%d,%d); /* %s */\n",a,n,comms[a].name);
      break;
    case BC_PUSHSYS:
      a=bcpc[i++];
      fprintf(optr,"PUSHSYS(%d); /* %s */\n",a,sysvars[a].name);
      break;
    case BC_PUSHSSYS:
      a=bcpc[i++];
      fprintf(optr,"PUSHSSYS(%d); /* %s */\n",a,syssvars[a].name);
      break;
    case BC_PUSHASYS:
      a=bcpc[i++];
      fprintf(optr,"PUSHASYS(%d); /* %s */\n",a,"??");
      break;
    case BC_PUSHX:
      n=bcpc[i++];
      CP4(&a,&bcpc[i],i);
      buf=malloc(n*8+8);
      b=frishmemcpy(buf,(unsigned char *)(rodata+a),n);
      buf[b]=0;
      fprintf(optr,"PUSHX(\"%s\"); /*len=%d*/\n",buf,n);
      free(buf);
      break;
    case BC_PUSHK:
      n=bcpc[i++];
      fprintf(optr,"PUSHK(%d);     /* %s */ \n",n,keywords[n]);
      break;
    case BC_PUSHS:
      { int len;
      CP4(&len,&bcpc[i],i);
      CP4(&a,&bcpc[i],i);
      buf=malloc(8*len+8);
      b=frishmemcpy(buf,(unsigned char *)(rodata+a),len);
      buf[b]=0;
      fprintf(optr,"PUSHS(\"%s\"); /*len=%d*/\n",buf,len);
      free(buf);
      }
      break;
    case BC_PUSHAI:
      { int len;
      CP4(&len,&bcpc[i],i);
      CP4(&a,&bcpc[i],i);
      buf=malloc(8*len+8);
      b=frishmemcpy(buf,(unsigned char *)(rodata+a),len);
      buf[b]=0;
      ARBINT a;
      mpz_init(a);
      mpz_set_str(a,(char *)buf,32);
      char *buf2=mpz_get_str(NULL,10,a);
      mpz_clear(a);
      fprintf(optr,"PUSHAI(\"%s\"); /*len=%d  %s*/\n",buf,len,buf2);
      free(buf);
      free(buf2);
      }
      break;
    case BC_PUSHA:
      { int len;
      CP4(&len,&bcpc[i],i);
      CP4(&a,&bcpc[i],i);
      buf=malloc(8*len+8);
      b=frishmemcpy(buf,(unsigned char *)(rodata+a),len);
      buf[b]=0;
      fprintf(optr,"PUSHA(\"%s\",%d); /*len=%d*/\n",buf,len,len);
      free(buf);
      }
      break;
    case BC_COMMENT:
      a=bcpc[i++];
      fprintf(optr,"/* %s (%d)*/\n",&bcpc[i],a);
      i+=a;
      break;
    case BC_ADD:  fprintf(optr,"ADD;\n");  break;
    case BC_ADDi: fprintf(optr,"ADDi;\n"); break;
    case BC_ADDf: fprintf(optr,"ADDf;\n"); break;
    case BC_ADDc: fprintf(optr,"ADDc;\n"); break;
    case BC_ADDs: fprintf(optr,"ADDs;\n"); break;
    case BC_AND:  fprintf(optr,"AND;\n");  break;
    case BC_ANDi: fprintf(optr,"ANDi;\n");  break;
    case BC_OR:   fprintf(optr,"OR;\n");   break;
    case BC_ORi:  fprintf(optr,"ORi;\n");   break;
    case BC_XOR:  fprintf(optr,"XOR;\n");  break;
    case BC_XORi: fprintf(optr,"XORi;\n");  break;
    case BC_SUB:  fprintf(optr,"SUB;\n");  break;
    case BC_SUBi: fprintf(optr,"SUBi;\n"); break;
    case BC_SUBf: fprintf(optr,"SUBf;\n"); break;
    case BC_SUBc: fprintf(optr,"SUBc;\n"); break;
    case BC_MUL:  fprintf(optr,"MUL;\n");  break;
    case BC_MULi: fprintf(optr,"MULi;\n"); break;
    case BC_MULf: fprintf(optr,"MULf;\n"); break;
    case BC_MULc: fprintf(optr,"MULc;\n"); break;
    case BC_DIV:  fprintf(optr,"DIV;\n");  break;
    case BC_DIVf:  fprintf(optr,"DIVf;\n");  break;
    case BC_DIVc:  fprintf(optr,"DIVc;\n");  break;
    case BC_POW:  fprintf(optr,"POW;\n");  break;
    case BC_EQUAL:fprintf(optr,"EQUAL;\n");break;
    case BC_GREATER:fprintf(optr,"GREATER;\n");break;
    case BC_LESS: fprintf(optr,"LESS;\n"); break;
    case BC_DUP:  fprintf(optr,"DUP;\n");  break;
    case BC_EXCH: fprintf(optr,"EXCH;\n"); break;
    case BC_CLEAR:fprintf(optr,"CLEAR;\n");break;
    case BC_COUNT:fprintf(optr,"COUNT;\n");break;
    case BC_NEG:  fprintf(optr,"NEG;\n");  break;
    case BC_NOT:  fprintf(optr,"NOT;\n");  break;
    case BC_NOTi:  fprintf(optr,"NOTi;\n");  break;
    case BC_X2I:  fprintf(optr,"X2I;\n");  break;
    case BC_X2AI: fprintf(optr,"X2AI;\n");  break;
    case BC_X2F:  fprintf(optr,"X2F;\n");  break;
    case BC_I2F:  fprintf(optr,"I2F;\n");  break;
    case BC_I2FILE:fprintf(optr,"I2FILE;\n");  break;
    case BC_F2C:  fprintf(optr,"F2C;\n");  break;
    case BC_X2C:  fprintf(optr,"X2C;\n");  break;
    case BC_MOD:  fprintf(optr,"MOD;\n");  break;
    case BC_POP:  fprintf(optr,"POP;\n");  break;
    case BC_EVAL: fprintf(optr,"EVAL;\n"); break;
    case BC_LOCAL:
      CP2(&ss,&bcpc[i],i);
      if(ss==-1) {
        printf("Error, Variable gibts nicht.\n");
	fprintf(optr,"LOCAL(%d); /* ERROR */\n",ss);
      } else fprintf(optr,"LOCAL(%d); /* %s */\n",ss,&strings[symtab[vidx[ss]].name]);
      break;
    case BC_ZUWEISi:
      CP2(&ss,&bcpc[i],i);
      if(ss==-1) {
        printf("Error, Variable gibts nicht.\n");
	fprintf(optr,"ZUWEIS(%d); /* ERROR */\n",ss);
      } else fprintf(optr,"ZUWEISi(%d); /* %s */\n",ss,&strings[symtab[vidx[ss]].name]);
      break;
    case BC_ZUWEISf:
      CP2(&ss,&bcpc[i],i);
      if(ss==-1) {
        printf("Error, Variable gibts nicht.\n");
	fprintf(optr,"ZUWEIS(%d); /* ERROR */\n",ss);
      } else fprintf(optr,"ZUWEISf(%d); /* %s */\n",ss,&strings[symtab[vidx[ss]].name]);
      break;
    case BC_ZUWEISc:
      CP2(&ss,&bcpc[i],i);
      if(ss==-1) {
        printf("Error, Variable gibts nicht.\n");
	fprintf(optr,"ZUWEIS(%d); /* ERROR */\n",ss);
      } else fprintf(optr,"ZUWEISc(%d); /* %s */\n",ss,&strings[symtab[vidx[ss]].name]);
      break;
    case BC_ZUWEIS:
      CP2(&ss,&bcpc[i],i);
      if(ss==-1) {
        printf("Error, Variable gibts nicht.\n");
	fprintf(optr,"ZUWEIS(%d); /* ERROR */\n",ss);
      } else fprintf(optr,"ZUWEIS(%d); /* %s */\n",ss,&strings[symtab[vidx[ss]].name]);
      break;
    case BC_ZUWEISINDEX:
      CP2(&ss,&bcpc[i],i);
      CP2(&ss2,&bcpc[i],i);
      fprintf(optr,"ZUWEISINDEX(%d,%d); /* %s */\n",ss,ss2,&strings[symtab[vidx[ss]].name]);
      break;
    case BC_PUSHV:
      CP2(&ss,&bcpc[i],i);
      if(ss==-1) {
        printf("Error, Variable gibts nicht.\n");
	fprintf(optr,"PUSHV(%d); /* ERROR */\n",ss);
      } else fprintf(optr,"PUSHV(%d); /* %s */\n",ss,&strings[symtab[vidx[ss]].name]);
      break;
    case BC_PUSHVV:
      CP2(&ss,&bcpc[i],i);
      fprintf(optr,"PUSHVV(%d); /* %s */\n",ss,&strings[symtab[vidx[ss]].name]);
      break;
    case BC_PUSHVVI:
      CP2(&ss,&bcpc[i],i);
      CP2(&ss2,&bcpc[i],i);
      fprintf(optr,"PUSHVVI(%d,%d); /* %s */\n",ss,ss2,&strings[symtab[vidx[ss]].name]);
      break;
    case BC_PUSHARRAYELEM:      
      CP2(&ss,&bcpc[i],i);
      CP2(&ss2,&bcpc[i],i);
      fprintf(optr,"PUSHARRAYELEM(%d,%d); /* %s */\n",ss,ss2,&strings[symtab[vidx[ss]].name]);
      break;
    case BC_RESTORE:
      CP4(&a,&bcpc[i],i);
      fprintf(optr,"RESTORE(0x%x);",a);
      if((b=havesymbol(a,STT_DATAPTR))>=0) fprintf(optr,"\t/* %s */\n",&strings[symtab[b].name]);
      else fprintf(optr,"\n");
      break;
    case BC_PUSHLABEL:
      CP4(&a,&bcpc[i],i);
      a-=sizeof(BYTECODE_HEADER);
      if((b=havesymbol(a,STT_LABEL))>=0) 
        fprintf(optr,"PUSHLABEL(&&%s);\t/* LABEL(0x%x); */\n",&strings[symtab[b].name],a);
      else if((b=havesymbol(a,0))>=0) 
        fprintf(optr,"PUSHLABEL(&&LBL_%x);\t/* LABEL(0x%x); */\n",a,a);
      else fprintf(optr,"PUSHLABEL(0x%x);\n",a);
      break;
    case BC_PUSHPROC: 
      CP4(&a,&bcpc[i],i);
      a-=sizeof(BYTECODE_HEADER);
      if((b=havesymbol(a,STT_FUNC))>=0) 
        fprintf(optr,"PUSHPROC(proc_%s);\t/* PROC(0x%x); */\n",&strings[symtab[b].name],a);
      else if((b=havesymbol(a,0))>=0) 
        fprintf(optr,"pushproc LBL_%x;\t/* PROC(0x%x); */\n",a,a);
      else fprintf(optr,"PUSHPROC(0x%x);\n",a);
      break;
    default:
      printf("xb2c: BC_ILLEGAL instruction %2x at %d\n",(int)cmd,i);
      memdump((unsigned char *)&(bcpc[i]),16);
    }
  }
  fprintf(optr,"}\n");
  if(verbose)  printf(" (done.)\n");
}

static int loadbcprg(char *filename) {  
  int len,i,c;
  char *p;
  FILE *dptr;
  dptr=fopen(filename,"rb"); len=lof(dptr); fclose(dptr);
  p=malloc(len);
  bload(filename,p,len);
  if(p[0]==BC_BRAs && p[1]==sizeof(BYTECODE_HEADER)-2) {
    bytecode=(BYTECODE_HEADER *)p;
    fprintf(optr,"/* X11-Basic-Compiler Version 1.24\n"
                 "   (c) Markus Hoffmann 2002-2016\n"
                 "\n"
                 "\nBytecode: %s (%d Bytes)\n\n",filename,len);
		 
#ifdef ATARI
WSWAP((char *)&bytecode->version);
#endif
    if(verbose) printf("Bytecode header found (V.%x)\n",bytecode->version);
    if(bytecode->version!=BC_VERSION) {
      printf("xb2c: ERROR: Cannot translate this bytecode.\nThis bytecode was compiled for a different version of "
      "X11-Basic.\nPlease consider to recompile it.\n");
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
        /* Sicherstellen, dass der Speicherbereich auch gross genug ist fuer bss segment*/
    if(bytecode->bssseglen>bytecode->relseglen+bytecode->stringseglen+bytecode->symbolseglen) {
      len+=bytecode->bssseglen-bytecode->stringseglen-bytecode->symbolseglen;
      if(verbose>1) printf("xb2c INFO: need realloc\n");
      p=realloc(p,len);
      bytecode=(BYTECODE_HEADER *)p;
    }
    
    fprintf(optr,"txt:    $%08x %d\n",(unsigned int)sizeof(BYTECODE_HEADER),(int)bytecode->textseglen);
    fprintf(optr,"rodata: $%08x %d\n",(unsigned int)sizeof(BYTECODE_HEADER)+
                                      (unsigned int)bytecode->textseglen,(int)bytecode->rodataseglen);
    rodata=p+sizeof(BYTECODE_HEADER)+(unsigned int)bytecode->textseglen;
    fprintf(optr,"sdata:  $%08x %d\n",(unsigned int)sizeof(BYTECODE_HEADER)+
                                      (unsigned int)bytecode->textseglen+
				      (unsigned int)bytecode->rodataseglen,(int)bytecode->sdataseglen);
    fprintf(optr,"data:   $%08x %d\n",(unsigned int)sizeof(BYTECODE_HEADER)+
                                    (unsigned int)bytecode->textseglen+
				    (unsigned int)bytecode->rodataseglen+
				    (unsigned int)bytecode->sdataseglen,(int)bytecode->dataseglen);
    datasec=(unsigned char *)&p[sizeof(BYTECODE_HEADER)+(unsigned int)bytecode->textseglen+
    bytecode->rodataseglen];
    fprintf(optr,"bss:    $%08x %d\n",(unsigned int)sizeof(BYTECODE_HEADER)+
                                    (unsigned int)bytecode->textseglen+
				    (unsigned int)bytecode->rodataseglen+
				    (unsigned int)bytecode->sdataseglen+
				    (unsigned int)bytecode->dataseglen,(int)bytecode->bssseglen);
    fprintf(optr,"str:    $%08x %d\n",(unsigned int)sizeof(BYTECODE_HEADER)+
                                   (unsigned int)bytecode->textseglen+
				   (unsigned int)bytecode->rodataseglen+
				   (unsigned int) bytecode->sdataseglen+
				   (unsigned int)bytecode->dataseglen,(int)bytecode->stringseglen);
    fprintf(optr,"sym:    $%08x %d\n",(unsigned int)sizeof(BYTECODE_HEADER)+
                                   (unsigned int)bytecode->textseglen+
				    (unsigned int)bytecode->rodataseglen+
				    (unsigned int)bytecode->sdataseglen+
				   (unsigned int)bytecode->dataseglen+
				   (unsigned int)bytecode->stringseglen,(int)bytecode->symbolseglen);
    strings=p+sizeof(BYTECODE_HEADER)+bytecode->textseglen+bytecode->rodataseglen+bytecode->sdataseglen+
    bytecode->dataseglen;
    fprintf(optr,"Strings: %s\n\n",strings);
    symtab=(BYTECODE_SYMBOL *)(p+sizeof(BYTECODE_HEADER)+bytecode->textseglen+bytecode->rodataseglen+bytecode->sdataseglen+
                                 bytecode->dataseglen+bytecode->stringseglen);
    c=bytecode->symbolseglen/sizeof(BYTECODE_SYMBOL);

    fprintf(optr,"%d symbols:\n",c);
    for(i=0;i<c;i++) {
	#ifdef ATARI
	  LWSWAP((short *)&symtab[i].name);
	  LWSWAP((short *)&symtab[i].adr);
	#endif
      if(verbose>1) fprintf(optr,"%4d : $%08x %s\n",i,(unsigned int)symtab[i].adr,&strings[symtab[i].name]);
    }
    fprintf(optr,"\t\t\t*/\n"
                 "#include <x11basic/xb2csol.h>\n\n");
    fflush(optr);
    return(0);
  } else {
    printf("xb2c: ERROR, file format not recognized.\n");
    return(-1);
  }
}


int main(int anzahl, char *argumente[]) {
  /* Initialize data segment buffer */
  if(anzahl<2) {    /* Kommandomodus */
    intro();usage();
  } else {
    kommandozeile(anzahl, argumente);    /* Kommandozeile bearbeiten */
    if(loadfile) {
      if(exist(ifilename)) {
          /* file oeffnen */
	#ifdef ATARI
	intro();
	printf("--> %s\n",ofilename);
	#endif
	optr=fopen(ofilename,"w");
        if(loadbcprg(ifilename)==0) {
          data_section();
	  translate();
	}
	fclose(optr);
	#ifdef ATARI
	printf("done.\n");
	#endif
      } else {
        printf("ERROR: %s not found !\n",ifilename);
	#ifdef ATARI
	sleep(2);
	#endif
        exit(EX_NOINPUT);
      }
    } else exit(EX_NOINPUT);
  }
  #ifdef ATARI
  sleep(2);
  #endif
  return(EX_OK);
}
