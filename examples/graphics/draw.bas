'
' X11-Basic Example Program (c) Markus Hoffmann 2003
'
' demonstrates the use of Mouse functions: MOUSEEVENT MOTIONEVENT and
' drawing function DRAW TO
'
CLEARW
DEFMOUSE 3
DEFLINE ,5
i=1
weiss=COLOR_RGB(1,1,1)
gelb=COLOR_RGB(1,1,0)
schwarz=COLOR_RGB(0,0,0)
COLOR gelb
TEXT 100,100,"Draw with Mouse, Middle Mouse Button=QUIT, right=change color"
SHOWPAGE
DO
  MOUSEEVENT x,y,k,,s
  PRINT "Klick:",x,y,k
  IF k=1
    DRAW x,y
    SHOWPAGE
    WHILE k=1
      MOTIONEVENT x,y,k,,s
      PRINT x,y,k
      DRAW TO x,y
      SHOWPAGE
    WEND
  ENDIF
  IF k=3
    COLOR schwarz
    PBOX 0,0,30,10
    IF s=4         ! control gedrueckt ?
      DEC i
    ELSE
      INC i
    ENDIF
    COLOR weiss
    TEXT 0,10,STR$(i)
    SHOWPAGE
    COLOR COLOR_RGB(RND(),RND(),RND())
  ENDIF
  EXIT IF k=2
LOOP
QUIT
