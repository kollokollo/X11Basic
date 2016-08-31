' Farbflaeche (c) Markus Hoffmann with X11-Basic V.1.00

SIZEW ,32*16,32*16
DIM c(10,10)

rpx=8
rpy=16
gpx=24
gpy=8
bpx=24
bpy=24
gelb=COLOR_RGB(1,1,0)
COLOR gelb
TEXT 100,100,"COLORTEST X11-BASIC"
t=TIMER
DO
  x=RANDOM(64)
  y=RANDOM(64)
  FOR x=0 TO 63
    FOR y=0 TO 63
      r=MIN(SQR((rpx-x)^2+(rpy-y)^2),64)
      g=MIN(SQR((gpx-x)^2+(gpy-y)^2),64)
      b=MIN(SQR((bpx-x)^2+(bpy-y)^2),64)
      c=COLOR_RGB(r/64,g/64,b/64)
      IF c>=0
        COLOR c
      ENDIF
      PBOX x*8,y*8,x*8+8,y*8+8
      ' pcircle x*16,y*16,5
      ' color 1
      ' text x*16-5,y*16+3,str$(c,3,3)
      ' circle x*16,y*16,5
      SHOWPAGE
    NEXT y
  NEXT x
  EXIT IF TRUE
LOOP
PRINT TIMER-t;" Sek."
QUIT
