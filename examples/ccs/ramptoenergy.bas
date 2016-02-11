' Rampt auf eine vorgegebene Energie, in der Hoffnung,
' dass der Strahl mitgenommen wird
'
print "Ramptoenergy brute force"
print

e1=csget("ELS_MAGNETE_DIPOL.ENERGIE_AC")
print "Aktuelle Energie ist ";e1;" GeV."
print "Wohin soll die Reise gehen ? (GeV) ";
input e2

print "HF-Leistung muss stimmen !"
print "Korrektoren- muessen stimmen ! fuer grosse Rampen."


' c=csstep("ELS_MAGNETE_DIPOL.ENERGIE_AC")
if e2>e1
  c=0.001
else
  c=-0.001
endif
print "Step ";c*1000;" MeV"

clr count


for i=e1 to e2 step c
  csput "ELS_MAGNETE_DIPOL.ENERGIE_AC",i
  pause 0.2
  inc count
  if (count mod 10)=0
    print "E=";i;" GeV"
  endif
next i
csput "ELS_MAGNETE_DIPOL.ENERGIE_AC",e2
print "fertig."
quit
