' test of fork command
' X11-Basic Version 1.08 (c) Markus Hoffmann
'
a=fork()
IF a=0    ! Child instance
  ALERT 1,"Hi, I am Child !",1," go on ",b
  DO
    LINE random(640),RANDOM(400),RANDOM(640),RANDOM(400)
    VSYNC
  LOOP
  ' This instance never ends ...
ELSE if a=-1
  PRINT "ERROR, fork() failed !"
  QUIT
ELSE      ! parent instance
  DUMP
  ALERT 1,"Hi, I am Parent. Child PID="+STR$(a),1," OK | Kill Child ! ",b
  DUMP
  IF b=2
    SYSTEM "kill "+STR$(a)
    ALERT 1,"Child PID="+STR$(a)+" killed !",1," OK ",b
  ENDIF
ENDIF
QUIT
