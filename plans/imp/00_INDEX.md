# 📑 فهرس خطط التنفيذ / Implementation Plans Index

**التاريخ:** 4 نوفمبر 2025  
**الإصدار:** 1.0  
**المجموع:** 8 ملفات خطة تنفيذية

---

## 📋 نظرة عامة / Overview

هذا المجلد يحتوي على **8 ملفات خطة تنفيذية تفصيلية** لتطوير لغة "ص" (Sad Language). كل ملف يحتوي على 14 قسماً موحداً يشمل الأهداف، البنية المعمارية، تقسيم العمل، الاختبارات، الجدول الزمني، والمخاطر.

---

## 📚 قائمة الخطط / Plans List

### 1️⃣ [09_integration_and_architecture.md](09_integration_and_architecture.md)
**العنوان:** التكامل والبنية المعمارية / Integration & Architecture  
**المدة:** أسبوعان (80 ساعة)  
**الأولوية:** 🔴 عالية جداً (Foundation)

**الملخص:**
- تعريف Core Interfaces: Token, AST Node, Value
- بنية النظام الكلية (Lexer → Parser → Interpreter)
- نظام القيم (Value System) مع union-based implementation
- Visitor Pattern للـ AST
- SadEngine pipeline رئيسي
- استراتيجية الاختبار والـ Logging
- PowerShell build scripts
- جدول Sprints (8 أسابيع، 2-4 مطورين)

**الاعتماديات:**
- Language Spec (docs/language_spec/)
- Master Plans (plans/)

**المخرجات:**
- 50+ ملف header/source
- Core interfaces كاملة
- Build system أساسي
- Sprint 1-2 task breakdown

---

### 2️⃣ [10_lexer_implementation_plan.md](10_lexer_implementation_plan.md)
**العنوان:** تنفيذ المحلل المعجمي / Lexer Implementation  
**المدة:** 3 أسابيع (120 ساعة)  
**الأولوية:** 🟠 عالية

**الملخص:**
- Token & Position system محسّن
- دعم Unicode/RTL كامل
- معالجة الأرقام (decimal, binary, octal, hex, scientific)
- معالجة النصوص (single, double, triple quotes, f-strings)
- معالجة المعرفات (عربي/إنجليزي)
- معالجة العوامل والرموز الخاصة
- معالجة التعليقات (سطر واحد ومتعددة)
- Streaming lexer للملفات الكبيرة

**الاعتماديات:**
- `09_integration_and_architecture.md` (Token interface)
- `01_types.md` (Type system)
- `06_syntax_examples.md` (Syntax examples)

**الملفات الجديدة:**
- `lexer_numbers.cpp`, `lexer_strings.cpp`, `lexer_identifiers.cpp`
- `lexer_operators.cpp`, `lexer_comments.cpp`
- 7 ملفات اختبار (60+ tests)

**معايير القبول:**
- ✅ 90+ TokenType supported
- ✅ Unicode + RTL works
- ✅ All number formats parse correctly
- ✅ F-strings work
- ✅ 60+ tests pass with ≥90% coverage

---

### 3️⃣ [11_parser_and_ast_plan.md](11_parser_and_ast_plan.md)
**العنوان:** المُحلل النحوي و AST / Parser & AST Implementation  
**المدة:** 4 أسابيع (160 ساعة)  
**الأولوية:** 🟠 عالية

**الملخص:**
- EBNF grammar كامل للغة
- AST Node Hierarchy (18+ classes)
- Recursive Descent Parser
- Operator precedence handling
- Error recovery مع bilingual diagnostics
- Visitor Pattern implementation
- AST Printer utility
- تحليل دلالي أساسي

**الاعتماديات:**
- `10_lexer_implementation_plan.md` (Token)
- `02_functions.md`, `03_oop.md` (Grammar rules)
- `09_integration_and_architecture.md` (AST interfaces)

**الملفات الجديدة:**
- `ast/expressions.h`, `ast/statements.h`, `ast/declarations.h`
- `ast_visitor.h`, `ast_printer.h`
- `parser_expressions.cpp`, `parser_statements.cpp`
- 7 ملفات اختبار (50+ tests)

**معايير القبول:**
- ✅ All expression types parse
- ✅ All statement types parse
- ✅ Precedence correct
- ✅ Error recovery works
- ✅ 50+ tests pass with ≥85% coverage

---

