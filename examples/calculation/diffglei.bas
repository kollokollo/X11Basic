'
' Vorversion DGL2 20.10.87 16.4.89   DGL2A
' UNI WUPPERTAL
'
'  V. 1.001 ATARI-ST  (c) Markus Hoffmann Begonnen April 1993
'                                       Letzte Bearbeitung: Mai 1994
'

@init
b=0.01213             ! Konstanten in der Diffgleichung
a=1-b                 ! Werden hier definiert
dt=0.005              ! Genauigkeit fÅr numerische Integration
MENUDEF menue$(),m
DEFMOUSE 5
DO
  MENU
  pause 0.01
  mouse mx,my,mk
  IF mk and pkob
    @vektor(mx,my)
  ENDIF
LOOP
'
' Beispiele fÅr Gewîhnliche Differentialgleichungen
'
' x__=-1              ! Freier Fall
' x__=-2*p*x1-q*x2    ! GEDéMPFTER OSZILLATOR
' x__=-SINQ(DEG(x2))  ! Mathematisches Pendel
' x__=-SIN(x2)-0.2*x1 ! gedÑmpftes mathematisches Pendel
' x__=-1/(x2)^2       ! Gravitation
'
PROCEDURE dgl2
  z1=((x-a)^2+y^2)^1.5
  z2=((x+b)^2+y^2)^1.5
  '  x__=x+2*y_-a*(x+b)/z2-b*(x-a)/z1              ! ZWEITE Ableitung
  x__=-SIN(x)-0.2*x_ ! gedÑmpftes mathematisches Pendel
  x_=x_+dt*x__           ! erste Ableitung
  x=x+x_*dt           ! Bewegungsgleichung
  ' Jetzt eine 2te Funktion
  y__=y-2*x_-a*(y)/z2-b*(y)/z1                  ! ZWEITE Ableitung
  y_=y_+dt*y__           ! erste Ableitung
  y=y+y_*dt           ! Bewegungsgleichung
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
  DIM f(100),f_(100)   ! FÅr Phasenrennen
  '
  rot=get_color(65530,0,0)
  gelb=get_color(65530,40000,0)
  grau=get_color(65530/2,65530/2,65530/2)
  weiss=get_color(65530,65530,65530)
  schwarz=get_color(0,0,0)
  lila=get_color(65530,0,65530)
  blau=get_color(10000,10000,65530)
  gruen=get_color(0,30000,0)
  bx=0
  by=16
  bw=640
  bh=384
  color weiss
  pbox bx,by,bx+bw,by+bh
  color schwarz
  RESTORE menudata
  FOR i=0 TO 80
    read t$
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
  READ pkoyv,pkoyb,pkoy_v,pkoy_b,pkorwy,pkorwy_
RETURN
procedure n
  ~form_alert(1,"[3][Funktion nicht implementiert !][ OH ]")
return
function kx(x)
  return bw/(kkob-kkoa)*(x-kkoa)
endfunc
function ky(y)
  return -bh/(kkod-kkoc)*(y+kkoc)+by
endfunc
function px(x)
return bw/(pkob-pkoa)*(x-pkoa)
endfunc
function py(y)
  return -bh/(pkod-pkoc)*(y+pkoc)+by
endfunc
function pbx(x)
  return x/bw*(pkob-pkoa)+pkoa           !bwU/(pkob-pkoa)*(x-pkoa)
endfunc
function pby(y)
  return -(y-by)/bh*(pkod-pkoc)-pkoc    !-bhU/(pkod-pkoc)*(y+pkoc)+byU
endfunc
function bx(x)
  return bw/(kob-koa)*(x-koa)
endfunc
function by(y)
  return -bh/(kod-koc)*(y+koc)+by
endfunc
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
    vsync
  WEND
  
  DEFMOUSE 5
  vsync
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
    vsync
    EXIT IF t>et 
  LOOP
  DEFMOUSE 5
  vsync
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
    vsync
    EXIT IF t>et 
  LOOP
  DEFMOUSE 5
  vsync
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
    vsync
    EXIT IF t>et 
  LOOP
  DEFMOUSE 5
  vsync
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
    vsync
    EXIT IF t>et 
  LOOP
  DEFMOUSE 5
  vsync
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
    vsync
    EXIT IF t>et 
  LOOP
  DEFMOUSE 5
  vsync
RETURN
y_data__:
DATA -1,7,-2,2,1,1
PROCEDURE sh_xy
  DEFMOUSE 2
  text 500,10,"Maustaste=stop"
  RESTORE xy_data
  @kkoordinate("x","y")
  RESTORE aw_data
  READ x,y,x_,y_
  DRAW @kx(x),@ky(y) to @kx(x),@ky(y)
  while mousek=0
    DRAW  TO @kx(x),@ky(y)
    vsync
    @dgl2
  wend
  DEFMOUSE 5
  vsync
