' Test of the data compression routines       Oct 26  2005
' built in X11Basic by Markus Hoffmann
'
' Demonstration on how to use COMPRESS$(), UNCOMPRESS$()
'

' First we nedd a big file to be compressed

' bigfile$="/home/hoffmann/PI-Bonn/projekte/doktorarbeit/main.ps"
bigfile$=param$(1)   ! This is this program itself

OPEN "I",#1,bigfile$
t$=INPUT$(#1,MIN(LOF(#1),1000000))   ! Max 1 Mbyte
CLOSE
PRINT bigfile$;" ";LEN(t$);" Bytes.",
FLUSH
t=TIMER
g$=COMPRESS$(t$)
PRINT STR$(TIMER-t,3,3);" sec. compressed down to ";LEN(g$);" bytes."
' show the first 32 Bytes of the compressed data
MEMDUMP VARPTR(g$),32
' now lets uncompress it again
n$=UNCOMPRESS$(g$)
PRINT "uncompressed it again to ";LEN(n$);" bytes."
' show the first 32 Bytes of the uncompressed data
MEMDUMP VARPTR(n$),64
PRINT
ff$=rle$(t$)
PRINT "after runlengh encoding: ";LEN(ff$);" bytes."
' show the first 32 Bytes of the compressed data
MEMDUMP VARPTR(ff$),64
' And now demystify the compression algorithm
ff$=bwte$(ff$)
PRINT "after burrow wheeler transformation: ";LEN(ff$);" bytes."
' show the first 32 Bytes of the compressed data
MEMDUMP VARPTR(ff$),64
ff$=mtfe$(ff$)
PRINT "after move-to-front encoding: ";LEN(ff$);" bytes."
' show the first 32 Bytes of the compressed data
MEMDUMP VARPTR(ff$),64
ff$=rle$(ff$)
PRINT "after another runlengh encoding: ";LEN(ff$);" bytes."
' show the first 32 Bytes of the compressed data
MEMDUMP VARPTR(ff$),64

ff$=arie$(ff$)
PRINT "after arithmetric encoding: ";LEN(ff$);" bytes."
' show the first 32 Bytes of the compressed data
MEMDUMP VARPTR(ff$),64
PRINT
' Now we can try to compress graphics:
'
CLEARW
COLOR COLOR_RGB(1,1,1)
PCIRCLE 100,100,100
COLOR COLOR_RGB(1,0,0)
TEXT 20,100,"This is a Test"
SHOWPAGE
GET 0,0,200,200,t$
PRINT "We got a portion of the screen: ";len(t$);" bytes."
PUT 300,200,t$
SHOWPAGE
PRINT "compressing takes a while..."
f$=compress$(t$)
PRINT "Compressed it to ";len(f$);" bytes. Thats only ";round(1000*len(f$)/len(t$))/10;"%."
' We can do also without arithmetric coding
ff$=rle$(mtfe$(bwte$(rle$(t$))))
' or without the burrow weeler trafo
fff$=arie$(mtfe$(rle$(t$)))
print " original len:           ";len(t$);" Bytes."
print "           compressed to ";len(f$);" Bytes."
print "2. method: compressed to ";len(ff$);" Bytes."
print "3. method: compressed to ";len(fff$);" Bytes."
PRINT "uncompressing is much faster..."
g$=UNCOMPRESS$(f$)
print "Identical? (-1=Yes, 0=no) : ";g$=t$
' This is the proof:
put 200,200,g$
for i=0 to 100 step 10
  put 0+i,300+i,g$
  showpage
next i

' Now some words concerning the burrow weeler transfor;

a$="The quick brown fox .... and so on ...################a"
print a$,len(a$)
print bwte$(a$),len(bwte$(a$))



pause 10
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

