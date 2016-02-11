color get_color(65535,0,0)
pbox 0,0,640,400
' Test der Rsrc-Funktionen
' Es koennen ATARI ST *.RSC-Files verwendet werden...



color get_color(0,65535,0)
for i=0 to 30
circle 320,200,i*5
next i
fileselect "RSC-Laden","./*.rsc","",f$
rsrc_load f$
for i=0 to 32
  form_do i,b
  print b
next i
alert_do 1,1,b
print b
rsrc_free
quit
