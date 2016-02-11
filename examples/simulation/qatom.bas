echo off
cls
print "Qatom... Fuer das Atomphysikseminar Jan. 1995 Prof. Meschede"
print "von Markus Hoffmann"
print
print "Thema: Klassische Elektronenbahnen in Rydbergatomen" 
input "Anzahl der Laeufer ? (n>3) ",n
print n
DIM phi(n),v(n)
FOR i=0 TO n
  v(i)=i+21
  phi(i)=0
NEXT i
CIRCLE 320,200,200
BOUNDARY 0
DO
  color 0
  pbox 200,80,440,320
  color 1
  FOR i=0 TO n-1
    ADD phi(i),v(i)*0.005
    PCIRCLE 320+COS(phi(i))*100,200+SIN(phi(i))*100,5
  NEXT i
  exit if mousek>500
  vsync
LOOP
