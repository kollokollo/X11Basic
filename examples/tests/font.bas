echo off
setfont "*writer*18*"
color get_color(65535,10000,10000)
a=14
b=20
deftext 0,a/100,b/100,0
defline ,2
for x=0 to 15
for y=0 to 15

ltext (a+5)*y+5,(b+5)*x,chr$(y+16*x)
text 320+16*y,20+24*x,chr$(y+16*x)
vsync
next y
next x
alert 1,"Fertig !",1," OK ",dummy
alert 2,"Fertig 2!",1," OK ",dummy
alert 3,"Fertig 3!",1," OK ",dummy
