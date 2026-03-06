; بسم الله الرحمن الرحيم
; ═══════════════════════════════════════════════════════════════════════════════
; مُثبّت لغة ص — Sad Programming Language Installer
; InnoSetup 6.x — Unicode, RTL Arabic
;
; الاستخدام من سطر الأوامر:
;   ISCC.exe sad-setup.iss
;   ISCC.exe sad-setup.iss /DMyVersion=1.0.0 /DMyConfig=Release
; ═══════════════════════════════════════════════════════════════════════════════

; ── المسارات الافتراضية (يمكن تجاوزها بـ /D عند التشغيل)
#ifndef MyVersion
  #define MyVersion "1.0.0"
#endif
#ifndef MyConfig
  #define MyConfig "Debug"
#endif
#ifndef MyRepoRoot
  #define MyRepoRoot "c:\s_lang\s-programming-language"
#endif
#ifndef MyOutputDir
  #define MyOutputDir MyRepoRoot + "\distribution\output"
#endif
#ifndef MyBinDir
  #define MyBinDir MyRepoRoot + "\build\bin\" + MyConfig
#endif
#ifndef MyStdlibDir
  #define MyStdlibDir MyRepoRoot + "\stdlib"
#endif
#ifndef MyReleaseBinDir
  #define MyReleaseBinDir MyRepoRoot + "\build\bin\Release"
#endif
#ifndef MyExamplesDir
  #define MyExamplesDir MyRepoRoot + "\examples"
#endif
#ifndef MyVSIXPath
  #define MyVSIXPath MyRepoRoot + "\tools\vscode-extension\sad-language-2.0.0.vsix"
#endif

; ── معلومات التطبيق
#define MyAppName        "لغة ص"
#define MyAppNameEn      "Sad Programming Language"
#define MyAppPublisher   "فريق لغة ص"
#define MyAppURL         "https://sad-lang.org"

