' Mandelbrotfraktale (c) Markus Hoffmann
' Simple and direct drawing algorithm.
'
bx=0
by=0
bw=256*2
bh=256*2

sx=-2.2
sy=-1.7
sw=3.4
sh=3.4

sizew ,bw,bh
clearw
t=timer
for x=bx to bx+bw
  for y=by to by+bh
    gx=(x-bx)/bw*sw+sx
    gy=(y-by)/bh*sh+sy
    zx=gx
    zy=gy
    for c=0 to 255
      nzx=zx^2-zy^2+gx
      zy=2*zx*zy+gy
      zx=nzx
      exit if zx^2+zy^2>4      
    next c
    color c*256+c*64
    plot x,y
  next y
vsync
next x
print timer-t;" Sekunden."
~inp(-2)
quit
data "Benchmark fot the virtual machine by Markus Hoffmann 2010"
