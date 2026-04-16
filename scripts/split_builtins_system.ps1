# =============================================================================
# Split sir_builder_builtins_system.cpp into 6 files
# =============================================================================
$ErrorActionPreference = "Stop"
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

$srcDir = "C:\s_lang\s-programming-language\compiler_new\src\frontend"
$srcFile = "$srcDir\sir_builder_builtins_system.cpp"

$lines = [IO.File]::ReadAllLines($srcFile, [Text.Encoding]::UTF8)
$total = $lines.Count
Write-Host "Source file: $total lines"

# Section boundaries (0-indexed):
# Hardware/Ports/Memory: lines 0..456  (keep in main, lines 1-457 in 1-based)
# Embedded:   lines 457..828  (458-829 in 1-based)  
# Security:   lines 829..1126 (830-1127 in 1-based)
# FFI:        lines 1127..1516 (1128-1517 in 1-based)
# Async:      lines 1517..3533 (1518-3534 in 1-based)
# UI:         lines 3534..4128 (3535-4129 in 1-based)

# Common header for all sub-files
$header = @"
// ============================================================================
// {FILENAME}
// (AR) {DESCRIPTION_AR}
// (EN) {DESCRIPTION_EN}
// ============================================================================

#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <optional>

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {
"@

$footer = @"

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
"@

# ─── Helper: extract body lines between section markers ───
function Extract-Section {
    param([int]$startLine, [int]$endLine)
    # startLine/endLine are 1-based inclusive, content lines within the function
    $result = [System.Collections.Generic.List[string]]::new()
    for ($i = $startLine - 1; $i -le $endLine - 1; $i++) {
        $result.Add($lines[$i])
    }
    return $result
}

# ═══════════════════════════════════════════════════════════════════════════
# FILE 1: sir_builder_builtins_system.cpp (main dispatcher + hardware)
# lines 1-457 body + delegation calls at the end
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating main dispatcher file..."
$sb1 = [System.Text.StringBuilder]::new()

# Copy original header (lines 1-42, before the function)
for ($i = 0; $i -le 41; $i++) {
    [void]$sb1.AppendLine($lines[$i])
}

# Copy hardware section (lines 43-457, function start through hardware)
for ($i = 42; $i -le 456; $i++) {
    [void]$sb1.AppendLine($lines[$i])
}

# Add delegation calls
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // === Delegation to sub-functions ===")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // Embedded functions (serial, GPIO, timer, DMA, etc.)")
[void]$sb1.AppendLine("                auto embeddedResult = buildBuiltinSystem_Embedded(funcName, isUserDefinedFunction, argResults, argOperands);")
[void]$sb1.AppendLine("                if (embeddedResult) return embeddedResult;")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // Security functions (assert, hash, encrypt, etc.)")
[void]$sb1.AppendLine("                auto securityResult = buildBuiltinSystem_Security(funcName, isUserDefinedFunction, argResults, argOperands);")
[void]$sb1.AppendLine("                if (securityResult) return securityResult;")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // FFI functions (printf, malloc, fopen, etc.)")
[void]$sb1.AppendLine("                auto ffiResult = buildBuiltinSystem_FFI(funcName, isUserDefinedFunction, argResults, argOperands);")
[void]$sb1.AppendLine("                if (ffiResult) return ffiResult;")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // Async/Await functions (spawn, join, channel, etc.)")
[void]$sb1.AppendLine("                auto asyncResult = buildBuiltinSystem_Async(funcName, isUserDefinedFunction, argResults, argOperands);")
[void]$sb1.AppendLine("                if (asyncResult) return asyncResult;")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // UI Widget factories (column, row, text, button, etc.)")
[void]$sb1.AppendLine("                auto uiResult = buildBuiltinSystem_UI(funcName, isUserDefinedFunction, argResults, argOperands);")
[void]$sb1.AppendLine("                if (uiResult) return uiResult;")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // Not a system builtin")
[void]$sb1.AppendLine("                return std::nullopt;")
[void]$sb1.AppendLine("            }")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("        } // namespace SIR")
[void]$sb1.AppendLine("    } // namespace Compiler")
[void]$sb1.AppendLine("} // namespace Sad")

$enc = [System.Text.UTF8Encoding]::new($false) # UTF-8 no BOM
[IO.File]::WriteAllText($srcFile, $sb1.ToString(), $enc)
$newCount = ($sb1.ToString() -split "`n").Count
Write-Host "  -> main dispatcher: ~$newCount lines"

# ═══════════════════════════════════════════════════════════════════════════
# FILE 2: sir_builder_builtins_embedded.cpp (lines 458-829)
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating embedded file..."
$sb2 = [System.Text.StringBuilder]::new()
$h2 = $header.Replace("{FILENAME}", "sir_builder_builtins_embedded.cpp")
$h2 = $h2.Replace("{DESCRIPTION_AR}", "Embedded builtins: serial, GPIO, timer, DMA, memory barriers")
$h2 = $h2.Replace("{DESCRIPTION_EN}", "Advanced Embedded Builtin Functions (18 functions)")
[void]$sb2.AppendLine($h2)
[void]$sb2.AppendLine("")
[void]$sb2.AppendLine("            std::optional<BuildResult> SIRBuilder::buildBuiltinSystem_Embedded(")
[void]$sb2.AppendLine("                const std::string &funcName,")
[void]$sb2.AppendLine("                bool isUserDefinedFunction,")
[void]$sb2.AppendLine("                std::vector<BuildResult> &argResults,")
[void]$sb2.AppendLine("                std::vector<SIROperand> &argOperands)")
[void]$sb2.AppendLine("            {")

# Copy body from lines 458-828 (0-idx: 457-827), skip section header comments
# The actual if-blocks start around line 475 (after the section header comments)
for ($i = 457; $i -le 827; $i++) {
    [void]$sb2.AppendLine($lines[$i])
}
[void]$sb2.AppendLine("")
[void]$sb2.AppendLine("                return std::nullopt;")
[void]$sb2.AppendLine("            }")
[void]$sb2.Append($footer)

[IO.File]::WriteAllText("$srcDir\sir_builder_builtins_embedded.cpp", $sb2.ToString(), $enc)
$c2 = ($sb2.ToString() -split "`n").Count
Write-Host "  -> embedded: ~$c2 lines"

# ═══════════════════════════════════════════════════════════════════════════
# FILE 3: sir_builder_builtins_security.cpp (lines 830-1127)
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating security file..."
$sb3 = [System.Text.StringBuilder]::new()
$h3 = $header.Replace("{FILENAME}", "sir_builder_builtins_security.cpp")
$h3 = $h3.Replace("{DESCRIPTION_AR}", "Security builtins: assert, hash, encrypt, sanitize")
$h3 = $h3.Replace("{DESCRIPTION_EN}", "Security Builtin Functions (14 functions)")
[void]$sb3.AppendLine($h3)
[void]$sb3.AppendLine("")
[void]$sb3.AppendLine("            std::optional<BuildResult> SIRBuilder::buildBuiltinSystem_Security(")
[void]$sb3.AppendLine("                const std::string &funcName,")
[void]$sb3.AppendLine("                bool isUserDefinedFunction,")
[void]$sb3.AppendLine("                std::vector<BuildResult> &argResults,")
[void]$sb3.AppendLine("                std::vector<SIROperand> &argOperands)")
[void]$sb3.AppendLine("            {")

for ($i = 828; $i -le 1125; $i++) {
    [void]$sb3.AppendLine($lines[$i])
}
[void]$sb3.AppendLine("")
[void]$sb3.AppendLine("                return std::nullopt;")
[void]$sb3.AppendLine("            }")
[void]$sb3.Append($footer)

[IO.File]::WriteAllText("$srcDir\sir_builder_builtins_security.cpp", $sb3.ToString(), $enc)
$c3 = ($sb3.ToString() -split "`n").Count
Write-Host "  -> security: ~$c3 lines"

# ═══════════════════════════════════════════════════════════════════════════
# FILE 4: sir_builder_builtins_ffi.cpp (lines 1128-1517)
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating FFI file..."
$sb4 = [System.Text.StringBuilder]::new()
$h4 = $header.Replace("{FILENAME}", "sir_builder_builtins_ffi.cpp")
$h4 = $h4.Replace("{DESCRIPTION_AR}", "FFI builtins: printf, malloc, fopen, C/C++ interop")
$h4 = $h4.Replace("{DESCRIPTION_EN}", "C/C++ Foreign Function Interface (20 functions)")
[void]$sb4.AppendLine($h4)
[void]$sb4.AppendLine("")
[void]$sb4.AppendLine("            std::optional<BuildResult> SIRBuilder::buildBuiltinSystem_FFI(")
[void]$sb4.AppendLine("                const std::string &funcName,")
[void]$sb4.AppendLine("                bool isUserDefinedFunction,")
[void]$sb4.AppendLine("                std::vector<BuildResult> &argResults,")
[void]$sb4.AppendLine("                std::vector<SIROperand> &argOperands)")
[void]$sb4.AppendLine("            {")

for ($i = 1126; $i -le 1515; $i++) {
    [void]$sb4.AppendLine($lines[$i])
}
[void]$sb4.AppendLine("")
[void]$sb4.AppendLine("                return std::nullopt;")
[void]$sb4.AppendLine("            }")
[void]$sb4.Append($footer)

[IO.File]::WriteAllText("$srcDir\sir_builder_builtins_ffi.cpp", $sb4.ToString(), $enc)
$c4 = ($sb4.ToString() -split "`n").Count
Write-Host "  -> FFI: ~$c4 lines"

# ═══════════════════════════════════════════════════════════════════════════
# FILE 5: sir_builder_builtins_async.cpp (lines 1518-3534)
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating async file..."
$sb5 = [System.Text.StringBuilder]::new()
$h5 = $header.Replace("{FILENAME}", "sir_builder_builtins_async.cpp")
$h5 = $h5.Replace("{DESCRIPTION_AR}", "Async/Await builtins: spawn, join, channel, select, APIC")
$h5 = $h5.Replace("{DESCRIPTION_EN}", "Async/Await & Concurrency Functions (23+ functions)")
[void]$sb5.AppendLine($h5)
[void]$sb5.AppendLine("")
[void]$sb5.AppendLine("            std::optional<BuildResult> SIRBuilder::buildBuiltinSystem_Async(")
[void]$sb5.AppendLine("                const std::string &funcName,")
[void]$sb5.AppendLine("                bool isUserDefinedFunction,")
[void]$sb5.AppendLine("                std::vector<BuildResult> &argResults,")
[void]$sb5.AppendLine("                std::vector<SIROperand> &argOperands)")
[void]$sb5.AppendLine("            {")

for ($i = 1516; $i -le 3532; $i++) {
    [void]$sb5.AppendLine($lines[$i])
}
[void]$sb5.AppendLine("")
[void]$sb5.AppendLine("                return std::nullopt;")
[void]$sb5.AppendLine("            }")
[void]$sb5.Append($footer)

[IO.File]::WriteAllText("$srcDir\sir_builder_builtins_async.cpp", $sb5.ToString(), $enc)
$c5 = ($sb5.ToString() -split "`n").Count
Write-Host "  -> async: ~$c5 lines"

# ═══════════════════════════════════════════════════════════════════════════
# FILE 6: sir_builder_builtins_ui.cpp (lines 3535-4125, before final return)
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating UI file..."
$sb6 = [System.Text.StringBuilder]::new()
$h6 = $header.Replace("{FILENAME}", "sir_builder_builtins_ui.cpp")
$h6 = $h6.Replace("{DESCRIPTION_AR}", "UI Widget Factory builtins: column, row, text, button, app")
$h6 = $h6.Replace("{DESCRIPTION_EN}", "Unified UI Widget Factory Functions")
[void]$sb6.AppendLine($h6)
[void]$sb6.AppendLine("")
[void]$sb6.AppendLine("            std::optional<BuildResult> SIRBuilder::buildBuiltinSystem_UI(")
[void]$sb6.AppendLine("                const std::string &funcName,")
[void]$sb6.AppendLine("                bool isUserDefinedFunction,")
[void]$sb6.AppendLine("                std::vector<BuildResult> &argResults,")
[void]$sb6.AppendLine("                std::vector<SIROperand> &argOperands)")
[void]$sb6.AppendLine("            {")

# Lines 3535-4125 (skip the last 4 lines: return std::nullopt; } namespace closes)
# The actual "return std::nullopt;" at line 4126, "}" at 4127, namespace closes 4128-4129
for ($i = 3533; $i -le 4121; $i++) {
    [void]$sb6.AppendLine($lines[$i])
}
[void]$sb6.AppendLine("")
[void]$sb6.AppendLine("                return std::nullopt;")
[void]$sb6.AppendLine("            }")
[void]$sb6.Append($footer)

[IO.File]::WriteAllText("$srcDir\sir_builder_builtins_ui.cpp", $sb6.ToString(), $enc)
$c6 = ($sb6.ToString() -split "`n").Count
Write-Host "  -> UI: ~$c6 lines"

Write-Host ""
Write-Host "=== SPLIT COMPLETE ==="
Write-Host "Main (hardware + dispatcher): ~$newCount lines"
Write-Host "Embedded:  ~$c2 lines"
Write-Host "Security:  ~$c3 lines"
Write-Host "FFI:       ~$c4 lines"
Write-Host "Async:     ~$c5 lines"
Write-Host "UI:        ~$c6 lines"
$sumAll = $newCount + $c2 + $c3 + $c4 + $c5 + $c6
Write-Host "Total:     ~$sumAll lines (original: $total)"
