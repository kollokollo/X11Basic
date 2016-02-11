' Programm for comunication with Garmin (etrex) GPS receiver
' (c) Markus Hoffmann 2002


'
' garmin    Version 1.03     1/2002    Markus Hoffmann
' 
'
DIM almanac$(32), waypoint$(600), track$(2000)

version$="V.1.04"
normfont$="-*-lucidatypewriter-*-r-*-*-12-*"
smallfont$="-*-helvetica-*-r-*-*-8-*"
setfont normfont$
anztrack=0
anzwp=0

sizew ,640,480
defmouse 2
@init
' Scaling fuer das Koordinatenprogramm  (defaults)
xxmin=-6.95
xxmax=-7.15
yymin=50.7
yymax=50.75

' Physikalische Einheit fuer die Achsen (defaults)
ex$="E"
ey$="N"
      
tname$="trackdat.track"

@window1

'########## Hauptprogramm ##########
'##########     begin     ##########

' initials
'OPEN "U",#1,"/dev/ttyS1:9600,N,8,1,DS,CS,RS,CD"
'OPEN "UX:9600,N,8,1,DS,CS,RS,CD",#1,"/dev/ttyS1",8+5+6*8
OPEN "UX:9600,N,8,1,DS,CS,RS,CD",#1,"/dev/ttyS1",8+5+6*8
OPEN "O",#5,tname$
color gelb
text 40*8,16*27,"Track: "+tname$


@qBOX(5, 315, 300, 403, rot)
vsync

' Garmin identifizieren
@sendmessage(254," ")
@procmessage(@getmessage$())
@procmessage(@getmessage$())


' Position request
@sendmessage(10,CHR$(2)+CHR$(0))
@procmessage(@getmessage$())
t$=@getmessage$()
@procmessage(t$)



 '  MENU
 '
  color weiss
  text 2*8,16*30,"F"
  text 9*8,16*30,"A"
  text 18*8,16*30,"D"
  text 24*8,16*30,"P"
  text 34*8,16*30,"R"
  text 41*8,16*30,"T"
  text 48*8,16*30,"C"
  text 55*8,16*30,"S"
  text 61*8,16*30,"ESC"
  text 9*8,16*30,"BSP"
  text 9*8,16*30,"Q"
  color gelb
  text 3*8,16*30,"ile, "
  text 10*8,16*30,"lmanac, "
  text 19*8,16*30,"ate, "
  text 25*8,16*30,"osition, "
  text 35*8,16*30,"oute, "
  text 42*8,16*30,"rack, "
  text 49*8,16*30,"lear, "
  text 56*8,16*30,"ave, "
  text 64*8,16*30," off, "
  text 2*8,16*30,"uit."
  BOX 0,16,639,479
  @showrange
jjj:

' PVT aktivieren
@sendmessage(10,CHR$(49)+CHR$(0))
MENUDEF menue$(),m
DEFMOUSE 0

