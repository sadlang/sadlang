# المرحلة 6: الاختبار والتحقق

## 🎯 الهدف

التأكد من أن إعادة التنظيم تمت بنجاح وأن المشروع يعمل بشكل صحيح.

---

## 📋 قائمة الاختبارات

### 1. اختبار البناء

```powershell
# تنظيف البناء السابق
Remove-Item -Recurse -Force "build" -ErrorAction SilentlyContinue

# إنشاء مجلد البناء
New-Item -ItemType Directory -Path "build" -Force
Set-Location "build"

# توليد ملفات البناء
cmake .. -G "Visual Studio 17 2022"

# بناء Debug
cmake --build . --config Debug

# بناء Release
cmake --build . --config Release
```

### 2. اختبار التشغيل

```powershell
# اختبار المفسر
.\bin\Debug\sad.exe --version
.\bin\Debug\sad.exe ..\test_basic.s

# اختبار الملفات المختلفة
.\bin\Debug\sad.exe ..\test_if_complete.s
.\bin\Debug\sad.exe ..\test_phase4_for.s
.\bin\Debug\sad.exe ..\test_phase5_array.s
```

### 3. اختبار الخيارات

```powershell
# بناء بدون المترجم
cmake .. -DSAD_BUILD_COMPILER=OFF
cmake --build . --config Debug

# بناء بدون الرسوميات
cmake .. -DSAD_BUILD_GRAPHICS=OFF
cmake --build . --config Debug

# بناء كامل
cmake .. -DSAD_BUILD_COMPILER=ON -DSAD_BUILD_GRAPHICS=ON
cmake --build . --config Debug
```

---

## 🧪 اختبارات الوحدات

### اختبار shared/

```powershell
# اختبار Lexer
.\bin\Debug\test_lexer.exe

# اختبار Parser
.\bin\Debug\test_parser.exe

# اختبار AST
.\bin\Debug\test_ast.exe
```

### اختبار interpreter/

```powershell
# اختبار Expression Evaluator
.\bin\Debug\test_expression_evaluator.exe

# اختبار Statement Executor
.\bin\Debug\test_statement_executor.exe
```

### اختبار compiler/

```powershell
# اختبار SIR Builder
.\bin\Debug\test_sir_builder.exe

# اختبار LLVM Codegen
.\bin\Debug\test_llvm_codegen.exe

# اختبار JIT
.\bin\Debug\test_jit.exe
```

---

## 📊 جدول التحقق

### المكونات الأساسية

| المكون | البناء | التشغيل | الاختبار |
|--------|--------|---------|----------|
| shared | ⬜ | ⬜ | ⬜ |
| interpreter | ⬜ | ⬜ | ⬜ |
| compiler | ⬜ | ⬜ | ⬜ |
| runtime | ⬜ | ⬜ | ⬜ |
| tools | ⬜ | ⬜ | ⬜ |

### الميزات

| الميزة | يعمل |
|--------|------|
| تحليل الكود | ⬜ |
| تنفيذ البرامج | ⬜ |
| الدوال | ⬜ |
| الأصناف | ⬜ |
| الاستيراد | ⬜ |
| Pattern Matching | ⬜ |
| المكتبة القياسية | ⬜ |

---

## 🔍 التحقق من المسارات

### التحقق من Include Paths

```powershell
# البحث عن أخطاء include
Get-ChildItem -Recurse -Filter "*.cpp" | ForEach-Object {
    $content = Get-Content $_.FullName -Raw
    if ($content -match '#include.*not found') {
        Write-Output "Error in: $($_.FullName)"
    }
}
```

### التحقق من الروابط

```powershell
# التحقق من أن جميع المكتبات مرتبطة
cmake --build . --config Debug 2>&1 | Select-String "unresolved"
```

---

## 📝 قائمة التحقق النهائية

### قبل الدمج

- [ ] جميع الملفات منسوخة
- [ ] جميع CMakeLists.txt محدثة
- [ ] البناء Debug ناجح
- [ ] البناء Release ناجح
- [ ] الاختبارات الأساسية تمر
- [ ] لا توجد تحذيرات خطيرة

### بعد الدمج

- [ ] حذف المجلدات القديمة
- [ ] تحديث .gitignore
- [ ] تحديث README.md
- [ ] تحديث ROADMAP.md
- [ ] إنشاء commit جديد

---

## 🛠️ إصلاح المشاكل الشائعة

### مشكلة: Include not found

```cpp
// قبل
#include "lexer.h"

// بعد
#include "shared/lexer/lexer.h"
// أو استخدم CMake include directories
```

### مشكلة: Unresolved external symbol

تأكد من:
1. الملف مضاف في CMakeLists.txt
2. المكتبة مرتبطة في target_link_libraries

### مشكلة: Multiple definitions

تأكد من:
1. عدم تضمين نفس الملف مرتين
2. استخدام #pragma once أو include guards

---

## 📦 النسخ الاحتياطي

### قبل أي تغيير

```powershell
# إنشاء نسخة احتياطية
$date = Get-Date -Format "yyyyMMdd_HHmmss"
$backup = "C:\s\s_language_backup_$date"
Copy-Item -Recurse "C:\s\s_language" $backup

Write-Output "Backup created at: $backup"
```

### استعادة النسخة الاحتياطية

```powershell
# في حالة الحاجة للاستعادة
$backup = "C:\s\s_language_backup_XXXXXXXX_XXXXXX"
Remove-Item -Recurse -Force "C:\s\s_language"
Copy-Item -Recurse $backup "C:\s\s_language"
```

---

## ✅ انتهاء التنظيم

بعد اكتمال جميع الخطوات:

1. ✅ shared/ - المكونات المشتركة
2. ✅ interpreter/ - المفسر
3. ✅ compiler/ - المترجم
4. ✅ runtime/ - وقت التشغيل
5. ✅ tools/ - الأدوات
6. ✅ CMake محدث
7. ✅ جميع الاختبارات تمر

---

## 📈 التحسينات المستقبلية

بعد اكتمال التنظيم، يمكن:

1. إضافة المزيد من الاختبارات
2. تحسين التوثيق
3. إضافة CI/CD
4. تحسين الأداء
5. إضافة ميزات جديدة
