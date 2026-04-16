# split v4 - Direct line extraction
$src = "c:\s_lang\s-programming-language\compiler_new\src\frontend\sir_builder_helpers.cpp"
$a = [IO.File]::ReadAllLines($src)
$n = $a.Count
Write-Host "Source lines: $n"

# -- shared text blocks --
$inc = "#include <string>`r`n#include `"sir_builder.h`"`r`n#include `"module_nodes.h`"`r`n#include `"module_resolver.h`"`r`n#include `"lexer_core.h`"`r`n#include `"parser_core.h`"`r`n#include `"pattern_nodes.h`"`r`n#include `"directive_nodes.h`"`r`n#include `"../../../shared/utils/include/utf8_utils.h`"`r`n#include <stdexcept>`r`n#include <iostream>`r`n#include <filesystem>`r`n#include <limits>`r`n#include <functional>`r`n#include <set>"
$nso = "`r`nnamespace Sad`r`n{`r`n    namespace Compiler`r`n    {`r`n        namespace SIR`r`n        {`r`n"
$nsc = "`r`n        } // namespace SIR`r`n    } // namespace Compiler`r`n} // namespace Sad`r`n"

# -- File 1: expressions dispatch (1-indexed lines 1098-4718 = 0-indexed 1097-4717) --
$sb1 = New-Object System.Text.StringBuilder(300000)
[void]$sb1.AppendLine("// ============================================================================")
[void]$sb1.AppendLine("// (AR) بناء التعبيرات - توزيع أنواع التعبيرات على معالجاتها")
[void]$sb1.AppendLine("// (EN) Expression building - dispatch expressions to their handlers")
[void]$sb1.AppendLine("// ============================================================================")
[void]$sb1.AppendLine("")
[void]$sb1.AppendLine($inc)
[void]$sb1.AppendLine($nso)
for($i = 1097; $i -le 4717; $i++) { [void]$sb1.AppendLine($a[$i]) }
[void]$sb1.AppendLine($nsc)
[IO.File]::WriteAllText("c:\s_lang\s-programming-language\compiler_new\src\frontend\sir_builder_expressions_dispatch.cpp", $sb1.ToString(), (New-Object System.Text.UTF8Encoding $false))
$c1 = ($sb1.ToString() -split "`r`n").Count
Write-Host "expressions_dispatch: $c1 lines"

# -- File 2: type inference (1-indexed lines 4720-5425 = 0-indexed 4719-5424) --
$sb2 = New-Object System.Text.StringBuilder(80000)
[void]$sb2.AppendLine("// ============================================================================")
[void]$sb2.AppendLine("// (AR) استنتاج الأنواع - جمع المتغيرات الحرة ومسح مواقع الاستدعاء")
[void]$sb2.AppendLine("// (EN) Type inference - free variable collection and call site scanning")
[void]$sb2.AppendLine("// ============================================================================")
[void]$sb2.AppendLine("")
[void]$sb2.AppendLine($inc)
[void]$sb2.AppendLine($nso)
for($i = 4719; $i -le 5424; $i++) { [void]$sb2.AppendLine($a[$i]) }
[void]$sb2.AppendLine($nsc)
[IO.File]::WriteAllText("c:\s_lang\s-programming-language\compiler_new\src\frontend\sir_builder_type_inference.cpp", $sb2.ToString(), (New-Object System.Text.UTF8Encoding $false))
$c2 = ($sb2.ToString() -split "`r`n").Count
Write-Host "type_inference: $c2 lines"

# -- File 0: trim original helpers to lines 1-1097 only --
$sb0 = New-Object System.Text.StringBuilder(80000)
for($i = 0; $i -le 1096; $i++) { [void]$sb0.AppendLine($a[$i]) }
[void]$sb0.AppendLine($nsc)
[IO.File]::WriteAllText($src, $sb0.ToString(), (New-Object System.Text.UTF8Encoding $false))
$c0 = ($sb0.ToString() -split "`r`n").Count
Write-Host "helpers (trimmed): $c0 lines"
Write-Host "TOTAL: $($c0 + $c1 + $c2) (was $n)"
