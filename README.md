<img alt="X11-Basic" src="artwork/x11basic-logo-2012.png" width="100%" />


X11-Basic
=========  

(c) 1997-2021 by Markus Hoffmann

A Basic interpreter and compiler with graphics capabilities

    Version : 1.28 
    Group   : Development/Languages
    License : GPL

Home-Page   : https://x11-basic.codeberg.page/


X11-Basic is a dialect of the BASIC programming language with graphics
capability. It has a very rich command set, though it is still easy to learn.

The structure of the language is similar to the ATARI-ST GFA-BASIC.
It is a structured dialect with no line numbers. 
A full manual and command reference is available.
Programs can be compiled into a platform independent bytecode.

You can directly type in commands and formulas, which are being evaluated. This
way the interpreter works also as a pocket calculator. It supports complex
numbers and big integers.

Basic programs can be written with any (third party) text editor.

Many more example programs can be found in this collection:
https://codeberg.org/kollo/X11-Basic_examples

For further documentation please refer to the X11-Basic home page:
https://x11-basic.codeberg.page/ or 
http://x11-basic.sourceforge.net/

The Android Version of X11-Basic can be found here: 
https://codeberg.org/kollo/X11-Basic

### Programs

This package includes the basic interpreter named xbasic and a set of
different compilers. 

A compiler manager (xbc) is included which makes stand alone binaries out of 
the BASIC programs, which run more than 10 times faster than the interpreted 
code. 

X11-Basic programs can also be compiled into a bytecode (see xbbc). This
bytecode can be interpreted by a virtual machine (see xbvm). Finally, the
bytecode can be converted to C source files (see xb2c) which can be compiled
with the gnu C compiler (or the tiny C compiler) to native excecutables.

Ancient ANSI-Basic programs (with line numbers) can be converted to X11-Basic
with the bas2x11basic program (see the man page bas2x11basic(1) ).

You will find the compiler sourcecode and some other sample programs in 
/usr/share/doc/packages/X11Basic/examples. However, a much bigger variety of 
other sample programs can be found in an X11-Basic examples repository 
https://codeberg.org/kollo/X11-Basic_examples

Please also refer to the home page https://x11-basic.codeberg.page/ . 
Also an up-to-date pdf-version of the manual can be found there.

Please read the file X11-Basic-manual.txt (or the .pdf manual) 
for further information.

