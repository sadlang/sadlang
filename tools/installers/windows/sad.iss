; Inno Setup Script for Sad Programming Language
; لغة ص - مثبت Windows

#define MyAppName "لغة ص - Sad"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "مجتمع لغة ص"
#define MyAppURL "https://sad-lang.org"
#define MyAppExeName "sad.exe"

[Setup]
AppId={{SAD-LANG-UNIQUE-ID}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\SadLang
DefaultGroupName={#MyAppName}
LicenseFile=..\..\..\..\LICENSE
InfoBeforeFile=README_INSTALLER.txt
InfoAfterFile=POSTINSTALL.txt
OutputDir=output
OutputBaseFilename=sad-{#MyAppVersion}-win64-setup
SetupIconFile=..\..\..\..\graphics\icons\sad.ico
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64
ChangesEnvironment=yes
PrivilegesRequired=admin

; العربية والإنجليزية
[Languages]
Name: "arabic"; MessagesFile: "compiler:Languages\Arabic.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "addtopath"; Description: "إضافة إلى PATH"; GroupDescription: "خيارات إضافية:"; Flags: checkedonce
Name: "fileassoc"; Description: "ربط ملفات .ص"; GroupDescription: "خيارات إضافية:"; Flags: checkedonce

[Files]
; Main executables
Source: "build\bin\Release\sad.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "build\bin\Release\sadc.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "build\bin\Release\sad-pkg.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "build\bin\Release\sad-fmt.exe"; DestDir: "{app}\bin"; Flags: ignoreversion
Source: "build\bin\Release\sad-lsp.exe"; DestDir: "{app}\bin"; Flags: ignoreversion

; Standard library
Source: "stdlib\*"; DestDir: "{app}\stdlib"; Flags: ignoreversion recursesubdirs createallsubdirs

; Documentation
Source: "docs\SAD_LANGUAGE_COMPLETE_REFERENCE.md"; DestDir: "{app}\docs"; Flags: ignoreversion
Source: "README.md"; DestDir: "{app}"; Flags: ignoreversion

; Examples
Source: "examples\*"; DestDir: "{app}\examples"; Flags: ignoreversion recursesubdirs createallsubdirs

; VS Code extension
Source: "tools\vscode-sad\*"; DestDir: "{app}\vscode-extension"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\Sad REPL"; Filename: "{app}\bin\sad.exe"; WorkingDir: "{userdesktop}"
Name: "{group}\الوثائق"; Filename: "{app}\docs\SAD_LANGUAGE_COMPLETE_REFERENCE.md"
Name: "{group}\الأمثلة"; Filename: "{app}\examples"
Name: "{group}\إلغاء التثبيت"; Filename: "{uninstallexe}"
Name: "{autodesktop}\Sad REPL"; Filename: "{app}\bin\sad.exe"; Tasks: desktopicon

[Registry]
; File association for .ص files
Root: HKCR; Subkey: ".ص"; ValueType: string; ValueName: ""; ValueData: "SadSourceFile"; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKCR; Subkey: "SadSourceFile"; ValueType: string; ValueName: ""; ValueData: "ملف مصدر لغة ص"; Flags: uninsdeletekey; Tasks: fileassoc
Root: HKCR; Subkey: "SadSourceFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\sad.exe,0"; Tasks: fileassoc
Root: HKCR; Subkey: "SadSourceFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\sad.exe"" ""%1"""; Tasks: fileassoc
Root: HKCR; Subkey: "SadSourceFile\shell\compile"; ValueType: string; ValueName: ""; ValueData: "ترجمة باستخدام sadc"; Tasks: fileassoc
Root: HKCR; Subkey: "SadSourceFile\shell\compile\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\sadc.exe"" ""%1"""; Tasks: fileassoc

[Code]
// Add to PATH
procedure CurStepChanged(CurStep: TSetupStep);
var
    Path: string;
begin
    if CurStep = ssPostInstall then
    begin
        if IsTaskSelected('addtopath') then
        begin
            RegQueryStringValue(HKLM, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path);
            if Pos(ExpandConstant('{app}\bin'), Path) = 0 then
            begin
                Path := Path + ';' + ExpandConstant('{app}\bin');
                RegWriteStringValue(HKLM, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path);
            end;
        end;
    end;
end;

// Remove from PATH on uninstall
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
    Path: string;
    P: Integer;
begin
    if CurUninstallStep = usPostUninstall then
    begin
        RegQueryStringValue(HKLM, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path);
        P := Pos(ExpandConstant('{app}\bin'), Path);
        if P &lt;&gt; 0 then
        begin
            Delete(Path, P - 1, Length(ExpandConstant('{app}\bin')) + 1);
            RegWriteStringValue(HKLM, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path);
        end;
    end;
end;

[Run]
Filename: "{app}\bin\sad.exe"; Description: "تشغيل Sad REPL"; Flags: nowait postinstall skipifsilent
