$path = "c:\s\s_language\compiler_new\src\backend\llvm\llvm_codegen.cpp"
$text = [System.IO.File]::ReadAllText($path)
$utf16le = [System.Text.Encoding]::Unicode
[System.IO.File]::WriteAllText($path, $text, $utf16le)
Write-Output "Re-saved as UTF-16 LE. Size: $((Get-Item $path).Length)"
