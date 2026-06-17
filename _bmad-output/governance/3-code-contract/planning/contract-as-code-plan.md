# Problem Solving Session: إلزام المطورين ووكلاء AI بمعايير الكود (Contract-as-Code)

**التاريخ:** 2026-05-17
**المشروع:** لغة ص (Sad Programming Language)
**المُحلِّل:** GitHub Copilot (Claude Sonnet 4.6)
**الجلسة:** `problem-solution-2026-05-17`
**التصنيف:** هندسة جودة الكود — أتمتة المعايير

---

## 🎯 تعريف المشكلة

### المشكلة الأولية

وكلاء الذكاء الاصطناعي والمطورون يخرقون معايير الكود المطلوبة (مثل: مقارنة نصوص عربية مباشرة، أسماء مجتزأة، استخدام `KW_*` بدل `KEYWORD_*`) لأن القواعد موجودة في وثائق نصية فقط (copilot-instructions.md) وغير مُفرَّضة آلياً.

**المثال المُكتشَف:**
```cpp
// expression_evaluator_ui.cpp:88 — مخالفة CS-01
if (node.widgetName == "\xd9\x86\xd8\xb5" || node.widgetName == "Text")
// يجب استخدام: sad::ui::UINodeType (enum) مباشرة
```

### المشكلة المُصاغة بدقة

> **غياب "Contract-as-Code":** قواعد الكود موثقة لكنها غير مُرمَّجة في أدوات البناء.
> نتيجة: لا توجد عقوبة تقنية على الخرق — الكود المخالف يُبنى ويعمل بشكل طبيعي.

### سياق المشكلة

- المشروع لغة برمجة عربية كبيرة (compiler + interpreter + VM + stdlib)
- فريق التطوير يعتمد بشكل كبير على وكلاء AI
- وكلاء AI يختارون "الحل الأسرع" بدون مراعاة المعايير إلا إذا أُجبروا تقنياً
- يوجد نموذج ناجح موجود بالفعل: `orphan_sources_guard.cmake` يفشل configure عند وجود ملفات يتيمة

### معايير النجاح

1. أي ملف `.cpp`/`.h` جديد يحتوي مخالفة → cmake configure يفشل بتقرير واضح
2. المطور يرى تحديداً: الملف + رقم السطر + القاعدة المخالفة + الحل الصحيح
3. إضافة قاعدة جديدة لا تتطلب تعديل Python أو CMake — فقط سطر YAML
4. الملفات الحالية المخالفة مُدرجة في whitelist للترحيل التدريجي

---

## 🔍 التشخيص والسبب الجذري

### حدود المشكلة (Is / Is Not)

| يحدث (IS) | لا يحدث (IS NOT) |
|---|---|
| في أي ملف `.cpp`/`.h` جديد | في الملفات القديمة المُدرجة في allowlist |
| عند استخدام وكيل AI | عند المراجعة اليدوية الدقيقة |
| الخرق الصامت (الكود يعمل لكنه مخالف) | الأخطاء المنطقية (مغطاة بالاختبارات) |
| معايير CW-09، CW-13، CW-26 | الأخطاء النحوية (يمسك بها المترجم) |

### تحليل السبب الجذري (Five Whys)

1. **لماذا خرق الوكيل القاعدة؟**
   → لأنه يبحث عن "الحل الأسرع" للمهمة الفورية

2. **لماذا يختار الأسرع؟**
   → لأن القاعدة غير ملزمة — لا عقوبة تقنية على الخرق

3. **لماذا لا توجد عقوبة تقنية؟**
   → لأن القواعد نصية فقط في copilot-instructions.md، غير مُرمَّجة في أدوات البناء

4. **لماذا لم تُرمَّج؟**
   → لم يُبنَ بعد نظام lint/guard مُصمَّم لهذه المعايير

5. **السبب الجذري:**
   → **غياب "Contract-as-Code"** — قواعد الكود يجب أن تكون كوداً قابلاً للتنفيذ، وليس وثائق قابلة للقراءة فقط.

