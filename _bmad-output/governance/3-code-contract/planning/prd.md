---
stepsCompleted: [step-01-init, step-02-discovery, step-03-goals, step-04-requirements, step-05-stories]
inputDocuments:
  - _bmad-output/governance/3-code-contract/contract-as-code-plan.md
  - .github/copilot-instructions.md
  - docs/architecture-cli-features.md
  - docs/بنية_المشروع/نظام_الأنواع.md
  - docs/بنية_المشروع/نظام_الأخطاء.md
workflowType: prd
projectType: brownfield
---

# وثيقة متطلبات المنتج (PRD)
## نظام Contract-as-Code — إلزام معايير الكود في لغة ص

**المؤلف:** صالح
**التاريخ:** 2026-05-17
**المشروع:** s-programming-language
**الإصدار:** 1.0

---

## 1. الملخص التنفيذي

### المشكلة

وكلاء الذكاء الاصطناعي والمطورون يخرقون معايير الكود لأنها موجودة في وثائق نصية فقط وغير مُفرَّضة آلياً. الكود المخالف يُبنى ويعمل دون أي عقوبة تقنية.

**مثال فعلي:**
```cpp
// interpreter/src/visitors/expression_evaluator_ui.cpp:88
if (node.widgetName == "\xd9\x86\xd8\xb5" || node.widgetName == "Text")
// يجب: sad::ui::arabicNameToNodeType() ثم مقارنة UINodeType enum
```

### الحل — نظام هجين بثلاث طبقات

| الطبقة | الأداة | الوقت | تغطي |
|---|---|---|---|
| 0 | Clang-Tidy محسَّن | كتابة الكود (IDE) | CS-04: reinterpret_cast |
| 0.5 | SAD_INVARIANT | وقت التشغيل | 6 ثوابت معمارية حرجة |
| 1-3 | Python/YAML + CMake | configure-time | CS-01/02/03/05 |

---

## 2. سياق المشروع (Brownfield)

### الأدوات الموجودة (تُبنى عليها)

- `cmake/orphan_sources_guard.cmake` — نمط ناجح للـ configure-time guard
- `.clang-tidy` — مُهيَّأ مع LLVM 18.1.8
- `scripts/codegen/test_migration_guard.py` — سابقة لـ lint tests

### البنية ذات الصلة

```
shared/lexer/        ← Lexer (40 كلمة محجوزة + سياقية)
shared/parser/       ← Parser + AST
interpreter/         ← InterpreterCore + goroutines + channels
compiler/src/backend/llvm/arabic_string_pool.cpp  ← أخطر نقطة
compiler/src/sir/sir_builder.cpp                  ← نقطة حرجة
cmake/               ← CMake modules
scripts/codegen/     ← Python codegen + tests
```

---

## 3. الأهداف ومعايير النجاح

| الهدف | المقياس |
|---|---|
| منع الخرق الصامت | cmake يفشل عند مخالفة جديدة — 100% |
| feedback واضح | ملف + سطر + قاعدة + حل في رسالة الخطأ |
| توسّع بلا تعقيد | إضافة قاعدة = سطر YAML واحد |
| ترحيل تدريجي | allowlist مؤقتة مع تاريخ انتهاء |
| توثيق نية التصميم | SAD_INVARIANT في موقع القرار |
| Configure overhead | < 3 ثواني |
| Test coverage للـ ENFORCER | ≥ 15 test case لكل قاعدة |

---

## 4. المتطلبات الوظيفية

### FR-01: تحسين Clang-Tidy (CS-04)

إضافة `cppcoreguidelines-pro-type-reinterpret-cast` إلى `.clang-tidy`.  
الاستثناء: مجلدات `ffi/`, `runtime_new/`, `vm/abi/`.  
يعمل عبر clangd في VS Code — لحظي بدون cmake overhead.

---

### FR-02: SAD_INVARIANT — ثوابت معمارية مُضمَّنة

**الملف الجديد:** `compiler/include/shared/sad_invariant.h`

```cpp
// الماكرو الثلاثي
#define SAD_INVARIANT_DEF(id, why, critical)   // توثيق في موقع الثابت (no-op runtime)
#define SAD_INVARIANT_CHECK(id)                // إعلان اعتماد (no-op runtime)
#define SAD_INVARIANT(id, condition, msg) \
    do { if (!(condition)) ::Sad::invariantViolated(id, msg, __FILE__, __LINE__); } while(0)
```

**الثوابت الستة المطلوبة:**

