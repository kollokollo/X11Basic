' ######################################################################
' ##                  ATOMIUM                              V. 1.01    ##
' ##                                                                  ##
' ##                  begonnen am 15.6.1992    Vorversion: 03.10.1982 ##
' ##   Markus Hoffmann                   letzte Bearbeitung: 03.08.97 ##
' ######################################################################
'
echo off
DATA (c) Markus Hoffmann ATOMIUM.PRG  erstellt 20.11.1992  V. 1.02
bx=0
by=20
bw=640
bh=380         ! Bildschirmdaten
ADD by,1
SUB bw,1
ADD bh,by-2
lc=0

rot=get_color(65530,0,0)
gelb=get_color(65530,40000,0)
grau=get_color(65530/2,65530/2,65530/2)
weiss=get_color(65530,65530,65530)
schwarz=get_color(0,0,0)
lila=get_color(65530,0,65530)
gruen=get_color(0,65530)
defmouse 2

@init
LET sound=TRUE
@l.i
dim stack(1000)
stackp=1000
stackend=1000

s$(1)="Computer"
s$(2)="Spieler"
@spielfeld
@spieler
spieler=-RANDOM(2)
print "Spieler: "+str$(spieler)
LINE 0,303,639,303
LINE 0,322,639,322
spiel=TRUE
ON MENU BUTTON 1,1,1 GOSUB button
' every 1,lauf
CLR cnt
defmouse 0
DO
  color gruen
  pcircle 10,360,4
  color schwarz
  circle 10,360,4
  vsync
  REPEAT
    @arbeit
    @lauf
  UNTIL in_arbeit=0
  color weiss
  pcircle 10,360,4
  color schwarz
  circle 10,360,4
  spieler=NOT spieler
  @check_win
  defmouse 2
  IF s$(ABS(spieler)+1)="Computer" AND spiel
    timevalue=ctimer
    @computer
    print "Zugzeit: ";ctimer-timevalue
    ' Zug ausfuehren
    stack(stackp-4)=256*255
    stack(stackp-3)=0
    stack(stackp-2)=x
    stack(stackp-1)=y
    SUB stackp,4
  ELSE IF s$(ABS(spieler)+1)="Thinkhalf" AND spiel
    
    timevalue=ctimer
    @thinkhalf(spieler)
    print "Zugzeit: ";ctimer-timevalue
    ' Zug ausfuehren
    stack(stackp-4)=256*255
    stack(stackp-3)=0
    stack(stackp-2)=x
    stack(stackp-1)=y
    SUB stackp,4
  ELSE IF s$(ABS(spieler)+1)="Thinklocal" AND spiel
    
    @local
    ' Zug ausfuehren
    stack(stackp-4)=256*255
    stack(stackp-3)=0
    stack(stackp-2)=x
    stack(stackp-1)=y
    SUB stackp,4
  ELSE IF s$(ABS(spieler)+1)="Think1" AND spiel
    
    @think1
    ' Zug ausfuehren
    stack(stackp-4)=256*255
    stack(stackp-3)=0
    stack(stackp-2)=x
    stack(stackp-1)=y
    SUB stackp,4
  ELSE
    fertig=0
    defmouse 0
    REPEAT
      VSYNC
     
'      IF INP?(2)
 '       ~INP(2)
       
'        FOR mx=0 TO xspielfeld-1
'          FOR my=0 TO yspielfeld-1
'            TEXT mx*32,my*32+18,STR$(cfeld(mx,my))
'          NEXT my
'        NEXT mx
'      ENDIF
      mouseevent x,y,k
      @button(x,y,k)
    UNTIL fertig
  ENDIF
  print "Zug: ",x,y
  vsync
  INC cnt
