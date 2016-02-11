' Demonstration of polyline and polymark
' X11Basic (c) Markus Hoffmann


dim polyline$(1000)
clr anzpolyline

color get_color(65535/2,65535/2,65535/2)
pbox 0,0,640,400

nn=100

for j=0 to 10
typ=random(4)
t$=chr$(typ)
t$=t$+mki$(nn)
for i=0 to nn-1
  t$=t$+mkf$(50+80*cos(j/40*2*pi*i/nn))+mkf$(50+80*sin(2*pi*i/nn))
next i
polyline$(anzpolyline)=t$
inc anzpolyline
next j

for i=0 to anzpolyline-1
  @draw_polyline(polyline$(i))
next i
vsync
vsync
pause 10
quit
procedure draw_polyline(p$)
  local n,typ,i
  typ=asc(left$(p$))
  n=cvi(mid$(p$,2,4))
  dim px(n),py(n)
  for i=0 to n-1
    px(i)=cvf(mid$(p$,4+8*i,4))
    py(i)=cvf(mid$(p$,4+8*i+4,4))
  next i
   if typ=0
     color get_color(65535/4,65535/4,65535/4)
     defline ,1,2
     polyline n,px(),py(),200,200
   else if typ=1
     color get_color(65535/4,65535/4,65535/4)
     defline ,6,2
     polyline n,px(),py(),200,200
     color get_color(65535/1.5,65535/1.5,65535/1.5)
     defline ,4,2
     polyline n,px(),py(),200,200
   else if typ=2
     color get_color(65535,65535,0)
     defline ,8,2
     polyline n,px(),py(),200,200
     color get_color(65535,0,0)
     defline ,6,2
     polyline n,px(),py(),200,200
     color get_color(65535/1.5,65535/1.5,65535/1.5)
     defline ,1,2
     polyline n,px(),py(),200,200
   else if typ=3
     color get_color(65535,65535,0)
     defline ,1,2
     defmark ,3,5
     polymark n,px(),py(),200,250
  endif
return
