' Simple test for the X11-Basic virtual Machine 2008-01-19
'
PLOT 1,1
GET_GEOMETRY 1,bx,by,bw,bh
PRINT "Los gehts..."
' ~inp(-2)
BOX bx,by,bx+bw,by+bh
COLOR COLOR_RGB(1,1,0)
TEXT 30,bh-40,"Test of the X11-Basic virtual machine"
TEXT 30,bh-30,"Hit Ctrl-c to end"
DO
  i=(i+1) MOD bw
  COLOR COLOR_RGB(RND(),RND(),RND())
  PBOX RANDOM(300),RANDOM(30),RANDOM(bw),RANDOM(bh-40)
  SHOWPAGE
  IF i=0
    PRINT AT(4,1);timer-t;" sek."
    t=TIMER
  ENDIF
  PLOT i,50*SIN(i/100*2*PI)+bh-40
  PAUSE 0.01
LOOP
QUIT
