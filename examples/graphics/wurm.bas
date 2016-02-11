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
SIZEW ,bw,bh
schwarz=COLOR_RGB(0,0,0)
CLEARW 
go:
TEXT 0,190,"Krieche wie ein Wurm ... und Du bist glücklich!  (c) Markus Hoffmann"
DO
  altschwanz=schwanz
  schwanz=schwanz MOD 25
  INC schwanz
  c=c MOD 200
  INC c
  COLOR schwarz
  CIRCLE xkreis(schwanz),ykreis(schwanz),3
  wechsel=RANDOM(10)
  IF wechsel<5
    ADD ri,0.1745
  ELSE
    SUB ri,0.1745
  ENDIF   
  neux=xkreis(altschwanz)+2*COS(ri)
  neuy=ykreis(altschwanz)+SIN(ri)
  neux=neux MOD bw
  neuy=neuy MOD bh
  IF neux<0
    ADD neux,bw
  ENDIF
  IF neuy<0
    ADD neuy,bh
  ENDIF
  xkreis(schwanz)=neux
  ykreis(schwanz)=neuy

  COLOR @farbe(c)
  CIRCLE neux,neuy,3
  EXIT IF MOUSEK>500
  SHOWPAGE
  PAUSE pt
  IF TIMER-t>0.025
    pt=pt/1.1
  ENDIF
  IF TIMER-t<0.021
    pt=pt*1.1+0.001
  ENDIF
  t=TIMER
LOOP
' " ... und der Wurm verpuppte sich und ward niemehr gesehen..."
QUIT
FUNCTION farbe(n)
  LOCAL r,g,b
  r=RND()
  g=RND()
  b=SQRt(1-r*r-g*g)
  RETURN COLOR_RGB(r,g,b)
ENDFUNC
