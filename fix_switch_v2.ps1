# fix_switch_v2.ps1 - Move misplaced switch cases to emitInstruction
$enc = New-Object System.Text.UTF8Encoding($false)
$path = "c:\s\s_language\compiler_new\src\backend\llvm\llvm_codegen.cpp"
$c = [System.IO.File]::ReadAllText($path, $enc)

# The wrong block is between these markers:
$blockStart = "        // ===== OS/Hardware - Port I/O ====="
$blockEnd = "            return emitBuiltinRandom(inst);"

$startIdx = $c.IndexOf($blockStart)
$endIdx = $c.IndexOf($blockEnd)
if ($startIdx -lt 0 -or $endIdx -lt 0) {
    Write-Output "ERROR: Cannot find markers. Start=$startIdx End=$endIdx"
    exit 1
}
$endIdx += $blockEnd.Length

# Also remove the blank line before the block (after "break;" of case 64)
$preBlank = $c.LastIndexOf("`n", $startIdx)
if ($preBlank -gt 0) { $startIdx = $preBlank + 1 }

Write-Output "Removing wrong block from position $startIdx to $endIdx"

# Extract the block
$switchBlock = $c.Substring($startIdx, $endIdx - $startIdx)
Write-Output "Block length: $($switchBlock.Length)"

# Remove from wrong location
$c = $c.Remove($startIdx, $endIdx - $startIdx)

# Now find the correct insertion point: before "default:" in emitInstruction
$target = "        default:`r`n            reportError(`"Unsupported opcode: `""
$targetIdx = $c.IndexOf($target)
if ($targetIdx -lt 0) {
    # Try without \r
    $target = "        default:`n            reportError(`"Unsupported opcode: `""
    $targetIdx = $c.IndexOf($target)
}
if ($targetIdx -lt 0) {
    Write-Output "ERROR: Cannot find emitInstruction default. Searching..."
    $idx = $c.IndexOf("Unsupported opcode:")
    Write-Output "Found 'Unsupported opcode:' at $idx"
    # Show surrounding text
    $s = [Math]::Max(0, $idx - 100)
    Write-Output "Context: $($c.Substring($s, 200))"
    exit 1
}

Write-Output "Inserting at position $targetIdx (before default in emitInstruction)"

# Insert the block + newlines
$insertion = "`r`n" + $switchBlock + "`r`n`r`n"
$c = $c.Insert($targetIdx, $insertion)

[System.IO.File]::WriteAllText($path, $c, $enc)
$bytes = [System.IO.File]::ReadAllBytes($path)
Write-Output "Done! File size: $($bytes.Length) bytes"
Write-Output "First 3 bytes: $($bytes[0]) $($bytes[1]) $($bytes[2])"

# Verify Arabic encoding
$c2 = [System.IO.File]::ReadAllText($path, $enc)
$arabIdx = $c2.IndexOf([char]0x631)
if ($arabIdx -gt 0) {
    $arabBytes = $enc.GetBytes($c2.Substring($arabIdx, 1))
    Write-Output "Arabic check: $($arabBytes -join ' ') (should be 216 177)"
}
