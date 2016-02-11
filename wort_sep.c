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

   2012: memories now may overlap and t==w2 or t==w1 is allowed.


  */
#include <stdio.h>
#include <string.h>
#include <ctype.h> 
#include "wort_sep.h" 

int wort_sep(const char *t,char c,int klamb ,char *w1, char *w2)    {
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
    ++t;
    while(*t) *w2++=*t++;
    *w2=0; 
    return(2);
  }
}

/*This is a faster version of wort_sep, which just returns pointers
  into the memory area pointed by t, which gets modifyed.*/

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
int wort_sep_multi(const char *t,const char *c, int klamb ,char *w1, char *w2)    {
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
    ++t;
    while(*t) *w2++=*t++;
    *w2=0; 
    return(2);
  }
}



int wort_sepr(const char *t,char c,int klamb ,char *w1, char *w2)    {
  register int i;
  int f=0, klam=0;
  
  if(!*t) return(*w1=*w2=0);
 // printf("klamb=%d\n",klamb);
  i=strlen(t)-1;

  while(i>=0 && (t[i]!=c || f || klam<0)) {
    if(t[i]=='"') f=!f;
    else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
    else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
    i--;
  }

  if(i<0) {
    while(*t) *w1++=*t++; 
    *w1=*w2=0;
    return(1);
  } else {
    while(i>0) {
      *w1++=*t++;
      i--;
    }
    *w1=0;
    t++;
    while(*t) *w2++=*t++;
    *w2=0;
    return(2);
  }
}



int wort_sep2(const char *t,const char *c,int klamb ,char *w1, char *w2)    {
  int f=0, klam=0, i=0, j=0;

  if(!*t) return(*w1=*w2=0);   /* hier gibts nix zu trennen */
  if(strlen(t)<=strlen(c)) {
    while(*t) *w1++=*t++; 
    *w1=*w2=0;
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
	i+=strlen(c);
	while(t[i]) *w2++=t[i++];
        *w2=0;
        return(2);
      } else {
        if(t[i]=='"') f=!f;
        else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
        else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
        w1[j++]=t[i++];
      }     /* ansonsten weitersuchen */
    }
  }
}

int wort_sep2_destroy(char *t,const char *c,int klamb ,char **w1, char **w2)    {
  int f=0, klam=0, i=0, j=0;

  *w1=t;
  if(!*t) {*w2=t;return(0);}   /* hier gibts nix zu trennen */
  int strc=strlen(c);
  int strt=strlen(t);

  if(strt<=strc) {
    *w2=t+strt;
    return(1);
  }
  for(;;) {
/* suche erstes Vorkommen von c */
    while(t[i] && (t[i]!=*c || f || klam>0)) {
      if(t[i]=='"') f=!f;
      else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
      else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
      j++;
      i++;
    }

    if(t[i]==0) { /* schon am ende ? */
      *w2=t+i;
      t[j]=0;
      return(1);
    } else {     /* ueberpruefe, ob auch der Rest von c vorkommt */
      if(strncmp(t+i,c,strc)==0) {
        t[j]=0;
	i+=strlen(c);
	*w2=t+i;
        return(2);
      } else {
        if(t[i]=='"') f=!f;
        else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
        else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
        j++;
	i++;
      }     /* ansonsten weitersuchen */
    }
  }
}



