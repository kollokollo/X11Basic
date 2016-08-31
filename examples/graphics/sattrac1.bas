' Seltsamer Attraktor von Henon
mmm=10000
DIM dx(mmm+1)
DIM dy(mmm+1)
schwarz=GET_COLOR(0,0,0)
weiss=GET_COLOR(65535,65535,65535)
CLEARW
x=0.5
y=0.5
c=0
COLOR weiss
FOR c=0 TO mmm
  ox=x
  x=y+1-1.4*x^2
  y=0.3*ox
  dx(c)=x
  dy(c)=y
NEXT c
sx=320
sy=200
px=320
py=200
DEFMOUSE 3
DO
  CLEARW
  SCOPE dy(),dx(),1,sy,py,sx,px
  TEXT 100,50,"Drag the image with the mouse... LEFT=move, MIDDLE=scale"
  SHOWPAGE
  ox=x
  oy=y
  ok=k
  IF mousek=0
    MOUSEEVENT x,y,k
    ox=x
    oy=y
  ELSE
    REPEAT
      MOTIONEVENT x,y,k
    UNTIL EVENT?(64)=FALSE OR k=1 ! clear all pending motion events
  ENDIF

  PRINT ox,oy,ok
  IF ok
    IF ok=4
      SUB py,16
      PAUSE 0.3
    ELSE if ok=5
      ADD py,16
      PAUSE 0.3
    ELSE
      IF k=2 AND abs(x-ox)<10 AND abs(y=oy)<10
        ADD sx,x-ox
        ADD sy,y-oy
      ELSE if k=1 AND abs(x-ox)<10 AND abs(y=oy)<10
        ADD px,x-ox
        ADD py,y-oy
      ENDIF
    ENDIF
  ENDIF
LOOP
QUIT
