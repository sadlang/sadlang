# المرحلة 2: إعادة تنظيم مجلد compiler

## 🎯 الهدف

توحيد وتنظيم مجلد compiler لإزالة التكرار وتحسين الهيكل.

---

## 📍 الوضع الحالي

```
compiler/
├── backend/                    # شبه فارغ (بعد المرحلة 1)
├── backends/                   # شبه فارغ (بعد المرحلة 1)
├── bytecode/                   # توليد bytecode
│   ├── include/
│   │   └── bytecode_file.h
│   └── src/
│       └── bytecode_file.cpp
├── frontend/                   # الواجهة الأمامية
│   ├── include/
│   │   ├── sir_builder.h
│   │   ├── sir_instruction.h
│   │   ├── sir_module.h
│   │   ├── sir_types.h
│   │   └── type_info.h
│   ├── src/
│   │   └── *.cpp
│   └── type_checker/
│       ├── include/
│       ├── src/
│       └── tests/
├── include/                    # ملفات رأس إضافية
├── jit/                        # محرك JIT
│   ├── include/
│   ├── src/
│   └── tests/
├── optimizer/                  # المحسّن
│   ├── include/
│   ├── src/
│   └── README.md
├── pipeline/                   # خط الأنابيب
│   ├── include/
│   ├── llvm/                   # (سيُحذف بعد المرحلة 1)
│   └── src/
├── src/                        # ملفات مصدر إضافية
└── type_system/                # نظام الأنواع
    ├── include/
    ├── src/
    └── tests/
```

### المشاكل:

1. **تكرار include/src**: كل مكون له مجلدات خاصة
2. **مجلدات فارغة**: backend, backends (بعد المرحلة 1)
3. **تشتت**: ملفات type_checker داخل frontend
4. **عدم وضوح**: الفرق بين optimizer في compiler و في compiler/optimizer

---

## 📁 الهيكل الجديد المقترح

```
src/compiler/
├── frontend/                   # الواجهة الأمامية
│   ├── sir/                    # التمثيل الوسيط
│   │   ├── sir_builder.h/cpp
│   │   ├── sir_instruction.h/cpp
│   │   ├── sir_module.h/cpp
│   │   └── sir_types.h/cpp
│   ├── type_checker/           # فحص الأنواع
│   │   ├── type_checker.h/cpp
│   │   └── type_info.h/cpp
│   └── semantic/               # التحليل الدلالي (إذا وجد)
│
├── middle/                     # المعالجة الوسطى
│   ├── optimizer/              # المحسّن
│   │   ├── passes/             # تمريرات التحسين
│   │   │   ├── constant_folding.h/cpp
│   │   │   ├── dead_code_elimination.h/cpp
│   │   │   ├── copy_propagation.h/cpp
│   │   │   ├── cse.h/cpp
│   │   │   └── register_coalescing.h/cpp
│   │   ├── optimizer.h/cpp
│   │   └── pass_manager.h/cpp
│   └── type_system/            # نظام الأنواع المتقدم
│       ├── types/
│       │   ├── primitive_type.h/cpp
│       │   ├── generic_type.h/cpp
│       │   ├── union_type.h/cpp
│       │   └── optional_type.h/cpp
│       ├── inference/
│       │   ├── type_inferencer.h/cpp
│       │   └── type_narrowing.h/cpp
│       ├── constraints/
│       │   ├── constraint.h/cpp
│       │   └── constraint_solver.h/cpp
│       └── type_registry.h/cpp
│
├── backend/                    # الواجهة الخلفية
│   ├── llvm/                   # (من المرحلة 1)
│   │   ├── codegen/
│   │   ├── runtime/
│   │   └── optimizer/
│   └── bytecode/               # توليد bytecode
│       └── bytecode_file.h/cpp
│
└── jit/                        # التجميع الفوري
    ├── engine/
    │   ├── jit_engine.h/cpp
    │   └── jit_cache.h/cpp
    ├── profiler/
    │   ├── jit_profiler.h/cpp
    │   ├── jit_metrics.h/cpp
    │   └── hot_path_detector.h/cpp
    └── bridge/
        └── jit_bridge.h/cpp
```

---

## 📋 خطوات التنفيذ

### الخطوة 1: إنشاء الهيكل الجديد

