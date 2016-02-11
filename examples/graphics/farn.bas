' Farn Fraktal (barnsley)
' 
' converted to X11-Basic 2010, to compare speed.
'
'
bw=450
bh=700
sizew ,bw,bh
clearw
clr x,y
text 10,80,"barnsley"
t=timer
t0=t
for n=1 to 500000
  r=rnd()
  if r<=0.01
    x=0 
    y=0.16*y
  else if r<=0.08
    x=0.2*x-0.26*y
    y=0.23*x+0.22*y+1.6
  else if r<=0.15
    x=-0.15*x+0.28*y
    y=0.26*x+0.24*y+0.44
  else
    x=0.85*x+0.04*y
    y=-0.04*x+0.85*y+1.6
  endif
  x1=(x+3)*70
  y1=bh-y*70
  plot x1,y1
  if (n mod 10000)=0
    vsync
    print timer-t;" Sekunden."
    t=timer
  endif
next n
print "Total time: ";timer-t0;" Seconds."
~inp(-2)
quit
