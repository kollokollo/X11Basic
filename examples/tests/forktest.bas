' test of fork command 
' X11-Basic Version 1.08 (c) Markus Hoffmann
' 
a=fork()
if a=0    ! Child instance
  alert 1,"Hi, I am Child !",1," go on ",b
  do
    line random(640),random(400),random(640),random(400)
    vsync
  loop
  ' This instance never ends ...
else if a=-1  
  print "ERROR, fork() failed !"
  quit
else      ! parent instance
  dump
  alert 1,"Hi, I am Parent. Child PID="+str$(a),1," OK | Kill Child ! ",b
  dump
  if b=2
    system "kill "+str$(a)
    alert 1,"Child PID="+str$(a)+" killed !",1," OK ",b
  endif
endif
quit
