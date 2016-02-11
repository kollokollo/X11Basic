' Programm berechnet RDS-Stereogramme  (c) Markus Hoffmann
' Theorie zum Programm siehe: http://cips02.physik.uni-bonn.de/~hoffmann/rds/rds.html
'

echo off
'
'
bx=0
by=0
bw=640
bh=400
mt=120
dim zeil(bw+mt)
usewindow 0
usewindow 1
vsync

usewindow 2
@testbild01

vsync
for y=0 to bh
print y'
usewindow 0
for x=0 to mt
  zeil(x)=point(x,y)
next x
print "."'
usewindow 1
for x=0 to bw
  ' f=point(x,y)*70
   f=mt/4*EXP(-SQR((x-(bw/2))^2+(y-(bh/2))^2)/100)*COS(SQR((x-(bw/2))^2+(y&-(bh/2))^2)/30)^2
   zeil(x+mt-f)=zeil(x)
next x
'usewindow 1
print "1"
for x=0 to bw
   color zeil(x)
  plot x,y
next x
vsync
next y
fileselect "Speichere Grafik als ...","./*.dmp","window.dmp",f$
if len(f$)
  savewindow f$
endif
PRINT "Das Betrachten dieses Bildes erfordert ein wenig Konzentration"
PRINT "und viel Geduld. Denn bis man den Dreh einmal raus hat, das erfordert"
PRINT "ein wenig öbung. Wobei 'ein wenig' auch durchaus eine viertel Stunde"
PRINT "sein kann. Weiû man, wie es geht, kann man jedes Bild in ein paar Sekunden"
PRINT "sehen."
PRINT "Der Kniff ist sehr schwer zu beschreiben. Jeder muû die Erfahrung und den"
PRINT "Aha-Effekt selber erleben."
PRINT "Meine treffendste Anleitung ist ungef‰hr:"
PRINT " Man drehe den Monitor auf guten Kontrast (nicht zu hell!), betrachte ihn aus"
PRINT "ungefÑhr einem halben Meter Entfernung und lasse die Augen durch den Monitor"
PRINT "hindurch in die Ferne schweifen. (nicht das Bild oder die Punkte "
PRINT "fixieren/scharfstellen!)"
PRINT "Nach einer Weile entsteht im Gehirn aus diesen wirren Punkten ein"
PRINT "dreidimensionales Bild."
PRINT "Voraussetztung ist natÅrlich, daû man auf beiden Augen sehen kann."
PRINT "SehschÑrfe spielt Åberhaupt keine Rolle. Es geht also auch ohne Brille."
PRINT "Man beachte: Die Punkte selber ergeben keine Muster o.Ñ. Sie sind nur zufÑllig"
PRINT "Sie dienen nur als Medium fÅr die eigentliche 3D-Information."
PRINT "Viel Spass. "
quit




PROCEDURE pottest
  ' Potentieltest
  cls
  DO
    MOUSE x&,y&,k&
    print x&,y&,@f
    EXIT IF k&
  LOOP
  end
RETURN


function f(ox,oy)
return mt/4*EXP(-SQR((x-(bw/2))^2+(y-(bh/2))^2)/100)*COS(SQR((x-(bw/2))^2+(y&-(bh/2))^2)/30)^2
PROCEDURE testbild01
  color 1
  PBOX 320,0,640,400
  color 0
  PCIRCLE 500,200,90
  color 1
  PCIRCLE 160,200,90
RETURN
