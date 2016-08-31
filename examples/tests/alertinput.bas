'
' demonstrates string-input within ALERT boxes
' (c) Markus Hoffmann Apr. 2002

CLEARW
i=1
name$="TEST01"
posx$="N54°50'32.3"
posy$="E007°50'32.3"
t$="Edit waypoint:||Name:     "+CHR$(27)+name$+"|"
t$=t$+"Latitude: "+CHR$(27)+posx$+"|"
t$=t$+"Longitude:"+CHR$(27)+posy$+"|"
t$=t$+"Height:   "+CHR$(27)+STR$(alt,5,5)+"|"
t$=t$+"Type:     "+CHR$(27)+HEX$(styp,4,4)+"|"
ALERT 0,t$,1,"OK|UPDATE|DELETE|CANCEL",a,f$
WHILE LEN(f$)
  SPLIT f$,CHR$(13),0,a$,f$
  PRINT "Feld";i;": ",a$
  INC i
WEND
PRINT "BUTTON: ";a
END
