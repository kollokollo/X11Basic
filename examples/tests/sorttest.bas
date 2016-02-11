' Test of the SORT function (c) Markus Hoffmann 2010
' written in X11-Basic (> 1.16)
'

n%=1000000
DIM t$(n%),u%(n%)

t=TIMER
PRINT "filling the string array with ";n%;" strings ..."
FOR i%=0 TO n%-1
  t$(i%)=STR$(RANDOM(n%))
  u%(i%)=i%
NEXT i%
PRINT "this took ";TIMER-t;" seconds."
PRINT "Sort the (string) array..."
t=TIMER
SORT t$(),n%,u%()
t1=TIMER-t
FOR i%=0 TO 20
  PRINT i%,t$(i%),u%(i%)
NEXT i%
PRINT "Time: ";t1;" Seconds."
PRINT "(back-)sort the int array:"
t=TIMER
SORT u%(),n%
t2=TIMER-t

FOR i%=0 TO 4
  PRINT i%,u%(i%)
NEXT i%
PRINT "Time: ";t2;" Seconds."
QUIT
