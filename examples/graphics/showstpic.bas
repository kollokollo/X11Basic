'
'  zeigt ein ATARI-ST Monochrom-Bild an (32000 Bytes)
' (c) Markus Hoffmann
'
'

echo off
dim a(18000),b(18000)
arrayfill a(),0
arrayfill b(),0
fileselect "Datei auswaehlen:","./*.pic","f1.pic",f$
open "I",#1,f$
x=0
y=0
count=0
while y<400
  a=inp(#1)
  if a<>0
  
    for i=0 to 7
      
      if btst(a,i)
     '   plot x-i+8,y
	a(count)=x-i+8
	b(count)=y
	inc count
      endif
     next i
   
   if (count mod 1000)<5 and count>2000
     @showit
   endif
   endif
  add x,8
if x>640
sub x,640
inc y
'vsync
print y,count
endif
wend
@showit
close #1
alert 0,"Fertig !",1," OK ",balert
quit
procedure showit
for t=1 to 15
  color get_color(65535,65535,10000)
  scope b(),a(),1,t/10,200-200*t/10,t/10,320-320*t/10
  color get_color(65535,0,65535)
  scope a(),b(),1,t/10,,t/10
  
  vsync
  color get_color(0,0,10000)
  pbox 0,0,640,400
next t
return
