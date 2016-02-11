
input "Host:",host$
input "Pfad:",pfad$
open "UC",#1,host$,80
print #1,"GET "+pfad$
do
  lineinput #1,t$
  print t$
  
  exit if trim$(upper$(t$))="</BODY>"
loop
for i=0 to 100
a=inp(#1)
print a,
next i
close #1
