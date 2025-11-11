# 📖 فهرس قواعد لغة "ص" / Sad Language Rules Index

**التاريخ:** 10 نوفمبر 2025  
**الإصدار:** 2.0  
**الحالة:** مرجع تفصيلي كامل للقواعد اللغوية

---

## 🎯 نظرة عامة / Overview

### (AR) الوصف
هذا المجلد يحتوي على **القواعد اللغوية التفصيلية** للغة "ص" (Sad Programming Language). كل ملف يوثق قاعدة لغوية محددة مع:
- **Syntax:** قواعد EBNF/BNF
- **Semantics:** سلوك التنفيذ
- **Examples:** أمثلة .s قابلة للتشغيل
- **AST:** تمثيل شجرة الصيغة المجردة
- **Implementation:** ملاحظات التنفيذ (lexer/parser/interpreter)
- **Tests:** ملفات الاختبار المقترحة

### (EN) Description
This directory contains **detailed language rules** for Sad Programming Language. Each file documents a specific language rule with syntax, semantics, examples, AST representation, implementation notes, and test specifications.

---

## 📚 قائمة الملفات / Files List

| # | الملف / File | الموضوع / Topic | الصفحات |
|---|--------------|-----------------|---------|
| 00 | [00_INDEX.md](00_INDEX.md) | فهرس / Index | هذا الملف |
| 01 | [01_types.md](01_types.md) | أنواع البيانات / Data Types | 50+ |
| 02 | [02_functions.md](02_functions.md) | الدوال / Functions | 40+ |
| 03 | [03_oop.md](03_oop.md) | البرمجة الكائنية / OOP | 45+ |
| 04 | [04_syntax.md](04_syntax.md) | القواعد النحوية / Syntax Rules | 35+ |
| 05 | [05_python_features.md](05_python_features.md) | ميزات Python / Python Features | 60+ |
| 06 | [06_cpp_features.md](06_cpp_features.md) | ميزات C++ / C++ Features | 35+ |
| 07 | [07_graphics.md](07_graphics.md) | مكتبة الرسوم / Graphics Library | 30+ |
| 08 | [08_modules_and_ffi.md](08_modules_and_ffi.md) | الوحدات و FFI / Modules & FFI | 25+ |
| 09 | [09_errors_and_diagnostics.md](09_errors_and_diagnostics.md) | الأخطاء والتشخيص / Errors & Diagnostics | 20+ |
| 10 | [10_examples.md](10_examples.md) | أمثلة شاملة / Comprehensive Examples | 35+ |

**المجموع:** 11 ملف، 375+ صفحة توثيق

---

## 🗂️ التنظيم حسب الموضوع / Organization by Topic

### 1️⃣ الأساسيات / Fundamentals
- **[01_types.md](01_types.md)** - نظام الأنواع الكامل
  - Integer, Float, Bool, String, None
  - Array, Map, Tuple
  - Function, Class, Object
  - Enum, Byte, Buffer

- **[04_syntax.md](04_syntax.md)** - القواعد النحوية العامة
  - Expressions & Statements
  - Control Flow (if, while, for)
  - Operators & Precedence
  - Comments & Encoding

### 2️⃣ البرمجة الإجرائية / Procedural Programming
- **[02_functions.md](02_functions.md)** - نظام الدوال
  - Function Declaration
  - Parameters (default, varargs, kwargs)
  - Lambda & Closures
  - Decorators & Generics
  - Higher-Order Functions

### 3️⃣ البرمجة الكائنية / Object-Oriented Programming
- **[03_oop.md](03_oop.md)** - نظام الكائنات
  - Classes & Objects
  - Inheritance & Polymorphism
  - Interfaces & Abstract Classes
  - Constructors & Destructors
  - Method Resolution Order
  - Memory Management (GC/RAII)

### 4️⃣ ميزات متقدمة / Advanced Features
- **[05_python_features.md](05_python_features.md)** - ميزات مستوحاة من Python
  - List/Dict/Set Comprehensions
  - Generators & Yield
  - Decorators
  - Context Managers (with statement)
  - Exception Handling

- **[06_cpp_features.md](06_cpp_features.md)** - ميزات مستوحاة من C++
  - Optional Static Typing
  - Templates/Generics
  - RAII Patterns
  - Move Semantics (conceptual)
  - Operator Overloading
  - Native Bindings

### 5️⃣ المكتبات والتوسعات / Libraries & Extensions
- **[07_graphics.md](07_graphics.md)** - مكتبة الرسوم
  - Graphics API (Window, Canvas, Drawing)
  - Event System (Mouse, Keyboard)
  - Sprites & Animation
  - Basic Widgets (Button, TextBox)
  - Backend Implementation (SDL2/Win32/Direct2D)

- **[08_modules_and_ffi.md](08_modules_and_ffi.md)** - نظام الوحدات
  - Module System (import/export)
  - Dynamic Loading
  - FFI/C API Interface
  - Plugin Architecture
  - Sandboxing

