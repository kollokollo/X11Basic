' Macht aus ASCII-Dateien ein Menu im *.mdf-Format
' Markus Hoffmann 1999    V.1.00
'
echo off
i=0
chrw=9
chrh=16
yoffs=120
xoffs=10
maxw=0
dim a$(200)
fileselect "ASCII-Datei auswaehlen:","./*.txt","",in$
if exist(in$)
  wort_sep in$,".txt",1,out$,a$
  out$=out$+".mdf"
  print "******"+out$
  open "I",#1,in$
  while not eof(#1)
    t$=lineinput$(#1)
    print t$
    if len(t$)>maxw
      maxw=len(t$)
    endif
    a$(i)=t$
    inc i
  wend
  close #1
  open "O",#2,out$
  print #2,"!"
  print #2,"! ascii2mdf 1.1 - "+date$+" - Do not edit here !"
  print #2,"!"
  print #2,"MenuValidation: VALID=1"
  print #2,"MenuVersion: VERSION=0 SV=1 SR=1 DATE=925124825 ORIGFNAME=ascii2mdf.mdf"
  print #2,"MenuProperty: XP=0 YP=0 WIDTH="+str$(chrw*maxw+2*xoffs)+" HEIGHT="+str$(chrh*i+yoffs)+" WBGC=[26214,26214,26214] FGC=[65535,65535,65535] BGC=[39321,32896,32896] LFONT=*-Helvetica-Medium-r-*-12-*-*-*-*-*-*-*"

  for u=0 to i-1
    print #2,"MenuString: XP="+str$(chrw+xoffs)+" YP="+str$(chrh*u+yoffs)+" WIDTH=0 HEIGHT=13 FGC=[65535,65535,65535] BGC=[52428,52428,52428] FONT=*-Courier-Bold-r-*-14-*-*-*-*-*-*-* TEXT="+chr$(34)+a$(u)+chr$(34)
  next u
  close #2

endif
quit
