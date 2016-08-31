' fractal tree (X11-Basic)

RANDOMIZE TIMER

DIM si(10),sx(10),sy(10),st(10)
DIM shf(10),sa(10),sb(10),sh(10)
DIM sd(10)
rot=COLOR_RGB(1,0,0)
SHOWPAGE
PAUSE 1
GET_GEOMETRY 1,bx,by,xmax,ymax

' TREE DATA
rand=1
IF ymax>320
  ' big screen
  brafan=ymax/5
  inpfac=0.75
  inphf=0.8
  depth=6
  density=5
ELSE
  ' small screen
  brafan=xmax/3.2
  inpfac=1
  inphf=1
  depth=4
  density=3
ENDIF

l=pi/180
a=90
h=ymax/4
nx=xmax/2
ny=1

CLEARW
TEXT 0,30,"random tree with X11-Basic"
flowercolor=COLOR_RGB(1,1,0)
COLOR rot
PLOT xmax/2,ymax
oxx=xmax/2
oyy=ymax
SHOWPAGE
@tree
END

PROCEDURE tree
  IF depth=0
    ' flower
    COLOR flowercolor
    PLOT nx-2,ymax-ny
    PLOT nx,ymax-(ny+3)
    PLOT nx+2,ymax-ny
    PLOT nx,ymax-ny
    ' oxx=nx
    ' oyy=ymax-ny
    COLOR rot
    SHOWPAGE
  ELSE
    si(stackpointer)=i
    sx(stackpointer)=x
    sy(stackpointer)=y
    st(stackpointer)=th
    shf(stackpointer)=hf
    INC stackpointer
    start=a-brafan/2
    th=brafan/density
    IF depth<=2
      hf=inphf/2
    ELSE
      hf=inphf
    ENDIF
    x=h*COS(a*l)
    y=h*SIN(a*l)
    a=start
    i=1
    500:
    nx=nx+x
    ny=ny+y
    LINE oxx,oyy,nx,ymax-ny
    ' plot nx,ymax-ny
    oxx=nx
    oyy=ymax-ny
    SHOWPAGE
    @brrand
    @tree
    a=sa(stackpointer)
    brafan=sb(stackpointer)
    h=sh(stackpointer)
    depth=sd(stackpointer)
    a=a+th
    nx=nx-x
    ny=ny-y
    PLOT nx,ymax-ny
    oxx=nx
    oyy=ymax-ny
    INC i
    IF i<=density+1
      GOTO 500
    ENDIF
    DEC stackpointer
    i=si(stackpointer)
    x=sx(stackpointer)
    y=sy(stackpointer)
    th=st(stackpointer)
    hf=shf(stackpointer)
    brafan=sb(stackpointer)
  ENDIF
RETURN

PROCEDURE brrand
  sa(stackpointer)=a
  sb(stackpointer)=brafan
  sh(stackpointer)=h
  sd(stackpointer)=depth
  IF rand=0
    brafan=brafan*inpfac
    h=h*hf
  ELSE
    a=a-th/2+RND(0)*th+1
    brafan=brafan*inpfac
    h=h*hf*RND(0)
  ENDIF
  DEC depth
RETURN
