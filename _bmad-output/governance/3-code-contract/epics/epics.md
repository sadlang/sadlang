---
stepsCompleted: [step-01-validate, step-02-design-epics, step-03-stories]
inputDocuments:
  - _bmad-output/planning-artifacts/prd.md
  - _bmad-output/governance/3-code-contract/contract-as-code-plan.md
  - .github/copilot-instructions.md
---

# s-programming-language — تفصيل الملاحم والقصص
## نظام Contract-as-Code

## مخزون المتطلبات

### المتطلبات الوظيفية (FRs)

FR-01: إضافة `cppcoreguidelines-pro-type-reinterpret-cast` إلى `.clang-tidy` لاكتشاف CS-04 عبر IDE فوراً  
FR-02: إنشاء `compiler/include/shared/sad_invariant.h` بالماكرو الثلاثي (DEF + CHECK + runtime assertion)  
FR-03: تطبيق ثابت `ARABIC_STRING_UTF8_NORMALIZED` على `arabic_string_pool.cpp`  
FR-04: تطبيق ثابت `SIR_SCOPE_STACK_BALANCED` على `compiler/src/sir/sir_builder.cpp`  
FR-05: تطبيق ثابت `LEXER_TOKEN_POSITION_ONE_BASED` على `shared/lexer/src/lexer_core.cpp`  
FR-06: تطبيق ثابت `PARSER_SCOPE_STACK_BALANCED` على `shared/parser/src/parser_core.cpp`  
FR-07: تطبيق ثابت `INTERPRETER_SCOPE_NO_LEAK` على `interpreter/src/core/interpreter_core.cpp`  
FR-08: تطبيق ثابت `GOROUTINE_SNAPSHOT_IMMUTABLE` على `interpreter/src/channel.cpp`  
FR-09: إنشاء `scripts/lint/check_invariants.py` يتحقق أن كل CHECK له DEF والـ critical مُحمي  
FR-10: إصلاح مخالفة CS-01 الموجودة في `interpreter/src/visitors/expression_evaluator_ui.cpp`  
FR-11: إنشاء `scripts/lint/code_standards.yaml` بقواعد CS-01/02/03/05  
FR-12: إنشاء `scripts/lint/check_code_standards.py` مع منطق CS-02 المُصحَّح  
FR-13: إنشاء `scripts/lint/code_standards_allowlist.yaml` مع سياسة expiry إلزامية  
FR-14: إنشاء `cmake/code_standards_guard.cmake` للـ configure-time enforcement  
FR-15: ربط الحارس في `CMakeLists.txt`  
FR-16: كتابة `scripts/codegen/test_code_standards.py` (≥15 test لكل قاعدة، fixtures معزولة)  
FR-17: كتابة `scripts/codegen/test_invariants.py`  

### المتطلبات غير الوظيفية (NFRs)

NFR-01: configure overhead < 3 ثواني على الكود الكامل  
NFR-02: صفر false positives في CS-02 (التحقق المزدوج للكلمات السياقية مسموح)  
NFR-03: كل test يستخدم fixtures معزولة — لا ملفات المشروع الحقيقية  
NFR-04: Python 3.8+ مع requirements.txt مثبَّت الإصدار  
NFR-05: `SAD_SKIP_STANDARDS_GUARD` يُسجَّل في log ويُنتج WARNING دائم في cmake --build  
NFR-06: `SAD_LINT_DISABLE` متاح فقط لـ CS-04/05 مع سبب مكتوب — لا تعطيل لـ CS-01/02/03  

### المتطلبات التقنية الإضافية

- التحقق من اكتمال `UINodeType` enum قبل تفعيل CS-01 guard
- إصلاح المخالفة الحالية قبل تفعيل الحارس (ترتيب حرج)
- LLVM 18.1.8 موجود في `C:\llvm_dev\LLVM\bin\clang-tidy.exe`
- `.clang-tidy` موجود ومُهيَّأ مسبقاً

### متطلبات UX Design

لا تنطبق — هذا نظام أدوات داخلي (بلا واجهة مستخدم بصرية).  
الـ "UX" هنا هو تجربة المطور: رسائل خطأ واضحة + feedback فوري.

### خارطة تغطية المتطلبات