LOOP
end
PROCEDURE button(x,y,k)
  '
  ' Hier, falls Mausklick
  '
  LOCAL ob,dummy$
  if @inbutton(600,380,"QUIT",x,y)
    @mbutton(600,380,"QUIT",1)
    vsync
    @ende
  else if @inbutton(300,380,"Zugvorschlag",x,y)
    @mbutton(300,380,"Zugvorschlag",1)
    defmouse 2
    vsync
    @zugvorschlag
    defmouse 0
    @mbutton(300,380,"Zugvorschlag",0)
    vsync
  else if @inbutton(500,380,"Sound",x,y)
    sound=not sound
    @mbutton(500,380,"Sound",abs(sound))
    vsync
  else if @inbutton(400,380,"neues Spiel",x,y)
    @mbutton(400,380,"neues Spiel",1)
    defmouse 2
    vsync
    @spieler
    spieler=-RANDOM(2)
    spiel=TRUE
    CLR cnt,feld()
    @show_f
    defmouse 0
    @mbutton(400,380,"neues Spiel",0)
    vsync
  else if @inbutton(200,380,"Farbwechsel",x,y)
    @mbutton(200,380,"Farbwechsel",1)
    vsync
    dummy$=s$(1)
    s$(1)=s$(2)
    s$(2)=dummy$
    spieler=NOT spieler
    fertig=TRUE    
    @mbutton(200,380,"Farbwechsel",0)
    vsync
  else if @inbutton(100,380,"Info",x,y)
    @mbutton(100,380,"Info",1)
    vsync
    @info
    @mbutton(100,380,"Info",0)
    vsync
  else
    IF spiel
      x=x DIV 32
      y=y DIV 32
      IF NOT (x>=xspielfeld OR y>=yspielfeld)
        IF (feld(x,y) and -256)/256=ABS(spieler)+1 OR (feld(x,y) and -256)/256=0
    	  stack(stackp-4)=256*255
          stack(stackp-3)=0
          stack(stackp-2)=x
          stack(stackp-1)=y  
          SUB stackp,4
          fertig=TRUE
        ELSE
          beep
        ENDIF
      ENDIF
    ENDIF
  ENDIF
RETURN
PROCEDURE l.i
  lt$=SPACE$(80)
  lt$=lt$+"* * * * * * * *   Das Programm, auf das die Welt gewartet hat ! Es ist ein weiteres Spiel von RF Soft !    Es heižt     ---> UND ... UND ... <---              Das große Spiel "
  lt$=lt$+"um kleine Teilchen (Atome)  !   Wir wünschen Ihnen mit diesem Programm viel Spaß ! "
  lt$=lt$+"--> --> --> --> -->  UND ... UND ... wurde programmiert von Thorsten "
  lt$=lt$+" Frei und Michael Rennekamp. (ATARI-VERSION und X11-BASIC-Version Markus Hoffmann) Eventuelle Geld- und Sachspenden bitte an eine der folgenden Adressen:     Michael Rennekamp , Oststraže 55 (Am Ostbunker) , 4500 Osnabrck "
  lt$=lt$+"   oder      Thorsten Frei  , Max-Reger-Straže 26 , 4500 Osnabrück      Wir bedanken uns ganz herzlich bei unserem Informatiklehrer fr die Flasche Sekt (Alles Gute fr Moritz auf seinem weiteren Leidensweg). Wir gržen Onkel Thomas. Es "
  lt$=lt$+"folgen ein paar Worte in seiner Muttersprache: ™hä Bööhrrr! Mageritte, kernig! Ihr fliegenden Sackratten! NEIN! Ihr Schrumpfhirne!    "
  lt$=lt$+"Wenn Ihnen UND ... UND ... gefällt (oder auch nicht), dann sollten Sie es auf keinen Fall vers„umen, uns eine eine Geld- oder Sachspende zukommen zu lassen. RF Soft bedankt sich schon mal im voraus. HALT!!! Wir gržen auch Gnther und "
  lt$=lt$+"Carsten und Markus und Hendrik und Hartmut und Olav und Detlef und Gott und die Welt und natrlich LUTZ !      "
  lt$=lt$+"                                                                                               "
  lt$=lt$+"Hier noch ein paar Tricks zu dieser Programmversion:   Anstelle des Standardalgorithmus 'Computer' gibt es noch einige Varianten: 'Thinklocal' 'Thinkhalf' und 'Think1'. Viel Spass beim Ausprobieren ..."
  lt$=lt$+"                                                                                               "
  lt$=lt$+"Es ist brigens auch eine IBM-Version dieses Spiels erhältlich ! Wir verschicken sie ggf. auf Anfrage gegen eine Sharewaregebhr von DM 23,- ...      "
  lt$=lt$+"                                                                                               "
  lt$=lt$+"Verbesserungsvorschläge und Fehlerberichte in der ATARI-Version bitte an UZS0E7@IBM.RHRZ.UNI-BONN.DE .     "
RETURN
PROCEDURE lauf
  INC lc
  IF lc/2>=LEN(lt$)
    lc=0
  ENDIF
  color weiss
  pbox 0,308,640,321
  color rot
  deftext 0,0.064,0.1,0
  text 0,318,MID$(lt$,lc/2+1,100)
  vsync
RETURN
PROCEDURE setzte(x,y)
  feld(x,y)=((feld(x,y) and 255)+1) or (ABS(spieler)+1)*256
  @make(x,y)
