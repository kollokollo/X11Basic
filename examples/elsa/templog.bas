' Programm zur Aussentemperaturauslese
' (c) 2000 Markus Hoffmann
'
'


stackcount=0
dim stackt$(3)
dim tempd(3)
dim tempt(3)

oldtemp=0
tendenz=0
tempmin=100
tempmax=-40
open "A",#1,"temp.log"
print #1,"#   Neustart des loggers: "+date$+" "+time$
cssetcallback "SUP_WASSER_AUSSLUFT.TEMP_AM",intr
sizew 1,100,200
alert 0,"||||||||",1," QUIT ",b
csclearcallbacks
quit



procedure intr
  temp=csget("SUP_WASSER_AUSSLUFT.TEMP_AM")
  print date$+" "+time$+" ",timer,temp
  if abs(temp-oldtemp)>0.02  
    tendenz=abs((temp-oldtemp)>0.02)-abs((temp-oldtemp)<-0.02)
    oldtemp=temp
    print "*"
	@thermometer(temp)
    stackt$(stackcount)=date$+" "+time$+" "+str$(timer)+" "+str$(temp)
	tempd(stackcount)=temp
	tempt(stackcount)=timer
	
	if stackcount<2
	  inc stackcount
	else
	  u=abs((tempd(1)-tempd(0))/(tempt(1)-tempt(0))*(timer-tempt(0))-temp+tempd(0))
      print u
	  if u>0.05 and u<5 or (timer-tempt(0)>300)
      print "<w>"
      print #1,stackt$(0)
	  flush #1  
	  tempd(0)=tempd(1)
	  tempd(1)=tempd(2)
	  tempt(0)=tempt(1)
	  tempt(1)=tempt(2)
	  
	  stackt$(0)=stackt$(1)
	  stackt$(1)=stackt$(2)
	  endif
	endif
  endif
return

procedure thermometer(tt)
  tempmin=min(tempmin,tt)
  tempmax=max(tempmax,tt)
  color get_color(0,0,0)
  pbox 0,0,100,160  
  if tendenz>0
    color get_color(65535,0,0)
	line 0,50,50,0
	line 50,0,100,50
	line 100,50,75,50
	line 75,50,75,100
	line 75,100,25,100
	line 25,100,25,50
	line 25,50,0,50
  else if tendenz<0
    color get_color(0,0,65535)
	line 0,100,50,150
	line 50,150,100,100
	line 100,100,75,100
	line 75,100,75,50
	line 75,50,25,50
	line 25,50,25,100
	line 25,100,0,100
  else 
    color get_color(32000,32000,0)
	pcircle 20,100,5
  endif
  color get_color(65535,65535,65535)
  box 50,30,60,120
  line 47,100,50,100
  text 38,104,"0"
  text 40,40,"C"
  text 70,150,"C"
  circle 35,30,2
  circle 65,140,2
  circle 55,126,10
  text 10,150,str$(tt,4,4)

  
  
  color get_color(65535,0,0)
  pbox 51,120,60,100-tt*2
  pcircle 55,126,9
  color get_color(65535,65535,65535)

  line 50,100-tempmin*2,60,100-tempmin*2
  line 50,100-tempmax*2,60,100-tempmax*2  
  vsync
return
