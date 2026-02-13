# Check original git file encoding and Arabic string
$path = "c:\s\s_language\compiler_new\src\backend\llvm\llvm_codegen.cpp"
$bytes = [System.IO.File]::ReadAllBytes($path)
Write-Output "File size: $($bytes.Length)"
Write-Output "First 3 bytes: $($bytes[0]) $($bytes[1]) $($bytes[2])"

# Find the Arabic string in the binary
$searchStr = [System.Text.Encoding]::UTF8.GetBytes('functions.find("')
for ($i = 0; $i -lt $bytes.Length - $searchStr.Length; $i++) {
    $match = $true
    for ($j = 0; $j -lt $searchStr.Length; $j++) {
        if ($bytes[$i + $j] -ne $searchStr[$j]) { $match = $false; break }
    }
    if ($match) {
        $context = $bytes[($i)..($i+60)]
        $hex = ($context | ForEach-Object { "{0:X2}" -f $_ }) -join ' '
        Write-Output "Found at byte $i : $hex"
    }
}
