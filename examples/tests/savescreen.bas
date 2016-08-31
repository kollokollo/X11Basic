' savescreen Test for X11-basic (c) Markus hoffmann 2008-01-19
'
' In the framebuffer-version (TomTom, Android, etc.) the file format is
' .bmp 24 Bit, under linux/UNIX it is XWD
'
' demonstrates the use of SGET, SPUT, GET, PUT, SAVEWINDOW, SAVESCREEN
'
' Following files are produced:
'  small.bmp      -- a 100x100 area of the graphics window
'  sget.bmp       -- the whole window, taken with SGET
'  window.bmp     -- the whole window as a result of SAVEWINDOW
'  fullscreen.bmp -- the whole screen as a result of SAVESCREEN
'
CLEARW           ! clear the screen, otherwise sometimes there is garbage left
FOR i=1 TO 64
  FOR j=1 TO 40
    COLOR COLOR_RGB(i/64,j/40,SQRT(1-(i/64)^2-(j/40)^2))
    CIRCLE i*10,j*10,3    ! draw something
  NEXT j
NEXT i
CIRCLE 100,100,30
SHOWPAGE
GET 0,0,100,100,a$
BSAVE "small.bmp",VARPTR(a$),LEN(a$)
PUT 200,200,a$
SHOWPAGE
PAUSE 3
SGET b$
BSAVE "sget.bmp",VARPTR(b$),LEN(b$)

CLEARW
SHOWPAGE
PRINT "now reput the screen"
PAUSE 1
SPUT b$
SHOWPAGE
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
PAUSE 5
QUIT
