echo off
'
' stromueberwachung.bas
' M. Hoffmann & H. Bongartz
'
' $Header: stromueberwachung.bas,v 1.9 2000/02/18 17:08:35 control Exp $
'
' Warngrenze in Prozent
grenze=30
wartedelay=5
warten=wartedelay
offset=0
'a=1000*CSGET("STE_DIAG_TOROID.TRANSQ_AM")
a=CSGET("SUP_COUNTER.GDH_STROM_AM")
clr count
clr alarm
clr ucount
clr ualarm
clr flag
clr lflag
sleeptime=3
' Soundausgabe an per Default
nosound=0
' Kein Sound bei Highscore per Default
hssound=0
' Wenn 1, pausiere Ausgabe
clr pause
' Wenn 1, fester Bezugswert
clr fix
' Wenn 1, warnen bei Ueberschreiten
clr ueber
dipolold$="DIPOLE: Netzgeraet OK !"
gosub help
do
  if pause=0
	  pause sleeptime
'	  b=1000*CSGET("STE_DIAG_TOROID.TRANSQ_AM")
	  b=CSGET("SUP_COUNTER.GDH_STROM_AM")
	  	  b=b-offset
'	  print @fstr$(a,2)+" pC    "+@fstr$(b,2)+" pC"
	  print @fstr$(a,2)+" pA    "+@fstr$(b,2)+" pA"
	  if b<(1-grenze/100)*a
    	inc count
		if count>warten
		  @dosound("doorbell.au")
		  print "Grenze unterschritten!"
		  clr count
		  inc alarm
    	endif
		if alarm>5
		  @dosound("clock.au")
		  print "Aufwachen! Die Grenze ist schon lange unterschritten!"
		  clr alarm
		endif
	  else if ueber and b>(1+grenze/100)*a
    	inc ucount
		if ucount>warten
    	  @dosound("whistle.au")
		  print "Grenze ueberschritten!"
		  clr ucount
		  inc ualarm
    	endif
		if ualarm>5
		  @dosound("ahhhhh.wav")
		  print "Aufwachen! Die Grenze ist schon lange ueberschritten!"
		  clr ualarm
		endif
	  else
    	clr count
		clr ucount
		clr alarm
		clr ualarm
	  endif
	  if fix=0 and b>a
    	a=b
		if hssound
			@dosound("spacemusic.au")
		endif
		print "Neuer Highscore!"
	  endif
	  dipol$=csget$("ELS_MAGNETE_DIPOL.STATUS_SM")
	  if dipol$<>dipolold$ and flag=0
    	print "DIPOL-Error:"
		flag=1
		@dosound("ST_Exclamation.wav")
	  endif
	  if dipol$=dipolold$
    	flag=0
	  endif
	  linachf=CSGET("INJ_LINAC2_MOD.MOD_BEREIT_DM")
	  if linachf=0 and lflag=0
		linacfreigabe=CSGET("RAD_INTERLOCK_LINAC1.FREIGABE_DM")
		if linacfreigabe=1
			print "Linac 2 HF ausgefallen!"
			@dosound("car_cras.wav")
		else
			print "Linac 2 HF aus, Interlock nicht ok."
		endif
		lflag=1
	  endif
	  if linachf=1
  		lflag=0
	  endif
  endif
  if inp?(-2) 
	eingabe=inp(-2)
