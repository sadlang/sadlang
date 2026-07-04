#Requires -Version 5.1
# ============================================================================
# (AR) مُعِين تحميل بيئة بناء MSVC إلى عمليّة PowerShell الحاليّة.
#      يستورد LIB / INCLUDE / PATH وسائرَ متغيّرات vcvars64.bat **بأمان** عبر أمر
#      `set` (لا عبر `echo %LIB%`). هذا هو جوهر الإصلاح:
#
#      الفخّ (batch):  echo %LIB% >lib.txt
#        ⇐ cmd يُدرج المسافة التي قبل «>» ضمن المُخرَج، فيصير آخرُ مقطعٍ من المسار
#          `…\um\x64 ` (بمسافة زائدة لاحقة) فيفشل الربط (lld-link/clang) بمسار تالف.
#
#      الحلّ:  `set` يطبع كلّ متغيّر بصيغة NAME=VALUE سطرًا سطرًا **بلا أيّ مسافة
#              زائدة**، فنلتقطه مباشرةً إلى بيئة العمليّة دون وسيط ملفّ ولا `echo`.
# ----------------------------------------------------------------------------
# (EN) Load the MSVC build environment into the current PowerShell process.
#      Imports LIB / INCLUDE / PATH and every other vcvars64.bat variable **safely**
#      via the `set` command (never `echo %LIB%`). That is the core of the fix:
#
#      The trap (batch):  echo %LIB% >lib.txt
#        ⇐ cmd includes the space *before* '>' in the output, so the final path
#          segment becomes `…\um\x64 ` (with a trailing space) and the linker
#          (lld-link/clang) fails on a corrupted path.
#
#      The fix:  `set` prints each variable as NAME=VALUE, one per line, with **no
#                trailing space**, captured straight into the process env — no file,
#                no echo.
# ----------------------------------------------------------------------------
# (AR) الاستعمال / (EN) Usage — يجب أن يكون في **نفس عمليّة** PowerShell (متغيّرات
#      البيئة على مستوى العمليّة تبقى بعد انتهاء السكربت؛ استدعاؤه عبر `-File` في
#      عمليّة جديدة لا يفيد المُستدعي):
#   . scripts/msvc_env.ps1          # أو:  & scripts/msvc_env.ps1
#   & scripts/msvc_env.ps1 -Quiet   # بلا رسالة تأكيد / no confirmation line
#
# (AR) أداةُ راحةٍ يدويّة — غير مربوطة بـ`x.py` ولا بالـCI. شغّلها قبل `sad-build`/الربط
#      اليدويّ حين تحتاج بيئة MSVC في جلسة PowerShell. البناء الآليّ يستعمل CMake مباشرةً.
# (EN) A manual convenience tool — not wired into `x.py` or CI. Run it before a manual
#      `sad-build`/link when you need the MSVC env in a PowerShell session. Automated
#      builds invoke CMake directly.
# ============================================================================

[CmdletBinding()]
param([switch]$Quiet)

