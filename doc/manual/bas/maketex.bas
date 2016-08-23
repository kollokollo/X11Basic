' Maketex Erstellt die X11-Basic Dokumentation (A.tex)
' der Kommando-Beschreibungen (?.sec)
' (c) Markus Hoffmann Jan. 2003
' letzte Bearbeitung 08/2016

outputfilename$="A.tex"
inputpath$="sec"

i=1
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
      PRINT "maketex: "+inputfile$+": file or path not found"
      CLR inputfile$
    ENDIF
  ENDIF
  INC i
WEND

OPEN "O",#2,outputfilename$
letter=ASC("A")
WHILE letter<=ASC("Z")
  g$=inputpath$+"/"+CHR$(letter)+".sec"
  IF exist(g$)
    PRINT #2,"\section{"+CHR$(letter)+"}"
    PRINT chr$(letter);
    FLUSH
    @doone(g$)
  ENDIF
  INC letter
WEND
PRINT
CLOSE #2
QUIT

PROCEDURE intro
  PRINT "maketex V.1.25 (c) Markus Hoffmann 2002-2016"
RETURN
PROCEDURE using
  PRINT "Usage: maketex [options] ..."
  PRINT "Options:"
  PRINT "  -h, --help               Display this information"
  PRINT "  -o <file>                Place the output into <file>, default: "+outputfilename$
RETURN

