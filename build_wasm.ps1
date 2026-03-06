﻿# ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€
# build_wasm.ps1 ג€" ״¨†״§״¡ sad.wasm …״¨״§״´״±״© ״¨״§״³״×״®״¯״§… emcc
# ״§„״§״³״×״®״¯״§…:  .\build_wasm.ps1  [-O <0|1|2>]  [-OutDir <…״³״§״±>]
# ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€
param(
    [string]$O       = "1",
    [string]$OutDir  = "$PSScriptRoot\website\docs\public"
)

Set-Location $PSScriptRoot

# ג"€ג"€ 1. ״×״¹„ Emscripten ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€
$env:PATH = "C:\emsdk;C:\emsdk\upstream\emscripten;" + $env:PATH
if (-not (Get-Command emcc -ErrorAction SilentlyContinue)) {
    Write-Error "„… ״¹״«״± ״¹„‰ emcc ג€" ״×״£ƒ״¯ …† ״×״«״¨״× emsdk  C:\emsdk"
    exit 1
}
Write-Host "ג" $(emcc --version 2>&1 | Select-Object -First 1)" -ForegroundColor Green

# ג"€ג"€ 2. …״¬„״¯ ״§„״¨†״§״¡ ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€
$build = "$PSScriptRoot\build_wasm"
New-Item -ItemType Directory -Force -Path $build | Out-Null