# (AR) الجسم داخل كتلة معزولة `& { }` — فلا يتسرّب أيّ متغيّر محلّيّ ولا تفضيل
#      ($ErrorActionPreference) إلى جلسة المُستدعي عند dot-source. تغييرات `env:`
#      وحدها تبقى لأنّها على مستوى العمليّة.
# (EN) Body runs in an isolated `& { }` block so no local variable or preference
#      ($ErrorActionPreference) leaks into the caller's session when dot-sourced.
#      Only the `env:` changes persist — they are process-scoped.
& {
    param([bool]$Quiet)
    $ErrorActionPreference = 'Stop'

    # (AR) مسارات ومكوّنات ثابتة مسمّاة — لا سلاسل حرفيّة متناثرة.
    # (EN) Named path/component constants — no scattered string literals.
    $VsWhereRelative  = 'Microsoft Visual Studio\Installer\vswhere.exe'
    $VcvarsRelative   = 'VC\Auxiliary\Build\vcvars64.bat'
    $VcToolsComponent = 'Microsoft.VisualStudio.Component.VC.Tools.x86.x64'
    $PathSeparator    = ';'

    # (AR) أداة vswhere القياسيّة تحدّد مسار تثبيت Visual Studio دون ترميزٍ صلب للإصدار
    #      (بخلاف المسار المُثبَّت في build_cov.bat الذي يكسره ترقيةُ VS).
    # (EN) The standard vswhere tool locates the VS install without hard-coding a
    #      version (unlike the pinned path in build_cov.bat that a VS upgrade breaks).
    $vsWhere = Join-Path ${env:ProgramFiles(x86)} $VsWhereRelative
    if (-not (Test-Path $vsWhere)) {
        throw "vswhere.exe غير موجود في `"$vsWhere`" — تأكّد من تثبيت Visual Studio أو Build Tools."
    }

    # (AR) `-products *` يشمل «أدوات البناء» (Build Tools) لا Visual Studio الكامل فقط،
    #      و`-requires` يضمن أنّ التثبيت المُختار يملك مجموعة أدوات C++ فعلًا (خوادم CI).
    # (EN) `-products *` includes Build Tools (not only full VS), and `-requires`
    #      ensures the chosen install actually has the C++ toolset (CI build servers).
    $vsPath = (& $vsWhere -latest -products * -requires $VcToolsComponent `
                   -property installationPath | Select-Object -First 1)
    if (-not $vsPath) {
        throw 'تعذّر العثور على تثبيت Visual Studio بمجموعة أدوات C++ عبر vswhere.'
    }

    $vcvars = Join-Path $vsPath.Trim() $VcvarsRelative
    if (-not (Test-Path $vcvars)) {
        throw "vcvars64.bat غير موجود في `"$vcvars`"."
    }

    # (AR) تحصين حقن الأوامر: المسار يُحقَن نصّيًّا في سلسلة `cmd /c`. مصدره vswhere موثوق،
    #      لكن محرفًا خاصًّا بـcmd («&» «"» «^» «<» «>» «|» «%») في المسار يكسر السلسلة أو
    #      يُغيّر دلالتها ⇒ نرفض صراحةً بدل تمرير أمر مشوَّه.
    # (EN) Command-injection hardening: the path is interpolated into a `cmd /c` string.
    #      Its vswhere source is trusted, but a cmd-special char (& " ^ < > | %) in the
    #      path would break/alter the command ⇒ reject explicitly instead of passing it on.
    if ($vcvars -match '[&"^<>|%]') {
        throw "مسار vcvars64.bat يحوي محرفًا خاصًّا بـcmd يمنع تحميله بأمان: `"$vcvars`"."
    }

    # (AR) المفتاح: التقاط بيئة الصدفة **بعد** استدعاء vcvars عبر `set`.
    #      نكتم شعار vcvars بـ`>nul 2>&1` ليبقى خرج `set` نقيًّا (NAME=VALUE فقط).
    # (EN) Key: capture the shell env **after** vcvars via `set`.
    #      Suppress the vcvars banner with `>nul 2>&1` so `set` output stays pure.
    $capture = cmd /c "call `"$vcvars`" >nul 2>&1 && set"

    # (AR) فحص الفشل الصامت: لو أخفق vcvars يقصر `&&` فلا يُنفَّذ `set` ويبقى الخرج
    #      فارغًا. النداءات الأصليّة لا ترمي رغم Stop ⇒ نفحص الرمز يدويًّا ونُظهر السبب.
    # (EN) Guard the silent failure: if vcvars fails, `&&` short-circuits, `set` never
    #      runs, output stays empty. Native calls don't throw despite Stop ⇒ check the
    #      code manually and surface the reason.
    if ($LASTEXITCODE -ne 0) {
        $diag = cmd /c "call `"$vcvars`" 2>&1 1>nul"
        throw ("فشل تحميل بيئة vcvars64 (رمز الخروج $LASTEXITCODE).`n" +
               ($diag -join [Environment]::NewLine))
    }

    # (AR) نسخ كلّ NAME=VALUE إلى بيئة العمليّة عبر SetEnvironmentVariable — أمتن من
    #      `Set-Item env:` مع الأسماء ذات المحارف الخاصّة مثل `ProgramFiles(x86)`.
    #      التقسيم على أوّل `=` يحافظ على القيم التي تحوي `=` داخلها.
    # (EN) Copy each NAME=VALUE into the process env via SetEnvironmentVariable —
    #      sturdier than `Set-Item env:` for names with special chars like
    #      `ProgramFiles(x86)`. Splitting on the first `=` preserves values with `=`.
    $imported = 0
    foreach ($line in $capture) {
        if ($line -match '^(?<name>[^=]+)=(?<value>.*)$') {
            [Environment]::SetEnvironmentVariable($matches['name'], $matches['value'], 'Process')
            $imported++
        }
    }

    # (AR) تحقّق أنّ الاستيراد وقع فعلًا وأنّ LIB مضبوط — وإلّا فبيئة فارغة/جزئيّة.
    # (EN) Verify the import actually happened and LIB is set — else empty/partial env.
    if ($imported -eq 0 -or [string]::IsNullOrEmpty($env:LIB)) {
        throw 'لم تُستورَد بيئة MSVC (LIB غير مضبوط) — تحقّق من تثبيت أدوات C++ (VC.Tools).'
    }

    if (-not $Quiet) {
        $libPaths = @($env:LIB -split $PathSeparator | Where-Object { $_ })
        # (AR) تحقّق فعليّ من غياب المسافة اللاحقة (لبّ الفخّ) قبل الادّعاء.
        # (EN) Actually verify the absence of a trailing space (the crux) before claiming it.
        $dirty = @($libPaths | Where-Object { $_ -match '\s$' })
        if ($dirty.Count -gt 0) {
            Write-Warning ("مسارات LIB تحوي مسافة لاحقة: " + ($dirty -join ' | '))
        }
        $suffix = if ($dirty.Count -eq 0) { ' — بلا مسافة زائدة.' } else { ' — [!] فيها مسافة لاحقة!' }
        Write-Host ("بيئة MSVC مُحمّلة: LIB يحوي $($libPaths.Count) مسارًا" + $suffix)
        Write-Host "المصدر / source: $vcvars"
    }
} ([bool]$Quiet)
