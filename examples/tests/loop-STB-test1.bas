' This benchmark was taken from stbasic09 (bench.LIS)
' (to compare the performance)
'
' X11-basic wins (of course...)
'
PRINT "Kilo-OPs Benchmarks ";DATE$;" - ";TIME$
DIM m(5)
loop%=10000

PRINT "Benchmark 1 : FOR-loop(";loop%;") "
t = TIMER
tdif = loop%
FOR i% = 1 TO loop%
  k% = i%
NEXT i%
timedif = TIMER-t
PRINT "Time= ";timedif;"  Kilo-OPs= ";INT(tdif/timedif)

PRINT "Benchmark 2 : REPEAT-loop(";loop%;") "
t = TIMER
k% = 0
REPEAT
  k% = k%+1
UNTIL k% > loop%-1
timedif = TIMER-t
PRINT "Time= ";timedif;"  Kilo-OPs= ";INT(tdif/timedif)

PRINT "Benchmark 2b : REPEAT-loop(";loop%;") +INC"
t = TIMER
k% = 0
REPEAT
  INC k%
UNTIL k% > loop%-1
timedif = TIMER-t
PRINT "Time= ";timedif;"  Kilo-OPs= ";INT(tdif/timedif)

PRINT "Benchmark 3 : DO/LOOP-loop(";loop%;") k/k*k+k-k"
t = TIMER
k = 0
DO 
  INC k
  a = k/k*k+k-k
EXIT IF k >= loop%
LOOP
timedif = TIMER-t
PRINT "Time= ";timedif;"  Kilo-OPs= ";INT(tdif/timedif)

PRINT "Benchmark 4 : WHILE-loop(";loop%;") k/2*3+4-5"
t = TIMER
k = 0
WHILE k < loop%
  k = k+1
  a = k/2*3+4-5
WEND
timedif = TIMER-t
PRINT "Time= ";timedif;"  Kilo-OPs= ";INT(tdif/timedif)

PRINT "Benchmark 5 : REPEAT-loop(";loop%;") k/2, GOSUB "
t = TIMER
k = 0
REPEAT
  k = k+1
  a = k/2*3+4-5
  GOSUB dummy
UNTIL k >= loop%
timedif = TIMER-t
PRINT "Time= ";timedif;"  Kilo-OPs= ";INT(tdif/timedif)

PRINT "Benchmark 6 : REPEAT-loop(";loop%;") k/2, GOSUB, FOR-loop(5)"
t = TIMER
k = 0
REPEAT
  k = k+1
  a = k/2*3+4-5
  GOSUB dummy
  FOR l = 1 TO 5
  NEXT l
UNTIL k >= loop%
timedif = TIMER-t
PRINT "Time= ";timedif;"  Kilo-OPs= ";INT(tdif/timedif)

PRINT "Benchmark 7 : REPEAT-loop(";loop%;") k/2, GOSUB, FOR-loop(5) m(l)"
t = TIMER
k = 0
REPEAT
  k = k+1
  a = k/2*3+4-5
  GOSUB dummy
  FOR l = 1 TO 5
    m(l) = a
  NEXT l
UNTIL k >= loop%
timedif = TIMER-t
PRINT "Time= ";timedif;"  Kilo-OPs= ";INT(tdif/timedif)

PRINT "Benchmark 8 : REPEAT-loop(";loop%;") k^2, LOG(k), SIN(k)"
t = TIMER
k = 0
REPEAT
  k = k+1
  a = k^2
  b = LOG(k)
  c = SIN(k)
UNTIL k >= loop%
timedif = TIMER-t
PRINT "Time= ";timedif;"  Kilo-OPs= ";INT(tdif/timedif)
quit
PROCEDURE dummy
  b = a
RETURN
