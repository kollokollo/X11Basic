t$="Hallo, das ist der String..."

i=varptr(t$)-2000
DO
  PRINT "$";HEX$(i,8,8)'
  FOR iu=0 TO 15
    PRINT HEX$(PEEK(i+iu) and 255,2,2)'
  NEXT iu
  PRINT '
  FOR iu=0 TO 15
    a=PEEK(i+iu)
    if a>31
      print chr$(a);
    else 
      print ".";
    endif
  NEXT iu
  PRINT
  ADD i,16
LOOP
quit