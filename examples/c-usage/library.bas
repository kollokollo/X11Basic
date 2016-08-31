' library.bas -- demonstration for X11-Basic (c) Markus Hoffmann
'
'
' This is an example how to produce .o linkabel object files and
' shared object files (.so) under linux (UNIX or MAC-OSX) with X11-Basic:
'
' Do a
' xbc -l -c library.bas
' or
' xbc -l -c -virtualm library.bas (for faster code)
' this will produce library.o
' gcc -shared library.o -o library.so
' will produce the shared object file.
'
' You can link multiple .o files together with normal (C) .o files.

PROCEDURE pa
  PRINT "This is procedure a"
  ALERT 1,"Hello, this is a message|from your X11-Basic program library.bas",1," Wow! ",dummy
RETURN
PROCEDURE b
  PRINT "This is procedure b"
RETURN
PROCEDURE c(x,y)
  PRINT "This is procedure c with parameters: x=";x,"y=";y
RETURN
FUNCTION d(x,y)
  PRINT "This is function d with parameters: x=";x,"y=";y
  RETURN x+y
ENDFUNCTION
FUNCTION greeting$(n%)
  PRINT "This is function greeting with parameter: n%=";n%
  RETURN DATE$+" "+TIME$+" Hello!!!! "+STR$(n%)
ENDFUNCTION
