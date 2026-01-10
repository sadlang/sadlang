# المرحلة 1: إنشاء مجلد shared/ ونقل الملفات المشتركة

## 🎯 الهدف

نقل جميع الملفات المشتركة بين المفسر والمترجم إلى مجلد `shared/` موحد.

---

## 📍 الملفات المشتركة

### 1. المحلل المعجمي (Lexer)

**المصدر الحالي:**
- `include/lexer/lexer_core.h`
- `include/lexer/lexer_keywords.h`
- `include/lexer/token.h`
- `src/lexer/lexer_core.cpp`
- `src/lexer/lexer_keywords.cpp`
- `src/lexer/token.cpp`

**الهدف الجديد:**
```
shared/lexer/
├── include/
│   ├── lexer_core.h
│   ├── lexer_keywords.h
│   └── token.h
└── src/
    ├── lexer_core.cpp
    ├── lexer_keywords.cpp
    └── token.cpp
```

### 2. المحلل النحوي (Parser)

**المصدر الحالي:**
- `include/parser/parser_core.h`
- `include/parser/parser_classes.h`
- `src/parser/parser_core.cpp`
- `src/parser/parser_core_helpers.cpp`
- `src/parser/parser_core_impl.cpp`
- `src/parser/parser_core_oop.cpp`
- `src/parser/parser_modules.cpp`

**الهدف الجديد:**
```
shared/parser/
├── include/
│   ├── parser_core.h
│   └── parser_classes.h
└── src/
    ├── parser_core.cpp
    ├── parser_core_helpers.cpp
    ├── parser_core_impl.cpp
    ├── parser_core_oop.cpp
    └── parser_modules.cpp
```

### 3. شجرة البناء المجردة (AST)

**المصدر الحالي:**
- `include/parser/ast/*.h` (15 ملف)
- `src/parser/ast/*.cpp` (8 ملفات)

**الهدف الجديد:**
```
shared/ast/
├── include/
│   ├── ast_node.h
│   ├── ast_visitor.h
│   ├── declarations.h
│   ├── expressions.h
│   ├── statements.h
│   ├── class_nodes.h
│   ├── module_nodes.h
│   ├── pattern_nodes.h
│   ├── property_nodes.h
│   ├── type_nodes.h
│   ├── comprehension_nodes.h
│   └── advanced_expr_nodes.h
└── src/
    ├── ast_node.cpp
    ├── declarations.cpp
    ├── expressions.cpp
    ├── statements.cpp
    ├── module_nodes.cpp
    ├── property_nodes.cpp
    └── type_nodes.cpp
```

### 4. نظام الأخطاء (Errors)

**المصدر الحالي:**
- `include/errors/*.h` (4 ملفات)
- `src/errors/*.cpp` (3 ملفات)

**الهدف الجديد:**
```
shared/errors/
├── include/
│   ├── diagnostic.h
│   ├── error_codes.h
│   ├── error_manager.h
│   └── source_location.h
└── src/
    ├── diagnostic.cpp
    ├── error_codes.cpp
    └── error_manager.cpp
```

### 5. نظام الوحدات (Modules)

**المصدر الحالي:**
- `include/modules/*.h` (8 ملفات)
- `src/modules/*.cpp` (8 ملفات)

**الهدف الجديد:**
```
shared/modules/
├── include/
│   ├── module.h
│   ├── module_cache.h
│   ├── module_loader.h
│   ├── module_resolver.h
│   ├── module_validator.h
│   ├── dependency_graph.h
│   ├── search_paths.h
│   └── symbol_resolver.h
└── src/
    ├── module.cpp
    ├── module_cache.cpp
    ├── module_loader.cpp
    ├── module_resolver.cpp
    ├── module_validator.cpp
    ├── dependency_graph.cpp
    ├── search_paths.cpp
    └── symbol_resolver.cpp
```

### 6. أنواع البيانات الأساسية (Types)

**المصدر الحالي:**
- `include/data/types/*.h` (6 ملفات)
- `src/data/types/*.cpp` (4 ملفات)

**الهدف الجديد:**
```
shared/types/
├── include/
│   ├── type.h
│   ├── value.h
│   ├── data_types.h
│   ├── class_type.h
│   ├── object_instance.h
│   └── generator.h
└── src/
    ├── value.cpp
    ├── class_type.cpp
    ├── object_instance.cpp
    └── generator.cpp
```

### 7. الأدوات المساعدة (Utils)

**المصدر الحالي:**
- `include/utils/string_utils.h`
- `src/utils/string_utils.cpp`

**الهدف الجديد:**
```
shared/utils/
├── include/
│   └── string_utils.h
└── src/
    └── string_utils.cpp
```

---

## 📋 خطوات التنفيذ

### الخطوة 1.1: إنشاء الهيكل

