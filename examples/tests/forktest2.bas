' test of fork command
' X11-Basic Version 1.14 (c) Markus Hoffmann
'

PIPE #1,#2

a=fork()
IF a=0    ! Child instance
  GPRINT "Hi, I am Child !",b
  DO
    VSYNC
    LINEINPUT #1,t$
    GPRINT t$
  LOOP
  ' This instance never ends ...
ELSE if a=-1
  PRINT "ERROR, fork() failed !"
  QUIT
ELSE      ! parent instance
  DO
    DUMP
    ALERT 1,"Hi, I am Parent. Child PID="+STR$(a),1," OK | Kill Child ! ",b
    DUMP
    PRINT #2,system$("date")
    FLUSH #2
    IF b=2
      SYSTEM "kill "+STR$(a)
      ALERT 1,"Child PID="+STR$(a)+" killed !",1," OK ",b
      QUIT
    ENDIF
  LOOP
ENDIF
QUIT
