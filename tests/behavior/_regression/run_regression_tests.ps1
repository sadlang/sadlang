<#
.SYNOPSIS
    Regression Tests Runner for Sad Language
.DESCRIPTION
    Runs all 22 regression tests against interpreter or compiler
.PARAMETER SadExe
    Path to executable (sad.exe or sadc.exe)
.PARAMETER Mode
    Run mode: interpreter or compiler
.EXAMPLE
    .\run_regression_tests.ps1 -SadExe .\build\bin\Debug\sad.exe -Mode interpreter
    .\run_regression_tests.ps1 -SadExe .\build\bin\Release\sad-build.exe -Mode compiler
#>

param(
    [Parameter(Mandatory=$true)]
    [string]$SadExe,
    
    [Parameter(Mandatory=$false)]
    [ValidateSet("interpreter", "compiler")]
    [string]$Mode = "interpreter",

    # (AR) مسار المفسّر: حين يُمرَّر في وضع المترجِم يُقاس **تكافؤ** المحرّكَين لا مجرّد
    #      غياب كلمة FAIL. وبدونه يعمل الحارس كما كان.
    # (EN) Interpreter path: when supplied in compiler mode, engine PARITY is measured
    #      instead of merely the absence of a FAIL marker.
    [Parameter(Mandatory=$false)]
    [string]$InterpExe = ""
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
Write-Host "  Regression Tests -- Sad Language (22 tests)"
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
        
        if ($Mode -eq "interpreter") {
            # Run interpreter with 30 second timeout
            $process = Start-Process -FilePath $SadExe -ArgumentList $testFile.FullName `
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
                $duration = [math]::Round(((Get-Date) - $startTime).TotalMilliseconds)
                
                if ($knownFailures -contains $testName) {
                    $xfailTests++
                    $xfailNames += "$testName (timeout)"
                    Write-Host "XFAIL (timeout -- known issue) (${duration}ms)"
                } else {
                    $failedTests++
                    $failedNames += $testName
                    Write-Host "FAIL (timeout -- possible infinite loop)"
                }
                continue
            }
            
            $exitCode = $process.ExitCode
            $output = Get-Content "$env:TEMP\sad_test_out.txt" -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
            $stderr = Get-Content "$env:TEMP\sad_test_err.txt" -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
            
        } else {
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

            $compileProcess.WaitForExit(60000)
            
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
        }
        
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
        $parityBroken = $false
        if ($Mode -eq "compiler" -and $InterpExe -ne "" -and -not $hasError) {
            $parityOut = "$env:TEMP\sad_parity_out.txt"
            $parityErr = "$env:TEMP\sad_parity_err.txt"
            # (AR) 🔑 يُمسحان قبل كلّ إطلاق: `SilentlyContinue` تبتلع **غيابَ** الملفّ
            #      لا **قِدَمَه**، فإخفاقُ إطلاقٍ كان يقارن خرجَ الاختبارِ السابقِ
            #      ⇒ حكمٌ يتبع ترتيبَ التنفيذِ لا الشيفرة.
            # (EN) Cleared before every launch: SilentlyContinue swallows a MISSING file,
            #      not a STALE one, so a failed launch compared the previous test's output,
            #      making the verdict depend on execution order rather than on the code.
            Remove-Item $parityOut, $parityErr -Force -ErrorAction SilentlyContinue
            $ip = Start-Process -FilePath $InterpExe -ArgumentList $testFile.FullName `
                -NoNewWindow -PassThru -RedirectStandardOutput $parityOut `
                -RedirectStandardError $parityErr
            $null = $ip.Handle
            $ipDone = $ip.WaitForExit(30000)
            if (-not $ipDone) {
                # (AR) مهلةٌ ليست تباعدًا: الخرجُ مبتورٌ فلا يُقارَن، ويُسمّى ما وقع.
                # (EN) A timeout is not a divergence: truncated output is not compared.
                $ip.Kill()
                $ip.WaitForExit()
                $parityBroken = $true
                $parityDetail = "PARITY: interpreter timed out (not an output difference)"
            }
            else {
                $iOut = Get-Content $parityOut -Raw -Encoding UTF8 -ErrorAction SilentlyContinue
                if ($null -eq $iOut) { $iOut = "" }
                $cOut = $output
                if ($null -eq $cOut) { $cOut = "" }
                if ($iOut.Trim() -ne $cOut.Trim()) {
                    $parityBroken = $true
                    $parityDetail = "PARITY: interpreter and compiler outputs differ"
                }
                # (AR) 🔑 ورمزُ الخروجِ كذلك. كان يُلتقَط ولا يُقرَأ، فمحرّكٌ يرمي خطأً
                #      قابلًا للالتقاطِ وآخرُ يُكمِل يمرّان أخضرَين ما دام المطبوعُ قبلَ
                #      الرميِ متطابقًا — وهو عينُ صنفِ التباعدِ الذي تُنشئه RUN007/RUN011،
                #      أي الذي وُضِع هذا الحارسُ ليقيسه.
                # (EN) And the exit code. It was captured but never read, so one engine
                #      raising a catchable error while the other carried on passed green
                #      whenever the pre-error output matched — exactly the divergence class
                #      RUN007/RUN011 create, i.e. the one this guard exists to measure.
                elseif ($ip.ExitCode -ne $exitCode) {
                    $parityBroken = $true
                    $parityDetail = "PARITY: exit codes differ (interpreter=$($ip.ExitCode) compiler=$exitCode)"
                }
            }
        }

        if ($hasError -or $hasFail -or $parityBroken) {
            if ($knownFailures -contains $testName) {
                $xfailTests++
                $xfailNames += $testName
                Write-Host "XFAIL (${duration}ms)"
            } else {
                $failedTests++
                $failedNames += $testName
                Write-Host "FAIL (${duration}ms)"
                if ($parityBroken) {
                    Write-Host "    PARITY: interpreter and compiler outputs differ"
                }
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
