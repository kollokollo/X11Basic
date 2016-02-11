echo off

PRINT @zahl$(101001)
rootwindow
color 0
pbox 0,0,640,400
color 1
text 100,100,"Klick mich !"
vsync
DO
  REPEAT
  UNTIL MOUSEK
  REPEAT
    u=MOUSEX*MOUSEY
    PRINT u,@zahl$(u)
  UNTIL MOUSEK=0
LOOP
' ################################################
FUNCTION zahl$(z)
  IF z=0
    RETURN ""
  ELSE
    IF z>1999
      RETURN @zahl$(z DIV 1000)+"tausend"+@zahl$(z MOD 1000)
    ELSE IF z>999
      RETURN "eintausend"+@zahl$(z MOD 1000)
    ELSE IF z>99
      RETURN @einser$(z DIV 100)+"hundert"+@zehner$(z MOD 100)
    ELSE
      RETURN @zehner$(z MOD 100)
    ENDIF
  ENDIF
  RETURN ""
ENDFUNC
FUNCTION zehner$(e)
  if e=0
  else if e=1
    RETURN "eins"
  else if e>=0 and e<=9
    RETURN @einser$(e)
  else if e=11
    RETURN "elf"
  else if e=12
    RETURN "zwölf"
  else if e=16
    RETURN "sechzehn"
  else if e=17
    RETURN "siebzehn"
  else if e=10 or (e>=13 and e<=19)
    RETURN @einser$(e-10)+"zehn"
  else if e=20
    RETURN "zwanzig"
  else if e>=21 and e<=29
    RETURN @einser$(e-20)+"undzwanzig"
  else if e=30
    RETURN "dreißig"
  else if e>=31 and e<=39
    RETURN @einser$(e-30)+"unddreißig"
  else if e=40 or e=50 or e=80 or e=90
    RETURN @einser$(e DIV 10)+"zig"
  else if (e>=41 and e<=49) or (e>=51 and e<=59) or (e>=81 and e<=89) or (e>=91 and e<=99)
    RETURN @einser$(e MOD 10)+"und"+@einser$(e DIV 10)+"zig"
  else if e=60
    RETURN "sechzig"
  else if e>=61 and e<=69
    RETURN @einser$(e-60)+"undsechzig"
  else if e=70
    RETURN "siebzig"
  else if e>=71 and e<=79
    RETURN @einser$(e-70)+"undsiebzig"
  endif
  RETURN ""
ENDFUNC
FUNCTION einser$(e)
  if e=0
    RETURN ""
  else if e=1
    RETURN "ein"
  else if e=2
    RETURN "zwei"
  else if e=3
    RETURN "drei"
  else if e=4
    RETURN "vier"
  else if e=5
    RETURN "fünf"
  else if e=6
    RETURN "sechs"
  else if e=7
    RETURN "sieben"
  else if e=8
    RETURN "acht"
  else if e=9
    RETURN "neun"
  ENDSELECT
  RETURN ""
ENDFUNC
