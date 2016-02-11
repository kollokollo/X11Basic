' Test the Sensors on Android devices:
' X11-Basic (c) Markus Hoffmann
'
'
PRINT "SENSOR?=";SENSOR?

SENSOR ON   ! Switch Sensor phallanx on
cls
PRINT "Get some data..."
PAUSE 2
for i=0 to 10
PRINT "Temperature: ";SENSOR(0);" °C"
PRINT "Pressure:    ";SENSOR(1);" hPa"
PRINT "Light:       ";SENSOR(2);" lux"
PRINT "Proximity:   ";SENSOR(3);" cm"
PRINT "Orientation: (";SENSOR(4);",";SENSOR(5);",";SENSOR(6);") deg"
PRINT "Gyro:        (";SENSOR(7);",";SENSOR(8);",";SENSOR(9);") rad/s"
PRINT "Mag. field:  (";SENSOR(10);",";SENSOR(11);",";SENSOR(12);") uT"
PRINT "Accel.:      (";SENSOR(13);",";SENSOR(14);",";SENSOR(15);") m/s2"
PRINT "Temperature: ";SENSOR(16);" °C"
PRINT "Humidity:    ";SENSOR(17);" %"
PRINT "Rotation:    (";SENSOR(18);",";SENSOR(19);",";SENSOR(20);")"
PRINT "Gravity:     (";SENSOR(21);",";SENSOR(22);",";SENSOR(23);") m/s2"
PRINT "Lin.Acc.:    (";SENSOR(24);",";SENSOR(25);",";SENSOR(26);") m/s2"
PRINT "Motion:      (";SENSOR(27);",";SENSOR(28);",";SENSOR(29);")"
PRINT "Game rot.:   (";SENSOR(30);",";SENSOR(31);",";SENSOR(32);")"
PRINT "Mag. uncal.: (";SENSOR(33);",";SENSOR(34);",";SENSOR(35);") uT"
PRINT "Gyr. uncal.: (";SENSOR(36);",";SENSOR(37);",";SENSOR(38);")"
PRINT "Step count:  ";SENSOR(39);" "
PRINT "Step detect: ";SENSOR(40);" "
if i<10
  PAUSE 2
  CLS
endif
next i

SENSOR OFF
END