### Important Note:

    X11-BASIC is free software and comes with NO WARRANTY - read the file
    COPYING for details
    
    (Basically that means, free, open source, use and modify as you like, don't
    incorporate it into non-free software, no warranty of any sort, don't blame me
    if it doesn't work.)
    
    Please read the file INSTALL for compiling instructions.

Standard Installation
=====================

Starting from the src/ directory you should do a

    ./configure
    make

And for a system wide installation

    sudo make install


To remove the installation a

    sudo make uninstall

will do.
  
For further details of the installation process and different install options
please read the file INSTALL.


### ADDITIONAL CONFIGURATION OPTIONS

(only if you have special needs or if the standard installation fails)

#### A)  The configure script takes additional arguments.
  
'configure' configures X11-Basic to adapt to many kinds of systems.
    
    Usage: ./configure [OPTION]... [VAR=VALUE]...

To assign environment variables (e.g., CC, CFLAGS...), specify them as
VAR=VALUE.  See below for descriptions of some of the useful variables.

Defaults for the options are specified in brackets.

<pre>
Configuration:
  -h, --help              display this help and exit
      --help=short        display options specific to this package
      --help=recursive    display the short help of all the included packages
  -V, --version           display version information and exit
  -q, --quiet, --silent   do not print `checking ...' messages
      --cache-file=FILE   cache test results in FILE [disabled]
  -C, --config-cache      alias for `--cache-file=config.cache'
  -n, --no-create         do not create output files
      --srcdir=DIR        find the sources in DIR [configure dir or `..']

Installation directories:
  --prefix=PREFIX         install architecture-independent files in PREFIX
                          [/usr/local]
  --exec-prefix=EPREFIX   install architecture-dependent files in EPREFIX
                          [PREFIX]

</pre>

By default, 'make install' will install all the files in
'/usr/local/bin', '/usr/local/lib' etc.  You can specify
an installation prefix other than '/usr/local' using '--prefix',
for instance '--prefix=$HOME'.

For better control, use the options below.

<pre>
Fine tuning of the installation directories:
  --bindir=DIR            user executables [EPREFIX/bin]
  --sbindir=DIR           system admin executables [EPREFIX/sbin]
  --libexecdir=DIR        program executables [EPREFIX/libexec]
  --sysconfdir=DIR        read-only single-machine data [PREFIX/etc]
  --sharedstatedir=DIR    modifiable architecture-independent data [PREFIX/com]
  --localstatedir=DIR     modifiable single-machine data [PREFIX/var]
  --runstatedir=DIR       modifiable per-process data [LOCALSTATEDIR/run]
  --libdir=DIR            object code libraries [EPREFIX/lib]
  --includedir=DIR        C header files [PREFIX/include]
  --oldincludedir=DIR     C header files for non-gcc [/usr/include]
  --datarootdir=DIR       read-only arch.-independent data root [PREFIX/share]
  --datadir=DIR           read-only architecture-independent data [DATAROOTDIR]
  --infodir=DIR           info documentation [DATAROOTDIR/info]
  --localedir=DIR         locale-dependent data [DATAROOTDIR/locale]
  --mandir=DIR            man documentation [DATAROOTDIR/man]
  --docdir=DIR            documentation root [DATAROOTDIR/doc/x11-basic]
  --htmldir=DIR           html documentation [DOCDIR]
  --dvidir=DIR            dvi documentation [DOCDIR]
  --pdfdir=DIR            pdf documentation [DOCDIR]
  --psdir=DIR             ps documentation [DOCDIR]

X features:
  --x-includes=DIR    X include files are in DIR
  --x-libraries=DIR   X library files are in DIR

Optional Features:
  --disable-option-checking  ignore unrecognized --enable/--with options
  --disable-FEATURE       do not include FEATURE (same as --enable-FEATURE=no)
  --enable-FEATURE[=ARG]  include FEATURE [ARG=yes]
  --enable-cryptography   enable cryptographic features [default=no]
  --enable-static         only produce static libraries [default=no]
  --enable-smallram       make version siutable for systems with small RAM
                          [default=no]
  --enable-german         make german version [default=no]

Optional Packages:
  --with-PACKAGE[=ARG]    use PACKAGE [ARG=yes]
  --without-PACKAGE       do not use PACKAGE (same as --with-PACKAGE=no)
  --with-bluetooth        support blutooth [default=yes]
  --with-usb              support USB [default=yes]
  --with-mqtt             support mqtt [default=no]
  --with-readline         support readline library [default=yes]
  --with-framebuffer      support framebuffer [default=no]
  --with-sdl              support SDL library [default=no]
  --with-x                use the X Window System [default=yes]

Some influential environment variables:
  CC          C compiler command
  CFLAGS      C compiler flags
  LDFLAGS     linker flags, e.g. -L<lib dir> if you have libraries in a
              nonstandard directory <lib dir>
  LIBS        libraries to pass to the linker, e.g. -l<library>
  CPPFLAGS    (Objective) C/C++ preprocessor flags, e.g. -I<include dir> if
              you have headers in a nonstandard directory <include dir>
  CPP         C preprocessor
  XMKMF       Path to xmkmf, Makefile generator for X Window System
</pre>

Use these variables to override the choices made by `configure' or to help
it to find libraries and programs with nonstandard names/locations.

#### B) Different make methods:

##### standalone excecutables (statically linked)

a) the interpreter
  
To compile the X11-Basic interpreter (called xbasic), simply type 
	
    make static

at the shell prompt. The produced file is independent of the x11basic 
library and does not need the x11basic framework to be installed.

After X11Basic has been compiled and linked successfully, you can start it
by typing

    ./xbasic.static

The command-line options can be listed with the '-h' option ('./xbasic -h').

INSTALLING the standalone excecutable:
 
Put the executable wherever you like. I have mine in ${HOME}/bin/.

b) compiler and libraries

If you want to use the compilers, you have to make both the static and dynamic
libraries.

    cd ./src
    make lib              will generate libx11basic.so
    make x11basic.a       will generate x11basic.a

-- now you have to be root to install the libraries at a place where the 
compiler can use them --

    mv libx11basic.so /usr/lib/
    mv x11basic.a /usr/lib/

-- please also check the permissions of the files:

    chmod 755 	/usr/lib/x11basic.a		
    chmod 755       /usr/lib/libx11basic.so 
    rehash

    make xbbc.static xbvm.static xb2c.static xbc

will produce the statically linked versions of the compilers.

#### C) The framebuffer version
  
It is possible to compile a version of X11-Basic and its libraries, which 
does not need the X.org framework, but instead uses the systems framebuffer
and direct mouse device for graphics in and output. This might be useful on
e.g. the Raspberry Pi or for other handheld devices with low resources.

    make clean fb

will produce the framebuffer version of the interpreter called 
xbasic.framebuffer

#### D) 64bit operating systems

X11-Basic will compile on 64bit systems but it will not be 100% functional.
X11-Basic internally uses 32bit pointers (--> VARPTR()). These are not 
compatible with 64bit. 
Some tricks have been implemented to make it work anyways, but do not expect 
100% compatibility. 


#### E) Crosscompiling for other platforms

a) ATARI ST (with m68k-atari-mint)

    make tos

b) TomTom (with the ARM linux toolchain)

    make TomTom

c) MS WINDOWS (with i586-mingw32msvc-gcc)

    make windows


For more details you may want to look into the Makefile. 
Improvements are welcome.

#### Download

Sources and binary packages:

<a href="https://codeberg.org/kollo/X11Basic/">
    <img alt="Get it on Codeberg" src="https://get-it-on.codeberg.org/get-it-on-blue-on-white.png" height="60">
</a>


<a href="https://f-droid.org/app/net.sourceforge.x11basic">
<img src="https://f-droid.org/badge/get-it-on.png" alt="Get it on F-Droid" height="80"></a>