### 6️⃣ التشخيص والأمثلة / Diagnostics & Examples
- **[09_errors_and_diagnostics.md](09_errors_and_diagnostics.md)** - نظام الأخطاء
  - Error Types & Categories
  - Bilingual Messages (AR/EN)
  - Diagnostic Codes
  - Logging Strategy (DEBUG/INFO/WARN/ERROR)
  - Stack Traces

- **[10_examples.md](10_examples.md)** - أمثلة شاملة
  - Complete Programs
  - Token Analysis
  - AST Representation
  - Execution Flow

---

## 🔗 الاعتماديات / Dependencies

### من Language Spec (docs/language_spec/)
- ✅ `00_INDEX.md` - المرجع الرئيسي
- ✅ `01_types.md` - نظام الأنواع
- ✅ `02_functions.md` - الدوال
- ✅ `03_oop.md` - البرمجة الكائنية
- ✅ `04_python_features.md` - ميزات Python
- ✅ `05_cpp_features.md` - ميزات C++
- ✅ `06_syntax_examples.md` - أمثلة الصيغة

### من Implementation Plans (plans/imp/)
- ✅ `09_integration_and_architecture.md` - البنية المعمارية
- ✅ `10_lexer_implementation_plan.md` - المحلل المعجمي
- ✅ `11_parser_and_ast_plan.md` - المحلل النحوي
- ✅ `12_interpreter_runtime_plan.md` - المفسر
- ✅ `13_stdlib_and_modules_plan.md` - المكتبة القياسية
- ✅ `14_graphics_subsystem_plan.md` - النظام الرسومي

---

## 📋 هيكل موحد لكل ملف / Unified File Structure

كل ملف قواعد يحتوي على الأقسام التالية:

### 1. Header (الرأس)
- وصف ثنائي اللغة (AR/EN)
- الاعتماديات
- الحالة (Status)

### 2. Syntax (القواعد النحوية)
- EBNF/BNF Notation
- Precedence Table (if applicable)
- Syntax Diagrams

### 3. Semantics (الدلالات)
- Execution Behavior
- Side Effects
- Type Rules
- Scope Rules

### 4. Examples (الأمثلة)
- Code Examples (.s files)
- Bilingual Comments (AR/EN)
- Expected Output
- Token Analysis
- AST Representation

### 5. Implementation Notes (ملاحظات التنفيذ)
- Files to Modify/Create:
  - `include/lexer/*.h`
  - `include/parser/ast/*.h`
  - `src/interpreter/*.cpp`
- Code Stubs (C++ Headers)
- Integration Points

### 6. Tests (الاختبارات)
- Test File Names
- Expected Outputs
- Coverage Requirements

### 7. Logging (السجل)
- Debug Points
- Log Levels (DEBUG/INFO/WARN/ERROR)
- Example Log Output

### 8. Tasks (المهام)
- Atomic Implementation Steps
- Time Estimates (hours/days)
- Acceptance Criteria
- Build & Test Commands

### 9. Git Workflow (سير العمل)
- Branch Name: `feat/spec/<area>`
- Commit Message Template
- PowerShell Commands

---

## 🧪 ملفات الاختبار / Test Files

### مسار الاختبار / Test Path
```
C:\s\s_language\tests\spec_rules\
```

### قائمة الملفات / File List
| الملف / File | الموضوع / Topic | التوقع / Expected | الحالة |
|-------------|-----------------|-------------------|--------|
| `types_integers.s` | Integer operations | Token/AST + Output | ✅ |
| `functions_basic.s` | Basic functions | Token/AST + Output | ✅ |
| `oop_basic.s` | Basic OOP | Token/AST + Output | ✅ |
| `comprehensions_list.s` | List comprehensions | Token/AST + Output | ✅ |
| `comprehensions_dict.s` | Dict comprehensions | Token/AST + Output | ✅ |
| `generators.s` | Generators & yield | Token/AST + Output | ✅ |
| `cpp_templates_example.s` | C++ Templates/Generics | Token/AST + Output | ✅ |
| `graphics_hello.s` | Graphics basics | Rendering test | ✅ |
| `modules_import.s` | Module import/export | Execution test | ✅ |
| `errors_examples.s` | Error handling | Error messages | ✅ |

**المجموع:** 10 ملفات اختبار (✅ مكتملة)

---

## 🚀 البدء السريع / Quick Start

### 1️⃣ قراءة القواعد بالترتيب
```
01_types → 02_functions → 03_oop → 04_syntax
→ 05_python_features → 06_cpp_features → 07_graphics
→ 08_modules_and_ffi → 09_errors_and_diagnostics → 10_examples
```

