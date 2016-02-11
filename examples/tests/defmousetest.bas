' Demonstration of DEFMOUSE in X11-Basic
'
CLEARW 1
gelb=COLOR_RGB(1,4/65,0)
weiss=COLOR_RGB(1,1,1)
schwarz=COLOR_RGB(0,0,0)
COLOR gelb
PBOX 0,0,100,100
COLOR weiss
PBOX 100,0,200,100
TEXT 32,150,"DEFMOUSE:   click here"
i=0
COLOR gelb,schwarz
DO
  DEFMOUSE i
  TEXT 190,160,str$(i)+" "
  SHOWPAGE
  MOUSEEVENT x,y,k
  IF k=1
    INC i
  ELSE IF k=2
    DEC i
  ELSE IF k=3
    QUIT
  ENDIF
LOOP
END
