echo off
cls
print "X 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0"
for u=0 to 3
for j=0 to 7
for i=0 to 7
  print at(j+2,2*i+2+16*u);chr$(27)+"["+str$(u)+";"+str$(30+i)+";"+str$(40+j)+"m *"; 

next i
next j
next u
print
quit

