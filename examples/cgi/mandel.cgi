#!/bin/xbasic
echo off
host$=""
'
' mandel.cgi (c) Markus Hoffmann 1999   V. 1.01
' darf mit dem Paket X11-Basic weitergegeben werden
'

print "Content-type: text/html"
print 
t$=env$("REQUEST_URI")
wort_sep t$,"?",1,a$,t$
wort_sep t$,"?",1,t$,u$

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

if len(u$)
  wort_sep u$,",",1,x$,y$
  nx=val(x$)/256*(x2-x1)+x1
  ny=(256-val(y$))/256*(y2-y1)+y1
  u=abs(x2-x1)
  x1=nx-u/4
  x2=nx+u/4
  u=abs(y2-y1)
  y1=ny-u/4
  y2=ny+u/4
endif
if x1=-2
  r$=env$("REMOTE_ADDR")
  h$=env$("REMOTE_HOST")
  open "A",#1,"/tmp/WEBMANDEL.log"
  print #1,date$+" "+time$+" "+r$+" "+h$
  close #1
endif

gifname$="mandelgif.cgi?x1="+str$(x1)+"&x2="+str$(x2)+"&y1="+str$(y1)+"&y2="+str$(y2)

print "<HTML> <HEAD> <TITLE>Markus Hoffmann's Mandelbrodmenge</TITLE></HEAD>"
print "<BODY bgcolor="#ffffff" link=2200aa vlink=008800><center>"
print "<H1> Die Mandelbrodmenge interaktiv</H1><HR>"
print "Klicken Sie in das Bild, um einen Bereich zu vergr&ouml;&szlig;ern. Alternativ k&ouml;nnen Sie die Koordinaten auch eingeben."
print "<form name=querybox action="+host$+"/cgi-bin/mandel.cgi method=get>"
print "<a href="+host$+"/cgi-bin/mandel.cgi?x1="+str$(x1)+"&x2="+str$(x2)+"&y1="+str$(y1)+"&y2="+str$(y2)+"><img src=/cgi-bin/"+gifname$+" ismap align=left></a>"
print "X1=<input type=text name=x1 value="+str$(x1)+" size=16><br>"
print "X2=<input type=text name=x2 value="+str$(x2)+" size=16><br>"
print "y1=<input type=text name=y1 value="+str$(y1)+" size=16><br>"
print "Y2=<input type=text name=y2 value="+str$(y2)+" size=16><br>"
print "<input value="+chr$(34)+"Koordinaten Berechnen"+chr$(34)+" type="+chr$(34)+"submit"+chr$(34)+">"
print "<a href="+host$+"/cgi-bin/mandel.cgi?> Zur&uuml;ck zum Start </a><p>"
print "<a href="+host$+"/cgi-bin/mandelposter.cgi?x1="+str$(x1)+"&x2="+str$(x2)+"&y1="+str$(y1)+"&y2="+str$(y2)+"> Ausschnitt als 800x800 Poster </a>"
print "<input type=hidden name=format value=2>"
print "</form><p></center><HR><br>"
print "<I>Kommentare oder Anregungen zu dieser WWW-Seite bitte "
print "<A HREF=mailto:hoffmann@physik.uni-bonn.de>hierhin</A>.</I><P>"
print "<FONT FACE="+chr$(34)+"ARIAL,HELVETICA"+chr$(34)+" SIZE=1>"
print "Erzeugt am "+time$+" "+date$
print "</FONT></BODY></HTML>"
quit
