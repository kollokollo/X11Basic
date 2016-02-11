color get_color(65535,0,0)
pbox 0,0,640,400
' Test der Rsrc-Funktionen
' Es koennen ATARI ST *.RSC-Files verwendet werden...



color get_color(0,65535,0)
for i=0 to 30
circle 320,200,i*5
next i
text 10,10,"Mit der rechten Maustaste geht es weiter."
fileselect "RSC-Laden","./rsc/*.rsc","",f$
rsrc_load f$
count=0
adr=rsrc_gaddr(15,count)
while adr<>-1
  t$=space$(20000)
  bmove adr,varptr(t$),10000
  wort_sep t$,chr$(0),0,t$,a$
  print count,adr,t$
  ~form_alert(1,t$)
  inc count
  adr=rsrc_gaddr(15,count)
wend
count=0
adr=rsrc_gaddr(0,count)
while adr<>-1
  print count,adr
  print form_do(adr)
  inc count
  adr=rsrc_gaddr(0,count)
wend
rsrc_free
quit
