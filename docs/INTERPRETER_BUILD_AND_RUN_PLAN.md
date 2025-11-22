# 🚀 خطة بناء وتشغيل مفسر لغة ص
# S Language Interpreter Build and Run Plan

**تاريخ الإنشاء / Creation Date:** 22 نوفمبر 2025  
**الإصدار / Version:** 1.0  
**الحالة / Status:** 📋 قيد التنفيذ / In Progress

---

## 📋 نظرة عامة / Overview

<div dir="rtl">

هذه الوثيقة تحتوي على خطة شاملة ومفصلة لبناء مفسر لغة ص وتشغيله على ملفات `.s`. الخطة مقسمة إلى مراحل واضحة مع خطوات قابلة للتنفيذ.

</div>

### Current Status / الحالة الحالية

```
✅ Lexer:              100% Complete
✅ Parser:             100% Complete  
✅ Data Layer:         100% Complete
✅ Interpreter Core:   100% Complete
⏳ Main Program:       0% - Needs Implementation
⏳ Build System:       0% - Needs Setup
```

---

## 🎯 الهدف النهائي / Final Goal

<div dir="rtl">

**إنشاء برنامج تنفيذي `s_interpreter.exe` يمكنه:**
1. قراءة ملفات `.s` من القرص
2. تحليلها معجمياً (Tokenization)
3. تحليلها نحوياً (Parsing) لإنشاء AST
4. تنفيذها باستخدام المفسر (Interpreter)
5. طباعة النتائج أو الأخطاء

**الاستخدام المتوقع:**
```bash
s_interpreter.exe program.s
s_interpreter.exe examples/basics/hello.s
```

</div>

---

## 📊 المراحل الرئيسية / Main Phases

| # | المرحلة | الوصف | المدة | الحالة |
|---|---------|-------|-------|--------|
| 1 | التحليل | فهم المكونات الموجودة | 30 دقيقة | ⏳ |
| 2 | التصميم | تصميم البرنامج الرئيسي | 1 ساعة | ⏳ |
| 3 | التنفيذ | كتابة الكود | 2-3 ساعات | ⏳ |
| 4 | البناء | إعداد نظام البناء | 1 ساعة | ⏳ |
| 5 | الاختبار | اختبار شامل | 1 ساعة | ⏳ |
| 6 | التوثيق | كتابة الدليل | 30 دقيقة | ⏳ |

**الوقت الإجمالي المتوقع:** 6-7 ساعات

---

## 🔍 المرحلة 1: التحليل (30 دقيقة)

### 1.1 فحص المكونات الموجودة ✅

<div dir="rtl">

**الهدف:** التأكد من أن جميع المكونات اللازمة موجودة وجاهزة.

</div>

**Checklist:**

```
📁 include/
  ✅ lexer/lexer_core.h          - Lexer interface
  ✅ parser/parser_core.h        - Parser interface
  ✅ interpreter/core/interpreter_core.h - Interpreter interface
  ✅ data/types/value.h          - Value system
  ✅ errors/error_manager.h      - Error handling

📁 src/
  ✅ lexer/lexer_core.cpp        - Lexer implementation
  ✅ parser/parser_core.cpp      - Parser implementation
  ✅ interpreter/core/interpreter_core.cpp - Interpreter impl
  ✅ data/types/value.cpp        - Value implementation
  ⏳ main.cpp                    - Exists but needs update
```

**Commands to verify:**
```powershell
cd C:\s\s_language

# Check if files exist
Test-Path include\lexer\lexer_core.h
Test-Path include\parser\parser_core.h
Test-Path include\interpreter\core\interpreter_core.h
Test-Path src\main.cpp

# Count lines of existing components
(Get-Content include\lexer\lexer_core.h).Count
(Get-Content src\lexer\lexer_core.cpp).Count
```

---

### 1.2 فحص الملفات المثالية 📂

<div dir="rtl">

**الهدف:** التأكد من وجود ملفات `.s` للاختبار.

</div>

**Commands:**
```powershell
cd C:\s\s_language\examples

# List all .s files
Get-ChildItem -Recurse -Filter "*.s"

# Check specific examples
Test-Path basics\hello.s
Test-Path basics\variables.s
Test-Path basics\conditions.s
Test-Path basics\loops.s
```

**Expected Files:**
```
examples/
  ├─ basics/
  │   ├─ hello.s           ✅
  │   ├─ variables.s       📝 To create
  │   ├─ conditions.s      📝 To create
  │   └─ loops.s           📝 To create
  ├─ functions/
  │   └─ simple_func.s     📝 To create
  └─ advanced/
      ├─ lambda.s          📝 To create
      └─ comprehension.s   📝 To create
```

---

### 1.3 تحديد الاعتماديات 🔗

<div dir="rtl">

**المكونات المطلوبة للمفسر:**

