' Hier wird gezeigt, wie man externe Programmroutinen verwenden kann

t$=env$("PWD")+"/tmp/goguo.so"
print t$
link #1,t$
dump "#"
argument=3.45678
exec sym_adr(#1,"gogo")
r=exec(sym_adr(#1,"gogo"),varptr(argument))
print "Return war: "+str$(r)
unlink #1
quit
