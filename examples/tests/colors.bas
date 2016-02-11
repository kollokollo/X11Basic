' Farbflaeche (c) Markus Hoffmann with X11-Basic V.1.00

sizew ,32*16,32*16
dim c(10,10)

rpx=8
rpy=16
gpx=24
gpy=8
bpx=24
bpy=24
gelb=get_color(65535,65535,0)
color gelb
text 100,100,"COLORTEST X11-BASIC"
t=timer
do
  x=random(64)
  y=random(64)
  for x=0 to 63
    for y=0 to 63 
      r=min(sqr((rpx-x)^2+(rpy-y)^2),64)
      g=min(sqr((gpx-x)^2+(gpy-y)^2),64)
      b=min(sqr((bpx-x)^2+(bpy-y)^2),64)
      c=get_color(r*1024,g*1024,b*1024) 
      if c>=0
        color c
      endif
      pbox x*8,y*8,x*8+8,y*8+8
      ' pcircle x*16,y*16,5
      ' color 1
      ' text x*16-5,y*16+3,str$(c,3,3)
      ' circle x*16,y*16,5
      vsync
    next y
  next x
  exit if true
loop
print timer-t;" Sek."
quit
