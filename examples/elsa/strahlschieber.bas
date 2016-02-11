' X-Y-Steuerung der Strahlschieber Dez. 1998 Markus Hoffmann
echo off
'
' CSXBASIC-Programm
' letzte Bearbeitung 11.9.1999     Markus Hoffmann
'
sizew ,560,450
defmouse 2
sw=100

filename$="/sgt/elsa/data/polq1/strahlschieber.dat"

weiss=get_color(65535,65535,65535)
grau=get_color(65535/1.2,65535/1.2,65535/1.2)
schwarz=get_color(0,0,0)
rot=get_color(65535,0,0)
gelb=get_color(65535,65535,0)
gruen=get_color(32000,65535,32000)


' Cops-Daten einlesen

restore copsdata
dim copx(50),copy(50),csx(50),csy(50)
dim bcx(50),bcy(50),sname$(50),scale(50),resx(50),resy(50)

clr c,anzcops
while c<>-1
  read c
  if c<>-1
    copx(anzcops)=c
    read csx(anzcops),copy(anzcops),csy(anzcops),sname$(anzcops),scale(anzcops)
    bcx(anzcops)=anzcops mod 5
    bcy(anzcops)=anzcops div 5
    inc anzcops
  endif
wend
print "Anzahl: ",anzcops


color grau
pbox 0,0,560,450
color schwarz
setfont "*Courier*-14-*"
text 150,380,"Strahlschieber an der 120kV Quelle"
@button(200,420,"REDRAW",0)
@button(300,420,"sichern",0)
@button(400,420,"laden",0)
@button(500,420,"QUIT",0)
@button(500,360,"Hilfe",0)

@redraw
defmouse 0
do
  vsync
  mouseevent x,y,k
  if @inbutton(500,360,"Hilfe",x,y)
      @button(500,360,"Hilfe",true)
      vsync
	  h$="Hilfe zm Strahlschiebermenue|"
	  h$=h$+"============================||"
	  h$=h$+"Mit der Maus können die roten Punkte in den |"
	  h$=h$+"Kästen verschoben werden.||"
	  h$=h$+"Durch Anklicken der gelben Kaestchen erfolgt ein|"
	  h$=h$+"kleinster Schritt in die jeweilige Richtung.|"
	  
      void form_alert(1,"[1]["+h$+"][  OK  ]")=1
      @button(500,360,"Hilfe",0)
      vsync
  else if @inbutton(500,420,"QUIT",x,y)
      @button(500,420,"QUIT",true)
      vsync
      if form_alert(1,"[2][Wirklich beenden ?][QUIT|CANCEL]")=1
        quit
      endif
      @button(500,420,"QUIT",0)
      vsync
  else if @inbutton(200,420,"REDRAW",x,y)
     @button(200,420,"REDRAW",TRUE) 
	 defmouse 2
     vsync 
     @redraw
	 defmouse 0
     @button(200,420,"REDRAW",0) 
     vsync 
  else if @inbutton(300,420,"sichern",x,y)
     @button(300,420,"sichern",TRUE) 
     vsync 
     @sichern
     @button(300,420,"sichern",0) 
     vsync 
  else if @inbutton(400,420,"laden",x,y)
     @button(400,420,"laden",TRUE)
	 defmouse 2 
     vsync 
     @laden
	 @redraw
     @button(400,420,"laden",0)
	 defmouse 0 
     vsync 
  else
    selected=@find_ob(x,y)
    if selected>=0 and selected<anzcops 
	 ' ist es gelbes Kaestchen ?
	 kr=@gelb(selected,x,y)
     if kr>0
	   if kr=1
	   else if kr=2
	   else if kr=3
	   else if kr=4
	   else
	     print "Fehler !"
	   endif
     else if @getcop(selected,x,y)
       
       motionevent x,y,,,k2
       while k2=256
         newx=@ox(selected,x)
         newy=@oy(selected,y)
         if abs(newx)<scale(selected)/2 and abs(newy)<scale(selected)/2
           csput "POL_SOURCE1_COPS"+str$(copx(selected))+".STROM"+str$(csx(selected))+"_AC",abs(newx)
           csput "POL_SOURCE1_COPS"+str$(copy(selected))+".STROM"+str$(csy(selected))+"_AC",abs(newy)
           csput "POL_SOURCE1_COPS"+str$(copx(selected))+".POL"+str$(csx(selected))+"_DM",(sgn(newx)+1)/2
           csput "POL_SOURCE1_COPS"+str$(copy(selected))+".POL"+str$(csy(selected))+"_DM",(sgn(newy)+1)/2
           @drawstrahls(10+(sw+10)*bcx(selected),10+(sw+10)*bcy(selected),newx,newy,scale(selected))
         endif
         vsync
         motionevent x,y,,,k2
       wend
       @drawcop(selected)
     endif
    endif
  endif
