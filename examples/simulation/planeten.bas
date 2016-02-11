' Konstanten
'
a:
ae=146900000000     ! Astronomische Einheit in Meter
tag=24*60*60        ! Tag in Sekunden
jahr=3.65242*tag    ! Jahr in Sekunden (tropisches)
r.erde=6378000      ! Radius der Erde in Meter (子uator)
m.erde=5.973E24     ! Masse der Erde in kg
RESTORE sp_data
DIM name$(12),sym$(12),radius(12),mas(12)
FOR i=0 TO 11
  READ name$(i),sym$(i)
  READ t$
  radius(i)=VAL(t$)
  READ t$
  mas(i)=VAL(t$)
NEXT i
color get_color(32000,32000,32000)
pbox 0,0,640,400
color get_color(0,0,0)
FOR i=0 TO 11
  CIRCLE 0+i*50,200,radius(i)/radius(0)*600
NEXT i

RESTORE a
'
' Planet-finder   von Markus Hoffmann
' 28.11.1986
DEFTEXT 1,0,0,13
DEFLINE 1,0,0,0
DEFMOUSE 2
DIM ptag(12)
FOR p=1 TO 12
  READ ptag(p)
NEXT p
DIM pname$(9)
FOR p=1 TO 9
  READ pname$(p)
NEXT p
DIM position(9)
' Planetposition vom 1.1.1980
FOR p=1 TO 9
  READ position(p)
NEXT p
DIM velocity(9)
FOR p=1 TO 9
  READ velocity(p)
NEXT p
DIM distance(9)
FOR p=1 TO 9
  READ distance(p)
NEXT p
progend=0
x=1
y=2
DIM dpos(2,9)
FOR a=1 TO 2
  FOR p=1 TO 9
    dpos(a,p)=-10
  NEXT p
NEXT a
DIM psymbol$(9)
FOR p=1 TO 9
  READ psymbol$(p)
NEXT p
DIM menue$(70)
FOR i=0 TO 20
  READ menue$(i)
NEXT i
menue$(i)=""
MENUDEF menue$(),menue
GET 0,18,639,381,hinter$
# MENUSET 11,2
# ON BREAK GOSUB breakk
DEFMOUSE 0
color get_color(65535,0,0)

schleife:
DO
  MENU
LOOP
PROCEDURE menue(k)
  IF menue$(k)="  Planeten-Finder"
    ALERT 1,"--Planetenfinder--|von Markus Hoffmann|   (c) 1986",1," Ok |Infos",h
    IF h=2
      @infos
    ENDIF
  ENDIF
  IF menue$(k)="  Ende"
    quit
  ENDIF
  IF menue$(k)="  Programmanfang"
    progend=0
    @start
  ENDIF
  IF menue$(k)="  Wiederhohlungsfaktor eingeben"
    @wiederhohlungsfaktor
  ENDIF
  IF menue$(k)="  Anfangsdatum eingeben"
    @anfangsdatum
  ENDIF
  IF menue$(k)="  Interval eingeben"
    @interval
  ENDIF
  IF menue$(k)="  Uhr Datum "
    ALERT 0,TIME$+"  "+DATE$,1," Ok ",h
  ENDIF
RETURN
PROCEDURE start
  ' ???????????????????
  jahrtag=stag+ptag(smon)
  ' ????????????????????
  WHILE progend=0
    @circle
    @data
    @restart
  WEND
  GET 0,18,639,381,hinter$
RETURN
PROCEDURE circle
  color get_color(0,0,0)
  pbox 0,20,640,400
  color get_color(65535,0,0)

  CIRCLE 320,200,198
  PCIRCLE 320,200,10
RETURN
PROCEDURE data
  @tagjahr
  WHILE period>0
    dloc=4
    @cdate
    @ddate
    FOR planet=1 TO 9
      @cposi
      @dposi
    NEXT planet
    ~INP(-2)
    period=period-1
    jahrtag=jahrtag+interval
    anztage=anztage+interval
    @cend
  WEND
