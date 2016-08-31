' 3D MAZE II-BASED ON CODE BY LODE VANDEVENNE
' http://lodev.org/cgtutor/raycasting.html
' Converted to X11-Basic 2015
'
RANDOMIZE
@init_palette
GET_GEOMETRY 1,bx%,by%,scrw%,scrh%
mw%=51       ! Map width in units
mh%=51       ! Map height in units
DIM MAP(mw%,mh%)
mmw%=13      ! Minimap size
mmh%=13
@mazegen
PX=1.5       ! Aktuelle Betrachterprosition
PY=1.5       ! Aktuelle Betrachterprosition
RS=RAD(2)    ! stepwith in rad
MS=0.06      ! Maschenweite, Schrittweite
CLR OMX,OMY,OS,OH1,OH2
IF MAP(PX+1,PY)=0
  PLA=0  ! Orientierung rechts
ELSE
  PLA=90.001  ! Orientierung unten
ENDIF
DO    ! ENGINE
  @render
  @minimap
  IF fadenkreuz%
    COLOR weiss
    CIRCLE scrw%/2,scrh%/2,20
    LINE scrw%/2-30,scrh%/2,scrw%/2+30,scrh%/2
    LINE scrw%/2,scrh%/2-30,scrw%/2,scrh%/2+30
  ENDIF
  keyagain:
  COLOR gelb,schwarz
  TEXT 130,20,"w=forward, y=back, a=turn left, d=turn right"
  SHOWPAGE
  KEYEVENT a,b,k$
  IF PX>=MW%-0.4 OR Py>=MH%-0.4  ! WIN!
    ALERT 1,"CONGRATULATIONS!||Play again?",1,"YES|NO",a%
    IF a%=1
      RUN
    ENDIF
    QUIT
  ENDIF
  ' MOVEMENT
  IF k$="q" OR k$=CHR$(27)
    ALERT 1,"Do you want to Quit?||",1,"YES|NO",a%
    IF a%=1
      QUIT
    ENDIF
  ELSE IF k$=" "
    fadenkreuz%=not fadenkreuz%
  ELSE IF k$="d"
    ADD PLA,RS
  ELSE IF k$="a"
    SUB PLA,RS
  ELSE IF k$="w"
    IF MAP(PX+DX*2*MS+0.5,PY)=0
      ADD PX,DX*MS
    ENDIF
    IF MAP(PX,PY+DY*2*MS+0.5)=0
      ADD PY,DY*MS
    ENDIF
    IF MAP(PX+DX*2*MS+0.5,PY)<>0 AND MAP(PX,PY+DY*2*MS+0.5)<>0
      COLOR weiss
      TEXT scrw%/2-32,scrh%/2+4,"AUTSCH!!!"
      GOTO keyagain
    ENDIF
    ' Do not remove this line until compiler bug is fixed....
  ELSE IF k$="y"
    IF MAP(PX-DX*2*MS+0.5,PY)=0
      SUB px,DX*MS
    ENDIF
    IF MAP(PX,PY-DY*2*MS+0.5)=0
      SUB py,DY*MS
    ENDIF
  ELSE
    PRINT "unknown key:",k$
  ENDIF
  CLEARW
LOOP
PROCEDURE render
  LOCAL x%
  COLOR bluesky      ! Himmel
  PBOX 0,0,scrw%-1,scrh%/2
  COLOR floorcol      ! Fussboden
  PBOX 0,scrh%/2,scrw%-1,scrh%-1
  DX=COS(PLA)
  DY=SIN(PLA)
  RX=PX
  RY=PY
  FOR x%=0 TO scrw%-1
    CX=2*x%/scrw%-1
    RDX=DX-DY*CX
    RDY=DY+DX*CX
    MX=INT(RX)
    MY=INT(RY)
    IF rdx=0
      ADD rdx,0.001
    ENDIF
    DDX=SQR(1+(RDY*RDY)/(RDX*RDX))
    IF rdy=0
      ADD rdy,0.001
    ENDIF
    DDY=SQR(1+(RDX*RDX)/(RDY*RDY))
    IF RDX<0
      STX=-1
      SDX=(RX-MX)*DDX
    ELSE
      STX=1
      SDX=(MX+1-RX)*DDX
    ENDIF
    IF RDY<0
      STY=-1
      SDY=(RY-MY)*DDY
    ELSE
      STY=1
      SDY=(MY+1-RY)*DDY
    ENDIF
    120:
    IF SDX<SDY
      ADD SDX,DDX
      ADD MX,STX
      S=0
    ELSE
      ADD SDY,DDY
      ADD MY,STY
      S=1
    ENDIF
    IF MX=mw%
      COLOR kante
      LINE x%,OH1,x%,OH2
      OH1=scrh%/2
      OH2=scrh%/2
    ELSE
      IF MAP(MX,MY)=0
        GOTO 120
      ENDIF
      '  HIT A WALL-WHICH?
      IF S=0
        PWD=ABS((MX-RX+(1-STX)/2)/RDX)
        ADD S,2*abs(RDX<0)
      ELSE
        PWD=ABS((MY-RY+(1-STY)/2)/RDY)
        ADD S,2*abs(RDY<0)
      ENDIF
      HT=ABS(INT(scrh%/PWD))
      H1=MAX(-HT/2+scrh%/2,0)
      H2=MIN(HT/2+scrh%/2,scrh%-1)
      ' RENDER THE RAY
      H=MAX(H1,OH1)
      J=MAX(H2,OH2)
      COLOR palette(abs(S*(((OMX=MX OR OMY=MY) AND OS=S) OR x%=0)))
      LINE x%,H,x%,J
      IF H1<>0
        COLOR kante
        LINE x%,H1,x%,OH1
      ENDIF
      COLOR kante
      LINE x%,H2,x%,OH2
      OMX=MX
      OMY=MY
      OS=S
      OH1=H1*abs(x%<scrw%-1)
      IF x%=scrw%-1
        OH2=scrh%
      ELSE
        OH2=H2
      ENDIF
    ENDIF
  NEXT x%
