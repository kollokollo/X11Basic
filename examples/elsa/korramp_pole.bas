'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
'%
'% Name: 
'%	rampe.bas
'%
'% Purpose: 
'%	Erzeugung von Rampen fuer den gesamten ELSA-Zyklus mit Variabilitaeten
'%   fuer den Betrieb mit Polarisierten Elektronen
'%
'% Author:
'% 	Markus Hoffmann im feb 1999
'%
'% Modification History:
'%		25.02.99 Markus Hoffmann: erste Version
'%      08.03.99 einigermassen getestet
'%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
ECHO OFF

anz_korrektoren=32
deltat=0.002

DIM vangle(anz_korrektoren)
DIM vflag(anz_korrektoren)
DIM hkicks(anz_korrektoren)
DIM vkicks(anz_korrektoren)
DIM harme(7),harmse(7),harmee(7),sincomp(7),coscomp(7)
DIM startbump(7),maxbump(7),endbump(7)

' Azimuthwinkel der Korrektoren
azdata:
DATA 0,0,1,0,2,0,4,0,7,8,0,10,0,11,0,0,12,12,13,0,14,15,0,17,19,20,0,22,0,23,0,0
restore azdata
for i=0 to anz_korrektoren-1
  read a
  vangle(i)=pi/12*a
next i

' Korrektoren fuer harmonische Korrektur benutzen oder nicht
vfdata:
data 1,0,1,0,1,0,1,0,1,1,0,1,0,1,0,0,1,0,1,0,1,1,0,1,1,1,0,1,0,1,0,0
restore vfdata
for i=0 to anz_korrektoren-1
  read a
  vflag(i)=a
next i

setfont "*Courier*24*"
schwarz=get_color(0,0,0)
weiss=get_color(65535,65535,65535)
gelb=get_color(65535,65535,0)
rot=get_color(65535,0,0)
lila=get_color(65535,0,65535)


' Resonanzdaten einlesen fuer imperfections
restore resonanzdata
for i=0 to 6
  read n,e,st,en,s,c
  harme(i)=e/1000
  harmse(i)=st/1000
  harmee(i)=en/1000
  sincomp(i)=s
  coscomp(i)=c
next i


color schwarz
pbox 0,0,640,400
color gelb
text 20,50,"Korrektoren-Rampen berechnen"

neuauslesen:
t$="Das Timing wird jetzt ausgelesen. Ist es gesetzt ?|"
t$=t$+"|Die aktuellen Einstellungen sind:|"



' Energien auslesen
'

injenergy=csget("ELS_ENERGY_MODEL.INJENERGY_AC")
extenergy=csget("ELS_ENERGY_MODEL.EXTENERGY_AC")
knickenergy=csget("ELS_ENERGY_MODEL.KNICKENERGY_AC")

' Timing auslesen
'
injshots=csget("ELS_TIMING_MODEL.INJTRAIN_DC")
injphases=csget("ELS_TIMING_MODEL.INJPHASES_DC")
injdamp=csget("ELS_TIMING_MODEL.INJDAMP_AC")
rampup=csget("ELS_TIMING_MODEL.RAMPUP_AC")
extprepare=csget("ELS_TIMING_MODEL.EXTPREPARE_AC")
exttime=csget("ELS_TIMING_MODEL.EXTTIME_AC")
rampdown=csget("ELS_TIMING_MODEL.RAMPDO_AC")
opttime=csget("ELS_TIMING_MODEL.OPTTIME_AC")
knicktime=csget("ELS_TIMING_MODEL.RAMPKNICK_AC")

' absolute Zeitpunkte
'
dampstart=injphases*injshots*0.020
rampstart=csget("ELS_TIMING_CYCLE.RAMPSTART_AC")
rampstop=csget("ELS_TIMING_CYCLE.RAMPSTOP_AC")
extstart=csget("ELS_TIMING_CYCLE.EXTSTART_AC")
extstop=csget("ELS_TIMING_CYCLE.EXTSTOP_AC")
zyklusstop=csget("ELS_TIMING_MODEL.CLENGTH_AC")
injprepstart=extstop+rampdown 


