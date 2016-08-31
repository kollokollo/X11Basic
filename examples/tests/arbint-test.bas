' Test of the infinite precision integer arithmetric in
' X11-Basic V.1.23 and later. (c) Markus Hoffmann 2014-06-28

a&=-12345678901234567890
b&=ABS(2*a&)

c&=12345678901234567890*6789012345678

PRINT a&,b&,c&*STIMER

d&=NEXTPRIME(c&)
PRINT c&;" --prime-->";d&

f&=FACT(50)
PRINT "FACT(50)=";f&

FOR i=0 TO 5
  g&=RANDOM(f&)
  PRINT "RANDOM()=";g&
NEXT i

c&=a&^100

PRINT c&

PRINT hex$(a&)

DUMP
QUIT
