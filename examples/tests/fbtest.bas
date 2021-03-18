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


SETFONT "/usr/share/fonts/truetype/open-sans/OpenSans-Light.ttf"
DEFTEXT ,120,,20
GRAPHMODE 2

TEXT 60,250,"Hello..."
LINE 200,200,400,300
DEFTEXT ,16,,0
SETFONT "/usr/share/fonts/truetype/open-sans/OpenSans-Bold.ttf"

TEXT 10,300,"This is a framebuffer test."

COLOR COLOR_RGB(0,1,1),COLOR_RGB(0,0.4,0.4)

gprint "All right!"

DEFLINE ,5

LINE 300,300,300,310
LINE 310,300,320,300

LINE 350,350,400,400

DEFTEXT ,200,,-10

TEXT 500,300,"!"


SHOWPAGE
SGET a$
t$=PNGENCODE$(a$)
BSAVE "framebuffer.png",VARPTR(t$),LEN(t$)
KILL framebufferfile$
QUIT