' Rampform auslesen
'

rampform=csget("ELS_ENERGY_MODEL.RMODEL_DC")


xerror=1
t$=t$+"|Rampe von "+str$(injenergy,4,4)+" GeV nach "+str$(extenergy,4,4)+" GeV|" 
if injenergy=extenergy
  t$=t$+"*** Rampe ??? ***|"
  xerror=2
endif
if rampform=0
  t$=t$+"Rampe linear ! Gradient: "
  
  ' Gradienten berechnen
  '
  gradientup=(extenergy-injenergy)/rampup
  gradientdown=(extenergy-injenergy)/rampdown
  t$=t$+str$(gradientup,5,5)+" GeV/s und "
  t$=t$+str$(gradientdown,5,5)+" GeV/s |"
  
else 
  t$=t$+"***Rampform ungueltig  ! ****|"
  xerror=2
endif
  t$=t$+"|Zyklusstart   :=  0|"
  t$=t$+"Daempfungsstart ="+str$(dampstart,4,4)+"|"
  t$=t$+"Rampstart       ="+str$(rampstart,4,4)+"|"
  t$=t$+"Rampstop        ="+str$(rampstop,4,4)+"|"
  t$=t$+"Extraktionsstart="+str$(extstart,4,4)+"|"
  t$=t$+"Extraktionsstop ="+str$(extstop,4,4)+"|"
  t$=t$+"Injprepstat     ="+str$(injprepstart,4,4)+"|"
  t$=t$+"Zykluslaenge    ="+str$(zyklusstop,4,4)+"|"
  

balert=form_alert(xerror,"[2]["+t$+"][ OK |Neu auslesen|Simulieren|ABBRUCH]")
if balert=2
  goto neuauslesen
else if balert=4
  quit
endif

defmouse 2
text 200,200,"bitte warten..."

' zum testen
if balert=3
  @status("--< Simulierte Werte, Test >--")
  print "--< Simulierte Werte, Test >--"
  rampform=0
  rampstart=0.2
  injenergy=1.2
  extenergy=2.1
  rampup=0.5
  rampdown=0.4
  rampstop=rampstart+rampup
  extstart=rampstop+0.1
  extstop=extstart+0.8
  zyklusstop=extstop+rampdown+0.1
  injprepstart=extstop+rampdown
  
  gradientup=(extenergy-injenergy)/rampup
  gradientdown=(extenergy-injenergy)/rampdown
endif
vsync
clen=round(zyklusstop/deltat)

'
'
' Erzeuge Energievektor 

print "Energievektor:"

@status("erzeuge Energievektor ...")
text 200,300,"Zeitschritt="+str$(deltat)+" s"
dim tvec(clen-1),evec(clen-1),tvec2(clen-1) 
dim hivec(31,clen-1),vivec(31,clen-1)
dim hivec2(31,clen-1),vivec2(31,clen-1)

@hscalerbar(430,360,100)
vsync
for i=0 to clen-1
  tvec(i)=i*deltat
  etime=i*deltat
  if etime<rampstart
    evec(i)=injenergy
  else if (etime>=rampstart) and (etime<rampstop)
    if rampform=0
      evec(i)=injenergy+gradientup*(etime-rampstart)
    endif
  else if (etime>=rampstop) and (etime<extstop)
    evec(i)=extenergy
  else if (etime>=extstop) and (etime<injprepstart)
    evec(i)=extenergy-gradientdown*(etime-extstop)
  else
    evec(i)=injenergy
  endif
  if (i mod 10)=0
  @do_hscaler(430,360,100,i/(clen-1)) 
  vsync
  endif 
next i
defmouse 0
@parameter_eingabe
@plot_erampe(10,300,620,100)
@read_orbit(orbitdf$)

