'
' Example program to demonstrate the Usage together with MQTT
'
' (c) Markus Hoffmann 2019 
' 
'
BROKER "tcp://localhost:1883"   ! This will use the local broker, e.q. mosquitto

' Subscribe to some topics
SUBSCRIBE "DEWPOINT",,callback   ! no global variable given here
SUBSCRIBE "CMD",cmd_var$,cmd_callback


' Now realize a so-called rule-engine
' The rule engine should listen for changes of
' Temperature and Humidity and then calculate the dew point and
' publish it. Note: values are stored in global variables specified here. 

SUBSCRIBE "TEMPERATURE",temp_var$,engine
SUBSCRIBE "HUMIDITY",hum_var$,engine

' If you need contents of other topics but you have no need to trigger
' on a change, you can just omit the callback routine

SUBSCRIBE "SOMETHING",something_var$


' It is very simple to publish something:

PUBLISH "TIME",time$,1,1
PUBLISH "STATUS","OK"

' You can now enter an endless loop and so just react to the callbacks. 
DO
  PUBLISH "COUNTER",str$(i)
  INC i
  PAUSE 0.1
  EXIT IF cmd_var$="exit"
  PRINT i
  if (i mod 10)=0
    PUBLISH "ACTIVITY",str$(INT(i/10) MOD 4)
    PUBLISH "TIME",time$
  endif
LOOP
QUIT

PROCEDURE callback(topic$,message$)
  PRINT "callback trigered on ";topic$;": message=";message$
RETURN
PROCEDURE cmd_callback(t$,message$)
  PRINT "cmd_callback trigered: cmd_var=";cmd_var$
RETURN

' This realizes a "rule". The input topics will trigger this. 
PROCEDURE engine(topic$,message$)
  LOCAL temp,hum
  PRINT "engine triggered on ";topic$
  temp=VAL(temp_var$)
  hum=VAL(hum_var$)
  PUBLISH "DEWPOINT",STR$(temp*hum)
RETURN
