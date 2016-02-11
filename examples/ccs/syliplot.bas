ECHO OFF

' Testprogramm fuer Xbasic mit Kontrollsystemanbindung
' Markus Hoffmann 23.10.1998
' letzte Bearbeitung 25.3.1999   callbacks
'
'

' Es wird ein Fenster mit dem Lebensdauerplot (fuer Syli) geoeffnet.
' 


bx=0               ! Koordinaten des Plots
by=20
bw=640
bh=320

updatesek=5

' Farbdefinitionen
weiss=GET_COLOR(65535,65535,65535)
schwarz=GET_COLOR(0,0,0)
rot=get_color(65535,0,0)
orange=get_color(65535,65535/2,0)
blau=get_color(0,0,65535/4)
gelb=get_color(65535,65535,0)
grau=get_color(65535/2,65535/2,65535/2)
hellgrau=get_color(65535/3*2,65535/3*2,65535/3*2)

count=0
defline ,,2         ! Abgerundete Linienenden
DIM datax(2048),datay(2048)
anzdata=0
experiment$=csget$("SUP_GLOBAL_MESSAGE.EXPERIMENT_SC")
IF experiment$="GDH"
  strommax=3
  lebenmax=1
  updatesek=1
ELSE IF experiment$="Montage"
  LTEXT bx+bw/2-ltextlen(experiment$)/2,200,experiment$
  ALERT 2,"Montagezeit.|Wollen Sie wirklich|fortfahren ?",2,"Ja|Nein",balert
  IF balert=2
    QUIT
  ENDIF
ELSE IF experiment$="Syli"
  updatesek=8
  strommax=csget("ELS_SYLIMODE_INPUT.MAXCURR_AC")+5
  lebenmax=400
else
  updatesek=5
  strommax=100
  lebenmax=300
endif
x=csget("ELS_DIAG_TOROID.STROM_AM")
y=csget("ELS_DIAG_TOROID.LEBEN_AM")/60
unitx$=csunit$("ELS_DIAG_TOROID.STROM_AM")
unity$=csunit$("ELS_DIAG_TOROID.LEBEN_AM")
strommax=max(strommax,int(x+1))
lebenmax=max(lebenmax,round(y))

xmax=strommax
xmin=-strommax/20
xstep=10
ymax=lebenmax
ymin=-lebenmax/40
ystep=50
ey$="Lebensdauer"
ex$="Strom"

gosub draw_szene

@stepsize

color orange
defline ,4
deftext 1,0.2,0.3

ltext bx+120-ltextlen(experiment$)/2,360,experiment$
defline ,1

@paper
every updatesek,tuwas
' cssetcallback "CCS_TIMER_GLOBAL.ACTIVITY_DM",interrupt

t=timer
s=timer
clr x,y 
do
  x2=csget("ELS_DIAG_TOROID.STROM_AM")
  y2=csget("ELS_DIAG_TOROID.LEBEN_AM")/60
  if x2>strommax
    CSCLEARCALLBACKS
    PRINT "##Neustart: "+date$+" "+time$
    RUN
  ENDIF  
  MOUSEEVENT mouse_x,mouse_y,mouse_k  
  IF mouse_k=1
    IF @inbutton(570,370,"ENDE",mouse_x,mouse_y)
      @button(570,370,"ENDE",TRUE)
      VSYNC
      QUIT
    ELSE IF @inbutton(370,370,"Loeschen",mouse_x,mouse_y)
'	  CSCLEARCALLBACKS
	  EVERY 100,tunix
      DEFMOUSE 2
      @button(370,370,"Loeschen",TRUE)
      VSYNC
      @paper
      @button(370,370,"Loeschen",FALSE)
	  DEFMOUSE 0
      VSYNC
	  EVERY updatesek,tuwas
'	  cssetcallback "CCS_TIMER_GLOBAL.ACTIVITY_DM",interrupt
    else if @inbutton(270,370,"STOP",mouse_x,mouse_y) 
      @button(270,370,"STOP",TRUE)
	  CSCLEARCALLBACKS
      ALERT 0,"gestoppt.",1,"weiter",balert
	  every updatesek,tuwas
'	  CSSETCALLBACK "CCS_TIMER_GLOBAL.ACTIVITY_DM",interrupt
	  @button(270,370,"STOP",FALSE)
      VSYNC
    ELSE IF mouse_y<bh+by
      circle mouse_x,mouse_y,4
    ENDIF
  ENDIF
