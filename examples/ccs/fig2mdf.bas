' Macht aus XFIG-Dateien ein Menu-mdf-Dateien
' Markus Hoffmann 1999
'
echo off
i=0
chrw=9
chrh=16
yoffs=120
xoffs=10
maxw=0
dim a$(2000)
fileselect "XFIG-Datei auswaehlen:","/sgt9/home/mhoffman/figures/*.fig","",in$
if exist(in$)
  open "I",#1,in$
  while not eof(#1)
    t$=lineinput$(#1)
    if len(t$)>maxw
      maxw=len(t$)
    endif
    print t$
    a$(i)=t$
    inc i
  wend
  close #1
  open "O",#2,"output.mdf"
  print #2,"!"
  print #2,"! fig2mdf 1.1 - "+date$+" - Do not edit here !"
  print #2,"!"
  print #2,"MenuValidation: VALID=1"
  print #2,"MenuVersion: VERSION=0 SV=1 SR=1 DATE=925124825 ORIGFNAME=ascii2mdf.mdf"
  print #2,"MenuProperty: XP=0 YP=0 WIDTH="+str$(chrw*80+2*xoffs)+" HEIGHT="+str$(chrh*50+yoffs)+" WBGC=[26214,26214,26214] FGC=[65535,65535,65535] BGC=[39321,32896,32896] LFONT=*-Helvetica-Medium-r-*-12-*-*-*-*-*-*-*"
  lw=1
  for u=0 to i-1
    t$=a$(u)
    wort_sep t$," ",1,w1$,w2$
    if w1$="2"
      print t$
      k$=trim$(a$(u+1))
      print k$
      wort_sep k$," ",1,x1$,k$
      wort_sep k$," ",1,y1$,k$
      wort_sep k$," ",1,x2$,k$
      wort_sep k$," ",1,y2$,k$
      x1=int(val(x1$)/50)
      y1=int(val(y1$)/50)
      x2=int(val(x2$)/50)
      y2=int(val(y2$)/50)
       
      print "Linie: "+x1$+" "+x2$
      print #2,"MenuLine: XP="+str$(x1)+" YP="+str$(y1)+" X2="+str$(x2)+" Y2="+str$(y2)+" LINEWIDTH="+str$(lw)+" FGC=[65535,65535,65535]"
      while len(k$)
        x1=x2
        y1=y2
	wort_sep k$," ",1,x2$,k$
        wort_sep k$," ",1,y2$,k$
      
        x2=int(val(x2$)/100)
        y2=int(val(y2$)/100)
	print #2,"MenuLine: XP="+str$(x1)+" YP="+str$(y1)+" X2="+str$(x2)+" Y2="+str$(y2)+" LINEWIDTH="+str$(lw)+" FGC=[65535,65535,65535]"

      wend
    endif
  next u
  close #2

endif
quit

function convert$(z$)
  print z$
  return ""
endfunc
