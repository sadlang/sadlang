# ======================================================================
# build_tests.ps1 - سكريبت بناء الاختبارات
# Build script for parser tests
# ======================================================================

Write-Host "`n===========================================================" -ForegroundColor Cyan
Write-Host "   Building Parser Tests / بناء اختبارات المحلل النحوي" -ForegroundColor Green
Write-Host "===========================================================`n" -ForegroundColor Cyan

# التأكد من وجود مجلد البناء
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

# الانتقال إلى مجلد البناء
Set-Location build

Write-Host "🔨 Compiling source files..." -ForegroundColor Yellow

# تجميع ملفات المصدر الأساسية
$sourceFiles = @(
    # Utils
    "../src/utils/string_utils.cpp",
    
    # Lexer
    "../src/lexer/token.cpp",
    "../src/lexer/lexer_core.cpp",
    "../src/lexer/lexer_keywords.cpp",
    
    # Parser
    "../src/parser/parser_core.cpp",
    "../src/parser/parser_core_impl.cpp",
    "../src/parser/parser_core_helpers.cpp",
    
    # AST
    "../src/parser/ast/ast_node.cpp",
    "../src/parser/ast/expressions.cpp",
    "../src/parser/ast/statements.cpp",
    "../src/parser/ast/declarations.cpp",
    "../src/parser/ast/ast_printer.cpp",
    
    # Error Management
    "../src/errors/error_manager.cpp",
    
    # Test Files
    "../tests/parser_tests/test_runner.cpp"
)

# خيارات المترجم
$compilerOptions = @(
    "-std=c++17",
    "-I../include",
    "-I../tests/parser_tests",
    "-o", "parser_tests.exe",
    "-DSAD_DEBUG_ENABLED"
)

# دمج جميع الملفات والخيارات
$allArgs = $sourceFiles + $compilerOptions

Write-Host "   Compiling with g++..." -ForegroundColor White

# تنفيذ الترجمة
$output = & g++ @allArgs 2>&1

if ($LASTEXITCODE -eq 0) {
    Write-Host "`n✅ Build successful! / البناء نجح!" -ForegroundColor Green
    Write-Host "   Output: build\parser_tests.exe" -ForegroundColor White
    
    Write-Host "`n===========================================================" -ForegroundColor Cyan
    Write-Host "   Running Tests / تشغيل الاختبارات" -ForegroundColor Green
    Write-Host "===========================================================`n" -ForegroundColor Cyan
    
    # تشغيل الاختبارات
    & .\parser_tests.exe
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "`n✅ All tests passed! / جميع الاختبارات نجحت!" -ForegroundColor Green
    } else {
        Write-Host "`n❌ Some tests failed / بعض الاختبارات فشلت" -ForegroundColor Red
    }
} else {
    Write-Host "`n❌ Build failed! / البناء فشل!" -ForegroundColor Red
    Write-Host "`nErrors:" -ForegroundColor Red
    Write-Host $output -ForegroundColor Red
}

Write-Host "`n===========================================================" -ForegroundColor Cyan

# العودة إلى المجلد الأصلي
Set-Location ..