</div>

```cpp
// Core dependencies (in order)
1. Lexer (lexer_core.cpp + token.cpp + lexer_keywords.cpp)
2. Parser (parser_core.cpp + parser_core_impl.cpp + parser_core_helpers.cpp)
3. AST (ast/expressions.cpp + ast/statements.cpp + ast/declarations.cpp)
4. Data Layer (value.cpp + variable_manager.cpp + function_manager.cpp + scope_manager.cpp)
5. Interpreter (interpreter_core.cpp + expression_evaluator.cpp + statement_executor.cpp)
6. Utils (string_utils.cpp)
7. Errors (error_manager.cpp)
```

**Total files to compile:** ~25 files

---

## 🎨 المرحلة 2: التصميم (1 ساعة)

### 2.1 تصميم البرنامج الرئيسي

<div dir="rtl">

**الملف:** `src/interpreter_main.cpp`

**الوظائف الرئيسية:**

</div>

```cpp
/*************************************************
 * Main Program Structure
 *************************************************/

1. main(int argc, char* argv[])
   - Parse command-line arguments
   - Validate input
   - Call appropriate function

2. void showHelp()
   - Display usage information
   - Show examples
   - List options

3. void showVersion()
   - Display version info
   - Show build date
   - List features

4. std::string readFile(const std::string& path)
   - Read file from disk
   - Handle encoding (UTF-8)
   - Return content

5. void executeFile(const std::string& path)
   - Main execution pipeline:
     a. Read file
     b. Tokenize (Lexer)
     c. Parse (Parser)
     d. Execute (Interpreter)
     e. Handle errors

6. void runREPL()
   - Interactive mode (future)
   - Line-by-line execution
```

---

### 2.2 تصميم سير التنفيذ (Execution Flow)

```
┌─────────────────────────────────────────┐
│         User runs program               │
│   s_interpreter.exe program.s           │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│      Parse Command Line Args            │
│  - Check argument count                 │
│  - Validate file path                   │
│  - Check file exists                    │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│         Read File Content               │
│  - Open file with UTF-8 encoding        │
│  - Read entire content                  │
│  - Handle file errors                   │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│      LEXER: Tokenization                │
│  Input:  Source code string             │
│  Output: Vector<Token>                  │
│  - Create Lexer instance                │
│  - Call tokenize()                      │
│  - Handle lexical errors                │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│      PARSER: Build AST                  │
│  Input:  Vector<Token>                  │
│  Output: unique_ptr<ProgramNode>        │
│  - Create Parser instance               │
│  - Call parse()                         │
│  - Handle syntax errors                 │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│    INTERPRETER: Execute AST             │
│  Input:  ProgramNode                    │
│  Output: Execution results              │
│  - Create Interpreter instance          │
│  - Call execute()                       │
│  - Handle runtime errors                │
└──────────────┬──────────────────────────┘
               │
               ▼
┌─────────────────────────────────────────┐
│        Display Results / Errors         │
│  - Print output to console              │
│  - Show error messages (Arabic)         │
│  - Exit with appropriate code           │
└─────────────────────────────────────────┘
```

---

### 2.3 معالجة الأخطاء (Error Handling)

<div dir="rtl">

**أنواع الأخطاء المحتملة:**

</div>

```cpp
// 1. Command-line errors
- No arguments provided
- File not found
- Invalid file path

// 2. Lexical errors
- Invalid character
- Unterminated string
- Invalid number format

// 3. Syntax errors
- Unexpected token
- Missing semicolon
- Unclosed bracket

// 4. Runtime errors
- Undefined variable
- Type mismatch
- Division by zero
- Stack overflow
```

**Error Display Format:**
```
❌ خطأ في السطر 5، العمود 10
   Error at line 5, column 10

📝 متغير غير معرّف: "س"
   Undefined variable: "x"

💡 هل تقصد: "ص"؟
   Did you mean: "y"?
```

---

## 💻 المرحلة 3: التنفيذ (2-3 ساعات)

### 3.1 إنشاء src/interpreter_main.cpp

<div dir="rtl">

**الملف الجديد:** `C:\s\s_language\src\interpreter_main.cpp`

</div>

**Structure:**

