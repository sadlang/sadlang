# تفكيك مجلد `shared/` — النواة المشتركة للغة ص

> **تاريخ التحديث:** 29 أبريل 2026
> **الغرض:** تفكيك كل وحدة في `shared/`، تحديد المكتبات المُجمَّعة منها، والمستهلكين عبر المسارات الأربعة.

---

## 🏗️ الفكرة المعمارية

`shared/` يحتوي على **كل ما هو مشترك بين المفسر (interpreter)، الـ VM، والمترجم (sadc)**:
- لا يجوز لـ `interpreter/` أن يضم رأساً من `compiler/` والعكس
- لكن **كلاهما يحتاج** إلى: lexer, parser, AST, types, errors, modules, semantic
- لذلك تُوضع هذه المكونات في `shared/` ويُجمَّع منها مكتبتان رئيسيتان

---

## 📊 المكتبات المُولَّدة من `shared/`

| المكتبة | المصدر CMake | المحتوى |
|---|---|---|
| `sad_shared` | [shared/CMakeLists.txt:10](shared/CMakeLists.txt#L10) | Lexer + Parser + AST + Types + Errors + Modules + Utils + class_manager |
| `sad_semantic_shared` | [shared/semantic/CMakeLists.txt:24](shared/semantic/CMakeLists.txt#L24) | Type Checker (3 ملفات) — مكتبة فرعية |
| `sad_type_system` | (مكتبة فرعية لـ types) | نظام الأنواع المتقدم |
| `sad_profiler_lib` | [tools/profiler/CMakeLists.txt:20](tools/profiler/CMakeLists.txt#L20) | يستهلك `shared/profiler/include` |
| `sad_core` | [cmake/libraries.cmake:32](cmake/libraries.cmake#L32) | **يكرر معظم محتويات `sad_shared`** عبر `ALL_SOURCES` |

> ⚠️ **اكتشاف ازدواج معماري:** `sad_shared` و `sad_core` يحتويان على نفس ملفات Lexer/Parser/AST/Types/Errors/Modules — لكن `sad_core` يضيف فوقها interpreter + stdlib + low_level. هذا يضاعف وقت الترجمة.

---

## 📁 المجلدات الفرعية بالتفصيل

### 1️⃣ `shared/lexer/` — المحلل المعجمي

| الملف | الغرض |
|---|---|
| `src/token.cpp` | تعريف Token + Position + TokenType (40 keyword) |
| `src/lexer_core.cpp` | المحلل المعجمي الرئيسي — يقرأ مصدر `.ص` ويُنتج tokens |
| `src/lexer_keywords.cpp` | جدول الكلمات المحجوزة (40 فقط، تسجيل عربي ↔ TT::KEYWORD_*) |

**المُستهلكون:** sad, sadc, vm, sad-lsp, sad-fmt, sad-analizer, wasm — **جميع المسارات الأربعة**

**نقطة دخول:** `Sad::Lexer::LexerCore::tokenize(source)`

### 2️⃣ `shared/parser/` — المحلل النحوي

> **32 ملف cpp + 6 headers** — أكبر مكون فرعي في shared.

| المجلد الفرعي | الملفات | الغرض |
|---|---|---|
| `src/core/` | parser_core, parser_main, parser_expressions, parser_helpers | النواة + التعابير |
| `src/declarations/` | parser_declarations, parser_oop, parser_modules | تعريفات (دالة/صنف/استورد) |
| `src/statements/` | parser_statements, parser_advanced | جمل (إذا/بينما/طابق/...) |
| `src/ui/` | parser_ui | بناء جملة UI الإعلاني |

**المُستهلكون:** جميع المسارات

**نقطة دخول:** `Sad::Parser::ParserCore::parseProgram()`

### 3️⃣ `shared/ast/` — شجرة بناء الجملة المجردة

| الملف | الغرض |
|---|---|
| `ast_node.cpp` | Base class — `ASTNode` |
| `expressions.cpp` | BinaryOp, Call, Member, Literal, Variable, ... |
| `statements.cpp` | If, While, For, Return, Block, ... |
| `declarations.cpp` | FunctionDecl, VarDecl |
| `class_nodes.cpp` | OOP: ClassDecl, ThisExpr, SuperExpr, NewExpr |
| `module_nodes.cpp` | ImportDecl, ExportDecl |
| `property_nodes.cpp` | PropertyDecl (getter/setter) |
| `type_nodes.cpp` | TypeAnnotation, GenericParam |
| `ast_printer.cpp` | عرض AST كنص (للديباغ) |
| `ast_clone.cpp` | نسخ عميق للـ AST (للقوالب والماكروز) |
| `docs_extractor.h/cpp` | محرّك استخراج التوثيق (يُستخدم من sad و sadc) |

**المُستهلكون:** جميع المسارات

### 4️⃣ `shared/types/` — نظام الأنواع

| الملف | الغرض |
|---|---|
| `value.cpp` | `Value` — std::variant على ValueType (الأساس!) |
| `class_type.cpp` | تعريف الأصناف وقت التشغيل |
| `object_instance.cpp` | كائنات الأصناف (`shared_ptr<ObjectInstance>`) |
| `generator.cpp` | generators / yield |
| `sad_type_system.cpp` | نظام الأنواع الموحَّد (ADR-001) |
| `type_bridge.cpp` | جسر `SadType ↔ Value` (ADR-01 Phase 1) |

**المُستهلكون:** جميع المسارات

**ملاحظة هامة:** `Value` هو حجر الأساس — كل القيم في وقت التشغيل تمر عبره.

### 5️⃣ `shared/errors/` — إدارة الأخطاء (24 ملف!)

> أضخم نظام أخطاء — **Phase 15 Smart Errors**

| الفئة | الملفات | الغرض |
|---|---|---|
| **أساسي** | `error_codes`, `diagnostic`, `error_manager`, `error_hints` | البنية التحتية للأخطاء |
| **ذكي (Smart)** | `smart_errors`, `smart_suggestions`, `smart_type_explanations`, `smart_multi_error`, `smart_fix_suggestions` | اقتراحات ذكية + شرح الأنواع |
| **تعليمي** | `smart_teacher_mode` | وضع المعلم — شرح للمبتدئين |
| **تعافي** | `smart_error_recovery`, `smart_cascade_prevention` | منع الأخطاء المتسلسلة |
| **تشخيصات** | `pattern_diagnostics`, `permission_diagnostics` | أنماط + ملكية (نُقلت من compiler/ — Fix #26) |

**المُستهلكون:** جميع المسارات

### 6️⃣ `shared/semantic/` — فاحص الأنواع (مكتبة فرعية)

> **مكتبة منفصلة `sad_semantic_shared`** — نُقلت من `compiler/` في Phase 3 (F-01).

| الملف | الغرض |
|---|---|
| `type_checker.cpp` | الفاحص الرئيسي |
| `type_checker_expr_visitors.cpp` | فحص التعابير |
| `type_checker_stmt_visitors.cpp` | فحص الجمل |

**المُستهلكون:**
- `sad` (المفسر) — فحص قبل التنفيذ عبر `interpreter_core.cpp`
- `sadc` — جزء من خط الترجمة
- **ليس في WASM** ولا VM مباشرة

**سبب النقل:** كان المفسر يضم رأساً من `compiler/` (انتهاك F-01) — النقل أصلح هذا الانتهاك.

### 7️⃣ `shared/modules/` — نظام الوحدات

| الملف | الغرض |
|---|---|
| `module.cpp` | تعريف Module |
| `search_paths.cpp` | مسارات البحث (stdlib, working dir, ...) |
| `module_cache.cpp` | تخزين مؤقت للوحدات المُحمَّلة |
| `dependency_graph.cpp` | رسم بياني للتبعيات (لمنع التدوير) |
| `module_loader.cpp` | تحميل وحدة `.ص` من القرص |
| `module_resolver.cpp` | حل أسماء الوحدات إلى مسارات |
| `symbol_resolver.cpp` | حل الرموز المُصدَّرة (`صدّر`) |
| `module_validator.cpp` | التحقق من صحة الوحدات |

**المُستهلكون:** sad, sadc, vm — **ليس في WASM** (لا قرص)

**نقطة دخول:** عند مواجهة `استورد X` → ModuleResolver → ModuleLoader → ModuleCache

### 8️⃣ `shared/builtins/` — الدوال المضمنة

> ⚠️ **مهم:** هذا ليس stdlib — بل **runtime يُحمَّل تلقائياً**.

| الملف | الغرض |
|---|---|
| `runtime/builtins.cpp` | محرك تسجيل الدوال (BUILTINS_SOURCES) |
| `runtime/stdlib_manager.cpp` | مسجِّل الوحدات + lazy loading |
| `runtime/type_functions.cpp` | `رقم()`, `نص()`, `طول()`, `نوع()`, `منطقي()`, `عشري()` |
| `runtime/array_functions.cpp` | طرق المصفوفات (`اضف`, `احذف`, `رتب`, ...) |
| `runtime/other_functions.cpp` | باقي الدوال المضمنة |
| `include/` | السجل المركزي الموحَّد (ADR-003) |

**المُستهلكون:** جميع المسارات (المسار 4 جزئياً)

### 9️⃣ `shared/profiler/` — مصحح الأداء

| الملف | الغرض |
|---|---|
| `src/profiler_engine.cpp` | محرك القياس + التنميط |
| `include/` | API للـ profiler |

**المُستهلكون:**
- `sad_core` (عبر `target_link_libraries(sad_core PRIVATE sad_profiler_lib)`)
- `sad-profiler` (المنفذ المستقل)

**ليس في WASM** (لا أداء قياس).

### 🔟 `shared/hot_reload/` — إعادة التحميل الساخن

| الملف | الغرض |
|---|---|
| `file_watcher.cpp` | مراقبة تغييرات الملفات |
| `hot_reload_engine.cpp` | إعادة تحميل الوحدات أثناء التشغيل |

**المُستهلكون:** `sad_core` فقط (مدمج عبر `target_include_directories`)

**ليس في WASM** ولا sadc.

### 1️⃣1️⃣ `shared/utils/` — أدوات مساعدة

| الملف | الغرض |
|---|---|
| `string_utils.cpp` | تحويلات نصية، تشذيب، فصل، ... |

**المُستهلكون:** جميع المسارات

---

## 🔍 خارطة الاستهلاك حسب المسار

| المكون | sad (P1) | --vm (P2) | sadc (P3) | wasm (P4) |
|---|---|---|---|---|
| `lexer` | ✅ | ✅ | ✅ | ✅ |
| `parser` | ✅ | ✅ | ✅ | ✅ |
| `ast` | ✅ | ✅ | ✅ | ✅ |
| `types` | ✅ | ✅ | ✅ | ✅ |
| `errors` (أساسي) | ✅ | ✅ | ✅ | ✅ |
| `errors` (smart/teacher) | ✅ | ✅ | ✅ | ⚠️ جزئي |
| `semantic` (sad_semantic_shared) | ✅ | ❌ | ✅ | ❌ |
| `modules` | ✅ | ✅ | ✅ | ❌ (لا قرص) |
| `builtins/runtime` | ✅ | ✅ | ✅ | ✅ |
| `profiler` | ✅ (PRIVATE) | ❌ | ❌ | ❌ |
| `hot_reload` | ✅ | ❌ | ❌ | ❌ |
| `utils` | ✅ | ✅ | ✅ | ✅ |

---

## 🚨 اكتشافات معمارية مهمة

### الاكتشاف 1: ازدواج هائل بين `sad_shared` و `sad_core`

`sad_shared` يحتوي على:
- Lexer, Parser, AST, Types, Errors, Modules, Utils, class_manager

`sad_core` (عبر `ALL_SOURCES` في sources.cmake) يحتوي على **نفس هذه الملفات** + interpreter + stdlib + low_level.

**النتيجة:**
- نفس الملفات تُترجم مرتين (في `sad_shared` و `sad_core`)
- زيادة وقت البناء بشكل كبير
- زيادة حجم الملف التنفيذي

**التحسين المقترح:**
- اجعل `sad_core` يربط `sad_shared` كـ PUBLIC dependency
- أزل ملفات `sad_shared` من `ALL_SOURCES`

### الاكتشاف 2: `sad_shared` يضم ملفاً من `interpreter/`

```cmake
# OOP Managers (مطلوبة للمترجم sadc)
${CMAKE_SOURCE_DIR}/interpreter/src/managers/class_manager.cpp
```

**المشكلة:** هذا انتهاك معماري — `shared/` يجب ألا يضم من `interpreter/`.

**الحل المقترح:** نقل `class_manager.cpp` إلى `shared/types/` أو `shared/managers/`.

### الاكتشاف 3: `shared/semantic/` نقل ناجح (Phase 3 / F-01)

كان `type_checker` في `compiler/`، والمفسر يضمه — **انتهاك معماري واضح**.
الحل: نقل إلى `shared/semantic/` كمكتبة منفصلة `sad_semantic_shared`.
الآن: المفسر و sadc كلاهما يربط `sad_semantic_shared` بدون انتهاك.

**نموذج جيد** يمكن تعميمه على `class_manager.cpp` أيضاً.

### الاكتشاف 4: `shared/profiler/` معزول

`sad_profiler_lib` مستقل في `tools/profiler/` لكن headerه في `shared/profiler/include/`. هذا تنظيم غريب — يجب توحيده إما في `shared/` أو في `tools/`.

### الاكتشاف 5: `errors/` ضخم جداً (24 ملف)

نظام Phase 15 Smart Errors نمو عضوي. يستحق:
- توثيق منفصل
- ربما تقسيم إلى مكتبتين: `sad_errors_basic` (لـ WASM) + `sad_errors_smart` (للسطح المكتبي)

### الاكتشاف 6: `hot_reload` غير مذكور في WASM ولا sadc

موجود فقط في `sad_core` كتضمين. هذا منطقي — لكن يجب توثيق أنه ميزة sad فقط.

---

## 📋 ملخص تنفيذي

| المكون الفرعي | عدد cpp | عدد headers | المكتبة الرئيسية |
|---|---|---|---|
| ast | 11 | 20 | sad_shared + sad_core |
| builtins | 5 | 8 | sad_core (BUILTINS_SOURCES) |
| errors | 24 | 17 | sad_shared + sad_core |
| hot_reload | 2 | 2 | sad_core فقط |
| lexer | 5 | 3 | sad_shared + sad_core |
| modules | 8 | 8 | sad_shared + sad_core |
| parser | 32 | 6 | sad_shared + sad_core |
| profiler | 1 | 2 | sad_profiler_lib |
| semantic | 3 | 1 | sad_semantic_shared (منفصل) |
| types | 6 | 12 | sad_shared + sad_core |
| utils | 1 | 2 | sad_shared + sad_core |
| **المجموع** | **98** | **81** | — |

---

## 🔗 المراجع

- [docs/architecture-cli-features.md](docs/architecture-cli-features.md)
- [docs/architecture-tools-breakdown.md](docs/architecture-tools-breakdown.md)
- [docs/architecture-stdlib-breakdown.md](docs/architecture-stdlib-breakdown.md)
- [docs/project-overview.md](docs/project-overview.md)
- [shared/CMakeLists.txt](shared/CMakeLists.txt)
- [shared/semantic/CMakeLists.txt](shared/semantic/CMakeLists.txt)
- [cmake/sources.cmake](cmake/sources.cmake)
- [cmake/libraries.cmake](cmake/libraries.cmake)
