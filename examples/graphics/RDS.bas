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
dim zeil(bw+mt)
usewindow 0
usewindow 1
vsync

usewindow 2
@testbild01
@info

for y=0 to bh
print y'
usewindow 0
t=timer
for x=0 to mt
  zeil(x)=point(x,y)
next x
print timer-t;"."'

t=timer
for x=0 to bw
   f=abs(point(x,y)=0)*30
   ' f=mt/4*EXP(-SQR((x-(bw/2))^2+(y-(bh/2))^2)/100)*COS(SQR((x-(bw/2))^2+(y&-(bh/2))^2)/30)^2
   zeil(x+mt-f)=zeil(x)
next x
print timer-t;
usewindow 1
print " 1"
t=timer
for x=0 to bw
   color zeil(x)
  plot x,y
next x
print timer-t;
vsync
next y
fileselect "Speichere Grafik als ...","./*.dmp","window.dmp",f$
if len(f$)
  savewindow f$
endif
quit
procedure info
  local t$
t$="Das Betrachten dieses Bildes erfordert ein wenig Konzentration"
t$=t$+"|und viel Geduld. Denn bis man den Dreh einmal raus hat, das erfordert"
t$=t$+"|ein wenig ¸bung. Wobei 'ein wenig' auch durchaus eine viertel Stunde"
t$=t$+"|sein kann. Weiﬂ man, wie es geht, kann man jedes Bild in ein paar"
t$=t$+"|Sekunden sehen."
t$=t$+"|Der Kniff ist sehr schwer zu beschreiben. Jeder muû die Erfahrung und den"
t$=t$+"|Aha-Effekt selber erleben."
t$=t$+"|Meine treffendste Anleitung ist ungef‰hr:"
t$=t$+"|Man drehe den Monitor auf guten Kontrast (nicht zu hell!), betrachte ihn aus"
t$=t$+"|ungefÑhr einem halben Meter Entfernung und lasse die Augen durch den Monitor"
t$=t$+"|hindurch in die Ferne schweifen. (nicht das Bild oder die Punkte "
t$=t$+"|fixieren/scharfstellen!)"
t$=t$+"|Nach einer Weile entsteht im Gehirn aus diesen wirren Punkten ein"
t$=t$+"|dreidimensionales Bild."
t$=t$+"|Voraussetztung ist natÅrlich, daû man auf beiden Augen sehen kann."
t$=t$+"|SehschÑrfe spielt Åberhaupt keine Rolle. Es geht also auch ohne Brille."
t$=t$+"|Man beachte: Die Punkte selber ergeben keine Muster o.‰. |Sie sind nur zufÑllig"
t$=t$+"|Sie dienen nur als Medium fÅr die eigentliche 3D-Information."
t$=t$+"|Viel Spass. "
alert 0,t$,1," OK ",a
return




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
  color get_color(65535,65535,65535)
  PBOX 320,0,640,400
  color 0
  PCIRCLE 500,200,90
  PBOX 0,0,320,400
  color get_color(65535,65535,65535)
  PCIRCLE 160,200,90
RETURN
