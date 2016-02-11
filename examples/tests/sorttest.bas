' Test of the SORT function (c) Markus Hoffmann 2010
' works only with version 1.16
'

n=1000000

dim t$(n)
dim u%(n)

t=timer
print "filling the array with ";n;" values ..."
for i=0 to n-1
  t$(i)=STR$(RANDOM(n))
  u%(i)=i
next i
print "this took ";timer-t;" seconds."
print "Sort the (string) arrays..."
t=timer
 sort t$(),n,u%()
t1=timer-t
for i=0 to 20
  print i,t$(i),u%(i)
next i
print "Time: ";t1;" Seconds."
print "(back-)sort the int array:"
t=timer
 sort u%(),n
t2=timer-t

for i=0 to 10
  print i,u%(i)
next i
print "Time: ";t2;" Seconds."
quit
