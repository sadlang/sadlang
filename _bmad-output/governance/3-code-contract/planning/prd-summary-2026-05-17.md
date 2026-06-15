# وثيقة متطلبات المنتج (PRD)
# نظام إلزام معايير الكود — Contract-as-Code

**الإصدار:** 1.0.0
**التاريخ:** 2026-05-17
**الحالة:** مُعتمَد للتنفيذ
**المشروع:** لغة ص (s-programming-language)
**المُعِد:** GitHub Copilot (Claude Sonnet 4.6)
**المرجع:** `_bmad-output/governance/3-code-contract/contract-as-code-plan.md`

---

## 1. ملخص تنفيذي

### المشكلة
قواعد معايير الكود في `copilot-instructions.md` موجودة في وثائق نصية فقط — غير مُفرَّضة آلياً. وكلاء الذكاء الاصطناعي والمطورون يخرقون هذه المعايير بشكل صامت (الكود يعمل لكنه مخالف) لأنه لا توجد عقوبة تقنية على الخرق.

### الحل
بناء نظام **"Contract-as-Code"** يُفشل `cmake configure` (قبل أي compilation) عند وجود مخالفة في أي ملف `.cpp`/`.h` جديد. يستند الحل على نمط `orphan_sources_guard.cmake` الموجود والناجح في المشروع.

### القيمة المُضافة
- **صفر مخالفات صامتة:** أي خرق يُوقف البناء فوراً بتقرير واضح
- **مرونة:** إضافة قاعدة جديدة = سطر YAML واحد فقط
- **ترحيل تدريجي:** الملفات القديمة تُدرج في allowlist دون توقف التطوير

---

## 2. الأهداف والمعايير

### أهداف قابلة للقياس
| الهدف | القياس | المستهدف |
|---|---|---|
| منع المخالفات الجديدة | `cmake -B build` يفشل عند وجود مخالفة | 100% coverage لـ CS-01..CS-03 |
| وضوح الخطأ | التقرير يشمل ملف+سطر+قاعدة+حل | 100% |
| سهولة إضافة قاعدة | عدد الملفات المطلوب تعديلها | 1 ملف YAML فقط |
| لا إبطاء للبناء | زمن configure الإضافي | < 3 ثوانٍ |

### خارج النطاق (Out of Scope)
- فحص ملفات Python أو ملفات `.ص` (لغة ص)
- فحص معايير تصميم (مثل: SOLID, DRY في مستوى المنطق)
- تكامل مع GitHub Actions (مستقبلاً — قصة منفصلة)

---

## 3. متطلبات وظيفية

### FR-01: ملف SPEC (المصدر الوحيد للحقيقة)
- **ما:** ملف YAML يحتوي تعريفات جميع قواعد الكود
- **تنسيق كل قاعدة:**
  ```yaml
  - id: CS-01
    name: no-direct-arabic-string-comparison
    severity: FATAL  # أو WARN
    pattern: '== *"(\\x[89a-f][0-9a-f]\\x[0-9a-f]{2})+"'
    file_extensions: [".cpp", ".h", ".cc"]
    message_ar: "مقارنة نص عربي مباشر — استخدم enum أو دالة lookup"
    message_en: "Direct Arabic string comparison — use enum or lookup function"
    fix_ar: "استخدم arabicNameToNodeType() أو enum مقابل ثابت"
    fix_en: "Use arabicNameToNodeType() or compare against enum constant"
    exclude_dirs: ["archived/", "third_party/"]
  ```

### FR-02: ENFORCER (check_code_standards.py)
- يقرأ SPEC من `code_standards.yaml`
- يقرأ allowlist من `code_standards_allowlist.txt`
- يقبل: `--dirs DIR [DIR...]` للمسارات المُفحوصة
- يقبل: `--spec FILE` و `--allowlist FILE`
- يُخرج: exit code 0 (نجاح) أو 1 (مع تقرير تفصيلي)
- يدعم `# SAD_LINT_DISABLE: CS-XX — السبب` لتعطيل سطري

