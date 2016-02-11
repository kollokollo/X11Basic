' KLeiner Internetserver auf Port 5555

open "US",#1,"",5000
do
  open "UA",#2,"",1
  print #2,"Welcome..."
  flush #2
  for t=0 to 20
  lineinput #2,t$
  print "got: "+t$
  next t

  close #2
  exit if left$(t$,4)="quit"
loop
close
quit
