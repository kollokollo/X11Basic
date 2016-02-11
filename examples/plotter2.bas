' ########################################################
' ##        P L O T T E R   2  (c) Markus Hoffmann      ##
' ##                                                    ##
' ## Idee: 1986, Erstversion 1986, Bearbeitet bis 1991  ##
' ##                                                    ##
' ## Quelltext darf nicht(!) frei kopiert werden !      ##
' ##                                                    ##
' ########################################################
'
' ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
' ++ Hier werden die Funktionen eingegeben :                                ++
' ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
' Alle Funktionen werden in einem Koordinatensystem dargestellt.
' Um eine Funktion nicht darzustellen, einfach f(x)=0 deklarieren.
echo off
weiss=get_color(65535,65535,65535)
schwarz=get_color(0,0,0)
grau=get_color(32000,32000,32000)
schaarstart=-1 
schaarend=1     
schaarstep=1 
color weiss
pbox 0,0,640,400
'
' ++++++++++++++++++++++++++++++++++++++++
' ++ Weitere Parameter Voreingestellt:  ++
' ++++++++++++++++++++++++++++++++++++++++
x1=0    
x2=5   
y1=-0.01   
y2=10    
ex=0.1     
ey=0.1     
xab$="X" 
yab$="Y"
'
'
' ~WIND_GET(4,0,bx&,by&,bw&,bh&)
bx=0
by=0
bw=640
bh=400

' GRAPHMODE 0
ON ERROR GOSUB errrr
' DEFTEXT 1,0,0,4
TEXT 30,370,"von Markus Hoffmann"
' DEFTEXT 1,16,0,13
' Koordinatentransformationen:



@make(bx,by,bw,bh)


keyevent a

CLR iii
ff:
ALERT 2," Flächen füllen ?",2,"JA|NEE",balert
IF balert=1
  PAUSE 1
  DEFFILL ,2,iii+1
  DEFMOUSE 5
  mouseevent a
  
  FILL MOUSEX,MOUSEY
  DEFMOUSE 0
  INC iii
  GOTO ff
ENDIF
end
FILESELECT #"Plot speichern ...",DIR$(0)+"\*.IMG","",aus$
IF aus$<>""
  DEFMOUSE 2
  @save_img(aus$,XBIOS(3),DPEEK(L~A-2),DPEEK(L~A-12),DPEEK(L~A-4),TRUE)
  DEFMOUSE 0
ENDIF
END
PROCEDURE errrr
  ON ERROR GOSUB errrr
  OUT 2,7
  PRINT ERR$(ERR)
  RESUME NEXT
RETURN
PROCEDURE make(bx,by,bw,bh)
  '
'  DEFFILL 1,0
  color grau
  PBOX bx-1,by-1,bx+bw+1,by+bh+1
  ' Koordinatenkreuz:
  '
  ' ---Hilfslinien
  color schwarz
  FOR x=0 TO MAX(ABS(x1),ABS(x2)) STEP ex
   ' DEFLINE &X11111111111111111010101010101011,0,0,0
    LINE @kx(x),by&,@kx(x),by&+bh&
    LINE @kx(-x),by&,@kx(-x),by&+bh&
   ' DEFLINE 1,1,0,0
    LINE @kx(x),@ky(0)-2,@kx(x),@ky(0)+2
    LINE @kx(-x),@ky(0)-2,@kx(-x),@ky(0)+2
  NEXT x
  FOR y=0 TO MAX(ABS(y1),ABS(y2)) STEP ey
   ' DEFLINE &X11111111111111111010101010101011,0,0,0
    LINE bx&,@ky(y),bx+bw,@ky(y)
    LINE bx&,@ky(-y),bx+bw,@ky(-y)
   ' DEFLINE 1,1,0,0
    LINE @kx(0)-2,@ky(y),@kx(0)+2,@ky(y)
    LINE @kx(0)-2,@ky(-y),@kx(0)+2,@ky(-y)
  NEXT y
  ' ---Koordinatenachsen
'  DEFLINE 1,1,0,1
  LINE @kx(0),by+bh,@kx(0),by&    !Y-Achse
  TEXT @kx(0)+10,by+15,yab$
  LINE bx&,@ky(0),bx+bw,@ky(0)    !X-Achse
  TEXT bx+bw-8*(LEN(xab$)),@ky(0),xab$
  ' ---Achsenbeschriftung
 ' DEFTEXT 1,0,0,6
  TEXT @kx(ex)-4,@ky(0)+12,STR$(ex)
  TEXT @kx(0)+3,@ky(ey)+3,STR$(ey)
  '
  ' Funktionen zeichnen
  '
  vsync
 ' DEFLINE 0,0,0,0
  ' ---F(x)
  yy=@ky(0)
  FOR x=bx TO bx+bw
    y=@ky(@f(@bkx(x)))
    LINE x-1,yy,x,y
    yy=y
  NEXT x
  vsync
  ' ---T(x)
  yy=@ky(0)
  FOR x=bx TO bx+bw
    y&=@ky(@t(@bkx(x)))
    LINE x-1,yy,x,y
    yy=y
  NEXT x
  vsync
  ' ---schaar(x)
  FOR a=schaarstart TO schaarend STEP schaarstep
    yy=@ky(0)
    FOR x=bx TO bx+bw
      y=@ky(@schaar(@bkx(x),a))
      LINE x-1,yy,x,y
      yy=y
    NEXT x
    vsync
  NEXT a
