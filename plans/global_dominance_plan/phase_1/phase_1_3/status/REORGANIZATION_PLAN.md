# 📁 خطة إعادة تنظيم بنية المشروع
# 📁 Project Reorganization Plan

**التاريخ:** 2 يناير 2026  
**الحالة:** 🟡 قيد المراجعة  
**المدة المتوقعة:** 2-3 أيام

---

## 🎯 الهدف (Objective)

إعادة تنظيم بنية المشروع لتكون أكثر وضوحاً ومنطقية:
1. ✅ **ملفات المترجم** تحت `compiler/`
2. ✅ **ملفات المفسر** تحت `interpreter/`
3. ✅ **الملفات المشتركة** تحت `shared/`
4. ✅ **ملفات الأدوات** تحت `tools/` مقسمة إلى مجلدات فرعية

---

## 📊 البنية الحالية (Current Structure)

```
C:\s\s_language\
├── include/                    # ❌ مختلط (مفسر + مشترك)
│   ├── interpreter/           # → مفسر
│   ├── data/types/            # → مفسر (Value System)
│   ├── lexer/                 # → مشترك
│   ├── parser/                # → مشترك
│   ├── errors/                # → مشترك
│   ├── modules/               # → مشترك
│   ├── stdlib/                # → مشترك
│   ├── low_level/             # → مشترك
│   ├── debug/                 # → مشترك
│   ├── cache/                 # → مشترك
│   ├── optimizer/             # → مترجم؟
│   └── utils/                 # → مشترك
│
├── src/                        # ❌ مختلط (مفسر + مشترك)
│   ├── interpreter/           # → مفسر
│   ├── ast/                   # → مشترك (؟)
│   ├── data/                  # → مفسر
│   ├── lexer/                 # → مشترك
│   ├── parser/                # → مشترك
│   ├── errors/                # → مشترك
│   ├── modules/               # → مشترك
│   ├── stdlib/                # → مشترك
│   ├── low_level/             # → مشترك
│   ├── debug/                 # → مشترك
│   ├── cache/                 # → مشترك
│   ├── optimizer/             # → مترجم؟
│   ├── utils/                 # → مشترك
│   └── main.cpp               # → مفسر
│
├── compiler/                   # ✅ واضح (مترجم)
│   ├── frontend/
│   ├── type_system/           # ✅ Phase 1.3.0
│   ├── backend/
│   ├── backends/
│   ├── jit/                   # ✅ Phase 1.2
│   ├── middleend/
│   ├── optimizer/
│   ├── runtime/
│   ├── bytecode/
│   ├── include/
│   └── src/
│
├── tools/                      # 🟡 يحتاج تنظيم
│   ├── compiler/              # ؟
│   ├── docgen/                # → doc-generator/
│   ├── lsp/                   # ✅
│   ├── pkg/                   # → package-manager/
│   ├── build/                 # → build-tools/
│   └── vscode-extension/      # ✅
│
├── bytecode/                   # → compiler/bytecode/ (مكرر)
├── optimizer/                  # → compiler/optimizer/ (مكرر)
├── runtime/                    # → compiler/runtime/ (مكرر)
├── vm/                         # → ؟ (VM منفصل)
│
├── tests/                      # ✅ (أو test/ موحد)
├── examples/                   # ✅
├── docs/                       # ✅
├── rules/                      # ✅
└── plans/                      # ✅
```

---

## 🎨 البنية المقترحة (Proposed Structure)

### المستوى الأول (Top-Level)

