' Test of the EXEC command. 
' 
print "Jetzt geht es los"
pause 3
a=EXEC("test","1012"+chr$(10)+"-eq"+chr$(10)+"101","")
print "Return value is: ";a
pause 2
if ANDROID?
  print "starte intent..."
  EXEC "android.intent.action.VIEW","http://x11-basic.sourceforge.net/"
 ' print "wieder zurueck vom intent..."
 ' pause 5
 ' print "starte intent..."
 ' EXEC "android.intent.action.SEND","file://test.txt"
  print "wieder zurueck vom intent..."
else
  a=EXEC("test","1012"+chr$(10)+"-eq"+chr$(10)+"101")
  print "Return value is: ";a

  print "I am calling env now..."
  EXEC "env","-u"+chr$(10)+"A","A=2"+chr$(10)+"B=1"
  print "never returns to caller"
endif
end
