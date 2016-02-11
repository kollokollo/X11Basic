' wurm. Nach einer Idee aus Spektrum der Wissenschaft
' (c) Markus Hoffmann V.1.08
'
DIM xkreis(25+1), ykreis(25+1)
xkreis(1)=80/2                 
ykreis(1)=24/2                 
ri=0
schwanz=1
c=1
pt=0.1
bw=320
bh=200
sizew ,bw,bh
schwarz=get_color(0,0,0)
clearw 
go:
text 0,190,"Krieche wie ein Wurm ... und Du bist glücklich!  (c) Markus Hoffmann"
DO
	altschwanz=schwanz
	schwanz=schwanz MOD 25
	inc schwanz
	c=c mod 200
	inc c
 	color schwarz
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
	
	color @farbe(c)
	circle neux,neuy,3
        exit if mousek>500
	vsync
        pause pt
	if timer-t>0.025
	  pt=pt/1.1
	endif
	if timer-t<0.021
	  pt=pt*1.1+0.001
	endif
        t=timer
loop
' " ... und der Wurm verpuppte sich und ward niemehr gesehen..."
quit
function farbe(n)
  local r,g,b,col
  r=random(1000)/1000
  g=random(1000)/1000
  b=sqrt(1-r*r-g*g)
  col=get_color(r*65535,g*65535,b*65535)
  return col
endfunc
