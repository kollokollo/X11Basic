gelb=get_color(65535,65535,0)
schwarz=get_color(0,0,0)
text 100,100,"DRAGBOX"
do
  motionevent
  x=mousex
  y=mousey
  color gelb
  rbox 100,100,x,y
  vsync
  color schwarz
  rbox 100,100,x,y
  exit if mousek=2
loop
quit
