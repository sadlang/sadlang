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
OutputDir=output
OutputBaseFilename=sad-{#MyAppVersion}-win64-setup
SetupIconFile=..\..\..\distribution\assets\sad_icon.ico
WizardImageFile=..\..\..\distribution\assets\wizard_banner.bmp
WizardSmallImageFile=..\..\..\distribution\assets\wizard_small.bmp
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64
ChangesEnvironment=yes
PrivilegesRequired=admin
ShowLanguageDialog=auto
LanguageDetectionMethod=uilanguage

; العربية والإنجليزية
[Languages]
Name: "arabic"; MessagesFile: "compiler:Languages\Arabic.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[CustomMessages]
arabic.TaskAddPath=إضافة لغة ص إلى متغير PATH (تشغيل من أي مكان)
arabic.TaskFileAssoc=ربط ملفات .ص بمفسر لغة ص
arabic.TaskDesktop=إنشاء اختصار على سطح المكتب
arabic.PostInstallRun=تشغيل مفسر لغة ص (REPL)
arabic.MenuDocs=التوثيق والمرجع
arabic.MenuExamples=أمثلة لغة ص
arabic.MenuUninstall=إزالة تثبيت لغة ص

english.TaskAddPath=Add Sad to PATH (run from anywhere)
english.TaskFileAssoc=Associate .ص files with Sad interpreter
english.TaskDesktop=Create Desktop shortcut
english.PostInstallRun=Launch Sad Interpreter (REPL)
english.MenuDocs=Documentation
english.MenuExamples=Sad Examples
english.MenuUninstall=Uninstall Sad

[Tasks]
Name: "desktopicon"; Description: "{cm:TaskDesktop}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "addtopath";   Description: "{cm:TaskAddPath}";   GroupDescription: "إعدادات النظام:"
Name: "fileassoc";   Description: "{cm:TaskFileAssoc}"; GroupDescription: "إعدادات النظام:"

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
Name: "{group}\مفسر لغة ص (REPL)"; Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; WorkingDir: "{userdocs}"
Name: "{group}\{cm:MenuDocs}";       Filename: "{app}\docs\SAD_LANGUAGE_COMPLETE_REFERENCE.md"
Name: "{group}\{cm:MenuExamples}";   Filename: "{app}\examples"
Name: "{group}\{cm:MenuUninstall}";  Filename: "{uninstallexe}"
Name: "{autodesktop}\لغة ص (REPL)"; Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; Tasks: desktopicon

[Registry]
; File association for .ص files
Root: HKCR; Subkey: ".ص"; ValueType: string; ValueName: ""; ValueData: "SadSourceFile"; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKCR; Subkey: "SadSourceFile"; ValueType: string; ValueName: ""; ValueData: "ملف مصدر لغة ص"; Flags: uninsdeletekey; Tasks: fileassoc
Root: HKCR; Subkey: "SadSourceFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\bin\sad.exe,0"; Tasks: fileassoc
Root: HKCR; Subkey: "SadSourceFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\sad.exe"" ""%1"""; Tasks: fileassoc
Root: HKCR; Subkey: "SadSourceFile\shell\compile"; ValueType: string; ValueName: ""; ValueData: "ترجمة باستخدام sadc"; Tasks: fileassoc
Root: HKCR; Subkey: "SadSourceFile\shell\compile\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\sadc.exe"" ""%1"""; Tasks: fileassoc

[Code]
{ إضافة المجلد لـ PATH }
procedure CurStepChanged(CurStep: TSetupStep);
var
    Path: string;
begin
    if CurStep = ssPostInstall then
    begin
        if WizardIsTaskSelected('addtopath') then
        begin
            RegQueryStringValue(HKLM, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path);
            if Pos(ExpandConstant('{app}\bin'), Path) = 0 then
            begin
                if (Length(Path) > 0) and (Path[Length(Path)] <> ';') then
                    Path := Path + ';';
                Path := Path + ExpandConstant('{app}\bin');
                RegWriteStringValue(HKLM, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path);
                { إخطار النظام بتغيير البيئة }
                SendBroadcastMessage($001A, 0, '');
            end;
        end;
    end;
end;

{ إزالة من PATH عند إلغاء التثبيت }
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
var
    Path: string;
    AppBin: string;
    P: Integer;
begin
    if CurUninstallStep = usPostUninstall then
    begin
        AppBin := ExpandConstant('{app}\bin');
        RegQueryStringValue(HKLM, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path);
        P := Pos(AppBin, Path);
        if P > 0 then
        begin
            { احذف المسار والفاصلة المحيطة به }
            if (P > 1) and (Path[P-1] = ';') then
                Delete(Path, P - 1, Length(AppBin) + 1)
            else
                Delete(Path, P, Length(AppBin) + 1);
            RegWriteStringValue(HKLM, 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment', 'Path', Path);
            SendBroadcastMessage($001A, 0, '');
        end;
    end;
end;

[Run]
Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; Description: "{cm:PostInstallRun}"; Flags: nowait postinstall skipifsilent unchecked
