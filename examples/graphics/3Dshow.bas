DIM pxU(4),pyU(4)
meldung$="WSHOW (c) Markus Hoffmann 1989 - 2001"
maxfl=20000
sortstart=0
clr anzfl
dim welt(3+13*maxfl),welt(4+13*maxfl),welt(5+13*maxfl)
dim welt(6+13*maxfl),welt(7+13*maxfl),welt(8+13*maxfl)
dim welt(9+13*maxfl),welt(10+13*maxfl),welt(11+13*maxfl)
dim welt(maxfl*13)
'
'
bxU=0
byU=18
bwU=640
bhU=400-18
' CLIP bxU,byU,bwU,bhU
' Koordinaten:
'
bwU=bhU
x1=-1
x2=1
y1=-1
y2=1
z1=-1
z2=1
'
sx=0.02
sy=0.02
sz=0.1
'
'
ebene=0    ! Projektionsebene (y-Koordinate)
prozy=-4  ! Projektionszentrum
prozx=0
prozz=0
'
stx=0.8         ! Beobachterstandpunkt
sty=-0.9
stz=0.8
'
bbx=-1         ! Beobachterblickrichtung
bby=1
bbz=-1
'
nwink=0       ! Neigungswinkel
zwink=SGN(bby)*SGN(bbx)*ATN(ABS(bbx/bby))+PI*ABS(bby<0)
xwink=-SGN(bbz)*ATN(ABS(bbz/SQR(bbz^2+bby^2)))
'
' Lichteinfallrichtung
lichtx=0
lichty=-0.8
lichtz=sqr(1-lichty^2)
' Lichtfarbe
lichtr=50000
lichtg=65535
lichtb=65535
'


if not exist("welt.xxx")
  @calc
  @save
endif
@load
@sort
@save
@plot
quit
procedure addfl(x1,y1,z1,x2,y2,z2,x3,y3,z3,x4,y4,z4)
  welt(0+13*anzfl)=x1
  welt(3+13*anzfl)=x2
  welt(6+13*anzfl)=x3
  welt(9+13*anzfl)=x4
  welt(1+13*anzfl)=y1
  welt(4+13*anzfl)=y2
  welt(7+13*anzfl)=y3
  welt(10+13*anzfl)=y4
  welt(2+13*anzfl)=z1
  welt(5+13*anzfl)=z2
  welt(8+13*anzfl)=z3
  welt(11+13*anzfl)=z4
  inc anzfl
return
PROCEDURE polar1(r,th,ph)
  x1=r*COS(ph)*SIN(th)
  y1=r*SIN(ph)*SIN(th)
  z1=r*COS(th)
RETURN
PROCEDURE polar2(r,th,ph)
  x2=r*COS(ph)*SIN(th)
  y2=r*SIN(ph)*SIN(th)
  z2=r*COS(th)
RETURN
PROCEDURE polar3(r,th,ph)
  x3=r*COS(ph)*SIN(th)
  y3=r*SIN(ph)*SIN(th)
  z3=r*COS(th)
RETURN
PROCEDURE polar4(r,th,ph)
  x4=r*COS(ph)*SIN(th)
  y4=r*SIN(ph)*SIN(th)
  z4=r*COS(th)
RETURN

procedure calc
  local z,zz,zzz,x,y,x,yy,t
  print "berechne ..."
  t=ctimer
 ' goto kug
  FOR y=y2 TO y1 STEP -sy
    yy=y
    FOR x=x1 TO x2 STEP sx
      z=@f(x,y)
      zz=@f(x+sx,y)
      zzz=@f(x,y+sy)
      zzzz=@f(x+sx,y+sy)
      @addfl(x,y,z,x+sx,y,zz,x+sx,y+sy,zzzz,x,y+sy,zzz)
    NEXT x
  NEXT y
print anzfl;" Flächen. in ";ctimer-t;" Sekunden."
kug:

  local r,spsp,st,phi,theta,x1,x2,x3,x4,y1,y2,y3,y4,z1,z2,z3,z4

  ' Innenkukel: Solid

r=0.3

st=PI/30
FOR theta=0.05 TO PI STEP st
  spsp=PI/30*(1+cos(theta)^2)
  FOR phi=0 TO 2*PI STEP spsp
  
    @polar1(r,theta,phi)
    @polar2(r,theta+st*1.05,phi)
    @polar3(r,theta+st*1.05,phi+spsp*1.05)
    @polar4(r,theta,phi+spsp*1.05)
    @addfl(x1+0.3,y1,z1+0.4,x2+0.3,y2,z2+0.4,x3+0.3,y3,z3+0.4,x4+0.3,y4,z4+0.4)
  
  NEXT phi
