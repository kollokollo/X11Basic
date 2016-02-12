' aus TESTBILD.GFA 1990-11-20
color color_rgb(0,1,1)
FOR i=0 TO 12
  LINE 0,i*32,639,i*32
NEXT i
color color_rgb(1,1,0)
FOR j=0 TO 20-1
  LINE j*32,0,j*32,400
NEXT j
color color_rgb(1,1,1)
DEFTEXT ,2,2,13
FOR i=0 TO 239
  TEXT (i DIV 12)*32+8,(i MOD 12)*32+17,STR$(i)
NEXT i

open "I",#1,"/video/NUGATROX/TomTom/bck/TomTomXL/INTERNAL/photos/AntitheftWide.bmp"

p$=input$(#1,lof(#1))
print "put external bmp:"
put 32,32,p$
close #1


SHOWPAGE
pause 3
print "get testbild"
get 0,0,300,300,t$
print "testbild: len=";len(t$)
BSAVE "test.bmp",varptr(t$),len(t$)
print "put testbild"
put 300,0,t$
' BSAVE "TSTICN.DOO",XBIOS(3),32000
print "make png"
p$=PNGENCODE$(T$)
bsave "test.png",varptr(p$),len(p$)

SHOWPAGE
pause 1
open "I",#1,"/home/hoffmann/workspace/xearth/bin/res/drawable/xearth.png"

p$=input$(#1,lof(#1))
close #1
print "load png"
t$=pngdecode$(p$)
bsave "test2.bmp",varptr(t$),len(t$)
print "len=";len(t$)
put 0,0,t$

showpage
pause 3
print "shift"
put 10,10,t$
showpage
pause 3
print "ende"
xrun
QUIT
