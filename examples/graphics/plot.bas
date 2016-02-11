' Demontrates the use of the & operator
' so the internal parser can be used.
' (c) Markus Hoffmann V.1.08
cls
print
f$="100*sin(x/100)+200"
print "Funktion eingeben, die geplottet wird: z.B. f(x)=";f$
input "f(x)=",f$
clearw
a$="y="+f$
gprint a$
dump
color get_color(65535,32000,0)
for x=0 to 300
  &a$
  plot x,y
next x
vsync
alert 0,"done.",1,"OK",a
quit
