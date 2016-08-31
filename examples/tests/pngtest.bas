' aus TESTBILD.GFA 1990-11-20
COLOR color_rgb(0,1,1)
FOR i=0 TO 12
  LINE 0,i*32,639,i*32
NEXT i
COLOR color_rgb(1,1,0)
FOR j=0 TO 20-1
  LINE j*32,0,j*32,400
NEXT j
COLOR color_rgb(1,1,1)
DEFTEXT ,2,2,13
FOR i=0 TO 239
  TEXT (i DIV 12)*32+8,(i MOD 12)*32+17,STR$(i)
NEXT i

OPEN "I",#1,"/video/NUGATROX/TomTom/bck/TomTomXL/INTERNAL/photos/AntitheftWide.bmp"

p$=input$(#1,lof(#1))
PRINT "put external bmp:"
PUT 32,32,p$
CLOSE #1

SHOWPAGE
PAUSE 3
PRINT "get testbild"
GET 0,0,300,300,t$
PRINT "testbild: len=";LEN(t$)
BSAVE "test.bmp",VARPTR(t$),LEN(t$)
PRINT "put testbild"
PUT 300,0,t$
' BSAVE "TSTICN.DOO",XBIOS(3),32000
PRINT "make png"
p$=PNGENCODE$(T$)
BSAVE "test.png",VARPTR(p$),LEN(p$)

SHOWPAGE
PAUSE 1
OPEN "I",#1,"/home/hoffmann/workspace/xearth/bin/res/drawable/xearth.png"

p$=input$(#1,lof(#1))
CLOSE #1
PRINT "load png"
t$=pngdecode$(p$)
BSAVE "test2.bmp",VARPTR(t$),LEN(t$)
PRINT "len=";LEN(t$)
PUT 0,0,t$

SHOWPAGE
PAUSE 3
PRINT "shift"
PUT 10,10,t$
SHOWPAGE
PAUSE 3
PRINT "ende"
XRUN
QUIT
