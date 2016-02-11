' Kleine Laufschrift auf dem Keithley im Bedienungsraum
' Markus Hoffmann 1998

echo off
clr i
a$="GUTEN@TAG,@FROHES@SCHAFFEN@@@"

do
  t$=mid$(a$+a$,i mod len(a$),10)
  csput "ELS_DIAG_TOROID.IECONTROL_SC","D"+t$+"X"
  pause 0.5
  print t$+chr$(13);
  flush
  inc i
loop


quit
