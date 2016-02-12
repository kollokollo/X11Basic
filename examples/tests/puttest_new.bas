' open "I",#1,"/usr/share/icons/oxygen/48x48/actions/games-config-tiles.png"
'open "I",#1,"/usr/share/games/enigma/gfx64/st_yinyang.png"
' open "I",#1,"/usr/share/games/enigma/gfx64/it_bomb.png"
open "I",#1,"/usr/share/games/enigma/gfx64/player_switch_anim.png"
t$=input$(#1,LOF(#1))
close #1
p$=PNGDECODE$(t$)
color 0
do
pbox 0,0,640,400
put 0,0,p$,,,0,64*i,64,64
inc i
if i>19
i=0
endif
put 200,0,p$,2,,0,64*i*2,64*2,64*2
SHOWPAGE
pause 0.05
loop
KEYEVENT
