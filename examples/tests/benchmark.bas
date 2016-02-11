count=0
cc=0
print "Benchmarktest:"
print
goto iii
t=ctimer
for i=0 to 10000
  gosub test
next i
print "For: ";ctimer-t;" Sekunden."
print
pause 2
t=ctimer
i=0
while i<10000
  gosub test
  inc i
wend
print "While: ";ctimer-t;" Sekunden."

print
pause 2
t=ctimer
i=0
repeat
  gosub test
  inc i
until i>=10000
print "REPEAT: ";ctimer-t;" Sekunden."
iii:
print
pause 1
t=ctimer
i=0
repeat
  dec i
until i<=-30000
print "REPEAT: DEC  ";ctimer-t;" Sekunden."

print
pause 2
t=ctimer
i=0
repeat
  inc i
until i>=30000
print "REPEAT: INC  ";ctimer-t;" Sekunden."

print
pause 2
t=ctimer
i=0
repeat
  i=i+1
until i>=30000
print "REPEAT: zuweis  ";ctimer-t;" Sekunden."
t=ctimer
repeat
  a=sin(cos(exp(1)))+3-4
  inc i
until i>=80000
print "REPEAT: Formel  ";ctimer-t;" Sekunden."

quit

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
