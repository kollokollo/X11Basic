' Simple test for the virtual Machine


plot 1,1
get_geometry 1,bx,by,bw,bh
print "Los gehts..."
' ~inp(-2)
box bx,by,bx+bw,by+bh
color get_color(65535,65535,0)
text 30,bh-40,"Test of the X11-Basic virtual machine"
text 30,bh-30,"Hit Ctrl-c to end"
do
  i=(i+1) mod bw
  color get_color(random(65535),random(65535),random(65535))
  pbox random(300),random(30),random(bw),random(bh-40)
  showpage
  if i=0
    print at(4,1);print timer-t;" sek."
    t=timer
  endif  
  plot i,50*sin(i/100*2*pi)+bh-40
  pause 0.01
loop

quit
