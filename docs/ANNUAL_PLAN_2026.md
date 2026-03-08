# الخطة السنوية لمشروع لغة ص — 2026
# تاريخ الإعداد: 6 مارس 2026
# المدة: 12 شهراً (مارس 2026 → فبراير 2027)

## تحديث التنفيذ (الجلسة الحالية)

> تاريخ التحديث: 6 مارس 2026
> المنفذ: ريم (توثيق ومحتوى)

### ما تم تنفيذه فعلياً الآن

- إنشاء ملفات الحوكمة في الجذر:
        - `LICENSE`
        - `CODE_OF_CONDUCT.md`
        - `SECURITY.md`
        - `CHANGELOG.md`
- تحديث احترافي لملف `README.md`.
- إنشاء مسار كتاب `docs/book/` بـ 16 فصلاً + فهرس.
- إنشاء مسار تحويل الكتاب إلى HTML/PDF:
        - `scripts/build_book.ps1`
        - `docs/book/BUILD.md`
        - ناتج HTML فعلي: `build/book/book.html`
        - ناتج PDF فعلي: `build/book/book.pdf`
- استكمال API docs بإضافة:
        - `docs/api/json.md`
        - `docs/api/filesystem.md`
- إنشاء توثيق المترجم:
        - `docs/compiler/README.md`
        - `docs/compiler/architecture.md`
        - `docs/compiler/usage.md`
        - `docs/compiler/troubleshooting.md`
- إنشاء المنهج الجامعي:
        - `docs/curriculum/README.md`
        - `docs/curriculum/weekly-plan.md`
- إنشاء 5 مقالات "لماذا لغة ص" في `docs/blog/`.
- إنشاء `docs/awesome-sad.md` وتوسيعه إلى 50+ مورد مصنف.
- إنشاء مسار ترجمة داخل المستودع `docs/en/` (4 ملفات).
- رفع أمثلة التطبيقات إلى 10 ملفات في `examples/apps/`.
- تنفيذ تدقيق روابط عميق لمستندات النطاق النشط:
        - `README.md`
        - `docs/book/`, `docs/api/`, `docs/compiler/`, `docs/curriculum/`, `docs/en/`
        - `website/docs/book/`

### ملاحظة مهمة

بعض عناصر نهاية السنة ذات طبيعة خارج المستودع أو تشغيلية (نشر domain، فيديوهات، شراكات جامعية، حملة إطلاق اجتماعية) وتم تهيئتها توثيقياً داخل المشروع لكنها تحتاج تنفيذاً تشغيلياً خارج Git.

---

## القسم الأول: تقرير الوضع الحالي (مارس 2026)

---

### 1. حجم المشروع

```
╔══════════════════════════════════════════════════════════════╗
║  مشروع لغة ص — إحصائيات مارس 2026                         ║
╠══════════════════════════════════════════════════════════════╣
║                                                              ║
║  المكون              الملفات    الأسطر      الحالة          ║
║  ─────────────────   ───────    ──────      ──────          ║
║  النواة المشتركة      80+       63,314     ✅ مستقر         ║
║  المفسر              60+       44,971     ✅ يعمل          ║
║  المترجم             273+     195,349     ⚠️ ربط معطل      ║
║  المكتبة القياسية    103+      80,404     🟡 جزئي          ║
║  الاختبارات          174+      ─────      ✅ 48/48 pass     ║
║  ─────────────────────────────────────────────────           ║
║  المجموع التقريبي:   700+     384,038 سطر                  ║
║                                                              ║
╠══════════════════════════════════════════════════════════════╣
║  الأهداف التنفيذية (Executables)                            ║
║  ──────────────────────────────                              ║
║  sad.exe          15.6 MB   ✅ يعمل (مفسر + 39 builtin)    ║
║  sadc.exe         75.8 MB   ✅ Release يعمل / ❌ Debug      ║
║  sad-lsp.exe       4.8 MB   ✅ v2.0.0 (LSP 3.17)           ║
║  sad-pkg.exe       2.0 MB   ✅ v2.0.0                       ║
║  sad-fmt.exe       1.0 MB   ✅ يعمل                         ║
║  31 مكتبة .lib     ─────    ✅ تبني جميعها                  ║
║  36 اختبار .exe    ─────    ✅ 48/48 CTest pass             ║
║                                                              ║
╠══════════════════════════════════════════════════════════════╣
║  أدوات إضافية مبنية                                        ║
║  ──────────────────────                                      ║
║  sad-apk.exe            ✅   بناء حزم Android               ║
║  sad_android_builder    ✅   باني Android                    ║
║  sad_desktop_demo       ✅   عرض واجهات Desktop              ║
║  sad_ui_demo2           ✅   عرض UI                          ║
║  benchmark_arabic_opt   ✅   أداء التحسينات                  ║
╚══════════════════════════════════════════════════════════════╝
```

### 2. اختبارات الانحدار — 22 مشكلة أصلية

```
╔═════════════════════════════════════════════════════════════════╗
║  22/22 PASS ✅  — جميع المشاكل الأصلية مُصلحة                  ║
╠═════════════════════════════════════════════════════════════════╣
║                                                                 ║
║  P0 (حرجة):                                                    ║
║  ✅ #1  array[i]["key"] crash داخل الدوال                       ║
║  ✅ #2  متغير x = x + 1 حلقة لانهائية (anti-shadowing)         ║
║  ✅ #3  .الطول() crash عند عمق ≥ 2                              ║
║  ✅ #10 ارجع بدون قيمة خطأ نحوي                                ║
║  ✅ #11 أصناف مخصصة كأنواع حقول                                ║
║  ✅ #12 القسمة تنتج float (8/4=2.0)                            ║
║  ✅ #16 += على حقل كائن داخل مصفوفة                             ║
║  ✅ #18 هذا.method() تغييرات لا تُحفظ                           ║
║  ✅ #19 القيم السادسة عشرية 0xFF                                ║
║                                                                 ║
║  مهمة:                                                          ║
║  ✅ #4  همزة تحت في اسم متغير                                   ║
║  ✅ #8  توقف/break في الحلقات                                   ║
║  ✅ #9  كلمات محجوزة غير موثقة                                   ║
║  ✅ #13 صنف كبادئة معرّف                                        ║
║  ✅ #14 أخطاء متسلسلة                                           ║
║  ✅ #17 جديد وسجل محجوزتان                                      ║
║  ✅ #20 أساس محجوزة                                             ║
║  ✅ #21 قالب محجوزة                                             ║
║                                                                 ║
║  تحسين:                                                         ║
║  ✅ #5  خلط عربي/لاتيني                                        ║
║  ✅ #6  أعداد كبيرة بتدوين علمي                                 ║
║  ✅ #7  += للمصفوفات                                            ║
║  ✅ #15 ترميز UTF-8 في Windows                                  ║
║  ✅ #22 صحيح كمعرّف                                             ║
║                                                                 ║
║  CTest: 48/48 PASS (100%) — 4.08 ثانية                         ║
╚═════════════════════════════════════════════════════════════════╝
```

