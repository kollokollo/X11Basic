echo off
clearw
f=40
color get_color(0,65535,0)
CIRCLE 320,200,1/0.2*f+2
CIRCLE 320,200,1/0.2*35+2
CIRCLE 320,200,1/0.2*37+2
CIRCLE 320,200,1/0.2*15+2
CIRCLE 320,200,1/0.2*17+2
vsync
color get_color(65535,65535,65535)
DO
  FOR t=0 TO 2*PI STEP 0.01
    r=1/(1+0.8*COS(t+t0))
    r=r*f
    x=r*COS(t)
    y=r*SIN(t)
    PLOT x+320,y+200
  NEXT t
  vsync
  t0=t0+1
  EXIT IF t0>11
LOOP

