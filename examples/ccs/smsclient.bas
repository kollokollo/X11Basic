host$="elsahp7"
csput "SUP_SMS_MESSAGE.STATUS_SM",at$
open "UC",#1,host$,7007

lineinput #1,g$
print g$
flush #1
name$=csget$("SUP_SMS_MESSAGE.NAME_SC")
text$=csget$("SUP_SMS_MESSAGE.MESSAGE_SC")

print #1,name$+chr$(13)
flush #1

pause 2

print #1,text$+chr$(13)
flush #1

at$="Antwort:"  
lineinput #1,t$
for i=0 to 6
  lineinput #1,t$
  at$=at$+"|"+t$
next i
csput "SUP_SMS_MESSAGE.STATUS_SM",at$
close #1
quit


