' This program codes a File for use for the INLINE$()-Function in
' X11-Basic V.1.06                    (c) Markus Hoffmann


f$=param$(2)
if exist(f$)
  open "I",#1,f$
  l=lof(#1)
  print "' output of inline.bas for X11-Basic "+date$
  print "' ";f$;" ";l;" Bytes."
  wort_sep f$,".",0,n$,b$    
  if rinstr(n$,"/")
    n$=right$(n$,len(n$)-rinstr(n$,"/"))
  endif
  t$=""
  print n$+"$="+chr$(34)+chr$(34)
  while l>3
    a$=input$(#1,3)
    t$=t$+chr$(36+(peek(varptr(a$)) and 0xfc)/4)
    t$=t$+chr$(36+(peek(varptr(a$)) and 0x3)*16+(peek(varptr(a$)+1) and 0xf0)/16)
    t$=t$+chr$(36+(peek(varptr(a$)+1) and 0xf)*4+(peek(varptr(a$)+2) and 0xc0)/64)
    t$=t$+chr$(36+(peek(varptr(a$)+2) and 0x3f))
    sub l,3
    if len(t$)>63
      print n$+"$="+n$+"$+"+chr$(34)+t$+chr$(34)
      t$=""
    endif
  wend
  if l
    a$=input$(#1,l)+mkl$(0)
    t$=t$+chr$(36+(peek(varptr(a$)) and 0xfc)/4)
    t$=t$+chr$(36+(peek(varptr(a$)) and 0x3)*16+(peek(varptr(a$)+1) and 0xf0)/16)
    t$=t$+chr$(36+(peek(varptr(a$)+1) and 0xf)*4+(peek(varptr(a$)+2) and 0xc0)/64)
    t$=t$+chr$(36+(peek(varptr(a$)+2) and 0x3f))
    print n$+"$="+n$+"$+"+chr$(34)+t$+chr$(34)
  endif
  close
else
  print f$;" existiert nicht."
  print "Usage: xbasic inline.bas filename"
endif
quit
