ECHO OFF
tunt$=chr$(27)+"[1m"
tnorm$=chr$(27)+"[m"
CLS
PRINT "   von Markus Hoffmann       07.05.1986"
PRINT "       *==> DURCH DIE WÜSTE <==*"
DIM a(10)
PRINT AT(5,9);tunt$+"SPIELANLEITUNG"+tnorm$
PRINT AT(7,9);"Sie müssen mit Ihrem Lastwagen von Lager 1 durch die Wüste zu"
PRINT "        Lager 10  gelangen. Sie  können  die  Zwischenstationen  dazu"
PRINT "        benutzen, Kanister ab- bzw. aufzuladen, denn auf dem Weg  von"
PRINT "        einem Lager zum nächsten verbraucht der Lastwagen 1 Kanister."
PRINT "        Sie  können  jedoch nur eine  bestimmte Anzahl  von Kanistern"
PRINT "        mitnehmen.    Wenn  Sie  jedoch  plötzlich  mit  leerem  Tank"
PRINT "        dastehen, haben Sie leider verloren."
PRINT
' Spiel
DO
	CLR a(),t,n
	a(1)=80
	
	s=1
	b=1
	PRINT
	PRINT
	REPEAT
		PRINT "  1 - 5 Schwierigkeitsgrad: 1 leicht 5 schwer"
		print "    0   Alle Schwierigkeitsstufen gestaffelt"
		print "    8   Altes Spiel fortsetzten"
		print "    9   Programmende"
		INPUT " Ihre Wahl: ";sc
	UNTIL sc>=0 AND sc<=5 or sc=8 or sc=9  
	if sc=9 
		end
	else if sc=8
		varload a(),m,t,b,sc,s,n
	else IF sc=0 OR sc=1
		m=8
	else IF sc=2 
		m=7
	else IF sc=3 
		m=6 
	else IF sc=4 
		m=5
	else IF sc=5 
		m=4
	ENDIF
	' Runde
	DO
		do
			CLS
			PRINT AT(1,1);"      Durch die Wueste        ein Denkspiel von Markus Hoffmann"
			PRINT AT(4,5);"Lager   :  1  2  3  4  5  6  7  8  9 10"
			print at(20,40);"Ladekapazität:"'m
		        print at(20,2);"9 = Programmende"
		        ' Step
		
			PRINT AT(5,5);"Kanister: ";
			for i=1 to 10
			  print str$(a(i),2,2);" ";
			next i
			print
			x=11+(s-1)*3
			PRINT TAB(x+4);"^     "
			PRINT TAB(x+4);"|     "
			PRINT TAB(x);"Lastwagen                "
			PRINT TAB(x+4);t;"      "
			IF s=10 
				print "GESCHAFFT !"
			'	exit if TRUE
				goto ex
			ENDIF
			IF t=0 AND a(s)=0 
				print  "Sie müssen leider laufen !"
			'	exit if true
				goto ex
			ENDIF
			PRINT at(14,2);"Ihre Möglichkeiten :"
			IF t>1 
				PRINT AT(15,2);"1 = Kanister abladen"
			else
				print at(15,2);"                    "
			ENDIF
			IF a(s)>0 
				PRINT AT(16,2);"2 = Kanister zuladen"
			else 
				print at(16,2);"                    "
			ENDIF
			IF t>0 
				PRINT AT(17,2);"3 = zum nächsten Lager"
			else
				print at(17,2);"                      "
			ENDIF
			IF t>0 AND s>1 
				PRINT AT(18,2);"4 = zum vorangehenden Lager"
			else
				print at(18,2);"                           "
			ENDIF
			
' Wahl
			repeat
				print at(21,10);"    ";chr$(13);
				input "Ihre Wahl :",wahl
			until wahl<10 and wahl>0
			
' ABLADEN
			IF t>1 AND wahl=1
				DO
					INPUT "Wieviele abladen :";a
					if a>t
						print "Sie können doch höchstens"'t'"abladen !"
					endif
					exit if a<=t and a=int(a) and a>=0
					print "Falsche Eingabe ! nocheinmal"
				LOOP
				a(s)=a(s)+a
				sub t,a
			ENDIF
' AUFLADEN
			IF a(s)>0 AND wahl=2
				do
					INPUT "Wieviele aufladen :";anz_auf
					if anz_auf>m-t
						print "Es ist nur Platz für "'m'"Kanister !"
					endif
					exit if anz_auf>=0 and anz_auf=int(anz_auf) and anz_auf<=a(s) and anz_auf<=m-t
					print "Falsche Eingabe ! nochmal!"
				loop
				add t,anz_auf
				a(s)=a(s)-anz_auf
			ENDIF
' N„chstes Lager
			IF t>0 AND wahl=3
				inc s
				dec t
				inc n
			ENDIF
' Voriges Lager
			IF t>0 AND s>1 AND wahl=4
				dec s
				dec t
				inc n
			ENDIF
' Ende
			if wahl=9
				input "Wollen Sie den Spielstand abspeichern ? [J/N]";janee$
				if left$(upper$(janee$))="J"
				'	varsave a(),m,t,b,sc,n
				endif
				print "Ich hoffe, Sie hatten viel Spaß. Bis zum naechsten mal ..."
				end
			endif
		LOOP	
' Spielende:
		print 
		print "Sie haben"'n'"Kanister gebraucht,"
		a=a(2)+a(3)+a(4)+a(5)+a(6)+a(7)+a(8)+a(9)
		print "brachten"'t'"mit und haben"'a'"in der Wueste gelassen."
		IF m=4 AND sc=0 AND s=10 
			print tab(12);"Herzlichen Glückwunsch, machen Sie sich bereit,"
			print tab(6);"die Lorbeeren zu empfangen:"
			print " Das war eine wahre Meisterleistung."
			print " T O L L ! ! !"
			exit if true
		ENDIF
		IF sc=0  
			print " Nächste Runde"
			print " -------------"
			dec m
			inc b
			pause 1
		ENDIF
		exit if sc<>0
	LOOP
	ex:
	IF sc=0 AND t=0 AND a(s)=0 
		PRINT " Sie sind bis zum"'b;"-ten Schwierigkeitsgrad gekommen."
	ENDIF
	input "Wollen Sie noch einmal spielen ? [J/N]";o$
	EXIT IF left$(upper$(o$),1)<>"J"
LOOP
END
REM ******* Markus Hoffmann *******
REM *******   7.5.1986      *******
