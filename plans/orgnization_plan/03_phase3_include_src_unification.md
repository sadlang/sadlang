# المرحلة 3: توحيد مجلدات include و src الرئيسية

## 🎯 الهدف

دمج مجلدات `include/` و `src/` في الجذر إلى هيكل موحد يتبع نمط "الملفات المصدرية بجانب ملفات الرأس".

---

## 📍 الوضع الحالي

### مجلد include/ الحالي:

```
include/
├── data/
│   ├── managers/
│   │   ├── class_manager.h
│   │   ├── function_manager.h
│   │   ├── object_manager.h
│   │   └── variable_manager.h
│   ├── scope/
│   │   └── scope_manager.h
│   └── types/
│       ├── class_type.h
│       ├── data_types.h
│       ├── generator.h
│       ├── object_instance.h
│       ├── type.h
│       └── value.h
├── errors/
│   ├── diagnostic.h
│   ├── error_codes.h
│   ├── error_manager.h
│   └── source_location.h
├── interpreter/
│   ├── ast_printer.h
│   ├── core/
│   ├── exception.h
│   ├── interpreter_classes.h
│   └── visitors/
├── lexer/
│   ├── lexer_core.h
│   ├── lexer_keywords.h
│   └── token.h
├── low_level/
│   ├── bitwise_ops.h
│   ├── interrupts.h
│   ├── io_ports.h
│   ├── memory_map.h
│   └── pointer_type.h
├── modules/
│   ├── dependency_graph.h
│   ├── module.h
│   ├── module_cache.h
│   ├── module_loader.h
│   ├── module_resolver.h
│   ├── module_validator.h
│   ├── search_paths.h
│   └── symbol_resolver.h
├── network/
│   ├── http/
│   ├── http_bindings.h
│   ├── network_bindings.h
│   ├── network_error.h
│   ├── socket_address.h
│   ├── socket_base.h
│   ├── tcp_socket.h
│   ├── udp_socket.h
│   └── websocket/
├── optimizer/
│   ├── advanced_optimizer.h
│   └── optimization_pass.h
├── parser/
│   ├── ast/
│   │   ├── advanced_expr_nodes.h
│   │   ├── ast_node.h
│   │   ├── ast_printer.h
│   │   ├── ast_visitor.h
│   │   ├── class_nodes.h
│   │   ├── comprehension_nodes.h
│   │   ├── declarations.h
│   │   ├── expressions.h
│   │   ├── module_nodes.h
│   │   ├── pattern_nodes.h
│   │   ├── property_nodes.h
│   │   ├── statements.h
│   │   └── type_nodes.h
│   ├── parser_classes.h
│   └── parser_core.h
└── utils/
    └── string_utils.h
```

### مجلد src/ الحالي:

```
src/
├── ast/
├── data/
├── errors/
├── interpreter/
│   ├── ast_printer.cpp
│   ├── core/
│   ├── exception.cpp
│   ├── oop/
│   └── visitors/
├── lexer/
│   ├── lexer_core.cpp
│   ├── lexer_keywords.cpp
│   └── token.cpp
├── low_level/
│   ├── bitwise_ops.cpp
│   ├── interrupts.cpp
│   ├── io_ports.cpp
│   ├── memory_map.cpp
│   └── pointer_type.cpp
├── main.cpp
├── modules/
│   ├── dependency_graph.cpp
│   ├── module.cpp
│   ├── module_cache.cpp
│   ├── module_loader.cpp
│   ├── module_resolver.cpp
│   ├── module_validator.cpp
│   ├── search_paths.cpp
│   └── symbol_resolver.cpp
├── optimizer/
│   ├── advanced_optimizer.cpp
│   └── optimization_pass.cpp
├── parser/
│   ├── ast/
│   │   ├── ast_node.cpp
│   │   ├── ast_printer.cpp
│   │   ├── declarations.cpp
│   │   ├── expressions.cpp
│   │   ├── module_nodes.cpp
│   │   ├── property_nodes.cpp
│   │   ├── statements.cpp
│   │   └── type_nodes.cpp
│   ├── oop/
│   └── parser_core*.cpp
└── utils/
    └── string_utils.cpp
```

---

## 📁 الهيكل الجديد المقترح