int wort_sepr2(const char *t,const char *c,int klamb ,char *w1, char *w2)    {
  register int i;
  int f=0, klam=0;
  int strc,strt;

  if(!*t)  return(*w1=*w2=0);  /* hier gibts nix zu trennen */
  strc=strlen(c);
  strt=strlen(t);
  if(strt<=strc) {
    while(*t) *w1++=*t++; 
    *w1=*w2=0;
    return(1);
  }
  i=strt-1;
  for(;;) {
/* suche erstes Vorkommen von c */
    while(i>=0 && (t[i]!=c[strc-1] || f || klam<0)) {
      if(t[i]=='"') f=!f;
      else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
      else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
      i--;
    }
    
    if(i<0) { /* schon am ende ? */
      while(*t) *w1++=*t++; 
      *w1=*w2=0;
      return(1);
    } else {     /* ueberpruefe, ob auch der Rest von c vorkommt */
    
      if(strncmp(t+i-strc+1,c,strc)==0) {
        strcpy(w1,t);
	w1[i-strc+1]=0;
        strcpy(w2,t+i+1);
        return(2);
      } else {
        if(t[i]=='"') f=!f;
        else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
        else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
        i--;
      }     /* ansonsten weitersuchen */
    }
  }
}
int wort_sepr2_destroy(char *t,const char *c,int klamb ,char **w1, char **w2)    {
  register int i;
  int f=0, klam=0;
  int strc,strt;
  *w1=t;
  if(!*t)  {*w2=t;return(0);}  /* hier gibts nix zu trennen */
  strc=strlen(c);
  strt=strlen(t);
  if(strt<=strc) {
    *w2=t+strt;
    return(1);
  }
  i=strt-1;
  for(;;) {
/* suche erstes Vorkommen von c */
    while(i>=0 && (t[i]!=c[strc-1] || f || klam<0)) {
      if(t[i]=='"') f=!f;
      else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
      else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
      i--;
    }
    
    if(i<0) { /* schon am ende ? */
      *w2=t+strt;
      return(1);
    } else {     /* ueberpruefe, ob auch der Rest von c vorkommt */
      if(strncmp(t+i-strc+1,c,strc)==0) {
	t[i-strc+1]=0;
        *w2=t+i+1;
        return(2);
      } else {
        if(t[i]=='"') f=!f;
        else if(!f && (((klamb&1) && t[i]=='(') || ((klamb&2) && t[i]=='[') || ((klamb&4) && t[i]=='{'))) klam++;
        else if(!f && (((klamb&1) && t[i]==')') || ((klamb&2) && t[i]==']') || ((klamb&4) && t[i]=='}'))) klam--;
        i--;
      }     /* ansonsten weitersuchen */
    }
  }
}

/* Funktion Trennt w1(w2) string in w1 und w2 auf */ 

int klammer_sep(const char *t,char *w1, char *w2)    {
  char *pos,*pos2;
  
  if(!(*t)) return(*w1=*w2=0);
  if((pos=searchchr(t,'('))!=NULL) {
    strncpy(w1,t,pos-t);
    w1[pos-t]=0;
    pos++;
    if((pos2=searchchr2(pos,')'))!=NULL) {
      pos2++;
      if(*pos2) printf("WARNING: Syntax error: expression <%s> is incomplete, rest: <%s>\n",t,pos2);
    } else printf("WARNING: Syntax error: missing closing parenthesis in <%s>.\n",t);
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
      if(*pos2) printf("WARNING: Syntax error: expression <%s> is incomplete, rest: <%s>\n",t,pos2);
    } else printf("WARNING: Syntax error: missing closing parenthesis in <%s>.\n",t);
    return(2);
  } else {
    *w2=&t[strlen(t)];
    return(1);
  }
}
  /*
   2012: memories now may overlap and t==w2 or t==w1 is allowed.
   */
int arg2(const char *t,int klamb ,char *w1, char *w2)    {
  int f=0, klam=0,ergeb;
  if(!*t) return(*w1=*w2=0);
  while(*t && ((*t!=';' && *t!=','&& *t!='\'') || f || klam!=0)) {
    if(*t=='"') f=!f;
    else if(!f && (((klamb&1) && *t=='(') || ((klamb&2) && *t=='[') || ((klamb&4) && *t=='{'))) klam++;
    else if(!f && (((klamb&1) && *t==')') || ((klamb&2) && *t==']') || ((klamb&4) && *t=='}'))) klam--;
    *w1++=*t++;
  }

  if(!*t) { *w1=*w2=0; return(1); } 
  else {
    if(*t==';') ergeb=2;
    else if(*t==',') ergeb=3;
    else if(*t=='\'') ergeb=4;
    else ergeb=-1;
    *w1=0;
    t++;
    while(*t) *w2++=*t++;
    *w2=0;
    return(ergeb);
  }
}

