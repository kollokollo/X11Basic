' Programm berechnet RDS-Stereogramme  (c) Markus Hoffmann
' Theorie zum Programm siehe: http://cips02.physik.uni-bonn.de/~hoffmann/rds/rds.html
'

echo off
'
'
bx=0
by=0
bw=640
bh=400
mt=120
dim zeil(bw+mt)
usewindow 0
usewindow 1
vsync

usewindow 2
color 0
pbox  0,0,640,400
color 1
pcircle 320,200,200
vsync
for y=0 to bh
print y'
usewindow 0
for x=0 to mt
  zeil(x)=point(x,y)
next x
print "."'
usewindow 1
for x=0 to bw
  ' f=point(x,y)*70
   f=mt/4*EXP(-SQR((x-(bw/2))^2+(y-(bh/2))^2)/100)*COS(SQR((x-(bw/2))^2+(y&-(bh/2))^2)/30)^2
   zeil(x+mt-f)=zeil(x)
next x
'usewindow 1
print "1"
for x=0 to bw
   color zeil(x)
  plot x,y
next x
vsync
next y
fileselect "Speichere Grafik als ...","./*.dmp","window.dmp",f$
if len(f$)
  savewindow f$
endif
quit
function f(ox,oy)
return mt/4*EXP(-SQR((x-(bw/2))^2+(y-(bh/2))^2)/100)*COS(SQR((x-(bw/2))^2+(y&-(bh/2))^2)/30)^2