| FR | الملحمة | القصة |
|---|---|---|
| FR-01 | Epic 1 | 1.1 |
| FR-02, 03, 04, 05, 06, 07, 08 | Epic 2 | 2.1 — 2.7 |
| FR-09 | Epic 2 | 2.8 |
| FR-10 | Epic 1 | 1.2 (قبل الحارس) |
| FR-11, 12, 13 | Epic 3 | 3.1, 3.2, 3.3 |
| FR-14, 15 | Epic 3 | 3.4 |
| FR-16, 17 | Epic 4 | 4.1, 4.2 |

---

## قائمة الملاحم

- **Epic 1:** الأساس — Clang-Tidy + إصلاح المخالفة الموجودة
- **Epic 2:** SAD_INVARIANT — ثوابت معمارية في جميع طبقات اللغة
- **Epic 3:** Contract-as-Code — YAML + Python + CMake Guard
- **Epic 4:** اختبار وتحقق الحارس

---

## Epic 1: الأساس — Clang-Tidy وإصلاح المخالفات الموجودة

**الهدف:** تفعيل الطبقة 0 (IDE feedback) وتنظيف المخالفات الحالية قبل تفعيل الحارس الصارم.

---

### Story 1.1: تحسين .clang-tidy لـ CS-04

كمطور يعمل على مشروع لغة ص،  
أريد أن يُحذِّرني IDE فوراً عند استخدام `reinterpret_cast` خارج حدود FFI،  
حتى أُصحح المخالفة في اللحظة التي أكتب فيها الكود.

**قبول الشرط:**

**Given** المطور مثبَّت لديه VS Code مع clangd extension  
**When** يكتب `reinterpret_cast<int*>(ptr)` خارج مجلدات `ffi/`, `runtime_new/`, `vm/abi/`  
**Then** يظهر underline أحمر/أصفر فوري مع رسالة: "استخدم std::bit_cast أو تحويل آمن"  
**And** لا يظهر تحذير داخل مجلدات FFI المُستثناة

**Given** المطور يُشغّل cmake -B build  
**When** `.clang-tidy` يحتوي `cppcoreguidelines-pro-type-reinterpret-cast`  
**Then** configure ينجح بدون أخطاء إضافية

**ملاحظات تقنية:**
- عدّل `.clang-tidy` بإضافة `cppcoreguidelines-pro-type-reinterpret-cast` في قسم `Checks:`
- أضف `HeaderFilterRegex` يشمل `ffi/` للسماح له فقط هناك
- اختبر مع `clangd --check=file.cpp`

---

### Story 1.2: التحقق من UINodeType enum وإصلاح expression_evaluator_ui.cpp

كمطور في فريق لغة ص،  
أريد إزالة مقارنات النصوص العربية المباشرة في expression_evaluator_ui.cpp،  
حتى نُعدّ الكود للحارس الجديد ونُنهي المخالفة الموثّقة.

**قبول الشرط:**

**Given** ملف `interpreter/src/visitors/expression_evaluator_ui.cpp` السطر 88  
**When** نفحصه  
**Then** نجد: `if (node.widgetName == "\xd9\x86\xd8\xb5" || node.widgetName == "Text")`

**Given** `sad::ui::UINodeType` enum موجود ومكتمل  
**When** نُطبّق الإصلاح  
**Then** يصبح: `if (arabicNameToNodeType(node.widgetName) == UINodeType::TextNode || ...)`  
**And** الكود يُبنى بنجاح بدون أخطاء  
**And** الاختبارات الموجودة لا تتراجع

**Given** الملف الذي يحتوي المخالفة CS-01  
**When** نُشغّل الحارس لاحقاً  
**Then** لا يُدرج هذا الملف في allowlist (تمت إزالة المخالفة)

**ملاحظات تقنية:**
- تحقق من `UINodeType` في `interpreter/include/` أو `shared/`
- إذا كان ناقصاً، أضف القيم المطلوبة قبل الإصلاح
- استخدم `sad::ui::arabicNameToNodeType(str)` للتحويل

---

## Epic 2: SAD_INVARIANT — ثوابت معمارية في جميع طبقات اللغة

**الهدف:** تضمين نية التصميم في الكود نفسه بالقرب من موقع القرار لمنع وكلاء AI من كسر الثوابت الحرجة.

---

### Story 2.1: إنشاء sad_invariant.h والثابت الأكثر خطورة

