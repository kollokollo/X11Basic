' Beispiele, wie man mit Grafik-Objekten umgeht.
' Wer das AES vom ATARI ST kennt, dem kommt es bekannt vor...


strings$="Bitte wählen Sie aus:"+chr$(0)
btext1$="OK"+chr$(0)
btext2$="CANCEL"+chr$(0)
x=100
y=100
w=320
h=200
sel=9
s$=""
for i=0 to sel-1
s$=s$+str$(i+1)+chr$(0)
next i

name$="Ichbins"+string$(30,chr$(0))
buf$=space$(20)+chr$(0)
ted$=mkl$(varptr(name$))+mkl$(varptr(buf$))+mkl$(varptr(buf$))
ted$=ted$+mki$(3)+mki$(0)+mki$(0)+mki$(0x100)+mki$(0)+mki$(1)+mki$(len(name$)-1)+mki$(len(buf$)-1)


ob$=mki$(-1)+mki$(1)+mki$(5+sel)+mki$(20)+mki$(0)+mki$(16)
ob$=ob$+mkl$(0x21100)+mki$(x)+mki$(y)+mki$(w)+mki$(h)

ob$=ob$+mki$(2)+mki$(-1)+mki$(-1)+mki$(28)+mki$(0)+mki$(0)
ob$=ob$+mkl$(varptr(strings$))+mki$(16)+mki$(20)+mki$(200)+mki$(20)

ob$=ob$+mki$(3)+mki$(-1)+mki$(-1)+mki$(26)+mki$(1 or 4 or 2)+mki$(0)
ob$=ob$+mkl$(varptr(btext1$))+mki$(16)+mki$(160)+mki$(16*8)+mki$(16)

ob$=ob$+mki$(4)+mki$(-1)+mki$(-1)+mki$(26)+mki$(1 or 4)+mki$(0)
ob$=ob$+mkl$(varptr(btext2$))+mki$(170)+mki$(160)+mki$(16*8)+mki$(16)

ob$=ob$+mki$(5+sel)+mki$(5)+mki$(5+sel-1)+mki$(20)+mki$(0)+mki$(32)
ob$=ob$+mkl$(-1)+mki$(50)+mki$(50)+mki$(200)+mki$(50)

for i=0 to sel-2
ob$=ob$+mki$(5+i+1)+mki$(-1)+mki$(-1)+mki$(26)+mki$(1 or 16)+mki$(0)
ob$=ob$+mkl$(varptr(s$)+2*i)+mki$(10+i*20)+mki$(16)+mki$(16)+mki$(16)
next i
ob$=ob$+mki$(4)+mki$(-1)+mki$(-1)+mki$(26)+mki$(1 or 16)+mki$(0)
ob$=ob$+mkl$(varptr(s$)+2*(sel-1))+mki$(10+(sel-1)*20)+mki$(16)+mki$(16)+mki$(16)

ob$=ob$+mki$(0)+mki$(-1)+mki$(-1)+mki$(29)+mki$(8 or 32)+mki$(0)
ob$=ob$+mkl$(varptr(ted$))+mki$(10)+mki$(110)+mki$(16*10)+mki$(16)


~form_dial(0,0,0,0,0,x,y,w,h)
~form_dial(1,0,0,0,0,x,y,w,h)
~objc_draw(varptr(ob$),0,-1,0,0)
vsync
while mousek=0
print objc_find(varptr(ob$),mousex,mousey)
pause 0.1
wend
~form_dial(2,0,0,0,0,x,y,w,h)
~form_dial(3,0,0,0,0,x,y,w,h)

vsync
~form_dial(0,0,0,0,0,x,y,w,h)
~form_dial(1,0,0,0,0,x,y,w,h)
nochmal:
ret=form_do(varptr(ob$))
print "RET:";ret
if ret=2   ! OK 
  gedr=-1
  for i=0 to sel-1
    if dpeek(varptr(ob$)+(5+i)*24+10)=1
      gedr=i
      print "Sie hatten nr.";gedr+1;" gewaehlt."
    endif
  next i
  if gedr=-1
  ~form_alert(1,"[3][Sie haben nix gewählt !][OH]")
  dpoke varptr(ob$)+(ret)*24+10,0
  goto nochmal
  endif
endif

~form_dial(2,0,0,0,0,x,y,w,h)
~form_dial(3,0,0,0,0,x,y,w,h)
vsync
print "Ihre Texteingabe war: ",name$
end