```powershell
# إنشاء المجلدات
$base = "C:\s\s_language\src\compiler"

# Frontend
New-Item -ItemType Directory -Path "$base\frontend\sir" -Force
New-Item -ItemType Directory -Path "$base\frontend\type_checker" -Force

# Middle
New-Item -ItemType Directory -Path "$base\middle\optimizer\passes" -Force
New-Item -ItemType Directory -Path "$base\middle\type_system\types" -Force
New-Item -ItemType Directory -Path "$base\middle\type_system\inference" -Force
New-Item -ItemType Directory -Path "$base\middle\type_system\constraints" -Force

# Backend
New-Item -ItemType Directory -Path "$base\backend\bytecode" -Force

# JIT
New-Item -ItemType Directory -Path "$base\jit\engine" -Force
New-Item -ItemType Directory -Path "$base\jit\profiler" -Force
New-Item -ItemType Directory -Path "$base\jit\bridge" -Force
```

### الخطوة 2: نقل ملفات Frontend

```powershell
# نقل SIR
$oldFrontend = "C:\s\s_language\compiler\frontend"
$newSir = "C:\s\s_language\src\compiler\frontend\sir"

Copy-Item "$oldFrontend\include\sir_*.h" -Destination $newSir
Copy-Item "$oldFrontend\src\sir_*.cpp" -Destination $newSir

# نقل type_info
Copy-Item "$oldFrontend\include\type_info.h" -Destination "$base\frontend\type_checker"
Copy-Item "$oldFrontend\src\type_info.cpp" -Destination "$base\frontend\type_checker"
```

### الخطوة 3: نقل Type Checker

```powershell
$oldTypeChecker = "C:\s\s_language\compiler\frontend\type_checker"
$newTypeChecker = "C:\s\s_language\src\compiler\frontend\type_checker"

Copy-Item "$oldTypeChecker\include\*.h" -Destination $newTypeChecker
Copy-Item "$oldTypeChecker\src\*.cpp" -Destination $newTypeChecker
```

### الخطوة 4: نقل Optimizer

```powershell
$oldOptimizer = "C:\s\s_language\compiler\optimizer"
$newOptimizer = "C:\s\s_language\src\compiler\middle\optimizer"
$newPasses = "$newOptimizer\passes"

# نقل الملفات الرئيسية
Copy-Item "$oldOptimizer\include\optimizer.h" -Destination $newOptimizer
Copy-Item "$oldOptimizer\include\pass_manager.h" -Destination $newOptimizer
Copy-Item "$oldOptimizer\include\pass.h" -Destination $newOptimizer
Copy-Item "$oldOptimizer\src\optimizer.cpp" -Destination $newOptimizer
Copy-Item "$oldOptimizer\src\pass_manager.cpp" -Destination $newOptimizer
Copy-Item "$oldOptimizer\src\pass.cpp" -Destination $newOptimizer

# نقل التمريرات
Copy-Item "$oldOptimizer\include\*_pass.h" -Destination $newPasses
Copy-Item "$oldOptimizer\src\*_pass.cpp" -Destination $newPasses
```

### الخطوة 5: نقل Type System

```powershell
$oldTypeSystem = "C:\s\s_language\compiler\type_system"
$newTypeSystem = "C:\s\s_language\src\compiler\middle\type_system"

# نقل أنواع البيانات
$typeFiles = @("primitive_type", "generic_type", "union_type", "optional_type", "type")
foreach ($file in $typeFiles) {
    Copy-Item "$oldTypeSystem\include\$file.h" -Destination "$newTypeSystem\types" -ErrorAction SilentlyContinue
    Copy-Item "$oldTypeSystem\src\$file.cpp" -Destination "$newTypeSystem\types" -ErrorAction SilentlyContinue
}

# نقل الاستدلال
$inferFiles = @("type_inferencer", "type_narrowing", "type_guard")
foreach ($file in $inferFiles) {
    Copy-Item "$oldTypeSystem\include\$file.h" -Destination "$newTypeSystem\inference" -ErrorAction SilentlyContinue
    Copy-Item "$oldTypeSystem\src\$file.cpp" -Destination "$newTypeSystem\inference" -ErrorAction SilentlyContinue
}

# نقل القيود
$constraintFiles = @("constraint", "constraint_solver", "type_constraint")
foreach ($file in $constraintFiles) {
    Copy-Item "$oldTypeSystem\include\$file.h" -Destination "$newTypeSystem\constraints" -ErrorAction SilentlyContinue
    Copy-Item "$oldTypeSystem\src\$file.cpp" -Destination "$newTypeSystem\constraints" -ErrorAction SilentlyContinue
}

# نقل الملفات الأخرى
Copy-Item "$oldTypeSystem\include\type_registry.h" -Destination $newTypeSystem
Copy-Item "$oldTypeSystem\src\type_registry.cpp" -Destination $newTypeSystem
```

