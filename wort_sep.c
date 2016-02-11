/* This file is part of X11BASIC, the basic interpreter for Unix/X
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
#include <string.h>
  
int wort_sep (char *t,char c,int klamb ,char *w1, char *w2)    {
  int f=0, klam=0, i=0, j=0;

  if(strlen(t)==0) {
     w1[0]=0;
     w2[0]=0;
    return(0);
  }

  while(t[i]!=0 && (t[i]!=c || f || klam>0)) {
    if(t[i]=='"') f=!f;
    else if(t[i]=='(' && klamb && !f) klam++;
    else if(t[i]==')' && klamb && !f) klam--;
    w1[j++]=t[i++];
  }


  if(t[i]==0) {
    w2[0]=0;
    w1[j]=0;
    return(1);
  } else {
    w1[j]=0;
    strcpy(w2,t+i+1);
    return(2);
  }
}

int wort_sepr(char *t,char c,int klamb ,char *w1, char *w2)    {
  int f=0, klam=0, i=0, j=0;
  
  if(strlen(t)==0) {
     w1[0]=0;
     w2[0]=0;
    return(0);
  }

  i=strlen(t)-1;

  while(i>=0 && (t[i]!=c || f || klam<0)) {
    if(t[i]=='"') f=!f;
    else if(t[i]=='(' && klamb && !f) klam++;
    else if(t[i]==')' && klamb && !f) klam--;
    i--;
  }

  strcpy(w1,t);
  if(i<0) {
    w2[0]=0;
    return(1);
  } else {
    w1[i]=0;
    strcpy(w2,t+i+1);
    return(2);
  }
}



int wort_sep2(char *t,char *c,int klamb ,char *w1, char *w2)    {

  int f=0, klam=0, i=0, j=0;


  if(strlen(t)==0) {   /* hier gibts nix zu trennen */
     w1[0]=0;
     w2[0]=0;
    return(0);
  } else if(strlen(t)<=strlen(c)) {
    strcpy(w1,t);w2[0]=0;
    return(1);
  }
  for(;;) {
/* suche erstes Vorkommen von c */
    while(t[i]!=0 && (t[i]!=c[0] || f || klam>0)) {
      if(t[i]=='"') f=!f;
      else if(t[i]=='(' && klamb && !f) klam++;
      else if(t[i]==')' && klamb && !f) klam--;
      w1[j++]=t[i++];
    }

    if(t[i]==0) { /* schon am ende ? */
      w2[0]=0;
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

  int f=0, klam=0, i=0, j=0;


  if(strlen(t)==0) {   /* hier gibts nix zu trennen */
     w1[0]=0;
     w2[0]=0;
    return(0);
  } else if(strlen(t)<=strlen(c)) {
    strcpy(w1,t);w2[0]=0;
    return(1);
  }
  i=strlen(t)-1;
  for(;;) {
/* suche erstes Vorkommen von c */
    while(i>=0 && (t[i]!=c[strlen(c)-1] || f || klam<0)) {
      if(t[i]=='"') f=!f;
      else if(t[i]=='(' && klamb && !f) klam++;
      else if(t[i]==')' && klamb && !f) klam--;
      i--;
    }
    
    if(i<0) { /* schon am ende ? */
      strcpy(w1,t);
      w2[0]=0;
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



int arg2(char *t,int klamb ,char *w1, char *w2)    {

  int f=0, klam=0, i=0, j=0,ergeb;

  /*printf("A2 <%s> \n",t);*/
  if(strlen(t)==0) {
     w1[0]=0;
     w2[0]=0;
    return(0);
  }

  while(t[i]!=0 && ((t[i]!=';' && t[i]!=','&& t[i]!='\'') || f || klam!=0)) {
    if(t[i]=='"') f=!f;
    else if(t[i]=='(' && klamb && !f) klam++;
    else if(t[i]==')' && klamb && !f) klam--;
    w1[j++]=t[i++];
  }

  if(t[i]==0) {
    w2[0]=0;
    w1[j]=0;
    return(1);
  } else {
    if(t[i]==';') ergeb=2;
    else if(t[i]==',') ergeb=3;
    else if(t[i]=='\'') ergeb=4;
    else {printf("arg2: Fehler: %d %d <%s>\n",t[i],i,t); ergeb=-1;}
    w1[j]=0;
    strcpy(w2,t+i+1); 
    return(ergeb);
  }
}

char *searchchr(char *buf, char c) {
 
 int f=0;
  while(buf[0]!=0) {
    if(buf[0]=='"') f= !f;
    if(buf[0]==c && !f) return(buf);
    buf++;
  }
  return(NULL);
}
char *rsearchchr(char *buf, char c) {
 char *pos;
 int f=0;
 
 pos=buf+strlen(buf)-1;
  while(pos>=buf) {
    if(pos[0]=='"') f= !f;
    if(pos[0]==c && !f) return(pos);
    pos--;
  }
  return(NULL);
}

char *rsearchchr2(char *start,char c,char *end) {
  int f=0;
  while((start[0]!=c || f) && start>=end) {
    if(start[0]=='\"') f= !f;
    start--;
  }
  return(start);
}