### العوامل المُساهِمة

- `copilot-instructions.md` لا يُحمَّل في كل رد للوكيل (سياق محدود)
- لا يوجد feedback فوري للوكيل عند الخرق قبل commit
- الملفات الكبيرة (>800 سطر) لا تُكتشف حالياً
- عدم وجود `@brief (AR)/(EN)` لا يمنع البناء

### ديناميكيات النظام

```
الوكيل يكتب كوداً ← يختبره محلياً (يعمل) ← يُدمج في المشروع
      ↑                                              ↓
  لا عقوبة فورية                          المخالفة تتراكم
      ↑                                              ↓
  يعود للنمط نفسه ←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←
```

**النمط المطلوب:**
```
الوكيل يكتب كوداً ← cmake configure يفشل ← الوكيل يُصحح فوراً
```

---

## 📊 تحليل القوى

### القوى الدافعة (تدعم الحل)

- نمط `orphan_sources_guard.cmake` ناجح وموجود → نُعيد استخدامه
- `scripts/codegen/test_migration_guard.py` → سابقة للـ lint tests
- Python + CMake + YAML الإطار متوفر تماماً في المشروع
- الفريق يريد الالتزام بالمعايير — المشكلة تقنية وليست ثقافية

### القوى المقاومة (تعيق الحل)

- ملفات legacy موجودة ومخالفة → يجب allowlist تدريجية
- وكلاء AI لا يمكن "تعليمها" مباشرة → يجب أن يكون الفشل في BUILD
- أي تعقيد زائد في configure يُبطئ التطوير

### القيد الحقيقي

> الوكيل يمكنه تعديل الكود، لكنه **لا يمكنه تعطيل cmake configure بشكل صامت.**
> المطور سيرى FATAL_ERROR ويُجبَر على الإصلاح.

### الرؤية الرئيسية

**أقوى ضمان:** الفشل في **configure-time** (قبل أي compilation) — لا يبدأ البناء أصلاً.

---

## 💡 الحل الموصى به: نظام "Contract-as-Code" — النموذج الهجين

> **التحديث 1:** بعد تقييم Clang-Tidy مقابل Python/Regex، اعتُمد نموذج هجين.
> **التحديث 2 (حفلة الوكلاء):** إضافة طبقة SAD_INVARIANT كـ "طبقة 0.5" — ثوابت معمارية مُضمَّنة في الكود تعالج السبب الجذري الأعمق: غياب نية التصميم عند الوكيل في لحظة القرار.

### البنية الهجينة (ثلاث طبقات)

```
┌─────────────────────────────────────────────────────────────┐
│  طبقة 0: Clang-Tidy المُحسَّن — IDE feedback فوري           │
│  .clang-tidy (يعمل عبر clangd في VS Code فوراً)            │
│  إضافة: cppcoreguidelines-pro-type-reinterpret-cast         │
│  يغطي: CS-04 (reinterpret_cast خارج FFI)                   │
│  zero compile overhead — لا تأثير على cmake                 │
└──────────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────────┐
│  طبقة 0.5: SAD_INVARIANT — ثوابت معمارية في الكود          │
│  compiler/include/shared/sad_invariant.h (جديد)            │
│                                                             │
│  ثلاث طبقات داخلية:                                        │
│  • SAD_INVARIANT_DEF: تعريف الثابت (id + why + owner)       │
│  • SAD_INVARIANT_CHECK: نقطة تحقق في الكود                  │
│  • SAD_INVARIANT(id, cond, msg): macro runtime assertion     │
│                                                             │
│  يعالج: السبب الجذري — غياب نية التصميم للوكيل             │
│  الـ linter يتحقق: كل CHECK له DEF، وكل critical مُحمي     │
│  مغطى في: 6 نقاط حرجة عبر جميع طبقات اللغة                │
└──────────────────────────────────────────────────────────────┘
                           ↓
┌─────────────────────────────────────────────────────────────┐
│  طبقة 1-3: Python/YAML + CMake — configure-time enforcement │
│  CS-01 عربية مباشرة │ CS-02 getValue()+محجوزة              │
│  CS-03 KW_ اختصار   │ CS-05 حجم > 800 سطر                  │
│  FATAL_ERROR مع تقرير كامل (ملف + سطر + حل)                │
└─────────────────────────────────────────────────────────────┘
```

