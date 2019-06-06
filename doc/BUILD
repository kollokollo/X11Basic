These are the requirements to build X11-Basic from its sources. 
The compilation must take place on a UNIX/LINUX/MACOS operating
system. 


On Ubuntu 16.04:

general:

sudo apt-get install autoconf xorg-dev libgcrypt20-dev libgmp-dev \
libasound2-dev liblapack-dev libreadline-dev libusb-dev fftw2 libc6-dev 

Note that if you have not installed all of these packages, the compilation 
may work, but some features will not be included. E.g. If you have not 
installed libasound2-dev, the X11-Basic produces will not have any sound 
cabapilities, but otherwise it works. The configure script is responsible
for checking, which features can be biuld.

Fur building the WINDOWS version:
sudo apt-get install gcc-mingw-w64 wine iscc innosetup-qsp-5.5.9.exe


Building the manual and doc-files:

sudo apt-get install asciidoctor <latex>


For building the Android Version:

sudo apt-get install gradle
