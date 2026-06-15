# خطة ترحيل رسائل الأخطاء إلى YAML

**المؤلف**: Amelia (bmad-agent-dev)
**التاريخ**: 2026-05-19
**الحالة**: مسوّدة (Draft) — بانتظار المراجعة
**النطاق**: نقل كل رسائل الأخطاء (~203 ErrorCode) من ملفات C++ إلى ملف YAML واحد مع توليد C++ تلقائياً، على نمط [data/language/keywords.yaml](data/language/keywords.yaml).

---

## 1. الوضع الراهن

### 1.1 الملفات المعنية

| الملف | الحجم | الدور |
|---|---|---|
| [shared/errors/include/error_codes.h](shared/errors/include/error_codes.h) | 203 enum | تعريف `ErrorCode` enum |
| [shared/errors/src/error_codes.cpp](shared/errors/src/error_codes.cpp) | 843 سطر / 410 إدخال | جدولان: `code→"SEM002"` + `code→{msgAr,msgEn}` افتراضية |
| [shared/errors/include/error_catalog.h](shared/errors/include/error_catalog.h) | — | `ErrorTemplate` (titleAr/En, briefAr/En, detailedAr/En, teacherAr/En, fixHintAr/En, codeExample) + `ErrorCatalog` Singleton |
| [shared/errors/src/error_catalog_init.cpp](shared/errors/src/error_catalog_init.cpp) | 213 سطر / 26 template غني | `registerDefaults()` يدوية |

### 1.2 المشكلة المعمارية

- **مصدران منفصلان** للنص العربي/الإنجليزي للخطأ الواحد:
  - الأساسي (الافتراضي) في `error_codes.cpp::errorMessages` map
  - الغني (template مع placeholders) في `error_catalog_init.cpp` (لـ26 خطأ فقط من 203)
- **177 ErrorCode بلا template غني** — تظهر بنص افتراضي عام بدون fixHint ولا codeExample.
- **يدوي بالكامل**: إضافة خطأ جديد = تعديل 3 ملفات (header + cpp + init).
- **انتهاك CW-19 (DRY)**: نفس النص قد يتكرر في الـmap الافتراضي وفي الـtemplate الغني.
- **انتهاك CW-20 (Open/Closed)**: لا يمكن إضافة رسالة بدون تعديل C++.

### 1.3 النمط المرجعي (لِما يجب اتباعه)

النظام يستخدم بالفعل codegen ناضج لـ:
- [data/language/keywords.yaml](data/language/keywords.yaml) → [shared/lexer/generated/keywords_generated.{h,cpp}](shared/lexer/generated/)
- `data/language/type_methods.yaml` → `shared/types/generated/type_methods_generated.{h,cpp}`

البنية التحتية الجاهزة:
- [scripts/codegen/_lib/loader.py](scripts/codegen/_lib/loader.py) — `load_yaml`, `load_schema`, `validate_schema`
- [scripts/codegen/_lib/emit.py](scripts/codegen/_lib/emit.py) — `cpp_string_literal`, `vector_literal`, `write_if_changed`
- [cmake/codegen.cmake](cmake/codegen.cmake) — `find_package(Python3 3.9)` + `add_custom_command` + `add_custom_target`
- Baseline files ملتزمة في git (للبناء بدون Python)

---

## 2. التصميم المستهدف

### 2.1 ملف SOT الجديد: `data/language/error_messages.yaml`

```yaml
version: "1.0"
language: "sad"

categories:
  lexical:   { prefix: "LEX", description_ar: "أخطاء معجمية",        description_en: "Lexical errors" }
  syntax:    { prefix: "SYN", description_ar: "أخطاء نحوية",         description_en: "Syntax errors" }
  semantic:  { prefix: "SEM", description_ar: "أخطاء دلالية",        description_en: "Semantic errors" }
  runtime:   { prefix: "RUN", description_ar: "أخطاء وقت التشغيل",   description_en: "Runtime errors" }
  ownership: { prefix: "OWN", description_ar: "أخطاء الملكية",       description_en: "Ownership errors" }
  import:    { prefix: "IMP", description_ar: "أخطاء الاستيراد",     description_en: "Import errors" }
  io:        { prefix: "IO",  description_ar: "أخطاء الإدخال/الإخراج", description_en: "I/O errors" }
  internal:  { prefix: "INT", description_ar: "أخطاء داخلية",        description_en: "Internal errors" }

errors:
  - code: SEM_TYPE_MISMATCH        # يطابق ErrorCode enum (الاسم بعد ::)
    id: "SEM002"                   # يطابق ^[A-Z]{2,3}\d{3}$
    category: semantic
    title:    { ar: "عدم تطابق الأنواع", en: "Type mismatch" }
    brief:    { ar: "متوقع '{expected}' لكن وُجد '{found}'",
                en: "Expected '{expected}' but found '{found}'" }
    fix_hint: { ar: "حوّل القيمة بـ {expected}(...)",
                en: "Convert with {expected}(...)" }
    detailed: { ar: "...", en: "..." }   # اختياري
    teacher:  { ar: "...", en: "..." }   # اختياري
    code_example: |
      متغير ن = رقم("12")
    placeholders: [expected, found]      # يُتحقَّق دلالياً
```

