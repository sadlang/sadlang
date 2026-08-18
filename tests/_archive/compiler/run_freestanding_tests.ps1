param(
    [string]$SadcPath = "c:\s_lang\s-programming-language\build\bin\Release\sadc.exe",
    [string]$TestDir = "c:\s_lang\s-programming-language\tests\compiler"
)

# (AR) إذا لم يُوجد sadc في المسار المُمرَّر، جرّب Release كبديل
# (EN) If sadc not found at given path, try Release as fallback
if (-not (Test-Path $SadcPath)) {
    # حاول استبدال Debug بـ Release (دعم / و \)
    $releasePath = $SadcPath -replace '[/\\]Debug[/\\]', '/Release/'
    if (Test-Path $releasePath) {
        $SadcPath = $releasePath
    }
    else {
        $releasePath2 = $SadcPath -replace '[/\\]Debug[/\\]', '\Release\'
        if (Test-Path $releasePath2) {
            $SadcPath = $releasePath2
        }
    }
}

$ErrorActionPreference = "Continue"
$SadExt = "." + [char]0x0635

# (AR) أعلامُ المُصرِّفِ القانونيّةُ عربيّةٌ منذُ إلغاءِ المرادفاتِ الإنجليزيّة (ISSUE-162):
#      --freestanding صار «--حرّ» و--emit-llvm صار «--أظهر-llvm».
# (AR) وتُبنَى من نقاطِ الترميزِ كما بُنِيَت $SadExt: هذا الملفُّ UTF-8 بلا BOM
#      فتقرأُه PowerShell 5.1 بـcp1252، فحرفٌ عربيٌّ مكتوبٌ حرفيًّا
#      يصلُ إلى المُصرِّفِ مشوَّهًا فيُرفَض «خيارٌ غيرُ معروف».
# (EN) Canonical compiler flags are Arabic since aliases were dropped; built
#      from code points because this file is BOM-less UTF-8 (PS 5.1 reads cp1252).
$FreestandingFlag = "--" + [char]0x062D + [char]0x0631 + [char]0x0651
$EmitLlvmFlag = "--" + [char]0x0623 + [char]0x0638 + [char]0x0647 + [char]0x0631 + "-llvm"
$passed = 0
$failed = 0
$total = 0

function Compile-Source {
    param([string]$SourceFile)
    $outFile = [System.IO.Path]::ChangeExtension($SourceFile, ".ll")
    # (AR) ⚠️ يُمسَح المُخرَجُ قبلَ الترجمة: ملفٌّ بائتٌ من شوطٍ سابقٍ يُقرَأُ مُخرَجًا طازجًا.
    # (AR) 🔑 و`Out-Null` ليست تجميلًا (ISSUE-165): دونَها يدخلُ مُخرَجُ المُصرِّفِ القياسيُّ
    #      في قيمةِ الدالة، فيُقاسُ النمطُ على ثرثرةِ [DEBUG] لا على LLVM IR — وقد قُسِمَت:
    #      ثرثرةُ ملفٍّ تَعذَّرَت ترجمتُه تحوي «while_cond» ١٦ مرّةً و«while_body» ٢١،
    #      فمرَّ تأكيدانِ ولا سطرَ IR واحدًا وُلِدَ.
    # (EN) Out-Null is not cosmetic: the compiler's stdout would otherwise become the
    #      returned «IR», so patterns matched [DEBUG] chatter instead of LLVM IR.
    if (Test-Path $outFile) { Remove-Item $outFile -Force -EA SilentlyContinue }
    $oldEAP = $ErrorActionPreference
    $ErrorActionPreference = "SilentlyContinue"
    & $SadcPath $SourceFile $FreestandingFlag $EmitLlvmFlag -T x86_64-unknown-none-elf -O1 -o $outFile 2>$null | Out-Null
    $ErrorActionPreference = $oldEAP
    if (Test-Path $outFile) {
        return [System.IO.File]::ReadAllText($outFile, [System.Text.Encoding]::UTF8)
    }
    return $null
}

function Check-Pattern {
    param(
        [string]$TestName,
        [string]$IRContent,
        [string]$Pattern,
        [string]$Description,
        [int]$ExpectedCount = 1
    )
    $script:total++
    # (AR) المُعامِلُ مُصرَّحٌ [string]، و`$null` يَرِدُ إليه مُحوَّلًا إلى "" — فـ`$null -eq`
    #      لا تصدُقُ قطُّ، ويَظهرُ انهيارُ الترجمةِ في هيئةِ «found 0» أي في هيئةِ
    #      انجرافِ شكلِ IR. والفرقُ بينهما تشخيصٌ كامِل.
    # (EN) [string] coerces $null to "", so the crash looked like IR-shape drift.
    if ([string]::IsNullOrWhiteSpace($IRContent)) {
        Write-Host "  [FAIL] $TestName - no IR content (compile failed)" -ForegroundColor Red
        $script:failed++
        return
    }
    $m = [regex]::Matches($IRContent, $Pattern)
    if ($m.Count -ge $ExpectedCount) {
        Write-Host "  [PASS] $TestName - $Description (found $($m.Count))" -ForegroundColor Green
        $script:passed++
    }
    else {
        Write-Host "  [FAIL] $TestName - $Description (expected >= $ExpectedCount, found $($m.Count))" -ForegroundColor Red
        $script:failed++
    }
}

