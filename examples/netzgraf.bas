echo off  
cls                         
sx=100
sy=sx*42/60
DIM x(sx,sy)
DIM y(sx,sy)
dx=12/sx
dy=12/sy
m=120
mt=500
bw=640
bh=400

gelb=get_color(65535,65535,0)
gruen=get_color(10000,65535,10000)

print "Netzgraf   von Markus Hoffmann  1987 "
s=60/sx*4.5
clearw 
LINE 200,0,639,0
LINE 200,0,0,100
color gelb
text 200,200,"Netzgraf   von Markus Hoffmann  1987 "
color gruen
vsync
FOR i=0 TO sx
  FOR j=0 TO sy
  
    z=mt/4*EXP(-SQR((i-50)^2+(j-40)^2)/15)*COS(SQR((i-50)^2+(j-40)^2)/2)^2
   ' z=m*SIN(i/2/pi)*SIN(j/2/pi)
    
    x(i,j)=200+(i-j)*2*s
    y(i,j)=100+j*s-z
  NEXT j
  PRINT AT(2,1);i''
  flush
NEXT i
LINE 200,0,200,y(0,0)
print "down"
vsync
FOR i=sx DOWNTO 1
  FOR j=sy DOWNTO 1
    y(i-1,j-1)=MIN(y(i-1,j-1),y(i,j))
  NEXT j
  PRINT AT(3,1);i''
  flush
NEXT i
'FILL 1,21
FOR i=0 TO sx-1
  FOR j=0 TO sy-1
    LINE x(i,j),y(i,j),x(i+1,j),y(i+1,j)
    LINE x(i,j),y(i,j),x(i,j+1),y(i,j+1)
  NEXT j
  PRINT AT(4,1);i''
  flush
  vsync
NEXT i
vsync

alert 0,"Fertig",1,"OK",balert
quit
