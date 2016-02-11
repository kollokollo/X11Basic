/* sfunctions.c   String functions (c) Markus Hoffmann*/

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>

#include "defs.h"
#include "x11basic.h"
#include "variablen.h"
#include "xbasic.h"
#include "functions.h"
#include "sfunctions.h"
#include "parser.h"
#include "parameter.h"
#include "array.h"
#include "io.h"
#include "wort_sep.h"

#include "decode.h"



static STRING f_errs(int n)    { return(create_string((char *)error_text((char)n,NULL))); }
static STRING f_envs(STRING n) { return(create_string(getenv(n.pointer)));}


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
  return(ergebnis);
}

static STRING f_uppers(STRING n) {   
  int i=0;
  STRING ergebnis;
  ergebnis.pointer=malloc(n.len+1);
  ergebnis.len=n.len;
  while(i<n.len) {ergebnis.pointer[i]=toupper(n.pointer[i]); i++;}
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
  FILE *fff=get_fileptr(plist->integer);
  if(fff) ergebnis.pointer=terminalname(fileno(fff));
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
  loctim=localtime((time_t *)(&n));
  ergebnis.pointer=malloc(12);
  sprintf(ergebnis.pointer,"%02d.%02d.%04d",loctim->tm_mday,loctim->tm_mon+1,1900+loctim->tm_year);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
static STRING f_unixtimes(int n) {
  STRING ergebnis;
  struct tm * loctim;
  loctim=localtime((time_t *)&n);
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
  sprintf(ergebnis.pointer,formatter,plist[0].real);
  ergebnis.len=strlen(ergebnis.pointer);
  return(ergebnis);
}
static STRING f_bins(PARAMETER *plist,int e) {
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
static STRING hexoct_to_string(char n,PARAMETER *plist, int e) {
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

static STRING f_hexs(PARAMETER *plist, int e) {return(hexoct_to_string('x',plist,e));}
static STRING f_octs(PARAMETER *plist, int e) {return(hexoct_to_string('o',plist,e));}


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
  ergebnis.pointer=malloc(n+1);
  ergebnis.len=n;
  while(i<n) ergebnis.pointer[i++]=' ';
  return(ergebnis);
}

static STRING f_lefts(PARAMETER *plist,int e) {
  STRING ergebnis;
  if(e>=1) {
    ergebnis.len=plist[0].integer;
    ergebnis.pointer=malloc(ergebnis.len+1);
    memcpy(ergebnis.pointer,plist[0].pointer,ergebnis.len);
    if(e<2) ergebnis.len=1;
    else ergebnis.len=min(max(plist[1].integer,0),ergebnis.len);
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
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
  STRING buffer,ergebnis;
  int j;
  if(e>=1) {
    buffer.len=plist[0].integer;
    buffer.pointer=plist[0].pointer;
    if(e<2) j=1;
    else j=min(max(plist[1].integer,0),buffer.len); 
    ergebnis.pointer=malloc(j+1);
    ergebnis.len=j;
    memcpy(ergebnis.pointer,buffer.pointer+buffer.len-j,j);
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  return(ergebnis);
}


static STRING f_mids(PARAMETER *plist,int e) {  
  STRING buffer,ergebnis;
  int p,l;
  if(e>=2) {
    buffer.len=plist->integer;
    buffer.pointer=plist->pointer;
    p=min(max(plist[1].integer,1),buffer.len);
    if(e<3) l=1;
    else l=min(max(plist[2].integer,0),buffer.len-p+1); 
    ergebnis.pointer=malloc(l+1);
    ergebnis.len=l;
    memcpy(ergebnis.pointer,buffer.pointer+p-1,l);  
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
  return(ergebnis);
}

static STRING f_strings(PARAMETER *plist,int e) {
  STRING buffer,ergebnis;
  int i=0,j;
  if(e>=2) {
    j=plist->integer;
    buffer.len=plist[1].integer;
    buffer.pointer=plist[1].pointer;
    ergebnis.pointer=malloc(j*buffer.len+1);
    ergebnis.len=j*buffer.len;
    while(i<j) {memcpy(ergebnis.pointer+i*buffer.len,buffer.pointer,buffer.len); i++;}
  } else {
    ergebnis.pointer=malloc(1);
    ergebnis.len=0;
  }
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
  ergebnis.pointer=malloc(sizeof(long)+1);
  ergebnis.len=sizeof(long);
  *((long *)ergebnis.pointer)=(long)n;
  *(ergebnis.pointer+sizeof(long))=0;
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

static STRING f_inlines(STRING n) {   
  STRING ergebnis;
  char *pos1=n.pointer;
  char *pos2;
  ergebnis.len=n.len*3/4;
  pos2=ergebnis.pointer=malloc(ergebnis.len+1);
  while(pos2-ergebnis.pointer<ergebnis.len-2) {
  *pos2=(((pos1[0]-36) & 0x3f)<<2)|(((pos1[1]-36) & 0x30)>>4);
    pos2[1]=(((pos1[1]-36) & 0xf)<<4)|(((pos1[2]-36) & 0x3c)>>2);
    pos2[2]=(((pos1[2]-36) & 0x3)<<6)|(((pos1[3]-36) & 0x3f));
    pos2+=3;
    pos1+=4;
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
  int primary_index;
  int size;   
  if(n.len>=sizeof(int)) {
    size=n.len-sizeof(int);
    primary_index=*((int *)n.pointer);
  } else {
    size=0;
    primary_index=0;
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
   
    for(i=0,j=0;i<256;i++) {
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



static STRING f_encrypts(PARAMETER *plist,int e) {
  STRING ergebnis;
  int len=plist[0].integer>>3;
#ifdef USE_BLOWFISH 
  int i;  
  unsigned long *L,*R;
#endif
  if(plist[0].integer&7) len++;
  len<<=3;
  ergebnis.pointer=malloc(len+1);
  ergebnis.len=len;
  memcpy(ergebnis.pointer,plist[0].pointer,plist[0].integer);
#ifdef USE_BLOWFISH 
  if(e==2) {
    BLOWFISH_CTX ctx;
    Blowfish_Init(&ctx, (unsigned char*)plist[1].pointer,plist[1].integer);
    len>>=3;
    for(i=0;i<len;i++) {
      L=(unsigned long *)&(ergebnis.pointer[i*8]);
      R=(unsigned long *)&(ergebnis.pointer[i*8+4]);
      Blowfish_Encrypt(&ctx,L,R);
    }
  } 
#else
#ifdef HAVE_MEMFROB
  memfrob(ergebnis.pointer,len);  
#endif
#endif  
  return(ergebnis);
}
static STRING f_decrypts(PARAMETER *plist,int e) {
  STRING ergebnis;
  int len=plist[0].integer>>3;
#ifdef USE_BLOWFISH
  int i;  
  unsigned long *L,*R;
#endif
  if(plist[0].integer&7) len++;
  len<<=3;
  ergebnis.pointer=malloc(len+1);
  ergebnis.len=len;
  memcpy(ergebnis.pointer,plist[0].pointer,plist[0].integer);
#ifdef USE_BLOWFISH 
  if(e==2) {
    BLOWFISH_CTX ctx;
    Blowfish_Init(&ctx, (unsigned char*)plist[1].pointer,plist[1].integer);
    len>>=3;
    for(i=0;i<len;i++) {
      L=(unsigned long *)&(ergebnis.pointer[i*8]);
      R=(unsigned long *)&(ergebnis.pointer[i*8+4]);
      Blowfish_Decrypt(&ctx,L,R);
    }
  } 
#else
 #ifdef HAVE_MEMFROB 
   memfrob(ergebnis.pointer,len);
 #endif 
#endif  
  return(ergebnis);
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


const SFUNCTION psfuncs[]= {  /* alphabetisch !!! */

 { F_CONST|F_ARGUMENT,  "!nulldummy", (sfunc)f_nop ,0,0   ,{0}},
 { F_CONST|F_SQUICK,    "ARID$"     , f_arids ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "ARIE$"     , f_aries ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE,    "BIN$"      , f_bins ,1,2   ,{PL_INT,PL_INT}},
 { F_CONST|F_SQUICK,    "BWTD$"     , f_bwtds ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "BWTE$"     , f_bwtes ,1,1   ,{PL_STRING}},

 { F_CONST|F_IQUICK,    "CHR$"      , f_chrs ,1,1   ,{PL_INT}},
 { F_CONST|F_SQUICK,    "COMPRESS$" , f_compresss ,1,1   ,{PL_STRING}},
#ifdef CONTROL
 { F_SQUICK,    "CSGET$"    , f_csgets ,1,1   ,{PL_STRING}},
 { F_IQUICK,    "CSPNAME$"  , f_cspnames ,1,1   ,{PL_INT}},
 { F_SQUICK,    "CSUNIT$"   , f_csunits ,1,1   ,{PL_STRING}},
#endif
 { F_CONST|F_SQUICK,    "DECLOSE$", f_decloses ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE,    "DECRYPT$", f_decrypts ,2,2   ,{PL_STRING,PL_STRING}},
#ifdef DOOCS
 { F_SQUICK,    "DOOCSGET$"    , f_doocsgets ,1,1   ,{PL_STRING}},
 { F_SQUICK,    "DOOCSINFO$"    , f_doocsinfos ,1,1   ,{PL_STRING}},
#endif

 { F_CONST|F_PLISTE,    "ENCLOSE$" , f_encloses ,1,2   ,{PL_STRING,PL_STRING}},
 { F_CONST|F_PLISTE,    "ENCRYPT$", f_encrypts ,2,2   ,{PL_STRING,PL_STRING}},
 { F_SQUICK,    "ENV$"    , f_envs ,1,1   ,{PL_STRING}},
 { F_CONST|F_IQUICK,    "ERR$"    , f_errs ,1,1   ,{PL_NUMBER}},
 { F_CONST|F_PLISTE,  "HEX$"    , f_hexs ,1,4   ,{PL_INT,PL_INT,PL_INT,PL_INT}},
 { F_CONST|F_SQUICK,    "INLINE$" , f_inlines ,1,1   ,{PL_STRING}},
 { F_ARGUMENT,  "INPUT$"  , f_inputs ,1,2   ,{PL_FILENR,PL_INT}},
 { F_CONST|F_IQUICK,    "JULDATE$" , f_juldates ,1,1   ,{PL_INT}},

 { F_CONST|F_PLISTE,    "LEFT$" , f_lefts ,1,2   ,{PL_STRING,PL_INT}},
 { F_CONST|F_PLISTE,    "LEFTOF$" , f_leftofs ,2,2   ,{PL_STRING,PL_STRING}},
 { F_PLISTE,    "LINEINPUT$" , f_lineinputs ,1,1   ,{PL_FILENR}},
 { F_CONST|F_SQUICK,    "LOWER$"    , f_lowers ,1,1   ,{PL_STRING}},

 { F_CONST|F_PLISTE,    "MID$"    , f_mids ,2,3   ,{PL_STRING,PL_INT,PL_INT}},
 { F_CONST|F_AQUICK,    "MKA$"    , array_to_string ,1,1   ,{PL_ARRAY}},
 { F_CONST|F_DQUICK,    "MKD$"    , f_mkds ,1,1   ,{PL_NUMBER}},
 { F_CONST|F_DQUICK,    "MKF$"    , f_mkfs ,1,1   ,{PL_NUMBER}},
 { F_CONST|F_IQUICK,    "MKI$"    , f_mkis ,1,1   ,{PL_INT}},
 { F_CONST|F_IQUICK,    "MKL$"    , f_mkls ,1,1   ,{PL_INT}},
 { F_CONST|F_DQUICK,    "MKS$"    , f_mkfs ,1,1   ,{PL_NUMBER}},
 { F_CONST|F_SQUICK,    "MTFD$"  , f_mtfds ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "MTFE$"  , f_mtfes ,1,1   ,{PL_STRING}},

 { F_CONST|F_PLISTE,  "OCT$"    , f_octs ,1,4   ,{PL_INT,PL_INT,PL_INT,PL_INT}},

 { F_IQUICK,    "PARAM$"  , f_params ,1,1   ,{PL_INT}},
 { F_CONST|F_IQUICK,    "PRG$"    , f_prgs ,1,1   ,{PL_INT}},
 { F_CONST|F_PLISTE,    "REPLACE$"  , f_replaces ,3,3   ,{PL_STRING,PL_STRING,PL_STRING}},
 { F_CONST|F_SQUICK,    "REVERSE$"  , f_reverses ,1,1   ,{PL_STRING}},
 { F_CONST|F_PLISTE,    "RIGHT$"  , f_rights ,1,2   ,{PL_STRING,PL_INT}},
 { F_CONST|F_PLISTE,    "RIGHTOF$" , f_rightofs ,2,2   ,{PL_STRING,PL_STRING}},
 { F_CONST|F_SQUICK,    "RLD$"  , f_rlds ,1,1   ,{PL_STRING}},
 { F_CONST|F_SQUICK,    "RLE$"  , f_rles ,1,1   ,{PL_STRING}},

 { F_CONST|F_IQUICK,    "SPACE$"  , f_spaces ,1,1   ,{PL_INT}},
 { F_CONST|F_PLISTE,  "STR$"    , f_strs ,1,4   ,{PL_NUMBER,PL_INT,PL_INT,PL_INT}},
 { F_CONST|F_PLISTE,  "STRING$" , f_strings ,2,2   ,{PL_INT,PL_STRING}},
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
 { F_CONST|F_PLISTE,    "WORD$"    , f_words ,2,3   ,{PL_STRING,PL_INT,PL_STRING}},
 { F_CONST|F_SQUICK,    "XTRIM$"   , f_xtrims ,1,1   ,{PL_STRING}}
};
const int anzpsfuncs=sizeof(psfuncs)/sizeof(SFUNCTION);

