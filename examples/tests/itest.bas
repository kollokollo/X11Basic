' test of the coding for the INLINE$() function in
' X11-Basic V.1.22 by Markus Hoffmann 04/2014
'

FOR i=0 TO 10
  a$=@radix$(SPACE$(i))
  PRINT i,LEN(a$),a$,inline$(a$),LEN(inline$(a$))
NEXT i
QUIT

FUNCTION radix$(g$)
  LOCAL pointer
  LOCAL t$,l
  l=LEN(g$)
  pointer=0
  CLR t$
  WHILE l>=3
    t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0xfc)/4)
    t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0x3)*16+(PEEK(VARPTR(g$)+pointer*3+1) AND 0xf0)/16)
    t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+1) AND 0xf)*4+(PEEK(VARPTR(g$)+pointer*3+2) AND 0xc0)/64)
    t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+2) AND 0x3f))
    SUB l,3
    INC pointer
  WEND
  IF l
    t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0xfc)/4)
    IF l=1
      t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0x3)*16)
    ELSE
      t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0x3)*16+(PEEK(VARPTR(g$)+pointer*3+1) AND 0xf0)/16)
      IF l=2
        t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+1) AND 0xf)*4)
      ELSE
        t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+1) AND 0xf)*4+(PEEK(VARPTR(g$)+pointer*3+2) AND 0xc0)/64)
        t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+2) AND 0x3f))
      ENDIF
    ENDIF
  ENDIF
  RETURN t$
ENDFUNCTION
