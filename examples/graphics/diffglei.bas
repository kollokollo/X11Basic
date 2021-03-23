'
' Vorversion DGL2 20.10.87 16.4.89   DGL2A
' UNI WUPPERTAL
'
'  V. 1.001 ATARI-ST  (c) Markus Hoffmann Begonnen April 1993
'                                       Letzte Bearbeitung: Mai 1994
'
DEFMOUSE 2
@init
MENUDEF menue$(),m
@gem_init
b=0.01213             ! Konstanten in der Diffgleichung
a=1-b                 ! Werden hier definiert
dt=0.005              ! Genauigkeit für numerische Integration
DEFMOUSE 5
DO
  MENU
  PAUSE 0.01
  MOUSE mx,my,mk
  IF mk AND pkob
    @vektor(mx,my)
  ENDIF
LOOP
'
' Beispiele für Gewöhnliche Differentialgleichungen
'
' x__=-1              ! Freier Fall
' x__=-2*p*x1-q*x2    ! GEDÄMPFTER OSZILLATOR
' x__=-SINQ(DEG(x2))  ! Mathematisches Pendel
' x__=-SIN(x2)-0.2*x1 ! gedämpftes mathematisches Pendel
' x__=-1/(x2)^2       ! Gravitation
'
PROCEDURE dgl2
  z1=((x-a)^2+y^2)^1.5
  z2=((x+b)^2+y^2)^1.5
  '  x__=x+2*y_-a*(x+b)/z2-b*(x-a)/z1              ! ZWEITE Ableitung
  x__=-SIN(x)-0.2*x_     ! gedämpftes mathematisches Pendel
  x_=x_+dt*x__           ! erste Ableitung
  x=x+x_*dt              ! Bewegungsgleichung
  ' Jetzt eine 2te Funktion
  y__=y-2*x_-a*(y)/z2-b*(y)/z1                  ! ZWEITE Ableitung
  y_=y_+dt*y__           ! erste Ableitung
  y=y+y_*dt              ! Bewegungsgleichung
  t=t+dt
RETURN
'
' Anfangswerte:
aw_data:
'      X Y X' Y'       X"Y"   t
DATA 1.2,0,0 ,-1.04936,0,0,   0
'
PROCEDURE init
  LOCAL i
  DIM menue$(80)
  DIM f(100),f_(100)   ! Für Phasenrennen
  '
  rot=COLOR_RGB(1,0,0)
  gelb=COLOR_RGB(1,4/6,0)
  grau=COLOR_RGB(1/2,1/2,1/2)
  weiss=COLOR_RGB(1,1,1)
  schwarz=COLOR_RGB(0,0,0)
  lila=COLOR_RGB(1,0,1)
  blau=COLOR_RGB(1/6,1/6,1)
  gruen=COLOR_RGB(0,1/2,0)
  bx=0
  by=0
  bw=640
  bh=400
  GET_GEOMETRY 1,bx,by,bw,bh
  ADD by,16
  SUB bh,bx

  COLOR weiss
  PBOX bx,by,bx+bw,by+bh
  COLOR schwarz,weiss
  RESTORE menudata
  FOR i=0 TO 80
    READ t$
    menue$(i)=t$
    EXIT IF menue$(i)="***"
  NEXT i
  menue$(i)=""
  menue$(i+1)=""
  ' Anfangswerte
  RESTORE aw_data
  READ x0,y0,x_0,y_0,x__0,y__0,t0
  ' Koordinatensysteme
  RESTORE px_data
  READ pkoxv,pkoxb,pkox_v,pkox_b,pkorwx,pkorwx_
  RESTORE py_data
  GOTO aaa
  aaa:
  READ pkoyv
  READ pkoyb
  READ pkoy_v
  READ pkoy_b
  READ pkorwy
  READ pkorwy_
RETURN
PROCEDURE n
  ~FORM_ALERT(1,"[3][Funktion nicht implementiert !][ OH ]")
RETURN

DEFFN kx(x)=bw/(kkob-kkoa)*(x-kkoa)
DEFFN ky(y)=by-bh/(kkod-kkoc)*(y+kkoc)
DEFFN px(x)=bw/(pkob-pkoa)*(x-pkoa)
DEFFN py(y)=by-bh/(pkod-pkoc)*(y+pkoc)
DEFFN pbx(x)=x/bw*(pkob-pkoa)+pkoa           !bwU/(pkob-pkoa)*(x-pkoa)
DEFFN pby(y)=-(y-by)/bh*(pkod-pkoc)-pkoc    !-bhU/(pkod-pkoc)*(y+pkoc)+byU
DEFFN bx(x)=bw/(kob-koa)*(x-koa)
DEFFN by(y)=by-bh/(kod-koc)*(y+koc)

PROCEDURE sh_x
  LOCAL at,et
  DEFMOUSE 2
  RESTORE x_data
  READ at,et
  RESTORE x_data
  @bkoordinate("t","x")
  t=t0
  x=x0
  y=y0
  x_=x_0
  y_=y_0
  WHILE t<MOUSEK=0
    @dgl2
    PLOT @bx(t),@by(x)
    SHOWPAGE
  WEND
  DEFMOUSE 5
  SHOWPAGE
RETURN
x_data:
DATA -1,7,-2,2,1,1
PROCEDURE sh_x_
  DEFMOUSE 2
  RESTORE x_data_
  READ t,et
  RESTORE x_data_
  @bkoordinate("t","x'")
  t=t0
  x=x0
  y=y0
  x_=x_0
  y_=y_0
  DO
    @dgl2
    PLOT @bx(t),@by(x_)
    SHOWPAGE
    EXIT IF t>et
  LOOP
  DEFMOUSE 5
  SHOWPAGE
RETURN
x_data_:
DATA -1,7,-2,2,1,1
PROCEDURE sh_x__
  DEFMOUSE 2
  RESTORE x_data__
  READ t,et
  RESTORE x_data__
  @bkoordinate("t","x""")
  RESTORE aw_data
  READ x,y,x_,y_
  DO
    @dgl2
    PLOT @bx(t),@by(x__)
    SHOWPAGE
    EXIT IF t>et
  LOOP
  DEFMOUSE 5
  SHOWPAGE
