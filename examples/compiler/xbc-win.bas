' Pseudo-Compiler for X11-Basic (MS WINDOWS Version)
' erzeugt allein lauffaehigen Code (.exe)
' (c) Markus Hoffmann 2010-2011
'
'* This file is part of X11BASIC, the BASIC interpreter / compiler
'* ======================================================================
'* X11BASIC is free software and comes with NO WARRANTY - read the file
'* COPYING for details
'*

i=1
compileonly=FALSE
precomponly=FALSE
lflag=FALSE
CLR dyn,inputfile$,collect$
outputfilename$="b.exe"
bfile$="b.b"

IF NOT EXIST("xbvm.exe")
  ~FORM_ALERT(1,"[3][xbc: ERROR: xbvm.exe not found.][CANCEL]")
  PRINT "xbc: ERROR: xbvm.exe not found."
  QUIT
ENDIF
IF NOT EXIST("xbbc.exe")
  ~form_alert(1,"[3][xbc: ERROR: xbbc.exe not found.][CANCEL]")
  print "xbc: ERROR: xbbc.exe not found."
  quit
endif

WHILE LEN(param$(i))
  IF LEFT$(param$(i))="-"
    IF param$(i)="--help" OR param$(i)="-h"
      @intro
      @using
    ELSE IF param$(i)="--version"
      @intro
      QUIT
    ELSE IF param$(i)="--dynamic"
      dyn=TRUE
    ELSE IF param$(i)="-b"
      precomponly=TRUE
      compileonly=TRUE
    ELSE IF param$(i)="-c"
      compileonly=TRUE
    ELSE IF param$(i)="-l"
      lflag=TRUE
    ELSE IF param$(i)="-q"
      qflag=TRUE
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
      PRINT "xbc: "+inputfile$+": file or path not found"
      CLR inputfile$
    ENDIF
  ENDIF
  INC i
WEND
IF qflag=0
 IF LEN(inputfile$)=0
  TEXT 10,10,"X11-Basic compiler V.1.18 (c) Markus Hoffmann 2010-2011"
  FILESELECT "select program to compile","./*.bas","demo.bas",inputfile$
  IF LEN(inputfile$)
    IF NOT EXIST(inputfile$)
      CLR inputfile$
    ENDIF
  ENDIF
 ENDIF
ENDIF
IF UPPER$(RIGHT$(inputfile$,4))<>".BAS"
  PRINT "File must have the extension: .bas!"
  QUIT
ENDIF

IF LEN(inputfile$)
  print "INPUT: ";inputfile$
  system "xbbc "+inputfile$+" -o "+bfile$
  if not exist(bfile$)
    if qflag=0
      ~form_alert(1,"[3][xbc/xbbc: FATAL ERROR: something is wrong.][CANCEL]")
    endif
    print "xbc/xbbc: FATAL ERROR: something is wrong."
    quit
  endif
  open "I",#1,"xbvm.exe"
  l=lof(#1)
  print "xbvm.exe (lof: ",l,")"
  close #1
  t$=space$(l)
  bload "xbvm.exe",varptr(t$)
  print "len(t$) ",len(t$)
  p=instr(t$,"4007111")
  print p
  memdump varptr(t$)+p-1,16
  if p=0
    if qflag=0
      ~form_alert(1,"[3][xbc: FATAL ERROR: something is wrong.][CANCEL]")
    endif
    print "xbc: FATAL ERROR: something is wrong."
    quit
  endif
  print "poking"
  u$=str$(len(t$),7,7,1)
  for i=0 to len(u$)-1
    poke varptr(t$)+p-1+i,peek(varptr(u$)+i)
  next i
  memdump varptr(t$)+p-1,16

  print "appending",bfile$
  open "I",#1,bfile$
  lb=lof(#1)
  print lb,"bytes."
  close #1
  t$=t$+space$(lb)
  bload bfile$,varptr(t$)+l
  memdump varptr(t$)+l,lb
  oagain:
  if qflag=0
    default$=right$(inputfile$,len(inputfile$)-rinstr(inputfile$,"/"))
    default$=right$(default$,len(default$)-rinstr(default$,"\"))
    default$=replace$(default$,".bas",".exe")
      fileselect "select program to compile","./*.exe",default$,outputfilename$
    if len(outputfilename$)=0
      quit
    endif
    if exist(outputfilename$)
      a=form_alert(2,"[3][File "+outputfilename$+" already exists!|Overwrite ?][Overwrite|CANCEL]")
      if a=2
        goto oagain
      ENDIF
    ENDIF
  ENDIF
  PRINT "saving ";outputfilename$,LEN(t$),"bytes."

  BSAVE outputfilename$,VARPTR(t$),LEN(t$)
  if qflag=0
    a=FORM_ALERT(1,"[0][done.| The program was stored under:|"+outputfilename$+".|Do you want to run it?][RUN|QUIT]")
    IF a=1
      SYSTEM outputfilename$
    ENDIF
  ENDIF
ELSE
  IF qflag=0
    ~FORM_ALERT(1,"[2][xbc: No input files.][QUIT]")
  ENDIF
  PRINT "xbc: No input files"
ENDIF
QUIT
PROCEDURE intro
  PRINT "X11-Basic Compiler V.1.18 (c) Markus Hoffmann 2002-2011"
  VERSION
RETURN
PROCEDURE using
  PRINT "Usage: xbc [options] file..."
  PRINT "Options:"
  PRINT "  -h, --help               Display this information"
  PRINT "  --dynamic                link with shared library libx11basic.so"
  PRINT "  --static                 link with static library (default)"
  PRINT "  -b                       only precompile"
  PRINT "  -c                       precompile and compile, but do not link"
  PRINT "  -l                       produce linkable object file (no executable)"
  PRINT "  -bytecode                produce a bytecode file"
  PRINT "  -virtualm                use virtual machine framework insted of psydo-interpreter"
  PRINT "  -shared                  produce shared object file"
  PRINT "  -win32                   produce Windows .exe file"
  PRINT "  -o <file>                Place the output into <file>"
RETURN
