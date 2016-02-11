t$="libsimlib.so"
if not exist(t$)
 ' quit
endif
print t$
link #1,t$
exec sym_adr(#1,"SimLibStartUp")
flush

kd=-0.593
kf=0.632
exec sym_adr(#1,"SimPutQdd"),D:kd
exec sym_adr(#1,"SimPutQdf"),D:kf
exec sym_adr(#1,"SimPutParticleMomentum"),D:1.6e9


' Orbit-Datei einlesen und fit mit virtuellen Korrektoren
' Als Fehlerquellen. Dies simuliert den Orbit und man erhaelt auch
' Zwischenwerte

o$="/home/hoffmann/physik/depol/orbits/orbit_unkorrigiert.dat"

DIM bpmspos(100),bpmread(100),bpmerror(100)
clr nummon
DIM korrspos(100),result(100)
clr numkorr
arrayfill result(),4711

open "I",#2,o$
while not eof(#2)
  lineinput #2, t$
  t$=trim$(t$)
  wort_sep t$," ",1,a$,b$
  wort_sep b$," ",1,b$,c$
  bpmspos(nummon)=val(a$)
  bpmread(nummon)=val(b$)
  bpmerror(nummon)=0.000001
 
  inc nummon
wend
close #2

numkorr=20

rc=exec(sym_adr(#1,"SimLibLinear"))
if rc
for i=0 to 20
  korrspos(i)=i*5
next i

  rc2=exec(sym_adr(#1,"SimLibMICADO"),nummon,numkorr,numkorr,L:varptr(bpmspos(0)),L:varptr(korrspos(0)),L:varptr(bpmread(0)),L:varptr(bpmerror(0)),0,L:varptr(result(0)),D:1/1e4) 

for i=0 to 20
  print korrspos(i),result(i)
next i


endif
if rc
  clr e0,qx,qz,e1
  exec sym_adr(#1,"SimGetXTune"),L:varptr(qx)
  exec sym_adr(#1,"SimGetZTune"),L:varptr(qz)
  exec sym_adr(#1,"SimGetXNaturalEmittance"),L:varptr(e0)
  exec sym_adr(#1,"SimGetZNaturalEmittance"),L:varptr(e1)
  print "Qx=";qx,"Qz=";qz,"Emmix=";e0,"Emmiz=";e1

  a$=space$(2*(11*8+1+4))
  data$=space$(79)
  print "s [m]","Alphax","Betax"
  for g=0 to 50 step 0.1
 ' print orbit
    exec sym_adr(#1,"SimLibTwissParam"),L:2,D:g,L:varptr(a$)
'    for i=0 to 100
'      print hex$(peek(varptr(a$)+i) and 255,2,2);
'    next i
'    print
    alpha=cvd(a$)
    beta=cvd(mid$(a$,8+1,8))
    gamma=cvd(mid$(a$,2*8+1,8))
    my=cvd(mid$(a$,3*8+1,8))
    mysign=cvl(mid$(a$,4*8+1,4))
    disp=cvd(mid$(a$,5*8+1+4,8))
    ddisp=cvd(mid$(a$,6*8+1+4,8))
    orbit=cvd(mid$(a$,7*8+1+4,8))
    dorbit=cvd(mid$(a$,8*8+1+4,8))
    intdisp=cvd(mid$(a$,9*8+1+4,8))
    dintdisp=cvd(mid$(a$,10*8+1+4,8))
    
    poke varptr(data$)+3*beta,asc("*")
    poke varptr(data$)+orbit*3000+40,asc("%")
   ' print g,(g mod 1),(g mod 1)-1
   
    if (g mod 1)<0.0001 or abs((g mod 1)-1)<0.01
      print g,data$
      data$=space$(79)
    endif
  next g
endif
unlink #1
quit
