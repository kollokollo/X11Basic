 ' Is there something wrong with the random generator ?
 
 
 dim m(10)
 n=10000
 for a=1 to n
   m=0
   For b=1 to 10
     m=min(m,round(random(100)/10))
   next b
   m(m)=m(m)+1
 next a
 
print "MIN:"

 for a=0 to 10
   print a,": ",int(m(a)/n*1000)/10
 next a

arrayfill m(),0
 n=10000
 for a=1 to n
   m=0
   For b=1 to 10
     m=max(m,round(random(100)/10))
   next b
   m(m)=m(m)+1
 next a
print "MAX:"

 for a=0 to 10
   print a,": ",int(m(a)/n*1000)/10
 next a
