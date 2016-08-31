' Calculates a determinant

DIM n%(3,3)
n%(0,0)=0
n%(0,1)=1
n%(0,2)=2
n%(1,0)=3
n%(1,1)=2
n%(1,2)=1
n%(2,0)=1
n%(2,1)=1
n%(2,2)=0

PRINT det(n%())
PRINT n%(0,0)

DIM b(3)
b(0)=3
PRINT det(b())

DIM a(4,4)
a(0,0)=3
a(0,1)=7
a(0,2)=3
a(0,3)=0
a(1,0)=0
a(1,1)=2
a(1,2)=-1
a(1,3)=1
a(2,0)=5
a(2,1)=4
a(2,2)=3
a(2,3)=2
a(3,0)=6
a(3,1)=6
a(3,2)=4
a(3,3)=-1

c()=inv(a())
@matprint(a())
PRINT "Determinant: ";det(a())
PRINT "inverse:"
@matprint(c())
PRINT "Determinant: ";det(c());"--> 1/";1/det(c())

d()=a()*c()
PRINT "product:"
@matprint(d())

QUIT
PROCEDURE matprint(a())
  LOCAL i,j
  FOR i=0 TO 3
    FOR j=0 TO 3
      PRINT a(i,j) using "##.###",
    NEXT j
    PRINT
  NEXT i
RETURN
