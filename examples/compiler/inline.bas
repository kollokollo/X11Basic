' This program codes a File for use for the INLINE$()-Function in
' X11-Basic V.1.06                    (c) Markus Hoffmann
'           V.1.15 2010 MH
'           V.1.22 2014 MH
'
'* This file is part of X11BASIC, the basic interpreter for Unix/X
'* ======================================================================
'* X11BASIC is free software and comes with NO WARRANTY - read the file
'* COPYING for details
'*

docomp=1       ! Flag if compression should be used
chuncklen=63   ! length of a line

i=1
CLR inputfile$,collect$
outputfilename$="a.bas"
WHILE LEN(PARAM$(i))
  IF LEFT$(PARAM$(i))="-"
    IF PARAM$(i)="--help" OR PARAM$(i)="-h"
      @intro
      @using
    ELSE IF PARAM$(i)="-c"
      docomp=0
    ELSE IF PARAM$(i)="-o"
      INC i
      IF LEN(PARAM$(i))
        outputfilename$=PARAM$(i)
      ENDIF
    ELSE
      collect$=collect$+PARAM$(i)+" "
    ENDIF
  ELSE
    inputfile$=PARAM$(i)
    IF NOT EXIST(inputfile$)
      PRINT "inline.bas: "+inputfile$+": file or path not found"
      CLR inputfile$
    ENDIF
  ENDIF
  INC i
WEND
IF LEN(inputfile$)
  @doit(inputfile$)
ELSE
  PRINT "inline.bas: No input files"
ENDIF
QUIT
PROCEDURE intro
  PRINT "X11-Basic inline V.1.22 (c) Markus Hoffmann 2002-2014"
  VERSION
RETURN
PROCEDURE using
  PRINT "Usage: inline [options] file..."
  PRINT "Options:"
  PRINT "  -h, --help               Display this information"
  PRINT "  -c                       do not compress the file"
  PRINT "  -o <file>                Place the output into <file>"
RETURN

PROCEDURE doit(f$)
  LOCAL l,ll,content$,g$,gg$,comp
  CLR comp
  IF EXIST(f$)
    OPEN "I",#1,f$
    ll=LOF(#1)
    content$=INPUT$(#1,ll)
    CLOSE #1
    IF docomp
      gg$=COMPRESS$(content$)
    ENDIF
    IF LEN(gg$)<LEN(content$) AND docomp=1
      g$=gg$
      comp=1
    ELSE
      g$=content$
    ENDIF
    l=LEN(g$)
    PRINT "' output of inline.bas for X11-Basic "+date$
    IF comp=1
      PRINT "' ";f$;" ";ll;" Bytes. (compressed: ";l;" Bytes, ";INT(l/ll*100);"%)"
    ELSE
      PRINT "' ";f$;" ";l;" Bytes."
    ENDIF
    SPLIT f$,".",0,n$,b$
    IF RINSTR(n$,"/")
      n$=RIGHT$(n$,LEN(n$)-RINSTR(n$,"/"))
    ENDIF
    t$=""
    n$=REPLACE$(n$,"-","_")
    PRINT n$+"$="+CHR$(34)+CHR$(34)
    pointer=0
    WHILE l>=3
      t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0xfc)/4)
      t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3) AND 0x3)*16+(PEEK(VARPTR(g$)+pointer*3+1) AND 0xf0)/16)
      t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+1) AND 0xf)*4+(PEEK(VARPTR(g$)+pointer*3+2) AND 0xc0)/64)
      t$=t$+CHR$(36+(PEEK(VARPTR(g$)+pointer*3+2) AND 0x3f))
      SUB l,3
      INC pointer
      IF LEN(t$)>chuncklen
        PRINT n$+"$="+n$+"$+"+ENCLOSE$(t$)
        t$=""
      ENDIF
    WEND
    IF l    ! handle the last 1 or 2 bytes
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
    IF len(t$)
      PRINT n$+"$="+n$+"$+"+ENCLOSE$(t$)
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