RETURN
PROCEDURE arbeit
  IF stackp<>stackend
    @do_hscaler(500,330,100,1-stackp/stackend)
    print stackp,hex$(stack(stackp))
    WHILE stack(stackp)=32*256+32
      ADD stackp,4
    WEND
    stackp=MIN(stackp,stackend)
    IF stackp<>stackend
      ss=stackp
      WHILE ss<>stackend
        c=stack(ss) AND 255
        chi=(stack(ss) AND -256) DIV 256
        cr=stack(ss+1)
        cx=stack(ss+2)
        cy=stack(ss+3)
        if c=0
        
          @setzte(cx,cy)
          stack(ss)=32*256+32
	  stack(ss+1)=32*256+32
	  stack(ss+2)=32*256+32
	  stack(ss+3)=32*256+32
        else if c<10 and c>0 
          stack(ss)=(stack(ss) AND -256) OR (c-1)
          IF (feld(cx,cy) and -256)/256=1
            color weiss
          ELSE
            color schwarz
          ENDIF
          cx2=cx*32+16
          cy2=cy*32+16
          if cr=1
            cx1=cx*32+16-32
            cy1=cy*32+16
          else if cr=2
            cx1=cx*32+16+32
            cy1=cy*32+16
          else if cr=3
            cx1=cx*32+16
            cy1=cy*32+16-32
          else if cr=4
            cx1=cx*32+16
            cy1=cy*32+16+32
          endif
          PCIRCLE cx2+(c)*(cx1-cx2)/chi,cy2+(c)*(cy1-cy2)/chi,4
          IF c<>chi
	    color weiss
            PCIRCLE cx2+(c+1)*(cx1-cx2)/chi,cy2+(c+1)*(cy1-cy2)/chi,4
	    vsync
          ENDIF
	  color schwarz
	  CIRCLE cx2+(c)*(cx1-cx2)/chi,cy2+(c)*(cy1-cy2)/chi,3
          vsync
        endif
        ADD ss,4
      WEND
      in_arbeit=TRUE
    ELSE
      CLR in_arbeit
    ENDIF
  ELSE
    CLR in_arbeit
  ENDIF
RETURN
PROCEDURE init
  xspielfeld=8    
  yspielfeld=8
  DIM s$(2)
  DIM cfeld(xspielfeld-1,yspielfeld-1)  ! Hier werden die Bewertungen abgespeichert
  DIM cfeld2(xspielfeld-1,yspielfeld-1)  ! buffer
  DIM feld(xspielfeld-1,yspielfeld-1)   ! Das ist das Spielfeld: im oberen Byte ist der Spieler codiert
  DIM feld2(xspielfeld-1,yspielfeld-1)   ! Das ist ein buffer
  DIM feld3(xspielfeld-1,yspielfeld-1)   ! Das ist ein buffer
  DIM i1$(4),i2$(4)
  arrayfill feld(),0
  randomize
RETURN
FUNCTION anzsteine(s)
  LOCAL x,y,a
  color gelb
  pcircle 10,350,4
  color schwarz
  circle 10,350,4
  vsync
  CLR a
  FOR x=0 TO xspielfeld-1
    FOR y=0 TO yspielfeld-1
      IF (feld(x,y) and -256)/256=s OR s=-1
        ADD a,feld(x,y) and 255
      ENDIF
    NEXT y
  NEXT x
  
  color weiss
  pcircle 10,350,4
  color schwarz
  circle 10,350,4
  RETURN a
ENDFUNC
FUNCTION anzfelder(s)
  LOCAL x,y,a
  
  IF s=-1
    RETURN xspielfeld*yspielfeld
  ELSE
  color gelb
  pcircle 20,350,4
  color schwarz
  circle 20,350,4
  vsync
    CLR a
    FOR x=0 TO xspielfeld-1
      FOR y=0 TO yspielfeld-1
        IF (feld(x,y) and -256)/256=s
          INC a
        ENDIF
      NEXT y
    NEXT x
    color weiss
  pcircle 20,350,4
  color schwarz
  circle 20,350,4
    RETURN a
  ENDIF
ENDFUNC
FUNCTION anzdreier(s)
  LOCAL x,y,a
  CLR a
   color rot
  pcircle 30,350,4
  color schwarz
  circle 30,350,4
  vsync
  FOR x=0 TO xspielfeld-1
    FOR y=0 TO yspielfeld-1
      IF (feld(x,y) and -256)/256=s OR s=-1) AND (feld(x,y) and 255)=3
        INC a
      ENDIF
    NEXT y
  NEXT x 
  color weiss
  pcircle 30,350,4
  color schwarz
  circle 30,350,4
  RETURN a
