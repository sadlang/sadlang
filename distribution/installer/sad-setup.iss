; بسم الله الرحمن الرحيم
; ═══════════════════════════════════════════════════════════════════════════════
; مُثبّت لغة ص — Sad Programming Language Installer
; InnoSetup 6.x — Unicode, RTL Arabic
; ═══════════════════════════════════════════════════════════════════════════════

#define MyAppName        "لغة ص"
#define MyAppNameEn      "Sad Programming Language"
#define MyAppVersion     "1.0.0"
#define MyAppPublisher   "Saleh Kadah — مطوّر لغة ص"
#define MyAppPublisherEn "Saleh Kadah — Sad Language Developer"
#define MyAppURL         "https://sad-lang.org"
#define MyAppContact     "kadah253@gmail.com"
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
AppCopyright=Copyright © 2025-2026 Saleh Kadah — حقوق محفوظة © صالح قدح
AppContact={#MyAppContact}

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
ShowLanguageDialog=auto
LanguageDetectionMethod=uilanguage

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
arabic.LaunchSad=عرض أوامر لغة ص
arabic.InstallVSCode=تثبيت إضافة VS Code (يتطلب VS Code)

; المكونات
arabic.CompHub=مركز الأدوات (أساسي — sad build وsad check…)
arabic.CompCompiler=المترجم sad-build/sadc (تحويل .ص إلى ملف تنفيذي أصلي)
arabic.CompLSP=خادم LSP + أداة التنسيق (دعم المحررات: VS Code / Vim / Neovim...)
arabic.CompPkg=مدير الحزم sad-pkg (تثبيت مكتبات لغة ص)
arabic.CompStdlib=المكتبة القياسية (io, math, string, network, graphics, database)
arabic.CompVSCode=إضافة VS Code (تمييز الصياغة، التكملة التلقائية، التشغيل المباشر)
arabic.GroupCore=المكونات الأساسية
arabic.GroupTools=أدوات التطوير
arabic.GroupIDE=دعم بيئات التطوير

; الطريق
arabic.AddToPath=إضافة لغة ص إلى متغير PATH (يسمح باستخدامها من أي مكان)
arabic.AssocExt=ربط امتداد .ص بمترجم لغة ص (نقر مزدوج للترجمة)
arabic.DesktopIcon=إنشاء اختصار على سطح المكتب
arabic.StartMenuIcon=إنشاء مجموعة في قائمة ابدأ

; خطأ
; (AR) ⚠️ `LLVM_Warning` **بلا قارئ**: لا `{cm:LLVM_Warning}` ولا
;      `CustomMessage('LLVM_Warning')` في الملفّ، و`LLVMWarningShown` يُسنَدُ
;      ولا يُقرَأ. وهذا سابقٌ لهذا التغيير، وقد صُحِّحَ نصُّها (كان يقول إنّ
;      بلا LLVM «يمكنك فقط استخدام المفسر») كي لا تكذبَ إن وُصِلَت يومًا.
; (EN) LLVM_Warning has no reader: no {cm:} reference, no CustomMessage call,
;      and LLVMWarningShown is assigned but never read. That predates this
;      change; the text was corrected (it used to say that without LLVM you
;      could only use the interpreter) so it will not lie if ever wired up.
arabic.LLVM_Warning=المترجم (sadc) مربوطٌ بـLLVM ساكنًا — لا تحتاج تثبيت شيء.
arabic.VSCode_NotFound=لم يُعثر على VS Code. يمكنك تثبيت الإضافة لاحقاً من:%nhttps://marketplace.visualstudio.com/items?itemName=sad-lang.sad-language

; صفحة LLVM
arabic.LLVMPageTitle=إعداد مترجم لغة ص (LLVM)
arabic.LLVMPageDesc=المترجم يربط LLVM ساكنًا — لا حاجة لتثبيتها (الصفحة متروكة سجلًّا)
arabic.LLVMFound=تم اكتشاف LLVM تلقائياً
arabic.LLVMNotFound=لم يُعثر على LLVM على جهازك
arabic.LLVMPathLabel=مسار مجلد LLVM:
arabic.LLVMBrowse=استعراض...
arabic.LLVMValidOK=✓ LLVM صالح (الإصدار: %1)
arabic.LLVMValidFail=✗ المسار لا يحتوي على LLVM صالح
arabic.LLVMVersionOld=⚠ الإصدار %1 قديم — يُفضّل الإصدار 14 أو أحدث
arabic.LLVMDownloadBtn=تحميل LLVM 18 من GitHub
arabic.LLVMSkip=تخطي (تثبيت بدون sadc — يمكن إضافة LLVM لاحقاً)
arabic.LLVMAutoDetect=إعادة الكشف التلقائي
arabic.LLVMBinsFound=الملفات المكتشفة: %1
arabic.LLVMLibsFound=مكتبات: موجودة ✓
arabic.LLVMLibsMissing=مكتبات: غير موجودة (قد لا يعمل sadc بشكل صحيح)
arabic.LLVMHeadersFound=ملفات Headers: موجودة ✓

; ───── رسائل إنجليزية
english.WelcomeTitle=Welcome to the Sad Language Installer
english.WelcomeDesc=This wizard will install Sad Programming Language {#MyAppVersion} on your computer.%n%nSad is a modern Arabic programming language with OOP, async/await, and LLVM native compilation support.%n%nPlease close all other applications before continuing.
english.FinishTitle=Sad Language Installation Complete
english.FinishDesc=Sad Programming Language has been successfully installed.%n%nYou can now write and run Sad programs.
english.LaunchSad=Show Sad commands
english.InstallVSCode=Install VS Code Extension (requires VS Code)

english.CompHub=Tool hub (core — sad build, sad check…)
english.CompCompiler=Compiler sad-build/sadc (compile .ص to a native binary)
english.CompLSP=LSP Server + Formatter (VS Code / Vim / Neovim / Emacs support)
english.CompPkg=Package Manager sad-pkg (install Sad libraries)
english.CompStdlib=Standard Library (io, math, string, network, graphics, database)
english.CompVSCode=VS Code Extension (syntax highlighting, autocomplete, run support)
english.GroupCore=Core Components
english.GroupTools=Developer Tools
english.GroupIDE=IDE Support

english.AddToPath=Add Sad to PATH (use from anywhere in terminal)
english.AssocExt=Associate .ص files with the Sad compiler
english.DesktopIcon=Create Desktop Shortcut
english.StartMenuIcon=Create Start Menu Group

english.LLVM_Warning=The compiler (sadc) links LLVM statically — nothing to install.
english.VSCode_NotFound=VS Code not found. You can install the extension later from:%nhttps://marketplace.visualstudio.com/items?itemName=sad-lang.sad-language

; LLVM Page
english.LLVMPageTitle=Sad Compiler Setup (LLVM)
english.LLVMPageDesc=The compiler links LLVM statically - nothing to install (page kept as record)
english.LLVMFound=LLVM was automatically detected
english.LLVMNotFound=LLVM was not found on your computer
english.LLVMPathLabel=LLVM installation folder:
english.LLVMBrowse=Browse...
english.LLVMValidOK=✓ Valid LLVM (Version: %1)
english.LLVMValidFail=✗ Path does not contain a valid LLVM installation
english.LLVMVersionOld=⚠ Version %1 is old — Version 14 or later recommended
english.LLVMDownloadBtn=Download LLVM 18 from GitHub
english.LLVMSkip=Skip (install without sadc — LLVM can be added later)
english.LLVMAutoDetect=Auto-detect again
english.LLVMBinsFound=Found binaries: %1
english.LLVMLibsFound=Libraries: present ✓
english.LLVMLibsMissing=Libraries: missing (sadc may not work correctly)
english.LLVMHeadersFound=Headers: present ✓

; ──────────────────────────────────────────────────────────────────────────────
[Types]
Name: "full";     Description: "حزمة كاملة — كل المكونات"
; (AR) 🔑 وُصِفَ «dev» بأنّه «المترجم + أدوات» و«minimal» بأنّه «المركزُ
;      والمترجم» — و`Tools\Compiler` كان `Types: full custom` وحدَهما، فلا
;      يقعُ في أيٍّ منهما. والوصفانِ **كانا صادقَينِ قبلَ حذفِ المفسّر** حينَ
;      كان `sad.exe` هو المحرّك؛ فلمّا صار مركزَ إرسالٍ لا محرّكًا، صارَ
;      «minimal» موزِّعًا بلا شيءٍ يوزِّعُ إليه. فالتصحيحُ في `Types` لا في
;      النصّ: المترجّمُ والمكتبةُ القياسيّةُ يدخلانِ الأدنى، وبهما يعملُ
;      `sad build`.
; (EN) "dev" claimed compiler+tools and "minimal" claimed hub+compiler, while
;      Tools\Compiler was Types: full custom only, so neither selected it.
;      Both descriptions were TRUE before the interpreter was deleted, when
;      sad.exe was itself the engine; once it became a dispatcher, "minimal"
;      installed a dispatcher with nothing to dispatch to. Fixed in Types,
;      not in the text: the compiler and the standard library now belong to
;      the smallest type, which is what makes `sad build` work.
Name: "dev";      Description: "مطوّر — المترجم + أدوات + VS Code"
Name: "minimal";  Description: "أساسي — المركز والمترجم والمكتبة القياسية"
Name: "custom";   Description: "مخصص — اختر بنفسك"; Flags: iscustom

; ──────────────────────────────────────────────────────────────────────────────
; ══════════════════════════════════════════════════════════════════════════════
; (AR) 🔑 **ما يَعِدُ به هذا المُثبِّتُ يجبُ أن يُنسَخَ فعلًا.**
;
;      كان المستخدمُ يؤشِّرُ «بيئة REPL التفاعلية» فيقولُ المُثبِّتُ «تمّ»
;      ولا يُنسَخُ شيء: مصدرُ `sad-repl.exe` يحملُ `skipifsourcedoesntexist`
;      فالكذبةُ صامتة. ومعه اختصارا «مفسر لغة ص (REPL)» في قائمةِ ابدأ
;      وسطحِ المكتب، وتشغيلٌ بعدَ التثبيتِ بـ`sad.exe --repl` — وثلاثتُها
;      تُنادي عَلَمًا لا وجودَ له.
;
;      ⚠️ واقترانُ `.ص` كان `sad.exe "%1"` — والمركزُ **يرفضُ** ملفًّا
;      عاريًا عمدًا (اختبار `HubMain_BareFileRejected` يُثبِّتُ ذلك). فنقرةٌ
;      مزدوجةٌ على ملفِّ مصدرٍ كانت تفتحُ نافذةً تقولُ «أمر غير معروف».
;      وقد رُبِطَ الاقترانُ بـ`sad build` — وهو الفعلُ القائمُ لملفِّ مصدر.
;
;      والمكوّنُ الأساسيُّ أُعيدت تسميتُه `Core\Hub`: `sad.exe`
;      مركزُ أدواتٍ لا مفسّرًا، والاسمُ كان يصفُ محرّكًا محذوفًا.
; (EN) What this installer promises must actually be copied. A user could
;      tick "Interactive REPL Environment", be told it was installed, and
;      receive nothing: the sad-repl.exe source carries
;      skipifsourcedoesntexist, so the lie was silent. With it went two
;      shortcuts and a post-install run, all invoking a flag that no longer
;      exists. The .ص association ran `sad.exe "%1"`, and the hub rejects a
;      bare file BY DESIGN (HubMain_BareFileRejected pins this), so a
;      double-click opened a window saying "unknown command"; it is now
;      bound to `sad build`. The core component is renamed Core\Hub: sad.exe
;      is a tool hub, and the old name described a deleted engine.
; ══════════════════════════════════════════════════════════════════════════════
[Components]
; ──── مجموعة: المكونات الأساسية
Name: "Core";             Description: "{cm:GroupCore}";      Types: full dev minimal custom; Flags: fixed
Name: "Core\Hub"; Description: "{cm:CompHub}"; Types: full dev minimal custom; Flags: fixed
Name: "Core\Stdlib";      Description: "{cm:CompStdlib}";      Types: full dev minimal custom

; ──── مجموعة: أدوات التطوير
Name: "Tools";            Description: "{cm:GroupTools}";     Types: full dev minimal custom
Name: "Tools\Compiler";   Description: "{cm:CompCompiler}";   Types: full dev minimal custom
Name: "Tools\LSP";        Description: "{cm:CompLSP}";        Types: full dev custom
Name: "Tools\Pkg";        Description: "{cm:CompPkg}";        Types: full dev custom

; ──── مجموعة: دعم بيئات التطوير
Name: "IDE";              Description: "{cm:GroupIDE}";       Types: full dev custom
Name: "IDE\VSCode";       Description: "{cm:CompVSCode}";     Types: full dev custom

; ──────────────────────────────────────────────────────────────────────────────
[Tasks]
Name: "addtopath";    Description: "{cm:AddToPath}";    GroupDescription: "إعدادات النظام:"
Name: "assocext";     Description: "{cm:AssocExt}";     GroupDescription: "إعدادات النظام:"
Name: "desktopicon";  Description: "{cm:DesktopIcon}";  GroupDescription: "اختصارات:"; Flags: unchecked
Name: "startmenu";    Description: "{cm:StartMenuIcon}"; GroupDescription: "اختصارات:"

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
Name: "{app}\examples"
Name: "{localappdata}\sad-lang\packages"
Name: "{localappdata}\sad-lang\cache"

; ──────────────────────────────────────────────────────────────────────────────
[Files]
; ──── مركز الأدوات (أساسي — دائماً)
Source: "..\..\build\bin\Release\sad.exe";           DestDir: "{app}\bin"; DestName: "sad.exe";     Flags: ignoreversion; Components: Core\Hub

; ──── المترجم
; (AR) 🔑 كان المصدرُ `sadc.exe` — و**لا يُنتِجُه أيُّ هدفِ بناء** (يقولُها
;      `tools/build/FindSad.cmake` نصًّا). ومعه `skipifsourcedoesntexist`،
;      فالمستخدمُ يؤشِّرُ «المترجم» ويُقالُ له «تمّ» ولا يُنسَخُ شيء — كذبةٌ
;      صامتة. والمبنيُّ اسمُه `sad-build.exe`، ويُنسَخُ **باسمَين**: `sad-build`
;      لأنّ المركزَ يجدُ إخوتَه بمسحِ البادئةِ `sad-` فبه وحدَه يعملُ
;      `sad build`، و`sadc` لأنّ الوثائقَ تَعِدُ به. وهو عينُ ما يفعلُه
;      `scripts/ci/release_tools.sh`. ولا `skipifsourcedoesntexist` بعدَ اليوم.
; (EN) The source was sadc.exe, which NO build target produces (FindSad.cmake
;      says so verbatim) - and with skipifsourcedoesntexist the user ticked
;      "compiler", was told it installed, and got nothing: a silent lie. The
;      built name is sad-build.exe, copied under BOTH names: sad-build so the
;      hub registers it by its sad- prefix and `sad build` works, and sadc
;      because the docs promise it. Same rule as release_tools.sh, and the
;      skip flag is gone.
Source: "..\..\build\bin\Release\sad-build.exe";      DestDir: "{app}\bin"; DestName: "sad-build.exe"; Flags: ignoreversion; Components: Tools\Compiler
Source: "..\..\build\bin\Release\sad-build.exe";      DestDir: "{app}\bin"; DestName: "sadc.exe";      Flags: ignoreversion; Components: Tools\Compiler

; ──── خادم LSP + المنسَّق
; (AR) 🔑 **`sad-check` مُلزَمٌ في كلِّ حزمةٍ ولم يكنْ يُشحَنُ في أيِّ
;      مُثبِّتِ ويندوز.** `scripts/ci/release_tools.sh` يُعلِنُه في
;      `SAD_REQUIRED_STANDARD` — أي «ما يجبُ أن يوجدَ وإلّا سقطَ الشوط» —
;      وحارسُ `check_installer_tool_lists.py` كان يقيسُ `install.sh`
;      و`install.ps1` وحدَهما، فلم يرَ المُثبِّتَين هذين أصلًا. ووُضِعَ
;      تحتَ `Core\Hub` المثبَّتِ (`Flags: fixed`) لأنّه مُلزَمٌ لا اختياريّ.
; (EN) sad-check is hard-required in every package and was shipped by NEITHER
;      Windows installer. release_tools.sh declares it in
;      SAD_REQUIRED_STANDARD, and check_installer_tool_lists.py measured only
;      install.sh and install.ps1, so it never looked at these files. Placed
;      under the fixed Core\Hub component because it is required, not optional.
Source: "..\..\build\bin\Release\sad-check.exe";      DestDir: "{app}\bin"; DestName: "sad-check.exe"; Flags: ignoreversion; Components: Core\Hub
; (AR) 🔑 نُزِعَ `skipifsourcedoesntexist` عن `sad-lsp`: هو مُلزَمٌ في
;      `SAD_REQUIRED_STANDARD`، والعَلَمُ يجعلُ غيابَه صامتًا فتُبنى حزمةٌ
;      ناقصةٌ بلا كلمة. المُلزَمُ يُنسَخُ أو يُخفِقُ البناء.
; (EN) skipifsourcedoesntexist removed from sad-lsp: it is hard-required by
;      SAD_REQUIRED_STANDARD, and the flag made its absence silent, building
;      an incomplete package without a word. Required means copied or fail.
Source: "..\..\build\bin\Release\sad-lsp.exe";       DestDir: "{app}\bin"; DestName: "sad-lsp.exe"; Flags: ignoreversion; Components: Tools\LSP
Source: "..\..\build\bin\Release\sad-fmt.exe";       DestDir: "{app}\bin"; DestName: "sad-fmt.exe"; Flags: ignoreversion skipifsourcedoesntexist; Components: Tools\LSP

; ──── مدير الحزم
Source: "..\..\build\bin\Release\sad-pkg.exe";       DestDir: "{app}\bin"; DestName: "sad-pkg.exe"; Flags: ignoreversion skipifsourcedoesntexist; Components: Tools\Pkg


; ──── المكتبة القياسية
Source: "..\..\stdlib\core\*";       DestDir: "{app}\stdlib\core";       Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Stdlib
Source: "..\..\stdlib\io\*";         DestDir: "{app}\stdlib\io";         Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Stdlib
Source: "..\..\stdlib\math\*";       DestDir: "{app}\stdlib\math";       Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Stdlib
Source: "..\..\stdlib\string\*";     DestDir: "{app}\stdlib\string";     Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Stdlib
Source: "..\..\stdlib\network\*";    DestDir: "{app}\stdlib\network";    Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Stdlib
Source: "..\..\stdlib\database\*";   DestDir: "{app}\stdlib\database";   Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Stdlib
Source: "..\..\stdlib\http\*";       DestDir: "{app}\stdlib\http";       Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Stdlib
Source: "..\..\stdlib\json\*";       DestDir: "{app}\stdlib\json";       Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Stdlib
Source: "..\..\stdlib\filesystem\*"; DestDir: "{app}\stdlib\filesystem"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Stdlib
Source: "..\..\stdlib\system\*";     DestDir: "{app}\stdlib\system";     Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Stdlib
Source: "..\..\stdlib\ui\*";         DestDir: "{app}\stdlib\ui";         Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Stdlib

; ──── إضافة VS Code
Source: "..\..\tools\vscode-extension\sad-language-2.0.0.vsix"; DestDir: "{tmp}"; Flags: ignoreversion deleteafterinstall; Components: IDE\VSCode

; ──── الأمثلة
Source: "..\..\examples\*"; DestDir: "{app}\examples"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist; Components: Core\Hub

; ──── الأيقونة
Source: "..\assets\sad_icon.ico"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist

; ──── ملف الترخيص
Source: "..\..\LICENSE"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist

; ──── ملف الإعداد الافتراضي
Source: "..\assets\sad.config.json"; DestDir: "{app}"; Flags: ignoreversion skipifsourcedoesntexist onlyifdoesntexist

; ──────────────────────────────────────────────────────────────────────────────
[Icons]
; قائمة ابدأ
Name: "{group}\مدير الحزم sad-pkg";   Filename: "{app}\bin\sad-pkg.exe"; WorkingDir: "{userdocs}"; IconFilename: "{app}\sad_icon.ico"; Tasks: startmenu; Components: Tools\Pkg
; (AR) ⚠️ حُذف اختصارُ «التوثيق»: لا سطرَ `[Files]` واحدٌ ينسخُ إلى
;      `{app}\docs`، فالمجلَّدُ يُنشَأُ فارغًا و`index.html` لا يوجدُ أصلًا.
;      فنقرةُ الاختصارِ تفتحُ نافذةَ «الملف غير موجود».
; (EN) The "documentation" shortcut was removed: not one [Files] line copies
;      anything into {app}\docs, so the folder is created empty and
;      index.html never exists - the shortcut opened a file-not-found dialog.
Name: "{group}\أمثلة لغة ص";         Filename: "{app}\examples"; Tasks: startmenu; Components: Core\Hub
Name: "{group}\إزالة تثبيت لغة ص";   Filename: "{uninstallexe}"; Tasks: startmenu

; سطح المكتب
Name: "{autodesktop}\لغة ص";          Filename: "{app}\bin\sad.exe"; Parameters: "--help"; IconFilename: "{app}\sad_icon.ico"; Tasks: desktopicon; Components: Core\Hub

; ──────────────────────────────────────────────────────────────────────────────
[Registry]
; ──── امتداد .ص
Root: HKCR; Subkey: ".ص";                         ValueType: string; ValueName: ""; ValueData: "SadSourceFile";          Flags: uninsdeletevalue; Tasks: assocext
Root: HKCR; Subkey: ".sad";                        ValueType: string; ValueName: ""; ValueData: "SadSourceFile";          Flags: uninsdeletevalue; Tasks: assocext
Root: HKCR; Subkey: "SadSourceFile";               ValueType: string; ValueName: ""; ValueData: "ملف لغة ص";              Flags: uninsdeletekey; Tasks: assocext
Root: HKCR; Subkey: "SadSourceFile\DefaultIcon";   ValueType: string; ValueName: ""; ValueData: "{app}\sad_icon.ico,0";   Tasks: assocext
Root: HKCR; Subkey: "SadSourceFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\bin\sad.exe"" ""build"" ""%1"""; Tasks: assocext

; ──── PATH (كل المستخدمين — يتطلب صلاحية admin)
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "SAD_HOME"; ValueData: "{app}"; Flags: uninsdeletevalue; Tasks: addtopath
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "SAD_STDLIB"; ValueData: "{app}\stdlib"; Flags: uninsdeletevalue; Tasks: addtopath

; ──── معلومات التثبيت
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "InstallPath"; ValueData: "{app}"; Flags: uninsdeletekey
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "Version"; ValueData: "{#MyAppVersion}"
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "Publisher"; ValueData: "{#MyAppPublisher}"
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "Contact"; ValueData: "{#MyAppContact}"
Root: HKLM; Subkey: "SOFTWARE\SadLang"; ValueType: string; ValueName: "URLInfoAbout"; ValueData: "{#MyAppURL}"

; ──────────────────────────────────────────────────────────────────────────────
[Run]
; ──── تثبيت إضافة VS Code
Filename: "{code:GetVSCodePath}"; Parameters: "--install-extension ""{tmp}\sad-language-2.0.0.vsix"" --force"; Flags: nowait skipifsilent; Components: IDE\VSCode; StatusMsg: "جاري تثبيت إضافة VS Code..."; Check: VSCodeExists

; ──── إشعار النظام بتغيير PATH
Filename: "{sys}\cmd.exe"; Parameters: "/C setx SAD_HOME ""{app}"" && setx SAD_STDLIB ""{app}\stdlib"""; Flags: runhidden; Tasks: addtopath

; ──── بعد التثبيت — عرض الأوامر المتاحة
Filename: "{app}\bin\sad.exe"; Parameters: "--help"; Description: "{cm:LaunchSad}"; Flags: nowait postinstall skipifsilent unchecked; Components: Core\Hub
; (AR) وكذلك «فتح التوثيق» بعدَ التثبيت — نفسُ الملفِّ غيرِ الموجود.
; (EN) The post-install "open documentation" action went to the same file.

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

  { صفحة LLVM المخصصة }
  LLVMPage: TWizardPage;
  LLVMPathEdit: TEdit;
  LLVMStatusLabel: TLabel;
  LLVMBrowseBtn: TButton;
  LLVMValidateBtn: TButton;
  LLVMDownloadBtn: TButton;
  LLVMSkipChk: TCheckBox;
  LLVMDetectedPath: String;
  LLVMIsValid: Boolean;
  LLVMDetectedVersion: String;

{ ═══════════════════════════════════════════════════════════════════════════
  دوال الكشف عن LLVM
  ═══════════════════════════════════════════════════════════════════════════ }

{ ── دالة مساعدة: هل الملف موجود؟ ── }
function FileExists2(const Path: String): Boolean;
begin
  Result := FileExists(Path);
end;

{ ── استخراج إصدار LLVM من llvm-config أو clang ── }
function GetLLVMVersion(const BinDir: String): String;
var
  LLVMConfigPath, ClangPath, TmpFile, Output: String;
  Lines: TArrayOfString;
  ExitCode: Integer;
begin
  Result := '';
  LLVMConfigPath := BinDir + '\llvm-config.exe';
  ClangPath      := BinDir + '\clang.exe';
  TmpFile        := ExpandConstant('{tmp}\llvm_ver.txt');

  if FileExists(LLVMConfigPath) then begin
    { نستخدم Exec مع إعادة التوجيه إلى ملف }
    if Exec('cmd.exe', '/C "' + LLVMConfigPath + '" --version > "' + TmpFile + '" 2>&1',
            '', SW_HIDE, ewWaitUntilTerminated, ExitCode) then begin
      if LoadStringsFromFile(TmpFile, Lines) and (GetArrayLength(Lines) > 0) then
        Result := Trim(Lines[0]);
    end;
  end else if FileExists(ClangPath) then begin
    if Exec('cmd.exe', '/C "' + ClangPath + '" --version > "' + TmpFile + '" 2>&1',
            '', SW_HIDE, ewWaitUntilTerminated, ExitCode) then begin
      if LoadStringsFromFile(TmpFile, Lines) and (GetArrayLength(Lines) > 0) then begin
        Output := Lines[0];
        { استخراج الرقم مثل "18.1.8" }
        if Pos('version ', LowerCase(Output)) > 0 then begin
          Result := Copy(Output, Pos('version ', LowerCase(Output)) + 8, 20);
          { اقتطع عند أول مسافة }
          if Pos(' ', Result) > 0 then
            Result := Copy(Result, 1, Pos(' ', Result) - 1);
        end;
      end;
    end;
  end;
  DeleteFile(TmpFile);
end;

{ ── الحصول على الإصدار الرئيسي ── }
function GetMajorVersion(const Ver: String): Integer;
var
  DotPos: Integer;
  Major: String;
begin
  Result := 0;
  DotPos := Pos('.', Ver);
  if DotPos > 0 then
    Major := Copy(Ver, 1, DotPos - 1)
  else
    Major := Ver;
  if Major <> '' then
    Result := StrToIntDef(Major, 0);
end;

{ ── التحقق الكامل من مجلد LLVM ── }
function ValidateLLVMDirectory(const Dir: String; var Version: String; var StatusMsg: String): Boolean;
var
  BinDir: String;
  FoundBins: String;
  HasClang, HasLLC, HasOpt, HasAr, HasConfig: Boolean;
  BinCount: Integer;
  MajorVer: Integer;
begin
  Result   := False;
  Version  := '';
  StatusMsg := '';

  if not DirExists(Dir) then begin
    StatusMsg := CustomMessage('LLVMValidFail') + ' (المجلد غير موجود)';
    Exit;
  end;

  BinDir   := Dir + '\bin';
  FoundBins := '';
  BinCount  := 0;

  HasClang  := FileExists(BinDir + '\clang.exe');
  HasLLC    := FileExists(BinDir + '\llc.exe');
  HasOpt    := FileExists(BinDir + '\opt.exe');
  HasAr     := FileExists(BinDir + '\llvm-ar.exe');
  HasConfig := FileExists(BinDir + '\llvm-config.exe');

  if HasClang  then begin FoundBins := FoundBins + 'clang ';   Inc(BinCount); end;
  if HasLLC    then begin FoundBins := FoundBins + 'llc ';     Inc(BinCount); end;
  if HasOpt    then begin FoundBins := FoundBins + 'opt ';     Inc(BinCount); end;
  if HasAr     then begin FoundBins := FoundBins + 'llvm-ar '; Inc(BinCount); end;
  if HasConfig then begin FoundBins := FoundBins + 'llvm-config'; Inc(BinCount); end;

  if BinCount < 2 then begin
    StatusMsg := CustomMessage('LLVMValidFail') + ' (ملفات تنفيذية غير كافية)';
    Exit;
  end;

  { استخراج الإصدار }
  Version := GetLLVMVersion(BinDir);
  if Version = '' then Version := 'unknown';

  { فحص الإصدار }
  MajorVer := GetMajorVersion(Version);
  if (MajorVer > 0) and (MajorVer < 14) then begin
    StatusMsg := FmtMessage(CustomMessage('LLVMVersionOld'), [Version]);
    Exit;
  end;

  { نجح الفحص }
  Result := True;
  StatusMsg := FmtMessage(CustomMessage('LLVMValidOK'), [Version]) + #13#10 +
               FmtMessage(CustomMessage('LLVMBinsFound'), [Trim(FoundBins)]);

  if DirExists(Dir + '\lib') then
    StatusMsg := StatusMsg + #13#10 + CustomMessage('LLVMLibsFound');
  if DirExists(Dir + '\include\llvm') then
    StatusMsg := StatusMsg + #13#10 + CustomMessage('LLVMHeadersFound');
end;

{ ── البحث التلقائي عن LLVM ── }
function AutoDetectLLVM(var FoundPath: String): Boolean;
var
  RegPath, RegVal: String;
  Paths: TArrayOfString;
  I: Integer;
  Ver, Msg: String;
begin
  Result    := False;
  FoundPath := '';

  { البحث في السجل أولاً }
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\LLVM\LLVM', '', RegVal) then begin
    if DirExists(RegVal) and ValidateLLVMDirectory(RegVal, Ver, Msg) then begin
      FoundPath := RegVal;
      Result := True;
      Exit;
    end;
  end;
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\WOW6432Node\LLVM\LLVM', '', RegVal) then begin
    if DirExists(RegVal) and ValidateLLVMDirectory(RegVal, Ver, Msg) then begin
      FoundPath := RegVal;
      Result := True;
      Exit;
    end;
  end;

  { البحث في مسارات شائعة }
  SetArrayLength(Paths, 12);
  Paths[0]  := 'C:\Program Files\LLVM';
  Paths[1]  := 'C:\Program Files\LLVM-18';
  Paths[2]  := 'C:\LLVM';
  Paths[3]  := 'C:\LLVM-18';
  Paths[4]  := 'C:\LLVM-Dev';
  Paths[5]  := 'C:\tools\LLVM';
  Paths[6]  := 'C:\msys64\mingw64';
  Paths[7]  := 'C:\msys64\ucrt64';
  Paths[8]  := 'C:\msys64\clang64';
  Paths[9]  := ExpandConstant('{localappdata}\Programs\LLVM');
  Paths[10] := ExpandConstant('{localappdata}\scoop\apps\llvm\current');
  Paths[11] := 'D:\Program Files\LLVM';

  for I := 0 to GetArrayLength(Paths) - 1 do begin
    if DirExists(Paths[I]) and ValidateLLVMDirectory(Paths[I], Ver, Msg) then begin
      FoundPath := Paths[I];
      Result := True;
      Exit;
    end;
  end;
end;

{ ═══════════════════════════════════════════════════════════════════════════
  إنشاء صفحة LLVM المخصصة
  ═══════════════════════════════════════════════════════════════════════════ }

procedure LLVMBrowseClick(Sender: TObject);
var
  Dir: String;
begin
  Dir := LLVMPathEdit.Text;
  if Dir = '' then Dir := 'C:\Program Files';
  if BrowseForFolder(CustomMessage('LLVMPageDesc'), Dir, False) then begin
    LLVMPathEdit.Text := Dir;
    { التحقق الفوري }
    LLVMValidateBtn.OnClick(LLVMValidateBtn);
  end;
end;

procedure LLVMValidateClick(Sender: TObject);
var
  Dir, Ver, Msg: String;
begin
  Dir := LLVMPathEdit.Text;
  if Dir = '' then begin
    LLVMStatusLabel.Caption  := 'أدخل مسار مجلد LLVM أولاً';
    LLVMStatusLabel.Font.Color := clRed;
    LLVMIsValid := False;
    Exit;
  end;

  LLVMStatusLabel.Caption   := '...جاري الفحص';
  LLVMStatusLabel.Font.Color := clBlue;

  if ValidateLLVMDirectory(Dir, Ver, Msg) then begin
    LLVMIsValid          := True;
    LLVMDetectedVersion  := Ver;
    LLVMDetectedPath     := Dir;
    LLVMStatusLabel.Caption   := Msg;
    LLVMStatusLabel.Font.Color := clGreen;
    LLVMSkipChk.Checked  := False;
  end else begin
    LLVMIsValid := False;
    LLVMStatusLabel.Caption   := Msg;
    LLVMStatusLabel.Font.Color := clRed;
  end;
end;

procedure LLVMDownloadClick(Sender: TObject);
var
  ErrCode: Integer;
begin
  ShellExec('open', 'https://github.com/llvm/llvm-project/releases/tag/llvmorg-18.1.8', '', '', SW_SHOWNORMAL, ewNoWait, ErrCode);
end;

procedure LLVMAutoDetectClick(Sender: TObject);
var
  FoundPath, Ver, Msg: String;
begin
  LLVMStatusLabel.Caption   := '...جاري الكشف التلقائي';
  LLVMStatusLabel.Font.Color := clBlue;

  if AutoDetectLLVM(FoundPath) then begin
    LLVMPathEdit.Text := FoundPath;
    ValidateLLVMDirectory(FoundPath, Ver, Msg);
    LLVMIsValid         := True;
    LLVMDetectedVersion := Ver;
    LLVMDetectedPath    := FoundPath;
    LLVMStatusLabel.Caption    := FmtMessage(CustomMessage('LLVMFound') + ': %1', [FoundPath]) + #13#10 + Msg;
    LLVMStatusLabel.Font.Color := clGreen;
    LLVMSkipChk.Checked := False;
  end else begin
    LLVMStatusLabel.Caption    := CustomMessage('LLVMNotFound');
    LLVMStatusLabel.Font.Color := clMaroon;
  end;
end;

procedure CreateLLVMPage;
var
  TitleLabel, DescLabel, PathLabel: TLabel;
  AutoDetectBtn: TButton;
  Y: Integer;
begin
  LLVMPage := CreateCustomPage(wpSelectComponents,
    CustomMessage('LLVMPageTitle'),
    CustomMessage('LLVMPageDesc'));

  Y := 8;

  { زر الكشف التلقائي }
  AutoDetectBtn := TButton.Create(LLVMPage);
  AutoDetectBtn.Parent  := LLVMPage.Surface;
  AutoDetectBtn.Caption := CustomMessage('LLVMAutoDetect');
  AutoDetectBtn.Left    := 0;
  AutoDetectBtn.Top     := Y;
  AutoDetectBtn.Width   := 180;
  AutoDetectBtn.Height  := 26;
  AutoDetectBtn.OnClick := @LLVMAutoDetectClick;

  Y := Y + 36;

  { تسمية حقل المسار }
  PathLabel := TLabel.Create(LLVMPage);
  PathLabel.Parent  := LLVMPage.Surface;
  PathLabel.Caption := CustomMessage('LLVMPathLabel');
  PathLabel.Left    := 0;
  PathLabel.Top     := Y;
  PathLabel.Width   := 400;
  Y := Y + 18;

  { حقل إدخال المسار }
  LLVMPathEdit := TEdit.Create(LLVMPage);
  LLVMPathEdit.Parent := LLVMPage.Surface;
  LLVMPathEdit.Left   := 0;
  LLVMPathEdit.Top    := Y;
  LLVMPathEdit.Width  := 330;
  LLVMPathEdit.Height := 22;
  LLVMPathEdit.Text   := '';

  { زر استعراض }
  LLVMBrowseBtn := TButton.Create(LLVMPage);
  LLVMBrowseBtn.Parent  := LLVMPage.Surface;
  LLVMBrowseBtn.Caption := CustomMessage('LLVMBrowse');
  LLVMBrowseBtn.Left    := 338;
  LLVMBrowseBtn.Top     := Y - 1;
  LLVMBrowseBtn.Width   := 80;
  LLVMBrowseBtn.Height  := 24;
  LLVMBrowseBtn.OnClick := @LLVMBrowseClick;

  Y := Y + 32;

  { زر التحقق }
  LLVMValidateBtn := TButton.Create(LLVMPage);
  LLVMValidateBtn.Parent  := LLVMPage.Surface;
  LLVMValidateBtn.Caption := '← تحقق من المسار / Validate';
  LLVMValidateBtn.Left    := 0;
  LLVMValidateBtn.Top     := Y;
  LLVMValidateBtn.Width   := 180;
  LLVMValidateBtn.Height  := 26;
  LLVMValidateBtn.OnClick := @LLVMValidateClick;

  Y := Y + 36;

  { تسمية الحالة }
  LLVMStatusLabel := TLabel.Create(LLVMPage);
  LLVMStatusLabel.Parent    := LLVMPage.Surface;
  LLVMStatusLabel.Caption   := '';
  LLVMStatusLabel.Left      := 0;
  LLVMStatusLabel.Top       := Y;
  LLVMStatusLabel.Width     := 420;
  LLVMStatusLabel.Height    := 60;
  LLVMStatusLabel.WordWrap  := True;
  LLVMStatusLabel.Font.Size := 9;

  Y := Y + 70;

  { زر تحميل LLVM }
  LLVMDownloadBtn := TButton.Create(LLVMPage);
  LLVMDownloadBtn.Parent  := LLVMPage.Surface;
  LLVMDownloadBtn.Caption := CustomMessage('LLVMDownloadBtn');
  LLVMDownloadBtn.Left    := 0;
  LLVMDownloadBtn.Top     := Y;
  LLVMDownloadBtn.Width   := 250;
  LLVMDownloadBtn.Height  := 26;
  LLVMDownloadBtn.Font.Color := clBlue;
  LLVMDownloadBtn.OnClick := @LLVMDownloadClick;

  Y := Y + 36;

  { مربع تخطي LLVM }
  LLVMSkipChk := TCheckBox.Create(LLVMPage);
  LLVMSkipChk.Parent  := LLVMPage.Surface;
  LLVMSkipChk.Caption := CustomMessage('LLVMSkip');
  LLVMSkipChk.Left    := 0;
  LLVMSkipChk.Top     := Y;
  LLVMSkipChk.Width   := 420;
  LLVMSkipChk.Height  := 20;
  LLVMSkipChk.Checked := False;
end;

{ ── البحث عن VS Code ── }
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

{ ═══════════════════════════════════════════════
  أحداث المثبّت
  ═══════════════════════════════════════════════ }

{ ── الإعداد الأولي ── }
procedure InitializeWizard();
var
  FoundPath: String;
begin
  LLVMWarningShown := False;
  VSCodePath       := FindVSCode();
  LLVMIsValid      := False;
  LLVMDetectedVersion := '';
  LLVMDetectedPath := '';

  { إنشاء صفحة LLVM }
  CreateLLVMPage;

  { الكشف التلقائي عند البداية }
  if AutoDetectLLVM(FoundPath) then begin
    LLVMPathEdit.Text   := FoundPath;
    LLVMIsValid         := True;
    LLVMDetectedPath    := FoundPath;
    { استخراج الإصدار وتحديث واجهة الصفحة }
    LLVMAutoDetectClick(nil);
  end;
end;

{ ── إخفاء أو إظهار صفحة LLVM حسب الاختيار ── }
function ShouldSkipPage(PageID: Integer): Boolean;
begin
  Result := False;
  { تخطي صفحة LLVM إذا لم يختر المستخدم المترجم }
  if PageID = LLVMPage.ID then begin
    { (AR) 🔑 صفحةُ LLVM تُتخطّى دائمًا. المترجّمُ يربطُ LLVM **ساكنًا**
      (cmake/llvm.cmake: "sad-build.exe imports no LLVM DLL")، فلا شيءَ
      يُثبَّت. وكانت الصفحةُ تطلبُ مسارَ LLVM ثمّ **تمنعُ المتابعةَ** برسالةٍ
      تعرضُ «التثبيت بدون sadc» — أي تحرمُ المستخدمَ من مترجّمٍ يعمل. }
    { (EN) The LLVM page is always skipped: the compiler links LLVM
      statically (cmake/llvm.cmake: "sad-build.exe imports no LLVM DLL"), so
      there is nothing to install. The page asked for an LLVM path and then
      BLOCKED the wizard, offering to install without sadc - denying the user
      a compiler that works. }
    Result := True;
  end;
end;

{ ── ما يحدث عند اكتمال التثبيت ── }
procedure CurStepChanged(CurStep: TSetupStep);
var
  LLVMBin: String;
begin
  if CurStep = ssPostInstall then begin
    { إضافة bin لـ PATH }
    if WizardIsTaskSelected('addtopath') then begin
      AddToPath(ExpandConstant('{app}\bin'));
    end;
    { حفظ مسار LLVM في متغيرات البيئة إذا تم التحقق منه }
    if LLVMIsValid and (LLVMDetectedPath <> '') then begin
      RegWriteExpandStringValue(HKEY_LOCAL_MACHINE,
        'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
        'LLVM_DIR', LLVMDetectedPath);
      LLVMBin := LLVMDetectedPath + '\bin';
      AddToPath(LLVMBin);
      { تسجيل المسار في السجل }
      RegWriteStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\SadLang', 'LLVMPath', LLVMDetectedPath);
      RegWriteStringValue(HKEY_LOCAL_MACHINE, 'SOFTWARE\SadLang', 'LLVMVersion', LLVMDetectedVersion);
    end;
    { إخطار مدير النوافذ بتغيير البيئة }
    SendBroadcastMessage($001A, 0, '');
  end;
end;

{ ── التحقق عند الانتقال إلى الصفحة التالية ── }
function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;

  { صفحة LLVM — تحقق قبل المتابعة }
  if CurPageID = LLVMPage.ID then begin
    if WizardIsComponentSelected('Tools\Compiler') then begin
      if (not LLVMIsValid) and (not LLVMSkipChk.Checked) then begin
        if LLVMPathEdit.Text <> '' then begin
          { حاول التحقق تلقائياً }
          LLVMValidateClick(nil);
          if not LLVMIsValid then begin
            MsgBox('المسار المدخل لا يحتوي على LLVM صالح.' + #13#10 +
                   'الرجاء تصحيح المسار، أو تفعيل خيار "تخطي" لتثبيت sad بدون sadc.',
                   mbError, MB_OK);
            Result := False;
            Exit;
          end;
        end else if not LLVMSkipChk.Checked then begin
          { لم يُدخل مساراً ولم يختر تخطي }
          if MsgBox('لم تحدد مسار LLVM ولم تختر "تخطي".' + #13#10 +
                    'هل تريد المتابعة بدون sadc؟', mbConfirmation, MB_YESNO) = IDNO then begin
            Result := False;
            Exit;
          end else begin
            LLVMSkipChk.Checked := True;
          end;
        end;
      end;
    end;
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
    { إزالة LLVM_DIR من البيئة إذا كان مضافاً بواسطة المثبت }
    RegDeleteValue(HKEY_LOCAL_MACHINE,
      'SYSTEM\CurrentControlSet\Control\Session Manager\Environment',
      'LLVM_DIR');
    SendBroadcastMessage($001A, 0, '');
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