### 3. اختبار وظيفي شامل — نتيجة فعلية (مارس 2026)

```
البرنامج:
─────────
صنف + باني + دوال عضوية + تعديل حقول عبر methods
مصفوفة كائنات + لكل + وصول حقول
دوال + معاملات + ارجع + مصفوفات كمعاملات
مطابقة أنماط (طابق/عندما/افتراضي) مع نطاقات

النتيجة: ✅ يعمل بالكامل
─────────
مرحبا أنا أحمد عمري 25
بعد عيد الميلاد: 26
مرحبا أنا سارة عمري 30
بعد عيد الميلاد: 31
مجموع = 15
25 سنة = بالغ
10 سنوات = طفل
```

### 4. المشاكل المفتوحة الحالية

```
╔═════════════════════════════════════════════════════════════════╗
║  المشاكل الحرجة المتبقية (مارس 2026)                          ║
║  [تحديث: تم إصلاح عدة مشاكل — انظر أدناه]                    ║
╠═════════════════════════════════════════════════════════════════╣
║                                                                 ║
║  🔴 حرجة:                                                      ║
║  ───────                                                        ║
║  C1. sadc.exe لا يبني في Debug                                  ║
║      السبب: LLVM 18 مبني Release / sadc يُبنى Debug             ║
║      _ITERATOR_DEBUG_LEVEL mismatch (3908 خطأ ربط)              ║
║      الحل: بناء LLVM في Debug أو بناء sadc في Release فقط      ║
║      sadc Release يعمل (75.8 MB) ✅                              ║
║      ✅ أُضيف guard في cmake/llvm.cmake يحذر من المزج            ║
║                                                                 ║
║  C2. 580 ملف غير محفوظ في Git                                   ║
║      الفرع الحالي: graphic (ليس main أو develop)                ║
║      آخر push: 74925ff3 على origin/graphic                       ║
║      3 commits محلية غير مُرسلة                                  ║
║                                                                 ║
║  C3. CI workflows بها أخطاء صياغة                               ║
║      release.yml: Context access invalid                         ║
║      nightly.yml: ✅ تم إصلاح $(date) → steps.date.outputs      ║
║                                                                 ║
║  🟡 متوسطة:                                                    ║
║  ──────────                                                      ║
║  M1. test_system_comprehensive.exe: namespace mismatch           ║
║  M2. LSP: ملفات legacy غير محذوفة                               ║
║  M3. VS Code Extension: لم تُنشر في Marketplace                ║
║  M4. الموقع: لم يُنشر على domain                                ║
║  M5. ✅ LICENSE موجود في الجذر (MIT)                             ║
║  M6. ✅ CODE_OF_CONDUCT موجود                                    ║
║  M7. ✅ CHANGELOG.md موجود                                       ║
║  M8. لا يوجد GitHub Releases                                    ║
║                                                                 ║
║  🟢 تحسين:                                                     ║
║  ──────────                                                      ║
║  L1. 60+ مجلد أداة (معظمها هياكل فارغة)                        ║
║  L2. node_modules متداخلة في vscode-extension (symlink loop)    ║
║  L3. ترميز العربية مشوش في PowerShell                           ║
║  L4. formatter بسيط جداً (لا يدعم كل الميزات)                  ║
╚═════════════════════════════════════════════════════════════════╝
```

### 5. تقييم النضج لكل مكون

```
╔══════════════════════════════════════════════════════════════════╗
║  مكون                     النضج   الجاهزية للنشر   ملاحظات     ║
╠══════════════════════════════════════════════════════════════════╣
║  Lexer (تحليل معجمي)      95%     ✅ جاهز          120+ token  ║
║  Parser (تحليل نحوي)      90%     ✅ جاهز          31 parser   ║
║  AST                       90%     ✅ جاهز          16 عقدة     ║
║  Interpreter (مفسر)       85%     ✅ جاهز          OOP يعمل    ║
║  Value system              75%     🟡 كافٍ          8 أنواع     ║
║  Compiler (sadc)           70%     🟡 Release فقط   SIR+LLVM   ║
║  VM                        60%     🟡 أساسي         مرتبط      ║
║  stdlib/core               85%     ✅ جاهز                      ║
║  stdlib/io,math,string     80%     ✅ جاهز                      ║
║  stdlib/network,http       50%     ❌ جزئي                      ║
║  stdlib/graphics,ui        45%     ❌ SDL2 فقط                  ║
║  stdlib/database,crypto    15%     ❌ مُخطط                     ║
║  LSP server                70%     🟡 يعمل         24 provider ║
║  Formatter                 40%     ❌ بسيط                      ║
║  Package Manager           50%     🟡 v2 CLI        لا registry║
║  REPL                      65%     🟡 يعمل                      ║
║  VS Code Extension         75%     ✅ v2.0          لم تُنشر   ║
║  Website                   45%     ❌ VitePress      لم يُنشر   ║
║  Distribution              60%     🟡 installers     لا releases║
║  Documentation             80%     ✅ مرجع شامل                 ║
║  CI/CD                     50%     ❌ أخطاء صياغة               ║
║  WASM                      55%     🟡 يبني          يحتاج اختبار║
╚══════════════════════════════════════════════════════════════════╝
```