ENDFUNC
PROCEDURE check_win
  LOCAL x,y,a,b
  CLR a,b
  FOR x=0 TO xspielfeld-1
    FOR y=0 TO yspielfeld-1
      IF (feld(x,y) and -256)/256=1
        ADD a,feld(x,y) and 255
      ENDIF
      IF (feld(x,y) and -256)/256=2
        ADD b,feld(x,y) and 255
      ENDIF
    NEXT y
  NEXT x
  
  color weiss
  pbox 320,130,520,300
  color schwarz
  box 320,130,520,300
  
  setfont "*-courier-bold-r-*-14-*"
  text 350,160,"Spielsteine:"  
  text 340,260,s$(abs(spieler)+1)+" ist dran !"

  setfont "*-courier-medium-r-*-12-*"
 
  text 350,220,"Gesamt: "
  text 350,180,s$(1)+" (weiß):    "
  text 350,200,s$(2)+" (schwarz): "
  text 480,180,STR$(a,4)
  text 480,200,STR$(b,4)
  text 480,220,STR$(a+b,4)

  IF a=0 AND b>1
    color 2
    pbox 320,240,520,300
    color schwarz
    box 320,240,520,300
    text 350,260,s$(2)+" hat gewonnen !"
    CLR spiel
  ENDIF
  IF b=0 AND a>1
    color 2
    pbox 320,240,520,300
    color schwarz
    box 320,240,520,300
    text 350,260,s$(1)+" hat gewonnen !"
    CLR spiel
  ENDIF
  vsync
RETURN
PROCEDURE show_f
  color weiss
  PBOX 0,0,8*32,8*32
  color schwarz
  PBOX 16,16,7*32+16,7*32+16
  color weiss
  FOR x=0 TO xspielfeld-1
    FOR y=0 TO yspielfeld-1
      @make(x,y)
    NEXT y
  NEXT x
  BOX 0,0,8*32,8*32
  BOX 0,0,8*32+2,8*32+2
  BOX 0,0,8*32+3,8*32+3
  vsync
RETURN
PROCEDURE make(x,y)
  @draw_platz(x,y,(feld(x,y) and -256)/256,feld(x,y) and 255)
  IF (feld(x,y) and 255)=4
    @explode((feld(x,y) and -256)/256,x,y)
  ENDIF
RETURN
PROCEDURE explode(a,x,y)
  IF sound
    WAVE -7,3,2,20000
  ENDIF
  feld(x,y)=0
  @make(x,y)
  arandom=RANDOM(3)+4
  IF x<xspielfeld-1
    feld(x+1,y)=a*256 or (feld(x+1,y) and 255)
    stack(stackp-4)=arandom*256+arandom
    stack(stackp-3)=1
    stack(stackp-2)=x+1
    stack(stackp-1)=y
    SUB stackp,4
  ENDIF
  IF x>0
    feld(x-1,y)=a*256 or (feld(x-1,y) and 255)
    stack(stackp-4)=arandom*256+arandom
    stack(stackp-3)=2
    stack(stackp-2)=x-1
    stack(stackp-1)=y
    SUB stackp,4
  ENDIF
  IF y<yspielfeld-1
    feld(x,y+1)=a*256 or (feld(x,y+1) and 255)
    stack(stackp-4)=arandom*256+arandom
    stack(stackp-3)=3
    stack(stackp-2)=x
    stack(stackp-1)=y+1
    SUB stackp,4
  ENDIF
  IF y>0
    feld(x,y-1)=a*256 or (feld(x,y-1) and 255)
    stack(stackp-4)=arandom*256+arandom
    stack(stackp-3)=4
    stack(stackp-2)=x
    stack(stackp-1)=y-1
    SUB stackp,4
  ENDIF
RETURN
PROCEDURE zugvorschlag
  LOCAL i
  @computer
  FOR i=0 TO 10
    @draw_platz(x,y,spieler,(feld(x,y) and 255)+1)
    vsync
    PAUSE 0.3
    @make(x,y)
    vsync
    pause 0.3
  NEXT i
