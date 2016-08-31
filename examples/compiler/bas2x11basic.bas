' Utility to convert ANSI BASIC Programs to X11Basic
'
'* This file is part of X11BASIC, the basic interpreter for Unix/X
'* ======================================================================
'* X11BASIC is free software and comes with NO WARRANTY - read the file
'* COPYING for details
'*
' (c) Markus Hoffmann 2003 (letzte Bearbeitung: 10.12.2002)
' (letzte Bearbeitung: 10.02.2004)
'
'

' 2012-10-29 Modifications by Marcos Cruz (programandala.net)

i=1
' outputfilename$="b.bas" ! xxx original
outputfilename$=@yyyymmddhhmmss$()+"_converted.xbas" ! xxx new, only for debugging
WHILE LEN(PARAM$(i))
  IF LEFT$(PARAM$(i))="-"
    IF PARAM$(i)="--help" OR PARAM$(i)="-h"
      @intro
      @using
    ELSE IF PARAM$(i)="--version"
      @intro
      QUIT
    ELSE IF PARAM$(i)="-o"
      INC i
      IF LEN(PARAM$(i))
        outputfilename$=PARAM$(i)
      ENDIF
    ENDIF
  ELSE
    inputfile$=PARAM$(i)
    IF NOT EXIST(inputfile$)
      PRINT "bas2x11basic: "+inputfile$+": file or path not found"
      CLR inputfile$
    ENDIF
  ENDIF
  INC i
WEND
IF (WIN32? OR ANDROID?) AND (LEN(inputfile$)=0 OR inputfile$="bas2x11basic.bas")
  FILESELECT "Load ANSI basic program","./*.bas","ANSI.bas",inputfile$
ENDIF
IF LEN(inputfile$)
  rumpf$=inputfile$
  WHILE LEN(rumpf$)
    WORT_SEP rumpf$,"/",1,a$,rumpf$
  WEND
  f$=a$
  rumpf$=a$
  WORT_SEP a$,".",1,rumpf$,typ$
  IF typ$="bas"
    IF EXIST(outputfilename$)
      PRINT "bas2x11basic: Outputfilename already exists: ";outputfilename$
    ELSE
      @convert
    ENDIF
  ELSE
    PRINT f$+": file not recognized: File format not recognized"
  ENDIF
ELSE
  PRINT "bas2x11basic: No input files"
ENDIF
QUIT
PROCEDURE intro
  PRINT "ANSI BASIC to X11-Basic Converter V.1.11 (c) Markus Hoffmann 2003-2004"
  VERSION
RETURN
PROCEDURE using
  PRINT "Usage: bas2x11basic [options] file..."
  PRINT "Options:"
  PRINT "  -h, --help		    Display this information"
  PRINT "  -o <file>		    Place the output into <file>"
