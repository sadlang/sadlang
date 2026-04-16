# split v5 - No Arabic in AppendLine calls
$src = "c:\s_lang\s-programming-language\compiler_new\src\frontend\sir_builder_helpers.cpp"
$a = [IO.File]::ReadAllLines($src, [System.Text.Encoding]::UTF8)
$n = $a.Count
Write-Host "Source lines: $n"

$inc = @"
#include <string>
#include "sir_builder.h"
#include "module_nodes.h"
#include "module_resolver.h"
#include "lexer_core.h"
#include "parser_core.h"
#include "pattern_nodes.h"
#include "directive_nodes.h"
#include "../../../shared/utils/include/utf8_utils.h"
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <limits>
#include <functional>
#include <set>
"@

$nso = @"

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

"@

$nsc = @"

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
"@

$hdr1 = "// ============================================================================" + "`r`n" + "// sir_builder_expressions_dispatch.cpp" + "`r`n" + "// ============================================================================" + "`r`n"

$hdr2 = "// ============================================================================" + "`r`n" + "// sir_builder_type_inference.cpp" + "`r`n" + "// ============================================================================" + "`r`n"

# --- File 1: expressions dispatch (0-indexed 1097-4717) ---
$sb1 = New-Object System.Text.StringBuilder(300000)
[void]$sb1.Append($hdr1)
[void]$sb1.Append($inc)
[void]$sb1.Append($nso)
for($i = 1097; $i -le 4717; $i++) { [void]$sb1.AppendLine($a[$i]) }
[void]$sb1.Append($nsc)
$p1 = "c:\s_lang\s-programming-language\compiler_new\src\frontend\sir_builder_expressions_dispatch.cpp"
[IO.File]::WriteAllText($p1, $sb1.ToString(), (New-Object System.Text.UTF8Encoding $false))
Write-Host "expressions_dispatch: $(($sb1.ToString() -split "`r`n").Count) lines"

# --- File 2: type inference (0-indexed 4719-5424) ---
$sb2 = New-Object System.Text.StringBuilder(80000)
[void]$sb2.Append($hdr2)
[void]$sb2.Append($inc)
[void]$sb2.Append($nso)
for($i = 4719; $i -le 5424; $i++) { [void]$sb2.AppendLine($a[$i]) }
[void]$sb2.Append($nsc)
$p2 = "c:\s_lang\s-programming-language\compiler_new\src\frontend\sir_builder_type_inference.cpp"
[IO.File]::WriteAllText($p2, $sb2.ToString(), (New-Object System.Text.UTF8Encoding $false))
Write-Host "type_inference: $(($sb2.ToString() -split "`r`n").Count) lines"

# --- File 0: trim helpers to lines 1-1097 + ns close ---
$sb0 = New-Object System.Text.StringBuilder(80000)
for($i = 0; $i -le 1096; $i++) { [void]$sb0.AppendLine($a[$i]) }
[void]$sb0.Append($nsc)
[IO.File]::WriteAllText($src, $sb0.ToString(), (New-Object System.Text.UTF8Encoding $false))
Write-Host "helpers: $(($sb0.ToString() -split "`r`n").Count) lines"
