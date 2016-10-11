' Test of the bluetooth functions of X11-Basic Version >= 1.25
'
'
' Scan for bluetooth devies around:
'
'
PRINT "Following devices could be found nearby:"
a$=FSFIRST$("","*","b")
while len(a$)
  print a$
  PRINT "Adress: ";WORD$(a$,1)
  PRINT "Name:   ";WORD$(a$,2)
  adr$=word$(a$,1)
  a$=FSNEXT$()
wend
PRINT "Now try to connect to ";adr$
OPEN "UL",#1,adr$,1
print #1,"Test"
CLOSE 

quit
