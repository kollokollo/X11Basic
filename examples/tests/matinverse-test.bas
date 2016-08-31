' Test of the matrix/linear equation solver function
' works only, if X11-Basic was compiled to use LAPACK.
'
' The matrix a() need be square.
'
'

r=4
c=4

DIM a(r,c)
ARRAYFILL a(),0
a(0,0)=1
a(0,2)=1
a(1,1)=10
a(2,2)=100
a(3,3)=1
a(2,3)=1
PRINT "solve:"
FOR i=0 TO r-1
  PRINT i;": (";
  FOR j=0 TO c-1
    PRINT a(i,j);
    IF j<c-1
      PRINT ", ";
    ENDIF
  NEXT j
  PRINT ")"
NEXT i
PRINT
er()=INV(a())
PRINT "solution:"
FOR i=0 TO r-1
  PRINT i;": (";
  FOR j=0 TO c-1
    PRINT er(i,j);
    IF j<c-1
      PRINT ", ";
    ENDIF
  NEXT j
  PRINT ")"
NEXT i
QUIT
