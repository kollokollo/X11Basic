' Tests a new function: WATCH and fileevent (still undocumented). WOrk only
' with X11-Basic Version 1.16 (and recent kernels).
'
' WATCH <filename> (or directory name)
' watches the file now for events
'
' FILEEVENT$ returns a blank separated list of events, each consist of 
' an indicator (3 charackters) and a filename.
' The charackters can be
' 1st character: d if file is a directory, - if regular file
' 2nd character: O=Open, C=close, D=delete, M=move
' 3rd character: r=read, w=write, a=attribute channge
'
'

WATCH "/tmp"
MKDIR "/tmp/ttt"
RMDIR "/tmp/ttt"
SYSTEM "touch /tmp/ttt"
SYSTEM "rm -f /tmp/ttt"
print "now open a file browser (or a terminal window), go"
print "to the /tmp directory and do something..."
do
  a$=fileevent$
  if len(a$)
    print a$
  ELSE 
    PAUSE 0.3
  ENDIF
LOOP

QUIT
