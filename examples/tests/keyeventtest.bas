sizew ,200,300
rpx=8
rpy=16
gpx=24
gpy=8
bpx=24
bpy=24
vsync
gelb=get_color(65535,65535,0)
schwarz=get_color(0,0,0)
defline ,15,2
deftext 0,1,2,0
color gelb
text 50,100,"Press any key ..."
vsync
do
  keyevent a,b,c$
  color schwarz
  pbox 0,0,200,300
  color gelb
  ltext 0,0,c$
  text 10,280,"Keycode: "+str$(a)+"/"+str$(b)
  vsync
  print c$
loop
