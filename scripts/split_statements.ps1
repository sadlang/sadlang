# =============================================================================
# Split sir_builder_statements.cpp into 5 files
# =============================================================================
$ErrorActionPreference = "Stop"
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

$srcDir = "C:\s_lang\s-programming-language\compiler_new\src\frontend"
$srcFile = "$srcDir\sir_builder_statements.cpp"

$lines = [IO.File]::ReadAllLines($srcFile, [Text.Encoding]::UTF8)
$total = $lines.Count
Write-Host "Source file: $total lines"

$enc = [System.Text.UTF8Encoding]::new($false)

$header = @"
// ============================================================================
// {FILENAME}
// (AR) {DESC_AR}
// (EN) {DESC_EN}
// ============================================================================

#include <string>
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
#include <set>

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

# ═══════════════════════════════════════════════════════════════════════
# FILE 1: sir_builder_statements.cpp (main dispatcher + basic stmts)
# Lines 1-487 + delegation calls
# ═══════════════════════════════════════════════════════════════════════
Write-Host "Creating main dispatcher + basic..."
$sb1 = [System.Text.StringBuilder]::new()

# Copy original header + includes (lines 1-38) 
for ($i = 0; $i -le 37; $i++) {
    [void]$sb1.AppendLine($lines[$i])
}
[void]$sb1.AppendLine("")

# Copy function header comments + function up to line 486 (0-idx: 38-485)
for ($i = 38; $i -le 485; $i++) {
    [void]$sb1.AppendLine($lines[$i])
}

# Add delegation calls before the function ends
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // === Delegation to sub-functions ===")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // Exception handling (try/catch/finally, raise)")
[void]$sb1.AppendLine("                if (buildStatement_Exceptions(stmt)) return;")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // Generator statements (yield, with)")
[void]$sb1.AppendLine("                if (buildStatement_Generators(stmt)) return;")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // Type declarations (enum, struct, namespace, class)")
[void]$sb1.AppendLine("                if (buildStatement_Types(stmt)) return;")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("                // Advanced statements (test, atomic, macro, go, select, etc.)")
[void]$sb1.AppendLine("                if (buildStatement_Advanced(stmt)) return;")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("#ifndef NDEBUG")
[void]$sb1.AppendLine('                std::cout << "[DEBUG] Unknown statement type: " << typeid(*stmt).name() << std::endl;')
[void]$sb1.AppendLine("#endif")
[void]$sb1.AppendLine("            }")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine("        } // namespace SIR")
[void]$sb1.AppendLine("    } // namespace Compiler")
[void]$sb1.AppendLine("} // namespace Sad")

[IO.File]::WriteAllText($srcFile, $sb1.ToString(), $enc)
$c1 = ($sb1.ToString() -split "`n").Count
Write-Host "  -> main + basic: ~$c1 lines"

# ═══════════════════════════════════════════════════════════════════════
# FILE 2: sir_builder_stmt_exceptions.cpp (try/catch/finally, raise)
# Lines 488-1118
# ═══════════════════════════════════════════════════════════════════════
Write-Host "Creating exceptions file..."
$sb2 = [System.Text.StringBuilder]::new()
$h2 = $header.Replace("{FILENAME}", "sir_builder_stmt_exceptions.cpp")
$h2 = $h2.Replace("{DESC_AR}", "Exception handling: try/catch/finally, raise")
$h2 = $h2.Replace("{DESC_EN}", "Exception handling statements")
[void]$sb2.AppendLine($h2)
[void]$sb2.AppendLine("            bool SIRBuilder::buildStatement_Exceptions(AST::Statement *stmt)")
[void]$sb2.AppendLine("            {")

# Copy body lines 488-1118 (0-idx: 487-1117)
for ($i = 487; $i -le 1117; $i++) {
    # Replace "return;" with "return true;" for handled cases
    $line = $lines[$i]
    if ($line -match '^\s+return;\s*$') {
        $line = $line.Replace("return;", "return true;")
    }
    [void]$sb2.AppendLine($line)
}
[void]$sb2.AppendLine("")
[void]$sb2.AppendLine("                return false;")
[void]$sb2.AppendLine("            }")
[void]$sb2.Append($footer)

[IO.File]::WriteAllText("$srcDir\sir_builder_stmt_exceptions.cpp", $sb2.ToString(), $enc)
$c2 = ($sb2.ToString() -split "`n").Count
Write-Host "  -> exceptions: ~$c2 lines"

