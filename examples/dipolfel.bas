' #################################### (c) Markus Hoffmann 1993
' ## Wie entsteht ein Dipolfeld ... ##
' ####################################

echo off

k=20
sizew ,k*8+8,k*8+8
clearw
DIM p(k,k),s(k,k)
ARRAYFILL p(),1
p(2,2)=1
DO
  p(k/2,k/2)=1
  p(k/2,k/2+1)=0
  @show
  vsync
  @make
LOOP
end
PROCEDURE show
  FOR x=1 TO k-1
    FOR y=1 TO k-1
      IF INT(p(x,y)*32)<>INT(s(x,y)*32)
        color p(x,y)*250
        PBOX x*8,y*8,x*8+7,y*8+7
      ENDIF
    NEXT y
  NEXT x
RETURN
PROCEDURE make
  FOR x=1 TO k-1
    FOR y=1 TO k-1
      s(x,y)=(p(x,y)+p(x+1,y)+p(x-1,y)+p(x,y+1)+p(x,y-1))/5
    NEXT y
  NEXT x
  SWAP s(),p()
RETURN
