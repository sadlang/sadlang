# المرحلة 1: توحيد ملفات LLVM

## 🎯 الهدف

توحيد جميع ملفات LLVM المتفرقة في مكان واحد لإزالة التكرار وتسهيل الصيانة.

---

## 📍 الوضع الحالي

### المواقع المتعددة لملفات LLVM:

```
compiler/
├── backend/llvm/              # موقع 1
│   ├── include/
│   └── src/
│
├── backends/llvm/             # موقع 2 (الأكثر اكتمالاً)
│   ├── arabic_optimizer.cpp/h
│   ├── llvm_codegen.cpp/h
│   ├── llvm_array_support.cpp/h
│   ├── llvm_class_support.cpp/h
│   ├── llvm_control_flow.cpp/h
│   ├── llvm_debug.cpp/h
│   ├── llvm_dict_support.cpp/h
│   ├── llvm_expression_builder.cpp/h
│   ├── llvm_linker.cpp/h
│   ├── llvm_memory_manager.cpp/h
│   ├── llvm_optimizer.cpp/h
│   ├── llvm_runtime.cpp/h
│   ├── llvm_runtime_io_gc.cpp
│   ├── llvm_target.cpp/h
│   ├── llvm_type_mapper.cpp/h
│   └── toolchain_detection.cpp/h
│
└── pipeline/llvm/             # موقع 3
    ├── include/
    │   └── llvm_generator.h
    └── src/
        └── llvm_generator.cpp
```

---

## 📁 الهيكل الجديد المقترح

```
src/compiler/llvm/
├── include/                    # جميع ملفات الرأس
│   ├── llvm_codegen.h
│   ├── llvm_generator.h
│   ├── llvm_optimizer.h
│   ├── llvm_type_mapper.h
│   ├── llvm_array_support.h
│   ├── llvm_class_support.h
│   ├── llvm_dict_support.h
│   ├── llvm_control_flow.h
│   ├── llvm_expression_builder.h
│   ├── llvm_memory_manager.h
│   ├── llvm_runtime.h
│   ├── llvm_linker.h
│   ├── llvm_target.h
│   ├── llvm_debug.h
│   ├── arabic_optimizer.h
│   └── toolchain_detection.h
│
└── src/                        # جميع ملفات المصدر
    ├── llvm_codegen.cpp
    ├── llvm_generator.cpp
    ├── llvm_optimizer.cpp
    ├── llvm_type_mapper.cpp
    ├── llvm_array_support.cpp
    ├── llvm_class_support.cpp
    ├── llvm_dict_support.cpp
    ├── llvm_control_flow.cpp
    ├── llvm_expression_builder.cpp
    ├── llvm_memory_manager.cpp
    ├── llvm_runtime.cpp
    ├── llvm_runtime_io_gc.cpp
    ├── llvm_linker.cpp
    ├── llvm_target.cpp
    ├── llvm_debug.cpp
    ├── arabic_optimizer.cpp
    └── toolchain_detection.cpp
```

---

## 📋 خطوات التنفيذ

### الخطوة 1: إنشاء الهيكل الجديد

```powershell
# إنشاء المجلدات الجديدة
New-Item -ItemType Directory -Path "C:\s\s_language\src\compiler\llvm\include" -Force
New-Item -ItemType Directory -Path "C:\s\s_language\src\compiler\llvm\src" -Force
```

### الخطوة 2: نقل الملفات من backends/llvm

```powershell
# نقل ملفات الرأس (.h)
$sourceDir = "C:\s\s_language\compiler\backends\llvm"
$destInclude = "C:\s\s_language\src\compiler\llvm\include"
$destSrc = "C:\s\s_language\src\compiler\llvm\src"

Get-ChildItem "$sourceDir\*.h" | ForEach-Object {
    Copy-Item $_.FullName -Destination $destInclude
}

# نقل ملفات المصدر (.cpp)
Get-ChildItem "$sourceDir\*.cpp" | ForEach-Object {
    Copy-Item $_.FullName -Destination $destSrc
}
```

### الخطوة 3: نقل الملفات من pipeline/llvm

