# =============================================================================
# Split compiler_driver_backend.cpp into 3 files
# =============================================================================
$ErrorActionPreference = "Stop"
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

$srcDir = "C:\s_lang\s-programming-language\tools\compiler"
$srcFile = "$srcDir\compiler_driver_backend.cpp"

$lines = [IO.File]::ReadAllLines($srcFile, [Text.Encoding]::UTF8)
$total = $lines.Count
Write-Host "Source file: $total lines"

$enc = [System.Text.UTF8Encoding]::new($false)

# Boundaries (1-based):
# Full header+stubs+includes: 1-161
# run_middleend: 162-279
# run_backend: 280-687
# print_ir_if_requested: 688-697
# invoke_linker: 698-898
# (helper generate_temp_file): ~882-897 
# cleanup_temp_files: 899-1066
# get_android_sysroot: 1067-1174
# compile_c_to_obj: 1175-1341
# link_with_embedded_lld: 1342-1680
# link_android_executable: 1681-2041
# link_object_to_executable: 2042-2980
# print_version: 2981-2995
# print_help: 2996-3582

# ═══════════════════════════════════════════════════════════════════════════
# FILE 1: compiler_driver_backend.cpp (core: middleend, backend, ir, version, help)
# Keep original header (lines 1-161), then core functions
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating backend (core)..."
$sb1 = [System.Text.StringBuilder]::new()

# Basic header + includes (lines 1-6, skip LLD includes)
for ($i = 0; $i -le 5; $i++) {
    [void]$sb1.AppendLine($lines[$i])
}
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine('#include "compiler_driver.h"')
[void]$sb1.AppendLine('#include "../../shared/utils/include/utf8_utils.h"')
[void]$sb1.AppendLine('#include "../../shared/lexer/include/lexer_core.h"')
[void]$sb1.AppendLine('#include "../../shared/parser/include/parser_core.h"')
[void]$sb1.AppendLine('#include "../../compiler_new/include/frontend/sir_builder.h"')
[void]$sb1.AppendLine('#include "../../compiler_new/include/frontend/sir_module.h"')
[void]$sb1.AppendLine('#include "../../compiler_new/include/backend/llvm/llvm_codegen.h"')
[void]$sb1.AppendLine('#include "../../compiler_new/include/backend/llvm/arabic_optimizer.h"')
[void]$sb1.AppendLine('#include "../../compiler_new/include/middle/optimizer.h"')
[void]$sb1.AppendLine('#include "../../compiler_new/include/semantic/borrow_checker.h"')
[void]$sb1.AppendLine('#include "../../shared/ast/include/statements.h"')
[void]$sb1.AppendLine('#include <llvm/Support/FileSystem.h>')
[void]$sb1.AppendLine('#include <llvm/Support/raw_ostream.h>')
[void]$sb1.AppendLine('#include <llvm/Bitcode/BitcodeWriter.h>')
[void]$sb1.AppendLine('#include <llvm/Passes/PassBuilder.h>')
[void]$sb1.AppendLine('#include <llvm/Transforms/Coroutines/CoroEarly.h>')
[void]$sb1.AppendLine('#include <llvm/Transforms/Coroutines/CoroSplit.h>')
[void]$sb1.AppendLine('#include <llvm/Transforms/Coroutines/CoroElide.h>')
[void]$sb1.AppendLine('#include <llvm/Transforms/Coroutines/CoroCleanup.h>')
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("#ifdef _WIN32")
[void]$sb1.AppendLine("#include <windows.h>")
[void]$sb1.AppendLine("#undef CYAN")
[void]$sb1.AppendLine("#undef YELLOW")
[void]$sb1.AppendLine("#undef RED")
[void]$sb1.AppendLine("#undef GREEN")
[void]$sb1.AppendLine("#undef BLUE")
[void]$sb1.AppendLine("#undef MAGENTA")
[void]$sb1.AppendLine("#undef WHITE")
[void]$sb1.AppendLine("#undef ERROR")
[void]$sb1.AppendLine("#undef FATAL")
[void]$sb1.AppendLine("#endif")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("#include <iostream>")
[void]$sb1.AppendLine("#include <fstream>")
[void]$sb1.AppendLine("#include <sstream>")
[void]$sb1.AppendLine("#include <chrono>")
[void]$sb1.AppendLine("#include <algorithm>")
[void]$sb1.AppendLine("#include <iomanip>")
[void]$sb1.AppendLine("#include <limits>")
[void]$sb1.AppendLine("#include <cstdlib>")
[void]$sb1.AppendLine("#include <cstdio>")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("namespace sad")
[void]$sb1.AppendLine("{")
[void]$sb1.AppendLine("    namespace driver")
[void]$sb1.AppendLine("    {")
[void]$sb1.AppendLine("")