كمطور يكتب كوداً في arabic_string_pool،  
أريد أن يُجبرني الكود على التحقق من NFC normalization قبل إضافة النص للبركة،  
حتى لا تحدث مقارنات عربية خاطئة صامتة في وقت التشغيل.

**قبول الشرط:**

**Given** الملف `compiler/include/shared/sad_invariant.h` غير موجود  
**When** ننشئه  
**Then** يحتوي الماكروز الثلاثة:
- `SAD_INVARIANT_DEF(id, why, critical)` — no-op في runtime
- `SAD_INVARIANT_CHECK(id)` — no-op في runtime
- `SAD_INVARIANT(id, condition, msg)` — يستدعي `Sad::invariantViolated()` عند كسر الشرط

**Given** `arabic_string_pool.cpp` يُضيف نصاً عربياً غير NFC  
**When** يستدعي `pool.intern(str)`  
**Then** يتوقف البرنامج مع رسالة واضحة: `[ARABIC_STRING_UTF8_NORMALIZED] نص غير NFC في arabic_string_pool.cpp:L`

**Given** النص مُطبَّع NFC بشكل صحيح  
**When** يستدعي `pool.intern(str)`  
**Then** يعمل بشكل طبيعي بدون أي تأثير على الأداء

**ملاحظات تقنية:**
```cpp
// في compiler/include/shared/sad_invariant.h
#pragma once
namespace Sad {
    [[noreturn]] void invariantViolated(const char* id, const char* msg, const char* file, int line);
}
#define SAD_INVARIANT_DEF(id, why, critical)
#define SAD_INVARIANT_CHECK(id)
#ifdef NDEBUG
    #define SAD_INVARIANT(id, cond, msg)  // no-op in release
#else
    #define SAD_INVARIANT(id, cond, msg) \
        do { if (!(cond)) ::Sad::invariantViolated(#id, msg, __FILE__, __LINE__); } while(0)
#endif
```

---

### Story 2.2: ثابت SIR_SCOPE_STACK_BALANCED في sir_builder.cpp

كمطور يعدّل SirBuilder،  
أريد أن يُجبرني الكود على الحفاظ على توازن النطاقات (pushScope/popScope)،  
حتى لا أُنتج LLVM IR مكسوراً بصمت.

**قبول الشرط:**

**Given** `SirBuilder::pushScope()` مُستدعى  
**Then** يزيد عداد النطاقات + يتحقق أنه لم يتجاوز `MAX_NESTING_DEPTH`

**Given** `SirBuilder::popScope()` مُستدعى على stack فارغ  
**Then** `SAD_INVARIANT` يوقف التنفيذ مع رسالة: `[SIR_SCOPE_STACK_BALANCED] popScope على stack فارغ`

**Given** الملف `compiler/src/sir/sir_builder.cpp`  
**When** يُفحص  
**Then** يحتوي:
```cpp
// SAD_INVARIANT_DEF(SIR_SCOPE_STACK_BALANCED,
//   "كل pushScope يجب أن يقابله popScope — كسره = IR مكسور",
//   critical=true)
```

---

### Story 2.3: ثابت LEXER_TOKEN_POSITION_ONE_BASED في lexer_core.cpp

كمطور يعدّل اللexer،  
أريد أن يُجبرني الكود على الحفاظ على أن Position يبدأ من 1،  
حتى لا تُنتج رسائل خطأ بمواضع خاطئة (سطر 0، عمود 0).

**قبول الشرط:**

**Given** Lexer يُنشئ token جديد  
**When** يُعيّن Position  
**Then** `SAD_INVARIANT` يتحقق أن `line >= 1 && column >= 1`

**Given** أي كود يُغيّر منطق Position  
**When** يُشغّل cmake -B build ثم يعمل البرنامج  
**Then** أي خرق يُوقف التنفيذ بـ `[LEXER_TOKEN_POSITION_ONE_BASED]`

---

### Story 2.4: ثابت PARSER_SCOPE_STACK_BALANCED في parser_core.cpp

كمطور يعدّل Parser،  
أريد أن يتحقق الكود من توازن `enterScope`/`exitScope` تلقائياً،  
حتى لا ينتج AST بنطاقات مكسورة.

**قبول الشرط:**

**Given** `enterScope()` مُستدعى N مرة  
**When** `exitScope()` مُستدعى N مرة  
**Then** النطاقات متوازنة — لا assertion

