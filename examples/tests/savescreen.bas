' savescreen Test for X11-basic (c) Markus hoffmann 2008
' 
' 
' In the framebuffer-version (TomTom etc) the file format is
' .bmp 24 Bit
' 
for i=1 to 64
for j=1 to 40
  circle i*10,j*10,3
next j
next i
circle 100,100,30
get 0,0,100,100,a$
bsave "small.bmp",varptr(a$),len(a$)
put 200,200,a$
sget b$
bsave "sget.bmp",varptr(b$),len(b$)

cls
print "now reput the screen"
pause 1
sput b$
print "now save window"

savewindow "window.bmp"

f$="/mnt/sdcard/antitheft.bmp"
if exist(f$)
  print "now load an image and display it"
  open "I",#1,f$
  t$=input$(#1,lof(#1))
  put 0,0,t$
  close
endif
savescreen "fullscreen.bmp"

quit