# ═══════════════════════════════════════════════════════════════════════
# FILE 3: sir_builder_stmt_generators.cpp (yield, with)
# Lines 1119-1534
# ═══════════════════════════════════════════════════════════════════════
Write-Host "Creating generators file..."
$sb3 = [System.Text.StringBuilder]::new()
$h3 = $header.Replace("{FILENAME}", "sir_builder_stmt_generators.cpp")
$h3 = $h3.Replace("{DESC_AR}", "Generator statements: yield, with (context manager)")
$h3 = $h3.Replace("{DESC_EN}", "Generator and context manager statements")
[void]$sb3.AppendLine($h3)
[void]$sb3.AppendLine("            bool SIRBuilder::buildStatement_Generators(AST::Statement *stmt)")
[void]$sb3.AppendLine("            {")

for ($i = 1118; $i -le 1533; $i++) {
    $line = $lines[$i]
    if ($line -match '^\s+return;\s*$') {
        $line = $line.Replace("return;", "return true;")
    }
    [void]$sb3.AppendLine($line)
}
[void]$sb3.AppendLine("")
[void]$sb3.AppendLine("                return false;")
[void]$sb3.AppendLine("            }")
[void]$sb3.Append($footer)

[IO.File]::WriteAllText("$srcDir\sir_builder_stmt_generators.cpp", $sb3.ToString(), $enc)
$c3 = ($sb3.ToString() -split "`n").Count
Write-Host "  -> generators: ~$c3 lines"

# ═══════════════════════════════════════════════════════════════════════
# FILE 4: sir_builder_stmt_types.cpp (enum, struct, namespace, class, destructor)
# Lines 1535-2227
# ═══════════════════════════════════════════════════════════════════════
Write-Host "Creating types file..."
$sb4 = [System.Text.StringBuilder]::new()
$h4 = $header.Replace("{FILENAME}", "sir_builder_stmt_types.cpp")
$h4 = $h4.Replace("{DESC_AR}", "Type declarations: enum, struct, namespace, class, destructor")
$h4 = $h4.Replace("{DESC_EN}", "Type declaration statements")
[void]$sb4.AppendLine($h4)
[void]$sb4.AppendLine("            bool SIRBuilder::buildStatement_Types(AST::Statement *stmt)")
[void]$sb4.AppendLine("            {")

for ($i = 1534; $i -le 2226; $i++) {
    $line = $lines[$i]
    if ($line -match '^\s+return;\s*$') {
        $line = $line.Replace("return;", "return true;")
    }
    [void]$sb4.AppendLine($line)
}
[void]$sb4.AppendLine("")
[void]$sb4.AppendLine("                return false;")
[void]$sb4.AppendLine("            }")
[void]$sb4.Append($footer)

[IO.File]::WriteAllText("$srcDir\sir_builder_stmt_types.cpp", $sb4.ToString(), $enc)
$c4 = ($sb4.ToString() -split "`n").Count
Write-Host "  -> types: ~$c4 lines"

# ═══════════════════════════════════════════════════════════════════════
# FILE 5: sir_builder_stmt_advanced.cpp (test, atomic, property, unsafe,
#          comptime, volatile, macro, go, select, extension, type alias,
#          re-export)
# Lines 2228-3428 (before Unknown debug + closing)
# ═══════════════════════════════════════════════════════════════════════
Write-Host "Creating advanced file..."
$sb5 = [System.Text.StringBuilder]::new()
$h5 = $header.Replace("{FILENAME}", "sir_builder_stmt_advanced.cpp")
$h5 = $h5.Replace("{DESC_AR}", "Advanced: test, atomic, property, macro, goroutine, select, extension")
$h5 = $h5.Replace("{DESC_EN}", "Advanced statement types: testing, concurrency, metaprogramming")
[void]$sb5.AppendLine($h5)
[void]$sb5.AppendLine("            bool SIRBuilder::buildStatement_Advanced(AST::Statement *stmt)")
[void]$sb5.AppendLine("            {")

# Lines 2228-3421 (0-idx: 2227-3420) — stop before the Unknown debug message
for ($i = 2227; $i -le 3420; $i++) {
    $line = $lines[$i]
    if ($line -match '^\s+return;\s*$') {
        $line = $line.Replace("return;", "return true;")
    }
    [void]$sb5.AppendLine($line)
}
[void]$sb5.AppendLine("")
[void]$sb5.AppendLine("                return false;")
[void]$sb5.AppendLine("            }")
[void]$sb5.Append($footer)

[IO.File]::WriteAllText("$srcDir\sir_builder_stmt_advanced.cpp", $sb5.ToString(), $enc)
$c5 = ($sb5.ToString() -split "`n").Count
Write-Host "  -> advanced: ~$c5 lines"

Write-Host ""
Write-Host "=== SPLIT COMPLETE ==="
Write-Host "Main + basic:   ~$c1 lines"
Write-Host "Exceptions:     ~$c2 lines"
Write-Host "Generators:     ~$c3 lines"
Write-Host "Types:          ~$c4 lines"
Write-Host "Advanced:       ~$c5 lines"
$sumAll = $c1 + $c2 + $c3 + $c4 + $c5
Write-Host "Total:          ~$sumAll lines (original: $total)"
