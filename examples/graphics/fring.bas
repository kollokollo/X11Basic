
CLS
PRINT "Qatom... Fuer das Atomphysikseminar Jan. 1995 Prof. Meschede"
PRINT "von Markus Hoffmann"
PRINT
PRINT "Thema: Klassische Elektronenbahnen in Rydbergatomen"
INPUT "Anzahl der Laeufer ? (n>3) ",n
PRINT n
DIM phi(n),v(n)
FOR i=0 TO n-1
  v(i)=i+21
  phi(i)=0
NEXT i
schwarz=GET_COLOR(0,0,0)
weiss=GET_COLOR(65535,0,0)
CIRCLE 320,200,200
BOUNDARY 0
radius=0
dt=0.004
DO
  COLOR schwarz
  '  pbox 200,80,440,320
  COLOR weiss
  FOR i=0 TO n-1
    ADD phi(i),v(i)*dt
    PLOT 320+COS(phi(i))*radius,200+SIN(phi(i))*radius
  NEXT i
  EXIT if mousek>1
  SHOWPAGE
  PAUSE 0.02
  INC radius
LOOP
QUIT
