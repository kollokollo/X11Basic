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
dim s(4,2)
s(0,0)=1
s(0,1)=0
s(1,0)=0
s(1,1)=1
s(2,0)=-1
s(2,1)=0
s(3,0)=0
s(3,1)=-1

' Field size in points, with skip over sl.
if sz and 1=1
  sz=sz+1
endif
width=sz*ps*2+1
height=width

' Maximum number of saved positions
max=width*height
dim px(max)
dim py(max)

sizew ,ps*width,ps*height
showpage
titlew 1,"Wandering Maze"

fg1=get_color(65535,65535,65535)
fg2=get_color(65535,0,0)
fg3=get_color(100,1000,1000)
fg=fg1
bg=get_color(0,0,0)
color bg
pbox 0,0,ps*width,ps*height
showpage

color fg
showpage
pbox 0,0,width*ps,ps*2
pbox 0,0,ps*2,height*ps
pbox width*ps,height*ps,width*ps-ps*2,0
pbox width*ps,height*ps,0,height*ps-ps*2
color fg3
defline ,8
circle 100,100,202
pcircle 100,100,40
line 100,300,100,400
color bg
line 10,100,(width-2)*ps,100
color fg
showpage
defline ,1

' Segment counter
sg=0
' Starting positions.
x=3
y=x
ox=x
oy=y

' Direction (0=-> , 1=v, 2=<-, 3=^ )
d=0
dim n(4)

' Memory storage pointers for free positions.
mx=0
mo=mx
px(mx)=0
py(mx)=0
mx=mx+1


' Main loop:

mainloop:

' print "MAINLOOP MX:";mx;" X:";x;" Y:";y;" DIR:";d
if mx<=mo
  goto fini
endif 
c=0
for i=0 to 3
  if @chkpoint(x+s(i,0)*sl,y+s(i,1)*sl)=bg
    n(c)=i
    c=c+1
  endif
next i
color bg
pbox 10,330,80,350
color fg2
text 10,350,"FREE DIR: "+str$(c)
color fg
' free position to go into?
if c=0 
  ' nope. Backtrack.
  ox=x
  oy=y
  dec mx
  x=px(1)
  y=py(1)
  
  if mx>1
  for i=1 to mx
    px(i)=px(i+1)
    py(i)=py(i+1)
  next i
  endif  
  
  ' print "BACK-TRACKED INTO ";x;",";y;" @ ";mx
else
  p=@chkpoint(x+s(d,0)*sl,y+s(d,1)*sl)
  ' print "CHECKING POINT: ";x+s(d,0);",";y+s(d,1);"=";p;" against ";bg
  if p<>bg
    ' we're colliding in the current direction, so
    ' chose a new direction at random from those we've seen free.
    d=n(RANDOM(c))
    ' print "CHANGING DIRECTION TO ";d
  else
    t=t-1
    if t<0
      ' It's time to change direction anyway.
      d=n(RANDOM(c))
      t=RANDOM(f+1)
      ' print "TIMED CHANGING DIRECTION TO ";d;" NEW TIME: ";t
    else
      ' print "FREE TO DRAW"
      ' Current direction if free, let's draw in it.
      x1=x
      y1=y
      ' Advance.
      x=x+s(d,0)*sl
      y=y+s(d,1)*sl
      ' print "NEW POSITION: ";x;",";y
      gosub drawline(x1,y1,x,y)
      sg=sg+1
      if pd
        pause pd
      endif
      ' Let's remember this position as (possibly) having free neighbours.
      px(mx)=x
      py(mx)=y
      mx=mx+1
      if mx>mh
        mh=mx
      endif
    endif
  endif
endif
goto mainloop


fini:
color fg
pbox ps*2,ps*3,ps*4,ps*4
pbox width*ps-ps*2,height*ps-ps*3,width*ps-ps*4,height*ps-ps*4
showpage

print "Segments drawn: "; sg
print "Maximum depth: ";mh
end

procedure drawline(xx1,yy1,xx2,yy2)
  color fg
  ' print "DRAW LINE FROM ";xx1;",";yy1;"-";xx2;",";yy2
  if xx1=xx2 
    if yy2>yy1
      for i=yy1 to yy2
        gosub drawpoint(xx1,i)
      next i
    else
      for i=yy2 to yy1
        gosub drawpoint(xx1,i)
      next i
    endif
  else
    if xx2>xx1
      for i=xx1 to xx2
        gosub drawpoint(i,yy1)
      next i
    else
      for i=xx2 to xx1
        gosub drawpoint(i,yy1)
      next i
    endif
  endif
return

function chkpoint(xx,yy)
  color fg
  ' print "CHECK POINT AT ";xx;",";yy
  showpage
  if xx<0 or yy<0
    print "*** INVALID!"
    stop
  else
    return point(xx*ps,yy*ps)
  endif
endfunction

procedure drawpoint(xx,yy)
  color fg
  ' print "DRAW POINT AT ";xx;",";yy
  pbox xx*ps,yy*ps,xx*ps+ps,yy*ps+ps
  showpage
return

procedure erasepoint(xx,yy)
  color bg
  print "ERASE POINT AT ";xx;",";yy
  pbox xx*ps,yy*ps,xx*ps+ps,yy*ps+ps
  showpage
return
