# Step 1: Restore original from git (in UTF-16 LE)
git checkout HEAD -- compiler_new/src/backend/llvm/llvm_codegen.cpp
Write-Output "Step 1: Restored from git"

# Step 2: Check if file is UTF-16 LE
$bytes = [System.IO.File]::ReadAllBytes("c:\s\s_language\compiler_new\src\backend\llvm\llvm_codegen.cpp")
Write-Output "BOM: $($bytes[0]) $($bytes[1]) - Size: $($bytes.Length)"