**Given** `exitScope()` مُستدعى على stack فارغ  
**Then** `[PARSER_SCOPE_STACK_BALANCED]` يوقف التنفيذ فوراً

---

### Story 2.5: ثابت INTERPRETER_SCOPE_NO_LEAK في interpreter_core.cpp

كمطور يعدّل InterpreterCore،  
أريد أن يتحقق الكود من عدم تسرب متغيرات بين النطاقات،  
حتى لا يرى الكود المتغيرات المحلية خارج نطاقها.

**قبول الشرط:**

**Given** دالة تُنفَّذ وتنتهي  
**When** ينتهي نطاقها  
**Then** `SAD_INVARIANT` يتحقق أن stack المتغيرات عاد لحجمه قبل الدالة

**Given** `ScopeManager::popScope()` مُستدعى  
**Then** عدد المتغيرات المرئية يتناقص (لا يبقى ثابتاً)

---

### Story 2.6: ثابت GOROUTINE_SNAPSHOT_IMMUTABLE في channel.cpp

كمطور يعدّل نظام Goroutines،  
أريد أن يتحقق الكود من أن snapshot المتغيرات عند `أطلق` لا يتغير بعد التقاطه،  
حتى لا يحدث race condition صامت.

**قبول الشرط:**

**Given** `captureVisibleVariables()` يلتقط snapshot  
**When** يُمرَّر لـ goroutine  
**Then** `SAD_INVARIANT` يتحقق أن snapshot لا يُشارك pointer مع scope الأصل

**Given** goroutine يُعدّل متغيراً داخلياً  
**Then** scope الأصل لا يرى التغيير

---

### Story 2.7: ترقيم وتوثيق جميع الثوابت في مكان مركزي

كمطور جديد في المشروع،  
أريد قائمة مركزية بجميع الثوابت المعمارية في المشروع،  
حتى أفهم الثوابت الحرجة قبل تعديل أي مكون.

**قبول الشرط:**

**Given** ملف `compiler/include/shared/sad_invariant.h`  
**When** أفتحه  
**Then** يحتوي تعليقاً في البداية يُدرج جميع الثوابت المُعرَّفة: `id` + `ملف` + `critical`

**Given** ثابت جديد يُضاف  
**Then** يُضاف للقائمة المركزية في نفس الـ PR

---

### Story 2.8: check_invariants.py — linter الثوابت

كمطور يُضيف ثابتاً جديداً،  
أريد أن يُحذِّرني الـ linter إذا أضفت `SAD_INVARIANT_CHECK` بدون `SAD_INVARIANT_DEF` مقابل،  
حتى لا تكون الثوابت معزولة وغير موثقة.

**قبول الشرط:**

**Given** ملف يحتوي `SAD_INVARIANT_CHECK(FOO_INVARIANT)` بدون DEF مقابل  
**When** يُشغَّل `check_invariants.py`  
**Then** يفشل بـ exit code 1 مع: `INVARIANT_ORPHAN: FOO_INVARIANT مُستخدم بدون تعريف`

**Given** ثابت بـ `critical=true` بدون أي CHECK  
**When** يُشغَّل `check_invariants.py`  
**Then** يفشل: `INVARIANT_UNCHECKED: X معرَّف لكن لا يُتحقق منه`

**Given** جميع الثوابت مُعرَّفة ومُستخدمة بشكل صحيح  
**When** يُشغَّل `check_invariants.py`  
**Then** يخرج بـ exit code 0 مع: "✅ جميع الثوابت المعمارية متوافقة"

---

## Epic 3: Contract-as-Code — YAML + Python + CMake Guard

**الهدف:** بناء الطبقة 1-3 — نظام الفحص الكامل بـ configure-time enforcement.

---

### Story 3.1: إنشاء code_standards.yaml — مصدر الحقيقة

كمطور يريد إضافة قاعدة جديدة لمعايير الكود،  
أريد ملف YAML واحد يصف جميع القواعد،  
حتى تكون إضافة قاعدة جديدة بسيطة وبدون تعديل Python أو CMake.

**قبول الشرط:**

**Given** ملف `scripts/lint/code_standards.yaml` جديد  
**When** يُفحص  
**Then** يحتوي على الأقل CS-01/02/03/05 بالحقول: `id`, `description`, `level`, `fix`

**Given** قاعدة CS-02  
**When** تُفحص  
**Then** تحتوي: `reserved_words_source` و`allowed_pattern` (للتحقق المزدوج)

