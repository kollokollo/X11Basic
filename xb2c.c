/* xb2c.C   The X11-basic to C translator   (c) Markus Hoffmann 2010-2011
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
#ifdef WINDOWS
#define EX_CANTCREAT	73	/* can't create (user) output file */
#define EX_NOINPUT	66	/* cannot open input */
#define EX_OK 0
#else
#include <sysexits.h>
#endif

#include "globals.h"
#include "x11basic.h"
#include "ptypes.h"
#include "wort_sep.h"
#include "xbasic.h"
#include "file.h"
#include "bytecode.h"
//#include "xb2c.h"


char ifilename[128]="b.b";       /* Standard inputfile  */
char ofilename[128]="11.c";       /* Standard outputfile     */
int loadfile=FALSE;
int verbose=0;


BYTECODE_HEADER *bytecode;

BYTECODE_SYMBOL *symtab;
char *strings;
unsigned char *datasec;

FILE *optr;

/* X11-Basic needs these declar<ations:  */
int prglen=0;
const char version[]="1.17";        /* Programmversion*/
const char vdate[]="2011-08-16";
char *programbuffer=NULL;
char *program[MAXPRGLEN];
int programbufferlen=0;

extern void memdump(unsigned char *,int);

void intro(){
  puts("***************************************************************");
  printf("*           X11-Basic to C translator                      *\n");
  puts("*                    by Markus Hoffmann 1997-2011 (c)         *");
  printf("* library V. %s date:   %30s      *\n",libversion,libvdate);
  puts("***************************************************************");
  puts("");
}
void usage(){
  puts("\n Usage:\n ------ \n");
  printf(" %s [-o <outputfile> -h] [<filename>] --- compile program [%s]\n\n","bytecode",ifilename);
  printf("-o <outputfile>\t--- put result in file [%s]\n",ofilename);
  puts("-h --help\t--- Usage\n"
       "-v\t\t--- be more verbose");

}

void kommandozeile(int anzahl, char *argumente[]) {
  int count;

  /* Kommandozeile bearbeiten   */
  for(count=1;count<anzahl;count++) {
    if (strcmp(argumente[count],"-o")==FALSE) {
      strcpy(ofilename,argumente[++count]);
    } else if (strcmp(argumente[count],"-h")==FALSE) {
      intro();usage();
    } else if (strcmp(argumente[count],"--help")==FALSE) {
      intro();usage();
    } else if (strcmp(argumente[count],"-v")==FALSE) {
      verbose++;
    } else if (strcmp(argumente[count],"-q")==FALSE) {
      verbose--;
    } else {
      if(!loadfile) {
        loadfile=TRUE;
        strcpy(ifilename,argumente[count]);
      }
    }
   }
}

