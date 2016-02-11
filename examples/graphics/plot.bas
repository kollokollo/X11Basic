echo off
cls
print
input "Funktion eingeben, die geplottet wird: f(x)=",f$
print "f(x)=";f$
color get_color(65535,32000,0)
for x=0 to 300
  a$="y="+f$
  &a$
  plot x,y
next x