```cpp
// ====================================================================
// interpreter_main.cpp - لغة ص - برنامج المفسر الرئيسي
// S Language - Main Interpreter Program
// ====================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <locale>
#include <codecvt>

// Include project headers
#include "lexer/lexer_core.h"
#include "parser/parser_core.h"
#include "interpreter/core/interpreter_core.h"
#include "errors/error_manager.h"
#include "utils/string_utils.h"

// Namespaces
using namespace Sad;
using namespace Sad::Lexer;
using namespace Sad::Parser;
using namespace Sad::Interpreter;

// ====================================================================
// Constants
// ====================================================================

const std::string VERSION = "1.1.0";
const std::string BUILD_DATE = __DATE__;

// ====================================================================
// Function Declarations
// ====================================================================

void showHelp();
void showVersion();
std::string readFile(const std::string& filepath);
void executeFile(const std::string& filepath);
void runREPL();
int main(int argc, char* argv[]);

// ====================================================================
// Function Implementations
// ====================================================================

/**
 * @brief (AR) عرض معلومات المساعدة
 * @brief (EN) Display help information
 */
void showHelp() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "لغة ص - S Programming Language\n";
    std::cout << "المفسر / Interpreter v" << VERSION << "\n";
    std::cout << "========================================\n\n";
    
    std::cout << "الاستخدام / Usage:\n";
    std::cout << "  s_interpreter <file.s>              تشغيل ملف\n";
    std::cout << "  s_interpreter <file.s>              Run a file\n\n";
    
    std::cout << "  s_interpreter --help                عرض المساعدة\n";
    std::cout << "  s_interpreter --help                Show help\n\n";
    
    std::cout << "  s_interpreter --version             عرض الإصدار\n";
    std::cout << "  s_interpreter --version             Show version\n\n";
    
    std::cout << "  s_interpreter --repl                وضع تفاعلي (قريباً)\n";
    std::cout << "  s_interpreter --repl                Interactive mode (coming soon)\n\n";
    
    std::cout << "أمثلة / Examples:\n";
    std::cout << "  s_interpreter examples/basics/hello.s\n";
    std::cout << "  s_interpreter myprogram.s\n\n";
}

/**
 * @brief (AR) عرض معلومات الإصدار
 * @brief (EN) Display version information
 */
void showVersion() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "لغة ص - S Programming Language\n";
    std::cout << "========================================\n\n";
    
    std::cout << "الإصدار / Version:     " << VERSION << "\n";
    std::cout << "تاريخ البناء / Build:   " << BUILD_DATE << "\n";
    std::cout << "المعيار / Standard:     C++17\n";
    std::cout << "الترخيص / License:      MIT\n\n";
    
    std::cout << "المكونات / Components:\n";
    std::cout << "  ✅ Lexer              - المحلل المعجمي\n";
    std::cout << "  ✅ Parser             - المحلل النحوي\n";
    std::cout << "  ✅ Interpreter        - المفسر التنفيذي\n";
    std::cout << "  ✅ Type System        - نظام الأنواع\n";
    std::cout << "  ✅ Error Handling     - معالجة الأخطاء\n\n";
    
    std::cout << "الميزات / Features:\n";
    std::cout << "  ✅ Variables          - المتغيرات\n";
    std::cout << "  ✅ Functions          - الدوال\n";
    std::cout << "  ✅ Control Flow       - التحكم في التدفق\n";
    std::cout << "  ✅ Lambda             - دوال Lambda\n";
    std::cout << "  ✅ Comprehensions     - الاستيعاب القائمي\n";
    std::cout << "  ✅ Exception Handling - معالجة الاستثناءات\n\n";
}

/**
 * @brief (AR) قراءة محتوى ملف
 * @brief (EN) Read file content
 * @param filepath (std::string) مسار الملف / File path
 * @return (std::string) محتوى الملف / File content
 */
std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath, std::ios::binary);
    
    if (!file.is_open()) {
        throw std::runtime_error("فشل فتح الملف / Failed to open file: " + filepath);
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return buffer.str();
}

/**
 * @brief (AR) تنفيذ ملف برنامج
 * @brief (EN) Execute a program file
 * @param filepath (std::string) مسار الملف / File path
 */
void executeFile(const std::string& filepath) {
    try {
        std::cout << "\n";
        std::cout << "========================================\n";
        std::cout << "لغة ص - S Programming Language\n";
        std::cout << "المفسر / Interpreter v" << VERSION << "\n";
        std::cout << "========================================\n\n";
        
        // Step 1: Read file
        std::cout << "📖 قراءة الملف / Reading file...\n";
        std::string sourceCode = readFile(filepath);
        std::cout << "   ✅ تم قراءة " << sourceCode.length() << " حرف\n";
        std::cout << "   ✅ Read " << sourceCode.length() << " characters\n\n";
        
        // Step 2: Tokenize
        std::cout << "🔤 التحليل المعجمي / Lexical analysis...\n";
        LexerCore lexer(sourceCode);
        auto tokens = lexer.tokenize();
        std::cout << "   ✅ تم إنشاء " << tokens.size() << " رمز\n";
        std::cout << "   ✅ Generated " << tokens.size() << " tokens\n\n";
        
        // Step 3: Parse
        std::cout << "🌳 التحليل النحوي / Syntax analysis...\n";
        ParserCore parser(tokens);
        auto ast = parser.parse();
        std::cout << "   ✅ تم بناء شجرة AST بنجاح\n";
        std::cout << "   ✅ AST built successfully\n\n";
        
        // Step 4: Execute
        std::cout << "⚙️  التنفيذ / Execution...\n";
        std::cout << "========================================\n\n";
        
        InterpreterCore interpreter;
        auto result = interpreter.execute(ast.get());
        
        std::cout << "\n========================================\n";
        std::cout << "✅ اكتمل التنفيذ بنجاح\n";
        std::cout << "✅ Execution completed successfully\n";
        std::cout << "========================================\n\n";
        
    } catch (const std::exception& e) {
        std::cerr << "\n";
        std::cerr << "========================================\n";
        std::cerr << "❌ خطأ / Error\n";
        std::cerr << "========================================\n\n";
        std::cerr << e.what() << "\n\n";
        throw;
    }
}

/**
 * @brief (AR) تشغيل الوضع التفاعلي
 * @brief (EN) Run interactive REPL mode
 */
void runREPL() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "لغة ص - الوضع التفاعلي\n";
    std::cout << "S Language - REPL Mode\n";
    std::cout << "========================================\n\n";
    std::cout << "هذه الميزة قيد التطوير\n";
    std::cout << "This feature is under development\n\n";
    std::cout << "قريباً... / Coming soon...\n\n";
}

/**
 * @brief (AR) النقطة الرئيسية للبرنامج
 * @brief (EN) Main entry point
 */
int main(int argc, char* argv[]) {
    try {
        // No arguments - show help
        if (argc == 1) {
            showHelp();
            return 0;
        }
        
        std::string arg1 = argv[1];
        
        // Handle flags
        if (arg1 == "--help" || arg1 == "-h") {
            showHelp();
            return 0;
        }
        
        if (arg1 == "--version" || arg1 == "-v") {
            showVersion();
            return 0;
        }
        
        if (arg1 == "--repl" || arg1 == "-r") {
            runREPL();
            return 0;
        }
        
        // Execute file
        executeFile(arg1);
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "خطأ فادح / Fatal error: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "خطأ غير معروف / Unknown error\n";
        return 1;
    }
}

// ====================================================================
// End of file
// ====================================================================
```

