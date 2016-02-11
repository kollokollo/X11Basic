' deffill <rule>,<style>,<pattern>


weiss=get_color(65535,65535,65535)
rot=get_color(65535,32000,32000)
schwarz=get_color(0,0,0)
color weiss
pbox 0,0,640,400
color schwarz,weiss
text 10,300,"DEFFILL-Test with X11-Basic (c) Markus Hoffmann"
for j=0 to 5
  for i=0 to 7
    color schwarz
    deffill ,2,i+j*8
    pbox j*32,i*32,j*32+32,i*32+32
    color get_color((i+j*8)*4000,32000,32000)
    pcircle 320+j*50,i*64,32
    color weiss
    deffill ,0
    pbox j*32,i*32+18,j*32+16,i*32+32
    color schwarz
    text 4+j*32,i*32+30,str$(i+j*8)
    color rot
    box j*32,i*32,j*32+32,i*32+32
  next i
  vsync
next j
vsync
end
