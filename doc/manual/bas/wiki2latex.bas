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
quit

procedure makepage(page$)
  LOCAL t$
  t$=@getpagecontent$(page$)
  print "\chapter{"+page$+"}"
  t$=replace$(t$,chr$(13),"")
  t$=@replacepairs$(t$,"'''","{\bf ","}")
  t$=@replacepairs$(t$,"''","{\em ","}")
  t$=@replacepairs$(t$,"===","\subsection{","}")
  t$=@replacepairs$(t$,"==","\section{","}")
      t$=replace$(t$,"$","\(DOLLAR)")
      t$=replace$(t$,"\(DOLLAR)","\$")
      t$=replace$(t$,"_","\(UNDERS)")
      t$=replace$(t$,"\(UNDERS)","\_")
      t$=replace$(t$,"&","\(ASC)")
      t$=replace$(t$,"\(ASC)","\&")
      t$=replace$(t$,"%","\(USC)")
      t$=replace$(t$,"\(USC)","\%")
      t$=replace$(t$,"^","\(EXP)")
      t$=replace$(t$,"\(EXP)","(HOCH)")
  while len(t$)
    SPLIT t$,chr$(10),0,a$,t$
    if left$(a$)=" "
      if preflag=0
        preflag=1
	print "\begin{verbatim}"
      endif
    else
      if preflag=1
         preflag=0
	 print "\end{verbatim}"
      endif
    endif
    if preflag=0
      if islist
        if left$(trim$(a$),1)<>"*"
	  print "\end{itemize}"
	  islist=0
	endif
      endif
      if left$(trim$(a$),2)="{|"
        print replace$(a$,"{|","\begin{tabular}{ccc} % ")
	inc istable
      else if left$(trim$(a$),2)="|}"
        print replace$(a$,"|}","\end{tabular} % ")
	dec istable
      else if left$(trim$(a$),2)="|-"
        print replace$(a$,"|-","\\ % ")
      else if left$(trim$(a$),1)="|"
        a$=replace$(a$,"||"," & ")
	a$=right$(a$,len(a$)-1)
	print a$
      else if left$(trim$(a$),1)="*"
        if islist=0
	  print "\begin{itemize}"
	  islist=1
	endif
	print replace$(a$,"*","\item ")
      else
        print a$
      endif
    else
      print a$
    endif
  wend
  if preflag=1
    preflag=0
    print "\end{verbatim}"
  endif
  if islist
    print "\end{itemize}"
    islist=0
  endif
  if isenum
    print "\end{enumerate}"
    isenum=0
  endif

return

function replacepairs$(t$,s$,r$,r2$)
  local i,count
  count=0
  i=0
  i=instr(t$,s$,i+1)
  while i
    if odd(count)
      t$=left$(t$,i-1)+r2$+right$(t$,len(t$)-i-len(s$)+1)
    else
      t$=left$(t$,i-1)+r$+right$(t$,len(t$)-i-len(s$)+1) 
    endif
    inc count
    i=instr(t$,s$,i+1)
  wend
  return t$
endfunction


function getpagecontent$(p$)
  local page$
  local server$
  local a$,b$,t$
  ret$=""
  server$="sourceforge.net"
  page$="/apps/mediawiki/x11-basic/index.php?title="+p$+"&action=raw&"
  open "UC",#1,server$,80
  print #1,"GET "+page$+" HTTP/1.0"+chr$(13)
  print #1,"Host: "+server$+chr$(13)
  print #1,"User-Agent: X11-Basic/1.12"+chr$(13)
  print #1,chr$(13)
  flush #1
  lineinput #1,response$
  split response$," ",0,protocol$,response$
  split response$," ",0,htmlerror$,response$
'  print "Response: ";response$
  if val(htmlerror$)=200
    ' Parse Header
    lineinput #1,t$
    t$=replace$(t$,chr$(13),"")
    while len(t$)
    '  print t$,len(t$)
      split t$,":",0,a$,b$
      if a$="Content-Length"
        length=val(b$)
      endif  
      lineinput #1,t$
      t$=replace$(t$,chr$(13),"")
    wend
   ' print "Len=";length;" Bytes."
    if length
      ret$=input$(#1,length)
    endif
  else
    print "Error: could not get data from the WIKI!"  
  endif
  close #1
  return replace$(ret$,chr$(13),"")
endfunction