DO
  MENU  
  pause 0.01
  mouse mx,my,mk
  IF mk=256
    COLOR schwarz
    CIRCLE @kx(omarx),@ky(omary),4
    LINE @kx(omarx)-5,@ky(omary),@kx(omarx)+5,@ky(omary)
    LINE @kx(omarx),@ky(omary)-5,@kx(omarx),@ky(omary)+5
    pbox 8*42,16*22,8*75,16*23
    marx=-@ox(mx)
    mary=@oy(my)
    COLOR gruen
    text 8*42,16*23,"Marker: y="+@breite$(marx)+" x="+@laenge$(mary)
    marx=-marx
    CIRCLE @kx(marx),@ky(mary),4
    LINE @kx(marx)-5,@ky(mary),@kx(marx)+5,@ky(mary)
    LINE @kx(marx),@ky(mary)-5,@kx(marx),@ky(mary)+5
    vsync
    omarx=marx
    omary=mary
  else if mk=2*256
    graphmode 3
    smx=mx
    smy=my
    repeat
      box smx,smy,mx,my
      vsync
       box smx,smy,mx,my
      mouse mx,my,mk
    until mk=0
    xmin=@ox(smx)
    ymin=@oy(my)
    xmax=@ox(mx)
    ymax=@oy(smy)
    graphmode 1
    @redraw
  else if mk=4*256
    xmin=xmin-(xmax-xmin)/2
    ymin=ymin-(ymax-ymin)/2
    xmax=xmax+(xmax-xmin)/3
    ymax=ymax+(ymax-ymin)/3
    @redraw
  ENDIF

  z$=INKEY$
  IF LEN(z$)<>0
    IF UPPER$(LEFT$(z$))="Q"
      @ende
    else IF UPPER$(LEFT$(z$))="C" 
      @paper
    else IF UPPER$(LEFT$(z$))="X"
      @range1
    else IF UPPER$(LEFT$(z$))=" "
      flag=0
      stable=0
      @sendmessage(10,CHR$(49)+CHR$(0))
    else IF UPPER$(LEFT$(z$))="S"
      @savewp
      @saveal      
    ELSE IF UPPER$(LEFT$(z$))="W"
      anzwp=0
      @sendmessage(10,CHR$(7)+CHR$(0))
    ELSE IF UPPER$(LEFT$(z$))="P"
      @sendmessage(10,CHR$(2)+CHR$(0))
    ELSE IF LEFT$(z$)=CHR$(27)
      @sendmessage(10,CHR$(0)+CHR$(0))
    ELSE IF LEFT$(z$)=CHR$(8) or LEFT$(z$)=CHR$(127)
      @sendmessage(10,CHR$(8)+CHR$(0))
    ELSE IF UPPER$(LEFT$(z$))="A"
      anzal=0
      @sendmessage(10,CHR$(1)+CHR$(0))
    ELSE IF UPPER$(LEFT$(z$))="N"
      @newwp
    ELSE IF UPPER$(LEFT$(z$))="R"
      @sendmessage(10,CHR$(4)+CHR$(0))
    ELSE IF UPPER$(LEFT$(z$))="I"
      @sendmessage(254," ")
    ELSE IF UPPER$(LEFT$(z$))="D"
      @sendmessage(10,CHR$(5)+CHR$(0))
    ELSE IF UPPER$(LEFT$(z$))="T"
      anztrack=0
      @sendmessage(10,CHR$(6)+CHR$(0))
    ELSE IF UPPER$(LEFT$(z$))="L"
      @loadwp
    ELSE
      @status(3,"Unbek. Taste: "+STR$(ASC(LEFT$(z$, 1))))
    ENDIF
    @showrange
  ENDIF
  IF TIMER-totzeit>5
    @sendmessage(10,CHR$(5)+CHR$(0))
    totzeit=TIMER
  ENDIF

  IF inp?(#1) 
    @status(2,"READ")
  WHILE inp?(#1) AND LEN(INKEY$)=0
    @procmessage(@getmessage$())
  WEND
  @status(2,"OK")
  totzeit=TIMER
  @showrange
  ENDIF
LOOP
@ende

PROCEDURE init
  LOCAL i,t$
  DIM menue$(80)
  '
  rot=get_color(65530,0,0)
  gelb=get_color(65530,40000,0)
  grau=get_color(65530/2,65530/2,65530/2)
  weiss=get_color(65530,65530,65530)
  schwarz=get_color(0,0,0)
  lila=get_color(65530,0,65530)
  blau=get_color(10000,10000,65530)
  gruen=get_color(0,65535,0)
  lila=get_color(65530,0,65530)
  bx=0
  by=16
  bw=640
  bh=384
  color schwarz
  pbox bx,by,640,480

  RESTORE menudata
  FOR i=0 TO 80
    read t$
    menue$(i)=t$
    EXIT IF menue$(i)="***"
  NEXT i
  menue$(i)=""
  menue$(i+1)=""
RETURN

PROCEDURE m(k)
  print k
  on k gosub info
  on k-10 gosub loadtrack,loadwp,13,loadal,15,16,17,18,19,20,ende
  on k-22 gosub 23,getal,getwp,gettrack,getroute,28,29,newwp
  on k-33 gosub garmincancel,n,garminoff,sh_x,sh_y,sh_x_,sh_y_,sh_x__,sh_y__
  on k-43 gosub sh_xy,n,n,n,koordin2,n,n,systemset,anfangsw
RETURN

PROCEDURE info
  ~form_alert(1,"[0][GARMIN.BAS |Kommuniziert mit einem GARMIN GPS|Empfänger, z.B. etrex|Angeschlossen ist:|"+garminmes$+"||(c) Markus Hoffmann][ OK ]")
RETURN

procedure getroute
  @sendmessage(10,CHR$(4)+CHR$(0))
return
procedure getal
  anzal=0
  @sendmessage(10,CHR$(1)+CHR$(0))
return
procedure getwp
  anzwp=0
  @sendmessage(10,CHR$(7)+CHR$(0))
return
procedure gettrack
  anztrack=0
  @sendmessage(10,CHR$(6)+CHR$(0))
return
procedure garmincancel
  @sendmessage(10,CHR$(0)+CHR$(0))
return
procedure garminoff
  @sendmessage(10,CHR$(8)+CHR$(0))
return

PROCEDURE boxtext (x, y, f, t$)
  COLOR gelb
  BOX x-2,y-2,x+130*f*LEN(t$)+2,y+130*f+2
  text x,y,t$
RETURN



PROCEDURE ende
  ' Quit the program, close all open files remove all installations
  CLOSE #1
  CLOSE #5
  quit
RETURN


FUNCTION getmessage$()
  t=TIMER
  s$=""
  flag=0
  flag2=0
  
  DO
    IF inp?(#1)
    t$=chr$(inp(#1))
    ' PRINT "Got:";ASC(t$)
    IF t$=CHR$(16)
      IF flag2=0
        flag2=1
      ELSE
        IF flag=0
          flag=1
        ELSE
          s$=s$+t$
          flag=0
        ENDIF
      ENDIF
    ELSE
      IF flag2=0
        PRINT "protokollfehler: muell ";ASC(t$)
      ELSE
      IF flag=1 AND t$=CHR$(3)
        GOTO t
      ELSE IF flag=1
        PRINT "Protokoll-ERROR 16"
        flag=0
      ENDIF
      s$=s$+t$
    ENDIF
    ENDIF
  ENDIF
 IF TIMER-t>4
   @status(1,"TIMEOUT")
   s$="TIMEOUT 0 ?"
    beep
      GOTO t
  ENDIF
LOOP
t:
  pid=ASC(MID$(s$,1,1))
  chk=ASC(MID$(s$,LEN(s$),1)) and 255
  chk2=0
  FOR i=1 TO LEN(s$)-1
    chk2=chk2-ASC(MID$(s$,i,1))
  NEXT i
  chk2=chk2 AND 255
  IF chk=chk2 AND pid<>6 AND pid<>21
    @sendACK(pid)
  ELSE IF chk<>chk2
    @status(1,"Uebertragungsfehler CHK")
    print "Uebertragungsfehler CHK"
  ENDIF
  return s$
ENDFUNCTION
function kx(dux)
  return bx+(dux-xmin)/(xmax-xmin)*bw
endfunc
function ky(duy)
  return by+bh-(duy-ymin)/(ymax-ymin)*bh
endfunc

FUNCTION ox(dux)
  return (dux-bx)*(xmax-xmin)/bw+xmin
ENDFUNCTION

FUNCTION oy(duy)
  return -(duy-by)*(ymax-ymin)/bh+ymax
ENDFUNCTION

PROCEDURE loadwp
  local dlen,f$

  fileselect "load waypoints ...","./*.dat","waypoint.dat",f$
  if len(f$)
    if exist(f$)
      if anzwp
        if form_alert(1,"[2][Daten an bestehende Daten anfügen|oder ersetzen ?][anfügen|ersetzen]")=2
          clr anzwp
	endif
      endif
      defmouse 2
      OPEN "I",#2,f$
    WHILE inp?(#2)
      dlen=inp(#2) and 255
      print "DELN=";dlen
      waypoint$(anzwp)=INPUT$(#2,dlen)
      print len(waypoint$(anzwp))
      inc anzwp
      print "EOF=";inp?(#2)
    WEND
    CLOSE #2
    @status(3,STR$(anzwp)+"WP geladen")
    @redraw
    defmouse 0
    else
      ~form_alert(1,"[3][Datei "+f$+"|existiert nicht !][ OH ]")
    endif
  endif
RETURN
PROCEDURE loadtrack
  local dlen,f$

  fileselect "load track ...","./*.track","my.track",f$
  if len(f$)
    if exist(f$)
      if anztrack
        if form_alert(1,"[2][Daten an bestehende Daten anfügen|oder ersetzen ?][anfügen|ersetzen]")=2
          clr anztrack
	endif
      endif
      defmouse 2
      OPEN "I",#2,f$
    WHILE inp?(#2)
      dlen=inp(#2) and 255
      track$(anztrack)=INPUT$(#2,dlen)
      inc anztrack
    WEND
    CLOSE #2
    @status(3,STR$(anztrack)+" TR geladen")
    @redraw
    defmouse 0
    else
      ~form_alert(1,"[3][Datei "+f$+"|existiert nicht !][ OH ]")
    endif
  endif
RETURN
PROCEDURE loadal
  local dlen,f$
  fileselect "load almanach ...","./*.dat","almanach.dat",f$
  if len(f$)
    if exist(f$)
      clr anzal
      defmouse 2
      OPEN "I",#2,f$
    WHILE inp?(#2)
      dlen=inp(#2) and 255
      almanac$(anzal)=INPUT$(#2,dlen)
      inc anzal
    WEND
    CLOSE #2
    @status(3,STR$(anzal)+" AL geladen")
    @showrange
    defmouse 0
    else
      ~form_alert(1,"[3][Datei "+f$+"|existiert nicht !][ OH ]")
    endif
  endif
RETURN

function breite$(x)
  local posx$
  IF x>0
    posx$="N"
  ELSE
    posx$="S"
  ENDIF
  x=ABS(x)
  posx$=posx$+RIGHT$("00"+str$(INT(x)),2)+":"
  x=x-INT(x)
  x=x*60
  posx$=posx$+RIGHT$("00"+str$(INT(x)),2)+":"
  x=x-INT(x)
  x=x*60
  posx$=posx$+RIGHT$("00"+str$(INT(x)),2)+"."
  x=x-INT(x)
  x=x*10
  posx$=posx$+str$(INT(x))
  return posx$
endfunc
function laenge$(x)
  local posx$
  IF x>0
    posx$="E"
  ELSE
    posx$="W"
  ENDIF
  x=ABS(x)
  posx$=posx$+RIGHT$("000"+str$(INT(x)),3)+":"
  x=x-INT(x)
  x=x*60
  posx$=posx$+RIGHT$("00"+str$(INT(x)),2)+":"
  x=x-INT(x)
  x=x*60
  posx$=posx$+RIGHT$("00"+str$(INT(x)),2)+"."
  x=x-INT(x)
  x=x*10
  posx$=posx$+str$(INT(x))
  return posx$
endfunc
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
  vsync
  keyevent a,b,c$
  color weiss
  if b and -256
    print ".";
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
  print a,"$"+hex$(b),b
  
loop
endfunction


PROCEDURE newwp
  local bg$
 ' get bg$,320,130,520,300
  name$="TEST"
  alt=0
  deftext 1,0.07,0.15,0  
  color weiss
  pbox 320,130,520,300
  color schwarz
  box 320,130,520,300
  
  ltext 330,140,"Wegpunkt ändern:"
  ltext 330,160,"Name:"
  ltext 330,180,"Breite:"
  ltext 330,200,"Länge:"
  ltext 330,220,"Höhe:"
  ltext 330,240,"Typ:"
  
 spaltencount=0
 ledit_curlen=25
 repeat
   if spaltencount=0
     name$=upper$(left$(@ledit$(name$,400,160),6))
   else if spaltencount=1
     posx$=upper$(@ledit$(posx$,400,180))
   else if spaltencount=2
     posy$=upper$(@ledit$(posy$,400,200))
   else if spaltencount=3
     alt=val(@ledit$(str$(alt,8,8),400,220))
   else if spaltencount=4
     styp=val(@ledit$(str$(styp),400,240))
   endif
   if ledit_status=2
     spaltencount=min(spaltencount+1,4)
   else if ledit_status=1
     spaltencount=max(spaltencount-1,0)
   endif
   if ledit_status=0 and spaltencount=0
  wpid=-1
  FOR i=0 TO anzwp-1
    IF name$+CHR$(0)=MID$(waypoint$(i),49,LEN(name$)+1)
      wpid=i
    ENDIF
  next i
  IF wpid=-1
    PRINT " ist neu !"
    x=50
    y=7
    styp=150
    alt=0
  ELSE
    styp=ASC(MID$(waypoint$(wpid),5,1))
    alt=CVS(MID$(waypoint$(wpid),33,4))
    x=180/2^31*CVL(MID$(waypoint$(wpid),25,4))
    y=180/2^31*CVL(MID$(waypoint$(wpid),29,4))
  ENDIF
  posx$=@breite$(x)
  posy$=@laenge$(y)
  color rot
  ltext 400,180,posx$
  ltext 400,200,posy$
  ltext 400,220,str$(alt,8,8)+" m"
  ltext 400,240,str$(styp)

   endif
 until ledit_status=0 and spaltencount>0
nochmal:
  
  posx$=UPPER$(edit$(r+3, c+9, posx$))
  posy$=UPPER$(edit$(r+4, c+9, posy$))
  IF LEN(posx$) <> 11 OR LEN(posy$) <> 12 THEN GOTO nochmal
  x=VAL(MID$(posx$, 2, 2))+VAL(MID$(posx$, 5, 2)) / 60+VAL(MID$(posx$, 8, 4)) / 60 / 60
  y=VAL(MID$(posy$, 2, 3))+VAL(MID$(posy$, 6, 2)) / 60+VAL(MID$(posy$, 9, 4)) / 60 / 60
  IF UPPER$(LEFT$(posx$, 1))="S" THEN x=-x
  IF UPPER$(LEFT$(posy$, 1))="W" THEN y=-y

  alt=VAL(edit$(r+5,c+9,STR$(alt)))
  styp=VAL(edit$(r+6,c+9,STR$(styp)))
  x=x*2^31/180
  y=y*2^31/180
  dtyp=0
  depth=1E+25
  dist=0
  comment$=""
  facility$=""
  city$=""
  addr$=""
  cr$=""
  t$=CHR$(0)+CHR$(255)+CHR$(dtyp)+CHR$(6 * 16)+CHR$(styp)+CHR$(0)
  t$=t$+CHR$(0)+CHR$(0)+MKL$(0)+MKL$(-1)+MKL$(-1)+MKL$(-1)
  t$=t$+MKL$(x)+MKL$(y)+MKS$(alt)+MKS$(depth)
  t$=t$+MKS$(dist)+"D MH"+name$+CHR$(0)
  t$=t$+comment$+CHR$(0)
  t$=t$+facility$+CHR$(0)+city$+CHR$(0)
  t$=t$+addr$+CHR$(0)+cr$+CHR$(0)
  INPUT "Wirklich aendern ?", a$
  IF UPPER$(LEFT$(a$,1))="J"
    @sendmessage(27,CHR$(1)+CHR$(0))
    @sendmessage(35,t$)
    @sendmessage(12,CHR$(7)+CHR$(0))
  ENDIF
RETURN


PROCEDURE procalmanacdata(t$)
  inc plen
  plen=ASC(MID$(t$,2,1))
  almanac$(anzal)=MID$(t$,3,plen)
  inc anzal
RETURN

PROCEDURE procmessage(t$)
  local pid,plen
  pid=ASC(LEFT$(t$,1))
  plen=ASC(MID$(t$,2,1))
  IF pid=6
   ' ACK, alles OK, ignorieren
    @status(1,"OK")
  ELSE IF pid=12
    @procxfercmplt(t$)
  ELSE IF pid=20 OR pid=14
    @proctimedata(t$)
  ELSE IF pid=21
    @status(1,"NAK !")
  ELSE IF pid=24 OR pid=17
    @procpositiondata(t$)
  ELSE IF pid=27
    @procrecords(t$)
  ELSE IF pid=29
    @procroutename(t$)
  ELSE IF pid=31
    @procalmanacdata(t$)
  ELSE IF pid=34
    @proctrackdata(t$)
  ELSE IF pid=35
    @procwpdata(t$)
  ELSE IF pid=99
    @proctrackname(t$)
  ELSE IF pid=-3
    @procprotocollarray(t$)
  ELSE IF pid=-2 OR pid=10
    PRINT "Paket nicht erlaubt ! Pid="; pid
  ELSE IF pid=-1
    @procproductdata(t$)
  ELSE
    @status(2,"? Paket pid="+STR$(pid)+" !")
  ENDIF
RETURN

PROCEDURE procpositiondata(pt$)
  COLOR schwarz
  CIRCLE @kx(oposx),@ky(oposy),4
  LINE @kx(oposx)-5,@ky(oposy),@kx(oposx)+5,@ky(oposy)
  LINE @kx(oposx),@ky(oposy)-5,@kx(oposx),@ky(oposy)+5
  pbox 8*42,16*23,8*75,16*24
  posy=CVD(MID$(pt$,3,8))*180/pi
  posx=CVD(MID$(pt$,11,8))*180/pi
  COLOR gelb
  text 8*42,16*24,"Position: y="+@breite$(posy)+" x="+@laenge$(posx)
  posx=-posx
  CIRCLE @kx(posx),@ky(posy),4
  LINE @kx(posx)-5,@ky(posy),@kx(posx)+5,@ky(posy)
  LINE @kx(posx),@ky(posy)-5,@kx(posx),@ky(posy)+5
  vsync
RETURN

PROCEDURE procproductdata(ut$)
  garminPID=CVI(MID$(ut$,3,2))
  garminVER=CVI(MID$(ut$,5,2))
  garminMES$=MID$(t$,7,LEN(ut$)-7-1)
  @status(5,garminMES$)
RETURN

PROCEDURE procprotocollarray (t$)
  local plen,anz
  plen=ASC(MID$(t$,2,1))
  anz=plen/3
  PRINT "Es werden folgende ";anz;" Protokolle unterstuetzt:"
  FOR i=0 TO anz-1
    PRINT MID$(t$,3+i*3,1);CVI(MID$(t$,3+i*3+1,2));" ";
  NEXT i
RETURN

PROCEDURE procrecords(t$)
  records=CVI(MID$(t$,3,2))
  @status(3,"load "+STR$(records)+"...")
RETURN

PROCEDURE procroutename(t$)
  color schwarz
  pbox 8*40,16*27,8*75,16*28
  COLOR gelb
  text 8*40,16*28,"Route: "+MID$(t$,3,ASC(MID$(t$,2,1))-1)+"  "
  vsync
RETURN

PROCEDURE proctimedata(t$)
  local month,day,year,hour,minute,second
  month=ASC(MID$(t$,3,1))
    day=ASC(MID$(t$,4,1))
   year=CVI(MID$(t$,5,2))
   hour=CVI(MID$(t$,7,2))
 minute=ASC(MID$(t$,9,1))
 second=ASC(MID$(t$,10,1))
 ndate$=STR$(day,2,2)+"."+STR$(month,2,2)+"."+STR$(year,4,4)
 ntime$=str$(hour,2,2)+":"+str$(minute,2,2)+":"+str$(second,2,2)
  color schwarz
  pbox 8*42,16*24,8*75,16*25
  COLOR gelb
  text 42*8,16*25,"ZEIT: "+ndate$+" "+ntime$
  vsync
RETURN

PROCEDURE proctrackdata(t$)
  local plen,x,y,s$,date,alt,depth,new
  plen=ASC(MID$(t$,2,1))
   track$(anztrack)=MID$(t$, 3, plen)
  inc anztrack
  y=180/2^31*CVL(MID$(t$,3,4))
  x=-180/2^31*CVL(MID$(t$,7,4))
  s$=CHR$(plen)+MID$(t$,3,plen)
  bput #5,varptr(s$),len(s$)
  date=CVL(MID$(t$,11,4))
  alt=CVS(MID$(t$,15,4))
  depth=CVS(MID$(t$,19,4))
  new=asc(MID$(t$,23,1))
  if new
    color gruen
    pCIRCLE @kx(x),@ky(y),2
  else
    color gruen
    CIRCLE @kx(x),@ky(y),1
  endif
  if alt>100
    color rot
  else
    color blau
  endif
  if new
    plot @kx(x),@ky(y)
  else
    line @kx(otx),@ky(oty),@kx(x),@ky(y)
  endif
  otx=x
  oty=y
  
RETURN

PROCEDURE proctrackname(t$)
  local dlen,disp,col
  dlen=ASC(MID$(t$,2,1))
  disp=ASC(MID$(t$,3,1))
  col=ASC(MID$(t$,4,1))
  tname$=MID$(t$,5,dlen-3)+".track"
  CLOSE #5
  OPEN "O",#5,tname$
  color schwarz
  pbox 8*40,16*26,75*40,16*27
  color gelb
  text 8*40,16*27,"Track: "+MID$(t$,5,dlen-3)+"   "
RETURN

procedure redraw
  local i
  defmouse 2
  vsync
  color schwarz
  pbox bx,by,bx+bw,by+bh
  if anzwp
    for i=0 to anzwp-1
      @procwpdata(" "+chr$(len(waypoint$(i)))+waypoint$(i))
      dec anzwp
    next i
  endif
  if anztrack
    for i=0 to anztrack-1
      @proctrackdata(" "+chr$(len(track$(i)))+track$(i))
      dec anztrack
    next i
  endif
  defmouse 0
  vsync
return

PROCEDURE procwpdata(t$)
  local plen,x,y
  setfont smallfont$
  plen=ASC(MID$(t$,2,1))
  waypoint$(anzwp)=MID$(t$,3,plen)
  inc anzwp
  y=180/2^31*CVL(MID$(t$,27,4))
  x=-180/2^31*CVL(MID$(t$,31,4))
  color grau
  text @kx(x)+5,@ky(y)+10,MID$(t$,51,6))
  color gelb
  CIRCLE @kx(x),@ky(y),3
  setfont normfont$
RETURN

PROCEDURE procxfercmplt(t$)
  @status(3,STR$(records)+" geladen (typ="+STR$(ASC(MID$(t$, 3, 1)))+")")
RETURN

PROCEDURE qBOX(x1,y1,x2,y2,c)
  COLOR schwarz 
  pbox x1,y1,x2,y2
  COLOR c 
  BOX x1,y1,x2,y2
  BOX x1+1,y1+1,x2-1,y2-1
  BOX x1+3,y1+3,x2-3,y2-3
RETURN


PROCEDURE saveal
IF anzal
  OPEN "O",#2,"almanac.dat"
    FOR i=0 TO anzal-1
      PRINT #2,CHR$(LEN(almanac$(i)));almanac$(i);
    NEXT i
    CLOSE #2
  ENDIF
RETURN


PROCEDURE savewp
  IF anzwp
    OPEN "O",#2,"waypoint.dat"
    FOR i=0 TO anzwp-1
      PRINT #2,CHR$(LEN(waypoint$(i)))+waypoint$(i);
    NEXT i
    CLOSE #2
  ENDIF
RETURN

procedure sendACK(pid)
  @sendmessage(6,CHR$(pid))
return

procedure sendmessage(id,m$)
  local i,a,chk,s$
  
  chk=0-id-LEN(m$)
  s$=CHR$(16)+CHR$(id)+CHR$(LEN(m$))
  IF LEN(m$)=16
    s$=s$+CHR$(16)
  ENDIF
  IF LEN(m$)
    FOR i=1 TO LEN(m$)
      a=ASC(MID$(m$,i,1)) and 255
      s$=s$+CHR$(a)
      IF a=16
        s$=s$+CHR$(16)
      ENDIF
      chk=chk-a
    NEXT i
  ENDIF
  chk=chk AND 255
  s$=s$+CHR$(chk)
  IF chk=16
    s$=s$+CHR$(16)
  ENDIF
  s$=s$+chr$(16)+chr$(3)
  BPUT #1,varptr(s$),len(s$)
  flush #1
return

procedure sendnack
  @sendmessage(21,"")
return

procedure showrange
  color schwarz
  pbox 0,26*16,30*8,29*16
  if anzal>31
    color gelb
  else
    color grau
  endif
  text 2*8,27*16,"ALMANAC"
  if anzwp>0
    color gelb
  else
    color grau
  endif
  text 2*8,28*16,"#WP: "+str$(anzwp,3,3)
  if anztrack>0
    color gelb
  else
    color grau
  endif
  text 2*8,29*16,"#TR: "+str$(anztrack,4,4)
  COLOR blau
  text 4*8,23*16,"ACTION:"
  text 4*8,24*16,"STATUS:"
  text 4*8,25*16,"RESULT:"
  vsync
return

procedure status(n,ssst$)
  color schwarz
  pbox 12*8,16*(25+1-n),(12+25)*8,16*(25-n)
  color weiss
  text 12*8,16*(25+1-n),LEFT$(ssst$+"                          ",25)
  vsync
return



procedure window1
  ' Bildschirmfenster fuer die Grafik
  bx=5
  by=20
  bw=630
  bh=270
  xmin=xxmin
  ymin=yymin
  xmax=xxmax
  ymax=yymax
  @stepsize
return

procedure window2
  ' 2.Bildschirmfenster fuer die Grafik
  bx=310
  by=300
  bw=320
  bh=170
  xmin=xxmin2
  xmax=xxmax2
  ymin=yymin2
  ymax=yymax2
  @stepsize
return
PROCEDURE paper

  ' display graphics window with grid an axis
  ' Globale Vars: bx,by,bw,bh, grid, 
  '               xmin,xmax,ymin,ymax,xstep,ystep

  local textfaktor,x$,y$,x2$,y2$,void,x,y
  
  
  COLOR schwarz
  pBOX bx,by,bx+bw,by+bh
  textfaktor=bw/10000
  void=0
  color grau
  BOX bx,by,bx+bw,by+bh

  FOR x=0 TO MAX(ABS(xmin),ABS(xmax)) STEP xstep
    COLOR hellgrau

   
    IF x>xmin AND x<xmax
      LINE @kx(x),@ky(ymin),@kx(x),@ky(ymax)
    ENDIF
    IF -x>xmin AND -x<xmax
      LINE @kx(-x),@ky(ymin),@kx(-x),@ky(ymax)
    ENDIF
    COLOR schwarz
    IF x>xmin AND x<xmax
      LINE @kx(x),@ky(0)+2,@kx(x),@ky(0)-2
    ENDIF
    IF -x>xmin AND -x<xmax 
      LINE @kx(-x),@ky(0)+2,@kx(-x),@ky(0)-2
    ENDIF
    x$=TRIM$(STR$(x))
    x2$=TRIM$(STR$(-x))
    IF @kx(x)-0.06*LEN(x$)*130/2>void+2
      color schwarz
      deftext 0,textfaktor,textfaktor,0
      ltext @kx(x)-textfaktor*LEN(x$)*130/2,@ky(0)+textfaktor*50,x$
      void=ltextlen(x$)
      ltext @kx(-x)-textfaktor*LEN(x2$)*130/2,@ky(0)+textfaktor*50,x2$
    ENDIF
  NEXT x
  
  FOR y=0 TO MAX(ABS(ymin),ABS(ymax)) STEP ystep
    COLOR hellgrau
    IF y>ymin AND y<ymax
      LINE @kx(xmin),@ky(y),@kx(xmax),@ky(y)
    ENDIF
    IF -y>ymin AND -y<ymax
      LINE @kx(xmin),@ky(-y),@kx(xmax),@ky(-y)
    ENDIF
    COLOR schwarz
    IF y>ymin AND y<ymax 
      LINE @kx(0)-2,@ky(y),@kx(0)+2,@ky(y)
    ENDIF
    IF -y>ymin AND -y<ymax
      LINE @kx(0)-2,@ky(-y),@kx(0)+2,@ky(-y)
    ENDIF
    y$=TRIM$(STR$(y))
    y2$=TRIM$(STR$(-y))
    DEFLINE ,1
    deftext 0,textfaktor,textfaktor,0
    IF y>ymin AND y<ymax 
      ltext @kx(0)-textfaktor*130*LEN(y$)-2,@ky(y)-textfaktor*50,y$
    ENDIF
    IF -y>ymin AND -y<ymax
      ltext @kx(0)-textfaktor*130*LEN(y2$)-2,@ky(-y)-textfaktor*50,y2$
    ENDIF
  NEXT y
  
  ' Koordinatenachsen
  COLOR weiss
  IF SGN(xmin)<>SGN(xmax)
    LINE @kx(0),@ky(ymin),@kx(0),@ky(ymax)
  ENDIF
  IF SGN(ymin)<>SGN(ymax) 
    LINE @kx(xmin),@ky(0),@kx(xmax),@ky(0)
  ENDIF
  FOR i=-3 TO 3                
    ' Pfeile
    IF SGN(xmin)<>SGN(xmax)
      LINE @kx(0)+i,@ky(ymax)+10,@kx(0),@ky(ymax)
    ENDIF
    IF SGN(ymin)<>SGN(ymax)
      LINE @kx(xmax)-10,@ky(0)+i,@kx(xmax),@ky(0)
    ENDIF
  NEXT i
  ' Beschriftung
  COLOR weiss
  deftext 1,textfaktor*2,textfaktor*3,0
  ltext bx+bw-textfaktor*2*130*LEN(ex$),@ky(0)-textfaktor*2*160,ex$
  ltext @kx(0)+textfaktor*2*130,by+130*textfaktor*1.5,ey$
  deftext ,textfaktor,textfaktor,-90
  color gelb
  ltext bx+bw-textfaktor*130,by+bh-130*textfaktor,"(C) MARKUS HOFFMANN 9'1995"
  VSYNC
  DEFTEXT ,,,0
RETURN

PROCEDURE stepsize
  ' calculate stepsize from range-data

  local i

  xstep=(xmax-xmin)/10
  ystep=(ymax-ymin)/8
  FOR i=-13 TO 13
    IF xstep>=8*10^i AND xstep<2*10^(i+1)
      xstep=10^(i+1)
	else IF xstep>=2*10^(i+1) AND xstep<5*10^(i+1)
	  xstep=2*10^(i+1)
    else IF xstep>=5*10^(i+1) AND xstep<8*10^(i+1)
	  xstep=5*10^(i+1)
    ENDIF
    IF ystep>=8*10^i AND ystep<2*10^(i+1) 
      ystep=10^(i+1)
	else IF ystep>=2*10^(i+1) AND ystep<5*10^(i+1)
	  ystep=2*10^(i+1)
	else IF ystep>=5*10^(i+1) AND ystep<8*10^(i+1)
	  ystep=5*10^(i+1)
    ENDIF
  NEXT i
RETURN

PROCEDURE hscalerbar(scaler_x,scaler_y,scaler_w)
  LOCAL i,k

  COLOR schwarz
  PBOX scaler_x,scaler_y,scaler_x+scaler_w,scaler_y+20
  COLOR weiss
  BOX scaler_x,scaler_y,scaler_x+scaler_w,scaler_y+20

  FOR i=0 TO 100 STEP 5
    IF (i MOD 50)=0
      k=7
      TEXT scaler_x+i/100*scaler_w-len(str$(i))*2.5,scaler_y+37,str$(i)
    eLSE IF (i MOD 10)=0
      k=5
    ELSE
      k=3
    ENDIF
    LINE scaler_x+i/100*scaler_w,scaler_y+20,scaler_x+i/100*scaler_w,scaler_y+20+k
  NEXT i
RETURN

procedure makrefig

open "O",#1,figfilename$
print #1,"#FIG 3.2"
print #1,"Portrait"
print #1,"Center"
print #1,"Metric"
print #1,"A4"
print #1,"100.00"
print #1,"Single"
print #1,"-2"
print #1,"1200 2"

@ps_dicke(1)
@ps_font(14)
@ps_color(0)
@ps_tmode(0)
@ps_box(10,10,120+10,120+10)
@ps_box(10,140,150+10,118+140)
@ps_color(0)
@ps_dotline(10+7,140,7+10,118+140)
@ps_dotline(10+143,140,143+10,118+140)
@ps_groesse(14)
@ps_angle(90)
@ps_font(1)
@ps_text(10+5,140+115,autor$(0)+": "+label$(0))
@ps_angle(270)
@ps_text(10+145,140+3,autor$(0)+": "+label$(0))
@ps_angle(0)
@ps_text(10+9,140+8,autor$(0)+": ")
@ps_font(2)
@ps_tmode(1)
@ps_groesse(18)
@ps_text(10+75,140+15,label$(0))
@ps_font(14)
@ps_groesse(14)
@ps_tmode(0)
abstand=5
if anztitel>21
  abstand=3
  @ps_groesse(13)
else if anztitel>15
  abstand=4
endif
for i=0 to anztitel-1
  @ps_text(23,165+abstand*i,dat$(i))
next i
@ps_text(23,165+abstand*i,space$(35+4*various)+"========")
inc i
@ps_text(23,165+abstand*i,space$(21+4*various)+"Gesamtlänge:  ("+str$(gm)+"'"+str$(gs,2,2,1)+")")


@ps_tmode(1)
@ps_groesse(32)
@ps_font(2)
@ps_text(10+60,10+60,label$(0))
@ps_tmode(1)
@ps_groesse(22)
@ps_font(1)
if various
  @ps_text(10+60,10+100-8/2,"")
else
if anzautor
  for i=0 to anzautor-1
    @ps_text(10+60,10+100+i*8-anzautor*8/2,autor$(i))
  next i
endif
endif
@ps_font(10)
@ps_tmode(0)
@ps_groesse(8)
@ps_text(80,140+116,"mp3Cover V."+version$+" (c) "+date$)
close #1
' system "fig2dev -L ps -z A4 -c "+figfilename$+" "+psfilename$
' system "rm "+figfilename$
' system "chmod 666 "+psfilename$


return


PROCEDURE do_hscaler(scaler_x,scaler_y,scaler_w,wert)
  COLOR schwarz
  PBOX scaler_x+1,scaler_y+1,scaler_x+scaler_w,scaler_y+20
  COLOR gelb
  PBOX scaler_x+1,scaler_y+1,scaler_x+1+(scaler_w-2)*wert,scaler_y+20
RETURN
procedure ps_line(x1,y1,x2,y2)
  @mmm
  print #1,"2 1 0 "+str$(dicken)+" "+str$(colorn)+" 7 0 0 -1 0.000 0 0 -1 0 0 2"
  print #1," "+str$(x1)+" "+str$(y1)+" "+str$(x2)+" "+str$(y2)
return
procedure ps_dotline(x1,y1,x2,y2)
  @mmm
  print #1,"2 1 2 "+str$(dicken)+" "+str$(colorn)+" 7 0 0 -1 3.000 0 0 -1 0 0 2"
  print #1," "+str$(x1)+" "+str$(y1)+" "+str$(x2)+" "+str$(y2)
return
procedure ps_box(x1,y1,x2,y2)
  @mmm
  print #1,"2 1 0 "+str$(dicken)+" "+str$(colorn)+" 7 0 0 -1 0.000 0 0 -1 0 0 5"
  print #1," "+str$(x1)+" "+str$(y1)+" "+str$(x2)+" "+str$(y1);
  print #1," "+str$(x2)+" "+str$(y2)+" "+str$(x1)+" "+str$(y2);
  print #1," "+str$(x1)+" "+str$(y1)
return
procedure ps_pbox(x1,y1,x2,y2)
  @mmm
  print #1,"2 1 0 "+str$(dicken)+" "+str$(colorn)+" 7 0 0 20 0.000 0 0 -1 0 0 5"
  print #1," "+str$(x1)+" "+str$(y1)+" "+str$(x2)+" "+str$(y1);
  print #1," "+str$(x2)+" "+str$(y2)+" "+str$(x1)+" "+str$(y2);
  print #1," "+str$(x1)+" "+str$(y1)
return
procedure ps_text(x1,y1,t$)
  @mmm  
  print #1,"4 "+str$(tmoden)+" 0 100 0 "+str$(fontn)+" "+str$(groessen)+" "+str$(anglen,6,6)+" 4 165 4830 "+str$(x1)+" "+str$(y1)+" "+t$+"\001"
return
procedure mmm
  mul x1,45
  mul y1,45
  mul x2,45
  mul y2,45
return

procedure ps_color(c)
  colorn=c
return
procedure ps_pattern(c)
  patternn=c
return
procedure ps_dicke(c)
  dicken=c
return
procedure ps_angle(c)
  anglen=c/180*pi
return
procedure ps_groesse(c)
  groessen=c
return
procedure ps_tmode(c)
  tmoden=c
return
procedure ps_font(c)
  fontn=c
return


menudata:
DATA "GARMIN","  Info","--------------------","- 1","- 2","- 3","- 4","- 5","- 6",""
DATA "Datei","  Track laden ...","  Wegpunkte laden ..."
data "  Route laden ...","  Almanac laden ..."
data "-----------------------","- Karte laden ...","  Screen löschen ","-----------------------","  Datei löschen ...","-----------------------","  Quit ",""
DATA "Transfer","  Almanac holen","  Wegpunkte holen","  Tracks holen","  Routen holen"
DATA "------------------------------"
DATA "  Almanac senden","  Wegpunkt senden","  Track senden","  Route senden"
DATA "------------------------------"
DATA "  Transfer unterbrechen   ESC"
DATA "------------------------------"
DATA "  Empfänger ausschalten   BSP",""
DATA "Darstellung","  *.fig erstellen","  Y'(t)","  X''(t)","  Y''(t)","------------------------","  (X,Y)","  Kraftfeld (X,Y)","  Satelliten aussetzten","------------------------","  Koordinaten setzten",""
DATA "***"

