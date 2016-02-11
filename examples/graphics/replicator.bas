' Replicator, Edward Fredkin, Serafino Amoroso, Gerald Cooper
' implemented by Markus Hoffmann 2012

nc=4  ! Number of colors
s=2    ! size of the pixel
sw=48
sh=48
bw=3*sw
bh=3*sh
dim col(nc)

sizew ,s*bw,s*bh+16

col(0)=get_color(0,0,0)
col(1)=get_color(65535,0,0)
col(2)=get_color(0,65535,0)

col(nc-1)=get_color(65535,65535,65535)
  color col(0)
  pbox 0,0,sw,sh
  color col(nc-1)
  ' text 10,10,"Hallo"
  text bw*s/2,bh*s+12,"replicator with X11-Basic"
  color col(1)
  ' text 10,20,"yes"
  ' box 5,5,sw-1-5,sh-1-5 
  text sw*s/2,sh*s+16,"initializing"
  plot sw/2,sh/2
  color col(2)
  box 0,0,sw-1,sh-1
'  plot sw/2,sh/2
vsync
pause 1
dim feld(bw,bh),ofeld(bw,bh)
arrayfill feld(),0

   for x=0 to sw-1
     for y=0 to sh-1
       ' print point(x,y)
       for i=0 to nc-1
          if point(x,y)=col(i)
	  feld(sw+x,sh+y)=i
	endif
	next i
     next y
   next x
do
  @showit
  @calcit
  inc count
loop
quit

procedure calcit
   for x=1 to bw-2
     for y=1 to bh-2
        sum=ofeld(x-1,y)+ofeld(x-1,y-1)+ofeld(x,y-1)+ofeld(x+1,y-1)+ofeld(x+1,y)+ofeld(x+1,y+1)+ofeld(x,y+1)+ofeld(x-1,y+1)
        feld(x,y)=(sum mod nc)
     next y
   next x
return

procedure showit
  color col(0)
  pbox 0,0,bw*s,bh*s
   for x=0 to bw-1
     for y=0 to bh-1
        color col(feld(x,y))
	  pbox x*s,y*s,(x+1)*s-1,(y+1)*s-1

	ofeld(x,y)=feld(x,y)
     next y
   next x
   color col(nc-1)
   text 20,s*bh+12,str$(count)
   vsync
return

