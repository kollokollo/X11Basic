weiss=COLOR_RGB(1,1,1)
grau=COLOR_RGB(0.3,0.3,0.3)
rot=COLOR_RGB(1,0,0)
schwarz=COLOR_RGB(0,0,0)
gelb=COLOR_RGB(1,1,0)
bx=16
by=32
chw=16
chh=24
CLEARW 1
COLOR gelb,schwarz
TEXT 8,20,"LTEXT scaling demonstration"
COLOR rot
DEFLINE ,3
LTEXT 250,00,"ABCDEFGHI"
LTEXT 250,60,"JKLMNOPQR"
LTEXT 250,120,"STUVWXYZ{"
LTEXT 250,180,"abcdefghi"
LTEXT 250,240,"jklmnopqr"
LTEXT 250,300,"stuvwxyz€"
@showchar(0)
COLOR gelb
TEXT 28,200,"press any key"
SHOWPAGE
DO
  KEYEVENT ,a
  @showchar(a)
  SHOWPAGE
LOOP
END

PROCEDURE showchar(c)
  DEFLINE ,1
  COLOR schwarz
  PBOX bx,by,bx+chw*10,by+chh*13
  COLOR grau
  FOR i=0 TO 10 STEP 0.1
    LINE bx,by+chh*i,bx+chw*10,by+chh*i
  NEXT i
  FOR i=0 TO 10 STEP 0.1
    LINE bx+chw*i,by,bx+chw*i,by+chh*10
  NEXT i
  COLOR weiss
  FOR i=0 TO 13
    LINE bx,by+chh*i,bx+chw*10,by+chh*i
    TEXT bx+chw*10+5,by+chh*i+4,STR$(i*10)
  NEXT i
  FOR i=0 TO 10
    LINE bx+chw*i,by,bx+chw*i,by+chh*10
  NEXT i
  COLOR rot
  DEFLINE ,5
  DEFTEXT ,chw/10,chh/10
  LTEXT bx,by,CHR$(c)
RETURN
