' Test einer Editierbaren eingabe mit Linienfont.
' (c) Markus Hoffmann 2000


weiss=get_color(65535,0,65535)
schwarz=get_color(0,0,0)
rot=get_color(65535,0,0)
ledit_curlen=40
deftext 1,0.1,0.3,0
okkk:
a$=@ledit$(a$,10,100)
if ledit_status=2
  b$=@ledit$(b$,10,150)
  if ledit_status=1
    goto okkk
  endif
endif
print b$
quit

function ledit$(t$,x,y)
  local t2$,cc,curpos,a,b,c$,curlen
  t2$=t$
  cc=len(t$)
  curpos=x+ltextlen(left$(t$,cc))
  curlen=max(20,ledit_curlen)
  color weiss
  ltext x,y,t$
  do
    color schwarz
    ltext x,y,t2$
    t2$=t$
    line curpos,y,curpos,y+curlen
    curpos=x+ltextlen(left$(t$,cc))
    color weiss
    ltext x,y,t2$
    color rot
    line curpos,y,curpos,y+curlen
    vsync
    keyevent a,b,c$
    color schwarz
    if b and -256
      if a=22    ! Bsp
        if len(t2$)
          t$=left$(t2$,cc-1)+right$(t2$,len(t2$)-cc)
          dec cc
        else
          bell
        endif
      else if a=36    ! Ret
        ledit_status=0
        line curpos,y,curpos,y+curlen
        return t2$
      else if a=98
        ledit_status=1
        line curpos,y,curpos,y+curlen
        return t2$
      else if a=104
        ledit_status=2 
        line curpos,y,curpos,y+curlen
        return t2$
      else if a=107   ! Del
        if cc<len(t2$)
          t$=left$(t2$,cc)+right$(t2$,len(t2$)-cc-1)
        endif
      else if a=100
        cc=max(0,cc-1)
      else if a=102
        cc=min(len(t2$),cc+1)
      endif
    else
      t$=left$(t2$,cc)+chr$(b)+right$(t2$,len(t2$)-cc)
      inc cc
    endif
    print a,hex$(b)
  loop
endfunction
