' test of the EVENT command in X11-Basic
' can have different results on different platforms/OS
'


@init
do
  if event?(1+2+4+8+0x40)=true or true
    t=4711
    EVENT t,x,y,xroot,yroot,s,k,ks,t$,tim
    if t=2
      @show("Key Press")
    print t,x,y,xroot,yroot,s,k,ks,t$,tim
      key(k)=1
      @key
    else if t=3
      @show("Key Release")
    print t,x,y,xroot,yroot,s,k,ks,t$,tim
       key(k)=0
      @key
   
    else if t=4
      @show("Mouse Button Press")
      mb(k)=1
      color rot
      pcircle x,y,5
      @mouse
    else if t=5
      @show("Mouse Button Release")
      mb(k)=0
      @mouse
     
    else if t=6
      @show("Mouse Motion")
      color gelb
      pcircle x,y,5
    else if t=10
      @show("Window Moved")
    else if t=13
      @show("Window Resize")
    '  sizew ,x,y
    '  vsync
    else
    print t,x,y,xroot,yroot,s,k,ks,t$,tim
    print "Event: typ=0x"+hex$(t)
    endif
  endif

loop
procedure init
  schwarz=color_RGB(0,0,0)
  rot=color_RGB(1,0,0)
  gelb=color_RGB(1,1,0)
  weiss=color_RGB(1,1,1)
  color schwarz
  get_geometry ,bx%,by%,bw%,bh%
  print "Screen dimensions: ";bx%,by%,bw%,bh%
  pbox bx%,by%,bw%,bh%
  dim mb(6)
  dim key(512)
return
procedure show(t$)
  color schwarz
  pbox 10,10,180,32
  color rot
  text 16,25,t$
  vsync
return
procedure mouse
   color weiss
   box 10,40,120,120
   for i=1 to 5
   color weiss
   box i*20,44,i*20+16,60
   if mb(i)
     color weiss
   else
     color schwarz
   endif
   pbox i*20+1,44+1,i*20+16-1,60-1
   next i
   vsync
return

procedure key

   color weiss
   box 320,60,420,120
   for j=0 to 15
   for i=0 to 31
   color weiss
   box 320+i*10,60+j*10,320+i*10+8,60+j*10+8
   if key(j*32+i)
     color weiss
   else
     color schwarz
   endif
   pbox 320+i*10+1,60+j*10-1,320+i*10+8+1,60+j*10+8-1

   next i
   next j
   vsync


return


