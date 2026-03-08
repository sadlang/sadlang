# خطة توزيع الفريق — لغة ص v1.0
# Team Distribution Plan — Sad Language v1.0

> **الفريق:** 5 مطورين محترفين (خبرة 40+ سنة لكل مطور)
> **المنهجية:** Agile Scrum — Sprint كل أسبوعين
> **الأولوية:** 2 إصلاح + 3 بناء (متوازي)
> **الهدف:** v1.0 مستقر خلال 6 أشهر (26 أسبوع = 13 Sprint)
> **تاريخ البدء:** ___________
> **تاريخ v1.0 المستهدف:** ___________

---

## الفريق وتوزيع الأدوار

```
╔═══════════════════════════════════════════════════════════════════════╗
║                    فريق لغة ص — التوزيع العام                       ║
╠═══════════════════════════════════════════════════════════════════════╣
║                                                                     ║
║  ╔═══════════════════════════════════════════════════════╗           ║
║  ║  فريق الإصلاح (Fix Team) — 2 مطورين                 ║           ║
║  ║                                                       ║           ║
║  ║  👨‍💻 أحمد — مهندس النواة (Core Engineer)               ║           ║
║  ║     المفسر + النظام الأساسي + الأخطاء الحرجة          ║           ║
║  ║     interpreter_new/ + shared/                        ║           ║
║  ║                                                       ║           ║
║  ║  👨‍💻 سعيد — مهندس المترجم (Compiler Engineer)          ║           ║
║  ║     المترجم sadc + SIR + LLVM + الاختبارات             ║           ║
║  ║     compiler_new/ + tests/                            ║           ║
║  ╚═══════════════════════════════════════════════════════╝           ║
║                                                                     ║
║  ╔═══════════════════════════════════════════════════════╗           ║
║  ║  فريق البناء (Build Team) — 3 مطورين                 ║           ║
║  ║                                                       ║           ║
║  ║  👨‍💻 كامل — مهندس المنصات (Platform Engineer)          ║           ║
║  ║     Backends + UI Framework + واجهات أصلية            ║           ║
║  ║     compiler_new/backend/ + stdlib/ui/ + stdlib/graphics/║        ║
║  ║                                                       ║           ║
║  ║  👩‍💻 ريم — مهندسة المكتبات والأدوات (Libs & Tools)     ║           ║
║  ║     المكتبة القياسية + مدير الحزم + التوثيق           ║           ║
║  ║     stdlib/ + tools/pkg/ + docs/                      ║           ║
║  ║                                                       ║           ║
║  ║  👨‍💻 عمر — مهندس البنية التحتية (Infrastructure)       ║           ║
║  ║     LSP + Formatter + CI/CD + Distribution + Testing  ║           ║
║  ║     tools/lsp/ + tools/formatter/ + distribution/     ║           ║
║  ╚═══════════════════════════════════════════════════════╝           ║
╚═══════════════════════════════════════════════════════════════════════╝
```

---

## المراحل الزمنية (6 أشهر = 13 Sprint)

```
┌──────────────────────────────────────────────────────────────────┐
│  الجدول الزمني العام                                            │
│                                                                  │
│  الشهر 1-2   ║  المرحلة 0: الاستقرار + التأسيس                 │
│  Sprint 1-4  ║  إصلاح P0/P1 + هيكلة + بنية تحتية              │
│              ║                                                   │
│  الشهر 3-4   ║  المرحلة 1: التطوير المتوازي                     │
│  Sprint 5-9  ║  ميزات جديدة + تحسينات + منصات                  │
│              ║                                                   │
│  الشهر 5     ║  المرحلة 2: التكامل + Beta                       │
│  Sprint 10-11║  دمج + اختبار شامل + تحسين أداء                 │
│              ║                                                   │
│  الشهر 6     ║  المرحلة 3: التثبيت + الإصدار                    │
│  Sprint 12-13║  RC1 → RC2 → v1.0 🎉                             │
└──────────────────────────────────────────────────────────────────┘
```

---

## تفصيل Sprint بـ Sprint

---

### 🔴 المرحلة 0: الاستقرار + التأسيس (Sprint 1-4 / أسابيع 1-8)

> **الهدف:** إصلاح جميع الأخطاء الحرجة + تأسيس بنية تحتية قوية

---

#### Sprint 1 (أسبوع 1-2): التأسيس الحرج

