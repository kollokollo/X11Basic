 ' Is there something wrong with the random generator ?
 dim m(10)
 n=20000
 randomize
t=ctimer 
print "MIN:"
 for a=1 to n
   m=10
   for b=1 to 10
     m=min(m,round(random(100)/10))
   next b
   m(m)=m(m)+1
 next a
 
 for a=0 to 10
   print a,": ",round(m(a)/n,5)
 next a

arrayfill m(),0
print "MAX:"

 for a=1 to n
   m=0
   For b=1 to 10
     m=max(m,round(random(100)/10))
   next b
   m(m)=m(m)+1
 next a

 for a=0 to 10
   print a,": ",round(m(a)/n,5)
 next a
print "CPU time: ",ctimer-t;" sec."
quit
