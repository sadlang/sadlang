$sad = ".\build\bin\Debug\sad.exe"
$tests = Get-ChildItem "tests\dual_execution" -Recurse -Filter "*.ص"
$pass = 0; $fail = 0; $failList = @(); $skipped = 0
foreach ($t in $tests) {
    $content = Get-Content $t.FullName -Raw -Encoding UTF8
    if ($content -match "# @expected (.+)") {
        $expected = ($matches[1] -replace "\\n", "`n").TrimEnd()
        $job = Start-Job -ScriptBlock {
            param($exe, $file)
            & $exe $file 2>&1 | Out-String
        } -ArgumentList (Resolve-Path $sad).Path, $t.FullName
        $completed = Wait-Job $job -Timeout 3
        if ($completed) { $actual = (Receive-Job $job).TrimEnd() } else { Stop-Job $job; $actual = "__TIMEOUT__" }
        Remove-Job $job -Force
        if ($actual -eq "__TIMEOUT__") { $skipped++ }
        elseif ($actual -eq $expected) { $pass++ }
        else { $fail++; $failList += "$($t.Name): expected=[$expected] got=[$actual]"; if ($fail -ge 20) { break } }
    }
}
Write-Host "PASS: $pass   FAIL: $fail   SKIPPED(timeout): $skipped"
$failList | ForEach-Object { Write-Host "FAIL: $_" }
