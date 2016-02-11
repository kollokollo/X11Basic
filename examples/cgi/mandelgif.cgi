#!/bin/xbasic
echo off
'
' mandelgif.cgi (c) Markus Hoffmann 1999   V. 1.01
' darf mit dem Paket X11-Basic weitergegeben werden
'
t$=env$("REQUEST_URI")
wort_sep t$,"?",1,a$,t$
if len(t$)<2
  x1=-2
  y1=-2
  x2=2
  y2=2
else
  wort_sep t$,"&",1,a$,t$
  while len(a$)
    wort_sep a$,"=",1,a$,b$
    if a$="x1"
      x1=val(b$)
    else if a$="x2"
      x2=val(b$)
    else if a$="y1"
      y1=val(b$)
    else if a$="y2"
      y2=val(b$)
    endif
    wort_sep t$,"&",1,a$,t$
  wend
endif

' print "/usr/local/bin/mandelraw "+str$(x1)+" "+str$(x2)+" "+str$(y1)+" "+str$(y2)+" | raw2gif -p /home/hoffmann/bin/colormap -s 256 256 > /tmp/testm.gif"
system "echo 'Content-type: image/png';echo ; /usr/local/bin/mandelraw "+str$(x1)+" "+str$(x2)+" "+str$(y1)+" "+str$(y2)+" | raw2gif -p /usr/local/httpd/cgi-bin/colormap -s 256 256 | /usr/X11R6/bin/giftopnm | /usr/X11R6/bin/pnmtopng -inter"

quit
