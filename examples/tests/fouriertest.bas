' Testet die eingebaute Fast-Fourier-Transformation with X11-Basic
' (c) by Markus Hoffmann 2000-2013
'
' Demonstrates the use of FFT and SCOPE commands
'

l=2^10   ! It is faster to use a power of two
c1=4
c2=86

DIM a(l),b(l)
SIZEW ,l,400
CLEARW
FOR i=0 TO l-1
  a(i)=200/100*@si(3*i/512*2*pi)+i/100*sin(20*i/512*2*pi)
  b(i)=a(i)
NEXT i

FOR c2=c1 TO c1+200 STEP 2
  CLEARW
  COLOR get_color(65535,32000,0)
  TEXT 10,32,"Fourier Transformation test with X11-Basic."
  SCOPE a(),1,-10,300
  FFT a()              ! Do the Fourier transformation
  ' Normalize
  FOR i=0 TO l-1
    a(i)=a(i)/SQRT(l)
  NEXT i

  COLOR COLOR_RGB(0,1,1)
  FOR i=0 TO l STEP 2
    LINE i/2,400,i/2,400-100/SQRT(l)*SQRT(a(i)^2+a(i+1)^2)
  NEXT i
  LINE c1/2,0,c1/2,10
  LINE c2/2,0,c2/2,10
  SHOWPAGE
  FOR i=c1 TO c2
    a(i)=0
  NEXT i
  FOR i=l-1 DOWNTO l-c2
    a(i)=0
  NEXT i
  FFT a(),-1
  COLOR COLOR_RGB(0,1/2,1)
  SCOPE a(),0,-10/SQRT(l),300

  ' open "O",#1,"testme"
  ' for i=0 to l
  ' f%=a(i)*20000
  ' out #1,f%
  ' next i
  ' close #1

  FOR g=1 TO 10
    COLOR COLOR_RGB(1,1,0)
    SCOPE a(),b(),1,2/SQRT(l)*g,200,-1*g,200
    SHOWPAGE
    COLOR COLOR_RGB(0,0,0)
    SCOPE a(),b(),1,2/SQRT(l)*g,200,-1*g,200
  NEXT g
  FOR i=0 TO l-1
    a(i)=a(i)/SQRT(l)
  NEXT i
  PAUSE 0.1
NEXT c2
ALERT 0,"Done.",1," OK ",d
QUIT

DEFFN si(x)=x MOD pi