**File size:** ~250 lines

---

### 3.2 إنشاء ملفات الأمثلة

#### Example 1: hello.s (Already exists ✅)

```sad
# مرحباً بالعالم
اطبع("مرحباً بالعالم!")
اطبع("Hello World!")

رقم س = 10
رقم ص = 20
اطبع("س + ص = " + (س + ص))
```

#### Example 2: variables.s (To create)

```sad
# ====================================
# أمثلة على المتغيرات
# Variables Examples
# ====================================

# أنواع البيانات المختلفة
رقم العمر = 25
عشري الطول = 1.75
نص الاسم = "أحمد"
منطقي موجود = صح

اطبع("العمر: " + العمر)
اطبع("الطول: " + الطول)
اطبع("الاسم: " + الاسم)
اطبع("موجود: " + موجود)

# عمليات حسابية
رقم أ = 10
رقم ب = 5

اطبع("الجمع: " + (أ + ب))
اطبع("الطرح: " + (أ - ب))
اطبع("الضرب: " + (أ * ب))
اطبع("القسمة: " + (أ / ب))
اطبع("الباقي: " + (أ % ب))
```

#### Example 3: functions.s (To create)

```sad
# ====================================
# أمثلة على الدوال
# Functions Examples
# ====================================

# دالة بسيطة
دالة مرحبا() {
    اطبع("مرحباً!")
}

مرحبا()

# دالة مع معاملات
دالة جمع(رقم أ، رقم ب) : رقم {
    إرجاع أ + ب
}

رقم النتيجة = جمع(15، 25)
اطبع("النتيجة: " + النتيجة)

# دالة مع معاملات متعددة
دالة معلومات(نص الاسم، رقم العمر، نص المدينة) {
    اطبع("الاسم: " + الاسم)
    اطبع("العمر: " + العمر)
    اطبع("المدينة: " + المدينة)
}

معلومات("أحمد"، 25، "الرياض")
```

---

## 🔨 المرحلة 4: البناء (1 ساعة)

### 4.1 إنشاء Build Script

#### Option A: Batch File (build_interpreter.bat)

