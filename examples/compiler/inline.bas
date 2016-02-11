' This program codes a File for use for the INLINE$()-Function in
' X11-Basic V.1.06                    (c) Markus Hoffmann

comp=0

i=2
while len(param$(i))
  @doit(param$(i))
  inc i
wend  

quit

procedure doit(f$)

if exist(f$)
  open "I",#1,f$
  g$=input$(#1,lof(#1))
  gg$=compress$(g$)
  if len(gg$)<len(g$)
    g$=gg$
    comp=1
  endif
  l=len(g$)
  close #1
  print "' output of inline.bas for X11-Basic "+date$
  print "' ";f$;" ";l;" Bytes."
  wort_sep f$,".",0,n$,b$    
  if rinstr(n$,"/")
    n$=right$(n$,len(n$)-rinstr(n$,"/"))
  endif
  t$=""
  print n$+"$="+chr$(34)+chr$(34)
  while l>3
    a$=left$(g$,3)
    t$=t$+chr$(36+(peek(varptr(a$)) and 0xfc)/4)
    t$=t$+chr$(36+(peek(varptr(a$)) and 0x3)*16+(peek(varptr(a$)+1) and 0xf0)/16)
    t$=t$+chr$(36+(peek(varptr(a$)+1) and 0xf)*4+(peek(varptr(a$)+2) and 0xc0)/64)
    t$=t$+chr$(36+(peek(varptr(a$)+2) and 0x3f))
    sub l,3
    g$=right$(g$,len(g$)-3)
    if len(t$)>63
      print n$+"$="+n$+"$+"+chr$(34)+t$+chr$(34)
      t$=""
    endif
  wend
  if l
    a$=g$+mkl$(0)
    t$=t$+chr$(36+(peek(varptr(a$)) and 0xfc)/4)
    t$=t$+chr$(36+(peek(varptr(a$)) and 0x3)*16+(peek(varptr(a$)+1) and 0xf0)/16)
    t$=t$+chr$(36+(peek(varptr(a$)+1) and 0xf)*4+(peek(varptr(a$)+2) and 0xc0)/64)
    t$=t$+chr$(36+(peek(varptr(a$)+2) and 0x3f))
    print n$+"$="+n$+"$+"+chr$(34)+t$+chr$(34)
  endif
  close
  if comp
    print n$+"_"+b$+"$=UNCOMPRESS$(INLINE$("+n$+"$))"
  else
    print n$+"_"+b$+"$=INLINE$("+n$+"$)"
  endif
else
  print f$;" existiert nicht."
  print "Usage: xbasic inline.bas filename"
endif
return
