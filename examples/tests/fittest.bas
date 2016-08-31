' This example program demonstrates the data fitting
' functions of X11-Basic  (c) Markus Hoffmann 2003-2013
'

' Fill the data arrays with 400 values. With the mouse you can modify
' the distribution a bit.
'
' The data values x() and y() should be small, such that high potences
' of them will not overflow.
'
n=400
DIM x(n),y(n),e(n),p(4)
FOR i=0 TO n-1
  x(i)=(i+RANDOM(10))/400
  y(i)=(1*i+GASDEV(1)*20+50-i/30*i/30+(400-MOUSEY))/400
  e(i)=1
  PLOT x(i)*400,400-y(i)*400
  SHOWPAGE
  PAUSE 0.01
NEXT i

' First do a linear fit
COLOR COLOR_RGB(1,1,1)
FIT_LINEAR x(),y(),,a,b,da,db,chi
PRINT a;"+/-";da
PRINT b;"+/-";db
PRINT "chi2=";chi
PRINT "q=";q
da=da*sqrt(chi/398)
db=db*sqrt(chi/398)
LINE 0,400-a*400,400,400-(a+b)*400
COLOR color_rgb(0.5,0.5,0.5)
LINE 0,400-(a+da)*400,400,400-(a+da+b)*400
LINE 0,400-(a-da)*400,400,400-(a-da+b)*400
LINE 0,400-(a)*400,400,400-(a+db+b)*400
LINE 0,400-(a)*400,400,400-(a-db+b)*400

VSYNC
PAUSE 5
PRINT
' And now do several polynomial fits with increasing order
FOR oo=3 TO 500
  @fii(oo)
  IF oo>10
    ADD oo,5
  ENDIF
  IF oo>100
    ADD oo,50
  ENDIF
NEXT oo
~inp(-2)
QUIT

PROCEDURE fii(oo)
  DIM p(oo)
  CLR ox,oy
  COLOR color_rgb(0,1,oo/10)
  TEXT 300,30,"Order: "+STR$(oo)+"  "
  FIT_POLY x(),y(),e(),400,p(),oo
  FOR i=0 TO oo-1
    IF p(i)>0.0005
      PRINT i,p(i)
    ENDIF
  NEXT i
  FOR x=0 TO 400
    y=0
    FOR i=0 TO oo-1
      ADD y,p(i)*(x/400)^i
    NEXT i
    LINE ox,400-oy*400,x,400-y*400
    ox=x
    oy=y
  NEXT x
  SHOWPAGE
  PAUSE 0.3
  PRINT
RETURN
