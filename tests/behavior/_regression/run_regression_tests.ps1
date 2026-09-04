<#
.SYNOPSIS
    Regression Tests Runner for Sad Language
.DESCRIPTION
    Runs every regression test found beside this script, against the compiler
.PARAMETER SadExe
    Path to executable (sad.exe or sadc.exe)
.PARAMETER Mode
    Run mode: compiler (the only engine)
.EXAMPLE
    .\run_regression_tests.ps1 -SadExe .\build\bin\Release\sad-build.exe -Mode compiler
#>

param(
    [Parameter(Mandatory=$true)]
    [string]$SadExe,
    
    [Parameter(Mandatory=$false)]
    # (AR) 🔑 كان الافتراضيُّ `interpreter` و`ValidateSet` يقبلُه، فمن نسيَ
    #      `-Mode` شغَّلَ فرعَ المفسّرِ على `sad-build`. صار `compiler` وحدَه،
    #      و`ValidateSet` يردُّ الاسمَ القديمَ برسالةٍ صريحةٍ لا بسلوكٍ خاطئ.
    #      وحُذف `-InterpExe`: كان يقيسُ تكافؤَ محرّكَين، وأحدُهما زال. ولم
    #      يُمرَّرْ من أيِّ موضعٍ في المستودعِ قطُّ (CI يُمرِّرُ `-Mode compiler` فقط).
    # (EN) The default was interpreter and ValidateSet accepted it, so omitting
    #      -Mode ran the interpreter branch against sad-build. compiler is now
    #      the only value, and ValidateSet rejects the old name with an explicit
    #      message instead of misbehaving. -InterpExe was removed: it measured
    #      parity between two engines, one of which is gone, and it was never
    #      passed anywhere in the repository (CI passes -Mode compiler only).
    [ValidateSet("compiler")]
    [string]$Mode = "compiler"
)

# ======================================================================
# Configuration
# ======================================================================
$ErrorActionPreference = "Continue"
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8
$ScriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$TestFiles = Get-ChildItem -Path $ScriptDir -Filter "test_p*" | Where-Object { $_.Extension -ne ".ps1" -and $_.Extension -ne ".sh" -and $_.Extension -ne ".md" -and $_.Extension -ne ".cpp" } | Sort-Object Name

# Statistics
$totalTests = 0
$passedTests = 0
$failedTests = 0
$xfailTests = 0
$errorTests = 0
$failedNames = @()

# ══════════════════════════════════════════════════════════════════════════
# (AR) الحمرةُ المعروفةُ **تُشتقُّ من السجلِّ ولا تُكتَبُ هنا**.
#
#      كانت هذه القائمةُ فارغةً، فيخرج العدّاءُ ١ على أيِّ إخفاقٍ مهما كان
#      مُعلَنًا — بينما `../DECLARED_REDS.tsv` يُسجِّل تلك الملفّاتِ عينَها
#      دَينًا مقيسًا. فحكمَت خطوتان في الوظيفةِ الواحدةِ على الشجرةِ ذاتِها
#      بعقدَين متناقضَين، وأحمرَّ الأخضرُ منهما (٨ صفوفٍ مسجَّلةٍ على ويندوز).
#
#      🔑 وليست هذه قائمةَ إذنٍ تبلى: مصدرُها سجلٌّ محروسٌ في الاتّجاهَين
#      بـ`test_declared_reds_registry.py` — صفٌّ يخضرُّ يُخفِقُ هناك حتّى
#      يُحذَف، وأحمرُ ليس فيه يُخفِقُ هناك حتّى يُسجَّل. فلا نسخةَ ثانيةً
#      لحقيقةٍ واحدةٍ تنجرفُ عن أصلِها بلا أن تُخفِق.
# (EN) Known reds are DERIVED from the registry, never listed here. The list was
#      empty, so this runner exited 1 on any failure however well declared, while
#      ../DECLARED_REDS.tsv records those very files as measured debt: two steps
#      in one job judging the same tree under contradictory contracts. This is not
#      a rotting allowlist — its source is guarded in BOTH directions by
#      test_declared_reds_registry.py, so a row that turns green fails there until
#      deleted and a red absent from it fails there until recorded.
# ══════════════════════════════════════════════════════════════════════════
$registryPath = Join-Path $PSScriptRoot "..\DECLARED_REDS.tsv"
if (-not (Test-Path $registryPath)) {
    Write-Host "[ERROR] Registry not found: $registryPath"
    exit 1
}
$knownFailures = @(
    Get-Content -LiteralPath $registryPath -Encoding UTF8 |
        Where-Object { $_.Trim() -ne "" -and -not $_.TrimStart().StartsWith("#") } |
        ForEach-Object { ($_ -split "`t")[0].Trim() } |
        Where-Object { $_.StartsWith("_regression/") } |
        ForEach-Object { [System.IO.Path]::GetFileNameWithoutExtension($_) }
)
# (AR) قائمةٌ خاويةٌ تعني انكسارَ الاشتقاقِ لا سدادَ الدَّين — وهي تُشدِّد لا
#      تُرخي، فتمرُّ صامتةً لو تُرِكت. تُعلَن صراحةً.
# (EN) An empty list means the derivation broke, not that the debt was paid.
if ($knownFailures.Count -eq 0) {
    Write-Host "[ERROR] No _regression rows parsed from $registryPath -- derivation broke"
    exit 1
}
$xfailNames = @()