loop
pause 5
end

procedure redraw
  local i
  for i=0 to anzcops-1
    @drawcop(i)
  next i
return

function ox(n,mx)
  return (mx-10-(sw+10)*bcx(n)-sw/2)*scale(n)/sw
endfunc
function oy(n,my)
  return -(my-10-(sw+10)*bcy(n)-sw/2)*scale(n)/sw
endfunc
function kx(n,a)
  return 10+(sw+10)*bcx(n)+sw/2+a/scale(n)*sw
endfunc
function ky(n,b)
  return 10+(sw+10)*bcy(n)+sw/2-b/scale(n)*sw
endfunc


function find_ob(fx,fy)
  local i,x,y
  x=fx div (sw+10)
  y=fy div (sw+10)
  for i=0 to anzcops-1
    if x=bcx(i) and y=bcy(i)
	  return i
    endif
  next i
  return -1
endfunc


function getcop(n,gx,gy)
  local hspx,hspy,a,b,sx,sy,wx,wy
  a=csget("POL_SOURCE1_COPS"+str$(copx(n))+".STROM"+str$(csx(n))+"_AC")
  a=-(1-2*csget("POL_SOURCE1_COPS"+str$(copx(n))+".POL"+str$(csx(n))+"_DM"))*a
  b=csget("POL_SOURCE1_COPS"+str$(copy(n))+".STROM"+str$(csy(n))+"_AC")
  b=-(1-2*csget("POL_SOURCE1_COPS"+str$(copy(n))+".POL"+str$(csy(n))+"_DM"))*b
  sx=10+(sw+10)*bcx(n)
  sy=10+(sw+10)*bcy(n)
  wx=a
  wy=b
  hspx=sx+sw/2+wx/scale(n)*sw
  hspy=sy+sw/2-wy/scale(n)*sw
  if (gx-hspx)^2+(gy-hspy)^2<16
    return  true
  endif
  return false
endfunc
function gelb(n,gx,gy)
' noch unvoll
  local hspx,hspy,a,b,sx,sy,wx,wy
  sx=10+(sw+10)*bcx(n)
  sy=10+(sw+10)*bcy(n)
  hspx=sx+sw/2+wx/scale(n)*sw
  hspy=sy+sw/2-wy/scale(n)*sw
  if (gx-hspx)^2+(gy-hspy)^2<16
    return  true
  endif
  return false
endfunc


procedure drawcop(n)
  local a,b
  a=csget("POL_SOURCE1_COPS"+str$(copx(n))+".STROM"+str$(csx(n))+"_AC")
  a=-(1-2*csget("POL_SOURCE1_COPS"+str$(copx(n))+".POL"+str$(csx(n))+"_DM"))*a
  b=csget("POL_SOURCE1_COPS"+str$(copy(n))+".STROM"+str$(csy(n))+"_AC")
  b=-(1-2*csget("POL_SOURCE1_COPS"+str$(copy(n))+".POL"+str$(csy(n))+"_DM"))*b

  @drawstrahls(10+(sw+10)*bcx(n),10+(sw+10)*bcy(n),a,b,scale(n))
  text 10+(sw+10)*bcx(n),10+(sw+10)*bcy(n),sname$(n)

return

procedure drawstrahls(sx,sy,wx,wy,ssmax)
  color weiss
  pbox sx,sy,sx+sw,sy+sw
  color schwarz
  box sx,sy,sx+sw,sy+sw
  color gruen
  line sx,sy+sw/2,sx+sw,sy+sw/2
  line sx+sw/2,sy,sx+sw/2,sy+sw
  color rot
  pcircle sx+sw/2+wx/ssmax*sw,sy+sw/2-wy/ssmax*sw,4
  color schwarz
  circle sx+sw/2+wx/ssmax*sw,sy+sw/2-wy/ssmax*sw,4
  ' @button(sx,sy,chr$(1),0)
  color gelb
  pbox sx+sw/2-3,sy,sx+sw/2+3,sy+6
  pbox sx+sw/2-3,sy+sw,sx+sw/2+3,sy+sw-6
  pbox sx,sy+sw/2-3,sx+6,sy+sw/2+3
  pbox sx+sw,sy+sw/2-3,sx+sw-6,sy+sw/2+3
  color schwarz
   box sx+sw/2-3,sy,sx+sw/2+3,sy+6
  box sx+sw/2-3,sy+sw,sx+sw/2+3,sy+sw-6
  box sx,sy+sw/2-3,sx+6,sy+sw/2+3
  box sx+sw,sy+sw/2-3,sx+sw-6,sy+sw/2+3
 
