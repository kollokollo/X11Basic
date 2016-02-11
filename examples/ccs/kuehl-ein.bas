' Kuehlanlagen einschalten

echo off
sizew 1,500,200
color get_color(0,65535,0)
pbox 0,0,500,200
m$="Kuehlanlagen einschalten:|=========================||"
pause 1
alert 2,m$+"Fuer welchen Betriebsmodus soll|eingeschaltet werden ?",4,"PT-Betrieb|Syli|GDH/CB|ABBRUCH",balert
if balert=4
  quit
endif
color get_color(65535,65535,65535)
setfont "*-34-*"
text 10,100,"bitte warten..."
vsync

' Auf jeden Fall RKW4 einschalten

@pumpenfrage(4,2,"SUP_WASSER_DROSTURM4.PUMPE2_DC","SUP_WASSER_DROSTURM4.STATUS_DM")
@pumpenfrage(4,1,"SUP_WASSER_DROSSEL.PUMPE1_DC","SUP_WASSER_DROSSEL.STATUS_DM")

if balert=2 or balert=3
  ' RKW3 einschalten

  @pumpenfrage(3,7,"SUP_WASSER_ELSATURM3.PUMPE7_DC","SUP_WASSER_ELSATURM3.STATUS_DM")
  @pumpenfrage(3,8,"SUP_WASSER_ELSAMAG.PUMPE8_DC","SUP_WASSER_ELSAMAG.STATUS_DM")
  @pumpenfrage(3,5,"SUP_WASSER_ELSAHF.PUMPE5_DC","SUP_WASSER_ELSAHF.STATUS_DM")
endif

if  balert=3
  ' RKW2 einschalten

  csput "SUP_WASSER_EXPTURM2.PUMPE9_DC",1
  t=timer
  a=csget("SUP_WASSER_EXPTURM2.PUMPE9A_DM")
  b=csget("SUP_WASSER_EXPTURM2.PUMPE9B_DM")
  while a=0 and b=0
    pause 1
    if timer-t>30
      void form_alert(1,"[3][RKW2 Pumpe 9|will nicht angehen.][ STOP ]")
      quit
    endif
    a=csget("SUP_WASSER_EXPTURM2.PUMPE9A_DM")
    b=csget("SUP_WASSER_EXPTURM2.PUMPE9B_DM")
  wend

  csput "SUP_WASSER_EXPMD.PUMPE6_DC",1
  t=timer
  a=csget("SUP_WASSER_EXPMD.PUMPE6A_DM")
  b=csget("SUP_WASSER_EXPMD.PUMPE6B_DM")
  while a=0 and b=0
    pause 1
    if timer-t>30
      void form_alert(1,"[3][RKW2 Pumpe 6|will nicht angehen.][ OH ]")
    endif
    a=csget("SUP_WASSER_EXPTURM2.PUMPE9A_DM")
    b=csget("SUP_WASSER_EXPTURM2.PUMPE9B_DM")
  wend
endif
quit


procedure pumpenfrage(rkw,n,e1$,e3$)
  ' Testen, ob die Pumpe lauft und ggf. einschalten
  local a,b,t

  
  a=csget(e1$)

  if a=0
 '   if form_alert(1,"[3][Die Pumpe "+str$(n)+" ist nicht an.|Soll sie eingeschaltet werden ?][Ja|Nein]")=1
      csput e1$,1
      t=timer
      a=csget(e2$)
      b=csget(e3$)
      while a=0 and b=0
        pause 1
        if timer-t>30
          if form_alert(1,"[3][RKW"+str$(rkw)+" Pumpe "+str$(n)+"|will nicht angehen.][ STOP |WEITER]")=1
            quit
	      endif
        endif
        a=csget(e1$)
      wend
  '  endif
  else
    print "RKW"+str$(rkw)+" Pumpe "+str$(n)+" ist an."
  endif
return
