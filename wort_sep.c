/* This file is part of X11BASIC, the basic interpreter for Unix
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */



/* Diese Funktion gibt zurueck
   0 Wenn der String leer ist
     w1$=w2$=""
   1 wenn der String t$ nur aus einem Wort
     besteht. w2$="" w1$=t$
   2 wenn der String t$ aus mehreren Worten
     besteht. w1$=erstes Wort
     w2$= Rest von t$
  ##########################################
  der String in t wird nicht veraendert,
  w1 und w2 sind Stringbuffer, die mindestens so gross wie t sein muessen
  
   Befindet sich ein Teil des Strings in 
   AnfÅhrungszeichen ("), so wird dieser Teil
   nicht berÅcksichtigt.
   wird klam! mit True angegeben, wird
   auch der Inhalt von Klammern () nicht berÅcksichtigt

   Die Wîrter werden als solche erkannt, wenn
   sie durch das Zeichen c getrennt sind.

   wort_sep2 hat einen string als separator
   wort_sepr und wort_sepr2 suchen rueckwaerts

   (c) Markus Hoffmann 1997


  */
#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include "wort_sep.h" 

int wort_sep(char *t,char c,int klamb ,char *w1, char *w2)    {
  int f=0, klam=0;

  /* klamb=0 : keine Klammern werden beruecksichtigt
     klamb=1 : normale Klammern werden beruecksichtigt
     klamb=2 : eckige Klammern werden beruecksichtigt
     klamb=4 : geschweifte Klammern werden beruecksichtigt
  */

  if(!(*t)) return(*w1=*w2=0);

  while(*t && (*t!=c || f || klam>0)) {
    if(*t=='"') f=!f;
    else if(!f && (((klamb&1) && *t=='(') || ((klamb&2) && *t=='[') || ((klamb&4) && *t=='{'))) klam++;
    else if(!f && (((klamb&1) && *t==')') || ((klamb&2) && *t==']') || ((klamb&4) && *t=='}'))) klam--;
    *w1++=*t++;
  }
  if(!(*t)) {
    *w2=*w1=0;
    return(1);
  } else {
    *w1=0;
    strcpy(w2,++t);
    return(2);
  }
}
int wort_sep_destroy(char *t,char c,int klamb ,char **w1, char **w2)    {
  int f=0, klam=0;

  /* klamb=0 : keine Klammern werden beruecksichtigt
     klamb=1 : normale Klammern werden beruecksichtigt
     klamb=2 : eckige Klammern werden beruecksichtigt
     klamb=4 : geschweifte Klammern werden beruecksichtigt
  */

  if(!(*t)) {*w1=*w2=t;return(0);}
  *w1=t;
  while(*t && (*t!=c || f || klam>0)) {
    if(*t=='"') f=!f;
    else if(!f && (((klamb&1) && *t=='(') || ((klamb&2) && *t=='[') || ((klamb&4) && *t=='{'))) klam++;
    else if(!f && (((klamb&1) && *t==')') || ((klamb&2) && *t==']') || ((klamb&4) && *t=='}'))) klam--;
    t++;
  }
  if(!(*t)) {
    *w2=t;
    return(1);
  } else {
    *t++=0;
    *w2=t;
    return(2);
  }
}


/* Dasselbe fuer eine Liste von n Zeichen */
int wort_sep_multi(char *t,char *c, int klamb ,char *w1, char *w2)    {
  int f=0, klam=0;

  if(!(*t)) return(*w1=*w2=0);

  while(*t && ((strchr(c,*t)==NULL) || f || klam>0)) {
    if(*t=='"') f=!f;
    else if(!f && (((klamb&1) && *t=='(') || ((klamb&2) && *t=='[') || ((klamb&4) && *t=='{'))) klam++;
    else if(!f && (((klamb&1) && *t==')') || ((klamb&2) && *t==']') || ((klamb&4) && *t=='}'))) klam--;
    *w1++=*t++;
  }
  if(!(*t)) {
    *w2=*w1=0;
    return(1);
  } else {
    *w1=0;
    strcpy(w2,++t);
    return(2);
  }
}




/* Spezielle Abwandlung zum erkennen von Exponentialformat */

