' 3D-grafics benchmark (c) Markus Hoffmann,
' die Idee und das Programm kommt noch aus der ATARI-ST Zeit (ca. 1991)
'
' uses polygon and polyfill commands
'
'
DIM pxU(4),pyU(4)
'
'
bx%=0
by%=18
bw%=640
bh%=400-50
weiss=COLOR_RGB(1,1,1)
schwarz=COLOR_RGB(0,0,0)
grau=COLOR_RGB(1/2,1/2,1/2)
gelb=COLOR_RGB(1,1,1/2)
blau=COLOR_RGB(1/2,1/2,1)
SHOWPAGE
GET_GEOMETRY 1,bx%,by%,bw%,bh%
CLIP bx%,by%,bw%,bh%

bx%=0.1*bw%
by%=0.1*bh%
SUB bw%,2*bx%
SUB bh%,2*by%

' Koordinaten:
'
'bw%=bh%
x1=-1
x2=1
y1=-1
y2=1
z1=-1
z2=1
'
sx=0.02
sy=0.03
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
lichtz=SQR(1-lichty^2)
' Lichtfarbe
lichtr=50000
lichtg=65535
lichtb=65535
'
CLEARW 1
COLOR grau
PBOX bx%,by%,bw%,bh%
COLOR weiss
pxU(0)=@kx(x1,y2,z1/2)
pyU(0)=@ky(x1,y2,z1/2)
pxU(1)=@kx(x2,y2,z1/2)
pyU(1)=@ky(x2,y2,z1/2)
pxU(2)=@kx(x2,y2,z2/2)
pyU(2)=@ky(x2,y2,z2/2)
pxU(3)=@kx(x1,y2,z2/2)
pyU(3)=@ky(x1,y2,z2/2)
POLYFILL 4,pxU(),pyU()
COLOR schwarz
POLYLINE 4,pxU(),pyU()
pxU(0)=@kx(x1,y2,z1/2)
pyU(0)=@ky(x1,y2,z1/2)
pxU(1)=@kx(x1,y2,z2/2)
pyU(1)=@ky(x1,y2,z2/2)
pxU(2)=@kx(x1,y1,z2/2)
pyU(2)=@ky(x1,y1,z2/2)
pxU(3)=@kx(x1,y1,z1/2)
pyU(3)=@ky(x1,y1,z1/2)
COLOR weiss
POLYFILL 4,pxU(),pyU()
COLOR blau
FOR i=z1/2 TO z2/2 STEP (z2-z1)/12
  LINE @kx(x1,y1,i),@ky(x1,y1,i),@kx(x1,y2,i),@ky(x1,y2,i)
  LINE @kx(x1,y2,i),@ky(x1,y2,i),@kx(x2,y2,i),@ky(x2,y2,i)
NEXT i
COLOR gelb
TEXT bx%+20,by%+20,"3D - Flaechengrafik mit X11-Basic   (c) Markus Hoffmann"
COLOR schwarz
LINE @kx(x1,y1,0),@ky(x1,y1,0),@kx(x1,y2,0),@ky(x1,y2,0)
LINE @kx(x1,y2,0),@ky(x1,y2,0),@kx(x2,y2,0),@ky(x2,y2,0)
POLYLINE 4,pxU(),pyU()

SHOWPAGE
tim=timer
FOR y=y2 DOWNTO y1 STEP sy
  IF SGN(y)<>SGN(yy)
    LINE @kx(x1,0,0),@ky(x1,0,0),@kx(x2,0,0),@ky(x2,0,0)
    LINE @kx(0,0,z1),@ky(0,0,z1),@kx(0,0,z2),@ky(0,0,z2)
  ENDIF
  yy=y
  FOR x=x1 TO x2 STEP sx
    ' print x,y
    z=@f(x,y)
    zz=@f(x+sx,y)
    zzz=@f(x,y+sy)
    zzzz=@f(x+sx,y+sy)
    '    LINE @kx(x,y,z),@ky(x,y,z),@kx(x+sx,y,zz),@ky(x+sx,y,zz)
    '    LINE @kx(x,y,z),@ky(x,y,z),@kx(x,y+sy,zzz),@ky(x,y+sy,zzz)
    pxU(0)=@kx(x,y,z)
    pyU(0)=@ky(x,y,z)
    pxU(1)=@kx(x+sx,y,zz)
    pyU(1)=@ky(x+sx,y,zz)
    pxU(2)=@kx(x+sx,y+sy,zzzz)
    pyU(2)=@ky(x+sx,y+sy,zzzz)
    pxU(3)=@kx(x,y+sy,zzz)
    pyU(3)=@ky(x,y+sy,zzz)
    '
    ' brechne Flaechen-Normale
    nx=(y-y)*(zzz-z)-(zz-z)*(y+sy-y)
    ny=(zz-z)*(x-x)-(x+sx-x)*(zzz-z)
    nz=(x+sx-x)*(y+sy-y)-(y-y)*(x-x)
    ' Zeige Normalenvektoren
    nnx=nx/SQR(nx^2+ny^2+nz^2)
    nny=ny/SQR(nx^2+ny^2+nz^2)
    nnz=nz/SQR(nx^2+ny^2+nz^2)
    '   DEFLINE ,1,2,1
    '    LINE @kx(x+sx/2,y+sy/2,z),@ky(x+sx/2,y+sy/2,z),@kx(x+sx/2+nnx,y+sy/2+nny,z+nnz),@ky(x+sx/2+nnx,y+sy/2+nny,z+nnz)
    '   DEFLINE ,0,0
    colorwink=nnx*lichtx+nny*lichty+nnz*lichtz
    IF colorwink<0
      COLOR schwarz
    ELSE
      COLOR get_color(colorwink*lichtr,colorwink*lichtg,colorwink*lichtb)
    ENDIF
    POLYFILL 4,pxU(),pyU()
  NEXT x
  LINE @kx(0,y-sy,0),@ky(0,y-sy,0),@kx(0,y,0),@ky(0,y,0)
  SHOWPAGE
NEXT y
'
' BSAVE "E:\ablage\funktion.doo",XBIOS(3),32000
PRINT timer-tim;" Sekunden."
ALERT 0,"Done !",1," OK |QUIT",balert
IF balert=2
  QUIT
ENDIF
END
'
'
DEFFN f(x,y)=0.8*EXP(-2*(x^2+y^2))*COS((x^2+y^2)*10)

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
  RETURN bx%+bw%/2+px*bw%/(x2-x1)
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
  RETURN by%+bh%/2-py*bh%/(z2-z1)
ENDFUNC
