# 📋 PRD v2 — نظام التوثيق الحي للغة ص

> **النسخة:** 2.0 (نهائية بعد فحص website + docs_emitter + tools/check)
> **يستبدل:** `prd-docs-system.md` (v1)
> **التاريخ:** 2026-05-09
> **المؤلفون:** John (PM) + Amelia (Dev) + Winston (Arch) + Sally (UX) + Murat (TEA)
> **الحالة:** جاهز للتنفيذ
> **التقدير:** **11 يوم عمل** (بدلاً من 31 يوم في v1 — توفير 65%)

---

## 1. ملخص تنفيذي

نظام التوثيق الحي يضمن أن **كل كلمة مفتاحية، دالة مدمجة، رسالة خطأ، وتوجيه في لغة ص** موثَّقة بشكل دقيق ومحدَّث وثنائي اللغة (AR/EN)، ويُنشر تلقائياً على [sad-lang.org](https://sad-lang.org) (الموقع الموجود في `C:\s_lang\website`).

### التغيير الجذري عن v1

| الجانب | v1 (مُلغى) | **v2 (الحالي)** |
|---|---|---|
| docgen | بناء جديد كامل | استخدام `compiler/include/meta/docs_emitter.h` (موجود) |
| استخراج keywords | راية في sad/sadc | **`sadinfo.exe` binary مستقل** |
| موقع التوثيق | VitePress جديد | **`C:\s_lang\website` (موجود ويعمل)** |
| i18n | بناء جديد | موجود (`locales.root` + `locales.en` + RTL) |
| اختبارات Markdown | بناء من الصفر | `tests/doc_gen_dual_execution/` (44/44 ✓) |
| الجهد | 31 يوم | **11 يوم** |

---

## 2. السياق والمشكلة

### الواقع الحالي

| الموجود | الوصف | الفجوة |
|---|---|---|
| ✅ `docs_emitter.h` | يولّد توثيق من كود `.ص` (HTML/MD/JSON) | لا يستخرج من نواة المفسر |
| ✅ `docs_extractor.h` | يحلل AST ويصدر Markdown | محدود بكود المستخدم |
| ✅ CLI متكامل | `sad/sadc --docs --docs-format=...` | لا يدعم keywords/builtins |
| ✅ 17 وسم AR/EN | `@نسخة @تعقيد @آمن_خيوط...` | متوفر |
| ✅ Dual tests | 44/44 ✓ | لا يغطي keywords |
| ✅ website VitePress | sad-lang.org جاهز للنشر | لا يستهلك بيانات النواة |

### المشكلة المتبقية

**40 كلمة محجوزة + 25 سياقية + 21 دالة مدمجة + رسائل أخطاء + توجيهات `@`** — كل هذه موجودة فقط في **runtime memory** (`KeywordTable`، `BuiltinRegistry`)، لا يمكن للموقع/المحرر/الذكاء الاصطناعي قراءتها.

### الحل

أداة جديدة `sadinfo.exe` تستخرج هذه البيانات إلى JSON/YAML، تُكتَب في `website/docs/_generated/`، ويستهلكها الموقع عبر VitePress dataLoaders.

---

## 3. المتطلبات الوظيفية (FRs)

> **مرقّمة بمجموعات:** A=استخراج (sadinfo)، B=Quality Gates، C=AI، D=تكامل website، E=توزيع

### A. استخراج البيانات (sadinfo.exe)

| FR# | الوصف | المعيار |
|---|---|---|
| FR-A-01 | binary مستقل `sadinfo.exe` في `tools/sadinfo/` | يُبنى عبر CMake target |
| FR-A-02 | يستخدم `shared/lexer/include/keyword_table.h` لاستخراج 40 محجوزة | بدون duplication |
| FR-A-03 | يستخدم `interpreter/builtins/` لاستخراج 21 دالة مدمجة | يقرأ من registry |
| FR-A-04 | يستخدم `shared/errors/` لاستخراج رسائل الأخطاء | كل رسالة بـ AR+EN |
| FR-A-05 | يستخرج التوجيهات `@حجم/@ذري/@غير_آمن/@وقت_الترجمة/@متطاير/@تجميع` | من ParserCore |
| FR-A-06 | راية `--dump-keywords` تُخرج JSON | schema مُعرَّف |
| FR-A-07 | راية `--dump-builtins` تُخرج JSON | يحوي signature + examples |
| FR-A-08 | راية `--dump-errors` تُخرج JSON | يحوي error code + AR + EN + recovery hint |
| FR-A-09 | راية `--dump-directives` تُخرج JSON | جميع التوجيهات `@` |
| FR-A-10 | راية `--dump-all --output-dir=<path>` تكتب 4 ملفات | ذرّية (atomic) |
| FR-A-11 | راية `--format=json\|yaml` للاختيار | افتراضي JSON |
| FR-A-12 | راية `--lang=ar\|en\|both` للترشيح | افتراضي both |
| FR-A-13 | راية `--validate <yaml-file>` تتحقق من YAML schema | exit 0/1 |
| FR-A-14 | راية `--version` و `--help` | معياري |
| FR-A-15 | كل JSON متطابق مع YAML schema | اختبار آلي |

### B. Quality Gates (G1-G7)

| FR# | الوصف | Gate |
|---|---|---|
| FR-B-01 | `check_keywords.py` يفحص أن كل keyword له entry في YAML | G1 (Coverage) |
| FR-B-02 | فحص drift: keyword جديد بدون توثيق → CI fail | G2 |
| FR-B-03 | كل مثال في docs يُنفَّذ ويُقارن مع expected | G3 |
| FR-B-04 | كل entry له AR + EN — `bilingual_check.py` | G4 |
| FR-B-05 | website build < 60s — benchmark في CI | G5 |
| FR-B-06 | axe-playwright على website — 0 critical violations | G6 |
| FR-B-07 | Test flake rate < 1% | G7 |
| FR-B-08 | جميع G1-G7 في GitHub Action `docs-quality.yml` | إلزامي قبل merge |

### C. AI Drafts Pipeline

| FR# | الوصف |
|---|---|
| FR-C-01 | `tools/sad-ai-docgen/` Python script يستهلك Claude API |
| FR-C-02 | YAML schema للمحرَّر يدوياً (`website/docs/_published/*.yaml`) |
| FR-C-03 | YAML للمسوّدات (`website/docs/_drafts/*.draft.yaml`) gitignored من النشر |
| FR-C-04 | Pre-commit hook يحظر `.draft.yaml` في PR للنشر |
| FR-C-05 | Human review workflow موثَّق في `CONTRIBUTING.md` |
| FR-C-06 | `_drafts → _published` يتطلب موافقة maintainer |

### D. تكامل website

| FR# | الوصف |
|---|---|
| FR-D-01 | sadinfo يكتب في `website/docs/_generated/*.json` |
| FR-D-02 | VitePress dataLoader في `.vitepress/config/sidebar.ts` يقرأ JSON |
| FR-D-03 | sidebar يولّد روابط ديناميكية لكل keyword/builtin/error |
| FR-D-04 | Vue component `KeywordCard.vue` في `theme/components/` |
| FR-D-05 | Vue component `BuiltinPlayground.vue` (يكامل CodeMirror الموجود) |
| FR-D-06 | Vue component `ErrorRecoveryDemo.vue` |
| FR-D-07 | i18n يعمل مع المحتوى المُولَّد (AR في root، EN في `/en/`) |
| FR-D-08 | RTL/LTR يبدّل تلقائياً مع تغيير اللغة (موجود في VitePress) |
| FR-D-09 | بحث VitePress يفهرس المحتوى المُولَّد |
| FR-D-10 | edit links تشير لمصدر الكلمة في `shared/lexer/` لا للـ JSON المُولَّد |

### E. التوزيع

| FR# | الوصف |
|---|---|
| FR-E-01 | npm package `@sad-lang/docs-data` يحوي JSON المُولَّد |
| FR-E-02 | `pip install sad-doctest` للأدوات Python (validators) |
| FR-E-03 | gh-pages deployment تلقائي (موجود — يحتاج تفعيل CI) |
| FR-E-04 | `mike` للنسخ القديمة من docs (اختياري Wave 4) |

---

## 4. المتطلبات غير الوظيفية (NFRs)

| NFR# | الفئة | المتطلب |
|---|---|---|
| NFR-01 | الأداء | sadinfo --dump-all يكتمل في < 2 ثانية |
| NFR-02 | الأداء | website build < 60 ثانية (G5) |
| NFR-03 | الموثوقية | sadinfo idempotent — تشغيله مرتين ينتج نفس النتيجة |
| NFR-04 | الموثوقية | اختبارات mutation: حذف keyword من Table → CI يفشل |
| NFR-05 | الأمان | لا أسرار في JSON المُولَّد — فحص بـ trufflesecurity |
| NFR-06 | الأمان | npm audit + pip-audit = 0 high/critical |
| NFR-07 | إمكانية الوصول | website يجتاز WCAG 2.1 AA (G6) |
| NFR-08 | إمكانية الوصول | RTL/LTR صحيح في كل الصفحات |
| NFR-09 | i18n | كل entry له AR + EN (G4) |
| NFR-10 | الصيانة | sadinfo binary < 5MB |
| NFR-11 | الصيانة | لا تكرار بين sadinfo وdocs_emitter |
| NFR-12 | التوافقية | sadinfo يعمل على Windows + Linux + macOS (CI matrix) |
| NFR-13 | الجودة | flake rate < 1% (G7) |
| NFR-14 | التتبع | كل JSON entry يحوي source location (file:line) |
| NFR-15 | التطوير | hot reload في website دون إعادة بناء sadinfo |

---

## 5. القرارات التقنية (الحاسمة)

| # | القرار | البديل المرفوض | السبب |
|---|---|---|---|
| TD-01 | `sadinfo.exe` مستقل | راية في sad/sadc | عزل + binary صغير + واضح المسؤولية |
| TD-02 | يستخدم `shared/lexer` (لا duplication) | كود منفصل | CW-19 (DRY) + sad-lang gotchas |
| TD-03 | يكتب في `website/docs/_generated/` | mono-repo `docs/` جديد | website موجود ومُعد |
| TD-04 | JSON أساسي + YAML للمحرَّر يدوياً | JSON فقط | YAML أفضل للقراءة البشرية |
| TD-05 | VitePress dataLoaders (لا API runtime) | API server | static + سريع + يعمل على gh-pages |
| TD-06 | gh-pages (موجود) | Vercel/Netlify | مجاني + integrated مع repo |
| TD-07 | Claude API للـ AI drafts | OpenAI/local | بناءً على `agents/skills/` الموجودة |
| TD-08 | `@sad-lang` npm org | scoped غير محدد | علامة تجارية واضحة |

---

## 6. المخاطر (Risk Catalog)

| # | المخاطرة | احتمال × تأثير | تخفيف |
|---|---|---|---|
| R1 | KeywordTable يتغيّر دون تحديث docs | 4×4=16 🟠 | G2 (drift check) في CI، fail PR |
| R2 | website build batters على المساهمين الجدد | 3×3=9 🟡 | docs/CONTRIBUTING.md + npm scripts بسيطة |
| R3 | Claude API rate limit يكسر AI pipeline | 3×3=9 🟡 | retry + cache + fallback يدوي |
| R4 | i18n يخل بالنشر إذا فشلت ترجمة | 2×4=8 🟡 | AR fallback + CI لا يفشل لـ EN فقط |
| R5 | sadinfo يفشل على macOS بسبب filesystem | 2×4=8 🟡 | CI matrix من اليوم الأول |
| R6 | npm @sad-lang مأخوذ | 2×3=6 🟢 | بديل `@saleh-sad-lang` |

---

## 7. Definition of Done (Project-Level)

- [ ] `sadinfo.exe` يُبنى على Windows + Linux + macOS (CI matrix)
- [ ] جميع 7 Quality Gates (G1-G7) تعمل في CI
- [ ] website يعرض keywords/builtins/errors من JSON مُولَّد
- [ ] i18n AR/EN يعمل في كل صفحات المحتوى المُولَّد
- [ ] axe-playwright = 0 critical violations
- [ ] gh-pages deployment تلقائي عند merge to main
- [ ] npm package منشور بنسخة 1.0.0
- [ ] CONTRIBUTING.md يشرح: AI draft → review → publish
- [ ] retro session تكشف خطة phase 2

---

## 8. الموافقات

| الدور | الاسم | الحالة |
|---|---|---|
| PM | John | ✅ موافق |
| Dev Lead | Amelia | ✅ موافق (مع gap analysis v2) |
| Architect | Winston | ✅ موافق (بعد فحص website) |
| UX | Sally | ⏳ مراجعة معلّقة |
| Test Architect | Murat | ⏳ مراجعة معلّقة |
| Owner | صلاح | ⏳ موافقة نهائية |

---

> **توقيع:** John (PM) — *"PRD v2 يعكس الواقع، ليس الخيال. الكود الموجود نقطة قوة، ليس عبئاً."*