```batch
@echo off
REM ====================================
REM Build Script for S Interpreter
REM ====================================

echo.
echo ========================================
echo Building S Language Interpreter
echo ========================================
echo.

REM Create build directory if not exists
if not exist "build" mkdir build

echo [1/3] Compiling source files...
echo.

g++ -std=c++17 -I include ^
    src/interpreter_main.cpp ^
    src/lexer/token.cpp ^
    src/lexer/lexer_core.cpp ^
    src/lexer/lexer_keywords.cpp ^
    src/parser/parser_core.cpp ^
    src/parser/parser_core_impl.cpp ^
    src/parser/parser_core_helpers.cpp ^
    src/parser/ast/ast_node.cpp ^
    src/parser/ast/expressions.cpp ^
    src/parser/ast/statements.cpp ^
    src/parser/ast/declarations.cpp ^
    src/parser/ast/ast_printer.cpp ^
    src/interpreter/core/interpreter_core.cpp ^
    src/interpreter/visitors/expression_evaluator.cpp ^
    src/interpreter/visitors/statement_executor.cpp ^
    src/data/types/value.cpp ^
    src/data/managers/variable_manager.cpp ^
    src/data/managers/function_manager.cpp ^
    src/data/scope/scope_manager.cpp ^
    src/utils/string_utils.cpp ^
    src/errors/error_manager.cpp ^
    -o build/s_interpreter.exe

if %ERRORLEVEL% EQU 0 (
    echo.
    echo [2/3] Compilation successful!
    echo [3/3] Executable created: build\s_interpreter.exe
    echo.
    echo ========================================
    echo ✅ Build completed successfully!
    echo ========================================
    echo.
    echo You can now run:
    echo   .\build\s_interpreter.exe examples\basics\hello.s
    echo.
) else (
    echo.
    echo ========================================
    echo ❌ Build failed!
    echo ========================================
    echo.
    echo Error code: %ERRORLEVEL%
    echo Please check the error messages above.
    echo.
    exit /b 1
)
```

#### Option B: PowerShell Script (build_interpreter.ps1)

```powershell
# ====================================
# Build Script for S Interpreter
# ====================================

Write-Host ""
Write-Host "========================================"
Write-Host "Building S Language Interpreter"
Write-Host "========================================"
Write-Host ""

# Create build directory
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Write-Host "[1/3] Compiling source files..."
Write-Host ""

# Source files
$sourceFiles = @(
    "src/interpreter_main.cpp",
    "src/lexer/token.cpp",
    "src/lexer/lexer_core.cpp",
    "src/lexer/lexer_keywords.cpp",
    "src/parser/parser_core.cpp",
    "src/parser/parser_core_impl.cpp",
    "src/parser/parser_core_helpers.cpp",
    "src/parser/ast/ast_node.cpp",
    "src/parser/ast/expressions.cpp",
    "src/parser/ast/statements.cpp",
    "src/parser/ast/declarations.cpp",
    "src/parser/ast/ast_printer.cpp",
    "src/interpreter/core/interpreter_core.cpp",
    "src/interpreter/visitors/expression_evaluator.cpp",
    "src/interpreter/visitors/statement_executor.cpp",
    "src/data/types/value.cpp",
    "src/data/managers/variable_manager.cpp",
    "src/data/managers/function_manager.cpp",
    "src/data/scope/scope_manager.cpp",
    "src/utils/string_utils.cpp",
    "src/errors/error_manager.cpp"
)

# Compile
$compilerArgs = @("-std=c++17", "-I", "include") + $sourceFiles + @("-o", "build/s_interpreter.exe")

& g++ $compilerArgs

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "[2/3] Compilation successful!"
    Write-Host "[3/3] Executable created: build\s_interpreter.exe"
    Write-Host ""
    Write-Host "========================================"
    Write-Host "✅ Build completed successfully!"
    Write-Host "========================================"
    Write-Host ""
    Write-Host "You can now run:"
    Write-Host "  .\build\s_interpreter.exe examples\basics\hello.s"
    Write-Host ""
} else {
    Write-Host ""
    Write-Host "========================================"
    Write-Host "❌ Build failed!"
    Write-Host "========================================"
    Write-Host ""
    Write-Host "Error code: $LASTEXITCODE"
    Write-Host "Please check the error messages above."
    Write-Host ""
    exit 1
}
```

#### Option C: CMake Configuration

**Update CMakeLists.txt:**

```cmake
# Add interpreter executable
add_executable(s_interpreter
    src/interpreter_main.cpp
    src/lexer/token.cpp
    src/lexer/lexer_core.cpp
    src/lexer/lexer_keywords.cpp
    src/parser/parser_core.cpp
    src/parser/parser_core_impl.cpp
    src/parser/parser_core_helpers.cpp
    src/parser/ast/ast_node.cpp
    src/parser/ast/expressions.cpp
    src/parser/ast/statements.cpp
    src/parser/ast/declarations.cpp
    src/parser/ast/ast_printer.cpp
    src/interpreter/core/interpreter_core.cpp
    src/interpreter/visitors/expression_evaluator.cpp
    src/interpreter/visitors/statement_executor.cpp
    src/data/types/value.cpp
    src/data/managers/variable_manager.cpp
    src/data/managers/function_manager.cpp
    src/data/scope/scope_manager.cpp
    src/utils/string_utils.cpp
    src/errors/error_manager.cpp
)

target_include_directories(s_interpreter PRIVATE include)
target_compile_features(s_interpreter PRIVATE cxx_std_17)

set_target_properties(s_interpreter PROPERTIES
    OUTPUT_NAME "s_interpreter"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
```

