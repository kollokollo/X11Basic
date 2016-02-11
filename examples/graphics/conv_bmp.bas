' Utility zum convertieren einer UNIX-Bitmap in ein Binaerformat
' (c) Markus Hoffmann 2002

FILESELECT "load bitmap ...","./*.bmp","test.bmp",f$
if len(f$)
  if exist(f$)
    open "I",#1,f$
    f$=""
    while not eof(#1)
      lineinput #1,t$
      t$=trim$(t$)
      print t$
      if left$(t$)="#"
      else if left$(t$)="0"  
        while len(t$)
          SPLIT t$,",",0,a$,t$
          if right$(a$,2)="};"
            a$=left$(a$,len(a$)-2)
          endif
          print a$
          if len(a$)
            f$=f$+chr$(val(a$))
          ENDIF
        WEND
      ENDIF
    WEND
    CLOSE
    PRINT len(f$)
    print "Output is: bbbb.bin"
    bsave "bbbb.bin",varptr(f$),len(f$)
  else 
    ~form_alert(1,"[3][File not found !][ OH ]")
  ENDIF
ENDIF
QUIT
