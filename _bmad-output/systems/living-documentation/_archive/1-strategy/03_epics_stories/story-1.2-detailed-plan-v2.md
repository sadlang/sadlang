# تخطيط Story 1.2 — توسيع `--dump-keywords` (النسخة النهائية)

> **النسخة:** 2.0 (مُحدَّثة بعد حفلة الوكلاء)
> **يستبدل:** v1.0 (لم يعد ساري المفعول)
> **التقدير النهائي:** 1.75 يوم عمل (دفعتين)
> **الحالة:** بانتظار الموافقة النهائية للبدء
> **مرجع الحفلة:** [story-1.2-agent-party-review.md](./story-1.2-agent-party-review.md)

---

## 1) الملخص التنفيذي

توسيع `sadinfo --dump-keywords` من إخراج JSON واحد مسطح (مُنجَز في 1.1) إلى نظام مرن يدعم:
- **خيارات الإخراج:** JSON/YAML، pretty/compact، lang ar/en/both، minimal، by-category.
- **خيارات التصفية:** filter متعدد (category/role/token-type/has-aliases) + include-deprecated.
- **حقول جديدة:** `id` (مشتق من english، مستقر للأبد) + `deprecated: true` للمُهمَلة.
- **ضمانات:** Idempotent، snapshot tests، ترتيب حقول ثابت، CI gate لـ ids.

**مخرجات نهائية:**
- ✅ `sadinfo.exe --dump-keywords` بكل الخيارات أعلاه.
- ✅ schema 1.1.0 + tool 0.2.0.
- ✅ ملف ذهبي `tests/sadinfo/golden/keyword_ids.json` يحوي قائمة الـ 91 id.
- ✅ مكتبة yaml-cpp مدمجة في sadinfo.
- ✅ 23 AC مُتحقَّق منها بـ unit tests.

---

## 2) فحص الواقع — البيانات المتاحة والفجوات

### 2.1 المتاح في `Sad::Lexer::Generated::KeywordEntry`
| الحقل | حالة 1.1 | حالة 1.2 |
|------|----------|----------|
| `primaryWord` | ✅ يُصدَر كـ `word` | ✅ يبقى |
| `english` | ✅ يُصدَر | ✅ يبقى + يُستخدم لاشتقاق `id` |
| `category` | ✅ enum→string | ✅ يبقى |
| `type` (TokenType) | ✅ يُصدَر كـ `tokenType` | ✅ يبقى + يُستخدم لكشف deprecated |
| `emittedByLexer` | ✅ يُصدَر | ✅ يبقى |
| `aliases` | ✅ يُصدَر | ✅ يبقى |
| `roles` | ✅ يُصدَر | ✅ يبقى |

### 2.2 الفجوات المُعالَجة في الحفلة
| المطلوب | الاستراتيجية المعتمدة | السبب |
|---------|------------------------|-------|
| `id` ثابت | اشتقاق من `(category, english)` — ليس من `TokenType` | `english` أكثر استقراراً (يدوم معجمياً)، `TokenType` قابل لإعادة التسمية. |
| `since` | تأجيل — افتراضي `"unspecified"` (أو حذف) | بيانات تاريخية غير دقيقة؛ يحتاج YAML v4.2 + Story لاحق. |
| `deprecated` | اكتشاف تلقائي عبر pattern `*_DEPRECATED$` على `tokenType` | `KEYWORD_GIVE_DEPRECATED` (اعطِ) موجود فعلياً. |

### 2.3 صيغة اشتقاق الـ `id` (المُعتمدة)
```
reserved      → "keyword."   + english.toLowerCase()    # KEYWORD_IF        → "keyword.if"
operator      → "operator."  + english.toLowerCase()    # OP_AND            → "operator.and"
contextual    → "contextual."+ english.toLowerCase()    # KEYWORD_ASYNC     → "contextual.async"
builtin_type  → "type."      + english.toLowerCase()    # TYPE_INTEGER      → "type.integer"

Special handling:
- english="true"/"false"/"null"  → "literal.<english>"  # حالات خاصة لـ LITERAL_*
- english ينتهي بـ "_deprecated" → نحذف اللاحقة قبل الاستخدام
- english يحوي مسافات/شرطات    → نُحوِّل إلى snake_case lower

أمثلة:
  give_deprecated → "contextual.give"
  end_with        → "contextual.end_with"
  ui_decl         → "contextual.ui_decl"
```

---

## 3) معايير القبول النهائية (23 AC في 6 مجموعات)

### المجموعة A — الإخراج الأساسي
- **AC-A1:** `sadinfo --dump-keywords` بدون خيارات يُنتج JSON صالح بـ 90 entry (deprecated مُستبعَدة افتراضياً).
- **AC-A2:** `--compact` يُنتج سطراً واحداً؛ افتراضي pretty (مسافة 2).
- **AC-A3:** `--output file.json` يكتب UTF-8 بدون BOM، نفس البايتات تماماً للإخراج إلى stdout.

