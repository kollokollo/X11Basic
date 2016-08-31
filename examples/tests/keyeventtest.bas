SIZEW ,200,300
rpx=8
rpy=16
gpx=24
gpy=8
bpx=24
bpy=24
VSYNC
gelb=GET_COLOR(65535,65535,0)
schwarz=GET_COLOR(0,0,0)
DEFLINE ,15,2
DEFTEXT 0,1,2,0
COLOR gelb
TEXT 50,100,"Press any key ..."
VSYNC
DO
  KEYEVENT a,b,c$
  COLOR schwarz
  PBOX 0,0,200,300
  COLOR gelb
  LTEXT 0,0,c$
  TEXT 10,280,"Keycode: "+STR$(a)+"/"+STR$(b)
  VSYNC
  PRINT c$
LOOP
