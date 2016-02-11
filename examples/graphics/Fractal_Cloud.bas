' cloud.bas
' From "Fractals" by Hans Lauwerier
' Shows Orbits of a dynamical system

CLEARW
a = 3.5
b=-3
x=3.21
y=6.54
@sub1
SHOWPAGE
PAUSE 1
get_geometry 1,bx,by,bw,bh
DO
  PLOT x+bw/2,y+bh/2
  exit if x>bw/2 or y>bh/2
  if (n mod 13)=0
   color get_color((n mod 1000)*65,(n mod 2000)*25,(n mod 2550)*25)
  endif
  if (n mod 1000)=0
    SHOWPAGE
  endif
  z=x
  x=y+w
  @sub1
  y=w-z
  inc n
LOOP
END

PROCEDURE sub1
  if x>1 
    w=a*x+b*(x-1)
  else if x<-1 
   w=a*x+b*(x+1)
  else
   w = a*x
  endif
return

