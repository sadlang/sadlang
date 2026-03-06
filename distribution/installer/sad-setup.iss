; بسم الله الرحمن الرحيم
; ═══════════════════════════════════════════════════════════════════════════════
; مُثبّت لغة ص — Sad Programming Language Installer
; InnoSetup 6.x — Unicode, RTL Arabic
; ═══════════════════════════════════════════════════════════════════════════════

#define MyAppName        "لغة ص"
#define MyAppNameEn      "Sad Programming Language"
#define MyAppVersion     "1.0.0"
#define MyAppPublisher   "فريق لغة ص"
#define MyAppURL         "https://sad-lang.org"
#define MyAppExeName     "sad.exe"
#define MyAppCompiler    "sadc.exe"
#define MyAppLSP         "sad-lsp.exe"
#define MyAppPkg         "sad-pkg.exe"
#define MyAppFmt         "sad-fmt.exe"

; ──────────────────────────────────────────────────────────────────────────────
[Setup]
; ───── معلومات التطبيق
AppId={{8F3A2B91-C4E7-4D8F-A6B2-1E9F23D05C84}
AppName={#MyAppName} ({#MyAppNameEn})
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}/support
AppUpdatesURL={#MyAppURL}/releases
AppCopyright=حقوق محفوظة © 2025-2026 فريق لغة ص

; ───── مسار التثبيت
DefaultDirName={autopf}\SadLang
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
DisableDirPage=no
DisableProgramGroupPage=no

; ───── ملف التثبيت الناتج
OutputDir=..\output
OutputBaseFilename=sad-setup-{#MyAppVersion}-windows-x64
SetupIconFile=..\assets\sad_icon.ico
WizardImageFile=..\assets\wizard_banner.bmp
WizardSmallImageFile=..\assets\wizard_small.bmp
WizardImageStretch=no
WizardStyle=modern

; ───── الضغط
Compression=lzma2/ultra64
SolidCompression=yes
LZMAUseSeparateProcess=yes
LZMANumBlockThreads=4

; ───── الأمان والتوافق
PrivilegesRequired=admin
PrivilegesRequiredOverridesAllowed=dialog
MinVersion=10.0.17763
ArchitecturesAllowed=x64compatible arm64
ArchitecturesInstallIn64BitMode=x64compatible arm64
CloseApplications=yes
CloseApplicationsFilter=*.exe
RestartIfNeededByRun=no

; ───── الشهادة الرقمية (يُفعّل عند الإصدار)
; SignTool=signtool
; SignedUninstaller=yes

; ───── اللغة والاتجاه — عربية RTL
ShowLanguageDialog=no

; ───── السجل
ChangesEnvironment=yes
ChangesAssociations=yes

; ───── سجل إزالة التثبيت
UninstallDisplayName={#MyAppName} {#MyAppVersion}
UninstallDisplayIcon={app}\bin\sad.exe
Uninstallable=yes
CreateUninstallRegKey=yes

; ──────────────────────────────────────────────────────────────────────────────
[Languages]
; اللغة العربية أولاً
Name: "arabic";  MessagesFile: "compiler:Languages\Arabic.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

; ──────────────────────────────────────────────────────────────────────────────
[CustomMessages]

; ───── رسائل عربية
arabic.WelcomeTitle=مرحباً بك في مُثبّت لغة ص
arabic.WelcomeDesc=سيقوم هذا المعالج بتثبيت لغة ص {#MyAppVersion} على جهازك.%n%nلغة ص هي لغة برمجة عربية حديثة تدعم البرمجة الكائنية والمتزامنة والتحويل إلى كود أصلي عبر LLVM.%n%nيُنصح بإغلاق جميع البرامج قبل الاستمرار.
arabic.FinishTitle=اكتمل تثبيت لغة ص
arabic.FinishDesc=تم تثبيت لغة ص بنجاح على جهازك.%n%nيمكنك الآن كتابة برامجك بالعربية والتشغيل مباشرة.
arabic.LaunchSad=تشغيل مفسّر لغة ص (REPL)
arabic.OpenDocs=فتح التوثيق
arabic.InstallVSCode=تثبيت إضافة VS Code (يتطلب VS Code)

; المكونات
arabic.CompInterpreter=المفسر (أساسي — تشغيل ملفات .ص مباشرة)
arabic.CompCompiler=المترجم sadc (تحويل .ص إلى ملف تنفيذي أصلي — يتطلب LLVM)
arabic.CompLSP=خادم LSP + أداة التنسيق (دعم المحررات: VS Code / Vim / Neovim...)
arabic.CompPkg=مدير الحزم sad-pkg (تثبيت مكتبات لغة ص)
arabic.CompStdlib=المكتبة القياسية (io, math, string, network, graphics, database)
arabic.CompVSCode=إضافة VS Code (تمييز الصياغة، التكملة التلقائية، التشغيل المباشر)
arabic.CompREPL=بيئة REPL التفاعلية
arabic.GroupCore=المكونات الأساسية
arabic.GroupTools=أدوات التطوير
arabic.GroupIDE=دعم بيئات التطوير

; الطريق
arabic.AddToPath=إضافة لغة ص إلى متغير PATH (يسمح باستخدامها من أي مكان)
arabic.AssocExt=ربط امتداد .ص بمفسر لغة ص (نقر مزدوج للتشغيل)
arabic.DesktopIcon=إنشاء اختصار على سطح المكتب
arabic.StartMenuIcon=إنشاء مجموعة في قائمة ابدأ

; خطأ
arabic.LLVM_Warning=تحذير: المترجم (sadc) يتطلب LLVM 18.%n%nيمكنك تحميل LLVM من:%nhttps://github.com/llvm/llvm-project/releases%n%nبدون LLVM، يمكنك فقط استخدام المفسر (sad).
arabic.VSCode_NotFound=لم يُعثر على VS Code. يمكنك تثبيت الإضافة لاحقاً من:%nhttps://marketplace.visualstudio.com/items?itemName=sad-lang.sad-language

; ───── رسائل إنجليزية
english.WelcomeTitle=Welcome to the Sad Language Installer
english.WelcomeDesc=This wizard will install Sad Programming Language {#MyAppVersion} on your computer.%n%nSad is a modern Arabic programming language with OOP, async/await, and LLVM native compilation support.%n%nPlease close all other applications before continuing.
english.FinishTitle=Sad Language Installation Complete
english.FinishDesc=Sad Programming Language has been successfully installed.%n%nYou can now write and run Sad programs.
english.LaunchSad=Launch Sad Interpreter (REPL)
english.OpenDocs=Open Documentation
english.InstallVSCode=Install VS Code Extension (requires VS Code)

english.CompInterpreter=Interpreter (core — run .ص files directly)
english.CompCompiler=Compiler sadc (compile .ص to native binary — requires LLVM)
english.CompLSP=LSP Server + Formatter (VS Code / Vim / Neovim / Emacs support)
english.CompPkg=Package Manager sad-pkg (install Sad libraries)
english.CompStdlib=Standard Library (io, math, string, network, graphics, database)
english.CompVSCode=VS Code Extension (syntax highlighting, autocomplete, run support)
english.CompREPL=Interactive REPL Environment
english.GroupCore=Core Components
english.GroupTools=Developer Tools
english.GroupIDE=IDE Support

english.AddToPath=Add Sad to PATH (use from anywhere in terminal)
english.AssocExt=Associate .ص files with Sad Interpreter
english.DesktopIcon=Create Desktop Shortcut
english.StartMenuIcon=Create Start Menu Group

english.LLVM_Warning=Warning: The compiler (sadc) requires LLVM 18.%n%nDownload LLVM from:%nhttps://github.com/llvm/llvm-project/releases%n%nWithout LLVM, you can only use the interpreter (sad).
english.VSCode_NotFound=VS Code not found. You can install the extension later from:%nhttps://marketplace.visualstudio.com/items?itemName=sad-lang.sad-language

; ──────────────────────────────────────────────────────────────────────────────
[Types]
Name: "full";     Description: "حزمة كاملة — كل المكونات";       DescriptionEn: "Full — All components"
Name: "dev";      Description: "مطوّر — مفسر + أدوات + VS Code"; DescriptionEn: "Developer — Interpreter + Tools + VS Code"
Name: "minimal";  Description: "أساسي — المفسر فقط";             DescriptionEn: "Minimal — Interpreter only"
Name: "custom";   Description: "مخصص — اختر بنفسك";             DescriptionEn: "Custom — Choose your own"; Flags: iscustom

; ──────────────────────────────────────────────────────────────────────────────
[Components]
; ──── ضروري
Name: "Core\Interpreter"; Description: "{cm:CompInterpreter}"; Types: full dev minimal custom; Flags: fixed
Name: "Core\Stdlib";      Description: "{cm:CompStdlib}";      Types: full dev custom

; ──── أدوات
Name: "Tools\Compiler"; Description: "{cm:CompCompiler}";   Types: full custom
Name: "Tools\LSP";      Description: "{cm:CompLSP}";         Types: full dev custom
Name: "Tools\Pkg";      Description: "{cm:CompPkg}";         Types: full dev custom
Name: "Tools\REPL";     Description: "{cm:CompREPL}";        Types: full dev custom

; ──── IDE
Name: "IDE\VSCode"; Description: "{cm:CompVSCode}"; Types: full dev custom

; ──────────────────────────────────────────────────────────────────────────────
[Tasks]
Name: "addtopath";    Description: "{cm:AddToPath}";    GroupDescription: "إعدادات النظام:"; Flags: checked
Name: "assocext";     Description: "{cm:AssocExt}";     GroupDescription: "إعدادات النظام:"; Flags: checked
Name: "desktopicon";  Description: "{cm:DesktopIcon}";  GroupDescription: "اختصارات:"; Flags: unchecked
Name: "startmenu";    Description: "{cm:StartMenuIcon}"; GroupDescription: "اختصارات:"; Flags: checked

; ──────────────────────────────────────────────────────────────────────────────
[Dirs]
Name: "{app}"
Name: "{app}\bin"
Name: "{app}\lib"
Name: "{app}\stdlib"
Name: "{app}\stdlib\core"
Name: "{app}\stdlib\io"
Name: "{app}\stdlib\math"
Name: "{app}\stdlib\string"
Name: "{app}\stdlib\network"
Name: "{app}\stdlib\graphics"
Name: "{app}\stdlib\database"
Name: "{app}\docs"
Name: "{app}\examples"
Name: "{localappdata}\sad-lang\packages"
Name: "{localappdata}\sad-lang\cache"

; ──────────────────────────────────────────────────────────────────────────────
[Files]
; ──── المفسر (أساسي — دائماً)
Source: "..\..\build\bin\Release\sad.exe";           DestDir: "{app}\bin"; DestName: "sad.exe";     Flags: ignoreversion; Components: Core\Interpreter
Source: "..\..\build\bin\Release\sad_core.dll";      DestDir: "{app}\bin"; Flags: ignoreversion skipifsourcedoesntexist; Components: Core\Interpreter

; ──── المترجم
Source: "..\..\build\bin\Release\sadc.exe";          DestDir: "{app}\bin"; DestName: "sadc.exe";    Flags: ignoreversion skipifsourcedoesntexist; Components: Tools\Compiler

; ──── خادم LSP + المنسَّق
Source: "..\..\build\bin\Release\sad-lsp.exe";       DestDir: "{app}\bin"; DestName: "sad-lsp.exe"; Flags: ignoreversion skipifsourcedoesntexist; Components: Tools\LSP
Source: "..\..\build\bin\Release\sad-fmt.exe";       DestDir: "{app}\bin"; DestName: "sad-fmt.exe"; Flags: ignoreversion skipifsourcedoesntexist; Components: Tools\LSP

; ──── مدير الحزم
Source: "..\..\build\bin\Release\sad-pkg.exe";       DestDir: "{app}\bin"; DestName: "sad-pkg.exe"; Flags: ignoreversion skipifsourcedoesntexist; Components: Tools\Pkg

; ──── REPL
Source: "..\..\build\bin\Release\sad-repl.exe";      DestDir: "{app}\bin"; DestName: "sad-repl.exe"; Flags: ignoreversion skipifsourcedoesntexist; Components: Tools\REPL

; ──── المكتبة القياسية
Source: "..\..\stdlib\core\*";     DestDir: "{app}\stdlib\core";     Flags: ignoreversion recursesubdirs createallsubdirs; Components: Core\Stdlib
Source: "..\..\stdlib\io\*";       DestDir: "{app}\stdlib\io";       Flags: ignoreversion recursesubdirs createallsubdirs; Components: Core\Stdlib
Source: "..\..\stdlib\math\*";     DestDir: "{app}\stdlib\math";     Flags: ignoreversion recursesubdirs createallsubdirs; Components: Core\Stdlib
Source: "..\..\stdlib\string\*";   DestDir: "{app}\stdlib\string";   Flags: ignoreversion recursesubdirs createallsubdirs; Components: Core\Stdlib
Source: "..\..\stdlib\network\*";  DestDir: "{app}\stdlib\network";  Flags: ignoreversion recursesubdirs createallsubdirs; Components: Core\Stdlib
Source: "..\..\stdlib\graphics\*"; DestDir: "{app}\stdlib\graphics"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: Core\Stdlib
Source: "..\..\stdlib\database\*"; DestDir: "{app}\stdlib\database"; Flags: ignoreversion recursesubdirs createallsubdirs; Components: Core\Stdlib

; ──── إضافة VS Code
Source: "..\..\tools\vscode-extension\sad-language-2.0.0.vsix"; DestDir: "{tmp}"; Flags: ignoreversion deleteafterinstall; Components: IDE\VSCode

; ──── الأمثلة
Source: "..\..\examples\*"; DestDir: "{app}\examples"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Interpreter

; ──── الأيقونة
Source: "..\assets\sad_icon.ico"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist

; ──── ملف الترخيص
Source: "..\..\LICENSE"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist

; ──── ملف الإعداد الافتراضي
Source: "..\assets\sad.config.json"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist onlyifdoesntexist

; ──────────────────────────────────────────────────────────────────────────────
[Icons]
; قائمة ابدأ
Name: "{group}\مفسر لغة ص (REPL)";    Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; WorkingDir: "{userdocs}"; IconFilename: "{app}\sad_icon.ico"; Tasks: startmenu; Components: Core\Interpreter
Name: "{group}\محرر لغة ص";           Filename: "{app}\bin\sad.exe"; Parameters: "--edit"; WorkingDir: "{userdocs}"; IconFilename: "{app}\sad_icon.ico"; Tasks: startmenu; Components: Core\Interpreter
Name: "{group}\مدير الحزم sad-pkg";   Filename: "{app}\bin\sad-pkg.exe"; WorkingDir: "{userdocs}"; IconFilename: "{app}\sad_icon.ico"; Tasks: startmenu; Components: Tools\Pkg
Name: "{group}\التوثيق";              Filename: "{app}\docs\index.html"; Tasks: startmenu; Components: Core\Interpreter
Name: "{group}\أمثلة لغة ص";         Filename: "{app}\examples"; Tasks: startmenu; Components: Core\Interpreter
Name: "{group}\إزالة تثبيت لغة ص";   Filename: "{uninstallexe}"; Tasks: startmenu

; سطح المكتب
Name: "{autodesktop}\لغة ص (REPL)";   Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; IconFilename: "{app}\sad_icon.ico"; Tasks: desktopicon; Components: Core\Interpreter

; ──────────────────────────────────────────────────────────────────────────────
[Registry]
; ──── امتداد .ص
Root: HKCR; Subkey: ".ص";                         ValueType: string; ValueName: ""; ValueData: "SadSourceFile";          Flags: uninsdeletevalue; Tasks: assocext
Root: HKCR; Subkey: ".sad";                        ValueType: string; ValueName: ""; ValueData: "SadSourceFile";          Flags: uninsdeletevalue; Tasks: assocext
Root: HKCR; Subkey: "SadSourceFile";               ValueType: string; ValueName: ""; ValueData: "ملف لغة ص";              Flags: uninsdeletekey; Tasks: assocext
Root: HKCR; Subkey: "SadSourceFile\DefaultIcon";   ValueType: string; ValueName: ""; ValueData: "{app}\sad_icon.ico,0";   Tasks: assocext
Root: HKCR; Subkey: "SadSourceFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\sad.exe"" ""%1"""; Tasks: assocext
Root: HKCR; Subkey: "SadSourceFile\shell\edit\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\sad.exe"" ""--edit"" ""%1"""; Tasks: assocext

; ──── PATH (كل المستخدمين — يتطلب صلاحية admin)
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "SAD_HOME"; ValueData: "{app}"; Flags: uninsdeletevalue; Tasks: addtopath
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "SAD_STDLIB"; ValueData: "{app}\stdlib"; Flags: uninsdeletevalue; Tasks: addtopath

; ──── معلومات التثبيت
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersion}"
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "Publisher"; ValueData: "{#MyAppPublisher}"

; ──────────────────────────────────────────────────────────────────────────────
[Run]
; ──── تثبيت إضافة VS Code
Filename: "{code:GetVSCodePath}"; Parameters: "--install-extension ""{tmp}\sad-language-2.0.0.vsix"" --force"; Flags: nowait skipifsilent; Components: IDE\VSCode; StatusMsg: "جاري تثبيت إضافة VS Code..."; Check: VSCodeExists

; ──── إشعار النظام بتغيير PATH
Filename: "{sys}\cmd.exe"; Parameters: "/C setx SAD_HOME ""{app}"" && setx SAD_STDLIB ""{app}\stdlib"""; Flags: runhidden; Tasks: addtopath

; ──── بعد التثبيت — تشغيل REPL
Filename: "{app}\bin\sad.exe"; Parameters: "--repl"; Description: "{cm:LaunchSad}"; Flags: nowait postinstall skipifsilent unchecked; Components: Core\Interpreter
; ──── فتح التوثيق
Filename: "{app}\docs\index.html"; Description: "{cm:OpenDocs}"; Flags: nowait postinstall shellexec skipifsilent unchecked

; ──────────────────────────────────────────────────────────────────────────────
[UninstallRun]
Filename: "{sys}\cmd.exe"; Parameters: "/C setx SAD_HOME """" && setx SAD_STDLIB """""; Flags: runhidden

; ──────────────────────────────────────────────────────────────────────────────
[UninstallDelete]
Type: filesandordirs; Name: "{localappdata}\sad-lang\cache"

; ──────────────────────────────────────────────────────────────────────────────
[Code]

{ ═══════════════════════════════════════════════════════════════════════════
  الكود البرمجي المخصص للمثبّت
  ═══════════════════════════════════════════════════════════════════════════ }

var
  LLVMWarningShown: Boolean;
  VSCodePath: String;

{ ── البحث عن VS Code ── }
function FindVSCode(): String;
var
  Paths: TArrayOfString;
  I: Integer;
  Path: String;
begin
  Result := '';
  SetArrayLength(Paths, 6);
  Paths[0] := ExpandConstant('{localappdata}\Programs\Microsoft VS Code\bin\code.cmd');
  Paths[1] := ExpandConstant('{pf}\Microsoft VS Code\bin\code.cmd');
  Paths[2] := ExpandConstant('{pf64}\Microsoft VS Code\bin\code.cmd');
  Paths[3] := ExpandConstant('{localappdata}\Programs\Microsoft VS Code\Code.exe');
  Paths[4] := ExpandConstant('{pf}\Microsoft VS Code\Code.exe');
  Paths[5] := ExpandConstant('{pf64}\Microsoft VS Code\Code.exe');
  for I := 0 to 5 do begin
    if FileExists(Paths[I]) then begin
      Result := Paths[I];
      Exit;
    end;
  end;
end;

function VSCodeExists(): Boolean;
begin
  Result := (VSCodePath <> '');
end;

function GetVSCodePath(Param: String): String;
begin
  Result := VSCodePath;
end;

{ ── إضافة المجلد لـ PATH ── }
procedure AddToPath(NewPath: String);
var
  OldPath: String;
  Separator: String;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', OldPath) then
    OldPath := '';

  Separator := ';';
  if Pos(';', OldPath) = 0 then Separator := ';';

  { تحقق أن المسار غير موجود مسبقاً }
  if Pos(LowerCase(NewPath), LowerCase(OldPath)) = 0 then begin
    if (Length(OldPath) > 0) and (OldPath[Length(OldPath)] <> ';') then
      OldPath := OldPath + ';';
    OldPath := OldPath + NewPath;
    RegWriteExpandStringValue(HKEY_LOCAL_MACHINE,
      'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
      'Path', OldPath);
  end;
end;

{ ── إزالة المجلد من PATH ── }
procedure RemoveFromPath(PathToRemove: String);
var
  OldPath: String;
  NewPath: String;
  Parts: TArrayOfString;
  I: Integer;
begin
  if not RegQueryStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', OldPath) then Exit;

  { بناء المسار الجديد بدون المسار المراد حذفه }
  NewPath := '';
  Parts := SplitString(OldPath, ';');
  for I := 0 to GetArrayLength(Parts) - 1 do begin
    if (Parts[I] <> '') and
       (CompareText(Parts[I], PathToRemove) <> 0) and
       (CompareText(Parts[I], PathToRemove + '\') <> 0) then begin
      if NewPath <> '' then NewPath := NewPath + ';';
      NewPath := NewPath + Parts[I];
    end;
  end;
  RegWriteExpandStringValue(HKEY_LOCAL_MACHINE,
    'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
    'Path', NewPath);
end;

{ ── تقسيم النص ── }
function SplitString(const S, Delimiters: string): TArrayOfString;
var
  I, N, StartIdx: Integer;
  IsDelim: Boolean;
begin
  SetArrayLength(Result, 0);
  N := 0;
  StartIdx := 1;
  for I := 1 to Length(S) do begin
    IsDelim := Pos(S[I], Delimiters) > 0;
    if IsDelim then begin
      if I > StartIdx then begin
        SetArrayLength(Result, N + 1);
        Result[N] := Copy(S, StartIdx, I - StartIdx);
        Inc(N);
      end;
      StartIdx := I + 1;
    end;
  end;
  if StartIdx <= Length(S) then begin
    SetArrayLength(Result, N + 1);
    Result[N] := Copy(S, StartIdx, Length(S) - StartIdx + 1);
  end;
end;

{ ═══════════════════════════════════════════════
  أحداث المثبّت
  ═══════════════════════════════════════════════ }

{ ── الإعداد الأولي ── }
procedure InitializeWizard();
begin
  LLVMWarningShown := False;
  VSCodePath := FindVSCode();
end;

{ ── ما يحدث عند اكتمال التثبيت ── }
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then begin
    { إضافة bin لـ PATH }
    if IsTaskSelected('addtopath') then begin
      AddToPath(ExpandConstant('{app}\bin'));
    end;
    { إخطار مدير النوافذ بتغيير البيئة }
    SendBroadcastMessage($001A, 0, nil);
  end;
end;

{ ── إنذار قبل التثبيت ── }
function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
  { تحذير LLVM إذا اختار المستخدم المترجم }
  if (CurPageID = wpSelectComponents) and
     (not LLVMWarningShown) and
     WizardIsComponentSelected('Tools\Compiler') then begin
    LLVMWarningShown := True;
    MsgBox(CustomMessage('LLVM_Warning'), mbInformation, MB_OK);
  end;
  { تحذير VS Code غير موجود }
  if (CurPageID = wpSelectComponents) and
     WizardIsComponentSelected('IDE\VSCode') and
     (not VSCodeExists()) then begin
    MsgBox(CustomMessage('VSCode_NotFound'), mbInformation, MB_OK);
  end;
end;

{ ── إزالة التثبيت ── }
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usPostUninstall then begin
    RemoveFromPath(ExpandConstant('{app}\bin'));
    SendBroadcastMessage($001A, 0, nil);
  end;
end;

{ ── التحقق من صحة مسار التثبيت ── }
function PrepareToInstall(var NeedsRestart: Boolean): String;
var
  BinPath: String;
begin
  BinPath := ExpandConstant('{app}\bin');
  Result := '';
end;

{ ── صفحة الترحيب — تخصيص النص ── }
procedure WelcomeActivate(Sender: TObject);
begin
  WizardForm.WelcomeLabel2.Caption := CustomMessage('WelcomeDesc');
end;
