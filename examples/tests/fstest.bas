' Demonstration of the use of FSFIRST() and FSNEXT()
' To list all files in a directory.
'
' X11Basic V.1.22 
'
if WIN32?
  a$=FSFIRST$("C:\","*.dat")
  PRINT "looks for *.dat files in C:\"
ELSE
  a$=FSFIRST$("/tmp","*.dat")
  PRINT "looks for *.dat files in /tmp/"
ENDIF
WHILE LEN(a$)
  PRINT a$
  name$=WORD$(a$,2)    ! This is the filename only
  a$=FSNEXT$()
WEND
END