static int is_operator(char c) {
  return(!(strchr("|&~!*/+-<>^ =",c)==NULL));
}

int wort_sep_e(char *t,char c,int klamb ,char *w1, char *w2)    {
  int f=0, klam=0,i=0,skip=0,zahl=0;

  if(!(*t)) return(w1[0]=w2[0]=0); 
#if 0
  printf("wsep: <%s>\n",t);
  printf("i=%d t[i]=%d zahl=%d, skip=%d\n",i,t[i],zahl,skip); 
#endif
  while(*t && (*t!=c || f || klam>0 || skip)) {
    skip=0;
    if(*t=='"') f=!f;
    else if(!f && (((klamb&1) && *t=='(') || ((klamb&2) && *t=='[') || ((klamb&4) && *t=='{'))) klam++;
    else if(!f && (((klamb&1) && *t==')') || ((klamb&2) && *t==']') || ((klamb&4) && *t=='}'))) klam--;
    else if(*t=='E' && klam==0 && !f && zahl) skip=1;
    if(i==0 || is_operator(*(t-1))) zahl=1;
    zahl=(!(strchr("-.1234567890",*t)==NULL) && zahl); 
    *w1++=*t++;
    i++;
#if 0
    printf("i=%d t[i]=%d zahl=%d, skip=%d\n",i,t[i],zahl,skip); 
#endif
  }
  if(!*t) {
    *w2=*w1=0;
    return(1);
  } else {
    *w1=0;
    strcpy(w2,t+1);
    return(2);
  }
}

int wort_sepr_e(char *t,char c,int klamb ,char *w1, char *w2)    {
  register int i;
  int f=0, klam=0,j,zahl=0;
  
  if(*t==0)  return(w1[0]=w2[0]=0);

  i=strlen(t)-1;
#if 0
  printf("wsep: <%s>\n",t);
#endif

  while(i>=0) {
    if(t[i]!=c || f || klam<0) {
      if(t[i]=='"') f=!f;
    else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
    else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
      if(i==strlen(t)-1 || is_operator(t[i+1])) zahl=1;
      zahl=(!(strchr("1234567890",t[i])==NULL) && zahl); 
    } else {
#if 0
      printf("Halt bei i=%d   zahl=%d\n",i,zahl);
#endif
      if(!zahl || i<2 || t[i-1]!='E') break;
      j=i-2;
      while(j>=0 && !(strchr("1234567890.",t[j])==NULL)) j--;
      if(j>=0 && !is_operator(t[j])) break;
    }
    i--;
#if 0
    printf("i=%d t[i]=%d zahl=%d\n",i,t[i],zahl); 
#endif
  }
  strcpy(w1,t);
  if(i<0) {*w2=0;return(1);}
  else {w1[i]=0;strcpy(w2,t+i+1);return(2);}
}

int wort_sepr(char *t,char c,int klamb ,char *w1, char *w2)    {
  register int i;
  int f=0, klam=0;
  
  if(!*t) return(*w1=*w2=0);
 
  i=strlen(t)-1;

  while(i>=0 && (t[i]!=c || f || klam<0)) {
    if(t[i]=='"') f=!f;
    else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
    else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
    i--;
  }

  strcpy(w1,t);
  if(i<0) {
    *w2=0;
    return(1);
  } else {
    w1[i]=0;
    strcpy(w2,t+i+1);
    return(2);
  }
}



int wort_sep2(char *t,char *c,int klamb ,char *w1, char *w2)    {

  int f=0, klam=0, i=0, j=0;


  if(!*t) return(*w1=*w2=0);   /* hier gibts nix zu trennen */
  else if(strlen(t)<=strlen(c)) {
    strcpy(w1,t);*w2=0;
    return(1);
  }
  for(;;) {
/* suche erstes Vorkommen von c */
    while(t[i] && (t[i]!=c[0] || f || klam>0)) {
      if(t[i]=='"') f=!f;
    else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
    else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
      w1[j++]=t[i++];
    }

    if(t[i]==0) { /* schon am ende ? */
      *w2=0;
      w1[j]=0;
      return(1);
    } else {     /* ueberpruefe, ob auch der Rest von c vorkommt */
    
      if(strncmp(t+i,c,strlen(c))==0) {
        w1[j]=0;
        strcpy(w2,t+i+strlen(c));
        return(2);
      } else {
        if(t[i]=='"') f=!f;
        else if(t[i]=='(' && klamb && !f) klam++;
        else if(t[i]==')' && klamb && !f) klam--;
        w1[j++]=t[i++];
      }     /* ansonsten weitersuchen */
    }
  }
}



