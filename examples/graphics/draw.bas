
' X11-Basic Example Program (c) Markus Hoffmann 2003

clearw
defmouse 3
defline ,5
i=1
weiss=get_color(65535,65535,65535)
gelb=get_color(65535,65535,0)
schwarz=get_color(1000,0,1000)
color gelb
text 100,100,"Draw with Mouse, Middle Mouse Button=QUIT, right=change color"
vsync
do
  mouseevent x,y,k,,s
  print "Klick:",x,y,k
  if k=1
    draw x,y
    vsync
    while k=1
      motionevent x,y,k,,s
      print x,y,k
      draw to x,y
      vsync
    wend
  endif
  if k=3
    color schwarz
    pbox 0,0,30,10
    if s=4         ! control gedrueckt ?
      dec i
    else
      inc i
    endif
    color weiss
    text 0,10,str$(i)
    vsync
    color get_color(random(65536),random(65536),random(65536))
  endif
  exit if k=2
loop
quit
