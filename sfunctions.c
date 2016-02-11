/* sfunctions.c   String functions (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "xbasic.h"
#include "sfunctions.h"
#include "parser.h"
#include "io.h"
#include "wort_sep.h"

#include "decode.h"

STRING vs_error() {return(create_string("<ERROR>"));}


STRING f_aries(STRING n) {  /* order-0 adaptive arithmetic encoding */
  STRING ergebnis;
  int i;
  int size=n.len;
  ergebnis.pointer=malloc(size+1);
  put_pointer=(unsigned char *)ergebnis.pointer;
  put_size=0;

  start_model();
  /* start_outputing_bits */
  buffer=0;
  bits_to_go=8;
  /* Start encoding  */
  low=0;
  high=Top_value;
  bits_to_follow=0;
    for (i=0;i<n.len;i++) {
        int ch; int symbol;
        ch=n.pointer[i]&0xff;
        symbol=char_to_index[ch];
        encode_symbol(symbol,cum_freq);
        update_model(symbol);
    }
    encode_symbol(EOF_symbol,cum_freq);
    /* done_encoding */
    bits_to_follow += 1;                        /* Output two bits that     */
    if (low<First_qtr) bit_plus_follow(0);      /* select the quarter that  */
    else bit_plus_follow(1);                    /* the current code range   */                                               /* contains.                */

  /* done_outputing_bits   */
  *put_pointer++=(buffer>>bits_to_go);
  put_size++;
  ergebnis.len=put_size;
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
}
STRING f_arids(STRING n) {  /* order-0 adaptive arithmetic decoding */
  STRING ergebnis;
  int j=0,i;
  int size=n.len;
  put_pointer=(unsigned char *)n.pointer;
  put_size=n.len;
  ergebnis.pointer=malloc(size+1);

  start_model();                              /* Set up other modules.    */

  /* start_inputing_bits */
  bits_to_go=0;
  garbage_bits=0;
  /* start_decoding   */
  value = 0;
  for(i=1;i<=Code_value_bits;i++) value=2*value+input_bit();
  low=0;
  high=Top_value;


    for (;;) {
        int ch; int symbol;
        symbol = decode_symbol(cum_freq);
        if (symbol==EOF_symbol) break;
        ch = index_to_char[symbol];

        ergebnis.pointer[j++]=ch;
        if(j>=size) {
	  size=2*(size+1);
	  ergebnis.pointer=realloc(ergebnis.pointer,size+1);
	}
        update_model(symbol);
    }
  ergebnis.len=j;
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
}

