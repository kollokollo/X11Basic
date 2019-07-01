' Pseudo-Compiler for X11-Basic (MS WINDOWS Version)
' erzeugt allein lauffaehigen Code (.exe)
' (c) Markus Hoffmann 2010-2019
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
CLR dyn,inputfile$,collect$,qflag
outputfilename$="b.exe"
bfile$="b.b"
cfile$="c.c"

xbbc_binary$="xbbc.exe"
xbvm_binary$="xbvm.exe"
xb2c_binary$="xb2c.exe"

WHILE LEN(PARAM$(i))
  IF LEFT$(PARAM$(i))="-"
    IF param$(i)="--help" OR PARAM$(i)="-h"
      @intro
      @using
    ELSE IF PARAM$(i)="--version"
      @intro
      QUIT
    ELSE IF PARAM$(i)="--dynamic"
      dyn=TRUE
    ELSE IF PARAM$(i)="-b"
      precomponly=TRUE
      compileonly=TRUE
    ELSE IF PARAM$(i)="-c"
      compileonly=TRUE
    ELSE IF PARAM$(i)="-l"
      lflag=TRUE
    ELSE IF PARAM$(i)="-q"
      qflag=TRUE
    ELSE IF PARAM$(i)="-o"
      INC i
      IF LEN(PARAM$(i))
        outputfilename$=PARAM$(i)
      ENDIF
    ELSE IF PARAM$(i)="--xbbc"
      INC i
      IF LEN(PARAM$(i))
        xbbc_binary$=PARAM$(i)
      ENDIF
    ELSE IF PARAM$(i)="--xbvm"
      INC i
      IF LEN(PARAM$(i))
        xbvm_binary$=PARAM$(i)
      ENDIF
    ELSE
      collect$=collect$+PARAM$(i)+" "
    ENDIF
  ELSE
    inputfile$=PARAM$(i)
    IF NOT EXIST(inputfile$)
      PRINT "xbc: "+inputfile$+": file or path not found"
      CLR inputfile$
    ENDIF
  ENDIF
  INC i
WEND
PRINT "Current working directory: ";DIR$(0)
PRINT "virtual machine:   "+xbvm_binary$
PRINT "bytecode compiler: "+xbbc_binary$
PRINT "input file: "+inputfile$

IF NOT EXIST(xbvm_binary$)
  ~FORM_ALERT(1,"[3][xbc: ERROR: "+xbvm_binary$+" not found.][CANCEL]")
  PRINT "xbc: ERROR: "+xbvm_binary$+" not found."
  QUIT
ENDIF
IF qflag=0
  schwarz=COLOR_RGB(0,0,0)
  weiss=COLOR_RGB(1,1,1)
  COLOR weiss,schwarz
  IF LEN(inputfile$)=0
    TEXT 10,10,"X11-Basic compiler V.1.27 (c) Markus Hoffmann 2010-2019"
    FILESELECT "select program to compile","./*.bas","demo.bas",inputfile$
    IF LEN(inputfile$)
      IF NOT EXIST(inputfile$)
        CLR inputfile$
      ENDIF
    ENDIF
  ENDIF
ENDIF

IF UPPER$(RIGHT$(inputfile$,2))=".B"
  bfile$=inputfile$
  @packvm(bfile$)
  QUIT
ENDIF
IF UPPER$(RIGHT$(inputfile$,4))<>".BAS" AND UPPER$(RIGHT$(inputfile$,5))<>".XBAS"
  PRINT "File must have the extension: .bas!"
  QUIT
ENDIF

IF LEN(inputfile$)
  t$="[2][You have now following choice:||"
  t$=t$+"1. make bytecode, then make a standalone exe from the bytecode,|"
  t$=t$+"2. make bytecode, then translate bytecode to C,|   and use tcc to compile it,|"
  t$=t$+"3. only produce the bytecode,|"
  t$=t$+"4. pseudo compile, then use tcc.|"
  t$=t$+"|Option 1 is recommended.|For options 2 and 4 tcc needs to be installed.|"

  t$=t$+"][ 1 | 2 | 3 | 4 |CANCEL]"
  IF qflag=0
    COLOR weiss,schwarz
    TEXT 10,32,inputfile$+" OK."
    a=FORM_ALERT(1,t$)
  ELSE
    a=1
  ENDIF
  IF a=1
    @make_bytecode(inputfile$,bfile$)
    @packvm(bfile$)
  ELSE IF a=2
    @make_bytecode(inputfile$,bfile$)
    IF NOT EXIST(xb2c_binary$)
      ~FORM_ALERT(1,"[3][xbc: ERROR: "+xb2c_binary$+" not found.][CANCEL]")
      PRINT "xbc: ERROR: "+xb2c_binary$+" not found."
      QUIT
    ENDIF
    SYSTEM "xb2c "+bfile$+" -o "+cfile$
    IF EXIST(cfile$)
      IF qflag=0
        COLOR weiss,schwarz
        TEXT 10,64,bfile$+" --> "+cfile$+" OK."
      ENDIF
      PRINT bfile$+" --> "+cfile$+" OK."
    ENDIF
    @usetcc
  ELSE IF a=3
    @make_bytecode(inputfile$,bfile$)
    outputfilename$=bfile$
  ELSE IF a=4
    @pseudo
    @usetcc
  ELSE
    QUIT
  ENDIF
  ' Now compilation should have been successful
  IF qflag=0
    IF EXIST(outputfilename$)
      a=FORM_ALERT(1,"[0][done.| |The program was stored under:|"+outputfilename$+".|Do you want to run it?][RUN|QUIT]")
      IF a=1
        IF outputfilename$=bfile$
          SYSTEM xbvm_binary$+" "+outputfilename$
        ELSE
          SYSTEM outputfilename$
        ENDIF
      ENDIF
    ELSE
      ~FORM_ALERT(1,"[3][Ups...|compilation was not successful!][ OH ]")
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
  PRINT "X11-Basic Compiler V.1.27 (c) Markus Hoffmann 2002-2019"
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

