#!/bin/xbasic
' sms.bas V.1.01 (c) Markus Hoffmann 1999
' Filterprogramm fuer Emails.
' Versucht im Wesentlichen, den Inhbalt einer Email so zu komprimieren,
' dass er zwar noch lesbar ist, aber weniger Zeichen benoetigt.
' Ausserdem wird verhindert, dass persoenliche Daten wie Adresse etc.
' Als SMS verschickt werden. Man weiss ja nie, wo das alles mitgeloggt wird.
'
'
' Mailumleitung mithilfe von procmail:
' 
' ============= .procmailrc ===================
' :0 c
' * ^Subject:.SMS*
' | ( cat > /tmp/sendme.sms ; /home/user/bin/sms.bas ; rm /tmp/sendme.sms ) 
' :0 
' * ^Subject:.SMS*
' /home/user/mail/SMSmail
' ==============================================
' Mails, deren Subject mit SMS: anfaengt, werden verschickt und in separatem
' Mail-Ordner SMSmail abgelegt
' Alle anderen Mails gehen den normalen Weg.
' Procmail laeuft uebrigens unter dem Useraccount.
'
'
' Ein Protokoll der versendeten SMS mit Status, ob es geklappt hat, wird
' angelegt
'
'
' benoetigt sendsms
'
nummer$="0173-69xxxxx"
protokoll$=env$("HOME")+"/mail/sendsms.log"
sendme$="/tmp/sendme.sms"
smsreturn$="/tmp/sendsms.return.html"
spammail=FALSE


st=0
data$=""
if not exist(sendme$)
  print "Achtung: keine Mail gefunden!?? "+sendme$
  quit
endif
open "I",#1,sendme$
while not eof(#1)
  lineinput #1,t$
  wort_sep t$,":",true,a$,b$
  if upper$(a$)="FROM"
    from$=b$
    ofrom$=b$
  else if upper$(a$)="SUBJECT"
    subject$=b$
  else if upper$(a$)="TO"
    to$=trim$(b$)
  endif
  if t$=""
    st=1
  endif
  if st=1
    data$=data$+trim$(t$)+" "
  endif
wend
close #1
subject$=trim$(subject$)

from$=@ersetze$(from$,chr$(34),"")
from$=@ersetze$(from$,"'","")

wort_sep from$,"<germnews@mathematik.uni-ulm.de>",true,a$,b$
from$=trim$(a$+b$)

data$=from$+":"+subject$+";"+data$

read a$,b$
while a$<>""
  data$=@ersetze$(data$,a$,b$)
  read a$,b$
wend
data$=@ersetze$(data$,chr$(34),":")
data$=@ersetze$(data$,"'",":")

text$=data$
text$=left$(trim$(text$),159)


' Spamfilter:
restore spamdata
read a$
while a$<>""
  if glob(upper$(trim$(to$)),upper$(trim$(a$)))
    spammail=TRUE
  endif
  read a$
wend


if spammail=0
 ' system  "sendsms -o "+smsreturn$+" "+nummer$+" "+chr$(34)+text$+chr$(34)
  system  "txt2sms "+nummer$+" "+chr$(34)+text$+chr$(34)

  ' Antwort auswerten

  nosend=1
  if exist(smsreturn$)
    open "I",#1,smsreturn$
    if not eof(#1)
      lineinput #1,status$ 
      wort_sep status$,"200",0,a$,b$
      if len(b$)
        nosend=0
      endif
    endif
    close #1
  endif
  if nosend
    ' Mail ueber Errormeldung ??
  endif
else
  status$="SPAMFILTER: not sent."
endif
system "rm -f "+smsreturn$
open "A",#1,protokoll$
print #1,date$+" "+time$+" To: "+chr$(34);
print #1,to$+chr$(34)+" From: "+chr$(34);
print #1,ofrom$+chr$(34)+" Text: "+text$+" Status: "+status$
close #1
system "chmod 600 "+protokoll$
quit

edata:
data "Hallo","Hi"
data "~","-"
data "!!","!","??","?","==","=","--","-","**","*"
data "und","&"
data " and "," & "
data " ueber "," ü "
data " über "," ü "
data " von "," v "
data " oder "," o "
data " mit "," m "
data " Zeit "," Zt "
data " bei "," b "
data " fuer "," f "
data " für "," f "
data "gleich","="
data "plus","+"
data "minus","-"
data " for "," 4 "
data " to "," 2 "
data " you "," U "
data " You "," U "
data "acht","8"
data "eins","1"
data "zwei","2"
data "drei","3"
data "vier","4"
data "neun","9"
data "null","0"
data "Montag ","Mo "
data "Dienstag ","Di "
data "Mittwoch ","Mi "
data "Donnerstag ","Do "
data "Freitag ","Fr "
data "Samstag ","Sa "
data "Sonntag ","So "


data "Markus","M-"
data "markus","M-"
data "Hoffmann","H-"
data "hoffmann","H-"
data "Hoffman","H-"
data "hoffman","H-"
data "Herr ","Hr "
data "Liebe","L-"
data "Bonn","BN"
data "Physik","P-"
data "Nussallee","N-"
data "Fachschaft","F-"
data "Heidelberg","HD"
data "Deutschland ","D "
data " Germany "," D "
data " germany "," D "

data "Sehr geehrte","Sg-"
data "Gruss","xx"
data "Gruesse","xxx"
data "[","(","{","(","]",")","}",")"


data "Physikalisches Institut","PI"
data "Universitaet","Uni"
data "physik.uni-bonn.de",""
data "uni-bonn.de",""
data ".de>",".>"

data "0 MESZ","0"
data "<HTML>",""

data "This is a multi-part message in MIME format.",""
data "This is a MIME-encapsulated message",""
data "Content-Type:",""
data "Content-Transfer-Encoding:",""
data "",""

spamdata:
' To-Filter
data "*undisclosed-recipients*"
data "*Undisclosed*Recipients*"
data "<amira12a@telenowa.de>"
data "tedoopie47@planet.com.mx"
data "wessa@ucs.com.tw"
data "Verteiler1*@mailings.gmx.de"
data "SR56qr@virtuale.it"
data ""


function ersetze$(era$,erb$,erc$)
  local tta$,ttb$
  wort_sep era$,erb$,0,tta$,ttb$
  while len(ttb$)
    era$=trim$(tta$+erc$+ttb$)
    wort_sep era$,erb$,0,tta$,ttb$
  wend
  return era$
endfunction
