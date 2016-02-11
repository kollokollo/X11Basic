' Draws a qubic bezier curve 2011 by Markus Hoffmann
'
dim px(4),py(4)

dim p2x(400),p2y(400)

schwarz=get_color(0,0,0)
gelb=get_color(65535,65535,0)
grau=get_color(20000,20000,20000)
do
  color schwarz
  pbox 0,0,640,400
  clr count

  px(0)=100
  py(0)=100
  px(1)=mousex
  py(1)=mousey
  px(2)=200
  py(2)=100
  px(3)=200
  py(3)=200
  color grau
  polyline 4,px(),py()
  for t=0 to 1 step 0.02
    p2x(count)=px(0)+(-px(0)+3*px(1)-3*px(2)+px(3))*t^3+(3*px(0)-6*px(1)+3*px(2))*t^2+(-3*px(0)+3*px(1))*t 
    p2y(count)=py(0)+(-py(0)+3*py(1)-3*py(2)+py(3))*t^3+(3*py(0)-6*py(1)+3*py(2))*t^2+(-3*py(0)+3*py(1))*t
    inc count
  next t
  color grau
  polyfill count,p2x(),p2y()
  color gelb
  polyline count,p2x(),p2y()
  vsync
  motionevent
loop
~inp(-2)
quit
