; File for Inno-Setup for X11-Basic   (c) by Markus Hoffmann
; Innosetup kann mit wine benutzt werden.
;
; TODO: start von bas files.
;

#define MyAppName "X11-Basic"
#define MyAppURL "http://x11-basic.sourceforge.net/"
#define MyAppExeName "xbasic-wrapper.bat"
#define MyAppVersion "1.27"
#define MyAppRelease "60"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{4B22E38E-73C8-4199-8C9F-8E4B8A00A97C}
AppName={#MyAppName}
AppVerName={#MyAppName} V.{#MyAppVersion} 
AppVersion={#MyAppVersion}-{#MyAppRelease}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
AppPublisherURL={#MyAppURL}


; Since no icons will be created in "{group}", we don't need the wizard
; to ask for a Start Menu folder name:
; DisableProgramGroupPage=yes
UninstallDisplayIcon={app}\x11basic.ico

OutputDir=Inno-Output
AppCopyright=Copyright (C) 1991-2019 Markus Hoffmann
LicenseFile=../COPYING 
InfoBeforeFile=../RELEASE_NOTES
InfoAfterFile=INTRO
OutputBaseFilename={#MyAppName}-{#MyAppVersion}-{#MyAppRelease}-setup


[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1


[Files]
Source: "xbasic.exe"; DestDir: "{app}"
Source: "xbc.exe"; DestDir: "{app}"
Source: "xb2c.exe"; DestDir: "{app}"
Source: "xbvm.exe"; DestDir: "{app}"
Source: "xbbc.exe"; DestDir: "{app}"
Source: "WINDOWS/lib/SDL.dll"; DestDir: "{sys}"; Flags: sharedfile onlyifdoesntexist  32bit
Source: "WINDOWS/demo.bas"; DestName: "demo.bas"; DestDir: "{app}/examples"
Source: "WINDOWS/readme-windows.txt"; DestDir: "{app}"; Flags: isreadme
Source: "WINDOWS/lib/libusb0.dll"; DestDir: "{sys}"; Flags: sharedfile onlyifdoesntexist  32bit
Source: "WINDOWS/lib/README-libusb.txt"; DestDir: "{app}/contrib";  Flags: isreadme
Source: "WINDOWS/x11basic.ico"; DestDir: "{app}"
Source: "../doc/manual/Outputs/X11-Basic-manual-1.27.pdf"; DestName: "X11-Basic.pdf"; DestDir: "{app}/doc"
Source: "WINDOWS/bas.ico"; DestDir: "{app}"
Source: "WINDOWS/lib/README-SDL.txt"; DestDir: "{app}/contrib" ;  Flags: isreadme
Source: "WINDOWS/lib/README-GMP.txt"; DestDir: "{app}/contrib"         ;  Flags: isreadme
Source: "WINDOWS/lib/README-LAPACK.txt"; DestDir: "{app}/contrib"      ;  Flags: isreadme
Source: "WINDOWS/ansicon/readme.txt"; DestName: "README-ANSICON.txt"; DestDir: "{app}/contrib"      ;  Flags: isreadme
Source: "WINDOWS/ansicon/LICENSE.txt"; DestName: "LICENSE-ANSICON.txt"; DestDir: "{app}/contrib"  
Source: "WINDOWS/ansicon/x86/ANSI32.dll"; DestDir: "{app}/contrib"  
Source: "WINDOWS/ansicon/x86/ansicon.exe";  DestDir: "{app}/contrib"  
Source: "WINDOWS/xbasic-wrapper.bat";  DestDir: "{app}"  


[Icons]
; Name: "{commonprograms}\{#MyAppName}"; Filename: "{app}\xbasic-wrapper.bat" ; IconFilename: "{app}\x11basic.ico"
Name: "{group}\X11-Basic"; Filename: "{app}\{#MyAppExeName}" ; IconFilename: "{app}\x11basic.ico"
; Name: "{commonprograms}\X11-Basic compiler"; Filename: "{app}\xbc.exe" ; IconFilename: "{app}\x11basic.ico"
Name: "{group}\X11-Basic compiler"; Filename: "{app}\xbc.exe" ; IconFilename: "{app}\x11basic.ico"
Name: "{commondesktop}\X11-Basic"; Filename: "{app}\{#MyAppExeName}" ; IconFilename: "{app}\x11basic.ico"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon
Name: "{group}\X11-Basic manual"; Filename: "{app}\doc\X11-Basic.pdf" 
Name: "{group}\Demo"; Filename: "{app}\examples\demo.bas" 

[Registry]
Root: HKCR; Subkey: ".bas"; ValueType: string; ValueName: ""; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".bas\ShellNew"; ValueType: string; ValueName: "NullFile"; ValueData: ""; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".b"; ValueType: string; ValueName: ""; ValueData: "{#MyAppName}"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".b\ShellNew"; ValueType: string; ValueName: "NullFile"; ValueData: ""; Flags: uninsdeletevalue

;Local: HKCR; Subkey: "SOFTWARE\\{#MyAppName}"; ValueType: string; ValueName: "path"; ValueData: "{app}\"; Flags: uninsdeletevalue
;Local: HKCR; Subkey: "SOFTWARE\\{#MyAppName}"; ValueType: string; ValueName: "librarypath"; ValueData: "{app}\lib\"; Flags: uninsdeletevalue
;Local: HKCR; Subkey: "SOFTWARE\\{#MyAppName}"; ValueType: string; ValueName: "font"; ValueData: "swiss13"; Flags: uninsdeletevalue
;Local: HKCR; Subkey: "SOFTWARE\\{#MyAppName}"; ValueType: string; ValueName: "geometry"; ValueData: "+10+10"; Flags: uninsdeletevalue
;Local: HKCR; Subkey: "SOFTWARE\\{#MyAppName}"; ValueType: string; ValueName: "version"; ValueData: "{MyAppVersion}"; Flags: uninsdeletevalue

Root: HKCR; Subkey: "{#MyAppName}"; ValueType: string; ValueName: ""; ValueData: "X11-Basic Program"; Flags: uninsdeletekey
Root: HKCR; Subkey: "{#MyAppName}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bas.ico,0"
Root: HKCR; Subkey: "{#MyAppName}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"" ""%2"" ""%3""" 
Root: HKCR; Subkey: "{#MyAppName}\shell\New"; ValueType: string; ValueName: ""; ValueData: "&Edit" 
Root: HKCR; Subkey: "{#MyAppName}\shell\New\command"; ValueType: string; ValueName: ""; ValueData: "{sys}\Notepad.exe" 
Root: HKCR; Subkey: "{#MyAppName}\shell\ViewDocu"; ValueType: string; ValueName: ""; ValueData: "&View docu" 
Root: HKCR; Subkey: "{#MyAppName}\shell\ViewDocu\command"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName} ""-doc_%1""" 

[Run]
; Filename: "{app}\xbc.exe"; Description: "Launch X11-Basic compiler"; 
Filename: "{app}\xbasic-wrapper.bat"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
