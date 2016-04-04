Guide to contributing to X11-Basic
==================================

X11-Basic exists for quite a while now. Therefore I consider  it to be in a
quite mature state. I am planning not to extend the language much,  e.g. I do
not want to add many more commands or functions. The language as it is should
stay much the same as it is now. 

However, there are still some bugs and also some of the features mentioned in 
the manual are not implemented. 

As there are:

1. The Array operators a(1:2,3) with a specified range are not implemented. 
   Always a complete row must be used: a(:,3) works fine.

2. Modifyable lvalued, like MIDS(t$,2,3)=a$ are not implemented,  also
   TIME$="12:04:06" does not work as you might expect (resulting in setting the
   systems time). However, an array with index range as a lvalue does already
   work.

These issues should be fixed somewhen. Also there are ideas for adding Bluetooth 
support and USB support to the Android version of X11-Basic. This should lead 
to as few additional commands and functions as possible. Maybe it can be done 
usinge external binaries called via the shell (like the SQL support is 
implemented).

Difficult bugs:
===============

There is a strange bug related to screen refresh in the Android version of 
X11-Basic. Does anybody have an idea what could be the problem?


More things left to do:
=======================
- Optimize a bit more, improve performance
- The WINDOWS-Version needs more work
- Fix the bugs
- work on the sound system
- work on the SDL-Graphics implementation (fill styles etc...)
- documentation needs more work
- find someone who can port it to apple mac/ipad/iphone
(etc. etc.)
