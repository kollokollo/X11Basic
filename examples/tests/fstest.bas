' Demonstration of the use of FSFIRST() and FSNEXT()
' To list all files in a directory.
'
' X11Basic V.1.22 
'
if WIN32?
  a$=FSFIRST$("C:\","*.dat")
else
  a$=FSFIRST$("/tmp","*.dat")
endif
WHILE len(a$)
  print a$
  name$=word$(a$,2)
  
  a$=FSNEXT$()
WEND
END
