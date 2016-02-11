echo off
print "Dieses Programm rechnet die Quadratwurzel einer Zahl"
input "Zahl=",z
r124=1
105:
r123=r124
r124=(r123^2+z)/(2*r123)
if abs(r124-r123)-0.00001>0
	print r124
	goto 105
endif
print "Das Ergebnis des Algoritmus:"'r124
print "Zum Vergleich: sqr(";z;")=";sqr(z)
print "Abweichung:"'abs(sqr(z)-r124)
end
