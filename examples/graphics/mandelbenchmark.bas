'
' A Fractal as a benchmark for the X11-basic virtual machine
'                        (c) Markus Hoffmann 2007
'

bx=0
by=0
bw=256
bh=256

sx=-2.2
sy=-1.5
sw=2
sh=2

sizew ,bw,bh
t=timer
while y<bh

  gx=(x-bx)/bw*sw+sx
  gy=(y-by)/bh*sh+sy
  zx=gx
  zy=gy
  c=0
  while c<256
    nzx=zx^2-zy^2+gx
    zy=2*zx*zy+gy
    zx=nzx
    if zx^2+zy^2>4
      goto out
    endif  
    c=c+1
  wend
out:
  if c<256
    color get_color(c*256,c*512,c*1024)
    plot x,y
  endif
  x=x+1
  if x=bw
    x=0
    y=y+1
    showpage
    print y
  endif    
wend
print timer-t
~inp(-2)
quit
