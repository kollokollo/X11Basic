
' Program to test the ARRAY and Matrix features of X11Basic
' There are still some bugs.      (c) Markus Hoffmann 2005

a()=[1,2,3,4]
b()=[2,3,4,5]

e()=smul(trans(a()),2)
c()=a()*e()
d()=a()*trans(b())
f()=e()*a()
g()=1(10)+smul(1(10),9)^2
DUMP

FOR i=0 TO 3
  PRINT e(i,0)
NEXT i
PRINT
MEMDUMP varptr(a(0))-8,5*8
PRINT
MEMDUMP varptr(e(0))-2*8,6*8
FOR i=0 TO 3
  FOR j=0 TO 3
    PRINT c(i,j);" ";
  NEXT j
  PRINT
NEXT i
FOR i=0 TO sqrt(dim?(g()))-1
  FOR j=0 TO sqrt(dim?(g()))-1
    PRINT g(i,j);" ";
  NEXT j
  PRINT
NEXT i

PRINT
PRINT f(0)
' cc()=c()*d()
QUIT

' Hier gibt es noch einen Bug !

a$()=["1","2","3","9";"4","5","6","9";"7","8","9","9"]
DUMP
b$()=trans(a$())
DUMP
b$()=a$()
FOR i=0 TO 3
  FOR j=0 TO 2
    PRINT b$(i,j);" ";
  NEXT j
  PRINT
NEXT i
FOR i=0 TO 2
  FOR j=0 TO 3
    PRINT a$(i,j);" ";
  NEXT j
  PRINT
NEXT i
DUMP
QUIT
