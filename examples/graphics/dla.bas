' Diffusion-Limited Aggregation
' Fractal by Christos Tziotzis 2012 converted from GFA Basic
'
'

CLS
PRINT "How many particles do you want (200 - 2000 is a good number)";
INPUT particles

CLEARW
yellow=COLOR_RGB(1,1,0)
blue=COLOR_RGB(0,1/2,1)
DIM screen%(640,400)
ARRAYFILL screen%(),0
DIM screen2%(640,400)
ARRAYFILL screen2%(),0
'DIM precalc&(4000,2)
th=90
' Fill array screen with initial values
COLOR COLOR_RGB(1,1/2,0)
' uncomment following line if you dont want a circle in the middle.
'goto skip
COLOR yellow
FOR i=0 TO 179 STEP 0.5
  x1=320+COS(RAD(th+i))*10
  y1=200+SIN(RAD(th+i))*10
  x2=320-COS(RAD(th+i))*10
  y2=y1
  FOR j=x1 TO x2
    PLOT j,y1
    screen%(j,y1)=1
  NEXT j
NEXT i
skip:
screen%(320,200)=1
color=blue
PLOT x,y
SHOWPAGE
counter=1
tttt=TIMER
@feed
@fractal
@saving
PRINT TIMER-tttt;" Seconds."
QUIT

PROCEDURE feed
  ' place a particle in a random position on the periphery of a circle
  ' of radius r
  new_particle:
  PRINT ".";
  FLUSH
  try_again:
  fi=RAD(RANDOM(360))
  r=15+radd%
  x=INT(320+r*COS(fi))
  y=INT(200+r*SIN(fi))
  IF screen%(x,y)=1
    GOTO try_again
  ENDIF
  'Check if the feed particle falls near an occupied place. It shouldn't happen but better safe than sorry.
  IF screen%(x-1,y-1)=1 OR screen%(x-1,y)=1 OR screen%(x-1,y+1)=1 OR screen%(x,y-1)=1 OR screen%(x,y+1)=1 OR screen%(x+1,y-1)=1 OR screen%(x+1,y)=1 OR screen%(x+1,y+1)=1
    screen%(x,y)=1
    GOTO new_particle
  ENDIF
  ARRAYFILL screen2%(),0
RETURN
PROCEDURE fractal
  ' This is the DLA algorithm. Particle follows brownian motion. In each step
  ' a check is being performed for the existance of a neigbouring particle.
  ' If such a particle exists then the particle sticks and a new one is being
  ' released

  DO
    IF RANDOM(2)=1
      INC x
    ELSE
      DEC x
    ENDIF
    IF x<320-2*r
      x=(320-r)
    ELSE IF x>320+2*r
      x=320+r
    ENDIF
    ' IF screen%(x,y)=1
    '	GOSUB fractal
    ' ENDIF
    IF screen2%(x,y)=0
      'Checking for neighbouring particle after motion on the x-axis
      IF screen%(x-1,y-1)=1 OR screen%(x-1,y)=1 OR screen%(x-1,y+1)=1 OR screen%(x,y-1)=1 OR screen%(x,y+1)=1 OR screen%(x+1,y-1)=1 OR screen%(x+1,y)=1 OR screen%(x+1,y+1)=1
        screen%(x,y)=1
        COLOR yellow
        PLOT x,y
        SHOWPAGE
        ' insert=1
        ' IF insert=1
        INC counter
        ' precalc&(counter,0)=x
        ' precalc&(counter,1)=y
        GOSUB feed
        ' ENDIF
      ENDIF
      screen2%(x,y)=1
    ENDIF
    IF RANDOM(2)=1
      INC y
    ELSE
      DEC y
    ENDIF

    IF y<200-1.5*r
      y=200-r
    ELSE IF y>200+1.5*r
      y=200+r
    ENDIF
    'Checking for neighbouring particle after motion on the y-axis
    IF screen2%(x,y)=0
      IF screen%(x-1,y-1)=1 OR screen%(x-1,y)=1 OR screen%(x-1,y+1)=1 OR screen%(x,y-1)=1 OR screen%(x,y+1)=1 OR screen%(x+1,y-1)=1 OR screen%(x+1,y)=1 OR screen%(x+1,y+1)=1
        screen%(x,y)=1
        COLOR blue
        PLOT x,y
        SHOWPAGE
        ' insert=1
        ' IF insert=1
        INC counter
        ' precalc&(counter,0)=x
        ' precalc&(counter,1)=y
        @feed
        ' ENDIF
      ENDIF
      screen2%(x,y)=1
    ENDIF
    ' PRINT AT(1,1);x,y,counter,r,particles
    EXIT IF counter>particles
    radd%=sqrt((x-320)^2+(y-200)^2)
  LOOP
  SHOWPAGE
RETURN
PROCEDURE saving
  'BSAVE "precalc.chr",V:precalc&(0,0),4000*2*2
  SAVEWINDOW "fractal.bmp"
RETURN
