' ZOOM-Funktion

faktor=3

FOR x=0 TO 639 STEP faktor
  FOR y=0 TO 399 STEP faktor
    ox=x/faktor
    oy=y/faktor
    ROOTWINDOW
    a=point(ox,oy)
    USEWINDOW 1
    COLOR a
    PBOX x,y,x+faktor,y+faktor
  NEXT y
  VSYNC
NEXT x
