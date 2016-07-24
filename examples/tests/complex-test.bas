' Demonstration how to use complex numbers and complex math in 
' X11-Basic V.1.23 and later. (c) Markus Hoffmann 2014-06-29
'

a#=3+4i
b#=CONJ(a#)
c#=SQRT(b#)

PRINT a#,b#,c#
t$=STR$(b#)
PRINT ENCLOSE$(t$)

QUIT