RETURN
procedure draw_platz(x,y,g,a)
  color weiss
  PCIRCLE x*32+16,y*32+16,16
  color schwarz
  CIRCLE x*32+16,y*32+16,16
  if a>0
    IF g=1
      color weiss
    else 
      color schwarz
    endif
    if a=1
      PCIRCLE x*32+16,y*32+16,4
    else if a=2
      PCIRCLE x*32+16-7,y*32+16,4
      PCIRCLE x*32+16+7,y*32+16,4
    else if a=3
      PCIRCLE x*32+16-7,y*32+16+7,4
      PCIRCLE x*32+16+7,y*32+16+7,4
      PCIRCLE x*32+16,y*32+16-7,4
    else if a=4
      PCIRCLE x*32+16-7,y*32+16+7,4
      PCIRCLE x*32+16+7,y*32+16+7,4
      PCIRCLE x*32+16-7,y*32+16-7,4
      PCIRCLE x*32+16+7,y*32+16-7,4
    endif
    color schwarz
    if a=1
      CIRCLE x*32+16,y*32+16,4
    else if a=2
      CIRCLE x*32+16-7,y*32+16,4
      CIRCLE x*32+16+7,y*32+16,4
    else if a=3
      CIRCLE x*32+16-7,y*32+16+7,4
      CIRCLE x*32+16+7,y*32+16+7,4
      CIRCLE x*32+16,y*32+16-7,4
    else if a=4
     CIRCLE x*32+16-7,y*32+16+7,4
     CIRCLE x*32+16+7,y*32+16+7,4
     CIRCLE x*32+16-7,y*32+16-7,4
     CIRCLE x*32+16+7,y*32+16-7,4
    endif
  endif
return

PROCEDURE computer
  ' Dies ist der zusammengestellte Algorithmus
  '
  '
  local a,g
  
  g=ABS(NOT spieler)+1  ! Gegner
  a=@anzdreier(-1)
  print "Dreier: ";a
  
  if a
    @think1
  ELSE
    @local
  ENDIF
RETURN
PROCEDURE local
  ' Dies ist der lokale Algorithmus
  '
  local s,g,muh
  '
  '
  s=ABS(spieler)+1      ! Spieler
  g=ABS(NOT spieler)+1  ! Gegner
  IF cnt=0   ! Computer fängt an
    x=RANDOM(xspielfeld)
    y=RANDOM(yspielfeld)
  ELSE
    clr cfeld()
    FOR x=0 TO xspielfeld-1
      FOR y=0 TO yspielfeld-1
        
        IF (feld(x,y) and -256)/256=g   ! Auf gegnerische Felder nicht setzten
          cfeld(x,y)=-32000
        ELSE
          ' Eigene Atome auf diagonalenm feldern als +Punkte
          ADD cfeld(x,y),@inh(x-1,y-1,s)+@inh(x+1,y+1,s)+@inh(x+1,y-1,s)+@inh(x-1,y+1,s)
          ' Gegnerische Atome auf diagonalen feldern als +Punkte
          ADD cfeld(x,y),@inh(x-1,y-1,g)+@inh(x+1,y+1,g)+@inh(x+1,y-1,g)+@inh(x-1,y+1,g)
          muh=@feld(x,y)
          IF muh=0   !Leeres feld
            ' Benachbarte Felder als +Punkte (max. 12)
            cfeld(x,y)=cfeld(x,y)+@inh(x-1,y,-1)+@inh(x+1,y,-1)+@inh(x,y-1,-1)+@inh(x,y+1,-1)
            ' Diagonale Felder als +Punkte   (max. 12)
            ADD cfeld(x,y),@inh(x-1,y-1,-1)+@inh(x+1,y+1,-1)+@inh(x+1,y-1,-1)+@inh(x-1,y+1,-1)
           
              ' Benachbarte gegnerische 3er als -Punkte (max. 4*12)
              a=ABS(@inh(x-1,y,g)=3)+ABS(@inh(x+1,y,g)=3)+ABS(@inh(x,y-1,g)=3)+ABS(@inh(x,y+1,g)=3)
              SUB cfeld(x,y),a*12
            
          ELSE IF muh=2   
            ' "Gegnerischer 3er ?
            a=ABS(@inh(x-1,y,g)=3)+ABS(@inh(x+1,y,g)=3)+ABS(@inh(x,y-1,g)=3)+ABS(@inh(x,y+1,g)=3)
            IF a=0
              ADD cfeld(x,y),4
              ' Benachbarte Felder als +Punkte (max. 12)
              ADD cfeld(x,y),@inh(x-1,y,-1)+@inh(x+1,y,-1)+@inh(x,y-1,-1)+@inh(x,y+1,-1)
            ENDIF
          ELSE IF muh=3   !3er ...
            ' anzahl der benachbarten gegnerischen  3er
            ' (max. 4*10 Pkt)
            a=ABS(@inh(x-1,y,g)=3)+ABS(@inh(x+1,y,g)=3)+ABS(@inh(x,y-1,g)=3)+ABS(@inh(x,y+1,g)=3)
            ADD cfeld(x,y),a*10
            ' bei explosion einnehmbare gegnerische Felder
            a=@inh(x-1,y,g)+@inh(x+1,y,g)+@inh(x,y-1,g)+@inh(x,y+1,g)
            ADD cfeld(x,y),a*3
            ' Verluste am Rand vermeiden
            
            IF y=0 OR y=yspielfeld-1 OR x=0 OR x=xspielfeld-1
              cfeld(x,y)=cfeld(x,y)-2
            ENDIF
	    ' Bei explosion generell vorsicht
            cfeld(x,y)=cfeld(x,y)-2  
          ELSE ! 1 Atom schon im feld
            ' Gegnerische einnehmbare Felder als -Punkte
            ADD cfeld(x,y),@inh(x-1,y,g)+@inh(x+1,y,g)+@inh(x,y-1,g)+@inh(x,y+1,g)
            a=ABS(@inh(x-1,y,g)=3)+ABS(@inh(x+1,y,g)=3)+ABS(@inh(x,y-1,g)=3)+ABS(@inh(x,y+1,g)=3)
            IF a>0
              SUB cfeld(x,y),9
            ENDIF
            ADD cfeld(x,y),3
          ENDIF
        ENDIF
      NEXT y
    NEXT x
    '
    
    ' Suche jetzt das beste Feld heraus
    '
    @getbest
  ENDIF
