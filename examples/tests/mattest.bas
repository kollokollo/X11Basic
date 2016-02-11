
' Program to test the ARRAY and Matrix features of X11Basic
' There are still some bugs.      (c) Markus Hoffmann 2005


a()=[1,2,3,4]
b()=[2,3,4,5]

e()=smul(trans(a()),2)
c()=a()*e()
d()=a()*trans(b())
f()=e()*a()
g()=1(10)+smul(1(10),9)^2
dump

for i=0 to 3
  print e(i,0)
next i
print
memdump varptr(a(0))-8,5*8
print
memdump varptr(e(0))-2*8,6*8
for i=0 to 3
  for j=0 to 3
    print c(i,j);" ";
  next j
  print
next i
for i=0 to sqrt(dim?(g()))-1
  for j=0 to sqrt(dim?(g()))-1
    print g(i,j);" ";
  next j
  print
next i

print
print f(0)
' cc()=c()*d()
quit











' Hier gibt es noch einen Bug !


a$()=["1","2","3","9";"4","5","6","9";"7","8","9","9"]
dump
b$()=trans(a$())
dump
b$()=a$()
for i=0 to 3
  for j=0 to 2
    print b$(i,j);" ";
  next j
  print
next i
for i=0 to 2
  for j=0 to 3
    print a$(i,j);" ";
  next j
  print
next i
dump
quit
