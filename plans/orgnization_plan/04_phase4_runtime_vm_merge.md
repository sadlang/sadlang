# المرحلة 4: دمج مجلدات runtime و vm و bytecode

## 🎯 الهدف

دمج المجلدات المنفصلة (`runtime/`, `vm/`, `bytecode/`) في هيكل موحد تحت `src/runtime/`.

---

## 📍 الوضع الحالي

### مجلد runtime/:

```
runtime/
├── CMakeLists.txt
├── abi/
├── exception/
│   ├── exception.cpp
│   └── exception.h
├── ffi/
│   ├── ffi.cpp
│   └── ffi.h
├── include/
├── memory/
│   ├── allocator.cpp
│   ├── allocator.h
│   ├── gc.cpp
│   └── gc.h
├── src/
├── syscall/
└── thread/
    ├── future.h
    ├── thread.cpp
    └── thread.h
```

### مجلد vm/:

```
vm/
├── CMakeLists.txt
├── include/
│   ├── value.h
│   ├── vm.h
│   └── vm_gc.h
└── src/
    ├── vm.cpp
    ├── vm_gc.cpp
    ├── vm_helpers.cpp
    └── vm_opcodes.cpp
```

### مجلد bytecode/:

```
bytecode/
├── include/
│   └── bytecode_file.h
└── src/
    └── bytecode_file.cpp
```

---

## 📁 الهيكل الجديد المقترح

```
src/runtime/
├── vm/                          # الآلة الافتراضية
│   ├── vm.h
│   ├── vm.cpp
│   ├── vm_gc.h
│   ├── vm_gc.cpp
│   ├── vm_helpers.cpp
│   ├── vm_opcodes.cpp
│   ├── value.h
│   └── bytecode/
│       ├── bytecode_file.h
│       └── bytecode_file.cpp
│
├── memory/                      # إدارة الذاكرة
│   ├── allocator.h
│   ├── allocator.cpp
│   ├── gc.h                     # Garbage Collector
│   └── gc.cpp
│
├── thread/                      # الخيوط والتزامن
│   ├── thread.h
│   ├── thread.cpp
│   └── future.h
│
├── ffi/                         # واجهة الدوال الخارجية
│   ├── ffi.h
│   └── ffi.cpp
│
├── exception/                   # معالجة الاستثناءات
│   ├── exception.h
│   └── exception.cpp
│
├── syscall/                     # استدعاءات النظام
│   └── ...
│
└── abi/                         # واجهة التطبيقات الثنائية
    └── ...
```

---

## 📋 خطوات التنفيذ

### الخطوة 1: إنشاء الهيكل الجديد

```powershell
$base = "C:\s\s_language\src\runtime"

New-Item -ItemType Directory -Path "$base\vm\bytecode" -Force
New-Item -ItemType Directory -Path "$base\memory" -Force
New-Item -ItemType Directory -Path "$base\thread" -Force
New-Item -ItemType Directory -Path "$base\ffi" -Force
New-Item -ItemType Directory -Path "$base\exception" -Force
New-Item -ItemType Directory -Path "$base\syscall" -Force
New-Item -ItemType Directory -Path "$base\abi" -Force
```

### الخطوة 2: نقل VM

```powershell
$newVm = "C:\s\s_language\src\runtime\vm"
$oldVm = "C:\s\s_language\vm"

# نقل ملفات الرأس
Copy-Item "$oldVm\include\*.h" -Destination $newVm

# نقل ملفات المصدر
Copy-Item "$oldVm\src\*.cpp" -Destination $newVm
```

### الخطوة 3: نقل Bytecode إلى داخل VM

```powershell
$newBytecode = "C:\s\s_language\src\runtime\vm\bytecode"
$oldBytecode = "C:\s\s_language\bytecode"

Copy-Item "$oldBytecode\include\*.h" -Destination $newBytecode
Copy-Item "$oldBytecode\src\*.cpp" -Destination $newBytecode
```

### الخطوة 4: نقل Memory

```powershell
$newMemory = "C:\s\s_language\src\runtime\memory"
$oldMemory = "C:\s\s_language\runtime\memory"

Copy-Item "$oldMemory\*.h" -Destination $newMemory
Copy-Item "$oldMemory\*.cpp" -Destination $newMemory
```

