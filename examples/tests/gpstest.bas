' A short demonstration of the GPS functionallity
' X11-Basic for Android
'

PRINT "initial (empty) values: "
PRINT GPS_LAT,GPS_LON,GPS_ALT
PAUSE 1

' Ask for last known location. This maybe an old value. Check the time!
GET_LOCATION lat,lon,alt,bearing,accuracy,s,t,p$
PRINT "last known location:"
PRINT lat,lon,alt,bearing,accuracy
PRINT "speed=";s
PRINT "time=";t
PRINT "provider=";p$

if timer-t>300
  print "last known location values are older that 5 Minutes. ";(timer-t)/60
endif

PRINT
PRINT "now get updates..."

' get location updates in the background
GPS ON
DO
  PRINT GPS_LAT,GPS_LON,GPS_ALT
  PAUSE 1
LOOP
END
