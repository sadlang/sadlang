@echo off
REM ============================================================================
REM ملف: build_interpreter.bat
REM File: build_interpreter.bat
REM ============================================================================
REM الوصف بالعربية:
REM   سكريبت بناء مفسر لغة ص البرمجية. يقوم بتجميع جميع ملفات المصدر
REM   اللازمة وإنشاء الملف التنفيذي s_interpreter.exe في مجلد build/
REM
REM English Description:
REM   Build script for S Language interpreter. Compiles all necessary source
REM   files and creates s_interpreter.exe executable in build/ directory
REM
REM الاستخدام / Usage:
REM   build_interpreter.bat                - بناء المفسر / Build interpreter
REM   build_interpreter.bat clean          - تنظيف ثم بناء / Clean then build
REM
REM المتطلبات / Requirements:
REM   - g++ (GCC compiler) مع دعم C++17 / with C++17 support
REM   - w64devkit أو MinGW / or MinGW
REM
REM المؤلف / Author: S Language Development Team
REM التاريخ / Date: 22 نوفمبر 2025 / November 22, 2025
REM الإصدار / Version: 1.0
REM ============================================================================

REM ────────────────────────────────────────────────────────────────────────
REM إعدادات الألوان للطباعة / Color settings for output
REM ────────────────────────────────────────────────────────────────────────
set ESC=[
set RESET=%ESC%0m
set GREEN=%ESC%32m
set RED=%ESC%31m
set YELLOW=%ESC%33m
set BLUE=%ESC%34m
set CYAN=%ESC%36m

REM ────────────────────────────────────────────────────────────────────────
REM عرض الشعار / Display banner
REM ────────────────────────────────────────────────────────────────────────
echo.
echo ════════════════════════════════════════════════════════════
echo    %CYAN%بناء مفسر لغة ص / Building S Language Interpreter%RESET%
echo ════════════════════════════════════════════════════════════
echo.

REM ────────────────────────────────────────────────────────────────────────
REM معالجة المعاملات / Handle arguments
REM ────────────────────────────────────────────────────────────────────────
if "%1"=="clean" (
    echo %YELLOW%[تنظيف / Cleaning]%RESET% حذف ملفات البناء القديمة...
    echo %YELLOW%[تنظيف / Cleaning]%RESET% Removing old build files...
    if exist "build\s_interpreter.exe" del /F /Q "build\s_interpreter.exe"
    if exist "build\*.o" del /F /Q "build\*.o"
    echo %GREEN%✓ تم التنظيف / Cleaned%RESET%
    echo.
)

REM ────────────────────────────────────────────────────────────────────────
REM إنشاء مجلد البناء / Create build directory
REM ────────────────────────────────────────────────────────────────────────
if not exist "build" (
    echo %BLUE%[إعداد / Setup]%RESET% إنشاء مجلد build/...
    echo %BLUE%[إعداد / Setup]%RESET% Creating build/ directory...
    mkdir build
    echo %GREEN%✓ تم إنشاء المجلد / Directory created%RESET%
    echo.
)

REM ────────────────────────────────────────────────────────────────────────
REM تحديد ملفات المصدر / Define source files
REM ────────────────────────────────────────────────────────────────────────
echo %BLUE%[1/3]%RESET% %CYAN%جمع ملفات المصدر / Collecting source files...%RESET%

REM الملف الرئيسي / Main file
set MAIN_FILE=src/interpreter_main.cpp

REM ملفات المحلل المعجمي / Lexer files
set LEXER_FILES=src/lexer/token.cpp src/lexer/lexer_core.cpp src/lexer/lexer_keywords.cpp

REM ملفات المحلل النحوي / Parser files
set PARSER_FILES=src/parser/parser_core.cpp src/parser/parser_core_impl.cpp src/parser/parser_core_helpers.cpp

REM ملفات AST / AST files
set AST_FILES=src/parser/ast/ast_node.cpp src/parser/ast/expressions.cpp src/parser/ast/statements.cpp src/parser/ast/declarations.cpp src/parser/ast/ast_printer.cpp

REM ملفات المفسر / Interpreter files
set INTERPRETER_FILES=src/interpreter/core/interpreter_core.cpp src/interpreter/visitors/expression_evaluator.cpp src/interpreter/visitors/statement_executor.cpp

REM ملفات طبقة البيانات / Data layer files
set DATA_FILES=src/data/types/value.cpp src/data/managers/variable_manager.cpp src/data/managers/function_manager.cpp src/data/scope/scope_manager.cpp

REM ملفات الأدوات المساعدة / Utility files
set UTIL_FILES=src/utils/string_utils.cpp

REM ملفات معالجة الأخطاء / Error handling files
set ERROR_FILES=src/errors/error_manager.cpp

REM دمج جميع الملفات / Combine all files
set ALL_FILES=%MAIN_FILE% %LEXER_FILES% %PARSER_FILES% %AST_FILES% %INTERPRETER_FILES% %DATA_FILES% %UTIL_FILES% %ERROR_FILES%

echo       %GREEN%✓ تم جمع 25 ملف مصدر / Collected 25 source files%RESET%
echo.

REM ────────────────────────────────────────────────────────────────────────
REM عملية الترجمة / Compilation process
REM ────────────────────────────────────────────────────────────────────────
echo %BLUE%[2/3]%RESET% %CYAN%ترجمة الملفات / Compiling files...%RESET%
echo       هذا قد يستغرق بضع ثوان... / This may take a few seconds...
echo.

REM أعلام الترجمة / Compiler flags
set COMPILER_FLAGS=-std=c++17 -Wall -Wextra -O2

REM مسارات التضمين / Include paths
set INCLUDE_PATHS=-I include

REM ملف الإخراج / Output file
set OUTPUT_FILE=-o build/s_interpreter.exe

REM تنفيذ الترجمة / Execute compilation
g++ %COMPILER_FLAGS% %INCLUDE_PATHS% %ALL_FILES% %OUTPUT_FILE%

REM ────────────────────────────────────────────────────────────────────────
REM فحص نتيجة الترجمة / Check compilation result
REM ────────────────────────────────────────────────────────────────────────
if %ERRORLEVEL% EQU 0 (
    echo       %GREEN%✓ نجحت الترجمة! / Compilation successful!%RESET%
    echo.
    
    REM ────────────────────────────────────────────────────────────────────
    REM التحقق من الملف التنفيذي / Verify executable
    REM ────────────────────────────────────────────────────────────────────
    echo %BLUE%[3/3]%RESET% %CYAN%التحقق من الملف التنفيذي / Verifying executable...%RESET%
    
    if exist "build\s_interpreter.exe" (
        REM الحصول على حجم الملف / Get file size
        for %%A in (build\s_interpreter.exe) do set FILE_SIZE=%%~zA
        
        echo       %GREEN%✓ تم إنشاء الملف التنفيذي / Executable created%RESET%
        echo       📁 الموقع / Location: %GREEN%build\s_interpreter.exe%RESET%
        echo       📊 الحجم / Size: %GREEN%%FILE_SIZE% bytes%RESET%
        echo.
        
        REM ────────────────────────────────────────────────────────────────
        REM عرض رسالة النجاح / Display success message
        REM ────────────────────────────────────────────────────────────────
        echo ════════════════════════════════════════════════════════════
        echo    %GREEN%✅ اكتمل البناء بنجاح! / Build completed successfully!%RESET%
        echo ════════════════════════════════════════════════════════════
        echo.
        echo %CYAN%الخطوات التالية / Next steps:%RESET%
        echo ────────────────────────────────────────────────────────────
        echo.
        echo   %YELLOW%1. اختبار المفسر / Test the interpreter:%RESET%
        echo      .\build\s_interpreter.exe --version
        echo.
        echo   %YELLOW%2. تشغيل برنامج مثال / Run example program:%RESET%
        echo      .\build\s_interpreter.exe examples\basics\hello.s
        echo.
        echo   %YELLOW%3. عرض المساعدة / Show help:%RESET%
        echo      .\build\s_interpreter.exe --help
        echo.
        echo   %YELLOW%4. تشغيل برنامجك / Run your program:%RESET%
        echo      .\build\s_interpreter.exe your_program.s
        echo.
        
    ) else (
        echo       %RED%✗ فشل إنشاء الملف التنفيذي / Failed to create executable%RESET%
        echo.
        echo ════════════════════════════════════════════════════════════
        echo    %RED%❌ فشل البناء! / Build failed!%RESET%
        echo ════════════════════════════════════════════════════════════
        echo.
        exit /b 1
    )
    
) else (
    REM ────────────────────────────────────────────────────────────────────
    REM معالجة خطأ الترجمة / Handle compilation error
    REM ────────────────────────────────────────────────────────────────────
    echo       %RED%✗ فشلت الترجمة! / Compilation failed!%RESET%
    echo.
    echo ════════════════════════════════════════════════════════════
    echo    %RED%❌ فشل البناء! / Build failed!%RESET%
    echo ════════════════════════════════════════════════════════════
    echo.
    echo %YELLOW%رمز الخطأ / Error code:%RESET% %ERRORLEVEL%
    echo.
    echo %CYAN%نصائح لحل المشكلة / Troubleshooting tips:%RESET%
    echo ────────────────────────────────────────────────────────────
    echo.
    echo   %YELLOW%1.%RESET% تحقق من تثبيت g++ بشكل صحيح
    echo      Make sure g++ is properly installed
    echo      Command: g++ --version
    echo.
    echo   %YELLOW%2.%RESET% تأكد من دعم C++17
    echo      Ensure C++17 support is available
    echo.
    echo   %YELLOW%3.%RESET% راجع رسائل الخطأ أعلاه
    echo      Review error messages above
    echo.
    echo   %YELLOW%4.%RESET% تأكد من وجود جميع ملفات المصدر
    echo      Ensure all source files exist
    echo.
    echo   %YELLOW%5.%RESET% جرب التنظيف ثم البناء
    echo      Try clean build: build_interpreter.bat clean
    echo.
    
    exit /b 1
)

REM ────────────────────────────────────────────────────────────────────────
REM النهاية / End
REM ────────────────────────────────────────────────────────────────────────
echo ════════════════════════════════════════════════════════════
echo    شكراً لاستخدامك لغة ص! / Thank you for using S Language!
echo    https://sadlang.org
echo ════════════════════════════════════════════════════════════
echo.

exit /b 0

REM ============================================================================
REM نهاية الملف / End of File
REM ============================================================================