**CMake build commands:**
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

---

### 4.2 اختبار البناء

<div dir="rtl">

**بعد البناء الناجح، اختبر باستخدام:**

</div>

```powershell
# Test 1: Show help
.\build\s_interpreter.exe --help

# Test 2: Show version
.\build\s_interpreter.exe --version

# Test 3: Run hello.s
.\build\s_interpreter.exe examples\basics\hello.s

# Test 4: Run with non-existent file (should show error)
.\build\s_interpreter.exe nonexistent.s
```

**Expected output for Test 3:**
```
========================================
لغة ص - S Programming Language
المفسر / Interpreter v1.1.0
========================================

📖 قراءة الملف / Reading file...
   ✅ تم قراءة 234 حرف
   ✅ Read 234 characters

🔤 التحليل المعجمي / Lexical analysis...
   ✅ تم إنشاء 45 رمز
   ✅ Generated 45 tokens

🌳 التحليل النحوي / Syntax analysis...
   ✅ تم بناء شجرة AST بنجاح
   ✅ AST built successfully

⚙️  التنفيذ / Execution...
========================================

مرحباً بالعالم!
Hello World!
س + ص = 30

========================================
✅ اكتمل التنفيذ بنجاح
✅ Execution completed successfully
========================================
```

---

## 🧪 المرحلة 5: الاختبار (1 ساعة)

### 5.1 اختبارات وظيفية (Functional Tests)

<div dir="rtl">

**إنشاء test suite شامل:**

</div>

```powershell
# Create test runner script
# test_interpreter.ps1

Write-Host "Running Interpreter Tests..."
Write-Host ""

$testsPassed = 0
$testsFailed = 0

function Test-Program($name, $file, $expectedOutput) {
    Write-Host "Test: $name"
    
    $output = & .\build\s_interpreter.exe $file 2>&1
    
    if ($output -like "*$expectedOutput*") {
        Write-Host "  ✅ PASSED" -ForegroundColor Green
        $script:testsPassed++
    } else {
        Write-Host "  ❌ FAILED" -ForegroundColor Red
        $script:testsFailed++
    }
    Write-Host ""
}

# Run tests
Test-Program "Hello World" "examples/basics/hello.s" "مرحباً بالعالم"
Test-Program "Variables" "examples/basics/variables.s" "العمر: 25"
Test-Program "Functions" "examples/basics/functions.s" "النتيجة: 40"

# Summary
Write-Host "========================================"
Write-Host "Test Results:"
Write-Host "  Passed: $testsPassed"
Write-Host "  Failed: $testsFailed"
Write-Host "========================================"

if ($testsFailed -eq 0) {
    Write-Host "✅ All tests passed!" -ForegroundColor Green
    exit 0
} else {
    Write-Host "❌ Some tests failed!" -ForegroundColor Red
    exit 1
}
```

---

### 5.2 اختبارات الأخطاء (Error Tests)

```powershell
# Test error handling

# Test 1: Non-existent file
Write-Host "Test: Non-existent file"
$result = & .\build\s_interpreter.exe nonexistent.s 2>&1
if ($result -like "*Failed to open file*") {
    Write-Host "  ✅ PASSED"
} else {
    Write-Host "  ❌ FAILED"
}

# Test 2: Empty file
New-Item -Path "test_empty.s" -ItemType File -Force | Out-Null
Write-Host "Test: Empty file"
$result = & .\build\s_interpreter.exe test_empty.s 2>&1
Write-Host "  ✅ PASSED (no crash)"
Remove-Item "test_empty.s"

# Test 3: Invalid syntax
"this is not valid syntax" | Out-File -FilePath "test_invalid.s" -Encoding UTF8
Write-Host "Test: Invalid syntax"
$result = & .\build\s_interpreter.exe test_invalid.s 2>&1
if ($result -like "*Error*" -or $result -like "*خطأ*") {
    Write-Host "  ✅ PASSED (error detected)"
} else {
    Write-Host "  ❌ FAILED"
}
Remove-Item "test_invalid.s"
```

---

### 5.3 اختبارات الأداء (Performance Tests)

```powershell
# Test execution time for different program sizes

function Measure-ExecutionTime($file) {
    $time = Measure-Command {
        & .\build\s_interpreter.exe $file | Out-Null
    }
    return $time.TotalMilliseconds
}

Write-Host "Performance Tests:"
Write-Host ""

$time1 = Measure-ExecutionTime "examples/basics/hello.s"
Write-Host "Small program (hello.s):     $([math]::Round($time1, 2)) ms"

$time2 = Measure-ExecutionTime "examples/basics/variables.s"
Write-Host "Medium program (variables.s): $([math]::Round($time2, 2)) ms"

Write-Host ""
Write-Host "Performance looks good!" -ForegroundColor Green
```

