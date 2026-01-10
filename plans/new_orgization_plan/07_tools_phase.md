# المرحلة 7: مجلد الأدوات (Tools)

## 🎯 الهدف

تنظيم أدوات المشروع في مجلد `tools/` منفصل.

---

## 📁 الهيكل المقترح

```
tools/
├── repl/                       # واجهة سطر الأوامر التفاعلية
│   ├── include/
│   │   └── repl.h
│   ├── src/
│   │   └── repl.cpp
│   └── CMakeLists.txt
│
├── lsp/                        # خادم LSP
│   ├── include/
│   │   ├── lsp_server.h
│   │   ├── lsp_handler.h
│   │   └── lsp_protocol.h
│   ├── src/
│   │   ├── lsp_server.cpp
│   │   ├── lsp_handler.cpp
│   │   └── lsp_protocol.cpp
│   └── CMakeLists.txt
│
├── pkg/                        # مدير الحزم
│   ├── include/
│   │   ├── package_manager.h
│   │   └── package_resolver.h
│   ├── src/
│   │   ├── package_manager.cpp
│   │   └── package_resolver.cpp
│   └── CMakeLists.txt
│
├── docgen/                     # مولد التوثيق
│   ├── include/
│   │   └── doc_generator.h
│   ├── src/
│   │   └── doc_generator.cpp
│   └── CMakeLists.txt
│
├── formatter/                  # منسق الكود
│   ├── include/
│   │   └── code_formatter.h
│   ├── src/
│   │   └── code_formatter.cpp
│   └── CMakeLists.txt
│
├── linter/                     # محلل الكود
│   ├── include/
│   │   └── linter.h
│   ├── src/
│   │   └── linter.cpp
│   └── CMakeLists.txt
│
└── CMakeLists.txt
```

---

## 🔧 CMakeLists.txt للأدوات

### tools/CMakeLists.txt

```cmake
# ============================================================================
# Tools - الأدوات
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# REPL
add_subdirectory(repl)

# LSP Server (اختياري)
if(SAD_BUILD_LSP)
    add_subdirectory(lsp)
endif()

# Package Manager (اختياري)
if(SAD_BUILD_PKG)
    add_subdirectory(pkg)
endif()

# Documentation Generator (اختياري)
if(SAD_BUILD_DOCGEN)
    add_subdirectory(docgen)
endif()

# Formatter (اختياري)
if(SAD_BUILD_FORMATTER)
    add_subdirectory(formatter)
endif()

# Linter (اختياري)
if(SAD_BUILD_LINTER)
    add_subdirectory(linter)
endif()
```

### tools/repl/CMakeLists.txt

```cmake
# ============================================================================
# REPL - واجهة سطر الأوامر التفاعلية
# ============================================================================

add_executable(sad_repl
    src/repl.cpp
)

target_include_directories(sad_repl
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/shared/include
        ${CMAKE_SOURCE_DIR}/interpreter/include
)

target_link_libraries(sad_repl
    PRIVATE
        sad_shared
        sad_interpreter
)

# تثبيت
install(TARGETS sad_repl
    RUNTIME DESTINATION bin
)
```

### tools/lsp/CMakeLists.txt

```cmake
# ============================================================================
# LSP Server - خادم بروتوكول خادم اللغة
# ============================================================================

add_executable(sad_lsp
    src/lsp_server.cpp
    src/lsp_handler.cpp
    src/lsp_protocol.cpp
)

target_include_directories(sad_lsp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/shared/include
)

target_link_libraries(sad_lsp
    PRIVATE
        sad_shared
)

# تثبيت
install(TARGETS sad_lsp
    RUNTIME DESTINATION bin
)
```

---

## 📋 خطوات التنفيذ

### الخطوة 7.1: إنشاء الهيكل

```powershell
$base = "C:\s\s_language\tools"

# REPL
New-Item -ItemType Directory -Path "$base\repl\include" -Force
New-Item -ItemType Directory -Path "$base\repl\src" -Force

# LSP
New-Item -ItemType Directory -Path "$base\lsp\include" -Force
New-Item -ItemType Directory -Path "$base\lsp\src" -Force

# Package Manager
New-Item -ItemType Directory -Path "$base\pkg\include" -Force
New-Item -ItemType Directory -Path "$base\pkg\src" -Force

# Doc Generator
New-Item -ItemType Directory -Path "$base\docgen\include" -Force
New-Item -ItemType Directory -Path "$base\docgen\src" -Force

# Formatter
New-Item -ItemType Directory -Path "$base\formatter\include" -Force
New-Item -ItemType Directory -Path "$base\formatter\src" -Force

# Linter
New-Item -ItemType Directory -Path "$base\linter\include" -Force
New-Item -ItemType Directory -Path "$base\linter\src" -Force
```

### الخطوة 7.2: نقل الملفات الموجودة

```powershell
# إذا كانت هناك ملفات REPL موجودة
if (Test-Path "src\repl.cpp") {
    Copy-Item "src\repl.cpp" -Destination "tools\repl\src\"
}

# إذا كانت هناك ملفات LSP موجودة
if (Test-Path "src\lsp\") {
    Copy-Item "src\lsp\*.cpp" -Destination "tools\lsp\src\"
    Copy-Item "include\lsp\*.h" -Destination "tools\lsp\include\"
}
```

---

## 📊 ملخص الأدوات

| الأداة | الوصف | الحالة |
|--------|-------|--------|
| REPL | واجهة تفاعلية | 🟡 قيد التطوير |
| LSP | دعم المحررات | 🔴 مخطط |
| pkg | مدير الحزم | 🔴 مخطط |
| docgen | توليد التوثيق | 🔴 مخطط |
| formatter | تنسيق الكود | 🔴 مخطط |
| linter | تحليل الكود | 🔴 مخطط |

---

## ✅ قائمة التحقق

- [ ] إنشاء هيكل tools/
- [ ] نقل ملفات REPL
- [ ] إنشاء CMakeLists.txt
- [ ] إضافة خيارات البناء في CMake الرئيسي
- [ ] اختبار البناء
