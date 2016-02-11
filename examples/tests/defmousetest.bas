pbox 320,200,640,400
i=0
do
color 1
pbox 320,200,640,400
color 0
defmouse i
text 340,300,str$(i)
vsync
mouseevent x,y,k
inc i
loop
