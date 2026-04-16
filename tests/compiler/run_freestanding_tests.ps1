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
$passed = 0
$failed = 0
$total = 0

function Compile-Source {
    param([string]$SourceFile)
    $outFile = [System.IO.Path]::ChangeExtension($SourceFile, ".ll")
    $oldEAP = $ErrorActionPreference
    $ErrorActionPreference = "SilentlyContinue"
    & $SadcPath $SourceFile --freestanding --emit-llvm -T x86_64-unknown-none-elf -O1 -o $outFile 2>$null
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
    if ($null -eq $IRContent) {
        Write-Host "  [FAIL] $TestName - no IR content" -ForegroundColor Red
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
Write-Host ""

# --- Test 1: String + Number Concatenation ---
Write-Host ">> Test 1: String + Number Concatenation" -ForegroundColor Yellow
$src1 = Join-Path $TestDir ("test_freestanding_string_concat" + $SadExt)
$ir1 = Compile-Source $src1

Check-Pattern -TestName "T1.1" -IRContent $ir1 -Pattern "__sad_itoa" -Description "itoa calls for number-to-string" -ExpectedCount 3
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

Check-Pattern -TestName "T3.1" -IRContent $ir3 -Pattern "__sad_itoa" -Description "itoa for edge case numbers" -ExpectedCount 8
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
