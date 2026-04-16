# =============================================================================
# Split sir_builder_control_flow.cpp into 3 files
# =============================================================================
$ErrorActionPreference = "Stop"
[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

$srcDir = "C:\s_lang\s-programming-language\compiler_new\src\frontend"
$srcFile = "$srcDir\sir_builder_control_flow.cpp"

$lines = [IO.File]::ReadAllLines($srcFile, [Text.Encoding]::UTF8)
$total = $lines.Count
Write-Host "Source file: $total lines"

# Boundaries (1-based):
# Header/includes: 1-38
# buildReturnStatement: 39-322
# buildBreakStatement: 323-378
# buildContinueStatement: 379-445
# buildAssignment: 446-613
# buildLocalVariable: 614-879
# buildIfStatement: 880-1151
# buildMatchStatement: 1152-2717
# buildWhileLoop: 2718-2976
# buildForLoop: 2977-3210
# buildForRangeLoop: 3211-3928

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

# ═══════════════════════════════════════════════════════════════════════════
# FILE 1: sir_builder_control_flow.cpp (LOOPS: while, for, for-range)
# Lines 2718-3928 from original + header/includes
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating control_flow (loops)..."
$sb1 = [System.Text.StringBuilder]::new()
$h1 = $header.Replace("{FILENAME}", "sir_builder_control_flow.cpp")
$h1 = $h1.Replace("{DESC_AR}", "Loops: buildWhileLoop, buildForLoop, buildForRangeLoop")
$h1 = $h1.Replace("{DESC_EN}", "Loop control flow: while, for, for-range loops")
[void]$sb1.AppendLine($h1)

# Copy lines 2718-3925 (0-idx: 2717-3924), before namespace closes
for ($i = 2717; $i -le 3924; $i++) {
    [void]$sb1.AppendLine($lines[$i])
}
[void]$sb1.Append($footer)

[IO.File]::WriteAllText($srcFile, $sb1.ToString(), $enc)
$c1 = ($sb1.ToString() -split "`n").Count
Write-Host "  -> control_flow (loops): ~$c1 lines"

# ═══════════════════════════════════════════════════════════════════════════
# FILE 2: sir_builder_control_branch.cpp (BRANCHES: return, break, continue,
#          assignment, local variable, if)
# Lines 39-1151 from original
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating control_branch..."
$sb2 = [System.Text.StringBuilder]::new()
$h2 = $header.Replace("{FILENAME}", "sir_builder_control_branch.cpp")
$h2 = $h2.Replace("{DESC_AR}", "Branches: return, break, continue, assignment, localvar, if")
$h2 = $h2.Replace("{DESC_EN}", "Branch control flow: return, break, continue, assignment, local variables, if/else")
[void]$sb2.AppendLine($h2)

# Lines 39-1151 (0-idx: 38-1150)
for ($i = 38; $i -le 1150; $i++) {
    [void]$sb2.AppendLine($lines[$i])
}
[void]$sb2.Append($footer)

[IO.File]::WriteAllText("$srcDir\sir_builder_control_branch.cpp", $sb2.ToString(), $enc)
$c2 = ($sb2.ToString() -split "`n").Count
Write-Host "  -> control_branch: ~$c2 lines"

# ═══════════════════════════════════════════════════════════════════════════
# FILE 3: sir_builder_control_match.cpp (MATCH: buildMatchStatement)
# Lines 1152-2717 from original
# ═══════════════════════════════════════════════════════════════════════════
Write-Host "Creating control_match..."
$sb3 = [System.Text.StringBuilder]::new()
$h3 = $header.Replace("{FILENAME}", "sir_builder_control_match.cpp")
$h3 = $h3.Replace("{DESC_AR}", "Pattern matching: buildMatchStatement")
$h3 = $h3.Replace("{DESC_EN}", "Pattern matching: match/when statement compilation")
[void]$sb3.AppendLine($h3)

# Lines 1152-2717 (0-idx: 1151-2716)
for ($i = 1151; $i -le 2716; $i++) {
    [void]$sb3.AppendLine($lines[$i])
}
[void]$sb3.Append($footer)

[IO.File]::WriteAllText("$srcDir\sir_builder_control_match.cpp", $sb3.ToString(), $enc)
$c3 = ($sb3.ToString() -split "`n").Count
Write-Host "  -> control_match: ~$c3 lines"

Write-Host ""
Write-Host "=== SPLIT COMPLETE ==="
Write-Host "control_flow (loops):  ~$c1 lines"
Write-Host "control_branch:        ~$c2 lines"
Write-Host "control_match:         ~$c3 lines"
$sumAll = $c1 + $c2 + $c3
Write-Host "Total:                 ~$sumAll lines (original: $total)"