```powershell
$base = "C:\s\s_language\shared"

# Lexer
New-Item -ItemType Directory -Path "$base\lexer\include" -Force
New-Item -ItemType Directory -Path "$base\lexer\src" -Force

# Parser
New-Item -ItemType Directory -Path "$base\parser\include" -Force
New-Item -ItemType Directory -Path "$base\parser\src" -Force

# AST
New-Item -ItemType Directory -Path "$base\ast\include" -Force
New-Item -ItemType Directory -Path "$base\ast\src" -Force

# Errors
New-Item -ItemType Directory -Path "$base\errors\include" -Force
New-Item -ItemType Directory -Path "$base\errors\src" -Force

# Modules
New-Item -ItemType Directory -Path "$base\modules\include" -Force
New-Item -ItemType Directory -Path "$base\modules\src" -Force

# Types
New-Item -ItemType Directory -Path "$base\types\include" -Force
New-Item -ItemType Directory -Path "$base\types\src" -Force

# Utils
New-Item -ItemType Directory -Path "$base\utils\include" -Force
New-Item -ItemType Directory -Path "$base\utils\src" -Force
```

### الخطوة 1.2: نسخ ملفات Lexer

```powershell
# Headers
Copy-Item "include\lexer\*.h" -Destination "shared\lexer\include\"

# Sources
Copy-Item "src\lexer\*.cpp" -Destination "shared\lexer\src\"
```

### الخطوة 1.3: نسخ ملفات Parser

```powershell
# Headers
Copy-Item "include\parser\parser_core.h" -Destination "shared\parser\include\"
Copy-Item "include\parser\parser_classes.h" -Destination "shared\parser\include\"

# Sources
Copy-Item "src\parser\parser_core.cpp" -Destination "shared\parser\src\"
Copy-Item "src\parser\parser_core_helpers.cpp" -Destination "shared\parser\src\"
Copy-Item "src\parser\parser_core_impl.cpp" -Destination "shared\parser\src\"
Copy-Item "src\parser\parser_core_oop.cpp" -Destination "shared\parser\src\"
Copy-Item "src\parser\parser_modules.cpp" -Destination "shared\parser\src\"
```

### الخطوة 1.4: نسخ ملفات AST

```powershell
# Headers
Copy-Item "include\parser\ast\*.h" -Destination "shared\ast\include\"

# Sources
Copy-Item "src\parser\ast\*.cpp" -Destination "shared\ast\src\"
```

### الخطوة 1.5: نسخ ملفات Errors

```powershell
# Headers
Copy-Item "include\errors\*.h" -Destination "shared\errors\include\"

# Sources
Copy-Item "src\errors\*.cpp" -Destination "shared\errors\src\"
```

### الخطوة 1.6: نسخ ملفات Modules

```powershell
# Headers
Copy-Item "include\modules\*.h" -Destination "shared\modules\include\"

# Sources
Copy-Item "src\modules\*.cpp" -Destination "shared\modules\src\"
```

### الخطوة 1.7: نسخ ملفات Types

```powershell
# Headers
Copy-Item "include\data\types\*.h" -Destination "shared\types\include\"

# Sources
Copy-Item "src\data\types\*.cpp" -Destination "shared\types\src\"
```

### الخطوة 1.8: نسخ ملفات Utils

```powershell
# Headers
Copy-Item "include\utils\*.h" -Destination "shared\utils\include\"

# Sources
Copy-Item "src\utils\*.cpp" -Destination "shared\utils\src\"
```

---

## 🔧 إنشاء CMakeLists.txt

### shared/CMakeLists.txt

```cmake
# ============================================================================
# Shared Components - المكونات المشتركة
# ============================================================================

cmake_minimum_required(VERSION 3.15)

add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(ast)
add_subdirectory(errors)
add_subdirectory(modules)
add_subdirectory(types)
add_subdirectory(utils)

# مكتبة مجمعة للمكونات المشتركة
add_library(sad_shared INTERFACE)
target_link_libraries(sad_shared INTERFACE
    sad_lexer
    sad_parser
    sad_ast
    sad_errors
    sad_modules
    sad_types
    sad_utils
)
```

### shared/lexer/CMakeLists.txt

```cmake
add_library(sad_lexer STATIC
    src/lexer_core.cpp
    src/lexer_keywords.cpp
    src/token.cpp
)

target_include_directories(sad_lexer
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
```

---

## 📊 ملخص الملفات

| المكون | Headers | Sources | المجموع |
|--------|---------|---------|---------|
| lexer | 3 | 3 | 6 |
| parser | 2 | 5 | 7 |
| ast | 12+ | 8 | 20+ |
| errors | 4 | 3 | 7 |
| modules | 8 | 8 | 16 |
| types | 6 | 4 | 10 |
| utils | 1 | 1 | 2 |
| **المجموع** | **36+** | **32** | **68+** |

---

## ✅ قائمة التحقق

- [ ] إنشاء هيكل المجلدات
- [ ] نسخ ملفات Lexer
- [ ] نسخ ملفات Parser
- [ ] نسخ ملفات AST
- [ ] نسخ ملفات Errors
- [ ] نسخ ملفات Modules
- [ ] نسخ ملفات Types
- [ ] نسخ ملفات Utils
- [ ] إنشاء ملفات CMakeLists.txt
- [ ] البناء والتحقق

---

## ⏭️ الخطوة التالية

المرحلة 2: إعادة تنظيم interpreter/