```
C:\s\s_language\
├── interpreter/        # ✅ المفسر (Interpreter)
├── compiler/           # ✅ المترجم (Compiler)
├── shared/             # ✅ الملفات المشتركة (Shared)
├── tools/              # ✅ الأدوات (Tools)
├── vm/                 # ✅ الآلة الافتراضية (Virtual Machine)
│
├── tests/              # ✅ الاختبارات (Tests)
├── examples/           # ✅ الأمثلة (Examples)
├── docs/               # ✅ التوثيق (Documentation)
├── rules/              # ✅ قواعد اللغة (Language Rules)
├── plans/              # ✅ خطط التطوير (Development Plans)
│
├── build/              # ✅ ملفات البناء (Build Artifacts)
├── CMakeLists.txt      # ✅ البناء الرئيسي (Main Build)
├── README.md           # ✅
└── LICENSE             # ✅
```

---

## 1️⃣ المفسر (Interpreter)

```
interpreter/
├── include/
│   ├── core/
│   │   ├── interpreter.h           # المفسر الرئيسي
│   │   ├── environment.h           # بيئة التنفيذ
│   │   └── scope.h                 # النطاقات
│   │
│   ├── visitors/
│   │   ├── expr_visitor.h          # زائر التعابير
│   │   ├── stmt_visitor.h          # زائر الجمل
│   │   └── pattern_visitor.h       # زائر الأنماط
│   │
│   ├── runtime/
│   │   ├── value.h                 # قيم runtime (من include/data/types/)
│   │   ├── class_type.h
│   │   ├── function.h
│   │   └── native_function.h
│   │
│   └── utils/
│       ├── exception.h             # استثناءات المفسر
│       └── ast_printer.h
│
├── src/
│   ├── core/
│   │   ├── interpreter.cpp
│   │   ├── environment.cpp
│   │   └── scope.cpp
│   │
│   ├── visitors/
│   │   ├── expr_visitor.cpp
│   │   ├── stmt_visitor.cpp
│   │   └── pattern_visitor.cpp
│   │
│   ├── runtime/
│   │   ├── value.cpp
│   │   ├── class_type.cpp
│   │   ├── function.cpp
│   │   └── native_function.cpp
│   │
│   └── main.cpp                    # نقطة الدخول
│
├── tests/                          # اختبارات المفسر
│   ├── test_interpreter.cpp
│   ├── test_values.cpp
│   └── ...
│
└── CMakeLists.txt                  # بناء المفسر
```

**الملفات المنقولة:**
```
include/interpreter/          → interpreter/include/core/
include/data/types/           → interpreter/include/runtime/
src/interpreter/              → interpreter/src/core/
src/data/                     → interpreter/src/runtime/
src/main.cpp                  → interpreter/src/main.cpp
```

---

## 2️⃣ المترجم (Compiler)

```
compiler/                           # ✅ كما هو (مع بعض التعديلات)
├── frontend/
│   ├── include/
│   │   ├── type_info.h            # SIR Type Info
│   │   ├── sir_types.h
│   │   └── ...
│   ├── src/
│   └── type_checker/              # فاحص الأنواع
│       ├── include/
│       ├── src/
│       └── tests/
│
├── type_system/                   # ✅ Phase 1.3.0
│   ├── include/
│   │   ├── type.h
│   │   ├── primitive_type.h
│   │   └── type_registry.h
│   ├── src/
│   └── tests/
│
├── backend/
│   └── llvm/                      # LLVM Backend
│       ├── include/
│       ├── src/
│       └── tests/
│
├── jit/                           # ✅ Phase 1.2 (JIT Engine)
│   ├── include/
│   ├── src/
│   └── benchmarks/
│
├── middleend/
│   └── optimizer/                 # تحسينات IR
│
├── bytecode/                      # نظام Bytecode
│   ├── include/
│   └── src/
│
├── runtime/                       # مكتبة runtime للكود المترجم
│   ├── include/
│   └── src/
│
└── CMakeLists.txt
```

**الملفات المنقولة:**
```
bytecode/                     → compiler/bytecode/ (دمج)
optimizer/ (الجذر)            → compiler/middleend/optimizer/
runtime/ (الجذر)              → compiler/runtime/ (دمج)
include/optimizer/            → compiler/middleend/optimizer/include/
src/optimizer/                → compiler/middleend/optimizer/src/
```

