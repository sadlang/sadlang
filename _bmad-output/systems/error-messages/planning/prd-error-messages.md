# وثيقة متطلبات المنتج (PRD)
# ترحيل رسائل الأخطاء إلى YAML — Error Messages YAML Migration

**الإصدار**: 1.0.0
**التاريخ**: 2026-05-19
**الحالة**: مُعتمَد للتنفيذ
**المشروع**: لغة ص (s-programming-language)
**المُعِد**: Amelia (bmad-agent-dev)
**المرجع**: [_bmad-output/planning-artifacts/error_messages_yaml_migration.md](../error_messages_yaml_migration.md)

---

## 1. ملخص تنفيذي

### المشكلة
رسائل الأخطاء في لغة ص (203 ErrorCode) مُعرَّفة يدوياً عبر ملفين منفصلين:
- [shared/errors/src/error_codes.cpp](../../../shared/errors/src/error_codes.cpp) — 843 سطر فيها جدولان كبيران (`errorCodeStrings` + `errorDescriptions`)
- [shared/errors/src/error_catalog_init.cpp](../../../shared/errors/src/error_catalog_init.cpp) — 213 سطر فيها 26 template غني فقط (من أصل 203)

النتائج السلبية:
- **177 خطأ بلا template غني** (87% من الأخطاء) — تظهر بنص افتراضي عام بدون `fixHint` ولا `codeExample`.
- إضافة خطأ جديد = تعديل 3 ملفات.
- تكرار نصوص (انتهاك CW-19 DRY).
- استحالة إضافة لغة ثالثة بدون تغيير C++ (انتهاك CW-20 Open/Closed).
- استحالة استخراج الكتالوج للـLSP/IDE بدون parsing لـC++.

### الحل
نقل كل رسائل الأخطاء إلى ملف YAML واحد (`data/language/error_messages.yaml`) كمصدر وحيد للحقيقة (SOT)، مع توليد C++ تلقائياً عبر سكربت Python — على نمط [data/language/keywords.yaml](../../../data/language/keywords.yaml) المُجرَّب.

### القيمة المُضافة
- **مصدر وحيد**: تعديل خطأ = سطر YAML واحد + إعادة بناء
- **بدون C++**: إضافة 177 fixHint و codeExample المفقودة بدون لمس أي ملف `.cpp`
- **قابلية الاستخراج**: `sadinfo --errors --format=json` للـLSP/IDE
- **أرضية للترجمة**: إضافة لغة ثالثة = إضافة حقول `*.fr` في YAML

---

## 2. الأهداف والمعايير

### أهداف قابلة للقياس

| الهدف | القياس | المستهدف |
|---|---|---|
| تقليص `error_codes.cpp` | عدد الأسطر | < 250 (من 843) — تقليص ≥ 70% |
| تقليص `error_catalog_init.cpp` | عدد الأسطر | < 80 (من 213) — تقليص ≥ 65% |
| تغطية fixHint | عدد ErrorCodes لها fixHint | ≥ 100 (من 26 حالياً) |
| لا تراجع | `run_tests.ps1` | `PASS:137 / FAIL:20` بالضبط |
| لا تغيير سلوكي | golden round-trip diff | 0 diffs |
| سهولة إضافة خطأ | عدد ملفات C++ المُعدَّلة | 0 (فقط YAML + enum) |

### خارج النطاق (Out of Scope)
- توليد `ErrorCode` enum من YAML (مؤجَّل لـPhase 2 — مخاطر ABI)
- إضافة لغة ثالثة (مدعوم في التصميم لكن غير منفَّذ)
- Hot-reload وقت التشغيل
- ربط بـsadinfo (Story EM-5 اختياري)

---

## 3. متطلبات وظيفية

