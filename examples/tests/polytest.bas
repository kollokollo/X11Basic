
' Demonstration of POLYLINE and POLYFILL
' X11-Basic (c) Markus Hoffmann

dim x(5),y(5)
x(0)=-50
y(0)=-50
x(1)=50
y(1)=-50
x(2)=50
y(2)=50
x(3)=-50
y(3)=50
x(4)=-50
y(4)=-50
sizew ,400,400
r=5/180
gelb=get_color(65535,32000,0)
schwarz=get_color(0,32000,0)
rot=get_color(65535,0,0)
weiss=get_color(65535,65535,32000)
deffill ,2,17
do
  for i=0 to 4
    xx=cos(r)*x(i)+sin(r)*y(i)
    y(i)=-sin(r)*x(i)+cos(r)*y(i)
    x(i)=xx
  next i
  color schwarz
  pbox 0,0,400,400
  color weiss
  polyfill 5,x(),y(),200,200
  color gelb
  polyline 5,x(),y(),200,200
  vsync
  pause 0.02
loop
