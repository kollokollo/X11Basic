' OPTIMIZED  :) rotozoomer in 9 lines by Antoni Gual
'for Rel's 9 LINER contest at QBASICNEWS.COM  1/2003
'------------------------------------------------------------------------
' converted to X11-Basic

SIZEW 1,320,200
DO
  ANG=ANG+0.08
  CS%=COS(ANG)*ABS(SIN(ANG))*128
  SS%=SIN(ANG)*ABS(SIN(ANG))*128
  FOR Y%=-100 TO 99
    FOR X%=-160 TO 159
      COLOR ((X%*CS%-Y%*SS%) AND (Y%*CS%+X%*SS%)) DIV 128
      PLOT X%+160,Y%+100
    NEXT X%
  NEXT Y%
  VSYNC
  EXIT IF LEN(INKEY$)
LOOP
END
' ----- End of program -----
