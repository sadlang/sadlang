# fix_switch_cases.ps1 - Move switch cases to the correct function
$enc = New-Object System.Text.UTF8Encoding($false)
$path = "c:\s\s_language\compiler_new\src\backend\llvm\llvm_codegen.cpp"
$c = [System.IO.File]::ReadAllText($path, $enc)

# Step 1: Remove the wrongly-placed cases from getConstantInt
# They start after "case 64:" and before the original "default:" of getConstantInt
# Find the wrong block: starts with "// ===== OS/Hardware" and ends before the default of getConstantInt
$wrongStart = $c.IndexOf("        // ===== OS/Hardware - Port I/O =====")
if ($wrongStart -lt 0) {
    Write-Output "ERROR: Could not find wrong OS/Hardware block"
    exit 1
}
Write-Output "Wrong cases start at: $wrongStart"

# Find where the wrong cases end - they go up to the BUILTIN_RANDOM case then the original default of getConstantInt
# Let's find the end pattern: the line after last wrong "case SIROpcode::BUILTIN_RANDOM:" + return
$wrongBlockEnd = $c.IndexOf("`n`n        default:", $wrongStart)
if ($wrongBlockEnd -lt 0) {
    # Try finding the pattern differently  
    $wrongBlockEnd = $c.IndexOf("        default:" + [char]13 + [char]10 + "            type = nullptr", $wrongStart)
    if ($wrongBlockEnd -lt 0) {
        # Search more carefully
        $defaultAfterWrong = $c.IndexOf("            type = nullptr", $wrongStart)
        if ($defaultAfterWrong -gt 0) {
            # The default: is just before this
            $wrongBlockEnd = $c.LastIndexOf("default:", $defaultAfterWrong)
            $wrongBlockEnd = $c.LastIndexOf("`n", $wrongBlockEnd)
            Write-Output "Found default at around: $wrongBlockEnd"
        }
    }
}

# Better approach: find the end of the wrong block by searching for the original getConstantInt default
# The original code after case 64 was: default: type = nullptr; break;
# Let me find the extent of our wrong insertion

# Find the exact content that needs to be removed
# From "        // ===== OS/Hardware" up to but not including the original "default:" of getConstantInt
$lines = $c -split "`n"
$startLine = -1
$endLine = -1
for ($i = 0; $i -lt $lines.Length; $i++) {
    if ($lines[$i] -match '// ===== OS/Hardware - Port I/O =====' -and $startLine -eq -1) {
        $startLine = $i
    }
    # Find the line with "type = nullptr" which is part of the original default
    if ($startLine -gt 0 -and $lines[$i] -match '^\s+type = nullptr') {
        # The default: line is the one before this
        $endLine = $i - 1  # default: line
        break
    }
}

Write-Output "Wrong block: lines $startLine to $endLine"

# Also find where the existing correct default is in emitInstruction
$emitDefaultLine = -1
for ($i = $endLine + 10; $i -lt $lines.Length; $i++) {
    if ($lines[$i] -match 'reportError.*Unsupported opcode') {
        $emitDefaultLine = $i - 1  # default: line
        break
    }
}
Write-Output "emitInstruction default at line: $emitDefaultLine"

# Extract the switch cases block (the good part)
$switchCasesBlock = ""
for ($i = $startLine; $i -lt $endLine; $i++) {
    $switchCasesBlock += $lines[$i] + "`n"
}
Write-Output "Extracted switch cases block: $($switchCasesBlock.Length) chars"

# Step 2: Remove the wrong cases from getConstantInt
# Replace lines startLine..endLine-1 content with nothing
$newLines = @()
for ($i = 0; $i -lt $lines.Length; $i++) {
    if ($i -ge $startLine -and $i -lt $endLine) {
        continue  # skip wrong cases
    }
    if ($i -eq $emitDefaultLine) {
        # Insert the switch cases BEFORE the default of emitInstruction
        $newLines += ""
        $caseLines = $switchCasesBlock -split "`n"
        foreach ($cl in $caseLines) {
            if ($cl.Trim().Length -gt 0) {
                $newLines += $cl
            }
        }
        $newLines += ""
    }
    $newLines += $lines[$i]
}

$result = $newLines -join "`n"
[System.IO.File]::WriteAllText($path, $result, $enc)
Write-Output "Fixed! New size: $($result.Length) chars, $([System.IO.File]::ReadAllBytes($path).Length) bytes"