RETURN
PROCEDURE getbest
  ' sucht aus cfeld() das beste Feld heraus
  '
  LOCAL a,b,exitf
  CLR a
  m=-32000
  FOR mx=0 TO xspielfeld-1
    FOR my=0 TO yspielfeld-1
        if cfeld(mx,my)<-20
	  @led(mx,my,rot)
	else if cfeld(mx,my)<0
	  @led(mx,my,gelb) 
	else if cfeld(mx,my)<2
	  @led(mx,my,weiss)
	else if cfeld(x,y)>10
	  @led(mx,my,gruen)
	else if cfeld(x,y)>5
	  @led(mx,my,lila)
	  
	else
	  @led(mx,my,cfeld(mx,my))
	endif
      IF cfeld(mx,my)>m
        m=cfeld(mx,my)
        CLR a
      ELSE IF cfeld(mx,my)=m
        INC a
      ENDIF
    NEXT my
  NEXT mx
  b=RANDOM(a+1)
  CLR a,exitf
  FOR mx=0 TO xspielfeld-1
    FOR my=0 TO yspielfeld-1
      IF cfeld(mx,my)=m
        INC a
        x=mx
        y=my
        IF a>=b
          exitf=TRUE
        ENDIF
      ENDIF
      EXIT IF exitf
    NEXT my
    EXIT IF exitf
  NEXT mx
  IF m=-32000
    STOP  !irgendwas stimmt nicht !
    CLR x,y
  ENDIF
RETURN
PROCEDURE thinkhalf(spieler)
  ' Computeralgorithmus
  ' Muss die Variablen x y und m fuer den besten Zug hinterlassen
  ' wird rekursiv aufgerufen
  '
  LOCAL as,af,s,g,i,j,ii,jj
  s=ABS(spieler)+1      ! Spieler
  g=ABS(NOT spieler)+1  ! Gegner
  
  IF cnt=0   ! Computer f„ngt an
    x=RANDOM(xspielfeld)
    y=RANDOM(yspielfeld)
  ELSE
    as=@anzsteine(s)
    af=@anzfelder(s)
    FOR i=0 TO xspielfeld-1
      FOR j=0 TO yspielfeld-1
        feld2(i,j)=feld(i,j)
      NEXT j
    NEXT i
    clr cfeld()
    FOR ii=0 TO xspielfeld-1
      FOR jj=0 TO yspielfeld-1
        IF (feld2(ii,jj) and -256)/256<>g
          @dozug(ii,jj,s)
          cfeld(ii,jj)=@anzfelder(s)+@anzsteine(s)-af-as
          FOR i=0 TO xspielfeld-1
            FOR j=0 TO yspielfeld-1
              feld(i,j)=feld2(i,j)
            NEXT j
          NEXT i
        ELSE
          cfeld(ii,jj)=-32000
        ENDIF
      NEXT jj
    NEXT ii
    @getbest
  ENDIF