### 4️⃣ [12_interpreter_runtime_plan.md](12_interpreter_runtime_plan.md)
**العنوان:** تنفيذ المُفسّر / Interpreter Runtime Implementation  
**المدة:** 5 أسابيع (200 ساعة)  
**الأولوية:** 🔴 عالية جداً

**الملخص:**
- Value System شامل (10 types)
- Environment/Scope management
- Call Stack + stack traces
- Expression evaluation (Binary, Unary, Call, Index, Member, Lambda)
- Statement execution (If, While, For, Return, Block)
- Function support مع Closures
- OOP support (Classes, Methods, Inheritance)
- Memory management (GC roadmap)
- FFI لربط StdLib

**الاعتماديات:**
- `11_parser_and_ast_plan.md` (AST + Visitor)
- `09_integration_and_architecture.md` (Value interface)
- `01_types.md`, `02_functions.md`, `03_oop.md`

**الملفات الجديدة:**
- `interpreter_core.cpp`, `call_stack.cpp`, `exceptions.cpp`
- `binary_evaluator.cpp`, `call_evaluator.cpp`
- `control_flow.cpp`, `class_instance.cpp`
- `environment.cpp`, `value.cpp` (extended)
- 7 ملفات اختبار (92+ tests)

**معايير القبول:**
- ✅ All value types work
- ✅ All expressions evaluate correctly
- ✅ All statements execute
- ✅ Functions + closures work
- ✅ Basic OOP works
- ✅ 92+ tests pass with ≥80% coverage

---

### 5️⃣ [13_stdlib_and_modules_plan.md](13_stdlib_and_modules_plan.md)
**العنوان:** المكتبة القياسية والموديولات / StdLib & Modules Implementation  
**المدة:** 3 أسابيع (120 ساعة)  
**الأولوية:** 🟡 متوسطة-عالية

**الملخص:**
- StdLib Manager مع dynamic registration
- **IO Module** (8 functions): print, input, file I/O
- **String Module** (14 functions): length, upper, lower, split, join, starts_with, ends_with
- **Math Module** (15 functions): abs, sqrt, pow, sin, cos, tan, random, PI, E
- **Time Module** (10 functions): now, sleep, clock, date, time_str
- Module loading system
- FFI interface

**الاعتماديات:**
- `12_interpreter_runtime_plan.md` (Value system)
- `09_integration_and_architecture.md` (StdLib Manager interface)

**الملفات الجديدة:**
- `io/print.cpp`, `io/input.cpp`, `io/file_io.cpp`
- `string/manipulation.cpp`
- `math/basic.cpp`, `math/trigonometry.cpp`, `math/random.cpp`
- `time/datetime.cpp`, `time/timing.cpp`
- 5 ملفات اختبار (68+ tests)

**معايير القبول:**
- ✅ All 4 modules work
- ✅ 47 functions implemented
- ✅ Module registration works
- ✅ 68+ tests pass with ≥85% coverage

---

### 6️⃣ [14_graphics_subsystem_plan.md](14_graphics_subsystem_plan.md)
**العنوان:** النظام الرسومي / Graphics Subsystem Implementation  
**المدة:** أسبوعان (80 ساعة)  
**الأولوية:** 🟢 متوسطة (Optional Phase 2)

**الملخص:**
- Graphics Backend Abstraction (Interface)
- SDL2 Backend implementation
- Window management
- Drawing primitives: line, rect, circle
- Event system (keyboard, mouse, quit)
- Color management
- StdLib integration (graphics module)
- Win32/Console fallback backends (roadmap)

**الاعتماديات:**
- `13_stdlib_and_modules_plan.md` (StdLib integration)
- SDL2 library (vcpkg)

**الملفات الجديدة:**
- `graphics_backend.h`, `graphics_module.h`
- `backends/sdl2_backend.cpp`
- 4 ملفات اختبار (35+ tests)

**معايير القبول:**
- ✅ Backend interface works
- ✅ SDL2 backend functional
- ✅ All drawing primitives work
- ✅ Event polling works
- ✅ 35+ tests pass with ≥80% coverage

**Roadmap Phase 3:**
- Advanced Text (SDL_ttf)
- Image Loading (SDL_image)
- Sprites, UI Widgets, Animation

---

### 7️⃣ [15_build_ci_and_tools_plan.md](15_build_ci_and_tools_plan.md)
**العنوان:** البناء و CI/CD / Build, CI/CD & Tools Implementation  
**المدة:** أسبوع واحد (56 ساعة)  
**الأولوية:** 🟡 متوسطة-عالية