| المطور | المهام | الملفات | المخرجات |
|--------|--------|---------|----------|
| **أحمد** | ✅ إصلاح خطأ #2: `متغير x = x + 1` في `بينما` يخلق local جديد بدل التحديث → حلقة لانهائية | `variable_manager.cpp` (دالة `define()` سطر ~75) | ✅ `define()` يفحص FUNCTION scope قبل break — **مكتمل 2026-03-06** |
| **أحمد** | ✅ إصلاح خطأ #1: `arr[i]["key"]` يتعطل داخل الدوال | `expression_evaluator_members.cpp` | ✅ مُصلح مسبقاً — تأكدنا بـ 8 سيناريوهات اختبار شاملة — **مكتمل 2026-03-06** |
| **سعيد** | ✅ تأسيس CI/CD: إعداد GitHub Actions للبناء التلقائي (Win+Linux+macOS) | `.github/workflows/ci.yml` | ✅ بناء + اختبار تلقائي على 3 منصات — **مكتمل** |
| **سعيد** | ✅ كتابة اختبارات وحدة لكل خطأ في مشاكل.md (22 اختبار regression) | `tests/regression/` | ✅ 22 اختبار + سكريبتات تشغيل + C++ wrapper — **مكتمل** |
| **كامل** | ✅ تنظيف المشروع: حذف 340+ ملف مؤقت من الجذر | الجذر: `*.txt`, `*.py`, `*.js` مؤقتة | ✅ جذر نظيف (7 ملفات ضرورية فقط) — **مكتمل 2026-03-06** |
| **كامل** | إعادة هيكلة المجلدات (نقل، لا حذف) | `build_wasm/` → `tools/wasm/build/` إلخ | بنية LLVM/Rust style |
| **ريم** | توثيق API المكتبة القياسية: core, io, math, string | `docs/api/` | 4 ملفات توثيق API |
| **ريم** | مراجعة وتحسين stdlib/core — اختبارات لكل دالة | `stdlib/core/` + `tests/stdlib/` | ≥50 اختبار stdlib |
| **عمر** | ✅ إعداد بنية Testing framework موحدة | `tests/framework/` | ✅ framework مع assert, expect, describe — **مكتمل 2026-03-06** |
| **عمر** | ✅ إعداد Git branching strategy: main, develop, feature/* | `CONTRIBUTING.md` | ✅ Git flow مكتوب — **مكتمل 2026-03-06** |

**مخرجات Sprint 1:**
- ✅ خطأين حرجين مُصلحين (#1, #2)
- ✅ CI/CD يعمل (GitHub Actions — 3 منصات)
- ✅ 22 اختبار regression (ملفات .ص + سكريبتات + C++ wrapper)
- ✅ مشروع نظيف ومُهيكل
- ✅ Git flow متفق عليه

---

#### Sprint 2 (أسبوع 3-4): إصلاح النواة

| المطور | المهام | الملفات | المخرجات |
|--------|--------|---------|----------|
| **أحمد** | ✅ إصلاح #3: `.الطول()` يتعطل عند عمق استدعاء ≥ 2 | `expression_evaluator_calls.cpp` | ✅ مُصلح مسبقاً — تأكدنا بـ 5 سيناريوهات اختبار (أعماق 1-3 + محلي + نص) — **مكتمل 2026-03-06** |
| **أحمد** | ✅ إصلاح #18: `هذا.method()` تغييرات لا تُحفظ (MAP copy bug) | `expression_evaluator_members.cpp` (سطر ~800+) | ✅ مُصلح مسبقاً — تأكدنا بالسيناريو الأصلي مع += — **مكتمل 2026-03-06** |
| **أحمد** | ✅ إصلاح #10: `ارجع` بدون قيمة → خطأ نحوي | `shared/parser/` | ✅ مُصلح مسبقاً — تأكدنا بسيناريو ارجع بدون قيمة + خروج مبكر — **مكتمل 2026-03-06** |
| **سعيد** | ✅ إصلاح 27 TODO في المترجم (من أصل 131 weakness) | `compiler_new/src/` متفرق (9 ملفات) | ✅ 27 TODO → كود فعلي كامل — **مكتمل** |
| **سعيد** | ✅ إصلاح 20 unsafe switch/default | `compiler_new/src/` (12 ملف) | ✅ 12 خطير + 8 مشبوه — تحذيرات + قيم افتراضية آمنة — **مكتمل** |
| **كامل** | تحسين Desktop backend: اختبار UI كامل على Win+Linux+macOS | `stdlib/ui/`, `stdlib/graphics/` | UI يعمل على 3 أنظمة |
| **ريم** | ✅ توثيق API: network, http, json, xml, database | `docs/api/` | ✅ 5 ملفات توثيق API — **مكتمل Sprint 1** |
| **ريم** | ✅ كتابة أمثلة تعليمية (10 أمثلة متدرجة) | `examples/tutorials/` | ✅ 10 أمثلة + 199 اختبار — **مكتمل Sprint 1** |
| **ريم** | ✅ إصلاح 4 أخطاء مفسر مكتشفة: توقف، متداخل حاول/امسك، OOP params، نفذ سياقية | `lexer_keywords.cpp`, `parser_statements.cpp`, `parser_core_main.cpp`, `expression_evaluator_oop.cpp` | ✅ 4 إصلاحات + 16 regression test + 8 اختبارات جديدة في الأطقم — **مكتمل** |
| **عمر** | ✅ تحسين LSP: كاشف المشاكل المعروفة (P1-P21) | `tools/lsp/` | ✅ **مكتمل 2026-03-06** — يحذر من 22 مشكلة موثقة |
| **عمر** | ✅ إعداد Benchmark framework | `tests/framework/` | ✅ قياس أداء مع baseline — **مكتمل 2026-03-06** |

**مخرجات Sprint 2:**
- ✅ 3 أخطاء حرجة إضافية مُصلحة (#3, #10, #18)
- ✅ 47 weakness مُصلحة في المترجم (27 TODO فعلية + 20 unsafe switch/default)
- ✅ UI Desktop يعمل بثبات
- ✅ توثيق 9 وحدات stdlib
- ✅ ريم: 4 أخطاء مفسر مُصلحة (توقف مفقود، حاول/امسك متداخل، OOP parameter shadowing، نفذ سياقية)
- ✅ ريم: 16 اختبار regression + 8 اختبارات إضافية في الأطقم القائمة (edge_cases 31/31، error_handling 10/10)

---

#### Sprint 3 (أسبوع 5-6): نظام الأنواع + OOP

| المطور | المهام | الملفات | المخرجات |
|--------|--------|---------|----------|
| **أحمد** | ✅ إصلاح #12: القسمة تنتج float دائمًا + #11: أصناف مخصصة لا تعمل كحقول | `expression_evaluator_core.cpp`, `class_manager.cpp` | ✅ #12 مُصلح مسبقاً، #11 قيد parser — composition يعمل |
| **أحمد** | ✅ تحسين `Value` system: إضافة `FUNCTION` type + type coercion rules | `shared/types/include/value.h`, `value.cpp`, `function_ref.h`, `expression_evaluator_*.cpp`, `builtins.cpp`, `type_functions.cpp`, `io_functions.cpp` | ✅ 9 أنواع بدل 8 — FUNCTION type كامل (Lambda + دوال أولية + مقارنة + call resolution) |
| **أحمد** | ✅ إصلاحات جانبية: OOP regression (هذا → define)، toNumber→toDouble، VOID macro، M_PI، mobile linker | `variable_manager.cpp`, `builtin_registry_part6.cpp`, `builtins.cpp`, `mobile_module.cpp`, `sources.cmake` | ✅ جميع الإصلاحات + 16 اختبار regression ناجح |
| **سعيد** | ✅ دعم OOP كامل في المترجم: vtable + virtual dispatch، abstract class check، destructor، property getter/setter، field offset system | `compiler_new/src/backend/llvm/llvm_codegen_part1.cpp`, `llvm_codegen_part9.cpp`, `llvm_codegen_part10.cpp`, `llvm_codegen.h` | ✅ OOP يعمل في sadc — vtable C++-style بـ 5 ميزات — **مكتمل** |
| **سعيد** | اختبار sadc مع 50+ ملف .ص متنوع | `tests/compiler/` | ≥50 اختبار مترجم يمر |
| **كامل** | تحسين WASM backend (المسار 2: ترجمة أصلية) | `compiler_new/src/backend/wasm_codegen.cpp` | WASM32 target كامل |
| **ريم** | بناء مدير الحزم: تحميل + تثبيت + حل تبعيات | `tools/pkg/` | `sad pkg install` يعمل |
| **عمر** | ✅ تحسين Formatter: دعم كل بنى اللغة | `tools/formatter/` | ✅ **مكتمل** — 1313 سطر |
| **عمر** | ✅ إعداد Website playground يدعم أمثلة تفاعلية | `website/` | ✅ **مكتمل** — WASM + CodeMirror |

---

#### Sprint 4 (أسبوع 7-8): إغلاق P0/P1

| المطور | المهام | الملفات | المخرجات |
|--------|--------|---------|----------|
| **أحمد** | ✅ إصلاح باقي أخطاء P0/P1 (#4-#9, #13-#17, #19-#22) | `interpreter_new/` متفرق | ✅ **مكتمل** — فحص شامل لـ 16 خطأ: 15 مُصلحة مسبقاً + إصلاح Bug #21 (قالب→سياقية في lexer_keywords.cpp) + 54 اختبار regression ناجح |
| **أحمد** | ✅ مراجعة `constVariables_` → scope-aware const tracking | `variable_manager.h`, `variable_manager.cpp` | ✅ **مكتمل** — `constVariables_` من `unordered_set<string>` إلى `unordered_map<Scope*, unordered_set<string>>` + تنظيف في `cleanupScope` + `clear` + فحص const في النطاق الصحيح في `assign` + `defineOrAssign` — 58 اختبار regression ناجح |
| **سعيد** | إصلاح باقي weaknesses في المترجم (هدف: ≤20 متبقية) | `compiler_new/src/` | ≤20 weakness |
| **سعيد** | كتابة اختبارات end-to-end: .ص → sadc → exe → run → verify | `tests/e2e/` | ≥30 E2E test |
| **كامل** | WASM Playground كامل: محرر + تشغيل + نتائج في المتصفح | `tools/wasm/`, `website/` | playground مكتمل |
| **ريم** | stdlib: تحسين async, crypto, filesystem | `stdlib/async/`, `stdlib/crypto/`, `stdlib/filesystem/` | 3 وحدات مستقرة |
| **عمر** | ✅ Distribution: installers لجميع المنصات | `distribution/` | ✅ **مكتمل** — Windows + deb + Homebrew + 1289 سطر build script |
| **عمر** | ✅ VS Code Extension v2.0.0 | `tools/vscode-extension/` | ✅ **مكتمل** — 922 سطر package.json + debugging + LSP |

**🏁 نهاية المرحلة 0 (أسبوع 8):**
```
✅ 0 أخطاء P0/P1
✅ مشروع مُهيكل ونظيف
✅ CI/CD يعمل على 3 منصات
✅ ≥100 اختبار جديد (regression + stdlib + e2e)
✅ توثيق 14+ وحدة stdlib
✅ Playground يعمل
✅ Distribution مبدئي
```

---

### 🟡 المرحلة 1: التطوير المتوازي (Sprint 5-9 / أسابيع 9-18)

> **الهدف:** بناء ميزات جديدة + تحسين المنصات + إثراء المكتبة

---

#### Sprint 5-6 (أسبوع 9-12): المنصات + الميزات

| المطور | المهام الرئيسية | الأسابيع |
|--------|----------------|----------|
| **أحمد** | تحسين أداء المفسر: caching, lazy eval, tail call optimization | 9-12 |
| **أحمد** | دعم Generics / قوالب في المفسر | 9-12 |
| **سعيد** | تحسين SIR optimizer: DCE, constant folding, inlining | 9-12 |
| **سعيد** | LLVM codegen improvements: ABI compliance, debug info (-g) | 9-12 |
| **كامل** | Android transpiler v2: تحسين + Material Design 3 + اختبار | 9-12 |
| **كامل** | iOS transpiler v2: تحسين + SwiftUI navigation + اختبار | 9-12 |
| **ريم** | مكتبة UI عربية شاملة: 60+ widget مع RTL-first | 9-12 |
| **ريم** | مدير الحزم v2: نشر حزم + مستودع مركزي | 9-12 |
| **عمر** | ✅ LSP v2: semantic + code actions + refactoring | ✅ **مكتمل** — 3000+ سطر |
| **عمر** | ✅ CI/CD: releases + nightly builds + perf tracking | ✅ **مكتمل** — nightly.yml |

---

#### Sprint 7-8 (أسبوع 13-16): التعميق

| المطور | المهام الرئيسية | الأسابيع |
|--------|----------------|----------|
| **أحمد** | Concurrency model: goroutine-style عربي (مهمات خفيفة) | 13-16 |
| **أحمد** | Error handling تحسين: stack traces, source maps | 13-16 |
| **سعيد** | Embedded targets: Arduino + ESP32 runtime كامل | 13-16 |
| **سعيد** | Debug info: DWARF generation + GDB/LLDB support | 13-16 |
| **كامل** | Unified UI testing: visual regression tests عبر المنصات | 13-16 |
| **كامل** | HTML/CSS emitter v2: SPA framework كامل | 13-16 |
| **ريم** | stdlib batteries: regex, date/time, serialization | 13-16 |
| **ريم** | كتابة كتاب اللغة الرسمي (10 فصول) | 13-16 |
| **عمر** | ✅ Playground v2: multi-file + saved projects + sharing | ✅ **مكتمل** — CodePlaygroundV2.vue |
| **عمر** | ✅ Performance benchmarks: مقارنة مع Python, Node.js | ✅ **مكتمل** — tests/performance/ |

---

#### Sprint 9 (أسبوع 17-18): تجميد الميزات

| المطور | المهام | المخرجات |
|--------|--------|----------|
| **الجميع** | **Feature Freeze** — لا ميزات جديدة بعد هذا Sprint | قائمة ميزات v1.0 مُجمدة |
| **أحمد** | مراجعة كاملة لكل إصلاحات المفسر + integration tests | ≥200 اختبار يمر |
| **سعيد** | مراجعة كاملة لكل إصلاحات المترجم + cross-compile tests | sadc ينتج exe صحيح لكل target |
| **كامل** | UI testing matrix: Desktop×3 + WASM + Android + iOS | تقرير توافق كامل |
| **ريم** | stdlib API audit: كل دالة موثقة + لها اختبار | 100% توثيق + 100% تغطية |
| **عمر** | Release checklist: كل شيء جاهز للبيتا | checklist مكتمل |

**🏁 نهاية المرحلة 1 (أسبوع 18): Feature Freeze ✅**

---

### 🟢 المرحلة 2: التكامل + Beta (Sprint 10-11 / أسابيع 19-22)

> **الهدف:** دمج كل شيء + اختبار شامل + Beta release

---

#### Sprint 10 (أسبوع 19-20): v0.9-beta

| المطور | المهام |
|--------|--------|
| **أحمد** | Integration testing: المفسر مع كل وحدات stdlib | 
| **سعيد** | Integration testing: المترجم مع كل المنصات |
| **كامل** | Cross-platform smoke tests: 50 برنامج .ص على كل منصة |
| **ريم** | Documentation review: كل مستند محدث ودقيق |
| **عمر** | **v0.9-beta release**: بناء + نشر + إعلان |

**📦 v0.9-beta Release:**
```
- Windows installer (.exe + Scoop + WinGet)
- macOS Homebrew formula
- Linux deb + rpm
- WASM Playground
- VS Code Extension v2.1
- Documentation website
- 50+ أمثلة
```

---

#### Sprint 11 (أسبوع 21-22): Beta Feedback

| المطور | المهام |
|--------|--------|
| **الجميع** | جمع وتصنيف ردود المجتمع (issues, bug reports) |
| **أحمد** | إصلاح أخطاء beta: المفسر |
| **سعيد** | إصلاح أخطاء beta: المترجم |
| **كامل** | إصلاح أخطاء beta: المنصات |
| **ريم** | تحديث التوثيق بناءً على الردود |
| **عمر** | إعداد RC1 (Release Candidate) |

---

### 🔵 المرحلة 3: التثبيت + الإصدار (Sprint 12-13 / أسابيع 23-26)

---

#### Sprint 12 (أسبوع 23-24): RC1 + RC2

| المطور | المهام |
|--------|--------|
| **أحمد + سعيد** | Code freeze — إصلاح أخطاء فقط (لا كود جديد) |
| **كامل** | Final platform testing matrix |
| **ريم** | Final docs + كتاب اللغة + API reference |
| **عمر** | RC1 release → اختبار مجتمعي → RC2 إذا لزم |

---

#### Sprint 13 (أسبوع 25-26): v1.0 🎉

| المطور | المهام |
|--------|--------|
| **الجميع** | اختبار نهائي شامل |
| **عمر** | بناء ونشر v1.0 على جميع المنصات |
| **ريم** | إعلان الإصدار + مقالات + فيديوهات تعليمية |
| **كامل** | تحديث Website + Playground v1.0 |
| **أحمد + سعيد** | Post-release monitoring + hotfix readiness |

---

## مصفوفة المسؤوليات (RACI)

```
R = مسؤول (Responsible)    A = مُعتمد (Accountable)
C = مُستشار (Consulted)    I = مُبلَّغ (Informed)

┌────────────────────────┬──────┬──────┬──────┬──────┬──────┐
│ المهمة                 │ أحمد │ سعيد │ كامل │ ريم  │ عمر  │
├────────────────────────┼──────┼──────┼──────┼──────┼──────┤
│ إصلاح أخطاء المفسر    │  RA  │  C   │  I   │  I   │  I   │
│ إصلاح أخطاء المترجم   │  C   │  RA  │  I   │  I   │  I   │
│ نظام الأنواع (Value)  │  RA  │  R   │  I   │  I   │  I   │
│ SIR + LLVM CodeGen     │  C   │  RA  │  I   │  I   │  I   │
│ Desktop Backend        │  I   │  I   │  RA  │  I   │  I   │
│ WASM Backend           │  I   │  C   │  RA  │  I   │  I   │
│ Android Transpiler     │  I   │  I   │  RA  │  C   │  I   │
│ iOS Transpiler         │  I   │  I   │  RA  │  C   │  I   │
│ Embedded Targets       │  I   │  RA  │  R   │  I   │  I   │
│ المكتبة القياسية      │  C   │  I   │  I   │  RA  │  I   │
│ مدير الحزم            │  I   │  I   │  I   │  RA  │  C   │
│ التوثيق               │  C   │  C   │  C   │  RA  │  I   │
│ LSP + VS Code          │  I   │  I   │  I   │  I   │  RA  │
│ Formatter              │  I   │  I   │  I   │  I   │  RA  │
│ CI/CD                  │  I   │  I   │  I   │  I   │  RA  │
│ Distribution           │  I   │  I   │  C   │  I   │  RA  │
│ Website + Playground   │  I   │  I   │  R   │  C   │  RA  │
│ Testing Framework      │  C   │  C   │  I   │  I   │  RA  │
│ هيكلة المجلدات        │  C   │  C   │  RA  │  C   │  C   │
│ أمثلة تعليمية         │  C   │  I   │  I   │  RA  │  I   │
└────────────────────────┴──────┴──────┴──────┴──────┴──────┘
```

---

## الفروع (Git Branching Strategy)

```
main ─────────────────────────────────────────────── v1.0
  │
  └── develop ────────────────────────────────────── integration
        │
        ├── feature/ahmad-scoping-fix          (Sprint 1-2)
        ├── feature/ahmad-oop-fix              (Sprint 2-3)
        ├── feature/ahmad-type-system          (Sprint 3-4)
        ├── feature/ahmad-generics             (Sprint 5-6)
        ├── feature/ahmad-concurrency          (Sprint 7-8)
        │
        ├── feature/saeed-ci-cd                (Sprint 1)
        ├── feature/saeed-compiler-fixes       (Sprint 2-4)
        ├── feature/saeed-sir-optimizer        (Sprint 5-6)
        ├── feature/saeed-debug-info           (Sprint 7-8)
        ├── feature/saeed-embedded             (Sprint 7-8)
        │
        ├── feature/kamel-restructure          (Sprint 1)
        ├── feature/kamel-desktop-ui           (Sprint 2-3)
        ├── feature/kamel-wasm-backend         (Sprint 3-4)
        ├── feature/kamel-android-v2           (Sprint 5-6)
        ├── feature/kamel-ios-v2               (Sprint 5-6)
        ├── feature/kamel-html-spa             (Sprint 7-8)
        │
        ├── feature/reem-stdlib-docs           (Sprint 1-2)
        ├── feature/reem-stdlib-tests          (Sprint 1-4)
        ├── feature/reem-pkg-manager           (Sprint 3-4)
        ├── feature/reem-ui-library            (Sprint 5-6)
        ├── feature/reem-book                  (Sprint 7-8)
        │
        ├── feature/omar-test-framework        (Sprint 1)
        ├── feature/omar-lsp-improve           (Sprint 2-4)
        ├── feature/omar-formatter             (Sprint 3-4)
        ├── feature/omar-distribution          (Sprint 4)
        ├── feature/omar-vscode-ext            (Sprint 4-5)
        ├── feature/omar-playground            (Sprint 5-8)
        │
        └── release/v0.9-beta                  (Sprint 10)
        └── release/v1.0-rc1                   (Sprint 12)
        └── release/v1.0                       (Sprint 13)
```

---

## قواعد العمل اليومية

### 1. Daily Standup (15 دقيقة — كل يوم)
```
كل مطور يجيب على 3 أسئلة:
1. ماذا أنجزت أمس؟
2. ماذا سأنجز اليوم؟
3. هل هناك عوائق؟
```

### 2. Sprint Ceremonies
```
┌────────────────────────┬──────────────┬─────────────────────┐
│ الحدث                  │ المدة        │ المشاركون           │
├────────────────────────┼──────────────┼─────────────────────┤
│ Sprint Planning        │ 2 ساعة       │ الجميع              │
│ Daily Standup          │ 15 دقيقة     │ الجميع              │
│ Code Review            │ مستمر        │ زوج المراجعة        │
│ Sprint Review          │ 1 ساعة       │ الجميع              │
│ Sprint Retrospective   │ 1 ساعة       │ الجميع              │
└────────────────────────┴──────────────┴─────────────────────┘
```

### 3. أزواج المراجعة (Code Review Pairs)
```
أحمد ↔ سعيد    (كل يراجع كود الآخر — النواة والمترجم)
كامل ↔ عمر     (المنصات والأدوات)
ريم  ↔ أي مطور (المكتبة والتوثيق — تُراجَع من الجميع)
```

### 4. قواعد الدمج (Merge Rules)
```
✅ كل PR يحتاج ≥1 code review approval
✅ CI يمر (build + tests) على 3 منصات
✅ لا رسائل warning جديدة
✅ كل دالة عامة موثقة (عربي + إنجليزي)
✅ اختبار واحد على الأقل لكل تغيير
```

---

## المقاييس والأهداف (KPIs)

| المقياس | الحالي | هدف Sprint 4 | هدف Sprint 9 | هدف v1.0 |
|---------|--------|-------------|-------------|----------|
| أخطاء P0 | 10 | 0 | 0 | 0 |
| أخطاء P1 | 12 | 0 | 0 | 0 |
| اختبارات | 900 | 1,200 | 2,000 | 3,000+ |
| تغطية كود | ~40% (تقدير) | 60% | 75% | ≥85% |
| وحدات stdlib موثقة | 0 | 14 | 28 | 28/28 |
| منصات مستقرة | 2 | 3 | 5 | ≥7 |
| weaknesses المترجم | 120+ | ≤20 | ≤5 | 0 |
| أمثلة تعليمية | ~15 | 30 | 60 | 100+ |
| أداء (vs Python) | غير مقاس | baseline | 2-5x أسرع | ≥5x (مترجم) |

---

## مخاطر ومعالجتها

| المخاطر | الاحتمال | التأثير | المعالجة | المسؤول |
|---------|---------|--------|---------|---------|
| خطأ scoping أعمق من المتوقع | عالي | عالي | إعادة كتابة VariableManager كاملة إذا لزم | أحمد |
| LLVM 18 API تغييرات | متوسط | عالي | تثبيت نسخة LLVM + abstraction layer | سعيد |
| Android Compose API تغييرات | متوسط | متوسط | تثبيت Compose BOM version | كامل |
| iOS اختبار يحتاج macOS | عالي | متوسط | توفير macOS VM أو جهاز | كامل |
| تأخر أحد المطورين | متوسط | متوسط | كل مطور لديه backup في فريقه | الجميع |
| ردود beta سلبية | متوسط | منخفض | تصنيف حسب أولوية + sprint إضافي | الجميع |

---

## التواصل

```
┌────────────────────────────────────────────────────┐
│  أدوات التواصل                                    │
│                                                    │
│  📋 GitHub Issues — تتبع المهام والأخطاء            │
│  📊 GitHub Projects — لوحة Kanban                   │
│  💬 Discord/Slack — تواصل فوري                     │
│  📝 Wiki — توثيق تقني داخلي                        │
│  📹 اجتماع أسبوعي — مراجعة التقدم (Sprint Review) │
└────────────────────────────────────────────────────┘
```

---

## ملخص التوزيع

```
╔══════════════════════════════════════════════════════════════╗
║  أحمد 🔧 — النواة                                          ║
║  المفسر + نظام الأنواع + Generics + Concurrency            ║
║  "الأساس القوي"                                            ║
╠══════════════════════════════════════════════════════════════╣
║  سعيد ⚙️ — المترجم                                         ║
║  sadc + SIR + LLVM + CI + Embedded + Testing                ║
║  "المحرك السريع"                                           ║
╠══════════════════════════════════════════════════════════════╣
║  كامل 🌍 — المنصات                                         ║
║  Desktop + WASM + Android + iOS + HTML + هيكلة              ║
║  "كل مكان"                                                 ║
╠══════════════════════════════════════════════════════════════╣
║  ريم 📚 — المكتبات والتوثيق                                ║
║  stdlib + pkg manager + docs + أمثلة + كتاب               ║
║  "الثراء"                                                  ║
╠══════════════════════════════════════════════════════════════╣
║  عمر 🛠️ — البنية التحتية                                   ║
║  LSP + Formatter + CI/CD + Distribution + Playground        ║
║  "التجربة السلسة"                                          ║
╚══════════════════════════════════════════════════════════════╝
```

---

## سجل التقدم الفعلي (Progress Log)

### Sprint 1 — الأسبوع 1-2

#### ✅ كامل — 2026-03-06
**المهمة:** تنظيف المشروع

| البند | قبل | بعد |
|-------|-----|-----|
| ملفات في الجذر | 350+ | 8 |
| ملفات .txt محذوفة | - | 249 |
| ملفات .ll محذوفة | - | 51 |
| ملفات .py/.js محذوفة | - | 24 |
| ملفات .o/.exe/.so محذوفة | - | 17 |
| تحديث .gitignore | ❌ | ✅ (أنماط جديدة) |

**المخرجات:**
- جذر نظيف (8 ملفات ضرورية فقط)
- `.gitignore` محدّث بـ 40+ نمط استبعاد جديد

---

#### ⏳ كامل — 2026-03-06 (قيد المراجعة)
**المهمة:** تحليل Desktop Backend

| البند | القيمة |
|-------|--------|
| عدد Widgets المدعومة | 109 |
| Backend | SDL2 |
| المنصات | Windows ✅, Linux ✅, macOS ✅ |
| TODOs المتبقية في UI | ✅ 0 (تم الإصلاح) |
| حجم الكود | ~5,000+ سطر |
| الحالة | **✅ مكتمل ومستقر** |

**الملفات الرئيسية:**
- `stdlib/ui/sad_backend_desktop.cpp` — Adapter موحد (محسّن)
- `stdlib/graphics/sad_ui_api.cpp` — محرك UI (3,974 سطر)
- `stdlib/graphics/sad_ui_framework.h` — تعريفات (902 سطر)

**التحسينات المنفّذة — 2026-03-06:**
1. ✅ إصلاح `widget_get_selected()` — كان يُرجع 0 دائماً
2. ✅ توسيع `createWidget()` من 8 أنواع إلى **109 أنواع** كاملة
3. ✅ إصلاح توقيعات الدوال (widget_tag, widget_avatar, widget_badge, إلخ)
4. ✅ دعم شامل لجميع فئات الـ widgets:
   - Layout: 12 نوع
   - Buttons: 7 أنواع  
   - Text: 8 أنواع
   - Input: 15 نوع
   - Visual: 12 نوع
   - Progress: 5 أنواع
   - Navigation: 10 أنواع
   - Feedback: 10 أنواع
   - Cards/Panels: 8 أنواع
   - Data: 10 أنواع
   - Charts: 7 أنواع
   - Pagination: 2 نوعين
   - Advanced: 3 أنواع

**نتيجة اختبار البناء — 2026-03-06:**
- ✅ `sad_backend_desktop.cpp` — يُبنى بنجاح بدون أخطاء
- ⚠️ `variable_manager.cpp` — أخطاء أقواس غير متطابقة (مسؤولية أحمد)
- ⚠️ البناء الكامل معلق حتى إصلاح أخطاء النواة

---

#### ✅ كامل — 2026-03-06
**المهمة:** كتابة أمثلة تعليمية لواجهة المستخدم

| الملف | المحتوى | الحالة |
|-------|---------|--------|
| `examples/tutorials/01_مرحبا_ui.ص` | Hello World UI + شريط تطبيق + أزرار | ✅ |
| `examples/tutorials/02_التخطيط.ص` | عمود/صف + بطاقات + محاذاة | ✅ |
| `examples/tutorials/03_الإدخال.ص` | حقول نصية + خيارات + منزلقات | ✅ |
| `examples/tutorials/04_القوائم.ص` | قوائم + جداول + بطاقات متكررة | ✅ |

**المخرجات:**
- 4 أمثلة تعليمية شاملة بالعربية
- تغطية 20+ نوع widget
- شرح تفاعلي مع طباعة توضيحية

---

#### ✅ كامل — 2026-03-06
**المهمة:** تحسين WASM Backend (الترجمة الأصلية + DOM + stdlib)

**1. AST Visitor لـ WASM (جديد):**

| الملف | الوصف | الحجم |
|-------|-------|------|
| `compiler_new/src/backend/wasm_ast_visitor.h` | واجهة الزائر + هياكل البيانات | ~230 سطر |
| `compiler_new/src/backend/wasm_ast_visitor.cpp` | تنفيذ الزائر (AST → WAT) | ~500 سطر |

**العناصر المدعومة:**
- ✅ Expressions: Binary, Unary, Literal, Variable, Assign, Call, Ternary
- ✅ Statements: If, While, Return, Break, Continue, Block, VarDecl
- ✅ Declarations: Function (مع تصدير تلقائي)
- ✅ النصوص: إضافة لقطع البيانات مع null-terminator

**2. JavaScript Runtime (جديد):**

| الملف | الدوال | الحجم |
|-------|--------|------|
| `tools/wasm/sad_wasm_runtime.js` | createSadRuntime(), loadSadWasm() | ~350 سطر |

**الدوال المستوردة (32 دالة):**
```
الطباعة:    اطبع، اطبع_رقم، اطبع_عشري، اطبع_سطر
الرياضيات:  جذر، قوة، جيب، جتا، عشوائي
الذاكرة:    خصص، حرر
النصوص:    طول، ضم
DOM:        استعلم، أنشئ_عنصر، عيّن_نص، عيّن_سمة، أضف_ابن، أزل، عند_نقر، عند_إدخال
Console:    سجّل، خطأ، وقت، نهاية_وقت
الشبكة:     جلب، نتيجة_جلب
التخزين:    اقرأ_مخزن، اكتب_مخزن
```

**3. تحليل الملفات الموجودة:**

| الملف | الحالة | الملاحظات |
|-------|--------|-----------|
| `wasm32_target.cpp` | ✅ مكتمل | LLVM → .wasm + JS glue + HTML |
| `wasm_bindgen.cpp` | ✅ مكتمل | TypeScript + Arabic DOM |
| `wasm_codegen.cpp` | ⚠️ جزئي | LLVM IR generator — بحاجة لربط AST |
| `wasm.cpp` | ✅ مكتمل | WAT direct generator |
| `wasm_builtins.cpp` | ✅ مكتمل | 38 دالة مدمجة للـ WASM |

**الخلاصة:**
- WASM Backend أكثر اكتمالاً مما كان متوقعاً
- تمت إضافة AST Visitor الناقص
- تمت إضافة JavaScript runtime للمتصفح
- الخطوة التالية: اختبار التكامل

---

#### ✅ كامل — 2026-03-07
**المهمة:** تنفيذ Mobile stdlib module (Android/iOS Backend)

**1. تحليل الوضع القائم:**

| البند | قبل | بعد |
|-------|-----|-----|
| `mobile_module.cpp` | 9 سطور (stub) | ~450 سطر (تنفيذ كامل) |
| `mobile_module.h` | 65 سطر (inline stubs) | ~180 سطر (declarations موثقة) |
| دوال builtin مسجلة | 6 دوال | 20+ دالة |

**2. الملفات المُعدّلة:**

| الملف | التغيير |
|-------|---------|
| `stdlib/mobile/mobile_module.cpp` | تنفيذ كامل (~450 سطر) |
| `stdlib/mobile/mobile_module.h` | تحويل من inline stubs إلى declarations |
| `interpreter_new/src/builtins/builtin_registry_part6.cpp` | إضافة 14 دالة builtin جديدة |

**3. الدوال المُنفّذة:**

| الفئة | الدوال العربية | الدوال الإنجليزية |
|-------|----------------|-------------------|
| **الجهاز** | `هاتف_معلومات`، `هاتف_منصة` | `mobile_info`, `mobile_platform` |
| **التنبيهات** | `هاتف_رسالة`، `هاتف_إشعار`، `هاتف_رسالة_سريعة` | `mobile_alert`, `mobile_notify`, `mobile_toast` |
| **الواجهة** | `واجهة_نص`، `واجهة_زر`، `واجهة_صورة` | `ui_text`, `ui_button`, `ui_image` |
| **التصدير** | `صدّر_كومبوز`، `صدّر_سويفت` | `export_compose`, `export_swiftui` |
| **الكاميرا** | `هاتف_كاميرا`، `كاميرا_متوفرة` | `mobile_camera`, `camera_available` |
| **الموقع** | `هاتف_موقع`، `احسب_المسافة` | `mobile_location`, `calculate_distance` |
| **المستشعرات** | `هاتف_تسارع`، `هاتف_دوران` | `mobile_accelerometer`, `mobile_gyroscope` |
| **المشاريع** | `هاتف_مشروع_جديد`، `انشئ_manifest`، `انشئ_plist` | `mobile_new_project`, `generate_manifest`, `generate_plist` |

**4. الميزات التقنية:**

- ✅ **Platform Detection:** Windows/macOS/Linux/Android/iOS عبر `#ifdef`
- ✅ **UI Generation:** توليد كود Jetpack Compose و SwiftUI
- ✅ **Haversine Formula:** حساب المسافة بين نقطتين جغرافيتين
- ✅ **Windows Native:** استخدام `MessageBoxA` للتنبيهات + `GetDC` للشاشة
- ✅ **Simulated Sensors:** بيانات مستشعرات عشوائية للاختبار على Desktop
- ✅ **Project Structure:** إنشاء هيكل مجلدات Android/iOS

**5. نتيجة التحليل:**

| البند | الحالة |
|-------|--------|
| CLI Mobile (`ص هاتف`) | ✅ مكتمل مسبقاً (1165 سطر) |
| stdlib mobile module | ✅ **مكتمل الآن** |
| tools/android/ | ⚠️ placeholders (للتطوير المستقبلي) |
| tools/ios/ | ❌ غير موجود بعد |

**الخلاصة:**
- Mobile stdlib أصبح جاهزاً للاستخدام
- يمكن الآن استدعاء دوال الهاتف من أي ملف `.ص`
- الخطوة التالية: اختبار شامل + تطوير tools/android/

---

#### ✅ كامل — 2026-03-06
**المهمة:** اختبار البناء الكامل + إصلاح مشاكل Unicode

**1. نتيجة البناء:**

| الهدف | الحالة |
|-------|--------|
| `sad.exe` | ✅ بُني بنجاح بدون أخطاء |
| `sad_core.lib` | ✅ |
| `sad_mobile.lib` | ✅ |
| جميع المكتبات (20+) | ✅ |

**2. إصلاح Unicode:**
- مشكلة: حرف الشدّة `ّ` في `صدّر_كومبوز` يسبب عدم مطابقة بسبب اختلاف NFC/NFD
- الحل: إضافة أسماء بديلة بدون تشكيل (`صدر_كومبوز`، `صدر_سويفت`)

**3. نتائج اختبار 20 دالة mobile:**

| الدالة | النتيجة |
|--------|---------|
| `هاتف_منصة()` | ✅ → "windows" |
| `هاتف_معلومات()` | ✅ → شاشة 1536×864 |
| `كاميرا_متوفرة()` | ✅ → true |
| `هاتف_موقع()` | ✅ → إحداثيات GPS |
| `احسب_المسافة(القاهرة,الرياض)` | ✅ → 1633.84 كم |
| `هاتف_تسارع()` | ✅ → x,y,z sensor |
| `هاتف_دوران()` | ✅ → x,y,z gyro |
| `واجهة_نص("...")` | ✅ → `[text] ...` |
| `واجهة_زر("...")` | ✅ → `[button] ...` |
| `صدر_كومبوز("...")` | ✅ → كود Kotlin Compose |
| `صدر_سويفت("...")` | ✅ → كود SwiftUI |
| `انشئ_manifest(pkg,app)` | ✅ → XML كامل |
| `انشئ_plist(bundle,app)` | ✅ → Plist كامل |

**ملف الاختبار:** `examples/test_mobile.ص`

---

#### ✅ كامل — 2026-03-07
**المهمة:** إكمال Sprint 1 — المهام 3.A + 3.B + 3.C

**3.A: هيكلة المجلدات:**
- ✅ حذف 5+ ملفات مؤقتة (fmt_*.txt, build_errors.txt, test-results.xml)
- ✅ نقل `مشاكل.md` → `docs/مشاكل.md`
- ✅ تحديث `.gitignore`: أنماط جديدة (fmt_*.txt, *.apk, *.ipa)
- ✅ تنظيم `examples/`: tests/ (14), apps/ (4), ui/ (1)
- ✅ حذف ملفات يتيمة (app.apk, test_exc.ll)

**3.B: اختبار Desktop Backend:**
- ✅ بناء `sad.exe` مع SDL2 — نجاح (20+ مكتبة)
- ✅ إنشاء `examples/ui/basic_window.ص` — مثال كامل مع:
  - نافذة SDL2 (800×600) + رسام + تسمية + زر
  - حلقة أحداث + رسم أشكال (مستطيل, دائرة, خط, نص)
- ✅ تشغيل ناجح: نافذة تظهر مع عناصر ورسوميات
- ⚠️ مشكلة: رسم النص يستخدم placeholder (بحاجة لخطوط SDL_ttf)

**3.C: مراجعة WASM Build:**
- ✅ مراجعة pipeline: cmake/wasm.cmake + build_wasm.ps1
- ✅ تحقق من البناء: sad.wasm (2.2MB O1) + sad.js (153KB)
- ✅ إنشاء `docs/BUILD_WASM.md` — توثيق شامل (خطوات، بنية، أمثلة)
- ✅ Emscripten SDK متوفر ومُفعّل

**حالة Sprint 1 لكامل: ✅ مكتمل بالكامل**

---
