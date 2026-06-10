# تخطيط Story 1.2 — توسيع `--dump-keywords`

> **النسخة:** 1.0 (تخطيط مفصل قبل التنفيذ)
> **المرجع المختصر:** [epics-docs-system-v2.md §Story 1.2](../planning-artifacts/epics-docs-system-v2.md)
> **التقدير الأصلي:** 1 يوم — **بعد التفصيل:** 1.5 يوم (لوجود فجوات بيانات)
> **الحالة:** قيد المراجعة قبل التنفيذ

---

## 1) ملخص

توسيع أمر `sadinfo --dump-keywords` من إخراج JSON واحد مسطح (مُنجَز في 1.1) إلى نظام مرن يدعم:
- **خيارات الإخراج:** `yaml`, `--lang=ar|en`, `--minimal`, `--by-category`, `--pretty/--compact`, `--output`.
- **خيارات التصفية:** `--filter category=X`, `--filter role=Y`, `--filter token-type=Z`, `--include-deprecated`.
- **حقول جديدة:** `id` (ثابت)، `since` (إصدار اللغة).
- **ضمانات:** Idempotent (byte-identical عند التشغيل المتكرر) + اختبارات وحدة.

---

## 2) فحص الواقع — ماذا يوجد فعلاً؟

### 2.1 المتاح في `Sad::Lexer::Generated::KeywordEntry`
| الحقل | النوع | حالة 1.1 | متاح لـ 1.2؟ |
|--------|------|----------|----------|
| `primaryWord` | string | ✅ يُصدَر كـ `word` | ✅ |
| `english` | string | ✅ يُصدَر | ✅ |
| `category` | enum (4) | ✅ يُصدَر كـ string | ✅ |
| `type` (TokenType) | enum | ✅ يُصدَر كـ `tokenType` | ✅ |
| `emittedByLexer` | bool | ✅ يُصدَر | ✅ |
| `aliases` | vector<string> | ✅ يُصدَر | ✅ |
| `roles` | vector<string> | ✅ يُصدَر | ✅ |

### 2.2 الفجوات — ما يطلبه AC ولكنه **غير موجود**
| المطلوب في AC | المصدر المتوقع | الحالة | الإجراء |
|----------------|----------------|--------|---------|
| `id` (مُعرّف عددي ثابت) | ❌ غير موجود في YAML أو الـ struct | فجوة | **استراتيجية A:** اشتقاقه من `tokenType` (مستقر). **استراتيجية B:** إضافة حقل `id` لـ YAML + توليده. **مختار:** A (لا تعديل على SOT). |
| `since` (إصدار اللغة) | ❌ غير موجود | فجوة | **استراتيجية A:** قيمة افتراضية ثابتة `"1.0"` لكل الكلمات الحالية (90/91)، مع TODO لإضافة الحقل لـ YAML في v4.2. **مختار:** A. |
| `--format=yaml` | لا منتج YAML في sadinfo | فجوة كود | كتابة `YamlWriter` بسيط (مبسط، لا dependency). |
| `--include-deprecated` | لا حقل `deprecated` في YAML | فجوة بيانات | **مختار:** قبول العلامة لكن دائماً تشمل (لا توجد كلمات مُهمَلة حالياً). موثَّق. |

### 2.3 أنواع التصفية المُقترحة
- `--filter category=reserved|operator|contextual|builtin_type` (تصفية بفئة واحدة)
- `--filter role=block_opener|block_closer|inter_block|import_keyword` (تصفية بدور)
- `--filter token-type=KEYWORD_IF` (مطابقة tokenType حرفية)
- `--filter has-aliases` (الكلمات التي لها aliases فقط)
- يمكن دمج عدة `--filter` (AND منطقي)

---

## 3) معايير القبول التفصيلية (مُحدَّثة من 9 إلى 18)

### المجموعة A — الإخراج الأساسي (محتفظ به من 1.1)
- **AC-A1:** `sadinfo --dump-keywords` بدون خيارات يُنتج JSON صالح بحقل `keywords` يحوي 91 إدخالاً (متطابق مع الإخراج الحالي).
- **AC-A2:** الإخراج الافتراضي `pretty` (مسافة بادئة 2)، و `--compact` يُنتج سطراً واحداً بدون مسافات إضافية.
- **AC-A3:** `--output file.json` يكتب لملف بترميز UTF-8 بدون BOM، نفس البايتات تماماً للإخراج إلى stdout.

