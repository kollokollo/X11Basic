' KLeiner Internetserver auf Port 5555

open "US",#1,"",5556
ff=0
do
  open "UA",#2,"",1
  print #2,"Welcome..."
  flush #2
  lineinput #2,t$
  while ff<3
  if len(t$)
    ff=0
  else
  inc ff
  endif
  print "got: "+t$
  lineinput #2,t$
  wend
  close #2
  exit if left$(t$,4)="quit"
loop
close
quit