# ======================================================================
# Start Tests
# ======================================================================
Write-Host ""
Write-Host "================================================================"
# (AR) 🔑 كان مكتوبًا «(22 tests)» والسطرُ الذي يليه يطبعُ العددَ المقيسَ
#      من `$TestFiles.Count` — وهو **٣٧**. فعددٌ منثورٌ نسخةٌ ثانيةٌ من حقيقةٍ
#      تحتَه مباشرةً، وتباعدَ عنها بخمسةَ عشرَ. والعددُ يُشتَقُّ ولا يُكتَب.
# (EN) The banner said "(22 tests)" while the very next line printed the
#      measured $TestFiles.Count — 37. A prose number is a second copy of a
#      fact printed directly beneath it, and it had drifted by fifteen. The
#      count is derived, never written.
Write-Host "  Regression Tests -- Sad Language"
Write-Host "  Mode: $Mode"
Write-Host "  Tool: $SadExe"
Write-Host "  Count: $($TestFiles.Count) tests"
Write-Host "================================================================"
Write-Host ""

# Check tool exists
if (-not (Test-Path $SadExe)) {
    Write-Host "[ERROR] Tool not found: $SadExe"
    exit 1
}

foreach ($testFile in $TestFiles) {
    $totalTests++
    $testName = $testFile.BaseName
    $testNum = $testName -replace 'test_p(\d+).*', '$1'
    
    Write-Host -NoNewline "  [$totalTests/$($TestFiles.Count)] $testName ... "
    
    try {
        $startTime = Get-Date
        
        # (AR) 🔑 حُذف فرعُ `$Mode -eq "interpreter"` — انظرَ التعليلَ عندَ
        #      المُعامِلِ أعلاه.
        # (EN) The $Mode -eq "interpreter" branch was removed; see the rationale
        #      at the parameter above.
        # Compiler mode: sadc compile then run
        $outExe = "$env:TEMP\sad_test_$testNum.exe"
        
        # Compile
        $compileProcess = Start-Process -FilePath $SadExe -ArgumentList $testFile.FullName, "-o", $outExe `
            -NoNewWindow -PassThru -RedirectStandardOutput "$env:TEMP\sad_compile_out.txt" `
            -RedirectStandardError "$env:TEMP\sad_compile_err.txt"
        # (AR) المقبضُ يُخزَّن قبلَ الانتظار: بدونه تعود ExitCode فارغةً في PS 5.1
        #      بعد WaitForExit(<مهلة>)، فيقرأ الحارسُ $null ويحكم بالإخفاقِ دائمًا.
        # (EN) Cache the handle before waiting: otherwise ExitCode is $null in PS 5.1
        #      after WaitForExit(<timeout>), so the guard always reports failure.
        $null = $compileProcess.Handle

        # (AR) ⚠️ `$null =` لازمة: `WaitForExit(<مهلة>)` تُرجِعُ قيمةً منطقيّةً
        #      تتسرَّبُ إلى مجرى المخرَجِ فيطبعُ العدّاءُ «True» لكلِّ بذرةٍ بعدَ
        #      ملخّصِه، ومن التقطَ خرجَه بـ`$out = .\run_...` أخذَ مصفوفةَ
        #      منطقيّاتٍ لا نصًّا.
        # (EN) The $null = is required: WaitForExit(<timeout>) returns a boolean
        #      that leaks into the output stream, so the runner printed "True"
        #      once per seed after its summary, and anyone capturing its output
        #      got an array of booleans instead of text.
        $null = $compileProcess.WaitForExit(60000)
        
        if ($compileProcess.ExitCode -ne 0 -or -not (Test-Path $outExe)) {
            $duration = [math]::Round(((Get-Date) - $startTime).TotalMilliseconds)
            if ($knownFailures -contains $testName) {
                $xfailTests++
                $xfailNames += "$testName (compile)"
                Write-Host "XFAIL (compile error -- known issue) (${duration}ms)"
            } else {
                $failedTests++
                $failedNames += "$testName (compile)"
                Write-Host "FAIL (compile error)"
                $compErr = Get-Content "$env:TEMP\sad_compile_err.txt" -Raw -ErrorAction SilentlyContinue
                if ($compErr) { Write-Host "    $compErr" }
            }
            continue
        }
        
        # Run compiled binary
        $process = Start-Process -FilePath $outExe `
            -NoNewWindow -PassThru -RedirectStandardOutput "$env:TEMP\sad_test_out.txt" `
            -RedirectStandardError "$env:TEMP\sad_test_err.txt"
        # (AR) المقبضُ يُخزَّن قبلَ الانتظار: بدونه تعود ExitCode فارغةً في PS 5.1
        #      بعد WaitForExit(<مهلة>)، فيقرأ الحارسُ $null ويحكم بالإخفاقِ دائمًا.
        # (EN) Cache the handle before waiting: otherwise ExitCode is $null in PS 5.1
        #      after WaitForExit(<timeout>), so the guard always reports failure.
        $null = $process.Handle

        $completed = $process.WaitForExit(30000)
        
        if (-not $completed) {
            $process.Kill()
            $failedTests++
            $failedNames += "$testName (runtime timeout)"
            Write-Host "FAIL (runtime timeout)"
            continue
        }
        
        $exitCode = $process.ExitCode
        $output = Get-Content "$env:TEMP\sad_test_out.txt" -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
        $stderr = Get-Content "$env:TEMP\sad_test_err.txt" -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
        
        # Cleanup
        Remove-Item $outExe -ErrorAction SilentlyContinue
        
        $duration = [math]::Round(((Get-Date) - $startTime).TotalMilliseconds)
        
        # Analyze results
        $hasFail = $output -match "FAIL:"
        $hasPass = $output -match "PASS:"
        $hasError = $exitCode -ne 0

        # ==================================================================
        # (AR) معيارُ التكافؤ: غيابُ «FAIL:» ورمزُ خروجٍ صفرٌ لا يعنيان السلامة.
        #      ص٣٥ كان يطبع مؤشِّرًا خامًا حيث يطبع المفسّرُ الكائنَ، برمزِ صفرٍ
        #      وبلا الكلمة — فمرّ أخضرَ. نقارن خرجَ المحرّكَين حين يُمرَّر -InterpExe.
        # (EN) Parity criterion: no "FAIL:" plus exit 0 does not mean correct.
        #      p35 printed a raw pointer where the interpreter printed the object,
        #      with exit 0 and no marker, so it passed. Compare both engines'
        #      output whenever -InterpExe is supplied.
        # ==================================================================
        # ==================================================================
        # (AR) ⚠️ **دَينٌ مُقيَّد: معيارُ النجاحِ هنا أضعفُ ممّا كان.**
        #      كان يقيسُ ما بعدَ «لا FAIL ورمزُ خروجٍ صفر»: يُقارِنُ خرجَ
        #      المحرّكَين ورمزَيهما حين يُمرَّر `-InterpExe`. وقد زالَ أحدُ
        #      المحرّكَين، فلا طرفَ يُقارَنُ به — وحُذفت الكتلةُ لأنّها لا
        #      تشتعلُ أبدًا، لا لأنّ العلّةَ زالت.
        #      والعلّةُ مقيسةٌ ومُدوَّنة: ص٣٥ طبعَ مؤشِّرًا خامًّا حيثُ يُطبَعُ
        #      الكائن، برمزِ خروجٍ صفرٍ وبلا كلمةِ FAIL — فمرّ أخضرَ. ومثلُه
        #      محرّكٌ يرمي خطأً قابلًا للالتقاطِ وآخرُ يُكمِل: يمرّان أخضرَين
        #      ما دام المطبوعُ قبلَ الرميِ متطابقًا (صنفُ RUN007/RUN011).
        #      فالبديلُ الصحيحُ توكيدٌ مُدوَّنٌ لكلِّ بذرةٍ (`@expected`) يُقاسُ
        #      عليه الخرجُ — وهو ما يفعلُه `tests/runner.py`. وحتّى يُنقَلَ إلى
        #      هنا، **لا يُقرَأُ خُضرةُ هذا العدّاءِ أنّ المخرجاتِ صحيحة**؛
        #      تُقرَأُ أنّها لم تطبعْ FAIL ولم تُخفِقْ بخروجٍ غيرِ صفر.
        # (EN) RECORDED DEBT: this runner's pass criterion is now WEAKER than it
        #      was. It used to go beyond "no FAIL marker and exit 0" by comparing
        #      both engines' output and exit codes whenever -InterpExe was given.
        #      One engine is gone, so there is nothing to compare against; the
        #      block was deleted because it can never fire, not because the
        #      weakness went away. The weakness is measured and on record: p35
        #      printed a raw pointer where the object should be, with exit 0 and
        #      no FAIL marker, and passed green; likewise one engine raising a
        #      catchable error while the other carried on passed whenever the
        #      pre-error output matched (the RUN007/RUN011 class). The right
        #      replacement is a recorded per-seed assertion (@expected) compared
        #      against the output, which is what tests/runner.py already does.
        #      Until that is brought here, green from this runner must NOT be
        #      read as "the output is correct" — only as "it printed no FAIL and
        #      did not exit non-zero".
        # ==================================================================
        if ($hasError -or $hasFail) {
            if ($knownFailures -contains $testName) {
                $xfailTests++
                $xfailNames += $testName
                Write-Host "XFAIL (${duration}ms)"
            } else {
                $failedTests++
                $failedNames += $testName
                Write-Host "FAIL (${duration}ms)"
                if ($hasFail) {
                    $failLines = ($output -split "`n") | Where-Object { $_ -match "FAIL:" }
                    foreach ($line in $failLines) {
                        Write-Host "    $($line.Trim())"
                    }
                }
                if ($stderr) {
                    $stderrLines = ($stderr -split "`n") | Select-Object -First 3
                    foreach ($line in $stderrLines) {
                        Write-Host "    $($line.Trim())"
                    }
                }
            }
        } else {
            $passedTests++
            Write-Host "PASS (${duration}ms)"
        }
        
    } catch {
        $errorTests++
        $failedNames += "$testName (exception)"
        Write-Host "ERROR: $($_.Exception.Message)"
    }
}

