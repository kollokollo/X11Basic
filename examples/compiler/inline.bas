' This program codes a File for use for the INLINE$()-Function in
' X11-Basic V.1.06                    (c) Markus Hoffmann
'           V.1.15 2010 MH
'
'* This file is part of X11BASIC, the basic interpreter for Unix/X
'* ======================================================================
'* X11BASIC is free software and comes with NO WARRANTY - read the file
'* COPYING for details
'*

docomp=1    ! Flag if compression should be used
chuncklen=63   ! length of a line

i=1
CLR inputfile$,collect$
outputfilename$="a.bas"
WHILE LEN(param$(i))
  IF LEFT$(param$(i))="-"
    IF param$(i)="--help" OR param$(i)="-h"
      @intro
      @using
    ELSE IF param$(i)="-c"
      docomp=0
    ELSE IF param$(i)="-o"
      INC i
      IF LEN(param$(i))
        outputfilename$=param$(i)
      ENDIF
    else 
      collect$=collect$+param$(i)+" " 
    ENDIF
  ELSE
    inputfile$=param$(i)
    IF NOT EXIST(inputfile$)
      PRINT "inline.bas: "+inputfile$+": file or path not found"
      CLR inputfile$
    ENDIF
  ENDIF
  INC i
WEND
IF LEN(inputfile$)
  @doit(inputfile$)
else
  print "inline.bas: No input files"
ENDIF
QUIT
PROCEDURE intro
  PRINT "X11-Basic inline V.1.15 (c) Markus Hoffmann 2002-2010"
  VERSION
RETURN
PROCEDURE using
  PRINT "Usage: inline [options] file..."
  PRINT "Options:"
  PRINT "  -h, --help               Display this information"
  PRINT "  -c                       do not compress the file"
  PRINT "  -o <file>                Place the output into <file>"
RETURN


procedure doit(f$)
  local l,ll,content$,g$,gg$,comp
  clr comp
  if exist(f$)
    open "I",#1,f$
    ll=lof(#1)
    content$=input$(#1,ll)
    close #1

    if docomp
      gg$=compress$(content$)
    endif
    if len(gg$)<len(content$) and docomp=1
      g$=gg$
      comp=1
    else
      g$=content$
    endif
    l=len(g$)
    print "' output of inline.bas for X11-Basic "+date$
    if comp=1
      print "' ";f$;" ";ll;" Bytes. (compressed: ";l;" Bytes, ";int(l/ll*100);"%)"
    else
      print "' ";f$;" ";l;" Bytes."
    endif
    SPLIT f$,".",0,n$,b$    
    if rinstr(n$,"/")
      n$=right$(n$,len(n$)-rinstr(n$,"/"))
    endif
    t$=""
    n$=replace$(n$,"-","_")
    print n$+"$="+chr$(34)+chr$(34)
    while l>3
      a$=left$(g$,3)
      t$=t$+chr$(36+(peek(varptr(a$)) and 0xfc)/4)
      t$=t$+chr$(36+(peek(varptr(a$)) and 0x3)*16+(peek(varptr(a$)+1) and 0xf0)/16)
      t$=t$+chr$(36+(peek(varptr(a$)+1) and 0xf)*4+(peek(varptr(a$)+2) and 0xc0)/64)
      t$=t$+chr$(36+(peek(varptr(a$)+2) and 0x3f))
      sub l,3
      g$=right$(g$,len(g$)-3)
      if len(t$)>chuncklen
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
    ENDIF
    IF comp
      PRINT n$+"_"+b$+"$=UNCOMPRESS$(INLINE$("+n$+"$))"
    ELSE
      PRINT n$+"_"+b$+"$=INLINE$("+n$+"$)"
    ENDIF
  ELSE
    PRINT "inline.bas: ";f$;" does not exist."
  ENDIF
RETURN