---

### 6. إصلاحات جودة الكود المُنجزة (مارس 2026)

```
╔══════════════════════════════════════════════════════════════════╗
║  فحص شامل لجودة الكود — 35+ إصلاح عبر 25+ ملف                ║
╠══════════════════════════════════════════════════════════════════╣
║                                                                  ║
║  المجلدات المفحوصة:                                             ║
║  ✅ compiler_new/src/{frontend, types, semantic, sir}            ║
║  ✅ compiler_new/src/backend/ (+ llvm/)                         ║
║  ✅ interpreter_new/src/                                        ║
║  ✅ shared/{parser, ast, modules}                               ║
║                                                                  ║
║  الأنماط المُصلحة:                                              ║
║  • 15× static std::string → thread_local (أمان الخيوط)         ║
║  • 8× فحوصات null مضافة (منع crashes)                          ║
║  • 3× reinterpret_cast → toString() (مفاتيح آمنة)             ║
║  • 1× std::atomic بدل static counter (منافسة بيانات)           ║
║  • 1× analyzeExports() مُنفّذة (كانت فارغة)                    ║
║  • 1× checkConflicts() مُنفّذة (كانت فارغة)                    ║
║  • 1× template substitution مُصلحة (أنواع الإرجاع)             ║
║  • 1× TypeCheckError API مُصلحة (struct بدل overload)           ║
║  • 1× hash improvement (golden ratio bit mixing)                ║
║  • 1× bounds validation (deserialization limits)                ║
║  • 2× empty stubs → warning messages (ActorRef)                ║
║                                                                  ║
║  النتيجة: البناء ✅ | التشغيل ✅ | 0 regressions                ║
╚══════════════════════════════════════════════════════════════════╝
```

---

## القسم الثاني: الخطة السنوية — 4 أرباع × 5 مطورين متوازيين

---

### الفلسفة: عمل متوازي 100% — لا تبعيات بين المطورين

```
╔══════════════════════════════════════════════════════════════════════╗
║  مبدأ التوازي الكامل:                                              ║
║                                                                      ║
║  كل مطور يمتلك "مساره" الخاص من الملفات التي لا يلمسها أحد غيره   ║
║  لا يوجد أي موقف ينتظر فيه مطور عمل مطور آخر                      ║
║                                                                      ║
║  أحمد ──→ interpreter_new/ + shared/types/                          ║
║  سعيد ──→ compiler_new/ + cmake/ + .github/                        ║
║  كامل ──→ stdlib/ + vm/ + graphics/ + network/                     ║
║  ريم  ──→ docs/ + examples/ + website/ + specs/                    ║
║  عمر  ──→ tools/ + distribution/ + tests/                          ║
║                                                                      ║
║  نقاط التقاء: code review فقط — لا blocking dependencies            ║
╚══════════════════════════════════════════════════════════════════════╝
```

### خريطة الملكية (Ownership Map)

```
╔════════════════════════════════════════════════════════════════════╗
║                                                                    ║
║  أحمد 🔧 — مالك النواة                                           ║
║  ────────────────────────                                          ║
║  interpreter_new/src/core/                                         ║
║  interpreter_new/src/visitors/                                     ║
║  interpreter_new/src/managers/                                     ║
║  interpreter_new/src/oop/                                          ║
║  interpreter_new/src/builtins/                                     ║
║  shared/types/                                                     ║
║  shared/ast/                                                       ║
║  shared/parser/                                                    ║
║                                                                    ║
║  سعيد ⚙️ — مالك المترجم والبنية التحتية                          ║
║  ──────────────────────────────────────                             ║
║  compiler_new/src/                                                 ║
║  compiler_new/include/                                             ║
║  cmake/                                                            ║
║  .github/workflows/                                                ║
║  shared/lexer/ (تغييرات tokens فقط)                                ║
║                                                                    ║
║  كامل 🌍 — مالك المكتبات والمنصات                                 ║
║  ───────────────────────────────                                   ║
║  stdlib/ (جميع المجلدات)                                           ║
║  vm/                                                               ║
║  graphics/                                                         ║
║  network/                                                          ║
║  runtime_new/ (إذا أعيد تفعيله)                                    ║
║                                                                    ║
║  ريم 📚 — مالكة التوثيق والمحتوى                                  ║
║  ──────────────────────────────                                     ║
║  docs/ (جميع الملفات)                                              ║
║  examples/ (جميع المجلدات)                                         ║
║  website/                                                          ║
║  specs/                                                            ║
║  README.md, CONTRIBUTING.md, CHANGELOG.md                          ║
║  LICENSE, CODE_OF_CONDUCT.md                                       ║
║  bee_os/ (توثيق فقط)                                               ║
║                                                                    ║
║  عمر 🛠️ — مالك الأدوات والتوزيع                                  ║
║  ─────────────────────────────                                     ║
║  tools/lsp/                                                        ║
║  tools/formatter/                                                  ║
║  tools/pkg/                                                        ║
║  tools/repl/                                                       ║
║  tools/vscode-extension/                                           ║
║  tools/docgen/                                                     ║
║  distribution/                                                     ║
║  tests/ (framework + infrastructure)                               ║
║                                                                    ║
╚════════════════════════════════════════════════════════════════════╝
```

---

## الربع الأول (Q1): مارس — مايو 2026
### العنوان: "التثبيت والتفعيل"

```
╔══════════════════════════════════════════════════════════════════════╗
║  الهدف: تثبيت النواة + نشر v0.5-alpha + بناء الحضور العام         ║
║  المُخرج: أول إصدار عام + أول ظهور للمجتمع                        ║
╚══════════════════════════════════════════════════════════════════════╝
```

### أحمد 🔧 — Q1: تعزيز المفسر

