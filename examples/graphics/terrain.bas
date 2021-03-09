' It's taken from some javascript code here:
' http://www.playfuljs.com/realistic-terrain-in-130-lines/
'
' Though I've modified it somewhat;
'
' Automatic conversion of tg.bas (SpecBAS) to X11-Basic
' by bas2x11basic V.1.11

' ----- Start of program -----
REM TERRAIN GENERATOR by ZXDUNNY
CLS
PRINT "Terrain Generator"
RANDOMIZE TIMER
DIM palette%(256),waterpalette%(256)
PRINT "make palette."
FOR F%=0 TO 255
  palette%(f%)=COLOR_RGB(f%/256,f%/256,f%/256)
  waterpalette%(f%)=COLOR_RGB(f%/600,f%/400,f%/200)
NEXT f%
SHOWPAGE
PAUSE 0.4
GET_GEOMETRY 1,bx%,by%,bw%,bh%
PRINT bx%,by%,bw%,bh%
x0%=bw%/2
y0%=bh%/2
GOTO L200
PROCEDURE L30
  CLR N,p%
  IF x%>=s% AND y%>=s%
    ADD N,L(x%-s%,y%-s%)
    INC p%
  ENDIF
  IF x%+s%<=mx% AND y%>s%
    ADD N,L(x%+s%,y%-s%)
    INC p%
  ENDIF
  IF x%+s%<=mx% AND y%+s%<=mx%
    ADD N,L(x%+s%,y%+s%)
    INC p%
  ENDIF
  IF x%>=s% AND y%+s%<=mx%
    ADD N,L(x%-s%,y%+s%)
    INC p%
  ENDIF
  L(x%,y%)=(N/p%)+O
RETURN
PROCEDURE L80
  CLR N,p%
  IF y%>=s%
    ADD N,L(x%,y%-s%)
    INC p%
  ENDIF
  IF x%+s%<=mx%
    ADD N,L(x%+s%,y%)
    INC p%
  ENDIF
  IF y%+s%<=mx%
    ADD N,L(x%,y%+s%)
    INC p%
  ENDIF
  IF x%>=s%
    ADD N,L(x%-s%,y%)
    INC p%
  ENDIF
  L(x%,y%)=(N/p%)+O
RETURN
PROCEDURE DIVIDE(sz%)
  IF sz%<2
    RETURN
  ENDIF
  s%=sz%/2
  SCL=ROUGHNESS*sz%
  FOR y%=s% TO mx% STEP sz%
    FOR x%=s% TO mx% STEP sz%
      O=SCL*(2*RND()-1)
      @L30
      INC pct%
    NEXT x%
  NEXT y%
  FOR y%=0 TO mx% STEP s%
    FOR x%=((y%+s%) MOD sz%) TO mx% STEP sz%
      O=SCL*(2*RND()-1)
      @L80
      INC pct%
    NEXT x%
  NEXT y%
  @DIVIDE(s%)
RETURN
L200:
TEXT 0,20,"Terrain generator with X11-Basic"
PRINT AT(2,1);"terraforming...."
DETAIL=10
ROUGHNESS=0.8
WATER=0.4
sz%=(2^DETAIL)+1
mx%=sz%-1
DIM L(sz%,sz%)
DIM N(4)
L(0,0)=RND()*mx%
L(mx%,mx%)=RND()*mx%
L(mx%,0)=RND()*mx%
L(0,mx%)=RND()*mx%
CLR pct%
mxp%=sz%*sz%
'EVERY 1,status
@DIVIDE(mx%)
'AFTER 1,status
PRINT "...please wait..."
REM DRAW THE LANDSCAPE
PRINT "calculate visible parts"
mxp%=0
mn%=9999
FOR x%=0 TO mx%
  FOR y%=0 TO mx%
    mn%=MIN(L(x%,y%),mn%)
    mxp%=MAX(L(x%,y%),mxp%)
  NEXT y%
NEXT x%
WL=(mxp%-mn%)*WATER
PRINT "Terrain: [";mn%;":";mxp%;"] Water: ";WL
FOR x%=0 TO mx%
  FOR y%=0 TO mx%
    SUB L(x%,y%),mn%
  NEXT y%
NEXT x%
m%=mx%-1
PRINT AT(2,1);"...drawing......."
FOR y%=0 TO m%
  FOR x%=0 TO m%
    tx%=x%
    ty%=y%
    Z=MAX(WL,L(x%,y%))
    @L370
    TLX=RX
    TLY=RY
    tx%=x%+1
    ty%=y%+1
    IF x%<m% AND y%<m%
      Z=MAX(WL,L(x%+1,y%+1))
    ELSE
      Z=WL*0.75
    ENDIF
    ' Z=IIF(X<M AND Y<M,MAX(WL,L(X+1,Y+1)),WL*0.75)
    @L370
    BRX=RX+1
    BRY=RY
    IF TLY>BRY
      GOTO L350
    ENDIF
    i%=16+(16*(y%=m%))
    IF x%<m% AND y%<m%
      i%=MIN(254,MAX(0,((L(x%+1,y%)-L(x%,y%))*50)+128))
    ENDIF
    IF L(x%,y%)<=WL   ! Waterline
      I%=(384+I%)/7
      COLOR waterpalette%(i%)
    ELSE
      COLOR palette%(i%)
    ENDIF
    PBOX TLX,TLY,BRX+1,BRY
    L350:
  NEXT x%
  SHOWPAGE
NEXT y%
GOTO L200
STOP
PROCEDURE L370
  PX=(sz%+tx%-ty%)/2
  PY=(tx%+ty%)/2
  Z=sz%*0.5-Z+PY*0.75
  X1=(PX-sz%*0.5)*6
  Y1=(sz%-PY)*0.010+1
  RX=x0%+X1/Y1
  RY=y0%+Z/Y1
RETURN
' ----- End of program -----
PROCEDURE status
  PRINT AT(3,1);INT((pct%/mxp%)*100);"% Completed"
RETURN