**Given** مطور يضيف قاعدة جديدة بسطر YAML  
**Then** الـ ENFORCER يفحصها تلقائياً في الـ run التالي

---

### Story 3.2: إنشاء check_code_standards.py — المُنفِّذ

كمطور يُشغّل cmake -B build،  
أريد أن يفحص Python script كل ملف `.cpp`/`.h` وفق قواعد YAML،  
حتى تُكتشف المخالفات قبل أن يبدأ البناء.

**قبول الشرط:**

**Given** ملف `.cpp` جديد يحتوي `if (name == "\xd9\x86\xd8\xb5")`  
**When** يُشغَّل `check_code_standards.py`  
**Then** يخرج بـ exit code 1 ورسالة:
```
[CS-01] path/to/file.cpp:15
المشكلة: مقارنة نص عربي مباشر
الحل   : استخدم UINodeType enum
```

**Given** ملف في allowlist مع expiry مستقبلي صالح  
**When** يُشغَّل الـ script  
**Then** يتجاهل الملف ويستمر

**Given** ملف في allowlist مع expiry منتهٍ  
**When** يُشغَّل الـ script  
**Then** يفشل: `ALLOWLIST_EXPIRED: path/to/file.cpp — انتهت صلاحية الاستثناء 2026-05-01`

**Given** كود يستخدم نمط التحقق المزدوج الصحيح:
`check(TT::KEYWORD_TRAIT) || (check(TT::IDENTIFIER) && current_.getValue() == "سمة")`  
**When** يُشغَّل فحص CS-02  
**Then** لا يُثير مخالفة (false positive صفر)

**Given** كود يستخدم `token.getValue() == "إذا"` مباشرة (كلمة محجوزة)  
**When** يُشغَّل فحص CS-02  
**Then** يُثير مخالفة FATAL

---

### Story 3.3: إنشاء code_standards_allowlist.yaml بسياسة الحذف

كمطور يريد استثناء ملف مخالف مؤقتاً،  
أريد allowlist تُلزمني بتحديد تاريخ انتهاء ومسؤول وسبب،  
حتى لا تتراكم الاستثناءات إلى الأبد.

**قبول الشرط:**

**Given** إدخال في allowlist بدون حقل `expiry`  
**When** يُشغَّل الـ ENFORCER  
**Then** يفشل: `ALLOWLIST_MISSING_EXPIRY: يجب تحديد تاريخ انتهاء`

**Given** إدخال بدون حقل `reason`  
**When** يُشغَّل الـ ENFORCER  
**Then** يفشل: `ALLOWLIST_MISSING_REASON`

**Given** إدخال صالح مع expiry بعد 14 يوم فقط  
**When** يُشغَّل الـ ENFORCER  
**Then** يُنتج WARNING (ليس ERROR): `⚠️ اقتراب انتهاء: file.cpp يُستثنى حتى YYYY-MM-DD`

**Given** إدخال صالح مع expiry بعيد  
**When** يُشغَّل الـ ENFORCER  
**Then** يتجاوز الملف بصمت

---

### Story 3.4: إنشاء code_standards_guard.cmake وربطه

كمطور يُشغّل cmake -B build على مشروع يحتوي مخالفة جديدة،  
أريد أن يفشل cmake فوراً بتقرير واضح،  
حتى لا يبدأ البناء أصلاً.

**قبول الشرط:**

**Given** ملف جديد يحتوي مخالفة CS-03 (`KW_PRINT`)  
**When** يُشغَّل `cmake -B build`  
**Then** يفشل بـ FATAL_ERROR:
```
CMake Error at cmake/code_standards_guard.cmake:42 (message):
[CS-03] new_file.cpp:5 — استخدم KEYWORD_PRINT بدل KW_PRINT
للتعطيل المؤقت: cmake -DSAD_SKIP_STANDARDS_GUARD=ON -B build
```

**Given** لا توجد مخالفات  
**When** يُشغَّل `cmake -B build`  
**Then** ينجح مع: `✅ معايير الكود: لا مخالفات — N ملف مفحوص`

**Given** يُشغَّل cmake بـ `-DSAD_SKIP_STANDARDS_GUARD=ON`  
**When** يُشغَّل `cmake --build build`  
**Then** يُنتج WARNING في كل build: `⚠️ حارس معايير الكود مُعطَّل`  
**And** يُكتب إلى `build/standards_guard_bypassed.log`