| الشهر | المهمة | الملفات | المُخرج |
|-------|--------|---------|---------|
| مارس | نظام أنواع موسّع | `shared/types/include/value.h` | إضافة Byte, Tuple, Set + تحويلات نوع آمنة |
| مارس | Generics أساسية | `interpreter_new/src/visitors/` | `دالة<ن> هوية(قيمة: ن): ن` يعمل |
| أبريل | Async/Await في المفسر | `interpreter_new/src/builtins/` | `غير_متزامن دالة` + `انتظر` يعملان |
| أبريل | Generators | `interpreter_new/src/visitors/` | `أنتج` + `مولد` يعملان |
| مايو | Traits/Interfaces | `interpreter_new/src/oop/` | `سمة` + `نفّذ` يعملان |
| مايو | Destructuring | `interpreter_new/src/visitors/` | `متغير {أ، ب} = كائن` يعمل |

**معايير القبول Q1 أحمد:**
- [ ] 12+ نوع في Value (بدل 8)
- [ ] Generics مع type inference
- [ ] async/await مع promise-based model
- [ ] 3+ اختبار لكل ميزة جديدة

---

### سعيد ⚙️ — Q1: إصلاح المترجم + CI

| الشهر | المهمة | الملفات | المُخرج |
|-------|--------|---------|---------|
| مارس | إصلاح LLVM Debug linking | `cmake/llvm.cmake` | sadc يبني في Debug + Release |
| مارس | إصلاح CI workflows | `.github/workflows/*.yml` | ci + release + nightly تعمل بدون أخطاء |
| مارس | أول GitHub Release | `.github/workflows/release.yml` | v0.5-alpha مع artifacts |
| أبريل | sadc: دعم OOP كامل | `compiler_new/src/codegen/` | أصناف + وراثة → LLVM IR |
| أبريل | sadc: pattern matching | `compiler_new/src/codegen/` | طابق/عندما → LLVM IR |
| مايو | sadc: closures + lambdas | `compiler_new/src/codegen/` | لامدا → LLVM function ptr |
| مايو | WASM target تحسين | `compiler_new/src/backend/wasm_*` | Array + Index + Import |

**معايير القبول Q1 سعيد:**
- [ ] sadc يبني في Debug بدون أخطاء
- [ ] CI يمر على 3 منصات
- [ ] 10+ برامج .ص تترجم إلى executables عبر sadc
- [ ] أول GitHub Release منشور

---

### كامل 🌍 — Q1: تثبيت المكتبات الأساسية

| الشهر | المهمة | الملفات | المُخرج |
|-------|--------|---------|---------|
| مارس | stdlib/core: توحيد API | `stdlib/core/*.h` | API عربي موحد لـ 17 ملف |
| مارس | stdlib/io: stdin/stdout كامل | `stdlib/io/*.h` | `اقرأ()`, `اقرأ_سطر()`, `اكتب_ملف()` |
| أبريل | stdlib/json: parser كامل | `stdlib/json/*.h` | parse + stringify + pretty print |
| أبريل | stdlib/filesystem: عمليات ملفات | `stdlib/filesystem/*.h` | مسارات + مجلدات + قراءة/كتابة |
| مايو | stdlib/xml: parser + builder | `stdlib/xml/*.h` | DOM-style API |
| مايو | stdlib/database: SQLite | `stdlib/database/*.h` | اتصال + استعلام + prepared statements |

**معايير القبول Q1 كامل:**
- [ ] 6 وحدات stdlib مستقرة مع API موثق
- [ ] 5+ اختبار لكل وحدة
- [ ] لا تبعيات خارجية إجبارية (كلها optional)

---

### ريم 📚 — Q1: التوثيق + النشر الأولي

| الشهر | المهمة | الملفات | المُخرج |
|-------|--------|---------|---------|
| مارس | LICENSE + CODE_OF_CONDUCT | جذر المشروع | MIT License + Contributor Covenant |
| مارس | CHANGELOG.md | جذر المشروع | سجل كل الإصدارات السابقة |
| مارس | README.md تحديث شامل | `README.md` | badges + روابط صحيحة + أمثلة |
| أبريل | كتاب "أساسيات ص" — فصول 1-5 | `docs/book/` | المتغيرات، الدوال، الشروط، الحلقات، المصفوفات |
| أبريل | الموقع: محتوى + playground | `website/docs/` | 10+ صفحة محتوى + playground عامل |
| مايو | كتاب "أساسيات ص" — فصول 6-10 | `docs/book/` | OOP، أخطاء، ملفات، مكتبات، أمثلة |
| مايو | API docs لـ 6 وحدات | `docs/api/` | core, io, math, string, json, filesystem |

**معايير القبول Q1 ريم:**
- [x] ملفات قانونية كاملة (LICENSE, CODE_OF_CONDUCT, SECURITY.md)
- [x] 10 فصول من كتاب "أساسيات ص"
- [x] 6 وثائق API
- [x] الموقع يعمل محلياً مع playground

---

### عمر 🛠️ — Q1: أدوات + توزيع

| الشهر | المهمة | الملفات | المُخرج |
|-------|--------|---------|---------|
| مارس | LSP: حذف legacy + تحسين | `tools/lsp/` | حذف .legacy files + إضافة hover docs |
| مارس | VS Code: نشر في Marketplace | `tools/vscode-extension/` | Extension منشورة + 10+ downloads |
| أبريل | Formatter: دعم كامل | `tools/formatter/` | indentation + spacing + line length |
| أبريل | REPL: تحسين | `tools/repl/` | history + autocomplete + multiline |
| مايو | Distribution: أول Windows installer | `distribution/windows/` | .exe installer مع PATH |
| مايو | Tests: إطار اختبار .ص | `tests/` | `اختبر "وصف" ... نهاية` يعمل |

**معايير القبول Q1 عمر:**
- [ ] VS Code Extension منشورة
- [ ] Formatter يتعامل مع 90% من الحالات
- [ ] Windows installer عامل
- [ ] 10+ اختبار .ص بالإطار الجديد

---

## الربع الثاني (Q2): يونيو — أغسطس 2026
### العنوان: "التوسع والأداء"

