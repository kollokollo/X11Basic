' GFA Punch 31 (code from tigen.ti-fr.com/)
' Feu d'artifice
sizew ,256,256
Dim XU(15+1),YU(15+1),VcoU(15+1),VsiU(15+1),TU(15+1)
clr IU
Do

  Exit If mousek
  Plot XU(IU),YU(IU)
  XU(IU)=XU(IU)+VcoU(IU)
  YU(IU)=YU(IU)-(VsiU(IU)-TU(IU))
  If XU(IU)>0 And XU(IU)<240 And YU(IU)>0 And YU(IU)<128 Then
    Plot XU(IU),YU(IU)
    TU(IU)=TU(IU)+1
  Else
    XU(IU)=120
    YU(IU)=64
    KU=2.+(Rnd()*12.)
    VcoU(IU)=KU*(-0.5+Rnd())
    VsiU(IU)=KU*(0.86+Rnd()*0.13)
    TU(IU)=0
  Endif
  IU=(-IU-1)*(IU<15)
  Inc CountU
  If CountU>=70
  vsync
  pause 0.05
    Clearw
    CountU=0
  Endif
Loop
quit
