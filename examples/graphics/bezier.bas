' Draws a qubic bezier curve 2011-09-23 by Markus Hoffmann
'
DIM px(4),py(4)
DIM p2x(400),p2y(400)

schwarz=COLOR_RGB(0,0,0)
gelb=COLOR_RGB(1,1,0)
grau=COLOR_RGB(2/6,2/6,2/6)
DO
  COLOR schwarz
  PBOX 0,0,640,400
  CLR count
  px(0)=100
  py(0)=100
  px(1)=MOUSEX
  py(1)=MOUSEY
  px(2)=200
  py(2)=100
  px(3)=200
  py(3)=200
  COLOR grau
  POLYLINE 4,px(),py()
  FOR t=0 TO 1 STEP 0.02
    p2x(count)=px(0)+(-px(0)+3*px(1)-3*px(2)+px(3))*t^3+(3*px(0)-6*px(1)+3*px(2))*t^2+(-3*px(0)+3*px(1))*t 
    p2y(count)=py(0)+(-py(0)+3*py(1)-3*py(2)+py(3))*t^3+(3*py(0)-6*py(1)+3*py(2))*t^2+(-3*py(0)+3*py(1))*t
    INC count
  NEXT t
  COLOR grau
  POLYFILL count,p2x(),p2y()
  COLOR gelb
  POLYLINE count,p2x(),p2y()
  SHOWPAGE
  MOTIONEVENT
LOOP
~INP(-2)
QUIT
