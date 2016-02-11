sizew ,200,300
rpx=8
rpy=16
gpx=24
gpy=8
bpx=24
bpy=24
vsync
defline ,15,2
deftext 0,1,2,0
do
keyevent a,b,c$
color 0
pbox 0,0,32*8,32*8
color 1
ltext 0,0,c$
vsync
print c$

loop