---

## 3️⃣ الملفات المشتركة (Shared)

```
shared/                             # الملفات المستخدمة من المفسر والمترجم
├── include/
│   ├── lexer/                     # المحلل المعجمي ✅
│   │   ├── lexer_core.h
│   │   ├── lexer_keywords.h
│   │   └── token.h
│   │
│   ├── parser/                    # المحلل النحوي ✅
│   │   ├── ast/                   # عقد AST ✅
│   │   │   ├── ast_node.h
│   │   │   ├── declarations.h
│   │   │   ├── expressions.h
│   │   │   ├── statements.h
│   │   │   ├── type_nodes.h      # (Phase 1.3.1)
│   │   │   └── ...
│   │   ├── parser_core.h
│   │   └── parser_classes.h
│   │
│   ├── errors/                    # إدارة الأخطاء ✅
│   │   ├── error_handler.h
│   │   ├── error_reporter.h
│   │   └── diagnostic.h
│   │
│   ├── modules/                   # نظام الوحدات ✅
│   │   ├── module_loader.h
│   │   ├── module_resolver.h
│   │   └── import_system.h
│   │
│   ├── stdlib/                    # المكتبة القياسية ✅
│   │   ├── io/
│   │   ├── math/
│   │   ├── string/
│   │   ├── collections/
│   │   └── system/
│   │
│   ├── utils/                     # أدوات مساعدة ✅
│   │   ├── string_utils.h
│   │   ├── file_utils.h
│   │   └── utf8_utils.h
│   │
│   ├── debug/                     # أدوات التنقيح ✅
│   │   ├── debugger.h
│   │   └── profiler.h
│   │
│   ├── cache/                     # التخزين المؤقت ✅
│   │   └── cache_manager.h
│   │
│   └── low_level/                 # عمليات منخفضة المستوى ✅
│       ├── memory.h
│       └── platform.h
│
├── src/
│   ├── lexer/                     # تنفيذ المحلل المعجمي
│   ├── parser/                    # تنفيذ المحلل النحوي
│   ├── ast/                       # تنفيذ عقد AST
│   ├── errors/                    # تنفيذ الأخطاء
│   ├── modules/                   # تنفيذ الوحدات
│   ├── stdlib/                    # تنفيذ المكتبة القياسية
│   ├── utils/                     # تنفيذ الأدوات
│   ├── debug/                     # تنفيذ التنقيح
│   ├── cache/                     # تنفيذ التخزين
│   └── low_level/                 # تنفيذ العمليات المنخفضة
│
└── CMakeLists.txt                 # بناء المكونات المشتركة
```

**الملفات المنقولة:**
```
include/lexer/                → shared/include/lexer/
include/parser/               → shared/include/parser/
include/errors/               → shared/include/errors/
include/modules/              → shared/include/modules/
include/stdlib/               → shared/include/stdlib/
include/utils/                → shared/include/utils/
include/debug/                → shared/include/debug/
include/cache/                → shared/include/cache/
include/low_level/            → shared/include/low_level/

src/lexer/                    → shared/src/lexer/
src/parser/                   → shared/src/parser/
src/ast/                      → shared/src/ast/
src/errors/                   → shared/src/errors/
src/modules/                  → shared/src/modules/
src/stdlib/                   → shared/src/stdlib/
src/utils/                    → shared/src/utils/
src/debug/                    → shared/src/debug/
src/cache/                    → shared/src/cache/
src/low_level/                → shared/src/low_level/
```

---

## 4️⃣ الأدوات (Tools)

