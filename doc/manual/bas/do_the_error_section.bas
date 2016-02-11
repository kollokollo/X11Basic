print "XXXX \=     X11-Basic Error messages:           \kill    \\"
for i=0 to 255
  t$=str$(i)+" \> "+err$(i)+"\\"
  t$=replace$(t$,"%s","")
  t$=replace$(t$,"#","\#")
  t$=replace$(t$,"_","\_")
  if  word$(trim$(err$(i)),2)<>str$(i) and word$(trim$(err$(i)),2)<>str$(i-256)
    print t$
  endif
next i
quit