**الملخص:**
- Enhanced CMakeLists.txt (Options, Testing, Documentation)
- PowerShell scripts: `build.ps1`, `test.ps1`, `clean.ps1`, `coverage.ps1`, `release.ps1`
- GitHub Actions CI pipeline (Windows + Linux)
- Code coverage integration (lcov/Codecov)
- Release workflow مع artifacts
- Doxygen documentation generation

**الاعتماديات:**
- All implementation plans (testing infrastructure)

**الملفات الجديدة:**
- `CMakeLists.txt` (comprehensive)
- `scripts/*.ps1` (5 scripts)
- `.github/workflows/*.yml` (3 workflows)
- `Doxyfile`

**معايير القبول:**
- ✅ CMake configures on Windows/Linux
- ✅ All targets build
- ✅ PowerShell scripts work
- ✅ CI passes on GitHub Actions
- ✅ Coverage reports generate

---

### 8️⃣ [16_extension_api_and_plugins.md](16_extension_api_and_plugins.md)
**العنوان:** Extension API والـ Plugins / Extension API & Plugins Implementation  
**المدة:** أسبوع واحد (56 ساعة)  
**الأولوية:** 🟢 متوسطة (Optional Phase 2)

**الملخص:**
- C API للـ language core
- Plugin loading system (DLL/SO)
- Native function registration
- Value marshalling (C ↔ Sad)
- Error handling في plugins
- Example plugin بلغة C++
- Plugin security roadmap

**الاعتماديات:**
- `12_interpreter_runtime_plan.md` (Value system)
- `13_stdlib_and_modules_plan.md` (StdLib Manager)

**الملفات الجديدة:**
- `api/sad_api.h`, `api/sad_api.cpp`
- `modules/plugin_loader.h`, `modules/plugin_loader.cpp`
- `examples/plugins/example_plugin.cpp`
- 2 ملفات اختبار (18+ tests)

**معايير القبول:**
- ✅ C API complete and documented
- ✅ Plugin loading works (DLL)
- ✅ Native functions register
- ✅ Example plugin works
- ✅ 18+ tests pass with ≥85% coverage

**Roadmap Phase 3:**
- Python bindings
- Sandboxing
- Hot reload
- Plugin manager

---

## 📊 إحصائيات / Statistics

### الجدول الزمني الكلي / Total Timeline

| الخطة | المدة | الساعات | الأولوية |
|-------|------|---------|----------|
| 09. Integration & Architecture | أسبوعان | 80 | 🔴 عالية جداً |
| 10. Lexer | 3 أسابيع | 120 | 🟠 عالية |
| 11. Parser & AST | 4 أسابيع | 160 | 🟠 عالية |
| 12. Interpreter | 5 أسابيع | 200 | 🔴 عالية جداً |
| 13. StdLib | 3 أسابيع | 120 | 🟡 متوسطة-عالية |
| 14. Graphics | أسبوعان | 80 | 🟢 متوسطة |
| 15. Build & CI | أسبوع | 56 | 🟡 متوسطة-عالية |
| 16. Extension API | أسبوع | 56 | 🟢 متوسطة |
| **المجموع** | **21 أسبوع** | **872 ساعة** | - |

**بفريق 2-4 مطورين:** 10-14 أسبوع عمل فعلي (2.5-3.5 شهر)

---

### الملفات / Files

| النوع | العدد |
|-------|-------|
| Header Files (.h) | 65+ |
| Source Files (.cpp) | 70+ |
| Test Files | 35+ |
| Scripts | 5 |
| Workflows | 3 |
| **المجموع** | **178+ ملف** |

---

### الاختبارات / Tests

| الوحدة | عدد الاختبارات |
|--------|---------------|
| Lexer | 60+ |
| Parser | 50+ |
| Interpreter | 92+ |
| StdLib | 68+ |
| Graphics | 35+ |
| API | 18+ |
| **المجموع** | **323+ test** |

**Target Coverage:** ≥80-90%

---

## 🔄 تسلسل التنفيذ / Execution Sequence

### Phase 1: Core Foundation (الأساس) - 6 أسابيع
1. ✅ **09. Integration & Architecture** (أسبوعان)
2. ✅ **10. Lexer** (3 أسابيع)
3. ✅ **15. Build & CI** (أسبوع) - متوازي مع Lexer

**المخرجات:**
- Token system كامل
- Build infrastructure
- CI pipeline
- Testing framework

---

