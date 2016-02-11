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
for j=0 to 400 step 10
  for i=0 to 640 step 50
    text i,j,str$(i)+"/"+str$(j)
  next i
next j
for i=0 to 10
  color get_color(i*6553,32000+i*6553/2,i*6553)
  pcircle 150+i/3,20-i/3,20-2*i
next i
get 130,0,40,40,kugel$

color schwarz
pbox 0,0,640,400
color gelb
deftext 1
text 100,170,"Copyarea GET und PUT mit XBASIC V.1.03   (c) Markus Hoffmann"
for i=0 to 5
  put 100+i*40,100,kugel$
next i


num=6
i=0
dim dx(num+1),dy(num+1),x(num+1),y(num+1),hinter$(num+1),kugel$(num+1),m(num+1)
arrayfill m(),1
m(2)=0.7
arrayfill dy(),1
for i=0 to num
  x(i)=random(600)
  y(i)=random(350)
  dx(i)=(random(400)-200)/50
next i


for j=0 to num
  color schwarz
  pbox 310,0,380,40

  for i=0 to 10
    color get_color(i*6553+j*(65535-i*6553)/num,32000+i*6553/2,i*6553)
    pcircle 350+i/3,20-i/3,m(j)*(20-2*i)
  next i
  get 350-m(j)*20,20-m(j)*20,2*m(j)*20,2*m(j)*20,kugel$(j)
next j

vsync
' pause 5

do
  for i=0 to num
    x(i)=max(0,min(x(i),599))
    y(i)=max(0,min(y(i),359))
  
    get x(i),y(i),40,40,hinter$(i)
  next i
  color schwarz
  for i=0 to num
    graphmode 1
    pcircle x(i)+20,y(i)+20,20
    graphmode 2
    put x(i),y(i),kugel$(i)
  next i
  vsync
  graphmode 1
  for i=0 to num
    put x(i),y(i),hinter$(i)
    add x(i),dx(i)
    add y(i),dy(i)
    if x(i)>=635-40 or x(i)<=5
      dx(i)=-dx(i)
    endif
    if y(i)>=395-40 or y(i)<=5
      dy(i)=-dy(i)
    endif
    ' Kraefte
    kraftx=0
    krafty=0
    for k=0 to num
      if k<>i
        l=sqrt((x(i)-x(k))^2+(y(i)-y(k))^2)
        r=@pot(l)
	add kraftx,r*(x(k)-x(i))/l
	add krafty,r*(y(k)-y(i))/l
      endif
    next k
    add dx(i),kraftx*m(i)
    add dy(i),krafty*m(i)
    
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
FOR i=0 to 10
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
