' This example program demonstrates the data fitting
' functions of X11-Basic  (c) Markus Hoffmann 2003-2013
' 

' Fill the data arrays with 400 values. With the mouse you can modify
' the distribution a bit.
'
' The data values x() and y() should be small, such that high potences 
' of them will not overflow.
'
n=400
DIM x(n),y(n),e(n),p(4)
FOR i=0 TO n-1
  x(i)=(i+RANDOM(10))/400
  y(i)=(1*i+GASDEV(1)*20+50-i/30*i/30+(400-MOUSEY))/400
  e(i)=1
  PLOT x(i)*400,400-y(i)*400
  SHOWPAGE
  PAUSE 0.01
NEXT i

' First do a linear fit
COLOR COLOR_RGB(1,1,1)
FIT_LINEAR x(),y(),,a,b,da,db,chi
print a;"+/-";da
print b;"+/-";db
print "chi2=";chi
print "q=";q
da=da*sqrt(chi/398)
db=db*sqrt(chi/398)
line 0,400-a*400,400,400-(a+b)*400
color color_rgb(0.5,0.5,0.5)
line 0,400-(a+da)*400,400,400-(a+da+b)*400
line 0,400-(a-da)*400,400,400-(a-da+b)*400
line 0,400-(a)*400,400,400-(a+db+b)*400
line 0,400-(a)*400,400,400-(a-db+b)*400

vsync
pause 5
print
' And now do several polynomial fits with increasing order
for oo=3 to 500
  @fii(oo)
  if oo>10
    add oo,5
  endif
  if oo>100
    add oo,50
  endif
next oo
~inp(-2)
quit


procedure fii(oo)
  dim p(oo)
  clr ox,oy
  color color_rgb(0,1,oo/10)
  text 300,30,"Order: "+str$(oo)+"  "
  fit_poly x(),y(),e(),400,p(),oo
  for i=0 to oo-1
    if p(i)>0.0005
    print i,p(i) 
    endif
  next i
  for x=0 to 400
    y=0
    for i=0 to oo-1
      add y,p(i)*(x/400)^i
    next i
    line ox,400-oy*400,x,400-y*400
    ox=x
    oy=y
  next x
  SHOWPAGE
  PAUSE 0.3
  PRINT
RETURN