### الخطوة 5: نقل Thread

```powershell
$newThread = "C:\s\s_language\src\runtime\thread"
$oldThread = "C:\s\s_language\runtime\thread"

Copy-Item "$oldThread\*.h" -Destination $newThread
Copy-Item "$oldThread\*.cpp" -Destination $newThread
```

### الخطوة 6: نقل FFI

```powershell
$newFfi = "C:\s\s_language\src\runtime\ffi"
$oldFfi = "C:\s\s_language\runtime\ffi"

Copy-Item "$oldFfi\*.h" -Destination $newFfi
Copy-Item "$oldFfi\*.cpp" -Destination $newFfi
```

### الخطوة 7: نقل Exception

```powershell
$newException = "C:\s\s_language\src\runtime\exception"
$oldException = "C:\s\s_language\runtime\exception"

Copy-Item "$oldException\*.h" -Destination $newException
Copy-Item "$oldException\*.cpp" -Destination $newException
```

### الخطوة 8: نقل Syscall (إذا وجد محتوى)

```powershell
$newSyscall = "C:\s\s_language\src\runtime\syscall"
$oldSyscall = "C:\s\s_language\runtime\syscall"

if (Test-Path "$oldSyscall\*") {
    Copy-Item "$oldSyscall\*" -Destination $newSyscall -Recurse
}
```

### الخطوة 9: نقل ABI (إذا وجد محتوى)

```powershell
$newAbi = "C:\s\s_language\src\runtime\abi"
$oldAbi = "C:\s\s_language\runtime\abi"

if (Test-Path "$oldAbi\*") {
    Copy-Item "$oldAbi\*" -Destination $newAbi -Recurse
}
```

---

## 🔧 تحديث مسارات Include

### أمثلة على التحديثات:

```cpp
// قبل
#include "vm/include/vm.h"
#include "vm/include/value.h"
#include "bytecode/include/bytecode_file.h"
#include "runtime/memory/gc.h"

// بعد
#include "runtime/vm/vm.h"
#include "runtime/vm/value.h"
#include "runtime/vm/bytecode/bytecode_file.h"
#include "runtime/memory/gc.h"
```

---

## 📝 تحديث CMakeLists.txt

```cmake
# Runtime sources
set(RUNTIME_SOURCES
    # VM
    src/runtime/vm/vm.cpp
    src/runtime/vm/vm_gc.cpp
    src/runtime/vm/vm_helpers.cpp
    src/runtime/vm/vm_opcodes.cpp
    src/runtime/vm/bytecode/bytecode_file.cpp
    
    # Memory
    src/runtime/memory/allocator.cpp
    src/runtime/memory/gc.cpp
    
    # Thread
    src/runtime/thread/thread.cpp
    
    # FFI
    src/runtime/ffi/ffi.cpp
    
    # Exception
    src/runtime/exception/exception.cpp
)

# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/src/runtime)
```

---

## ✅ قائمة التحقق

- [ ] إنشاء الهيكل الجديد
- [ ] نقل VM
- [ ] نقل Bytecode
- [ ] نقل Memory
- [ ] نقل Thread
- [ ] نقل FFI
- [ ] نقل Exception
- [ ] نقل Syscall
- [ ] نقل ABI
- [ ] تحديث مسارات #include
- [ ] تحديث CMakeLists.txt
- [ ] بناء المشروع
- [ ] تشغيل الاختبارات
- [ ] حذف المجلدات القديمة

---

## 🗑️ المجلدات المطلوب حذفها بعد النجاح

```powershell
# حذف المجلدات القديمة
Remove-Item -Recurse -Force "C:\s\s_language\runtime"
Remove-Item -Recurse -Force "C:\s\s_language\vm"
Remove-Item -Recurse -Force "C:\s\s_language\bytecode"
```

---

## ⚠️ ملاحظات

1. **JIT**: تأكد من تحديث مسارات JIT التي تعتمد على VM
2. **CMakeLists.txt**: قد يكون هناك CMakeLists.txt منفصل في كل مجلد
3. **التبعيات**: VM يعتمد على Memory وBytecode

---

## 🔗 المرحلة التالية

[المرحلة 5: تنظيف stdlib](05_phase5_stdlib_cleanup.md)
