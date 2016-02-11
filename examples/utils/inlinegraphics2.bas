f$=param$(2)

if exist(f$)
  open "I",#1,f$
  len=lof(#1)
  close #1
  t$=space$(len)
  bload f$,varptr(t$)
  while len(f$)
    wort_sep f$,"/",0,a$,f$
  wend
print "' Ausgabe von inlinegraphics (c) Markus Hoffmann"
print "RESTORE "+a$
print "READ len,clen"
print a$+"$="+chr$(34)+chr$(34)
print "FOR i=0 to clen"
print "  READ a$"
print "FOR j=0 to len(a$)/2-1"
print "  "+a$+"$="+a$+"$+chr$(16*(peek(varptr(a$)+j*2)-65)+peek(varptr(a$)+1+j*2)-97)"
print "NEXT j"
print "NEXT i"
print "PUT 0,0,"+a$+"$"
print "VSYNC"
print "END"
print "' internes Image "+a$
print a$+":"
print "data "+str$(len)
count=0

tn$=""
for i=0 to len-1
a=peek(varptr(t$)+i) and 255
u$=chr$((a div 16)+asc("A"))+chr$((a and 15)+asc("a"))
tn$=tn$+u$
next i

print "DATA "+str$(len(tn$) div 70)
while len(tn$)-count>70
print "DATA "+chr$(34)+mid$(tn$,count+1,70)+chr$(34)
add count,70
wend
print "DATA "+chr$(34)+mid$(tn$,count+1,len(tn$)-count)+chr$(34)
else
print f$+" nicht gefunden."
endif

quit
