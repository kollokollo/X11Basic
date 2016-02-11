' Abbildungen an der Linse
' geschrieben von Markus Hoffmann für  Atari nach der Idee von ... Hock
'       15.6.1986

echo off
weiss=get_color(65535,65535,65535)
schwarz=get_color(0,0,0)
rot=get_color(65535,0,0)
orange=get_color(65535,30000,0)


o$="Fehler"

DEFTEXT 1,0.1,0.2,0
DEFMOUSE 2
clearw
color weiss
text 0,20,"  Menuleiste noch nicht erstellt...    |  Maus noch nicht aktiv  | Markus Hoffmann 1986"
color rot
ltext 160,40,"Optik"
color weiss
DEFTEXT 0,0.05,0.1,0
ltext 100,80,"Dieses Programm berechnet Werte"
ltext 100,100,"         aus der Optik nach der"
ltext 100,125,"               1   1   1"
ltext 100,140,"LINSENFORMEL : - + - = -"
ltext 100,155,"               g   b   f"
ltext 10,200," Es ergeben sich fuer die einzelnen Werte"
ltext 10,220," nach Umformung die folgenden Formeln"
ltext 10,260,"   1.           Brennweite f=(g*b)/(g+b)"
ltext 10,280,"   2.     Gegenstandsweite g=(b*f)/(b-f)"
ltext 10,300,"   3.            Bildweite b=(g*f)/(g-f)"
ltext 310,180,"   Ansonsten gelten folgende Zeichen"
ltext 310,220,"     G =  Gegenstandsgroesse"
ltext 310,240,"     B =  Bildgroesse"
ltext 310,260,"     A =  Abbildungsmassstab"
ltext 310,300,"    Der Wert > G < muss vorliegen !"
ltext 310,320,"    > A < wird immer mitberechnet"
ltext 310,380,"    > Weiter, Taste druecken"

vsync
keyevent a
390:
color orange
PBOX 10,10,629,389
defmouse 0
t$=" Was soll berechnet werden ? ||"
t$=t$+"  Bildweite b und Bildgröße B = < 1 >||"
t$=t$+"           Gegenstandsweite g = < 2 >||"
t$=t$+"                 Brennweite f = < 3 >|"

alert 0,t$,4,"1|2|3|QUIT",n
IF n=4
  quit
ENDIF
GOTO 760

procedure 500
  ' ---------------------------
  o$="Bildweite und Bildgröße"
  ' ---------------------------
'   DEFTEXT 1,0,0,13
'  TITLEW #1,"Eingaben"
'  INFOW #1,"Berechnung der "+o$
'  OPENW 1
  CLS
  PRINT
  PRINT
  INPUT " Gegenstandsgröße G  (0-110) ";gg
  INPUT " Gegenstandsweite g  (0-250) ";g
  INPUT " Brennweite       f  ((-200)-200) ";f
  b=g*f/(g-f)
  aa=b/g
  bb=aa*gg
'  CLOSEW 1
RETURN
procedure 580
  ' ----------------------
  o$="Gegenstandsweite"
  ' ----------------------
'  DEFTEXT 1,0,0,13
'  TITLEW #1,"Eingaben"
'  INFOW #1,"Berechnung der "+o$
'  OPENW 1
  CLS
  PRINT
  PRINT
  INPUT " Gegenstandsgröße G  (0-110) ";gg
  INPUT " Brennweite f        ((-200)-200) ";f
  INPUT " Bildweite b         (0-250) ";b
  g=(b*f)/(b-f)
  aa=b/g
  bb=aa*gg
'  CLOSEW 1
RETURN
procedure 660
  ' ------------------
  o$="Brennweite "
  ' ------------------
  DEFTEXT 1,0,0,13
'  TITLEW #1,"Eingaben"
'  INFOW #1,"Berechnung der "+o$
'  OPENW 1
  CLS
  PRINT
  PRINT
  INPUT " Gegenstandsgröße G  (0-110) ";gg
  INPUT " Gegenstandsweite g  (0-250) ";g
  INPUT " Bildweite b         (0-250) ";b
  f=(b*g)/(b+g)
  aa=b/g
  bb=(b/g)*gg
'  CLOSEW 1
RETURN
'
' Spagetticode (jaja, das war frueher so ... )
760:
if n=1 
  GOSUB 500
else if n=2 
  gosub 580
else if n=3
  gosub 660
endif
fac=0
IF gg>55
  fac=2
  gg=gg/2
  bb=bb/2
ENDIF
IF gg<15
  fac=0.5
  gg=gg*2
  bb=bb*2
ENDIF

vsync

color weiss
PBOX 0,0,639,399
color orange
PBOX 10,10,609,179
color rot
PBOX 20,20,599,169

color schwarz
line 30,85,590,85
line 310,30,310,140
line 310+f,88,310+f,82
line 310-f,88,310-f,82
 TEXT 310-f,93,"f"
 TEXT 310+f,93,"f"
FOR x=-7 TO 7
  line 310-g+x,85,310-g,85-gg
NEXT x

line 310-g,85-gg,310+b,85+bb
line 310-g,85-gg,310,85-gg
line 310,85-gg,310+b,85+bb

FOR x=-7 TO 7
  line 310+b+x,85,310+b,85+bb
NEXT x

' DEFTEXT 1,0,0,13
 TEXT 318-g,90-gg/2,"G"
 TEXT 318+b,90+bb/2,"B"

' DEFLINE 1,0,1,1
 line 310-g,140,310,140
 line 310,140,310+b,140
 TEXT 310-g/2,155,"g"
 TEXT 310+b/2,155,"b"
IF fac=2
  gg=gg*2
  bb=bb*2
ENDIF
IF fac=0.5
  gg=gg/2
  bb=bb/2
ENDIF
color schwarz
ltext 20,300,"Gegenstandsgroesse G = "+str$(gg)
ltext 20,320,"Gegenstandsweite   g = "+str$(g)
ltext 20,340,"Brennweite         f = "+str$(f)
ltext 320,300,"Bildweite          b = "+str$(b)
ltext 320,320,"Bildgroesse        B = "+str$(bb)
ltext 320,340,"Abbildungsmassstab A = "+str$(aa)+" zu 1"
defmouse 0
alert 0,"Noch eine Berechnung der |"+o$,2,"Ja|Nein",balert
IF balert=1
  GOTO 760
ENDIF
GOTO 390