| id | الملف المستهدف | السبب | خطر الفشل الصامت |
|---|---|---|---|
| `LEXER_TOKEN_POSITION_ONE_BASED` | `shared/lexer/src/lexer_core.cpp` | Position من 1 — كسره يُفسد رسائل الأخطاء | عالٍ |
| `PARSER_SCOPE_STACK_BALANCED` | `shared/parser/src/parser_core.cpp` | enterScope يقابله exitScope | عالٍ |
| `INTERPRETER_SCOPE_NO_LEAK` | `interpreter/src/core/interpreter_core.cpp` | تسرب متغيرات بين النطاقات | متوسط |
| `GOROUTINE_SNAPSHOT_IMMUTABLE` | `interpreter/src/channel.cpp` | snapshot عند `أطلق` لا يتغير | متوسط |
| `SIR_SCOPE_STACK_BALANCED` | `compiler/src/sir/sir_builder.cpp` | pushScope يقابله popScope | عالٍ |
| `ARABIC_STRING_UTF8_NORMALIZED` | `compiler/src/backend/llvm/arabic_string_pool.cpp` | NFC — مقارنات `"نص"=="نص"` خاطئة صامتة | **حرج** |

**شكل التطبيق:**
```cpp
// SAD_INVARIANT_DEF(ARABIC_STRING_UTF8_NORMALIZED,
//   "كل نص عربي يجب NFC — بدونه مقارنات خاطئة صامتة",
//   critical=true)
StringRef ArabicStringPool::intern(StringRef str) {
    // SAD_INVARIANT_CHECK(ARABIC_STRING_UTF8_NORMALIZED)
    SAD_INVARIANT("ARABIC_STRING_UTF8_NORMALIZED", isNFCNormalized(str), "نص غير NFC");
    return pool_.intern(str);
}
```

**Invariant Linter:** `scripts/lint/check_invariants.py`
- كل `SAD_INVARIANT_CHECK(X)` يجب أن يقابله `SAD_INVARIANT_DEF(X)`
- الثوابت `critical=true` يجب أن تكون في نقطة تحقق واحدة على الأقل

---

### FR-03: SPEC — code_standards.yaml

**الملف:** `scripts/lint/code_standards.yaml`

```yaml
rules:
  - id: CS-01
    description: "مقارنة نص عربي مباشر (UTF-8 bytes) في C++"
    pattern: '==\s*"[\x80-\xff]'
    scope: [".cpp", ".h"]
    level: FATAL
    fix: "استخدم enum مقابل (UINodeType, TokenType, ...) بدل النص المباشر"
    exclude_patterns: ["// SAD_LINT_DISABLE: CS-01"]  # غير متاح

  - id: CS-02
    description: "getValue()==keyword_محجوز بدون check(TT::KEYWORD_*) مسبقه"
    logic: custom  # انظر منطق Python التفصيلي
    reserved_words_source: "shared/lexer/src/lexer_keywords.cpp"
    level: FATAL
    fix: "استخدم check(TT::KEYWORD_FOO) أو نمط التحقق المزدوج للكلمات السياقية"
    allowed_pattern: "check(TT::KEYWORD_*) || (check(TT::IDENTIFIER) && getValue() == ...)"

  - id: CS-03
    description: "اختصار KW_ بدل KEYWORD_ في enum/token"
    pattern: '\bKW_[A-Z_]+\b'
    scope: [".cpp", ".h"]
    level: FATAL
    fix: "استخدم KEYWORD_FOO بدل KW_FOO"

  - id: CS-05
    description: "حجم ملف يتجاوز 800 سطر"
    check: line_count > 800
    scope: [".cpp", ".h"]
    level: FATAL
    fix: "قسّم إلى module_core.cpp + module_helpers.cpp"
    inline_disable: "SAD_LINT_DISABLE: CS-05"  # متاح مع سبب
```

---

### FR-04: ENFORCER — check_code_standards.py

**المتطلبات:**
- يقرأ `code_standards.yaml` ويفحص الملفات المُمرَّرة
- يتحقق من `code_standards_allowlist.yaml` — يرفض إدخالاً بدون `expiry`
- يرفض إدخالاً منتهي الصلاحية (يستخدم تاريخ اليوم الفعلي)
- يُنذر بـ WARNING إذا اقترب الانتهاء < 14 يوم
- يُنتج تقريراً منسقاً:

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
❌ مخالفة معايير الكود
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
[CS-01] interpreter/src/visitors/expression_evaluator_ui.cpp:88
المشكلة : مقارنة نص عربي مباشر
السطر   : if (node.widgetName == "\xd9\x86\xd8\xb5" || ...)
الحل    : استخدم sad::ui::arabicNameToNodeType() ثم قارن UINodeType
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

