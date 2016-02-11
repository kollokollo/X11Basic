' wurm. Nach einer Idee aus Spektrum der Wissenschaft
ECHO OFF
CLS
DIM xkreis(25), ykreis(25)
xkreis(1)=80/2                 
ykreis(1)=24/2                 
ri=0
schwanz=1
c=1
sizew ,320,200
clearw 
go:
text 0,190,"Krieche wie ein Wurm ... und Du bist glücklich!  (c) Markus Hoffmann"

DO
	altschwanz=schwanz
	schwanz=schwanz MOD 25
	inc schwanz
	c=c mod 200
	inc c
	' print at(xkreis(schwanz),ykreis(schwanz));" ";
 	color 0
	circle xkreis(schwanz),ykreis(schwanz),3
	wechsel=RANDOM(10)
	IF wechsel<5
		add ri,0.1745
	ELSE
		sub ri,0.1745
	ENDIF	
	neux=xkreis(altschwanz)+2*COS(ri)
	neuy=ykreis(altschwanz)+SIN(ri)
	neux=neux MOD 320
	neuy=neuy MOD 200
	IF neux<0
		add neux,320
	ENDIF
	IF neuy<0
		add neuy,200
	ENDIF
	xkreis(schwanz)=neux
	ykreis(schwanz)=neuy
	
	' print at(neux,neuy);"*";
	color c
	circle neux,neuy,3
        exit if mousek>500
	vsync
        pause 0.001
loop
' " ... und der Wurm verpuppte sich und ward niemehr gesehen..."
quit
