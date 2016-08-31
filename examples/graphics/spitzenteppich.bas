' fractal 2011

gelb=COLOR_RGB(1,0.5,0)
schwarz=COLOR_RGB(0,0.5,0)
rot=COLOR_RGB(1,0,0)
weiss=COLOR_RGB(1,1,0.5)

@figur(320,200,0,1)
SHOWPAGE
~INP(-2)
QUIT

PROCEDURE figur(ox,oy,r,scale)
  DIM x(5),y(5)
  x(0)=-scale*100/2
  y(0)=-scale*100/2
  x(1)=scale*100/2
  y(1)=-scale*100/2
  x(2)=scale *100/2
  y(2)=scale*100/2
  x(3)=-scale*100/2
  y(3)=scale*100/2
  x(4)=-scale*100/2
  y(4)=-scale*100/2

  FOR i=0 TO 4
    xx=cos(r)*x(i)+sin(r)*y(i)
    y(i)=-sin(r)*x(i)+cos(r)*y(i)
    x(i)=xx
  NEXT i
  COLOR weiss
  POLYFILL 5,x(),y(),ox,oy
  COLOR gelb
  POLYLINE 5,x(),y(),ox,oy
  SHOWPAGE
  IF scale>0.005
    @figur(scale*100+ox,oy,pi/2,scale/2)
    @figur(ox,oy+scale*100,pi/2,scale/2)
    @figur(-scale*100+ox,oy,pi/2,scale/2)
    @figur(ox,oy-scale*100,pi/2,scale/2)
  ENDIF
  '  pause 0.1
RETURN
