# سجل التغييرات / Changelog — sadinfo

جميع التغييرات الملحوظة في أداة `sadinfo` تُسجَّل هنا.
الصيغة مبنية على [Keep a Changelog](https://keepachangelog.com/).

---

## [0.2.0] — Story 1.2

### مُضاف / Added

- **معرّف ثابت `id`** لكل كلمة (مفتاح ترجمة عبر الإصدارات):
  - `keyword.<english>` للمحجوزة
  - `operator.<english>` للعوامل
  - `contextual.<english>` للسياقية
  - `type.<english>` للأنواع المدمجة
  - `literal.<english>` للحرفيات (true/false/null)
- **حقل `deprecated`** يظهر فقط عندما يكون `true`.
- **خيار `--format json|yaml`** — كاتب YAML يدوي مدمج (بدون yaml-cpp).
- **خيار `--lang ar|en|both`** — تصفية اللغة (يحذف `english` مع `ar`).
- **خيار `--minimal`** — مخرج مختصر (id, word, category, tokenType فقط).
- **خيار `--by-category`** — تجميع المخرج تحت `categories.{reserved, operator, contextual, builtin_type}`.
- **خيار `--include-deprecated`** — شمول الكلمات المُهملة (افتراضياً مستبعدة).
- **خيار `--filter key=value`** (قابل للتكرار) — تصفية بـ AND-logic:
  - `category`, `role`, `token-type`, `has-aliases`
  - مفاتيح غير معروفة تُرفض بـ exit=2.
- **خيار `--compact` / `--pretty`** للتحكم بالتنسيق.
- **حقول جذرية جديدة:** `schemaVersion`, `tool`, `toolVersion`,
  `totalCount`, `categoryCounts`.
- **اختبارات:**
  - `test_sadinfo_keyword_transforms` (291 فحص للدوال النقية).
  - `SadInfoGoldenIds` (snapshot لـ 91 معرفاً يكسر CI عند تغيير english).

### مُعدَّل / Changed

- **مخطط البيانات: 1.0.0 → 1.1.0** (متوافق للخلف — حقول جديدة فقط).
- **نسخة الأداة: 0.1.0 → 0.2.0**.
- **ترتيب حقول المُدخل ثابت ومضمون:** id → word → english → category →
  tokenType → emittedByLexer → deprecated → aliases → roles.
- **السلوك الافتراضي:** الكلمات المُهملة مستبعدة (`totalCount=90`).
  استخدم `--include-deprecated` للحصول على 91.
- **اشتقاق `deprecated`:** يعتمد على لاحقة `_deprecated` في حقل `english`
  من `keywords.yaml` (أكثر موثوقية من اسم TokenType).

### مُصلَح / Fixed

- (BF-04) كان `--dump-keywords` يُرجع 91 بدلاً من 90 بسبب أن
  `Token::typeToString` لا يغطي `KEYWORD_GIVE_DEPRECATED`. الإصلاح:
  اشتقاق المُهمَلة من حقل `english` مباشرة (مصدر الحقيقة في YAML).
- (BF-25) كان `--filter badkey` يُرجع نتيجة فارغة بصمت (exit=0). الإصلاح:
  رفض المفاتيح غير المعروفة في وقت التحليل بـ exit=2 ورسالة ثنائية اللغة.

### مُدمَج لاحقاً / Integrated (Story 1.2 follow-up)

- **`yaml-cpp v0.8.0`** عبر CMake `FetchContent` في
  `cmake/yaml_cpp.cmake`. يُفعَّل افتراضياً عبر `ENABLE_YAML_CPP=ON`
  ويمكن تعطيله للحصول على fallback اليدوي (نفس البنية).
  - حماية محسّنة للحروف العربية وعلامات الاقتباس.
  - مخرج canonical يُمرَّر `python -c "import yaml; yaml.safe_load(...)"`
    دون أخطاء.
  - يُربط بـ `sadinfo` فقط — لا يُربط بـ `sad_shared` أو غيرها.

---

## [0.1.0] — Story 1.1

### مُضاف / Added

- هيكل أداة `sadinfo` الأولي.
- الأمر `--dump-keywords` بمخطط 1.0.0 (قائمة مسطحة، JSON فقط).
- ربط بـ `sad_shared` فقط (بناء سريع).
- تكامل `KeywordRegistry` المركزي (مصدر واحد للحقيقة من
  `data/language/keywords.yaml`).
