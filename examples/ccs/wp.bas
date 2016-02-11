' Laesst den Mauszeiger fallen ...
' Markus Hoffmann 1997

echo off
rootwindow

i=0
repeat
  setmouse 2,i,1  
  vsync
  inc i
until mousey>800
quit