RETURN
PROCEDURE restart
  ALERT 2,"--------------------| Ende des Ablaufs |--------------------",1," Ok ",resp
  progend=1
  FOR a=1 TO 2
    FOR p=1 TO 9
      dpos(a,p)=-10
    NEXT p
  NEXT a
  period=frperiod
RETURN
PROCEDURE tagjahr
  intag=stag
  inmon=smon
  injah=sjah
  asjah=injah-1
  asjah=int(asjah*365.25)+420
  injah=sjah
  IF inmon>2
    GOTO 100
  ENDIF
  injah=injah-1
  inmon=inmon+12
100:
  inmon=inmon+1
  anztage=int(injah*365.25)+int(inmon*30.6)+intag
  anztage=anztage-723258
  ' (date f〉 1.1.1980)
RETURN
PROCEDURE cdate
  counttag=0
  countmon=0
  WHILE jahrtag>counttag
    countmon=countmon+1
    counttag=counttag+ptag(countmon)
  WEND
  dismon=countmon
  distag=jahrtag-(counttag-ptag(countmon))
RETURN
PROCEDURE ddate
  @cdate
  disjah=sjah
  PRINT AT(1,1);STRING$(18," ")
  PRINT AT(1,1);distag;".";dismon;".";disjah
  PRINT STRING$(15,"=")
RETURN
PROCEDURE cposi
  planloc=anztage*velocity(planet)+position(planet)
  planloc=FRAC(planloc/360)*360
  IF planloc<0
    planloc=planloc+360
  ENDIF
RETURN
PROCEDURE dposi
  color get_color(0,0,65535)
  TEXT dpos(x,planet),dpos(y,planet)," "
  ' Deg
  lanloc=planloc/360*2*3.1415927
  dpos(x,planet)=320+distance(planet)*COS(lanloc)
  dpos(y,planet)=200+distance(planet)*SIN(lanloc)
  TEXT dpos(x,planet),dpos(y,planet),psymbol$(planet)
  dloc=dloc+2
  PRINT AT(1,dloc);pname$(planet);INT(planloc);" "
RETURN
PROCEDURE cend
150:
  IF jahrtag<338+ptag(2)
    GOTO 15
  ENDIF
  sjah=sjah+1
  ptag(2)=28
  IF FRAC(sjah/4)=0
    ptag(2)=29
  ENDIF
  jahrtag=jahrtag-337-ptag(2)
  GOTO 150
15:
RETURN
PROCEDURE infos
  cls
  PRINT AT(1,1)
  PRINT STRING$(80,"*")
  PRINT "******************************** Planeten-Finder *******************************"
  PRINT STRING$(60,"*");" von Markus Hoffmann"
  PRINT
  PRINT "    Dieses Programm berechnet die Position Der Planeten im Sonnensystem"
  PRINT "    zu jedem Datum zwischen 1900 und 2000."
  PRINT "    Sie m《sen aber zuerst das Anfangsdatum, den Interval der Beobachtung und       den Wiederhohlungsfaktor eingeben."
  PRINT
  PRINT STRING$(80,"=")
RETURN
PROCEDURE anfangsdatum
  PRINT AT(1,1)
  PRINT TAB(28);"Planet-finder"
  PRINT TAB(60);"von Markus Hoffmann"
  PRINT STRING$(80,"=")
ein:
  PRINT
  INPUT "Dieses Programm berechnet die relativen Positionen der Planeten im Sonnensystem. Bitte geben Sie ein Anfangsdatum ein (TT,MM,JJJJ)";stag,smon,sjah
  PRINT
  IF sjah>2000 OR sjah<1900
    PRINT " Jahreseingabe inkorreckt !!!"
    PRINT
    GOTO ein
  ENDIF
  IF smon>12 OR smon<1
    PRINT " Monatseingabe inkorreckt !!!"
    PRINT
    GOTO ein
  ENDIF
  IF FRAC(sjah/4)=0
    ptag(2)=ptag(2)+1
  ENDIF
  IF stag<1 OR stag>ptag(smon)
    PRINT " Tageseingabe inkorreckt !!!"
    PRINT
    GOTO ein
  ENDIF
  anf=1
  @anpr
