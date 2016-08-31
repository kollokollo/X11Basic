' Test of the DEFLINE Line type and style function.
'
'
'

CLEARW
FOR j=0 TO 8
  TEXT 0,20+i*32+j*32,STR$(i)+STR$(j)
  DEFLINE j,i
  LINE 20,20+i*32+32*j,180,20+i*32+32*j
  VSYNC
NEXT j
FOR j=0x11 TO 0xff
  IF (j AND 15)=0
    INC j
  ENDIF
  PRINT j
  TEXT 200,20+j,STR$(i)+STR$(j)
  DEFLINE j
  LINE 200,20+j,380,20+j
  VSYNC
NEXT j
FOR j=0x100 TO 0x200
  IF (j AND 15)=0
    INC j
  ENDIF
  PRINT j
  TEXT 400,20+j-0x100,STR$(i)+STR$(j)
  DEFLINE j
  LINE 400,20+j-0x100,580,20+j-0x100
  VSYNC
  PAUSE 0.1
NEXT j
SHOWPAGE
END