RETURN
PROCEDURE think1
  ' Computeralgorithmus
  ' Muss die Variablen x y und m fuer den besten Zug hinterlassen
  LOCAL as,af,s,g,i,j,ii,jj
  s=ABS(spieler)+1      ! Spieler
  g=ABS(NOT spieler)+1  ! Gegner
  
  IF cnt=0   ! Computer f„ngt an
    x=RANDOM(6)+1
    y=RANDOM(6)+1
  ELSE
    as=@anzsteine(s)
    af=@anzfelder(s)
    FOR i=0 TO xspielfeld-1
      FOR j=0 TO yspielfeld-1
        feld3(i,j)=feld(i,j)
      NEXT j
    NEXT i
    clr cfeld()
    
    FOR ii=0 TO xspielfeld-1
      FOR jj=0 TO yspielfeld-1
        PLOT ii,jj+305
        vsync
        IF (feld3(ii,jj) and -256)/256<>g
          @dozug(ii,jj,s)
          IF @anzfelder(g)
            @thinkhalf(NOT spieler)
            @dozug(x,y,g)
            cfeld2(ii,jj)=@anzfelder(s)+@anzsteine(s)-af-as
          ELSE
            ' Hat er gewonnen
            cfeld2(ii,jj)=999
          ENDIF
          FOR i=0 TO xspielfeld-1
            FOR j=0 TO yspielfeld-1
              feld(i,j)=feld3(i,j)
            NEXT j
          NEXT i
        ELSE
          cfeld2(ii,jj)=-32000
        ENDIF
      NEXT jj
    NEXT ii
    FOR i=0 TO xspielfeld-1
      FOR j=0 TO yspielfeld-1
        cfeld(i,j)=cfeld2(i,j)
      NEXT j
    NEXT i

    @getbest
  ENDIF
RETURN
PROCEDURE dozug(x,y,g)

  ' Fuehrt einen Zug aus, kann rekursiv aufgerufen werden
  ' nur interne Berechnung des neuen feld()
  '
  '
 
  feld(x,y)=(g*256 or (feld(x,y) and 255))
  IF (feld(x,y) and 255)=3
    feld(x,y)=feld(x,y) and -256
    IF x>0
      @dozug(x-1,y,g)
    ENDIF
    IF y>0
      @dozug(x,y-1,g)
    ENDIF
    IF x<xspielfeld-1
      @dozug(x+1,y,g)
    ENDIF
    IF y<yspielfeld-1
      @dozug(x,y+1,g)
    ENDIF
  ELSE
    feld(x,y)=(feld(x,y) and -256) or ((feld(x,y) and 255)+1)
  ENDIF
RETURN
FUNCTION inh(ina,inb,inc)
  IF ina<0 OR inb<0 or ina>=xspielfeld OR inb>=yspielfeld
    RETURN 0
  ELSE
    IF (feld(ina,inb) and -256)/256=inc OR inc=-1
      RETURN feld(ina,inb) and 255
    ELSE
      RETURN 0
    ENDIF
  ENDIF
ENDFUNC
FUNCTION feld(ina,inb)
  IF ina<0 OR inb<0 or ina>=xspielfeld OR inb>=yspielfeld
    RETURN 0
  ELSE
    RETURN (feld(a,b) and 255) 
  ENDIF
ENDFUNC

PROCEDURE ende
  quit
RETURN

PROCEDURE spieler
  '
 deftext 1,0.07,0.15,0  
  color weiss
  pbox 320,130,520,300
  color schwarz
  box 320,130,520,300
  
  ltext 330,140,"Spielernamen eingeben:"
  ltext 330,160,"weiss:"
  ltext 330,180,"schwarz:"
  

 ledit_curlen=25
 repeat
   s$(1)=@ledit$(s$(1),400,160)
   if ledit_status=0 or ledit_status=2
     s$(2)=@ledit$(s$(2),400,180)
   endif
 until ledit_status=0
 ' input "Spieler 2 :",s$(2)
 
RETURN

procedure mbutton(button_x,button_y,button_t$,sel)
  local x,y,w,h
  defline ,1
  deftext 1,0.05,0.1,0
  button_l=ltextlen(button_t$)

  x=button_x-button_l/2-10
  y=button_y-10
  w=button_l+20
  h=20
  color grau
  pbox x+5,y+5,x+w+5,y+h+5
  color abs(sel=0)
  pbox x,y,x+w,y+h
  if sel
   color weiss
  else 
   color schwarz
  endif  
  box x,y,x+w,y+h
 
  ltext button_x-button_l/2,button_y-5,button_t$
return

function inbutton(button_x,button_y,button_t$,mx,my)
  local ax,ay,aw,ah,button_l
  
  deftext 1,0.05,0.1,0
  button_l=ltextlen(button_t$)

  ax=button_x-button_l/2-10
  ay=button_y-10
  aw=button_l+20
  ah=20
  if mx>=ax and my>=ay and  mx<=ax+aw and my<=ay+ah
    return true
  else
    return false
  endif