STRING f_compresss(STRING n) {
  STRING ergebnis,a,b;
  a=f_rles(n);
  b=f_bwtes(a);
  free(a.pointer);
  a=f_mtfes(b);
  free(b.pointer);
  b=f_rles(a);
  free(a.pointer);
  ergebnis=f_aries(b);
  free(b.pointer);
  return(ergebnis);
}
STRING f_uncompresss(STRING n) {
  STRING ergebnis,a,b;
  b=f_arids(n);
  a=f_rlds(b);
  free(b.pointer);
  b=f_mtfds(a);
  free(a.pointer);
  a=f_bwtds(b);
  free(b.pointer);
  ergebnis=f_rlds(a);
  free(a.pointer);
  return(ergebnis);
}
STRING f_trims(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  xtrim(n.pointer,0,ergebnis.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}

STRING f_xtrims(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  xtrim(n.pointer,1,ergebnis.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_envs(STRING n) {return(create_string(getenv(n.pointer)));}
#ifdef CONTROL
STRING f_csgets(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=csgets(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_csunits(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=csunit(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_cspnames(int n) {
  STRING ergebnis;
  ergebnis.pointer=cspname(n);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
#endif
#ifdef TINE
STRING f_tinegets(STRING n) { return(tinegets(n.pointer)); }

STRING f_tineunits(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=tineunit(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_tineinfos(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=tineinfo(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_tinequerys(PARAMETER *plist,int e) {
  if(e>=2) return(tinequery(plist[0].pointer,plist[1].integer));
  else return(create_string(NULL));
}
#endif
#ifdef DOOCS
STRING f_doocsgets(STRING n) { return(doocsgets(n.pointer)); }
STRING f_doocsinfos(STRING n) { return(doocsinfos(n.pointer)); }
#endif
STRING f_terminalnames(PARAMETER *plist,int e) {
  STRING ergebnis;
  int i=plist[0].integer;
  if(filenr[i]) ergebnis.pointer=terminalname(fileno(dptr[i]));
  else {
    xberror(24,""); /* File nicht geoeffnet */
    return(vs_error());
  }
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_systems(STRING n) {
  STRING ergebnis;
  FILE *dptr=popen(n.pointer,"r");

  if (dptr==NULL) {
    io_error(errno,"popen");
    ergebnis.pointer=malloc(38+n.len);
    sprintf(ergebnis.pointer,"couldn't execute '%s'. errno=%d",n.pointer,errno);
  } else {
    int len=0;
    int limit=1024;
    int c;
    ergebnis.pointer=NULL;
    do {
      ergebnis.pointer=realloc(ergebnis.pointer,limit);
     /* printf("Bufferlaenge: %d Bytes.\n",limit); */
      while(len<limit) {
        c=fgetc(dptr);
        if(c==EOF) {ergebnis.pointer[len]='\0';break;}
        ergebnis.pointer[len++]=(char)c;
      }
      limit+=len;
    } while(c!=EOF);
    if(pclose(dptr)==-1) io_error(errno,"pclose");
  }
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_prgs(int n) {
  if(n>=prglen || n<0) xberror(16,"PRG$"); /* Feldindex zu gross*/
  return(create_string(program[min(prglen-1,max(n,0))]));
}
STRING f_params(int n) {
  if(n>=param_anzahl || n<0) return(create_string(NULL));
  else return(create_string(param_argumente[min(param_anzahl-1,max(n,0))]));
}
STRING f_unixdates(int n) {
  STRING ergebnis;
  struct tm * loctim;
  loctim=localtime((time_t *)(&n));
  ergebnis.pointer=malloc(12);
  sprintf(ergebnis.pointer,"%02d.%02d.%04d",loctim->tm_mday,loctim->tm_mon+1,1900+loctim->tm_year);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_unixtimes(int n) {
  STRING ergebnis;
  struct tm * loctim;
  loctim=localtime((time_t *)&n);
  ergebnis.pointer=malloc(16);
  sprintf(ergebnis.pointer,"%02d:%02d:%02d",loctim->tm_hour,loctim->tm_min,loctim->tm_sec);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_strs(PARAMETER *plist,int e) {         /* STR$(a[,b[,c[,d]]])     */
  STRING ergebnis;
  int b=-1,c=13,mode=0;
  char formatter[24];
  ergebnis.pointer=malloc(64);
  if(e>1) b=min(50,max(0,plist[1].integer));
  if(e>2) c=min(50,max(0,plist[2].integer));
  if(e>3) mode=plist[3].integer;
  if(mode==0 && b!=-1) sprintf(formatter,"%%%d.%dg",b,c);
  else if (mode==1 && b!=-1) sprintf(formatter,"%%0%d.%dg",b,c);
  else  sprintf(formatter,"%%.13g");
  sprintf(ergebnis.pointer,formatter,plist[0].real);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING f_bins(PARAMETER *plist,int e) {
  STRING ergebnis;
  unsigned int a=plist[0].integer;
  int j,b=8,i=0;
  if(e==2) b=max(0,plist[1].integer);
  ergebnis.pointer=malloc(b+1);
  for(j=b;j>0;j--) ergebnis.pointer[i++]=((a&(1<<(j-1)))  ? '1':'0');
  ergebnis.len=i;
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
}

  /* STR$(a[,b[,c[,d]]])     */
STRING hexoct_to_string(char n,PARAMETER *plist, int e) {
  STRING ergebnis;
  char formatter[20];
  int b=-1,c=13,mode=0;
  unsigned int a=plist[0].integer;
  if(e>1) b=min(50,max(0,plist[1].integer));
  if(e>2) c=min(50,max(0,plist[2].integer));
  if(e>3) mode=plist[3].integer;
	
  if(mode==0 && b!=-1) sprintf(formatter,"%%%d.%d%c",b,c,n);
  else if (mode==1 && b!=-1) sprintf(formatter,"%%0%d.%d%c",b,c,n);
  else  sprintf(formatter,"%%.13%c",n);
  ergebnis.pointer=malloc(31);
  sprintf(ergebnis.pointer,formatter,a);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}

STRING f_hexs(PARAMETER *plist, int e) {return(hexoct_to_string('x',plist,e));}
STRING f_octs(PARAMETER *plist, int e) {return(hexoct_to_string('o',plist,e));}
STRING vs_date() {
  STRING ergebnis;
  time_t timec;
  struct tm * loctim;
  timec = time(&timec);
  loctim=localtime(&timec);
  ergebnis.pointer=malloc(12);
  sprintf(ergebnis.pointer,"%02d.%02d.%04d",loctim->tm_mday,loctim->tm_mon+1,1900+loctim->tm_year);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING vs_time() {
  STRING ergebnis;
  time_t timec;
  struct tm * loctim;
  timec = time(&timec);
  loctim=localtime(&timec);
  ergebnis.pointer=malloc(9);
  strncpy(ergebnis.pointer,ctime(&timec)+11,8);
  ergebnis.len=8;
  ergebnis.pointer[8]=0;
  return(ergebnis);
}
STRING vs_trace() {
  if(pc>=0 && pc<prglen) {
    STRING ergebnis=create_string(program[pc]);
    xtrim(ergebnis.pointer,TRUE,ergebnis.pointer);
    ergebnis.len=strlen(ergebnis.pointer);
    return(ergebnis);
  } else return(vs_error());
}
STRING vs_terminalname() {
  STRING ergebnis;
  ergebnis.pointer=terminalname(STDIN_FILENO);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING vs_fileevent() {
  STRING ergebnis;
  ergebnis.pointer=fileevent();
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
STRING vs_inkey() {  return(create_string(inkey())); }