```
src/
├── core/                        # النواة الأساسية
│   ├── lexer/
│   │   ├── lexer_core.h
│   │   ├── lexer_core.cpp
│   │   ├── lexer_keywords.h
│   │   ├── lexer_keywords.cpp
│   │   ├── token.h
│   │   └── token.cpp
│   │
│   ├── parser/
│   │   ├── parser_core.h
│   │   ├── parser_core.cpp
│   │   ├── parser_core_helpers.cpp
│   │   ├── parser_core_impl.cpp
│   │   ├── parser_core_oop.cpp
│   │   ├── parser_modules.cpp
│   │   └── parser_classes.h
│   │
│   ├── ast/
│   │   ├── ast_node.h
│   │   ├── ast_node.cpp
│   │   ├── ast_visitor.h
│   │   ├── declarations.h
│   │   ├── declarations.cpp
│   │   ├── expressions.h
│   │   ├── expressions.cpp
│   │   ├── statements.h
│   │   ├── statements.cpp
│   │   ├── type_nodes.h
│   │   ├── type_nodes.cpp
│   │   ├── module_nodes.h
│   │   ├── module_nodes.cpp
│   │   ├── class_nodes.h
│   │   ├── property_nodes.h
│   │   ├── property_nodes.cpp
│   │   ├── pattern_nodes.h
│   │   ├── comprehension_nodes.h
│   │   └── advanced_expr_nodes.h
│   │
│   ├── types/
│   │   ├── value.h
│   │   ├── type.h
│   │   ├── data_types.h
│   │   ├── class_type.h
│   │   ├── object_instance.h
│   │   └── generator.h
│   │
│   ├── managers/
│   │   ├── variable_manager.h
│   │   ├── function_manager.h
│   │   ├── class_manager.h
│   │   ├── object_manager.h
│   │   └── scope_manager.h
│   │
│   ├── errors/
│   │   ├── error_codes.h
│   │   ├── error_manager.h
│   │   ├── diagnostic.h
│   │   └── source_location.h
│   │
│   ├── modules/
│   │   ├── module.h
│   │   ├── module.cpp
│   │   ├── module_loader.h
│   │   ├── module_loader.cpp
│   │   ├── module_cache.h
│   │   ├── module_cache.cpp
│   │   ├── module_resolver.h
│   │   ├── module_resolver.cpp
│   │   ├── module_validator.h
│   │   ├── module_validator.cpp
│   │   ├── dependency_graph.h
│   │   ├── dependency_graph.cpp
│   │   ├── symbol_resolver.h
│   │   ├── symbol_resolver.cpp
│   │   ├── search_paths.h
│   │   └── search_paths.cpp
│   │
│   └── utils/
│       ├── string_utils.h
│       └── string_utils.cpp
│
├── interpreter/
│   ├── interpreter.h
│   ├── interpreter_classes.h
│   ├── exception.h
│   ├── exception.cpp
│   ├── core/
│   ├── visitors/
│   │   ├── expression_evaluator.h
│   │   ├── expression_evaluator.cpp
│   │   └── ...
│   └── oop/
│
├── optimizer/                   # محسّن المستوى العالي
│   ├── advanced_optimizer.h
│   ├── advanced_optimizer.cpp
│   ├── optimization_pass.h
│   └── optimization_pass.cpp
│
├── low_level/
│   ├── bitwise_ops.h
│   ├── bitwise_ops.cpp
│   ├── pointer_type.h
│   ├── pointer_type.cpp
│   ├── memory_map.h
│   ├── memory_map.cpp
│   ├── io_ports.h
│   ├── io_ports.cpp
│   ├── interrupts.h
│   └── interrupts.cpp
│
├── compiler/                    # (من المرحلة 2)
│
└── main.cpp
```

---

## 📋 خطوات التنفيذ

### الخطوة 1: إنشاء الهيكل الجديد

```powershell
$base = "C:\s\s_language\src"

# Core
New-Item -ItemType Directory -Path "$base\core\lexer" -Force
New-Item -ItemType Directory -Path "$base\core\parser" -Force
New-Item -ItemType Directory -Path "$base\core\ast" -Force
New-Item -ItemType Directory -Path "$base\core\types" -Force
New-Item -ItemType Directory -Path "$base\core\managers" -Force
New-Item -ItemType Directory -Path "$base\core\errors" -Force
New-Item -ItemType Directory -Path "$base\core\modules" -Force
New-Item -ItemType Directory -Path "$base\core\utils" -Force
```

### الخطوة 2: نقل Lexer

```powershell
$newLexer = "C:\s\s_language\src\core\lexer"

# نقل ملفات الرأس
Copy-Item "C:\s\s_language\include\lexer\*.h" -Destination $newLexer

# نقل ملفات المصدر
Copy-Item "C:\s\s_language\src\lexer\*.cpp" -Destination $newLexer
```

### الخطوة 3: نقل Parser

```powershell
$newParser = "C:\s\s_language\src\core\parser"

# نقل ملفات الرأس
Copy-Item "C:\s\s_language\include\parser\parser_*.h" -Destination $newParser

# نقل ملفات المصدر
Copy-Item "C:\s\s_language\src\parser\parser_*.cpp" -Destination $newParser
```

### الخطوة 4: نقل AST

```powershell
$newAst = "C:\s\s_language\src\core\ast"

# نقل ملفات الرأس
Copy-Item "C:\s\s_language\include\parser\ast\*.h" -Destination $newAst

# نقل ملفات المصدر
Copy-Item "C:\s\s_language\src\parser\ast\*.cpp" -Destination $newAst
```

### الخطوة 5: نقل Types

```powershell
$newTypes = "C:\s\s_language\src\core\types"

Copy-Item "C:\s\s_language\include\data\types\*.h" -Destination $newTypes
```

### الخطوة 6: نقل Managers

