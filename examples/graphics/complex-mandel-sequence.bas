' Mandelbrot fractals 
' Simple and direct drawing algorithm using complex math in X11-Basic V.1.23
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
for i=1.0 to 3 step 0.05
if not exist(str$(i*100)+".bmp")
CLEARW
t=TIMER
FOR x=bx TO bx+bw
  FOR y=by TO by+bh
    g#=(x-bx)/bw*sw+sx+1i*((y-by)/bh*sh+sy)
    z#=g#
    FOR c=0 TO 255
      z#=z#^i+g#
      EXIT IF CABS(z#)>4
    NEXT c
    COLOR c*256+c*64
    PLOT x,y
  NEXT y
  SHOWPAGE
NEXT x
savewindow str$(i*100)+".bmp"
else
system "touch "+str$(i*100)+".bmp"
endif
next i
PRINT TIMER-t;" seconds."
END