char *searchchr(const char *buf, char c) {
 int f=0;
  while(*buf) {
    if(*buf=='"') f= !f;
    if(!f && *buf==c) return((char *)buf);
    buf++;
  }
  return(NULL);
}
char *searchchr2(const char *buf, char c) { /*( Auch Klammerungen beruecksichtigen ! */ 
 int f=0,klam=0;
  while(*buf!=0) {
    if(*buf=='"') f= !f;
    if(!f) {
      if(*buf=='(') klam++;
      if(*buf==c && !(klam>0)) return((char *)buf);
      if(*buf==')') klam--;
    }
    buf++;
  }
  return(NULL);
}
char *searchchr3(const char *buf, char c) { /* Alle 3  Klammertypen (()[]{}) beruecksichtigen ! */ 
 int f=0,klam=0,klam2=0,klam3=0;;
  while(*buf!=0) {
    if(*buf=='"') f= !f;
    else if(!f) {
      if(*buf=='(') klam++;
      else if(*buf=='{') klam2++;
      else if(*buf=='[') klam3++;
      if(*buf==c && !(klam>0 || klam2>0 || klam3>0)) return((char *)buf);
      if(*buf==')') klam--;
      else if(*buf=='}') klam2--;
      else if(*buf==']') klam3--;
    }
    buf++;
  }
  return(NULL);
}
char *searchchr3_multi(const char *buf, const char *c) { /* Alle 3  Klammertypen (()[]{}) beruecksichtigen ! */ 
 int f=0,klam=0,klam2=0,klam3=0;;
  while(*buf!=0) {
    if(*buf=='"') f= !f;
    else if(!f) {
      if(*buf=='(') klam++;
      else if(*buf=='{') klam2++;
      else if(*buf=='[') klam3++;
      if((strchr(c,*buf)!=NULL) && !(klam>0 || klam2>0 || klam3>0)) return((char *)buf);
      if(*buf==')') klam--;
      else if(*buf=='}') klam2--;
      else if(*buf==']') klam3--;
    }
    buf++;
  }
  return(NULL);
}

/*Dasselbe, aber jetzt werden mehrere Zeichen gefunden */

char *searchchr2_multi(const char *buf, const char *c) { 
 int f=0,klam=0;
  while(*buf!=0) {
    if(*buf=='"') f= !f;
    else if(*buf=='('  && !f) klam++;
    if((strchr(c,*buf)!=NULL) && !f && !(klam>0)) return((char *)buf);
    if(*buf==')'  && !f) klam--;
    buf++;
  }
  return(NULL);
}

/*Dasselbe, aber jetzt werden mehrere Zeichen gefunden von rechts */

char *searchchr2_multi_r(const char *buf, const char *c,const char *pos) { 
 int f=0,klam=0;
  while(--pos>=buf) {
    if(*pos=='"') f= !f;
    else if(*pos=='('  && !f) klam--;
    if((strchr(c,*pos)!=NULL) && !f && !(klam>0)) return((char *)pos);
    if(*pos==')' && !f) klam++;
  }
  return(NULL);
}





char *rsearchchr(const char *buf, char c) {
 char *pos;
 int f=0;
 
 pos=(char *)buf+strlen(buf)-1;
  while(pos>=buf) {
    if(*pos=='"') f= !f;
    if(*pos==c && !f) return(pos);
    pos--;
  }
  return(NULL);
}

char *rsearchchr2(const char *start,char c,const char *end) {
  int f=0;
  while((*start!=c || f) && start>=end) {
    if(*start=='\"') f= !f;
    start--;
  }
  return((char *)start);
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
 
void xtrim(const char *t,int f, char *w ) {
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

/*Dasselbe, aber entferne auch blanks nach ,;+- und vor ,; 

Damit das ganze funktioniert, muss man evtl ein Kommando 
am Anfang beruecksichtigen. Hier muss das Blank bleiben.

Die Blankentfernung wird erst gestartet, sobald eines der folgenden Zeichen aufgetaucht ist:

blank


*/


void xtrim2(const char *t,int f, char *w ) {
  register int a=0,u=0,b=0,z=0,z2=0;
  /*a sagt, ob im String
    b sagt, ob schonam ein blank nach einem space da war
    u sagt, ob schonmal ein nichtspace da war*/
  const char solo[]=",;+-*/^'(~@<>="; /*Potentielle Zeichen, wo ein blank danach entfallen kann*/
  const char solo2[]="*/^)<>="; /*Potentielle Zeichen, wo ein blank davor niemals simm macht*/
  /*evtl. hier das * herausnehmen, da es bei HELP und REM als erstes Zeichen des Arguments auftauchen kann*/
#if 0
char *w2=w;
printf("XTRIM2 <%s> --> ",t);
#endif

  while(*t) {
    while(*t && (!isspace(*t) || a)) {
      if(*t=='"') a=!a;
      u=1;                             /*Erster nicht-space Buchstabe wurde gefunden*/
      z2=(strchr(solo2,*t)!=NULL);     /*davor ein Blank?*/
      if(z2 && b) b=0;                 /*das zurueckgehaltene Blank nicht schreiben*/
      if(b) {*w++=' '; b=0;}           /*es muss noch ein Blank davor*/
      z=(strchr(solo,*t)!=NULL);        
      if(f && !a) *w++=toupper(*t++);  /*Zeichen kopieren*/
      else *w++=*t++;                  /*Zeichen kopieren*/
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