### توزيع القواعد على الطبقات

| القاعدة | الطبقة | الأداة | السبب |
|---|---|---|---|
| CS-01 عربية مباشرة | 1 | Python/Regex | لا check مدمج في Clang-Tidy |
| CS-02 getValue()+محجوزة | 1 | Python/Regex (مُصحَّح) | custom AST check مُكلف جداً |
| CS-03 KW_ اختصار | 1 | Python/Regex | regex بسيط وكافٍ |
| CS-04 reinterpret_cast | 0 | Clang-Tidy مدمج | `pro-type-reinterpret-cast` دقيق عبر AST |
| CS-05 حجم > 800 سطر | 1 | Python/عدّ أسطر | Clang-Tidy لا يقيس الأسطر |
| CS-06 @brief مزدوج | مستقبل | Python/Regex | مؤجل |

### مبرر الاختيار

| البديل | القرار | السبب |
|---|---|---|
| pre-commit hook فقط | مرفوض | سهل التجاوز، لا يُلزم الوكيل |
| CI/CD check فقط | مرفوض | متأخر جداً، feedback بطيء |
| custom Clang-Tidy checks | مرفوض | أسابيع عمل لكل قاعدة، لا YAML extensibility |
| CMAKE_CXX_CLANG_TIDY | مرفوض | يُبطئ compilation 3-5x بدون مكسب لـ CS-01/02/03 |
| **النموذج الهجين** | **مُختار** | Clang-Tidy للسياق (CS-04)، Python للنص (CS-01/02/03) |

---

## 🚀 خطة التنفيذ

### الخطوات التفصيلية (ترتيب مُصحَّح — النموذج الهجين)

#### المرحلة أ: Clang-Tidy (طبقة 0) — تغيير صغير، أثر فوري في IDE

| # | الخطوة | الملف | الوصف |
|---|---|---|---|
| أ-1 | تحسين `.clang-tidy` | `.clang-tidy` | أضف `cppcoreguidelines-pro-type-reinterpret-cast` لتغطية CS-04 |
| أ-2 | اختبار IDE | VS Code + clangd | تحقق أن clangd يُظهر تحذير عند `reinterpret_cast` خارج سياقه |

#### المرحلة أ.5: SAD_INVARIANT (طبقة 0.5) — ثوابت معمارية

| # | الخطوة | الملف | الوصف |
|---|---|---|---|
| أ.5-1 | إنشاء Header الثوابت | `compiler/include/shared/sad_invariant.h` | Macro الثلاثي: DEF + CHECK + INVARIANT assertion |
| أ.5-2 | إضافة invariant linter | `scripts/lint/check_invariants.py` | يتحقق: كل CHECK له DEF، الـ critical مُحمي |
| أ.5-3 | تطبيق ثوابت الطبقة المشتركة | `shared/lexer/` + `shared/parser/` + `shared/ast/` | 2 ثوابت حرجة (انظر جدول الجرد) |
| أ.5-4 | تطبيق ثوابت المفسر | `interpreter/src/` | 2 ثوابت حرجة |
| أ.5-5 | تطبيق ثوابت المترجم | `compiler/src/backend/llvm/` | 2 ثوابت حرجة |
| أ.5-6 | ربط بالـ CMake guard | `cmake/code_standards_guard.cmake` | أضف invariant check للـ configure-time |

##### جرد الثوابت الحرجة — 6 نقاط عبر جميع طبقات اللغة