RETURN
xy_data:
DATA -1.5,1.5,-1,1,.5,.5
px_data:
DATA -5,5,-3,3,1,1
py_data:   ! x: von bis y: von bis step x step y
DATA             -5,  5,    -3,  3,     1,     1
PROCEDURE bkoordinate(tx$,ty$)
  LOCAL x,y
  DEFLINE 1,1,0,1
  DEFTEXT 1,0,0,4
  READ koa,kob,koc,kod,koe,kof
  LINE @bx(koa),@by(0),@bx(kob),@by(0)
  LINE @bx(0),@by(koc),@bx(0),@by(kod)
  DEFLINE 1,1,0,0
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
  DEFLINE 1,1,0,1
  DEFTEXT 1,0,0,4
  READ kkoa,kkob,kkoc,kkod,kkoe,kkof
  LINE @kx(kkoa),@ky(0),@kx(kkob),@ky(0)
  LINE @kx(0),@ky(kkoc),@kx(0),@ky(kkod)
  DEFLINE 1,1,0,0
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
  DEFLINE 1,1,0,1
  DEFTEXT 1,0,0,4
  LINE @px(pkoa),@py(0),@px(pkob),@py(0)
  LINE @px(0),@py(pkoc),@px(0),@py(pkod)
  DEFLINE 1,1,0,0
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
  print k
  on k gosub info
  on k-10 gosub load.dgl,save.dgl,n,load,save,clear,n,delete,n,ende
  on k-22 gosub vfeldxn,vfeldxp,n,vfeldyn,@vfeldyp,n,ox,oy,rennenx,renneny
  on k-33 gosub koordin,n,n,sh_x,sh_y,sh_x_,sh_y_,sh_x__,sh_y__
  on k-43 gosub sh_xy,n,n,n,koordin2,n,n,systemset,anfangsw
RETURN
PROCEDURE save
  FILESELECT "SAVE SCREEN ...","./*.xwd","BILD.xwd",name$
  IF name$<>""
    savewindow name$
  ENDIF
