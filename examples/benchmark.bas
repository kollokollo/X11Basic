echo off
cls
print "Benchmarktest:"
print
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

end

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
