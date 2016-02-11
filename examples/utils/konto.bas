
' Kontofuehrung grafisch angezeigt  (c) Markus Hoffmann
' Letzte Bearbeitung 20.12.1997



' Die Dateien mit den Kontobewegungen muessen folgendes Format haben:
'
' DATUM,        UMSATZ,  KONTOSTAND, Kommentar
' 02.05.1997,   1300.00,    2593.99, "Gehalt"
' 02.06.1997,   1300.00,    3893.99, "Gehalt"
' 10.06.1997,  -1893.00,    2000.99, "Restsparen"
' 30.06.1997,      1.30,    2002.29, "Zinsen"
' 01.07.1997,    -32.87,    1969.42, "Telekom-Rechnung"
' ...
'
'
'



echo off

'########## Hauptprogramm ##########
'##########     begin     ##########

tty=0

offset1=-70
faktor1=0.21
offset2=-3000
faktor2=1
DIM mon$(12)
RESTORE mdata
FOR i=1 TO 12
  READ mon$(i)
NEXT i
mdata:
DATA Jan,Feb,Mar,Apr,Mai,Jun,Jul,Aug,Sep,Okt,Nov,Dez

grau=get_color(65535/2,65535/2,65535/2)
weiss=get_color(65535,65535,65535)
rot=get_color(65535,0,0)
gelb=get_color(65535,65535,0)
gruen=get_color(0,65535,0)
blau=get_color(10000,10000,65535)
schwarz=get_color(0,0,0)

'
' Dateneingabe fr Konto:
'
konto$="konto.dat"

maxkontostand=22000
maxkontostand2=12000
bx=0
by=240 
bw=740 
bh=220
bx2=0
by2=10
bw2=740
bh2=200

sizew ,bw,bh+bh2+100
vsync
clearw
cls
COLOR grau
LINE bx,by+bh,bx+bw,by+bh
LINE bx,by,bx+bw,by
LINE bx,200,bx+bw,200
color gelb
setfont "*-Courier-*34*"
text 150,500,"Kontostand "+date$
deftext 1,0.04,0.04,0
setfont "*Courier*10*"



if tty
  input "Welches Konto anzeigen ? 1=DB, 2=Sparda, 3=SPK, 4=ABBRUCH",fa
else
  fa=form_alert(1,"[0][KONTO.BAS||Welches Konto anzeigen ?][DB|Sparda|andere|Quit]")
endif
if fa=4
  print "Dann ende ..."
  quit
else if fa=2
  konto$="sparda.dat"
  maxkontostand=6000
else if fa=3
  fileselect "Konto auswählen...","./*.dat","konto.dat",konto$
  if konto$=""
    quit
  endif
endif

count=0
FOR j=1991 TO 2002
  FOR m=1 TO 12
    x=((j-1992)*365+@mondaysum(m))*faktor1+offset1
    x2=((j-1992)*365+@mondaysum(m))*faktor2+offset2
    color grau
    LINE x2,@ky2(0)-3,x2,@ky2(0)
    color gelb
    TEXT x2+4,@ky2(0)+10,mon$(m)
  NEXT m
  color grau
  LINE x,@ky(maxkontostand),x,@ky(0)
  color gelb
  TEXT x,by-10,STR$(j+1))
  color grau
  FOR y=0 TO maxkontostand STEP int(maxkontostand/1000/5)*1000 
    LINE x-2,@ky(y),x+2,@ky(y)
    TEXT x+2,@ky(y),STR$(y)
  NEXT y
NEXT j

IF NOT EXIST(konto$)
  if form_alert(2,"[3]["+konto$+"|existiert nicht !||File anlegen ?][Neu|ABBRUCH]")=2
    print konto$;" existiert nicht !"
    end
  else
    OPEN "O",#1,konto$
    PRINT #1,date$+",0,0,"+""""+"Eroeffnung des Kontos "+""""
    CLOSE #1
  endif
ENDIF
OPEN "I",#1,konto$

COLOR grau
FOR y=0 TO maxkontostand STEP INT(maxkontostand/1000/10)*1000
  mmmy=@ky2(y)
  LINE 40,mmmy,bx2+bw2,mmmy
  TEXT 4,mmmy,STR$(y)
