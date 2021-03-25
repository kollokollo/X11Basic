These are build instructions for X11-Basic on various platforms
===============================================================

The focus is on automatically build processes on a fresh linux
installation. All features (sound, bluetooth, etc. ) should be
compiled in.

These are the requirements to build X11-Basic from its sources. 
The compilation must take place on a UNIX/LINUX/MACOS operating
system. 

The resulting files are installable binary packages. .deb 
on linux, setup.exe on WINDOWS and .apk on android. For TomTom and 
ATARI ST you will get a .zip file.


Hint: If you are a package maintainer and want to know how to reliably 
build the binary distributions, maybe you want to have a look into the
docker build instructions (see below) and into the Dockerfile.


## Make the .deb file for Ubuntu linux

On Ubuntu 16.04:

general:

sudo apt-get install \
    autoconf \
    xorg-dev \
    libgcrypt20-dev \
    libgmp-dev \
    libasound2-dev \
    liblapack-dev \
    libreadline-dev \
    libusb-dev \
    fftw2 \
    libc6-dev \
    git \
    make \
    gcc \
    ImageMagick \
    transfig \
    fftw-dev \
    libblas-dev \
    libbluetooth-dev \
    libgmp-dev \
    build-essential \
    checkinstall

git clone https://codeberg.org/kollo/X11Basic.git
cd X11Basic/src/;./configure 
cd X11Basic/src;make;sudo make install

or 

cd X11Basic/src;make;sudo make deb


Note that if you have not installed all of these packages, the compilation 
may work, but some features will not be included. E.g. If you have not 
installed libasound2-dev, the X11-Basic produces will not have any sound 
cabapilities, but otherwise it works. The configure script is responsible
for checking, which features can be biuld.

## Build the setup.exe file for MS WINDOWS

sudo apt-get install \
    gcc-mingw-w64 \
    wine \
    iscc 

cd X11Basic/src;wget https://files.jrsoftware.org/is/5/isetup-5.5.8.exe
cd X11Basic;wine isetup-5.5.8.exe 
cd X11Basic/src;make clean
cd X11Basic/src;make windows

## Build the manual (.pdf) and doc-files:

sudo apt-get install asciidoctor <latex>


## Build the Android .apk file

sudo apt-get install git gradle

git clone https://codeberg.org/kollo/X11-Basic.git

## Build the ATARI ST version (.zip file)

This is a bit complicated and works only on Ubuntu:

sudo add-apt-repository ppa:vriviere/ppa
sudo apt install cross-mint-essential
cd X11Basic/src;make clean
cd X11Basic/src;make tos



## Build the TomTom ONE version (.zip file)

cd X11Basic/src;make tomtom

## Docker Builds

We now also offer the possibility to create the binary files via a docker
environment. On Ubuntu you should have docker-compose installed.


Then, a simple

make docker 

should do the job.

The outputs are in 

src/docker:

isetup-5.5.8.exe		-- InnoSetup installer (not really needed)
x11basic_1.28-64_amd64.deb	-- The debian package for X11-Basic for linux
X11-Basic-manual-1.28.pdf	-- The User Manual
X11-Basic-1.28-64-setup.exe	-- The Installer for WINDOWS
xbasic.mqtt			-- Version of X11-basic with MQTT support compiled in (for linux)
X11-Basic-1.28-64-st.zip	-- A .zip distribution with the ATARI-ST Version of X11-basic
xbasic.sdl			-- Version of X11-basic with SDL support compiled in (for linux)


## see also:

-------- CI for debian:jessie with a Dockerfile ----------
see: https://github.com/shakenfr/x11basic_docker


