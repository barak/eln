; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{84AF32E8-9487-421E-9A8B-73712049EBBD}
AppName=ELN
AppVersion=1.1.18
AppPublisher=Daniel Wagenaar
AppPublisherURL=http://www.danielwagenaar.net
AppSupportURL=http://www.danielwagenaar.net
AppUpdatesURL=http://www.danielwagenaar.net
DefaultDirName={pf}\ELN
DisableProgramGroupPage=yes
LicenseFile=C:\Users\Daniel Wagenaar\Documents\Progs\eln\GPL-3.0.txt
OutputDir=..\..\releases
OutputBaseName=eln-1.1.18-x86-setup
SetupIconFile=C:\Users\Daniel Wagenaar\Documents\Progs\eln\src\App\eln.ico
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Users\Daniel Wagenaar\Documents\Progs\eln\release-eln-x86\*"; DestDir: "{app}"; Flags: ignoreversion  recursesubdirs createallsubdirs

[Icons]
Name: "{commonprograms}\ELN"; Filename: "{app}\eln.exe"
Name: "{commondesktop}\ELN"; Filename: "{app}\eln.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\eln.exe"; Description: "{cm:LaunchProgram,ELN}"; Flags: nowait postinstall skipifsilent

