' Test the SOUND command. It Sounds the internal speaker.
' permissions for /dev/console have to be set correctly if xterm
' is used.     (c) Markus Hoffmann 2003
'
do
  read f
  exit if f=-1
  read p
  sound f
  pause p/1000
loop 
sound 0
quit

data 100,100
data 200,100
data 300,100
data 400,100
data 500,100
data 600,100
data 100,100
data 200,100
data 300,100
data 400,100
data 500,100
data 600,100
data 500,100
data 400,100
data 300,100
data 200,100
data 100,100
data 100,100
data 0,600
data 400,50
data 00,50
data 400,50
data 00,50
data 400,50
data 00,50
data 600,80
data 00,50
data 400,80
data 00,50
data -1,-1
