
' savescreen Test for X11-basic (c) Markus hoffmann 2008
' 
' 
' In the framebuffer-version (TomTom etc) the file format is
' .bmp 24 Bit
' 
for i=0 to 32
for j=0 to 20
  circle i*20,j*20,8
  if odd(j+i)
    pcircle i*20,j*20,3
  endif
next j
next i

circle 100,100,30
circle 200,200,60

pcircle 200,300,70

a$=system$("locate .bmp")
while len(a$)
  wort_sep a$,chr$(10),0,t$,a$
  if exist(t$)
    open "I",#1,t$
    t$=input$(#1,lof(#1))
    put 0,0,t$
    close
    vsync
    pause 1
  endif
wend

quit