### 2.2 Schema: `data/language/error_messages.schema.json`

تحقُّقات إلزامية:
- `version` نمط `^\d+\.\d+$`
- كل `code` فريد
- كل `id` فريد ويطابق نمط `^[A-Z]{2,3}\d{3}$` ويبدأ بـprefix فئته (تحقُّق دلالي في المولِّد)
- `title.ar` و `brief.ar` و `title.en` و `brief.en` إلزامية
- `placeholders` array — كل `{name}` في النصوص يجب أن يكون في القائمة (تحقُّق دلالي)
- `code` يجب أن يطابق قيمة موجودة في `ErrorCode` enum (تحقُّق دلالي يقرأ `error_codes.h`)

### 2.3 الكود المُولَّد

**`shared/errors/generated/error_messages_generated.h`**:
```cpp
namespace Sad::Errors::Generated {
    struct GeneratedErrorEntry {
        ErrorCode    code;
        const char*  id;
        const char*  category;
        const char*  titleAr;     const char* titleEn;
        const char*  briefAr;     const char* briefEn;
        const char*  detailedAr;  const char* detailedEn;
        const char*  teacherAr;   const char* teacherEn;
        const char*  fixHintAr;   const char* fixHintEn;
        const char*  codeExample;
    };
    extern const GeneratedErrorEntry kErrorMessages[];
    extern const std::size_t kErrorMessagesCount;

    // وصول سريع
    const GeneratedErrorEntry* findByCode(ErrorCode code);   // O(1) via static unordered_map
    const GeneratedErrorEntry* findById(std::string_view id);
}
```

**`shared/errors/generated/error_messages_generated.cpp`**: مصفوفة مولَّدة + lookup-tables تُبنى عند الـstartup (CW-26).

### 2.4 الملفات المختصرة

| الملف | قبل | بعد |
|---|---|---|
| `error_codes.cpp` | 843 سطر (جدولان كبيران يدوياً) | ~200 سطر — `getErrorCodeString()` و `getErrorMessages()` تستخدمان `findByCode` |
| `error_catalog_init.cpp` | 213 سطر | ~60 سطر — `registerDefaults()` حلقة واحدة على `kErrorMessages` |

---

## 3. Stories مرتبطة بمعايير قبول

### Story EM-1: استخراج SOT
**AC-1.1**: `data/language/error_messages.yaml` يحوي **كل** الـ203 ErrorCodes بـ `id` + `category` + `title.{ar,en}` + `brief.{ar,en}` على الأقل.
**AC-1.2**: الـ26 ErrorCode التي لها templates غنية تنُقل بكل حقولها.
**AC-1.3**: `data/language/error_messages.schema.json` موجود ويتحقَّق بنجاح من YAML.
**أداة مساعدة**: `scripts/codegen/migrate_error_messages.py` (مؤقت) — يقرأ `error_codes.cpp` و `error_catalog_init.cpp` بـregex ويُولِّد YAML أولي. يُحذف بعد المراجعة اليدوية.

### Story EM-2: المولِّد + التحقُّق
**AC-2.1**: `scripts/codegen/gen_error_messages.py` ينتج `error_messages_generated.{h,cpp}`.
**AC-2.2**: المولِّد يفحص دلالياً:
- كل `code` موجود في `ErrorCode` enum
- كل `{placeholder}` في النصوص مذكور في `placeholders`
- كل `id` يطابق `prefix` فئته
**AC-2.3**: `cmake/codegen.cmake` يحوي target `sad_error_messages_codegen` بنفس نمط `sad_keywords_codegen`.
**AC-2.4**: الـbaseline files ملتزمة في git.
**AC-2.5**: `scripts/codegen/test_gen_error_messages.py` يحاكي نمط `test_gen_builtins.py`.

