f$="fish_24.xwd"
' fileselect "Image laden...","./*.xwd","",f$
if len(f$)
if exist(f$)
open "I",#1,f$
len=lof(#1)
close #1
t$=space$(len)
bload f$,varptr(t$)
print len(t$)
put 0,0,t$
endif
endif
schwarz=get_color(0,0,0)
for i=0 to 10
  color get_color(i*6553,32000+i*6553/2,i*6553)
  pcircle 350+i/3,20-i/3,20-2*i
next i
get 330,0,40,40,kugel$
text 100,170,"Copyarea GET und PUT mit XBASIC V.1.03   (c) Markus Hoffmann"
num=6
i=0
dim dx(num),dy(num),x(num),y(num),hinter$(num),kugel$(num),m(num)
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
  next i
  exit if inp?(-2)
loop

get 0,0,20,20,t$
print len(t$)
for i=0 to 10
put 0,20+i*20,t$
put 100,20,t$
next i
vsync
fileselect "Image speichern ...","/tmp/","kugel.dump",f$
if len(f$)
if not exist(f$)
  bsave f$,varptr(kugel$),len(kugel$)
endif
endif
quit

function pot(d)
return -exp(-d^2/20/20)*100
endfunc