### المجموعة B — اللغات والتنسيقات والإهمال
- **AC-B1:** `--format json` (افتراضي) و `--format yaml` يُنتجان نفس البيانات.
- **AC-B2:** `--lang ar` يحذف `english`.
- **AC-B3:** `--lang en` يبقي `word` و `english` (word مفتاح أساسي وليس ترجمة).
- **AC-B4:** `--lang both` (افتراضي) يبقي كل الحقول.
- **AC-B5:** `--lang foo` → exit=2 + رسالة على stderr.
- **AC-B6:** بدون `--include-deprecated` → 90 entry (deprecated محذوفة).
- **AC-B7:** مع `--include-deprecated` → 91 entry + الحقل `"deprecated": true` لـ `KEYWORD_GIVE_DEPRECATED`.

### المجموعة C — الحقول الجديدة
- **AC-C1:** كل entry يحوي `id` نوع string، مُشتق وفق §2.3.
- **AC-C2:** ❌ ~~`since` لكل entry~~ → مؤجَّل لـ v4.2 (TODO في README).
- **AC-C3:** `--minimal` يحتفظ فقط بـ `word`, `tokenType`, `category` ويحذف الباقي.
- **AC-C4:** entries المُهمَلة (مع `--include-deprecated`) تحوي `"deprecated": true` بعد `emittedByLexer`.

### المجموعة D — التصفية
- **AC-D1:** `--filter category=reserved` → 40 entry.
- **AC-D2:** `--filter category=operator` → 3 entries.
- **AC-D3:** `--filter category=contextual` → 38 entries (39 - 1 deprecated بالافتراض). مع `--include-deprecated`: 39.
- **AC-D4:** `--filter category=builtin_type` → 9 entries.
- **AC-D5:** `--filter role=block_opener` → كل الكلمات بدور `block_opener`.
- **AC-D6:** فلاتر متعددة `--filter category=reserved --filter role=block_opener` → AND منطقي.
- **AC-D7:** `--filter category=foo` → مصفوفة فارغة + تحذير stderr، exit=0.
- **AC-D8:** `--filter category` (بدون `=`) → exit=2 + رسالة "missing value for filter key".

### المجموعة E — التجميع
- **AC-E1:** `--by-category` → بنية `{categories: {reserved:[...], operator:[...], contextual:[...], builtin_type:[...]}}`.
- **AC-E2:** `--by-category --minimal` يعمل معاً.

### المجموعة F — الجودة
- **AC-F1:** Idempotent: تشغيلان متتاليان يُنتجان بايتات متطابقة.
- **AC-F2:** ترتيب الـ entries في `keywords` array = ترتيب YAML الأصلي.
- **AC-F3:** ترتيب الحقول داخل entry ثابت: `id, word, english, category, tokenType, emittedByLexer, deprecated?, aliases, roles`.
- **AC-F4:** Unit tests في `tests/sadinfo/test_dump_keywords.cpp` تغطي: عدد الـ entries لكل فئة، حضور الحقول الجديدة، صحة JSON parse-roundtrip.
- **AC-F5:** Snapshot ذهبي: `tests/sadinfo/golden/keyword_ids.json` يحوي قائمة الـ 91 id (مع deprecated).
- **AC-F6:** Snapshot ذهبي: `tests/sadinfo/golden/keywords_default.json` يُقارن byte-by-byte.

---

## 4) خطة التنفيذ — دفعتين

### 🟢 الدفعة 1 — Filters + IDs (4 ساعات)

**الهدف:** sadinfo يُصدر JSON كامل مع `id` + كل الفلاتر، بدون yaml-cpp بعد.

| Task | الوصف | الوقت |
|------|--------|-------|
| **T1** | توسعة `DumpOptions` struct في `tools/sadinfo/include/sadinfo.h` | 15m |
| **T2** | توسعة CLI parser في `tools/sadinfo/src/cli.cpp` (إضافة `--lang`, `--filter`, `--minimal`, `--by-category`, `--include-deprecated`) | 30m |
| **T3** | إنشاء `tools/sadinfo/src/keyword_transforms.{h,cpp}` يحوي:<br>- `deriveStableId(category, english) -> string`<br>- `isDeprecated(tokenType) -> bool`<br>- `applyFilters(entries, filters) -> entries`<br>- `groupByCategory(entries) -> map<string, entries>`<br>- `applyMinimal(entry) -> entry` | 1h 30m |
| **T4** | تحديث `dump_keywords.cpp` لاستخدام التحويلات + إخراج الحقول الجديدة + الترتيب الثابت | 1h |
| **T5** | إنشاء snapshot أولي `tests/sadinfo/golden/keyword_ids.json` (91 id) | 15m |
| **T6** | تحديث `kSchemaVersion` → `"1.1.0"` و `kToolVersion` → `"0.2.0"` | 5m |
| **T7** | بناء + اختبار يدوي (3 سيناريوهات على الأقل) | 25m |

**نقطة فحص (Checkpoint):** عرض النتائج للمستخدم قبل الانتقال للدفعة 2.

### 🟡 الدفعة 2 — YAML + Tests + Docs (7-8 ساعات)