```
tools/
├── lsp/                           # ✅ Language Server Protocol
│   ├── include/
│   ├── src/
│   ├── tests/
│   └── CMakeLists.txt
│
├── package-manager/               # مدير الحزم (pkg → package-manager)
│   ├── include/
│   ├── src/
│   ├── tests/
│   └── CMakeLists.txt
│
├── doc-generator/                 # مولد التوثيق (docgen → doc-generator)
│   ├── include/
│   ├── src/
│   └── CMakeLists.txt
│
├── build-tools/                   # أدوات البناء (tools/build → build-tools)
│   ├── scripts/
│   └── configs/
│
├── vscode-extension/              # ✅ امتداد VS Code
│   ├── src/
│   ├── package.json
│   └── README.md
│
├── debugger/                      # أداة التنقيح (جديد)
│   ├── include/
│   ├── src/
│   └── CMakeLists.txt
│
└── profiler/                      # أداة القياس (جديد)
    ├── include/
    ├── src/
    └── CMakeLists.txt
```

**الملفات المنقولة:**
```
tools/pkg/                    → tools/package-manager/
tools/docgen/                 → tools/doc-generator/
tools/build/                  → tools/build-tools/
tools/compiler/               → tools/build-tools/compiler-tools/
```

---

## 5️⃣ الآلة الافتراضية (Virtual Machine)

```
vm/                                # ✅ كما هو
├── include/
│   └── vm_core.h
├── src/
│   └── vm_core.cpp
└── CMakeLists.txt
```

**الملاحظة:** `vm/` يبدو أنه VM منفصل للـ bytecode. نبقيه منفصلاً.

---

## 📋 خطة التنفيذ (Implementation Plan)

### ⚠️ تحذير مهم

**هذا تغيير كبير جداً يؤثر على:**
- ✅ 1000+ ملف
- ✅ جميع مسارات `#include`
- ✅ جميع ملفات `CMakeLists.txt`
- ✅ جميع الاختبارات
- ✅ جميع الأدوات
- ✅ CI/CD configurations
- ✅ VSCode extensions

**المخاطر:**
- ⚠️ قد يكسر البناء بالكامل
- ⚠️ قد يستغرق 2-3 أيام لإصلاح جميع المسارات
- ⚠️ قد يؤثر على Phase 1.3.1

---

### المرحلة 1: التحضير (يوم 1 - صباحاً)

#### 1.1 النسخ الاحتياطي
```bash
# إنشاء نسخة احتياطية كاملة
git branch backup-before-reorganization
git commit -am "Backup before reorganization"
git push origin backup-before-reorganization
```

#### 1.2 إنشاء الهيكل الجديد
```bash
mkdir interpreter interpreter/include interpreter/src
mkdir shared shared/include shared/src
mkdir -p tools/package-manager tools/doc-generator tools/build-tools
```

---

### المرحلة 2: نقل الملفات (يوم 1 - ظهراً)

#### 2.1 المفسر (Interpreter)
```bash
# نقل ملفات المفسر
git mv include/interpreter interpreter/include/core
git mv include/data/types interpreter/include/runtime
git mv src/interpreter interpreter/src/core
git mv src/data interpreter/src/runtime
git mv src/main.cpp interpreter/src/main.cpp
```

#### 2.2 الملفات المشتركة (Shared)
```bash
# نقل الملفات المشتركة
git mv include/lexer shared/include/lexer
git mv include/parser shared/include/parser
git mv include/errors shared/include/errors
git mv include/modules shared/include/modules
git mv include/stdlib shared/include/stdlib
git mv include/utils shared/include/utils
git mv include/debug shared/include/debug
git mv include/cache shared/include/cache
git mv include/low_level shared/include/low_level

git mv src/lexer shared/src/lexer
git mv src/parser shared/src/parser
git mv src/ast shared/src/ast
git mv src/errors shared/src/errors
git mv src/modules shared/src/modules
git mv src/stdlib shared/src/stdlib
git mv src/utils shared/src/utils
git mv src/debug shared/src/debug
git mv src/cache shared/src/cache
git mv src/low_level shared/src/low_level
```

