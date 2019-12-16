'
' Example program to demonstrate the Usage together with MQTT
'
' (c) Markus Hoffmann 2019 
' 
'
BROKER "tcp://localhost:1883"   ! This will use the local broker, e.q. mosquitto

' Subscribe to some topics
SUBSCRIBE "DEWPOINT",result_var$,callback
SUBSCRIBE "CMD",cmd_var$,cmd_callback


' Now realize a so-called rule-engine
' The rule engine should listen for changes of
' Temperature and Humidity and then calculate the dew point and
' publish it

SUBSCRIBE "TEMPERATURE",temp_var$,engine
SUBSCRIBE "HUMIDITY",hum_var$,engine

' If you need contents of other topics but you have no need to trigger
' on a change, you can just omit the callback routine

SUBSCRIBE "SOMETHING",something_var$


' It is very simple to publish something:

PUBLISH "TIME",time$,1,1

' You can now enter an endless loop and so just react to the callbacks. 
DO
  PUBLISH "ACTIVITY",str$(i MOD 4)
  INC i
  PAUSE 1
  EXIT IF cmd_var$="exit"
LOOP
QUIT

PROCEDURE callback
  PRINT "callback trigered: ";
  print "result_var=";result_var$
RETURN
PROCEDURE cmd_callback
  PRINT "cmd_callback trigered: cmd_var=";cmd_var$
RETURN

' This realizes a "rule". The input topics will trigger this. 
PROCEDURE engine
  LOCAL temp,hum
  PRINT "engine triggered"
  temp=VAL(temp_var$)
  hum=VAL(hum_var$)
  PUBLISH "DEWPOINT",STR$(temp*hum)
RETURN