PROCEDURE doone(f$)
  OPEN "I",#1,f$
  CLR count,vflag
  WHILE NOT EOF(#1)
    LINEINPUT #1,t$
    ' if not vflag
    ' endif
    IF LEFT$(t$,4)="####"
      IF vflag
        PRINT #2,"\end{verbatim}}"
        PRINT #2,"\end{mdframed}"
        CLR vflag
      ENDIF
      PRINT #2,"\clearpage"
    ELSE IF LEFT$(t$,4)="*###"
      IF vflag
        PRINT #2,"\end{verbatim}}"
        PRINT #2,"\end{mdframed}"
        CLR vflag
      ENDIF
      PRINT #2,"\begin{center}"
      PRINT #2,"\begin{tabular}{c}"
      PRINT #2,"\verb|                *                |\\"
      PRINT #2,"\hline\end{tabular}"
      PRINT #2,"\end{center}"
    ELSE IF LEFT$(t$,7)="Variabl" OR LEFT$(t$,7)="Keyword" OR LEFT$(t$,7)="Command" OR LEFT$(t$,7)="Operato" OR LEFT$(t$,7)="Functio"
      SPLIT t$,":",1,a$,b$
      b$=XTRIM$(b$)
      PRINT #2,"\begin{mdframed}[hidealllines=true,backgroundcolor=red!20]"
      PRINT #2,"{\Large \normalfont \bfseries "+a$+":} {\Huge \normalfont \bfseries \hspace{2cm}\verb|"+b$+"|}";
      WHILE len(b$)
        SPLIT b$,",",1,a$,b$
        a$=REPLACE$(REPLACE$(a$,"_","\(UNT)"),"\(UNT)","\_")
        a$=REPLACE$(REPLACE$(a$,"$","\(DOLLAR)"),"\(DOLLAR)","\$")
        a$=REPLACE$(REPLACE$(a$,"#","\(KREUZCHEN)"),"\(KREUZCHEN)","\#")
        PRINT #2,"\index{"+a$+"|bb}";
        a$=REPLACE$(a$,"\$","S")
        a$=REPLACE$(a$,"\_","i")
        a$=REPLACE$(a$,"\%","i")
        a$=REPLACE$(a$,"\&","u")
        a$=REPLACE$(a$,"\#","c")
        a$=REPLACE$(a$,"?","f")
        a$=REPLACE$(a$," ","b")
        a$=REPLACE$(a$,".","p")
        a$=REPLACE$(a$,"@","a")
        a$=REPLACE$(a$,"'","h")
        a$=REPLACE$(a$,"~","t")
        a$=REPLACE$(a$,"\nobreakspace","")
        a$=REPLACE$(a$,"{}","")
        PRINT #2,"\label{"+REPLACE$(a$,"()","")+"}";
        b$=TRIM$(b$)
      WEND
      PRINT #2,"\\[5ex]"
      PRINT #2,"\end{mdframed}"
    ELSE if LEFT$(t$,7)="Syntax:"
      PRINT #2,"\begin{mdframed}[hidealllines=true,backgroundcolor=yellow!20]"
      WORT_SEP t$,":",1,a$,b$
      PRINT #2,"\begin{tabular}{ll}"
      PRINT #2,"{\Large {\normalfont \itshape Syntax:}} & \verb|"+TRIM$(b$)+"|\\"
      sflag=1
    ELSE if LEFT$(t$,7)="DESCRIP"
      IF sflag
        PRINT #2,"\end{tabular}\\[4ex]"
        PRINT #2,"\end{mdframed}"
        sflag=0
      ENDIF
      WORT_SEP t$,":",1,a$,b$
      PRINT #2,"{\Large \normalfont \bfseries DESCRIPTION:}\\[2ex] "+b$+""
      PRINT #2
    ELSE if LEFT$(t$,7)="EXAMPLE"
      WORT_SEP t$,":",1,a$,b$
      PRINT #2,"\vspace{1cm}"
      PRINT #2,"\begin{mdframed}[hidealllines=true,backgroundcolor=blue!20]"

      PRINT #2,"\section*{\normalfont \bfseries "+a$+":} "+b$+""
      PRINT #2,"{\footnotesize\linespread{0.8}\begin{verbatim}"
      vflag=true
    ELSE if LEFT$(t$,7)="SEE ALS"
      WORT_SEP t$,":",1,a$,b$
      IF vflag
        PRINT #2,"\end{verbatim}}"
        PRINT #2,"\end{mdframed}"
        CLR vflag
      ENDIF
      b$=TRIM$(b$)
      PRINT #2,"\vspace{1cm} {\large \normalfont \bfseries SEE ALSO:}\hspace{1cm} "
      WHILE len(b$)
        e=wort_sep(b$,",",1,a$,b$)
        a$=REPLACE$(REPLACE$(a$,"_","\(UNT)"),"\(UNT)","\_")
        a$=REPLACE$(REPLACE$(a$,"$","\(DOLLAR)"),"\(DOLLAR)","\$")
        PRINT #2,"\index{"+a$+"}\verb|"+a$+"|";
        IF e=2
          PRINT #2,",";
        ENDIF
        PRINT #2
        b$=TRIM$(b$)
      WEND
    ELSE
      IF sflag
        IF LEN(TRIM$(t$))
          PRINT #2," & \verb|"+TRIM$(t$)+"|\\"
        ENDIF
      ELSE
        IF NOT vflag
          t$=REPLACE$(t$,"$","\(DOLLAR)")
          t$=REPLACE$(t$,"[","\(EKA)")
          t$=REPLACE$(t$,"\(EKA)","$[$")
          t$=REPLACE$(t$,"]","\(EKZ)")
          t$=REPLACE$(t$,"\(EKZ)","$]$")
          t$=REPLACE$(t$,"#","\(ASX)")
          t$=REPLACE$(t$,"\(ASX)","\#")
          ' t$=REPLACE$(t$,"{","\(SK1)")
          ' t$=REPLACE$(t$,"\(SK1)","$\{$")
          ' t$=REPLACE$(t$,"}","\(SK2)")
          ' t$=REPLACE$(t$,"\(SK2)","$\}$")
          t$=REPLACE$(t$,"&","\(ASC)")
          t$=REPLACE$(t$,"\(ASC)","\&")
          t$=REPLACE$(t$,"%","\(USC)")
          t$=REPLACE$(t$,"\(USC)","\%")
          t$=REPLACE$(t$,"^","\(EXP)")
          t$=REPLACE$(t$,"\(EXP)","\verb|^|")
          t$=REPLACE$(t$,"\(DOLLAR)","\$")
          t$=REPLACE$(t$,"_","\(UNDERS)")
          t$=REPLACE$(t$,"\(UNDERS)","\_")
        ENDIF
        PRINT #2,t$
      ENDIF
    ENDIF
    INC count
  WEND
  CLOSE #1
RETURN