```powershell
$newManagers = "C:\s\s_language\src\core\managers"

Copy-Item "C:\s\s_language\include\data\managers\*.h" -Destination $newManagers
Copy-Item "C:\s\s_language\include\data\scope\*.h" -Destination $newManagers
```

### الخطوة 7: نقل Errors

```powershell
$newErrors = "C:\s\s_language\src\core\errors"

Copy-Item "C:\s\s_language\include\errors\*.h" -Destination $newErrors
```

### الخطوة 8: نقل Modules

```powershell
$newModules = "C:\s\s_language\src\core\modules"

Copy-Item "C:\s\s_language\include\modules\*.h" -Destination $newModules
Copy-Item "C:\s\s_language\src\modules\*.cpp" -Destination $newModules
```

### الخطوة 9: نقل Utils

```powershell
$newUtils = "C:\s\s_language\src\core\utils"

Copy-Item "C:\s\s_language\include\utils\*.h" -Destination $newUtils
Copy-Item "C:\s\s_language\src\utils\*.cpp" -Destination $newUtils
```

### الخطوة 10: نقل Interpreter

```powershell
$newInterpreter = "C:\s\s_language\src\interpreter"

# نقل ملفات الرأس الرئيسية
Copy-Item "C:\s\s_language\include\interpreter\*.h" -Destination $newInterpreter

# نقل المجلدات الفرعية
Copy-Item "C:\s\s_language\include\interpreter\core" -Destination $newInterpreter -Recurse
Copy-Item "C:\s\s_language\include\interpreter\visitors" -Destination $newInterpreter -Recurse

# نقل ملفات المصدر
Copy-Item "C:\s\s_language\src\interpreter\*.cpp" -Destination $newInterpreter
Copy-Item "C:\s\s_language\src\interpreter\core\*" -Destination "$newInterpreter\core" -Recurse
Copy-Item "C:\s\s_language\src\interpreter\visitors\*" -Destination "$newInterpreter\visitors" -Recurse
Copy-Item "C:\s\s_language\src\interpreter\oop" -Destination $newInterpreter -Recurse
```

### الخطوة 11: نقل Optimizer (المستوى العالي)

```powershell
$newOptimizer = "C:\s\s_language\src\optimizer"

Copy-Item "C:\s\s_language\include\optimizer\*.h" -Destination $newOptimizer
Copy-Item "C:\s\s_language\src\optimizer\*.cpp" -Destination $newOptimizer
```

### الخطوة 12: نقل Low Level

```powershell
$newLowLevel = "C:\s\s_language\src\low_level"

Copy-Item "C:\s\s_language\include\low_level\*.h" -Destination $newLowLevel
Copy-Item "C:\s\s_language\src\low_level\*.cpp" -Destination $newLowLevel
```

---

## 🔧 تحديث مسارات Include

### أمثلة على التحديثات:

```cpp
// قبل
#include "lexer/lexer_core.h"
#include "parser/parser_core.h"
#include "parser/ast/ast_node.h"
#include "data/types/value.h"

// بعد
#include "core/lexer/lexer_core.h"
#include "core/parser/parser_core.h"
#include "core/ast/ast_node.h"
#include "core/types/value.h"
```

---

## ✅ قائمة التحقق

- [ ] إنشاء الهيكل الجديد
- [ ] نقل Lexer
- [ ] نقل Parser
- [ ] نقل AST
- [ ] نقل Types
- [ ] نقل Managers
- [ ] نقل Errors
- [ ] نقل Modules
- [ ] نقل Utils
- [ ] نقل Interpreter
- [ ] نقل Optimizer
- [ ] نقل Low Level
- [ ] تحديث مسارات #include
- [ ] تحديث CMakeLists.txt
- [ ] بناء المشروع
- [ ] تشغيل الاختبارات
- [ ] حذف المجلدات القديمة

---

## 🗑️ المجلدات المطلوب حذفها بعد النجاح

```powershell
# حذف include/ القديم
Remove-Item -Recurse -Force "C:\s\s_language\include"

# حذف المجلدات القديمة من src/
Remove-Item -Recurse -Force "C:\s\s_language\src\lexer"
Remove-Item -Recurse -Force "C:\s\s_language\src\parser"
Remove-Item -Recurse -Force "C:\s\s_language\src\data"
Remove-Item -Recurse -Force "C:\s\s_language\src\errors"
Remove-Item -Recurse -Force "C:\s\s_language\src\modules"
Remove-Item -Recurse -Force "C:\s\s_language\src\utils"
Remove-Item -Recurse -Force "C:\s\s_language\src\ast"
```

---

## ⚠️ ملاحظات هامة

1. **main.cpp**: يبقى في src/ مباشرة
2. **التبعيات الدائرية**: تحقق من عدم وجود تبعيات دائرية بعد النقل
3. **ملفات الشبكة**: ستُعالج في المرحلة 5 (stdlib)

---

## 🔗 المرحلة التالية

[المرحلة 4: دمج runtime و vm](04_phase4_runtime_vm_merge.md)