# ======================================================================
# Summary
# ======================================================================
Write-Host ""
Write-Host "================================================================"
Write-Host "  Regression Test Summary"
Write-Host "================================================================"
Write-Host "  Total:     $totalTests"
Write-Host "  Passed:    $passedTests"
Write-Host "  Failed:    $failedTests"
Write-Host "  XFail:     $xfailTests"

# (AR) الأسماءُ تُطبَع دائمًا: عددٌ بلا أسماءٍ لا يكشف تبدُّلَ المجموعة.
# (EN) Names always printed: a count alone hides a change of membership.
if ($xfailNames.Count -gt 0) {
    Write-Host ""
    Write-Host "  XFail (declared in DECLARED_REDS.tsv):"
    foreach ($name in $xfailNames) {
        Write-Host "    ~ $name"
    }
}

if ($failedNames.Count -gt 0) {
    Write-Host ""
    Write-Host "  Failed Tests:"
    foreach ($name in $failedNames) {
        Write-Host "    X $name"
    }
}

Write-Host ""
if ($failedTests -eq 0) {
    Write-Host "  [OK] All regression tests passed!"
    exit 0
} else {
    $rate = [math]::Round(($passedTests / $totalTests) * 100, 1)
    Write-Host "  [!!] Pass rate: ${rate}%"
    exit 1
}
