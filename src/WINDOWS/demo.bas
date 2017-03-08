##                                               rot.bas / demo.bas
## Demonstration Program for the X11-Basic Package on WINDOWS
##
## A Spin arrow rotates around the directon of a magnetic field arrow
## which can be moved with the mouse.
##
' Test der Vektorrotationsroutinen zum Thema Polarisierte Elektronen
' Try to catch the spin!
'
## (c) Markus Hoffmann im Nov. 1999
'
'
bw=240
bh=240
pts=0.031
pt=pts
SIZEW 1,bw,bh
schwarz=COLOR_RGB(0,0,0)
weiss=COLOR_RGB(1,1,1)
rot=COLOR_RGB(1,0,0)
gelb=COLOR_RGB(1,1,0)
CLEARW
x1=0
x2=1
x3=0
i=0
DO
  n1=MOUSEX-bw/2
  n2=MOUSEY-bh/2
  n3=SQRT(n1^2+n2^2)-100
  betrag=SQRT(n1^2+n2^2+n3^2)
  n1=n1/betrag
  n2=n2/betrag
  n3=n3/betrag
  alpha=20*PI/180
  INC i
  IF n3<x3
    COLOR weiss
    LINE bw/2,bh/2,bw/2+x1*100,bh/2+x2*100
    PCIRCLE bw/2+x1*100,bw/2+x2*100,5*(-x3+1)
    COLOR rot
    LINE bw/2,bh/2,bw/2+n1*100,bh/2+n2*100
    PCIRCLE bw/2+n1*100,bh/2+n2*100,5*(-n3+1)
  ELSE
    COLOR rot
    LINE bw/2,bh/2,bw/2+n1*100,bh/2+n2*100
    PCIRCLE bw/2+n1*100,bh/2+n2*100,5*(-n3+1)
    COLOR weiss
    LINE bw/2,bh/2,bw/2+x1*100,bh/2+x2*100
    PCIRCLE bw/2+x1*100,bw/2+x2*100,5*(-x3+1)
  ENDIF
  SHOWPAGE
  PAUSE pt
  IF TIMER-t>pts
    pt=pt*0.9
  ELSE IF TIMER-t<pts*0.9
    pt=pt*1.1
  ENDIF
  t=TIMER
  COLOR schwarz
  PBOX 0,0,bw,bh
  COLOR gelb
  BOX bw/2-100,bh/2-100,bw/2+100,bh/2+100
  COLOR weiss
  sa=SIN(alpha)
  ca=COS(alpha)-1
  xs1=(1+ca*(n3^2+n2^2))*x1+(-n3*sa-n2*n1*ca)*x2+(n2*sa-n3*n1*ca)*x3
  xs2=(n3*sa-ca*n1*n2)*x1+(1+ca*(n3^2+n1^2))*x2+(-n1*sa-n2*n3*ca)*x3
  xs3=(-n2*sa-n3*n1*ca)*x1+(n1*sa-n2*n3*ca)*x2+(1+ca*(n2^2+n1^2))*x3
  x1=xs1
  x2=xs2
  x3=xs3
  b=x1^2+x2^2+x3^2
  TEXT bw-80,bh-7,STR$(b)
LOOP
