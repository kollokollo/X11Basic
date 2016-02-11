COLOR COLOR_RGB(0,0,0)
PBOX 0,0,640,400
DEFTEXT 1
COLOR COLOR_RGB(1,1/6,1/6)
DEFTEXT ,0.08*0.9,0.16*0.9,0
'DEFLINE ,2,2,2
FOR x=0 to 15
  for y=0 to 15
    ltext 8*y,16*x,chr$(y+16*x)
setfont "BIG"
    text 160+8*y,16*x+16,chr$(y+16*x)
setfont "MEDIUM"
    text 300+8*y,16*x+16,chr$(y+16*x)
setfont "SMALL"
    text 440+8*y,16*x+16,chr$(y+16*x)
    SHOWPAGE
  next y
next x
PAUSE 5
ALERT 0,"Fertig !",1," OK ",dummy
QUIT
