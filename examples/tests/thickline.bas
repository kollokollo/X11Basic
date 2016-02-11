defline ,80


color get_color(54424,0,0)

line 100,100,99,200

line 200,200,199,100




for i=0 to 360 step 30
  color get_color(65535,65535,0)
  line 400,400,400+200*cos(i/180*pi),400+200*sin(i/180*pi)
  vsync
  pause 0.1
  color 0
'  line 400,400,400+200*cos(i/180*pi),400+200*sin(i/180*pi)

 '  pause 0.01
next i
pause 1
quit