RETURN
PROCEDURE mazegen
  ' GENERATE MAZE
  LOCAL CC,m$,b$,stk$
  DIM OF(4+1),NB(4+1)
  M$=STRING$(mw%*mh%,"1")
  B$=STRING$(mw%,"1")+STRING$(mh%-2,"1"+STRING$(mw%-2," ")+"1")+STRING$(mw%,"1")
  CC=mw%+2
  OF(0)=-1
  OF(1)=-mw%
  OF(2)=1
  OF(3)=mw%
  ' DIM OF=-1,-mw%,1,mw%
  POKE VARPTR(M$)+CC,ASC("0")
  STK$=MKL$(CC)
  340:
  NN=0
  FOR N=1 TO 4
    IF MID$(B$,CC+OF(N)+1)<>"1"
      IF MID$(M$,CC+OF(N)*2+1)<>"0"
        NB(nn)=n
        INC NN
      ENDIF
    ENDIF
  NEXT N
  IF nn>1
    COLOR rot
  ELSE
    COLOR gelb
  ENDIF
  BOX 4*(cc MOD mw%),4*(cc DIV mw%),4*(cc MOD mw%)+3,4*(cc DIV mw%)+3
  SHOWPAGE
  IF NN>0
    O=OF(NB(INT(RANDOM(NN+1))))
    IF cc+o>0 AND cc+o<len(m$)
      POKE VARPTR(M$)+CC+O,ASC("0")
      LINE 4*(cc MOD mw%)+2,4*(cc DIV mw%)+2,4*((cc+o) MOD mw%)+2,4*((cc+o) DIV mw%)+2
    ENDIF
    IF cc+o*2>0 AND cc+o*2<len(m$)
      POKE VARPTR(M$)+CC+O*2,ASC("0")
    ENDIF
    IF nn>1 AND cc>0
      STK$=MKL$(CC)+STK$
    ENDIF
    IF cc+2*o>0 AND cc+2*o<len(m$)
      ADD CC,O*2   ! new position
      GOTO 340
    ENDIF
  ENDIF
  IF STK$<>""
    CC=CVL(STK$)
    STK$=RIGHT$(STK$,LEN(STK$)-4)
    GOTO 340
  ENDIF
  FOR N=0 TO LEN(M$)-1
    MAP(N MOD MW%,N DIV MW%)=VAL(MID$(M$,N+1))*208
  NEXT N
  MAP(1,1)=0
  MAP(1,2)=0
  MAP(2,2)=0
  MAP(2,1)=0
  MAP(mw%-1,mh%-2)=0
RETURN
PROCEDURE init_palette
  ' SET UP PALETTE
  DIM palette(8)
  rot=COLOR_RGB(1,0,0)
  gelb=COLOR_RGB(1,1,0)
  schwarz=COLOR_RGB(0,0,0)
  weiss=COLOR_RGB(1,1,1)
  metallic=COLOR_RGB(0,128/256,1)
  bluesky=COLOR_RGB(0,1,1)
  floorcol=COLOR_RGB(0,128/256,0)
  kante=COLOR_RGB(0.3,0,0)
  palette(0)=COLOR_RGB(0.5,0.1,0.3)
  palette(1)=COLOR_RGB(192/256,128/256,0)
  palette(2)=COLOR_RGB(80/256,80/256,0)
  palette(3)=COLOR_RGB(128/256,80/256,0)
  palette(4)=COLOR_RGB(192/256,192/256,0)
RETURN

PROCEDURE minimap
  ' MINIMAP
  LOCAL fx%,fy%,ax%,ay%,qx%,qy%
  ax%=MIN(MAX(0,INT(PX)-INT(mmw%/2)),mw%-(mmw%+1))
  ay%=MIN(MAX(0,INT(PY)-INT(mmh%/2)),mh%-(mmh%+1))
  FOR fx%=ax% TO MIN(ax%+mmw%,mw%-1)
    FOR fy%=ay% TO MIN(ay%+mmh%,mh%-1)
      IF MAP(fx%,fy%)=0
        COLOR schwarz
      ELSE
        COLOR metallic
      ENDIF
      PBOX (fx%-ax%)*8,(fy%-ay%)*8,(fx%-ax%)*8+7,(fy%-ay%)*8+7
    NEXT fy%
  NEXT fx%
  COLOR rot
  qx%=(PX-ax%)*8+4
  qy%=(py-ay%)*8+4
  PCIRCLE qx%,qy%,4
  COLOR gelb
  LINE qx%,qy%,qx%+(DX*8),qy%+(DY*8)
RETURN
