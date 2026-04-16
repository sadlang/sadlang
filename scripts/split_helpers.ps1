# سكريبت تقسيم sir_builder_helpers.cpp
$src = "c:\s_lang\s-programming-language\compiler_new\src\frontend\sir_builder_helpers.cpp"
$dir = "c:\s_lang\s-programming-language\compiler_new\src\frontend"
$all = [IO.File]::ReadAllLines($src)
Write-Host "Source: $($all.Count) lines"

# Header مشترك لجميع الملفات الجديدة
$includes = @(
    '#include <string>',
    '#include "sir_builder.h"',
    '#include "module_nodes.h"',
    '#include "module_resolver.h"',
    '#include "lexer_core.h"',
    '#include "parser_core.h"',
    '#include "pattern_nodes.h"',
    '#include "directive_nodes.h"',
    '#include "../../../shared/utils/include/utf8_utils.h"',
    '#include <stdexcept>',
    '#include <iostream>',
    '#include <filesystem>',
    '#include <limits>',
    '#include <functional>',
    '#include <set>'
)

$nsOpen = @(
    '',
    'namespace Sad',
    '{',
    '    namespace Compiler',
    '    {',
    '        namespace SIR',
    '        {',
    ''
)

$nsClose = @(
    '',
    '        } // namespace SIR',
    '    } // namespace Compiler',
    '} // namespace Sad',
    ''
)

function Write-SplitFile($path, $description_ar, $description_en, $bodyLines) {
    $header = @(
        '// ============================================================================',
        "// (AR) $description_ar",
        "// (EN) $description_en",
        '// ============================================================================'
    )
    $content = $header + '' + $includes + $nsOpen + $bodyLines + $nsClose
    [IO.File]::WriteAllLines($path, $content, (New-Object System.Text.UTF8Encoding $false))
    Write-Host "Created: $([IO.Path]::GetFileName($path)) ($($content.Count) lines)"
}

# === ملف 1: sir_builder_expressions_dispatch.cpp (سطور 1098-4723, 0-idx: 1097-4722) ===
Write-SplitFile `
    "$dir\sir_builder_expressions_dispatch.cpp" `
    "بناء التعبيرات - توزيع أنواع التعبيرات على معالجاتها" `
    "Expression building - dispatch expressions to their handlers" `
    $all[1097..4722]

# === ملف 2: sir_builder_type_inference.cpp (سطور 4725-5434, 0-idx: 4724..end) ===
Write-SplitFile `
    "$dir\sir_builder_type_inference.cpp" `
    "استنتاج الأنواع - جمع المتغيرات الحرة ومسح مواقع الاستدعاء" `
    "Type inference - free variable collection and call site scanning" `
    $all[4724..($all.Count-1)]

# === تعديل الملف الأصلي: نبقي فقط سطور 1-1097 + إغلاق namespace ===
$keepLines = $all[0..1096]
$originalContent = $keepLines + $nsClose
[IO.File]::WriteAllLines($src, $originalContent, (New-Object System.Text.UTF8Encoding $false))
Write-Host "Updated: sir_builder_helpers.cpp ($($originalContent.Count) lines)"