| Task | الوصف | الوقت |
|------|--------|-------|
| **T8** | إضافة yaml-cpp إلى cmake (FetchContent أو vcpkg) + linking | 1h 30m |
| **T9** | إنشاء `tools/sadinfo/src/dump_writers.{h,cpp}` مع وظيفتين: `writeJson(value, options)` و `writeYaml(value, options)` | 1h 30m |
| **T10** | تحديث `dump_keywords.cpp` لاختيار writer حسب `--format` | 30m |
| **T11** | كتابة unit tests كاملة في `tests/sadinfo/test_dump_keywords.cpp` (تغطية AC-A* AC-B* AC-C* AC-D* AC-E*) | 2h |
| **T12** | إنشاء snapshot `tests/sadinfo/golden/keywords_default.json` + اختبار byte-comparison | 30m |
| **T13** | تحديث `tools/sadinfo/CMakeLists.txt` لربط test target | 30m |
| **T14** | تحديث `tools/sadinfo/README.md` مع جدول كامل لكل الخيارات + 8 أمثلة عملية | 45m |
| **T15** | بناء كامل + تشغيل كل الاختبارات + إصلاح أي تراجع | 45m |

**المجموع:** 4h + 8h = **12 ساعة صافي ≈ 1.5 يوم تركيز** + 4h احتياط = **1.75 يوم**.

---

## 5) قرارات معمارية مُلزِمة

### 5.1 ترتيب الحقول الثابت (CW-09 + idempotency)
داخل كل entry، الترتيب **بهذا الترتيب الحرفي**:
```json
{
  "id":              "...",
  "word":            "...",
  "english":         "...",        // محذوف عند --lang ar
  "category":        "...",
  "tokenType":       "...",
  "emittedByLexer":  true,
  "deprecated":      true,         // فقط إذا deprecated وموجود مع --include-deprecated
  "aliases":         [...],        // محذوف إذا فارغ
  "roles":           [...]         // محذوف إذا فارغ
}
```

### 5.2 ترقية الإصدارات
- `kSchemaVersion`: `"1.0.0"` → `"1.1.0"` (إضافة backward-compatible: `id` + `deprecated`).
- `kToolVersion`: `"0.1.0"` → `"0.2.0"` (إضافة ميزات).
- توثيق في `tools/sadinfo/CHANGELOG.md` (يُنشأ في T14).

### 5.3 stdout vs stderr (CW-22)
- **stdout:** JSON/YAML فقط (machine-parseable).
- **stderr:** تحذيرات (filter بقيمة غير معروفة)، أخطاء (lang foo)، معلومات (deprecated count).

### 5.4 الاعتماديات الجديدة
- **yaml-cpp:** عبر `FetchContent` في cmake (لا تثبيت يدوي مطلوب).
- لا dependencies أخرى. sadinfo يبقى يربط `sad_shared` فقط + `yaml-cpp`.

---

## 6) المخاطر والتخفيف

| الخطر | الاحتمال | الأثر | التخفيف |
|------|----------|-------|----------|
| yaml-cpp يفشل في البناء على Windows MSVC | متوسط | عالي | fallback: writer داخلي بسيط (45m إضافية) |
| تغيير `english` لإحدى الكلمات يكسر `id` | منخفض | متوسط | snapshot test + CI gate (Story 1.6) |
| تكلفة yaml-cpp تتجاوز التقدير | متوسط | منخفض | احتياط 4h موجود |
| اختبارات golden تفشل بشكل متكرر بسبب ترتيب JSON | عالي | متوسط | استخدام `nlohmann::json` الموجود + set ordered keys صراحة |

---

## 7) ما لن يُنفَّذ في 1.2 (مؤجَّل صراحة)

| البند | السبب | متى |
|-------|-------|-----|
| حقل `since` | بيانات تاريخية غير دقيقة | YAML v4.2 + Story منفصل |
| حقل `description` (شرح الكلمة) | لا يوجد في YAML | YAML v4.2 |
| حقل `examples` (أمثلة استخدام) | يحتاج محرر منفصل | Story 1.6 أو لاحقاً |
| تصفية بـ regex (`--filter word~=...`) | تعقيد بدون طلب فعلي | عند الحاجة الأولى |
| آلية `id` override صريحة | لا حاجة فعلية الآن | Story 1.6 (مع `--validate`) |
| `published-keyword-ids-v1.json` registry للـ CI | يحتاج Story 1.7/1.8 | Story 1.7 |

---

## 8) قائمة الموافقة النهائية

- [x] القرار 1: id مشتق من `english` (ليس tokenType).
- [x] AC الجديدة B6, B7, C4 (deprecated detection).
- [x] حذف `since` من 1.2 (مؤجَّل لـ YAML v4.2).
- [x] رفع `kToolVersion` إلى `"0.2.0"`.
- [x] نهج الدفعتين (1: filters+ids → checkpoint → 2: yaml+tests).
- [x] التقدير: 1.75 يوم.
- [x] yaml-cpp عبر FetchContent.
- [x] الترتيب الثابت للحقول.
- [x] snapshot tests + golden files.

**عند الموافقة:** البدء فوراً بالدفعة 1 (T1-T7).
