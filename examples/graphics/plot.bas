' Demontrates the use of the & operator
' so the internal parser can be used.
' (c) Markus Hoffmann V.1.08
CLS
PRINT
f$="100*sin(x/100)+200"
PRINT "Enter a function, which should be plotted: e.g. f(x)=";f$
INPUT "f(x)=",f$
CLEARW
a$="y="+f$
GPRINT a$
DUMP
COLOR COLOR_RGB(1,0.5,0)
FOR x=0 TO 300
  &a$
  PLOT x,y
NEXT x
SHOWPAGE
ALERT 0,"done.",1,"OK",a
QUIT
