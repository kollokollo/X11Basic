' Diffusion-Limited Aggregation
' Fractal by Christos Tziotzis 2012 converted from GFA Basic
'
'

CLS
PRINT "How many particles do you want (200 - 2000 is a good number)";
input particles

CLEARW 
yellow=GET_COLOR(65535,65535,0)
blue=GET_COLOR(0,32000,65535)
DIM screen%(640,400)
arrayfill screen%(),0
DIM screen2%(640,400)
arrayfill screen2%(),0
'DIM precalc&(4000,2)
th=90
' Fill array screen with initial values
color get_color(65535,32000,0)
' uncomment following line if you dont want a circle in the middle.
'goto skip
	color GET_COLOR(65535,65535,0)	
 FOR i=0 TO 179 STEP 0.5
  x1=320+COS(rad(th+i))*10
  y1=200+SIN(rad(th+i))*10
  x2=320-COS(rad(th+i))*10
  y2=y1
  FOR j=x1 TO x2
    plot j,y1
    screen%(j,y1)=1
  NEXT j
NEXT i
skip:
screen%(320,200)=1
color=blue
plot x,y
vsync
counter=1
tttt=timer
@feed
@fractal
@saving
print timer-tttt;" Seconds."
quit
PROCEDURE feed
  ' place a particle in a random position on  the periphery of a circle
  ' of radius r
new_particle:
  print ".";
  flush
try_again:
  fi=RAD(RANDOM(360))
  r=15+radd%
  x=INT(320+r*COS(fi))
  y=INT(200+r*SIN(fi))
  IF screen%(x,y)=1 
    GOto try_again
  ENDIF
'Check if the feed particle falls near an occupied place. It shouldn't happen but better safe than sorry. 
  IF screen%(x-1,y-1)=1 OR screen%(x-1,y)=1 OR screen%(x-1,y+1)=1 OR screen%(x,y-1)=1 OR screen%(x,y+1)=1 OR screen%(x+1,y-1)=1 OR screen%(x+1,y)=1 OR screen%(x+1,y+1)=1 
    screen%(x,y)=1
    GOTO new_particle
  ENDIF
  arrayfill screen2%(),0
RETURN
PROCEDURE fractal
  ' This is the DLA algorithm. Particle follows brownian motion. In each step
  ' a check is being performed for the existance of a neigbouring particle.
  ' If such a particle exists then the particle sticks and a new one is being
  ' released
	 
  DO
    if RANDOM(2)=1
      inc x
    else
      dec x
    endif
    IF x<320-2*r 
	 x=(320-r)
    else IF x>320+2*r 
	x=320+r
    ENDIF
 '   IF screen%(x,y)=1 
'	GOSUB fractal
'    ENDIF
if screen2%(x,y)=0
'Checking for neighbouring particle after motion on the x-axis 
    IF screen%(x-1,y-1)=1 OR screen%(x-1,y)=1 OR screen%(x-1,y+1)=1 OR screen%(x,y-1)=1 OR screen%(x,y+1)=1 OR screen%(x+1,y-1)=1 OR screen%(x+1,y)=1 OR screen%(x+1,y+1)=1 
	screen%(x,y)=1
	color yellow
	PLOT x,y
	vsync
'	insert=1
'	IF insert=1
	  inc counter
	 ' precalc&(counter,0)=x
	 ' precalc&(counter,1)=y
	  GOSUB feed
'	ENDIF
    ENDIF
screen2%(x,y)=1
endif
    if RANDOM(2)=1
	inc y
    else 
	dec y    
    endif

    IF y<200-1.5*r 
 	y=200-r
    else IF y>200+1.5*r 
 	y=200+r
    ENDIF
'Checking for neighbouring particle after motion on the y-axis 	
if screen2%(x,y)=0
   IF screen%(x-1,y-1)=1 OR screen%(x-1,y)=1 OR screen%(x-1,y+1)=1 OR screen%(x,y-1)=1 OR screen%(x,y+1)=1 OR screen%(x+1,y-1)=1 OR screen%(x+1,y)=1 OR screen%(x+1,y+1)=1 
	screen%(x,y)=1
	color blue	
	PLOT x,y
	vsync
'	insert=1
'	IF insert=1
	  inc counter
	'  precalc&(counter,0)=x
	' precalc&(counter,1)=y
	@feed
'	ENDIF
    ENDIF
screen2%(x,y)=1
endif
   ' PRINT AT(1,1);x,y,counter,r,particles
    exit IF counter>particles
    radd%=sqrt((x-320)^2+(y-200)^2)
  LOOP
  vsync
RETURN
PROCEDURE saving
  'BSAVE "precalc.chr",V:precalc&(0,0),4000*2*2
  savewindow "fractal.bmp"
RETURN
