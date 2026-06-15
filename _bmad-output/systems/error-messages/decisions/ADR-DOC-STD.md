---
title: "ADR-DOC-STD — معايير توثيق YAML الرسمية للغة ص"
status: "Draft"
date: "2026-05-19"
author: "Amelia (مع توجيه من صاحب المشروع)"
supersedes: []
related:
  - "_bmad-output/systems/doc-ir/ADR-006_توحيد_نظام_التوليد.md"
  - "_bmad-output/systems/doc-ir/ADR-006b-spec.md"
  - "_bmad-output/systems/doc-ir/error_system/ADR-EM-3.md"
  - "_bmad-output/STATUS.md"
scope: "data/language/**/*.yaml"
---

# ADR-DOC-STD — معايير توثيق YAML الرسمية للغة ص

## 1. السياق (Context)

### 1.1 المشكلة الجذرية

ملفات YAML في `data/language/` هي **مصدر الحقيقة الواحد (SoT)** لكل من:

| الملف | يُولّد |
|---|---|
| `error_messages.yaml` | رسائل أخطاء C++ + صفحات موقع |
| `builtin_function.schema.json` + `builtins/*.yaml` | توثيق الدوال المضمنة |
| `keywords.yaml` | جدول الكلمات المفتاحية + LSP |
| `modules.yaml` | فهرس الوحدات |
| `type_methods.yaml` | طرق الأنواع المدمجة |

**الواقع الحالي:**

| المؤشر | النتيجة |
|---|---|
| schemas تفرض **حد أدنى فقط** (`minLength: 1`) | ✅ موجود |
| معايير جودة موثَّقة رسمياً | ❌ غير موجود |
| فحص آلي للجودة | ❌ غير موجود |
| CI gate يرفض الرسائل الفقيرة | ❌ غير موجود |
| الحقل `teacher` مستخدم | 0/231 رسالة |
| الحقل `code_example` مستخدم | 0/231 رسالة |
| نصوص مكررة (title == brief) في INT_* | 121/156 رسالة |

**النتيجة:** الموقع الذي يُولَّد من هذه الـYAML **فقير تعليمياً** — يعرض الحد الأدنى فقط.

### 1.2 الهدف

إنشاء **نظام توثيق رسمي ملزم** يجبر المطور على إثراء كل ملف YAML بمحتوى تعليمي غني، يتحول تلقائياً إلى صفحات تعليمية في موقع لغة ص.

## 2. القرار (Decision)

اعتماد **نظام معايير توثيق رباعي الطبقات (Quality Tiers)** + **schemas مُحدَّثة** + **فاحص آلي** + **CI gate** + **README رسمي شامل**.

### 2.1 طبقات الجودة

| الطبقة | الحقول الإلزامية | الحد الأدنى للأطوال | الفئات المعنية |
|---|---|---|---|
| **MINIMAL** | title، brief | title≥3 كلمات، brief≥10 حرف، title≠brief | INT_INVARIANT_* (داخلي للمطور) |
| **STANDARD** | + detailed، fix_hint | detailed≥60، fix_hint≥30 | LEX، SYN، SEM (للمستخدم المتقدم) |
| **RICH** | + teacher، code_example | teacher≥80، مثال كود `.ص` صالح | RUN، SEM-للمبتدئ |
| **TEACHING** | + سيناريوهات متعددة، روابط see_also | ≥2 أمثلة، ≥1 رابط | الرسائل التعليمية المركزية |

**قاعدة الانتقال:** كل رسالة جديدة تبدأ من **STANDARD** افتراضياً. الرسائل المرئية للمستخدم النهائي يجب أن تكون **RICH** قبل دمجها.

### 2.2 المكوّنات الخمسة

#### المكوّن 1: دليل معماري شامل

**موقع:** `data/language/README.md`

