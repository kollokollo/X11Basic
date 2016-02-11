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
system "mkdir /tmp/ttt"
system "rmdir /tmp/ttt"
system "touch /tmp/ttt"
system "rm -f /tmp/ttt"

do
  a$=fileevent$
  if len(a$)
    print a$
  else 
    pause 0.3
  endif
loop

quit