NEXT theta

torus:


  print anzfl;" Flächen. in ";ctimer-t;" Sekunden."
return

procedure sort
  local a,i,mx,my,mz,flb,t,buf$,adr,badr,ad,j
  print "Berechne dist..."
  for i=0 to anzfl-1    
    mx=(welt(6+13*i)+welt(13*i))/2
    my=(welt(7+13*i)+welt(1+13*i))/2
    mz=(welt(8+13*i)+welt(2+13*i))/2
    welt(12+13*i)=(mx-stx)^2+(my-sty)^2+(mz-stz)^2
  next i
  PRINT "SORT:"
  buf$=space$(8*13)
  badr=varptr(buf$)
  adr=varptr(welt(0))
  ad=8*13
  t=ctimer
 ' randomize
 ' sortstart=random(0.9*(anzfl-2))
      FOR i=sortstart TO anzfl-2
        for j=i+1 to anzfl-1
        IF welt(12+13*i)<welt(12+13*j)
	  bmove adr+ad*i,badr,ad
	  bmove adr+ad*j,adr+ad*i,ad
	  bmove badr,adr+ad*j,ad  
          flb=TRUE
        ENDIF
	next j
	if ctimer-s>2
	  @progress(anzfl,i)
	  flush
	  s=ctimer
	endif
	exit if ctimer-t>30
      NEXT i
  sortstart=i
  print "in ";ctimer-t;" Sekunden."
return
procedure save
  open "O",#1,"welt.xxx"
  bput #1,varptr(meldung$),64
  bput #1,varptr(anzfl),8
  bput #1,varptr(sortstart),8
  bput #1,varptr(welt(0)),anzfl*13*8
  close #1
return
procedure load
  open "I",#1,"welt.xxx"
  bget #1,varptr(meldung$),64
  bget #1,varptr(anzfl),8
  bget #1,varptr(sortstart),8
  print meldung$
  bget #1,varptr(welt(0)),anzfl*8*13
  close #1
  print anzfl;" Flächen."
return
procedure plot
weiss=get_color(65535,65535,65535)
schwarz=get_color(0,0,0)
grau=get_color(32000,32000,32000)
gelb=get_color(65535,65535,32000)
blau=get_color(32000,32000,65535)
color grau
pbox 0,0,640,400
color weiss
pxU(0)=@kx(x1,y2,z1/2)
pyU(0)=@ky(x1,y2,z1/2)
pxU(1)=@kx(x2,y2,z1/2)
pyU(1)=@ky(x2,y2,z1/2)
pxU(2)=@kx(x2,y2,z2/2)
pyU(2)=@ky(x2,y2,z2/2)
pxU(3)=@kx(x1,y2,z2/2)
pyU(3)=@ky(x1,y2,z2/2)
POLYFILL 4,pxU(),pyU()
color schwarz
POLYLINE 4,pxU(),pyU()
pxU(0)=@kx(x1,y2,z1/2)
pyU(0)=@ky(x1,y2,z1/2)
pxU(1)=@kx(x1,y2,z2/2)
pyU(1)=@ky(x1,y2,z2/2)
pxU(2)=@kx(x1,y1,z2/2)
pyU(2)=@ky(x1,y1,z2/2)
pxU(3)=@kx(x1,y1,z1/2)
pyU(3)=@ky(x1,y1,z1/2)
color weiss
POLYFILL 4,pxU(),pyU()
color blau
for i=z1/2 to z2/2 step (z2-z1)/12
  LINE @kx(x1,y1,i),@ky(x1,y1,i),@kx(x1,y2,i),@ky(x1,y2,i)
  LINE @kx(x1,y2,i),@ky(x1,y2,i),@kx(x2,y2,i),@ky(x2,y2,i)
next i
color gelb
text 20,20,"3D - Flaechengrafik mit X11-Basic   (c) Markus Hoffmann"
color schwarz
LINE @kx(x1,y1,0),@ky(x1,y1,0),@kx(x1,y2,0),@ky(x1,y2,0)
LINE @kx(x1,y2,0),@ky(x1,y2,0),@kx(x2,y2,0),@ky(x2,y2,0)
POLYline 4,pxU(),pyU()

vsync

