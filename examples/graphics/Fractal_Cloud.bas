' cloud.bas
' From "Fractals" by Hans Lauwerier
' Shows Orbits of a dynamical system

CLEARW
a=3.5
b=-3
x=3.21
y=6.54
@sub1
SHOWPAGE
PAUSE 1
GET_GEOMETRY 1,bx,by,bw,bh
DO
  PLOT x+bw/2,y+bh/2
  EXIT if x>bw/2 OR y>bh/2
  IF (n MOD 13)=0
    COLOR get_color((n MOD 1000)*65,(n MOD 2000)*25,(n MOD 2550)*25)
  ENDIF
  IF (n MOD 1000)=0
    SHOWPAGE
  ENDIF
  z=x
  x=y+w
  @sub1
  y=w-z
  INC n
LOOP
END

PROCEDURE sub1
  IF x>1
    w=a*x+b*(x-1)
  ELSE if x<-1
    w=a*x+b*(x+1)
  ELSE
    w=a*x
  ENDIF
RETURN

