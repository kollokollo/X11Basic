' Zeichnet Praezession auf Kugeloberflaeche (c) Markus Hoffmann 2001
' Draws a precession trace onto a spere
'

bw=600
gucktheta=80/180*PI
guckphi=10/180*PI
SIZEW 1,bw,bw

rot=COLOR_RGB(1,0,0)
gelb=COLOR_RGB(1,0.8,0)
grau=COLOR_RGB(1/1.5,1/1.5,1/1.5)
blau=COLOR_RGB(0,0,1)
weiss=COLOR_RGB(1,1,1)
schwarz=COLOR_RGB(0,0,0)
lila=COLOR_RGB(1,0,1)
gruen=COLOR_RGB(0,1,0)
PAUSE 0.1
SHOWPAGE
PAUSE 0.1
GET_GEOMETRY 1,bx,by,bw,bh
bw=MIN(bw,bh)
bh=bw
COLOR weiss
PBOX bx,by,bw,bh
COLOR schwarz
CIRCLE bw/2,bh/2,MIN(bw,bh)/2-1
COLOR grau

LINE @kx(0,0,0),@ky(0,0,0),@kx(1,0,0),@ky(1,0,0)
LINE @kx(0,0,0),@ky(0,0,0),@kx(1,-pi/2,0),@ky(1,-pi/2,0)
LINE @kx(0,0,0),@ky(0,0,0),@kx(1,pi/2,0),@ky(1,pi/2,0)
LINE @kx(0,0,0),@ky(0,0,0),@kx(1,0,-pi),@ky(1,0,-pi)
LINE @kx(0,0,0),@ky(0,0,0),@kx(1,0,-pi*1.5),@ky(1,0,-pi*1.5)
LINE @kx(0,0,0),@ky(0,0,0),@kx(1,0,-pi*0.5),@ky(1,0,-pi*0.5)
FOR i=0 TO 360 STEP 10
  FOR j=-60 TO -30 STEP 10
    LINE @kx(1,j/180*pi,i/180*pi),@ky(1,j/180*pi,i/180*pi),@kx(1,(j-10)/180*pi,i/180*pi),@ky(1,(j-10)/180*pi,i/180*pi)
    LINE @kx(1,j/180*pi,(i-10)/180*pi),@ky(1,j/180*pi,(i-10)/180*pi),@kx(1,(j)/180*pi,i/180*pi),@ky(1,(j)/180*pi,i/180*pi)
  NEXT j
  SHOWPAGE
NEXT i

DEFLINE ,4
COLOR rot

FOR i=0 TO 360
  j=@f(i)
  ii=i-1
  jj=@f(ii)
  LINE @kx(1,j/180*pi,i/180*pi),@ky(1,j/180*pi,i/180*pi),@kx(1,jj/180*pi,ii/180*pi),@ky(1,jj/180*pi,ii/180*pi)
  SHOWPAGE
NEXT i

COLOR blau
FOR i=0 TO 360 STEP 2
  j=-30
  ii=i-1
  jj=-30
  LINE @kx(1,j/180*pi,i/180*pi),@ky(1,j/180*pi,i/180*pi),@kx(1,jj/180*pi,ii/180*pi),@ky(1,jj/180*pi,ii/180*pi)
  SHOWPAGE
NEXT i
FOR i=0 TO 360 STEP 4
  j=-70
  ii=i-1
  jj=-70
  LINE @kx(1,j/180*pi,i/180*pi),@ky(1,j/180*pi,i/180*pi),@kx(1,jj/180*pi,ii/180*pi),@ky(1,jj/180*pi,ii/180*pi)
  SHOWPAGE
NEXT i
SHOWPAGE
COLOR schwarz

LINE @kx(0,0,0),@ky(0,0,0),@kx(1,-pi/2,0),@ky(1,-pi/2,0)

FOR i=0 TO 360 STEP 10
  LINE @kx(0.9,-85/180*pi,i/180*pi),@ky(0.9,-85/180*pi,i/180*pi),@kx(1,-pi/2,0),@ky(1,-pi/2,0)
NEXT i
SHOWPAGE
PRINT "Press any key."
~INP(-2)
END

DEFFN f(x)=SIN(x/180*PI*6)*20-50

FUNCTION kx(r,theta,phi)
  LOCAL x,y,z
  x=r*COS(theta)*COS(phi)
  y=r*COS(theta)*SIN(phi)
  z=r*SIN(theta)
  x=SIN(guckphi)*x+COS(guckphi)*y
  RETURN bw/2+x*bw/2
ENDFUNC

FUNCTION ky(r,theta,phi)
  LOCAL x,y,z
  x=r*cos(theta)*cos(phi)
  y=r*cos(theta)*sin(phi)
  z=r*sin(theta)
  'xx=sin(guckphi)*x+cos(guckphi)*y
  y=cos(guckphi)*x-sin(guckphi)*y
  'x=xx
  'yy=sin(gucktheta)*y+cos(gucktheta)*z
  z=cos(gucktheta)*y-sin(gucktheta)*z
  RETURN bh/2-z*bh/2
ENDFUNC
