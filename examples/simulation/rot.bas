' Test der Vektorrotationsroutinen
'
' (c) Markus Hoffmann im Nov. 1999
'
'
bw=240
bh=240

sizew 1,bw,bh
clearw



x1=0
x2=1
x3=0

i=0
do

n1=mousex-bw/2
n2=mousey-bh/2
n3=sqrt(n1^2+n2^2)-100


betrag=sqrt(n1^2+n2^2+n3^2)

n1=n1/betrag
n2=n2/betrag
n3=n3/betrag
alpha=25*pi/180
inc i
if n3<x3
  color get_color(65535,65535,65535)

  line bw/2,bh/2,bw/2+x1*100,bh/2+x2*100
  pcircle bw/2+x1*100,bw/2+x2*100,5*(-x3+1)

  color get_color(65535,0,0)
  line bw/2,bh/2,bw/2+n1*100,bh/2+n2*100
  pcircle bw/2+n1*100,bh/2+n2*100,5*(-n3+1)
else
  color get_color(65535,0,0)
  line bw/2,bh/2,bw/2+n1*100,bh/2+n2*100
  pcircle bw/2+n1*100,bh/2+n2*100,5*(-n3+1)
  color get_color(65535,65535,65535)

  line bw/2,bh/2,bw/2+x1*100,bh/2+x2*100
  pcircle bw/2+x1*100,bw/2+x2*100,5*(-x3+1)

endif
  vsync
   color get_color(0,0,0)
 
  pbox 0,0,bw,bh
  color get_color(65535,65535,0)
  box bw/2-100,bh/2-100,bw/2+100,bh/2+100
  color get_color(65535,65535,65535)
  sa=sin(alpha)
  ca=cos(alpha)-1
  xs1=(1+ca*(n3^2+n2^2))*x1+(-n3*sa-n2*n1*ca)*x2+(n2*sa-n3*n1*ca)*x3
  xs2=(n3*sa-ca*n1*n2)*x1+(1+ca*(n3^2+n1^2))*x2+(-n1*sa-n2*n3*ca)*x3
  xs3=(-n2*sa-n3*n1*ca)*x1+(n1*sa-n2*n3*ca)*x2+(1+ca*(n2^2+n1^2))*x3
  x1=xs1
  x2=xs2
  x3=xs3
  b=x1^2+x2^2+x3^2
  text bw-80,bh-7,str$(b)
loop
