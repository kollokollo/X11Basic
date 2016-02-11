' "sieve.bas" , a prime number sieve benchmark


@sieve(50000)



quit



procedure sieve(s)
local i
tc=ctimer
dim f(s+1)
arrayfill f(),1
clr c
for i=2 to s
  if f(i)
    if 2*i<s
      for k=2*i to s step i 
        clr f(k)
      next k
      flush
    endif
    print i,
    inc c
   endif
next i
 print
print c;" primes up to ";s;" found. ";
tc=ctimer-tc
print "CPU time: ";tc;" s"
return