```powershell
# نقل llvm_generator
Copy-Item "C:\s\s_language\compiler\pipeline\llvm\include\*.h" -Destination $destInclude
Copy-Item "C:\s\s_language\compiler\pipeline\llvm\src\*.cpp" -Destination $destSrc
```

### الخطوة 4: التحقق من backend/llvm

```powershell
# فحص محتويات backend/llvm لأي ملفات فريدة
Get-ChildItem -Recurse "C:\s\s_language\compiler\backend\llvm"
```

---

## 🔧 تحديث مسارات Include

### الملفات التي تحتاج تحديث:

بعد النقل، يجب تحديث مسارات `#include` في:

1. **ملفات LLVM نفسها** - تحديث المسارات النسبية
2. **compiler/jit/** - يستخدم LLVM
3. **compiler/frontend/** - يستخدم LLVM generator
4. **CMakeLists.txt** - تحديث مسارات الملفات المصدرية

### مثال على التحديث:

```cpp
// قبل
#include "llvm_codegen.h"
#include "../backends/llvm/llvm_optimizer.h"

// بعد
#include "compiler/llvm/include/llvm_codegen.h"
#include "compiler/llvm/include/llvm_optimizer.h"
```

---

## 📝 تحديث CMakeLists.txt

```cmake
# ملفات LLVM الموحدة
set(LLVM_SOURCES
    src/compiler/llvm/src/llvm_codegen.cpp
    src/compiler/llvm/src/llvm_generator.cpp
    src/compiler/llvm/src/llvm_optimizer.cpp
    src/compiler/llvm/src/llvm_type_mapper.cpp
    src/compiler/llvm/src/llvm_array_support.cpp
    src/compiler/llvm/src/llvm_class_support.cpp
    src/compiler/llvm/src/llvm_dict_support.cpp
    src/compiler/llvm/src/llvm_control_flow.cpp
    src/compiler/llvm/src/llvm_expression_builder.cpp
    src/compiler/llvm/src/llvm_memory_manager.cpp
    src/compiler/llvm/src/llvm_runtime.cpp
    src/compiler/llvm/src/llvm_runtime_io_gc.cpp
    src/compiler/llvm/src/llvm_linker.cpp
    src/compiler/llvm/src/llvm_target.cpp
    src/compiler/llvm/src/llvm_debug.cpp
    src/compiler/llvm/src/arabic_optimizer.cpp
    src/compiler/llvm/src/toolchain_detection.cpp
)

# إضافة مسار include
include_directories(${CMAKE_SOURCE_DIR}/src/compiler/llvm/include)
```

---

## 🗑️ حذف المجلدات القديمة

بعد التأكد من نجاح البناء والاختبارات:

```powershell
# حذف المجلدات القديمة
Remove-Item -Recurse -Force "C:\s\s_language\compiler\backend\llvm"
Remove-Item -Recurse -Force "C:\s\s_language\compiler\backends\llvm"
Remove-Item -Recurse -Force "C:\s\s_language\compiler\pipeline\llvm"
```

---

## ✅ قائمة التحقق

- [ ] إنشاء الهيكل الجديد
- [ ] نقل ملفات backends/llvm
- [ ] نقل ملفات pipeline/llvm
- [ ] التحقق من backend/llvm
- [ ] تحديث مسارات #include في ملفات LLVM
- [ ] تحديث مسارات #include في الملفات الأخرى
- [ ] تحديث CMakeLists.txt
- [ ] بناء المشروع
- [ ] تشغيل الاختبارات
- [ ] حذف المجلدات القديمة
- [ ] commit التغييرات

---

## ⚠️ ملاحظات

1. **التبعيات**: ملفات JIT تعتمد على LLVM - تأكد من تحديث مساراتها
2. **CMake**: قد تحتاج لإعادة تكوين CMake بعد النقل
3. **Git**: استخدم `git mv` للحفاظ على تاريخ الملفات

---

## 🔗 المرحلة التالية

[المرحلة 2: إعادة تنظيم compiler](02_phase2_compiler_reorganization.md)
