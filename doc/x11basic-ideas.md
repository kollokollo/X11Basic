# Ideas for future releases of X11-Basic


## System Variables

DATAPOINTER oder _DATA mit Restoremöglichkeit

ISDIR(d$)  --> STAT ?  --> geht mit user-funktion...


## SDL Graphics

* Use TTF fonts (this is a complicated task...)
* GPRINT Routinen zu voller VT100 Emulation mit standard-GEM-Font ausweiten. (einschließlich cursor). Die Routinen koennen von TTconsole genommen werden. (teilw. erledigt)
* 16 mögliche Fenster, USEWINDOW, --> SETWINDOW num,mode
* Was tun mit SCREEN und ROOTWINDOW? SAVESCREEN/SAVEWINDOW?


## Sound

* mit MIX kommando kann man die Mixtur beeinflussen (would be nice to have!)
* Event, wenn sound(file) zuende (oder testfunktionen)

## Bluetooth

FSFIRST$() and FSNEXT$() can be used to scan for bluetooth devices. (done)

with OPEN "UB" you can open a RFCOMM connection
with OPEN "UP" cou can open a L2CAP connection.

you can use SEND/RECEIVE/CLOSE on L2CAP connections.
you can use PRINT/INPUT/OUT/INP()/CLOSE on RFCOMM connections.

you can as well setup a server and open a listening socket with OPEN, 
much the same as you would do it with normal internet connections. 


## encryption

The enhancements and new ideas concerning cryptography must be held separately 
from the master branch, because the can cause problems on US export limitations
on sourceforge (I think)...

    err=KEYGEN(typ%,pubkey$,privkey$)
    

# Incompatible Aenderungen:

* xbasic und xbasic.exe umbenennen in x11basic und x11basic.exe
(vielleicht mit einem link von xbasic)

