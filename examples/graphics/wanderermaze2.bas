'
' (C) 2011 Danny Chouinard.  Released to the public domain.
'
' This is ported from 6809 assembler, so there's a lot of optimization
' to be done, especially replacing the drawing routines, bu it works.

' You can edit these parameters.

' Point size.  Think of this as a "zoom factor".  Must be at least 2.
ps=3

' Size to a side. Will be rounded to odd value. Window size will be ps*2*sz
sz=20

' fuzzyness
f=1

' Delay between segment draws (1/speed).
pd=0.01

' Stop editing now. :)

' Step length.
sl=2

' Step directions
DIM s(4,2)
s(0,0)=1
s(0,1)=0
s(1,0)=0
s(1,1)=1
s(2,0)=-1
s(2,1)=0
s(3,0)=0
s(3,1)=-1

' Field size in points, with skip over sl.
IF sz AND 1=1
  sz=sz+1
ENDIF
width=sz*ps*2+1
height=width

' Maximum number of saved positions
max=width*height
DIM px(max)
DIM py(max)

SIZEW ,ps*width,ps*height
SHOWPAGE
TITLEW 1,"Wandering Maze"

fg1=GET_COLOR(65535,65535,65535)
fg2=GET_COLOR(65535,0,0)
fg3=GET_COLOR(100,1000,1000)
fg=fg1
bg=GET_COLOR(0,0,0)
COLOR bg
PBOX 0,0,ps*width,ps*height
SHOWPAGE

COLOR fg
SHOWPAGE
PBOX 0,0,width*ps,ps*2
PBOX 0,0,ps*2,height*ps
PBOX width*ps,height*ps,width*ps-ps*2,0
PBOX width*ps,height*ps,0,height*ps-ps*2
COLOR fg3
DEFLINE ,8
CIRCLE 100,100,202
PCIRCLE 100,100,40
LINE 100,300,100,400
COLOR bg
LINE 10,100,(width-2)*ps,100
COLOR fg
SHOWPAGE
DEFLINE ,1

' Segment counter
sg=0
' Starting positions.
x=3
y=x
ox=x
oy=y

' Direction (0=-> , 1=v, 2=<-, 3=^ )
d=0
DIM n(4)

' Memory storage pointers for free positions.
mx=0
mo=mx
px(mx)=0
py(mx)=0
mx=mx+1

' Main loop:

mainloop:

' print "MAINLOOP MX:";mx;" X:";x;" Y:";y;" DIR:";d
IF mx<=mo
  GOTO fini
ENDIF
c=0
FOR i=0 TO 3
  IF @chkpoint(x+s(i,0)*sl,y+s(i,1)*sl)=bg
    n(c)=i
    c=c+1
  ENDIF
NEXT i
COLOR bg
PBOX 10,330,80,350
COLOR fg2
TEXT 10,350,"FREE DIR: "+STR$(c)
COLOR fg
' free position to go into?
IF c=0
  ' nope. Backtrack.
  ox=x
  oy=y
  DEC mx
  x=px(1)
  y=py(1)

  IF mx>1
    FOR i=1 TO mx
      px(i)=px(i+1)
      py(i)=py(i+1)
    NEXT i
  ENDIF

  ' print "BACK-TRACKED INTO ";x;",";y;" @ ";mx
ELSE
  p=@chkpoint(x+s(d,0)*sl,y+s(d,1)*sl)
  ' print "CHECKING POINT: ";x+s(d,0);",";y+s(d,1);"=";p;" against ";bg
  IF p<>bg
    ' we're colliding in the current direction, so
    ' chose a new direction at random from those we've seen free.
    d=n(RANDOM(c))
    ' print "CHANGING DIRECTION TO ";d
  ELSE
    t=t-1
    IF t<0
      ' It's time to change direction anyway.
      d=n(RANDOM(c))
      t=RANDOM(f+1)
      ' print "TIMED CHANGING DIRECTION TO ";d;" NEW TIME: ";t
    ELSE
      ' print "FREE TO DRAW"
      ' Current direction if free, let's draw in it.
      x1=x
      y1=y
      ' Advance.
      x=x+s(d,0)*sl
      y=y+s(d,1)*sl
      ' print "NEW POSITION: ";x;",";y
      GOSUB drawline(x1,y1,x,y)
      sg=sg+1
      IF pd
        PAUSE pd
      ENDIF
      ' Let's remember this position as (possibly) having free neighbours.
      px(mx)=x
      py(mx)=y
      mx=mx+1
      IF mx>mh
        mh=mx
      ENDIF
    ENDIF
  ENDIF
ENDIF
GOTO mainloop

fini:
COLOR fg
PBOX ps*2,ps*3,ps*4,ps*4
PBOX width*ps-ps*2,height*ps-ps*3,width*ps-ps*4,height*ps-ps*4
SHOWPAGE

PRINT "Segments drawn: "; sg
PRINT "Maximum depth: ";mh
END

PROCEDURE drawline(xx1,yy1,xx2,yy2)
  COLOR fg
  ' print "DRAW LINE FROM ";xx1;",";yy1;"-";xx2;",";yy2
  IF xx1=xx2
    IF yy2>yy1
      FOR i=yy1 TO yy2
        GOSUB drawpoint(xx1,i)
      NEXT i
    ELSE
      FOR i=yy2 TO yy1
        GOSUB drawpoint(xx1,i)
      NEXT i
    ENDIF
  ELSE
    IF xx2>xx1
      FOR i=xx1 TO xx2
        GOSUB drawpoint(i,yy1)
      NEXT i
    ELSE
      FOR i=xx2 TO xx1
        GOSUB drawpoint(i,yy1)
      NEXT i
    ENDIF
  ENDIF
RETURN

FUNCTION chkpoint(xx,yy)
  COLOR fg
  ' print "CHECK POINT AT ";xx;",";yy
  SHOWPAGE
  IF xx<0 OR yy<0
    PRINT "*** INVALID!"
    STOP
  ELSE
    RETURN point(xx*ps,yy*ps)
  ENDIF
ENDFUNCTION

PROCEDURE drawpoint(xx,yy)
  COLOR fg
  ' print "DRAW POINT AT ";xx;",";yy
  PBOX xx*ps,yy*ps,xx*ps+ps,yy*ps+ps
  SHOWPAGE
RETURN

PROCEDURE erasepoint(xx,yy)
  COLOR bg
  PRINT "ERASE POINT AT ";xx;",";yy
  PBOX xx*ps,yy*ps,xx*ps+ps,yy*ps+ps
  SHOWPAGE
RETURN