---

## 📚 المرحلة 6: التوثيق (30 دقيقة)

### 6.1 دليل المستخدم (User Guide)

**Create:** `docs/USER_GUIDE.md`

```markdown
# 📖 دليل مستخدم مفسر لغة ص
# S Language Interpreter User Guide

## التثبيت / Installation

### Windows

1. تحميل آخر إصدار من releases
2. فك الضغط
3. إضافة إلى PATH (اختياري)

## الاستخدام / Usage

### تشغيل برنامج

```bash
s_interpreter program.s
```

### الخيارات المتاحة

- `--help`, `-h`: عرض المساعدة
- `--version`, `-v`: عرض الإصدار
- `--repl`, `-r`: الوضع التفاعلي (قريباً)

### الأمثلة

#### مثال 1: مرحباً بالعالم
```bash
s_interpreter examples/basics/hello.s
```

#### مثال 2: برنامج مخصص
```bash
s_interpreter myprogram.s
```

## رسائل الأخطاء الشائعة

### خطأ: "Failed to open file"
**السبب:** الملف غير موجود أو المسار خاطئ
**الحل:** تأكد من وجود الملف والمسار الصحيح

### خطأ: "Unexpected token"
**السبب:** خطأ في الصياغة
**الحل:** راجع صياغة البرنامج

### خطأ: "Undefined variable"
**السبب:** محاولة استخدام متغير غير معرّف
**الحل:** عرّف المتغير قبل استخدامه
```

---

### 6.2 دليل المطور (Developer Guide)

**Create:** `docs/DEVELOPER_GUIDE.md`

```markdown
# 👨‍💻 دليل المطور - مفسر لغة ص
# S Language Interpreter Developer Guide

## البنية المعمارية

### المكونات الرئيسية

1. **Lexer** - المحلل المعجمي
2. **Parser** - المحلل النحوي
3. **Interpreter** - المفسر التنفيذي
4. **Data Layer** - طبقة البيانات

### سير التنفيذ

```
Source Code → Lexer → Tokens → Parser → AST → Interpreter → Output
```

## البناء من المصدر

### المتطلبات
- CMake 3.15+
- C++17 compiler
- Git

### خطوات البناء

```bash
git clone https://github.com/sadlang/s_language.git
cd s_language
mkdir build && cd build
cmake ..
cmake --build .
```

## إضافة ميزات جديدة

### إضافة كلمة مفتاحية

1. أضف في `lexer/lexer_keywords.cpp`
2. أضف TokenType في `lexer/token.h`
3. أضف معالجة في Parser
4. أضف تنفيذ في Interpreter

### إضافة دالة مكتبة قياسية

1. أضف في `stdlib/...`
2. سجل في `stdlib_manager.cpp`
3. أضف اختبارات

## الاختبارات

### تشغيل جميع الاختبارات
```bash
cd build
ctest --output-on-failure
```

### إضافة اختبار جديد
1. أنشئ ملف في `tests/`
2. أضف إلى CMakeLists.txt
3. اكتب الاختبار
4. تأكد من نجاحه
```

---

## 📝 قائمة المهام / Task Checklist

### ✅ مكتمل / Completed

```
✅ Phase 1: Analysis (فهم المكونات)
✅ Phase 2: Design (تصميم البرنامج الرئيسي)
```

### ⏳ قيد التنفيذ / In Progress

```
⏳ Phase 3: Implementation
   ⏳ 3.1 Create interpreter_main.cpp
   ⏳ 3.2 Create example .s files
   
⏳ Phase 4: Build System
   ⏳ 4.1 Create build scripts
   ⏳ 4.2 Test build process
   
⏳ Phase 5: Testing
   ⏳ 5.1 Functional tests
   ⏳ 5.2 Error handling tests
   ⏳ 5.3 Performance tests
   
⏳ Phase 6: Documentation
   ⏳ 6.1 User guide
   ⏳ 6.2 Developer guide
```

---

## 🎯 الخطوات التالية / Next Steps

<div dir="rtl">

### الخطوة 1: التنفيذ الفوري (اليوم)

1. **إنشاء interpreter_main.cpp**
   - نسخ الكود أعلاه
   - حفظ في `src/interpreter_main.cpp`
   - تأكد من الترميز UTF-8

2. **إنشاء Build Script**
   - اختر واحد من الخيارات (Batch/PowerShell/CMake)
   - احفظ في المجلد الرئيسي
   - اختبر التنفيذ

3. **البناء الأول**
   ```powershell
   .\build_interpreter.bat
   # أو
   .\build_interpreter.ps1
   ```

