PRINT "test of on x gosub ..."
PRINT "Input x=";
a=0
INPUT a

ON a+1 gosub hallo,ja(1.3),hier,auch
ON a+1 goto 1,2,3,4

QUIT
1:
PRINT "1"
2:
PRINT "2"
3:
PRINT "3"
4:
PRINT "4"
QUIT

PROCEDURE hallo
  PRINT "Hallo"
RETURN
PROCEDURE ja(n)
  PRINT "Ja "+STR$(n)
RETURN
PROCEDURE hier
  PRINT "hier"
RETURN
PROCEDURE auch
  PRINT "auch"
RETURN
