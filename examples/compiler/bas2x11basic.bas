' Utility to convert ANSI BASIC Programs to X11Basic
' (c) Markus Hoffmann 2003 (letzte Bearbeitung: 10.12.2002)
' (letzte Bearbeitung: 10.02.2004)
'
'
i=1
outputfilename$="b.bas"
while len(param$(i))
  if left$(param$(i))="-"
    if param$(i)="--help" or param$(i)="-h"
      @intro
      @using
    else if param$(i)="--version"
      @intro
      quit
    else if param$(i)="-o"
      inc i
      if len(param$(i))
        outputfilename$=param$(i)
      endif
    endif
  else
    inputfile$=param$(i)
    if not exist(inputfile$)
      print "bas2x11basic: "+inputfile$+": file or path not found"
      clr inputfile$
    endif
  endif
  inc i
wend
if len(inputfile$)
  rumpf$=inputfile$
  while len(rumpf$)
    wort_sep rumpf$,"/",1,a$,rumpf$
  wend
  f$=a$
  rumpf$=a$
  wort_sep a$,".",1,rumpf$,typ$
  if typ$="bas"
    if exist(outputfilename$)
      print "bas2x11basic: Outputfilename already exists: ";outputfilename$
    else
      @convert
    endif
  else 
    print f$+": file not recognized: File format not recognized"
  endif
else
  print "bas2x11basic: No input files"
endif
quit
procedure intro
  print "ANSI BASIC to X11-Basic Converter V.1.10 (c) Markus Hoffmann 2003-2004"
  version
return
procedure using
  print "Usage: bas2x11basic [options] file..."
  print "Options:"
  print "  -h, --help               Display this information"
  print "  -o <file>                Place the output into <file>"
return
procedure convert
  dim linenr(3000),zeile$(3000),labeling(3000),procs(3000)
  anzlabel=0
  anzprocs=0
  anzline=0
  open "O",#2,outputfilename$
  print #2,"' bas2x11basic V.1.10 ("+f$+")"
  print #2,"'      (c) Markus Hoffmann "+date$+" "+time$
  ' PASS 1
  print "PASS 1"
  pass=1
  open "I",#1,inputfile$
  while not eof(#1)
    lineinput #1,t$
    t$=xtrim$(t$)
    if len(t$)
      wort_sep t$," ",1,b$,t$
      linenr(anzline)=val(b$)
      zeile$(anzline)=t$
      inc anzline
      if upper$(left$(t$,2))="IF"
        @processifline(t$)
      else
        @processline(t$)
      endif
    endif
  wend
  close #1
  print #2,"' ";anzline;" lines."
  print #2,"' ";anzlabel;" labels."
  print #2,"' ";anzprocs;" procs."
  print #2,"' ----- Start of program -----"
 ' PASS 2
  print "PASS 2"
  pass=2
  
  for i=0 to anzline-1
    flag=0
    lln=linenr(i)
    for j=0 to anzlabel-1
      if labeling(j)=lln
        flag=1
      endif
    next j
    if flag
      print #2,"L";lln;":"
    endif
    flag=0
    lln=linenr(i)
    for j=0 to anzprocs-1
      if procs(j)=lln
        flag=1
      endif
    next j
    if flag
      print #2,"PROCEDURE P";lln
    endif
    tt$=trim$(zeile$(i))
    if upper$(left$(tt$,2))="IF"
      @processifline(tt$)
    else
      @processline(tt$)
    endif
  next i
  print #2,"' ----- End of program -----"
  close #2
return

procedure processline(t$)
  t$=replace$(t$,", ",",")
  while len(t$)
    wort_sep t$,":",1,b$,t$
    b$=xtrim$(b$)
      b$=replace$(b$," REM "," ! ")
      b$=replace$(b$," = ","=")
      b$=replace$(b$,"= ","=")
      b$=replace$(b$," + ","+")
      b$=replace$(b$," - ","-")
      b$=replace$(b$," * ","*")
      b$=replace$(b$," / ","/")
      b$=replace$(b$," ^ ","^")
      b$=replace$(b$," > ",">")
      b$=replace$(b$," < ","<")
      b$=replace$(b$," <> ","<>")
      @processcommand(b$)
  wend
return
procedure processifline(t$)
  local ifauf
  if pass<>1
    print #2,"' "+t$
  endif
  t$=replace$(t$,", ",",")
  while len(t$)
    wort_sep t$,":",1,b$,t$
    b$=xtrim$(b$)
    b$=replace$(b$," REM "," ! ")
    b$=replace$(b$," = ","=")
    b$=replace$(b$,"= ","=")
    b$=replace$(b$," + ","+")
    b$=replace$(b$," - ","-")
    b$=replace$(b$," * ","*")
    b$=replace$(b$," / ","/")
    b$=replace$(b$," ^ ","^")
    b$=replace$(b$," > ",">")
    b$=replace$(b$," < ","<")
    b$=replace$(b$," <> ","<>")
    wort_sep b$," ",1,b$,a$
    if upper$(b$)="IF"
      wort_sep a$,"THEN ",1,a$,c$
      if len(c$)=0
	wort_sep a$," ",1,a$,c$
      endif
      if pass<>1
        print #2,"IF "+a$ 
      endif
      
      if len(c$)
	wort_sep c$,"ELSE ",1,c$,d$
	  if c$=str$(val(c$))
            @processcommand("GOTO "+c$)
	  else
            @processcommand(c$)
          endif
	  if len(d$)
	    if pass<>1
	      print #2,"ELSE"
	    endif
	    if d$=str$(val(d$))
              @processcommand("GOTO "+d$)
	    else
              @processcommand(d$)
            endif
	  endif
          ifauf=1
      endif	  
    else
      @processcommand(b$+" "+a$)
    endif	
  wend
  if ifauf
    if pass<>1
      print #2,"ENDIF"
    endif
  endif
return

procedure processcommand(b$)
  b$=replace$(b$,"PRINT"+chr$(34),"PRINT "+chr$(34))
  b$=replace$(b$,"INPUT"+chr$(34),"INPUT "+chr$(34))
  wort_sep b$," ",1,b$,a$
  if upper$(b$)="GOTO"
        wort_sep a$," ",1,a$,c$
	if pass=1
	  labeling(anzlabel)=val(a$)
	  inc anzlabel
	else
          print #2,b$;" L";a$+" "+c$         
	endif
      else if upper$(b$)="GOSUB"
        wort_sep a$," ",1,a$,c$
	if pass=1
          procs(anzprocs)=val(a$)
	  inc anzprocs
	else
          print #2,"@P";a$+" "+c$         
	endif
      else if upper$(b$)="IF"
	print #2,"' ERROR: No if here !"      
      else
        if pass=1
	else
        print #2,b$;" ";a$
      endif	
      endif
return