endfunc
procedure spielfeld
  color weiss
  pbox 0,0,640,400
  color lila
  deftext 1,0.2,0.2,0
  lTEXT 450-ltextlen("ATOMIUM")/2,0,"ATOMIUM"
  @show_f

  TEXT 262,25,"SPIELREGELN:"
  TEXT 262,35,"Die Spieler setzten nacheinander die Steine (Neutronen) in die"
  TEXT 262,45,"Felder (Kristallgitter). Mit vier Neutronen wird ein Atom instabil,"
  TEXT 262,55,"und es zerfällt, wobei seine Neutronen von den Nachbaratomen"
  TEXT 262,65,"absorbiert werden, wodurch diese zu eigenen Atomen werden."
  TEXT 262,75,"Gewonnen hat, wer alle Atome des Gegners zu eigenen gemacht hat."
 
  @mbutton(600,380,"QUIT",0)
  @mbutton(500,380,"Sound",0)
  @mbutton(400,380,"neues Spiel",0)
  @mbutton(300,380,"Zugvorschlag",0)
  @mbutton(200,380,"Farbwechsel",0)
  @mbutton(100,380,"Info",0)
  @hscalerbar(500,330,100)
  deftext 1,0.05,0.05,0
  ltext 10,266,"(c) MARKUS HOFFMANN 1992"
  vsync
return
procedure hscalerbar(scaler_x,scaler_y,scaler_w)
  local i,k

  color weiss
  pbox scaler_x,scaler_y,scaler_x+scaler_w,scaler_y+20
  color schwarz 
  box scaler_x,scaler_y,scaler_x+scaler_w,scaler_y+20
  for i=0 to 100 step 5
    if (i mod 50)=0
      k=7
      text scaler_x+i/100*scaler_w-len(str$(i))*2.5,scaler_y+37,str$(i)
    else if (i mod 10)=0
      k=5
    else
      k=3
    endif
    line scaler_x+i/100*scaler_w,scaler_y+20,scaler_x+i/100*scaler_w,scaler_y+20+k
  next i
return

procedure do_hscaler(scaler_x,scaler_y,scaler_w,wert)
  color weiss
  pbox scaler_x+1,scaler_y+1,scaler_x+scaler_w,scaler_y+20
  color gelb
  pbox scaler_x+1,scaler_y+1,scaler_x+(scaler_w-2)*wert+1,scaler_y+20
return

function ledit$(t$,x,y)
  local t2$,cc,curpos,a,b,c$,curlen
  
  t2$=t$

  cc=len(t$)
  curpos=x+ltextlen(left$(t$,cc))
  curlen=max(20,ledit_curlen)

do
  color weiss
  ltext x,y,t2$
  t2$=t$
  line curpos,y,curpos,y+curlen
  curpos=x+ltextlen(left$(t$,cc))
  color schwarz
  ltext x,y,t2$
  color rot
  line curpos,y,curpos,y+curlen
  vsync
  keyevent a,b,c$
  color weiss
  if b and -256
    print ".";
    b=b and 255
    if b=8    ! Bsp
      if len(t2$)
        t$=left$(t2$,cc-1)+right$(t2$,len(t2$)-cc)
        dec cc
      else
        bell
      endif
    else if b=13    ! Ret
      ledit_status=0
      line curpos,y,curpos,y+curlen
      return t2$
    else if b=82
      ledit_status=1
      line curpos,y,curpos,y+curlen
      return t2$
    else if b=84
      ledit_status=2 
      line curpos,y,curpos,y+curlen
      return t2$
    else if b=255   ! Del
      if cc<len(t2$)
        t$=left$(t2$,cc)+right$(t2$,len(t2$)-cc-1)
      endif
    else if b=81
      cc=max(0,cc-1)
    else if b=83
     cc=min(len(t2$),cc+1)
    endif
  else
    
    t$=left$(t2$,cc)+chr$(b)+right$(t2$,len(t2$)-cc)
    inc cc
  endif
  print a,"$"+hex$(b),b
  
loop
endfunction

procedure info
alert 1,"      A T O M I U M|| (c) Markus Hoffmann 1991| |Dieses Programm darf nur zusammen |mit dem Paket X11-BASIC weiter-|gegeben werden.",1," OK ",dummy
return

procedure led(x,y,c)
  color c
  pcircle x*10+550,100+y*10,4
  color schwarz
  circle x*10+550,100+y*10,4
return
