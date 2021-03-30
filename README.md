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
and sound. It has a very rich command set, though it is still easy to learn.
The structure of the language is similar to the ATARI-ST GFA-BASIC.
It is a structured dialect with no line numbers. 

X11-Basic supports complex numbers and complex math, as well as arbitrary
big numbers and calculations, as well as very fast 32bit integer and 
64bit floating point operations, very powerful string handling functions for
character strings of any length and any content.

Basic programs can be written with any (third party) text editor.

Programs can be compiled into a platform independent bytecode.

You can directly type in commands and formulas, which are being evaluated. This
way the interpreter works also as a pocket calculator. 

A full manual and command reference is available.

Many example programs can be found in this collection:
https://codeberg.org/kollo/X11-Basic_examples

For further documentation please refer to the X11-Basic home page:
https://x11-basic.codeberg.page/ or 
http://x11-basic.sourceforge.net/

The Android version of X11-Basic can be found here: 
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
Also an up-to-date pdf-version of the user manual can be found there.
You may want to have a look there for further information.

### Important Note:

    X11-BASIC is free software and comes with NO WARRANTY - 
    read the file [COPYING](COPYING) for details
    
(Basically that means, free, open source, use and modify as you like, don't
incorporate it into non-free software, no warranty of any sort, don't blame me
if it doesn't work.)


Standard Installation
=====================

Starting from the src/ directory you should do a

    ./configure
    make

And for a system wide installation:

    sudo make install


To remove the installation:

    sudo make uninstall

  
For further details of the installation process, 
additional configuration options and different install options
please read the file [INSTALL](doc/INSTALL).


### ADDITIONAL CONFIGURATION OPTIONS

(only if you have special needs or if the standard installation fails)

#### The configure script takes additional arguments.
  
'configure' configures X11-Basic to adapt to many kinds of systems.
    
    Usage: ./configure [OPTION]... [VAR=VALUE]...

To assign environment variables (e.g., CC, CFLAGS...), specify them as
VAR=VALUE.  See below for descriptions of some of the useful variables.

Defaults for the options are specified in brackets.

<pre>
Configuration:
  -h, --help              display this help and exit
  -V, --version           display version information and exit
  -q, --quiet, --silent   do not print `checking ...' messages
  -C, --config-cache      alias for `--cache-file=config.cache'
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
  --libdir=DIR            object code libraries [EPREFIX/lib]
  --includedir=DIR        C header files [PREFIX/include]
  --oldincludedir=DIR     C header files for non-gcc [/usr/include]
  --datarootdir=DIR       read-only arch.-independent data root [PREFIX/share]
  --datadir=DIR           read-only architecture-independent data [DATAROOTDIR]
  --mandir=DIR            man documentation [DATAROOTDIR/man]
  --docdir=DIR            documentation root [DATAROOTDIR/doc/x11-basic]

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
</pre>

Use these variables to override the choices made by `configure' or to help
it to find libraries and programs with nonstandard names/locations.

### Different make methods:

#### standalone excecutables (statically linked)
  
To compile the X11-Basic interpreter (called xbasic.static), simply type 
	
    make static

at the shell prompt. The produced file is independent of the x11basic 
library and does not need the x11basic framework to be installed.

After X11Basic has been compiled and linked successfully, you can start it
by typing

    ./xbasic.static

INSTALLING the standalone excecutable:
 Put the executable wherever you like. I have mine in ${HOME}/bin/.

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

Finally, this will produce the statically linked versions of the compilers:

    make xbbc.static xbvm.static xb2c.static xbc


#### The framebuffer version
  
It is possible to compile a version of X11-Basic and its libraries, which 
does not need the X.org framework, but instead uses the systems framebuffer
and direct mouse device for graphics in and output. This might be useful on
e.g. the Raspberry Pi or for other handheld devices with low resources.

    make fb

will produce the framebuffer version of the interpreter called 
fbxbasic.

#### 64bit operating systems

X11-Basic will compile on 64bit systems but it will not be 100% functional.
X11-Basic internally uses 32bit pointers (--> VARPTR()). These are not 
compatible with 64bit. 
Some tricks have been implemented to make it work anyways, but do not expect 
100% compatibility. 


#### Crosscompiling for other platforms

a) ATARI ST (with m68k-atari-mint)

    make tos

b) TomTom (with the ARM linux toolchain)

    make TomTom

c) MS WINDOWS (with i586-mingw32msvc-gcc)

    make windows


For more details you may want to look into the Makefile. 
Improvements are welcome.

### Using docker

You can also automate the build process using a docker image. To create the
docker image do

    cd src/
    touch docker/Dockerfile
    make docker

If everything works out well, you will find debian packages for linux as 
well as the ATARI ST version and the WINDOWS installer in src/docker. Please read 
also the correspnding [README](src/docker/README) file. You can of course also 
run X11-Basic from that docker container:

    XAUTHORITY=$(xauth info | grep "Authority file" | awk '{ print $3 }')
    docker run -ti -v`pwd`:/work/ --net=host -e DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix:ro \
	-v $(XAUTHORITY):/root/.Xauthority:ro kollo/x11basic:1.28-64


### Download

Sources and binary packages (look at the releases section):

<a href="https://codeberg.org/kollo/X11Basic/">
    <img alt="Get it on Codeberg" src="https://get-it-on.codeberg.org/get-it-on-blue-on-white.png" height="60">
</a>


<a href="https://f-droid.org/app/net.sourceforge.x11basic">
<img src="https://f-droid.org/badge/get-it-on.png" alt="Get it on F-Droid" height="80"></a>


