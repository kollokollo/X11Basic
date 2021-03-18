' Test of the new feature: True Type Fonts (>1.27-63)
' may work on linux and framebuffer

SETFONT "/usr/share/fonts/truetype/ubuntu/UbuntuMono-B.ttf"
' SETFONT "UbuntuMono-B.ttf"

COLOR COLOR_RGB(1,0,0)

DEFTEXT ,64,100,0
TEXT 180,370,date$
' keyevent
 SETFONT "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf"
COLOR COLOR_RGB(1,1,0)

DEFTEXT ,100,100,0
TEXT 100,200,"Hallo"



SHOWPAGE

COLOR COLOR_RGB(1,1,1)

' KEYEVENT


' SETFONT "/home/hoffmann/gitlab/solar/UbuntuMono-B.ttf"
SETFONT "/usr/share/fonts/truetype/msttcorefonts/Arial.ttf"
DEFTEXT ,12,100,20
TEXT 100,300,"This is a test with TTF."

SHOWPAGE
do
pause 1
SETFONT "/usr/share/fonts/truetype/ubuntu/UbuntuMono-B.ttf"

COLOR COLOR_RGB(1,0,0)

DEFTEXT ,64,100,0
TEXT 180,370,time$
SHOWPAGE
loop
KEYEVENT

QUIT

