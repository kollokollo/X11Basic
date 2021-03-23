' Demonstration of the COMPRESS$() Function in X11-Basic
' V. 1.01 (c) Markus Hoffmann 2005

' usage: xbasic myzip.bas anyfile
'
' will produce a compressed file anyfile.myz
' In most cases the comprtession is better that gzip
' and comparable to bzip2

i=1
CLR doencrypt,force
verbose=1
blocksize=200000
CLR inputfile$,collect$
outputfilename$=""
comment$="myzip (c) Markus Hoffmann 2005"
WHILE LEN(PARAM$(i))
  IF LEFT$(PARAM$(i))="-"
    IF PARAM$(i)="--help" OR PARAM$(i)="-h"
      @intro
      @using
    ELSE IF PARAM$(i)="--version"
      @intro
      QUIT
    ELSE IF PARAM$(i)="--verbose" OR PARAM$(i)="-v"
      INC verbose
    ELSE IF PARAM$(i)="--quiet" OR PARAM$(i)="-q"
      DEC Verbose
    ELSE IF PARAM$(i)="--force" OR PARAM$(i)="-f"
      force=TRUE
    ELSE IF PARAM$(i)="-b"
      INC i
      IF LEN(PARAM$(i))
        blocksize=VAL(PARAM$(i))
      ENDIF
    ELSE IF PARAM$(i)="-c"
      INC i
      comment$=PARAM$(i)
    ELSE IF PARAM$(i)="-e"
      INC i
      key$=PARAM$(i)
      doencrypt=TRUE
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
      PRINT "myzip: "+inputfile$+": file or path not found"
      CLR inputfile$
    ENDIF
  ENDIF
  INC i
WEND
IF LEN(inputfile$)
  rumpf$=inputfile$
  WHILE LEN(rumpf$)
    SPLIT rumpf$,"/",1,a$,rumpf$
  WEND
  rumpf$=a$
  IF outputfilename$=""
    outputfilename$=rumpf$+".myz"
  ENDIF
  IF verbose
    PRINT "Input-File: ";inputfile$
    PRINT "Output-File: ";outputfilename$
  ENDIF
  IF EXIST(outputfilename$) AND force=0
    PRINT outputfilename$;" already exists!"
  ELSE
    @doit
  ENDIF
ELSE
  PRINT "myzip: No input files"
ENDIF
QUIT
PROCEDURE intro
  PRINT "myzip V.1.01 (c) Markus Hoffmann 2005"
  VERSION
RETURN
PROCEDURE using
  PRINT "Usage: myzip [options] file..."
  PRINT "Options:"
  PRINT "  -h, --help               Display this information"
  PRINT "  -q, --quiet              be more quiet"
  PRINT "  -v, --verbose            be more verbose"
  PRINT "  -b <blocksize>           set blocksize to b [";blocksize;"]"
  PRINT "  -c <comment>             Place a comment into file"
  PRINT "  -e <key>                 encrypt file with key"
  PRINT "  -o <file>                Place the output into <file>"
RETURN

PROCEDURE doit
  OPEN "I",#1,inputfile$
  OPEN "O",#2,outputfilename$
  ' Write header
  id1=ASC("M")
  id2=ASC("Z")
  cm=33+doencrypt
  os=3
  flg=BSET(0,3)
  IF LEN(comment$)
    flg=BSET(flg,4)
  ENDIF
  PRINT #2,CHR$(id1)+CHR$(id2)+CHR$(cm)+CHR$(flg)+MKL$(TIMER)+CHR$(xfl)+CHR$(os);
  PRINT #2,rumpf$+CHR$(0);
  IF LEN(comment$)
    PRINT #2,comment$+CHR$(0);
  ENDIF
  l=LOF(#1)
  ll=l
  WHILE l>blocksize
    @progress(ll,ll-l)
    tin$=INPUT$(#1,blocksize)
    PRINT #2,@doblock$(tin$);
    SUB l,blocksize
  WEND
  IF l>0
    @progress(ll,ll-l)
    tin$=INPUT$(#1,l)
    PRINT #2,@doblock$(tin$);
  ENDIF
  CLOSE #1
  CLOSE #2
RETURN
FUNCTION doblock$(b$)
  t=TIMER
  IF verbose>1
    PRINT "Compress ";LEN(b$);" Bytes. ";
  ENDIF
  tout$=COMPRESS$(b$)
  IF verbose>2
    PRINT LEN(tout$);" ratio: ";STR$(100*LEN(tout$)/LEN(tin$),3,3);"%, took ";STR$(TIMER-t,3,3);" Seconds."
  ENDIF
  IF doencrypt
    ex=(LEN(tout$) MOD 8)
    IF ex>0
      tout$=tout$+STRING$(8-ex,RIGHT$(tout$))
    ENDIF
    RETURN MKL$(LEN(tout$))+MKL$(CRC(b$))+ENCRYPT$(tout$,key$)
  ELSE
    RETURN MKL$(LEN(tout$))+MKL$(CRC(b$))+tout$
  ENDIF
RETURN
PROCEDURE progress(a,b)
  LOCAL t$
  IF verbose
    PRINT CHR$(13);"[";STRING$(b/a*32,"-");">";STRING$((1.03-b/a)*32,"-");"| ";STR$(INT(b/a*100),3,3);"% ]";
    FLUSH
  ENDIF
RETURN
