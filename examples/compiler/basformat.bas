#!/usr/bin/xbasic
' Format an X11-Basic Program such that            (c) Markus Hoffmann 2016
' a) all unnecessary whitespaces are removes
' b) Commands and BASIC Keywords are in capital letters
' c) Structure formatting by Einrueckungen
'
' Uage: xbasic basformat.bas input.bas -o output.bas
'
' The default output file name is a.bas
'
'
i=1
outputfilename$="a.bas"

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
      PRINT "basformat: "+inputfile$+": file or path not found"
      CLR inputfile$
    ENDIF
  ENDIF
  INC i
WEND
IF LEN(inputfile$)
  IF EXIST(outputfilename$)
    PRINT "basformat: Outputfilename already exists: ";outputfilename$
  ELSE
    @convert
  ENDIF
ELSE
  PRINT "basformat: No input files"
ENDIF
QUIT

PROCEDURE intro
  PRINT "X11-Basic source formatter and beautifier V.1.00 (c) Markus Hoffmann 2016"
  VERSION
RETURN
PROCEDURE using
  PRINT "Usage: basformat [options] file..."
  PRINT "Options:"
  PRINT "  -h, --help		    Display this information"
  PRINT "  -o <file>		    Place the output into <file>"
RETURN

PROCEDURE convert
  levelin$()=["WHILE","DO","REPEAT","IF","FUNCTION","ELSE","FOR","PROCEDURE","SELECT","CASE"]
  levelout$()=["WEND","LOOP","UNTIL","ENDIF","ENDFUNCTION","ENDFUNC","ELSE","NEXT","RETURN","ENDSELECT","CASE"]

  OPEN "O",#2,outputfilename$
  OPEN "I",#1,inputfile$
  WHILE NOT EOF(#1)
    LINEINPUT #1,torig$
    t$=torig$
    CLR comcount
    CLR comment$
    ' Leerzeichen vorne und hinten entfernen.
    WHILE left$(t$)=" " OR LEFT$(t$)=CHR$(9)
      t$=RIGHT$(t$,LEN(t$)-1)
    WEND
    WHILE right$(t$)=" " OR RIGHT$(t$)=CHR$(9)
      t$=LEFT$(t$,LEN(t$)-1)
    WEND
    IF left$(t$)<>"'" AND LEFT$(t$)<>"#" AND LEFT$(t$)<>"!"
      ' Nach Kommentaren hinter der Zeile suchen.
      SPLIT t$,"!",1,w1$,w2$    ! Hier ist ein Kommentar hinter der Zeile
      IF len(w2$)
        ' Zaehle die Leerzeichen vor dem Kommentar
        t$=w1$
        WHILE right$(t$)=" " OR RIGHT$(t$)=CHR$(9)
          IF right$(t$)=CHR$(9)
            ADD comcount,7
          ENDIF
          t$=LEFT$(t$,LEN(t$)-1)
          INC comcount
        WEND
        comment$=w2$
      ENDIF
      t$=TRIM$(t$)
    ELSE
      @lineout(t$)     !  Kommentar rausschreiben
    ENDIF
    IF len(t$)
      IF left$(t$)="#" OR LEFT$(t$)="'" OR LEFT$(t$)="!"
      ELSE
        IF left$(t$)="&" OR LEFT$(t$)="@" OR LEFT$(t$)="~"
          @lineout(t$)
        ELSE
          SPLIT t$," ",TRUE,a$,t$
          IF len(t$) AND LEFT$(t$)="="
            a$=a$+t$
          ENDIF
          IF upper$(a$)="REM" OR UPPER$(a$)="DATA"
            @lineout(upper$(a$)+" "+t$)
          ELSE
            FOR i=0 TO DIM?(levelin$())-1
              IF upper$(a$)=levelin$(i)
                levelflag=TRUE
                BREAK
              ENDIF
            NEXT i
            FOR i=0 TO DIM?(levelout$())-1
              IF upper$(a$)=levelout$(i)
                DEC level
                BREAK
              ENDIF
            NEXT i
            IF upper$(a$)="RETURN" AND LEN(t$)
              INC level
            ENDIF
            IF len(t$)=0
              IF INSTR(a$,"=") OR RIGHT$(a$)=":"
                @lineout(@rep$(a$))
              ELSE
                @lineout(UPPER$(a$))
              ENDIF
            ELSE
              IF instr(a$,"=")
                @lineout(@rep$(a$)+" "+t$)
              ELSE
                @lineout(UPPER$(a$)+" "+@rep$(t$))
              ENDIF
            ENDIF
            IF levelflag
              INC level
              levelflag=0
            ENDIF
          ENDIF
        ENDIF
      ENDIF
      lastlinewasempty=FALSE
    ELSE
      IF lastlinewasempty=FALSE
        @lineout("")
        lastlinewasempty=TRUE
      ENDIF
    ENDIF

    IF level<0
      PRINT "' ERROR: something is wrong..."
      level=0
    ENDIF
    INC anzzeilen
  WEND
  CLOSE #1,#2
  PRINT "' numlines=";anzzeilen
RETURN
PROCEDURE lineout(l$)
  IF l$=""
    PRINT #2
  ELSE
    IF len(comment$) OR comcount>0
      PRINT #2,STRING$(level,"  ")+l$+SPACE$(comcount)+"!"+comment$
    ELSE
      PRINT #2,STRING$(level,"  ")+l$
    ENDIF
  ENDIF
RETURN
FUNCTION rep$(t$)
  LOCAL wa$
  t$=@myreplace$(t$," step "," STEP ")
  t$=@myreplace$(t$," to "," TO ")
  t$=@myreplace$(t$," or "," OR ")
  t$=@myreplace$(t$," and "," AND ")
  t$=@myreplace$(t$," div "," DIV ")
  t$=@myreplace$(t$," mul "," MUL ")
  t$=@myreplace$(t$," mod "," MOD ")
  t$=@myreplace$(t$," not "," NOT ")
  t$=@myreplace$(t$," =","=")
  t$=@myreplace$(t$,"= ","=")
  t$=@myreplace$(t$," +","+")
  t$=@myreplace$(t$,"+ ","+")
  RESTORE functions
  READ wa$
  WHILE wa$<>"*"
    t$=@myreplace$(t$,","+wa$,","+UPPER$(wa$))
    t$=@myreplace$(t$,"+"+wa$,"+"+UPPER$(wa$))
    t$=@myreplace$(t$,"("+wa$,"("+UPPER$(wa$))
    t$=@myreplace$(t$,"="+wa$,"="+UPPER$(wa$))
    t$=@myreplace$(t$,";"+wa$,";"+UPPER$(wa$))
    t$=@myreplace$(t$," "+wa$," "+UPPER$(wa$))
    READ wa$
  WEND
  RETURN t$
ENDFUNCTION
' Ersetzt in Anfuehrungszeichen nicht
FUNCTION myreplace$(a$,b$,c$)
  LOCAL w1$,w2$,ret$
  SPLIT a$,b$,0,w1$,w2$
  IF a$=w1$
    RETURN a$
  ENDIF
  RETURN w1$+c$+@myreplace$(w2$,b$,c$)
ENDFUNCTION
functions:
DATA "chr$(","upper$(","left$(","right$(","mid$(","string$(","space$(","len(","varptr(","str$(","get_color(","color_rgb("
DATA "val(","int(","replace$(","word$(","trim$(","julian(","inp(","inp?(","peek(","param$("
DATA "exist(","hex$(","lpeek(","asc(","random(","max(","min("
DATA "*"
