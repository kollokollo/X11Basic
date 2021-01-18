    X     X    1   1        BBBB
    X     X    1   1        B   B                   i
      X X      1   1        B   B            sss
       X       1   1 -----  BBBB    aaaa    s   s   i   cccc
      X X      1   1        B   B  a   a      s     i  c
    X     X    1   1        B   B  a   a    s   s   i  c   
    X     X    1   1        BBBB    aaaaa    sss    i   cccc
    
    
    
                       VERSION 1.27

            (C) 1997-2021 by Markus Hoffmann
               (kollo@users.sourceforge.net)
            (http://x11-basic.sourceforge.net/)

	             ALL RIGHTS RESERVED
 THIS PROGRAM IS PROVIDED "AS IS" WITHOUT A WARRANTY OF ANY KIND

   X11-Basic is subject to the GNU General Public License.

      See the file `COPYING' for licensing information

This is the WINDOWS-Version of X11-Basic.

Summary: A Basic interpreter and compiler with graphics capabilities
Vendor: Markus Hoffmann
Name: X11Basic
Version: 1.27
Copyright: GPLv2
Group: Development/Languages
Source: https://gitlab.com/kollo/X11Basic/
URL: http://x11-basic.sourceforge.net/
Packager: Markus Hoffmann <kollo@users.sourceforge.net>

%description

X11-Basic is a dialect of the BASIC programming language with graphics 
that integrates features like shell scripting, cgi-Programming 
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
 To INSTALL X11-Basic: click on setup.exe
 To REMOVE X11-Basic: click on the software icon of the control panel
 To CUSTOMIZE: Edit the registry-keys in 
       HKEY_LOCAL_MACHINE/SOFTWARE/x11basic

 A short description of the syntax of X11-Basic can be found 
 in the file:
                 X11-Basic.pdf

============================================================================
Requirements for the Windows version:

    - The X11-Basic interpreter (xbasic.exe) as well as the compiler 
      (xbc.exe) and the other excecutables coming with this package need
      at least Windows 95 to run -- it was not tested in Windows 3.x yet.

    - The SDL.dll (the Simple DirectMedia library) must be present. Al 
      though it is incuded in this package, and also installed by the 
      X11-Basic installer, there might already be a more recent version 
      available. A recent version can be downloadded at: http://www.libsdl.org/
      See the File README-SDL.txt for more information.

    - The libgfx.lib routines ( (c) A. Schiffler, 1999-2003, licensed under the 
      LGPL) are statically linked to the X11-Basic library. 
      See: http://www.ferzkopp.net/Software/SDL_gfx-2.0/

    - X11-Basic for WINDOWS comes with ANSICON included in the package. This 
      enables the text console features under WINDOWS.
      Please see the file README-ANSICON for details. 
      On older WINDOWS version ANSI.SYS need to be present to make use of the 
      text console features. You can find instructions on how to use and install
      ANSI.SYS on your computer in the internet.

    - optionally: you may want to install tcc, the tiny C compiler, to improve 
      the performance of the X11-Basic compiler. 
      See: http://bellard.org/tcc/
      
