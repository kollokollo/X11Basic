' Maketex Erstellt die X11-Basic Dokumentation (A.tex) 
' der Kommando-Beschreibungen (?.sec)
' (c) Markus Hoffmann Jan. 2003
' letzte Bearbeitung 01/2016

outputfilename$="A.tex"
inputpath$="sec"

i=1
WHILE LEN(param$(i))
  IF LEFT$(param$(i))="-"
    IF param$(i)="--help" OR param$(i)="-h"
      @intro
      @using
    ELSE IF param$(i)="--version"
      @intro
      QUIT
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
      PRINT "maketex: "+inputfile$+": file or path not found"
      CLR inputfile$
    ENDIF
  ENDIF
  INC i
WEND



OPEN "O",#2,outputfilename$
letter=ASC("A")
WHILE letter<=ASC("Z")  
  g$=inputpath$+"/"+chr$(letter)+".sec"
  
  if exist(g$)
    print #2,"\section{"+chr$(letter)+"}"
    print chr$(letter);
    flush
    @doone(g$)
  endif
  inc letter
WEND
PRINT
CLOSE #2
QUIT

PROCEDURE intro
  PRINT "maketex V.1.24 (c) Markus Hoffmann 2002-2016"
RETURN
PROCEDURE using
  PRINT "Usage: maketex [options] ..."
  PRINT "Options:"
  PRINT "  -h, --help               Display this information"
  PRINT "  -o <file>                Place the output into <file>, default: "+outputfilename$
RETURN

