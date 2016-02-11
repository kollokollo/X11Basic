' fractal 2011

gelb=COLOR_RGB(1,0.5,0)
schwarz=COLOR_RGB(0,0.5,0)
rot=COLOR_RGB(1,0,0)
weiss=COLOR_RGB(1,1,0.5)

@figur(320,200,0,1)
SHOWPAGE
~INP(-2)
QUIT



procedure figur(ox,oy,r,scale)
  dim x(5),y(5)
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
  
  for i=0 to 4
    xx=cos(r)*x(i)+sin(r)*y(i)
    y(i)=-sin(r)*x(i)+cos(r)*y(i)
    x(i)=xx
  next i
  color weiss
  polyfill 5,x(),y(),ox,oy
  color gelb
  polyline 5,x(),y(),ox,oy
  SHOWPAGE
  if scale>0.005
    @figur(scale*100+ox,oy,pi/2,scale/2)
    @figur(ox,oy+scale*100,pi/2,scale/2)
    @figur(-scale*100+ox,oy,pi/2,scale/2)
    @figur(ox,oy-scale*100,pi/2,scale/2)
  endif
'  pause 0.1
return
