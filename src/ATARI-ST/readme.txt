    X     X    1   1        BBBB
    X     X    1   1        B   B                   i
      X X      1   1        B   B            sss
       X       1   1 -----  BBBB    aaaa    s   s   i   cccc
      X X      1   1        B   B  a   a      s     i  c
    X     X    1   1        B   B  a   a    s   s   i  c   
    X     X    1   1        BBBB    aaaaa    sss    i   cccc
    
    
    
                       VERSION 1.26

            (C) 1997-2018 by Markus Hoffmann
               (kollo@users.sourceforge.net)
            (http://x11-basic.sourceforge.net/)

	             ALL RIGHTS RESERVED
 THIS PROGRAM IS PROVIDED "AS IS" WITHOUT A WARRANTY OF ANY KIND

   X11-Basic is subject to the GNU General Public License.

      See the file `COPYING' for licensing information

This is the Version for ATARI-ST.

Summary: A Basic interpreter and compiler with graphics capabilities
Vendor: Markus Hoffmann
Name: X11Basic
Version: 1.26
Release: 52
Copyright: GPL
Group: Development/Languages
Source: http://x11-basic.sourceforge.net/X11Basic-1.26.tar.gz
URL: http://x11-basic.sourceforge.net/
Packager: Markus Hoffmann <kollo@users.sourceforge.net>

%description

  X11-Basic is a dialect of the BASIC programming language with graphics
  capability. 

The syntax is most similar to the old GFA-Basic ATARI-ST 
implementation.

  This package includes the basic interpreter named xbasic and a set of
  different compilers. 
  A compiler manager (xbc) is included which makes stand alone binaries out of 
  the BASIC programs, which run more than 10 times faster than the interpreted 
  code. 

  X11-Basic programs can also be compiled into a bytecode (see xbbc). This
  bytecode can be interpreted by a virtual machine (see xbvm). Finally, the
  bytecode can be converted to C sourcefiles (see xb2c) which can be compiled
  with the gnu C compiler (or the tiny C compiler) to native excecutables.

You will find the compiler and many other sample programs in 
{install-dir}\examples.

However, a much bigger variety of 
  other sample programs can be found in an X11-Basic examples repository on
  gitlab (https://gitlab.com/kollo/X11-Basic_examples/tree/master) or in a
  package X11-Basic-examples-1.26.zip, which you can download from the homepage
  (http://x11-basic.sourceforge.net/). Also an up-to-date pdf-version of the
  manual can be found there.

  Please read the file X11-Basic-manual.txt (or the .pdf manual) 
  for further information.

  X11-BASIC is free software and comes with NO WARRANTY - read the file
  COPYING for details
 
  (Basically that means, free, open source, use and modify as you like, don't
  incorporate it into non-free software, no warranty of any sort, don't blame me
  if it doesn't work.)

Authors:
--------
    Markus Hoffmann <kollo@users.sourceforge.net>


============================================================================
Requirements for the ATARI ST version:

    - The X11-Basic interpreter (xbasic.prg) as well as the psydocompiler 
      (xbc.prg) and the other excecutables coming with this package need
      at least a standard ATARI ST, 8MHz 1MByte RAM. But it is so slow
      that you want 32MHz and at least 4MBytes RAM.

    - VT100EMU.PRG needs to be present (it should have been run from the
      AUTO-Folder)  to make use of the text console features. VT100EMU.PRG which
      comes with the package only works in monochrome resolution. 

    - optionally: you may want to install tcc, the tiny C compiler, to improve 
      the performance of the X11-Basic compiler. 
      See: http://bellard.org/tcc/
      
