
' Markus Hoffmann 23.04.2000

' Programm fuer die Online-Energiemessung mit den Touschek-Zaehlern
' V.1.00 noch Testbetrieb

echo off

kleistung=100
kamplitude=sqrt((kleistung+5.672)/791.4)-0.05504
periode=1200
scanbereich=0.01

umlauf=(csget("ELS_HF_MASTER.FREQ_DC")-36e5)/274/100
qz=csget("ELS_MAGNETE_OPTIK.QZ_AC")
energie=csget("ELS_MAGNETE_DIPOL.ENERGIE_AC")*1000
print "Energie="+str$(energie)+" MeV"
print "Qz="+str$(qz)+""
gammaa=energie/440.6485620
print "Gamma a="+str$(gammaa)+""

sema=0

frequenz=abs(gammaa-int(gammaa+1))*umlauf 
gfreq=abs(qz-int(qz+1))*umlauf 
print "Arbeitspunktresonanz="+str$(gfreq)+" kHz"

' fabs(gamma_a-(double)((int)(gamma_a+1)))*umlauf; 
energiemin=energie*(1-scanbereich)
energiemax=energie*(1+scanbereich)
energiemin=2501
energiemax=2507
frequenzmin=@etofreq(energiemin)
frequenzmax=@etofreq(energiemax)

print "Energie-Suchbereich: "+str$(energiemin)+" MeV bis "+str$(energiemax)+" MeV."
print "Frequenz bei Sollenergie ="+str$(frequenz)+" kHz"
print "Rampe zwischen "+str$(frequenzmin)+" kHz und "+str$(frequenzmax)+" kHz."
print "Periode: "+str$(periode)+" Min"
print "Kickerleistung: P="+str$(kleistung)+" W"
csset "ELS_MAGNETE_SHAKER.AMPLITUDE_AC",0
csset "ELS_MAGNETE_SHAKER.FREQUENZ_AC",frequenz
csset "ELS_MAGNETE_SHAKER.AMPLITUDE_AC",kamplitude
csset "ELS_MAGNETE_SHAKER.FREQUENZ_AC",frequenzmin
print ccserr
if ccserr
  print "Kann Shaker nicht erreichen..."
  stop
endif
' Farbdefinitionen
'weiss=GET_COLOR(65535,65535,65535)
'schwarz=GET_COLOR(0,0,0)
'rot=get_color(65535,0,0)
'orange=get_color(65535,65535/2,0)
'blau=get_color(0,0,65535/4)
'gelb=get_color(65535,65535,0)
'grau=get_color(65535/2,65535/2,65535/2)
'hellgrau=get_color(65535/3*2,65535/3*2,65535/3*2)

experiment$=csget$("SUP_GLOBAL_MESSAGE.EXPERIMENT_SC")

open "A",#1,"brandau.log"
print #1,"# Ausgabe von counter.bas vom "+date$
print #1,"# Experiment: "+experiment$
print #1,"# Energie: "+str$(csget("ELS_MAGNETE_DIPOL.ENERGIE_AM"))+" GeV"
print #1,"# Datum Uhrzeit Zeit[s] Strom[mA] Lebensdauer[min] Druck[mbar] Kickfrequenz Brandau Dipolfeld[T] "
' tbase=timer
tbase=0
cssetcallback "SUP_SISCOUNTER.FREQ5_AD",interrupt
print "(";
do
  pause 1
  pause 0.1
  ' Jetzt neue Frequenz setzen
  en=(energiemax-energiemin)/periode*((timer-tbase) MOD periode)+energiemin
  freq=@etofreq(en)
  print "Frequenz: "+str$(freq,5,5)+" kHz. Sollenergie: "+str$(en,6,6)+" MeV."
  sema=1
  csset "ELS_MAGNETE_SHAKER.FREQUENZ_AC",freq
  sema=0
  frequenz=freq
loop
quit

function etofreq(energie2)
  local gammaa
  gammaa=energie2/440.6485620
  return abs(gammaa-int(gammaa+1))*umlauf 
endfunc

procedure interrupt
  if sema=0
  strom=csget("ELS_DIAG_TOROID.STROM_AM")
  leben=csget("ELS_DIAG_TOROID.LEBEN_AM")/60
  nmr=csget("ELS_DIAG_NMR.FELD_AM")
  druck=csget("ELS_VAKUUM_SYS.IGPMEAN_AD")
  print #1,date$+" "+time$+" "+str$(timer-tbase);
  print #1," "+str$(strom)+" "+str$(leben)+" "+str$(druck)+" "+str$(frequenz);
  print #1," "+str$(csget("SUP_SISCOUNTER.FREQ5_AD"))+" ";
  print #1," "+str$(csget("SUP_SISCOUNTER.FREQ6_AD"))+" ";
  print #1," "+str$(csget("SUP_SISCOUNTER.FREQ6_AD"))+" ";
  print #1," "+str$(csget("SUP_SISCOUNTER.FREQ11_AD"))+" ";
  print #1," "+str$(csget("SUP_SISCOUNTER.FREQ12_AD"))+" ";
  print #1," "+str$(csget("SUP_SISCOUNTER.FREQ14_AD"))+" ";
  print #1," "+str$(csget("SUP_SISCOUNTER.FREQ15_AD"))+" ";
  print #1," "+str$(csget("SUP_SISCOUNTER.FREQ16_AD"))+" "+str$(nmr)
  flush #1
    print ".";
  else
    print "<->";
  endif
  flush
return
