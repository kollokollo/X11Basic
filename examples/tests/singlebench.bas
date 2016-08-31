ac=5000000

PRINT "empty FOR loop with ";ac;" iterations:"
t=CTIMER
FOR i=0 TO ac
NEXT i
ref=(ctimer-t)/ac
PRINT "Ref=",STR$(ref*1000,5,5);" ms"

t=ctimer
FOR i=0 TO ac










NEXT i
mes=(ctimer-t)/ac
PRINT "Mes=",STR$(mes*1000,5,5);" ms"
t=ctimer
FOR i=0 TO ac

NEXT i
ref=(CTIMER-t)/ac
PRINT "Ref=",STR$(ref*1000,5,5);" ms"
QUIT
