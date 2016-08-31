' Draw some thick lines to see if everything is OK.
'
DEFLINE ,80
black=COLOR_RGB(0,0,0)
yellow=COLOR_RGB(1,1,0)

COLOR COLOR_RGB(0.9,0,0)

LINE 100,100,99,200
LINE 200,200,199,100

FOR i=0 TO 360 STEP 30
  COLOR yellow
  LINE 400,400,400+200*COS(i/180*PI),400+200*SIN(i/180*PI)
  SHOWPAGE
  PAUSE 0.1
  COLOR black
  ' line 400,400,400+200*cos(i/180*pi),400+200*sin(i/180*pi)
  ' pause 0.01
NEXT i
PAUSE 1
QUIT
