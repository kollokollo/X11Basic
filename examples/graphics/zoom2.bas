faktor=0.3
winkel=pi/7
posx=300
posy=200

s=sin(winkel)*faktor
c=cos(winkel)*faktor

for x=0 to 639 
for y=0 to 399
ox=c*x+s*y+posx
oy=-s*x+c*y+posy

rootwindow
a=point(ox,oy)
usewindow 1
color a
plot x,y
next y
vsync
next x
