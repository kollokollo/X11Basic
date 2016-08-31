' deffill <rule>,<style>,<pattern>

weiss=COLOR_RGB(1,1,1)
rot=COLOR_RGB(1,0.5,0.5)
schwarz=COLOR_RGB(0,0,0)
COLOR weiss
PBOX 0,0,640,400
COLOR schwarz,weiss
TEXT 10,300,"DEFFILL-Test with X11-Basic (c) Markus Hoffmann"
FOR j=0 TO 5
  FOR i=0 TO 7
    COLOR schwarz
    DEFFILL ,2,i+j*8
    PBOX j*32,i*32,j*32+32,i*32+32
    COLOR GET_COLOR((i+j*8)*4000,32000,32000)
    PCIRCLE 320+j*50,i*64,32
    COLOR weiss
    DEFFILL ,0
    PBOX j*32,i*32+18,j*32+16,i*32+32
    COLOR schwarz
    TEXT 4+j*32,i*32+30,STR$(i+j*8)
    COLOR rot
    BOX j*32,i*32,j*32+32,i*32+32
  NEXT i
  SHOWPAGE
NEXT j
SHOWPAGE
END
