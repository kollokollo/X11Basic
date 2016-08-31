'
' Sort String arrays with a diffent alphabet than ASCII or
' sort String arrays regardingless of the CASE of the charackters.
' (c) Markus Hoffmann 2014
'
'

DIM test$(100)

DO
  READ a$
  EXIT IF a$="*"
  test$(anzdata)=a$
  ? test$(anzdata)
  INC anzdata
LOOP

? anzdata;" Elements."

? "Sort starts"

@asort(test$(),anzdata)

FOR x=0 TO anzdata-1
  ? test$(x)
NEXT x

@usort(test$(),anzdata)

PRINT "Result of USORT:"
FOR x=0 TO anzdata-1
  ? test$(x)
NEXT x
END
'
' Sort with a custom alphabet
PROCEDURE asort(VAR s$(),anz%)
  LOCAL k%,alphabet$,j%,t$(),i%(),t2$()
  t$()=s$()
  alphabet$="0123456789AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz"
  DIM i%(anz%)

  FOR k%=0 TO anz%-1
    i%(k%)=k%
    FOR j%=0 TO LEN(t$(k%))-1
      POKE VARPTR(t$(k%))+j%,INSTR(alphabet$,MID$(t$(k%),j%+1))
    NEXT j%
  NEXT k%
  SORT t$(),anz%,i%()
  DIM t2$(DIM?(s$()))
  FOR k%=0 TO anz%-1
    t2$(k%)=s$(i%(k%))
  NEXT k%
  s$()=t2$()
RETURN
'
' Sort case insensitive
'
PROCEDURE usort(VAR s$(),anz%)
  LOCAL k%,t$(),i%(),t2$()
  t$()=s$()
  DIM i%(anz%)

  FOR k%=0 TO anz%-1
    i%(k%)=k%
    t$(k%)=UPPER$(t$(k%))
  NEXT k%
  SORT t$(),anz%,i%()
  DIM t2$(DIM?(s$()))
  FOR k%=0 TO anz%-1
    t2$(k%)=s$(i%(k%))
  NEXT k%
  s$()=t2$()
RETURN

DATA "aaa","aac","hot","Hotel","ada","aza","aya","aba","aka","ala","CVB","Hello"
DATA "Help","helau","a","*"

