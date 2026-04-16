; -*- coding: utf-8 -*-
; Sad Programming Language Installer v2
; InnoSetup 6.x - Unicode, Arabic/English

#define MyAppName        "Sad Language"
#define MyAppNameAr      "لغة ص"
#define MyAppVersion     "1.0.0"
#define MyAppPublisher   "Sad Language Team"
#define MyAppURL         "https://sad-lang.org"
#define MyAppExeName     "sad.exe"

[Setup]
AppId={{8F3A2B91-C4E7-4D8F-A6B2-1E9F23D05C84}
AppName={#MyAppNameAr} ({#MyAppName})
AppVersion={#MyAppVersion}
AppVerName={#MyAppNameAr} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}/support
AppUpdatesURL={#MyAppURL}/releases
AppCopyright=Copyright 2025-2026 Sad Language Team

DefaultDirName={autopf}\SadLang
DefaultGroupName={#MyAppNameAr}
AllowNoIcons=yes
DisableDirPage=no
DisableProgramGroupPage=no

OutputDir=..\output
OutputBaseFilename=sad-setup-{#MyAppVersion}-windows-x64
SetupIconFile=..\assets\sad_icon.ico
WizardImageFile=..\assets\wizard_banner.bmp
WizardSmallImageFile=..\assets\wizard_small.bmp
WizardImageStretch=no
WizardStyle=modern

Compression=lzma2/ultra64
SolidCompression=yes
LZMAUseSeparateProcess=yes

PrivilegesRequired=admin
PrivilegesRequiredOverridesAllowed=dialog
MinVersion=10.0.17763
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
CloseApplications=yes
RestartIfNeededByRun=no

ShowLanguageDialog=auto

ChangesEnvironment=yes
ChangesAssociations=yes

UninstallDisplayName={#MyAppNameAr} {#MyAppVersion}
UninstallDisplayIcon={app}\bin\sad.exe
Uninstallable=yes
CreateUninstallRegKey=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[CustomMessages]
english.CompInterpreter=Interpreter (run .sad files directly)
english.CompCompiler=Compiler sadc (compile to native binary - LLD embedded)
english.CompLSP=LSP Server + Formatter (editor support)
english.CompPkg=Package Manager sad-pkg
english.CompStdlib=Standard Library (io, math, string, network, graphics)
english.CompExamples=Example Programs
english.AddToPath=Add Sad to system PATH
english.AssocExt=Associate .sad files with Sad interpreter
english.DesktopIcon=Create Desktop shortcut
english.StartMenuIcon=Create Start Menu group
english.PrevInstallFound=A previous installation of Sad Language was found at:%n%n%1%n%nVersion: %2%n%nWhat would you like to do?
english.PrevUninstallFirst=Uninstall previous version first, then install new
english.PrevUpgrade=Upgrade (install over existing)
english.PrevCancel=Cancel installation

[Types]
Name: "full"; Description: "Full installation - all components"
Name: "minimal"; Description: "Minimal - interpreter only"
Name: "custom"; Description: "Custom - choose components"; Flags: iscustom

[Components]
Name: "interpreter"; Description: "{cm:CompInterpreter}"; Types: full minimal custom; Flags: fixed
Name: "compiler"; Description: "{cm:CompCompiler}"; Types: full custom
Name: "lsp"; Description: "{cm:CompLSP}"; Types: full custom
Name: "pkg"; Description: "{cm:CompPkg}"; Types: full custom
Name: "stdlib"; Description: "{cm:CompStdlib}"; Types: full custom
Name: "examples"; Description: "{cm:CompExamples}"; Types: full custom

[Tasks]
Name: "addtopath"; Description: "{cm:AddToPath}"
Name: "assocext"; Description: "{cm:AssocExt}"
Name: "desktopicon"; Description: "{cm:DesktopIcon}"; Flags: unchecked
Name: "startmenu"; Description: "{cm:StartMenuIcon}"

[Dirs]
Name: "{app}"
Name: "{app}\bin"
Name: "{app}\stdlib"
Name: "{app}\examples"
Name: "{commonappdata}\sad-lang\packages"
Name: "{commonappdata}\sad-lang\cache"

[Files]
; Interpreter (always)
Source: "..\..\build\bin\Release\sad.exe"; DestDir: "{app}\bin"; DestName: "sad.exe"; Flags: ignoreversion; Components: interpreter

; Compiler
Source: "..\..\build\bin\Release\sadc.exe"; DestDir: "{app}\bin"; DestName: "sadc.exe"; Flags: ignoreversion skipifsourcedoesntexist; Components: compiler

; LSP + Formatter
Source: "..\..\build\bin\Release\sad-lsp.exe"; DestDir: "{app}\bin"; Flags: ignoreversion skipifsourcedoesntexist; Components: lsp
Source: "..\..\build\bin\Release\sad-fmt.exe"; DestDir: "{app}\bin"; Flags: ignoreversion skipifsourcedoesntexist; Components: lsp

; Package Manager
Source: "..\..\build\bin\Release\sad-pkg.exe"; DestDir: "{app}\bin"; Flags: ignoreversion skipifsourcedoesntexist; Components: pkg

; SDL2
Source: "..\..\build\bin\Release\SDL2.dll"; DestDir: "{app}\bin"; Flags: ignoreversion skipifsourcedoesntexist; Components: interpreter

; Standard Library
Source: "..\..\stdlib\core\*"; DestDir: "{app}\stdlib\core"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib
Source: "..\..\stdlib\io\*"; DestDir: "{app}\stdlib\io"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib
Source: "..\..\stdlib\math\*"; DestDir: "{app}\stdlib\math"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib
Source: "..\..\stdlib\string\*"; DestDir: "{app}\stdlib\string"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib
Source: "..\..\stdlib\network\*"; DestDir: "{app}\stdlib\network"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib
Source: "..\..\stdlib\graphics\*"; DestDir: "{app}\stdlib\graphics"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib

; Examples
Source: "..\..\examples\*"; DestDir: "{app}\examples"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: examples

; Icon
Source: "..\assets\sad_icon.ico"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist

; License
Source: "..\..\LICENSE"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist

; Config
Source: "..\assets\sad.config.json"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist onlyifdoesntexist

[Icons]
Name: "{group}\Sad Interpreter"; Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; WorkingDir: "{commondocs}"; IconFilename: "{app}\sad_icon.ico"; Tasks: startmenu
Name: "{group}\Uninstall Sad"; Filename: "{uninstallexe}"; Tasks: startmenu
Name: "{autodesktop}\Sad Language"; Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; IconFilename: "{app}\sad_icon.ico"; Tasks: desktopicon

[Registry]
; File associations
Root: HKCR; Subkey: ".sad"; ValueType: string; ValueName: ""; ValueData: "SadSourceFile"; Flags: uninsdeletevalue; Tasks: assocext
Root: HKCR; Subkey: "SadSourceFile"; ValueType: string; ValueName: ""; ValueData: "Sad Source File"; Flags: uninsdeletekey; Tasks: assocext
Root: HKCR; Subkey: "SadSourceFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\sad_icon.ico,0"; Tasks: assocext
Root: HKCR; Subkey: "SadSourceFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\sad.exe"" ""%1"""; Tasks: assocext

; Environment variables
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "SAD_HOME"; ValueData: "{app}"; Flags: uninsdeletevalue; Tasks: addtopath
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "SAD_STDLIB"; ValueData: "{app}\stdlib"; Flags: uninsdeletevalue; Tasks: addtopath

; Install info
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersion}"
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "Publisher"; ValueData: "{#MyAppPublisher}"

[Run]
Filename: "{sys}\cmd.exe"; Parameters: "/C setx SAD_HOME ""{app}"" && setx SAD_STDLIB ""{app}\stdlib"""; Flags: runhidden; Tasks: addtopath
Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; Description: "Launch Sad REPL"; Flags: nowait postinstall skipifsilent unchecked

[UninstallRun]
Filename: "{sys}\cmd.exe"; Parameters: "/C setx SAD_HOME """" && setx SAD_STDLIB """""; Flags: runhidden; RunOnceId: "ClearSadEnv"

[UninstallDelete]
Type: filesandordirs; Name: "{commonappdata}\sad-lang\cache"

[Code]
var
  PrevInstallPath: String;
  PrevInstallVersion: String;
  PrevInstallFound: Boolean;

procedure AddToPath(const NewPath: String);
var
  OldPath: String;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', OldPath) then
    OldPath := '';
  if Pos(Lowercase(NewPath), Lowercase(OldPath)) = 0 then begin
    if (Length(OldPath) > 0) and (OldPath[Length(OldPath)] <> ';') then
      OldPath := OldPath + ';';
    OldPath := OldPath + NewPath;
    RegWriteExpandStringValue(HKEY_LOCAL_MACHINE,
      'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
      'Path', OldPath);
  end;
end;

procedure RemoveFromPath(const PathToRemove: String);
var
  OldPath, NewPath, Part: String;
  I, Start: Integer;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', OldPath) then Exit;
  NewPath := '';
  Start := 1;
  for I := 1 to Length(OldPath) do begin
    if (OldPath[I] = ';') or (I = Length(OldPath)) then begin
      if I = Length(OldPath) then
        Part := Copy(OldPath, Start, I - Start + 1)
      else
        Part := Copy(OldPath, Start, I - Start);
      if (Part <> '') and (CompareText(Part, PathToRemove) <> 0) then begin
        if NewPath <> '' then NewPath := NewPath + ';';
        NewPath := NewPath + Part;
      end;
      Start := I + 1;
    end;
  end;
  RegWriteExpandStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', NewPath);
end;

function DetectPreviousInstall(): Boolean;
begin
  Result := False;
  PrevInstallPath := '';
  PrevInstallVersion := '';
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\SadLang', 'InstallPath', PrevInstallPath) then begin
    if DirExists(PrevInstallPath) then begin
      RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\SadLang', 'Version', PrevInstallVersion);
      if PrevInstallVersion = '' then PrevInstallVersion := 'unknown';
      Result := True;
    end;
  end;
  { Also check default InnoSetup uninstall registry }
  if not Result then begin
    if RegQueryStringValue(HKEY_LOCAL_MACHINE,
      'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{8F3A2B91-C4E7-4D8F-A6B2-1E9F23D05C84}_is1',
      'InstallLocation', PrevInstallPath) then begin
      if DirExists(PrevInstallPath) then begin
        RegQueryStringValue(HKEY_LOCAL_MACHINE,
          'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{8F3A2B91-C4E7-4D8F-A6B2-1E9F23D05C84}_is1',
          'DisplayVersion', PrevInstallVersion);
        if PrevInstallVersion = '' then PrevInstallVersion := 'unknown';
        Result := True;
      end;
    end;
  end;
end;

function RunPreviousUninstaller(): Boolean;
var
  UninstallStr: String;
  ResultCode: Integer;
begin
  Result := False;
  if RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{8F3A2B91-C4E7-4D8F-A6B2-1E9F23D05C84}_is1',
    'UninstallString', UninstallStr) then begin
    if FileExists(RemoveQuotes(UninstallStr)) then begin
      Result := Exec(RemoveQuotes(UninstallStr), '/SILENT /NORESTART', '', SW_SHOW, ewWaitUntilTerminated, ResultCode);
      if Result then
        Result := (ResultCode = 0);
    end;
  end;
end;

function InitializeSetup(): Boolean;
var
  Choice: Integer;
  Msg: String;
begin
  Result := True;
  PrevInstallFound := DetectPreviousInstall();
  if PrevInstallFound then begin
    Msg := 'A previous installation of Sad Language was found:' + #13#10 + #13#10 +
           'Path: ' + PrevInstallPath + #13#10 +
           'Version: ' + PrevInstallVersion + #13#10 + #13#10 +
           'Click YES to uninstall the previous version first,' + #13#10 +
           'click NO to install over it (upgrade),' + #13#10 +
           'or click CANCEL to abort.';
    Choice := MsgBox(Msg, mbConfirmation, MB_YESNOCANCEL);
    case Choice of
      IDYES: begin
        if not RunPreviousUninstaller() then begin
          if MsgBox('Could not uninstall the previous version.' + #13#10 +
                    'Continue with installation anyway?',
                    mbError, MB_YESNO) = IDNO then begin
            Result := False;
            Exit;
          end;
        end;
      end;
      IDNO: begin
        { Upgrade - continue with install }
      end;
      IDCANCEL: begin
        Result := False;
        Exit;
      end;
    end;
  end;
end;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then begin
    if WizardIsTaskSelected('addtopath') then begin
      AddToPath(ExpandConstant('{app}\bin'));
    end;
  end;
end;

procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usPostUninstall then begin
    RemoveFromPath(ExpandConstant('{app}\bin'));
    RegDeleteValue(HKEY_LOCAL_MACHINE,
      'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
      'SAD_HOME');
    RegDeleteValue(HKEY_LOCAL_MACHINE,
      'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
      'SAD_STDLIB');
  end;
end;