**محتوى:**
- خريطة كل ملفات YAML في المجلد
- روابط لكل STANDARD منفصل
- شرح كيف يتحول كل حقل إلى صفحة في الموقع
- مثال minimal vs ideal

#### المكوّن 2: معايير لكل نوع YAML

**موقع:** `data/language/STANDARDS/<type>.STANDARD.md`

**ملفات مقترحة:**

```
data/language/STANDARDS/
├── README.md                       # فهرس المعايير
├── error_messages.STANDARD.md      # معايير رسائل الأخطاء
├── builtins.STANDARD.md            # معايير الدوال المضمنة
├── keywords.STANDARD.md            # معايير الكلمات المفتاحية
├── modules.STANDARD.md             # معايير الوحدات
└── type_methods.STANDARD.md        # معايير طرق الأنواع
```

**محتوى كل STANDARD:**
- جدول الحقول مع: إلزامية/اختيارية، طول أدنى، طول مثالي
- 3 أمثلة: minimal (يمر بالكاد)، standard (متوسط)، ideal (نموذجي)
- جدول الأنماط الممنوعة (title==brief، نص حشو، إلخ)
- ربط كل حقل بصفحة الموقع المتولدة منه

#### المكوّن 3: schemas مُحدَّثة

تحديث `error_messages.schema.json` و schemas الأخرى لإضافة:
- `minLength` أعلى لكل حقل (وفق الطبقات)
- نمط `not` يمنع `title == brief`
- حقل اختياري `quality_tier: ["MINIMAL"|"STANDARD"|"RICH"|"TEACHING"]` يصرّح بنية المطور
- نمط للتحقق أن placeholders المعرَّفة مستخدمة فعلاً في النصوص

#### المكوّن 4: فاحص جودة آلي

**موقع:** `scripts/codegen/yaml_quality_check.py` + `test_yaml_quality_check.py`

**يفحص:**
1. كل ملف yaml ضد STANDARD المناسب
2. تطابق `quality_tier` مع المحتوى الفعلي
3. وجود placeholders في النصوص حين تُعرَّف
4. عدم تكرار title == brief
5. أمثلة الكود `.ص` صالحة نحوياً (يستدعي parser)
6. روابط see_also تشير لرموز موجودة

**مخرجات:**
- تقرير `data/language/quality_report.md` (مُولَّد) يصنّف كل رسالة بطبقتها
- خروج بحالة 1 إذا انخفض المستوى عن baseline

#### المكوّن 5: CI gate إلزامي

**موقع:** `cmake/codegen.cmake` + `.github/workflows/yaml_quality.yml`

**يفعل:**
- يستدعي `yaml_quality_check.py` قبل توليد الـheaders
- البناء **يفشل** إذا انخفض المعدّل عن baseline في `data/language/quality_baseline.json`
- baseline يُحدَّث آلياً عند رفع المستوى (monotonic ratchet)

### 2.3 ربط النظام بموقع لغة ص

كل حقل في YAML يخدم قسماً في صفحة الموقع:

| حقل YAML | قسم الصفحة |
|---|---|
| `title.ar/en` | عنوان الصفحة (H1) |
| `brief.ar/en` | شريط الملخص العلوي |
| `detailed.ar/en` | قسم "التفاصيل" |
| `teacher.ar/en` | بلوك **"درس مصغّر"** (مميَّز بصرياً) |
| `fix_hint.ar/en` | بلوك **"كيف تصلحه؟"** |
| `code_example` | بلوك كود قابل للنسخ والتجربة |
| `placeholders` | جدول "المتغيرات المُستخدمة" |
| `see_also` | روابط في أسفل الصفحة |

## 3. التقسيم إلى مراحل (Stories)

