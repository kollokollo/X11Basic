#!/bin/xbasic
echo off
'
' brabbel.cgi von Markus Hoffmann (Jan 2001)
'
host$=""
print "Content-type: text/html"
print 
t$=env$("REQUEST_URI")
wort_sep t$,"?",1,a$,t$
iq=4
ut=0
wort_sep t$,"&",1,a$,t$
while len(a$)
  wort_sep a$,"=",1,b$,a$
  if b$="url"
    url$=a$
  else if b$="IQ"
    iq=min(15,max(val(a$),1))
  else if b$="button"
    if a$="URL+Testen"
      ut=1
    endif
  else 
    print "Unbekannte Option: "+b$+" : "+a$+"<p>"
  endif
  wort_sep t$,"&",1,a$,t$
wend

t$=url$
@ersetzen

tmpfile$="/tmp/brabbeltmp"+str$(stimer)
url$=t$

if len(url$)=0
  url$="http://cips01.physik.uni-bonn.de/~hoffmann/texte/party.html"
endif

if left$(url$,5)="http:"
  wort_sep url$,"//",1,uhost$,url$
endif
wort_sep url$,"/",1,uhost$,url$
url$="/"+url$

print "<HTML> <HEAD> <TITLE>Markus Hoffmann's Webgebrabbel</TITLE></HEAD>"
print "<BODY bgcolor="#ffffff" link=2200aa vlink=008800><center>"
print "<H1> Webgebrabbel</H1><HR>"
print "Geben Sie die Web-Seite ein, &uuml;ber die gebrabbelt werden soll. Der Intelligenzquotient bestimmt, "
print "wie sinnig der Ausgabetext sein soll: 1=unsinnig -- 10=Orginaltext. Werte 3,4,5 sind sinnvoll."
print "<form name=querybox action="+host$+"/cgi-bin/brabbel.cgi method=get>"
print "<font size=2 color=000000>"

print "URL=<input type=text name=url value="+chr$(34)+"http://"+uhost$+url$+chr$(34)+" size=60><br>"
print "Intelligenzquotient=<input type=text name=IQ value="+chr$(34)+str$(iq)+chr$(34)+" size=2>"


  r$=env$("REMOTE_ADDR")
  h$=env$("REMOTE_HOST")
  open "A",#1,"/tmp/WEBBrabbel.log"
  print #1,date$+" "+time$+" "+r$+" "+h$+" ";
  print #1,"UT="+str$(ut)+" IQ="+str$(iq)+" ";
  print #1,chr$(34)+"http://"+uhost$+url$+chr$(34)
  close #1

print "<input name=button value="+chr$(34)+"Losbrabbeln"+chr$(34)+" type="+chr$(34)+"submit"+chr$(34)+">"
print "<input name=button value="+chr$(34)+"URL Testen"+chr$(34)+" type="+chr$(34)+"submit"+chr$(34)+">"
print "</font>"
print "</form><p></center><HR><font size=2 color=ff0000>"
flush
s$="wget -O "+tmpfile$+".brab "+chr$(34)+uhost$+url$+chr$(34)+" ; "
s$=s$+"html2text -nobs "+tmpfile$+".brab > "+tmpfile$+" ; "
s$=s$+"rm -f "+tmpfile$+".brab"
system s$
if ut=0
open "I",#1,tmpfile$
l=lof(#1)
t$=space$(l+1)
close #1
bload tmpfile$,varptr(t$),l
u$=trim$(t$)
read a$,b$
while a$<>""
  u$=@ersetze$(u$,a$,b$)
  read a$,b$
wend
bsave tmpfile$,varptr(u$),len(u$)

  s$="/usr/local/bin/brabbel -intelligenz "+str$(iq)+" -laenge "+str$(min(l,5000))+" -input "+tmpfile$
else 
  s$="echo '</font><font size=2 color=1000ff><pre>' ; cat "+tmpfile$
endif
s$=s$+" ; "+"rm -f "+tmpfile$
system s$

print "</font></pre><hr><font size=1>*** Webgebrabbel Version: 1.00 (c) Markus Hoffmann "
print " *** letzte Bearbeitung 5.1.2001 </font><hr>"
print "<I>Kommentare oder Anregungen zu dieser WWW-Seite bitte "
print "<A HREF=mailto:hoffmann@physik.uni-bonn.de>hierhin</A>.</I><P>"
print "<FONT FACE="+chr$(34)+"ARIAL,HELVETICA"+chr$(34)+" SIZE=1>"
print "Erzeugt am "+time$+" "+date$
print "</FONT></BODY></HTML>"
quit

data "~","-"
data "!!","!","??","?","==","=","--","-","**","*"
data "",""

procedure ersetzen
  local y$,a$,b$,i
  for i=20 to 255
    y$="%"+right$(upper$(hex$(i)),2)
    wort_sep t$,y$,1,a$,b$
    while a$<>t$
      t$=a$+chr$(i)+b$
      wort_sep t$,y$,1,a$,b$
    wend
  next i
return

function ersetze$(era$,erb$,erc$)
  local tta$,ttb$
  wort_sep era$,erb$,0,tta$,ttb$
  while len(ttb$)
    era$=trim$(tta$+erc$+ttb$)
    wort_sep era$,erb$,0,tta$,ttb$
  wend
  return era$
endfunction