FOR i=0 to anzfl-1
    pxU(0)=@kx(welt(13*i),welt(1+13*i),welt(2+13*i))
    pyU(0)=@ky(welt(13*i),welt(1+13*i),welt(2+13*i))
    pxU(1)=@kx(welt(3+13*i),welt(4+13*i),welt(5+13*i))
    pyU(1)=@ky(welt(3+13*i),welt(4+13*i),welt(5+13*i))
    pxU(2)=@kx(welt(6+13*i),welt(7+13*i),welt(8+13*i))
    pyU(2)=@ky(welt(6+13*i),welt(7+13*i),welt(8+13*i))
    pxU(3)=@kx(welt(9+13*i),welt(10+13*i),welt(11+13*i))
    pyU(3)=@ky(welt(9+13*i),welt(10+13*i),welt(11+13*i))
    '
    ' brechne Flaechen-Normale
    nx=(welt(4+13*i)-welt(1+13*i))*(welt(11+13*i)-welt(2+13*i))-(welt(5+13*i)-welt(2+13*i))*(welt(10+13*i)-welt(1+13*i))
    ny=(welt(5+13*i)-welt(2+13*i))*(welt(9+13*i)-welt(0+13*i))-(welt(3+13*i)-welt(0+13*i))*(welt(11+13*i)-welt(2+13*i))
    nz=(welt(3+13*i)-welt(0+13*i))*(welt(10+13*i)-welt(1+13*i))-(welt(4+13*i)-welt(1+13*i))*(welt(9+13*i)-welt(0+13*i))
    nl=SQR(nx^2+ny^2+nz^2)


    ' Zeige Normalenvektoren
    nnx=nx/nl
    nny=ny/nl
    nnz=nz/nl
    '   DEFLINE ,1,2,1
    '    LINE @kx(x+sx/2,y+sy/2,z),@ky(x+sx/2,y+sy/2,z),@kx(x+sx/2+nnx,y+sy/2+nny,z+nnz),@ky(x+sx/2+nnx,y+sy/2+nny,z+nnz)
    '   DEFLINE ,0,0
    colorwink=nnx*lichtx+nny*lichty+nnz*lichtz
    IF colorwink<0
      color schwarz
    ELSE
      color get_color(min(65535,1.4*colorwink*lichtr),min(65535,1.4*colorwink*lichtg),min(65535,1.4*colorwink*lichtb))
    ENDIF
    POLYFILL 4,pxU(),pyU()
  
  vsync
NEXT i
'
' BSAVE "E:\ablage\funktion.doo",XBIOS(3),32000
alert 0,"Fertig !",1," OK ",balert
quit
'
'
function f(x,y)
return 0.8*EXP(-2*(x^2+y^2))*COS((x^2+y^2)*10)
endfunc
' Koordinatentransformationen mit Perspektive:
FUNCTION kx(x,y,z)
  LOCAL xx
  '  Verschiebe an den Beobachterstandpunkt
  x=x-stx
  y=y-sty
  z=z-stz
  '
  ' Rotation um die Z-Achse
  xx=x
  x=COS(zwink)*x-SIN(zwink)*y
  y=SIN(zwink)*xx+COS(zwink)*y
  '
  ' Rotation um die X-Achse
  xx=z
  z=COS(xwink)*z+SIN(xwink)*y
  y=-SIN(xwink)*xx+COS(xwink)*y
  '
  '
  ' Neigung:
'  xx=x
  x=COS(nwink)*x+SIN(nwink)*z
'  z=-SIN(nwink)*xx+COS(nwink)*z
  '
  ' Perspektive:
'  py=(z-prozz)*(ebene-prozy)/(y-prozy)
  px=(x-prozx)*(ebene-prozy)/(y-prozy)
  '
  RETURN bxU+bwU/2+px*bwU/(x2-x1)
ENDFUNC
FUNCTION ky(x,y,z)
  LOCAL xx
  '  Verschiebe an den Beobachterstandpunkt
  x=x-stx
  y=y-sty
  z=z-stz
  ' Rotation um die Z-Achse
  xx=x
  x=COS(zwink)*x-SIN(zwink)*y
  y=SIN(zwink)*xx+COS(zwink)*y
  ' Rotation um die X-Achse
  xx=z
  z=COS(xwink)*z+SIN(xwink)*y
  y=-SIN(xwink)*xx+COS(xwink)*y
  '
  ' Neigung:
  xx=x
'  x=COS(nwink)*x+SIN(nwink)*z
  z=-SIN(nwink)*xx+COS(nwink)*z
  ' persp
  py=(z-prozz)*(ebene-prozy)/(y-prozy)
'  px=(x-prozx)*(ebene-prozy)/(y-prozy)
  RETURN byU+bhU/2-py*bhU/(z2-z1)
ENDFUNC
procedure progress(a,b)
  local t$
  
  print chr$(13);"[";string$(b/a*32,"-");">";string$((1.03-b/a)*32,"-");"| ";str$(int(b/a*100),2,2);"% ]";
  flush
return
