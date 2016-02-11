cls
print
f$="100*sin(x/100)+200"
print "Funktion eingeben, die geplottet wird: z.B. f(x)=";f$
input "f(x)=",f$
clearw
color get_color(65535,32000,0)
for x=0 to 300
  a$="y="+f$
  &a$
  plot x,y
next x
vsync
