' Test der Shared_memory Funktinen
shmid=shm_malloc(32,12345)
print "SHMID:"'shmid
adr=shm_attach(shmid)
print "ADR:"'hex$(adr)

if len(param$(2))
  t$=mkd$(val(param$(2)))
  bmove varptr(t$),adr,len(t$)

' shm_free shmid

else
  weiss=get_color(65535,65535,65535)
  grau=get_color(65535/1.2,65535/1.2,65535/1.2)
  schwarz=get_color(0,0,0)
  rot=get_color(65535,0,0)
  groesse=0.7
  offset=22
  faktor=1000

SIZEW ,800*groesse+200,200*groesse+200
vsync
COLOR schwarz
PBOX 0,0,800*groesse+200,200*groesse+200

t$=space$(32)
'if peek(adr)=200
do
bmove adr,varptr(t$),32
aa=a
a=cvd(t$)
if a<>aa
  @display(a)
endif
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
endif
quit
procedure display(wert)
  COLOR schwarz
  PBOX 20,92,800*groesse+200,200*groesse+200
  COLOR weiss
  DEFLINE ,25*groesse,2
  DEFTEXT 1,groesse,2*groesse
  LTEXT 500-LTEXTLEN(STR$(wert)),100,STR$(wert)
  VSYNC
return
