'
' 7 segments calculator (c) Markus Hoffmann 2004
'
'


ziff%()=[0x7b,0x42,0x37,0x67,0x4e,0x6d,0x7d,0x43,0x7f,0x6f]

rot=get_color(65535,0,0)
grau=get_color(15535,0,0)
schwarz=get_color(0,0,0)
t$=""

scale=0.5

bw=640*scale
bh=400*scale

sizew ,bw,bh
do
  t$=@conv$(str$(mem,13,13))
  groesse=scale
  @puts7(0,0,t$)
  groesse=0.5*scale
  t$=@conv$(time$+"  "+date$)
  @puts7(0,100*scale,t$)
  t$=@conv$(str$(timer))
  @puts7(400*scale,100*scale,t$)
  groesse=2*scale
  t$=@conv$(str$(acu,10,7))
  @puts7(0,200*scale,t$)
  groesse=1*scale
  t$=@conv$(hex$(mem,13,13))
  @puts7(0,320*scale,t$)

  vsync
  keyevent a,b,c$
  if left$(c$)>="0" and left$(c$)<="9"
    if clearacu
      acu=0
      clearacu=0
    endif
    acu=acu*10+(asc(left$(c$))-asc("0"))
  else if instr("+-/^*",left$(c$))
    x=acu
    clearacu=true
    mod$=left$(c$)
  else if left$(c$)="~"
    acu=-acu
  else if left$(c$)="p"
    acu=pi
    clearacu=true
  else if left$(c$)="m"
    mem=acu
  else if left$(c$)="="
    if mod$="+"
      acu=acu+x
    else if mod$="-"
      acu=x-acu
    else if mod$="*"
      acu=x*acu
    else if mod$="/"
       if acu<>0
       acu=x/acu
       else
       acu=nan
       endif
   else if mod$="^"
       acu=x^acu
    endif
    clearacu=true
  else
    print at(1,1);a,hex$(b),c$,"   "
  endif
loop
end

procedure puts7(x,y,c$)
  local i
  for i=0 to len(c$)-1
    @put7(x,y,groesse,peek(varptr(c$)+i))
    add x,32*groesse
    if x>640*scale
    x=0
    add y,64*groesse
    endif
  next i  
return
procedure put7(x,y,s,c)
  local i
  color schwarz
  pbox x,y,x+s*32,y+s*64
d%()=[3,3,29,3;29,3,29,29;3,32,29,32;3,3,3,29;3,32,3,61;3,61,29,61;29,61,29,32;34,64,35,64]
for i=0 to 7
  if btst(c,i)
    color rot
  else
    color grau
  endif
  defline ,4*s,2
  line x+d%(i,0)*s*0.8,y+d%(i,1)*s*0.6,x+d%(i,2)*s*0.8,y+d%(i,3)*s*0.6
next i
return

function conv$(t$)
  local i,a$
  a$=""
  for i=0 to len(t$)-1
    if peek(varptr(t$)+i)=asc(".")
      if len(a$)
        poke varptr(a$)+len(a$)-1,peek(varptr(a$)+len(a$)-1) or 0x80
      else
        a$=chr$(ziff%(0) or 0x80)
      endif
    else  if peek(varptr(t$)+i)=asc(" ")
      a$=a$+chr$(0)
    else  if peek(varptr(t$)+i)=asc("-")
      a$=a$+chr$(4)
    else  if peek(varptr(t$)+i)=asc("e")
      a$=a$+chr$(0x3d)
    else  if peek(varptr(t$)+i)=asc("E")
      a$=a$+chr$(0x3d)
    else  if peek(varptr(t$)+i)=asc("f")
      a$=a$+chr$(0x1d)
    else  if peek(varptr(t$)+i)=asc("a")
      a$=a$+chr$(0x5f)
   else
      a$=a$+chr$(ziff%(peek(varptr(t$)+i)-asc("0")))
    endif
  next i
  return a$
endfunction
