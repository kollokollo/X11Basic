' Demonstration of the COMPRESS$() Function in X11-Basic
' V.1.01 (c) Markus Hoffmann 2005

' usage: xbasic myunzip.bas anyfile.myz
'
' will restore the file anyfile

i=1
CLR inputfile$,collect$
outputfilename$=""

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
    ELSE
      collect$=collect$+PARAM$(i)+" "
    ENDIF
  ELSE
    inputfile$=PARAM$(i)
    IF NOT EXIST(inputfile$)
      PRINT "myunzip: "+inputfile$+": file or path not found"
      CLR inputfile$
    ENDIF
  ENDIF
  INC i
WEND
IF LEN(inputfile$)
  PRINT "Input-File: ";inputfile$
  @doit
ELSE
  PRINT "myunzip: No input files"
ENDIF
QUIT
PROCEDURE intro
  PRINT "myunzip V.1.01 (c) Markus Hoffmann 2005"
  VERSION
RETURN
PROCEDURE using
  PRINT "Usage: myunzip [options] file..."
  PRINT "Options:"
  PRINT "  -h, --help               Display this information"
  PRINT "  -o <file>                Place the output into <file>"
RETURN

PROCEDURE doit
  PRINT "Read file ..."
  OPEN "I",#1,inputfile$
  id1=INP(#1)
  id2=INP(#1)
  cm=INP(#1)
  flg=INP(#1)
  mtime=CVL(INPUT$(#1,4))
  xfl=INP(#1)
  os=INP(#1)
  gesamtlaenge=0
  PRINT "MEMBER: ID1=";id1;" ID2=";id2
  PRINT "	  cm=";cm;"  flg=";flg
  PRINT "      mtime=";mtime;" "+UNIXTIME$(mtime);" ";UNIXDATE$(mtime)
  IF BTST(flg,2)
    len=CVI(INPUT$(#1,2))
    extra$=INPUT$(#1,len)
    PRINT len;" Bytes of EXTRA Field."
  ENDIF
  IF BTST(flg,3)
    fname$=""
    a=INP(#1)
    WHILE a<>0
      fname$=fname$+CHR$(a)
      a=INP(#1)
    WEND
    PRINT "Original Filename: <";fname$;">"
    IF outputfilename$=""
      IF EXIST(fname$)
        PRINT fame$;" already exists!"
        CLOSE
        QUIT
      ELSE
        outputfilename$=fname$
      ENDIF
    ENDIF
  ENDIF
  IF BTST(flg,4)
    comment$=""
    a=INP(#1)
    WHILE a<>0
      comment$=comment$+CHR$(a)
      a=INP(#1)
    WEND
    PRINT "Comment: <";comment$;">"
  ENDIF
  IF LEN(outputfilename$)=0
    PRINT "ERROR, Abort"
    QUIT
  ENDIF

  IF cm=32
    INPUT "Enter Passphrase to decrypt file: ",key$
  ENDIF

  OPEN "O",#2,outputfilename$
  WHILE LOF(#1)>LOC(#1)
    @doblock
  WEND
  PRINT
  CLOSE #1
  CLOSE #2
RETURN
PROCEDURE doblock
  l=CVL(INPUT$(#1,4))
  c=CVL(INPUT$(#1,4))
  PRINT "#";blockcount;" ";l;" Bytes: ";
  tin$=INPUT$(#1,l)
  t=TIMER
  IF cm=32
    tin$=DECRYPT$(tin$,key$)
  ENDIF
  tout$=UNCOMPRESS$(tin$)
  ADD gesamtlaenge,LEN(tout$)
  PRINT LEN(tout$);" ratio: ";STR$(LEN(tout$)/LEN(tin$),3,3);", ";STR$(TIMER-t,3,3);" Seconds. ";gesamtlaenge;
  PRINT #2,tout$;
  INC blockcount
  IF CRC(tout$)=c
    PRINT " CRC OK.";CHR$(13);
  ELSE
    PRINT " CRC ERROR."
    QUIT
  ENDIF
RETURN