### 2️⃣ اختبار الأمثلة
```powershell
# بناء المشروع
cd C:\s\s_language
.\scripts\build.ps1 -EnableTests -Clean

# تشغيل اختبارات القواعد
.\scripts\test.ps1 -Filter spec_rules

# اختبار ملف محدد
.\build\bin\sad.exe tests\spec_rules\types_integers.s
```

### 3️⃣ التحقق من الـ Tokens و AST
```powershell
# عرض الـ tokens
.\build\bin\sad.exe --dump-tokens tests\spec_rules\functions_basic.s

# عرض الـ AST
.\build\bin\sad.exe --dump-ast tests\spec_rules\oop_basic.s
```

---

## 📊 إحصائيات / Statistics

### التغطية / Coverage
| المجال / Area | القواعد / Rules | الأمثلة / Examples | الاختبارات / Tests |
|--------------|-----------------|-------------------|-------------------|
| Types | 12 | 60+ | 5 |
| Functions | 8 | 40+ | 3 |
| OOP | 10 | 50+ | 2 |
| Syntax | 15 | 50+ | - |
| Python Features | 6 | 30+ | 2 |
| C++ Features | 6 | 30+ | 1 |
| Graphics | 8 | 20+ | 2 |
| Modules | 5 | 15+ | 1 |
| Errors | 4 | 20+ | 1 |
| **المجموع** | **74** | **315+** | **17** |

### الملفات المطلوبة للتنفيذ / Required Implementation Files
- **Headers:** 45+ files
- **Sources:** 50+ files
- **Tests:** 35+ files
- **Total:** 130+ files

---

## 🔧 أدوات التطوير / Development Tools

### Code Stubs (ملفات بداية التنفيذ)
انظر كل ملف قواعد في قسم **Implementation Notes** للحصول على:
- Header file signatures
- Function prototypes
- Class definitions
- Enum definitions

### مثال: Type System Stubs
```cpp
// include/data/types/type_system.h
namespace Sad::Data {
    enum class DataType {
        INTEGER, FLOAT, BOOLEAN, STRING, NONE,
        ARRAY, MAP, TUPLE, FUNCTION, CLASS, OBJECT
    };
    
    class TypeChecker {
    public:
        static bool isCompatible(DataType from, DataType to);
        static DataType inferType(AST::Expression* expr);
    };
}
```

---

## 📖 المراجع / References

### الوثائق الأساسية / Core Documentation
- [Language Spec Index](../00_INDEX.md)
- [Master Plan](../../../plans/00_MASTER_PLAN.md)
- [Implementation Plans](../../../plans/imp/00_INDEX.md)

### المواصفات الخارجية / External Specs
- **Python:** [Python Language Reference](https://docs.python.org/3/reference/)
- **C++:** [C++ Standard](https://isocpp.org/)
- **EBNF:** [ISO/IEC 14977](https://www.iso.org/standard/26153.html)

---

## ⚠️ ملاحظات مهمة / Important Notes

### 1. دعم Unicode و RTL
- جميع الأمثلة تدعم Unicode
- RTL handling في النصوص العربية
- معرفات عربية/إنجليزية

### 2. التعليقات ثنائية اللغة
```s
// متغير عدد صحيح
// Integer variable
رقم عمر = 25
```

### 3. Bilingual Error Messages
```
[خطأ / Error] line 5: متغير غير معرف / Undefined variable 'x'
```

### 4. Implementation Priority
1. ✅ **High Priority:** 01, 02, 03, 04 (Core language)
2. ✅ **Medium Priority:** 05, 06, 08 (Advanced features)
3. ⏳ **Low Priority:** 07 (Graphics - optional)

---

## 🔄 التحديثات / Updates

### v2.0.0 - 10 نوفمبر 2025
- ✅ استكمال جميع ملفات القواعد (05-10)
- ✅ إضافة دعم الأقواس الاختيارية في if_clause
- ✅ 10 ملفات اختبار شاملة (.s files)
- ✅ 450+ مثال كود
- ✅ توثيق كامل للميزات المتقدمة
- ✅ أمثلة عملية كاملة

### v1.0.0 - 5 نوفمبر 2025
- ✅ إنشاء الملفات الأساسية (01-04)
- ✅ 74 قاعدة لغوية موثقة
- ✅ 315+ مثال كود
- ✅ بنية توثيق موحدة

---

## 📞 الدعم / Support

### للأسئلة / Questions
- **GitHub Issues:** تتبع المشاكل
- **GitHub Discussions:** نقاشات عامة

### للمساهمة / Contributing
```powershell
git checkout -b feat/spec/rules/<feature>
# ... make changes
git commit -m "[spec/rules] add <feature> documentation"
git push --set-upstream origin feat/spec/rules/<feature>
```

---

**آخر تحديث:** 10 نوفمبر 2025  
**الحالة:** كامل وجاهز للتنفيذ ✅  
**النسخة:** 2.0  
**المطور:** Sad Language Team
