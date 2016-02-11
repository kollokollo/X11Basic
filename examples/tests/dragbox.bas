' Okt 12  2003 dragbox.bas
'

gelb=COLOR_RGB(1,1,0)
schwarz=COLOR_RGB(0,0,0)
CLEARW
do
  k=0
  TEXT 100,100,"DRAGBOX"
  SHOWPAGE
  WHILE k=0     ! Wait until a MOUSE button is pressed
    MOUSEEVENT sx,sy,k
  WEND
  ' set the starting point
  EXIT IF k>1   ! if right button was pressed, exit
  ' clear the event queue
  while event?(64)
    MOTIONEVENT
  wend
  DO
    MOTIONEVENT x,y,k
    COLOR gelb
    RBOX sx,sy,x,y
    SHOWPAGE
    IF k=1
      COLOR schwarz
      RBOX sx,sy,x,y
    ELSE
      BREAK
    ENDIF
  LOOP
LOOP
QUIT
