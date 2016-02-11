'
' Demonstration of POLYLINE and POLYFILL Jan 19  2008
' written in X11-Basic (c) Markus Hoffmann
'
bw=320
bh=240
DIM x(5),y(5)     ! Define points for a regular square
x(0)=-50
y(0)=-50
x(1)=50
y(1)=-50
x(2)=50
y(2)=50
x(3)=-50
y(3)=50
x(4)=-50          ! The last point should be identical with the first one
y(4)=-50
SIZEW ,bw,bh
CLEARW
SHOWPAGE
r=5/180           ! Define the rotation angle for each step
gelb=COLOR_RGB(1,1/2,0)
schwarz=COLOR_RGB(0,1/2,0)
rot=COLOR_RGB(1,0,0)
weiss=COLOR_RGB(1,1,1)
DO
  FOR i=0 TO 4                ! Rotate all 4 points by an angle r
    xx=COS(r)*x(i)+SIN(r)*y(i)
    y(i)=-SIN(r)*x(i)+COS(r)*y(i)
    x(i)=xx
  NEXT i
  DEFFILL ,1,16
  COLOR schwarz
  PBOX 0,0,bw,bh
  COLOR weiss,schwarz
  BOX bw-16,bh-16,bw,bh
  DEFFILL ,3,17
  TEXT 10,10,"Polyline/Polyfill test X11-Basic"
  POLYFILL 3,x(),y(),bw/3,bh/3
  DEFFILL ,3,10
  POLYFILL 5,x(),y(),bw/2,bh/2
  DEFFILL ,2,18
  POLYFILL 3,x(),y(),bw/3,2*bh/3
  DEFFILL ,0
  COLOR gelb
  POLYLINE 5,x(),y(),bw/2,bh/2
  SHOWPAGE
  PAUSE 0.05
LOOP