```
╔══════════════════════════════════════════════════════════════════════╗
║  الهدف: v0.8-beta + أداء محسّن + منصات متعددة + مجتمع أولي      ║
║  المُخرج: beta release + 500+ مستخدم + playground عام             ║
╚══════════════════════════════════════════════════════════════════════╝
```

### أحمد 🔧 — Q2: ميزات متقدمة للمفسر

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| يونيو | Operator Overloading | `عامل +(آخر)` في الأصناف |
| يونيو | Properties (get/set) | `خاصية اسم احصل ... عيّن ... نهاية` |
| يوليو | Error types مخصصة | `ارمي خطأ_مخصص("رسالة")` + `امسك(خ: نوع)` |
| يوليو | Modules system | `استورد "وحدة" من "مسار"` يعمل |
| أغسطس | تحسين أداء المفسر | Cache لـ variable lookup + constant folding |
| أغسطس | Iterator protocol | `لكل` يعمل مع أي كائن يُنفّذ `سمة_مكرر` |

**معايير القبول Q2 أحمد:**
- [ ] Operator overloading مع 7 عوامل أساسية
- [ ] Module system مع resolve paths
- [ ] أداء المفسر +30% (benchmark)
- [ ] كل ميزة مع 5+ اختبار

---

### سعيد ⚙️ — Q2: sadc ينتج برامج حقيقية

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| يونيو | sadc: Modules + imports | `استورد` → LLVM module linking |
| يونيو | sadc: Error handling | `حاول/امسك/ارمي` → LLVM exception handling |
| يوليو | sadc: Generics codegen | Templates → LLVM monomorphization |
| يوليو | sadc: Optimizer (SIR level) | 5+ optimization passes |
| أغسطس | sadc: ARM64 target | Cross-compile لـ ARM64 |
| أغسطس | sadc: test suite | 50+ برنامج .ص → .exe وجميعها تمر |

**معايير القبول Q2 سعيد:**
- [ ] 50 برنامج .ص يترجم ويعمل
- [ ] ARM64 output يعمل على Raspberry Pi / M1
- [ ] 5 optimization passes مع benchmarks
- [ ] Automated release pipeline

---

### كامل 🌍 — Q2: مكتبات متقدمة + منصات

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| يونيو | stdlib/crypto: SHA256 + AES | تشفير/فك تشفير أساسي |
| يونيو | stdlib/network: TCP/UDP sockets | خادم/عميل TCP |
| يوليو | stdlib/http: HTTP server | خادم HTTP بسيط + routing |
| يوليو | stdlib/async: event loop | Event loop + I/O غير متزامن |
| أغسطس | WASM: sad.wasm محسّن | حجم < 1MB + أداء محسّن |
| أغسطس | Android: أول APK عامل | تطبيق "مرحبا" على Android |

**معايير القبول Q2 كامل:**
- [ ] HTTP server يخدم 100 req/sec
- [ ] WASM يعمل في المتصفح
- [ ] APK يُثبّت على Android حقيقي
- [ ] 10+ اختبار لكل وحدة

---

### ريم 📚 — Q2: كتاب + موقع + مجتمع

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| يونيو | كتاب — فصول 11-14 | OOP متقدم، async، modules، generics |
| يونيو | الموقع: نشر على domain | sad-lang.org أو sadlang.dev |
| يوليو | Playground عام | تشغيل كود ص في المتصفح |
| يوليو | 10 أمثلة تطبيقات كاملة | `examples/apps/` — حاسبة، todo، chat |
| أغسطس | كتاب — فصول 15-16 | مشاريع عملية كاملة |
| أغسطس | فيديو "Killer Demo" | 3 دقائق: من .ص إلى تطبيق عامل |

**معايير القبول Q2 ريم:**
- [x] كتاب 16 فصل مكتمل (PDF + HTML)
- [ ] الموقع عام مع playground عامل
- [x] 10 أمثلة تطبيقات في `examples/apps/`
- [ ] فيديو demo منشور

---

### عمر 🛠️ — Q2: أدوات ناضجة + registry

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| يونيو | LSP: completion + diagnostics | إكمال ذكي + أخطاء فورية |
| يونيو | Formatter: configurable | `sad-fmt.toml` للتخصيص |
| يوليو | Package registry (v1) | `sad-pkg install حزمة` يعمل |
| يوليو | 10 حزم رسمية أولية | math, collections, json, crypto, http... |
| أغسطس | Linter أساسي | `sad-lint` مع 10+ قواعد |
| أغسطس | Linux/macOS installers | `.deb` + `.rpm` + homebrew formula |

**معايير القبول Q2 عمر:**
- [ ] LSP completion يقترح بشكل صحيح  
- [ ] Registry يخدم 10+ حزم
- [ ] Linter مع 10 قواعد
- [ ] Installers لـ 3 منصات

---

## الربع الثالث (Q3): سبتمبر — نوفمبر 2026
### العنوان: "الصقل والاختبار"

```
╔══════════════════════════════════════════════════════════════════════╗
║  الهدف: v0.9-RC1 + اختبارات شاملة + polish + community growth    ║
║  المُخرج: Release Candidate + 2000+ اختبار + 50+ حزمة             ║
╚══════════════════════════════════════════════════════════════════════╝
```

### أحمد 🔧 — Q3: استقرار + أداء نهائي

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| سبتمبر | Concurrency primitives | `mutex`, `channel`, `atomic` يعملون |
| سبتمبر | Memory management تحسين | GC أو reference counting محسّن |
| أكتوبر | تحسين رسائل الأخطاء | "Teacher mode" مع suggestions دقيقة |
| أكتوبر | Debugger protocol (DAP) | breakpoints + step + inspect variables |
| نوفمبر | Fuzzing المفسر | 0 crashes بعد 1M test cases |
| نوفمبر | Benchmark suite | 10 benchmarks مقارنة مع Python/JS |

**معايير القبول Q3 أحمد:**
- [ ] 0 crashes من fuzzing (1M cases)
- [ ] أداء ≥ Python 3.12 في 8/10 benchmarks
- [ ] DAP يعمل مع VS Code
- [ ] رسائل خطأ واضحة بالعربي مع اقتراحات

