' Programm zum Vermessen des Sichtfeldes eines Auges

' (c) Markus Hoffmann

' Erstellt eine Karte der blinden Bereiche der Netzhaut, um evtl. Augensch"aden 
' sichtbar zu machen
'

' Ein Auge zuhalten, mit dem Anderen aus ca. 30 cm vom Bildschirm
' den roten Punkt in der Mitte fixieren
' Das Auftauchen des weissen Punktes durch einen Tastendruck quittieren

sizew ,800,800
color get_color(0,0,0)
pbox 0,0,800,800
color get_color(65535,0,0)
pcircle 400,400,6
randomize
vsync
pause 5
do
  pause 0.3
  x=random(20)*10+600
  y=random(20)*10+400

  color get_color(65535,65535,65535)
  pcircle x,y,4
  beep
  vsync
  pause 0.3
  color get_color(0,0,0)
  pcircle x,y,4
  vsync
  t=timer
  keyevent
  lll=timer-t
  if lll*10>25
    color get_color(45535,5535,5535)
    pcircle x,y,7
  endif
  if lll*10>8
    color get_color(5535,5535,65535)
    text x-4,y+4,str$(int(lll*10))
  else
    color get_color(5535,5535,25535)
    pcircle x,y,4
  endif
loop
quit