procedure doone(f$)
  open "I",#1,f$
  clr count,vflag
  while not eof(#1)
    lineinput #1,t$
   ' if not vflag
   ' endif
    if left$(t$,4)="####"
      if vflag
        print #2,"\end{verbatim}}"
	print #2,"\end{mdframed}"
        clr vflag
      endif
      print #2,"\clearpage"
    else if left$(t$,4)="*###"
      if vflag
        print #2,"\end{verbatim}}"
	print #2,"\end{mdframed}"

        clr vflag
      endif
      print #2,"\begin{center}"
      print #2,"\begin{tabular}{c}"
      print #2,"\verb|                *                |\\"
      print #2,"\hline\end{tabular}"
      print #2,"\end{center}"
    else if left$(t$,7)="Variabl" or left$(t$,7)="Keyword" or left$(t$,7)="Command" or left$(t$,7)="Operato" or left$(t$,7)="Functio"
      wort_sep t$,":",1,a$,b$
      b$=xtrim$(b$)
      print #2,"\begin{mdframed}[hidealllines=true,backgroundcolor=red!20]"

      print #2,"{\Large \bf "+a$+":} {\Huge \bf \hspace{2cm}\verb|"+b$+"|}";
      while len(b$)
        wort_sep b$,",",1,a$,b$	
	a$=@ersetze$(@ersetze$(a$,"_","\(UNT)"),"\(UNT)","\_")
	a$=@ersetze$(@ersetze$(a$,"$","\(DOLLAR)"),"\(DOLLAR)","\$")
	a$=@ersetze$(@ersetze$(a$,"#","\(KREUZCHEN)"),"\(KREUZCHEN)","\#")
        print #2,"\index{"+a$+"|bb}";
	a$=replace$(a$,"\$","S")
	a$=replace$(a$,"\_","i") 
	a$=replace$(a$,"\%","i")
	a$=replace$(a$,"\&","u")
	a$=replace$(a$,"\#","c")
	a$=replace$(a$,"?","f")
	a$=replace$(a$," ","b")
	a$=replace$(a$,".","p")
	a$=replace$(a$,"@","a")
	a$=replace$(a$,"'","h")
	a$=replace$(a$,"~","t")
	a$=replace$(a$,"\nobreakspace","")
	a$=replace$(a$,"{}","")
	print #2,"\label{"+replace$(a$,"()","")+"}";
        b$=trim$(b$)
      wend
      print #2,"\\[5ex]" 
      print #2,"\end{mdframed}" 
    else if left$(t$,7)="Syntax:"
      print #2,"\begin{mdframed}[hidealllines=true,backgroundcolor=yellow!20]"
      wort_sep t$,":",1,a$,b$
      print #2,"\begin{tabular}{ll}"
      print #2,"{\Large {\it Syntax:}} & \verb|"+trim$(b$)+"|\\"  
      sflag=1
    else if left$(t$,7)="DESCRIP"
      if sflag
        print #2,"\end{tabular}\\[4ex]"
	print #2,"\end{mdframed}"
	sflag=0
      endif
      wort_sep t$,":",1,a$,b$
      print #2,"{\Large \bf DESCRIPTION:}\\[2ex] "+b$+""
      print #2  
    else if left$(t$,7)="EXAMPLE"
      wort_sep t$,":",1,a$,b$
      print #2,"\vspace{1cm}"
      print #2,"\begin{mdframed}[hidealllines=true,backgroundcolor=blue!20]"
      
      print #2,"\section*{\bf "+a$+":} "+b$+""
      print #2,"{\footnotesize\linespread{0.8}\begin{verbatim}"  
      vflag=true
    else if left$(t$,7)="SEE ALS"
      wort_sep t$,":",1,a$,b$
      if vflag
        print #2,"\end{verbatim}}"
	print #2,"\end{mdframed}"
        clr vflag
      endif
      b$=trim$(b$)
      print #2,"\vspace{1cm} {\large \bf SEE ALSO:}\hspace{1cm} "
      while len(b$)
        e=wort_sep(b$,",",1,a$,b$)
	a$=@ersetze$(@ersetze$(a$,"_","\(UNT)"),"\(UNT)","\_")
	a$=@ersetze$(@ersetze$(a$,"$","\(DOLLAR)"),"\(DOLLAR)","\$")
        print #2,"\index{"+a$+"}\verb|"+a$+"|";
	if e=2
	  print #2,",";
	endif
	print #2
        b$=trim$(b$)
      wend
    else
      if sflag
        if len(trim$(t$))
          print #2," & \verb|"+trim$(t$)+"|\\"  
        endif
      else
      if not vflag
      t$=replace$(t$,"$","\(DOLLAR)")
        t$=replace$(t$,"[","\(EKA)")
        t$=replace$(t$,"\(EKA)","$[$")
        t$=replace$(t$,"]","\(EKZ)")
        t$=replace$(t$,"\(EKZ)","$]$")
        t$=replace$(t$,"#","\(ASX)")
        t$=replace$(t$,"\(ASX)","\#")
   '   t$=@ersetze$(t$,"{","\(SK1)")
   '   t$=@ersetze$(t$,"\(SK1)","$\{$")
   '   t$=@ersetze$(t$,"}","\(SK2)")
   '   t$=@ersetze$(t$,"\(SK2)","$\}$")
      t$=@ersetze$(t$,"&","\(ASC)")
      t$=@ersetze$(t$,"\(ASC)","\&")
      t$=@ersetze$(t$,"%","\(USC)")
      t$=@ersetze$(t$,"\(USC)","\%")
      t$=@ersetze$(t$,"^","\(EXP)")
      t$=@ersetze$(t$,"\(EXP)","\verb|^|")
      t$=replace$(t$,"\(DOLLAR)","\$")
      t$=replace$(t$,"_","\(UNDERS)")
      t$=replace$(t$,"\(UNDERS)","\_")
      endif
        print #2,t$
      endif
    endif
    inc count
  wend
  close #1
return
function ersetze$(era$,erb$,erc$)
  local tta$,ttb$
  wort_sep era$,erb$,0,tta$,ttb$
  while era$<>tta$
    era$=tta$+erc$+ttb$
    wort_sep era$,erb$,0,tta$,ttb$
  wend
  return era$
endfunction
