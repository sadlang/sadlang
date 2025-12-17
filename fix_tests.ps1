# PowerShell script to fix all test files for VariableManager API changes

# Find all test files that use VariableManager
$testFiles = Get-ChildItem -Path "tests" -Include "*.cpp" -Recurse | 
    Where-Object { (Get-Content $_.FullName -Raw) -match "VariableManager" }

foreach ($file in $testFiles) {
    Write-Host "Processing: $($file.FullName)"
    
    $content = Get-Content $file.FullName -Raw
    
    # Pattern 1: VariableManager varMgr; needs to become VariableManager varMgr(scopeMgr);
    # But we need to add ScopeManager scopeMgr; first
    
    # Check if file creates VariableManager without ScopeManager
    if ($content -match "VariableManager\s+\w+\s*;\s*\n") {
        Write-Host "  Found VariableManager construction pattern"
        
        # Replace pattern: add ScopeManager before VariableManager
        $content = $content -replace "(    )(VariableManager\s+(\w+)\s*;)", "`$1ScopeManager scopeMgr;`n`$1VariableManager `$3(scopeMgr);"
    }
    
    # Pattern 2: getScopeManager()-> needs to become getScopeManager().
    $content = $content -replace "getScopeManager\(\s*\)->", "getScopeManager()."
    
    # Pattern 3: getScopeManager() != nullptr checks should be removed or changed
    # Just comment them out for now
    $content = $content -replace "(assert\(.*getScopeManager\(\s*\)\s*!=\s*nullptr\);)", "// `$1  // getScopeManager() now returns reference"
    
    # Write back
    Set-Content -Path $file.FullName -Value $content -NoNewline
    Write-Host "  Updated!"
}

Write-Host "`nDone! Updated $($testFiles.Count) files."
