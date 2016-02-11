' Seltsamer Attraktor von Henon
mmm=10000
dim dx(mmm+1)
dim dy(mmm+1)
schwarz=get_color(0,0,0)
weiss=get_color(65535,65535,65535)
clearw
x=0.5
y=0.5
c=0
color weiss
for c=0 to mmm
  ox=x
  x=y+1-1.4*x^2
  y=0.3*ox
  dx(c)=x
  dy(c)=y
next c
sx=320
sy=200
px=320
py=200
defmouse 3
do
  clearw
  scope dy(),dx(),1,sy,py,sx,px
  text 100,50,"Drag the image with the mouse... LEFT=move, MIDDLE=scale"
  vsync
  ox=x
  oy=y
  ok=k
  if mousek=0
    mouseevent x,y,k
    ox=x
    oy=y
  else
    motionevent x,y,k
  endif

  print ox,oy,ok
  if ok
    if ok=4
      sub py,16
      pause 0.3
    else if ok=5
      add py,16
      pause 0.3
    else   
      if k=2 and abs(x-ox)<10 and abs(y=oy)<10
        add sx,x-ox
        add sy,y-oy
      else if k=1 and abs(x-ox)<10 and abs(y=oy)<10
        add px,x-ox
        add py,y-oy
      endif

    endif
  endif
loop
quit
