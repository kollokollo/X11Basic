
' How to use the on error and on break statements.
' TODO: This is not yet working with bytecode

EVERY 10,ddd
ON ERROR GOSUB e
ON BREAK GOSUB a
DO
  PRINT TIME$;CHR$(13);
  FLUSH
  IF RANDOM(10000)=100
    ERROR 22
    PRINT 1/0    ! This should cause an error
    PAUSE 1
  ENDIF
LOOP
QUIT
exiot:

PRINT "break and exit with ";ERR
QUIT

PROCEDURE a
  PRINT "no break !!!!   Try again."
  ON BREAK GOTO exiot
RETURN
PROCEDURE e
  PRINT "Error #";ERR;" occured."
  PRINT "continue ...."
  ON ERROR GOSUB e
RETURN
PROCEDURE ddd
  PRINT "timeout"
RETURN
