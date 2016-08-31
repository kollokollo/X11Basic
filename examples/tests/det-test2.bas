' Test of the Determinal Array function
' for X11-Basic V.1.22    MH 2014
'

DIM a(3,3)
CLR odds,zeros

FOR i=0 TO 2500000
  'A(0,0)=i AND 255
  'A(0,1)=shr(i,8) AND 255
  'A(1,0)=shr(i,16) AND 255
  'A(1,1)=shr(i,24) AND 255
  A(0,0)=RANDOM(256)
  A(0,1)=RANDOM(256)
  A(0,2)=RANDOM(256)
  A(1,0)=RANDOM(256)
  A(1,1)=RANDOM(256)
  A(1,2)=RANDOM(256)
  A(2,0)=RANDOM(256)
  A(2,1)=RANDOM(256)
  A(2,2)=RANDOM(256)

  d=DET(a()) mod 256

  IF ODD(d)
    INC odds
  ENDIF
  IF d=0
    INC zeros
  ENDIF
  IF (i MOD 10000)=1
    PRINT i,INT(zeros/i*1000)/10;"%",INT(odds/i*10000)/100;"%"
  ENDIF
NEXT i
