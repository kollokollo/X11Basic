' Test the framebuffer version of X11-Basic
' when no real framebuffer (depth 16 bit) is available.
'
' (c) Markus Hoffmann 2020
'
' We use this to test the graphics functions
' This can also be used to produce images without 
' having X.org installed (e.g. in cgi-scripts).
'
framebufferfile$="/tmp/screen0"
OPEN "O",#1,framebufferfile$
PRINT #1,STRING$(640*400*2,CHR$(0));
CLOSE #1

TEXT 100,100,"Hallo"
LINE 200,200,400,300

DEFLINE ,5

LINE 300,300,300,310
LINE 310,300,320,300

LINE 350,350,400,400

SHOWPAGE
SGET a$
t$=PNGENCODE$(a$)
BSAVE "framebuffer.png",VARPTR(t$),LEN(t$)
KILL framebufferfile$
QUIT
