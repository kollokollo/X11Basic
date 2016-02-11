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

r=5/180
deffill ,2,17
do
for i=0 to 4
xx=cos(r)*x(i)+sin(r)*y(i)
y(i)=-sin(r)*x(i)+cos(r)*y(i)
x(i)=xx
next i
color 0
pbox 0,0,400,400
color 1
polyFILL 5,x(),y(),200,200
color 2
polyline 5,x(),y(),200,200

vsync
loop
