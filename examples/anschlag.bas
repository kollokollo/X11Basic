print "ESC=ABBRUCH"
system "cat vorgabe.dat"
open "O",#1,"tippdata.dat"
print "Los gehts... (wenn los, mit SPACE Starten...)"
a=inp(-2)
print "LOS:"
t=timer
clr c
a=1
while a<>27
  a=inp(-2)

  print chr$(a);
  print #1,chr$(a);
  flush
  
  if a=8
    dec c
  else
    inc c
  endif
  if (timer-t)>=15
    print
    print "Anschlaege: "+str$(int(c/(timer-t)*60))+" pro Min."
    clr c
    t=timer
  endif
wend
close 
system "wdiff tippdata.dat vorgabe.dat"
quit
