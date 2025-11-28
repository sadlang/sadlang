# 🔧 دليل حل المشاكل - لغة ص
# Troubleshooting Guide - Sad Programming Language

**دليل شامل لحل المشاكل التقنية والأخطاء الشائعة 🛠️**

---

## 📑 فهرس المحتويات

1. [مشاكل التثبيت](#مشاكل-التثبيت)
2. [أخطاء التشغيل](#أخطاء-التشغيل)
3. [مشاكل الترميز والأحرف](#مشاكل-الترميز-والأحرف)
4. [أخطاء الذاكرة والأداء](#أخطاء-الذاكرة-والأداء)
5. [مشاكل البرمجة الكائنية](#مشاكل-البرمجة-الكائنية)
6. [مشاكل المحرر والبيئة](#مشاكل-المحرر-والبيئة)
7. [أدوات التشخيص](#أدوات-التشخيص)

---

## 💻 مشاكل التثبيت

### ❗ "CMake not found" أو "cmake command not recognized"

**الأعراض**:
```
'cmake' is not recognized as an internal or external command
```

**الحلول**:

**على Windows**:
```bash
# 1. تحميل وتثبيت CMake من الموقع الرسمي
# https://cmake.org/download/

# 2. إضافة CMake إلى PATH
$env:PATH += ";C:\Program Files\CMake\bin"

# 3. التحقق من التثبيت
cmake --version
```

**على Linux (Ubuntu/Debian)**:
```bash
# تثبيت CMake
sudo apt update
sudo apt install cmake build-essential

# التحقق
cmake --version
```

**على macOS**:
```bash
# باستخدام Homebrew
brew install cmake

# أو تحميل من الموقع الرسمي
```

### ❗ "Compiler not found" أو مشاكل C++

**الأعراض**:
```
No CMAKE_CXX_COMPILER could be found
```

**الحلول**:

**على Windows**:
```bash
# تثبيت Visual Studio Community (مجاني)
# أو تثبيت Build Tools for Visual Studio

# التحقق من compiler
where cl
```

**على Linux**:
```bash
# تثبيت GCC
sudo apt install g++ gcc

# التحقق
g++ --version
```

**على macOS**:
```bash
# تثبيت Xcode Command Line Tools
xcode-select --install

# التحقق
clang++ --version
```

### ❗ "Git not found" أو مشاكل التحميل

**الأعراض**:
```
'git' is not recognized as an internal or external command
```

**الحلول**:
```bash
# تحميل وتثبيت Git من
# https://git-scm.com/download

# التحقق من التثبيت
git --version

# إعادة المحاولة
git clone https://github.com/sadlang/sad-language.git
```

### ❗ مشاكل البناء (Build Errors)

**خطأ**: `CMakeCache.txt` مُعطل
```bash
# حذف ملفات البناء القديمة
Remove-Item -Recurse -Force .\build\*
cd build

# إعادة التكوين
cmake ..
cmake --build . --config Release
```

**خطأ**: مكتبات مفقودة
```bash
# التأكد من وجود جميع المتطلبات
cmake --version  # >= 3.10
g++ --version    # أو cl.exe على Windows

# إعادة البناء بمعلومات تشخيصية
cmake .. -DCMAKE_VERBOSE_MAKEFILE=ON
```

---

## 🚨 أخطاء التشغيل

### ❗ "File not found" عند تشغيل البرامج

**الأعراض**:
```
Error: Could not open file 'myprogram.s'
```

**الحلول**:
```bash
# 1. التحقق من وجود الملف
ls myprogram.s     # Linux/macOS
dir myprogram.s    # Windows

# 2. التحقق من المسار الحالي
pwd               # Linux/macOS  
Get-Location      # Windows PowerShell

# 3. استخدام المسار الكامل
.\build\bin\Debug\sad.exe "C:\full\path\to\myprogram.s"

# 4. التحقق من امتداد الملف (.s)
```

### ❗ "Lexical Error" - أخطاء في قراءة الكود

**أمثلة شائعة**:

**1. أحرف غير صالحة**:
```sad
# ❌ خطأ - أحرف غير مدعومة
رقم العمر = 25 ; # استخدام ; عادي بدلاً من ؛ عربي
```

**الحل**:
```sad
# ✅ صحيح - استخدام ; عادي
رقم العمر = 25 ;
```

**2. علامات اقتباس مختلطة**:
```sad
# ❌ خطأ - علامات اقتباس مختلفة
اطبع("مرحبا") ;   # استخدام " و "
```

**الحل**:
```sad
# ✅ صحيح - علامات اقتباس متماثلة
اطبع("مرحبا") ;   # " و " أو ' و '
```

### ❗ "Parse Error" - أخطاء في بناء الجملة

**أمثلة وحلول**:

**1. أقواس غير متطابقة**:
```sad
# ❌ خطأ
إذا ( العمر >= 18
    اطبع("بالغ") ;
نهاية

# ✅ صحيح
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
نهاية
```

**2. نسيان كلمة `نهاية`**:
```sad
# ❌ خطأ
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
# نسيان نهاية

رقم متغير = 10 ;

# ✅ صحيح
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
نهاية

رقم متغير = 10 ;
```

**3. ترتيب كلمات مفاتيح خاطئ**:
```sad
# ❌ خطأ - وإلا قبل إذا
وإلا
    اطبع("قاصر") ;
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
نهاية

# ✅ صحيح
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
وإلا
    اطبع("قاصر") ;
نهاية
```

### ❗ "Runtime Error" - أخطاء وقت التشغيل

**1. قسمة على صفر**:
```sad
# ❌ يسبب خطأ
رقم أ = 10 ;
رقم ب = 0 ;
رقم النتيجة = أ / ب ;  # Runtime Error!

# ✅ الحل - التحقق أولاً
رقم أ = 10 ;
رقم ب = 0 ;
إذا ( ب != 0 )
    رقم النتيجة = أ / ب ;
    اطبع("النتيجة:", النتيجة) ;
وإلا
    اطبع("خطأ: لا يمكن القسمة على صفر") ;
نهاية
```

**2. فهرس خارج النطاق**:
```sad
# ❌ يسبب خطأ
مصفوفة أرقام = [1, 2, 3] ;
اطبع(أرقام[10]) ;  # Runtime Error!

# ✅ الحل - التحقق من الحدود
مصفوفة أرقام = [1, 2, 3] ;
رقم الفهرس = 10 ;
إذا ( الفهرس < طول(أرقام) )
    اطبع(أرقام[الفهرس]) ;
وإلا
    اطبع("الفهرس خارج النطاق") ;
نهاية
```

**3. متغير غير معرف**:
```sad
# ❌ يسبب خطأ
اطبع(متغير_غير_موجود) ;  # Runtime Error!

# ✅ الحل - تعريف المتغير أولاً
رقم متغير_معرف = 25 ;
اطبع(متغير_معرف) ;
```

---

## 🔤 مشاكل الترميز والأحرف

### ❗ النص العربي لا يظهر بشكل صحيح

**الأعراض**:
```
# بدلاً من: مرحباً
# يظهر: ????????? أو مربعات فارغة
```

**الحلول**:

**1. تأكد من ترميز الملف (UTF-8)**:
```bash
# في VS Code
# File -> Save with Encoding -> UTF-8

# في Notepad++
# Encoding -> Convert to UTF-8
```

**2. إعداد Terminal للدعم العربي**:

**Windows PowerShell**:
```powershell
# تغيير code page للدعم UTF-8
chcp 65001

# أو إضافة إلى profile
echo 'chcp 65001' >> $PROFILE
```

**Windows Command Prompt**:
```cmd
chcp 65001
```

**3. إعداد المحرر**:

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

### ❗ الأحرف العربية تظهر منفصلة

**المشكلة**: الأحرف لا تتصل ببعضها

**الحل**:
```bash
# تأكد من استخدام خط يدعم العربية
# Fonts المقترحة:
# - Arabic Typesetting
# - Traditional Arabic  
# - Tahoma
# - Calibri
```

**في VS Code**:
```json
{
    "editor.fontFamily": "Arabic Typesetting, Consolas, monospace",
    "editor.fontSize": 14
}
```

### ❗ اتجاه النص خاطئ (LTR بدلاً من RTL)

**VS Code إعدادات**:
```json
{
    "editor.detectLanguage": true,
    "bidi.editingEnable": true
}
```

**أو استخدام Alt+Shift لتبديل الاتجاه**

---

## 💾 أخطاء الذاكرة والأداء

### ❗ "Out of Memory" أو استنفاد الذاكرة

**الأعراض**:
```
Error: Out of memory
Segmentation fault
```

**الأسباب والحلول**:

**1. حلقات لا نهائية**:
```sad
# ❌ حلقة لا نهائية
رقم عداد = 0 ;
بينما ( صحيح )  # لا ينتهي أبداً!
    عداد = عداد + 1 ;
    اطبع(عداد) ;
نهاية

# ✅ الحل - شرط إنهاء واضح
رقم عداد = 0 ;
بينما ( عداد < 100 )  # شرط إنهاء واضح
    عداد = عداد + 1 ;
    اطبع(عداد) ;
نهاية
```

**2. مصفوفات كبيرة جداً**:
```sad
# ❌ قد يسبب مشاكل ذاكرة
مصفوفة كبيرة = [] ;
رقم عداد = 0 ;
بينما ( عداد < 10000000 )  # 10 مليون عنصر!
    كبيرة = أضف_عنصر(كبيرة, عداد) ;
    عداد = عداد + 1 ;
نهاية

# ✅ الحل - معالجة تدريجية
رقم عداد = 0 ;
بينما ( عداد < 10000000 )
    # معالجة العنصر مباشرة بدلاً من التخزين
    رقم نتيجة = عداد * 2 ;
    # استخدام النتيجة
    عداد = عداد + 1 ;
نهاية
```

**3. استدعاء دوال متداخل عميق**:
```sad
# ❌ قد يسبب Stack Overflow
دالة عد_تنازلي( رقم العدد )
    إذا ( العدد > 0 )
        اطبع(العدد) ;
        عد_تنازلي(العدد - 1) ;  # استدعاء متكرر عميق
    نهاية
نهاية

عد_تنازلي(100000) ;  # خطر!

# ✅ الحل - استخدام حلقة
دالة عد_تنازلي_آمن( رقم العدد )
    بينما ( العدد > 0 )
        اطبع(العدد) ;
        العدد = العدد - 1 ;
    نهاية
نهاية
```

### ❗ البرنامج بطيء جداً

**التشخيص والحلول**:

**1. قياس الأداء**:
```sad
# إضافة مؤقتات
رقم البداية = وقت_الآن() ;

# الكود الذي تريد قياسه
رقم عداد = 0 ;
بينما ( عداد < 1000000 )
    # عمليات معقدة
    عداد = عداد + 1 ;
نهاية

رقم النهاية = وقت_الآن() ;
رقم المدة = النهاية - البداية ;
اطبع("استغرق:", المدة, "ثانية") ;
```

**2. تحسين الحلقات**:
```sad
# ❌ بطيء - حساب في كل مرة
مصفوفة البيانات = [1, 2, 3, /*...1000 عنصر*/] ;
رقم عداد = 0 ;
بينما ( عداد < طول(البيانات) )  # طول() يُحسب في كل مرة!
    # معالجة
    عداد = عداد + 1 ;
نهاية

# ✅ أسرع - حفظ القيمة
مصفوفة البيانات = [1, 2, 3, /*...1000 عنصر*/] ;
رقم الحد = طول(البيانات) ;  # حساب مرة واحدة فقط
رقم عداد = 0 ;
بينما ( عداد < الحد )
    # معالجة
    عداد = عداد + 1 ;
نهاية
```

---

## 🏗️ مشاكل البرمجة الكائنية

### ❗ "Method not found" أو دالة غير موجودة

**الأعراض**:
```
Error: Method 'method_name' not found in class 'class_name'
```

**أمثلة وحلول**:

```sad
صنف طالب
    نص الاسم ;
    
    دالة طالب( نص اسم )
        الاسم = اسم ;
    نهاية
    
    # هذه الدالة موجودة
    دالة احصل_على_الاسم()
        أرجع الاسم ;
    نهاية
نهاية

طالب أحمد = جديد طالب("أحمد") ;

# ✅ يعمل - الدالة موجودة
نص الاسم = أحمد.احصل_على_الاسم() ;

# ❌ خطأ - الدالة غير موجودة
# أحمد.احصل_على_العمر() ;  # Method not found!
```

**الحل**:
```sad
# إضافة الدالة المفقودة
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
    
    # إضافة الدالة المطلوبة
    دالة احصل_على_العمر()
        أرجع العمر ;
    نهاية
نهاية
```

### ❗ مشاكل الوراثة

**1. استدعاء البانى الأساسي**:
```sad
# ❌ خطأ شائع
صنف طالب_جامعي يرث طالب
    نص التخصص ;
    
    دالة طالب_جامعي( نص اسم ، رقم عمر ، نص تخصص )
        # نسيان استدعاء البانى الأساسي
        التخصص = تخصص ;
    نهاية
نهاية

# ✅ الحل الصحيح
صنف طالب_جامعي يرث طالب
    نص التخصص ;
    
    دالة طالب_جامعي( نص اسم ، رقم عمر ، نص تخصص )
        عام(اسم, عمر) ;  # استدعاء البانى الأساسي
        التخصص = تخصص ;
    نهاية
نهاية
```

**2. الوصول للخصائص الخاصة**:
```sad
صنف حساب_بنكي
    خاص رقم الرصيد ;
    
    # ... دوال الصنف
نهاية

حساب_بنكي حسابي = جديد حساب_بنكي() ;

# ❌ خطأ - الخاصية خاصة
# حسابي.الرصيد = 1000 ;  # Error: Private property

# ✅ الحل - استخدام دوال عامة
# يجب إضافة دالة عامة للتعديل
```

---

## 🖥️ مشاكل المحرر والبيئة

### ❗ VS Code لا يتعرف على ملفات .s

**الحل**:
```json
// في .vscode/settings.json
{
    "files.associations": {
        "*.s": "sad-language"
    }
}
```

**أو إنشاء Language Extension بسيط**:
```json
// package.json لامتداد VS Code
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

### ❗ مشاكل التشغيل من داخل المحرر

**إعداد tasks.json للتشغيل السريع**:
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

**إعداد launch.json للتصحيح**:
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

### ❗ خط غير مناسب للعربية

**الحل في VS Code**:
```json
{
    "editor.fontFamily": "'Arabic Typesetting', 'Tahoma', 'Segoe UI', monospace",
    "editor.fontSize": 16,
    "editor.lineHeight": 24
}
```

**خطوط مقترحة للعربية**:
- Arabic Typesetting
- Traditional Arabic
- Tahoma
- Segoe UI
- Amiri (خط حر)

---

## 🔍 أدوات التشخيص

### 🛠️ تشخيص مشاكل البناء

**سكريبت فحص شامل**:
```powershell
# فحص البيئة
Write-Host "=== Sad Language Build Diagnostics ==="

# فحص المتطلبات
Write-Host "1. Checking requirements..."
cmake --version
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ CMake not found" -ForegroundColor Red
} else {
    Write-Host "✅ CMake found" -ForegroundColor Green
}

# فحص Compiler
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

# فحص Git
Write-Host "3. Checking Git..."
git --version
if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Git not found" -ForegroundColor Red
} else {
    Write-Host "✅ Git found" -ForegroundColor Green
}

# فحص ملفات المشروع
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

### 🛠️ اختبار سريع للغة

**ملف test_basic.s**:
```sad
# اختبار أساسي للغة
اطبع("=== اختبار أساسي للغة ص ===") ;

# اختبار المتغيرات
رقم العمر = 25 ;
نص الاسم = "أحمد" ;
منطق طالب = صحيح ;

اطبع("العمر:", العمر) ;
اطبع("الاسم:", الاسم) ;
اطبع("طالب:", طالب) ;

# اختبار العمليات الحسابية
رقم أ = 10 ;
رقم ب = 5 ;
اطبع("الجمع:", أ + ب) ;
اطبع("الضرب:", أ * ب) ;

# اختبار الشروط
إذا ( العمر >= 18 )
    اطبع("بالغ") ;
وإلا
    اطبع("قاصر") ;
نهاية

# اختبار الحلقات
رقم عداد = 1 ;
بينما ( عداد <= 3 )
    اطبع("العدد:", عداد) ;
    عداد = عداد + 1 ;
نهاية

اطبع("=== انتهى الاختبار ===") ;
```

**تشغيل الاختبار**:
```bash
# في مجلد المشروع
.\build\bin\Debug\sad.exe test_basic.s
```

**النتيجة المتوقعة**:
```
=== اختبار أساسي للغة ص ===
العمر: 25
الاسم: أحمد
طالب: true
الجمع: 15
الضرب: 50
بالغ
العدد: 1
العدد: 2
العدد: 3
=== انتهى الاختبار ===
```

### 🛠️ كشف تسريبات الذاكرة

**على Linux مع Valgrind**:
```bash
# تثبيت valgrind
sudo apt install valgrind

# تشغيل برنامج مع فحص الذاكرة
valgrind --leak-check=full ./sad myprogram.s

# البحث عن:
# - Memory leaks
# - Invalid memory access
# - Uninitialized variables
```

**على Windows مع Visual Studio**:
```cpp
// إضافة في بداية main.cpp
#ifdef _DEBUG
#include <crtdbg.h>
#endif

int main() {
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    
    // باقي الكود...
}
```

---

## 📞 الحصول على مساعدة إضافية

### 🆘 عندما لا تعمل الحلول المذكورة

**1. جمع معلومات النظام**:
```powershell
# Windows
Get-ComputerInfo | Select-Object WindowsProductName, WindowsVersion, TotalPhysicalMemory

# PowerShell version
$PSVersionTable

# .NET version
Get-ItemProperty "HKLM:SOFTWARE\Microsoft\NET Framework Setup\NDP\v4\Full\" -Name Release
```

**2. إنشاء تقرير مشكلة شامل**:

**Template للـ GitHub Issue**:
```markdown
## وصف المشكلة
[وصف مختصر للمشكلة]

## خطوات إعادة الإنتاج
1. [خطوة 1]
2. [خطوة 2]  
3. [خطوة 3]

## السلوك المتوقع
[ما كان يجب أن يحدث]

## السلوك الفعلي
[ما حدث فعلاً]

## بيئة النظام
- **نظام التشغيل**: Windows 11 / Ubuntu 22.04 / macOS 13
- **إصدار CMake**: [نتيجة cmake --version]
- **Compiler**: [نتيجة g++ --version أو cl]
- **إصدار لغة ص**: [من git log --oneline -1]

## رسالة الخطأ الكاملة
```
[نسخ ولصق رسالة الخطأ كاملة]
```

## الكود المسبب للمشكلة
```sad
[الكود الذي يسبب المشكلة]
```

## محاولات الحل
- [x] جربت الحل A
- [ ] جربت الحل B
- [x] قرأت التوثيق
```

**3. مصادر المساعدة**:
- **GitHub Issues**: [github.com/sadlang/sad-language/issues](https://github.com/sadlang/sad-language/issues)
- **Discord Community**: [discord.gg/sadlang](https://discord.gg/sadlang)
- **البريد الإلكتروني**: support@sadlang.org

---

**🔧 هذا دليل شامل لحل المشاكل التقنية في لغة ص**

إذا لم تجد حل مشكلتك هنا، لا تتردد في طلب المساعدة من المجتمع أو فريق التطوير.

---

*آخر تحديث: ٢٨ نوفمبر ٢٠٢٥*  
*لغة ص - الإصدار ١.٠*