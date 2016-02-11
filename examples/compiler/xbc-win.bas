' Pseudo-Compiler fuer X11-Basic (MS WINDOWS Version)
' erzeugt allein lauffaehigen Code (.exe)
' (c) Markus Hoffmann 2010
'

i=1
compileonly=FALSE
precomponly=FALSE
lflag=FALSE
CLR dyn,inputfile$,collect$
outputfilename$="b.exe"

if not exist("xbvm.exe")
  print "xbc: ERROR: xbvm.exe not found."
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
IF LEN(inputfile$)

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
  print "xbc: FATAL ERROR: something is wrong."
  quit
endif
print "poking"
u$=str$(len(t$),7,7,1)
for i=0 to len(u$)-1
  poke varptr(t$)+p-1+i,peek(varptr(u$)+i)
next i
  memdump varptr(t$)+p-1,16

  print "appending",inputfile$
  open "I",#1,inputfile$
  lb=lof(#1)
  print lb,"bytes."
  close #1
  t$=t$+space$(lb)
  bload inputfile$,varptr(t$)+l
  memdump varptr(t$)+l,lb
  print "saving ";outputfilename$,len(t$),"bytes."
  bsave outputfilename$,varptr(t$),len(t$)
else
  print "xbc: No input files"
ENDIF
QUIT
PROCEDURE intro
  PRINT "X11-Basic Compiler V.1.16 (c) Markus Hoffmann 2002-2010"
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