#### 2.3 المترجم (Compiler)
```bash
# دمج الملفات المكررة
git mv bytecode compiler/bytecode
git mv optimizer compiler/middleend/optimizer
git mv runtime compiler/runtime
```

#### 2.4 الأدوات (Tools)
```bash
# إعادة تسمية وتنظيم
git mv tools/pkg tools/package-manager
git mv tools/docgen tools/doc-generator
git mv tools/build tools/build-tools
git mv tools/compiler tools/build-tools/compiler-tools
```

---

### المرحلة 3: تحديث المسارات (يوم 1 - مساءً + يوم 2)

#### 3.1 تحديث #include في جميع الملفات

**أمثلة التغييرات:**

```cpp
// قبل:
#include "lexer/token.h"
#include "parser/ast/expressions.h"
#include "data/types/value.h"
#include "interpreter/core/interpreter.h"

// بعد:
#include "shared/lexer/token.h"
#include "shared/parser/ast/expressions.h"
#include "interpreter/runtime/value.h"
#include "interpreter/core/interpreter.h"
```

**Script للتحديث التلقائي:**
```python
# update_includes.py
import os
import re

mappings = {
    r'#include "lexer/': r'#include "shared/lexer/',
    r'#include "parser/': r'#include "shared/parser/',
    r'#include "errors/': r'#include "shared/errors/',
    r'#include "data/types/': r'#include "interpreter/runtime/',
    r'#include "interpreter/': r'#include "interpreter/core/',
    # ... المزيد
}

def update_file(filepath):
    with open(filepath, 'r', encoding='utf-8') as f:
        content = f.read()
    
    for old, new in mappings.items():
        content = re.sub(old, new, content)
    
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(content)

# تطبيق على جميع ملفات .cpp و .h
for root, dirs, files in os.walk('.'):
    for file in files:
        if file.endswith(('.cpp', '.h')):
            update_file(os.path.join(root, file))
```

#### 3.2 تحديث CMakeLists.txt

**Root CMakeLists.txt:**
```cmake
# قبل:
add_subdirectory(src)
add_subdirectory(compiler)
add_subdirectory(tools)

# بعد:
add_subdirectory(shared)          # الملفات المشتركة
add_subdirectory(interpreter)     # المفسر
add_subdirectory(compiler)        # المترجم
add_subdirectory(vm)              # VM
add_subdirectory(tools)           # الأدوات
```

**إنشاء CMakeLists.txt جديدة:**
- `interpreter/CMakeLists.txt`
- `shared/CMakeLists.txt`
- `tools/package-manager/CMakeLists.txt`
- `tools/doc-generator/CMakeLists.txt`

---

### المرحلة 4: الاختبار (يوم 2 - مساءً)

#### 4.1 بناء المشروع
```bash
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
```

#### 4.2 تشغيل الاختبارات
```bash
# اختبار المفسر
.\bin\Release\sad.exe examples\test_main_function.s

# اختبار المترجم
.\bin\Release\test_type_system_core.exe

# اختبار JIT
.\bin\Release\test_jit_fibonacci.exe
```

---

### المرحلة 5: التوثيق (يوم 3)

#### 5.1 تحديث README.md
```markdown
# بنية المشروع الجديدة

## المجلدات الرئيسية

- `interpreter/` - المفسر (Interpreter)
- `compiler/` - المترجم (Compiler)
- `shared/` - الملفات المشتركة (Shared)
- `tools/` - الأدوات (Tools)
- `vm/` - الآلة الافتراضية (VM)
```

#### 5.2 إنشاء دليل الهجرة
```
docs/migration/REORGANIZATION_GUIDE.md
```

---

## ⚖️ الموازنة (Trade-offs)

### ✅ الفوائد (Benefits)

