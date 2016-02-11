count=0
cc=false
clr x,y,z

dim a(10)

t$="(c) Markus Hoffmann"+" 2011"

print t$

a$="print 4711"
&a$
~random(3)


a(1=2)=4+true

print "Benchmarktest:"
print
print "FOR-empty:   ";
flush
' goto iii
t=ctimer
for i=0 to 5000000
  '
next i
dur=ctimer-t
ff=int(8.31/dur*100)/100
print dur;" Sekunden. ";ff;" mal."

print "FOR-gosub:   ";
flush
t=ctimer
for i=0 to 500000
  gosub test
next i
dur=ctimer-t
ff=int(8.51/dur*100)/100
print dur;" Sekunden. ";ff;" mal."

print "WHILE-gosub:  ";
flush
t=ctimer
i=0
while i<500000
  gosub test
  inc i
wend
dur=ctimer-t
ff=int(8.89/dur*100)/100
print dur;" Sekunden. ";ff;" mal."

print "REPEAT-gosub: ";
flush
t=ctimer
i=0
repeat
  gosub test
  inc i
until i>=500000
print ctimer-t;" Sekunden. ";int(8.78/(ctimer-t)*100)/100;" mal."
iii:
print

t=ctimer
i=0
repeat
  dec i
until i<=-5000000
print "REPEAT";i;": DEC  ",ctimer-t;" Sekunden. ";int(16.67/(ctimer-t)*100)/100;" mal."
print

t=ctimer
i=0
repeat
  inc i
until i>=5000000
print "REPEAT";i;": INC  ",ctimer-t;" Sekunden. ";int(12.69/(ctimer-t)*100)/100;" mal."

t=ctimer
i=0
do
  inc i
  exit if i>=5000000
loop
print "DO-LOOP";i;": INC  ",ctimer-t;" Sekunden. ";int(13.23/(ctimer-t)*100)/100;" mal."

t=ctimer
i=0
repeat
  i=i+1
until i>=500000
print "REPEAT: zuweis  ",ctimer-t;" Sekunden. ";1.87/(ctimer-t);" mal."
clr i
t=ctimer
repeat
  a=sin(cos(exp(1)))+3-4*(2+5-2*3)
  inc i
until i>=5000000
print "REPEAT";i;": const-Formel  ",ctimer-t;" Sekunden. ";int(87.22/(ctimer-t)*100/100);" mal."
clr i
t=ctimer
repeat
  a=sin(cos(exp(i)))+i-4*(2+i-2*3)
  inc i
until i>=5000000
print "REPEAT";i;": Formel2  ",ctimer-t;" Sekunden. ";int(86.32/(ctimer-t)*100)/100;" mal."
'
clr i
t=ctimer
while i<500000
  a=1
  inc i
wend
print "WHILE-pur";i;": ",ctimer-t;" Sekunden. ";1.79/(ctimer-t);" mal."
'
clr i
t=ctimer
while i<500000
  a=@f(x)
  inc i
wend
print "WHILE-DEFFN";i;": ",ctimer-t;" Sekunden. ";int(4.24/(ctimer-t)*100)/100;" mal."
'
clr i
t=ctimer
while i<500000
  a=@g(x)
  inc i
wend
print "WHILE-FUNCT";i;": ",ctimer-t;" Sekunden. ";int(4.81/(ctimer-t)*100)/100;" mal."

quit

DEFFN f(x)=2*x+sin(x/40)

function g(x)
  return 2*x+sin(x/40)
endfunction

PROCEDURE test
' Testroutine
  a=sqrt(x^2+y^2+z^2)

  inc count
  if (count mod 500)=0
      color random(256)
      pbox random(320),random(200),random(320),random(200)
      vsync
     inc cc      
  endif
return