# ג"€ג"€ 3. ‚״§״¦…״© ״§„…״µ״§״¯״± (״¨״¯ˆ† SDL2/HTTP/DB/״±״³ˆ…״§״×/†״¸״§… …†״®״¶ ״§„…״³״×ˆ‰) ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€
$R = $PSScriptRoot
$srcs = @(
    # ״£״¯ˆ״§״× …״³״§״¹״¯״©
    "$R\shared\utils\src\string_utils.cpp",

    # ״§„…״­„„ ״§„…״¹״¬…
    "$R\shared\lexer\src\token.cpp",
    "$R\shared\lexer\src\lexer_core.cpp",
    "$R\shared\lexer\src\lexer_keywords.cpp",

    # ״§„…״­„„ ״§„†״­ˆ
    "$R\shared\parser\src\parser_core.cpp",
    "$R\shared\parser\src\parser_core_main.cpp",
    "$R\shared\parser\src\parser_declarations.cpp",
    "$R\shared\parser\src\parser_statements.cpp",
    "$R\shared\parser\src\parser_expressions.cpp",
    "$R\shared\parser\src\parser_advanced.cpp",
    "$R\shared\parser\src\parser_core_helpers.cpp",
    "$R\shared\parser\src\parser_core_oop.cpp",
    "$R\shared\parser\src\parser_modules.cpp",

    # ״§„״´״¬״±״© ״§„†״­ˆ״©
    "$R\shared\ast\src\ast_node.cpp",
    "$R\shared\ast\src\ast_printer.cpp",
    "$R\shared\ast\src\expressions.cpp",
    "$R\shared\ast\src\statements.cpp",
    "$R\shared\ast\src\declarations.cpp",
    "$R\shared\ast\src\property_nodes.cpp",
    "$R\shared\ast\src\module_nodes.cpp",
    "$R\shared\ast\src\type_nodes.cpp",
    "$R\shared\ast\src\class_nodes.cpp",

    # ״§„״£†ˆ״§״¹
    "$R\shared\types\src\value.cpp",
    "$R\shared\types\src\generator.cpp",
    "$R\shared\types\src\class_type.cpp",
    "$R\shared\types\src\object_instance.cpp",

    # ״¥״¯״§״±״© ״§„״£״®״·״§״¡
    "$R\shared\errors\src\error_codes.cpp",
    "$R\shared\errors\src\diagnostic.cpp",
    "$R\shared\errors\src\error_manager.cpp",
    "$R\shared\errors\src\smart_errors.cpp",
    "$R\shared\errors\src\smart_suggestions.cpp",
    "$R\shared\errors\src\smart_type_explanations.cpp",
    "$R\shared\errors\src\smart_multi_error.cpp",
    "$R\shared\errors\src\smart_fix_suggestions.cpp",
    "$R\shared\errors\src\smart_teacher_mode.cpp",
    "$R\shared\errors\src\smart_error_recovery.cpp",
    "$R\shared\errors\src\smart_cascade_prevention.cpp",

    # ״§„ˆ״­״¯״§״×
    "$R\shared\modules\src\module.cpp",
    "$R\shared\modules\src\search_paths.cpp",
    "$R\shared\modules\src\module_cache.cpp",
    "$R\shared\modules\src\dependency_graph.cpp",
    "$R\shared\modules\src\module_loader.cpp",
    "$R\shared\modules\src\module_resolver.cpp",
    "$R\shared\modules\src\symbol_resolver.cpp",
    "$R\shared\modules\src\module_validator.cpp",

    # ״§„…״³״± ג€" ״§„†ˆ״§״©
    "$R\interpreter_new\src\core\interpreter_core.cpp",
    "$R\interpreter_new\src\visitors\expression_evaluator_core.cpp",
    "$R\interpreter_new\src\visitors\expression_evaluator_calls.cpp",
    "$R\interpreter_new\src\visitors\expression_evaluator_oop.cpp",
    "$R\interpreter_new\src\visitors\expression_evaluator_members.cpp",
    "$R\interpreter_new\src\visitors\statement_executor.cpp",
    "$R\interpreter_new\src\visitors\statement_executor_oop.cpp",
    "$R\interpreter_new\src\visitors\statement_executor_modules.cpp",
    "$R\interpreter_new\src\exception.cpp",
    "$R\interpreter_new\src\ast_printer.cpp",

    # ״§„…״¯״±ˆ†
    "$R\interpreter_new\src\managers\variable_manager.cpp",
    "$R\interpreter_new\src\managers\function_manager.cpp",
    "$R\interpreter_new\src\managers\scope_manager.cpp",
    "$R\interpreter_new\src\managers\ownership_manager.cpp",
    "$R\interpreter_new\src\managers\class_manager.cpp",
    "$R\interpreter_new\src\managers\object_manager.cpp",

    # OOP
    "$R\interpreter_new\src\oop\interpreter_classes.cpp",
    "$R\interpreter_new\src\oop\interpreter_classes_fixed.cpp",

    # …״³״¬‘„״§״× ״§„…״¯…״¬״§״× (1ג€"38)
    "$R\interpreter_new\src\builtins\builtin_registry.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part1.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part2.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part3.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part4.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part5.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part6.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part7.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part8.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part9.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part10.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part11.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part12.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part13.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part14.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part15.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part16.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part17.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part18.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part19.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part20.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part21.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part22.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part23.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part24.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part25.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part26.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part27.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part28.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part29.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part30.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part31.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part32.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part33.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part34.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part35.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part36.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part37.cpp",
    "$R\interpreter_new\src\builtins\builtin_registry_part38.cpp",

    # ״§„…ƒ״×״¨״© ״§„‚״§״³״© (״§„†ˆ״§״© ‚״·)
    "$R\stdlib\core\builtins.cpp",
    "$R\stdlib\core\stdlib_manager.cpp",
    "$R\stdlib\core\type_functions.cpp",
    "$R\stdlib\core\array_functions.cpp",
    "$R\stdlib\core\other_functions.cpp",
    "$R\stdlib\string\string_functions.cpp",
    "$R\stdlib\math\math_functions.cpp",
    "$R\stdlib\math\advanced_math.cpp",
    "$R\stdlib\io\io_functions.cpp",

    # †‚״·״© ״¯״®ˆ„ WASM
    "$R\tools\wasm\sad_wasm.cpp"
)

# ג"€ג"€ 4. …״³״§״±״§״× ״§„‡״¯״± ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€
$includes = @(
    $R,
    "$R\shared\utils\include",
    "$R\shared\lexer\include",
    "$R\shared\parser\include",
    "$R\shared\ast\include",
    "$R\shared\types\include",
    "$R\shared\errors\include",
    "$R\shared\modules\include",
    "$R\interpreter_new\include",
    "$R\interpreter_new\include\core",
    "$R\interpreter_new\include\visitors",
    "$R\interpreter_new\include\managers",
    "$R\stdlib\core",
    "$R\stdlib\string",
    "$R\stdlib\math",
    "$R\stdlib\io"
) | ForEach-Object { "-I$_" }