# run_middleend: lines 162-279 (0-idx: 161-278)
for ($i = 161; $i -le 278; $i++) {
    [void]$sb1.AppendLine($lines[$i])
}
[void]$sb1.AppendLine("")

# run_backend: lines 280-687 (0-idx: 279-686)
for ($i = 279; $i -le 686; $i++) {
    [void]$sb1.AppendLine($lines[$i])
}
[void]$sb1.AppendLine("")

# print_ir_if_requested: lines 688-697 (0-idx: 687-696)
for ($i = 687; $i -le 696; $i++) {
    [void]$sb1.AppendLine($lines[$i])
}
[void]$sb1.AppendLine("")

# print_version: lines 2981-2995 (0-idx: 2980-2994)
for ($i = 2980; $i -le 2994; $i++) {
    [void]$sb1.AppendLine($lines[$i])
}
[void]$sb1.AppendLine("")

# print_help: lines 2996-3579 (before namespace close, 0-idx: 2995-3578)
for ($i = 2995; $i -le 3578; $i++) {
    [void]$sb1.AppendLine($lines[$i])
}
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("    } // namespace driver")
[void]$sb1.AppendLine("} // namespace sad")

[IO.File]::WriteAllText($srcFile, $sb1.ToString(), $enc)
$c1 = ($sb1.ToString() -split "`n").Count
Write-Host "  -> backend (core): ~$c1 lines"

# ═══════════════════════════════════════════════════════════════════════════
# FILE 2: compiler_driver_linker.cpp (linking: invoke, lld, android, object)
# Needs the full header with LLD stubs
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating linker file..."
$sb2 = [System.Text.StringBuilder]::new()

# Copy the FULL original header including LLD stubs (lines 1-161)
for ($i = 0; $i -le 160; $i++) {
    [void]$sb2.AppendLine($lines[$i])
}

# invoke_linker: lines 698-898 (0-idx: 697-897)
for ($i = 697; $i -le 897; $i++) {
    [void]$sb2.AppendLine($lines[$i])
}
[void]$sb2.AppendLine("")

# link_with_embedded_lld: lines 1342-1680 (0-idx: 1341-1679)
for ($i = 1341; $i -le 1679; $i++) {
    [void]$sb2.AppendLine($lines[$i])
}
[void]$sb2.AppendLine("")

# link_android_executable: lines 1681-2041 (0-idx: 1680-2040)
for ($i = 1680; $i -le 2040; $i++) {
    [void]$sb2.AppendLine($lines[$i])
}
[void]$sb2.AppendLine("")

# link_object_to_executable: lines 2042-2980 (0-idx: 2041-2979)
for ($i = 2041; $i -le 2979; $i++) {
    [void]$sb2.AppendLine($lines[$i])
}
[void]$sb2.AppendLine("")
[void]$sb2.AppendLine("    } // namespace driver")
[void]$sb2.AppendLine("} // namespace sad")

[IO.File]::WriteAllText("$srcDir\compiler_driver_linker.cpp", $sb2.ToString(), $enc)
$c2 = ($sb2.ToString() -split "`n").Count
Write-Host "  -> linker: ~$c2 lines"

