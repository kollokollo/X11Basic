' Another benchmark test for X11-Basic
'
count=0
cc=false
CLR x,y,z

DIM a(10)

t$="(c) Markus Hoffmann"+" 2011"

PRINT t$

a$="print 4711"
&a$
~random(3)

a(1=2)=4+true

PRINT "Benchmarktest:"
PRINT
PRINT "FOR-empty:   ";
FLUSH
' goto iii
t=ctimer
FOR i=0 TO 5000000
  '
NEXT i
dur=ctimer-t
ff=INT(8.31/dur*100)/100
PRINT dur;" Sekunden. ";ff;" mal."

PRINT "FOR-gosub:   ";
FLUSH
t=ctimer
FOR i=0 TO 500000
  GOSUB test
NEXT i
dur=ctimer-t
ff=INT(8.51/dur*100)/100
PRINT dur;" Sekunden. ";ff;" mal."

PRINT "WHILE-gosub:  ";
FLUSH
t=ctimer
i=0
WHILE i<500000
  GOSUB test
  INC i
WEND
dur=ctimer-t
ff=INT(8.89/dur*100)/100
PRINT dur;" Sekunden. ";ff;" mal."

PRINT "REPEAT-gosub: ";
FLUSH
t=ctimer
i=0
REPEAT
  GOSUB test
  INC i
UNTIL i>=500000
PRINT ctimer-t;" Sekunden. ";INT(8.78/(ctimer-t)*100)/100;" mal."
iii:
PRINT

t=ctimer
i=0
REPEAT
  DEC i
UNTIL i<=-5000000
PRINT "REPEAT";i;": DEC  ",ctimer-t;" Sekunden. ";INT(16.67/(ctimer-t)*100)/100;" mal."
PRINT

t=ctimer
i=0
REPEAT
  INC i
UNTIL i>=5000000
PRINT "REPEAT";i;": INC  ",ctimer-t;" Sekunden. ";INT(12.69/(ctimer-t)*100)/100;" mal."

t=ctimer
i=0
DO
  INC i
  EXIT if i>=5000000
LOOP
PRINT "DO-LOOP";i;": INC  ",ctimer-t;" Sekunden. ";INT(13.23/(ctimer-t)*100)/100;" mal."

t=ctimer
i=0
REPEAT
  i=i+1
UNTIL i>=500000
PRINT "REPEAT: zuweis  ",ctimer-t;" Sekunden. ";1.87/(ctimer-t);" mal."
CLR i
t=ctimer
REPEAT
  a=sin(cos(exp(1)))+3-4*(2+5-2*3)
  INC i
UNTIL i>=5000000
PRINT "REPEAT";i;": const-Formel  ",ctimer-t;" Sekunden. ";INT(87.22/(ctimer-t)*100/100);" mal."
CLR i
t=ctimer
REPEAT
  a=sin(cos(exp(i)))+i-4*(2+i-2*3)
  INC i
UNTIL i>=5000000
PRINT "REPEAT";i;": Formel2  ",ctimer-t;" Sekunden. ";INT(86.32/(ctimer-t)*100)/100;" mal."
'
CLR i
t=ctimer
WHILE i<500000
  a=1
  INC i
WEND
PRINT "WHILE-pur";i;": ",ctimer-t;" Sekunden. ";1.79/(ctimer-t);" mal."
'
CLR i
t=ctimer
WHILE i<500000
  a=@f(x)
  INC i
WEND
PRINT "WHILE-DEFFN";i;": ",ctimer-t;" Sekunden. ";INT(4.24/(ctimer-t)*100)/100;" mal."
'
CLR i
t=ctimer
WHILE i<500000
  a=@g(x)
  INC i
WEND
PRINT "WHILE-FUNCT";i;": ",ctimer-t;" Sekunden. ";INT(4.81/(ctimer-t)*100)/100;" mal."

QUIT

DEFFN f(x)=2*x+sin(x/40)

FUNCTION g(x)
  RETURN 2*x+sin(x/40)
ENDFUNCTION

PROCEDURE test
  ' Testroutine
  a=SQRT(x^2+y^2+z^2)

  INC count
  IF (count MOD 500)=0
    COLOR RANDOM(256)
    PBOX RANDOM(320),RANDOM(200),RANDOM(320),RANDOM(200)
    SHOWPAGE
    INC cc
  ENDIF
RETURN
