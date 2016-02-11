' Simple test for the virtual Machine

print "Los gehts..."
' ~inp(-2)
color get_color(65535,65535,0)
text 30,360,"Test of the X11-Basic virtual machine"
do
  i=(i+1) mod 640
  color get_color(random(65535),random(65535),random(65535))
  pbox random(300),random(0),random(640),random(300)
  showpage
  if i=0
    print timer-t;" sek."
    t=timer
  endif  
  plot i,50*sin(i/100*2*pi)+350
'  pause 0.1
loop

quit