### Phase 2: Parsing & Execution (التحليل والتنفيذ) - 9 أسابيع
4. ✅ **11. Parser & AST** (4 أسابيع)
5. ✅ **12. Interpreter** (5 أسابيع)

**المخرجات:**
- AST complete
- Interpreter runtime
- Functions + Closures
- Basic OOP

---

### Phase 3: Libraries & Extensions (المكتبات والتوسعات) - 6 أسابيع
6. ✅ **13. StdLib** (3 أسابيع)
7. ⏳ **14. Graphics** (أسبوعان) - Optional
8. ⏳ **16. Extension API** (أسبوع) - Optional

**المخرجات:**
- StdLib modules (io, string, math, time)
- Graphics subsystem (optional)
- Plugin system (optional)

---

## 🚀 البدء السريع / Quick Start

### 1. قراءة الخطط بالترتيب
```
09 → 10 → 11 → 12 → 13 → (14) → 15 → (16)
```

### 2. تنفيذ Sprint 1 (من 09_integration)
```powershell
cd C:\s\s_language

# إنشاء فرع
git checkout -b sprint1/integration-foundation

# بناء واختبار
.\scripts\build.ps1 -EnableTests -Clean
.\scripts\test.ps1
```

### 3. متابعة الـ TODO Lists (في كل ملف)
كل ملف خطة يحتوي على:
- ✅ Task breakdown تفصيلي
- ✅ Code examples
- ✅ Test cases
- ✅ Acceptance criteria

---

## 📖 الوثائق المرتبطة / Related Documentation

### Language Spec
- `docs/language_spec/00_INDEX.md` - Main index
- `docs/language_spec/01_types.md` - Type system
- `docs/language_spec/02_functions.md` - Functions
- `docs/language_spec/03_oop.md` - OOP
- `docs/language_spec/04_python_features.md` - Python features
- `docs/language_spec/05_cpp_features.md` - C++ features
- `docs/language_spec/06_syntax_examples.md` - Examples

### Master Plans
- `plans/00_MASTER_PLAN.md` - High-level roadmap
- `plans/02_PHASE_BREAKDOWN.md` - Phase details
- `plans/03_WORKFLOW_GUIDE.md` - Git workflow
- `plans/04_QUALITY_STANDARDS.md` - Code standards

---

## 🔧 أدوات التطوير / Development Tools

### Required
- **CMake** ≥3.20
- **Ninja** (build system)
- **Visual Studio 2022** (MSVC)
- **Git**

### Testing
- **Google Test** (GTest)
- **CTest** (CMake testing)

### Optional
- **vcpkg** (package manager)
- **SDL2** (graphics backend)
- **Doxygen** (documentation)
- **lcov** (code coverage)

---

## ⚠️ ملاحظات مهمة / Important Notes

### 1. الاعتماديات بين الخطط / Plan Dependencies
- **09** هو الأساس لكل شيء
- **10, 11, 12** يجب تنفيذهم بالترتيب
- **13** يعتمد على **12**
- **14, 16** optional (Phase 2 extended)
- **15** يمكن تنفيذه متوازياً

### 2. Bilingual Support (AR/EN)
- جميع الخطط ثنائية اللغة
- Code comments باللغة الإنجليزية
- User-facing messages باللغة العربية + الإنجليزية
- Debug logs بالإنجليزية

### 3. Testing Strategy
- Unit tests لكل module
- Integration tests بين modules
- Coverage target: 80-90%
- CI يجب أن يمر قبل merge

### 4. Code Quality
- RAII for memory management
- Smart pointers (unique_ptr, shared_ptr)
- const correctness
- Error handling (exceptions + error codes)

---

## 📞 الدعم / Support

### Issues
- GitHub Issues لتتبع المشاكل
- Labels: `bug`, `enhancement`, `documentation`, `testing`

### Discussion
- GitHub Discussions للأسئلة العامة
- Categories: `Q&A`, `Ideas`, `Show and Tell`

---

## 📝 سجل التغييرات / Changelog

### v1.0.0 - 4 نوفمبر 2025
- ✅ إنشاء 8 ملفات خطة تنفيذية
- ✅ تفاصيل كاملة لكل خطة (14 قسم)
- ✅ Code examples شاملة
- ✅ Test cases محددة
- ✅ PowerShell scripts
- ✅ GitHub Actions workflows
- ✅ جدول زمني مفصل (21 أسبوع)

---

**آخر تحديث:** 4 نوفمبر 2025  
**الحالة:** كامل وجاهز للتنفيذ  
**المطور:** Sad Language Team
