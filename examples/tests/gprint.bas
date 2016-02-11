outsx=0
outsy=0
setfont "*courier-medium-r-*14*"
@gprint("Hallo, das ist ein Text....")
@gprint(" (c) Markus Hoffmann 2001")
@gprint("-----------------------------")
fileselect "Text laden ...","./*.txt","diffglei.txt",f$
if not exist(f$)
  quit
endif
open "I",#1,f$
while not eof(#1)
s=inp(#1)
@outs(s)
wend
close
end



procedure gprint(a$)
local i
for i=0 to len(a$)-1
@outs(peek(varptr(a$)+i))
next i
@outs(10)
return

procedure outs(a)
if a=13
  outsx=0
  vsync
else if a=8
  outsx=max(0,outsx-1)
else if a=9
  outsx=min(79,8*((outsx+8) div 8))

else if a=10
  inc outsy
  clr outsx
  vsync
  if outsy>24
    outsy=24
    
    
    copyarea 0,16,640,400-16,0,0
    color get_color(65535,65535,65535)
    pbox 0,400-16,640,400
    color get_color(0,0,0)
  endif
else
  text outsx*8,outsy*16+13,chr$(a)
  inc outsx
  if outsx=80
    clr outsx
    @outs(10)
  endif
endif
return