### FR-01: ملف SOT (Single Source of Truth)
- **ما**: `data/language/error_messages.yaml` يحوي كل الـ203 ErrorCode.
- **بنية**:
  ```yaml
  errors:
    - code: SEM_TYPE_MISMATCH       # يطابق ErrorCode enum
      id: "SEM002"                  # يطابق ^[A-Z]{2,3}\d{3}$
      category: semantic
      title:    { ar: "...", en: "..." }
      brief:    { ar: "...", en: "..." }
      fix_hint: { ar: "...", en: "..." }   # اختياري
      detailed: { ar: "...", en: "..." }   # اختياري
      teacher:  { ar: "...", en: "..." }   # اختياري
      code_example: |                       # اختياري
        كود لغة ص
      placeholders: [expected, found]
  ```

### FR-02: JSON Schema
- **ما**: `data/language/error_messages.schema.json` (draft-07).
- **يفرض**: تفرّد `code` و `id`، نمط `id`، إلزامية `title.ar/en` و `brief.ar/en`.

### FR-03: المولِّد (Generator)
- **ما**: `scripts/codegen/gen_error_messages.py`.
- **يستخدم**: `_lib/loader.py` و `_lib/emit.py` (موجودة).
- **يُنتج**: `shared/errors/generated/error_messages_generated.{h,cpp}`.
- **تحقُّقات دلالية**:
  - كل `code` في YAML موجود في `ErrorCode` enum (يقرأ `error_codes.h` بـregex).
  - كل `{placeholder}` في النصوص مذكور في `placeholders` array.
  - كل `id` يبدأ بـprefix فئته.
  - ترتيب الإدخالات: حسب `category` ثم `id`.
  - نص فارغ `""` يُحوَّل إلى `nullptr`.

### FR-04: تكامل CMake
- **ما**: `cmake/codegen.cmake` يحوي target `sad_error_messages_codegen`.
- **يطابق نمط**: `sad_keywords_codegen` (موجود).
- **يضمن**: تحديث YAML يُعيد التوليد تلقائياً عبر `add_custom_command(DEPENDS yaml schema script)`.

### FR-05: Baseline في git
- **ما**: `error_messages_generated.{h,cpp}.baseline` ملتزمة في git.
- **لماذا**: البناء يعمل بدون Python (نمط `keywords_generated.cpp.baseline`).

### FR-06: استخدام الكود المُولَّد
- **ما**:
  - `getErrorCodeString(code)` يصبح `findByCode(code)->id`.
  - `getErrorDescription(code, lang)` يصبح `findByCode(code)->briefAr/En`.
  - `getErrorCategory(code)` يصبح `findByCode(code)->category` (لا `substr`).
  - `ErrorCatalog::registerDefaults()` يصبح حلقة على `kErrorMessages`.

### FR-07: Lookup كفؤ
- **ما**: `findByCode(ErrorCode)` بـ `O(1)` عبر `std::unordered_map` مبني مرة عند الـstartup (CW-26).

---

## 4. متطلبات غير وظيفية

| الفئة | المتطلب | القياس |
|---|---|---|
| الأداء | زمن startup الإضافي | < 5 ms |
| الذاكرة | حجم الـbinary | ≤ +20 KB (نصوص literals) |
| الترميز | UTF-8 صحيح لكل النصوص العربية | golden test |
| التوافق | لا كسر API عام | `BF-15` — كل الدوال العامة تحتفظ بتوقيعها |
| الاختبار | تغطية الـcatalog | 100% (كل ErrorCode يُختبر) |

---

## 5. خارج النطاق (Out of Scope) — مفصَّل

- **Phase 2**: توليد `ErrorCode` enum من YAML.
- **Phase 2**: ترجمة لغة ثالثة (fr/de).
- **Phase 2**: تكامل LSP/IDE مباشر.
- **Phase 3**: تحرير الرسائل عبر واجهة ويب.

---

## 6. التبعيات

### تقنية
- Python 3.9+ مع `pyyaml` و `jsonschema` (موجود).
- CMake 3.20+ (موجود).
- البنية التحتية لـcodegen في [scripts/codegen/_lib/](../../../scripts/codegen/_lib/) (موجودة).

