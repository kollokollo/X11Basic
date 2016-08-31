' Demonstration of polyline and polymark      Feb 12  2005
' written in X11Basic (c) Markus Hoffmann
'
'
DIM polyline$(1000)
CLR anzpolyline
RANDOMIZE TIMER
COLOR COLOR_RGB(1/2,1/2,1/2)
PBOX 0,0,640,400

nn=200

FOR j=0 TO 10
  typ=RANDOM(4)
  t$=CHR$(typ)
  t$=t$+MKI$(nn)
  FOR i=0 TO nn-1
    t$=t$+MKF$(50-j*20+100*COS(j*2*PI*i/nn))+MKF$(50-j*20+100*SIN(2*PI*(j+1)*i/nn))
  NEXT i
  polyline$(anzpolyline)=t$
  INC anzpolyline
NEXT j

FOR i=0 TO anzpolyline-1
  @draw_polyline(polyline$(i),1)
NEXT i
FOR i=0 TO anzpolyline-1
  @draw_polyline(polyline$(i),2)
NEXT i
SHOWPAGE
PAUSE 2
RUN

' This procedure draws different types of combined polylines,
' which can be used to draw street maps and such.
' If flag=1 only the background part is drawn.
' If flag=2 only the forground part is draw
'
PROCEDURE draw_polyline(p$,flag)
  LOCAL n,typ,i
  typ=ASC(LEFT$(p$))
  n=CVI(MID$(p$,2,4))
  DIM px(n),py(n)
  FOR i=0 TO n-1
    px(i)=CVF(MID$(p$,4+8*i,4))
    py(i)=CVF(MID$(p$,4+8*i+4,4))
  NEXT i
  IF typ=0
    COLOR COLOR_RGB(1/4,1/4,1/4)
    DEFLINE ,1,2
    POLYLINE n,px(),py(),320,200
  ELSE IF typ=1
    IF flag<>2
      COLOR COLOR_RGB(1/4,1/4,1/4)
      DEFLINE ,6,2
      POLYLINE n,px(),py(),320,200
    ENDIF
    IF flag<>1
      COLOR COLOR_RGB(1/1.5,1/1.5,1/1.5)
      DEFLINE ,4,2
      POLYLINE n,px(),py(),320,200
    ENDIF
  ELSE IF typ=2
    IF flag<>2
      COLOR COLOR_RGB(1,1,0)
      DEFLINE ,8,2
      POLYLINE n,px(),py(),320,200
    ENDIF
    COLOR COLOR_RGB(1,0,0)
    DEFLINE ,6,2
    POLYLINE n,px(),py(),320,200
    IF flag<>1
      COLOR COLOR_RGB(1/1.5,1/1.5,1/1.5)
      DEFLINE ,1,2
      POLYLINE n,px(),py(),320,200
    ENDIF
  ELSE IF typ=3
    COLOR COLOR_RGB(1,1,0)
    DEFLINE ,1,2
    DEFMARK ,3,5
    POLYMARK n,px(),py(),320,250
  ENDIF
RETURN
