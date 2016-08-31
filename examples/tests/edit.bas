' Test einer Editierbaren eingabe mit Linienfont.
' (c) Markus Hoffmann 2000

weiss=GET_COLOR(65535,0,65535)
schwarz=GET_COLOR(0,0,0)
rot=GET_COLOR(65535,0,0)
ledit_curlen=40
DEFTEXT 1,0.1,0.3,0
okkk:
a$=@ledit$(a$,10,100)
IF ledit_status=2
  b$=@ledit$(b$,10,150)
  IF ledit_status=1
    GOTO okkk
  ENDIF
ENDIF
PRINT b$
QUIT

FUNCTION ledit$(t$,x,y)
  LOCAL t2$,cc,curpos,a,b,c$,curlen
  t2$=t$
  cc=LEN(t$)
  curpos=x+ltextlen(LEFT$(t$,cc))
  curlen=MAX(20,ledit_curlen)
  COLOR weiss
  LTEXT x,y,t$
  DO
    COLOR schwarz
    LTEXT x,y,t2$
    t2$=t$
    LINE curpos,y,curpos,y+curlen
    curpos=x+ltextlen(LEFT$(t$,cc))
    COLOR weiss
    LTEXT x,y,t2$
    COLOR rot
    LINE curpos,y,curpos,y+curlen
    VSYNC
    KEYEVENT a,b,c$
    COLOR schwarz
    IF b AND -256
      IF a=22    ! Bsp
        IF len(t2$)
          t$=LEFT$(t2$,cc-1)+RIGHT$(t2$,LEN(t2$)-cc)
          DEC cc
        ELSE
          BELL
        ENDIF
      ELSE if a=36    ! Ret
        ledit_status=0
        LINE curpos,y,curpos,y+curlen
        RETURN t2$
      ELSE if a=98
        ledit_status=1
        LINE curpos,y,curpos,y+curlen
        RETURN t2$
      ELSE if a=104
        ledit_status=2
        LINE curpos,y,curpos,y+curlen
        RETURN t2$
      ELSE if a=107   ! Del
        IF cc<len(t2$)
          t$=LEFT$(t2$,cc)+RIGHT$(t2$,LEN(t2$)-cc-1)
        ENDIF
      ELSE if a=100
        cc=MAX(0,cc-1)
      ELSE if a=102
        cc=MIN(LEN(t2$),cc+1)
      ENDIF
    ELSE
      t$=LEFT$(t2$,cc)+CHR$(b)+RIGHT$(t2$,LEN(t2$)-cc)
      INC cc
    ENDIF
    PRINT a,HEX$(b)
  LOOP
ENDFUNCTION