---

### سعيد ⚙️ — Q3: sadc إنتاجي

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| سبتمبر | sadc: async codegen | async/await → LLVM coroutines |
| سبتمبر | sadc: stdlib linking | ربط المكتبة القياسية مع executables |
| أكتوبر | sadc: WASM output مباشر | `.ص → .wasm` بدون Emscripten |
| أكتوبر | sadc: debug info (DWARF) | source maps لـ debugging |
| نوفمبر | sadc: optimization level -O2 | أداء مقارب لـ C في microbenchmarks |
| نوفمبر | sadc: 100+ test programs | كل ميزة لغوية → executable عامل |

**معايير القبول Q3 سعيد:**
- [ ] 100+ برنامج يعمل عبر sadc
- [ ] WASM output مباشر
- [ ] Debug symbols تعمل مع LLDB/GDB
- [ ] أداء executables مقارب لـ C (-O2)

---

### كامل 🌍 — Q3: Mobile + Desktop UI

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| سبتمبر | Android Transpiler تحسين | Kotlin Compose code generation كامل |
| سبتمبر | iOS Transpiler أساسي | Swift/SwiftUI output أولي |
| أكتوبر | Desktop UI: widgets متقدمة | 20+ widget عامل (input, list, grid, tabs) |
| أكتوبر | Desktop UI: layout engine | Flexbox-style layout بالعربية |
| نوفمبر | stdlib/image: معالجة صور كاملة | resize, crop, filters, save |
| نوفمبر | stdlib/audio3d: صوت أساسي | تشغيل wav/mp3 + 3D positioning |

**معايير القبول Q3 كامل:**
- [ ] Android Transpiler ينتج APK عامل مع UI
- [ ] iOS Transpiler ينتج مشروع Xcode عامل
- [ ] 20 widget يعمل على Desktop
- [ ] image + audio يعملان

---

### ريم 📚 — Q3: مجتمع + تعليم

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| سبتمبر | منهج جامعي (16 أسبوع) | `docs/curriculum/` — slides + assignments |
| سبتمبر | مقالات "لماذا لغة ص" (5 مقالات) | blog posts + Medium/Dev.to |
| أكتوبر | توثيق sadc (المترجم) | `docs/compiler/` — كيفية الترجمة |
| أكتوبر | توثيق كل stdlib (16 وحدة) | `docs/api/` — 16 ملف |
| نوفمبر | "Awesome Sad" repository | قائمة مشاريع + حزم + موارد |
| نوفمبر | ترجمة التوثيق إلى الإنجليزية | `docs/en/` — للمطورين غير العرب |

**معايير القبول Q3 ريم:**
- [x] منهج 16 أسبوع كامل
- [x] 5 مقالات منشورة
- [x] 16 وثيقة API
- [x] Awesome Sad مع 30+ entry

---

### عمر 🛠️ — Q3: جودة + أدوات متقدمة

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| سبتمبر | Test framework بلغة ص | `اختبر`, `تأكد`, `تأكد_يساوي` |
| سبتمبر | Coverage tool | تقرير تغطية أسطر الكود |
| أكتوبر | LSP: refactoring | rename + extract function |
| أكتوبر | Profiler أساسي | `sad --profile script.ص` → flame graph |
| نوفمبر | VS Code: debugger integration | launch.json + breakpoints + watch |
| نوفمبر | Package search + discovery | `sad-pkg search "http"` مع تصنيف |

**معايير القبول Q3 عمر:**
- [ ] Test framework بلغة ص يعمل
- [ ] LSP rename يعمل
- [ ] Profiler ينتج تقرير
- [ ] VS Code debugging يعمل مع DAP

---

## الربع الرابع (Q4): ديسمبر 2026 — فبراير 2027
### العنوان: "الإطلاق — v1.0"

```
╔══════════════════════════════════════════════════════════════════════╗
║  الهدف: v1.0.0 Stable Release                                     ║
║  المُخرج: لغة برمجة عربية كاملة قابلة للاستخدام الإنتاجي        ║
╚══════════════════════════════════════════════════════════════════════╝
```

### أحمد 🔧 — Q4: تثبيت + توافق عكسي

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| ديسمبر | API freeze | لا تغييرات كاسرة بعد هذه النقطة |
| ديسمبر | Backward compat tests | 100 برنامج .ص من v0.5-alpha يعملون |
| يناير | إصلاح كل الأخطاء المتبقية | 0 known P0/P1 bugs |
| يناير | Performance regression tests | لا تراجع في الأداء بين الإصدارات |
| فبراير | Release preparation | تثبيت نهائي + smoke tests |
| فبراير | Post-v1.0 roadmap | خطة 2027 |

---

### سعيد ⚙️ — Q4: sadc v1.0

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| ديسمبر | sadc: كل ميزات اللغة | 100% من syntax → executable |
| ديسمبر | sadc: cross-compilation matrix | x86_64 + ARM64 + WASM + bare metal |
| يناير | sadc: release builds | Stripped, optimized executables |
| يناير | sadc: Windows + Linux + macOS CI | Release artifacts لكل منصة |
| فبراير | sadc: documentation | `sadc --help` شامل + man page |
| فبراير | sadc: stability tests | 500+ program compilation test |

---

### كامل 🌍 — Q4: platform polish

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| ديسمبر | stdlib: polish كل الوحدات | لا deprecated APIs |
| ديسمبر | Android: 3 تطبيقات نموذجية | calculator, todo, chat |
| يناير | WASM: playground إنتاجي | < 500KB, < 1s load time |
| يناير | Desktop UI: مثبتة | 0 crashes, consistent API |
| فبراير | Integration tests | كل stdlib × كل platform |
| فبراير | Performance benchmarks | مقارنة مع Python, Lua, JavaScript |

---

