'Mandala by Antoni gual
'for Rel's 9 LINER contest at QBASICNEWS.COM  1/2003
'------------------------------------------------------------------------
TEXT 0,16,"Mandala by Antoni gual"
DIM palette%(16)
FOR i%=0 TO 15
  palette%(i%)=COLOR_RGB((i% + 1)/16,0,0)
NEXT i%
' 1 SCREEN 12
2:
v%=RND()*20+10
DIM VX%(v%+2),VY%(v%+2)
FOR d1%=-1 TO v%
  FOR d2%=d1%+1 TO v%
    IF d1% = -1 
      VX%(d2%) = 320 + (SIN(6.283185 * (d2% / v%)) * 200) 
    ELSE 
      COLOR palette%(v% MOD 16)
      LINE VX%(d1%),VY%(d1%),VX%(d2%),VY%(d2%)
    endif
    IF d1% = -1
      VY%(d2%) = 200 + (COS(6.283185 * (d2% / v%)) * 200)
      endif
  NEXT d2%
NEXT d1%
SHOWPAGE
PAUSE 0.1
IF LEN(INKEY$) = 0
  GOTO 2
ENDIF