### الخطوة 6: نقل Bytecode

```powershell
$oldBytecode = "C:\s\s_language\compiler\bytecode"
$newBytecode = "C:\s\s_language\src\compiler\backend\bytecode"

Copy-Item "$oldBytecode\include\*.h" -Destination $newBytecode
Copy-Item "$oldBytecode\src\*.cpp" -Destination $newBytecode
```

### الخطوة 7: نقل JIT

```powershell
$oldJit = "C:\s\s_language\compiler\jit"

# Engine
Copy-Item "$oldJit\include\jit_engine.h" -Destination "$base\jit\engine"
Copy-Item "$oldJit\include\jit_cache.h" -Destination "$base\jit\engine"
Copy-Item "$oldJit\src\jit_engine.cpp" -Destination "$base\jit\engine"
Copy-Item "$oldJit\src\jit_cache.cpp" -Destination "$base\jit\engine"

# Profiler
Copy-Item "$oldJit\include\jit_profiler.h" -Destination "$base\jit\profiler"
Copy-Item "$oldJit\include\jit_metrics.h" -Destination "$base\jit\profiler"
Copy-Item "$oldJit\include\hot_path_detector.h" -Destination "$base\jit\profiler"
Copy-Item "$oldJit\src\jit_profiler.cpp" -Destination "$base\jit\profiler"
Copy-Item "$oldJit\src\jit_metrics.cpp" -Destination "$base\jit\profiler"
Copy-Item "$oldJit\src\hot_path_detector.cpp" -Destination "$base\jit\profiler"

# Bridge
Copy-Item "$oldJit\include\jit_bridge.h" -Destination "$base\jit\bridge"
Copy-Item "$oldJit\src\jit_bridge.cpp" -Destination "$base\jit\bridge"
```

---

## 🔧 تحديث المسارات

### مثال على التحديثات المطلوبة:

```cpp
// قبل
#include "compiler/frontend/include/sir_builder.h"
#include "compiler/optimizer/include/optimizer.h"
#include "compiler/type_system/include/type_inferencer.h"

// بعد
#include "compiler/frontend/sir/sir_builder.h"
#include "compiler/middle/optimizer/optimizer.h"
#include "compiler/middle/type_system/inference/type_inferencer.h"
```

---

## ✅ قائمة التحقق

- [ ] إنشاء الهيكل الجديد
- [ ] نقل ملفات Frontend (SIR)
- [ ] نقل Type Checker
- [ ] نقل Optimizer
- [ ] نقل Type System
- [ ] نقل Bytecode
- [ ] نقل JIT
- [ ] تحديث مسارات #include
- [ ] تحديث CMakeLists.txt
- [ ] بناء المشروع
- [ ] تشغيل الاختبارات
- [ ] حذف المجلدات القديمة في compiler/

---

## 🗑️ المجلدات المطلوب حذفها بعد النجاح

```powershell
Remove-Item -Recurse -Force "C:\s\s_language\compiler\backend"
Remove-Item -Recurse -Force "C:\s\s_language\compiler\backends"
Remove-Item -Recurse -Force "C:\s\s_language\compiler\bytecode"
Remove-Item -Recurse -Force "C:\s\s_language\compiler\frontend"
Remove-Item -Recurse -Force "C:\s\s_language\compiler\include"
Remove-Item -Recurse -Force "C:\s\s_language\compiler\jit"
Remove-Item -Recurse -Force "C:\s\s_language\compiler\optimizer"
Remove-Item -Recurse -Force "C:\s\s_language\compiler\pipeline"
Remove-Item -Recurse -Force "C:\s\s_language\compiler\src"
Remove-Item -Recurse -Force "C:\s\s_language\compiler\type_system"
```

---

## 🔗 المرحلة التالية

[المرحلة 3: توحيد include و src](03_phase3_include_src_unification.md)
