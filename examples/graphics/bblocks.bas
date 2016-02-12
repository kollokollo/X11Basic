' sinecube 2006 mennonite 
' public domain
' 2015-08-20 begin Smb translation
' converted to X11-Basic 2015
'
DIM colors(20)
RANDOMIZE
colors(0)=COLOR_RGB(0,0,0)
FOR i%=1 TO 19
  colors(i%)=COLOR_RGB(RND(),RND(),RND())
NEXT i%
COLOR colors(1)
TEXT 320,50,"sinecube 2006 mennonite in X11-Basic"
B$="00000000..."
B$=B$+"llnnnnnnl.."
B$=B$+"l8lnnnnnnl."
B$=B$+"l88llllllll"
B$=B$+"l88l000000l"
B$=B$+"l88l000000l"
B$=B$+"l88l000000l"
B$=B$+"l88l000000l"
B$=B$+".l8l000000l"
B$=B$+"..ll000000l"
B$=B$+"...llllllll"
FOR l=8*32 TO 1 STEP -8
  FOR y%=4 TO 4*32 STEP 4
    h2%=y%+l+4
    FOR x=8*32 TO 1 STEP -8
      IF 1=SGN(SIN(x*y%*l*3.14))           ! not PI !
        h1%=x+y%-3
        FOR by%=0 TO 10
          FOR bx%=0 TO 10
	    a%=ASC(MID$(b$,by%*11+bx%+1))
            IF a%<>ASC(".")
              COLOR colors(a% MOD 16+(y% MOD 2))
              PLOT bx%+h1%,by%+h2%
            ENDIF
          NEXT bx%
        NEXT by%
      ENDIF
    NEXT x
    SHOWPAGE
  NEXT y%
NEXT l
COLOR colors(2)
TEXT 400,380,"press any key..."
SHOWPAGE
KEYEVENT
END
