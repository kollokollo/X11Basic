' GFA Punch 31 (code from tigen.ti-fr.com/)
' Feu d'artifice 2011-04-04
SIZEW ,256,256
DIM XU(15+1),YU(15+1),VcoU(15+1),VsiU(15+1),TU(15+1)
CLR IU
DO
  EXIT IF MOUSEK
  PLOT XU(IU),YU(IU)
  XU(IU)=XU(IU)+VcoU(IU)
  YU(IU)=YU(IU)-(VsiU(IU)-TU(IU))
  IF XU(IU)>0 AND XU(IU)<240 AND YU(IU)>0 AND YU(IU)<128
    PLOT XU(IU),YU(IU)
    TU(IU)=TU(IU)+1
  ELSE
    XU(IU)=120
    YU(IU)=64
    KU=2.+(RND()*12.)
    VcoU(IU)=KU*(-0.5+RND())
    VsiU(IU)=KU*(0.86+RND()*0.13)
    TU(IU)=0
  ENDIF
  IU=(-IU-1)*(IU<15)
  INC CountU
  IF CountU>=70
    SHOWPAGE
    PAUSE 0.05
    CLEARW
    CountU=0
  ENDIF
LOOP
QUIT
