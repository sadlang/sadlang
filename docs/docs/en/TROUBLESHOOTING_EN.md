# 🔧 Troubleshooting Guide - Sad Programming Language
# دليل حل المشاكل - لغة ص

**Comprehensive guide for solving technical issues and common errors 🛠️**

---

## 📑 Table of Contents

1. [Installation Issues](#installation-issues)
2. [Runtime Errors](#runtime-errors)
3. [Character Encoding Issues](#character-encoding-issues)
4. [Memory and Performance Issues](#memory-and-performance-issues)
5. [Object-Oriented Programming Issues](#object-oriented-programming-issues)
6. [Editor and Environment Issues](#editor-and-environment-issues)
7. [Diagnostic Tools](#diagnostic-tools)

---

## 💻 Installation Issues

### ❗ "CMake not found" or "cmake command not recognized"

**Symptoms**:
```
'cmake' is not recognized as an internal or external command
```

**Solutions**:

**On Windows**:
```bash
# 1. Download and install CMake from official website
# https://cmake.org/download/

# 2. Add CMake to PATH
$env:PATH += ";C:\Program Files\CMake\bin"

# 3. Verify installation
cmake --version
```

**On Linux (Ubuntu/Debian)**:
```bash
# Install CMake
sudo apt update
sudo apt install cmake build-essential

# Verify
cmake --version
```

**On macOS**:
```bash
# Using Homebrew
brew install cmake

# Or download from official website
```

### ❗ "Compiler not found" or C++ issues

**Symptoms**:
```
No CMAKE_CXX_COMPILER could be found
```

**Solutions**:

**On Windows**:
```bash
# Install Visual Studio Community (free)
# Or install Build Tools for Visual Studio

# Verify compiler
where cl
```

**On Linux**:
```bash
# Install GCC
sudo apt install g++ gcc

# Verify
g++ --version
```

**On macOS**:
```bash
# Install Xcode Command Line Tools
xcode-select --install

# Verify
clang++ --version
```

### ❗ "Git not found" or download issues

**Symptoms**:
```
'git' is not recognized as an internal or external command
```

**Solutions**:
```bash
# Download and install Git from
# https://git-scm.com/download

# Verify installation
git --version

# Retry
git clone https://github.com/sadlang/sad-language.git
```

### ❗ Build Errors

**Error**: Corrupted `CMakeCache.txt`
```bash
# Delete old build files
Remove-Item -Recurse -Force .\build\*
cd build

# Reconfigure
cmake ..
cmake --build . --config Release
```

**Error**: Missing libraries
```bash
# Verify all requirements
cmake --version  # >= 3.10
g++ --version    # or cl.exe on Windows

# Rebuild with verbose output
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
```

---

## 🚨 Runtime Errors

### ❗ "File not found" when running programs

**Symptoms**:
```
Error: Could not open file 'myprogram.s'
```

**Solutions**:
```bash
# 1. Check if file exists
ls myprogram.s     # Linux/macOS
dir myprogram.s    # Windows

# 2. Check current directory
pwd               # Linux/macOS  
Get-Location      # Windows PowerShell

# 3. Use absolute path
.\build\bin\Debug\sad.exe "C:\full\path\to\myprogram.s"

# 4. Check file extension (.s)
```

### ❗ "Lexical Error" - code reading errors

**Common examples**:

**1. Invalid characters**:
```sad
# ❌ Error - unsupported characters
رقم العمر = 25 ; # using normal ; instead of Arabic ؛
```

**Solution**:
```sad
# ✅ Correct - use normal ;
رقم العمر = 25 ;
```

**2. Mixed quotation marks**:
```sad
# ❌ Error - different quotation marks
اطبع("مرحبا") ;   # using " and "
```

**Solution**:
```sad
# ✅ Correct - matching quotation marks
اطبع("مرحبا") ;   # " and " or ' and '
```

### ❗ "Parse Error" - syntax errors

**Examples and solutions**:

**1. Unmatched parentheses**:
```sad
# ❌ Error
إذا ( العمر >= 18
    اطبع("بالغ") ;
نهاية

# ✅ Correct
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
نهاية
```

**2. Forgetting `نهاية` keyword**:
```sad
# ❌ Error
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
# Forgot نهاية

رقم متغير = 10 ;

# ✅ Correct
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
نهاية

رقم متغير = 10 ;
```

**3. Wrong keyword order**:
```sad
# ❌ Error - وإلا before إذا
وإلا
    اطبع("قاصر") ;
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
نهاية

# ✅ Correct
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
وإلا
    اطبع("قاصر") ;
نهاية
```

### ❗ "Runtime Error" - execution-time errors

**1. Division by zero**:
```sad
# ❌ Causes error
رقم أ = 10 ;
رقم ب = 0 ;
رقم النتيجة = أ / ب ;  # Runtime Error!

# ✅ Solution - check first
رقم أ = 10 ;
رقم ب = 0 ;
إذا ( ب != 0 )
    رقم النتيجة = أ / ب ;
    اطبع("النتيجة:", النتيجة) ;
وإلا
    اطبع("Error: Cannot divide by zero") ;
نهاية
```

**2. Index out of bounds**:
```sad
# ❌ Causes error
مصفوفة أرقام = [1, 2, 3] ;
اطبع(أرقام[10]) ;  # Runtime Error!

# ✅ Solution - check bounds
مصفوفة أرقام = [1, 2, 3] ;
رقم الفهرس = 10 ;
إذا ( الفهرس < طول(أرقام) )
    اطبع(أرقام[الفهرس]) ;
وإلا
    اطبع("Index out of bounds") ;
نهاية
```

**3. Undefined variable**:
```sad
# ❌ Causes error
اطبع(متغير_غير_موجود) ;  # Runtime Error!

# ✅ Solution - define variable first
رقم متغير_معرف = 25 ;
اطبع(متغير_معرف) ;
```

---

## 🔤 Character Encoding Issues

### ❗ Arabic text not displaying correctly

**Symptoms**:
```
# Instead of: مرحباً
# Shows: ????????? or empty squares
```

**Solutions**:

**1. Ensure file encoding is UTF-8**:
```bash
# In VS Code
# File -> Save with Encoding -> UTF-8

# In Notepad++
# Encoding -> Convert to UTF-8
```

**2. Setup Terminal for Arabic support**:

**Windows PowerShell**:
```powershell
# Change code page to UTF-8 support
chcp 65001

# Or add to profile
echo 'chcp 65001' >> $PROFILE
```

**Windows Command Prompt**:
```cmd
chcp 65001
```

**3. Editor configuration**:

**VS Code** - `settings.json`:
```json
{
    "files.encoding": "utf8",
    "terminal.integrated.defaultProfile.windows": "PowerShell",
    "terminal.integrated.profiles.windows": {
        "PowerShell": {
            "source": "PowerShell",
            "args": ["-NoExit", "-Command", "chcp 65001"]
        }
    }
}
```

### ❗ Arabic characters appear disconnected

**Problem**: Characters don't connect to each other

**Solution**:
```bash
# Ensure using font that supports Arabic
# Suggested Fonts:
# - Arabic Typesetting
# - Traditional Arabic  
# - Tahoma
# - Calibri
```

**In VS Code**:
```json
{
    "editor.fontFamily": "Arabic Typesetting, Consolas, monospace",
    "editor.fontSize": 14
}
```

### ❗ Wrong text direction (LTR instead of RTL)

**VS Code settings**:
```json
{
    "editor.detectLanguage": true,
    "bidi.editingEnable": true
}
```

**Or use Alt+Shift to toggle direction**

---

## 💾 Memory and Performance Issues

### ❗ "Out of Memory" or memory exhaustion

**Symptoms**:
```
Error: Out of memory
Segmentation fault
```

**Causes and solutions**:

**1. Infinite loops**:
```sad
# ❌ Infinite loop
رقم عداد = 0 ;
بينما ( صحيح )  # Never ends!
    عداد = عداد + 1 ;
    اطبع(عداد) ;
نهاية

# ✅ Solution - clear termination condition
رقم عداد = 0 ;
بينما ( عداد < 100 )  # Clear termination condition
    عداد = عداد + 1 ;
    اطبع(عداد) ;
نهاية
```

**2. Arrays that are too large**:
```sad
# ❌ May cause memory issues
مصفوفة كبيرة = [] ;
رقم عداد = 0 ;
بينما ( عداد < 10000000 )  # 10 million elements!
    كبيرة = أضف_عنصر(كبيرة, عداد) ;
    عداد = عداد + 1 ;
نهاية

# ✅ Solution - progressive processing
رقم عداد = 0 ;
بينما ( عداد < 10000000 )
    # Process element directly instead of storing
    رقم نتيجة = عداد * 2 ;
    # Use result
    عداد = عداد + 1 ;
نهاية
```

**3. Deep recursive function calls**:
```sad
# ❌ May cause Stack Overflow
دالة عد_تنازلي( رقم العدد )
    إذا ( العدد > 0 )
        اطبع(العدد) ;
        عد_تنازلي(العدد - 1) ;  # Deep recursive call
    نهاية
نهاية

عد_تنازلي(100000) ;  # Danger!

# ✅ Solution - use loop
دالة عد_تنازلي_آمن( رقم العدد )
    بينما ( العدد > 0 )
        اطبع(العدد) ;
        العدد = العدد - 1 ;
    نهاية
نهاية
```

### ❗ Program is very slow

**Diagnosis and solutions**:

**1. Performance measurement**:
```sad
# Add timers
رقم البداية = وقت_الآن() ;

# Code you want to measure
رقم عداد = 0 ;
بينما ( عداد < 1000000 )
    # Complex operations
    عداد = عداد + 1 ;
نهاية

رقم النهاية = وقت_الآن() ;
رقم المدة = النهاية - البداية ;
اطبع("Took:", المدة, "seconds") ;
```

**2. Loop optimization**:
```sad
# ❌ Slow - calculation every time
مصفوفة البيانات = [1, 2, 3, /*...1000 elements*/] ;
رقم عداد = 0 ;
بينما ( عداد < طول(البيانات) )  # طول() calculated every time!
    # Processing
    عداد = عداد + 1 ;
نهاية

# ✅ Faster - store value
مصفوفة البيانات = [1, 2, 3, /*...1000 elements*/] ;
رقم الحد = طول(البيانات) ;  # Calculate only once
رقم عداد = 0 ;
بينما ( عداد < الحد )
    # Processing
    عداد = عداد + 1 ;
نهاية
```

---

## 🏗️ Object-Oriented Programming Issues

### ❗ "Method not found" or function not found

**Symptoms**:
```
Error: Method 'method_name' not found in class 'class_name'
```

**Examples and solutions**:

```sad
صنف طالب
    نص الاسم ;
    
    دالة طالب( نص اسم )
        الاسم = اسم ;
    نهاية
    
    # This method exists
    دالة احصل_على_الاسم()
        أرجع الاسم ;
    نهاية
نهاية

طالب أحمد = جديد طالب("أحمد") ;

# ✅ Works - method exists
نص الاسم = أحمد.احصل_على_الاسم() ;

# ❌ Error - method doesn't exist
# أحمد.احصل_على_العمر() ;  # Method not found!
```

**Solution**:
```sad
# Add missing method
صنف طالب
    نص الاسم ;
    رقم العمر ;
    
    دالة طالب( نص اسم ، رقم عمر )
        الاسم = اسم ;
        العمر = عمر ;
    نهاية
    
    دالة احصل_على_الاسم()
        أرجع الاسم ;
    نهاية
    
    # Add required method
    دالة احصل_على_العمر()
        أرجع العمر ;
    نهاية
نهاية
```

### ❗ Inheritance issues

**1. Calling base constructor**:
```sad
# ❌ Common error
صنف طالب_جامعي يرث طالب
    نص التخصص ;
    
    دالة طالب_جامعي( نص اسم ، رقم عمر ، نص تخصص )
        # Forgot to call base constructor
        التخصص = تخصص ;
    نهاية
نهاية

# ✅ Correct solution
صنف طالب_جامعي يرث طالب
    نص التخصص ;
    
    دالة طالب_جامعي( نص اسم ، رقم عمر ، نص تخصص )
        عام(اسم, عمر) ;  # Call base constructor
        التخصص = تخصص ;
    نهاية
نهاية
```

**2. Accessing private properties**:
```sad
صنف حساب_بنكي
    خاص رقم الرصيد ;
    
    # ... class methods
نهاية

حساب_بنكي حسابي = جديد حساب_بنكي() ;

# ❌ Error - property is private
# حسابي.الرصيد = 1000 ;  # Error: Private property

# ✅ Solution - use public methods
# Must add public method for modification
```

---

## 🖥️ Editor and Environment Issues

### ❗ VS Code doesn't recognize .s files

**Solution**:
```json
// In .vscode/settings.json
{
    "files.associations": {
        "*.s": "sad-language"
    }
}
```

**Or create simple Language Extension**:
```json
// package.json for VS Code extension
{
    "contributes": {
        "languages": [{
            "id": "sad",
            "aliases": ["Sad Language", "sad"],
            "extensions": [".s"],
            "configuration": "./language-configuration.json"
        }]
    }
}
```

### ❗ Issues running from within editor

**Setup tasks.json for quick execution**:
```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Run Sad Program",
            "type": "shell",
            "command": "${workspaceFolder}/build/bin/Debug/sad.exe",
            "args": ["${file}"],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "presentation": {
                "echo": true,
                "reveal": "always",
                "focus": false,
                "panel": "shared"
            },
            "problemMatcher": []
        }
    ]
}
```

**Setup launch.json for debugging**:
```json
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Debug Sad Program",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceFolder}/build/bin/Debug/sad.exe",
            "args": ["${file}"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [],
            "externalConsole": true
        }
    ]
}
```

### ❗ Font not suitable for Arabic

**Solution in VS Code**:
```json
{
    "editor.fontFamily": "'Arabic Typesetting', 'Tahoma', 'Segoe UI', monospace",
    "editor.fontSize": 16,
    "editor.lineHeight": 24
}
```

**Recommended fonts for Arabic**:
- Arabic Typesetting
- Traditional Arabic
- Tahoma
- Segoe UI
- Amiri (free font)

---

## 🔍 Diagnostic Tools

### 🛠️ Build issues diagnosis

**Comprehensive check script**:
```powershell
# Environment check
Write-Host "=== Sad Language Build Diagnostics ==="

# Check requirements
Write-Host "1. Checking requirements..."
cmake --version
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ CMake not found" -ForegroundColor Red
} else {
    Write-Host "✅ CMake found" -ForegroundColor Green
}

# Check Compiler
Write-Host "2. Checking compiler..."
where cl.exe
if ($LASTEXITCODE -ne 0) {
    g++ --version
    if ($LASTEXITCODE -ne 0) {
        Write-Host "❌ No C++ compiler found" -ForegroundColor Red
    } else {
        Write-Host "✅ GCC compiler found" -ForegroundColor Green
    }
} else {
    Write-Host "✅ MSVC compiler found" -ForegroundColor Green
}

# Check Git
Write-Host "3. Checking Git..."
git --version
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Git not found" -ForegroundColor Red
} else {
    Write-Host "✅ Git found" -ForegroundColor Green
}

# Check project files
Write-Host "4. Checking project files..."
if (Test-Path "CMakeLists.txt") {
    Write-Host "✅ CMakeLists.txt found" -ForegroundColor Green
} else {
    Write-Host "❌ CMakeLists.txt not found" -ForegroundColor Red
}

if (Test-Path "src/") {
    Write-Host "✅ src/ directory found" -ForegroundColor Green
} else {
    Write-Host "❌ src/ directory not found" -ForegroundColor Red
}

Write-Host "=== End Diagnostics ==="
```

### 🛠️ Quick language test

**File test_basic.s**:
```sad
# Basic language test
اطبع("=== Basic Sad Language Test ===") ;

# Test variables
رقم العمر = 25 ;
نص الاسم = "أحمد" ;
منطق طالب = صحيح ;

اطبع("Age:", العمر) ;
اطبع("Name:", الاسم) ;
اطبع("Student:", طالب) ;

# Test arithmetic operations
رقم أ = 10 ;
رقم ب = 5 ;
اطبع("Addition:", أ + ب) ;
اطبع("Multiplication:", أ * ب) ;

# Test conditions
إذا ( العمر >= 18 )
    اطبع("Adult") ;
وإلا
    اطبع("Minor") ;
نهاية

# Test loops
رقم عداد = 1 ;
بينما ( عداد <= 3 )
    اطبع("Number:", عداد) ;
    عداد = عداد + 1 ;
نهاية

اطبع("=== Test Complete ===") ;
```

**Run test**:
```bash
# In project directory
.\build\bin\Debug\sad.exe test_basic.s
```

**Expected output**:
```
=== Basic Sad Language Test ===
Age: 25
Name: أحمد
Student: true
Addition: 15
Multiplication: 50
Adult
Number: 1
Number: 2
Number: 3
=== Test Complete ===
```

### 🛠️ Memory leak detection

**On Linux with Valgrind**:
```bash
# Install valgrind
sudo apt install valgrind

# Run program with memory check
valgrind --leak-check=full ./sad myprogram.s

# Look for:
# - Memory leaks
# - Invalid memory access
# - Uninitialized variables
```

**On Windows with Visual Studio**:
```cpp
// Add at beginning of main.cpp
#ifdef _DEBUG
#include <crtdbg.h>
#endif

int main() {
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    
    // Rest of code...
}
```

---

## 📞 Getting Additional Help

### 🆘 When the mentioned solutions don't work

**1. Gather system information**:
```powershell
# Windows
Get-ComputerInfo | Select-Object WindowsProductName, WindowsVersion, TotalPhysicalMemory

# PowerShell version
$PSVersionTable

# .NET version
Get-ItemProperty "HKLM:SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Full\" -Name Release
```

**2. Create comprehensive issue report**:

**GitHub Issue Template**:
```markdown
## Problem Description
[Brief description of the issue]

## Steps to Reproduce
1. [Step 1]
2. [Step 2]  
3. [Step 3]

## Expected Behavior
[What should have happened]

## Actual Behavior
[What actually happened]

## System Environment
- **Operating System**: Windows 11 / Ubuntu 22.04 / macOS 13
- **CMake Version**: [result of cmake --version]
- **Compiler**: [result of g++ --version or cl]
- **Sad Language Version**: [from git log --oneline -1]

## Complete Error Message
```
[Copy and paste complete error message]
```

## Code Causing the Issue
```sad
[Code that causes the issue]
```

## Solution Attempts
- [x] Tried Solution A
- [ ] Tried Solution B
- [x] Read documentation
```

**3. Help sources**:
- **GitHub Issues**: [github.com/sadlang/sad-language/issues](https://github.com/sadlang/sad-language/issues)
- **Discord Community**: [discord.gg/sadlang](https://discord.gg/sadlang)
- **Email**: support@sadlang.org

---

**🔧 This is a comprehensive guide for solving technical issues in Sad Programming Language**

If you don't find a solution to your problem here, don't hesitate to ask for help from the community or development team.

---

*Last updated: November 28, 2025*  
*Sad Language - Version 1.0*