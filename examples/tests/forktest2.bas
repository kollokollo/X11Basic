' test of fork command 
' X11-Basic Version 1.14 (c) Markus Hoffmann
' 

pipe #1,#2


a=fork()
if a=0    ! Child instance
  gprint "Hi, I am Child !",b
  do
    vsync
    lineinput #1,t$
    gprint t$
  loop
  ' This instance never ends ...
else if a=-1  
  print "ERROR, fork() failed !"
  quit
else      ! parent instance
  do
  dump
  alert 1,"Hi, I am Parent. Child PID="+str$(a),1," OK | Kill Child ! ",b
  dump
  print #2,system$("date")
  flush #2
  if b=2
    system "kill "+str$(a)
    alert 1,"Child PID="+str$(a)+" killed !",1," OK ",b
    quit
  endif
  loop
endif
quit
