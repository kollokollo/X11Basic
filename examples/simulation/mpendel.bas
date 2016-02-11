' Demonstrationsprogramm (c) Markus Hoffmann
'
echo off
' s$=SPACE$(32256)
' Mathematisches Pendel
' L„nge in Meter
l=0.98
g=9.81        ! Gravitationskonstante
' D„mpfung (Geschwindigkeitsproportionalit„tsfaktor)
r=0.1
' Maximalausschlag in Bogenmaž  (-Pi bis Pi) (Pi heižt PI)
x=PI
' Anfangsgeschwindigkeit in m/s
x_=0
' Erregerfunktion
liy=50   
ll=140   ! L„nge des Bildschirmpendelmodells
gn=5    ! Integrationsgenauigkeit (fr grože Werte ruckartiger Bildaufbau)
my=220
'
rot=get_color(65530,0,0)
gelb=get_color(65530,40000,0)
grau=get_color(65530/2,65530/2,65530/2)
weiss=get_color(65530,65530,65530)
schwarz=get_color(0,0,0)
lila=get_color(65530,0,65530)
blau=get_color(10000,10000,65530)
gruen=get_color(0,30000,0)

color weiss
pbox 0,0,640,400
color schwarz
DIM xx(2),xx(2),yy(2),ee(2)
LINE 0,liy+6,640,liy+6
LINE 0,liy-1,640,liy-1
LINE @kx(-PI),liy+10,@kx(-PI),liy-5
LINE @kx(PI),liy+10,@kx(PI),liy-5
LINE @kx(0),liy+10,@kx(0),liy-5

FOR i=0 TO 18
  LINE @kx(RAD(i*10)),liy-1,@kx(RAD(i*10)),liy-4
  TEXT 1+@kx(RAD(i*10))-LEN(STR$(i))*6/2,liy-10,STR$(i)
  LINE @kx(RAD(-i*10)),liy-1,@kx(RAD(-i*10)),liy-4
  TEXT 1+@kx(RAD(-i*10))-LEN(STR$(i))*6/2,liy-10,STR$(i)
NEXT i
BOX 9,59,21,399
LINE 0,@ky(0),26,@ky(0)
FOR i=0 TO 10
  LINE 4,@ky(i),9,@ky(i)
  LINE 4,@ky(-i),9,@ky(-i)
  '  TEXT 1+@kx(RAD(i*10))-LEN(STR$(i))*6/2,liy-10,STR$(i)
  '  LINE @ky(RAD(-i*10)),liy-1,@kx(RAD(-i*10)),liy-4
  '  TEXT 1+@kx(RAD(-i*10))-LEN(STR$(i))*6/2,liy-10,STR$(i)
NEXT i
' CIRCLE 320,my,ll+10
DEFTEXT 1,0.07,0.14,0
color gruen
titel$="Angeregtes gedaempftes mathematisches Pendel. Echtzeitsimulation"
ltext 320-ltextlen(titel$)/2,7,titel$
dt=0.01
color schwarz
t=TIMER
v=1
x=0
DO
  MOUSE mx,fmy,k
  IF k=1*256
    x=(MOUSEX-320)/100
    x_=(MOUSEY-200)/10
  ELSE IF k=1024
     end
  ENDIF
  @m(x)
  color weiss
  LINE 0,5,200,5
  color schwarz
  dt=(TIMER-t)/gn
  LINE 0,5,dt*gn*500,5
  IF dt>0
    t=TIMER
    FOR i=1 TO gn
      @dgl2
      IF SGN(x_)<>SGN(v)
        um=t-ts
        umx=x
        um=2
        ts=t
        v=x_
      ENDIF
    NEXT i
    IF x>0
      x=((x+PI) MOD 2*PI)-PI
    ELSE
      x=-(((-x+PI) MOD 2*PI)-PI)
    ENDIF
  ENDIF
LOOP
end

PROCEDURE dgl2
  x__=-g/l*SIN(x)-r*x_+@err(t)/l   ! ged„mpftes mathematisches Pendel
  x_=x_+dt*x__           ! erste Ableitung
  x=x+x_*dt           ! Bewegungsgleichung
  t=t+dt
RETURN
PROCEDURE m(x)
  IF um
    PRINT AT(70,22);"T="'STR$(um,5)'
    PRINT AT(70,23);"E="'STR$(ABS((COS(umx))-1)*g*2*l,5)'
    DEC um
  ENDIF
  y=@ky(x_)
  e=@kx(@err(t))
  IF y<>yy(sc)
    color weiss
    PBOX 10,my,21,MAX(yy(sc),60)
    color blau
    PBOX 10,my,21,MAX(y,60)
    yy(sc)=y
    color schwarz
  ENDIF
  IF e<>ee(sc)
    color 1
    LINE ee(sc)-3,liy,ee(sc)-3,liy+5
    LINE ee(sc)+3,liy+5,ee(sc)+3,liy
    '
    color 0
    '
    LINE e-3,liy,e-3,liy+5
    LINE e+3,liy+5,e+3,liy
    '
    ee(sc)=e
  ENDIF
  IF x<>xx(sc)
    color 1
    LINE @kx(xx(sc))-3,liy,@kx(xx(sc))+3,liy+5
    LINE @kx(xx(sc))-3,liy+5,@kx(xx(sc))+3,liy
    '
    LINE 320,my,320+ll*SIN(xx(sc)),my+ll*COS(xx(sc))
    PCIRCLE 320+ll*SIN(xx(sc)),my+ll*COS(xx(sc)),8
    color 0
    '
    LINE @kx(x)-3,liy,@kx(x)+3,liy+5
    LINE @kx(x)-3,liy+5,@kx(x)+3,liy
    '
    LINE 320,my,320+ll*SIN(x),my+ll*COS(x)
    PCIRCLE 320+ll*SIN(x),my+ll*COS(x),7
    xx(sc)=x
  ENDIF
  sc=(sc+1) MOD 2
  s=TIMER
  vsync
RETURN

function err(t)
return 0.3*SIN(SQR(g/l)*t)

' DEFFN err(t)=(MOUSEX-320)/100   ! Hier wird mit der Maus erregt ...
' DEFFN err(t)=0
function kx(okkx)
return 320+100*okkx
function ky(y)
return my+10*y