Write-Host ""
Write-Host "=== Freestanding Bug Fix Tests ===" -ForegroundColor Cyan
# (AR) ⚠️ الإبرةُ كانت «@sprintf» وصارت «__sad_itoa»: تحويلُ العددِ إلى نصٍّ في الوضعِ الحرِّ
#      لم يعد يمرُّ بـsprintf بل بمُعينٍ حرٍّ. و«@sprintf» ما تزال تطابِقُ مرّةً واحدةً —
#      تعريفَ الكُعبةِ `weak_odr` لا نداءً — فإبرةٌ بـ«ExpectedCount 1» كانت ستَخضَرُّ بلا تحويلٍ واحد.
# (EN) Needle moved from @sprintf to __sad_itoa: freestanding int-to-string no longer
#      routes through sprintf. @sprintf still matches once — the weak_odr shim
#      definition, not a call — so a count-1 needle would pass with zero conversions.
Write-Host ""

# --- Test 1: String + Number Concatenation ---
Write-Host ">> Test 1: String + Number Concatenation" -ForegroundColor Yellow
$src1 = Join-Path $TestDir ("test_freestanding_string_concat" + $SadExt)
$ir1 = Compile-Source $src1

Check-Pattern -TestName "T1.1" -IRContent $ir1 -Pattern "call i32 @__sad_itoa\(ptr %strbuf" -Description "__sad_itoa calls for number-to-string" -ExpectedCount 3
Check-Pattern -TestName "T1.2" -IRContent $ir1 -Pattern "call ptr @malloc" -Description "malloc for concat buffer" -ExpectedCount 3
Check-Pattern -TestName "T1.3" -IRContent $ir1 -Pattern "call ptr @memcpy" -Description "memcpy for concat" -ExpectedCount 6
Check-Pattern -TestName "T1.4" -IRContent $ir1 -Pattern "call i64 @strlen" -Description "strlen for length calc" -ExpectedCount 6

Remove-Item ([System.IO.Path]::ChangeExtension($src1, ".ll")) -Force -EA SilentlyContinue

# --- Test 2: Parameter Reassignment in Loops ---
Write-Host ""
Write-Host ">> Test 2: Parameter Reassignment in Loops" -ForegroundColor Yellow
$src2 = Join-Path $TestDir ("test_freestanding_param_reassign" + $SadExt)
$ir2 = Compile-Source $src2

Check-Pattern -TestName "T2.1" -IRContent $ir2 -Pattern "while_cond" -Description "while_cond blocks created" -ExpectedCount 4
Check-Pattern -TestName "T2.2" -IRContent $ir2 -Pattern "while_body" -Description "while_body blocks created" -ExpectedCount 4
Check-Pattern -TestName "T2.3" -IRContent $ir2 -Pattern "store i64 %subtmp" -Description "store subtmp in param alloca" -ExpectedCount 1
Check-Pattern -TestName "T2.4" -IRContent $ir2 -Pattern "icmp sgt" -Description "compare updated param" -ExpectedCount 1
Check-Pattern -TestName "T2.5" -IRContent $ir2 -Pattern "alloca i64" -Description "allocas for parameters" -ExpectedCount 5

Remove-Item ([System.IO.Path]::ChangeExtension($src2, ".ll")) -Force -EA SilentlyContinue

# --- Test 3: String Edge Cases ---
Write-Host ""
Write-Host ">> Test 3: String + Number Edge Cases" -ForegroundColor Yellow
$src3 = Join-Path $TestDir ("test_freestanding_string_edge_cases" + $SadExt)
$ir3 = Compile-Source $src3

Check-Pattern -TestName "T3.1" -IRContent $ir3 -Pattern "call i32 @__sad_itoa\(ptr %strbuf" -Description "__sad_itoa for edge case numbers" -ExpectedCount 8
Check-Pattern -TestName "T3.2" -IRContent $ir3 -Pattern "call ptr @malloc" -Description "malloc for edge case concat" -ExpectedCount 8
Check-Pattern -TestName "T3.3" -IRContent $ir3 -Pattern "call ptr @memcpy" -Description "memcpy for edge case concat" -ExpectedCount 16
Check-Pattern -TestName "T3.4" -IRContent $ir3 -Pattern "call i64 @strlen" -Description "strlen for edge cases" -ExpectedCount 16

Remove-Item ([System.IO.Path]::ChangeExtension($src3, ".ll")) -Force -EA SilentlyContinue

# --- Test 4: Parameter Edge Cases ---
Write-Host ""
Write-Host ">> Test 4: Parameter Reassignment Edge Cases" -ForegroundColor Yellow
$src4 = Join-Path $TestDir ("test_freestanding_param_edge_cases" + $SadExt)
$ir4 = Compile-Source $src4

Check-Pattern -TestName "T4.1" -IRContent $ir4 -Pattern "while_cond" -Description "while_cond in nested/edge loops" -ExpectedCount 5
Check-Pattern -TestName "T4.2" -IRContent $ir4 -Pattern "while_body" -Description "while_body in nested/edge loops" -ExpectedCount 5
Check-Pattern -TestName "T4.3" -IRContent $ir4 -Pattern "alloca i64" -Description "allocas for params + locals" -ExpectedCount 10
Check-Pattern -TestName "T4.4" -IRContent $ir4 -Pattern "icmp" -Description "comparisons for loop conditions" -ExpectedCount 5

Remove-Item ([System.IO.Path]::ChangeExtension($src4, ".ll")) -Force -EA SilentlyContinue

# --- Summary ---
Write-Host ""
Write-Host "=== Results ===" -ForegroundColor Cyan
$color = if ($failed -eq 0) { "Green" } else { "Red" }
Write-Host ("  PASS: {0}/{1}  FAIL: {2}/{3}" -f $passed, $total, $failed, $total) -ForegroundColor $color
Write-Host ""

exit $failed
