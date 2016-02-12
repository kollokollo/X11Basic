; File f"ur Inno-Setup fuer X11-basic
; Innosetup kann mit wine benutzt werden.
;
; TODO. .bas extension registrieren
;       Manual vom Startmenu aus aufrufen
;       xbasic.exe im consolenfenster starten.
[Setup]
AppName=X11-Basic   
AppVerName=X11-Basic V1.24 
AppVersion=1.24-24
DefaultDirName={pf}\X11-Basic
; Since no icons will be created in "{group}", we don't need the wizard
; to ask for a Start Menu folder name:
DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\x11basic.ico
OutputDir=Inno-Output
AppCopyright=Copyright (C) 1991-2015 Markus Hoffmann
LicenseFile=COPYING  
InfoBeforeFile=RELEASE_NOTES   
InfoAfterFile=README
AppPublisherURL=http://x11-basic.sourceforge.net

[Files]
Source: "xbasic.exe"; DestDir: "{app}"
Source: "xbc.exe"; DestDir: "{app}"
Source: "xb2c.exe"; DestDir: "{app}"
Source: "xbvm.exe"; DestDir: "{app}"
Source: "xbbc.exe"; DestDir: "{app}"
Source: "Windows.extension/lib/SDL.dll"; DestDir: "{sys}"; Flags: sharedfile regserver 32bit
Source: "Windows-Installer/demo.bas"; DestDir: "{app}/examples"
Source: "Windows-Installer/readme.txt"; DestDir: "{app}"; Flags: isreadme
Source: "Windows.extension/lib/libusb0.dll"; DestDir: "{sys}"; Flags: sharedfile regserver 32bit
Source: "Windows.extension/lib/README-libusb.txt"; DestDir: "{app}/contrib";  Flags: isreadme
Source: "Windows-Installer/x11basic.ico"; DestDir: "{app}"
Source: "Windows-Installer/X11-Basic.pdf"; DestDir: "{app}/doc"
Source: "Windows-Installer/bas.ico"; DestDir: "{app}"
Source: "Windows.extension/lib/README-SDL.txt"; DestDir: "{app}/contrib" ;  Flags: isreadme
Source: "Windows.extension/lib/README-GMP.txt"; DestDir: "{app}/contrib"         ;  Flags: isreadme
Source: "Windows.extension/lib/README-LAPACK.txt"; DestDir: "{app}/contrib"      ;  Flags: isreadme


[Icons]
Name: "{commonprograms}\X11-Basic"; Filename: "{app}\xbasic.exe" ; IconFilename: "{app}\x11basic.ico"
Name: "{commonprograms}\X11-Basic compiler"; Filename: "{app}\xbc.exe" ; IconFilename: "{app}\x11basic.ico"
Name: "{commondesktop}\X11-Basic"; Filename: "{app}\xbasic.exe" ; IconFilename: "{app}\x11basic.ico"

[Run]
; Filename: "{app}\xbc.exe"; Description: "Launch X11-Basic compiler"; 
