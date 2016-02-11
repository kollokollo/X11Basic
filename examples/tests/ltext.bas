weiss=color_rgb(1,1,1)
grau=color_rgb(0.3,0.3,0.3)
rot=color_rgb(1,0,0)
schwarz=color_rgb(0,0,0)
gelb=color_rgb(1,1,0)
bx=16
by=32
chw=16
chh=24
clearw 1
color gelb,schwarz
text 8,20,"LTEXT scaling demonstration"
color rot
defline ,3
ltext 250,00,"ABCDEFGHI"
ltext 250,60,"JKLMNOPQR"
ltext 250,120,"STUVWXYZ{"
ltext 250,180,"abcdefghi"
ltext 250,240,"jklmnopqr"
ltext 250,300,"stuvwxyz€"
@showchar(0)
color gelb
text 28,200,"press any key"
  SHOWPAGE
do
  keyevent ,a
  @showchar(a)
  SHOWPAGE

loop
end


procedure showchar(c)
  defline ,1
color schwarz
pbox bx,by,bx+chw*10,by+chh*13
color grau
for i=0 to 10 step 0.1
  line bx,by+chh*i,bx+chw*10,by+chh*i
next i
for i=0 to 10 step 0.1
  line bx+chw*i,by,bx+chw*i,by+chh*10
next i
color weiss
for i=0 to 13
  line bx,by+chh*i,bx+chw*10,by+chh*i
  text bx+chw*10+5,by+chh*i+4,str$(i*10)
next i
for i=0 to 10
  line bx+chw*i,by,bx+chw*i,by+chh*10
next i

color rot
defline ,5
deftext ,chw/10,chh/10

ltext bx,by,chr$(c)


return
