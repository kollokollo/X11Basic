rem "sieve.bas" , a prime number sieve benchmark
t=timer
tc=ctimer
dim f(30000)
arrayfill f(),1

s=29999
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
print c;" primes found in ";
t=timer-t
tc=ctimer-tc
print t;" seconds (";tc;" s CPU )"
quit
