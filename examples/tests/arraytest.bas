' test of the Array capabilities of X11-Basic
'
'
PRINT "Assign a constant to a():"
a()=[0,1,2,3;4,5,6;7,8,9;10,11,12,13,14,15, \
1,2,3,4,5,6, 1,2,34,7]
PRINT "Assign a string constant to b$():"
b$()=["Hallo","Das","ist","ein","Test";"1Hallo","1Das";"Jajaj"]

DUMP
PRINT "b$()="
FOR i=0 TO 2
  FOR j=0 TO 4
    PRINT enclose$(b$(i,j));" ";
  NEXT j
  PRINT
NEXT i

PRINT "a()="
FOR i=0 TO 3
  FOR j=0 TO 15
    PRINT a(i,j);" ";
  NEXT j
  PRINT
NEXT i

PRINT
' Redimension without altering the content
DIM a(6,6,100)
PRINT "a()="
a(4,5,0)=-1
FOR i=0 TO 5
  FOR j=0 TO 5
    PRINT a(i,j,0);" ";
  NEXT j
  PRINT
NEXT i
DIM a(6)
PRINT
PRINT "a()="
FOR j=0 TO 5
  PRINT a(j);" ";
NEXT j
PRINT
QUIT
