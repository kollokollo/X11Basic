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

SIZEW ,bw,bh
CLEARW
t=timer
FOR x=bx TO bx+bw
  FOR y=by TO by+bh
    gx=(x-bx)/bw*sw+sx
    gy=(y-by)/bh*sh+sy
    zx=gx
    zy=gy
    FOR c=0 TO 255
      nzx=zx^2-zy^2+gx
      zy=2*zx*zy+gy
      zx=nzx
      EXIT if zx^2+zy^2>4
    NEXT c
    COLOR c*256+c*64
    PLOT x,y
  NEXT y
  VSYNC
NEXT x
PRINT timer-t;" Sekunden."
~inp(-2)
QUIT
DATA "Benchmark fot the virtual machine by Markus Hoffmann 2010"
