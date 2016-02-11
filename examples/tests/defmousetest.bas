clearw 1
gelb=get_color(65535,40000,0)
weiss=get_color(65535,65535,65535)
schwarz=get_color(0,0,0)
color gelb
pbox 0,0,100,100
color weiss
pbox 100,0,200,100
text 150,150,"DEFMOUSE:   click here"
i=0
do
  color schwarz
  pbox 320,200,640,400
  color gelb
  defmouse i
  text 340,300,str$(i)
  vsync
  mouseevent x,y,k
  if k=1
  inc i
  endif
  if k=2
  dec i
  endif
loop