### المجموعة B — اللغات والتنسيقات
- **AC-B1:** `--format json` (افتراضي) و `--format yaml` يُنتجان نفس البيانات بترميزين مختلفين.
- **AC-B2:** `--lang ar` يحذف الحقل `english` من كل entry.
- **AC-B3:** `--lang en` يحتفظ بـ `english` ويبقي `word` (لأنه المُعرّف الأساسي).
- **AC-B4:** `--lang both` (افتراضي) يبقي كل الحقول.
- **AC-B5:** قيمة غير صالحة لـ `--lang` تُنتج خطأ على stderr مع exit=2.

### المجموعة C — الحقول الجديدة
- **AC-C1:** كل entry يحوي حقل `id` نوع string، مشتق من tokenType (مثلاً `"keyword.if"` لـ `KEYWORD_IF`، `"operator.and"` لـ `OP_AND`، `"contextual.async"` لكلمة `غير_متزامن`).
- **AC-C2:** كل entry يحوي حقل `since` نوع string، قيمة `"1.0"` لجميع الـ 91 كلمة الحالية.
- **AC-C3:** `--minimal` يحتفظ فقط بـ `word`, `tokenType`, `category` ويحذف باقي الحقول.

### المجموعة D — التصفية
- **AC-D1:** `--filter category=reserved` يُنتج بالضبط 40 entry كلها reserved.
- **AC-D2:** `--filter category=operator` يُنتج بالضبط 3 entries (`و`, `أو`, `ليس`).
- **AC-D3:** `--filter category=contextual` يُنتج بالضبط 39 entries.
- **AC-D4:** `--filter category=builtin_type` يُنتج بالضبط 9 entries.
- **AC-D5:** `--filter role=block_opener` يُنتج كل الكلمات التي تحوي `block_opener` في `roles`.
- **AC-D6:** فلترات متعددة `--filter category=reserved --filter role=block_opener` تطبق AND.
- **AC-D7:** فلتر بقيمة غير معروفة (مثلاً `category=foo`) يُنتج مصفوفة فارغة + رسالة تحذير على stderr (exit=0).

### المجموعة E — التجميع
- **AC-E1:** `--by-category` يحوّل البنية من `{keywords: [...]}` إلى `{categories: {reserved: [...], operator: [...], contextual: [...], builtin_type: [...]}}`.
- **AC-E2:** `--by-category --minimal` يعمل معاً (التجميع + تقليل الحقول).

### المجموعة F — الجودة
- **AC-F1:** Idempotent: تشغيل نفس الأمر مرتين يُنتج بايتات متطابقة (بما فيها ترتيب الـ keys في JSON).
- **AC-F2:** كل ترتيب: `keywords` array بترتيب YAML الأصلي (لا فرز عشوائي).
- **AC-F3:** اختبار وحدة C++ في `tests/sadinfo/test_dump_keywords.cpp` يفحص:
  - عدد الإدخالات لكل فئة.
  - حضور الحقول الجديدة `id` و `since`.
  - صحة JSON المُنتَج (parse-roundtrip).
- **AC-F4:** اختبار snapshot ذهبي: ملف `tests/sadinfo/golden/keywords_default.json` يُقارن byte-by-byte.

---

## 4) خطة التنفيذ التفصيلية

### Task 1 — توسعة DumpOptions struct (15 دقيقة)
في [tools/sadinfo/include/sadinfo.h](../../tools/sadinfo/include/sadinfo.h):
```cpp
struct DumpOptions {
    bool        compact         = false;
    std::string outputPath;          // فارغ = stdout
    std::string format          = "json";    // json | yaml
    std::string lang            = "both";    // ar | en | both
    bool        minimal         = false;
    bool        byCategory      = false;
    bool        includeDeprecated = false;   // قبول الراية فقط
    std::vector<std::string> filters;        // ["category=reserved", "role=block_opener"]
};
```

### Task 2 — توسعة CLI parser (30 دقيقة)
في `tools/sadinfo/src/cli.cpp`:
- إضافة معالجة `--format`, `--lang`, `--minimal`, `--by-category`, `--filter`, `--include-deprecated`.
- التحقق من القيم المسموحة + رسائل خطأ ثنائية اللغة.

### Task 3 — اشتقاق `id` من tokenType (45 دقيقة)
دالة جديدة `tokenTypeToStableId(TokenType, KeywordCategory)`:
- `KEYWORD_IF` → `"keyword.if"`
- `KEYWORD_FUNCTION` → `"keyword.function"`
- `OP_AND` → `"operator.and"`
- `IDENTIFIER` (سياقية) → `"contextual.<english>"` أو `"contextual.<word>"` كـ fallback.
- `IDENTIFIER` (builtin_type) → `"type.<english>"`.

### Task 4 — تطبيق التصفية (30 دقيقة)
دالة `applyFilters(entries, filters) -> entries`:
- تحليل كل filter إلى `(key, value)`.
- بناء lambda مركّب يطبق كل الشروط (AND).
- إرجاع نسخة مُصفّاة (`std::vector<KeywordEntry>` بالنسخ).

