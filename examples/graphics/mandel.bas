' Mandelbrotfraktale (c) Markus Hoffmann
randomize
bx=0
by=0
bw=256*2
bh=256*2
sizew ,bw,bh
clearw
dim col(256) 
col(0)=get_color(0,0,0)
col(255)=get_color(65535,65535,65535)
for i=1 to 254
  col(i)=get_color(random(65535),random(65535),random(65535))
next i 
dim field(bw,bh)
arrayfill field(),0

sx=-2.2
sy=-1.7
sw=3.4
sh=3.4

x=0
y=0
count=0
count2=0
do
  if mousek
    vsync
    if mousex>=0 and mousey>=0 and mousex<bw and mousey<bh
      x=mousex
      y=mousey
    endif
  endif
  t0=timer
  while field(x,y) and timer-t0<3
     x=random(bw)
     y=random(bh)
     inc count2
  wend
  c=0
  gx=(x-bx)/bw*sw+sx
  gy=(y-by)/bh*sh+sy
  zx=gx
  zy=gy
  while c<256
    nzx=zx^2-zy^2+gx
    zy=2*zx*zy+gy
    zx=nzx
    exit if zx^2+zy^2>4
    inc c
  wend
  field(x,y)=true
  c=c and 255
  if c>0
    color col(c)
    plot x,y
    if (count2 mod 128) or timer-t>3=0
      color col(0)
      pbox 0,0,70,20
      color col(255)
      text 10,10,str$(int(100*count/bw/bh))+"%  "+str$(count2-count)
      vsync
      t=timer
    endif
  endif
  inc count
  exit if int(100*count/bw/bh)=100
loop
print "Press any key..."
~inp(-2)
quit
