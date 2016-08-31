' Test of the data compression routines       Oct 26  2005
' built in X11Basic by Markus Hoffmann
'
' Demonstration on how to use COMPRESS$(), UNCOMPRESS$()
'

' First we nedd a big file to be compressed

' bigfile$="/home/hoffmann/PI-Bonn/projekte/doktorarbeit/main.ps"
bigfile$=PARAM$(1)   ! This is this program itself

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
PRINT "We got a portion of the screen: ";LEN(t$);" bytes."
PUT 300,200,t$
SHOWPAGE
PRINT "compressing takes a while..."
f$=compress$(t$)
PRINT "Compressed it to ";LEN(f$);" bytes. Thats only ";round(1000*len(f$)/len(t$))/10;"%."
' We can do also without arithmetric coding
ff$=rle$(mtfe$(bwte$(rle$(t$))))
' or without the burrow weeler trafo
fff$=arie$(mtfe$(rle$(t$)))
PRINT " original len:           ";LEN(t$);" Bytes."
PRINT "           compressed to ";LEN(f$);" Bytes."
PRINT "2. method: compressed to ";LEN(ff$);" Bytes."
PRINT "3. method: compressed to ";LEN(fff$);" Bytes."
PRINT "uncompressing is much faster..."
g$=UNCOMPRESS$(f$)
PRINT "Identical? (-1=Yes, 0=no) : ";g$=t$
' This is the proof:
PUT 200,200,g$
FOR i=0 TO 100 STEP 10
  PUT 0+i,300+i,g$
  SHOWPAGE
NEXT i

' Now some words concerning the burrow weeler transfor;

a$="The quick brown fox .... and so on ...################a"
PRINT a$,LEN(a$)
PRINT bwte$(a$),LEN(bwte$(a$))

PAUSE 10
QUIT
ttt:

FOR j=1 TO 100
  ll=j*1000
  PRINT "Build...",ll,
  FLUSH
  IF j=1
    t$=SPACE$(ll*4)
    FOR i=0 TO ll
      LPOKE varptr(t$)+4*i,i
    NEXT i
    hh$=t$
  ELSE
    t$=t$+hh$
  ENDIF
  PRINT len(t$),
  PRINT "COMPRESS... ",
  FLUSH
  t=timer
  g$=compress$(t$)
  PRINT str$(timer-t,3,3);" Sekunden. ",LEN(g$),
  PRINT "UNCOMPRESS... ",
  FLUSH
  t=timer
  t$=uncompress$(g$)
  PRINT str$(timer-t,3,3);" Sekunden. ",LEN(t$)

NEXT j
QUIT

a$="Hallo ich gehe noch nicht nach Hause ################a"
PRINT len(a$)
PRINT bwte$(a$)
PRINT bwtd$(bwte$(a$))

b$=compress$(a$)
PRINT len(b$)
c$=uncompress$(b$)
PRINT len(c$)
PRINT c$
FOR i=0 TO LEN(b$)-1
  PRINT peek(VARPTR(b$)+i);" ";
NEXT i
PRINT
PRINT "-----"