### Task 5 — كاتب YAML بسيط (45 دقيقة)
`tools/sadinfo/src/yaml_writer.{h,cpp}`:
- لا dependency خارجي.
- يدعم: scalars (string, int, bool), arrays, nested maps.
- escape للسلاسل التي تحوي `:` أو `#` أو `\n`.

### Task 6 — تحديث dump_keywords.cpp (1 ساعة)
- استدعاء التصفية.
- شعبة `--by-category` (تجميع).
- شعبة `--minimal` (حذف حقول).
- شعبة `--lang` (حذف english).
- اختيار writer (JSON أو YAML).
- إضافة `id` و `since` لكل entry.

### Task 7 — اختبارات الوحدة (1 ساعة)
`tests/sadinfo/test_dump_keywords.cpp`:
- اختبار افتراضي.
- اختبار كل فلتر.
- اختبار `--minimal`، `--by-category`.
- اختبار idempotency.
- ملفات golden في `tests/sadinfo/golden/`.

### Task 8 — توثيق (30 دقيقة)
تحديث `tools/sadinfo/README.md` مع جدول كل الخيارات + 6 أمثلة عملية.

**الإجمالي:** ~5 ساعات صافي + 1 ساعة احتياط = **1.5 يوم عمل**.

---

## 5) المخاطر والاختيارات المعمارية

### 5.1 خطر: تغيير شكل الـ JSON الافتراضي
- **الوضع الحالي (1.1):** الإخراج هو `{schemaVersion, tool, toolVersion, totalCount, categoryCounts, keywords:[...]}`.
- **التغيير:** إضافة `id` و `since` لكل entry يكسر أي مستهلك يفترض ترتيب/حقول الإصدار 1.0.0.
- **القرار:** زيادة `kSchemaVersion` من `"1.0.0"` إلى `"1.1.0"` (إضافة backward-compatible).

### 5.2 خطر: عدم استقرار `id` المُشتقّ
- إذا تغيّر اسم `tokenType` لاحقاً (مثلاً `KEYWORD_IF` → `KEYWORD_CONDITIONAL`)، تتغيّر كل الـ ids.
- **التخفيف:** توثيق صريح في README أن الـ id مستقر طالما لم يتغيّر `TokenType` enum، وهذا لن يحدث (CW-24: التوافقية الخلفية).

### 5.3 خطر: kacheen YAML writer يُنتج YAML غير صحيح
- **التخفيف:** اختبار roundtrip بـ `pyyaml` في Python script ضمن CI.

### 5.4 قرار: `--include-deprecated` بلا أثر فعلي
- لا توجد كلمات مُهمَلة حالياً، لكن العلامة مطلوبة في AC.
- **القرار:** قبول العلامة (لا خطأ) + تحذير stderr "no deprecated keywords in this version" + مرفقة في README.

---

## 6) قائمة التحقق قبل البدء

- [ ] هل التغييرات في `kSchemaVersion` (1.0.0 → 1.1.0) مقبولة؟
- [ ] هل اشتقاق `id` من `TokenType` (بدون تعديل YAML) مقبول؟
- [ ] هل القيمة الافتراضية `since: "1.0"` للكل مقبولة (مع TODO لإضافة الحقل في YAML v4.2)؟
- [ ] هل كاتب YAML داخلي بسيط مقبول، أو يُفضّل dependency خارجي (`yaml-cpp`)؟
- [ ] هل تكلفة 1.5 يوم (بدلاً من 1 يوم في الخطة الأصلية) مقبولة؟

---

## 7) ما لن يُنفَّذ في 1.2 (مؤجَّل)

- ❌ **حقل `description`** (شرح الكلمة): لا يوجد في YAML، يحتاج إضافة كاملة في v4.2.
- ❌ **حقل `examples`** (أمثلة استخدام): يحتاج محرر منفصل، مؤجل لـ Story 1.6.
- ❌ **التصفية بـ regex** (`--filter word~=^إذا$`): تعقيد زائد، نُكتفي بالمطابقة الحرفية.
- ❌ **الإخراج إلى stdout مع `--output`**: متعارض دلالياً، نرفضه بخطأ.

---

## 8) قرار

**هل نمضي قدماً؟** بعد إجابة المستخدم على نقاط القائمة §6:
- إذا "نعم" → بدء التنفيذ بحسب Tasks 1-8.
- إذا "تعديل" → تحديث هذه الوثيقة ثم البدء.
- إذا "تأجيل" → الانتقال لـ Story 1.3 أو 1.4.
