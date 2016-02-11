' test of the coding for the INLINE$() function in 
' X11-Basic V.1.22 by Markus Hoffmann 04/2014
'

for i=0 to 10
  a$=@radix$(space$(i))
  print i,len(a$),a$,inline$(a$),len(inline$(a$))
next i
quit


function radix$(g$)
  local pointer
  local t$,l
  l=len(g$)
  pointer=0
  clr t$
    WHILE l>=3
      t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0xfc)/4)
      t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0x3)*16+(PEEK(VARPTR(g$)+pointer*3+1) and 0xf0)/16)
      t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+1) AND 0xf)*4+(PEEK(VARPTR(g$)+pointer*3+2) and 0xc0)/64)
      t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+2) AND 0x3f))
      SUB l,3
      INC pointer
    WEND
    IF l
      t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0xfc)/4)
      if l=1
        t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0x3)*16)
      else 
        t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0x3)*16+(PEEK(VARPTR(g$)+pointer*3+1) and 0xf0)/16)
        if l=2
          t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+1) AND 0xf)*4)
	else
          t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+1) AND 0xf)*4+(PEEK(VARPTR(g$)+pointer*3+2) and 0xc0)/64)
          t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+2) AND 0x3f))
        endif
      endif
    ENDIF
  return t$
endfunction
