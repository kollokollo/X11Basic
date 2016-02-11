a()=[0,1,2,3;4,5,6;7,8,9;10,11,12,13,14,15, \
1,2,3,4,5,6, 1,2,34,7]
b$()=["Hallo","Das","ist","ein","Test";"1Hallo","1Das";"Jajaj"]

for i=0 to 2
for j=0 to 4
print b$(i,j);" ";
next j
print
next i

for i=0 to 3
for j=0 to 10
print a(i,j);" ";
next j
print
next i

dump
