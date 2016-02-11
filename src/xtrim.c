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

  int a=0,i=0,j=0,u=0,b=0;

  while(t[i]!=0) {
    while(t[i]!=0 && ( !isspace(t[i]) || a)) {
      if(t[i]=='"') a=!a;
      u=1; if(b==1) {w[j++]=' '; b=0;}
      if(f && !a) w[j++]=toupper(t[i++]); else w[j++]=t[i++];
    }
    if(u && !b) { b=1;}
    if(t[i]!=0) i++;
  }
  w[j]=0;
}
