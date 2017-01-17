; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{715DDC95-B7DD-4D56-B564-F36527D2CCC7}
AppName=ELN
AppVersion=1.1.11
;AppVerName=ELN 1.1.11
AppPublisher=Daniel Wagenaar
AppPublisherURL=http://www.danielwagenaar.net
AppSupportURL=http://www.danielwagenaar.net
AppUpdatesURL=http://www.danielwagenaar.net
DefaultDirName={pf}\ELN
DisableProgramGroupPage=yes
LicenseFile=C:\Users\Daniel Wagenaar\Documents\Progs\eln\gpl-3.0.txt
OutputDir=../build
OutputBaseFilename=eln-1.1.11-setup
SetupIconFile=C:\Users\Daniel Wagenaar\Documents\Progs\eln\src\App\eln.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Users\Daniel Wagenaar\Documents\Progs\eln\release-eln-x64\*"; DestDir: "{app}"; Flags: ignoreversion  recursesubdirs createallsubdirs

[Icons]
Name: "{commonprograms}\ELN"; Filename: "{app}\eln.exe"
Name: "{commondesktop}\ELN"; Filename: "{app}\eln.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\eln.exe"; Description: "{cm:LaunchProgram,ELN}"; Flags: nowait postinstall skipifsilent

