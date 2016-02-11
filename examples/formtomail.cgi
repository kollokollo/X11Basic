#!/bin/xbasic

' Formtomail.cgi (c) Markus Hoffmann 2001
' Das Programm verschickt Emails mit dem Inhalt des Formulars

' Die Prozedur formtomail.cgi bewirkt, dass die ins Formular
' eingegebenen Parameter mit ihren Werten per Mail an die Adresse
' geschickt werden, die in dem Input-Feld mit dem Namen "Mailto" unter
' "Value" angegeben ist. Die beiden Input-Felder "Mailto" und "Subject"
' muessen genau in der unten spezifizierten Form angegeben werden
' (Gross-/Kleinschreibung bei "Mailto" und "Subject" beachten). Das
' Input-Feld "Subject" ist dafuer da, dass der Benutzer bei mehreren
' verschiedenen Formularen eine Zuordnung vornehmen kann (da es nicht
' moeglich ist, den Text des Formulars mitzuschicken, sondern nur die
' Parameter). Diese beiden Felder haben das Attribut "hidden", damit der
' Benutzer, der das Formular ausfuellt, sie nicht veraendern kann. >

print "Content-type: text/html"+chr$(13)
print ""+chr$(13)
flush
dim value$(200)
t$=""

print "<html><head></head><body>"
print "Vielen Dank f&uuml;r das Ausf&uuml;llen des Formulars.<p>"
print "Es wird so schnell wie m&ouml;glich bearbeitet...<p>"
print "<pre>"


  r$=env$("REMOTE_ADDR")
  h$=env$("REMOTE_HOST")
  refer$=env$("HTTP_REFERER")
  length=val(env$("CONTENT_LENGTH"))

if length
  for i=0 to length-1
    t$=t$+chr$(inp(-2))
  next i
orig$=t$
count=0
wort_sep t$,"&",1,a$,t$
while len(t$)
  value$(count)=a$
  inc count
  print a$
  wort_sep t$,"&",1,a$,t$
wend
value$(count)=a$
inc count
print a$
endif
if count
  for i=0 to count-1 
   wort_sep value$(i),"=",1,a$,b$ 
   if a$="Mailto"
     mailto$=b$
   endif
   if a$="Subject"
     subject$=b$
   endif
  next i
  mailto$=@ersetze$(mailto$,"%40","@")

  flush
  if upper$(right$(mailto$,3))=".DE"
    s$="mail -s "+chr$(34)+"[Form-to-Mail]: "+subject$+chr$(34)+" "+mailto$+" << EOF"+chr$(13)
    for i=0 to count-1 
      s$=s$+value$(i)+chr$(10)
    next i
    s$=S$+chr$(10)+"EOF"+chr$(10)
    system s$
    status$="sent to "+mailto$
  else 
    status$="unsent ("+mailto$+")"
  endif
endif


print "</pre>"
print "</body></html>"
flush  

open "A",#1,"/tmp/WEBformtomail.log"
print #1,date$+" "+time$+" "+r$+" "+h$+" ";
print #1,"B="+chr$(34)+refer$+chr$(34)+" ";
print #1,"C="+chr$(34)+orig$+chr$(34)+" Status="+status$
close #1

quit

function ersetze$(era$,erb$,erc$)
  local tta$,ttb$
  wort_sep era$,erb$,0,tta$,ttb$
  while len(ttb$)
    era$=trim$(tta$+erc$+ttb$)
    wort_sep era$,erb$,0,tta$,ttb$
  wend
  return era$
endfunction
