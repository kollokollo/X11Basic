ac=500000

t=ctimer
for i=0 to ac
next i
ref=(ctimer-t)/ac
print "Ref=",str$(ref*1000,5,5);" ms"

t=ctimer
for i=0 to ac










next i
mes=(ctimer-t)/ac-ref
print "Mes=",str$(mes*1000,4,4);" ms"
quit
