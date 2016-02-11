' Hier wird gezeigt, wie man externe Programmroutinen verwenden kann

't$=env$("PWD")+"/tmp/goguo.so"
t$="/usr/lib/libm.so"
print t$
link #1,t$
dump "#"
argument=3.45678
exec sym_adr(#1,"sin")
r=exec(sym_adr(#1,"sin"),0.1)
print "Return war: "+str$(r)
unlink #1
quit
