# Ideas for future releases of X11-Basic


## System Variables

DATAPOINTER oder _DATA mit Restoremöglichkeit

ISDIR(d$)  --> STAT ?  --> geht mit user-funktion...



## Text console

FORMINPUT  
Syntax:    FORMINPUT a$[,n,prompt$,mask$]
Action:    Enables the insertion of a character string during program 
           execution.
           'n' = the maximum length of the character string.
           a$ is the name of the string variable.
           If a$ is not empty, the current value of a$  is 
           displayed, and can be edited.
	   the text promt$ will be displayed at the beginning of a line. 
           The n charackters of mask$ are used as a mask for the input:
	   X -- any charackter
	   9 -- only digits
	   If n is ommitted or n > len(prompt$), the last charackter of 
	   prompt$ is used as a mask for positions > len(prompt$).
	   
	   Soll auch nach PRINT AT() funktionieren!
	   evtl. promt$ weglassen!




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