1. **وضوح البنية:** سهولة فهم أين يذهب كل ملف
2. **فصل المخاوف:** المفسر منفصل عن المترجم
3. **سهولة الصيانة:** تغييرات المفسر لا تؤثر على المترجم
4. **توثيق ذاتي:** البنية توضح الوظائف
5. **سهولة البناء:** يمكن بناء المفسر فقط أو المترجم فقط

### ⚠️ المخاطر (Risks)

1. **وقت التنفيذ:** 2-3 أيام عمل
2. **كسر البناء:** قد يستغرق وقتاً لإصلاح جميع المسارات
3. **تأخير Phase 1.3.1:** سيتأخر 2-3 أيام
4. **صعوبة الرجوع:** صعب التراجع بعد البدء
5. **تحديث الأدوات:** VS Code extension قد يحتاج تحديث

---

## 🤔 الاقتراح (Recommendation)

### الخيار 1: تنفيذ كامل الآن ⏳

**الإيجابيات:**
- ✅ بنية نظيفة للمستقبل
- ✅ سهولة الصيانة

**السلبيات:**
- ⚠️ تأخير 2-3 أيام
- ⚠️ قد يكسر البناء
- ⚠️ تأخير Phase 1.3.1

**الجدول الزمني:**
- يوم 1: نقل الملفات
- يوم 2: تحديث المسارات وإصلاح البناء
- يوم 3: الاختبار والتوثيق

### الخيار 2: تنفيذ تدريجي 🐢

**المرحلة 1:** نقل الملفات المشتركة فقط
```
include/ → shared/include/
src/ → shared/src/
```

**المرحلة 2:** (بعد Phase 1.3.1) نقل المفسر
```
include/interpreter/ → interpreter/include/
src/interpreter/ → interpreter/src/
```

**المرحلة 3:** (بعد Phase 1.3.2) إعادة تنظيم الأدوات
```
tools/pkg/ → tools/package-manager/
```

**الإيجابيات:**
- ✅ لا تأخير كبير
- ✅ أقل خطورة

**السلبيات:**
- ⚠️ بنية مختلطة لفترة طويلة
- ⚠️ قد يسبب ارتباك

### الخيار 3: تأجيل حتى Phase 1.4 🔮

تأجيل إعادة التنظيم حتى نكمل Phase 1.3 بالكامل (الأسابيع الـ6).

**الإيجابيات:**
- ✅ لا تأخير
- ✅ التركيز على Type System

**السلبيات:**
- ⚠️ البنية المختلطة تستمر
- ⚠️ صعوبة أكبر لاحقاً

---

## 🎯 التوصية النهائية (Final Recommendation)

### ✅ الخيار المقترح: **تنفيذ تدريجي (الخيار 2)**

**الخطة:**

1. **الآن (قبل Phase 1.3.1):** نقل الملفات المشتركة فقط
   - `include/lexer/` → `shared/include/lexer/`
   - `include/parser/` → `shared/include/parser/`
   - الخ...
   - **المدة:** نصف يوم فقط
   - **التأثير:** محدود

2. **بعد Phase 1.3.3:** نقل المفسر
   - **المدة:** يوم واحد
   - **التأثير:** متوسط

3. **بعد Phase 1.3.6:** إعادة تنظيم الأدوات
   - **المدة:** نصف يوم
   - **التأثير:** محدود

**الفوائد:**
- ✅ تحسين تدريجي
- ✅ لا تأخير كبير
- ✅ أقل خطورة
- ✅ إمكانية التراجع

---

## 📝 السؤال للمستخدم

**ما هو الخيار المفضل؟**

1. ⏩ **تنفيذ كامل الآن** (2-3 أيام، تأخير Phase 1.3.1)
2. 🐢 **تنفيذ تدريجي** (نصف يوم الآن، باقي لاحقاً) ← **مُقترح**
3. 🔮 **تأجيل حتى Phase 1.4** (لا تأخير، بنية مختلطة)

---

**الحالة:** 🟡 بانتظار قرار المستخدم  
**التاريخ:** 2 يناير 2026
