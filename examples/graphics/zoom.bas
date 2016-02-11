faktor=3

for x=0 to 639 step faktor
for y=0 to 399 step faktor
ox=x/faktor
oy=y/faktor
rootwindow
a=point(ox,oy)
usewindow 1
color a
pbox x,y,x+faktor,y+faktor
next y
vsync
next x