RETURN
PROCEDURE save_img(f$,adr%,aw&,bw&,bh&,fl!)
  LOCAL i&,anz&,azeile&,vzeile&,patlen&
  ' Nur monochrome (1 Plane)
  '
  ' f$ dateiname
  ' adr% adresse der Bitmap
  ' aw&  Breite einer Zeile in Bytes
  ' bw&  Breite in Pixeln
  ' bh&  H”he in Zeilen
  ' fl!=true : gepackt  0= ungepackt
  '
  '
  '
  patlen&=2
  OPEN "O",#1,f$
  PRINT #1,MKI$(1);MKI$(8);MKI$(1);MKI$(patlen&);MKI$(200);MKI$(200);
  PRINT #1,MKI$(bw&);MKI$(bh&);
  '
  IF fl!
    vzeile$=SPACE$(aw&)
    anz&=1
    CLR azeile$
    i&=0
    WHILE i&<bh&
      LINE 0,i&-1,640,i&-1
      '      PRINT AT(1,1);"Zeile"'i&
      BMOVE adr%+i&*aw&,V:vzeile$,aw&
      IF vzeile$=azeile$
        INC anz&
      ELSE
        IF LEN(azeile$)        !  (1 Zeile hat keinen Vorgaenger)
          IF anz&>1
            PRINT #1,CHR$(0);CHR$(0);CHR$(&HFF);CHR$(anz&);
          ENDIF
          PRINT #1,@pack_zeile$(azeile$);
          ~FRE(0)
        ENDIF
        azeile$=vzeile$
        anz&=1
      ENDIF
      INC i&
    WEND
    ' nun die letzte zeile speichern :
    IF LEN(azeile$)
      IF anz&>1
        PRINT #1,CHR$(0);CHR$(0);CHR$(&HFF);CHR$(anz&);
      ENDIF
      PRINT #1,@pack_zeile$(azeile$);
    ENDIF
    ~FRE(0)
  ELSE
    '
    ' Ungepackt speichern (sehr Speicherintensiv !)
    '
    '
    FOR i&=0 TO bh&-1
      PRINT #1,CHR$(&H80);CHR$(aw&);
      BPUT #1,adr%+i&*aw&,aw&
    NEXT i&
  ENDIF
  CLOSE #1
RETURN
> FUNCTION pack_zeile$(z$)
LOCAL c$
CLR c$,bu$
'
' Diese Prozedur ist zeitkritisch, deshalb gut optimieren !!!
'
WHILE LEN(z$)
  CLR ff&
  null&=@nulltest(z$)
  IF null&=0
    ff&=@fftest(z$)
  ENDIF
  pat&=@pattest(z$)
  vpat=pat&*patlen&/(2+patlen&)
  IF ff& AND ff&>=vpat
    IF LEN(bu$) AND ff&=1
      bu$=bu$+LEFT$(z$)
      z$=RIGHT$(z$,LEN(z$)-1)
    ELSE
      IF LEN(bu$)
        c$=c$+CHR$(&H80)+CHR$(LEN(bu$))+bu$
        CLR bu$
      ENDIF
      c$=c$+CHR$(ff& OR &X10000000)
      z$=RIGHT$(z$,LEN(z$)-ff&)
    ENDIF
  ELSE IF null& AND null&>=vpat
    IF LEN(bu$) AND null&=1
      bu$=bu$+LEFT$(z$)
      z$=RIGHT$(z$,LEN(z$)-1)
    ELSE
      IF LEN(bu$)
        c$=c$+CHR$(&H80)+CHR$(LEN(bu$))+bu$
        CLR bu$
      ENDIF
      c$=c$+CHR$(null& OR &X0)
      z$=RIGHT$(z$,LEN(z$)-null&)
    ENDIF
  ELSE IF vpat>ff& AND vpat>null& AND pat&>1
    IF LEN(bu$)
      c$=c$+CHR$(&H80)+CHR$(LEN(bu$))+bu$
      CLR bu$
    ENDIF
    c$=c$+CHR$(0)+CHR$(pat&)+LEFT$(z$,2)
    z$=RIGHT$(z$,LEN(z$)-pat&*patlen&)
  ELSE
    bu$=bu$+LEFT$(z$)
    z$=RIGHT$(z$,LEN(z$)-1)
  ENDIF
WEND
IF LEN(bu$)
  c$=c$+CHR$(&H80)+CHR$(LEN(bu$))+bu$
  CLR bu$
ENDIF
RETURN c$
ENDFUNC
FUNCTION nulltest(z$)
LOCAL c,i
IF LEN(z$)
c=0
FOR i=0 TO MIN(LEN(z$),127)-1
  IF PEEK(V:z$+i)=0
    INC c
  ELSE
    RETURN c
  ENDIF
NEXT i
RETURN c
ELSE
RETURN 0
ENDIF
ENDFUNC
FUNCTION fftest(z$)
LOCAL c&,i&
IF LEN(z$)
c=0
FOR i=0 TO MIN(LEN(z$),127)-1
IF PEEK(V:z$+i)=&HFF
  INC c
ELSE
  RETURN c
ENDIF
NEXT i
RETURN c
ELSE
RETURN 0
ENDIF
ENDFUNC

function f(x)
return sqr(-(0.6*2300)/(pi*274*0.063)*(cos(pi+x/180*pi)-1))
function t(x)
return COS(x)*SIN(x)+SIN(x)           ! # Funktion T(x)
function schaar(x,a)
return 1/(x+ABS(x==0)*0.01)    !# Funktionenschaar

function kx(x)
return bw/(x2-x1)*(x-x1)+bx
function bkx(x)
return (x-bx)/bw*(x2-x1)+x1
function ky(y)
return -bh/(y2-y1)*(y+y1)+by
function bky(y)
return -(y-by)/bh*(y2-y1)-y1
