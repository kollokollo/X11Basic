' Farn Fraktal (barnsley)
'
' converted to X11-Basic 2010, to compare speed.
'
'
bw=450
bh=700
SIZEW ,bw,bh
CLEARW
CLR x,y
TEXT 10,80,"barnsley"
t=timer
t0=t
FOR n=1 TO 500000
  r=rnd()
  IF r<=0.01
    x=0
    y=0.16*y
  ELSE if r<=0.08
    x=0.2*x-0.26*y
    y=0.23*x+0.22*y+1.6
  ELSE if r<=0.15
    x=-0.15*x+0.28*y
    y=0.26*x+0.24*y+0.44
  ELSE
    x=0.85*x+0.04*y
    y=-0.04*x+0.85*y+1.6
  ENDIF
  x1=(x+3)*70
  y1=bh-y*70
  PLOT x1,y1
  IF (n MOD 10000)=0
    VSYNC
    PRINT timer-t;" Sekunden."
    t=timer
  ENDIF
NEXT n
PRINT "Total time: ";timer-t0;" Seconds."
~inp(-2)
QUIT
