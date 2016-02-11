'
' ANALOGUHR    V.1.08
' Demoprogramm. Es soll nur die Funktionalitaet von X11-BASIC
' demonstrieren. Hier kann man zur Syntax etc. lernen
' Letzte Bearbeitung 09.03.2002  Markus Hoffmann
' Letzte Bearbeitung 18.01.2008  Markus Hoffmann
' Letzte Bearbeitung 27.07.2011  Markus Hoffmann
'

' define some colors

gelb=get_color(65535,65535,0)
rot=get_color(65535,0,0)
weiss=get_color(65535,65535,65535)
schwarz=get_color(0,35535,25535)

bw=640 ! The default with and height of the window
bh=400

setfont "*Courier-bold-r-*12*"
sizew ,bw,bh   ! Set the window size
color schwarz
get_geometry 1,bx,by,bw,bh   ! Ask what size we really got
pbox bx,by,bw,bh
vsync
color rot,schwarz
TEXT 10,10,"Analoguhr mit X11-BASIC von Markus Hoffmann"

'
' PRINT AT(5,24);"Bitte Mittelpunkt der Uhr angeben (Maus)."
'
x=bx+bw/2
y=by+bh/2
r=min(bh,bw)/2.2
color weiss
CIRCLE x,y,r+5
'
' ***** Radien fr Zeiger usw. berechnen.
'
vsync

bs=r/50
bm=r/20
t=r/12
r1=r-5
r2=r*0.914
r3=r*0.857
r4=r*0.84
r5=r*0.8
r6=r*0.571
'
' ***** Aktuelle Zeit in Ti$ bernehmen.
'
@u_hr_zeichnen_1

CLR ti$
al$="Systemzeit oder Neueingabe|der Uhrzeit?"
' ALERT 2,al$,1,"System|Eingabe",v
v=1
IF v=1
  ti$=LEFT$(TIME$,2)
  ti$=ti$+MID$(TIME$,4,2)
  ti$=ti$+RIGHT$(TIME$,2)
ELSE
  CLS
  PRINT AT(10,10);"Bitte geben Sie die ";
  PRINT "aktuelle Uhrzeit ein (HHMMSS): ";
  INPUT  ti$
ENDIF
'
'
' ***** Werte aus Ti$ an Uhrvariablen bergeben.
'
s=VAL(MID$(ti$,5,2))
w1=s-15
m=VAL(MID$(ti$,3,2))
w2=m-15
st=VAL(MID$(ti$,1,2))
IF st>12
  st=st-12
ENDIF
st=st*5+INT(m/12)
w3=st-15
@u_hr_zeichnen_2
'
ti=STIMER
'
' ***** Warten, bis eine Sekunde vergangen ist.
'
do
  IF STIMER>ti
    ti=STIMER
    @s_ekunde
    color gelb
    setfont "*Courier*18*"
    text 20,bh-10,time$
    text 10,bh-30,date$
    vsync
    pause 1-timer+stimer-0.05
    setfont "*Courier-*-r-*18*"
  ENDIF
loop
'
'
PROCEDURE s_ekunde
  INC s
  COLOR schwarz
  DEFLINE ,bs
  LINE x,y,x1,y1
  COLOR weiss
  INC w1
  x1=x+INT(COS(w1*6*PI/180)*r5)
  y1=y+INT(SIN(w1*6*PI/180)*r5)
  LINE x,y,x1,y1
  IF s=60
    CLR s
    @m_inute
  ENDIF
  DEFLINE ,bm
  LINE x,y,x2,y2
  LINE x,y,x3,y3
RETURN

PROCEDURE m_inute
  INC m
  COLOR schwarz
  DEFLINE ,bm
  LINE x,y,x2,y2
  COLOR weiss
  INC w2
  x2=x+INT(COS(w2*6*PI/180)*r5)
  y2=y+INT(SIN(w2*6*PI/180)*r5)
  LINE x,y,x2,y2
  IF m/12=INT(m/12)
    @s_tunde
  ENDIF
  '
 ' s=VAL(MID$(ti$,5,2))
 ' w1=s-15
 ' m=VAL(MID$(ti$,3,2))
 ' w2=m-15
 ' st=VAL(MID$(ti$,1,2))
 ' IF st>12
 '   st=st-12
 ' ENDIF
 ' st=st*5+INT(m/12)
 ' w3=st-15
RETURN
PROCEDURE s_tunde
  INC st
  COLOR schwarz
  DEFLINE ,bm
  LINE x,y,x3,y3
  COLOR weiss
  INC w3
  x3=x+INT(COS(w3*6*PI/180)*r6)
  y3=y+INT(SIN(w3*6*PI/180)*r6)
  LINE x,y,x3,y3
  IF st=60
    CLR st
  ENDIF
RETURN
PROCEDURE u_hr_zeichnen_1
  DEFLINE ,1
  DEFTEXT 1,0,0,t
  graphmode 2
  w=-15
  REPEAT
    INC w
    x4=x+INT(COS(w*6*PI/180)*r3)
    y4=y+INT(SIN(w*6*PI/180)*r3)
    x5=x+INT(COS(w*6*PI/180)*r2)
    y5=y+INT(SIN(w*6*PI/180)*r2)
    x5a=x+INT(COS(w*6*PI/180)*r4)
    y5a=y+INT(SIN(w*6*PI/180)*r4)
    x6=x+INT(COS(w*6*PI/180)*r1)
    y6=y+INT(SIN(w*6*PI/180)*r1)
    LINE x4,y4,x5,y5
    IF w/5=INT(w/5)
      TEXT x6-t/2,y6+t/2,STR$((w+15)/5)
      LINE x5a,y5a,x5,y5
    ENDIF
  UNTIL w=45
  graphmode 1
RETURN
PROCEDURE u_hr_zeichnen_2
  DEFLINE ,bs
  x1=x+INT(COS(w1*6*PI/180)*r5)
  y1=y+INT(SIN(w1*6*PI/180)*r5)
  LINE x,y,x1,y1
  DEFLINE ,bm
  x2=x+INT(COS(w2*6*PI/180)*r5)
  y2=y+INT(SIN(w2*6*PI/180)*r5)
  LINE x,y,x2,y2
  x3=x+INT(COS(w3*6*PI/180)*r6)
  y3=y+INT(SIN(w3*6*PI/180)*r6)
  LINE x,y,x3,y3
RETURN