'	print eingabe
	' Leerzeichen - aktueller Messwert als Maximalwert
	if eingabe=32
	  a=b
  	  print "Maximalwert auf ";a;" gesetzt."
	  fix=0
	' a - neue maximale Abweichung eingeben
	else if eingabe=97
	  input "Neue maximale Abweichung in Prozent", grenze
	  if grenze<0
		grenze=0
	  endif
	  print "Neue maximale Abweichung ist ";grenze;" %"
	' f - festen Schwellwert eingeben
	else if eingabe=102
	  input "Fester Bezugswert in pC",a
	  if a<0
	  	a=0
	  endif
	  print "Fester Bezugswert ist ";a;" pC, kein Unterschreiten erlaubt"
	  fix=1
	  grenze=0
	' h - Hilfe
	else if eingabe=104
	  gosub help
	else if eingabe=111
	  print "Alter Offset ist ",offset;" pC"
	  input "Neuer Offset in pC",offset
	  print "Neuer Offset ist ",offset;" pC"
	else if eingabe=79
	  offset=offset+b
	  print "Offset wurde gesetzt auf ";offset;" pC"
	else if eingabe=112
	  if pause=0
	    print "Ausgabe angehalten, zum Weitermachen <p> druecken."
	  endif
	  pause=1-pause
	else if eingabe=108
	  nosound=1-nosound
	  if nosound=0
	  	print "Sound ist jetzt an"
	  else
	    print "Sound ist jetzt aus"
	  endif
	else if eingabe=115
	  print "Ab jetzt wird sofort gewarnt!"
	  warten=0
	else if eingabe=83
	  print "Ab jetzt wird wieder spaeter gewarnt."
	  warten=wartedelay
	else if eingabe=117
	  print "Jetzt wird auch bei Ueberschreiten gewarnt."
	  ueber=1
	else if eingabe=85
	  print "Jetzt wird nicht mehr bei Ueberschreiten gewarnt."
	  ueber=0
	else if eingabe=106
	  hssound=1-hssound
	  if hssound=1
	  	print "Bei Highscore wird jetzt jubiliert."
	  else
	    print "Bei Highscore wird jetzt nicht mehr jubiliert."
	  endif
	else if eingabe=119
	  print "Altes Warndelay ist ";wartedelay;" Zyklen"
	  input "Neues Warndelay in Zyklen",wartedelay
	  if wartedelay<1
	  	wartedelay=1
	  endif
	  print "Neues Warndelay ist ";wartedelay;" Zyklen"
	  warten=wartedelay
	else if eingabe=122
	  print "Alte Zeit zwischen Updates ist ";sleeptime;" s"
	  input "Neue Zeit",sleeptime
	  if sleeptime<1
	  	sleeptime=1
	  endif
	else if eingabe=116
	  print "Teste Lautstaerke..."
	  @dosound("doorbell.au")
	else if eingabe=27
	  quit
	endif
  endif
loop
quit

procedure help
	print
	print "Bedienung:"
	print "<space> Bezugswert auf aktuellen Wert setzen"
	print "<a> Neue maximale Abweichung eingeben, momentan ";grenze;" %"
	print "<f> Festen Grenzwert eingeben, Abweichung auf Null"
	print "<o> Neuen Offset eingeben (momentan ";offset;" pC)"
	print "<O> Momentanen Wert als Offset abziehen"
	print "<s> Sofort bei Abweichung warnen"
	print "<S> Spaeter (mit Warndelay von ";wartedelay;" Zyklen) warnen"
	print "<w> Anderes Warndelay setzen, momentan ";wartedelay;" Zyklen"
	print "<u> Auch bei Ueberschreiten warnen"
	print "<U> Nicht mehr bei Ueberschreiten warnen"
	print "<j> Bei neuem Highscore jubilieren"
	print "<p> Pause (bei Strahlunterbrechungen)"
	print "<z> Setze Zeit zwischen Updates (momentan ";sleeptime;" s)"
	print "<t> Lautstaerke testen"
	print "<l> Laut/leise - Sound an und aus schalten";
	if nosound
		print " (ist aus)"
	else	
		print " (ist an)"
	endif
	print "<h> Diese Hilfe ausgeben"
	print "<esc> Beenden"
	print 
	print "Sollwert [pC]   Aktueller Wert [pC]"
return

procedure dosound(sndfile$)
	if nosound=0
		system "send_sound -server elsahp2 /sgt/ccs/snd/"+sndfile$
	endif
return

function fstr$(z,i)
	local s$
	if z<0
		s$="-"
	else
		s$=" "
	endif
	s$=s$+str$(abs(int(z)),5)+"."+str$(abs(int(frac(z)*10^i)),i,,1)
return s$
