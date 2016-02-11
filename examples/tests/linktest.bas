' Hier wird gezeigt, wie man externe Programmroutinen verwenden kann

t$="/usr/lib/libreadline.so"
print t$
link #1,t$
dump "#"
promt$=">>>"
adr=exec(sym_adr(#1,"readline"),L:varptr(promt$))
r=adr
while peek(r)>0
  print chr$(peek(r));
  inc r
wend
print
unlink #1
free adr
quit
