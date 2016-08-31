' test of the EVENT command in X11-Basic
' can have different results on different platforms/OS
'

@init
DO
  IF event?(1+2+4+8+0x40)=true OR true
    t=4711
    EVENT t,x,y,xroot,yroot,s,k,ks,t$,tim
    IF t=2
      @show("Key Press")
      PRINT t,x,y,xroot,yroot,s,k,ks,t$,tim
      key(k)=1
      @key
    ELSE if t=3
      @show("Key Release")
      PRINT t,x,y,xroot,yroot,s,k,ks,t$,tim
      key(k)=0
      @key

    ELSE if t=4
      @show("Mouse Button Press")
      mb(k)=1
      COLOR rot
      PCIRCLE x,y,5
      @mouse
    ELSE if t=5
      @show("Mouse Button Release")
      mb(k)=0
      @mouse

    ELSE if t=6
      @show("Mouse Motion")
      COLOR gelb
      PCIRCLE x,y,5
    ELSE if t=10
      @show("Window Moved")
    ELSE if t=13
      @show("Window Resize")
      '  sizew ,x,y
      '  vsync
    ELSE
      PRINT t,x,y,xroot,yroot,s,k,ks,t$,tim
      PRINT "Event: typ=0x"+HEX$(t)
    ENDIF
  ENDIF

LOOP
PROCEDURE init
  schwarz=color_RGB(0,0,0)
  rot=color_RGB(1,0,0)
  gelb=color_RGB(1,1,0)
  weiss=color_RGB(1,1,1)
  COLOR schwarz
  GET_GEOMETRY ,bx%,by%,bw%,bh%
  PRINT "Screen dimensions: ";bx%,by%,bw%,bh%
  PBOX bx%,by%,bw%,bh%
  DIM mb(6)
  DIM key(512)
RETURN
PROCEDURE show(t$)
  COLOR schwarz
  PBOX 10,10,180,32
  COLOR rot
  TEXT 16,25,t$
  VSYNC
RETURN
PROCEDURE mouse
  COLOR weiss
  BOX 10,40,120,120
  FOR i=1 TO 5
    COLOR weiss
    BOX i*20,44,i*20+16,60
    IF mb(i)
      COLOR weiss
    ELSE
      COLOR schwarz
    ENDIF
    PBOX i*20+1,44+1,i*20+16-1,60-1
  NEXT i
  VSYNC
RETURN

PROCEDURE key
  COLOR weiss
  BOX 320,60,420,120
  FOR j=0 TO 15
    FOR i=0 TO 31
      COLOR weiss
      BOX 320+i*10,60+j*10,320+i*10+8,60+j*10+8
      IF key(j*32+i)
        COLOR weiss
      ELSE
        COLOR schwarz
      ENDIF
      PBOX 320+i*10+1,60+j*10-1,320+i*10+8+1,60+j*10+8-1
    NEXT i
  NEXT j
  VSYNC
RETURN

