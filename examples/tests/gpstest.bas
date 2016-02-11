' A short demonstration of the GPS functionallity in 
' X11-Basic for Android (c) Markus Hoffmann 08/2013
'

' Check is the operating system supports GPS (location service)
cls
if GPS?=0
  print "This computer has no GPS support."
  print "You should run this example on a device with location service (GPS)"
  print "maybe an Android phone or tablet."
endif


' Before you used GPS ON or GET_LOCATION the system variables 
' GPS_LAT,GPS_LON,GPS_ALT will have empty values
' GPS_LAT and GPS_LON usually have -1
'

PRINT "initial (empty) values: "
PRINT GPS_LAT,GPS_LON,GPS_ALT
PAUSE 1

' Now you can ask for a location, which already has been measured before.
' This is either a location the device has accomplished by the network or
' cell phone triangulation or -- if the GPS was allowed by the user --
' the last measured position by GPS device. 
'
' It can happen, that you get a valid location when GPS was disabled by the user
' but you wont, when GPS is enabled. This happens when no GPS fix could be
' acheived so far.


' Ask for last known location. This maybe an old value. Check the time!
GET_LOCATION lat,lon,alt,bearing,accuracy,s,t,p$
PRINT "last known location:"
PRINT lat,lon,alt,bearing,accuracy
PRINT "speed=";s
PRINT "time=";t
PRINT "provider=";p$

if timer-t>300
  print "last known location values are older than 5 Minutes. ";int((timer-t)/60); "Min."
endif
pause 5
cls

' Usually you want to get more recent positions and therefor you need to switch 
' on the location measurement in the device. 
' GPS ON turns on the location updates either by network provider (if GPS was
' disabled by the user or uis the device has no GPS chip) or by GPS device 
' (as soon as it gets a satellite fix). In the latter case also the GPS icon
' will appear in the status line (press the home button to see it).
'

PRINT
PRINT "now get updates..."

' get location updates in the background
GPS ON
DO
  PRINT AT(1,1);"Position by ";p$;"  ";
  PRINT AT(2,1);"Latitude:   ";@breite$(GPS_LAT);"  ";
  PRINT AT(3,1);"Longitude: ";@laenge$(GPS_LON);"  ";  
  PRINT AT(4,1);"Altitude:  ";GPS_ALT;" m   "
  ' you can of course also use GET_LOCATION here to get more
  ' information
  GET_LOCATION lat,lon,alt,bearing,accuracy,s,t,p$
  PRINT "acuracy: ";accuracy;" m "
  PRINT "speed: ";s;" m/s "
  PRINT "baering: ";bearing;" deg"
  PRINT "timestamp: ";t
  ' The timestamp is useful to find out if the measurement is really new
  PRINT "time of measurement:";UNIXDATE$(t);" ";UNIXTIME$(t)
  PAUSE 0.5  
LOOP
' switch off gps position measurement (and save battery)
GPS OFF
END

' Here some useful functions for handling GPS data:



FUNCTION breite$(x)
  LOCAL posx$
  IF x>0
    posx$="N"
  ELSE
    posx$="S"
  ENDIF
  x=ABS(x)
  posx$=posx$+RIGHT$("00"+str$(INT(x)),2)+":"
  x=x-INT(x)
  x=x*60
  posx$=posx$+RIGHT$("00"+str$(INT(x)),2)+":"
  x=x-INT(x)
  x=x*60
  posx$=posx$+RIGHT$("00"+str$(INT(x)),2)+"."
  x=x-INT(x)
  x=x*10
  posx$=posx$+str$(INT(x))
  RETURN posx$
ENDFUNC
FUNCTION laenge$(x)
  LOCAL posx$
  IF x>0
    posx$="E"
  ELSE
    posx$="W"
  ENDIF
  x=ABS(x)
  posx$=posx$+RIGHT$("000"+str$(INT(x)),3)+":"
  x=x-INT(x)
  x=x*60
  posx$=posx$+RIGHT$("00"+str$(INT(x)),2)+":"
  x=x-INT(x)
  x=x*60
  posx$=posx$+RIGHT$("00"+str$(INT(x)),2)+"."
  x=x-INT(x)
  x=x*10
  posx$=posx$+str$(INT(x))
  RETURN posx$
ENDFUNC
' calculate the distance in meters between two locations a nd b 
' given by lat,lon each.
function distance(lona,lata,lonb,latb)
  local DEG2RAD,l0,l1,b0,b1
  DEG2RAD=(3.14159265358979323846/180.0)
  l0=lona*DEG2RAD
  l1=lonb*DEG2RAD
  b0=lata*DEG2RAD
  b1=latb*DEG2RAD
  return 6371*2*asin(sqrt(cos(b1)*cos(b0)*sin(0.5*(l1-l0))*sin(0.5*(l1-l0))+sin(0.5*(b1-b0))*sin(0.5*(b1 - b0))))
endfunc

function qthlocator$(breite,laenge)
  t$=chr$(int(laenge/20)+asc("J"))
  t$=t$+chr$(asc("J")+int(breite/10))
  laenge=laenge-int(laenge/20)*20
  laenge=laenge/2
  t$=t$+str$(int(laenge))
  breite=breite-int(breite/10)*10
  t$=t$+str$(int(breite))
  breite=breite-int(breite)
  laenge=laenge-int(laenge)
  t$=t$+chr$(asc("A")+24*laenge)
  t$=t$+chr$(asc("A")+24*breite)
  return t$
endfunc

