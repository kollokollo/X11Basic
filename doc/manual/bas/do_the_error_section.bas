PRINT "XXXX \=     X11-Basic Error messages:           \kill    \\"
FOR i=0 TO 255
  t$=STR$(i)+" \> "+err$(i)+"\\"
  t$=REPLACE$(t$,"%s","")
  t$=REPLACE$(t$,"#","\#")
  t$=REPLACE$(t$,"_","\_")
  IF word$(TRIM$(err$(i)),2)<>str$(i) AND WORD$(TRIM$(err$(i)),2)<>str$(i-256)
    PRINT t$
  ENDIF
NEXT i
QUIT
