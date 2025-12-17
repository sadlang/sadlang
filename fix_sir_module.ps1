# Fix sir_module.cpp by removing SIRBasicBlock implementation
$file = "C:\s\s_language\compiler\frontend\src\sir_module.cpp"

Write-Host "Reading file..."
$content = Get-Content $file -Raw

# Fix 1: Replace type->toString() with sirTypeToString(type)
Write-Host "Fix 1: Replacing type->toString()..."
$content = $content -replace 'type->toString\(\)', 'sirTypeToString(type)'

# Fix 2: Remove SIRBasicBlock implementation (lines between two markers)
Write-Host "Fix 2: Removing SIRBasicBlock implementation..."
$pattern = '// ={70}\r?\n// SIRBasicBlock Implementation\r?\n// ={70}\r?\n.*?(?=// ={70}\r?\n// SIRFunction Implementation)'
$content = $content -replace $pattern, ''

Write-Host "Writing file..."
$content | Set-Content $file -Encoding UTF8 -NoNewline

Write-Host "Done! File fixed."
