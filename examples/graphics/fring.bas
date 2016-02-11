
cls
print "Qatom... Fuer das Atomphysikseminar Jan. 1995 Prof. Meschede"
print "von Markus Hoffmann"
print
print "Thema: Klassische Elektronenbahnen in Rydbergatomen" 
input "Anzahl der Laeufer ? (n>3) ",n
print n
DIM phi(n),v(n)
FOR i=0 TO n-1
  v(i)=i+21
  phi(i)=0
NEXT i
schwarz=get_color(0,0,0)
weiss=get_color(65535,0,0)
CIRCLE 320,200,200
BOUNDARY 0
radius=0
dt=0.004
DO
  color schwarz
'  pbox 200,80,440,320
  color weiss
  FOR i=0 TO n-1
    ADD phi(i),v(i)*dt
    PLOT 320+COS(phi(i))*radius,200+SIN(phi(i))*radius
  NEXT i
  exit if mousek>1
  SHOWPAGE
  pause 0.02
  inc radius
LOOP
quit
