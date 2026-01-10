# المرحلة 4: إعادة تنظيم وقت التشغيل (Runtime)

## 🎯 الهدف

دمج جميع ملفات وقت التشغيل في مجلد `runtime/` واحد منظم يشمل:
- VM (الآلة الافتراضية)
- Memory (إدارة الذاكرة و GC)
- Thread (المعالجة المتوازية)
- FFI (الواجهة الخارجية)
- Exception (معالجة الاستثناءات)

---

## 📍 الملفات الحالية

### من vm/

```
vm/
├── include/
│   ├── vm.h
│   ├── value.h
│   └── vm_gc.h
└── src/
    ├── vm.cpp
    ├── vm_gc.cpp
    ├── vm_helpers.cpp
    └── vm_opcodes.cpp
```

### من runtime/

```
runtime/
├── memory/
│   ├── include/ (gc.h, memory_manager.h)
│   └── src/ (gc.cpp, memory_manager.cpp)
│
├── thread/
│   ├── include/ (thread_pool.h, async_runtime.h)
│   └── src/ (thread_pool.cpp, async_runtime.cpp)
│
├── ffi/
│   ├── include/ (ffi_interface.h, native_bridge.h)
│   └── src/ (ffi_interface.cpp, native_bridge.cpp)
│
├── exception/
│   ├── include/ (exception_handler.h)
│   └── src/ (exception_handler.cpp)
│
├── abi/
│   └── ...
│
└── syscall/
    └── ...
```

---

## 📁 الهيكل الجديد المقترح

```
runtime/
├── include/
│   ├── vm/
│   │   ├── vm.h
│   │   ├── value.h
│   │   ├── vm_gc.h
│   │   └── opcodes.h
│   │
│   ├── memory/
│   │   ├── gc.h
│   │   ├── memory_manager.h
│   │   ├── allocator.h
│   │   └── memory_pool.h
│   │
│   ├── thread/
│   │   ├── thread_pool.h
│   │   ├── async_runtime.h
│   │   ├── task_scheduler.h
│   │   └── synchronization.h
│   │
│   ├── ffi/
│   │   ├── ffi_interface.h
│   │   ├── native_bridge.h
│   │   └── type_marshalling.h
│   │
│   └── exception/
│       ├── exception_handler.h
│       └── stack_trace.h
│
├── src/
│   ├── vm/
│   │   ├── vm.cpp
│   │   ├── vm_gc.cpp
│   │   ├── vm_helpers.cpp
│   │   └── vm_opcodes.cpp
│   │
│   ├── memory/
│   │   ├── gc.cpp
│   │   └── memory_manager.cpp
│   │
│   ├── thread/
│   │   ├── thread_pool.cpp
│   │   └── async_runtime.cpp
│   │
│   ├── ffi/
│   │   ├── ffi_interface.cpp
│   │   └── native_bridge.cpp
│   │
│   └── exception/
│       └── exception_handler.cpp
│
└── CMakeLists.txt
```

---

## 📋 خطوات التنفيذ

### الخطوة 4.1: إنشاء الهيكل الجديد

```powershell
$base = "C:\s\s_language\runtime_new"

# Include directories
New-Item -ItemType Directory -Path "$base\include\vm" -Force
New-Item -ItemType Directory -Path "$base\include\memory" -Force
New-Item -ItemType Directory -Path "$base\include\thread" -Force
New-Item -ItemType Directory -Path "$base\include\ffi" -Force
New-Item -ItemType Directory -Path "$base\include\exception" -Force

# Source directories
New-Item -ItemType Directory -Path "$base\src\vm" -Force
New-Item -ItemType Directory -Path "$base\src\memory" -Force
New-Item -ItemType Directory -Path "$base\src\thread" -Force
New-Item -ItemType Directory -Path "$base\src\ffi" -Force
New-Item -ItemType Directory -Path "$base\src\exception" -Force
```

### الخطوة 4.2: نسخ ملفات VM

```powershell
# Headers
Copy-Item "vm\include\*.h" -Destination "runtime_new\include\vm\"

# Sources
Copy-Item "vm\src\*.cpp" -Destination "runtime_new\src\vm\"
```

