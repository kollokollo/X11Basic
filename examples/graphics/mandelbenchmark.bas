'
' A Fractal as a benchmark for the X11-basic virtual machine
'                        (c) Markus Hoffmann 2007
'

bx=0
by=0
bw=256
bh=240

sx=-2.2
sy=-1.5
sw=2
sh=2

SIZEW ,bw,bh
TEXT 0,bh/2,"Drawing a fractal. Be patient..."
t=timer
WHILE y<bh

  gx=(x-bx)/bw*sw+sx
  gy=(y-by)/bh*sh+sy
  zx=gx
  zy=gy
  c=0
  WHILE c<256
    nzx=zx^2-zy^2+gx
    zy=2*zx*zy+gy
    zx=nzx
    IF zx^2+zy^2>4
      GOTO out
    ENDIF
    c=c+1
  WEND
  out:
  IF c<256
    COLOR get_color(c*256,c*512,c*1024)
    PLOT x,y
  ENDIF
  x=x+1
  IF x=bw
    x=0
    y=y+1
    SHOWPAGE
    PRINT y;"  ";CHR$(13);
    FLUSH
  ENDIF
WEND
PRINT timer-t;" Seconds.  ";86/(timer-t);" times."
END