RETURN
PROCEDURE wiederhohlungsfaktor
  PRINT AT(1,1)
  PRINT TAB(28);"Planet-finder"
  PRINT TAB(60);"von Markus Hoffmann"
  PRINT STRING$(80,"=")
  PRINT
  PRINT " Sie k馬nen einen Wiederhohlungsfaktor und einen Beobachtungsinterval bestimmen. Vorschlag:10,30 ( 10 mal einen Interval von 30 Tagen darstellen )."
  INPUT " Bitte geben Sie den Wiederhohlungsfaktor ein ";period
  wie=1
  frperiod=period
  @anpr
RETURN
PROCEDURE interval
  PRINT AT(1,1)
  PRINT TAB(28);"Planet-finder"
  PRINT TAB(60);"von Markus Hoffmann"
  PRINT STRING$(80,"=")
  PRINT
  PRINT " Sie k馬nen einen Wiederhohlungsfaktor und einen Beobachtungsinterval bestimmen. Vorschlag:10,30 ( 10 mal einen Interval von 30 Tagen darstellen )."
  INPUT " Bitte geben Sie das Interval der Beobachtung ein ";interval
  interv=1
  @anpr
RETURN
PROCEDURE anpr
  IF interv=1 AND wie=1 AND anf=1
    MENU 11,3
  ENDIF
RETURN
PROCEDURE breakk
  ALERT 2,"Programm verlassen ?      |oder Hauptmenu ?     ",2,"Verlassen|Hauptmen|weiter",balert
  IF balert=3
    CONT
  ENDIF
  IF balert=1
    END
  ENDIF
  IF balert=2
    RESUME schleife
  ENDIF
  MENU OFF
RETURN

DATA 31,28,31,30,31,30,31,31,30,31,30,31
DATA "Merkur(m)","Venus    ","Erde     ","Mars(M)  ","Jupiter  ","Saturn   ","Uranus   ","Neptun   ","Pluto    "
DATA 242.5352528,356.3298392,99.44113707,131.3867597,150.4441072
DATA 170.8914991,231.3638537,259.8523031,204.0724906
DATA 4.092179199,1.6021064,0.985602007,0.524027203
DATA 0.083078881,0.033460735,0.011745630,0.005958761,0.003953409
DATA 30,50,70,90,110,130,150,170,190
DATA "m","V","E","M","J","S","U","N","P"
DATA "INFO","  Planeten-Finder"
DATA "-----------------------"
DATA "- 1","- 2","- 3","- 4","- 5","- 6",""
DATA "Programm","  Programmanfang","  Anfangsdatum eingeben","  Interval eingeben","  Wiederhohlungsfaktor eingeben","--------------------------------","  Ende",""
DATA "Uhr Datum","  Uhr Datum ",""
DATA ***
p_data:
' Planetennamen
DATA "Merkur","Venus","Erde","Mars","Ceres u.a.","Jupiter","Saturn","Uranus","Neptun","Pluto"
' Entfernungen von der Sonne
DATA
' Radien (子uator) R/Rerde
DATA 0.38,0.95,
' Umlaufzeiten in Jahren (365,25 Tage)
DATA 0.241,0.615,1,1.881,4.601,11.87,29.63,84.67,165.5,251.9
'
sp_data:
'    Name  Zeichen Radius in Erdradien Masse/M[erde] Entfernung von Sonne in AE
DATA "Sonne  ","O ", 109.12511        , 332998.5
DATA "Merkur ","m ", 0.38             ,      0.055
DATA "Venus  ","V ", 0.95             ,      0.82
DATA "Erde   ","E ", 1.00             ,      1.00
DATA "Mars   ","M ", 0.53             ,      0.11
DATA "Ceres  ","C ", 0.0001           ,      0.00001
DATA "Jupiter","J ", 11.2             ,    317.8
DATA "Saturn ","S ", 9.45             ,     95.1
DATA "Uranus ","U ", 4.02             ,     14.6
DATA "Neptun ","N ", 3.89             ,     17.1
DATA "Pluto  ","P ", 0.18             ,      0.002
DATA "Mond   ","Mo", 0.27             ,      0.012