### ريم 📚 — Q4: إطلاق المحتوى

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| ديسمبر | الموقع الرسمي v2 | Design محترف + SEO + analytics |
| ديسمبر | إعلان v1.0 (drafts) | بيان صحفي + مقالة + فيديو |
| يناير | شراكات جامعية (3) | اتفاقيات مع 3 جامعات عربية |
| يناير | مراجعة نهائية للتوثيق | proof-reading + consistency |
| فبراير | 🎉 إطلاق v1.0 | Hacker News + Reddit + Twitter + YouTube |
| فبراير | فيديو BeeOS demo | إقلاع نظام في QEMU |

---

### عمر 🛠️ — Q4: توزيع نهائي

| الشهر | المهمة | المُخرج |
|-------|--------|---------|
| ديسمبر | Installers: 4 منصات | Windows (MSI) + macOS (pkg) + deb + rpm |
| ديسمبر | Scoop + WinGet + Homebrew | مُدارة تقليدية |
| يناير | VS Code Extension v3.0 | كل الميزات + dark theme + RTL |
| يناير | Registry: 50+ حزم | فهرس البحث + تصنيفات |
| فبراير | Release automation | `git tag v1.0.0` → كل شيء تلقائي |
| فبراير | Security audit | OWASP review + dependency audit |

---

## ملخص المُخرجات السنوية

```
╔══════════════════════════════════════════════════════════════════════════╗
║                                                                          ║
║  مشروع لغة ص — مُخرجات 2026                                           ║
║                                                                          ║
║  ┌────────────────────────────────────────────────────────────────┐      ║
║  │ Q1 (مارس-مايو)           Q2 (يونيو-أغسطس)                    │      ║
║  │ v0.5-alpha               v0.8-beta                            │      ║
║  │ • 22 خطأ مُصلح ✅         • أداء +30%                        │      ║
║  │ • أول GitHub Release     • 50+ برنامج sadc                    │      ║
║  │ • VS Code في Marketplace • HTTP server                        │      ║
║  │ • 6 وحدات stdlib         • Registry v1                        │      ║
║  │ • 10 فصول كتاب           • Playground عام                     │      ║
║  │ • Generics + Async       • 10 تطبيقات نموذجية                 │      ║
║  │ • Windows installer      • 3 installers                       │      ║
║  ├────────────────────────────────────────────────────────────────┤      ║
║  │ Q3 (سبتمبر-نوفمبر)      Q4 (ديسمبر-فبراير)                  │      ║
║  │ v0.9-RC1                  v1.0.0 🎉                           │      ║
║  │ • 2000+ اختبار           • 0 known P0/P1 bugs                 │      ║
║  │ • 100+ sadc programs     • 500+ sadc programs                 │      ║
║  │ • Android APK            • 4 platform installers              │      ║
║  │ • iOS Xcode project      • 50+ packages                       │      ║
║  │ • 16 فصل كتاب            • شراكات جامعية                      │      ║
║  │ • DAP debugger           • v1.0 launch event                  │      ║
║  │ • Fuzzing: 0 crashes     • Security audit                     │      ║
║  │ • منهج جامعي              • BeeOS demo                        │      ║
║  └────────────────────────────────────────────────────────────────┘      ║
║                                                                          ║
╠══════════════════════════════════════════════════════════════════════════╣
║  بنهاية 2026:                                                           ║
║  • لغة برمجة عربية كاملة v1.0                                          ║
║  • مفسر مستقر + مترجم أصلي (LLVM)                                     ║
║  • 16+ وحدة stdlib موثقة ومُختبرة                                      ║
║  • أدوات: LSP + Formatter + Linter + Pkg + REPL + Debugger             ║
║  • 5 منصات: Windows + Linux + macOS + Web (WASM) + Android             ║
║  • 2000+ اختبار (100% pass)                                            ║
║  • 50+ حزمة مجتمعية                                                    ║
║  • كتاب + منهج جامعي + موقع + playground                              ║
║  • VS Code Extension v3                                                 ║
║  • مجتمع: 1000+ مستخدم + 3 جامعات + Discord نشط                       ║
╚══════════════════════════════════════════════════════════════════════════╝
```

---

## جدول التوازي — عرض شهري لكل الفريق

```
        مارس    أبريل    مايو    ──   يونيو    يوليو    أغسطس
أحمد    Types   Async    Traits  ──   OpOvld   Errors   Perf
سعيد    LLVM    OOP-CG   Lambda  ──   Module   Generic  ARM64
كامل    Core    JSON     DB      ──   Crypto   HTTP     WASM
ريم     Legal   Book1    API     ──   Book2    Site     Video
عمر     LSP     Fmt      Install ──   LSP++    Registry Linter

        سبتمبر  أكتوبر  نوفمبر  ──   ديسمبر  يناير    فبراير
أحمد    Concur  ErrMsg   Fuzz    ──   Freeze   Fix-all  v1.0
سعيد    Async   WASM-D   Tests   ──   100%     Release  v1.0
كامل    Android Desktop  Image   ──   Polish   Perf     v1.0
ريم     Curric  StdDoc   Awesome ──   Site-v2  Univ     v1.0 🎉
عمر     TestFW  Refactor Debug   ──   Install  Ext-v3   v1.0
```

### قاعدة التوازي:
- **لا يلمس أحد ملفات مكون آخر** إلا عبر PR + code review
- **كل مطور** لديه branch خاص: `ahmed/Q1-types`, `saeed/Q1-llvm`, ...
- **دمج أسبوعي** إلى `develop` (كل جمعة)
- **دمج ربع سنوي** إلى `main` (نهاية كل ربع)
- **اجتماع يومي** 15 دقيقة (standup)
- **Sprint review** كل أسبوعين

---

## مؤشرات الأداء (KPIs)

| المؤشر | Q1 | Q2 | Q3 | Q4 |
|--------|-----|-----|-----|-----|
| اختبارات ناجحة | 200+ | 500+ | 1500+ | 2000+ |
| برامج sadc | 10+ | 50+ | 100+ | 500+ |
| وحدات stdlib مستقرة | 6 | 12 | 16 | 16+ |
| حزم مجتمعية | - | 10+ | 30+ | 50+ |
| مستخدمون | 50+ | 200+ | 500+ | 1000+ |
| فصول كتاب | 10 | 16 | 16 | 16 |
| وثائق API | 6 | 12 | 16 | 16+ |
| P0/P1 bugs مفتوحة | 0 | 0 | 0 | 0 |
| CI pass rate | 95%+ | 98%+ | 99%+ | 100% |

