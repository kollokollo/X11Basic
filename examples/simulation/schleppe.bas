i=0
sizew 1,600,800
pbox 0,0,640,800
istwert=0
sollwert=0

' dt=1 millisek
dt=1

tau=50*dt
fakt=1-exp(ln(0.1)/tau)
print fakt

p=8
i=1
d=0

p=12.5
i=1.25
d=0

a=p+d/dt+i*dt
b=2*d/dt-p
c=d/dt

do
metasollwert=@f(i)
add sollwert,(metasollwert-istwert)*a+h1*b+h2*c
' sollwert=metasollwert


h2=h1
h1=(metasollwert-istwert)

add istwert,(sollwert-istwert)*fakt
color get_color(65535,0,0)
circle 20+istwert,i,3
color get_color(0,65535,0)
circle 20+sollwert,i,3
color get_color(0,0,65535)
circle 20+metasollwert,i,3
 inc i
 altsollwert=sollwert
 metaaltsollwert=metasollwert
vsync
if i>800
i=0
endif
loop
quit

function f(n)
if n>200
return 0
else if n<50
return 20
else if n>100
return 320
endif
return (n-50)*6+20
endfunc