; ──────────────────────────────────────────────────────────────────────────────
[Setup]
AppId={{8F3A2B91-C4E7-4D8F-A6B2-1E9F23D05C84}
AppName={#MyAppName} ({#MyAppNameEn})
AppVersion={#MyVersion}
AppVerName={#MyAppName} {#MyVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}/docs
AppUpdatesURL={#MyAppURL}/releases
AppCopyright=حقوق محفوظة © 2025-2026 فريق لغة ص

DefaultDirName={autopf}\SadLang
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
DisableDirPage=no
DisableProgramGroupPage=no

; مجلد الخرج
OutputDir={#MyOutputDir}
OutputBaseFilename=sad-setup-{#MyVersion}-windows-x64

; الضغط الأقصى
Compression=lzma2/ultra64
SolidCompression=yes

; متطلبات النظام
PrivilegesRequired=admin
PrivilegesRequiredOverridesAllowed=dialog
MinVersion=10.0.17763
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

; إعدادات أخرى
CloseApplications=yes
CloseApplicationsFilter=sad.exe
ChangesEnvironment=yes
ChangesAssociations=yes
WizardStyle=modern

; معلومات إزالة التثبيت
UninstallDisplayName={#MyAppName} {#MyVersion}
UninstallDisplayIcon={app}\bin\sad.exe
Uninstallable=yes
CreateUninstallRegKey=yes

; ──────────────────────────────────────────────────────────────────────────────
[Languages]
Name: "arabic";  MessagesFile: "compiler:Languages\Arabic.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

; ──────────────────────────────────────────────────────────────────────────────
[CustomMessages]

; ─── عربي
arabic.WelcomeTitle=مرحباً بك في مُثبّت لغة ص
arabic.WelcomeDesc=سيقوم هذا المعالج بتثبيت لغة ص {#MyVersion} على جهازك.%n%nلغة ص هي لغة برمجة عربية حديثة تدعم البرمجة الكائنية والمتزامنة.%n%nيُنصح بإغلاق جميع البرامج الأخرى قبل الاستمرار.
arabic.FinishTitle=اكتمل تثبيت لغة ص!
arabic.FinishDesc=تم تثبيت لغة ص {#MyVersion} بنجاح على جهازك.%n%nيمكنك الآن كتابة برامجك بالعربية.
arabic.LaunchREPL=تشغيل مفسّر لغة ص (REPL)
arabic.OpenExamples=فتح مجلد الأمثلة
arabic.CompInterp=المفسر sad.exe — تشغيل ملفات .ص مباشرة (أساسي)
arabic.CompLSP=خادم LSP + أداة التنسيق — دعم المحررات
arabic.CompPkg=مدير الحزم sad-pkg — تثبيت مكتبات لغة ص
arabic.CompFmt=أداة التنسيق sad-fmt — تنسيق أكواد لغة ص
arabic.CompStdlib=المكتبة القياسية (io, math, string, network, graphics)
arabic.CompExamples=ملفات أمثلة لغة ص
arabic.CompVSCode=إضافة VS Code (تمييز الصياغة والتكملة التلقائية)
arabic.GroupCore=المكونات الأساسية
arabic.GroupTools=أدوات التطوير
arabic.CompCompiler=المترجم sadc.exe — تحويل .ص إلى ملف تنفيذي أصلي (LLVM مدمج — 75 MB)
arabic.LLVM_Info=ملاحظة: المترجم sadc.exe يحتوي LLVM بداخله وحجمه 75 MB.التثبيت الكامل سيكون حوالي 85 MB.%n%nإذا كنت تريد تشغيل ملفات .ص فقط دون تحويل إلى كود أصلي، اختر المفسّر فقط.

english.CompCompiler=Compiler sadc.exe — compile .sad to native binary (LLVM bundled — 75 MB)
arabic.TaskPath=إضافة لغة ص إلى متغير PATH (تشغيل 'sad' من أي مكان)
arabic.TaskAssoc=ربط امتداد .ص بمفسر لغة ص (نقر مزدوج للتشغيل)
arabic.TaskDesktop=إنشاء اختصار على سطح المكتب
arabic.LLVMNote=ملاحظة: المترجم sadc يتطلب LLVM 18 ويتم تنزيله بشكل منفصل.
arabic.VSCodeNote=VS Code غير مثبّت — يمكنك تثبيت الإضافة لاحقاً.

; ─── إنجليزي
english.WelcomeTitle=Welcome to the Sad Language Installer
english.WelcomeDesc=This wizard will install Sad Programming Language {#MyVersion}.%n%nSad is a modern Arabic programming language with full OOP and async support.
english.FinishTitle=Sad Language Installation Complete!
english.FinishDesc=Sad Programming Language {#MyVersion} has been successfully installed.
english.LaunchREPL=Launch Sad Interpreter (REPL)
english.OpenExamples=Open Examples Folder
english.CompInterp=Interpreter sad.exe — run .ص files directly (required)
english.CompLSP=LSP Server — VS Code/Vim/Neovim language support
english.CompPkg=Package Manager sad-pkg — install Sad libraries
english.CompFmt=Formatter sad-fmt — format Sad code
english.CompStdlib=Standard Library (io, math, string, network, graphics)
english.CompExamples=Example Sad programs
english.CompVSCode=VS Code Extension (syntax highlighting, autocomplete)
english.GroupCore=Core Components
english.GroupTools=Developer Tools
english.TaskPath=Add Sad to PATH (run 'sad' from anywhere)
english.TaskAssoc=Associate .ص files with Sad Interpreter
english.TaskDesktop=Create Desktop Shortcut
english.LLVMNote=Note: Compiler sadc requires LLVM 18 (downloaded separately).
english.LLVM_Info=Info: sadc.exe bundles LLVM internally (75 MB). Full installation will be ~85 MB.%n%nIf you only want to run .sad files without native compilation, choose Interpreter only.
english.VSCodeNote=VS Code not found — install extension later.

; ──────────────────────────────────────────────────────────────────────────────
[Types]
Name: "full";     Description: "كاملة — كل المكونات بما فيها المترجم"
Name: "dev";      Description: "مطوّر — مفسر + أدوات + دعم VS Code"
Name: "minimal";  Description: "أساسية — المفسر فقط"
Name: "custom";   Description: "مخصصة — اختر بنفسك"; Flags: iscustom

; ──────────────────────────────────────────────────────────────────────────────
[Components]
Name: "interp";    Description: "{cm:CompInterp}";    Types: full dev minimal custom; Flags: fixed
Name: "compiler";  Description: "{cm:CompCompiler}";  Types: full custom
Name: "lsp";       Description: "{cm:CompLSP}";       Types: full dev custom
Name: "pkg";       Description: "{cm:CompPkg}";       Types: full dev custom
Name: "fmt";       Description: "{cm:CompFmt}";       Types: full dev custom
Name: "stdlib";    Description: "{cm:CompStdlib}";    Types: full dev custom
Name: "examples"; Description: "{cm:CompExamples}";  Types: full dev custom
Name: "vscode";    Description: "{cm:CompVSCode}";   Types: full dev custom

; ──────────────────────────────────────────────────────────────────────────────
[Tasks]
Name: "addpath";  Description: "{cm:TaskPath}";    GroupDescription: "System Settings:"
Name: "assoc";    Description: "{cm:TaskAssoc}";   GroupDescription: "System Settings:"
Name: "desktop";  Description: "{cm:TaskDesktop}"; GroupDescription: "Shortcuts:"; Flags: unchecked

; ──────────────────────────────────────────────────────────────────────────────
[Dirs]
Name: "{app}"
Name: "{app}\bin"
Name: "{app}\stdlib"
Name: "{app}\stdlib\core"
Name: "{app}\stdlib\io"
Name: "{app}\stdlib\math"
Name: "{app}\stdlib\string"
Name: "{app}\stdlib\network"
Name: "{app}\stdlib\graphics"
Name: "{app}\stdlib\database"
Name: "{app}\examples"
Name: "{localappdata}\sad-lang"
Name: "{localappdata}\sad-lang\packages"
Name: "{localappdata}\sad-lang\cache"

; ──────────────────────────────────────────────────────────────────────────────
[Files]

; ── المفسر (مطلوب دائماً)
Source: "{#MyBinDir}\sad.exe";    DestDir: "{app}\bin"; DestName: "sad.exe";    Flags: ignoreversion; Components: interp

; ── المترجم sadc (Release — LLVM مدمج — 75 MB)
Source: "{#MyReleaseBinDir}\sadc.exe"; DestDir: "{app}\bin"; DestName: "sadc.exe"; Flags: ignoreversion skipifsourcedoesntexist; Components: compiler

; ── خادم LSP
Source: "{#MyBinDir}\sad-lsp.exe"; DestDir: "{app}\bin"; DestName: "sad-lsp.exe"; Flags: ignoreversion skipifsourcedoesntexist; Components: lsp

; ── مدير الحزم
Source: "{#MyBinDir}\sad-pkg.exe"; DestDir: "{app}\bin"; DestName: "sad-pkg.exe"; Flags: ignoreversion skipifsourcedoesntexist; Components: pkg

; ── المنسّق
Source: "{#MyBinDir}\sad-fmt.exe"; DestDir: "{app}\bin"; DestName: "sad-fmt.exe"; Flags: ignoreversion skipifsourcedoesntexist; Components: fmt

; ── DLLs مشتركة (إذا وُجدت)
Source: "{#MyBinDir}\*.dll";       DestDir: "{app}\bin"; Flags: ignoreversion skipifsourcedoesntexist

; ── المكتبة القياسية
Source: "{#MyStdlibDir}\core\*";     DestDir: "{app}\stdlib\core";     Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib
Source: "{#MyStdlibDir}\io\*";       DestDir: "{app}\stdlib\io";       Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib
Source: "{#MyStdlibDir}\math\*";     DestDir: "{app}\stdlib\math";     Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib
Source: "{#MyStdlibDir}\string\*";   DestDir: "{app}\stdlib\string";   Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib
Source: "{#MyStdlibDir}\network\*";  DestDir: "{app}\stdlib\network";  Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib
Source: "{#MyStdlibDir}\graphics\*"; DestDir: "{app}\stdlib\graphics"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib
Source: "{#MyStdlibDir}\database\*"; DestDir: "{app}\stdlib\database"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: stdlib

; ── الأمثلة
Source: "{#MyExamplesDir}\*"; DestDir: "{app}\examples"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: examples

; ── إضافة VS Code
Source: "{#MyVSIXPath}"; DestDir: "{tmp}"; DestName: "sad-language.vsix"; Flags: ignoreversion deleteafterinstall skipifsourcedoesntexist; Components: vscode

; ── ملف الإعداد
Source: "{#MyRepoRoot}\distribution\assets\sad.config.json"; DestDir: "{app}"; DestName: "sad.config.json"; Flags: ignoreversion onlyifdoesntexist skipifsourcedoesntexist

; ──────────────────────────────────────────────────────────────────────────────
[Icons]
; قائمة ابدأ
Name: "{group}\مفسّر لغة ص (REPL)";  Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; WorkingDir: "{userdocs}"; Comment: "تشغيل مفسّر لغة ص التفاعلي"
Name: "{group}\مدير الحزم sad-pkg";   Filename: "{app}\bin\sad-pkg.exe"; WorkingDir: "{userdocs}"; Comment: "إدارة حزم لغة ص"; Components: pkg
Name: "{group}\الأمثلة";              Filename: "{app}\examples"; Comment: "أمثلة برمجة بلغة ص"; Components: examples
Name: "{group}\إزالة تثبيت لغة ص";   Filename: "{uninstallexe}"; Comment: "إزالة لغة ص من الجهاز"

; سطح المكتب
Name: "{autodesktop}\لغة ص (REPL)";  Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; Tasks: desktop; Comment: "مفسّر لغة ص"

; ──────────────────────────────────────────────────────────────────────────────
[Registry]
; ── امتداد .ص
Root: HKCR; Subkey: ".ص";                               ValueType: string; ValueName: ""; ValueData: "SadSourceFile"; Flags: uninsdeletevalue; Tasks: assoc
Root: HKCR; Subkey: ".sad";                              ValueType: string; ValueName: ""; ValueData: "SadSourceFile"; Flags: uninsdeletevalue; Tasks: assoc
Root: HKCR; Subkey: "SadSourceFile";                     ValueType: string; ValueName: ""; ValueData: "ملف لغة ص";    Flags: uninsdeletekey; Tasks: assoc
Root: HKCR; Subkey: "SadSourceFile\DefaultIcon";         ValueType: string; ValueName: ""; ValueData: "{app}\bin\sad.exe,0"; Tasks: assoc
Root: HKCR; Subkey: "SadSourceFile\shell\open\command";  ValueType: string; ValueName: ""; ValueData: """{app}\bin\sad.exe"" ""%1"""; Tasks: assoc
Root: HKCR; Subkey: "SadSourceFile\shell\edit\command";  ValueType: string; ValueName: ""; ValueData: """{app}\bin\sad.exe"" ""--edit"" ""%1"""; Tasks: assoc

; ── متغيرات البيئة
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "SAD_HOME";   ValueData: "{app}"; Flags: uninsdeletevalue; Tasks: addpath
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "SAD_STDLIB"; ValueData: "{app}\stdlib"; Flags: uninsdeletevalue; Tasks: addpath

; ── معلومات التثبيت
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "Version";     ValueData: "{#MyVersion}"
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "Publisher";   ValueData: "{#MyAppPublisher}"

; ──────────────────────────────────────────────────────────────────────────────
[Run]
; ── تثبيت إضافة VS Code (إذا وُجد)
Filename: "{code:GetVSCodeExe}"; Parameters: "--install-extension ""{tmp}\sad-language.vsix"" --force"; Flags: nowait skipifsilent; StatusMsg: "تثبيت إضافة VS Code..."; Components: vscode; Check: IsVSCodeFound

; ── إشعار النظام بتغيير PATH
Filename: "{sys}\cmd.exe"; Parameters: "/C setx SAD_HOME ""{app}"" /M && setx SAD_STDLIB ""{app}\stdlib"" /M"; Flags: runhidden; Tasks: addpath

; ── خيارات ما بعد التثبيت
Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; Description: "{cm:LaunchREPL}"; Flags: nowait postinstall skipifsilent unchecked

; ──────────────────────────────────────────────────────────────────────────────
[UninstallRun]
Filename: "{sys}\cmd.exe"; Parameters: "/C setx SAD_HOME """" /M && setx SAD_STDLIB """" /M"; Flags: runhidden; RunOnceId: "ClearEnvVars"

; ──────────────────────────────────────────────────────────────────────────────
[UninstallDelete]
Type: filesandordirs; Name: "{localappdata}\sad-lang\cache"

; ──────────────────────────────────────────────────────────────────────────────
[Code]

var
  VSCodeExe: String;

function FindVSCode(): String;
var
  Paths: TArrayOfString;
  I: Integer;
begin
  Result := '';
  SetArrayLength(Paths, 6);
  Paths[0] := ExpandConstant('{localappdata}\Programs\Microsoft VS Code\bin\code.cmd');
  Paths[1] := ExpandConstant('{pf}\Microsoft VS Code\bin\code.cmd');
  Paths[2] := ExpandConstant('{pf64}\Microsoft VS Code\bin\code.cmd');
  Paths[3] := ExpandConstant('{localappdata}\Programs\Microsoft VS Code\Code.exe');
  Paths[4] := ExpandConstant('{pf}\Microsoft VS Code\Code.exe');
  Paths[5] := ExpandConstant('{pf64}\Microsoft VS Code\Code.exe');
  for I := 0 to 5 do
    if FileExists(Paths[I]) then begin
      Result := Paths[I];
      Exit;
    end;
end;

function IsVSCodeFound(): Boolean;
begin
  Result := (VSCodeExe <> '');
end;

function GetVSCodeExe(Param: String): String;
begin
  Result := VSCodeExe;
end;

{ إضافة bin إلى PATH }
procedure AddToPATH(BinPath: String);
var
  CurrentPath: String;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', CurrentPath) then CurrentPath := '';
  if Pos(LowerCase(BinPath), LowerCase(CurrentPath)) = 0 then begin
    if (Length(CurrentPath) > 0) and (CurrentPath[Length(CurrentPath)] <> ';') then
      CurrentPath := CurrentPath + ';';
    CurrentPath := CurrentPath + BinPath;
    RegWriteExpandStringValue(HKEY_LOCAL_MACHINE,
      'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
      'Path', CurrentPath);
  end;
end;

{ إزالة bin من PATH }
procedure RemoveFromPATH(BinPath: String);
var
  OldPath, NewPath, Part: String;
  TmpPath: String;
  P: Integer;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', OldPath) then Exit;
  NewPath := '';
  TmpPath := OldPath + ';';
  P := Pos(';', TmpPath);
  while P > 0 do begin
    Part := Copy(TmpPath, 1, P - 1);
    Delete(TmpPath, 1, P);
    P := Pos(';', TmpPath);
    if (Part <> '') and (CompareText(Part, BinPath) <> 0)
                    and (CompareText(Part, BinPath + '\') <> 0) then begin
      if NewPath <> '' then NewPath := NewPath + ';';
      NewPath := NewPath + Part;
    end;
  end;
  RegWriteExpandStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', NewPath);
end;

{ تهيئة }
procedure InitializeWizard();
begin
  VSCodeExe := FindVSCode();
  WizardForm.WelcomeLabel2.Caption := CustomMessage('WelcomeDesc');
end;

{ بعد التثبيت — إضافة PATH }
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then begin
    if WizardIsTaskSelected('addpath') then
      AddToPATH(ExpandConstant('{app}\bin'));
    { إشعار Windows بتغيير البيئة }
    SendBroadcastMessage($001A, 0, 'Environment');
  end;
end;

{ تحذير VS Code + معلومات المترجم }
function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
  if CurPageID = wpSelectComponents then begin
    { معلومات المترجم sadc }
    if WizardIsComponentSelected('compiler') then
      MsgBox(CustomMessage('LLVM_Info'), mbInformation, MB_OK);
    { تحذير VS Code غير موجود }
    if WizardIsComponentSelected('vscode') and (not IsVSCodeFound()) then
      MsgBox(CustomMessage('VSCodeNote'), mbInformation, MB_OK);
  end;
end;

{ إزالة التثبيت }
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usPostUninstall then begin
    RemoveFromPATH(ExpandConstant('{app}\bin'));
    SendBroadcastMessage($001A, 0, 'Environment');
  end;
end;
