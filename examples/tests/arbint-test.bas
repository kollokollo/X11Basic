' Test of the infinite precision integer arithmetric in 
' X11-Basic V.1.23 and later. (c) Markus Hoffmann 2014-06-28


a&=-12345678901234567890
b&=ABS(2*a&)

c&=12345678901234567890*6789012345678

print a&,b&,c&*STIMER


d&=NEXTPRIME(c&)
print c&;" --prime-->";d&

f&=FACT(50)
print "FACT(50)=";f&

for i=0 to 5
g&=RANDOM(f&)
print "RANDOM()=";g&
next i

c&=a&^100



print c&

print hex$(a&)

dump
quit
