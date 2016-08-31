
' savescreen Test for X11-basic (c) Markus hoffmann 2008
'
'
' In the framebuffer-version (TomTom etc) the file format is
' .bmp 24 Bit
'
FOR i=0 TO 32
  FOR j=0 TO 20
    CIRCLE i*20,j*20,8
    IF ODD(j+i)
      PCIRCLE i*20,j*20,3
    ENDIF
  NEXT j
NEXT i

CIRCLE 100,100,30
CIRCLE 200,200,60

PCIRCLE 200,300,70

a$=SYSTEM$("locate .bmp")
WHILE LEN(a$)
  SPLIT a$,CHR$(10),0,t$,a$
  IF EXIST(t$)
    OPEN "I",#1,t$
    t$=INPUT$(#1,LOF(#1))
    PUT 0,0,t$
    CLOSE
    VSYNC
    PAUSE 1
  ENDIF
WEND
QUIT
