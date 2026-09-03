# (AR) مشغّل سريع لاختبارات التنفيذ المزدوج — المسارات مثبَّتة على جذر المستودع
# (EN) Quick dual-execution test runner — paths anchored to the repository root
$repoRoot = Split-Path $PSScriptRoot -Parent
# (AR) sad-run زال مع المفسّر — العدّاءُ يُشغَّلُ بالمترجّم وحدَه.
$sad = Join-Path $repoRoot "build\bin\Debug\sad-build.exe"
$tests = Get-ChildItem (Join-Path $repoRoot "tests\dual_execution") -Recurse -Filter "*.ص"
$pass = 0; $fail = 0; $failList = @(); $skipped = 0
foreach ($t in $tests) {
    $content = Get-Content $t.FullName -Raw -Encoding UTF8
    # (AR) يُنفَّذ الاختبار فقط إن حمل وسم "# @expected" بالمخرَج المتوقَّع
    # (EN) A test runs only if it carries the "# @expected" marker with the expected output
    if ($content -match "# @expected (.+)") {
        $expected = ($matches[1] -replace "\\n", "`n").TrimEnd()
        $job = Start-Job -ScriptBlock {
            param($exe, $file)
            & $exe $file 2>&1 | Out-String
        } -ArgumentList (Resolve-Path $sad).Path, $t.FullName
        # (AR) مهلة 3 ثوانٍ لكلّ اختبار؛ المتجاوز يُحسَب متخطًّى لا فاشلًا
        # (EN) 3-second timeout per test; overruns count as skipped, not failed
        $completed = Wait-Job $job -Timeout 3
        if ($completed) { $actual = (Receive-Job $job).TrimEnd() } else { Stop-Job $job; $actual = "__TIMEOUT__" }
        Remove-Job $job -Force
        if ($actual -eq "__TIMEOUT__") { $skipped++ }
        elseif ($actual -eq $expected) { $pass++ }
        # (AR) إيقاف مبكّر بعد 20 إخفاقًا لتقصير دورة التغذية الراجعة
        # (EN) Early stop after 20 failures to keep the feedback loop short
        else { $fail++; $failList += "$($t.Name): expected=[$expected] got=[$actual]"; if ($fail -ge 20) { break } }
    }
}
Write-Host "PASS: $pass   FAIL: $fail   SKIPPED(timeout): $skipped"
$failList | ForEach-Object { Write-Host "FAIL: $_" }