### FR-03: CMake INTEGRATION
- ملف `cmake/code_standards_guard.cmake`
- يُشغَّل عند `cmake configure` (وليس فقط عند build)
- يفشل بـ `FATAL_ERROR` مع تقرير منسق
- يدعم `SAD_SKIP_STANDARDS_GUARD=ON` للتعطيل المؤقت
- يدعم `SAD_STANDARDS_GUARD_MODE=WARN` لوضع التحذير
- يُضاف كاعتمادية لـ `sad` و `sadc`

### FR-04: Allowlist للترحيل التدريجي
- ملف نصي بسيط: مسار نسبي لكل سطر
- تعليقات بـ `#`
- يُستخدم خلال مرحلة الترحيل فقط

### FR-05: اختبارات الوحدة
- pytest لـ ENFORCER يغطي: كل قاعدة CS-XX
- اختبارات إيجابية (كود مخالف يُكتشف) وسلبية (كود صحيح لا يُكتشف)
- اختبار inline disable (`SAD_LINT_DISABLE`)
- اختبار allowlist

---

## 4. القواعد المطلوب تطبيقها (v1.0)

| رمز | القاعدة | مستوى | مرجع copilot-instructions |
|---|---|---|---|
| CS-01 | مقارنة نص عربي/UTF-8 مباشرة في C++ | FATAL | CW-09، "لا تضع نصوص عربية في switch/match" |
| CS-02 | `getValue() == "..."` بدون سياق KEYWORD_ | FATAL | معايير الكلمات السياقية في Parser |
| CS-03 | اختصار `KW_` بدل `KEYWORD_` في token/enum | FATAL | CW-09: "أسماء كاملة إلزامية" |
| CS-04 | `reinterpret_cast` خارج مجلدات FFI | WARN | CW-13 |
| CS-05 | ملف `.cpp`/`.h` يتجاوز 800 سطر | WARN | CW-05 |

---

## 5. متطلبات غير وظيفية

- **الأداء:** وقت configure الإضافي < 3 ثوانٍ على المشروع كاملاً
- **الصيانة:** إضافة قاعدة = تعديل YAML فقط، لا Python ولا CMake
- **التوافق الخلفي:** لا تُكسر أي target بناء موجودة
- **الوضوح:** رسالة الخطأ تتضمن: الملف + السطر + القاعدة + الحل الصحيح

---

## 6. بنية الملفات

```
scripts/lint/
├── code_standards.yaml              ← SPEC (مصدر الحقيقة)
├── check_code_standards.py          ← ENFORCER
└── code_standards_allowlist.txt     ← allowlist للترحيل

cmake/
└── code_standards_guard.cmake       ← INTEGRATION

scripts/codegen/  (أو tests/lint/)
└── test_code_standards.py           ← pytest unit tests

CMakeLists.txt                        ← تعديل: include + استدعاء
```

---

## 7. نموذج رسالة الخطأ المطلوبة

```
CMake Error at cmake/code_standards_guard.cmake (message):

  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  ❌ مخالفة معايير الكود — البناء متوقف (SAD_SKIP_STANDARDS_GUARD=ON للتجاوز)
  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

  [CS-01] interpreter/src/visitors/expression_evaluator_ui.cpp:88
  المشكلة: مقارنة نص عربي مباشر (UTF-8 bytes) في كود C++
  السطر  : if (node.widgetName == "\xd9\x86\xd8\xb5" || node.widgetName == "Text")
  الحل   : استخدم sad::ui::arabicNameToNodeType() ثم قارن UINodeType (enum)

  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
  إجمالي المخالفات: 1 FATAL
  للتجاوز المؤقت: cmake -DSAD_SKIP_STANDARDS_GUARD=ON -B build
  ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

---

_تم الإنشاء بواسطة: GitHub Copilot (Claude Sonnet 4.6) — 2026-05-17_