| id | الموقع | الملف | why | silent_failure |
|---|---|---|---|---|
| `LEXER_TOKEN_POSITION_ONE_BASED` | Lexer | `shared/lexer/src/lexer_core.cpp` | Position يبدأ من 1 (سطر، عمود) — كسره يُفسد كل رسائل الخطأ | نعم |
| `PARSER_SCOPE_STACK_BALANCED` | Parser | `shared/parser/src/parser_core.cpp` | كل `enterScope` يقابله `exitScope` — كسره = AST مكسور صامت | نعم |
| `INTERPRETER_SCOPE_NO_LEAK` | Interpreter | `interpreter/src/core/interpreter_core.cpp` | كل نطاق مفتوح يُغلق — كسره = تسرب متغيرات بين النطاقات | نعم |
| `GOROUTINE_SNAPSHOT_IMMUTABLE` | Goroutines | `interpreter/src/channel.cpp` | snapshot المتغيرات عند `أطلق` لا يتغير بعد التقاطه | نعم |
| `SIR_SCOPE_STACK_BALANCED` | SIR Builder | `compiler/src/sir/sir_builder.cpp` | كل pushScope يقابله popScope — كسره = IR مكسور | نعم |
| `ARABIC_STRING_UTF8_NORMALIZED` | String Pool | `compiler/src/backend/llvm/arabic_string_pool.cpp` | كل نص عربي يدخل البركة يجب أن يكون NFC — كسره = مقارنات خاطئة صامتة | **نعم (الأخطر)** |

##### شكل التطبيق الموحّد

```cpp
// ── في ملف الـ Header ────────────────────────────────────────
// sad_invariant.h

#pragma once

// (AR) ماكرو تعريف الثابت — للتوثيق في موقع التعريف
// (EN) Invariant definition macro — documents at declaration site
#define SAD_INVARIANT_DEF(id, why, critical)  // no-op at runtime

// (AR) ماكرو نقطة التحقق — يُعلن أن هذا الكود يعتمد على الثابت
// (EN) Invariant check point — declares this code relies on the invariant
#define SAD_INVARIANT_CHECK(id)               // no-op at runtime

// (AR) ماكرو التحقق الفعلي في وقت التشغيل
// (EN) Runtime enforcement macro
#define SAD_INVARIANT(id, condition, msg)          \
    do {                                           \
        if (!(condition)) {                        \
            ::Sad::invariantViolated(id, msg,      \
                __FILE__, __LINE__);               \
        }                                          \
    } while(0)

// ── مثال في arabic_string_pool.cpp ──────────────────────────

// SAD_INVARIANT_DEF(ARABIC_STRING_UTF8_NORMALIZED,
//   "كل نص عربي في البركة يجب أن يكون UTF-8 NFC — "
//   "المقارنة بين نصوص غير مُطبَّعة تُنتج false صامتاً",
//   critical=true)

StringRef ArabicStringPool::intern(StringRef str) {
    // SAD_INVARIANT_CHECK(ARABIC_STRING_UTF8_NORMALIZED)
    SAD_INVARIANT("ARABIC_STRING_UTF8_NORMALIZED",
                  isNFCNormalized(str),
                  "نص عربي غير مُطبَّع يدخل البركة");
    return pool_.intern(str);
}
```

---

#### المرحلة ب: Python/CMake (طبقة 1) — configure-time enforcement

| # | الخطوة | الملف | الوصف |
|---|---|---|---|
| ب-0 | **التحقق المسبق** | `shared/lexer/src/lexer_keywords.cpp` + UINodeType | تأكد من وجود البدائل (enum) قبل إلزام الحارس |
| ب-1 | إصلاح المخالف أولاً | `interpreter/src/visitors/expression_evaluator_ui.cpp` | يجب إصلاح الكود المخالف **قبل** تفعيل الحارس |
| ب-2 | إنشاء SPEC | `scripts/lint/code_standards.yaml` | تعريف CS-01/02/03/05 بالتعريفات المُصحَّحة (CS-04 في Clang-Tidy) |
| ب-3 | إنشاء ENFORCER | `scripts/lint/check_code_standards.py` | فاحص Python يقرأ YAML + منطق CS-02 المُصحَّح |
| ب-4 | إنشاء allowlist | `scripts/lint/code_standards_allowlist.txt` | مع سياسة الحذف الإلزامية (انظر أدناه) |
| ب-5 | إنشاء CMake guard | `cmake/code_standards_guard.cmake` | configure-time enforcement |
| ب-6 | ربط CMakeLists.txt | `CMakeLists.txt` | include + sad_check_code_standards() |
| ب-7 | اختبار pytest | `scripts/codegen/test_code_standards.py` | اختبارات الوحدة للـ ENFORCER |