| Story | الحجم | المخرجات |
|---|---|---|
| **DOC-STD-1** | S | إنشاء `data/language/README.md` + هيكل `STANDARDS/` (مجلد فارغ + README) |
| **DOC-STD-2** | M | كتابة `error_messages.STANDARD.md` كاملاً مع 3 أمثلة minimal/standard/ideal |
| **DOC-STD-3** | M | تحديث `error_messages.schema.json` لفرض المعايير + اختبار pytest |
| **DOC-STD-4** | M | بناء `yaml_quality_check.py` + tests + تقرير baseline |
| **DOC-STD-5** | L | إثراء LEX/SYN/SEM (23 رسالة) بـ teacher + code_example + رفع إلى RICH |
| **DOC-STD-6** | L | إثراء RUN (52 رسالة) بـ teacher + code_example + رفع إلى RICH |
| **DOC-STD-7** | M | إصلاح INT_INVARIANT_* (156 رسالة) — كسر title==brief + قوالب طبقية |
| **DOC-STD-8** | S | تفعيل CI gate في `cmake/codegen.cmake` + workflow |
| **DOC-STD-9** | M | تمديد النظام لـ `builtins/*.yaml` (نفس النمط) |
| **DOC-STD-10** | M | تمديد لـ `keywords.yaml` + `modules.yaml` + `type_methods.yaml` |
| **DOC-STD-11** | M | ربط بمولّد الموقع (DocIR) — يخرج بلوكات teacher/code_example في صفحات vitepress |

## 4. النتائج المتوقعة (Expected Outcomes)

### 4.1 قابلة للقياس

| المؤشر | الحالي | الهدف بعد DOC-STD |
|---|---|---|
| رسائل بطبقة RICH | 0/231 | ≥75/231 (RUN+SEM-user) |
| رسائل بطبقة STANDARD على الأقل | 75/231 | 231/231 |
| رسائل title==brief | 121/231 | 0/231 |
| استخدام teacher | 0% | ≥30% |
| استخدام code_example | 0% | ≥30% |
| CI gate يفرض الجودة | لا | نعم |

### 4.2 الأثر على المطور

- **قبل:** المطور يكتب `title=brief="رسالة"` ويتجاوز الفحص.
- **بعد:** schema يرفض، فاحص يرفض، CI يرفض. لا بد من كتابة محتوى غني.

### 4.3 الأثر على المستخدم

- صفحات موقع تحتوي شروحات تعليمية + أمثلة كود + روابط
- LSP في VS Code يعرض teacher عند hover (بدلاً من brief فقط)

## 5. البدائل المرفوضة (Alternatives Rejected)

| البديل | لماذا رُفض |
|---|---|
| الإبقاء على schema الحالي + توصية للمطورين | لا إجبار = لا التزام (تجربة 8 شهور أثبتت ذلك) |
| استخدام linter خارجي (markdownlint) | لا يفهم بنية YAML الخاصة بنا |
| توليد teacher آلياً من brief باستخدام LLM | جودة منخفضة + غير قابلة للتحكم + كلفة مستمرة |
| ترك INT_INVARIANT_* كما هي | تكسر اتساق النظام؛ المطور الذي يصل إليها يستحق رسالة جيدة |

## 6. المخاطر (Risks)

| المخاطرة | الأثر | المخفّف |
|---|---|---|
| تأخير دمج PRs بسبب رفض CI | متوسط | baseline يتحرك للأعلى فقط (ratchet) + grace period أسبوع لكل ترقية |
| كتابة teacher/code_example تستهلك وقتاً | متوسط | قوالب جاهزة في STANDARD + أمثلة قابلة للنسخ |
| الـschema المعقدة تربك الأدوات | منخفض | اختبارات pytest شاملة + توافق مع JSON Schema 2020-12 |

## 7. القرار النهائي

**اعتماد:** نعم — يبدأ التنفيذ عبر Stories DOC-STD-1 → DOC-STD-11 بالترتيب.

**نقطة قرار العودة:** بعد DOC-STD-4 (الفاحص الآلي جاهز) — نقيس الجدوى الفعلية ونحدد ما إذا كنا سنواصل DOC-STD-5+.
