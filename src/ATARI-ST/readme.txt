    X     X    1   1        BBBB
    X     X    1   1        B   B                   i
      X X      1   1        B   B            sss
       X       1   1 -----  BBBB    aaaa    s   s   i   cccc
      X X      1   1        B   B  a   a      s     i  c
    X     X    1   1        B   B  a   a    s   s   i  c   
    X     X    1   1        BBBB    aaaaa    sss    i   cccc
    
    
    
                       VERSION 1.23

            (C) 1997-2014 by Markus Hoffmann
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
Version: 1.23
Release: 10
Copyright: GPL
Group: Development/Languages
Source: http://x11-basic.sourceforge.net/X11Basic-1.23.tar.gz
URL: http://x11-basic.sourceforge.net/
Packager: Markus Hoffmann <kollo@users.sourceforge.net>

%description

X11-Basic is a dialect of the BASIC programming language with graphics 
capability that integrates features like shell scripting, cgi-Programming 
and full graphical visualisation into the easy to learn basic language on 
modern computers. The syntax is most similar to the old GFA-Basic ATARI-ST 
implementation.

This package includes the basic interpreter named xbasic. It can be used as a
shell, can run basic-scripts. You can make excecutable  scripts p.ex.
*.cgi-Scripts for handling web-input. A pseudo compiler is included which
makes stand alone binaries out of the scripts. It compiles itself.

You will find the compiler and many other sample programs in 
{install-dir}\examples.

Authors:
--------
    Markus Hoffmann <kollo@users.sourceforge.net>

============================================================================

 A short description of the syntax of X11-Basic can be found 
 in the file:
                 X11-Basic.pdf

============================================================================
Requirements for the ATARI ST version:

    - The X11-Basic interpreter (xbasic.prg) as well as the psydocompiler 
      (xbc.prg) and the other excecutables coming with this package need
      at least a standard ATARI ST, 8MHz 1MByte RAM.  

    - VT100EMU.PRG needs to be present (it should have been run from the
      AUTO-Folder)  to make use of the text console features. VT100EMU.PRG which
      comes with the package only works in monochrome resolution. 

    - optionally: you may want to install tcc, the tiny C compiler, to improve 
      the performance of the X11-Basic compiler. 
      See: http://bellard.org/tcc/
      
