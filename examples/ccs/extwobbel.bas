' Wobbel.bas    fuer die 120 kV Quelle
' Markus Hoffmann im Nov. 1998

' goto wobbel2
i=0
echo off
parameter$="EXT_MAGNETE_QD1.STROM_AC"

sollwert=50
amplitude=45


do

  ww=sollwert+amplitude*sin(i/3)
  
  print space$((ww-sollwert)/amplitude*40+40)+"*"
  
  csput parameter$,ww
  if sin(i/3)<-0.9
    ' beep
  endif
  pause 0.2
  inc i

loop

quit




wobbel2:

echo off
sollwert1=1.093
ampl1=0.01
sollwert2=0.3
ampl2=0.3
i=0
print "Dipol Wobbelt ..."
do
  inc i
  ww1=ampl1*sin(i/20)+sollwert1
'  ww2=-ampl2*sin(i/8)+sollwert2
print space$((ampl1-sollwert1+ww1)*40/ampl1);"#"
 
'  print ww1,ww2
  csset "POL_SOURCE1_NG2_4.STROM_AC",ww1
'  csset "POL_SOURCE1_NG1_7.STROM_AC",ww2
  pause 0.01
loop
