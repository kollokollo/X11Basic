

goto tttt
OPEN "I",#1,"/home/hoffmann/PI-Bonn/projekte/doktorarbeit/main.ps"
t$=INPUT$(#1,min(lof(#1),1000000))
bsave "delme",varptr(t$),len(t$)
close
print len(t$),
flush
t=timer
g$=compress$(t$)
print str$(timer-t,3,3);" Sekunden. ",len(g$)

quit
ttt:

for j=1 to 100 
  ll=j*1000
  print "Build...",ll,
  flush
  if j=1
  t$=space$(ll*4)
  for i=0 to ll
    lpoke varptr(t$)+4*i,i
  next i
  hh$=t$
  else
  t$=t$+hh$
  endif
  print len(t$),
  print "COMPRESS... ",
flush
t=timer
g$=compress$(t$)
print str$(timer-t,3,3);" Sekunden. ",len(g$),
  print "UNCOMPRESS... ",
flush
t=timer
t$=uncompress$(g$)
print str$(timer-t,3,3);" Sekunden. ",len(t$)

next j
quit


a$="Hallo ich gehe noch nicht nach Hause ################a"
print len(a$)
print bwte$(a$)
print bwtd$(bwte$(a$))

b$=compress$(a$)
print len(b$)
c$=uncompress$(b$)
print len(c$)
print c$
for i=0 to len(b$)-1
print peek(varptr(b$)+i);" ";
next i
print
print "-----"

tttt:

rootwindow
color get_color(65535,65535,65535)
pcircle 100,100,100
color get_color(65535,0,0)
text 20,100,"Das ist ein Test"
showpage
get 0,0,200,200,t$
print len(t$)
put 300,200,t$
showpage
f$=compress$(t$)
ff$=rle$(mtfe$(bwte$(rle$(t$))))
print " original len:           ";len(t$);" Bytes."
print "           compressed to ";len(f$);" Bytes."
print "2. method: compressed to ";len(ff$);" Bytes."

print "Original:"
memdump varptr(t$),160
print "Method 1:"
memdump varptr(f$),96
print "Method 2:"
memdump varptr(ff$),96


print len(f$)
g$=uncompress$(f$)
put 200,200,g$
print len(ff$)
gg$=rld$(ff$)
print len(gg$)
gg$=mtfd$(gg$)
print len(gg$)

gg$=bwtd$(gg$)
print len(gg$)
gg$=rld$(gg$)
print len(gg$)
gg$=rld$(bwtd$(mtfd$(rld$(ff$))))
if gg$<>t$
  print "ERROR"
  print len(gg$),len(t$)
else
  put 200,200,g$
  for i=0 to 100
  put 0+i,300+i,gg$
  showpage
  next i
endif
showpage

print len(g$)
for i=0 to 30
print peek(varptr(g$)+i);" ";
next i
print
print 1*(g$=t$)
quit
