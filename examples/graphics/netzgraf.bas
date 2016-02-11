cls                         
sx=100
sy=sx*42/60
DIM x(sx+1,sy+1)
DIM y(sx+1,sy+1)
dx=12/sx
dy=12/sy
m=120

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
    z=m*SIN(i/2/pi)*SIN(j/2/pi)    
    x(i,j)=200+(i-j)*2*s
    y(i,j)=100+j*s-z
    ' print i,j,x(i,j),y(i,j)
  NEXT j
  PRINT i''chr$(13);
  flush
NEXT i

LINE 200,0,200,y(0,0)
vsync
FOR i=sx DOWNTO 1
  FOR j=sy DOWNTO 1
    y(i-1,j-1)=MIN(y(i-1,j-1),y(i,j))
  NEXT j
  PRINT i''chr$(13);
  flush
NEXT i
'FILL 1,21
FOR i=0 TO sx-1
  FOR j=0 TO sy-1
    LINE x(i,j),y(i,j),x(i+1,j),y(i+1,j)
    LINE x(i,j),y(i,j),x(i,j+1),y(i,j+1)
  NEXT j
  PRINT i''chr$(13);
  flush
  vsync
NEXT i
vsync
alert 0,"Fertig",1,"OK",balert
quit