RETURN
x_data__:
DATA -1,7,-2,2,1,1
PROCEDURE sh_y
  DEFMOUSE 2
  RESTORE y_data
  READ t,et
  RESTORE y_data
  @bkoordinate("t","y")
  RESTORE aw_data
  READ x,y,x_,y_
  DO
    @dgl2
    PLOT @bx(t),@by(y)
    SHOWPAGE
    EXIT IF t>et
  LOOP
  DEFMOUSE 5
  SHOWPAGE
RETURN
y_data:
DATA -1,7,-2,2,1,1
PROCEDURE sh_y_
  DEFMOUSE 2
  RESTORE y_data_
  READ t,et
  RESTORE y_data_
  @bkoordinate("t","y'")
  RESTORE aw_data
  READ x,y,x_,y_
  DO
    @dgl2
    PLOT @bx(t),@by(y_)
    SHOWPAGE
    EXIT IF t>et
  LOOP
  DEFMOUSE 5
  SHOWPAGE
RETURN
y_data_:
DATA -1,7,-2,2,1,1
PROCEDURE sh_y__
  DEFMOUSE 2
  RESTORE y_data__
  READ t,et
  RESTORE y_data__
  @bkoordinate("t","y""")
  RESTORE aw_data
  READ x,y,x_,y_
  DO
    @dgl2
    PLOT @bx(t),@by(y__)
    SHOWPAGE
    EXIT IF t>et
  LOOP
  DEFMOUSE 5
  SHOWPAGE
RETURN
y_data__:
DATA -1,7,-2,2,1,1
PROCEDURE sh_xy
  DEFMOUSE 2
  TEXT 500,10,"Maustaste=stop"
  RESTORE xy_data
  @kkoordinate("x","y")
  RESTORE aw_data
  READ x,y,x_,y_
  DRAW @kx(x),@ky(y) TO @kx(x),@ky(y)
  WHILE mousek=0
    DRAW TO @kx(x),@ky(y)
    SHOWPAGE
    @dgl2
  WEND
  DEFMOUSE 5
  SHOWPAGE
RETURN
xy_data:
DATA -1.5,1.5,-1,1,.5,.5
px_data:
DATA -5,5,-3,3,1,1
py_data:   ! x: von bis y: von bis step x step y
DATA             -5,  5,    -3,  3,     1,     1
PROCEDURE bkoordinate(tx$,ty$)
  LOCAL x,y
  DEFLINE ,1,0,1
  DEFTEXT ,0,0,4
  READ koa,kob,koc,kod,koe,kof
  LINE @bx(koa),@by(0),@bx(kob),@by(0)
  LINE @bx(0),@by(koc),@bx(0),@by(kod)
  DEFLINE ,1,0,0
  FOR x=0 TO MAX(ABS(koa),ABS(kob)) STEP koe
    LINE @bx(x),@by(0)-2,@bx(x),@by(0)+2
    LINE @bx(-x),@by(0)-2,@bx(-x),@by(0)+2
    TEXT @bx(x)-4,@by(0)+9,STR$(x)
    TEXT @bx(-x)-4,@by(0)+9,STR$(-x)
  NEXT x
  FOR y=0 TO MAX(ABS(koc),ABS(kod)) STEP kof
    LINE @bx(0)-2,@by(y),@bx(0)+2,@by(y)
    LINE @bx(0)-2,@by(-y),@bx(0)+2,@by(-y)
    TEXT @bx(0)+7,@by(y)+2,STR$(y)
    TEXT @bx(0)+7,@by(-y)+2,STR$(-y)
  NEXT y
  DEFTEXT 1,0,0,6
  TEXT @bx(kob)-8*LEN(tx$)-8,@by(0)+8,tx$
  TEXT @bx(0)-8*LEN(ty$)-8,@by(kod)+8,ty$
RETURN
PROCEDURE kkoordinate(tx$,ty$)
  LOCAL x,y
  DEFLINE ,1,0,1
  DEFTEXT ,0,0,4
  READ kkoa,kkob,kkoc,kkod,kkoe,kkof
  LINE @kx(kkoa),@ky(0),@kx(kkob),@ky(0)
  LINE @kx(0),@ky(kkoc),@kx(0),@ky(kkod)
  DEFLINE ,1,0,0
  FOR x=0 TO MAX(ABS(kkoa),ABS(kkob)) STEP kkoe
    LINE @kx(x),@ky(0)-2,@kx(x),@ky(0)+2
    LINE @kx(-x),@ky(0)-2,@kx(-x),@ky(0)+2
    TEXT @kx(x)-4,@ky(0)+9,STR$(x)
    TEXT @kx(-x)-4,@ky(0)+9,STR$(-x)
  NEXT x
  FOR y=0 TO MAX(ABS(kkoc),ABS(kkod)) STEP kkof
    LINE @kx(0)-2,@ky(y),@kx(0)+2,@ky(y)
    LINE @kx(0)-2,@ky(-y),@kx(0)+2,@ky(-y)
    TEXT @kx(0)+7,@ky(y)+2,STR$(y)
    TEXT @kx(0)+7,@ky(-y)+2,STR$(-y)
  NEXT y
  TEXT @kx(kkob)-8*LEN(tx$)-8,@ky(0)+8,tx$
  TEXT @kx(0)-8*LEN(ty$)-8,@ky(kkod)+8,ty$
RETURN
PROCEDURE pkoordinate(tx$,ty$)
  LOCAL x,y
  DEFLINE ,1,0,1
  DEFTEXT ,0,0,4
  LINE @px(pkoa),@py(0),@px(pkob),@py(0)
  LINE @px(0),@py(pkoc),@px(0),@py(pkod)
  DEFLINE ,1,0,0
  FOR x=0 TO MAX(ABS(pkoa),ABS(pkob)) STEP pkoe
    LINE @px(x),@py(0)-2,@px(x),@py(0)+2
    LINE @px(-x),@py(0)-2,@px(-x),@py(0)+2
    TEXT @px(x)-4,@py(0)+9,STR$(x)
    TEXT @px(-x)-4,@py(0)+9,STR$(-x)
  NEXT x
  FOR y=0 TO MAX(ABS(pkoc),ABS(pkod)) STEP pkof
    LINE @px(0)-2,@py(y),@px(0)+2,@py(y)
    LINE @px(0)-2,@py(-y),@px(0)+2,@py(-y)
    TEXT @px(0)+7,@py(y)+2,STR$(y)
    TEXT @px(0)+7,@py(-y)+2,STR$(-y)
  NEXT y
  TEXT @px(pkob)-8*LEN(tx$)-8,@py(0)+8,tx$
  TEXT @px(0)-8*LEN(ty$)-8,@py(pkod)+8,ty$