int wort_sepr2(char *t,char *c,int klamb ,char *w1, char *w2)    {
  register int i;
  int f=0, klam=0;


  if(!*t)  return(*w1=*w2=0);  /* hier gibts nix zu trennen */
  else if(strlen(t)<=strlen(c)) {
    strcpy(w1,t);w2[0]=0;
    return(1);
  }
  i=strlen(t)-1;
  for(;;) {
/* suche erstes Vorkommen von c */
    while(i>=0 && (t[i]!=c[strlen(c)-1] || f || klam<0)) {
      if(t[i]=='"') f=!f;
    else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
    else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
    i--;
    }
    
    if(i<0) { /* schon am ende ? */
      strcpy(w1,t);
      *w2=0;
      return(1);
    } else {     /* ueberpruefe, ob auch der Rest von c vorkommt */
    
      if(strncmp(t+i-strlen(c)+1,c,strlen(c))==0) {
        strcpy(w1,t);
	w1[i-strlen(c)+1]=0;
        strcpy(w2,t+i+1);
        return(2);
      } else {
        if(t[i]=='"') f=!f;
        else if(t[i]=='(' && klamb && !f) klam++;
        else if(t[i]==')' && klamb && !f) klam--;
        i--;
      }     /* ansonsten weitersuchen */
    }
  }
}

/* Funktion Trennt w1(w2) string in w1 und w2 auf */ 

int klammer_sep(char *t,char *w1, char *w2)    {
  char *pos,*pos2;
  
  if(!(*t)) return(*w1=*w2=0);
  if((pos=searchchr(t,'('))!=NULL) {
    strncpy(w1,t,pos-t);
    w1[pos-t]=0;
    pos++;
    if((pos2=searchchr2(pos,')'))!=NULL) {
      pos2++;
      if(*pos2) printf("<%s> klammer_sep: Hier war ein Rest: <%s>\n",t,pos2);
    } else printf("WARNING: fehlende schliessende Klammer <%s>\n",t);
    strncpy(w2,pos,pos2-pos);
    w2[pos2-pos-1]=0;
   // printf("w1=<%s>, w2=<%s>\n",w1,w2);
    return(2);
  } else {
    strcpy(w1,t);
    *w2=0;
    return(1);
  }
}
int klammer_sep_destroy(char *t,char **w1, char **w2) {
  char *pos,*pos2;
  
  if(!(*t)) {*w1=*w2=t; return(0);}
  *w1=t;
  
  if((pos=searchchr(t,'('))!=NULL) {
    *pos++=0;
    *w2=pos;
    if((pos2=searchchr2(pos,')'))!=NULL) {
      *pos2++=0;
      if(*pos2) printf("<%s> klammer_sep: Hier war ein Rest: <%s>\n",t,pos2);
    } else printf("WARNING: fehlende schliessende Klammer <%s>\n",t);
    return(2);
  } else {
    *w2=&t[strlen(t)];
    return(1);
  }
}

int arg2(char *t,int klamb ,char *w1, char *w2)    {
  int f=0, klam=0, i=0, j=0,ergeb;
  if(!*t) return(*w1=*w2=0);
  while(t[i] && ((t[i]!=';' && t[i]!=','&& t[i]!='\'') || f || klam!=0)) {
    if(t[i]=='"') f=!f;
    else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
    else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
    w1[j++]=t[i++];
  }

  if(!t[i]) {
    *w2=0;
    w1[j]=0;
    return(1);
  } else {
    if(t[i]==';') ergeb=2;
    else if(t[i]==',') ergeb=3;
    else if(t[i]=='\'') ergeb=4;
    else ergeb=-1;
    w1[j]=0;
    strcpy(w2,t+i+1); 
    return(ergeb);
  }
}

