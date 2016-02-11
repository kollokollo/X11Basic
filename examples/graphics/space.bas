echo off
a&=6
DIM pkx(200),pky(200),r&(200),v&(200)
ARRAYFILL pkx(),320
ARRAYFILL pky(),200
FOR i&=0 TO 200
  r&(i&)=RANDOM(1000)
NEXT i&
FOR i&=0 TO 200
  v&(i&)=RANDOM(20)+3
NEXT i&
weiss=get_color(65535,65535,65535)
t=TIMER
zc=0
color get_color(65535,65535,20000)
clearw 
text 200,30," S P A C E      von Markus Hoffmann "
c&=0
DO
  EXIT IF MOUSEK
  COLOR schwarz
  circle pkx(c&),pky(c&),v&(c&)
  COLOR weiss
  z0=360*r&(c&)/1000
  v1=v&(c&)*(SQR((pkx(c&)-320)^2+(pky(c&)-200)^2)+0.1)*0.01
  ADD pkx(c&),SIN(rad(z0))*v1
  ADD pky(c&),COS(rad(z0))*v1
  circle pkx(c&),pky(c&),v&(c&)
  IF pkx(c&)>639 OR pkx(c&)<0 OR pky(c&)>399 OR pky(c&)<0
    COLOR schwarz
    circle pkx(c&),pky(c&),v&(c&)
    r&(c&)=RANDOM(1000)
    pkx(c&)=320
    pky(c&)=200
    z0=2*PI*r&(c&)/1000
    z1=SIN(z0)
    z2=COS(z0)
    v1=RANDOM(100)
    ADD pkx(c&),z1*v1
    ADD pky(c&),z2*v1
    v&(c&)=RANDOM(5)+3
  ENDIF
  inc c&
 ' print c&,a&,zc
  if c&>a&
    inc zc
    vsync
    clr c&
  endif
 
    if zc=40
      print timer-t
      IF TIMER-t>3
        DEC a&
        print "-"'a&
      ELSE
        INC a&
        print "+"'a&
      ENDIF
      t=TIMER
      clr zc
    endif
LOOP