NEXT y
vsync
COLOR weiss
clr oldx,oldx2
oldy=@ky(0)
oldy2=@ky2(0)
cls

WHILE NOT EOF(#1)
  INPUT #1,d$,a,s,c$
 
  day=VAL(LEFT$(d$,2))
  mon=VAL(MID$(d$,4,2))
  ja=VAL(MID$(d$,7,4))
  PRINT at(1,1);day,mon$(mon);" ";ja;"   "
  x=((ja-1992)*365+@mondaysum(mon)+day)*faktor1+offset1
  x2=((ja-1992)*365+@mondaysum(mon)+day)*faktor2+offset2
  y=@ky(s)
  y2=@ky2(s)
  
  COLOR grau
  IF x>20 AND x<bw
    LINE x,@ky(0),x,y
  endif  
  IF x2>20 AND x2<bw
    LINE x2,@ky2(0),x2,y2
  endif
  IF s>0
    COLOR gruen 
  ELSE 
    COLOR rot 
  endif
  IF x>20 AND x<bw
    LINE oldx,oldy,x,oldy
  endif
  IF x2>20 AND x2<bw 
    LINE oldx2,oldy2,x2,oldy2
  endif
  IF x>20 AND x<bw
    LINE x,oldy,x,y
  endif
  IF x2>20 AND x2<bw
    LINE x2,oldy2,x2,y2
  endif
  COLOR blau
  IF x>20 AND x<bw
    LINE x,@ky(0),x,@ky(ABS(a))
  endif
  IF x2>20 AND x2<bw
    LINE x2,@ky2(0),x2,@ky2(ABS(a))
  endif
  oldx=x
  oldx2=x2
  oldy=y
  oldy2=y2
  if (count mod 100)=0
    vsync
  endif
  inc count
WEND
if tty
  cls
  vsync
  @ttyhardcopy
else
  alert 0,"Fertig !",1,"OK",dummy
endif
quit

' absolutes Ende (des Hauptprogramms)

FUNCTION ky(kky)
  return by+bh-kky/maxkontostand*bh
ENDFUNCTION

FUNCTION ky2(kky)
  return by2+bh2-kky/maxkontostand2*bh2
ENDFUNCTION

FUNCTION monday(n)
  if n=1 or n=3 or n=5 or n=7 or n=8 or n=10 or n=12
    return 31
  else if n=2
    return 28
  else if n=4 or n=6 or n=9 or n=11
    return 30
  else 
    return -1
  endif
ENDFUNCTION

FUNCTION mondaysum(n)
  LOCAL i,s,o
  s=0
  FOR i=1 TO n
    o=@monday(i)
    IF o<0 
      RETURN o
    ENDIF
    s=s+o
  NEXT i
  return s
ENDFUNCTION
procedure ttyhardcopy
local bbp,bx,by,bw,bh
bbp=8
bx=0
by=0
bw=640
bh=400

dim zeil(16)
dim old(16)


for y=0 to 399 step 16
  for x=0 to bw-1 step 8
    flag=0
    for j=0 to 15
      a=0
  
      for i=0 to 7
        u=point(x+i,y+j)
        if bbp=24
        r=u and 255
        g=(u/256) and 255
        b=(u/256/256) and 255
        u=sqrt(r*r+g*g+b*b)
        if random(u)>80 or u>350
          a=bset(a,7-i)
          flag=1
        endif
        else
          if u and 1
            a=bset(a,7-i)
            flag=1
          endif
        endif
      next i
      zeil(j)=a
    next j
    if flag=0
      print " ";
    else
      rflag=0
      for i=0 to 15
        if old(i)<>zeil(i)
          rflag=1
        endif
      next i
      if rflag
      print chr$(27);"[255;";
      for j=0 to 14
        print str$(zeil(j))+";";
        old(j)=zeil(j)
      next j
      old(15)=zeil(15)
      print str$(zeil(15))+"z";
      endif
      print chr$(255);
    endif
  next x
  flush
  if bw<640
   print
  endif
next y
return
