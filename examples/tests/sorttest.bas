' Test of the SORT function (c) Markus Hoffmann 2010
' works only with version 1.16
'

n=1000000

dim t$(n)
dim u%(n)

for i=0 to n-1
  t$(i)=str$(random(n))
  u%(i)=i
next i
print "los."
t=timer
 sort t$(),n,u%()
t1=timer-t
for i=0 to 20
  print i,t$(i),u%(i)
next i
print "Zeit: ";t1;" Sek."
t=timer
 sort u%(),n
t2=timer-t

for i=0 to 20
  print i,t$(i),u%(i)
next i
print "Zeit: ";t2;" Sek."
quit
