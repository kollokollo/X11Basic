echo off
' ######################################################### Version 1.00
' ##	Programm berechnet den Wochentag eines Datums    ## letzte Bearbeitung:
' ## 					     24.09.1993  ## 25.09.1993
' ############### Markus Hoffmann #########################
'
'	PROGRAM datum1
'	
if parm$<>""
  in$=parm$
else	
  print "Bitte das Datum eingeben und zwar in der Form:"
  print "TT.MM.JJJJ : (z.B. ";date$;" )";
  input "",in$
  if in$=""
    in$=date$
  endif
endif
	tag=val(left$(in$,2))
	xtag=val(left$(date$,2))
	jahr=val(right$(in$,4))
	xjahr=val(right$(date$,4))
	monat=val(mid$(in$,4,2))
	xmonat=val(mid$(date$,4,2))	

	DIM tage(12)
	dim wochentage$(7)
	wochentage$(0)="Sonntag"
	wochentage$(1)="Montag"
	wochentage$(2)="Dienstag"
	wochentage$(3)="Mittwoch"
	wochentage$(4)="Donnerstag"
	wochentage$(5)="Freitag"
	wochentage$(6)="Samstag"
	wochentage$(7)="Glueckstag"
	
'	tage()=0,31,29,31,30,31,30,31,31,30,31,30,31
	tage(1)=31
	tage(2)=29
	tage(3)=31
	tage(4)=30
	tage(5)=31
	tage(6)=30
	tage(7)=31
	tage(8)=31
	tage(9)=30
	tage(10)=31
	tage(11)=30
	tage(12)=31
	
'  Fr Julianischen Kalender (beginnt mit M„rz)
	IF monat=1 OR monat=2 
		DEC jahr
		ADD monat,12
	ENDIF
	SUB monat,2

'   Das Jahrhundert nach c[entury]
'
	c=jahr div 100
'
'  Jahreszahl ohne Jahrhundert nach a[nno]
'
	a=jahr MOD 100
'
'        v+--- Das ergibt den Wochentag (0=Sonntag .... 6=Samstag)
'	       ===================================================
	wtag=(tag+INT(2.6*monat-0.1)+a+int(a/4)+int(c/4)-2*c) MOD 7

5:
	zflg=(Jahr>xjahr) or (jahr=xjahr and (monat>xmonat or (monat=xmonat and tag>xtag)))
	wjahr=jahr
	IF monat>10
		wjahr=jahr+1
	ENDIF
	print " Der "'tag;".";1+((monat-1+2) MOD 12);".";wjahr''
	if jahr=xjahr and monat=xmonat and tag=xtag
		print "ist"'
	else
		if zflg
			print "wird sein"'
		else
			print "war"'
		endif
  endif
        print " ein ";wochentage$(wtag);"."
	
	if wtag=5 and tag=13
                
		end
	endif	
'
' Freitag, den 13. berechnen:
'
	if tag<13
		DEC monat
	endif
	tag=13
do
	INC monat
	IF (monat>12)
		INC jahr
		monat=1
	ENDIF
	c=jahr div 100
	a=jahr mod 100
	wtag=(tag+INT(2.6*monat-0.1)+a+int(a/4)+int(c/4)-2*c) mod 7
	if wtag=5 
		GOTO 5
	endif
loop

quit
'
'   ### und nun das fakultative ....
END
