' Hier wird gezeigt, wie man externe Programmroutinen verwenden kann
' In diesem Fall eine Routine aus der bereits installierten libreadline


t$="/usr/lib/i386-linux-gnu/libreadline.so"
PRINT t$
IF EXIST(t$)
  LINK #1,t$
  DUMP "#"
  promt$=">>>"
  adr%=CALL(SYM_ADR(#1,"readline"),L:VARPTR(promt$))
  r=adr%
  WHILE PEEK(r)>0
    PRINT CHR$(PEEK(r));
    INC r
  WEND 
  PRINT
  UNLINK #1
  FREE adr%
ENDIF
QUIT
