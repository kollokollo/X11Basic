' Programm zum Faedeln einer Beamline
' Nov. 1998 Markus Hoffmann
'

' Parameter der Maximiert werden soll:
echo off
optimumparameter$="POL_LASER_AUSBEUTE.INTENSITY_AM"

dim parameter$(100),pmax(100),pmin(100),wert(100)
dim gradient(100),owert(100)
anzparameter=0

repeat
  read parameter$(anzparameter),pmin(anzparameter),pmax(anzparameter),wert(anzparameter)
  gradient(anzparameter)=0
  owert(anzparameter)=wert(anzparameter)
  inc anzparameter
  print parameter$(anzparameter-1)
until parameter$(anzparameter-1)="xxx"
dec anzparameter

gosub setzewert

startwert=csget(optimumparameter$)
zwischenwert=startwert
count=0
do
  gosub messe_gradient 
  gosub gehe_gradient
  pause 10
  altzwischenwert=zwischenwert
  zwischenwert=csget(optimumparameter$)
  print "Faedelung um "+str$(zwischenwert/startwert,4,4)+"% verbessert."
  rate=zwischenwert-altzwischenwert
  print "Iteration #"+str$(count)+"  brachte "+str$(rate)
  if rate<0
    print "### Verschlechterung !!!"
  else if rate=0
    print "### Keine Optimierung !!!"
  endif 
  exit if rate<0.001
  inc count
loop

end

procedure setzewert
  local i
  print "Werte werden gesetzt.."
  for i=0 to anzparameter-1
    if wert(i)<=pmax(i) and wert(i)>=pmin(i)
      csset parameter$(i),wert(i)
      pause 0.1
	endif
  next i
return

procedure messe_gradient
  local i,s,altwert,t
  print "Gradient wird berechnet..."
  s=zwischenwert
  print "zu uebertreffen: ";s
  for i=0 to anzparameter-1
    print "prüfe Parameter: "+parameter$(i)
	altwert=wert(i)
	wert(i)=wert(i)+pmax(i)/100
	csput parameter$(i),wert(i)
	pause 10
	t=csget(optimumparameter$)
	wert(i)=altwert
	csput parameter$(i),wert(i)
	gradient(i)=(t-s)*100/pmax(i)
	print "gradient[";i;"]="+str$(gradient(i))
	print 
  next i
return


procedure gehe_gradient
  local i
  for i=0 to anzparameter-1
    if abs(gradient(i))>0.001
      wert(i)=wert(i)+gradient(i)*pmax(i)/100/2
    endif
  next i
  @setzewert
return



' Parameter, an denen gedreht werden soll

data "POL_SOURCE1_COPS1.STROM1_AC",0,2,0.194
data "POL_SOURCE1_COPS1.STROM2_AC",0,2,0.162
data "POL_SOURCE1_COPS1.STROM3_AC",0,2,0
data "POL_SOURCE1_COPS1.STROM4_AC",0,2,0
data "POL_SOURCE1_COPS1.STROM5_AC",0,2,0.134
data "POL_SOURCE1_COPS1.STROM6_AC",0,2,0.118
data "POL_SOURCE1_COPS1.STROM7_AC",0,2,0.125
data "POL_SOURCE1_COPS1.STROM8_AC",0,2,0.031
'
data "POL_SOURCE1_COPS2.STROM1_AC",0,2,0.193
data "POL_SOURCE1_COPS2.STROM2_AC",0,2,0.016
data "POL_SOURCE1_COPS2.STROM3_AC",0,2,0.081
data "POL_SOURCE1_COPS2.STROM4_AC",0,2,0.054
data "POL_SOURCE1_COPS2.STROM5_AC",0,2,0.147
data "POL_SOURCE1_COPS2.STROM6_AC",0,2,0.060
data "POL_SOURCE1_COPS2.STROM7_AC",0,2,0.044
data "POL_SOURCE1_COPS2.STROM8_AC",0,2,0.022
'
data "POL_SOURCE1_COPS3.STROM1_AC",0,2,0.110
data "POL_SOURCE1_COPS3.STROM2_AC",0,2,0.020
data "POL_SOURCE1_COPS3.STROM3_AC",0,2,0.042
data "POL_SOURCE1_COPS3.STROM4_AC",0,2,0.009
data "POL_SOURCE1_COPS3.STROM5_AC",0,2,0.048
data "POL_SOURCE1_COPS3.STROM6_AC",0,2,0.040
data "POL_SOURCE1_COPS3.STROM7_AC",0,2,0.111
data "POL_SOURCE1_COPS3.STROM8_AC",0,2,0.018
'
data "POL_SOURCE1_COPS4.STROM1_AC",0,2,0
data "POL_SOURCE1_COPS4.STROM2_AC",0,2,0
data "POL_SOURCE1_COPS4.STROM3_AC",0,2,0.240
data "POL_SOURCE1_COPS4.STROM4_AC",0,2,0.170
data "POL_SOURCE1_COPS4.STROM5_AC",0,2,0.104
data "POL_SOURCE1_COPS4.STROM6_AC",0,2,0.197
data "POL_SOURCE1_COPS4.STROM7_AC",0,2,0.046
data "POL_SOURCE1_COPS4.STROM8_AC",0,2,1.245
'
data "POL_SOURCE1_NG1_1.STROM_AC",0,6,5.311
data "POL_SOURCE1_NG1_2.STROM_AC",0,6,4.460
data "POL_SOURCE1_NG1_3.STROM_AC",0,6,5.591
data "POL_SOURCE1_NG1_4.STROM_AC",0,6,1.2
data "POL_SOURCE1_NG1_5.STROM_AC",0,6,1.130
data "POL_SOURCE1_NG1_6.STROM_AC",0,6,1.423
data "POL_SOURCE1_NG1_7.STROM_AC",0,6,0
data "POL_SOURCE1_NG2_1.STROM_AC",0,6,1.150
data "POL_SOURCE1_NG2_2.STROM_AC",0,6,5.3
data "POL_SOURCE1_NG2_3.STROM_AC",0,6,2.513
data "POL_SOURCE1_NG2_4.STROM_AC",0,6,1.093
' data "POL_SOURCE1_NG2_5.STROM_AC",0,20000,11936
' data "POL_SOURCE1_NG2_6.STROM_AC",0,20000,11453
'
data "xxx",0,0,0
