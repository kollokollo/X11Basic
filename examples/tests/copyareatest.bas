' rootwindow
clearw 1
color get_color(65535,65535,0)
pcircle 400,10,10
color get_color(65535,0,0)
for i=0 to 5
  pcircle i*20+10,4,4
  line i*20+10,4,i*20+10-2*i,20
  line i*20+10,4,i*20+10+2*i,20
  copyarea i*20,0,20,20,(10-i)*20,0
next i

color get_color(65535,65535,0)
line 150,4,150,0
for i=0 to 10
color get_color(i*6553,32000+i*6553/2,i*6553)
pcircle 350+i/3,20-i/3,20-2*i
next i
copyarea 330,0,40,40,200,200
text 100,170,"Copyarea mit XBASIC V.1.03   (c) Markus Hoffmann"
i=0
repeat
  copyarea 600,380,20,20,x,y+20
  mouse x,y,k
  copyarea x,y+20,20,20,600,380
  graphmode 2
  copyarea i*20,0,20,20,x,y+20
  graphmode 1
  vsync
  pause 0.1
  inc i
  i=i mod 10
until k<>0

quit
