echo off
cls
print
input "Funktion eingeben, die geplottet wird: f(x)=",f$
print "f(x)=";f$
for x=0 to 300
  a$="y="+f$
  &a$
  plot x,y
next x