char *searchchr(char *buf, char c) {
 int f=0;
  while(*buf) {
    if(*buf=='"') f= !f;
    if(*buf==c && !f) return(buf);
    buf++;
  }
  return(NULL);
}
char *searchchr2(char *buf, char c) { /*( Auch Klammerungen beruecksichtigen ! */ 
 int f=0,klam=0;
  while(*buf!=0) {
    if(*buf=='"') f= !f;
    else if(*buf=='('  && !f) klam++;
    if(*buf==c && !f && !(klam>0)) return(buf);
    if(*buf==')'  && !f) klam--;
    buf++;
  }
  return(NULL);
}

/*Dasselbe, aber jetzt werden mehrere Zeichen gefunden */

char *searchchr2_multi(char *buf, char *c) { 
 int f=0,klam=0;
  while(*buf!=0) {
    if(*buf=='"') f= !f;
    else if(*buf=='('  && !f) klam++;
    if((strchr(c,*buf)!=NULL) && !f && !(klam>0)) return(buf);
    if(*buf==')'  && !f) klam--;
    buf++;
  }
  return(NULL);
}

char *rsearchchr(char *buf, char c) {
 char *pos;
 int f=0;
 
 pos=buf+strlen(buf)-1;
  while(pos>=buf) {
    if(*pos=='"') f= !f;
    if(*pos==c && !f) return(pos);
    pos--;
  }
  return(NULL);
}

char *rsearchchr2(char *start,char c,char *end) {
  int f=0;
  while((*start!=c || f) && start>=end) {
    if(*start=='\"') f= !f;
    start--;
  }
  return(start);
}

void *memmem(const void *buf,  size_t buflen,const void *pattern, size_t len) {
  size_t i, j;
  char *bf = (char *)buf, *pt = (char *)pattern;

      if(len > buflen) return (void *)NULL;
      for(i = 0; i <= (buflen - len); ++i) {
        for (j = 0; j < len; ++j) {
          if (pt[j] != bf[i + j]) break;
        }
        if (j == len) return (bf + i);
      }
      return NULL;
}

#define min(a,b) ((a<b)?a:b)
char *rmemmem(char *s1,int len1,char *s2,int len2) {
  char *pos=s1+len1;
  while(pos>=s1) {
    if(memcmp(pos,s2,min(len2,s1-pos+len1+1))==0) return(pos);
    pos--;
  }
  return(NULL);
}

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */



/*  (c) Markus Hoffmann
  ' Teile in AnfÅhrungszeichen werden nicht verÑndert
  ' Ersetzt Tabulator durch Leerzeichen
  ' mehrfache Leerzeichen zwischen Wîrtern werden entfernt
  ' wenn fl&=1 ,  gib Groûbuchstaben zurÅck
 */
 
void xtrim(char *t,int f, char *w ) {
  register int a=0,u=0,b=0;

  while(*t) {
    while(*t && (!isspace(*t) || a)) {
      if(*t=='"') a=!a;
      u=1; if(b==1) {*w++=' '; b=0;}
      if(f && !a) *w++=toupper(*t++); else *w++=*t++;
    }
    if(u && !b) b=1;
    if(*t) t++;
  } *w=0;
}

/*Dasselbe, aber entferne auch blanks nach ,;+- und vor ,; */


void xtrim2(char *t,int f, char *w ) {
  register int a=0,u=0,b=0,z=0,z2=0;
  const char solo[]=",;+-*/^'(~@<>=";
  const char solo2[]=";*/^')<>=";
#if 0
char *w2=w;
printf("XTRIM2 <%s> --> ",t);
#endif

  while(*t) {
    while(*t && (!isspace(*t) || a)) {
      if(*t=='"') a=!a;
      u=1; 
      z2=(strchr(solo2,*t)!=NULL);
      if(z2 && b) b=0;
      if(b) {*w++=' '; b=0;}
      z=(strchr(solo,*t)!=NULL);
      if(f && !a) *w++=toupper(*t++); 
      else *w++=*t++;
    }
    if(u && !b) b=1;
    if(b && z) b=0;
    if(*t) t++;
  } 
  *w=0;
#if 0
  printf(" <%s> \n",w2);
#endif
}
