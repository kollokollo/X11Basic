' Zeichnet Praezession auf Kugeloberflaeche (c) Markus Hoffmann 2001


bw=600
gucktheta=80/180*pi
guckphi=10/180*pi


rot=get_color(65530,0,0)
gelb=get_color(65530,40000,0)
grau=get_color(65530/1.5,65530/1.5,65530/1.5)
blau=get_color(0,0,65530)
weiss=get_color(65530,65530,65530)
schwarz=get_color(0,0,0)
lila=get_color(65530,0,65530)
gruen=get_color(0,65530,0)
sizew 1,bw,bw
vsync
get_geometry 1,bx,by,bw,bh
color weiss
pbox bx,by,bw,bh
color schwarz
circle bw/2,bh/2,min(bw,bh)/2-1
color grau

line @kx(0,0,0),@ky(0,0,0),@kx(1,0,0),@ky(1,0,0)
line @kx(0,0,0),@ky(0,0,0),@kx(1,-pi/2,0),@ky(1,-pi/2,0)
line @kx(0,0,0),@ky(0,0,0),@kx(1,pi/2,0),@ky(1,pi/2,0)
line @kx(0,0,0),@ky(0,0,0),@kx(1,0,-pi),@ky(1,0,-pi)
line @kx(0,0,0),@ky(0,0,0),@kx(1,0,-pi*1.5),@ky(1,0,-pi*1.5)
line @kx(0,0,0),@ky(0,0,0),@kx(1,0,-pi*0.5),@ky(1,0,-pi*0.5)


for i=0 to 360 step 10  
for j=-60 to -30 step 10
  line @kx(1,j/180*pi,i/180*pi),@ky(1,j/180*pi,i/180*pi),@kx(1,(j-10)/180*pi,i/180*pi),@ky(1,(j-10)/180*pi,i/180*pi)
  line @kx(1,j/180*pi,(i-10)/180*pi),@ky(1,j/180*pi,(i-10)/180*pi),@kx(1,(j)/180*pi,i/180*pi),@ky(1,(j)/180*pi,i/180*pi)
next j
vsync
next i

defline ,4
color rot

for i=0 to 360   
  j=@f(i)
  ii=i-1
  jj=@f(ii)
  line @kx(1,j/180*pi,i/180*pi),@ky(1,j/180*pi,i/180*pi),@kx(1,jj/180*pi,ii/180*pi),@ky(1,jj/180*pi,ii/180*pi)
  vsync
next i

color blau
for i=0 to 360 step 2  
  j=-30
  ii=i-1
  jj=-30
  line @kx(1,j/180*pi,i/180*pi),@ky(1,j/180*pi,i/180*pi),@kx(1,jj/180*pi,ii/180*pi),@ky(1,jj/180*pi,ii/180*pi)
  vsync
next i
for i=0 to 360   step 4
  j=-70
  ii=i-1
  jj=-70
  line @kx(1,j/180*pi,i/180*pi),@ky(1,j/180*pi,i/180*pi),@kx(1,jj/180*pi,ii/180*pi),@ky(1,jj/180*pi,ii/180*pi)
  vsync
next i
vsync
color schwarz

line @kx(0,0,0),@ky(0,0,0),@kx(1,-pi/2,0),@ky(1,-pi/2,0)

for i=0 to 360 step 10
line @kx(0.9,-85/180*pi,i/180*pi),@ky(0.9,-85/180*pi,i/180*pi),@kx(1,-pi/2,0),@ky(1,-pi/2,0)
next i  

vsync
end

function f(x)
  return sin(x/180*pi*6)*20-50
endfunc


function kx(r,theta,phi)

x=r*cos(theta)*cos(phi)
y=r*cos(theta)*sin(phi)
z=r*sin(theta)
xx=sin(guckphi)*x+cos(guckphi)*y
'y=cos(guckphi)*x-sin(guckphi)*y
x=xx


'yy=sin(gucktheta)*y+cos(gucktheta)*z
'z=cos(gucktheta)*y-sin(gucktheta)*z
'y=yy

'if y>0
'  defline ,,2
'endif
  return bw/2+(x)*bw/2
endfunc
function ky(r,theta,phi)

x=r*cos(theta)*cos(phi)
y=r*cos(theta)*sin(phi)
z=r*sin(theta)
'xx=sin(guckphi)*x+cos(guckphi)*y
y=cos(guckphi)*x-sin(guckphi)*y
'x=xx

'yy=sin(gucktheta)*y+cos(gucktheta)*z
z=cos(gucktheta)*y-sin(gucktheta)*z
'if yy>0
'color weiss
'else
'color blau
'endif
  return bh/2-(z)*bh/2
endfunc
