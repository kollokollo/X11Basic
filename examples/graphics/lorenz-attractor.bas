' Lorenz Attractor in X11-Basic adapted from a program on
' http://forum.basicprogramming.org/
'
y=20
z=20
white=COLOR_RGB(1,1,1)
COLOR white
SIZEW ,640,500
SHOWPAGE
CLEARW
TEXT 280,20,"LORENZ BUTTERFLY"
tick=TIMER
DO
  xx=x+0.01*(-10.0*x+10.0*y)
  yy=y+0.01*(28.0*x-y-x*z)
  zz=z+0.01*(-8.0*z/3.0+x*y)
  LINE x*10+320,-z*10+500,xx*10+320,-zz*10+500
  IF (count MOD 100)=1
    SHOWPAGE
    COLOR COLOR_RGB((count MOD 256)/256,count/120,count/1300)
  ENDIF
  x=xX
  y=yY
  z=zZ
  INC count
  EXIT iF count=20000
LOOP
tack=TIMER-tick
COLOR white
TEXT 0,460,"TIME: "+STR$(tack)+" SECONDS"
TEXT 0,440,STR$(count)+" LINES DRAWN"
SHOWPAGE
KEYEVENT
END
