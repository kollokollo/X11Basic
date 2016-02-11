' GFA Punch (code from tigen.ti-fr.com/)
' Carré 3D en rotation
sizew ,100,100
T%=Timer
For iU=0 To 50*10
  Clearw
  X=Cos(T)*20.
  Y=Sin(T)*18.
  R=Sin(T+T)*2.5
  Line X+50.,Y+70.-R,­Y+50.,X+70.-R
  Line ­Y+50.,X+70.-R,­X+50.,­Y+70.-R
  Line ­X+50.,­Y+70.-R,Y+50.,­X+70.-R
  Line Y+50.,­X+70.-R,X+50.,Y+70.-R
  Line X+50.,Y+20.+R,­Y+50.,X+20.+R
  Line ­Y+50.,X+20.+R,­X+50.,­Y+20.+R
  Line ­X+50.,­Y+20.+R,Y+50.,­X+20.+R
  Line Y+50.,­X+20.+R,X+50.,Y+20.+R
  Line X+50.,Y+70.-R,X+50.,Y+20.+R
  Line ­Y+50.,X+70.-R,­Y+50.,X+20.+R
  Line ­X+50.,­Y+70.-R,­X+50.,­Y+20.+R
  Line Y+50.,­X+70.-R,Y+50.,­X+20.+R
  vsync
  pause 0.1
  Add T,0.15

  For JU=0 To 600
  Next JU
Next iU
Print (Timer-T%)
quit
