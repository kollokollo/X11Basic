' ############################ (c) Markus Hoffmann 1990
' ## mehr-K”rper Simulation ##
' ############################
'
echo off
's$=SPACE$(32256)
's1=V:s$ AND &HFFFFFF00
's2=XBIOS(2)
's=XBIOS(2)
'ON BREAK GOSUB b
'
schwarz=get_color(0,0,0)
weiss=get_color(65535,65535,10000)
anz&=6
dt=1
DIM x(anz&),y(anz&),z(anz&),vx(anz&),vy(anz&),vz(anz&),m(anz&)
'
FOR i&=1 TO anz&
  m(i&)=1
  x(i&)=RANDOM(20)+320
  y(i&)=RANDOM(20)+220
NEXT i&
m(3)=9
x(3)=40
y(3)=300
m(2)=3
x(2)=300
vx(2)=-5*9/10
y(2)=270
m(1)=10
x(1)=320
y(1)=200
vx(1)=5
'
'~XBIOS(5,L:s1,L:s2,-1)
'SWAP s1,s2
'
'
CLR mges,spx,spy
FOR i&=1 TO anz&
  mges=mges+m(i&)
NEXT i&
DO
  x(2)=MOUSEX
  y(2)=MOUSEY
  m(2)=10
  cls
  FOR i&=1 TO anz&
    x=x(i&)-(spx-320)
    y=y(i&)-(spy-200)
    IF x<0 OR y<0 OR x>640 OR y>400
     ' PRINT x,y
      PAUSE 50/200
      SWAP x(i&),x(anz&)
      SWAP y(i&),y(anz&)
      SWAP m(i&),m(anz&)
      DEC anz&
      IF anz&=0
        @b
      ENDIF
      CLR mges,spx,spy
      FOR i&=1 TO anz&
        mges=mges+m(i&)
      NEXT i&
      x=x(i&)-(spx-320)
      y=y(i&)-(spy-200)
    ENDIF
    BOX x-m(i&),y-m(i&),x+m(i&),y+m(i&)
  NEXT i&
  x=spx-(spx-320)
  y=spy-(spy-200)
  LINE x-2,y,x+2,y
  LINE x,y-2,x,y+2
 ' PRINT AT(2,20);TIMER-t'''
  vsync
  color schwarz
  pbox 0,0,640,400
  color weiss
'  ~XBIOS(5,L:s1,L:s2,-1)
  t=TIMER
'  SWAP s1,s2
  CLR spx,spy,spz
  FOR i&=1 TO anz&
    f=dt/m(i&)
    FOR j&=1 TO anz&
      IF i&<>j&
        ADD vx(i&),f*@fx(x(i&),y(i&),z(i&),m(i&),x(j&),y(j&),z(j&),m(j&))
        ADD vy(i&),f*@fy(x(i&),y(i&),z(i&),m(i&),x(j&),y(j&),z(j&),m(j&))
        ADD vz(i&),f*@fz(x(i&),y(i&),z(i&),m(i&),x(j&),y(j&),z(j&),m(j&))
      ENDIF
    NEXT j&
    ADD x(i&),vx(i&)*dt
    ADD y(i&),vy(i&)*dt
    ADD z(i&),vz(i&)*dt
    ADD spx,x(i&)*m(i&)
    ADD spy,y(i&)*m(i&)
    ADD spz,z(i&)*m(i&)
  NEXT i&
  spx=spx/mges
  spy=spy/mges
  spz=spz/mges
LOOP

quit


function fx(x1,y1,z1,m1,x2,y2,z2,m2)
return (x2-x1)*m1*m2/((x2-x1)^2+(y2-y1)^2+(z2-z1)^2)
endfunc

function fy(x1,y1,z1,m1,x2,y2,z2,m2)
return (y2-y1)*m1*m2/((x2-x1)^2+(y2-y1)^2+(z2-z1)^2)
endfunc

function fz(x1,y1,z1,m1,x2,y2,z2,m2)
return (z2-z1)*m1*m2/((x2-x1)^2+(y2-y1)^2+(z2-z1)^2)
endfunc
