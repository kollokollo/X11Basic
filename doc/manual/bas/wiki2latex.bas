' Get Pages from the X11-Basic WIKI and save them as latex files so that they
' can be included in the manual
'
' (c) Markus Hoffmann 2014
'
' http://sourceforge.net/apps/mediawiki/x11-basic/index.php?title=Main_Page&action=raw
'
'

@makepage("FAQ")
@makepage("Compatibility")
QUIT

PROCEDURE makepage(page$)
  LOCAL t$
  t$=@getpagecontent$(page$)
  PRINT "\chapter{"+page$+"}"
  t$=REPLACE$(t$,CHR$(13),"")
  t$=@replacepairs$(t$,"'''","{\normalfont \bfseries ","}")
  t$=@replacepairs$(t$,"''","{\em ","}")
  t$=@replacepairs$(t$,"===","\subsection{","}")
  t$=@replacepairs$(t$,"==","\section{","}")
  t$=REPLACE$(t$,"$","\(DOLLAR)")
  t$=REPLACE$(t$,"\(DOLLAR)","\$")
  t$=REPLACE$(t$,"_","\(UNDERS)")
  t$=REPLACE$(t$,"\(UNDERS)","\_")
  t$=REPLACE$(t$,"&","\(ASC)")
  t$=REPLACE$(t$,"\(ASC)","\&")
  t$=REPLACE$(t$,"%","\(USC)")
  t$=REPLACE$(t$,"\(USC)","\%")
  t$=REPLACE$(t$,"^","\(EXP)")
  t$=REPLACE$(t$,"\(EXP)","(HOCH)")
  WHILE len(t$)
    SPLIT t$,CHR$(10),0,a$,t$
    IF left$(a$)=" "
      IF preflag=0
        preflag=1
        PRINT "\begin{verbatim}"
      ENDIF
    ELSE
      IF preflag=1
        preflag=0
        PRINT "\end{verbatim}"
      ENDIF
    ENDIF
    IF preflag=0
      IF islist
        IF left$(TRIM$(a$),1)<>"*"
          PRINT "\end{itemize}"
          islist=0
        ENDIF
      ENDIF
      IF left$(TRIM$(a$),2)="{|"
        PRINT replace$(a$,"{|","\begin{tabular}{ccc} % ")
        INC istable
      ELSE if LEFT$(TRIM$(a$),2)="|}"
        PRINT replace$(a$,"|}","\end{tabular} % ")
        DEC istable
      ELSE if LEFT$(TRIM$(a$),2)="|-"
        PRINT replace$(a$,"|-","\\ % ")
      ELSE if LEFT$(TRIM$(a$),1)="|"
        a$=REPLACE$(a$,"||"," & ")
        a$=RIGHT$(a$,LEN(a$)-1)
        PRINT a$
      ELSE if LEFT$(TRIM$(a$),1)="*"
        IF islist=0
          PRINT "\begin{itemize}"
          islist=1
        ENDIF
        PRINT replace$(a$,"*","\item ")
      ELSE
        PRINT a$
      ENDIF
    ELSE
      PRINT a$
    ENDIF
  WEND
  IF preflag=1
    preflag=0
    PRINT "\end{verbatim}"
  ENDIF
  IF islist
    PRINT "\end{itemize}"
    islist=0
  ENDIF
  IF isenum
    PRINT "\end{enumerate}"
    isenum=0
  ENDIF

RETURN

FUNCTION replacepairs$(t$,s$,r$,r2$)
  LOCAL i,count
  count=0
  i=0
  i=instr(t$,s$,i+1)
  WHILE i
    IF odd(count)
      t$=LEFT$(t$,i-1)+r2$+RIGHT$(t$,LEN(t$)-i-len(s$)+1)
    ELSE
      t$=LEFT$(t$,i-1)+r$+RIGHT$(t$,LEN(t$)-i-len(s$)+1)
    ENDIF
    INC count
    i=instr(t$,s$,i+1)
  WEND
  RETURN t$
ENDFUNCTION

FUNCTION getpagecontent$(p$)
  LOCAL page$
  LOCAL server$
  LOCAL a$,b$,t$
  ret$=""
  server$="sourceforge.net"
  page$="/apps/mediawiki/x11-basic/index.php?title="+p$+"&action=raw&"
  OPEN "UC",#1,server$,80
  PRINT #1,"GET "+page$+" HTTP/1.0"+CHR$(13)
  PRINT #1,"Host: "+server$+CHR$(13)
  PRINT #1,"User-Agent: X11-Basic/1.25"+CHR$(13)
  PRINT #1,CHR$(13)
  FLUSH #1
  LINEINPUT #1,response$
  SPLIT response$," ",0,protocol$,response$
  SPLIT response$," ",0,htmlerror$,response$
  '  print "Response: ";response$
  IF val(htmlerror$)=200
    ' Parse Header
    LINEINPUT #1,t$
    t$=REPLACE$(t$,CHR$(13),"")
    WHILE len(t$)
      ' print t$,len(t$)
      SPLIT t$,":",0,a$,b$
      IF a$="Content-Length"
        length=VAL(b$)
      ENDIF
      LINEINPUT #1,t$
      t$=REPLACE$(t$,CHR$(13),"")
    WEND
    ' print "Len=";length;" Bytes."
    IF length
      ret$=INPUT$(#1,length)
    ENDIF
  ELSE
    PRINT "Error: could not get data from the WIKI!"
  ENDIF
  CLOSE #1
  RETURN REPLACE$(ret$,CHR$(13),"")
ENDFUNCTION
