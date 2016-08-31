' Programm berechnet RDS-Stereogramme  (c) Markus Hoffmann
' Theorie zum Programm siehe: http://cips02.physik.uni-bonn.de/~hoffmann/rds/rds.html
'
'
'
bx=0
by=0
bw=640
bh=400
mt=120
DIM zeil(bw+mt+1)
USEWINDOW 0
USEWINDOW 1
VSYNC

USEWINDOW 2
@testbild01
@info

FOR y=0 TO bh
  PRINT y'
  USEWINDOW 0
  t=timer
  FOR x=0 TO mt
    zeil(x)=point(x,y)
  NEXT x
  PRINT timer-t;"."'

  t=timer
  FOR x=0 TO bw
    f=abs(point(x,y)=0)*30
    ' f=mt/4*EXP(-SQR((x-(bw/2))^2+(y-(bh/2))^2)/100)*COS(SQR((x-(bw/2))^2+(y&-(bh/2))^2)/30)^2
    zeil(x+mt-f)=zeil(x)
  NEXT x
  PRINT timer-t;
  USEWINDOW 1
  PRINT " 1"
  t=timer
  FOR x=0 TO bw
    COLOR zeil(x)
    PLOT x,y
  NEXT x
  PRINT timer-t;
  VSYNC
NEXT y
FILESELECT "Speichere Grafik als ...","./*.dmp","window.dmp",f$
IF len(f$)
  SAVEWINDOW f$
ENDIF
QUIT
PROCEDURE info
  LOCAL t$
  t$="Das Betrachten dieses Bildes erfordert ein wenig Konzentration"
  t$=t$+"|und viel Geduld. Denn bis man den Dreh einmal raus hat, das erfordert"
  t$=t$+"|ein wenig übung. Wobei 'ein wenig' auch durchaus eine viertel Stunde"
  t$=t$+"|sein kann. Weiß man, wie es geht, kann man jedes Bild in ein paar"
  t$=t$+"|Sekunden sehen."
  t$=t$+"|Der Kniff ist sehr schwer zu beschreiben. Jeder muß die Erfahrung und den"
  t$=t$+"|Aha-Effekt selber erleben."
  t$=t$+"|Meine treffendste Anleitung ist ungefähr:"
  t$=t$+"|Man drehe den Monitor auf guten Kontrast (nicht zu hell!), betrachte ihn aus"
  t$=t$+"|ungefähr einem halben Meter Entfernung und lasse die Augen durch den Monitor"
  t$=t$+"|hindurch in die Ferne schweifen. (nicht das Bild oder die Punkte "
  t$=t$+"|fixieren/scharfstellen!)"
  t$=t$+"|Nach einer Weile entsteht im Gehirn aus diesen wirren Punkten ein"
  t$=t$+"|dreidimensionales Bild."
  t$=t$+"|Voraussetztung ist natürlich, daß man auf beiden Augen sehen kann."
  t$=t$+"|Sehschärfe spielt überhaupt keine Rolle. Es geht also auch ohne Brille."
  t$=t$+"|Man beachte: Die Punkte selber ergeben keine Muster o.ä. |Sie sind nur zufällig"
  t$=t$+"|Sie dienen nur als Medium für die eigentliche 3D-Information."
  t$=t$+"|Viel Spass. "
  ALERT 0,t$,1," OK ",a
RETURN

PROCEDURE pottest
  ' Potentieltest
  CLS
  DO
    MOUSE x&,y&,k&
    PRINT x&,y&,@f
    EXIT IF k&
  LOOP
  END
RETURN

FUNCTION f(ox,oy)
  RETURN mt/4*EXP(-SQR((x-(bw/2))^2+(y-(bh/2))^2)/100)*COS(SQR((x-(bw/2))^2+(y&-(bh/2))^2)/30)^2
ENDFUNCTION
PROCEDURE testbild01
  COLOR get_color(65535,65535,65535)
  PBOX 320,0,640,400
  COLOR 0
  PCIRCLE 500,200,90
  PBOX 0,0,320,400
  COLOR get_color(65535,65535,65535)
  PCIRCLE 160,200,90
RETURN
