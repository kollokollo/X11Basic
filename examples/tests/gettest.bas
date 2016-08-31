f$="fish.xwd"
RANDOMIZE TIMER
' fileselect "Image laden...","./*.xwd","",f$
IF LEN(f$)
  IF EXIST(f$)
    OPEN "I",#1,f$
    len=LOF(#1)
    CLOSE #1
    t$=SPACE$(len)
    BLOAD f$,VARPTR(t$)
    PRINT LEN(t$)
    PUT 0,0,t$
  ENDIF
ENDIF
schwarz=GET_COLOR(0,0,0)
gelb=GET_COLOR(65535,65535,0)
COLOR gelb
FOR j=0 TO 400 STEP 10
  FOR i=0 TO 640 STEP 50
    TEXT i,j,STR$(i)+"/"+STR$(j)
  NEXT i
NEXT j
FOR i=0 TO 10
  COLOR get_color(i*6553,32000+i*6553/2,i*6553)
  PCIRCLE 150+i/3,20-i/3,20-2*i
NEXT i
GET 130,0,40,40,kugel$

COLOR schwarz
PBOX 0,0,640,400
COLOR gelb
DEFTEXT 1
TEXT 100,170,"Copyarea GET und PUT mit XBASIC V.1.03   (c) Markus Hoffmann"
FOR i=0 TO 5
  PUT 100+i*40,100,kugel$
NEXT i

num=6
i=0
DIM dx(num+1),dy(num+1),x(num+1),y(num+1),hinter$(num+1),kugel$(num+1),m(num+1)
ARRAYFILL m(),1
m(2)=0.7
ARRAYFILL dy(),1
FOR i=0 TO num
  x(i)=RANDOM(600)
  y(i)=RANDOM(350)
  dx(i)=(RANDOM(400)-200)/50
NEXT i

FOR j=0 TO num
  COLOR schwarz
  PBOX 310,0,380,40

  FOR i=0 TO 10
    COLOR get_color(i*6553+j*(65535-i*6553)/num,32000+i*6553/2,i*6553)
    PCIRCLE 350+i/3,20-i/3,m(j)*(20-2*i)
  NEXT i
  GET 350-m(j)*20,20-m(j)*20,2*m(j)*20,2*m(j)*20,kugel$(j)
NEXT j

VSYNC
' pause 5

DO
  FOR i=0 TO num
    x(i)=MAX(0,MIN(x(i),599))
    y(i)=MAX(0,MIN(y(i),359))
    GET x(i),y(i),40,40,hinter$(i)
  NEXT i
  COLOR schwarz
  FOR i=0 TO num
    GRAPHMODE 1
    PCIRCLE x(i)+20,y(i)+20,20
    GRAPHMODE 2
    PUT x(i),y(i),kugel$(i)
  NEXT i
  VSYNC
  GRAPHMODE 1
  FOR i=0 TO num
    PUT x(i),y(i),hinter$(i)
    ADD x(i),dx(i)
    ADD y(i),dy(i)
    IF x(i)>=635-40 OR x(i)<=5
      dx(i)=-dx(i)
    ENDIF
    IF y(i)>=395-40 OR y(i)<=5
      dy(i)=-dy(i)
    ENDIF
    ' Kraefte
    kraftx=0
    krafty=0
    FOR k=0 TO num
      IF k<>i
        l=sqrt((x(i)-x(k))^2+(y(i)-y(k))^2)
        r=@pot(l)
        ADD kraftx,r*(x(k)-x(i))/l
        ADD krafty,r*(y(k)-y(i))/l
      ENDIF
    NEXT k
    ADD dx(i),kraftx*m(i)
    ADD dy(i),krafty*m(i)

    ' Zusammenstoesse
    '   if i<num
    '     for k=i+1 to num
    '       vx=x(k)-x(i)
    '       vy=y(k)-y(i)
    '       if vx^2+vy^2<40^2
    '         s=vx*dx(k)+vy*dy(k)
    '         s=s/sqrt(vx^2+vy^2)/sqrt(dx(k)^2+dy(k)^2)
    '         w=2*acos(s)
    '     dx(k)=cos(w)*dx(k)+sin(w)*dy(k)
    '     dy(k)=-sin(w)*dx(k)+cos(w)*dy(k)
    '     dx(i)=cos(w)*dx(i)+sin(w)*dy(i)
    '     dy(i)=-sin(w)*dx(i)+cos(w)*dy(i)
    '     dx(i)=-dx(i)
    '     dy(i)=-dy(i)
    '     dx(k)=-dx(k)
    '     dy(k)=-dy(k)
    '   endif
    ' next k
    ' endif
  NEXT i
  EXIT IF INP?(-2)
LOOP

GET 0,0,20,20,t$
PRINT LEN(t$)
FOR i=0 TO 10
  PUT 0,20+i*20,t$
  PUT 100,20,t$
NEXT i
VSYNC
FILESELECT "Image speichern ...","/tmp/","kugel.dump",f$
IF LEN(f$)
  IF NOT EXIST(f$)
    BSAVE f$,VARPTR(kugel$),LEN(kugel$)
  ENDIF
ENDIF
QUIT

FUNCTION pot(d)
  RETURN -EXP(-d^2/20/20)*100
ENDFUNC
