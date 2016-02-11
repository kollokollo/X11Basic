fileselect "Image laden...","./*.xwd","",f$
if len(f$)
if exist(f$)
open "I",#1,f$
len=lof(#1)
close #1
t$=space$(len)
bload f$,varptr(t$)
print len(t$)
for j=0 to 3
for i=0 to 5
put 70*i,35*j,t$
next i
next j
while len(f$)
wort_sep f$,"/",0,a$,f$
wend
print "' Ausgabe von inlinegraphics (c) Markus Hoffmann"
print "RESTORE "+a$
print "READ len"
print a$+"$="+chr$(34)+chr$(34)
print "FOR i=0 to len-1"
print "  READ a"
print "  "+a$+"$="+a$+"$+chr$(a)"
print "NEXT i"
print "PUT 0,0,"+a$+"$"
print "VSYNC"
print "END"
print "' internes Image "+a$
print a$+":"
print "data "+str$(len)
count=0
print "DATA ";
for i=0 to len-1
a=peek(varptr(t$)+i)
print str$(a);
inc count
if i<len-1
if count>20
  print
  print "DATA ";
  count=0
else
  print ",";
endif
endif
next i
print
endif
endif
quit
