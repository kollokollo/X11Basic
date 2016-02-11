' GFA Punch (code from tigen.ti-fr.com/)
' Carré 3D en rotation
SIZEW ,100,100
t%=TIMER
FOR i%=0 TO 50*10
  CLEARW
  X=COS(T)*20.
  Y=SIN(T)*18.
  R=SIN(T+T)*2.5
  LINE X+50.,Y+70.-R,­Y+50.,X+70.-R
  LINE ­Y+50.,X+70.-R,­X+50.,­Y+70.-R
  LINE ­X+50.,­Y+70.-R,Y+50.,­X+70.-R
  LINE Y+50.,­X+70.-R,X+50.,Y+70.-R
  LINE X+50.,Y+20.+R,­Y+50.,X+20.+R
  LINE ­Y+50.,X+20.+R,­X+50.,­Y+20.+R
  LINE ­X+50.,­Y+20.+R,Y+50.,­X+20.+R
  LINE Y+50.,­X+20.+R,X+50.,Y+20.+R
  LINE X+50.,Y+70.-R,X+50.,Y+20.+R
  LINE ­Y+50.,X+70.-R,­Y+50.,X+20.+R
  LINE ­X+50.,­Y+70.-R,­X+50.,­Y+20.+R
  LINE Y+50.,­X+70.-R,Y+50.,­X+20.+R
  SHOWPAGE
  PAUSE 0.05
  ADD T,0.15
NEXT i%
PRINT (TIMER-t%)
QUIT