' Skalierungsfaktoren fuer Korrektoren auslesen
'
hscaling()=csvget("ELS_MAGNETE_HCORRS.SCALING_AM",32)
vscaling()=csvget("ELS_MAGNETE_VCORRS.SCALING_AM",32)

' Stromrampen erzeugen
'
print "Stromrampen fuer die Korrektoren werden berechnet..."
@status("Stromrampen erzeugen")

' Energieskalierte Korrektur:

for j=0 to 32-1
  hivec(j,:)=smul(evec(),hkicks(j)*hscaling(j))
  vivec(j,:)=smul(evec(),vkicks(j)*vscaling(j))
next j

@rampen_plotten(10,50,620,250)


' Hinzufuegen der harm. Korrektur
'

print "Hinzufuegen der harm. Korrektur"
@status("Hinzufuegen der harm. Korrektur")

for i=0 to 6
  startbump(i)=rampstart+(harmse(i)-injenergy)/gradientup
  maxbump(i)=rampstart+(harme(i)-injenergy)/gradientup
  endbump(i)=rampstart+(harmee(i)-injenergy)/gradientup
next i

dim scalevec(clen-1)

for j=0 to 6
  if (harme(j)>injenergy) and (harme(j)<extenergy)
    print "Bump #"+str$(j+1)+" [";
	flush
	arrayfill scalevec(),0
    for i=0 to clen-1
	  etime=i*deltat
	 ' if etime<startbump(j) 
	'	scalevec(i)=0
  	  if (etime>=startbump(j)) and (etime<maxbump(j))
		scalevec(i)=(etime-startbump(j))/(maxbump(j)-startbump(j))
	  else if (etime>=maxbump(j)) and (etime<=endbump(j))
		scalevec(i)=(etime-endbump(j))/(maxbump(j)-endbump(j))
	 ' else if etime>=maxbump 
	 '	scalevec(i)=0
	  endif
    next i 
	print "+";
	flush
    for i=0 to 32-1
      if (vscaling(i)<>0) and (vflag(i)<>0)
        if sincomp(j)<>0 or coscomp(j)<>0
	      vivec(i,:)=vivec(i,:)+smul(scalevec(),vscaling(i)*sincomp(j)*sin(harme(j)/0.44056*vangle(i))+vscaling(i)*coscomp(j)*cos(harme(j)/0.44056*vangle(i)))
	    endif
		print ".";
	  else
	    print "-";
      endif
	  flush
    next i
    print "]"
  endif
next j

@rampen_plotten(10,50,620,280)
@rampen_komprimieren

if form_alert(2,"[2][Sollen die berechneten | Rampen wirklich | gesetzt werden ?][Ja| Nein ]")=1
  @vektoren_setzen
endif

quit



procedure rampen_komprimieren
  local i,j,t,ctr
  print "Rampen werden komprimiert..."
  @status("Rampen werden komprimiert...")

' Der Rampvektor eines Bump-Korrektors benoetigt die feinste Aufloesung...
'

' Die Rampe wird mit den Rampstart-Trigger gestartet. Um der ISR-Rountine
' auf der VAC1 Zeit zur Vorbereitung der neuen Rampe zu geben, wird die Rampe
' bereits mit der Injektionspraeperationszeit beendet.
'
 
  start=round(rampstart/deltat)+1
  stop=round(injprepstart/deltat)+1

' j = corrs(1);
  j=2

  hivec2(:,1)=hivec(:,1)
  vivec2(:,1)=vivec(:,1)
  t=0
  ctr=1

  for i=start to stop
	if (abs(hivec(j,i)-hivec(j,i-1))>0.0001) or t>65535 
		tvec2(t)=ctr
		hivec2(:,t)=hivec(:,i-1)
		vivec2(:,t)=vivec(:,i-1)
		ctr=1	
		inc t
	else					
	    ' gleicher Wert -> komprimieren
		inc ctr		
	endif
  next i