RETURN
PROCEDURE load
  FILESELECT "LOAD SCREEN ...","./*.xwd","",a$
  IF a$<>""
    IF EXIST(a$)
      open "I",#55,a$
      screen$=space$(lof(#55))
      close #55
      BLOAD a$,varptr(screen$)
      sput screen$
    ENDIF
  ENDIF
RETURN
PROCEDURE delete
  FILESELECT "DELETE FILE ...","./*.xwd","",a$
  IF a$<>""
    IF EXIST(a$)
      system "rm -f "+a$
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
  color weiss
  pbox bx,by,bx+bw,by+bh
  color schwarz
  MENUDEF menue$(),m
RETURN
PROCEDURE ende
  quit
RETURN

PROCEDURE info
  ~form_alert(1,"[0][DIFFGLEI.BAS |(c) Markus Hoffmann][ OK ]")
RETURN
PROCEDURE koordin
  LOCAL xU,yU,wU,hU,fdoretU,b$
  '
  ~@rsrc_gaddr(0,koordin__U,dialogadr%)
  ~FORM_CENTER(dialogadr%,xU,yU,wU,hU)
  '  ~FORM_DIAL(0,0,0,10,20,xU,yU,wU,hU)
  ~FORM_DIAL(1,0,0,10,20,xU,yU,wU,hU)
  GET xU,yU,xU+wU,yU+hU,b$
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
  ~OBJC_DRAW(dialogadr%,0,8,xU,yU,wU,hU)
  fdoretU=FORM_DO(dialogadr%,0)
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
  PUT xU,yU,b$
  '  ~FORM_DIAL(3,0,0,10,20,xU,yU,wU,hU)
  ~FORM_DIAL(2,0,0,10,20,xU,yU,wU,hU)
  ~OBJC_CHANGE(dialogadr%,fdoretU,0,xU,yU,wU,hU,0,0)
RETURN
PROCEDURE koordin2
  LOCAL xU,yU,wU,hU,fdoretU,b$
  '
  ~@rsrc_gaddr(0,koordin2__U,dialogadr%)
  ~FORM_CENTER(dialogadr%,xU,yU,wU,hU)
  '  ~FORM_DIAL(0,0,0,10,20,xU,yU,wU,hU)
  ~FORM_DIAL(1,0,0,10,20,xU,yU,wU,hU)
  GET xU,yU,xU+wU,yU+hU,b$
  @stext(dialogadr%,k2xv__U,STR$(t0,6))
  ~OBJC_DRAW(dialogadr%,0,8,xU,yU,wU,hU)
  fdoretU=FORM_DO(dialogadr%,0)
  '  ~FORM_DIAL(3,0,0,10,20,xU,yU,wU,hU)
  PUT xU,yU,b$
  ~FORM_DIAL(2,0,0,10,20,xU,yU,wU,hU)
  ~OBJC_CHANGE(dialogadr%,fdoretU,0,xU,yU,wU,hU,0,0)
RETURN
PROCEDURE systemset
  LOCAL t$,a,f$
  t$="System-Settings: ||Integrationsinterval dt="+chr$(27)+str$(dt,8,8)+"|"
  alert 0,t$,1," OK |CANCEL",a,f$
  if a=1
    dt=val(f$)
  endif
RETURN
PROCEDURE anfangsw
  LOCAL t$,a,f$
  t$="Anfangswerte:|| t0="+chr$(27)+str$(t0,8,8)+"|"
  t$=t$+" x0="+chr$(27)+str$(x0,8,8)+"|"
  t$=t$+" y0="+chr$(27)+str$(y0,8,8)+"|"
  t$=t$+"x'0="+chr$(27)+str$(x_0,8,8)+"|"
  t$=t$+"y'0="+chr$(27)+str$(y_0,8,8)+"|"
  t$=t$+"x"+chr$(34)+"0="+chr$(27)+str$(x__0,8,8)+"|"
  t$=t$+"y"+chr$(34)+"0="+chr$(27)+str$(y__0,8,8)+"|"

  alert 0,t$,1," OK |CANCEL",a,f$
  if a=1
    wort_sep f$,chr$(13),0,a$,f$
    t0=val(a$)
    wort_sep f$,chr$(13),0,a$,f$
    x0=val(a$)
    wort_sep f$,chr$(13),0,a$,f$
    y0=val(a$)
    wort_sep f$,chr$(13),0,a$,f$
    x_0=val(a$)
    wort_sep f$,chr$(13),0,a$,f$
    y_0=val(a$)
    wort_sep f$,chr$(13),0,a$,f$
    x__0=val(a$)
    wort_sep f$,chr$(13),0,a$,f$
    y__0=val(a$)
  endif
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
    DRAW  TO @px(x),@py(x_)
    vsync
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
    DRAW  TO @px(y),@py(y_)
    vsync
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
  vsync
  DO
    text 500,10,"links Nr. "+str$(nU)+"    Rechts  Start "
    REPEAT
      MOUSE xU,yU,kU
    UNTIL kU
    EXIT IF kU=512
    LINE xU-2,yU,xU+2,yU
    LINE xU,yU-2,xU,yU+2
    f(nU)=@pbx(xU)
    f_(nU)=@pby(yU)
    INC nU
    vsync
    REPEAT
      MOUSE xU,yU,kU
    UNTIL kU=0
    EXIT IF nU=100
  LOOP
  REPEAT
    MOUSE xU,yU,kU
  UNTIL kU=0
  DEFLINE 1,2,2,2
  ON ERROR GOSUB error
  text 500,10," Links fixieren  Rechts beenden"
  DEFMOUSE 2
  DO
    IF MOUSEK=256
      DEFLINE 1,6,2,2
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
        plot @px(f(iU)),@py(f_(iU))
        
      ENDIF
    NEXT iU
    vsync
    EXIT IF MOUSEK=512
    REPEAT
    UNTIL MOUSEK=0
    DEFLINE 1,1,0,0
  LOOP
  DEFMOUSE 5
  MENU menue$()
RETURN
PROCEDURE vektor(xU,yU)
  GRAPHMODE 3
  cls        
  dump
  x=@pbx(xU)
  y=@pby(yU)
  print "Hallo"
  DEFLINE 1,2,2,1
  @dgl2
  print "Hallo"
  DEFMOUSE 5
  line xU,yU,@px(@pbx(xU)+(@pbx(xU)-x)*20),@py(@pby(yU)+(@pby(yU)-y)*20)
  VSYNC
  line xU,yU,@px(@pbx(xU)+(@pbx(xU)-x)*20),@py(@pby(yU)+(@pby(yU)-y)*20)
 
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
      DEFLINE 1,1,1,1
      ' Kreuz
      LINE xU-2,yU,xU+2,yU
      LINE xU,yU-2,xU,yU+2
      DEFLINE 1,2,2,1
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
      vsync
      EXIT IF MOUSEK
    NEXT yU
    EXIT IF MOUSEK
  NEXT xU
  DEFMOUSE 0
  vsync
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

menudata:
DATA "..","  DIFFGLEI Info","--------------------"
data "- 1","- 2","- 3","- 4","- 5"
data "- 6",""
DATA "Datei","  Werte laden ...","  Werte speichern ..."
data "-----------------------","  Bild laden ...","  Bild speichern ...","  Screen lˆschen ","-----------------------","  Datei lˆschen ...","-----------------------","  Quit ",""
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