RETURN
PROCEDURE convert
  DIM linenr(3000),zeile$(3000),labeling(3000),procs(3000)
  anzlabel=0
  anzprocs=0
  anzline=0
  OPEN "O",#2,outputfilename$
  ' xxx original header:
  '  PRINT #2,"' bas2x11basic V.1.10 ("+f$+")"
  '  PRINT #2,"'	   (c) Markus Hoffmann "+date$+" "+time$
  ' xxx new suggested header:
  PRINT #2,"' "+outputfilename$
  PRINT #2,"' ";@iso_time$()
  PRINT #2,"' Automatic conversion of "+f$+" to X11-Basic"
  PRINT #2,"' by bas2x11basic V.1.11 "
  PRINT #2,"' Copyright (C) 2002-2004 Markus Hoffmann"
  PRINT #2
  ' PASS 1
  PRINT "PASS 1"
  pass=1
  OPEN "I",#1,inputfile$
  WHILE NOT EOF(#1)
    LINEINPUT #1,t$
    t$=XTRIM$(t$)
    IF LEN(t$)
      WORT_SEP t$," ",1,b$,t$
      linenr(anzline)=VAL(b$)
      zeile$(anzline)=t$
      INC anzline
      IF UPPER$(LEFT$(t$,2))="IF"
        @processifline(t$)
      ELSE
        @processline(t$)
      ENDIF
    ENDIF
  WEND
  CLOSE #1
  PRINT #2,"' ";anzline;" lines."
  PRINT #2,"' ";anzlabel;" labels."
  PRINT #2,"' ";anzprocs;" procs."
  PRINT #2,"' ----- Start of program -----"
  ' PASS 2
  PRINT "PASS 2"
  pass=2

  FOR i=0 TO anzline-1
    flag=0
    lln=linenr(i)
    FOR j=0 TO anzlabel-1
      IF labeling(j)=lln
        flag=1
      ENDIF
    NEXT j
    IF flag
      PRINT #2,"L";lln;":"
    ENDIF
    flag=0
    lln=linenr(i)
    FOR j=0 TO anzprocs-1
      IF procs(j)=lln
        flag=1
      ENDIF
    NEXT j
    IF flag
      PRINT #2,"PROCEDURE P";lln
    ENDIF
    tt$=TRIM$(zeile$(i))
    IF UPPER$(LEFT$(tt$,2))="IF"
      @processifline(tt$)
    ELSE
      @processline(tt$)
    ENDIF
  NEXT i
  PRINT #2,"' ----- End of program -----"
  CLOSE #2
RETURN

PROCEDURE processline(t$)
  t$=REPLACE$(t$,", ",",")
  WHILE LEN(t$)
    WORT_SEP t$,":",1,b$,t$
    b$=XTRIM$(b$)
    b$=REPLACE$(b$," REM "," ! ")
    b$=REPLACE$(b$," = ","=")
    b$=REPLACE$(b$,"= ","=")
    b$=REPLACE$(b$," + ","+")
    b$=REPLACE$(b$," - ","-")
    b$=REPLACE$(b$," * ","*")
    b$=REPLACE$(b$," / ","/")
    b$=REPLACE$(b$," ^ ","^")
    b$=REPLACE$(b$," > ",">")
    b$=REPLACE$(b$," < ","<")
    b$=REPLACE$(b$," <> ","<>")
    @processcommand(b$)
  WEND
RETURN
PROCEDURE processifline(t$)
  LOCAL ifauf,a$,b$
  ' xxx why is this in the original?, it seems debugging code:
  ' IF pass<>1
  '   PRINT #2,"' "+t$
  ' ENDIF
  t$=REPLACE$(t$,", ",",")
  WHILE LEN(t$)
    WORT_SEP t$,":",1,b$,t$
    b$=XTRIM$(b$)
    b$=REPLACE$(b$," REM "," ! ")
    b$=REPLACE$(b$," = ","=")
    b$=REPLACE$(b$,"= ","=")
    b$=REPLACE$(b$," + ","+")
    b$=REPLACE$(b$," - ","-")
    b$=REPLACE$(b$," * ","*")
    b$=REPLACE$(b$," / ","/")
    b$=REPLACE$(b$," ^ ","^")
    b$=REPLACE$(b$," > ",">")
    b$=REPLACE$(b$," < ","<")
    b$=REPLACE$(b$," <> ","<>")
    WORT_SEP b$," ",1,b$,a$
    IF UPPER$(b$)="IF"
      WORT_SEP a$,"THEN ",1,a$,c$
      IF LEN(c$)=0
        WORT_SEP a$," ",1,a$,c$
      ENDIF
      IF pass<>1
        PRINT #2,"IF "+a$
      ENDIF

      IF LEN(c$)
        WORT_SEP c$,"ELSE ",1,c$,d$
        IF c$=STR$(VAL(c$))
          @processcommand("GOTO "+c$)
        ELSE
          @processcommand(c$)
        ENDIF
        IF LEN(d$)
          IF pass<>1
            PRINT #2,"ELSE"
          ENDIF
          IF d$=STR$(VAL(d$))
            @processcommand("GOTO "+d$)
          ELSE
            @processcommand(d$)
          ENDIF
        ENDIF
        ifauf=1
      ENDIF
    ELSE
      @processcommand(b$+" "+a$)
    ENDIF
  WEND
  IF ifauf
    IF pass<>1
      PRINT #2,"ENDIF"
    ENDIF
  ENDIF
RETURN

PROCEDURE processcommand(b$)
  b$=REPLACE$(b$,"PRINT"+CHR$(34),"PRINT "+CHR$(34))
  b$=REPLACE$(b$,"INPUT"+CHR$(34),"INPUT "+CHR$(34))
  WORT_SEP b$," ",1,b$,a$
  IF UPPER$(b$)="GOTO"
    WORT_SEP a$," ",1,a$,c$
    IF pass=1
      labeling(anzlabel)=VAL(a$)
      INC anzlabel
    ELSE
      PRINT #2,b$;" L";a$+" "+c$
    ENDIF
  ELSE IF UPPER$(b$)="GOSUB"
    WORT_SEP a$," ",1,a$,c$
    IF pass=1
      procs(anzprocs)=VAL(a$)
      INC anzprocs
    ELSE
      PRINT #2,"@P";a$+" "+c$
    ENDIF
  ELSE IF UPPER$(b$)="IF"
    '    PRINT #2,"' ERROR: No if here !" ! xxx original
    @processifline(b$+" "+a$) ! xxx new
  ELSE
    IF pass>1
      PRINT #2,b$;" ";a$
    ENDIF
  ENDIF
RETURN

FUNCTION yyyymmddhhmmss$() ! xxx new, only for debugging
  LOCAL year$,month$,day$,hour$,minute$,second$
  year$=RIGHT$(date$,4)
  month$=LEFT$(date$,2)
  day$=MID$(date$,4,2)
  hour$=LEFT$(time$,2)
  minute$=MID$(time$,4,2)
  second$=RIGHT$(time$,2)
  RETURN year$+month$+day$+hour$+minute$+second$
ENDFUNCTION

FUNCTION iso_time$() ! xxx new, just a suggestion
  LOCAL year$,month$,day$,hour$,minute$,second$
  year$=RIGHT$(date$,4)
  month$=LEFT$(date$,2)
  day$=MID$(date$,4,2)
  hour$=LEFT$(time$,2)
  minute$=MID$(time$,4,2)
  second$=RIGHT$(time$,2)
  RETURN year$+"-"+month$+"-"+day$+" "+hour$+":"+minute$+":"+second$
ENDFUNCTION
