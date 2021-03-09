' Sierpinski Random Chaos Algorithm
'
' converted to X11-Basic 2010
'
bw=400
bh=400
SIZEW ,bw,bh
CLEARW
x1=bw/2
x2=bh/2
y1=0
y2=0
r=1
' for r=1 to 2 step 0.1
CLEARW
t=timer
FOR k=1 TO 50000
  ecke=RANDOM(3)
  ' ecke=(ecke+1) mod 3
  IF ecke=0
    x1=(x2+bw/2)/2
    y1=y2/2
  ELSE if ecke=1
    x1=x2/2
    y1=(y2+bh)/2
  ELSE
    x1=(x2+bw)/2
    y1=(y2+bh)/2
  ENDIF
  a=INT(y1/bh*250)
  b=INT(x1/bw*250)
  COLOR get_color(a*256,b*256,(255-a)*256)
  PLOT x1,y1
  IF (k MOD 1000)=0
    SHOWPAGE
    PRINT timer-t;" Sekunden.";CHR$(13);
    t=timer
  ENDIF
  x2=x1
  y2=y1
NEXT k
SHOWPAGE
' next r
~inp(-2)
QUIT

