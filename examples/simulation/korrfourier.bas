' Testet die eingebaute Fast-Fourier-Transformation 
' (geht nur, wenn man numerical recepies besitzt.)
'
echo off
l=2^10
c1=4
c2=86
korrnr=val(param$(2))
clr ghgh
dim a(l),b(l),diff(l),c(l),d(l),s(l)
sizew ,min(l,1224),400
open "I",#1,"/sgt/elsa/matlab/messungen/mondata_rampe_neu.dat"
  lineinput #1,t$
for  j=0 to 2+korrnr*2
    wort_sep t$," ",1,a$,t$
next j
a(0)=-val(a$)/4096*2
wort_sep t$," ",1,a$,t$
s(0)=-val(a$)/4096*2
 
for i=0 to l/2
  if i>50
  lineinput #1,t$
  for  j=0 to 2+korrnr*2
    wort_sep t$," ",1,a$,t$
  next j
  a(i)=-val(a$)/4096*2
     wort_sep t$," ",1,a$,t$
  s(i)=-val(a$)/4096*2

  else
  a(i)=a(0)
  s(i)=s(0)
  endif
  a(l-i)=a(i)
  s(l-i)=s(i)
  b(l-i)=a(l-i)
  c(l-i)=a(l-i)
  b(i)=a(i)
  c(i)=a(i)
next i
close
@uebertragungsf

do
  color get_color(10000,10000,10000)
  pbox 0,0,min(l,1224),400
  color get_color(65535,32000,0)
  scope a(),1,-300,300
  color get_color(0,65535,32000)  
  scope b(),0,-300,300
  color get_color(65535,0,0)  
  scope s(),0,-300,300
  
  fft a()
  color get_color(65535,65535,65535)
  for i=0 to l step 2
    line i/2,300,i/2,300-1000/l*sqrt(a(i)^2+a(i+1)^2)
  next i

  line c1/2,0,c1/2,10
  line c2/2,0,c2/2,10
  for i=0 to l-1 step 2
    d(0)=a(0)
    c1=a(i+1)
    c2=a(i+2)
    u1=uea(int(i/l*uel/2)*2+1)
    u2=uea(int(i/l*uel/2)*2+2)

  if i<l/16
  
    d(i+1)=c1/sqrt(u1^2+u2^2)
    d(i+2)=c2/sqrt(u1^2+u2^2)
  else
    d(i+1)=0
    d(i+2)=0
  endif
  if i<l
    a(i+1)=(c1*u1-c2*u2)
    a(i+2)=(c1*u2+c2*u1)
   ' a(i+1)=c1*sqrt(u1^2+u2^2)
   ' a(i+2)=c2*sqrt(u1^2+u2^2)
  else
    a(i+1)=0
    a(i+2)=0

  endif
  next i
   color get_color(65535,0,0)
  for i=0 to l step 2
    line i/2,300,i/2,300+1000/l*sqrt(a(i)^2+a(i+1)^2)
  next i
 
  
  
  fft a(),-1
  fft d(),-1

  color get_color(0,32535,65535)
  scope a(),0,-600/l,300
  for i=0 to l
    diff(i)=-(a(i)/l*2-b(i))
  next i


' open "O",#1,"testme"
' for i=0 to l
' f%=a(i)*20000
' out #1,f%
' next i
' close #1

  for g=1 to 10
    color get_color(65535,65535,0)
    scope a(),b(),1,2/l*g,200,-1*g,200
    vsync
    color get_color(0,0,0)
    scope a(),b(),1,2/l*g,200,-1*g,200
  next g

  for i=0 to l
  '  a(i)=a(i)/l*2+diff(i)
    a(i)=d(i)/l*2
  next i
  inc ghgh
  alert 0,"1",1," OK ",d
  exit if ghgh=2
loop
alert 0,"Fertig",1," OK ",d
quit

function si(x)
return x mod pi
endfunc

procedure uebertragungsf
  local i,a$,t$
  usewindow 2
  uel=2^10
  dim uea(uel)
  open "I",#1,"/sgt/elsa/matlab/messungen/mondata_puls_0.0A_neu.dat"
  for i=0 to 900
    lineinput #1,t$
  next i
  for i=0 to 500
    lineinput #1,t$
    for  j=0 to 3+korrnr*2
      wort_sep t$," ",1,a$,t$
    next j
    uea(i)=(val(a$))/800
  next i
  for i=500 to uel
    uea(i)=uea(499)
  next i
  close #1
  sizew ,min(uel,1224),400

  color get_color(10000,10000,10000)
  pbox 0,0,min(uel,1224),400

  color get_color(65535,32000,0)
  scope uea(),0,-2000,300
  vsync
  fft uea()
  color get_color(65535,65535,65535)
  for i=0 to uel step 2
    uea(i)=uea(i)/sqrt(uea(1)^2+uea(2)^2)
    uea(i+1)=uea(i+1)/sqrt(uea(1)^2+uea(2)^2)
    draw to i,300,i,300-300*(0.9+0.15*log(sqrt(uea(i+2)^2+uea(i+1)^2)))

  next i
  color get_color(65535,65535,0)
  basephas=atan2(uea(2),uea(1))
  print "basephase=";basephas
  for i=0 to uel-1 step 2
    pp=atan2(uea(i+2),uea(i+1))
    circle i,300-30*pp,2
uea(i+2)=cos(-basephas+pp)
uea(i+1)=sin(-basephas+pp)
  next i
  vsync
  usewindow 1
return