return

procedure button(button_x,button_y,button_t$,sel)
  local x,y,w,h
  defline ,1
  deftext 1,0.05,0.1,0
  button_l=ltextlen(button_t$)

  x=button_x-button_l/2-10
  y=button_y-10
  w=button_l+20
  h=20
  color grau  
  pbox x+5,y+5,x+w+5,y+h+5
  color abs(sel)*schwarz+abs(not sel)*weiss
  pbox x,y,x+w,y+h
  if sel=-1
   color weiss
  else 
   color schwarz
  endif  
  box x,y,x+w,y+h
  box x-1,y-1,x+w+1,y+h+1
 
  ltext button_x-button_l/2,button_y-5,button_t$
return

function inbutton(button_x,button_y,button_t$,mx,my)
  local x,y,w,h
  
  deftext 1,0.05,0.1,0
  button_l=ltextlen(button_t$)

  x=button_x-button_l/2-10
  y=button_y-10
  w=button_l+20
  h=20
  if mx>=x and my>=y and  mx<=x+w and my<=y+h
    return true
  else
    return false
  endif
endfunc


procedure sichern
  local i,a,b,nam1$,nam2$,wert1,wert2,pol1,pol2,pnam1$,pnam2$
  open "O",#1,filename$
  print #1,"# Dieses File wurde erzeugt von /sgt/elsa/bas/strahlschieber.bas "
  print #1,"# Am "+date$+" um "+time$+" von "+env$("USER")
  print #1,"#================================================================"
  print #1,"#"
  for i=0 to anzcops-1
    nam1$="POL_SOURCE1_COPS"+str$(copx(i))+".STROM"+str$(csx(i))+"_AC" 
    wert1=csget(nam1$)
	pnam1$="POL_SOURCE1_COPS"+str$(copx(i))+".POL"+str$(csx(i))+"_DM"
	pol1=csget(pnam1$)
    a=-(1-2*pol1)*wert1
    nam2$="POL_SOURCE1_COPS"+str$(copy(i))+".STROM"+str$(csy(i))+"_AC"
    wert2=csget(nam2$)
	pnam2$="POL_SOURCE1_COPS"+str$(copy(i))+".POL"+str$(csy(i))+"_DM"
	pol2=csget(pnam2$)
    b=-(1-2*pol2)*wert2
	print #1,"# "+sname$(i)+": x="+str$(a)+" y="+str$(b)
	print #1,pnam1$+", "+str$(pol1)
	print #1,nam1$+", "+str$(wert1)
	print #1,pnam2$+", "+str$(pol2)
	print #1,nam2$+", "+str$(wert2)
  next i
  close #1
return

procedure laden
  local t$,wert
  if not exist(filename$)
    void form_alert(1,"[3]["+filename$+"|existiert nicht !|Ein Datensatz muss erst durch SICHERN|erzeugt werden !][ABBRUCH]")
  else
    open "I",#1,filename$
	while not eof(#1)
      input #1,t$
      if left$(t$)<>"#"
        input #1,wert
        csset t$,wert
      endif
    wend
    close #1
  endif
return


copsdata:
data 1,1,1,2,"SSQuelle",1
data 1,3,1,4,"SSA1",0.5
data 1,5,1,6,"SSA2",1
data 1,6,1,8,"SSB12",1
data 2,1,2,2,"SSB11",1
data 2,3,2,4,"SSB21",1
data 2,5,2,6,"SSB22",1
data 2,7,2,8,"SSC",1
data 3,1,3,2,"SSD11",1
data 3,3,3,4,"SSD12",1
data 3,5,3,6,"SSD2",1
data 3,7,3,8,"SSE1",1
data 4,1,4,2,"SSE2",1
data 4,3,4,4,"SSF1",1
data 4,5,4,6,"SSF2",1
data 4,7,4,8,"SSLinac",4
data -1,-1,-1,-1,"",-1
