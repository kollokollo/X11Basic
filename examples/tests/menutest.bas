' Test-program fuer die Drop-Down-Menus
'
DIM field$(50)
FOR i=0 TO 50
  READ field$(i)
  EXIT IF field$(i)="***"
NEXT i
oh=0
field$(i)=""
print i
DATA "INFO","  Menutest"
DATA "---------------"
DATA "- Access.1","- Access.2","- Access.3","- Access.4","- Access.5","- Access.6",""
DATA "FILE","  new","  open ...","  save","  save as ...","--------------","  print","--------------","  Quit",""
DATA "EDIT","  cut","  copy","  paste","----------","  help1","  helper","  assist",""
DATA "HELP","  online help","--------------","  edifac","  editor","  edilink","  edouard",""
DATA "***"

grau=get_color(32000,32000,32000)
color grau
pbox 0,0,640,400
COLOR 5
PCIRCLE 500,200,100
dump "@"
MENUDEF field$(),menuaction
DO 
  pause 0.05
  MENU 
LOOP
quit

PROCEDURE menuaction(k)
  local b
  IF (field$(k)="  Quit") OR (field$(k)="  exit") 
    quit
  ELSE IF field$(k)="  online help"
    oh=not oh
    MENUSET k,4*abs(oh)
  ELSE IF field$(k)="  Menutest" 
    ~form_alert(1,"[0][---- Menutest ----||(c) Markus Hoffmann 2001|X11-Basic V.1.03][ OK ]")
  ELSE   
    PRINT "MENU selected ";k;" contents: ";field$(k)
    b=form_alert(1,"[1][--- Menutest ---||You selected item (Nr. "+str$(k)+"),| fuer den es keine|Funktion gibt !][ OK |Deaktivieren]")
    if b=2
      MENUSET k,8
    endif
  ENDIF
RETURN
