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
print chr$(27);"[m"
' Special Attributes
print at(15,1);chr$(27);"[m";"0 normal"
print at(16,1);chr$(27);"[1m";"1 bold";chr$(27);"[m"
print at(17,1);chr$(27);"[2m";"2 dim";chr$(27);"[m"
print at(18,1);chr$(27);"[3m";"3 ?";chr$(27);"[m"
print at(19,1);chr$(27);"[4m";"4 underline";chr$(27);"[m"
print at(20,1);chr$(27);"[5m";"5 blink";chr$(27);"[m"
print at(21,1);chr$(27);"[6m";"6 ?";chr$(27);"[m"
print at(22,1);chr$(27);"[7m";"7 inverse";chr$(27);"[m"
print
' Text colors
print at(15,20);chr$(27);"[0;30;41m";"0 black";chr$(27);"[m"
print at(16,20);chr$(27);"[0;31m";"1 red";chr$(27);"[m"
print at(17,20);chr$(27);"[0;32m";"2 green";chr$(27);"[m"
print at(18,20);chr$(27);"[0;33m";"3 yellow";chr$(27);"[m"
print at(19,20);chr$(27);"[0;34m";"4 blue";chr$(27);"[m"
print at(20,20);chr$(27);"[0;35m";"5 magenta";chr$(27);"[m"
print at(21,20);chr$(27);"[0;36m";"6 cyan";chr$(27);"[m"
print at(22,20);chr$(27);"[0;37m";"7 white";chr$(27);"[m"
print chr$(27);"[2;36;41m";"And of course all combinations."

print chr$(27);"[m"
end

