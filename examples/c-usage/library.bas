' This is an example, how to produce .o linkabel object files and
' shared object files (.so) under linux with X11-Basic:
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



procedure pa
  print "This is procedure a"
  alert 1,"Hello, this is a message|from your X11-Basic program library.bas",1," Wow! ",dummy
return
procedure b
  print "This is procedure b"
return
procedure c(x,y)
  print "This is procedure c with parameters: x=";x,"y=";y
return
function d(x,y)
  print "This is function d with parameters: x=";x,"y=";y
  return x+y
endfunction
function greeting$(n%)
  print "This is function greeting with parameter: n%=";n%
  return date$+" "+time$+" Hello!!!! "+str$(n%)
endfunction