' letzten Wert setzen
' keine zyklische Rampe (wird durch Rampstart-Trigger getriggert)
'
  tvec2(t)=ctr
  tvec2(t+1)=0

  hivec2(:,t)=hivec(:,i-1)
  vivec2(:,t)=vivec(:,i-1)
  hivec2(:,t+1)=hivec(:,i-1)
  vivec2(:,t+1)=vivec(:,i-1)
  komprim_anz=t+2
  print "Rampe: "+str$(stop-start)

  print "Komprimiert von "+str$(clen-1)+" auf "+str$(komprim_anz)

  @complot(2,10,320,200)

return

procedure complot(bx,by,bw,bh)

  local i,j
  color schwarz
  pbox bx-2,by-2,bx+bw+2,by+bh+2
  defline ,1
  color weiss
  box bx,by,bx+bw,by+bh

  for i=0 to 31
    zwvec()=vivec2(i,:)
    color gelb+i
    scope zwvec(),0,-bh/3,by+bh/2
    VSYNC
  next i
return


procedure vektoren_setzen

  ' Rampen abbrechen
  csput "ELS_MAGNETE_CORRCONTROL.CMD_DC",10
  pause 2

  ' Lade Datensatz mit Nullstroemen
  csput "ELS_MAGNETE_CORRCONTROL.FILENAME_SC","zerokick"
  csput "ELS_MAGNETE_CORRCONTROL.CMD_DC",3
  pause 2

  ' Setze alle Korrketorstroeme skalar auf 0
  csput "ELS_MAGNETE_CORRCONTROL.CMD_DC",1
  pause 5

  ' Setze Timingvektor auf willkuerliche Vorinitialisierung (um Nichtverschicken
  ' des richtigen Vektors durch Kontrollsystem auszuschliessen)
  dim test(1)
  test(0)=20
  test(1)=0
  csvput "ELS_MAGNETE_CORRCONTROL.TIME_DC",test()
  pause 2


' Setze Stromwerte skalar nochmals einzeln auf 0


  for i=0 to 31
	if hscaling(i)<>0
	    hname$="ELS_MAGNETE_HC"+str$(i+1,2,2,1)+".STROM_AC"
	    csput hname$,0
	endif
	if vscaling(i)<>0
  	   vname$="ELS_MAGNETE_VC"+str$(i+1,2,2,1)+".STROM_AC"
	   csput vname$,0
	endif
  next i


' Vektoren setzen
'
  local i
  @status("Vektoren setzen...")
  print "Laenge der Vektoren: "+str$(komprim_anz)

' Abarbeitungsfrequenz auf 1/deltat Hz setzen
  csput "ELS_MAGNETE_CORRCONTROL.IRQFREQ_DC",1/deltat

  csvput "ELS_MAGNETE_CORRCONTROL.TIME_DC",tvec2(),komprim_anz

  for i=0 to 31
	if hscaling(i)<>0
	    hname$="ELS_MAGNETE_HC"+str$(i+1,2,2,1)+".STROM_AC"
	    csvput hname$,hivec2(i,:),komprim_anz
	endif
	if vscaling(i)<>0
  	   vname$="ELS_MAGNETE_VC"+str$(i+1,2,2,1)+".STROM_AC"
	   csvput vname$,vivec2(i,:),komprim_anz
	endif
  next i

  ' Berechnen der Rampe

  csput "ELS_MAGNETE_CORRCONTROL.CMD_DC",8
  pause 2
  
  ' Gebe das Rampen frei
  csput "ELS_MAGNETE_CORRCONTROL.CMD_DC",9
return

procedure rampen_plotten(bx,by,bw,bh)
' Rampen plotten
  local i,j
  color schwarz
  pbox bx-2,by-2,bx+bw+2,by+bh+2
  defline ,1
  color weiss
  box bx,by,bx+bw,by+bh
  dim zwvec(clen-1)

  for i=0 to 32-1
    zwvec()=vivec(i,:)
    color gelb+i
    scope zwvec(),tvec(),0,-bh/3,by+bh/2,bw/zyklusstop,bx
    VSYNC
  next i
return

