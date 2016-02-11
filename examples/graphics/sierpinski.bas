' Sierpinski Random Chaos Algorithm 
' 
' converted to X11-Basic 2010
'
bw=400
bh=400
sizew ,bw,bh
clearw
x1=bw/2
x2=bh/2
y1=0
y2=0
r=1
' for r=1 to 2 step 0.1
clearw
t=timer
for k=1 to 50000
  ecke=random(3)
 ' ecke=(ecke+1) mod 3
  if ecke=0
    x1=(x2+bw/2)/2
    y1=y2/2
  else if ecke=1
    x1=x2/2
    y1=(y2+bh)/2
  else 
    x1=(x2+bw)/2
    y1=(y2+bh)/2
  endif
  a=int(y1/bh*250) 
  b=int(x1/bw*250) 
  color get_color(a*256,b*256,(255-a)*256)
  plot x1,y1
  if (k mod 1000)=0
    vsync
    print timer-t;" Sekunden."
    t=timer
  endif
  x2=x1
  y2=y1
next k
vsync
' next r
~inp(-2)
quit
  
