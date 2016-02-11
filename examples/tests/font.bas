color 0
pbox 0,0,640,400
deftext 1
color get_color(65535,10000,10000)
deftext ,0.08,0.16,0
defline ,2,2,2
for x=0 to 15
  for y=0 to 15
    ltext 8*y,16*x,chr$(y+16*x)
setfont "BIG"
    text 160+8*y,16*x,chr$(y+16*x)
setfont "MEDIUM"
    text 300+8*y,16*x,chr$(y+16*x)
setfont "SMALL"
    text 440+8*y,16*x,chr$(y+16*x)
    vsync
  next y
next x
pause 5
alert 0,"Fertig !",1," OK ",dummy
quit