### القواعد المُطبَّقة (CS-XX)

| رمز | القاعدة | النمط | مستوى الإلزام |
|---|---|---|---|
| CS-01 | مقارنة نص عربي مباشر في `.cpp`/`.h` | `==\s*"[\x80-\xff]` أو UTF-8 Arabic literals خارج التعليقات والنصوص الاختبارية | FATAL |
| CS-02 | `getValue() == "..."` بدون سياق التحقق المزدوج المُعتمَد | انظر التعريف الدقيق أدناه | FATAL |
| CS-03 | اختصار `KW_` بدل `KEYWORD_` | `\bKW_[A-Z_]+\b` في enum/token | FATAL |
| CS-04 | `reinterpret_cast` خارج FFI | `reinterpret_cast` خارج مجلدات `ffi/`, `runtime_new/`, `vm/abi/` | WARN |
| CS-05 | حجم ملف > 800 سطر | عدد الأسطر | FATAL (ليس WARN — WARN يُتجاهل) |
| CS-06 | عدم وجود `@brief (AR)` في API عام | دوال عامة خارج `detail/` و `internal/` بدون توثيق مزدوج | WARN |

#### تعريف CS-02 الدقيق (بعد إصلاح التناقض مع copilot-instructions.md)

**المشكلة المُكتشَفة في النقد:** copilot-instructions.md تُوصي صراحةً بالنمط:
```cpp
// نمط التحقق المزدوج للكلمات السياقية — مسموح به ومطلوب:
if (check(TT::KEYWORD_TRAIT) || 
    (check(TT::IDENTIFIER) && current_.getValue() == "سمة"))
```

لذلك CS-02 **يجب أن تمسك بحالات محددة فقط**:

**مُحظور (CS-02 FATAL):**
```cpp
// مقارنة getValue() مباشرة بدون فحص TokenType أولاً
if (token.getValue() == "إذا")           // ← خطأ: كلمة محجوزة لها KEYWORD_IF
if (token.getValue() == "صنف")           // ← خطأ: كلمة محجوزة لها KEYWORD_CLASS
// القاعدة: إذا كان للكلمة KEYWORD_* مقابل في enum → يجب استخدام check(TT::KEYWORD_*)
```

**مسموح به (لا تُثير CS-02):**
```cpp
// نمط التحقق المزدوج للكلمات السياقية (مُوصى به في copilot-instructions.md)
if (check(TT::KEYWORD_TRAIT) || (check(TT::IDENTIFIER) && current_.getValue() == "سمة"))

// getValue() في سياقات غير المقارنة مع كلمات محجوزة
std::string name = token.getValue();     // ← مسموح
```

**آلية الفحص في ENFORCER:**
- إذا وجد `getValue() == "X"` → تحقق هل X كلمة محجوزة (موجودة في lexer_keywords.cpp)
- إذا كانت محجوزة ولم يسبقها `check(TT::KEYWORD_*)` في نفس `if` → FATAL
- إذا كانت سياقية → مسموح

### آلية allowlist (الترحيل التدريجي) — مع سياسة الحذف الإلزامية

```yaml
# scripts/lint/code_standards_allowlist.txt
# (AR) كل إدخال يتطلب: مسار + قاعدة + تاريخ انتهاء + مسؤول
# (EN) Each entry requires: path + rule + expiry_date + owner
# بدون تاريخ انتهاء → ENFORCER يرفض الإدخال ويفشل configure

- path: "interpreter/src/visitors/expression_evaluator_ui.cpp"
  rule: CS-01
  expiry: "2026-06-15"
  owner: "dev-team"
  reason: "مُجدوَل للإصلاح بعد اكتمال UINodeType enum"
```

