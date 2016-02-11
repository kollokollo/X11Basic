' sdl plasma example on: https://gist.github.com/stevenlr/824019
' ported to CMLua by Cybermonkey 08/2014
' ported to OxygenBasic by Peter Wirbelauer  o7.o8.2o14
' ported to X11Basic by vsync o8.o8.2o14
bw%=200
bh%=120
SIZEW ,bw%,bh%
DIM c(256)
FOR x%=0 TO 255
  r=255-((SIN(PI*2*x%/255)+1)*127)
  c(x%)=COLOR_RGB(r/256,(SIN(PI*2*x%/127)+1)/4,1-r/256)
NEXT x%
WHILE LEN(INKEY$)=0
  t=TIMER
  k1=(1+COS(1.2*f))*bh%/2
  k2=(1+SIN(0.8*f))*bw%/2
  FOR y%=0 TO bh%
    FOR x%=0 TO bw%
      a=k1-y%
      b=k2-x%
      COLOR c((SIN(x%/50+f+2*y%/bh%)+SIN(SQR(b*b+a*a)/50))*63+127)
      PLOT x%,y%
    NEXT x%
    ' VSYNC
  NEXT y%
  fps=1/(TIMER-t)
  TEXT 8,24,USING$(f,"##.#")+" "+USING$(fps,"##.#")
  f=f+0.2
  SHOWPAGE
WEND
QUIT
