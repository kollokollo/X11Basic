' Testet die eingebaute Fast-Fourier-Transformation 
' (geht nur, wenn man numerical recepies besitzt.)
'

l=2^11
c1=4
c2=86

dim a(l),b(l)
sizew ,l,400
for i=0 to l
  a(i)=200/100*@si(3*i/512*2*pi)+i/100*sin(20*i/512*2*pi)
  b(i)=a(i)
next i

if form_alert(2,"[2][Do you have full X11-Basic |Version (with NR)?][yes|no]")=2
  print "sorry..."
  quit
endif

for c2=c1 to c1+100 step 2
  print c2
  color get_color(65535,32000,0)
  scope a(),1,-10,300
  fft a()
  color 1
  for i=0 to l step 2
    line i/2,300,i/2,300-100/l*sqrt(a(i)^2+a(i+1)^2)
  next i
  line c1/2,0,c1/2,10
  line c2/2,0,c2/2,10
  for i=c1 to c2
    a(i)=0
  next i
  fft a(),-1
  color get_color(0,32535,65535)
  scope a(),0,-20/l,300

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
    a(i)=b(i)
  next i
  color get_color(0,0,0)
  pbox 0,0,l,400
next c2
alert 0,"Fertig",1," OK ",d
quit

function si(x)
return x mod pi
endfunction
