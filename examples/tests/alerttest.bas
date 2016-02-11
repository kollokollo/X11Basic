' Displays original ATARI ST TOS messages  2003-01-01
'
ALERT 1,"This program displays original ATARI ST TOS messages.",1,"OK|QUIT",balert
IF balert=2
  QUIT
ENDIF
FILESELECT "Alert-Datei auswaehlen:","./alerts.*","",f$
IF LEN(f$)
  IF EXIST(f$)
    OPEN "I",#1,f$
    WHILE NOT EOF(#1)
      LINEINPUT #1,t$
      PRINT "Button: ",FORM_ALERT(1,t$)
    WEND
    CLOSE #1
  ENDIF
ENDIF
QUIT