### وثائقية
- نمط مرجعي: [data/language/keywords.yaml](../../../data/language/keywords.yaml) + [scripts/codegen/gen_keywords.py](../../../scripts/codegen/gen_keywords.py).
- قواعد الكود: [.github/copilot-instructions.md](../../../.github/copilot-instructions.md) (CW-04, CW-19, CW-20, CW-26).
- قواعد الإصلاح: BF-15 (لا كسر API)، BF-22 (اختبار سلبي/golden)، BF-29 (لا تراجع).

---

## 7. القرارات المعمارية

| ID | القرار | السبب |
|---|---|---|
| D-1 | `ErrorCode` enum يبقى يدوياً في Phase 1 | enum مستخدم في 200+ موقع — مخاطر ABI |
| D-2 | ترتيب `kErrorMessages[]` = category ثم id | reproducibility + `sadinfo --errors` متوقع (CW-27) |
| D-3 | `""` في YAML → `nullptr` في C++ | يبسّط فحص "هل الحقل موجود؟" |
| D-4 | `LocalizedString` struct يبقى | توافق خلفي (BF-15) — يُملأ من `findByCode` |
| D-5 | `const char*` في GeneratedErrorEntry (لا `std::string`) | string literals لها static storage — لا allocations (CW-25) |

---

## 8. معايير القبول (Plan-level)

- **PLAN-AC-1**: كل ErrorCode في enum له entry في YAML (لا حذف صامت).
- **PLAN-AC-2**: golden round-trip = 0 diffs.
- **PLAN-AC-3**: `run_tests.ps1` = `PASS:137 / FAIL:20` (لا تراجع).
- **PLAN-AC-4**: `error_codes.cpp` ≤ 250 سطر (تقليص ≥ 70%).
- **PLAN-AC-5**: `error_catalog_init.cpp` ≤ 80 سطر (تقليص ≥ 65%).
- **PLAN-AC-6**: إضافة خطأ تجريبي = تعديل YAML + enum فقط (لا `.cpp`).
- **PLAN-AC-7**: المولِّد يفشل عند placeholder غير مُعرَّف (تحقُّق دلالي).
- **PLAN-AC-8**: المولِّد يفشل عند `code` غير موجود في enum.

---

## 9. المخاطر

| المخاطرة | الاحتمال | الأثر | المخفِّف |
|---|---|---|---|
| فقدان رسائل أثناء الترحيل | متوسط | عالٍ | `migrate_error_messages.py` + diff تلقائي + مراجعة بشرية |
| تغيير صامت في النصوص | متوسط | عالٍ | golden round-trip (BF-22) |
| تباطؤ startup | منخفض | منخفض | قياس قبل/بعد |
| UTF-8 في C++ literals | منخفض | متوسط | `cpp_string_literal` من `_lib/emit.py` |
| مولِّد مفقود في CI نظيفة | منخفض | عالٍ | baseline files في git |
| تصادم placeholders | منخفض | متوسط | فحص دلالي بـregex |
| كسر API الحالي | منخفض | عالٍ | الواجهة لا تتغير — فقط مصدر البيانات |

---

## 10. خطة الإصدار

| المرحلة | المدة المقدَّرة | النتيجة |
|---|---|---|
| Phase 1.1 (EM-1) | يوم 1-2 | YAML + Schema + 203 entry مُراجَعة |
| Phase 1.2 (EM-2) | يوم 3 | المولِّد + CMake + baseline |
| Phase 1.3 (EM-3) | يوم 4-5 | تكامل + تقليص C++ |
| Phase 1.4 (EM-4) | يوم 6 | golden test + run_tests |
| Phase 1.5 (EM-5) | يوم 7 (اختياري) | sadinfo --errors |

**الإجمالي**: 5-7 أيام عمل فعلية.

---

**حالة الوثيقة**: مُعتمَد للتنفيذ.
