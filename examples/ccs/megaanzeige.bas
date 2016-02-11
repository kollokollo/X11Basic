' Programm zeigt die Pulsladung im transferkanal an
' Letzte Bearbeitung: Markus Hoffmann    2.99
' umgestellt auf callback

ECHO OFF

groesse=0.7
offset=22
faktor=1000

clr funktion
clr maximum
clr minimum
weiss=get_color(65535,65535,65535)
grau=get_color(65535/1.2,65535/1.2,65535/1.2)
schwarz=get_color(0,0,0)
rot=get_color(65535,0,0)

SIZEW ,800*groesse+200,200*groesse+200
a=CSGET("STE_DIAG_TOROID.TRANSQ_AM")
PAUSE 1
COLOR schwarz
PBOX 0,0,800*groesse+200,200*groesse+200
@button(100,20,"QUIT",0) 
@button(200,20,"Zero",0) 
@button(300,20,"Funktion",0) 
CSSETCALLBACK "STE_DIAG_TOROID.TRANSQ_AM",interrupt
PAUSE 1
DO 
  MOUSEEVENT x,y,k
  IF k
    IF @inbutton(100,20,"QUIT",x,y)
     @button(100,20,"QUIT",1)
     VSYNC
     QUIT
    ELSE IF @inbutton(200,20,"Zero",x,y)
	  csclearcallbacks
	  @button(200,20,"Zero",1)
	  defmouse 2
	  VSYNC 
	  offset=1000*CSGET("STE_DIAG_TOROID.TRANSQ_AM")
	  clr maximum
	  clr minimum
	  @button(200,20,"Zero",0) 
	  defmouse 0
	  @display(0)
	  CSSETCALLBACK "STE_DIAG_TOROID.TRANSQ_AM",interrupt
    ELSE IF @inbutton(300,20,"Funktion",x,y)
	  csclearcallbacks
	  @button(300,20,"Funktion",1)
	  defmouse 2
	  VSYNC 
	  inc funktion
	  if funktion>=4
	    funktion=0
	  endif
	  @button(300,20,"Funktion",0) 
	  defmouse 0
	  VSYNC
	  
      if funktion=1
        COLOR schwarz
        PBOX 20,40,800*groesse+200,200*groesse+200
        DEFLINE ,15*groesse,2
	    color rot
	    DEFTEXT 1,groesse/2,groesse/2
        LTEXT 350,50,"OFFSET"
    
        a=offset
        a=ROUND(a*1)/1
        @display(a)
      else if funktion=2
        COLOR schwarz
        PBOX 20,40,800*groesse+200,200*groesse+200
        DEFLINE ,15*groesse,2
	    color rot
	    DEFTEXT 1,groesse/2,groesse/2
        LTEXT 350,50,"MAX"
        @display(maximum)
      else if funktion=3
        COLOR schwarz
        PBOX 20,40,800*groesse+200,200*groesse+200
        DEFLINE ,15*groesse,2
	    color rot
	    DEFTEXT 1,groesse/2,groesse/2
        LTEXT 350,50,"MIN"
	    @display(minimum)
      else if funktion=0
        PBOX 20,40,800*groesse+200,200*groesse+200	
      endif
      CSSETCALLBACK "STE_DIAG_TOROID.TRANSQ_AM",interrupt
    endif
  endif
LOOP
QUIT

PROCEDURE interrupt  
  a=1000*CSGET("STE_DIAG_TOROID.TRANSQ_AM")-offset
  a=ROUND(a*1)/1
  minimum=min(minimum,a)
  maximum=max(maximum,a)
  if funktion=0
	@display(a)
  else if funktion=2
    @display(maximum)  
  else if funktion=3
    @display(minimum)
  endif
RETURN

procedure display(wert)
	COLOR schwarz
    PBOX 20,92,800*groesse+200,200*groesse+200
    COLOR weiss
	DEFLINE ,25*groesse,2
    DEFTEXT 1,groesse,2*groesse
    LTEXT 500,100,"pC"
    LTEXT 400-LTEXTLEN(STR$(wert)),100,STR$(wert)
    VSYNC
return



' Malt einen Button mit text
' sel=1 --> Der Button wird selektiert dargestellt
PROCEDURE button(button_x,button_y,button_t$,sel)
  LOCAL x,y,w,h
  DEFLINE ,1
  DEFTEXT 1,0.05,0.1,0
  button_l=ltextlen(button_t$)
  x=button_x-button_l/2-10
  y=button_y-10
  w=button_l+20
  h=20
  COLOR grau  
  PBOX x+5,y+5,x+w+5,y+h+5
  COLOR abs(sel)*schwarz+abs(not sel)*weiss
  PBOX x,y,x+w,y+h
  IF sel=-1
    COLOR weiss
  ELSE 
    COLOR schwarz
  ENDIF  
  BOX x,y,x+w,y+h
  BOX x-1,y-1,x+w+1,y+h+1
  LTEXT button_x-button_l/2,button_y-5,button_t$
RETURN

FUNCTION inbutton(button_x,button_y,button_t$,mx,my)
  LOCAL x,y,w,h
  
  DEFTEXT 1,0.05,0.1,0
  button_l=ltextlen(button_t$)
  x=button_x-button_l/2-10
  y=button_y-10
  w=button_l+20
  h=20
  IF mx>=x and my>=y and  mx<=x+w and my<=y+h
    RETURN TRUE
  ELSE
    RETURN FALSE
  ENDIF
ENDFUNC