### الخطوة 4.3: نسخ ملفات Memory

```powershell
# Headers
Copy-Item "runtime\memory\include\*.h" -Destination "runtime_new\include\memory\"

# Sources
Copy-Item "runtime\memory\src\*.cpp" -Destination "runtime_new\src\memory\"
```

### الخطوة 4.4: نسخ ملفات Thread

```powershell
# Headers
Copy-Item "runtime\thread\include\*.h" -Destination "runtime_new\include\thread\"

# Sources
Copy-Item "runtime\thread\src\*.cpp" -Destination "runtime_new\src\thread\"
```

### الخطوة 4.5: نسخ ملفات FFI

```powershell
# Headers
Copy-Item "runtime\ffi\include\*.h" -Destination "runtime_new\include\ffi\"

# Sources
Copy-Item "runtime\ffi\src\*.cpp" -Destination "runtime_new\src\ffi\"
```

### الخطوة 4.6: نسخ ملفات Exception

```powershell
# Headers
Copy-Item "runtime\exception\include\*.h" -Destination "runtime_new\include\exception\"

# Sources
Copy-Item "runtime\exception\src\*.cpp" -Destination "runtime_new\src\exception\"
```

---

## 🔧 إنشاء CMakeLists.txt

### runtime/CMakeLists.txt

```cmake
# ============================================================================
# Runtime - وقت التشغيل
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# VM Library (الآلة الافتراضية)
# ============================================================================
add_library(sad_vm STATIC
    src/vm/vm.cpp
    src/vm/vm_gc.cpp
    src/vm/vm_helpers.cpp
    src/vm/vm_opcodes.cpp
)

target_include_directories(sad_vm
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/shared/include
)

# ============================================================================
# Memory Library (إدارة الذاكرة)
# ============================================================================
add_library(sad_memory STATIC
    src/memory/gc.cpp
    src/memory/memory_manager.cpp
)

target_include_directories(sad_memory
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============================================================================
# Thread Library (المعالجة المتوازية)
# ============================================================================
add_library(sad_thread STATIC
    src/thread/thread_pool.cpp
    src/thread/async_runtime.cpp
)

target_include_directories(sad_thread
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(Threads REQUIRED)
target_link_libraries(sad_thread
    PUBLIC
        Threads::Threads
)

# ============================================================================
# FFI Library (الواجهة الخارجية)
# ============================================================================
add_library(sad_ffi STATIC
    src/ffi/ffi_interface.cpp
    src/ffi/native_bridge.cpp
)

target_include_directories(sad_ffi
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============================================================================
# Exception Library (معالجة الاستثناءات)
# ============================================================================
add_library(sad_exception STATIC
    src/exception/exception_handler.cpp
)

target_include_directories(sad_exception
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ============================================================================
# Unified Runtime Library
# ============================================================================
add_library(sad_runtime INTERFACE)

target_link_libraries(sad_runtime
    INTERFACE
        sad_vm
        sad_memory
        sad_thread
        sad_ffi
        sad_exception
)

target_include_directories(sad_runtime
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

---

## 📊 ملخص الملفات

| المكون | Headers | Sources | المجموع |
|--------|---------|---------|---------|
| vm | 3 | 4 | 7 |
| memory | ~3 | ~2 | ~5 |
| thread | ~3 | ~2 | ~5 |
| ffi | ~2 | ~2 | ~4 |
| exception | ~2 | ~1 | ~3 |
| **المجموع** | **~13** | **~11** | **~24** |

---

## 🔗 التبعيات

```
sad_runtime
├── sad_vm → sad_shared
├── sad_memory (مستقل)
├── sad_thread → std::thread
├── sad_ffi (مستقل)
└── sad_exception (مستقل)
```

---

## ✅ قائمة التحقق

- [ ] إنشاء هيكل المجلدات
- [ ] نسخ ملفات VM
- [ ] نسخ ملفات Memory
- [ ] نسخ ملفات Thread
- [ ] نسخ ملفات FFI
- [ ] نسخ ملفات Exception
- [ ] إنشاء CMakeLists.txt
- [ ] البناء والتحقق

---

## ⏭️ الخطوة التالية

المرحلة 5: تحديث نظام البناء CMake
