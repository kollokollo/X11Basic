' Space.bas in X11-Basic
'
'
a%=6
DIM pkx(200+1),pky(200+1),r%(200+1),v%(200+1)
FOR i%=0 TO 200
  r%(i%)=RANDOM(1000)
NEXT i%
FOR i%=0 TO 200
  v%(i%)=RANDOM(20)+3
NEXT i%
weiss=COLOR_RGB(1,1,1)
t=TIMER
zc=0
COLOR GET_COLOR(65535,65535,20000)
CLEARW 
TEXT 200,30," S P A C E      von Markus Hoffmann "
c%=0
bw=640
bh=400
' get_geometry 1,bx,by,bw,bh
ARRAYFILL pkx(),bw/2
ARRAYFILL pky(),bh/2
DO
  EXIT IF MOUSEK
  COLOR schwarz
  CIRCLE pkx(c%),pky(c%),v%(c%)
  COLOR weiss
  z0=360*r%(c%)/1000
  v1=v%(c%)*(SQR((pkx(c%)-bw/2)^2+(pky(c%)-bh/2)^2)+0.1)*0.01
  ADD pkx(c%),SIN(rad(z0))*v1
  ADD pky(c%),COS(rad(z0))*v1
  CIRCLE pkx(c%),pky(c%),v%(c%)
  IF pkx(c%)>bw-30 OR pkx(c%)<30 OR pky(c%)>bh-30 OR pky(c%)<30
    COLOR schwarz
    CIRCLE pkx(c%),pky(c%),v%(c%)
    r%(c%)=RANDOM(1000)
    pkx(c%)=bw/2
    pky(c%)=bh/2
    z0=2*PI*r%(c%)/1000
    z1=SIN(z0)
    z2=COS(z0)
    v1=RANDOM(100)
    ADD pkx(c%),z1*v1
    ADD pky(c%),z2*v1
    v%(c%)=RANDOM(5)+3
  ENDIF
  INC c%
 ' print c%,a%,zc
  IF c%>a%
    INC zc
    SHOWPAGE
    CLR c%
  ENDIF
  IF zc=40
    PRINT AT(1,1);TIMER-t
    IF TIMER-t>3
      DEC a%
      PRINT "-"'a%
    ELSE
      INC a%
      PRINT "+"'a%
    ENDIF
    t=TIMER
    CLR zc
  ENDIF
LOOP
QUIT