**سياسة الحذف الإلزامية:**
- كل إدخال في allowlist له تاريخ انتهاء إلزامي
- عند انتهاء التاريخ → ENFORCER يُعامل الملف كمخالف عادي (يفشل configure)
- تمديد التاريخ يتطلب تعديل allowlist يدوياً (لا يُفعَله الوكيل بشكل صامت)
- **مقياس الصحة:** حجم allowlist يجب أن يتراجع بمرور الوقت، وليس يزداد

### تعطيل سطري (Inline Disable) — مُقيَّد

```cpp
// SAD_LINT_DISABLE: CS-04 — السبب: FFI boundary مع مكتبة خارجية، مُعتمد في code-review 2026-05-17
if (name == "sqlite3") { ... }
```

**قيود:**
- `SAD_LINT_DISABLE` متاح فقط لـ CS-04 و CS-05 (ليس CS-01 أو CS-02 أو CS-03)
- ENFORCER يتحقق أن التعليق يحتوي "السبب:" وإلا يرفضه
- CS-01/CS-02/CS-03 لا يمكن تعطيلها سطرياً — فقط عبر allowlist بتاريخ انتهاء

### شكل رسالة الخطأ

```
CMake Error at cmake/code_standards_guard.cmake:42 (message):

  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  ❌ مخالفة معايير الكود — البناء متوقف
  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  [CS-01] interpreter/src/visitors/expression_evaluator_ui.cpp:88
  المشكلة : مقارنة نص عربي مباشر (UTF-8 bytes) في كود C++
  السطر   : if (node.widgetName == "\xd9\x86\xd8\xb5" || ...)
  الحل    : استخدم sad::ui::arabicNameToNodeType() ثم قارن UINodeType (enum)

  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  للتعطيل المؤقت (يُسجَّل في build/standards_guard_bypassed.log):
  cmake -DSAD_SKIP_STANDARDS_GUARD=ON -B build
  ⚠️  تحذير: التعطيل يُسجَّل تلقائياً ويظهر في cmake --build كـ WARNING
  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

> **ملاحظة هامة:** `-DSAD_SKIP_STANDARDS_GUARD=ON` موجود للطوارئ فقط. يجب أن يُنتج CMake تحذيراً مرئياً عند كل `cmake --build` طالما هو مُفعَّل، حتى لا يُنسى في بيئة تطوير.

---

## 📈 المراقبة والتحقق

### مقاييس النجاح

- `cmake -B build` يفشل عند وجود مخالفة جديدة ✓
- `cmake -B build` ينجح بدون مخالفات جديدة ✓
- pytest يتضمن اختبارات check_code_standards.py ✓
- صفر مخالفات CS-01/02/03 خارج allowlist ✓

### خطة التحقق

1. أنشئ ملف test مخالف → تحقق من فشل configure
2. أضف الملف للـ allowlist → تحقق من نجاح configure
3. أصلح المخالفة → احذف من allowlist → تحقق من نجاح configure
4. شغّل pytest على check_code_standards.py

### إدارة المخاطر

| الخطر | الاحتمال | الإجراء |
|---|---|---|
| تباطؤ configure بسبب فحص كل الملفات | متوسط | فحص الملفات المُعدَّلة فقط (git diff) في المستقبل |
| false positive على كود C++ مشروع | منخفض | allowlist inline + regex دقيقة |
| المطور يُعطّل الحارس نهائياً | منخفض | تسجيل في CI/CD بـ WARN إذا عُطِّل |

### محفزات التعديل

- إذا ظهر false positive متكرر → أدق regex أو إضافة استثناء للـ YAML
- إذا احتاج configure > 5 ثواني بسبب الفحص → تفعيل فحص diff فقط

---

## 📝 الدروس المستفادة

### الرؤية الجوهرية

> **"قاعدة لا تُنفَّذ آلياً ليست قاعدة — هي مجرد اقتراح"**
> أي معيار كود يُريد المشروع إلزامه يجب أن يكون:
> 1. مُعرَّفاً في ملف SPEC قابل للآلة (YAML/JSON)
> 2. مُفرَّضاً في دورة حياة البناء (build lifecycle)
> 3. موثقاً بتقرير خطأ يشير إلى الحل الصحيح

### ما يجب تجنبه

- الاعتماد على وثائق نصية فقط لإلزام معايير تقنية
- pre-commit hooks كآلية الإلزام الوحيدة (سهل التجاوز)
- أرسال رسائل خطأ بدون اقتراح الحل الصحيح

---

## 🗂️ الملفات المُنشأة / المُعدَّلة

```
# ── طبقة 0: Clang-Tidy ─────────────────────────────────────
.clang-tidy                            ← تعديل: pro-type-reinterpret-cast (CS-04)

