/* sfunctions.c   String functions (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif

#include "defs.h"
#ifdef HAVE_GCRYPT
  #include <gcrypt.h>
  static int gcrypt_init=0;
#else
  #include "md5.h"
  #include "sha1.h"
#endif
#include "x11basic.h"
#include "variablen.h"
#include "xbasic.h"
#include "type.h"
#include "functions.h"
#include "sfunctions.h"
#include "parser.h"
#include "parameter.h"
#include "array.h"
#include "io.h"
#include "wort_sep.h"

#include "decode.h"


#ifdef DUMMY_LIST
#define f_nop NULL
#define array_to_string NULL
#define f_fsfirsts NULL
#define f_fsnexts NULL
#define f_inputs NULL
#define f_lineinputs NULL
#define f_xtrims NULL
#define f_words  NULL
#define f_usings NULL
#define f_unixtimes NULL
#define f_unixdates NULL
#define f_uncompresss NULL
#define f_uppers NULL
#define f_lowers NULL
#define f_chrs NULL
#define f_trims NULL
#define f_terminalnames NULL
#define f_systems NULL
#define f_prgs NULL
#define f_replaces NULL
#define f_reverses NULL
#define f_mids NULL
#define f_rights NULL
#define f_lefts NULL
#define f_strings NULL
#define f_strs NULL
#define f_signs NULL
#define f_spaces NULL
#define f_rles NULL
#define f_rlds NULL
#define f_pngdecodes NULL
#define f_pngencodes NULL
#define f_hexs NULL
#define f_octs NULL
#define f_bins NULL
#define f_hashs NULL
#define f_errs NULL
#define f_envs NULL
#define f_dirs NULL
#define f_inlines NULL
#define f_radixs NULL
#define f_rightofs NULL
#define f_leftofs NULL
#define f_params NULL
#define f_mkis NULL
#define f_mkls NULL
#define f_mkss NULL
#define f_mkds NULL
#define f_mkfs NULL
#define f_aries NULL
#define f_arids NULL
#define f_bwtes NULL
#define f_bwtds NULL
#define f_calls NULL
#define f_compresss NULL
#define f_encloses NULL
#define f_decloses NULL
#define f_decrypts NULL
#define f_encrypts NULL
#define f_juldates NULL
#define f_mtfds NULL
#define f_mtfes NULL


#else
static STRING f_errs(int n)    { return(create_string((char *)error_text((char)n,NULL))); }
static STRING f_envs(STRING n) { return(create_string(getenv(n.pointer)));}
static STRING f_dirs(int n)    { return(create_string_and_free2(getcwd(NULL,0)));}


static STRING f_aries(STRING n) {  /* order-0 adaptive arithmetic encoding */
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
static STRING f_arids(STRING n) {  /* order-0 adaptive arithmetic decoding */
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


static STRING f_lowers(STRING n) {   
  int i=0;
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  ergebnis.len=n.len;
  while(i<n.len) {ergebnis.pointer[i]=tolower(n.pointer[i]); i++;}
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
}

static STRING f_uppers(STRING n) {   
  int i=0;
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  ergebnis.len=n.len;
  while(i<n.len) {ergebnis.pointer[i]=toupper(n.pointer[i]); i++;}
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
}



static STRING f_trims(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  xtrim(n.pointer,0,ergebnis.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}

static STRING f_xtrims(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  xtrim(n.pointer,1,ergebnis.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}


static STRING f_decloses(STRING n) {
  if(n.len>1) {
    if((*(n.pointer)=='\"' && n.pointer[n.len-1]=='\"') ||
       (*(n.pointer)=='\'' && n.pointer[n.len-1]=='\'') ||
       (*(n.pointer)=='`' && n.pointer[n.len-1]=='´') ||
       (*(n.pointer)=='(' && n.pointer[n.len-1]==')') ||
       (*(n.pointer)=='[' && n.pointer[n.len-1]==']') ||
       (*(n.pointer)=='{' && n.pointer[n.len-1]=='}') ||
       (*(n.pointer)=='<' && n.pointer[n.len-1]=='>') ) {
   
      STRING ergebnis;
      ergebnis.pointer=malloc(n.len-1);
      ergebnis.len=n.len-2;
      memcpy(ergebnis.pointer,n.pointer+1,ergebnis.len);
      ergebnis.pointer[ergebnis.len]=0;
      return(ergebnis);
    } 
  }
  return(double_string(&n));
}
static STRING f_encloses(PARAMETER *plist,int e) {
  char c1='\"';
  char c2='\"';
  STRING ergebnis;
  if(e>1) {
    if(plist[1].integer>0) {
      c1=*(char *)(plist[1].pointer);
      c2=((char *)plist[1].pointer)[plist[1].integer-1];
    }
  }
  ergebnis.len=plist[0].integer+2;
  ergebnis.pointer=malloc(ergebnis.len+1);
  ergebnis.pointer[0]=c1;
  memcpy(ergebnis.pointer+1,plist->pointer,plist->integer);
  ergebnis.pointer[ergebnis.len-1]=c2;
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
}
static STRING f_usings(PARAMETER *plist,int e) {
  if(plist->typ==PL_INT) plist->real=(double)plist->integer;
  else if(plist->typ==PL_ARBINT)  plist->real=mpz_get_d(*(ARBINT *)(plist->pointer));
  STRING form;
  form.pointer=plist[1].pointer;
  form.len=plist[1].integer;
  return(do_using(plist->real,form));
}

#ifdef CONTROL
static STRING f_csgets(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=csgets(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
static STRING f_csunits(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=csunit(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
static STRING f_cspnames(int n) {
  STRING ergebnis;
  ergebnis.pointer=cspname(n);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
#endif
#ifdef TINE
static STRING f_tinegets(STRING n) { return(tinegets(n.pointer)); }

static STRING f_tineunits(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=tineunit(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
static STRING f_tineinfos(STRING n) {
  STRING ergebnis;
  ergebnis.pointer=tineinfo(n.pointer);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
static STRING f_tinequerys(PARAMETER *plist,int e) {
  if(e>=2) return(tinequery(plist[0].pointer,plist[1].integer));
  else return(create_string(NULL));
}
#endif
#ifdef DOOCS
static STRING f_doocsgets(STRING n) { return(doocsgets(n.pointer)); }
static STRING f_doocsinfos(STRING n) { return(doocsinfos(n.pointer)); }
#endif
static STRING f_terminalnames(PARAMETER *plist,int e) {
  STRING ergebnis;
  FILEINFO fff=get_fileptr(plist->integer);
  if(fff.typ) ergebnis.pointer=terminalname(fileno(fff.dptr));
  else {
    xberror(24,""); /* File nicht geoeffnet */
    return(vs_error());
  }
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
static STRING f_systems(STRING n) {
  STRING ergebnis;
  FILE *dptr=popen(n.pointer,"r");
  if (dptr==NULL) {
    io_error(errno,"popen");
    return(vs_error());
  } else {
    int c=0,len=0;
    int limit=1024;
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
static STRING f_prgs(int n) {
  if(n>=prglen || n<0) xberror(16,"PRG$"); /* Feldindex zu gross*/
  return(create_string(program[min(prglen-1,max(n,0))]));
}
static STRING f_params(int n) {
  if(n>=param_anzahl || n<0) return(create_string(NULL));
  else return(create_string(param_argumente[min(param_anzahl-1,max(n,0))]));
}
static STRING f_unixdates(int n) {
  STRING ergebnis;
  struct tm * loctim;
  time_t nn=(time_t)n;
  loctim=localtime(&nn);
  ergebnis.pointer=malloc(32);
  sprintf(ergebnis.pointer,"%02d.%02d.%04d",loctim->tm_mday,loctim->tm_mon+1,1900+loctim->tm_year);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
static STRING f_unixtimes(int n) {
  STRING ergebnis;
  struct tm * loctim;
  time_t nn=(time_t)n;
  loctim=localtime(&nn);
  ergebnis.pointer=malloc(16);
  sprintf(ergebnis.pointer,"%02d:%02d:%02d",loctim->tm_hour,loctim->tm_min,loctim->tm_sec);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
static STRING f_strs(PARAMETER *plist,int e) {         /* STR$(a[,b[,c[,d]]])     */
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
  switch(plist->typ) {
  case PL_INT:
    plist->real=(double)plist->integer;
  case PL_FLOAT:
    sprintf(ergebnis.pointer,formatter,plist->real);
    break;
  case PL_COMPLEX: {
    char *formatter2=malloc(2*24+4);
    if(plist->imag>=0) {
      sprintf(formatter2,"(%s+%si)",formatter,formatter);
      sprintf(ergebnis.pointer,formatter2,plist->real,plist->imag);
    } else {
      sprintf(formatter2,"(%s%si)",formatter,formatter);
      sprintf(ergebnis.pointer,formatter2,plist->real,plist->imag);
    }
    free(formatter2);
    }
    break;
  case PL_ARBINT: {
    char *buf=mpz_get_str(NULL,10,*(ARBINT *)(plist->pointer));
    free(ergebnis.pointer);
    ergebnis.pointer=buf;
    break;
    }
  default:
    printf("Unknown parameter typ in STR$():\n");
    dump_parameterlist(plist,e);
  }
  
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}


static char *i2a(unsigned i, char *a, unsigned r) {
  if(i/r>0) a=i2a(i/r,a,r);
  *a = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz@$!?"[i % r];
  return a+1;
}


/* We have to use the constant here, because log(2) 
  produces slightly different results on WINDOWS (compilers).
  --> Codeberg issue #6 
*/

#define LOG2 0.6931471805599

static STRING f_bins(PARAMETER *plist,int e) {
  STRING ergebnis;
  if(plist->typ==PL_INT) {
    unsigned int value=plist->integer;
    /*Predict length of string*/
    int plen=1;
    if(value) plen=(log(value)/(double)LOG2)+1;
    int len=plen;
    if(e>1) len=max(plen,plist[1].integer);
    ergebnis.pointer=malloc(len+1);
    ergebnis.len=len;
    char *a=ergebnis.pointer;
    while(len>plen) {*a++='0';len--;}
    *i2a(value,a,2)=0;
    return(ergebnis);
  } 
  cast_to_arbint(plist);
  char *s=mpz_get_str(NULL,2,*(ARBINT *)(plist->pointer));
  int plen=strlen(s);
  int len=plen;
  if(e>1) len=max(plen,plist[1].integer);
  if(plen>=len) {
    ergebnis.pointer=s;
    ergebnis.len=plen;
    return(ergebnis);
  }
  char *a=ergebnis.pointer=malloc(len+1);
  ergebnis.len=len;
  while(len>plen) {*a++='0';len--;}
  strcpy(a,s);
  free(s);
  return(ergebnis);
}
static STRING f_octs(PARAMETER *plist,int e) {
  STRING ergebnis;  
  if(plist->typ==PL_INT) {
    unsigned int value=plist[0].integer;
    /*Predict length of string*/
    int plen=1;
    if(value) plen=(log(value)/(double)log(8))+1;
    int len=plen;
    if(e>1) len=max(plen,plist[1].integer);
    ergebnis.pointer=malloc(len+1);
    ergebnis.len=len;
    char *a=ergebnis.pointer;
    while(len>plen) {*a++='0';len--;}
    *i2a(value,a,8)=0;
    return(ergebnis);
  }
  cast_to_arbint(plist);
  char *s=mpz_get_str(NULL,8,*(ARBINT *)(plist->pointer));
  int plen=strlen(s);
  int len=plen;
  if(e>1) len=max(plen,plist[1].integer);
  if(plen>=len) {
    ergebnis.pointer=s;
    ergebnis.len=plen;
    return(ergebnis);
  }
  char *a=ergebnis.pointer=malloc(len+1);
  ergebnis.len=len;
  while(len>plen) {*a++='0';len--;}
  strcpy(a,s);
  free(s);
  return(ergebnis);
}
static STRING f_hexs(PARAMETER *plist,int e) {
  STRING ergebnis;
  if(plist->typ==PL_INT) {
    unsigned int value=plist[0].integer;
    /*Predict length of string*/
    int plen=1;
    if(value) plen=(int)(log((double)value)/(double)log(16))+1;
    int len=plen;
    if(e>1) len=max(plen,plist[1].integer);
    ergebnis.pointer=malloc(len+1);
    ergebnis.len=len;
    char *a=ergebnis.pointer;
 // printf("valuse=%d, len=%d, plen=%d\n",value, len,plen);
    while(len>plen) {*a++='0';len--;}
 // printf("ergebnis=<%s>\n",ergebnis.pointer);
    *i2a(value,a,16)=0;
    return(ergebnis);
  }
  cast_to_arbint(plist);
  char *s=mpz_get_str(NULL,16,*(ARBINT *)(plist->pointer));
  int plen=strlen(s);
  int len=plen;
  if(e>1) len=max(plen,plist[1].integer);
  if(plen>=len) {
    ergebnis.pointer=s;
    ergebnis.len=plen;
    return(ergebnis);
  }
  char *a=ergebnis.pointer=malloc(len+1);
  ergebnis.len=len;
  while(len>plen) {*a++='0';len--;}
  strcpy(a,s);
  free(s);
  return(ergebnis);
}

/*  a$=RADIX$(n%[,b%[,l%]])*/
static STRING f_radixs(PARAMETER *plist, int e) {
  STRING ergebnis;
  unsigned int base=64;
  if(e>1) base=min(62,max(2,plist[1].integer));
  if(plist->typ==PL_INT) {
    int len=0,plen=1;
    int value=plist->integer;
    int sign=1;
    if(value<0) {sign=-1;value=-value;}
  /*Predict length of string*/
  
    if(value) plen=(log(value)/(double)log(base))+1;
    if(sign<0) plen++;
    if(e>2) len=max(plen,plist[2].integer);
    else len=plen;
    ergebnis.pointer=malloc(len+1);
    ergebnis.len=len;
    char *a=ergebnis.pointer;
  
    if(sign<0) {*a++='-';len--;}
    while(len>plen) {*a++='0';len--;}
    *i2a(value,a,base)=0;
    return(ergebnis);
  }
  cast_to_arbint(plist);
  char *s=mpz_get_str(NULL,base,*(ARBINT *)(plist->pointer));
  int plen=0;
  if(!s) printf("ERROR in RADIX() base=%d\n",base);
  else plen=strlen(s);
  int len=plen;
  if(e>2) len=max(plen,plist[2].integer);
  if(plen>=len) {
    ergebnis.pointer=s;
    ergebnis.len=plen;
    return(ergebnis);
  }
  char *a=ergebnis.pointer=malloc(len+1);
  ergebnis.len=len;
  while(len>plen) {*a++='0';len--;}
  if(s) strcpy(a,s);
  else *a++=0;
  free(s);
  return(ergebnis);
}



/* Gibt das i te Wort aus Zeichenkette zurueck (mit Blank getrennt, Teile in
Anfuehrungszeichen werden als ein Wort behandelt. (c) Markus Hoffmann 2010)*/
static STRING f_words(PARAMETER *plist,int e) {
  STRING ergebnis;
  int c=1,f=0,i=0,j=0,k,l=1;
  char *p,delimiter=' ';

  if(e>=3) delimiter=*(char *)plist[2].pointer;
  if(e>=2) l=plist[1].integer;
  if(e>=1) {
    k=ergebnis.len=plist->integer;
    ergebnis.pointer=malloc(ergebnis.len+1);
    p=plist->pointer;
    while(i<k) {
      if(p[i]==delimiter && !f) c++;
      else if(p[i]=='"') f=!f;
      else if(c==l) ergebnis.pointer[j++]=p[i];
      i++; 
    }
    ergebnis.pointer[j]=0;
    ergebnis.len=j;
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  return(ergebnis);
}

static STRING f_spaces(int n) {   
  STRING ergebnis;
  int i=0; 
  if(n<0) n=0;
  ergebnis.pointer=malloc(n+1);
  ergebnis.len=n;
  while(i<n) ergebnis.pointer[i++]=' ';
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
}

static STRING f_lefts(PARAMETER *plist,int e) {
  STRING ergebnis;
  int l=1;
  if(e>1) l=plist[1].integer;
  if(l>0) {
    if(l>plist->integer) l=plist->integer;
    ergebnis.len=l;
    ergebnis.pointer=malloc(l+1);
    memcpy(ergebnis.pointer,plist->pointer,l);
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
}
static STRING f_leftofs(PARAMETER *plist,int e) {
  STRING ergebnis;
  ergebnis.len=plist->integer;
  ergebnis.pointer=malloc(ergebnis.len+1);

  if(plist->integer<=plist[1].integer) {
    memcpy(ergebnis.pointer,plist->pointer,plist->integer);
    ergebnis.pointer[ergebnis.len]=0;
  } else {
    char *t=plist->pointer;
    char *c=plist[1].pointer;
    char *w1=ergebnis.pointer;
    int i=0,j=0,f=0;
    while(i<plist->integer) {
/* suche erstes Vorkommen von c */
      while(i<plist->integer && (t[i]!=*c || f)) {
        if(t[i]=='"') f=!f;
        w1[j++]=t[i++];
      }

      if(i>=plist->integer) { /* schon am ende ? */
        w1[j]=0;
	ergebnis.len=j;
        return(ergebnis);
      } else {     /* ueberpruefe, ob auch der Rest von c vorkommt */
    
        if(memcmp(t+i,c,plist[1].integer)==0) {
          w1[j]=0;
	  ergebnis.len=j;
          return(ergebnis);
        } else {
          if(t[i]=='"') f=!f;
          w1[j++]=t[i++];
        }     /* ansonsten weitersuchen */
      }
    }
  }
  return(ergebnis);
}

static STRING f_rightofs(PARAMETER *plist,int e) {
  STRING ergebnis;
  ergebnis.len=plist->integer;
  ergebnis.pointer=malloc(ergebnis.len+1);

  if(plist->integer<=plist[1].integer) {
    ergebnis.len=0;
    ergebnis.pointer[ergebnis.len]=0;
  } else {
    char *t=plist->pointer;
    char *c=plist[1].pointer;
    char *w2=ergebnis.pointer;
    int i=0,f=0;
    while(i<plist->integer) {
/* suche erstes Vorkommen von c */
      while(i<plist->integer && (t[i]!=*c || f)) {
        if(t[i]=='"') f=!f;
        i++;
      }

      if(i>=plist->integer) { /* schon am ende ? */
        w2[0]=0;
	ergebnis.len=0;
        return(ergebnis);
      } else {     /* ueberpruefe, ob auch der Rest von c vorkommt */
    
        if(memcmp(t+i,c,plist[1].integer)==0) {
	  int j=0;
	  i+=plist[1].integer;
  	  while(i<plist->integer) w2[j++]=t[i++];
          w2[j]=0;
	  ergebnis.len=j;	  
          return(ergebnis);
        } else {
          if(t[i]=='"') f=!f;
          i++;
        }     /* ansonsten weitersuchen */
      }
    }
  }
  return(ergebnis);
}

static STRING f_rights(PARAMETER *plist,int e) {
  STRING ergebnis;
  int l=1;
  if(e>1) l=plist[1].integer;
  if(l>0) {
    if(l>plist->integer) l=plist->integer;
    ergebnis.pointer=malloc(l+1);
    ergebnis.len=l;
    memcpy(ergebnis.pointer,plist->pointer+plist->integer-l,l);
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
}


static STRING f_mids(PARAMETER *plist,int e) {  
  STRING ergebnis;
  int l=1;
  if(e>2) l=plist[2].integer;
  int p=plist[1].integer;
  if(p==0) p=1;
  if(l>0 && p>0 && p<=plist->integer) {
    p--;
    if(p+l>plist->integer) l=plist->integer-p;
    ergebnis.pointer=malloc(l+1);
    ergebnis.len=l;
    memcpy(ergebnis.pointer,plist->pointer+p,l);  
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
}

static STRING f_strings(PARAMETER *plist,int e) {
  STRING ergebnis;
  int j=plist->integer;
  if(j>0) {
    int i=0;
    ergebnis.len=j*plist[1].integer;    
    ergebnis.pointer=malloc(ergebnis.len+1);
    while(i<j) {memcpy(ergebnis.pointer+i*plist[1].integer,
                       plist[1].pointer,plist[1].integer); i++;}
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  ergebnis.pointer[ergebnis.len]=0;
  return(ergebnis);
}

static STRING f_chrs(int n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(2);
  ergebnis.len=1;
  *ergebnis.pointer=(char)n;
  *(ergebnis.pointer+1)=0;
  return(ergebnis);
}
static STRING f_mkis(int n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(sizeof(short)+1);
  ergebnis.len=sizeof(short);
  *((short *)ergebnis.pointer)=(short)n;
  *(ergebnis.pointer+sizeof(short))=0;
  return(ergebnis);
}
static STRING f_mkls(int n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(4+1);
  ergebnis.len=4;
  *((uint32_t *)ergebnis.pointer)=(uint32_t)n;
  *(ergebnis.pointer+4)=0;
  return(ergebnis);
}
static STRING f_mkfs(double n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(sizeof(float)+1);
  ergebnis.len=sizeof(float);
  *((float *)ergebnis.pointer)=(float)n;
  *(ergebnis.pointer+sizeof(float))=0;
  return(ergebnis);
}
static STRING f_mkds(double n) {   
  STRING ergebnis;
  ergebnis.pointer=malloc(sizeof(double)+1);
  ergebnis.len=sizeof(double);
  *((double *)ergebnis.pointer)=n;
  *(ergebnis.pointer+sizeof(double))=0;
  return(ergebnis);
}

static STRING f_replaces(PARAMETER *plist,int e) {  /* MH 10.02.2004 */
  STRING ergebnis;
  char *pos;
  int i=0;
  int start=0;
  ergebnis.len=0;
  ergebnis.pointer=malloc(1);
  if(e==3) {
 // printf("REPLACE: <%s>--<%s>/<%s>\n",plist[0].pointer,plist[1].pointer,plist[2].pointer );

    pos=(char *)memmem(&(((char *)(plist[0].pointer))[start]),plist[0].integer-start,
                       plist[1].pointer,plist[1].integer);
    while(pos!=NULL) {         
      i=((int)(pos-(char *)plist[0].pointer))-start;     
      ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+i+plist[2].integer);
      memcpy((char *)ergebnis.pointer+ergebnis.len,(char *)plist[0].pointer+start,i);
      memcpy((char *)ergebnis.pointer+ergebnis.len+i,(char *)plist[2].pointer,plist[2].integer);
      ergebnis.len+=i+plist[2].integer;
      start+=i+plist[1].integer;
      pos=(char *)memmem(&(((char *)(plist[0].pointer))[start]),plist[0].integer-start,
                       plist[1].pointer,plist[1].integer);
    }		       
    ergebnis.pointer=realloc(ergebnis.pointer,ergebnis.len+(plist[0].integer-start));
    memcpy((char *)ergebnis.pointer+ergebnis.len,(char *)plist[0].pointer+start,plist[0].integer-start);
    ergebnis.len+=(plist[0].integer-start);
  }
  return(ergebnis);
}


/*  Conversion from Base64 (+36) radix data to 8 Bit (Base256) binary data*/

static STRING f_inlines(STRING n) {   
  int l1=n.len>>2;
  int a=n.len&3;
  STRING ergebnis;
  if(a==0) ergebnis.len=l1*3;
  else if(a==1) ergebnis.len=l1*3+1;  /* Should not happen*/
  else if(a==2) ergebnis.len=l1*3+1;  
  else  ergebnis.len=l1*3+2;  
 
  char *p=n.pointer;
  char *q=ergebnis.pointer=malloc(ergebnis.len+1);
  while(l1-->0) {
   *q++=(((p[0]-36) & 0x3f)<<2)|(((p[1]-36) & 0x30)>>4);
   *q++=(((p[1]-36) & 0xf)<<4) |(((p[2]-36) & 0x3c)>>2);
   *q++=(((p[2]-36) & 0x3)<<6) |(((p[3]-36) & 0x3f));
    p+=4;
  }
  if(a==1) *q++=(((*p-36) & 0x3f)<<2); /* soll nicht vorkommen*/
  else if(a==2) *q++=(((p[0]-36) & 0x3f)<<2)|(((p[1]-36) & 0x30)>>4);
  else if(a==3) {
    *q++=(((p[0]-36) & 0x3f)<<2)|(((p[1]-36) & 0x30)>>4);
    *q++=(((p[1]-36) & 0xf)<<4)|(((p[2]-36) & 0x3c)>>2);
  }
  (ergebnis.pointer)[ergebnis.len]=0;
  return(ergebnis);
}




static STRING f_reverses(STRING n) {   
  int i=0;
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  ergebnis.len=n.len;
  while(i<n.len) {ergebnis.pointer[i]=n.pointer[n.len-i-1]; i++;}
  (ergebnis.pointer)[ergebnis.len]=0; 
  return(ergebnis);
}


#define IGREG 2299161
static STRING f_juldates(int n) {   
  STRING ergebnis;
  long ja,jalpha,jb;
  long day,mon,jahr;
  ergebnis.pointer=malloc(16);
  if(n>=IGREG) {
    jalpha=(long)(((float)(n-1867216)-0.25)/36524.25);
    ja=n+1+jalpha-(long)(0.25*jalpha);
  } else ja=n;
  jb=ja+1524;
  jahr=(long)(6680.0+((float)(jb-2439870)-122.1)/365.25);
  day=(long)(365*jahr+(0.25*jahr));
  mon=(long)((jb-day)/30.6001);
  day=jb-day-(long)(30.6001*mon);
  mon--;
  if(mon>12) mon-=12;
  jahr-=4715;
  if(mon>2) --jahr;
  if(jahr<=0) --jahr;
  sprintf(ergebnis.pointer,"%02d.%02d.%04d",(int)day,(int)mon,(int)jahr);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
#undef IGREG



/* ************ Code Engines   ************************ */
/* *********  RLE -- Run Length Encoding/Decoding ***** */

/*  In the output stream, any two consecutive
    characters with the same value flag a run.  A byte following
    those two characters gives the count of *additional*
    repeat characters, which can be anything from 0 to 255.  */

static STRING f_rles(STRING n) {  /*Run Length Encoding*/
  int j=0,i=0;
  unsigned char c,count,last=0;
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+(int)(n.len/2)+1);
  while(i<n.len) {
    c=n.pointer[i++];
    ergebnis.pointer[j++]=c;
    if(c==last) {
      count=0;
      while(count<255 && i<n.len) {
        c=n.pointer[i++];
        if(c==last) count++;
        else break;
      }
      ergebnis.pointer[j++]=count;
      if(count!=255 && c!=last) ergebnis.pointer[j++]=c;
    }
    last=c;
  }
  ergebnis.len=j;
  (ergebnis.pointer)[ergebnis.len]=0; 
  return(ergebnis);
}
static STRING f_rlds(STRING n) {  /*Run Length Decoding*/
  int j=0,i=0;
  unsigned char c,count,last=0;
  STRING ergebnis;
 
  while(i<n.len) {        /* Laenge bestimmen : */
      c=n.pointer[i++];
      j++;
      if(c==last) {
            count=n.pointer[i++];
            j+=(count&0xff);
      }
      last=c;
  }
  last=0;
  ergebnis.pointer=malloc(j+1);
  i=j=0;
  while(i<n.len) {        /* Decodieren */
      c=n.pointer[i++];
      ergebnis.pointer[j++]=c;
      if(c==last) {
        count=n.pointer[i++];
        while(count-->0) ergebnis.pointer[j++]=c;
      }
      last=c;
  }
  ergebnis.len=j;
  (ergebnis.pointer)[ergebnis.len]=0; 
  return(ergebnis);
}


/*  An MTF encoder encodes each character using the
    count of distinct previous characters seen since the characters
    last appearance.  This is implemented by keeping an array of
    characters.  Each new input character is encoded with its
    current position in the array.  The character is then moved to
    position 0 in the array, and all the higher order characters
    are moved down by one position to make roon.
  
    Both the encoder and decoder have to start with the order array
    initialized to the same values.   */

static STRING f_mtfes(STRING n) {  /* Move To Front Encoding*/
    unsigned char order[256];
    unsigned int i,j,k;
    unsigned char c;
    STRING ergebnis;
    ergebnis.pointer=malloc(n.len+1);
    for(i=0;i<256;i++) order[i]=(unsigned char)i;
    i=0;
    while(i<n.len) {
      c=n.pointer[i];
      /* Find the char, and output it  */
     for(j=0;j<256;j++)
       if(order[j]==(c&0xff)) break;
      ergebnis.pointer[i++]=j;
          /* Now shuffle the order array  */
      for(k=j;k>0;k--) order[k]=order[k-1];
      order[0]=c;
    }  
  ergebnis.len=n.len;
  (ergebnis.pointer)[ergebnis.len]=0; 
  return(ergebnis);
}
static STRING f_mtfds(STRING n) {  /* Move To Front Decoding*/
    unsigned char order[256];
    unsigned int i,j,k;
    unsigned char c;
    STRING ergebnis;
    ergebnis.pointer=malloc(n.len+1);
    for(i=0;i<256;i++) order[i]=(unsigned char)i;
    i=0;
    while(i<n.len) {
      j=n.pointer[i] & 0xff;
      c=order[j];
      ergebnis.pointer[i++]=c;
          /* Now shuffle the order array  */
      for(k=j;k>0;k--) order[k]=order[k-1];
      order[0]=c;
    }  
  ergebnis.len=n.len;
  (ergebnis.pointer)[ergebnis.len]=0; 
  return(ergebnis);
}



static STRING f_bwtes(STRING n) {  /* Burrows-Wheeler transform*/
  STRING ergebnis;
  int indices[n.len];
  int i;
  ergebnis.pointer=malloc(n.len+1+sizeof(int));
  for(i=0;i<n.len;i++) indices[i]=i;
  rotlexcmp_buf=(unsigned char *)n.pointer;
  rottexcmp_bufsize=n.len;
  qsort(indices,n.len,sizeof(int),rotlexcmp);
  for(i=0;i<n.len;i++)
        ergebnis.pointer[i+sizeof(int)]=n.pointer[(indices[i]+n.len-1)%n.len];

  for(i=0;i<n.len;i++) {
    if(indices[i]==1) {
      *((int *)ergebnis.pointer)=i;
      break;
    }
  }
  ergebnis.len=n.len+sizeof(int);
  (ergebnis.pointer)[ergebnis.len]=0; 
  return(ergebnis);
}
static STRING f_bwtds(STRING n) {  /* inverse Burrows-Wheeler transform */
  STRING ergebnis;
  int primary_index=0;
  int size=0;   
  if(n.len>=sizeof(int)) {
    size=n.len-sizeof(int);
    primary_index=*((int *)n.pointer);
  }
  ergebnis.pointer=malloc(size+1);
  {
    unsigned char F[size];
    int buckets[256];
    int i,j,k;
    int indices[size];

    for(i=0;i<256;i++) buckets[i]=0;
    for(i=0;i<size;i++) buckets[n.pointer[sizeof(int)+i]&0xff]++;
    for(i=0,k=0;i<256;i++)
      for(j=0;j<buckets[i];j++) F[k++]=i;
    j=0;
    for(i=0;i<256;i++) {
      while(i>F[j] && j<size) j++;
      buckets[i]=j;     // it will get fake values if there is no i in F, but
                        // that won't bring us any problems
    }

    for(i=0;i<size;i++) indices[buckets[n.pointer[sizeof(int)+i]&0xff]++]=i;
    for(i=0,j=primary_index;i<size;i++) {
      ergebnis.pointer[i]=n.pointer[j+sizeof(int)];
      j=indices[j];
    }
  }
  ergebnis.len=size;
  (ergebnis.pointer)[ergebnis.len]=0; 
  return(ergebnis);
}
static STRING f_hashs(PARAMETER *plist,int e) {
  STRING ergebnis;
  int typ=1;  /*  Default is md5 */
  if(e>1) typ=plist[1].integer;
  if(typ==0 || (typ>11 && typ<301) || typ>307) typ=1;
#ifdef HAVE_GCRYPT
  if(!gcrypt_init) {
    if(!gcry_check_version(GCRYPT_VERSION)) {
      puts("ERROR: libgcrypt version mismatch\n");
    } 
    gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN);
    gcry_control(GCRYCTL_INIT_SECMEM, 16384, 0);
    gcry_control(GCRYCTL_RESUME_SECMEM_WARN);
    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
    gcrypt_init=1;
  }
  int hash_length = gcry_md_get_algo_dlen(typ);
  ergebnis.len=hash_length;
  ergebnis.pointer=malloc(ergebnis.len+1);
  gcry_md_hash_buffer(typ,ergebnis.pointer, plist[0].pointer, plist[0].integer);
#else
  if(typ==1) {    /*  MD5 */
    MD5_CTX ctx;
    MD5_Init(&ctx);
    MD5_Update(&ctx, plist->pointer, plist->integer);
    ergebnis.len=MD5_DIGEST_LENGTH;
    ergebnis.pointer=malloc(ergebnis.len+1);
    ergebnis.pointer[MD5_DIGEST_LENGTH]=0;
    MD5_Final((unsigned char *)ergebnis.pointer, &ctx);
  } else if(typ==2) {    /*  SHA1 */
    sha1_context ctx;
    sha1_starts(&ctx);
    sha1_update(&ctx, plist->pointer, plist->integer);
    ergebnis.len=SHA1_DIGEST_LENGTH;
    ergebnis.pointer=malloc(ergebnis.len+1);
    ergebnis.pointer[SHA1_DIGEST_LENGTH]=0;
    sha1_finish(&ctx, (unsigned char *)ergebnis.pointer);
  } else {
    printf("The %s function is not implemented \n"
    " in this version of X11-Basic because the GCRYPT library \n"
    " was not present at compile time.\n","HASH$()");
    ergebnis.len=plist[0].integer;
    ergebnis.pointer=malloc(ergebnis.len+1);
  }
#endif
  (ergebnis.pointer)[ergebnis.len]=0; 
  return(ergebnis);
}




static STRING do_encrypt(STRING *message, STRING *key, int typ) {
  STRING ergebnis;
#ifdef HAVE_GCRYPT
  if(!gcrypt_init) {
    if(!gcry_check_version(GCRYPT_VERSION)) {
      puts("ERROR: libgcrypt version mismatch\n");
    } 
    gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN);
    gcry_control(GCRYCTL_INIT_SECMEM, 16384, 0);
    gcry_control(GCRYCTL_RESUME_SECMEM_WARN);
    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
    gcrypt_init=1;
  }
  size_t blkLength = gcry_cipher_get_algo_blklen(typ);
  size_t keyLength = gcry_cipher_get_algo_keylen(typ);
  gcry_cipher_hd_t hd;
  
  if(key->len<keyLength) printf("WARNING: Key too short (%d). It must be at least %d bytes.\n",key->len,keyLength);
  if(message->len%blkLength) printf("WARNING: The message length (%d) must be a multiple of %d bytes.\n",message->len,blkLength);
  int len=(message->len-1)/blkLength+1;
  
    ergebnis.len=len*blkLength;
    ergebnis.pointer=malloc(ergebnis.len+1);
    gcry_cipher_open(&hd, typ, GCRY_CIPHER_MODE_CBC, 0);
    gcry_cipher_setkey(hd,key->pointer, keyLength);
   // gcry_cipher_setiv(hd, iniVector, blkLength);
    int ret=gcry_cipher_encrypt(hd, ergebnis.pointer,ergebnis.len, message->pointer,len*blkLength);
    if(ret) printf("cipher failed:  %s/%s\n",gcry_strsource(ret),gcry_strerror(ret));
    gcry_cipher_close(hd);
#else
  printf("The %s function is not implemented \n"
  " in this version of X11-Basic because the GCRYPT library \n"
  " was not present at compile time.\n","ENCRYPT$()");
  ergebnis.len=message->len;
  ergebnis.pointer=malloc(ergebnis.len+1);
  memcpy(ergebnis.pointer,message->pointer,ergebnis.len);
#endif
  (ergebnis.pointer)[ergebnis.len]=0; 
  return(ergebnis);
}
static STRING f_encrypts(PARAMETER *plist,int e) {
  STRING ergebnis;
  int typ=4;  /*  Default is BLOWFISH */
  if(e>2) typ=plist[2].integer;
   ergebnis=do_encrypt((STRING *)&(plist[0].integer),(STRING *)&(plist[1].integer),typ);
  return(ergebnis);
}

/* 
   SIGN$(message$,key$,typ)
   Wie soll das ueberhaupt gehen?
   1. Wir berechnen einen Hash der Message z.B. mit SHA1
   2. der Hash wird nun mit dem privaten Schluessel verschluesselt
   3. der verschluesselte Hash als Signatur zurueckgeliefert.
   
   Es fehlt dann noch die Umkehrfunktion VERYFY(message$, signature$, pubkey$)
   
   */

static STRING f_signs(PARAMETER *plist,int e) {
  STRING ergebnis;
  STRING tmp;
  int typ=4;  /*  Default is BLOWFISH */
  if(e>2) typ=plist[2].integer;
  /* ALso erst einen SHA1 Hash berechnen....*/
#ifdef HAVE_GCRYPT
#else
  sha1_context ctx;
  sha1_starts(&ctx);
  sha1_update(&ctx, plist->pointer, plist->integer);
  tmp.len=SHA1_DIGEST_LENGTH;
  tmp.pointer=malloc(tmp.len+1);
  tmp.pointer[SHA1_DIGEST_LENGTH]=0;
  sha1_finish(&ctx, (unsigned char *)tmp.pointer);
#endif
  /* Jetzt verschluesseln (geht noch nicht)*/  
  ergebnis=do_encrypt(&tmp,(STRING *)&(plist[1].integer),typ);
  free_string(&tmp);
  return(ergebnis);
}

static STRING do_decrypt(STRING *message, STRING *key, int typ) {
  STRING ergebnis;

#ifdef HAVE_GCRYPT
  if(!gcrypt_init) {
    if(!gcry_check_version(GCRYPT_VERSION)) {
      puts("ERROR: libgcrypt version mismatch\n");
    } 
    gcry_control(GCRYCTL_SUSPEND_SECMEM_WARN);
    gcry_control(GCRYCTL_INIT_SECMEM, 16384, 0);
    gcry_control(GCRYCTL_RESUME_SECMEM_WARN);
    gcry_control (GCRYCTL_INITIALIZATION_FINISHED, 0);
    gcrypt_init=1;
  }
  size_t blkLength = gcry_cipher_get_algo_blklen(typ);
  size_t keyLength = gcry_cipher_get_algo_keylen(typ);
  gcry_cipher_hd_t hd;
  
  if(key->len<keyLength) printf("WARNING: Key too short (%d). It must be at least %d bytes.\n",key->len,keyLength);
  if(message->len%blkLength) printf("WARNING: The message length (%d) must be a multiple of %d bytes.\n",message->len,blkLength);
  int len=(message->len-1)/blkLength+1;
  
    ergebnis.len=len*blkLength;
    ergebnis.pointer=malloc(ergebnis.len+1);
    gcry_cipher_open(&hd, typ, GCRY_CIPHER_MODE_CBC, 0);
    gcry_cipher_setkey(hd,key->pointer, keyLength);
   // gcry_cipher_setiv(hd, iniVector, blkLength);
    int ret=gcry_cipher_decrypt(hd, ergebnis.pointer,ergebnis.len, message->pointer, len*blkLength);
    if(ret) printf("cipherdecrypt failed:  %s/%s\n",gcry_strsource(ret),gcry_strerror(ret));
    gcry_cipher_close(hd);

#else
  printf("The %s function is not implemented \n"
  " in this version of X11-Basic because the GCRYPT library \n"
  " was not present at compile time.\n","DECRYPT$()");
  ergebnis.len=message->len;
  ergebnis.pointer=malloc(ergebnis.len+1);
  memcpy(ergebnis.pointer,message->pointer,message->len);
#endif
  (ergebnis.pointer)[ergebnis.len]=0; 
  return(ergebnis);
}

static STRING f_decrypts(PARAMETER *plist,int e) {
  STRING ergebnis;
  int typ=4;  /*  Default is BLOWFISH */
  if(e>2) typ=plist[2].integer;
  ergebnis=do_decrypt((STRING *)&(plist[0].integer),(STRING *)&(plist[1].integer),typ);
  return(ergebnis);
}


int do_verify(STRING *message,STRING *signature,STRING *key, int typ) {
  /*  Erst hash der message berechnen.*/
  STRING sha1,tmp;
  int flag=1;
#ifdef HAVE_GCRYPT
  sha1.len=gcry_md_get_algo_dlen(8);
#else
  sha1_context ctx;
  sha1_starts(&ctx);
  sha1_update(&ctx, (unsigned char *)message->pointer, message->len);
  sha1.len=SHA1_DIGEST_LENGTH;
  sha1.pointer=malloc(sha1.len+1);
  sha1_finish(&ctx, (unsigned char *)sha1.pointer);
#endif
  /*  Dann signatur entschluesseln*/
  tmp=do_decrypt(signature,key,typ);
  /* Dann hashes vergleichen*/
  if(tmp.len==sha1.len)
    flag=memcmp(sha1.pointer,tmp.pointer,sha1.len);
  free_string(&sha1);
  free_string(&tmp);
  return((flag==0)?-1:0);
}
static STRING f_compresss(STRING n) {
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
static STRING f_uncompresss(STRING n) {
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

STRING pngtobmp(unsigned char *data, size_t pngsize);
STRING bmptopng(unsigned char *data);

static STRING f_pngdecodes(STRING n) {
  return(pngtobmp((unsigned char *)n.pointer,(size_t)n.len));
}
static STRING f_pngencodes(STRING n) {  
  return(bmptopng((unsigned char *)n.pointer));
}
#endif
const SFUNCTION psfuncs[]= {  /* alphabetisch !!! */

 { F_CONST|F_ARGUMENT,  "!nulldummy", (sfunc)f_nop ,0,0   ,{0}},
 { F_CONST|F_SQUICK,    "ARID$"     , f_arids ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "ARIE$"     , f_aries ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE,    "BIN$"      , f_bins ,1,2    ,{PL_NUMBER,PL_INT}},
 { F_CONST|F_SQUICK,    "BWTD$"     , f_bwtds ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "BWTE$"     , f_bwtes ,1,1   ,{PL_STRING}},

 { F_PLISTE,            "CALL$"     , f_calls ,1,-1  ,{PL_INT,PL_EVAL}},
 { F_CONST|F_IQUICK,    "CHR$"      , f_chrs ,1,1    ,{PL_INT}},
 { F_CONST|F_SQUICK,    "COMPRESS$" , f_compresss ,1,1   ,{PL_STRING}},
#ifdef CONTROL
 { F_SQUICK,            "CSGET$"    , f_csgets ,1,1   ,{PL_STRING}},
 { F_IQUICK,            "CSPNAME$"  , f_cspnames ,1,1   ,{PL_INT}},
 { F_SQUICK,            "CSUNIT$"   , f_csunits ,1,1   ,{PL_STRING}},
#endif
 { F_CONST|F_SQUICK,    "DECLOSE$"  , f_decloses ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE,    "DECRYPT$"  , f_decrypts ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},
 { F_IQUICK,            "DIR$"      , f_dirs ,0,1   ,{PL_INT}},
#ifdef DOOCS
 { F_SQUICK,    "DOOCSGET$"    , f_doocsgets ,1,1   ,{PL_STRING}},
 { F_SQUICK,    "DOOCSINFO$"    , f_doocsinfos ,1,1   ,{PL_STRING}},
#endif

 { F_CONST|F_PLISTE,    "ENCLOSE$" , f_encloses ,1,2   ,{PL_STRING,PL_STRING}},
 { F_CONST|F_PLISTE,    "ENCRYPT$", f_encrypts ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},
 { F_SQUICK,    "ENV$"    , f_envs ,1,1   ,{PL_STRING}},
 { F_CONST|F_IQUICK,    "ERR$"    , f_errs ,1,1   ,{PL_INT}},
 { F_PLISTE,    "FSFIRST$"    , f_fsfirsts ,1,3,  {PL_STRING,PL_STRING,PL_STRING} },
 { F_SIMPLE,    "FSNEXT$"    , f_fsnexts ,0,0   },


 { F_CONST|F_PLISTE,  "HASH$", f_hashs ,1,2   ,{PL_STRING,PL_INT}},

 { F_CONST|F_PLISTE,  "HEX$"    , f_hexs ,1,2   ,{PL_NUMBER,PL_INT}},
 { F_CONST|F_SQUICK,    "INLINE$" , f_inlines ,1,1   ,{PL_STRING}},
 { F_ARGUMENT,  "INPUT$"  , f_inputs ,1,2   ,{PL_FILENR,PL_INT}},
 { F_CONST|F_IQUICK,    "JULDATE$" , f_juldates ,1,1   ,{PL_INT}},

 { F_CONST|F_PLISTE,    "LEFT$" , f_lefts ,1,2   ,{PL_STRING,PL_INT}},
 { F_CONST|F_PLISTE,    "LEFTOF$" , f_leftofs ,2,2   ,{PL_STRING,PL_STRING}},
 { F_PLISTE,    "LINEINPUT$" , f_lineinputs ,1,1   ,{PL_FILENR}},
 { F_CONST|F_SQUICK,    "LOWER$"    , f_lowers ,1,1   ,{PL_STRING}},

 { F_CONST|F_PLISTE,    "MID$"    , f_mids ,2,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_CONST|F_AQUICK,    "MKA$"    , array_to_string ,1,1   ,{PL_ARRAY}},
 { F_CONST|F_DQUICK,    "MKD$"    , f_mkds ,1,1   ,{PL_FLOAT}},
 { F_CONST|F_DQUICK,    "MKF$"    , f_mkfs ,1,1   ,{PL_FLOAT}},
 { F_CONST|F_IQUICK,    "MKI$"    , f_mkis ,1,1   ,{PL_INT}},
 { F_CONST|F_IQUICK,    "MKL$"    , f_mkls ,1,1   ,{PL_INT}},
 { F_CONST|F_DQUICK,    "MKS$"    , f_mkfs ,1,1   ,{PL_FLOAT}},
 { F_CONST|F_SQUICK,    "MTFD$"  , f_mtfds ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "MTFE$"  , f_mtfes ,1,1   ,{PL_STRING}},

 { F_CONST|F_PLISTE,  "OCT$"    , f_octs ,1,2   ,{PL_NUMBER,PL_INT}},

 { F_IQUICK,    "PARAM$"  , f_params ,1,1   ,{PL_INT}},
 
 { F_CONST|F_SQUICK,    "PNGDECODE$"  , f_pngdecodes ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "PNGENCODE$"  , f_pngencodes ,1,1   ,{PL_STRING}},
 
 { F_CONST|F_IQUICK,    "PRG$"    , f_prgs ,1,1   ,{PL_INT}},
 { F_CONST|F_PLISTE,    "RADIX$"    , f_radixs ,1,3   ,{PL_NUMBER,PL_INT,PL_INT}},
 { F_CONST|F_PLISTE,    "REPLACE$"  , f_replaces ,3,3   ,{PL_STRING,PL_STRING,PL_STRING}},
 { F_CONST|F_SQUICK,    "REVERSE$"  , f_reverses ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE,    "RIGHT$"  , f_rights ,1,2   ,{PL_STRING,PL_INT}},
 { F_CONST|F_PLISTE,    "RIGHTOF$" , f_rightofs ,2,2   ,{PL_STRING,PL_STRING}},
 { F_CONST|F_SQUICK,    "RLD$"  , f_rlds ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "RLE$"  , f_rles ,1,1   ,{PL_STRING}},

 { F_CONST|F_PLISTE,  "SIGN$", f_signs ,2,3   ,{PL_STRING,PL_STRING,PL_INT}},
/* bei STRING$ und SPACE$ machen wir kein F_CONST*/
 { F_IQUICK,    "SPACE$"  , f_spaces ,1,1   ,{PL_INT}},
 { F_CONST|F_PLISTE,  "STR$"    , f_strs ,1,4   ,{PL_NUMBER,PL_INT,PL_INT,PL_INT}},
/* bei STRING$ und SPACE$ machen wir kein F_CONST*/
 { F_PLISTE,  "STRING$" , f_strings ,2,2   ,{PL_INT,PL_STRING}},
 { F_SQUICK,    "SYSTEM$"    , f_systems ,1,1   ,{PL_STRING}},
 { F_PLISTE,    "TERMINALNAME$"    , f_terminalnames ,1,1 ,{PL_FILENR}},
#ifdef TINE
 { F_SQUICK,    "TINEGET$"    , f_tinegets ,1,1   ,{PL_STRING}},
 { F_SQUICK,    "TINEINFO$"   , f_tineinfos ,1,1   ,{PL_STRING}},
 { F_PLISTE,    "TINEQUERY$"  , f_tinequerys ,2,2   ,{PL_STRING,PL_INT}},
 { F_SQUICK,    "TINEUNIT$"   , f_tineunits ,1,1   ,{PL_STRING}},
#endif
 { F_CONST|F_SQUICK,    "TRIM$"   , f_trims ,1,1   ,{PL_STRING}},

 { F_CONST|F_SQUICK,    "UCASE$"    , f_uppers ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "UNCOMPRESS$" , f_uncompresss ,1,1   ,{PL_STRING}},
 { F_CONST|F_IQUICK,    "UNIXDATE$" , f_unixdates ,1,1   ,{PL_INT}},
 { F_CONST|F_IQUICK,    "UNIXTIME$" , f_unixtimes ,1,1   ,{PL_INT}},
 { F_CONST|F_SQUICK,    "UPPER$"    , f_uppers ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE,    "USING$"    , f_usings ,2,2   ,{PL_NUMBER,PL_STRING}},
 { F_CONST|F_PLISTE,    "WORD$"    , f_words ,2,3   ,{PL_STRING,PL_INT,PL_STRING}},
 { F_CONST|F_SQUICK,    "XTRIM$"   , f_xtrims ,1,1   ,{PL_STRING}}
};
const int anzpsfuncs=sizeof(psfuncs)/sizeof(SFUNCTION);
