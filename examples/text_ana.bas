echo off
alert 3,"Dieses Programm laeuft noch |nicht richtig !||trotzdem fortfahren ?",2,"Ja|Nein",a
if a=2
  quit
endif
DIM c(255),b(255)
DO
  clearw
  zab&=18
  y&=80
  x&=0
  ARRAYFILL b(),0
  ARRAYFILL c(),0
  PRINT CHR$(27);"[7m             Textanalysator   (c) Markus Hoffmann         V.1.00                ";CHR$(27);"[m"
  print  
  print "Haetten Sie gewusst, dass Ihr ATARI ST ihre Briefe und Texte lesen und verstehen kann ?"
  print "Nein? Dann lassen Sie sich ueberzeugen..."
  print "Sie muessen zuerst dem Programm ein ASCII-Textfile zu lesen geben. Dieser File sollte"
  print "mindestens 3 KBytes lang sein und maximal 32 Kbytes."
  print "Nach dem Lesen, wird die statistische Buchstabenhaeufigkeit ermittelt und grafisch angezeigt."
  print "Nach einem Tastendruck legt der Rechner mit der Inhaltsangabe los."
  print "Diese koennen Sie zusaetzlich auf dem Drucker ausgeben lassen."
  print "Durch den Intelligenzquozienten bestimmen Sie, wie verstaendlich das Resultat"
  print "sein soll. Hier empfiehlt sich aber eine moeglichst kleine Zahl (z.b. 5), damit"
  print "das Resultat nicht zusehr dem Orginal aehnelt. Prinzipiell gilt:"
  print "Je groesser der Textfile, desto hoeher darf der Intelligenzquotient sein."
  print "Optimal sind zum Beispiel 35000 Bytes Text und IQ=8"
  print "Viel Spass garantiert                                             Markus Hoffmann"
  text 0,20,"Text laden:"
  FOR i=ASC("A") TO ASC("Z")
    text (i-ASC("A"))*16,380,CHR$(i)+" "
  NEXT i
   text (i-ASC("A"))*16,380,"ž Ž ™ š  9"
  FOR i=0 TO 100 STEP 10
    LINE 0,383-i*383/100,639,383-i*383/100
  NEXT i
  FILESELECT ,"./*.txt","",f$
  IF NOT EXIST(f$)
    quit
  ENDIF
  text 100,20,f$
  '
  OPEN "I",#1,f$
  l=LOF(#1)
  a=MALLOC(l)
  IF a<=0
    quit
  ENDIF
  BGET #1,a,l
  CLOSE
  '

  FOR i=0 TO l-1
    u=peek(a+i)
    INC c(u)
    IF (i MOD 1000)=0
      PRINT AT(1,4);"Lese: ";i
    ENDIF
  NEXT i
  PRINT AT(1,4);"Lese: ";i-1
nn:
  v=l-c(32)-c(10)-c(13)
  FOR i=ASC("A") TO ASC("Z")
    PBOX (i-65)*16+1,383,(i-65)*16+8,383-((c(i)+c(i+ASC("a")-ASC("A")))/v)*383*4
  NEXT i
  PBOX 26*16+1,383,26*16+8,383-(c(ASC("ž"))/v)*383*4
  PBOX 27*16+1,383,27*16+8,383-((c(ASC("Ž"))+c(ASC("„")))/v)*383*4
  PBOX 28*16+1,383,28*16+8,383-((c(ASC("™"))+c(ASC("”")))/v)*383*4
  PBOX 29*16+1,383,29*16+8,383-((c(ASC("š"))+c(ASC("")))/v)*383*4
  CLR z
  FOR i=ASC("0") TO ASC("9")
    ADD z,c(i)
  NEXT i
  PBOX 30*16+9,383,30*16+8+8,383-(z/v)*383*4
  vsync
  ~INP(-2)
  PRINT "Interpretation:"
  PRINT " Einen Moment bitte...";CHR$(13)'
  b$=SPACE$(32500)
 ' anz=C:ana(L:a,L:V:b$,L:l,L:LEN(b$))
  PRINT "ANZ:"'anz
  b$=LEFT$(b$,anz)
  PRINT "Len b$:"'LEN(b$)
  '
  PRINT "Bitte Intelligenzquotienten eingeben: (2-10)"'
  INPUT "",o$
  o=MAX(VAL(o$),2)
  '
  PRINT
  PRINT "Zufallstext Typ: 'AFFE' ";o;". Ordnung..."
  PRINT "-----------------------------------------"
  as:
  ALERT 2,"Ausgabe auf ...                ",3,"Drucker|Datei|Screen",v
  IF v=1
    OPEN "O",#1,"/dev/lpt0"
  ELSE IF v=2
    FILESELECT "Ausgabe:","\*.*","",s$
    IF LEN(s$)=0
      GOTO as
    ENDIF
    OPEN "O",#1,s$
  ENDIF
  IF v=1
    IF OUT?(0)=0
      PRINT "Drucker nicht an, abgebrochen ..."
      v=3
    ENDIF
  ENDIF
  IF v<>3
    PRINT #1,"Zufallstext Typ: 'AFFE' ";o;". Ordnung..."
    PRINT #1,"nach"'f$
    PRINT #1,"-------------------------------------------"
    PRINT #1
  ENDIF
  CLR pos
  g$=" "
  DO
    ARRAYFILL b(),0
    CLR cc
    PRINT "CC="'cc,"POS="'pos
    PRINT "LEN(b$):"'LEN(b$)
    PRINT "LEN(g$):"'LEN(g$)
    DO
      pos=INSTR(b$,g$,pos+1)
      EXIT IF pos=0
      INC b(ASC(MID$(b$,pos+LEN(g$),1)))
      INC cc
      PRINT "ADD cc"
    LOOP
    PRINT "CC:"'cc
    r&=@zufall
    IF r&=0
      r&=ASC("0")
    ENDIF
    @outs(antikro,r&)
    IF v<>3
      OUT #1,r&
    ENDIF
    g$=g$+CHR$(r&)
    PRINT "Hier vor Rightsting"
    g$=RIGHT$(g$,o)
    EXIT IF LEN(INKEY$)
  LOOP
  CLOSE
  ALERT 2,"Nochmal denselben Text|verwenden ?",1,"Ja|Nein",vb
  IF vb=1
    GOTO nn
  ENDIF
  '
    PRINT AT(1,3);"Reihenfolge:"''
    CLR g$
    c=0
    b(c)=0
    FOR i=ASC("A") TO ASC("Z")+1
      FOR j=ASC("A") TO ASC("Z")
        IF b(j)>b(c)
          c=j
        ENDIF
      NEXT j
      print chr$(c)+",";
      g$=g$+CHR$(c)
      b(c)=0
    NEXT i
    PRINT
    IF LEFT$(g$,5)="EN"
      PRINT ">>DEUTSCH:"''
    ELSE IF LEFT$(g$,5)="ER"
      PRINT ">>LISTE:"''
    ELSE IF LEFT$(g$,5)="ET"
      PRINT ">>ENGLISCH:"''
    ENDIF
    IF LEFT$(g$,5)="ENIRT"
      PRINT "Computer-Anleitung, Wissensch. Aufsatz"
    ELSE IF LEFT$(g$,5)="ERTAN"
      PRINT ">>Adressenliste Deutschland"
    ELSE IF LEFT$(g$,5)="ENIRS"
      PRINT ">>DEUTSCH Wissensch. Abhandlung"
    ELSE IF LEFT$(g$,5)="ENITR"
      PRINT ">>Deutsch Aufsatz"
    ELSE IF LEFT$(g$,5)="ETOAN"
      PRINT ">>ENGLISCH"
    ELSE IF LEFT$(g$,5)="ETONS"
      PRINT ">>Basic-Listing (englisch)"
    ELSE IF LEFT$(g$,5)="ERNIA"
      PRINT ">>(GFA)-Basic-Listing (englisch)"
    ELSE IF LEFT$(g$,5)="NIEDA"
      PRINT ">>(GFA)-Basic-Unterroutine (englisch)"
    ELSE IF LEFT$(g$,5)="ENISR"
      PRINT ">>Deutscher Lehrbuchtext"
    ELSE IF LEFT$(g$,5)="ENRIS"
      PRINT ">>Deutscher Brief/Aufsatz"
    ELSE IF LEFT$(g$,5)="ENRDS"
      PRINT ">>S„chsisch  Geschaeftsbrief/Aufsatz"
    ELSE
      PRINT ">>???"
    ENDIF
    ~INP(-2)
  ~MFREE(a)
LOOP
FUNCTION zufall
  IF cc=0
    ~FORM_ALERT(1,"[3][Der Text ist auf das Ende gelaufen!  CC=0][Oh!]")
    CLR g$
    RETURN 32
  ENDIF
  z:
  zz=RANDOM(cc+1)
  IF zz=0
    GOTO z
  ENDIF
  CLR ii
  DO
    SUB zz,b(ii)
    IF zz<=0
      RETURN ii
    ENDIF
    INC ii
  LOOP
ENDFUNC

PROCEDURE outs(fontadr,asc)
  print chr$(asc);
RETURN
