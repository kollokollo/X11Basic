' Test der Shared_memory Funktinen             Aug 27  2001
' written in X11Basic by Markus Hoffmann
'
' Demonstrates the use of the shared memory routines. Works only on
' UNIX/linux
'
' Use it the following way:
' run the program without commandline arguments to start the 
' display
' run the program from another commandline with arguments (numbers) 
' to make the first program display them.
' This way two different programs can talk to each other or exchange
' chuncks of data

' allocate a buffer of 32 bytes using a "secret" key 12345
' If a buffer of the same size and same key was allocated by another
' program before, we get the same shmid and can acceess the same
' memory
'
shmid%=SHM_MALLOC(32,12345)
PRINT "SHMID:"'shmid%
adr=SHM_ATTACH(shmid%)    ! Attach the buffer so we can access it
PRINT "ADR:"'hex$(adr)

IF LEN(PARAM$(2))
  t$=MKD$(VAL(PARAM$(2)))
  BMOVE VARPTR(t$),adr,LEN(t$)

' shm_free shmid

ELSE
  PRINT "now let this instance of the progam run and start another"
  PRINT "instance with commandline arguments!"
  PRINT
  ' Make sure that there is not a 4711 in already
  t$=MKD$(815)
  BMOVE VARPTR(t$),adr,LEN(t$)

  weiss=COLOR_RGB(1,1,1)
  grau=COLOR_RGB(1/1.2,1/1.2,1/1.2)
  schwarz=COLOR_RGB(0,0,0)
  rot=COLOR_RGB(1,0,0)
  groesse=0.7
  offset=22
  faktor=1000

  SIZEW ,800*groesse+200,200*groesse+200
  SHOWPAGE
  COLOR schwarz
  PBOX 0,0,800*groesse+200,200*groesse+200

  t$=SPACE$(32)
  'if peek(adr)=200
  PRINT "waiting for shared memory content to change."
  DO
    BMOVE adr,VARPTR(t$),32
    aa=a
    a=CVD(t$)
    IF a<>aa
      BEEP
      @display(a)
    ENDIF
'endif
't$=chr$(200)+" Hallo, ich bins "+time$
'bmove varptr(t$),adr,len(t$)
'system "ipcs"
'pause 5
'shm_detach adr
'system "ipcs"
'pause 5
  exit if a=4711
  pause 1
loop

shm_free shmid
ENDIF
QUIT
'
' Display a value in very big letters
'
PROCEDURE display(wert)
  COLOR schwarz
  PBOX 20,92,800*groesse+200,200*groesse+200
  COLOR weiss
  DEFLINE ,25*groesse,2
  DEFTEXT 1,groesse,2*groesse
  LTEXT 500-LTEXTLEN(STR$(wert)),100,STR$(wert)
  SHOWPAGE
RETURN
