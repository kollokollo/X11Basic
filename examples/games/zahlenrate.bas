ECHO OFF
CLS
PRINT TAB(12);"*=*=*=*>  Zahlenraten  <*=*=*=*     (c) Markus Hoffmann"
print tab(40);"Version 1.03"
PRINT
PRINT "Ziel des Spieles :"
PRINT "In mˆglichst wenigen Versuchen      "
PRINT "eine Zahl zwischen 1 und 1000 zu erraten."
print
' varload best.user.zahlenr$,best.vers.zahlenr,zahlenrdate$
do
	print "Der Bisherig beste User ist ";

	if best.vers.zahlenr
		print best.user.zahlenr$'"mit"'best.vers.zahlenr'"Versuchen am"'zahlenrdate$'"."
	else
		print " bisher niemand."
	endif
	'
	print string$(79,"-")
	PRINT "Neues Spiel:"
	PRINT
        k=0
	z=RANDOM(1000)
	DO
		INPUT "Die Ratezahl eingeben:",r
		inc k
		EXIT IF r=z
		if r<0 or r>1000
		        print "Es hat wenig Sinn, negative Zahlen oder Zahlen groesser 1000 einzugeben ! "
			input "Wollen Sie das Spiel beenden ? 1=ja ",yn
			if yn=1
			  end
			endif
		ELSE IF r>z
			print "Die Zahl"'r'"ist zu groﬂ !"
		else
			PRINT "Die Zahl"'r'"ist zu klein !"
		endif
	LOOP
	PRINT "Richtig geraten, die Zahl ist"'z;"."
	PRINT "Es waren"'k'"Versuche noetig."
	if k<best.vers.zahlenr or best.vers.zahlenr=0
		print "Du bist der Beste bisher !"
		best.user.zahlenr$=env$("USER")
		best.vers.zahlenr=k
		zahlenrdate$=date$+" "+time$
	'	varsave best.user.zahlenr$,best.vers.zahlenr,zahlenrdate$
	endif
	INPUT "Noch ein Spiel 1=Ja";r
	EXIT IF r<>1 
	k=0
LOOP
END
REM Verfasser  ***** Markus Hoffmann ******
REM            *****     26.4.1986   ******
