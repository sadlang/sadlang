# المرحلة 2: إعادة تنظيم مجلد المفسر (Interpreter)

## 🎯 الهدف

تنظيم ملفات المفسر في مجلد `interpreter/` منفصل يحتوي فقط على الملفات الخاصة بالتفسير.

---

## 📍 الملفات الحالية للمفسر

### من include/interpreter/

```
include/interpreter/
├── ast_printer.h
├── exception.h
├── interpreter_classes.h
├── core/
│   └── interpreter_core.h
└── visitors/
    ├── expression_evaluator.h
    └── statement_executor.h
```

### من src/interpreter/

```
src/interpreter/
├── ast_printer.cpp
├── exception.cpp
├── core/
│   ├── builtin_registry.cpp
│   └── interpreter_core.cpp
├── oop/
│   └── ... (ملفات OOP)
└── visitors/
    ├── expression_evaluator.cpp
    ├── statement_executor.cpp
    └── statement_executor_oop.cpp
```

### من include/data/managers/

```
include/data/managers/
├── class_manager.h
├── function_manager.h
├── object_manager.h
└── variable_manager.h
```

### من src/data/managers/

```
src/data/managers/
├── class_manager.cpp
├── function_manager.cpp
├── object_manager.cpp
└── variable_manager.cpp
```

### من include/data/scope/

```
include/data/scope/
└── scope_manager.h
```

---

## 📁 الهيكل الجديد المقترح

```
interpreter/
├── include/
│   ├── interpreter_core.h
│   ├── interpreter_classes.h
│   ├── exception.h
│   ├── visitors/
│   │   ├── expression_evaluator.h
│   │   └── statement_executor.h
│   ├── managers/
│   │   ├── class_manager.h
│   │   ├── function_manager.h
│   │   ├── object_manager.h
│   │   ├── variable_manager.h
│   │   └── scope_manager.h
│   └── builtins/
│       └── builtin_registry.h
│
├── src/
│   ├── interpreter_core.cpp
│   ├── exception.cpp
│   ├── visitors/
│   │   ├── expression_evaluator.cpp
│   │   ├── statement_executor.cpp
│   │   └── statement_executor_oop.cpp
│   ├── managers/
│   │   ├── class_manager.cpp
│   │   ├── function_manager.cpp
│   │   ├── object_manager.cpp
│   │   └── variable_manager.cpp
│   └── builtins/
│       └── builtin_registry.cpp
│
└── CMakeLists.txt
```

---

## 📋 خطوات التنفيذ

### الخطوة 2.1: إنشاء الهيكل

```powershell
$base = "C:\s\s_language\interpreter"

# Include directories
New-Item -ItemType Directory -Path "$base\include\visitors" -Force
New-Item -ItemType Directory -Path "$base\include\managers" -Force
New-Item -ItemType Directory -Path "$base\include\builtins" -Force

# Source directories
New-Item -ItemType Directory -Path "$base\src\visitors" -Force
New-Item -ItemType Directory -Path "$base\src\managers" -Force
New-Item -ItemType Directory -Path "$base\src\builtins" -Force
```

### الخطوة 2.2: نسخ ملفات النواة

```powershell
# Headers
Copy-Item "include\interpreter\core\interpreter_core.h" -Destination "interpreter\include\"
Copy-Item "include\interpreter\interpreter_classes.h" -Destination "interpreter\include\"
Copy-Item "include\interpreter\exception.h" -Destination "interpreter\include\"

# Sources
Copy-Item "src\interpreter\core\interpreter_core.cpp" -Destination "interpreter\src\"
Copy-Item "src\interpreter\exception.cpp" -Destination "interpreter\src\"
```

### الخطوة 2.3: نسخ ملفات Visitors

```powershell
# Headers
Copy-Item "include\interpreter\visitors\*.h" -Destination "interpreter\include\visitors\"

# Sources
Copy-Item "src\interpreter\visitors\*.cpp" -Destination "interpreter\src\visitors\"
```

### الخطوة 2.4: نسخ ملفات Managers

```powershell
# Headers
Copy-Item "include\data\managers\*.h" -Destination "interpreter\include\managers\"
Copy-Item "include\data\scope\*.h" -Destination "interpreter\include\managers\"

# Sources
Copy-Item "src\data\managers\*.cpp" -Destination "interpreter\src\managers\"
```

### الخطوة 2.5: نسخ ملفات Builtins

```powershell
# Sources
Copy-Item "src\interpreter\core\builtin_registry.cpp" -Destination "interpreter\src\builtins\"
```

---

## 🔧 إنشاء CMakeLists.txt

### interpreter/CMakeLists.txt

```cmake
# ============================================================================
# Interpreter - المفسر
# ============================================================================

cmake_minimum_required(VERSION 3.15)

# ============================================================================
# مكتبة المفسر
# ============================================================================
add_library(sad_interpreter STATIC
    # النواة
    src/interpreter_core.cpp
    src/exception.cpp
    
    # Visitors
    src/visitors/expression_evaluator.cpp
    src/visitors/statement_executor.cpp
    src/visitors/statement_executor_oop.cpp
    
    # Managers
    src/managers/class_manager.cpp
    src/managers/function_manager.cpp
    src/managers/object_manager.cpp
    src/managers/variable_manager.cpp
    
    # Builtins
    src/builtins/builtin_registry.cpp
)

target_include_directories(sad_interpreter
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/shared/lexer/include
        ${CMAKE_SOURCE_DIR}/shared/parser/include
        ${CMAKE_SOURCE_DIR}/shared/ast/include
        ${CMAKE_SOURCE_DIR}/shared/types/include
        ${CMAKE_SOURCE_DIR}/shared/errors/include
        ${CMAKE_SOURCE_DIR}/shared/modules/include
)

target_link_libraries(sad_interpreter
    PUBLIC
        sad_shared
)
```

---

## 📊 ملخص الملفات

| المكون | Headers | Sources | المجموع |
|--------|---------|---------|---------|
| النواة | 3 | 2 | 5 |
| visitors | 2 | 3 | 5 |
| managers | 5 | 4 | 9 |
| builtins | 0 | 1 | 1 |
| **المجموع** | **10** | **10** | **20** |

---

## 🔗 التبعيات

المفسر يعتمد على:
- `sad_shared` (lexer, parser, ast, types, errors, modules)

---

## ✅ قائمة التحقق

- [ ] إنشاء هيكل المجلدات
- [ ] نسخ ملفات النواة
- [ ] نسخ ملفات Visitors
- [ ] نسخ ملفات Managers
- [ ] نسخ ملفات Builtins
- [ ] إنشاء CMakeLists.txt
- [ ] البناء والتحقق

---

## ⏭️ الخطوة التالية

المرحلة 3: إعادة تنظيم compiler/