# ג"€ג"€ 5. ״·״¨״§״¹״© ״§„…„״§״× ״§„…‚ˆ״¯״© ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€
$missing = $srcs | Where-Object { -not (Test-Path $_) }
if ($missing) {
    Write-Warning "״§„…„״§״× ״§„…‚ˆ״¯״©:"
    $missing | ForEach-Object { Write-Warning "  MISSING: $_" }
}

# ג"€ג"€ 6. ״×״¹״±״§״× ״§„״×״±״¬…״© ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€
$defs = @(
    "-DSAD_WASM_BUILD=1",
    "-DHAD_GRAPHICS=0",
    "-DSAD_NO_FILESYSTEM=1",
    "-DSAD_NO_NETWORK=1",
    "-DSAD_NO_SDL2=1",
    "-DUNICODE=1"
)

# ג"€ג"€ 7. ״×†״° emcc ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€ג"€
$outJs    = "$build\sad.js"
$exported = '["_sad_execute","_sad_version","_malloc","_free"]'
$runtime  = '["cwrap","ccall","UTF8ToString","stringToUTF8","lengthBytesUTF8"]'

$flags = @(
    "-O$O",
    "-std=c++17",
    "-fexceptions",
    "-sWASM=1",
    "-sMODULARIZE=1",
    "-sEXPORT_NAME=SadWasm",
    "-sEXPORTED_FUNCTIONS=$exported",
    "-sEXPORTED_RUNTIME_METHODS=$runtime",
    "-sALLOW_MEMORY_GROWTH=1",
    "-sMAXIMUM_MEMORY=268435456",
    "-sSTACK_SIZE=5MB",
    "-sFORCE_FILESYSTEM=0",
    "--no-entry",
    "-o", $outJs
)

$cmd = @("emcc") + $defs + $includes + $srcs + $flags
Write-Host ""
Write-Host "ג–¶ ״¨״¯״¡ ״¨†״§״¡ sad.wasm (…״³״×ˆ‰ ״§„״×״­״³†: O$O) ..." -ForegroundColor Cyan
Write-Host "  ״§„…״µ״§״¯״±: $($srcs.Count) …„"
Write-Host "  ״§„…״®״±״¬״§״×: $outJs"
Write-Host ""

$startTime = Get-Date
& emcc @($defs + $includes + ($srcs | ForEach-Object { $_ }) + $flags)
$exitCode  = $LASTEXITCODE
$elapsed   = (Get-Date) - $startTime

if ($exitCode -eq 0) {
    Write-Host ""
    Write-Host "ג" ״§„†״¬״§״­  $([int]$elapsed.TotalSeconds)s" -ForegroundColor Green
    $jsSize   = [int]((Get-Item "$build\sad.js").Length   / 1KB)
    $wasmSize = [int]((Get-Item "$build\sad.wasm").Length / 1KB)
    Write-Host "  sad.js   = ${jsSize} KB"
    Write-Host "  sad.wasm = ${wasmSize} KB"

    # †״³״® ״§„…„״§״× „„…ˆ‚״¹
    New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
    Copy-Item "$build\sad.js"   "$OutDir\sad.js"   -Force
    Copy-Item "$build\sad.wasm" "$OutDir\sad.wasm" -Force
    Write-Host ""
    Write-Host "ג" ״×… ״§„†״³״® ״¥„‰: $OutDir" -ForegroundColor Green
    Write-Host ""
    Write-Host "  ״§„״®״·ˆ״© ״§„״×״§„״©: cd website ; npx vitepress build ." -ForegroundColor Yellow
} else {
    Write-Host ""
    Write-Host "ג— ״´„ ״§„״¨†״§״¡ (exit=$exitCode)" -ForegroundColor Red
    Write-Host "  ״±״§״¬״¹ ״§„״£״®״·״§״¡ ״£״¹„״§‡ ג€" …ƒ† ״×״¬״±״¨״©: .\build_wasm.ps1 -O 0" -ForegroundColor Yellow
    exit $exitCode
}