# ═══════════════════════════════════════════════════════════════════════════
# FILE 3: compiler_driver_build_utils.cpp (utils: cleanup, sysroot, compile_c)
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating build_utils file..."
$sb3 = [System.Text.StringBuilder]::new()
[void]$sb3.AppendLine("// ============================================================================")
[void]$sb3.AppendLine("// compiler_driver_build_utils.cpp")
[void]$sb3.AppendLine("// (AR) Build utilities: cleanup, sysroot, C compilation")
[void]$sb3.AppendLine("// (EN) Build utilities: temp file cleanup, Android sysroot, C-to-obj compile")
[void]$sb3.AppendLine("// ============================================================================")
[void]$sb3.AppendLine("")
[void]$sb3.AppendLine('#include "compiler_driver.h"')
[void]$sb3.AppendLine('#include "../../shared/utils/include/utf8_utils.h"')
[void]$sb3.AppendLine('#include <llvm/Support/FileSystem.h>')
[void]$sb3.AppendLine('#include <llvm/Support/raw_ostream.h>')
[void]$sb3.AppendLine("")
[void]$sb3.AppendLine("#ifdef _WIN32")
[void]$sb3.AppendLine("#include <windows.h>")
[void]$sb3.AppendLine("#undef ERROR")
[void]$sb3.AppendLine("#undef FATAL")
[void]$sb3.AppendLine("#endif")
[void]$sb3.AppendLine("")
[void]$sb3.AppendLine("#include <iostream>")
[void]$sb3.AppendLine("#include <fstream>")
[void]$sb3.AppendLine("#include <sstream>")
[void]$sb3.AppendLine("#include <cstdlib>")
[void]$sb3.AppendLine("#include <cstdio>")
[void]$sb3.AppendLine("#include <filesystem>")
[void]$sb3.AppendLine("#include <algorithm>")
[void]$sb3.AppendLine("")
[void]$sb3.AppendLine("namespace sad")
[void]$sb3.AppendLine("{")
[void]$sb3.AppendLine("    namespace driver")
[void]$sb3.AppendLine("    {")
[void]$sb3.AppendLine("        namespace colors")
[void]$sb3.AppendLine("        {")
[void]$sb3.AppendLine("            extern const char *RESET;")
[void]$sb3.AppendLine("            extern const char *BOLD;")
[void]$sb3.AppendLine("            extern const char *RED;")
[void]$sb3.AppendLine("            extern const char *GREEN;")
[void]$sb3.AppendLine("            extern const char *YELLOW;")
[void]$sb3.AppendLine("            extern const char *BLUE;")
[void]$sb3.AppendLine("            extern const char *MAGENTA;")
[void]$sb3.AppendLine("            extern const char *CYAN;")
[void]$sb3.AppendLine("            extern const char *WHITE;")
[void]$sb3.AppendLine("        }")
[void]$sb3.AppendLine("")

# cleanup_temp_files: lines 899-1066 (0-idx: 898-1065)
for ($i = 898; $i -le 1065; $i++) {
    [void]$sb3.AppendLine($lines[$i])
}
[void]$sb3.AppendLine("")

# get_android_sysroot: lines 1067-1174 (0-idx: 1066-1173)
for ($i = 1066; $i -le 1173; $i++) {
    [void]$sb3.AppendLine($lines[$i])
}
[void]$sb3.AppendLine("")

# compile_c_to_obj: lines 1175-1341 (0-idx: 1174-1340)
for ($i = 1174; $i -le 1340; $i++) {
    [void]$sb3.AppendLine($lines[$i])
}
[void]$sb3.AppendLine("")
[void]$sb3.AppendLine("    } // namespace driver")
[void]$sb3.AppendLine("} // namespace sad")

[IO.File]::WriteAllText("$srcDir\compiler_driver_build_utils.cpp", $sb3.ToString(), $enc)
$c3 = ($sb3.ToString() -split "`n").Count
Write-Host "  -> build_utils: ~$c3 lines"

Write-Host ""
Write-Host "=== SPLIT COMPLETE ==="
Write-Host "Backend (core):   ~$c1 lines"
Write-Host "Linker:           ~$c2 lines"
Write-Host "Build utils:      ~$c3 lines"
$sumAll = $c1 + $c2 + $c3
Write-Host "Total:            ~$sumAll lines (original: $total)"