LOOP
QUIT
' #####################################################################
'
' Die Funktionen kx und ky transformieren auf Bildschirmkoordinaten
' Entsprechend den einstellungen des Koordinatensystems

function kx(dux)
  return bx+(dux-xmin)/(xmax-xmin)*bw
endfunc
function ky(duy)
  return by+bh-(duy-ymin)/(ymax-ymin)*bh
endfunc

procedure button(button_x,button_y,button_t$,sel)
  local x,y,w,h
  defline ,1
  deftext 1,0.05,0.1,0
  button_l=ltextlen(button_t$)

  x=button_x-button_l/2-10
  y=button_y-10
  w=button_l+20
  h=20
  color grau  
  pbox x+5,y+5,x+w+5,y+h+5
  color abs(sel)*schwarz+abs(not sel)*weiss
  pbox x,y,x+w,y+h
  if sel=-1
   color weiss
  else 
   color schwarz
  endif  
  box x,y,x+w,y+h
 
  ltext button_x-button_l/2,button_y-5,button_t$
RETURN

FUNCTION inbutton(button_x,button_y,button_t$,mx,my)
  local x,y,w,h
  
  deftext 1,0.05,0.1,0
  button_l=ltextlen(button_t$)

  x=button_x-button_l/2-10
  y=button_y-10
  w=button_l+20
  h=20
  if mx>=x and my>=y and  mx<=x+w and my<=y+h
    RETURN TRUE
  ELSE
    RETURN FALSE
  ENDIF
ENDFUNC

PROCEDURE draw_szene
  COLOR blau
  PBOX 0,0,640,400
 
  @button(270,370,"STOP",0)
  @button(570,370,"ENDE",0)
  @button(370,370,"Loeschen",0)
  @hscalerbar(430,360,100)
RETURN

PROCEDURE paper

  ' display graphics window with grid an axis
  ' Globale Vars: bx,by,bw,bh, grid, 
  '               xmin,xmax,ymin,ymax,xstep,ystep

  local textfaktor,x$,y$,x2$,y2$,void,x,y
  
  
  COLOR weiss
  pBOX bx,by,bx+bw,by+bh
  textfaktor=bw/10000
  void=0
  color schwarz
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
  COLOR schwarz
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
  COLOR schwarz
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

PROCEDURE do_hscaler(scaler_x,scaler_y,scaler_w,wert)
  COLOR schwarz
  PBOX scaler_x+1,scaler_y+1,scaler_x+scaler_w,scaler_y+20
  COLOR gelb
  PBOX scaler_x+1,scaler_y+1,scaler_x+1+(scaler_w-2)*wert,scaler_y+20
RETURN
PROCEDURE interrupt
  INC sekunde
  IF sekunde>updatesek
    sekunde=0
	@tuwas
  ENDIF
RETURN
PROCEDURE tunix
  ' nixtun
RETURN
PROCEDURE tuwas
 
  x2=csget("ELS_DIAG_TOROID.STROM_AM")
  y2=csget("ELS_DIAG_TOROID.LEBEN_AM")/60

  status$=csget$("SUP_GLOBAL_MESSAGE.MOTD_SC")
  COLOR schwarz
  DEFTEXT 1,0.05,0.1
  PBOX bx+bw/2-ltextlen(status$)/2,340, bx+bw/2+ltextlen(status$)/2,353
  PBOX 0,0,bx+bw,by-1
  COLOR weiss
  
  defline ,1
  ltext bx+bw/2-ltextlen(status$)/2,340,status$
  text 200,15,"Strom: "+str$(x2,4,4)+" "+unitx$+",  Lebensdauer: "+str$(y2,4,4)+" min."
  @do_hscaler(430,360,100,x2/strommax)  
  DEFLINE ,2
  COLOR rot
  IF ABS(x-x2)<strommax/8 AND ABS(y-y2)<lebenmax/8
    LINE @kx(x),@ky(y),@kx(x2),@ky(y2)
  ELSE
    t=TIMER
  ENDIF
  x=x2
  y=y2
  IF TIMER-t>300
    DEFLINE ,1
    COLOR blau
    LINE @kx(x),@ky(y)-3,@kx(x),@ky(y)+3
    IF count=2
      TEXT @kx(x),@ky(y)-3,STR$(ROUND((TIMER-s)/60))
      count=0
    ENDIF
    t=TIMER
    INC count
    DEFLINE ,2
  ENDIF
  VSYNC
RETURN
