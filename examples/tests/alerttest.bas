' Displays original ATARI ST TOS messages
'
alert 1,"This program displays original ATARI ST TOS messages.",1,"OK|QUIT",balert
if balert=2
  quit
endif
fileselect "Alert-Datei auswaehlen:","./alerts.*","",f$
if len(f$)
  if exist(f$)
    open "I",#1,f$
    while not eof(#1)
      lineinput #1,t$
      print "Button: ",form_alert(1,t$)
    wend
    close #1
  endif
endif
quit