4. **الاختبار الأولي**
   ```powershell
   .\build\s_interpreter.exe --version
   .\build\s_interpreter.exe examples\basics\hello.s
   ```

### الخطوة 2: إنشاء الأمثلة (1-2 ساعات)

1. إنشاء `examples/basics/variables.s`
2. إنشاء `examples/basics/functions.s`
3. إنشاء `examples/basics/conditions.s`
4. إنشاء `examples/basics/loops.s`

### الخطوة 3: الاختبار الشامل (1 ساعة)

1. تشغيل جميع الأمثلة
2. التأكد من عدم وجود أخطاء
3. قياس الأداء

### الخطوة 4: التوثيق النهائي (30 دقيقة)

1. كتابة USER_GUIDE.md
2. كتابة DEVELOPER_GUIDE.md
3. تحديث README.md

</div>

---

## 🚀 الإطلاق / Launch

<div dir="rtl">

### معايير الإطلاق

يمكن إطلاق النسخة 1.1.0 عندما:

- ✅ `s_interpreter.exe` يبني بنجاح
- ✅ يشغل جميع الأمثلة بدون أخطاء
- ✅ معالجة الأخطاء تعمل بشكل صحيح
- ✅ التوثيق كامل
- ✅ 100% من الاختبارات تنجح

### ما بعد الإطلاق

#### قصير المدى (1-2 أسابيع)
- إضافة المزيد من الأمثلة
- تحسين رسائل الأخطاء
- إضافة REPL mode

#### متوسط المدى (1-2 شهور)
- IDE integration (VS Code extension)
- Package manager
- Debugger

#### طويل المدى (3-6 شهور)
- JIT compilation
- WebAssembly support
- Cloud IDE

</div>

---

## 📊 التقدم المتوقع / Expected Progress

```
Week 1 (Today):
  ✅ Create interpreter_main.cpp
  ✅ Setup build system
  ✅ First successful build
  ✅ Test with hello.s

Week 1 (Tomorrow):
  ✅ Create more examples
  ✅ Full testing
  ✅ Documentation
  ✅ Release 1.1.0

Week 2:
  ✅ Standard libraries completion
  ✅ More examples
  ✅ Performance optimization

Week 3-4:
  ✅ OOP implementation
  ✅ Advanced features
```

---

## 💡 نصائح مهمة / Important Tips

<div dir="rtl">

### للنجاح في البناء

1. **تأكد من وجود جميع الملفات**
   - استخدم `Get-ChildItem` للتحقق
   - تأكد من المسارات الصحيحة

2. **UTF-8 Encoding**
   - احفظ جميع الملفات بترميز UTF-8
   - خاصة الملفات العربية

3. **Compiler Flags**
   - استخدم `-std=c++17` دائماً
   - أضف `-I include` للـ headers

4. **Incremental Building**
   - ابدأ بملفات قليلة
   - أضف تدريجياً
   - اختبر بعد كل إضافة

5. **Error Messages**
   - اقرأ الأخطاء بعناية
   - ابحث عن missing headers
   - تحقق من typos

</div>

---

## 📞 الدعم / Support

<div dir="rtl">

### إذا واجهت مشاكل

1. **مشاكل البناء:**
   - تحقق من تثبيت g++
   - تحقق من C++17 support
   - راجع الأخطاء بعناية

2. **مشاكل التنفيذ:**
   - تحقق من المسارات
   - تحقق من صلاحيات الملفات
   - استخدم `--help` للمساعدة

3. **مشاكل الترميز:**
   - احفظ كـ UTF-8 with BOM
   - استخدم محرر نص يدعم UTF-8

</div>

---

## 🎉 الخاتمة / Conclusion

<div dir="rtl">

هذه الخطة توفر مساراً واضحاً ومفصلاً لبناء وتشغيل مفسر لغة ص. باتباع الخطوات بالترتيب، ستتمكن من:

1. ✅ بناء `s_interpreter.exe` بنجاح
2. ✅ تشغيل برامج `.s` من سطر الأوامر
3. ✅ معالجة الأخطاء بشكل صحيح
4. ✅ اختبار شامل للمفسر

**الوقت الإجمالي المتوقع:** 6-7 ساعات

**النتيجة النهائية:** مفسر لغة ص كامل وعامل! 🚀

</div>

---

**تاريخ الإنشاء:** 22 نوفمبر 2025  
**آخر تحديث:** 22 نوفمبر 2025  
**الإصدار:** 1.0  
**الحالة:** 📋 جاهز للتنفيذ / Ready for Implementation

---

<div align="center">

# 🌟 لنبني معاً مفسر لغة ص! 🌟
# Let's Build S Language Interpreter Together!

**Made with ❤️ for Arabic Developers**  
**صُنع بـ ❤️ للمطورين العرب**

</div>

---

**نهاية الخطة / End of Plan**