int havesymbol(int adr,int typ) {
  int i,c=bytecode->symbolseglen/sizeof(BYTECODE_SYMBOL);
  for(i=0;i<c;i++) {
    if(symtab[i].adr==adr && symtab[i].typ==typ) return(i);
  }
  return(-1);
}
int frishmemcpy(char *d,char *s,int n) {
  int i,j=0;
  for(i=0;i<n;i++) {
    if(s[i]=='\"' || s[i]=='\\') {
      d[j++]='\\';
      d[j++]=s[i];  
    } else if(!isprint(s[i])) {
      sprintf(&d[j],"\\x%02x",s[i]);
      j+=4;
    } else d[j++]=s[i];
  
  }
  return(j);  
}
void data_section() {
  int i=0,c;
  int count=0;
  fprintf(optr,"/* compiled by xb2c. */\n\n");
  if(bytecode->dataseglen) {
    fprintf(optr,"const char datasec[%d]=\n",(int)bytecode->dataseglen);
    fprintf(optr,"\"");
    while(i<bytecode->dataseglen) {
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
}

void compile() {
  char *buf;
  char c;
  int i,a,n,b;
  short ss;
  double d;
  char *bcpc=(char *)bytecode+sizeof(BYTECODE_HEADER);
  unsigned char cmd;
  a=bytecode->symbolseglen/sizeof(BYTECODE_SYMBOL);
  if(a>0) { 
    for(i=0;i<a;i++) {
      if(symtab[i].typ==STT_FUNC) {
        if(symtab[i].name)
           fprintf(optr,"void %s();\t/* $%x */\n",&strings[symtab[i].name],symtab[i].adr);
        else  fprintf(optr,"void FUNC_%x();\t /* 0x%x */\n",symtab[i].adr,symtab[i].adr);
      }
    }
  }
  fprintf(optr,"\nmain(int anzahl, char *argumente[]) {\n"
               "  MAIN_INIT;\n");
  fprintf(optr,"databufferlen=%d;\n",bytecode->dataseglen);
  if(bytecode->dataseglen) fprintf(optr,"databuffer=datasec;\n");
  
  i=0;
  while((cmd=bcpc[i]) && i<bytecode->textseglen) {
    if((b=havesymbol(i,STT_FUNC))>=0) {
      fprintf(optr,"}\nvoid %s() {\n",&strings[symtab[b].name]);
    } 
    fprintf(optr,"/* %02x */",i);

    if((b=havesymbol(i,STT_LABEL))>=0) 
      fprintf(optr,"%s:  ",&strings[symtab[b].name]);
    else if((b=havesymbol(i,0))>=0) {
      if(symtab[b].name)
        fprintf(optr,"%s:  ",&strings[symtab[b].name]);
      else fprintf(optr,"LBL_%x:  ",symtab[b].adr);
    } else fprintf(optr,"    ");
    i++;
     switch(cmd) {
    case BC_NOOP:
      fprintf(optr,"NOOP;\n");
      break;
    case BC_JSR:
      memcpy(&a,&bcpc[i],sizeof(int));i+=sizeof(int);
      a-=sizeof(BYTECODE_HEADER);
      if((b=havesymbol(a,STT_FUNC))>=0) {
        fprintf(optr,"%s();",&strings[symtab[b].name]);
        fprintf(optr,"\t/* JSR(0x%x);*/\n",a);
      } else fprintf(optr,"JSR(0x%x);\n",a);
      break;
    case BC_JMP:
      memcpy(&a,&bcpc[i],sizeof(int));i+=sizeof(int);
      a-=sizeof(BYTECODE_HEADER);
      if((b=havesymbol(a,STT_LABEL))>=0) 
        fprintf(optr,"goto %s;\t/* JMP(0x%x); */\n",&strings[symtab[b].name],a);
      else if((b=havesymbol(a,0))>=0) 
        fprintf(optr,"goto LBL_%x;\t/* JMP(0x%x); */\n",a,a);
      else fprintf(optr,"JMP(0x%x);\n",a);
      break;
    case BC_JEQ:
      memcpy(&a,&bcpc[i],sizeof(int));i+=sizeof(int);
      a-=sizeof(BYTECODE_HEADER);
      if((b=havesymbol(a,STT_LABEL))>=0) 
        fprintf(optr,"if((--opstack)->integer==0) goto %s\t/* JEQ(0x%x); */\n",&strings[symtab[b].name],a);
      else if((b=havesymbol(a,0))>=0) 
        fprintf(optr,"if((--opstack)->integer==0) goto LBL_%x;\t/* JEQ(0x%x); */\n",a,a);
      else fprintf(optr,"JEQ(0x%x);\n",a);
      break;
    case BC_BRA:
      memcpy(&ss,&bcpc[i],sizeof(short));i+=sizeof(short);
     if(b=havesymbol(i+ss,STT_LABEL)>=0) 
        fprintf(optr,"goto %s;\t/*BRA(%d); $%x */\n",&strings[symtab[b].name],ss,i+ss);
      else if((b=havesymbol(i+ss,0))>=0) 
        fprintf(optr,"goto LBL_%x;\t/* BRA(%d); */\n",i+ss,ss);
      else fprintf(optr,"BRA(%d);\t/* $%x */\n",ss,i+ss); 
      break;
    case BC_BEQ:
      memcpy(&ss,&bcpc[i],sizeof(short));i+=sizeof(short);
      if(b=havesymbol(i+ss,STT_LABEL)>=0) 
        fprintf(optr,"if((--opstack)->integer==0) goto %s;\t/*BEQ(%d); $%x */\n",&strings[symtab[b].name],ss,i+ss);
      else if((b=havesymbol(i+ss,0))>=0) 
        fprintf(optr,"if((--opstack)->integer==0) goto LBL_%x;\t/* BEQ(%d); */\n",i+ss,ss);
      else fprintf(optr,"BEQ(%d);\t/* $%x */\n",ss,i+ss); 
      break;
    case BC_BSR:
      memcpy(&ss,&bcpc[i],sizeof(short));i+=sizeof(short);
      if((b=havesymbol(i+ss,STT_FUNC))>=0) {
        fprintf(optr,"%s();",&strings[symtab[b].name]);
        fprintf(optr,"\t/* BSR(%d); $%x */\n",ss,i+ss);
      } else fprintf(optr,"BSR(%d);\n",ss);
      break;
    case BC_RTS:
      fprintf(optr,"return;\n");
      break;
    case BC_BRAs:
      c=bcpc[i++];
      if(b=havesymbol(i+c,STT_LABEL)>=0) 
        fprintf(optr,"goto %s;\t/*BRA_s(%d); $%x */\n",&strings[symtab[b].name],c,i+c);
      else if((b=havesymbol(i+c,0))>=0) 
        fprintf(optr,"goto LBL_%x;\t/* BRA_s(%d); */\n",i+c,c);
      else fprintf(optr,"BRA_s(%d);\t/* $%x */\n",c,i+c); 
      break;
    case BC_BEQs:
      c=bcpc[i++];
      if(b=havesymbol(i+c,STT_LABEL)>=0) 
        fprintf(optr,"if((--opstack)->integer==0) goto %s;\t/*BEQ_s(%d); $%x */\n",&strings[symtab[b].name],c,i+c);
      else if((b=havesymbol(i+c,0))>=0) 
        fprintf(optr,"if((--opstack)->integer==0) goto LBL_%x;\t/* BEQ_s(%d); */\n",i+c,c);
      else fprintf(optr,"BEQ_s(%d);\t/* $%x */\n",c,i+c); 
      break;
    case BC_PUSHF:
      memcpy(&d,&bcpc[i],sizeof(double));i+=sizeof(double);
      fprintf(optr,"PUSHF(%g);\n",d);
      break;
    case BC_PUSHI:
      memcpy(&a,&bcpc[i],sizeof(int));i+=sizeof(int);
      fprintf(optr,"PUSHI(%d);\n",a);
      break;
    case BC_PUSHW:
      memcpy(&ss,&bcpc[i],sizeof(short));i+=sizeof(short);
      fprintf(optr,"PUSHW(%d);\n",ss);
      break;
    case BC_PUSHB:
      fprintf(optr,"PUSHB(%d);\n",bcpc[i++]);
      break;
    case BC_PUSHLEER:
      fprintf(optr,"PUSHLEER;\n");
      break;
    case BC_PUSH0:
      fprintf(optr,"PUSH0;\n");
      break;
    case BC_PUSH1:
      fprintf(optr,"PUSH1;\n");
      break;
    case BC_PUSH2:
      fprintf(optr,"PUSH2;\n");
      break;
    case BC_PUSHM1:
      fprintf(optr,"PUSHM1;\n");
      break;
    case BC_PUSHS:
      memcpy(&a,&bcpc[i],sizeof(int));i+=sizeof(int);
      buf=malloc(2*a+1);
      b=frishmemcpy(buf,&bcpc[i],a);
      buf[b]=0;
      fprintf(optr,"PUSHS(\"%s\"); /*len=%d*/\n",buf,a);
      i+=a;
      free(buf);
      break;
    case BC_PUSHFUNC:
      a=bcpc[i++]&0xff;
      n=bcpc[i++]&0xff;
      fprintf(optr,"PUSHFUNC(%d,%d); /* %s */\n",a,n,pfuncs[a].name);
      break;
    case BC_PUSHSFUNC:
      a=bcpc[i++]&0xff;
      n=bcpc[i++]&0xff;
      fprintf(optr,"PUSHSFUNC(%d,%d); /* %s */\n",a,n,psfuncs[a].name);
      break;
    case BC_PUSHCOMM:
      a=bcpc[i++]&0xff;
      n=bcpc[i++]&0xff;
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
      a=bcpc[i++];
      buf=malloc(a*2+1);
      b=frishmemcpy(buf,&bcpc[i],a);
      buf[b]=0;
      fprintf(optr,"PUSHX(\"%s\"); /*len=%d*/\n",buf,a);
      i+=a;
      free(buf);
      break;
    case BC_COMMENT:
      a=bcpc[i++];
      fprintf(optr,"/* %s (%d)*/\n",&bcpc[i],a);
      i+=a;
      break;
    case BC_ADD:
      fprintf(optr,"ADD;\n");
      break;
    case BC_ADDi:
      fprintf(optr,"ADDi;\n");
      break;
    case BC_ADDf:
      fprintf(optr,"ADDf;\n");
      break;
    case BC_ADDs:
      fprintf(optr,"ADDs;\n");
      break;
    case BC_OR:
      fprintf(optr,"OR;\n");
      break;
    case BC_XOR:
      fprintf(optr,"XOR;\n");
      break;
    case BC_SUB:
      fprintf(optr,"SUB;\n");
      break;
    case BC_SUBi:
      fprintf(optr,"SUBi;\n");
      break;
    case BC_SUBf:
      fprintf(optr,"SUBf;\n");
      break;
    case BC_MUL:
      fprintf(optr,"MUL;\n");
      break;
    case BC_MULi:
      fprintf(optr,"MULi;\n");
      break;
    case BC_MULf:
      fprintf(optr,"MULf;\n");
      break;
    case BC_DIV:
      fprintf(optr,"DIV;\n");
      break;
    case BC_POW:
      fprintf(optr,"POW;\n");
      break;
    case BC_AND:
      fprintf(optr,"AND;\n");
      break;
    case BC_EQUAL:
      fprintf(optr,"EQUAL;\n");
      break;
    case BC_GREATER:
      fprintf(optr,"GREATER;\n");
      break;
    case BC_LESS:
      fprintf(optr,"LESS;\n");
      break;
    case BC_DUP:
      fprintf(optr,"DUP;\n");
      break;
    case BC_EXCH:
      fprintf(optr,"EXCH;\n");
      break;
    case BC_CLEAR:
      fprintf(optr,"CLEAR;\n");
      break;
    case BC_COUNT:
      fprintf(optr,"COUNT;\n");
      break;
    case BC_NEG:
      fprintf(optr,"NEG;\n");
      break;
    case BC_NOT:
      fprintf(optr,"NOT;\n");
      break;
    case BC_X2I:
      fprintf(optr,"X2I;\n");
      break;
    case BC_X2F:
      fprintf(optr,"X2F;\n");
      break;
    case BC_MOD:
      fprintf(optr,"MOD;\n");
      break;
    case BC_POP:
      fprintf(optr,"POP;\n");
      break;
    case BC_ZUWEIS:
      fprintf(optr,"ZUWEIS;\n");
      break;
    case BC_PUSHVV:
      fprintf(optr,"PUSHVV;\n");
      break;
    case BC_ZUWEISINDEX:
      a=bcpc[i++]&0xff;
      fprintf(optr,"ZUWEISINDEX(%d);\n",a);
      break;
    case BC_PUSHV:
      fprintf(optr,"PUSHV;\n");
      break;
    case BC_PUSHARRAYELEM:      
      a=bcpc[i++]&0xff;
      fprintf(optr,"PUSHARRAYELEM(%d);\n",a);
      break;
    case BC_RESTORE:
      memcpy(&a,&bcpc[i],sizeof(int));i+=sizeof(int);
      fprintf(optr,"RESTORE(0x%x);",a);
      if((b=havesymbol(a,STT_DATAPTR))>=0) fprintf(optr,"\t/* %s */\n",&strings[symtab[b].name]);
      else fprintf(optr,"\n");
      break;
    case BC_EVAL:
      fprintf(optr,"EVAL;\n");
      break;
    default:
      printf("VM: BC_ILLEGAL instruction %2x at %d\n",(int)cmd,i);
      memdump((unsigned char *)&(bcpc[i]),16);
    }
  }
  fprintf(optr,"}\n");
}

int loadbcprg(char *filename) {  
  int len,i,c;
  char *p;
  FILE *dptr;
  dptr=fopen(filename,"r"); len=lof(dptr); fclose(dptr);
  p=malloc(len);
  bload(filename,p,len);
  if(p[0]==BC_BRAs && p[1]==sizeof(BYTECODE_HEADER)-2) {
    bytecode=(BYTECODE_HEADER *)p;
    fprintf(optr,"/* X11-Basic-Code-VM.c (%s)\n",filename);
    fprintf(optr,"   X11-BAsic-Compiler Version 1.17\n"
                 "   (c) Markus Hoffmann 2002-2011\n"
                 "*/\n");
    if(verbose) printf("Bytecode header found (V.%x)\n",bytecode->version);
    if(bytecode->version!=BC_VERSION) {
      printf("WARNING: This Bytecode was compiled for a different version of"
      "X11-Basic.\n ");
    }
    fprintf(optr,"/*\nBytecode: %s (%d Bytes)\n",filename,len);
    fprintf(optr,"txt:  $%08x %d\n",sizeof(BYTECODE_HEADER),bytecode->textseglen);
    fprintf(optr,"data: $%08x %d\n",sizeof(BYTECODE_HEADER)+bytecode->textseglen,bytecode->dataseglen);
    datasec=(unsigned char *)&p[sizeof(BYTECODE_HEADER)+bytecode->textseglen];
    fprintf(optr,"bss:  $%08x %d\n",sizeof(BYTECODE_HEADER)+bytecode->textseglen+bytecode->dataseglen,bytecode->bssseglen);
    fprintf(optr,"str:  $%08x %d\n",sizeof(BYTECODE_HEADER)+bytecode->textseglen+bytecode->dataseglen,bytecode->stringseglen);
    fprintf(optr,"sym:  $%08x %d\n",sizeof(BYTECODE_HEADER)+bytecode->textseglen+bytecode->dataseglen+bytecode->stringseglen,bytecode->symbolseglen);
    strings=&p[sizeof(BYTECODE_HEADER)+bytecode->textseglen+bytecode->dataseglen];
    fprintf(optr,"Strings: %s\n",strings);
    symtab=(BYTECODE_SYMBOL *)(p+sizeof(BYTECODE_HEADER)+bytecode->textseglen+bytecode->dataseglen+bytecode->stringseglen);
    c=bytecode->symbolseglen/sizeof(BYTECODE_SYMBOL);
    fprintf(optr,"%d symbols.\n",c);
    for(i=0;i<c;i++) {
      if(verbose) fprintf(optr,"%d : $%08x %s\n",i,symtab[i].adr,&strings[symtab[i].name]);
    }
    fprintf(optr,"    */\n");
    fprintf(optr,"#include <x11basic/xb2csol.h>\n");
    return(0);
  } else {
    printf("VM: ERROR, file format not recognized.\n");
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
	optr=fopen(ofilename,"w");
        
        if(loadbcprg(ifilename)==0) {
          data_section();
	  compile();
	}
	fclose(optr);
      } else {
        printf("ERROR: %s not found !\n",ifilename);
        exit(EX_NOINPUT);
      }
    } else exit(EX_NOINPUT);
  }
  return(EX_OK);
}
