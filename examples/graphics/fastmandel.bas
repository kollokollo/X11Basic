' MANDELBROT by Antoni Gual 2003  
' for Rel's 9 LINER contest at QBASICNEWS.COM  1/2003
'------------------------------------------------------------------------
' converted to X11-Basic
'
bw%=400
bh%=400
starttime=TIMER
DO
  CLR iter%,im,re
  x%=(x%+123) MOD bw%*bh%
  EXIT IF x%=0
  IF RANDOM(300)=50
    SHOWPAGE
  ENDIF
L3:
  im2=im*im
  IF iter%
    im=2*re*im+((x% DIV bw%)/150-1.3) 
    re=re*re-im2+((x% MOD bw%)/150-2.1) 
  ENDIF
  INC iter%
  IF im2>4 OR iter%>254
    color iter%*1234567
    PLOT x% MOD bw%, x% DIV bw%
  ELSE 
    GOTO L3 
  ENDIF
LOOP
SHOWPAGE
PRINT "";TIMER-starttime;" seconds."
PRINT "Press any key..."
~INP(-2)
END