### Story EM-3: تكامل
**AC-3.1**: `ErrorCatalog::registerDefaults()` < 30 سطر.
**AC-3.2**: `getErrorMessages(code)` و `getErrorCodeString(code)` تستخدمان `findByCode` (CW-26 lookup).
**AC-3.3**: `error_codes.cpp` < 250 سطر.
**AC-3.4**: `error_catalog_init.cpp` < 80 سطر.
**AC-3.5**: نتيجة `run_tests.ps1` تبقى `PASS:137 / FAIL:20` بالضبط (BF-29).

### Story EM-4: اختبارات الإثبات
**AC-4.1**: `tests/errors/test_generated_messages.cpp` يفحص:
- عدد entries = عدد ErrorCode enum values
- كل `id` فريد
- كل `code` فريد
- `findByCode(SEM_TYPE_MISMATCH)->briefAr` يساوي النص المتوقع
**AC-4.2**: اختبار جولة كاملة (golden test) يقارن render قبل/بعد لكل خطأ.

### Story EM-5: sadinfo (اختياري — Phase 2)
**AC-5.1**: `sadinfo --errors --format=json` يصدّر كامل الكتالوج.
**AC-5.2**: `sadinfo --errors --code=SEM002` يطبع تفاصيل خطأ واحد.

---

## 4. تقييم المخاطر

| المخاطرة | الاحتمال | الأثر | المخفِّف |
|---|---|---|---|
| فقدان رسائل أثناء الترحيل | متوسط | عالٍ | سكربت الترحيل التلقائي + diff قبل/بعد |
| تباطؤ startup بسبب lookup | منخفض | منخفض | `unordered_map` يُبنى مرة (CW-26)؛ قياس قبل/بعد |
| سطور Arabic في C++ literals | منخفض | متوسط | `cpp_string_literal` من `_lib/emit.py` يتعامل مع UTF-8 |
| مولِّد Python مفقود في CI نظيفة | منخفض | عالٍ | baseline files ملتزمة في git (نمط `type_methods`) |
| تصادم placeholders مع نصوص عربية | منخفض | متوسط | فحص دلالي بـregex `\{[a-z_]+\}` في المولِّد |
| كسر API الحالي لـ`ErrorCatalog::render` | منخفض | عالٍ | الواجهة العامة لا تتغير — فقط مصدر البيانات |

---

## 5. الفوائد المتوقعة

| القاعدة | الفائدة |
|---|---|
| CW-04 تناسق | نفس نمط `keywords.yaml` و `type_methods.yaml` |
| CW-05 حجم ملف | `error_codes.cpp`: 843 → ~200 سطر |
| CW-19 DRY | مصدر واحد بدل مصدرين |
| CW-20 Open/Closed | إضافة خطأ = سطر YAML بدون لمس C++ |
| CW-26 جداول بحث | lookup-table مبنية مرة بدل maps يدوية كبيرة |
| — | أرضية للترجمة لاحقاً، تصدير الكتالوج للـLSP/IDE، تحرير غير-المبرمج للنصوص |

---

## 6. تسلسل التنفيذ

```
يوم 1: EM-1  — استخراج SOT (سكربت + مراجعة 203 رسالة)
يوم 2: EM-2  — المولِّد + Schema + CMake
يوم 3: EM-3  — إعادة كتابة C++ + إعادة بناء + run_tests
يوم 4: EM-4  — اختبارات الإثبات
يوم 5: EM-5  — sadinfo (اختياري)
```

---

## 7. الملفات الجديدة والمُعدَّلة

### جديدة
- `data/language/error_messages.yaml`
- `data/language/error_messages.schema.json`
- `scripts/codegen/gen_error_messages.py`
- `scripts/codegen/migrate_error_messages.py` (مؤقت)
- `scripts/codegen/test_gen_error_messages.py`
- `shared/errors/generated/error_messages_generated.h`
- `shared/errors/generated/error_messages_generated.cpp`
- `shared/errors/generated/error_messages_generated.{h,cpp}.baseline`
- `tests/errors/test_generated_messages.cpp`

### مُعدَّلة
- [cmake/codegen.cmake](cmake/codegen.cmake) — إضافة target جديد
- [shared/errors/src/error_codes.cpp](shared/errors/src/error_codes.cpp) — اختصار + استخدام `findByCode`
- [shared/errors/src/error_catalog_init.cpp](shared/errors/src/error_catalog_init.cpp) — حلقة واحدة على `kErrorMessages`
- [shared/CMakeLists.txt](shared/CMakeLists.txt) — إضافة الملف المولَّد إلى `sad_shared` (إن لزم)
- [memories/repo/yaml_cpp_integration.md](memories/repo/yaml_cpp_integration.md)
- [memories/repo/compiler_fix_notes.md](memories/repo/compiler_fix_notes.md)

---

