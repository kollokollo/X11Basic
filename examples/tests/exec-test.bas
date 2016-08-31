' Test of the EXEC command.
'
PRINT "Jetzt geht es los"
PAUSE 3
a=EXEC("test","1012"+CHR$(10)+"-eq"+CHR$(10)+"101","")
PRINT "Return value is: ";a
PAUSE 2
IF ANDROID?
  PRINT "starte intent..."
  EXEC "android.intent.action.VIEW","http://x11-basic.sourceforge.net/"
  ' print "wieder zurueck vom intent..."
  ' pause 5
  ' print "starte intent..."
  ' EXEC "android.intent.action.SEND","file://test.txt"
  PRINT "wieder zurueck vom intent..."
ELSE
  a=EXEC("test","1012"+CHR$(10)+"-eq"+CHR$(10)+"101")
  PRINT "Return value is: ";a

  PRINT "I am calling env now..."
  EXEC "env","-u"+CHR$(10)+"A","A=2"+CHR$(10)+"B=1"
  PRINT "never returns to caller"
ENDIF
END
