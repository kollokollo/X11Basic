' Pseudo-Kompiler fuer X11-Basic
' erzeugt allein lauffaehigen Code auf Wunsch auch statisch mit der
' X11-Basic-Library gelinkt
' (c) Markus Hoffmann Apr. 2001 (letzte Bearbeitung: 10.8.2001)

clearw
gelb=get_color(65535,65535,0)
color gelb
text 10,15,"X11-Basic Compiler V.1.04b (c) Markus Hoffmann 2001"
fileselect "Select source file...","./*.bas","input.bas",f$
if len(f$)=0
  quit
endif
if not exist(f$)
  ~form_alert(1,"[3][File not found!][CANCEL]")
  quit
endif
rumpf$=f$
while len(rumpf$)
  wort_sep rumpf$,"/",1,a$,rumpf$
wend
wort_sep a$,".bas",1,rumpf$,a$

open "O",#2,"/tmp/psydo-code.c"
print #2,"/* PSEUDO-Code.C ("+f$+")" 
print #2,"   X11-BAsic-Pseudo-Compiler Version 1.04b"
print #2,"   (c) Markus Hoffmann "
print #2,"*/"
print #2
print #2,"#include <stdio.h>"
print #2,"#include <stdlib.h>"
print #2,"#include <string.h>"
print #2,"void reset_input_mode();"
print #2,"void x11basicStartup();"
print #2,"void programmlauf();"
print #2,"const char version[]="+chr$(34)+"1.04"+chr$(34)+";"
print #2,"const char vdate[]="+chr$(34)+date$+" "+time$+" xxxx 1.04"+chr$(34)+";"
print #2,"int programbufferlen=0;"
print #2,"extern int param_anzahl;"
print #2,"extern char **param_argumente;"
print #2,"char ifilename[]="+chr$(34)+f$+chr$(34)+";"

print #2,"char *programbuffer=NULL;"
print #2,"char *program[]={"

open "I",#1,f$
while not eof(#1)
  lineinput #1,t$

  t$=xtrim$(t$)
  if len(t$)
    if left$(t$)<>"'"
      wort_sep t$," !",1,t$,b$
      if left$(t$,6)="PRINT "
        t$="? "+right$(t$,len(t$)-6)
      else if left$(t$,6)="GOSUB "
        t$="@"+right$(t$,len(t$)-6)
      else if left$(t$,5)="VOID "
        t$="~"+right$(t$,len(t$)-5)
      endif
      t$=@ersetze$(t$,chr$(34),"##AN"+"F##")
      t$=@ersetze$(t$,"\","##BACKS"+"LASH##")

      t$=@ersetze$(t$,"##A"+"NF##","\042")
      t$=@ersetze$(t$,"##BACK"+"SLASH##","\\")
      
      print #2,chr$(34)+t$+chr$(34)+","
    endif
  endif
wend
close #1
print #2,"};"
print #2,"int prglen=sizeof(program)/sizeof(char *);"
print #2,"main(int anzahl, char *argumente[]) { "
print #2,"  x11basicStartup(); set_input_mode(1,0);" 
print #2,"  atexit(reset_input_mode);"
print #2,"  param_anzahl=anzahl;"
print #2,"  param_argumente=argumente;"
print #2,"  init_program(); c_run(""); programmlauf();"
print #2,"}"
close #2
system "cc -O2 -c /tmp/psydo-code.c"
a=form_alert(2,"[2][Shall I link the x11basic library| static or dynamic ?| ][static|dynamic|CANCEL]")
if a=2
  system "cc -o a.out psydo-code.o -L/usr/lib/termcap -L/usr/X11R6/lib -lx11basic -lm -lX11 -lreadline -ldl -ltermcap"
  system "strip a.out"
  @endname
else if a=1
  system "cc -L/usr/X11R6/lib -L/usr/lib/termcap -o a.out psydo-code.o  /usr/local/lib/x11basic.a  -lm -lX11 -lreadline -ldl -ltermcap"
  system "strip a.out"
  @endname
else 

endif
system "rm -f /tmp/psydo-code.c psydo-code.o"
quit
function ersetze$(era$,erb$,erc$)
  local tta$,ttb$
  wort_sep era$,erb$,0,tta$,ttb$
  while era$<>tta$
    era$=trim$(tta$+erc$+ttb$)
    wort_sep era$,erb$,0,tta$,ttb$
  wend
  return era$
endfunction
procedure endname
  fileselect "Select executable output file...","./*",rumpf$,f$
  if len(f$)
    if exist(f$)
      if form_alert(1,"[3][File already exist!][overwrite|CANCEL]")=1
        system "mv a.out "+f$
      endif
    else
      system "mv a.out "+f$
    endif
  endif
return