RETURN
PROCEDURE m(k)
  ' print k
  ON k gosub info
  ON k-10 gosub load.dgl,save.dgl,n,load,save,clear,n,delete,n,ende
  ON k-22 gosub vfeldxn,vfeldxp,n,vfeldyn,vfeldyp,n,ox,oy,rennenx,renneny
  ON k-33 gosub koordin,n,n,sh_x,sh_y,sh_x_,sh_y_,sh_x__,sh_y__
  ON k-43 gosub sh_xy,n,n,n,koordin2,n,n,systemset,anfangsw
RETURN
PROCEDURE save
  FILESELECT "SAVE SCREEN ...","./*.xwd","BILD.xwd",name$
  IF name$<>""
    SAVEWINDOW name$
  ENDIF
RETURN
PROCEDURE load
  FILESELECT "LOAD SCREEN ...","./*.xwd","",a$
  IF a$<>""
    IF EXIST(a$)
      OPEN "I",#55,a$
      screen$=SPACE$(lof(#55))
      CLOSE #55
      BLOAD a$,VARPTR(screen$)
      SPUT screen$
    ENDIF
  ENDIF
RETURN
PROCEDURE delete
  FILESELECT "DELETE FILE ...","./*.xwd","",a$
  IF a$<>""
    IF EXIST(a$)
      KILL a$
    ENDIF
  ENDIF
RETURN

PROCEDURE save.dgl
  SGET bild$
  FILESELECT "SAVE DATA ...","./*.dgl","NONAME.dgl",name$
  IF name$<>""
    '
  ENDIF
RETURN
PROCEDURE load.dgl
  FILESELECT "LOAD DATA ...","./*.dgl","",a$
  IF a$<>""
    IF EXIST(a$)
      '
    ENDIF
  ENDIF
RETURN
PROCEDURE clear
  COLOR weiss
  PBOX bx,by,bx+bw,by+bh
  COLOR schwarz
  MENUDEF menue$(),m
RETURN
PROCEDURE ende
  QUIT
RETURN

fdoret%=FORM_DO(dialogadr%,0)
~FORM_DIAL(2,0,0,10,20,x%,y%,w%,h%)
PROCEDURE info
  '  ~form_alert(1,"[0][DIFFGLEI.BAS |Spiel mit Differentialgleichungen.|(c) Markus Hoffmann][ OK ]")
  LOCAL x%,y%,w%,h%,fdoret%
  '
  dialogadr%=rsrc_gaddr(0,info__u)
  ~FORM_CENTER(dialogadr%,x%,y%,w%,h%)
  ~FORM_DIAL(0,0,0,10,20,x%,y%,w%,h%)
  ~FORM_DIAL(1,0,0,10,20,x%,y%,w%,h%)
  ~OBJC_DRAW(dialogadr%,0,8,0,0,bw,bh)
  fdoret%=FORM_DO(dialogadr%,0)
  ~FORM_DIAL(2,0,0,10,20,x%,y%,w%,h%)
  ~FORM_DIAL(3,0,0,10,20,x%,y%,w%,h%)
  IF fdoret%>=0
    DPOKE dialogadr%+24*fdoret%+10,0
  ENDIF
RETURN
PROCEDURE koordin
  LOCAL x%,y%,w%,h%,fdoret%
  '
  dialogadr%=rsrc_gaddr(0,koordin__U)
  ~FORM_CENTER(dialogadr%,x%,y%,w%,h%)
  ~FORM_DIAL(0,0,0,10,20,x%,y%,w%,h%)
  ~FORM_DIAL(1,0,0,10,20,x%,y%,w%,h%)
  @stext(dialogadr%,pxv__U,STR$(pkoxv,6))
  @stext(dialogadr%,pxb__U,STR$(pkoxb,6))
  @stext(dialogadr%,pxsv__U,STR$(pkox_v,6))
  @stext(dialogadr%,pxsb__U,STR$(pkox_b,6))
  @stext(dialogadr%,prwx__U,STR$(pkorwx,6))
  @stext(dialogadr%,prwxs__U,STR$(pkorwx_,6))
  @stext(dialogadr%,pyv__U,STR$(pkoyv,6))
  @stext(dialogadr%,pyb__U,STR$(pkoyb,6))
  @stext(dialogadr%,pysv__U,STR$(pkoy_v,6))
  @stext(dialogadr%,pysb__U,STR$(pkoy_b,6))
  @stext(dialogadr%,prwy__U,STR$(pkorwy,6))
  @stext(dialogadr%,prwys__U,STR$(pkorwy_,6))
  ~OBJC_DRAW(dialogadr%,0,8,0,0,bw,bh)
  fdoret%=FORM_DO(dialogadr%,0)
  pkoxv=VAL(@ob_text$(dialogadr%,pxv__U))
  pkoxb=VAL(@ob_text$(dialogadr%,pxb__U))
  pkox_v=VAL(@ob_text$(dialogadr%,pxsv__U))
  pkox_b=VAL(@ob_text$(dialogadr%,pxsb__U))
  pkorwx=VAL(@ob_text$(dialogadr%,prwx__U))
  pkorwx_=VAL(@ob_text$(dialogadr%,prwxs__U))
  pkoyv=VAL(@ob_text$(dialogadr%,pyv__U))
  pkoyb=VAL(@ob_text$(dialogadr%,pyb__U))
  pkoy_v=VAL(@ob_text$(dialogadr%,pysv__U))
  pkoy_b=VAL(@ob_text$(dialogadr%,pysb__U))
  pkorwy=VAL(@ob_text$(dialogadr%,prwy__U))
  pkorwy_=VAL(@ob_text$(dialogadr%,prwys__U))
  ~FORM_DIAL(2,0,0,10,20,x%,y%,w%,h%)
  ~FORM_DIAL(3,0,0,10,20,x%,y%,w%,h%)
  IF fdoret%>=0
    DPOKE dialogadr%+24*fdoret%+10,0
  ENDIF
RETURN
PROCEDURE stext(a%,n,t$)
  t$=t$+CHR$(0)
  ' print t$
  ' memdump LPEEK(a%+24*n+12),4
  ' memdump LPEEK(LPEEK(a%+24*n+12)),16
  BMOVE LPEEK(LPEEK(a%+24*n+12)),VARPTR(t$),LEN(t$)+1
RETURN
DEFFN ob_text$(adr%,obj%)=@peekstring$(LPEEK(LPEEK(adr%+obj%*24+12)),20)
FUNCTION peekstring$(adr%,n)
  LOCAL t$,a$
  t$=SPACE$(n)
  BMOVE VARPTR(t$),adr%,n
  SPLIT t$,CHR$(0),0,t$,a$
  RETURN t$
ENDFUNCTION

PROCEDURE koordin2                    ! ToDO
  LOCAL x%,y%,w%,h%,fdoret%
  '
  dialogadr%=rsrc_gaddr(0,koordin2__U)
  ~FORM_CENTER(dialogadr%,x%,y%,w%,h%)
  ~FORM_DIAL(0,0,0,10,20,x%,y%,w%,h%)
  ~FORM_DIAL(1,0,0,10,20,x%,y%,w%,h%)
  @stext(dialogadr%,k2xv__U,STR$(t0,6))
  ~OBJC_DRAW(dialogadr%,0,8,0,0,bw,bh)
  fdoret%=FORM_DO(dialogadr%,0)
  ~FORM_DIAL(2,0,0,10,20,x%,y%,w%,h%)
  ~FORM_DIAL(3,0,0,10,20,x%,y%,w%,h%)
  IF fdoret%>=0
    DPOKE dialogadr%+24*fdoret%+10,0
  ENDIF
RETURN
PROCEDURE systemset
  LOCAL t$,a,f$
  t$="System-Settings: ||Integrationsinterval dt="+CHR$(27)+STR$(dt,8,8)+"|"
  ALERT 0,t$,1," OK |CANCEL",a,f$
  IF a=1
    dt=VAL(f$)
  ENDIF
RETURN
PROCEDURE anfangsw
  LOCAL t$,a,f$
  t$="Anfangswerte:|| t0="+CHR$(27)+STR$(t0,8,8)+"|"
  t$=t$+" x0="+CHR$(27)+STR$(x0,8,8)+"|"
  t$=t$+" y0="+CHR$(27)+STR$(y0,8,8)+"|"
  t$=t$+"x'0="+CHR$(27)+STR$(x_0,8,8)+"|"
  t$=t$+"y'0="+CHR$(27)+STR$(y_0,8,8)+"|"
  t$=t$+"x"+CHR$(34)+"0="+CHR$(27)+STR$(x__0,8,8)+"|"
  t$=t$+"y"+CHR$(34)+"0="+CHR$(27)+STR$(y__0,8,8)+"|"

  ALERT 0,t$,1," OK |CANCEL",a,f$
  IF a=1
    SPLIT f$,CHR$(13),0,a$,f$
    t0=VAL(a$)
    SPLIT f$,CHR$(13),0,a$,f$
    x0=VAL(a$)
    SPLIT f$,CHR$(13),0,a$,f$
    y0=VAL(a$)
    SPLIT f$,CHR$(13),0,a$,f$
    x_0=VAL(a$)
    SPLIT f$,CHR$(13),0,a$,f$
    y_0=VAL(a$)
    SPLIT f$,CHR$(13),0,a$,f$
    x__0=VAL(a$)
    SPLIT f$,CHR$(13),0,a$,f$
    y__0=VAL(a$)
  ENDIF
RETURN
PROCEDURE ox
  DEFMOUSE 2
  t=t0
  x=x0
  y=x0
  x_=x_0
  y_=y_0
  pkoa=pkoxv
  pkob=pkoxb
  pkoc=pkox_v
  pkod=pkox_b
  pkoe=pkorwx
  pkof=pkorwx_
  @pkoordinate("x","x'")
  PLOT @px(x),@py(x_)
  REPEAT
    @dgl2
    DRAW TO @px(x),@py(x_)
    SHOWPAGE
  UNTIL MOUSEK
  DEFMOUSE 5
RETURN
PROCEDURE oy
  DEFMOUSE 2
  t=t0
  x=x0
  y=x0
  x_=x_0
  y_=y_0
  pkoa=pkoyv
  pkob=pkoyb
  pkoc=pkoy_v
  pkod=pkoy_b
  pkoe=pkorwy
  pkof=pkorwy_
  @pkoordinate("y","y'")
  PLOT @px(y),@py(y_)
  REPEAT
    @dgl2
    DRAW TO @px(y),@py(y_)
    SHOWPAGE
  UNTIL MOUSEK
  DEFMOUSE 5
RETURN
PROCEDURE rennenx
  @rennen(1)
RETURN
PROCEDURE renneny
  @rennen(2)
RETURN
PROCEDURE rennen(nnU)
  LOCAL nU,iU
  CLR nU
  IF nnU=1
    pkoa=pkoxv
    pkob=pkoxb
    pkoc=pkox_v
    pkod=pkox_b
    pkoe=pkorwx
    pkof=pkorwx_
    @pkoordinate("x","x'")
  ELSE
    pkoa=pkoyv
    pkob=pkoyb
    pkoc=pkoy_v
    pkod=pkoy_b
    pkoe=pkorwy
    pkof=pkorwy_
    @pkoordinate("y","y'")
  ENDIF
  DEFMOUSE 5
  SHOWPAGE
  DO
    TEXT 500,10,"links Nr. "+STR$(nU)+"    Rechts  Start "
    REPEAT
      MOUSE xU,yU,kU
    UNTIL kU
    EXIT IF kU>1
    LINE xU-2,yU,xU+2,yU
    LINE xU,yU-2,xU,yU+2
    f(nU)=@pbx(xU)
    f_(nU)=@pby(yU)
    INC nU
    SHOWPAGE
    REPEAT
      MOUSE xU,yU,kU
    UNTIL kU=0
    EXIT IF nU=100
  LOOP
  REPEAT
    MOUSE xU,yU,kU
  UNTIL kU=0
  DEFLINE ,2,2,2
  '  ON ERROR GOSUB error
  TEXT 500,10," Links fixieren  Rechts beenden"
  DEFMOUSE 2
  DO
    IF MOUSEK=1
      DEFLINE ,6,2,2
    ENDIF
    EXIT IF nU<1
    FOR iU=0 TO nU-1
      IF nnU=1
        CLR y,y_
        x_=f_(iU)
        x=f(iU)
      ELSE
        CLR x,x_
        y_=f_(iU)
        y=f(iU)
      ENDIF
      @dgl2
      IF nnU=1
        f_(iU)=x_
        f(iU)=x
      ELSE
        f_(iU)=y_
        f(iU)=y
      ENDIF
      IF f(iU)<pkoa
        f(iU)=f(nU-1)
        f_(iU)=f_(nU-1)
        DEC nU
      ELSE
        PLOT @px(f(iU)),@py(f_(iU))
      ENDIF
    NEXT iU
    SHOWPAGE
    EXIT IF MOUSEK>1
    REPEAT
    UNTIL MOUSEK=0
    DEFLINE ,1,0,0
  LOOP
  DEFMOUSE 5
  MENU menue$()
RETURN
PROCEDURE vektor(xU,yU)
  GRAPHMODE 3
  x=@pbx(xU)
  y=@pby(yU)
  DEFLINE ,2,2,1
  @dgl2
  DEFMOUSE 5
  LINE xU,yU,@px(@pbx(xU)+(@pbx(xU)-x)*20),@py(@pby(yU)+(@pby(yU)-y)*20)
  SHOWPAGE
  LINE xU,yU,@px(@pbx(xU)+(@pbx(xU)-x)*20),@py(@pby(yU)+(@pby(yU)-y)*20)
  GRAPHMODE 1
RETURN
PROCEDURE vektorfeld(nU,f)
  DEFMOUSE 2
  IF nU=1
    pkoa=pkoxv
    pkob=pkoxb
    pkoc=pkox_v
    pkod=pkox_b
    pkoe=pkorwx
    pkof=pkorwx_
    @pkoordinate("x","x'")
  ELSE
    pkoa=pkoyv
    pkob=pkoyb
    pkoc=pkoy_v
    pkod=pkoy_b
    pkoe=pkorwy
    pkof=pkorwy_
    @pkoordinate("y","y'")
  ENDIF
  FOR xU=bx TO bx+bw STEP 20
    FOR yU=by TO by+bh STEP 20
      CLR x,y,x_,y_
      DEFLINE ,1,1,1
      ' Kreuz
      LINE xU-2,yU,xU+2,yU
      LINE xU,yU-2,xU,yU+2
      DEFLINE ,2,2,1
      IF nU=1
        x=@pbx(xU)
        x_=@pby(yU)
      ELSE
        y=@pbx(xU)
        y_=@pby(yU)
      ENDIF
      @dgl2
      IF nU<>1
        LINE xU,yU,@px(@pbx(xU)+(@pbx(xU)-y)*20),@py(@pby(yU)+(@pby(yU)-y_)*20)
      ELSE
        LINE xU,yU,@px(@pbx(xU)+(@pbx(xU)-x)*20),@py(@pby(yU)+(@pby(yU)-x_)*20)
      ENDIF
      SHOWPAGE
      EXIT IF MOUSEK
    NEXT yU
    EXIT IF MOUSEK
  NEXT xU
  DEFMOUSE 0
  SHOWPAGE
RETURN
PROCEDURE vfeldxp
  @vektorfeld(1,TRUE)
RETURN
PROCEDURE vfeldxn
  @vektorfeld(1,FALSE)
RETURN
PROCEDURE vfeldyp
  @vektorfeld(2,TRUE)
RETURN
PROCEDURE vfeldyn
  @vektorfeld(2,FALSE)
RETURN
'
PROCEDURE gem_init
  LET anfangsw__u=2
  LET aok__u=11
  LET at0__u=2
  LET ax__u=4
  LET axs__u=5
  LET axss__u=6
  LET ay__u=8
  LET ays__u=9
  LET ayss__u=10
  LET fok__u=19
  LET fsyntax__u=20
  LET function__u=0
  LET info__u=6
  LET k2ok__u=44
  LET k2rwt1__u=9
  LET k2rwt2__u=23
  LET k2rwx__u=15
  LET k2rwy__u=29
  LET k2t1b__u=13
  LET k2t1v__u=7
  LET k2t2b__u=27
  LET k2t2v__u=21
  LET k2xb__u=14
  LET k2xv__u=8
  LET k2xyx__u=37
  LET k2xyxb__u=41
  LET k2xyxv__u=35
  LET k2xyy__u=43
  LET k2xyyb__u=42
  LET k2xyyv__u=36
  LET k2yb__u=28
  LET k2yv__u=22
  LET kok__u=30
  LET koordin__u=1
  LET koordin2__u=3
  LET prwx__u=9
  LET prwxs__u=15
  LET prwy__u=23
  LET prwys__u=29
  LET pxb__u=13
  LET pxsb__u=14
  LET pxsv__u=8
  LET pxv__u=7
  LET pyb__u=27
  LET pysb__u=28
  LET pysv__u=22
  LET pyv__u=21
  LET syntax__u=4
  LET sysdt__u=9
  LET sysok__u=10
  LET syst__u=4
  LET system__u=5
  '
  ' ---------------------
  prorecource$="diffglei.rsc"
  IF not EXIST(prorecource$)
    ' ~FORM_ALERT(1,"[3]["+prorecource$+" not found ! |or memory full !][CANCEL]")
    ' QUIT
    IF @rsrc_load(prorecource$)=0
      ~FORM_ALERT(1,"[3]["+prorecource$+" not found ! |or memory full !][CANCEL]")
      QUIT
    ENDIF
  ELSE
    RSRC_LOAD prorecource$
  ENDIF
RETURN
FUNCTION rsrc_load(n$)
  LOCAL cwu,chu,acwu,achu,duml,diffglei$

  ' output of inline.bas for X11-Basic 09.02.2015
  ' diffglei.rsx 9130 Bytes. (compressed: 3006 Bytes, 32%)
  diffglei$=""
  diffglei$=diffglei$+"^7N\74I($^]ID[c0YS^\_Ya.Q16-JM>DSG*96IOBD$+^IcTN]TC($%X%3VO`7<AS"
  diffglei$=diffglei$+"YS]$@Qa3GQ`B>Ta:01<69R97OJJGb?FI&HR2baE@KO4=c%5H&b/:0PUU40W..UZ_"
  diffglei$=diffglei$+"'C\T3^M7?5]1[Z;>,\\N-,ZG<X^OXc^S`HNa7;J7aE8'=;E)6T0[:=F\-5$>IRJb"
  diffglei$=diffglei$+"W`]%I0AK>[2WX9VJKK1\N8WI&Q:[cG\T9T[D`R]8K[8(`M)QV,W-;@1HBDY4J/B_"
  diffglei$=diffglei$+"]ZX*`9-G<4*M_UN.WTKS&,[(AA/2JT4DccM0J&->:6AZQ2-5bC_7\%9*/c/R=AE?"
  diffglei$=diffglei$+"c9<)>33DM?+^2Y&cXcYI>U--?WED<C>-&6)<1+-`_`LSAEETF>E:><D-LT?D%RT7"
  diffglei$=diffglei$+"^JC0^9+%]MH.@C'-1/+AO4Z1=9.?NDG=T@R.cIUHW1V9>O4%,;Y=3DA[?-%&<61V"
  diffglei$=diffglei$+"C:(Cc[UEOGRWIB1Z;QE2_*9F<%>G'+--++V%E@(J\N]2+4V42CWa_Q+91_W$OWO'"
  diffglei$=diffglei$+"SG_EM[^DJ85$.?<JYMRGWDU,>-8G?J0E;2XRZb]bc\$Ub>I$D@=JG]5O*cA04U8C"
  diffglei$=diffglei$+"7./A3@+/(/UTOAG`TcX=W,>9CIE\/^/^*W]\UI,I]CX7B;-;3>,?]*8Y^';YR$>^"
  diffglei$=diffglei$+"bN'DBPL9)QS@:N/7AYG48.>V[/^1VW6>0?&H$IO2;_:C0C.[:%OQ.7+Z$AKE/B&9"
  diffglei$=diffglei$+"@JNLNU<3MU\5,bVT'W5W>OV_*3(V]'S`=B1CAY0J(63^FPKaT_/I8[H=J`Na-<a`"
  diffglei$=diffglei$+"LK/5?2ELI.'M:$1),1L96=):P789ECa6+T*%5GII48P>I]\FGY;BO/YbcKT50?[R"
  diffglei$=diffglei$+"696%3I8J>S/X[CBN$\4c^C2BA5+Q;EJ`.7CL+%`B1%S(-Da.MJN)80Yc;:$O]-:E"
  diffglei$=diffglei$+"1]SG`NGH4F=BZDSZ1F+30)?'N]S;<D9`X5AJL_4H3:Y2+P2%&5W(&1F&G2E7YaJ8"
  diffglei$=diffglei$+"O.4MBAMcE8bN4^:K[557*K[W/QM=SK>Y6RMZ`*D$6P01:9*IRFc)T(JFSXO[V.XY"
  diffglei$=diffglei$+"**=^1AUYTJF?CEL1,,W?&B8cY&6+?CJ2Ub`c+NB`N=_%V:&PNU=SZI2+aO9]Wc*P"
  diffglei$=diffglei$+"*\CWZ59/NJVS&:(HJ.;(cD<?@6c,9(F398Q`&aF?]^C;D&FLSC`L>R6&6>N8cY^7"
  diffglei$=diffglei$+"9.8JKXTV-)O:/S2\6F3cG+\>9;<YTR7H%_C0D/\0>K;K2<K_MD]EG;-Jb8&70-J9"
  diffglei$=diffglei$+"][%.49_PW8H9><.9;HGMcL50%*DDT@9`b;6.^bF;?H>DO'=SR^@c(bBYVb>[5^D3"
  diffglei$=diffglei$+"]RF>a*F+LF;>U;-T0>C'%.DBPV[)VVC'TX4BXb%K+>H-ZC,QO;WWY7cP&]CN%$K>"
  diffglei$=diffglei$+"NC-9JL5(AOPZ$V2_13D%(D:`8M5S.W1HPMFIH%(`>-Ua4&F83@OE8IN,Y>Y/Ca2,"
  diffglei$=diffglei$+"=`*I3`,EQ2T4^6?>F[%@[T9_'%J-?MbRS',9E91/Q5V@0$@84S'B_2,$>\])`'-R"
  diffglei$=diffglei$+"F%D(59+PG.N@WY*^S-BO4F:A43)b,C'/H;(HM=9HLa0&aNFM$H''-2cZ3GIJB%(J"
  diffglei$=diffglei$+"J9U4NU,)R)=cLb-1B,0S`97b84\0@\U1*;6@@'R6,_6S]??.^F(;$$)'Z4T.)%)*"
  diffglei$=diffglei$+"KRH6LIK+R\7,])77MW&F1:)K8@<82$4E<B$J]C_,F`/[=F6(Z*RcABKO`Qb@O>V."
  diffglei$=diffglei$+"]BZ@O1-FZQK)ZQ(P]LTL@>['ZK5OHTHJ&,=;O>BF_8&L6V=GC9b<B151M^4S?<E7"
  diffglei$=diffglei$+"W<CM_G-XU%`KSA?_35?2caAa6%D0`cY48MH2^=H?=]47XIcQB^8A,[L*>;1H]VIC"
  diffglei$=diffglei$+"O\Z*I>[X='JG5<7>8ZbbU9BH<)CC_X/KQ_3U_J1+HC[ZU=\-GQGN$L=?A.\U=9U$"
  diffglei$=diffglei$+"A[LMFcGU$&6W&3ZA4U@4VY.4P6O4-UA/6/,\)K78C.I7QB43(]&/cF'DQ<WU^P30"
  diffglei$=diffglei$+"/R_SGE/GJ;ZB$`*BHQ11P+=1C)3A4N5G3X3E:(>)>&R/?Y6APA?V`7-?)BN_D/P&"
  diffglei$=diffglei$+"LF,bF,6UF?=SY<aQ6\%X8BITYB+_%BP$*M<YI@bEOE6@$LX0DCV\61-[B06(Ab$`"
  diffglei$=diffglei$+"-J)I@5)2.a9R1bL'**@2'M1K`'K+-KP8`2.O;<(]]'?+2F]1G&KN=[)C6T(I(AU5"
  diffglei$=diffglei$+"<c@M?>W4EcTUH7M;@L9@P&?606HBO0B@:)O<\U_KWGKDZHKDW::1b%L1O\KQ4$S\"
  diffglei$=diffglei$+"MU*9XIANZ'1b5]KSJH/0)]REc.=$'IDW/GYJZM:T:N_\J+(0Ga$A[&[B>c3,$.1Q"
  diffglei$=diffglei$+"L])@67@YL%-J,<V5a0@+C/a:.NJ^X(::1^11&AE-9IK0]>L8.BL*aP`=TZD?@*b("
  diffglei$=diffglei$+"LW\`N=M8+b[QT=1H1-$\QC(>D[`4:5<1(&[Y_[/`^QSXOGXOTNG$1Z46[)aMXZ9D"
  diffglei$=diffglei$+"W%^'3\N[^AN=_OG3PL9M+Z+?/0L&(?5K.;IO09GR0CD<5H(%XO(-B9899E[Q*0:+"
  diffglei$=diffglei$+"L0U:91I9KQC-<3>PPU6$'&))N0RU3[4XSC9J]6:4:%TDA94`R+:<-3P?TV?9c4]Q"
  diffglei$=diffglei$+"G<CaP.1?[0-CO_>Y?5ZUR9FZRKa9C3Q5JV^I^Z)).DDY>UJH>,.?Z,P.X2WW*DE1"
  diffglei$=diffglei$+"P;;'T?L7O(YU\BbI21+=7$6FbE\Fb90ZHMV&4]M-BSAO=P-bL3:SRB$11D=U(;T$"
  diffglei$=diffglei$+"HGL3G[9VB&$LU'cH::HD9,=5/VI1UDA$RNU=SNZK?,?:KbMF?[RGEYaD-KZ_J_QH"
  diffglei$=diffglei$+"T=S9-aS?\MBKS<S=PZ>2.R9;E+-c*1PS7P1:+A*(&T%70]E$Yc`[S80R8AFV(cXT"
  diffglei$=diffglei$+"OG)>^[2H@O'I]`[GRZNS;CJ2XP<%[9[:N0>bII%7$J6JcL'V?,B$J&WX66M.V](Q"
  diffglei$=diffglei$+"C_?WbcS047A6,)A@^RaMCN`9*Q9<DUX=P3VC%8MN/<(>,/YT.23$'YLTFGEBJ4AL"
  diffglei$=diffglei$+"YZb@J%2:OH%`X0EAE)(.&S[0)I:00cY*]P_:-?@I&<TPY)`?@@/PENc*STa-O>+3"
  diffglei$=diffglei$+";6$6W;bI%2X(520N;ZVa&JY-:T[)P8%&'WT.Ga$(;,><(0RQM;^JG\A?DPOUUN;P"
  diffglei$=diffglei$+"HXTY&^2L<M`8$F?-Rc$9L*Z\Bc_8W[)P(\:&\:ITT;C\GU=[7aA@M+<9:F3b4^K8"
  diffglei$=diffglei$+"`@69WO^+)<.1@Uba2J/1)2<>E0:G^Z':FaA$S^LUX_H*FT80)JJX^R@C593LP7Sa"
  diffglei$=diffglei$+"D\TF?8Y\4$S)`$$F'YR-Z/:44RcS4c[V(V_S\E4'E,1S]9Q*%1&P8.6]]&A0;M6Z"
  diffglei$=diffglei$+"`A&610)3b;FU(>M?9(U?UAI[7?&VU:-L:0bQ`UJ@&cMM81GG)\2K5'N^AJ;6>[?:"
  diffglei$=diffglei$+",`?WOF&+)LL*\5)aMc'YR74QYJ5:.a_(=:(_VU)<E83A:JT5=6>Z@WV6%3:`4NUA"
  diffglei$=diffglei$+"KPQ)IH6;[3ER>^8_TEaX1TL9BXaF32Q<>cJ%11=FO75^[-(SADIZKR-BT=.QL-CA"
  diffglei$=diffglei$+"5C)@<10bXX`]E=^^LJ23DRc?S870_C?L0WO&[\TP<\F:>4a6$7]Y&_^`P8:H1\?4"
  diffglei$=diffglei$+"W$XEH,;MNGNXU$08]GN?7.9L\*_'DNE.*$?X0YQ?VVWU%IcPC,']_]B,JG=2I(%C"
  diffglei$=diffglei$+"86DM&M;/D*)U)[)6(_Q=G/-(`&aA,@c?ZE4I]b&H;<UNQ$__^5.5c2T.EZK-J`',"
  diffglei$=diffglei$+"OQ=A]DA/[1b$7(S=O]c/)N+(.HEVa`*@DACb9)@LP5`\I?*&8+2L<M47FbZAQ)I-"
  diffglei$=diffglei$+"WYOK4X7P`ZP6EO[N+D$V&][VKD='WJ@P%\5'a<&Vb*%<]b?,2;`M0S=@0>/N,<(2"
  diffglei$=diffglei$+"@XV-JLKPZ\AS(F85UX-YOZ0.&VJXTI[Z3E@aS^%4%>]IaK%+Ta5S[:cIV**$<K\:"
  diffglei$=diffglei$+"<1Y@\bNJ:,<@@%GLS?;b0;4bTV+IBFaV]N<N1cJP*/GTO?>bEX*)V'IXF)Ca@*>Y"
  diffglei$=diffglei$+"-9EIG;Xb7,3[MX8_U9Ba*Qc.@.*\4&9M-b@L\Z0T:_V,K[AB0M5\RR.M;&+EJ7YQ"
  diffglei$=diffglei$+")$%R+bKIKIO8,>%M$%QU:@?aJ82-HE$A.0/@%9<b)@K?aJ\2WD`7'29?&H0:Y4DN"
  diffglei$=diffglei$+")*/PDGM8S`-T8;W=)F;WLLP<\6BG[H`a^C\G[L$$"
  diffglei_rsx$=UNCOMPRESS$(INLINE$(diffglei$))
  rscinlineadresse%=VARPTR(diffglei_rsx$)
  ritreetab%=@DSWAP(DPEEK(rscinlineadresse%+18))+rscinlineadresse%
  ritreeanzu=@DSWAP(DPEEK(rscinlineadresse%+22))
  rifreestrtab%=@DSWAP(DPEEK(rscinlineadresse%+10))+rscinlineadresse%
  rifreestranzu=@DSWAP(DPEEK(rscinlineadresse%+30))
  PRINT "anztree=";ritreeanzu
  PRINT "anzfreestr=";rifreestranzu
  PRINT "treeadro=";ritreetab%-rscinlineadresse%
  acwu=8
  achu=16
  cwu=1
  chu=1
  IF rscinlineadresse%<>0x0004087c
    @rsrc_convert(-0x0004087c,cwu,chu,acwu,achu)
    BSAVE "diffglei.rsc",rscinlineadresse%,LEN(diffglei_rsx$)
    RETURN 1
  ELSE
    RETURN 3
  ENDIF
ENDFUNC

DEFFN DSWAP(a)=(a AND 255)*256+(a AND 0xff00)/256
DEFFN LSWAP(a)=CVL(REVERSE$(MKL$(a)))

DEFFN OB_TYPE(adr%,n)=@DSWAP(DPEEK(adr%+24*n+6))
DEFFN OB_FLAGS(adr%,n)=@DSWAP(DPEEK(adr%+24*n+8))
DEFFN OB_SPEC(adr%,n)=@LSWAP(LPEEK(adr%+24*n+12))

PROCEDURE rsrc_convert(delta%,cwu,chu,acwu,achu)
  LOCAL tu,ou,tree%,typeu,ad%
  FOR tu=0 TO ritreeanzu-1
    tree%=@LSWAP(LPEEK(ritreetab%+4*tu))+rscinlineadresse%
    ou=-1
    REPEAT
      INC ou
      IF cwu<>acwu OR chu<>achu
        IF acwu<>0
          DPOKE tree%+24*ou+16,@DSWAP(SHL((@DSWAP(DPEEK(tree%+24*ou+16)) MOD acwu),8)+@DSWAP(DPEEK(tree%+24*ou+16)) DIV acwu)
          DPOKE tree%+24*ou+18,@DSWAP(SHL((@DSWAP(DPEEK(tree%+24*ou+18)) MOD achu),8)+@DSWAP(DPEEK(tree%+24*ou+18)) DIV achu)
          DPOKE tree%+24*ou+20,@DSWAP(SHL((@DSWAP(DPEEK(tree%+24*ou+20)) MOD acwu),8)+@DSWAP(DPEEK(tree%+24*ou+20)) DIV acwu)
          DPOKE tree%+24*ou+22,@DSWAP(SHL((@DSWAP(DPEEK(tree%+24*ou+22)) MOD achu),8)+@DSWAP(DPEEK(tree%+24*ou+22)) DIV achu)
        ENDIF
        '       OB_X(treel,ou)=SHR((OB_X(treel,ou)),8)+BYTE(OB_X(treel,ou))*cwu
        '       OB_Y(treel,ou)=SHR((OB_Y(treel,ou)),8)+BYTE(OB_Y(treel,ou))*chu
        '       OB_W(treel,ou)=SHR((OB_W(treel,ou)),8)+BYTE(OB_W(treel,ou))*cwu
        '       OB_H(treel,ou)=SHR((OB_H(treel,ou)),8)+BYTE(OB_H(treel,ou))*chu
      ENDIF
      typeu=@OB_TYPE(tree%,ou)
      IF typeu>20 AND typeu<33 AND NOT (typeu=25 OR typeu=27)
        ad%=@OB_SPEC(tree%,ou)+delta%
        LPOKE tree%+ou*24+12,@LSWAP(ad%)
        SELECT typeu
        CASE 21,22,29,30,31
          LPOKE ad%+rscinlineadresse%,@LSWAP(@LSWAP(LPEEK(ad%+rscinlineadresse%))+delta%)
          LPOKE ad%+4+rscinlineadresse%,@LSWAP(@LSWAP(LPEEK(ad%+4+rscinlineadresse%))+delta%)
          LPOKE ad%+8+rscinlineadresse%,@LSWAP(@LSWAP(LPEEK(ad%+8+rscinlineadresse%))+delta%)
        CASE 23
          LPOKE ad%+rscinlineadresse%,@LSWAP(@LSWAP(LPEEK(ad%+rscinlineadresse%))+delta%)
        CASE 24
          LPOKE ad%+rscinlineadresse%,@LSWAP(@LSWAP(LPEEK(ad%+rscinlineadresse%))+delta%)
          LPOKE ad%+4+rscinlineadresse%,@LSWAP(@LSWAP(LPEEK(ad%+4+rscinlineadresse%))+delta%)
        ENDSELECT
      ENDIF
    UNTIL BTST(@OB_FLAGS(tree%,ou),5)
  NEXT tu
RETURN

FUNCTION rsrc_gaddr(artu,nru)
  adr%=-1
  SELECT artu
  CASE 0
    IF nru>-1 AND nru<ritreeanzu
      adr%=@LSWAP(LPEEK(ritreetab%+nru*4))+rscinlineadresse%
    ENDIF
  CASE 15
    IF nru>-1 AND nru<rifreestranzu
      adr%=@LSWAP(LPEEK(rifreestrtab%+nru*4))+rscinlineadresse%
    ENDIF
  ENDSELECT
  RETURN adr%
ENDFUNC
menudata:
DATA "..","  DIFFGLEI Info","--------------------"
DATA "- 1","- 2","- 3","- 4","- 5"
DATA "- 6",""
DATA "Datei","  Werte laden ...","  Werte speichern ..."
DATA "-----------------------","  Bild laden ...","  Bild speichern ...","  Screen löschen ","-----------------------","  Datei löschen ...","-----------------------","  Quit ",""
DATA "Phasenraum","  X Phasenraumfeld normiert","  X Phasenraumfeld proportional"
DATA "--------------------------------"
DATA "  Y Phasenraumfeld normiert","  Y Phasenraumfeld proportional"
DATA "--------------------------------"
DATA "  X Orbit","  Y Orbit"
DATA "  X Phasenrennen","  Y Phasenrennen"
DATA "--------------------------------"
DATA "  Koordinaten setzten",""
DATA "Bewegung","  X(t)","  Y(t)","  X'(t)","  Y'(t)","  X''(t)","  Y''(t)","------------------------","  (X,Y)","  Kraftfeld (X,Y)","  Satelliten aussetzten","------------------------","  Koordinaten setzten",""
DATA "Parameter","  System-Settings  ","  Anfangswerte",""
DATA "***"