## 8. خطة التحقق النهائية

1. `python scripts/codegen/gen_error_messages.py --yaml ... --schema ... --header ... --source ...` ينجح بدون أخطاء.
2. `cmake --build build --config Debug --target sad` ينجح.
3. `cmake --build build --config Debug --target sadc` ينجح.
4. `.\run_tests.ps1` → `PASS:137 / FAIL:20` (دون تراجع).
5. `tests/errors/test_generated_messages.cpp` ينجح بالكامل.
6. مقارنة `render(SEM_TYPE_MISMATCH, ...)` قبل/بعد = متطابقة.

---

**حالة الخطة**: مراجعة (CR) أُجريت — انظر القسم 9.

---

## 9. مراجعة الكود/الخطة (Amelia CR)

### 9.1 فجوات مكتشفة في المسودة الأولى

**G-1 (حرج)**: المسودة تشير إلى `errorMessages` map لكن الاسم الفعلي في [shared/errors/src/error_codes.cpp:782](shared/errors/src/error_codes.cpp) هو `errorDescriptions` (نوع `LocalizedString`) + `errorCodeStrings`. يجب تصحيح كل الإشارات.

**G-2 (حرج)**: `getErrorCategory()` في `error_codes.cpp` يستنتج الفئة بـ `codeStr.substr(0, 3)` — يجب أن يصبح بعد الترحيل قراءةً من `findByCode(code)->category` (مصدر واحد، CW-19) بدلاً من إعادة التحليل من string.

**G-3 (متوسط)**: المسودة لم تذكر `getErrorDescription(code, Language)` التي تُستخدم في 5+ مواقع — تحتاج إعادة توجيه إلى `findByCode`.

**G-4 (متوسط)**: لم تُحدَّد استراتيجية لـ `LocalizedString` struct الموجودة في `error_codes.h` — هل تُحذف أم تبقى كـwrapper حول `const char*`؟ **القرار**: تبقى للتوافق الخلفي (BF-15)، تُملأ من `findByCode`.

**G-5 (منخفض)**: المسودة لم تذكر [shared/errors/src/error_catalog.cpp:146](shared/errors/src/error_catalog.cpp) — `render()` يستخدم `getErrorCodeString` كـfallback. لا تأثير لأنه يبقى كما هو.

**G-6 (حرج — أمان)**: استخدام `const char*` في `GeneratedErrorEntry` آمن (string literals لها static storage)، لكن أي تحويل ضمني لـ`std::string` داخل حلقات يجب تجنبه (CW-25). الاستخدام يجب أن يكون `std::string_view` حيث أمكن.

**G-7 (متوسط — ترتيب)**: ترتيب `registerTemplate` في الـmap الحالي قد لا يكون مهماً (unordered)، لكن **ترتيب** الإدخالات في YAML سيُحفَظ في المصفوفة المولَّدة. هذا يضمن قابلية الاستنساخ (reproducibility) ومخرجات `sadinfo --errors` متوقعة (CW-27).

**G-8 (متوسط — ergonomics)**: المسودة لم تذكر كيف يحدث dev workflow: إذا غيّر مطوّر YAML، يجب إعادة بناء target `sad_error_messages_codegen` تلقائياً عبر `add_custom_command` (DEPENDS = YAML). هذا موجود في النمط الحالي — يكفي محاكاته.

**G-9 (منخفض — توثيق)**: المسودة لم تذكر تحديث [.github/copilot-instructions.md](.github/copilot-instructions.md) لإضافة ملاحظة: "إضافة خطأ جديد = تعديل `data/language/error_messages.yaml` فقط، ثم build."

**G-10 (حرج — اختبار)**: AC-3.5 (`PASS:137 / FAIL:20` بالضبط) ليس كافياً وحده. مطلوب أيضاً:
- **golden round-trip test**: قبل التغيير، تشغيل سكربت يطبع كل `(code → render(BRIEF, AR/EN))` ويحفظ snapshot. بعد التغيير، إعادة تشغيله ومقارنته (diff = ∅).
- هذا يكشف أي تغيير صامت في النصوص (BF-22).

### 9.2 قرارات معمارية تحتاج توثيقاً صريحاً

**D-1**: `ErrorCode` enum يبقى مصدراً يدوياً في C++ (لا يُولَّد من YAML في Phase 1) — السبب: enum مستخدم في 200+ موقع كأنواع، وتغييره من YAML يفتح خطر إعادة ترتيب القيم (ABI). **مؤجَّل لـPhase 2**.

**D-2**: ترتيب الإدخالات في `kErrorMessages[]` يحدده YAML — يجب فرض ترتيب: حسب `category` ثم حسب `id` (تصاعدياً). يُضاف فحص في المولِّد.

