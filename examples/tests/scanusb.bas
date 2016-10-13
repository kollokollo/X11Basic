' Test of the USB functions of X11-Basic Version >= 1.25
'
'
' Scan for USB devies around:
'
'
PRINT "Following USB devices could be found nearby:"
a$=FSFIRST$("","*","u")
while len(a$)
  print a$
  @decode(a$)
  a$=FSNEXT$()
wend
quit
procedure decode(a$)
  print " USB bus/device:           ";word$(a$,1)
  print " USB vendor-ID/product-ID: ";word$(a$,2)
  print " Manufacturer:             ";word$(a$,3)
  print " Product:                  ";word$(a$,4)
  print " Serial-Nr:                ";word$(a$,5)
  print " Number of configurations: ";word$(a$,6)
return
