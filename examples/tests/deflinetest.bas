' Test of the DEFLINE Line type and style function.
'
'
'

clearw
for j=0 to 8
  text 0,20+i*32+j*32,str$(i)+str$(j)
  defline j,i
  line 20,20+i*32+32*j,180,20+i*32+32*j
  vsync
next j
for j=0x11 to 0xff
  if (j and 15)=0
    inc j
  endif
print j
  text 200,20+j,str$(i)+str$(j)
  defline j
  line 200,20+j,380,20+j
  vsync
next j
for j=0x100 to 0x200
  if (j and 15)=0
    inc j
  endif
print j
  text 400,20+j-0x100,str$(i)+str$(j)
  defline j
  line 400,20+j-0x100,580,20+j-0x100
  vsync
  pause 0.1
next j
showpage
end