procedure parameter_eingabe
  local bx,by,bw,bh,anzparm,akt,i

  anzparm=1+3*7
  dim s$(anzparm),t$(anzparm)  

  s$(0)="Korrektordatenfile:"
  t$(0)="pole_1200_990115" 
  akt=0
  for i=0 to 6    
    if (harme(i)>injenergy) and (harme(i)<extenergy)
      s$(1+3*akt)="Resonanz "+str$(i+1)+" [Gev]:"
      t$(1+3*akt)=str$(harme(i))
      s$(1+3*akt+1)="SIN: [mrad]"
      t$(1+3*akt+1)=str$(sincomp(i)) 
      s$(1+3*akt+2)="COS: [mrad]"
      t$(1+3*akt+2)=str$(coscomp(i)) 
      inc akt
    endif
  next i
  anzparm=1+3*akt  
  bx=100
  by=40
  bw=300
  bh=40+30+anzparm*20
  deftext 1,0.07,0.15,0
  defline ,2,2
  color weiss
  pbox bx-2,by-2,520+2,by+bh+2
  color schwarz
  box bx,by,520,by+bh
  color rot
  ltext bx+(bw-ltextlen("Harmonische Korrektur:"))/2,by+10,"Harmonische Korrektur:"
  color schwarz
  for i=0 to anzparm-1
    ltext bx+20,by+40+i*20,s$(i)
  next i
nochmal:
 akt=0
 ledit_curlen=25
 while akt<anzparm
   t$(akt)=@ledit$(t$(akt),bx+bw-100,by+40+akt*20)
   if ledit_status=1
     akt=max(0,akt-1)
   else if ledit_status=2
     akt=min(akt+1,anzparm-1)
   else if ledit_status=0
     akt=min(akt+1,anzparm)
   endif
 wend
 orbitdf$=t$(0)
 if not exist("/sgt/elsa/data/korrektoren/"+orbitdf$+".kicks")
   alert 3,"File nicht gefunden !|"+orbitdf$,1,"Nochmal|ABBRUCH",balert
   if balert=2
     quit
   else
     goto nochmal
   endif 
 endif
   akt=0
  for i=0 to 6    
    if (harme(i)>injenergy) and (harme(i)<extenergy)
      harme(i)=val(t$(1+akt*3))
  '   harmse(i)=harme(i)-0.1
  '   harmee(i)=harme(i)+0.1
      sincomp(i)=val(t$(1+akt*3+1))
      coscomp(i)=val(t$(1+akt*3+2))
      inc akt
    endif
  next i

 erase s$(),t$()
return