# ── طبقة 0.5: SAD_INVARIANT ────────────────────────────────
compiler/include/shared/
└── sad_invariant.h                    ← جديد: Macro الثلاثي (DEF + CHECK + runtime)

scripts/lint/
└── check_invariants.py                ← جديد: linter للثوابت

# ── تطبيق الثوابت (6 ملفات) ────────────────────────────────
shared/lexer/src/lexer_core.cpp        ← تعديل: LEXER_TOKEN_POSITION_ONE_BASED
shared/parser/src/parser_core.cpp      ← تعديل: PARSER_SCOPE_STACK_BALANCED
interpreter/src/core/interpreter_core.cpp ← تعديل: INTERPRETER_SCOPE_NO_LEAK
interpreter/src/channel.cpp            ← تعديل: GOROUTINE_SNAPSHOT_IMMUTABLE
compiler/src/sir/sir_builder.cpp       ← تعديل: SIR_SCOPE_STACK_BALANCED
compiler/src/backend/llvm/arabic_string_pool.cpp ← تعديل: ARABIC_STRING_UTF8_NORMALIZED

# ── طبقة 1: Python/CMake ────────────────────────────────────
scripts/lint/
├── code_standards.yaml                ← جديد: SPEC (CS-01/02/03/05)
├── check_code_standards.py            ← جديد: ENFORCER
└── code_standards_allowlist.yaml      ← جديد: allowlist بسياسة الحذف الإلزامية

cmake/
└── code_standards_guard.cmake         ← جديد: configure-time (يشمل invariant check)

CMakeLists.txt                         ← تعديل: include + استدعاء الحارس

# ── اختبارات ────────────────────────────────────────────────
scripts/codegen/
├── test_code_standards.py             ← جديد: اختبارات ENFORCER (≥15 test لـ CS-02)
└── test_invariants.py                 ← جديد: اختبارات invariant linter

# ── إصلاح مسبق (قبل تفعيل الحارس) ─────────────────────────
interpreter/src/visitors/
└── expression_evaluator_ui.cpp        ← تعديل: إصلاح CS-01
```

### ملاحظة: تغيير صيغة allowlist

الملف تغيّر من `.txt` إلى `.yaml` لدعم سياسة الحذف الإلزامية:
```yaml
# scripts/lint/code_standards_allowlist.yaml
entries:
  - path: "interpreter/src/visitors/expression_evaluator_ui.cpp"
    rule: CS-01
    expiry: "2026-06-15"
    owner: "dev-team"
    reason: "مُجدوَل للإصلاح بعد اكتمال UINodeType enum"
```
ENFORCER يرفض أي إدخال بدون `expiry` ويُفشل configure إذا انتهى تاريخ إدخال.

---

_تم التوثيق بواسطة: GitHub Copilot (Claude Sonnet 4.6)_
_المنهجية: BMAD Creative Intelligence Suite — Problem Solving Workflow_
_التاريخ: 2026-05-17_