**Given** `CMakeLists.txt`  
**When** يُفحص  
**Then** يحتوي في نهايته:
```cmake
include(cmake/code_standards_guard.cmake)
sad_check_code_standards()
```

---

## Epic 4: الاختبار والتحقق

**الهدف:** ضمان موثوقية الحارس نفسه — من يختبر الحارس؟

---

### Story 4.1: test_code_standards.py — اختبارات ENFORCER بـ fixtures معزولة

كمطور يُعدّل regex أو منطق في ENFORCER،  
أريد test suite مستقلاً تماماً عن ملفات المشروع الحقيقية،  
حتى لا تنكسر الاختبارات بسبب تغييرات غير ذات صلة.

**قبول الشرط:**

**Given** اختبارات CS-01  
**When** تُشغَّل  
**Then** تستخدم fixtures مؤقتة (`tmp_path` في pytest) وليس ملفات المشروع

**Given** اختبار CS-02 — النمط المسموح (تحقق مزدوج)  
**When** يُشغَّل  
**Then** لا يُثير مخالفة (يختبر false positive = صفر)

**Given** اختبار CS-02 — النمط المحظور (getValue مع كلمة محجوزة)  
**When** يُشغَّل  
**Then** يُثير مخالفة FATAL

**Given** اختبارات الـ allowlist  
**Then** تشمل:
- إدخال صالح → لا خطأ
- إدخال بدون expiry → FAIL
- إدخال منتهٍ (باستخدام `freezegun`) → FAIL
- إدخال قريب الانتهاء < 14 يوم → WARNING

**Given** جميع الاختبارات  
**When** تُشغَّل  
**Then** تكتمل في < 10 ثواني (لأنها fixtures بسيطة)

**ملاحظات تقنية:**
- استخدم `pytest` + `tmp_path` fixture
- استخدم `freezegun` لاختبار الـ expiry dates
- ≥15 test case لكل قاعدة من CS-01/02/03/05
- اختبر: ملف فارغ، ملف تعليقات فقط، ملف حجمه 799 و801 سطر

---

### Story 4.2: test_invariants.py — اختبارات invariant linter

كمطور يعتمد على check_invariants.py،  
أريد أن أثق أن الـ linter يكتشف الثوابت المعزولة والحرجة غير المحمية،  
حتى لا يكذب الـ linter بصمت.

**قبول الشرط:**

**Given** كود يحتوي `SAD_INVARIANT_CHECK(FOO)` بدون `SAD_INVARIANT_DEF(FOO, ...)`  
**When** يُشغَّل `check_invariants.py`  
**Then** يفشل: `INVARIANT_ORPHAN: FOO`

**Given** `SAD_INVARIANT_DEF(BAR, ..., critical=true)` بدون أي CHECK  
**When** يُشغَّل `check_invariants.py`  
**Then** يفشل: `INVARIANT_UNCHECKED: BAR`

**Given** كود نظيف مع DEF + CHECK متطابقة  
**When** يُشغَّل  
**Then** exit code 0

**Given** ملف فارغ أو بدون invariants  
**When** يُشغَّل  
**Then** exit code 0 (لا إنذارات كاذبة)

---

## ملاحظات التنفيذ

### ترتيب التنفيذ الحرج

```
Story 1.1 → Story 1.2 → Story 2.1 → Stories 2.2-2.8 (يمكن بالتوازي)
→ Story 3.1 → Story 3.2 → Story 3.3 → Story 3.4
→ Stories 4.1, 4.2 (يمكن بالتوازي مع 3.x)
```

**القاعدة الحرجة:** Story 1.2 (إصلاح المخالفة) يجب أن يكتمل **قبل** Story 3.4 (تفعيل الحارس).

### تعريف "مكتمل" (Definition of Done)

- الكود يُبنى بدون أخطاء
- جميع ACs محققة
- الاختبارات الموجودة لا تتراجع
- لا مخالفات CS-01/02/03/05 في الكود الجديد
- التعليقات بالعربية وفق معايير المشروع

---

_وثيقة الملاحم والقصص — نظام Contract-as-Code_
_مُستخرجة من PRD + حفلة الوكلاء (Winston، Dr. Quinn، Murat)_
_التاريخ: 2026-05-17_
