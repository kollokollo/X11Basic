' This little example shows you
' how to use different colos on the
' Text (terminal) output of X11-Basic
' You can have
' 8 special attributes,
' 8 foreground colors and
' 8 background colors
'                    by Markus Hoffmann
CLS
PRINT
PRINT " example how to use the ansi color spec."
PRINT
PRINT "X 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0"
FOR u=0 TO 2
  FOR j=0 TO 7
    FOR i=0 TO 7
      PRINT AT(j+6,2*i+2+16*u);CHR$(27)+"["+STR$(u)+";"+STR$(30+i)+";"+STR$(40+j)+"m *";
    NEXT i
  NEXT j
NEXT u
PRINT chr$(27);"[m"
' Special Attributes
PRINT at(15,1);CHR$(27);"[m";"0 normal"
PRINT at(16,1);CHR$(27);"[1m";"1 bold";CHR$(27);"[m"
PRINT at(17,1);CHR$(27);"[2m";"2 dim";CHR$(27);"[m"
PRINT at(18,1);CHR$(27);"[3m";"3 ?";CHR$(27);"[m"
PRINT at(19,1);CHR$(27);"[4m";"4 underline";CHR$(27);"[m"
PRINT at(20,1);CHR$(27);"[5m";"5 blink";CHR$(27);"[m"
PRINT at(21,1);CHR$(27);"[6m";"6 ?";CHR$(27);"[m"
PRINT at(22,1);CHR$(27);"[7m";"7 inverse";CHR$(27);"[m"
PRINT
' Text colors
PRINT at(15,20);CHR$(27);"[0;30;41m";"0 black";CHR$(27);"[m"
PRINT at(16,20);CHR$(27);"[0;31m";"1 red";CHR$(27);"[m"
PRINT at(17,20);CHR$(27);"[0;32m";"2 green";CHR$(27);"[m"
PRINT at(18,20);CHR$(27);"[0;33m";"3 yellow";CHR$(27);"[m"
PRINT at(19,20);CHR$(27);"[0;34m";"4 blue";CHR$(27);"[m"
PRINT at(20,20);CHR$(27);"[0;35m";"5 magenta";CHR$(27);"[m"
PRINT at(21,20);CHR$(27);"[0;36m";"6 cyan";CHR$(27);"[m"
PRINT at(22,20);CHR$(27);"[0;37m";"7 white";CHR$(27);"[m"
PRINT chr$(27);"[2;36;41m";"And of course all combinations."

PRINT chr$(27);"[m"
END