---

### FR-05: CMake Guard

**الملف:** `cmake/code_standards_guard.cmake`

```cmake
function(sad_check_code_standards)
    if(SAD_SKIP_STANDARDS_GUARD)
        file(APPEND "${CMAKE_BINARY_DIR}/standards_guard_bypassed.log"
             "${CMAKE_CURRENT_LIST_FILE}: bypassed at ${CMAKE_CURRENT_LIST_LINE}\n")
        message(WARNING "⚠️ حارس معايير الكود مُعطَّل (SAD_SKIP_STANDARDS_GUARD=ON)")
        return()
    endif()
    # يُشغّل check_code_standards.py + check_invariants.py
    # FATAL_ERROR إذا exit code != 0
endfunction()
```

**التكامل في CMakeLists.txt:**
```cmake
include(cmake/code_standards_guard.cmake)
sad_check_code_standards()  # في نهاية CMakeLists الرئيسي
```

---

### FR-06: Allowlist بسياسة الحذف الإلزامية

**الملف:** `scripts/lint/code_standards_allowlist.yaml`

```yaml
entries:
  - path: "interpreter/src/visitors/expression_evaluator_ui.cpp"
    rule: CS-01
    expiry: "2026-06-15"
    owner: "dev-team"
    reason: "مُجدوَل للإصلاح بعد اكتمال UINodeType enum"
```

**القواعد:**
- كل الحقول إلزامية — بدون أي منها: configure يفشل
- انتهاء الصلاحية = مخالفة عادية (FATAL)
- `SAD_LINT_DISABLE` inline: متاح **فقط** لـ CS-04 و CS-05، يتطلب سبباً مكتوباً
- CS-01/02/03: لا inline disable — فقط allowlist

---

## 5. المتطلبات غير الوظيفية

| المتطلب | المعيار |
|---|---|
| الأداء | configure overhead < 3 ثواني |
| الموثوقية | fixtures معزولة للـ ENFORCER tests (ليس ملفات المشروع) |
| القابلية | إضافة قاعدة = سطر YAML واحد |
| التوافق | Python 3.8+ + requirements.txt محدد |
| الشفافية | SAD_SKIP_STANDARDS_GUARD يُسجَّل دائماً |

---

## 6. ما هو خارج النطاق

- Custom Clang-Tidy checks (أسابيع عمل، لا YAML extensibility)
- CMAKE_CXX_CLANG_TIDY (يُبطئ compilation 3-5x)
- CS-06 (@brief مزدوج) — مؤجل
- CI/CD integration — مرحلة لاحقة

---

## 7. المخاطر

| الخطر | الاحتمال | التخفيف |
|---|---|---|
| CS-02 false positive | متوسط | ATDD: ≥15 test قبل الـ regex |
| allowlist تنمو | عالٍ | expiry إلزامي + مراجعة شهرية |
| Python version mismatch | منخفض | venv + requirements.txt مثبَّت |
| وكيل يستخدم SKIP_GUARD | متوسط | تسجيل إلزامي + WARNING دائم |

---

## 8. خارطة الطريق

### Sprint 1 (الأعلى أولوية)

| # | المهمة | الجهد التقديري |
|---|---|---|
| S1-1 | تعديل `.clang-tidy` + اختبار IDE | 30 دقيقة |
| S1-2 | إصلاح `expression_evaluator_ui.cpp` | 1 ساعة |
| S1-3 | `sad_invariant.h` + تطبيق على `arabic_string_pool.cpp` | 2 ساعة |
| S1-4 | `code_standards.yaml` + `check_code_standards.py` | 3 ساعات |
| S1-5 | `code_standards_allowlist.yaml` | 30 دقيقة |
| S1-6 | `code_standards_guard.cmake` + ربط CMakeLists | 1 ساعة |

### Sprint 2

| # | المهمة |
|---|---|
| S2-1 | SAD_INVARIANT على الـ 5 ملفات الباقية |
| S2-2 | `test_code_standards.py` (≥15 test per rule) |
| S2-3 | `test_invariants.py` + `check_invariants.py` |

---

_المستند يعكس قرارات: جلسة Problem Solving BMAD CIS + حفلة الوكلاء (Winston، Dr. Quinn، Murat)_
_آخر تحديث: 2026-05-17_
