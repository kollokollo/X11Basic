
' demonstrates string-input within ALERT boxes
' (c) Markus Hoffmann Apr. 2002

clearw 
i=1
name$="TEST01"
posx$="N54°50'32.3"
posy$="E007°50'32.3"
 t$="Edit waypoint:||Name:   "+chr$(27)+name$+"|"
  t$=t$+"Breite: "+chr$(27)+posx$+"|"
  t$=t$+"Länge:  "+chr$(27)+posy$+"|"
  t$=t$+"Höhe:   "+chr$(27)+str$(alt,5,5)+"|"
  t$=t$+"Typ:    "+chr$(27)+hex$(styp,4,4)+"|" 
  alert 0,t$,1,"OK|UPDATE|LÖSCHEN|CANCEL",a,f$
while len(f$)
  wort_sep f$,chr$(13),0,a$,f$
  print "Feld";i;": ",a$
  inc i
wend
quit
fileselect "Lade irgendwas","./*","test.txt",f$
print f$
alert 3,"Hallo| Zeile 2|HALX="+chr$(27)+"s999|Name="+chr$(27)+"Markus",1," OK |CANCEL|STOP",a,f$
print f$
quit
