ac=5000000

print "empty FOR loop with ";ac;" iterations:"
t=ctimer
for i=0 to ac
next i
ref=(ctimer-t)/ac
print "Ref=",str$(ref*1000,5,5);" ms"

t=ctimer
for i=0 to ac










next i
mes=(ctimer-t)/ac
print "Mes=",str$(mes*1000,5,5);" ms"
t=ctimer
for i=0 to ac

next i
ref=(ctimer-t)/ac
print "Ref=",str$(ref*1000,5,5);" ms"

quit
