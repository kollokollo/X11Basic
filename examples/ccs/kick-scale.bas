' Programm zum Nachtraeglichen Skalieren von Korrektordatensaetzen
' Gehoert zum Harmcorr-Experten. Falls man die Korrektur nachtraeglich
' applizieren will, ohne die Lage des Strahls in der Extraktionsbeamline zu
' veraendern 

' Markus Hoffmann 1999


echo off
dim hkicks(32),vkicks(32)
fileselect "Bitte Korrektordatenfile auswaehlen...","/sgt/elsa/data/korrektoren/*.kicks","",f$
  if len(f$) 
    if not exist(f$)
      void form_alert(1,"[3][File |"+f$+" |existiert nicht !][ OH ]")
    else
      @read_orbit(f$)
      open "O",#1,"my_kicks.kicks"
      print #1,"Korrektor-Einstellungen (geschrieben von csxbasic)"
      for i=1 to 32
        print #1,"HZ "+str$(i)+" "+str$(hkicks(i-1)*1.2/1.9)+" "+str$(vkicks(i-1)*1.2/1.9)
      next i
     close
    endif
  endif
quit

procedure read_orbit(fname$)
  '
  ' Globale Korrektur einlesen
  '
  local t$,i,a$,b$
  print "Orbit wird eingelesen..."
  if fname$=""
    input "Name des Korrekturdatensatzes = ",fname$
    filename$="/sgt/elsa/data/korrektoren/"+fname$+".kicks"
  else
    filename$=fname$
  endif
  if exist(filename$)
    OPEN "I",#1,filename$
    lineinput #1,t$
    print filename$+" [";
    while not eof(#1)
      lineinput #1,t$
      wort_sep t$," ",TRUE,a$,b$
      if a$="HZ"
	  wort_sep b$," ",TRUE,a$,b$
          i=val(a$)-1
	  wort_sep b$," ",TRUE,a$,b$
	  hkicks(i)=val(a$)
	  vkicks(i)=val(b$)
        print ".";
      endif
    wend
    close #1
    print "]"
  else
    alert 3,"File nicht gefunden !|"+filename$,1," OH ",balert
  endif
return
