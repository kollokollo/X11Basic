' Replicator, Edward Fredkin, Serafino Amoroso, Gerald Cooper
' implemented by Markus Hoffmann 2012

nc=4  ! Number of colors
s=2   ! size of the pixel
sw=48
sh=48
bw=3*sw
bh=3*sh
DIM col(nc)

SIZEW ,s*bw,s*bh+16

col(0)=GET_COLOR(0,0,0)
col(1)=GET_COLOR(65535,0,0)
col(2)=GET_COLOR(0,65535,0)

col(nc-1)=GET_COLOR(65535,65535,65535)
COLOR col(0)
PBOX 0,0,sw,sh
COLOR col(nc-1)
' text 10,10,"Hallo"
TEXT bw*s/2,bh*s+12,"replicator with X11-Basic"
COLOR col(1)
' text 10,20,"yes"
' box 5,5,sw-1-5,sh-1-5
TEXT sw*s/2,sh*s+16,"initializing"
PLOT sw/2,sh/2
COLOR col(2)
BOX 0,0,sw-1,sh-1
'  plot sw/2,sh/2
SHOWPAGE
PAUSE 1
DIM feld(bw,bh),ofeld(bw,bh)
ARRAYFILL feld(),0

FOR x=0 TO sw-1
  FOR y=0 TO sh-1
    ' print point(x,y)
    FOR i=0 TO nc-1
      IF point(x,y)=col(i)
        feld(sw+x,sh+y)=i
      ENDIF
    NEXT i
  NEXT y
NEXT x
DO
  @showit
  @calcit
  INC count
LOOP
QUIT

PROCEDURE calcit
  FOR x=1 TO bw-2
    FOR y=1 TO bh-2
      sum=ofeld(x-1,y)+ofeld(x-1,y-1)+ofeld(x,y-1)+ofeld(x+1,y-1)+ofeld(x+1,y)+ofeld(x+1,y+1)+ofeld(x,y+1)+ofeld(x-1,y+1)
      feld(x,y)=(sum MOD nc)
    NEXT y
  NEXT x
RETURN

PROCEDURE showit
  COLOR col(0)
  PBOX 0,0,bw*s,bh*s
  FOR x=0 TO bw-1
    FOR y=0 TO bh-1
      COLOR col(feld(x,y))
      PBOX x*s,y*s,(x+1)*s-1,(y+1)*s-1
      ofeld(x,y)=feld(x,y)
    NEXT y
  NEXT x
  COLOR col(nc-1)
  TEXT 20,s*bh+12,STR$(count)
  SHOWPAGE
RETURN

