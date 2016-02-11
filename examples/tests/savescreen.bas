' savescreen Test for X11-basic (c) Markus hoffmann 2008-01-19
' 
' 
' In the framebuffer-version (TomTom etc) the file format is
' .bmp 24 Bit
' 
FOR i=1 TO 64
  FOR j=1 TO 40
    CIRCLE i*10,j*10,3
  NEXT j
NEXT i
CIRCLE 100,100,30
GET 0,0,100,100,a$
BSAVE "small.bmp",VARPTR(a$),LEN(a$)
PUT 200,200,a$
SGET b$
BSAVE "sget.bmp",VARPTR(b$),LEN(b$)

CLS
PRINT "now reput the screen"
PAUSE 1
SPUT b$
PRINT "now save window"

SAVEWINDOW "window.bmp"

f$="/mnt/sdcard/antitheft.bmp"
IF EXIST(f$)
  PRINT "now load an image and display it"
  OPEN "I",#1,f$
  t$=INPUT$(#1,LOF(#1))
  PUT 0,0,t$
  CLOSE
ENDIF
SAVESCREEN "fullscreen.bmp"
QUIT
