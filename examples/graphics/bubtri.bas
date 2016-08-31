' bubbles and triangles.bas for SmallBASIC MGA/B+ 2015-07-12
' original Bubbles Particles by PeterMaria Alias Peter Wirbelauer 11.o7.2o15
' converted to X11-Basic 2015
'
' color 11,0
xmax=640
ymax=400
bm=500
b1=bm-1
DIM bx(bm),by(bm),bdx(bm),bdy(bm)
tm=250
t1=tm-1
DIM tx(tm),ty(tm),tos(tm),tc(tm)
DIM colors(16)
colors(0)=COLOR_RGB(0,0,0)
colors(1)=COLOR_RGB(1,1,0)
colors(9)=COLOR_RGB(0,0,1)
colors(11)=COLOR_RGB(1,0,0)
colors(12)=COLOR_RGB(0,1,0)
colors(14)=COLOR_RGB(0,1,1)
tx(0)=10
ty(0)=5
tstart=1
k$="a"
' pen on
WHILE MOUSEK=0
  COLOR colors(0)
  PBOX 0,0,xmax,ymax
  COLOR colors(1)
  TEXT 50,20,"bubbles and triangles MGA/B+"
  FOR i=1 TO 10
    @newb(RANDOM(bm))
  NEXT i
  FOR i=0 TO b1
    IF bdy(i)
      COLOR colors(11)
      CIRCLE bx(i),by(i),5
      bx(i)=bx(i)+bdx(i)
      by(i)=by(i)+bdy(i)
      IF bx(i)<0 OR bx(i)>xmax OR by(i)<0 OR by(i)>ymax
        bdy(i)=0
        IF bx(i)>0 AND bx(i)<xmax
          tx(tstart)=bx(i)-bdx(i)
          ty(tstart)=5
          tos(tstart)=0
          rc=RND()
          IF rc<0.333
            tc(tstart)=12
          ELSE IF rc<0.667
            tc(tstart)=9
          ELSE
            tc(tstart)=14
          ENDIF
          INC tstart
          IF tstart>t1
            tstart=0
          ENDIF
        ENDIF
      ENDIF
    ENDIF
  NEXT i
  FOR i=0 TO t1
    IF tx(i)<>0 OR ty(i)<>0
      @tri(tx(i),ty(i),tos(i),tc(i))
      IF ty(i)<ymax-10
        ty(i)=ty(i)+7
        tos(i)=tos(i)+0.18
      ENDIF
    ENDIF
  NEXT i
  SHOWPAGE
  PAUSE 0.1
WEND
' pen off
PRINT "done"
END
PROCEDURE newb(i)
  bx(i)=MOUSEX
  by(i)=MOUSEY
  bdx(i)=RANDOM(11)-5
  bdy(i)=-RANDOM(5)-1
RETURN
PROCEDURE tri(xc,yc,toff,colr)
  LOCAL i,t,a,b,x,y
  DIM x(3),y(3)
  FOR i=0 TO 2
    t=COS(i*2*PI/3+toff)
    x(i)=xc+t*10
    t=SIN(i*2*PI/3+toff)
    y(i)=yc+t*10
  NEXT i
  COLOR colors(colr)
  FOR a=0 TO 2
    b=a+1
    IF b>2
      b=0
    ENDIF
    LINE x(a),y(a),x(b),y(b)
  NEXT a
RETURN

