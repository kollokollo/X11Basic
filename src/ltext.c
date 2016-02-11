/* ltext.c (c) Markus Hoffmann
   Fontdaten einlesen fuer Linienfont char font[zeile][spalte] */

/* This file is part of X11BASIC, the basic interpreter for Unix/X
 * ============================================================
 * X11BASIC is free software and comes with NO WARRANTY - read the file
 * COPYING for details
 */

#include <math.h>
#include <string.h>
#include "fdata.c"

#define FALSE 0
#define TRUE -1
#define rad(x)   (3.141592654*x/180)

extern void line(int x1,int y1,int x2, int y2);

double ltext(int x, int y, double s, double t, double wk, int pflg, char *tt) 
{
   double obxx,bxx=0,msin,mcos;
   unsigned int a;
   int i,j,len,len2,fx,fy,charw,center,px,py,pxo,pyo;
   int xx,yy,ox,oy;
   
  len=strlen(tt);
  if(len>0) {
    msin=sin(rad(wk));
    mcos=cos(rad(wk)); 

    for(i=0;i<len;i++) {
      a=tt[i];
      if(a=='ß') a=16;
      else if(a=='Ä') a=17;
      else if(a=='ä') a=18;
      else if(a=='Ö') a=19;
      else if(a=='ö') a=20;
      else if(a=='Ü') a=21;
      else if(a=='ü') a=22;
      else a&=0x7f;
      fy=bxx*msin+y;
      fx=bxx*mcos+x;
      len2=strlen(font[a]);
      
      charw=(int)font[a][0];
      
      if(pflg==FALSE) {
        center=(100-charw)/2;
        charw=100;
      } else center=0;
      if(len2>1) {
        for(j=1;j<len2;j++) {
          xx=font[a][j++];
          yy=font[a][j];


          if(xx>100) xx=xx-101+center;
          else {
            xx+=center;
            if (j>2) {
              pxo=ox*s*mcos-oy*t*msin;
              pyo=oy*t*mcos+ox*s*msin;
              px=xx*s*mcos-yy*t*msin;
              py=yy*t*mcos+xx*s*msin;
	      line(pxo+fx,pyo+fy,px+fx,py+fy); 
            }
          }
          ox=xx; oy=yy;
        }
      }
      obxx=bxx;
      bxx=bxx+(charw+30)*s;
    }
  }
  return(bxx);
}
double ltextlen (double s, int pflg, char *tt) {
  int a,i,len,len2,charw=100,bxx=0;
  
  len=strlen(tt);
  if(len>0) {
    for(i=0;i<len;i++) {
      a=(int)tt[i];
      if(a=='ß') a=16;
      else if(a=='Ä') a=17;
      else if(a=='ä') a=18;
      else if(a=='Ö') a=19;
      else if(a=='ö') a=20;
      else if(a=='Ü') a=21;
      else if(a=='ü') a=22;
      else a&=0x7f;
      len2=strlen(font[a]);
      if(len2>1) { 
        if(pflg) charw=(int)font[a][0];
      } 
      bxx+=(charw+30);
    }
  }
  return(bxx*s);
}