**D-3**: نص فارغ في YAML (`""`) يُحوَّل إلى `nullptr` في C++ — يبسّط فحص "هل الحقل موجود؟" إلى `entry->detailedAr != nullptr`.

**D-4**: لا تكرار لـ`brief` كـfallback لـ`detailed`/`teacher` في YAML — هذا منطق runtime في `ErrorCatalog::render` يبقى دون تغيير.

### 9.3 تحديثات إلزامية على Stories

**EM-1 → AC إضافي**:
- **AC-1.4**: السكربت `migrate_error_messages.py` يولِّد YAML أولي وملف diff يقارن النصوص المستخرَجة بالأصل (BF-22).
- **AC-1.5**: مراجعة بشرية لكل entry بعد التوليد التلقائي (لا الاعتماد على regex وحده).

**EM-2 → AC إضافي**:
- **AC-2.6**: المولِّد يفرض ترتيب `kErrorMessages[]` (category ثم id).
- **AC-2.7**: المولِّد يحوّل `""` إلى `nullptr` (D-3).

**EM-3 → AC إضافي**:
- **AC-3.6**: `getErrorCategory()` يستخدم `findByCode` بدل `substr` (G-2).
- **AC-3.7**: `getErrorDescription(code, lang)` تُعاد كتابتها لاستخدام `findByCode` (G-3).
- **AC-3.8**: `LocalizedString` تبقى للتوافق الخلفي (G-4, BF-15).

**EM-4 → AC إضافي (حرج)**:
- **AC-4.3**: golden round-trip test — قبل التغيير سجّل كل `render(code, BRIEF, AR)` و `render(code, BRIEF, EN)` لكل 203 ErrorCode. بعد التغيير قارن النتائج (BF-22).
- **AC-4.4**: AddressSanitizer/UBSan على اختبار يستدعي `findByCode` لكل قيمة + قيم خارج النطاق (`static_cast<ErrorCode>(999999)`).

### 9.4 تحسينات على تقدير الجهد

التقدير الأصلي (5 أيام) متفائل. التقدير المنقَّح:

```
يوم 1: EM-1 جزء 1 — كتابة migrate_error_messages.py + توليد YAML أولي
يوم 2: EM-1 جزء 2 — مراجعة بشرية لكل من 203 entries + تنظيف
يوم 3: EM-2 — المولِّد + Schema + اختبار Python
يوم 4: EM-2 تكملة + EM-3 جزء 1 — CMake + إعادة كتابة error_codes.cpp
يوم 5: EM-3 جزء 2 — error_catalog_init.cpp + إصلاح getErrorCategory/Description
يوم 6: EM-4 — golden test + run_tests + إصلاح أي تراجع
يوم 7 (اختياري): EM-5 — sadinfo
```

**الإجمالي**: 5-7 أيام عمل فعلية.

### 9.5 معايير قبول الخطة (Plan-level AC)

- **PLAN-AC-1**: كل ErrorCode في enum له entry في YAML (لا حذف صامت).
- **PLAN-AC-2**: golden round-trip = صفر diffs.
- **PLAN-AC-3**: `run_tests.ps1` = `PASS:137 / FAIL:20` (لا تراجع).
- **PLAN-AC-4**: `error_codes.cpp` ينخفض بـ ≥ 70%.
- **PLAN-AC-5**: `error_catalog_init.cpp` ينخفض بـ ≥ 65%.
- **PLAN-AC-6**: إضافة خطأ تجريبي جديد (مثلاً `TEST_DUMMY_ERROR`) تكلفته = تعديل سطر في YAML + إضافة قيمة في enum + إعادة بناء. لا تعديل في `error_codes.cpp` ولا `error_catalog_init.cpp`.

### 9.6 خلاصة المراجعة

| البند | الحالة |
|---|---|
| المعمارية | ✅ سليمة — تتبع نمط `keywords.yaml` المُجرَّب |
| الفجوات الحرجة | 4 (G-1, G-2, G-6, G-10) — كلها معالجة في 9.1 |
| المخاطر المُغفَلة | 2 (golden test، ترتيب reproducible) — مضافة |
| التقدير | منقَّح من 5 إلى 5-7 أيام |
| **القرار** | ✅ **خطة مقبولة بعد دمج 9.1–9.5** — جاهزة للتنفيذ |

---

**الحالة النهائية**: الخطة + المراجعة جاهزتان. الخطوات التالية تتبع EM-1 → EM-2 → EM-3 → EM-4 بالـAC المنقَّحة.
