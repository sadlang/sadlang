# خط التوثيق — لغة ص (Documentation Pipeline)

> هذا المجلد يجمع كل آثار التخطيط والتنفيذ الخاصة بنظام التوثيق
> (sadinfo + MkDocs + LSP data) في مكان واحد، مرتب حسب مرحلة دورة الحياة.

---

## بنية المجلدات

| المجلد | المحتوى | المرحلة |
|---|---|---|
| `01_prd/` | متطلبات المنتج (Product Requirements Document) ورحلات المستخدم | **اكتشاف** |
| `02_architecture/` | قرارات معمارية ومخططات النظام | **تصميم** |
| `03_epics_stories/` | الـ epics والـ stories التفصيلية + مراجعات الوكلاء | **تخطيط** |
| `04_gap_analysis/` | تحليل الفجوات + تقارير جاهزية التنفيذ | **تقييم** |
| `05_ux/` | مواصفات UX ومراجعات مكونات Vue | **تصميم بصري** |
| `06_testing/` | خطط الاختبار للأدوات المُسلَّمة | **ضمان جودة** |
| `07_party_sessions/` | جلسات الذكاء الجماعي (BMAD party mode) | **مراجعة** |
| `08_implementation_artifacts/` | (محجوز للاستخدام المستقبلي) | **تنفيذ** |
| `99_arabic_misc/` | ملفات عربية تاريخية أو متنوعة | **أرشيف** |

---

## محتوى كل مجلد

### 01_prd — متطلبات المنتج

| الملف | الوصف |
|---|---|
| `prd-docs-system-v2.md` | PRD رئيسي لنظام التوثيق (الإصدار 2 — المعتمد) |

### 02_architecture — المعمارية

| الملف | الوصف |
|---|---|
| `architecture-docs-system-v2.md` | مخطط النظام: sadinfo → JSON/YAML → MkDocs/LSP |

### 03_epics_stories — الـ Epics والـ Stories

| الملف | الوصف |
|---|---|
| `epics-docs-system-v2.md` | **مرجع الـ epics الرئيسي** (Stories 1.1–4.x) |
| `story-docs-0.0-verify-infrastructure.md` | Story 0.0: التحقق من البنية التحتية |
| `story-1.2-detailed-plan.md` | الخطة التفصيلية لـ Story 1.2 (الإصدار الأول) |
| `story-1.2-detailed-plan-v2.md` | الخطة التفصيلية لـ Story 1.2 (المعتمد) |
| `story-1.2-agent-party-review.md` | مراجعة وكلاء BMAD لـ Story 1.2 |

### 04_gap_analysis — تحليل الفجوات

| الملف | الوصف |
|---|---|
| `gap-analysis-docs-system-v2.md` | تحليل الفجوات بين الواقع والمأمول |

### 05_ux — تجربة المستخدم

| الملف | الوصف |
|---|---|
| `ux-docs-system.md` | مواصفات UX لنظام التوثيق |
| `ux-review-vue-components.md` | مراجعة UX لمكونات Vue المرشَّحة |

### 06_testing — الاختبارات

| الملف | الوصف |
|---|---|
| `test-plan-docs-1.1-sadinfo-skeleton.md` | خطة اختبار Story 1.1 (هيكل sadinfo) |

### 07_party_sessions — جلسات الذكاء الجماعي

| الملف | الوصف |
|---|---|
| `2026-05-09_documentation_system.md` | جلسة وكلاء BMAD حول نظام التوثيق |

### 99_arabic_misc — متنوعات عربية

| الملف | الوصف |
|---|---|
| `معمارية_توثيق_لغة_ص.md` | وثيقة معمارية مبكرة بالعربية |

---

## حالة الإنجاز (آخر تحديث: Story 1.3)

| Story | الحالة | الـ commit | الملاحظات |
|---|---|---|---|
| 0.0 — Verify infrastructure | ✅ مكتمل | (مبكر) | البنية التحتية مُتحققة |
| 1.1 — sadinfo skeleton | ✅ مكتمل | (مبكر) | الهيكل + CLI أساسي |
| 1.2 — `--dump-keywords` | ✅ مكتمل | `905ceee` | 90 كلمة + 91 مع deprecated، YAML + JSON، 3/3 ctest |
| **1.3 — `--dump-builtins`** | ✅ **مكتمل** | `84d43ae` | **15 دالة + 43 طريقة، schema 1.2.0، 4/4 ctest** |
| 1.4 — `--dump-errors` | 🟡 قيد التخطيط | — | بانتظار مراجعة Mary's workshop suggestion |
| 1.5 — `--dump-directives` | 🟡 قيد التخطيط | — | توجيهات `@` (حجم/ذري/...) |
| 2.x — MkDocs PoC | 🟡 مقترح القفز إليه | — | اقتراح الوكلاء (Paige + Winston + Mary): قفز مبكر للحصول على feedback loop قصير |
| 3.x — VS Code Extension | 🔴 لم يبدأ | — | يستهلك JSON من sadinfo |
| 4.x — AI Training Data | 🔴 لم يبدأ | — | bilingual corpus generation |

---

## القرارات المعمارية الرئيسية (ADR محفوظة هنا)

1. **sadinfo مستقل عن المفسر/المترجم** — لا يربط `interpreter_new` ولا `compiler_new`. يعتمد فقط على `sad_shared` + `yaml-cpp` (اختياري).
2. **truth tables يدوية في `tools/sadinfo/src/builtin_data.cpp`** — قرار براغماتي (Story 1.3). دين تقني محفوظ في `compiler_fix_notes.md`؛ الحل المستقبلي: توليد من `data/language/builtins.yaml` في build time.
3. **Schema versioning صارم** — `schemaVersion` (1.2.0 حالياً) منفصل عن `toolVersion` (0.3.0). الحقول الجديدة تُضاف backward-compatible فقط.
4. **Stable IDs كمفاتيح ترجمة** — `keyword.if`, `print`, `array.push`، إلخ. أي تغيير في `id` = breaking.

---

## الملفات ذات الصلة خارج هذا المجلد

| المسار | الدور |
|---|---|
| `tools/sadinfo/` | الكود المصدري للأداة |
| `tests/sadinfo/` | الاختبارات + golden snapshots |
| `data/language/keywords.yaml` | مصدر بيانات الكلمات المفتاحية |
| `cmake/yaml_cpp.cmake` | تكامل yaml-cpp v0.8.0 |
| `/memories/repo/compiler_fix_notes.md` | ديون تقنية متعلقة بالمصدر المزدوج للحقيقة |
