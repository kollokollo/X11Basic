uel=2^8
dim a(uel)
open "I",#1,"mondata_delta_0.5A.dat"
for i=0 to 480
  lineinput #1,t$
next i
for i=0 to uel
  lineinput #1,t$
  for  j=0 to 5
    wort_sep t$," ",1,a$,t$
  next j
  a(i)=(val(a$)-1000)/4096*2
next i
close
sizew ,min(uel,1224),400

color get_color(10000,10000,10000)
pbox 0,0,min(uel,1224),400

  color get_color(65535,32000,0)
  scope a(),0,-2000,300
vsync
fft a()
  color 1
  for i=0 to uel step 2
    line i,300,i,300-100000/uel*sqrt(a(i)^2+a(i+1)^2)
  next i
  color get_color(65535,65535,0)
  for i=0 to uel step 2
    draw to i,300-10000/uel*atan2(a(i+1),a(i))
  next i
vsync