---

## إدارة المخاطر

| الخطر | الاحتمال | الأثر | التخفيف |
|-------|---------|-------|---------|
| LLVM Debug لا يُحل | متوسط | 🔴 | بناء sadc في Release فقط (حل مؤقت موجود) |
| مطور يغادر | منخفض | 🔴 | توثيق كل شيء + pair programming |
| أداء المفسر بطيء | متوسط | 🟡 | JIT layer أو bytecode VM |
| منافس عربي يظهر | منخفض | 🟡 | التميز بالعمق وليس السعة |
| WASM حجمه كبير | عالي | 🟡 | tree shaking + lazy loading |
| المجتمع لا ينمو | متوسط | 🟡 | شراكات جامعية + محتوى تعليمي |

---

## ملحق تنفيذي (مارس 2026) — حالة مهام كامل Q1

> هذا الملحق يربط خطة Q1 بالواقع الحالي في الشجرة البرمجية.
> الهدف: تحويل البنود العامة إلى حالة تنفيذ دقيقة + خطوات قادمة قصيرة.

### 1) الحالة الفعلية لمهام كامل (Q1)

| مهمة Q1 (كامل) | الحالة الفعلية | الدليل من المستودع | فجوة التنفيذ |
|---|---|---|---|
| توحيد API لـ `stdlib/core` | 🟡 جزئي | `stdlib/core/` موجود ويحتوي وحدات رئيسية (`builtins.*`, `array_functions.*`, `type_functions.*`, `other_functions.*`) | يحتاج تعريف معيار API موحد ومراجعة اتساق الأسماء العربية/الإنجليزية |
| اكتمال stdin/stdout في `stdlib/io` | ✅ متقدم | `stdlib/io/io_functions.cpp`, `stdlib/io/io_functions.h` | لا توجد فجوة حرجة حالياً؛ يوصى بتوسيع اختبارات تكامل `.ص` |
| JSON parser كامل | ✅ متقدم | `stdlib/json/json_module.cpp`, `stdlib/json/json_module.h`, `stdlib/json/json_builtins.cpp` + `interpreter_new/src/builtins/builtin_registry_part31.cpp` + اختبارات `tests/regression/test_p24_json_unified_builtin.ص`, `tests/regression/test_p26_json_invalid_input.ص` | تم توحيد نقطة الدخول في interpreter؛ القيد الحالي: يتطلب `استورد محرك` لتفعيل تسجيل part31 |
| Filesystem operations | ✅ متقدم | `stdlib/filesystem/filesystem_module.cpp`, `stdlib/filesystem/filesystem_builtins.cpp` + `tests/stdlib/test_filesystem_module.cpp` + `tests/regression/test_p28_filesystem_smoke.ص`, `tests/regression/test_p29_filesystem_invalid_input.ص` | تحسن كبير في تغطية smoke/negative؛ بقيت سيناريوهات المنصات/الصلاحيات المتقدمة |
| XML parser + builder | ✅ متقدم | `stdlib/xml/xml_module.cpp`, `stdlib/xml/xml_module.h`, `stdlib/xml/xml_builtins.cpp` + `interpreter_new/src/builtins/builtin_registry_part31.cpp` + اختبارات `tests/regression/test_p25_xml_unified_builtin.ص`, `tests/regression/test_p27_xml_invalid_input.ص` | تم توحيد نقطة الدخول في interpreter؛ القيد الحالي: يتطلب `استورد محرك` لتفعيل تسجيل part31 |
| SQLite في `stdlib/database` | 🟡 متوسط | `stdlib/database/database_module.cpp`, `stdlib/database/database_builtins.cpp` + `tests/regression/test_p30_database_availability.ص` | أضيف اختبار عقد التوفر/الاستيراد؛ ما زال الربط التشغيلي الكامل (`db_open` افتراضياً) يحتاج تفعيل وحدة database في interpreter |

### 2) ملاحظات تنفيذية مهمة

- كثير من وحدات Q1 موجودة ككود فعلي واختبارات وحدة C++، لكن مستوى "التفعيل للمستخدم النهائي داخل interpreter" غير موحد بين الوحدات.
- تم توحيد JSON/XML فعلياً داخل `builtin_registry_part31.cpp` مع أسماء عربية/إنجليزية موحدة، وإضافة توثيق مطابق في `docs/api/json.md` و`docs/api/xml.md`.
- أضيفت اختبارات انحدار إيجابية وسلبية تغطي: التحليل/التحويل، بالإضافة إلى إدخال غير صالح (negative tests) لـ JSON/XML.
- فجوة Q1 المتبقية هنا تشغيلية: جعل التسجيل متاحاً افتراضياً بدون شرط `استورد محرك`.

### 3) Backlog قصير (تنفيذي) بعد هذا التحديث

1. إزالة قيد `استورد محرك` لدوال JSON/XML عبر نقل/تكرار التسجيل لمسار عام دائم التحميل.
2. استكمال الربط التشغيلي الكامل لوحدة `database` داخل interpreter (حالياً اختبار العقد موجود لكن autoload غير مفعل).
3. إغلاق فجوة `stdlib/core` عبر ملف معيار API مختصر (Naming + Stability + Alias policy).
4. إضافة تحقق CTest مركّز لمصفوفة negative tests (JSON/XML + filesystem/database لاحقاً).

### 4) قرار تخطيطي

- تُعتبر مهام كامل في Q1 بحالة: **شبه مكتملة عملياً مع فجوة تشغيلية محددة (autoload registration)**.
- معيار الإغلاق المقترح لكل بند: "اختبار `.ص` يمر + توثيق API مطابق + builtin registration موحد".

---

*الخطة السنوية — v1.0*
*تاريخ الإعداد: 6 مارس 2026*
*التحديث القادم: نهاية Sprint 2 (أبريل 2026)*
