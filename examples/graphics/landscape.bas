' Landscape by PeterMaria
' converted from SDLbasic to X11-Basic 2015
'
SIZEW 1,600,600
COLOR COLOR_RGB(220/256,200/256,220/256)
BOX 0,0,700,700
TEXT 50,20,"landscpe by PeterMaria / X11-Basic"
RANDOMIZE TIMER
dummy=RANDOM(100000)
DIM range(800)
CLR pointa,pointb,a,x,interp,lowmount,laker,lakeg,lakeb,decrease,k
lowr=10
col=180
sw=1
startval1=256
startval2=2

FOR chain=1 TO 6
  newval1=startval1
  newval2=startval2
  FOR inc=1 TO 6
    newval1=newval1/2
    newval2=newval2*2
    seed=RANDOM(1000)
    amplitude=newval1
    frequency=newval2
    CLR oldx,k
    @perlin
  NEXT inc
  oldrange=range(0)
  COLOR COLOR_RGB(col/256,col/256,col/256)
  FOR i= 0 TO 599
    LINE i-1,oldrange+lowmount,i,range(i)+lowmount
    LINE i-1,oldrange+lowmount,i-1,600
    oldrange=range(i)
  NEXT i
  ADD lowmount,lowr
  ADD lowr,25
  CLR range()
  SUB col,23
NEXT chain
laker=col+23
lakeg=col+23
lakeb=100
decrease=(lakeb-(col+23))/100
FOR i=1 TO 100
  COLOR COLOR_RGB(laker/256,lakeg/256,lakeb/256)
  LINE 0,600-i,600,600-i
  SUB lakeb,decrease
NEXT i
SHOWPAGE
KEYEVENT
QUIT


PROCEDURE myRnd
  seed=(221*seed)+2113
  seed=seed-(INT(seed/10000)*10000)
  a=seed/10000
RETURN

PROCEDURE perlin
  seed=(221*seed)+2113
  seed=seed-(INT(seed/10000)*10000)
  a=seed/10000
  pointb=a
  FOR zz= 1 TO frequency
    pointa=pointb
    @myRnd
    pointb=a
    FOR x=0 TO 1 STEP (1/(600/frequency))
      IF sw=1
        ft=x*PI
        f=(1-COS(ft))*0.5
        interp=pointa*(1-f)+pointb*f
      ELSE
        interp=pointa*(1-x)+pointb*x
      ENDIF
      range(k)=range(k)+(interp*amplitude)
      INC k
    NEXT x
    ADD oldx,(600/frequency)
  NEXT zz
RETURN