procedure read_orbit(fname$)
  '
  ' Globale Korrektur einlesen
  '
  local t$,i,a$,b$
  print "Orbit wird eingelesen..."
  if fname$=""
    input "Name des Korrekturdatensatzes = ",fname$
  endif
  filename$="/sgt/elsa/data/korrektoren/"+fname$+".kicks"
  if exist(filename$)
  OPEN "I",#1,filename$
  lineinput #1,t$
  print filename$+" [";
  while not eof(#1)
    lineinput #1,t$
	wort_sep t$," ",TRUE,a$,b$
	if a$="HZ"
	  wort_sep b$," ",TRUE,a$,b$
          i=val(a$)-1
	  wort_sep b$," ",TRUE,a$,b$
	  hkicks(i)=val(a$)
	  vkicks(i)=val(b$)
	' print i,val(a$),val(b$)
	
      print ".";
	endif
  wend
  close #1
  print "]"
  else
    alert 3,"File nicht gefunden !|"+filename$,1," OH ",balert
  endif
return

dim a(5000),t(5000)

t()=csvget("ELS_MAGNETE_DIPOL.TIMER_AC")
a()=csvget("ELS_MAGNETE_DIPOL.STROM_AC")
color 0
dim z(5000)
pbox 0,0,640,400
color 1
summe=0
i=0
repeat
  add summe,t(i)
  z(i)=summe
  plot i/10,200-summe/100
  plot i/10,300-a(i)/10
  inc i
until (summe+rampstart*1000)>=injprepstart*1000 or i>=5000
anzbins=i


color get_color(65535,0,0)
scope a(),z(),0,-0.2,300,640/summe,0
VSYNC
print summe



' Hilfsfunktion

function ledit$(t$,x,y)
  local t2$,cc,curpos,a,b,c$,curlen
  
  t2$=t$

  cc=len(t$)
  curpos=x+ltextlen(left$(t$,cc))
  curlen=max(20,ledit_curlen)

  do
    color weiss
    ltext x,y,t2$
    t2$=t$
    line curpos,y,curpos,y+curlen
    curpos=x+ltextlen(left$(t$,cc))
    color schwarz
    ltext x,y,t2$
    color rot
    line curpos,y,curpos,y+curlen
    VSYNC
    keyevent a,b,c$
    color weiss
    if b and -256
    '  print ".";
      b=b and 255
      if b=8    ! Bsp
        if len(t2$)
          t$=left$(t2$,cc-1)+right$(t2$,len(t2$)-cc)
          dec cc
        else
          bell
        endif
      else if b=13    ! Ret
        ledit_status=0
        line curpos,y,curpos,y+curlen
        return t2$
      else if b=82
        ledit_status=1
        line curpos,y,curpos,y+curlen
        return t2$
      else if b=84
        ledit_status=2 
        line curpos,y,curpos,y+curlen
        return t2$
      else if b=255   ! Del
        if cc<len(t2$)
          t$=left$(t2$,cc)+right$(t2$,len(t2$)-cc-1)
        endif
      else if b=81
        cc=max(0,cc-1)
      else if b=83
        cc=min(len(t2$),cc+1)
      endif
    else
      t$=left$(t2$,cc)+chr$(b)+right$(t2$,len(t2$)-cc)
      inc cc
    endif
  loop
endfunction

procedure plot_erampe(bx,by,bw,bh)
  '
  '
  ' Rampe mal plotten
  color schwarz
  pbox bx,by,bx+bw,by+bh

  color weiss
  box bx,by,bx+bw,by+bh
  scope evec(),tvec(),0,-bh/3.5,by+bh,bw/zyklusstop,bx
  
  VSYNC
return


procedure hscalerbar(scaler_x,scaler_y,scaler_w)
  local i,k

  color 0
  pbox scaler_x,scaler_y,scaler_x+scaler_w,scaler_y+20
  color 1 
  box scaler_x,scaler_y,scaler_x+scaler_w,scaler_y+20
  setfont "*Courier*10*"
  for i=0 to 100 step 5
    if (i mod 50)=0
      k=7
      text scaler_x+i/100*scaler_w-len(str$(i))*2.5,scaler_y+37,str$(i)
    else if (i mod 10)=0
      k=5
    else
      k=3
    endif
    line scaler_x+i/100*scaler_w,scaler_y+20,scaler_x+i/100*scaler_w,scaler_y+20+k
  next i
return
procedure do_hscaler(scaler_x,scaler_y,scaler_w,wert)
  color 0
  pbox scaler_x+1,scaler_y+1,scaler_x+scaler_w,scaler_y+20
  color gelb
  pbox scaler_x+1,scaler_y+1,scaler_x+1+(scaler_w-2)*wert,scaler_y+20
return

procedure status(t$)
  color schwarz
  pbox 320,0,640,30
  color rot
  setfont "*Helvetica*12*"
  text 330,20,t$
  vsync
return



resonanzdata:
' Imperfection Resonanzen
'    #, Energie [Mev], start, end bump, SIN, COS
data 1, 440.6,         440,    500,0,0
data 2, 881,           700,   1000,0,0
data 3,1322,          1200,   1450,0,0
data 4,1762,          1600,   1900,0,0
data 5,2203,          2050,   2300,0,0
data 6,2644,          2450,   2800,0,0
data 7,3084,          2950,   3200,0,0


