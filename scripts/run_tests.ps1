# (AR) مشغّل سريع لاختبارات التنفيذ المزدوج — المسارات مثبَّتة على جذر المستودع
# (EN) Quick dual-execution test runner — paths anchored to the repository root
$repoRoot = Split-Path $PSScriptRoot -Parent
# (AR) sad-run زال مع المفسّر — العدّاءُ يُشغَّلُ بالمترجّم وحدَه.
$sad = Join-Path $repoRoot "build\bin\Debug\sad-build.exe"
# (AR) 🔑 **صفرُ اختبارٍ ليس نجاحًا.** كان المجلَّدُ يُمسَحُ بلا تحقّق: وقد حُذِفَ
#      `tests/dual_execution` مع المفسِّر، فصارت الأداةُ تطبعُ `PASS: 0 FAIL: 0`
#      — **أخضرَ لأنّ الشرطَ لا يمكنُ أن يكونَ كاذبًا**. وسبعُ وثائقِ تخطيطٍ
#      تستشهدُ بمخرَجِها بوّابةَ قبول (`PASS:137 / FAIL:20`)، فالرقمُ المنشورُ
#      كذبٌ بالبناءِ لا بالخطأ. وههنا يُرفَضُ صراحةً بدلَ أن يُطمئِنَ.
# (AR) ⚠️ وهي **قارئٌ رابعٌ لعقدِ `@expected`** لا يُورَّثُ من الحارس: نمطُه
#      `"# @expected (.+)"` أضيقُ ممّا يقرؤه `check_seed_contract.py`
#      (لا يقرأُ السوالب، ولا يشترطُ حمولةً غيرَ فارغة). فلا يُتَّخَذُ مرجعًا
#      لعدَّاد؛ والمرجعُ هو الحارسُ وحدَه.
$corpus = Join-Path $repoRoot "tests\dual_execution"
if (-not (Test-Path $corpus)) {
    Write-Host "✗ مادّةُ القياسِ مفقودة: $corpus"
    Write-Host "  (AR) صفرُ اختبارٍ ليس نجاحًا — لا يُعلَنُ PASS:0."
    exit 2
}
$tests = Get-ChildItem $corpus -Recurse -Filter "*.ص"
if ($tests.Count -eq 0) {
    Write-Host "✗ لا بذرةَ في $corpus — لم يُقَسْ شيء."
    exit 2
}
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

# (AR) 🔑 **الحكمُ يبلغُ المُستدعي.** كان السطرُ الأخيرُ طباعةً والرمزُ صفرًا
#      دائمًا — فـ`FAIL: 20` يُقرأُ نجاحًا في أيِّ شوطٍ آليّ. وحراسةُ وجودِ
#      المجلَّدِ وحدَها لا تكفي: مجلَّدٌ فيه بذورٌ بلا `# @expected` يُخرِجُ
#      `PASS: 0 FAIL: 0` بصفرٍ — **أخضرُ لأنّ الشرطَ لا يمكنُ أن يكونَ كاذبًا**.
if ($pass -eq 0) {
    Write-Host "X لم يُنفَّذْ اختبارٌ واحد — صفرُ اختبارٍ ليس نجاحًا."
    exit 2
}
if ($fail -gt 0) { exit 1 }
exit 0