PROCEDURE make_bytecode(file$,bfile$)
  IF NOT EXIST(xbbc_binary$)
    IF qflag=0
      ~FORM_ALERT(1,"[3][xbc: ERROR: "+xbbc_binary$+" not found.][CANCEL]")
    ENDIF
    PRINT "xbc: ERROR: "+xbbc_binary$+" not found."
    QUIT
  ENDIF
  PRINT "INPUT: ";file$
  SYSTEM xbbc_binary$+" "+file$+" -o "+bfile$
  PRINT "--> "+bfile$
  IF NOT EXIST(bfile$)
    IF qflag=0
      ~FORM_ALERT(1,"[3][xbc/xbbc: FATAL ERROR: something is wrong.][CANCEL]")
    ENDIF
    PRINT "xbc/xbbc: FATAL ERROR: something is wrong."
    QUIT
  ENDIF
  IF EXIST(bfile$)
    IF qflag=0
      COLOR weiss,schwarz
      TEXT 10,48,file$+" --> "+bfile$+" OK."
    ENDIF
    PRINT file$+" --> "+bfile$+" OK."
  ENDIF
RETURN

PROCEDURE packvm(bfile$)
  LOCAL t$,l,lb,p,u$
  OPEN "I",#1,xbvm_binary$
  l=LOF(#1)
  PRINT xbvm_binary$+" (lof: ",l,")"
  CLOSE #1
  t$=SPACE$(l)
  BLOAD xbvm_binary$,VARPTR(t$)
  PRINT "len(t$) ",LEN(t$)
  p=INSTR(t$,"4007111")
  PRINT p
  MEMDUMP VARPTR(t$)+p-1,16
  IF p=0
    IF qflag=0
      ~FORM_ALERT(1,"[3][xbc: FATAL ERROR: something is wrong.][CANCEL]")
    ENDIF
    PRINT "xbc: FATAL ERROR: something is wrong."
    QUIT
  ENDIF
  PRINT "poking"
  u$=using$(LEN(t$),"#######")
  PRINT u$,p
  FOR i=0 TO LEN(u$)-1
    POKE VARPTR(t$)+p-1+i,PEEK(VARPTR(u$)+i)
  NEXT i
  MEMDUMP VARPTR(t$)+p-1,16

  PRINT "appending",bfile$
  OPEN "I",#1,bfile$
  lb=LOF(#1)
  PRINT lb,"bytes."
  CLOSE #1
  t$=t$+SPACE$(lb)
  BLOAD bfile$,VARPTR(t$)+l
  MEMDUMP VARPTR(t$)+l,lb
  oagain:
  IF qflag=0
    default$=RIGHT$(inputfile$,LEN(inputfile$)-rinstr(inputfile$,"/"))
    default$=RIGHT$(default$,LEN(default$)-rinstr(default$,"\"))
    default$=REPLACE$(default$,".bas",".exe")
    FILESELECT "select filename to write to","./*.exe",default$,outputfilename$
    IF LEN(outputfilename$)=0
      QUIT
    ENDIF
    IF EXIST(outputfilename$)
      a=FORM_ALERT(2,"[3][File "+outputfilename$+" already exists!|Overwrite ?][Overwrite|CANCEL]")
      IF a=2
        GOTO oagain
      ENDIF
    ENDIF
  ENDIF
  ON ERROR gosub file_error
  PRINT "saving ";outputfilename$,LEN(t$),"bytes."
  BSAVE outputfilename$,VARPTR(t$),LEN(t$)
  KILL bfile$
RETURN
PROCEDURE pseudo
  LOCAL linecount
  linecount=0
  OPEN "O",#2,cfile$
  PRINT #2,"/* PSEUDO-Code.c ("+f$+")"
  PRINT #2,"   X11-BAsic-Pseudo-Compiler Version 1.20"
  PRINT #2,"   (c) Markus Hoffmann"
  PRINT #2,"*/"
  PRINT #2,"#include <stdio.h>"
  PRINT #2,"#include <stdlib.h>"
  PRINT #2,"#include <string.h>"
  PRINT #2,"extern int param_anzahl;"
  PRINT #2,"extern char **param_argumente;"
  IF lflag
    PRINT #2,"void programmlauf();"
    PRINT #2,"extern char **program;"
    PRINT #2,"extern int prglen;"
    PRINT #2,"char *routine_"+rumpf$+"[]={"
  ELSE
    PRINT #2,"void reset_input_mode(),x11basicStartup(),programmlauf();"
    PRINT #2,"int verbose=0;"
    PRINT #2,"int programbufferlen=0;"
    PRINT #2,"char ifilename[]="+ENCLOSE$(f$)+";"
    PRINT #2,"char *programbuffer=NULL;"
    PRINT #2,"char **program=(char *[]) {"
  ENDIF
  OPEN "I",#1,inputfile$
  WHILE NOT EOF(#1)
    LINEINPUT #1,t$
    t$=XTRIM$(t$)
    IF LEN(t$)
      IF LEFT$(t$)<>"'"
        SPLIT t$," !",1,t$,b$
        IF LEFT$(t$,6)="PRINT "
          t$="? "+RIGHT$(t$,LEN(t$)-6)
        ELSE IF LEFT$(t$,6)="GOSUB "
          t$="@"+RIGHT$(t$,LEN(t$)-6)
        ELSE IF LEFT$(t$,5)="VOID "
          t$="~"+RIGHT$(t$,LEN(t$)-5)
        ENDIF
        t$=REPLACE$(t$,CHR$(34),"##AN"+"F##")
        t$=REPLACE$(t$,"\","##BACKS"+"LASH##")
        t$=REPLACE$(t$,"##A"+"NF##","\042")
        t$=REPLACE$(t$,"##BACK"+"SLASH##","\\")
        PRINT #2,ENCLOSE$(t$)+","
        INC linecount
      ENDIF
    ENDIF
  WEND
  CLOSE #1
  PRINT #2,"};"
  IF lflag
    PRINT #2,"int routinelen_"+rumpf$+"=sizeof(routine_"+rumpf$+")/sizeof(char *);"
    PRINT #2,"typedef struct {int len; char *pointer;} STRING;"
    PRINT #2,"extern union {double f; STRING str;} returnvalue;"
    PRINT #2,"double enter_"+rumpf$+"(char *n){"
    PRINT #2,"int oldprglen; char **oldprogram;"
    PRINT #2,"oldprglen=prglen; oldprogram=program;"
    PRINT #2,"program=routine_"+rumpf$+";"
    PRINT #2,"prglen=routinelen_"+rumpf$+";"
    PRINT #2,"init_program(prglen);c_run("");c_gosub(n);programmlauf();"
    PRINT #2,"program=oldprogram;prglen=oldprglen;"
    PRINT #2,"return(returnvalue.f);}"
  ELSE
    ' PRINT #2,"int prglen=sizeof(program)/sizeof(char *);"
    PRINT #2,"int prglen="+STR$(linecount)+";"
    IF win32
      PRINT #2,"#include <windows.h>"
      PRINT #2,"HINSTANCE hInstance;"
    ENDIF
    PRINT #2,"main(int anzahl, char *argumente[]){"
    IF win32
      PRINT #2,"hInstance=GetModuleHandle(NULL);"
    ENDIF
    PRINT #2,"x11basicStartup(); set_input_mode(1,0);"
    PRINT #2,"atexit(reset_input_mode);"
    PRINT #2,"param_anzahl=anzahl;"
    PRINT #2,"param_argumente=argumente;"
    PRINT #2,"init_program(prglen); do_run(); programmlauf();}"
  ENDIF
  CLOSE #2
RETURN

PROCEDURE usetcc
  IF qflag=0
    default$=RIGHT$(inputfile$,LEN(inputfile$)-RINSTR(inputfile$,"/"))
    default$=RIGHT$(default$,LEN(default$)-RINSTR(default$,"\"))
    default$=REPLACE$(default$,".bas",".exe")
    o2again:
    FILESELECT "select filename to write to","./*.exe",default$,outputfilename$
    IF LEN(outputfilename$)=0
      QUIT
    ENDIF
    IF EXIST(outputfilename$)
      a=FORM_ALERT(2,"[3][File "+outputfilename$+" already exists!|Overwrite ?][Overwrite|CANCEL]")
      IF a=2
        GOTO o2again
      ENDIF
    ENDIF
  ENDIF
  SYSTEM "tcc "+cfile$+" x11basic.lib libgfx.lib -o "+outputfilename$
RETURN
PROCEDURE file_error
  PRINT "FILE ERROR ERR=";ERR
  a=FORM_ALERT(1,"[3][File writing error #"+STR$(ERR)+"!|Do you have write-permission in the|specified directory?][QUIT]")
RETURN
