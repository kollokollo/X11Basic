' Modifikation von ZOOM

faktor=0.3
winkel=pi/7
posx=300
posy=200

s=sin(winkel)*faktor
c=cos(winkel)*faktor

FOR x=0 TO 639
  FOR y=0 TO 399
    ox=c*x+s*y+posx
    oy=-s*x+c*y+posy
    ROOTWINDOW
    a=point(ox,oy)
    USEWINDOW 1
    COLOR a
    PLOT x,y
  NEXT y
  VSYNC
NEXT x
