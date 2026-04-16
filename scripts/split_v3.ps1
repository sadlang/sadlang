$src = "c:\s_lang\s-programming-language\compiler_new\src\frontend\sir_builder_helpers.cpp"
$dir = "c:\s_lang\s-programming-language\compiler_new\src\frontend"
$all = [IO.File]::ReadAllLines($src)
Write-Host "Source: $($all.Count) lines"

$inc = @'
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
'@

$nsO = @'

namespace Sad
{
    namespace Compiler
    {
        namespace SIR
        {

'@

$nsC = @'

        } // namespace SIR
    } // namespace Compiler
} // namespace Sad
'@

# --- File 1: expressions dispatch (lines 1098-4723 = 0-idx 1097-4722) ---
$h1 = @'
// ============================================================================
// (AR) بناء التعبيرات - توزيع أنواع التعبيرات على معالجاتها
// (EN) Expression building - dispatch expressions to their handlers
// ============================================================================

'@
$body1 = [string]::Join("`r`n", $all[1097..4716])
$f1 = $h1 + $inc + $nsO + $body1 + $nsC
[IO.File]::WriteAllText("$dir\sir_builder_expressions_dispatch.cpp", $f1, (New-Object System.Text.UTF8Encoding $false))
Write-Host "expressions_dispatch: $(($f1 -split "`r`n").Count) lines"

# --- File 2: type inference (lines 4725 to end = 0-idx 4718..end) ---
$h2 = @'
// ============================================================================
// (AR) استنتاج الأنواع - جمع المتغيرات الحرة ومسح مواقع الاستدعاء
// (EN) Type inference - free variable collection and call site scanning
// ============================================================================

'@
$lastIdx = $all.Count - 4  # skip the closing namespace lines
$body2 = [string]::Join("`r`n", $all[4718..$lastIdx])
$f2 = $h2 + $inc + $nsO + $body2 + $nsC
[IO.File]::WriteAllText("$dir\sir_builder_type_inference.cpp", $f2, (New-Object System.Text.UTF8Encoding $false))
Write-Host "type_inference: $(($f2 -split "`r`n").Count) lines"

# --- File 0: trim original to keep only helpers (lines 1-1097) ---
# Keep original header + includes + namespace + helpers functions + close namespace
$keep = [string]::Join("`r`n", $all[0..1096])
$f0 = $keep + "`r`n" + $nsC
[IO.File]::WriteAllText($src, $f0, (New-Object System.Text.UTF8Encoding $false))
Write-Host "helpers (trimmed): $(($f0 -split "`r`n").Count) lines"
