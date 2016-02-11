weiss=COLOR_RGB(1,1,1)
schwarz=COLOR_RGB(0,0,0)
yellow=COLOR_RGB(1/2,1/2,1/2)

FOR i=0 TO 8
  GRAPHMODE 1
  COLOR schwarz
  PBOX 0,0,320,400
  COLOR weiss
  PBOX 320,0,640,400
  COLOR schwarz,yellow
  TEXT 317,200," "+STR$(i)+" "
  GRAPHMODE i
  COLOR weiss,yellow
  PBOX 100,100,200,200
  TEXT 145,155,"white"
  PBOX 150,150,250,250
  TEXT 200,200,"white"

  PBOX 300,10,340,30
  TEXT 310,20,"white"

  COLOR schwarz,yellow
  PBOX 420,100,520,200
  TEXT 470,150,"black"

  PBOX 470,150,570,250
  TEXT 520,200,"black"

  PBOX 300,50,340,80
  TEXT 310,65,"black"

  SHOWPAGE
  PAUSE 2
NEXT i
QUIT
