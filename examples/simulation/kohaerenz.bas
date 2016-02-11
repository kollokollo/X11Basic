dim px(10),py(10)
bx=200
by=200
sizew ,bx,by
anzpunkt=3

lambda=50

px(0)=90
py(0)=100
px(1)=130
py(1)=100
px(2)=110
py(2)=130
rot=get_color(65535,0,0)
weiss=get_color(65535,65535,65535)
gelb=get_color(65535/2,65535/2,0)
dim feld(bx,by)
arrayfill feld(),-1
counter=0
do
while feld(x,y)>=0
x=random(bx)
y=random(by)
wend

'for x=0 to bx
'  for y=0 to by
clr sum
for i=0 to anzpunkt-1
add sum,cos(2*pi*(sqrt((px(i)-x)^2+(py(i)-y)^2)/lambda))
next i
 
  sum=sum/anzpunkt
 ' if sum^2<0.1
 '   color weiss
 ' else if sum^2<0.4
 '   color gelb
 ' else 
 ' color rot
 ' endif
  feld(x,y)=sum^2
  color int(sum^2*20)*10
  plot x,y
'  next y
inc counter
if timer-t>5
  vsync
  t=timer
print str$(counter/bx/by*100,3,3)
endif
loop
  vsync
'next z
