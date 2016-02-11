' Test the FILL command (flood fill algorithm) Markus Hoffmann 2005
color get_color(65535,65535,0)
pbox 0,0,640,400
alert 1,"ready ?",1,"OK",b
color get_color(65535,65535,0)
pbox 0,0,640,400
color get_color(65535,65535,65535)
line 10,10,600,300
color get_color(0,0,0)
line 600,10,10,300
circle 400,400,50
circle 300,400,50
circle 300,300,50
circle 100,100,50
circle 180,100,50
circle 250,100,50
circle 250,140,50
text 280,200,"This is a flood-fill test"
vsync
color get_color(0,0,65535)
fill 200,200
alert 1,"fertig",1,"OK",b
color get_color(0,65535,0)
fill 200,200
alert 1,"fertig",1,"OK",b
color get_color(0,0,0)
fill 200,200
alert 1,"fertig",1,"OK",b
color get_color(65535,0,0)
fill 200,200
alert 1,"fertig",1,"OK",b
quit
