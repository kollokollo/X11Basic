' Utility zum convertieren einer UNIX-Bitmap in ein Binaerformat
' (c) Markus Hoffmann 2002

FILESELECT "load bitmap ...","./*.bmp","test.bmp",f$
IF len(f$)
  IF exist(f$)
    OPEN "I",#1,f$
    f$=""
    WHILE not eof(#1)
      LINEINPUT #1,t$
      t$=TRIM$(t$)
      PRINT t$
      IF left$(t$)="#"
      ELSE if LEFT$(t$)="0"
        WHILE len(t$)
          SPLIT t$,",",0,a$,t$
          IF right$(a$,2)="};"
            a$=LEFT$(a$,LEN(a$)-2)
          ENDIF
          PRINT a$
          IF len(a$)
            f$=f$+CHR$(VAL(a$))
          ENDIF
        WEND
      ENDIF
    WEND
    CLOSE
    PRINT len(f$)
    PRINT "Output is: bbbb.bin"
    BSAVE "bbbb.bin",VARPTR(f$),LEN(f$)
  ELSE
    ~form_alert(1,"[3][File not found !][ OH ]")
  ENDIF
ENDIF
QUIT
