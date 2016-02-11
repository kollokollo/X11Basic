' Test of the matrix/linear equation solver function
' works only, if X11-Basic was compiled to use LAPACK.
'
' The matrix a() need not be square. The algorithm finds a solution
' in nearly any case. Even if there is no (exact) solution to the 
' equation system. 
'
' The returned vector is the one, which minimizes the error |a*x-b|
'

r=3
c=5

DIM a(r,c),b(r)
ARRAYFILL a(),0
a(0,0)=1
a(0,2)=1
a(1,1)=10
a(2,2)=100
b(0)=4
b(1)=2
b(2)=300
PRINT "solve:"
FOR i=0 TO r-1
    PRINT i;": (";
    FOR j=0 TO c-1
      PRINT a(i,j);
      IF j<c-1 
        PRINT ", ";
      ENDIF
    NEXT j
    PRINT ")(x)=(";b(i);")"
NEXT i
PRINT
er()=SOLVE(a(),b())
PRINT "solution:"
FOR i=0 TO c-1
  PRINT "(x";i;")=(";er(i);")"
NEXT i
QUIT
