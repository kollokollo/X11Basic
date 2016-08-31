' Taken from stbasic09 (to test compatibility ...)

' Test-program "alert.LIS" for stbasic
PRINT "Test ALERT-Box with different texts"
TITLEW 1,"ALERT-BOX-TEST"
CLEARW 1
DEFFILL 2,3,3
PRBOX 40,140,600,190
COLOR get_color(65535,0,0)
BOX 50,200,590,250
DEFFILL 7,4,4
PELLIPSE 320,200,100,150
ALERT 3,"ALERT-TEST 1|text with 4 lines|and 3 buttons|select 2",2,"But1|But2|But3",a%
PRINT "ALERT-BOX 1  return value =";a%
COLOR get_color(0,65535,0)
DEFFILL 5,5,5
PBOX 60,260,580,320
COLOR 5
DEFLINE 0,5
CIRCLE 320,200,190
ALERT 2,"ALERT-Test 2",1,"but1|but2",a%
PRINT "ALERT-BOX 2  return value =";a%
QUIT
