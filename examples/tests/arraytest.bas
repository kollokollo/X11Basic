' test of the Array capabilities of X11-Basic
'
'
PRINT "Assign a constant to a():"
a()=[0,1,2,3;4,5,6;7,8,9;10,11,12,13,14,15, \
1,2,3,4,5,6, 1,2,34,7]
PRINT "Assign a string constant to b$():"
b$()=["Hallo","Das","ist","ein","Test";"1Hallo","1Das";"Jajaj"]

dump
print "b$()="
for i=0 to 2
  for j=0 to 4
    print enclose$(b$(i,j));" ";
  next j
  print
next i

print "a()="
for i=0 to 3
  for j=0 to 15
    print a(i,j);" ";
  next j
  print
next i

print
' Redimension without altering the content
dim a(6,6,100)
print "a()="
a(4,5,0)=-1
for i=0 to 5
  for j=0 to 5
    print a(i,j,0);" ";
  next j
  print
next i
dim a(6)
print
print "a()="
  for j=0 to 5
    print a(j);" ";
  next j
print
quit
