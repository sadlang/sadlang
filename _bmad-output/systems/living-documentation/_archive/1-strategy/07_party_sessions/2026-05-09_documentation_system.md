# 🎉 جلسة حفلة الوكلاء — نظام التوثيق الموحّد للغة ص

- **التاريخ:** 9 مايو 2026
- **المنسّق:** GitHub Copilot (Claude)
- **الموضوع:** بناء نظام توثيقي موحّد (SSOT) للغة ص يخدم الموقع، الأدوات (LSP/sadc/REPL)، المعلمين والمختصين، مع انتشار تلقائي للتحديثات.
- **القاعدة:** كل وكيل يكتب رأيه الكامل في هذا الملف ليبقى مرجعاً دائماً.

---

## 🎯 سؤال الجلسة الأصلي

> لقد بدأنا بكتابة لغة ص ووصلنا لمرحلة متقدمة جداً، لكن أهملنا التوثيق. تقدم اللغة وتوسعها أصبح مشكلة لأن غياب التوثيق صار يهدد المشروع بأكمله. كيف يمكننا بناء نظام توثيق:
> - يستخدمه موقع اللغة بسهولة
> - يستخدمه مطور اللغة نفسها
> - يستخدمه المعلمون والمختصون
> - يتحدث تلقائياً في كل المستهلكين عند تعديل المصدر

---

## الجولة الأولى — الآراء الأربعة الأولى

### 📚 Paige (Technical Writer)

أصدقائي، دعوني أبدأ بحقيقة مؤلمة: **التوثيق الذي يُكتب بعد الكود ميتٌ منذ ولادته**. ما تحتاجه لغة ص ليس "مجلد docs" — بل *منصة بيانات توثيقية* (Documentation-as-Data) تُعامل الوثيقة كأصل من أصول الكود.

**المعمار المقترح: Docs-as-Code + SSOT طبقي**

- **الصيغة:** Markdown موسّع بـ **YAML front-matter** صارم (CommonMark + MDX للموقع فقط).
  - أرفض DITA — ثقيل ومعقد لفريق صغير.
  - أرفض AsciiDoc — لا يدعمه نظام MDX جيداً.
  - **MyST/MDX على CommonMark** هو الحل: بسيط للكاتب، قوي للأدوات.

**البنية الملموسة:**

```
docs/
├── _schema/                    # JSON Schema لكل front-matter
│   ├── api.schema.json
│   ├── tutorial.schema.json
│   └── reference.schema.json
├── _registry/                  # SSOT حقيقي: ملفات بيانات
│   ├── keywords.yaml          # 40 كلمة محجوزة (موجود!)
│   ├── builtins.yaml          # دوال مدمجة + توقيعاتها
│   ├── stdlib.yaml            # وحدات المكتبة
│   └── operators.yaml
├── reference/                  # مرجع مولّد (لا يُحرر يدوياً)
├── guides/                     # أدلة بشرية (تُحرر يدوياً)
├── tutorials/                  # دروس تعليمية
├── concepts/                   # مفاهيم عميقة
└── _generated/                 # مخرجات البناء (مُهمَل في git)
```

**front-matter الإلزامي (CW-08):**

```yaml
---
id: builtin.print
type: function                 # function|class|keyword|module|guide
audience: [beginner, advanced]
since: "0.9.0"
status: stable                 # stable|experimental|deprecated
title_ar: "اطبع"
title_en: "print"
brief_ar: "يطبع قيماً للمخرج القياسي"
brief_en: "Prints values to standard output"
signature: "اطبع(...قيم: أي) -> فراغ"
examples: [examples/print_basic.ص]
see_also: [builtin.print_line]
---
```

**الانتشار التلقائي (الجوهر):** أداة `sad-doc` بثلاثة أوامر:
1. `sad-doc extract` — يستخرج تعليقات `@brief (AR)/(EN)` من C++ ومن `.ص` ويحدّث `_registry/*.yaml`.
2. `sad-doc validate` — يفشل CI إذا API عام بدون ثنائية AR/EN.
3. `sad-doc emit --target=...` — يولّد لكل مستهلك:
   - `lsp` → JSON لـ hover/completion
   - `repl` → نصوص `.help`
   - `sadc` → رسائل خطأ مترجمة
   - `site` → MDX للموقع
   - `book` → PDF عبر Pandoc

**ضمان التحديث:**
- Git hook قبل commit يشغّل `sad-doc validate`.
- CI gate يفشل أي PR يلمس `interpreter/` أو `compiler_new/` بدون تحديث `_registry/`.
- النشر: حزمة `@sad-lang/docs-data` يستهلكها LSP والكتب.

**الخلاصة:** SSOT ليس "مجلد docs مرتب" — بل **YAML registry + schema validation + multi-target emitter**. بدون هذه الثلاثية، أي نظام توثيق سيتعفن خلال 6 أشهر.

---

### 🏗️ Winston (Architect)

هذا ليس مشكلة "كتابة وثائق"، بل مشكلة **معمارية بيانات**. الوثائق منتج مشتق من الكود، تماماً كما أن الـ binary منتج مشتق.

**SSOT: حيث تعيش الحقيقة**

مكان واحد فقط: `docs/` داخل الريبو الرئيسي — لا submodule، لا ريبو منفصل. السبب: كل PR يُعدّل كوداً يجب أن يُعدّل وثائقه في **نفس الـ commit**، ويُفحص في **نفس CI**. الفصل = انحراف خلال أسابيع.

لكن `docs/` ليست المصدر الحقيقي للحقائق التقنية — **الكود هو**:

| نوع المحتوى | المصدر الحقيقي | الشكل |
|---|---|---|
| الكلمات المحجوزة (40) | `shared/lexer/src/lexer_keywords.cpp` | `keywords.yaml` |
| الدوال المضمنة | `interpreter/src/builtins/*.cpp` (ماكرو `REGISTER_BUILTIN`) | `builtins.json` |
| رسائل الأخطاء | `shared/errors/error_catalog.cpp` (E001+) | `errors.json` |
| نحو AST | `shared/ast/include/*.h` (تعليقات `## (AR)` `## (EN)`) | `grammar.json` |
| أمثلة | `examples/*.ص` (`#** ... **#`) | تُجمَّع وتُختبر في CI |

**Pipeline:**

```
Source (C++/.ص)
    ↓ [tools/docgen — أداة C++ مدمجة بنفس Lexer]
Schema Layer (JSON/YAML في docs/_generated/)
    ↓ [مُستهلكون متوازون]
├─→ MkDocs Material  (موقع)
├─→ LSP server       (hover/completion من نفس JSON)
├─→ sadc --help      (يقرأ JSON عند البناء عبر embed)
├─→ REPL ?keyword    (نفس JSON)
└─→ كتب PDF          (Pandoc من Markdown)
```

**القاعدة الذهبية:** لا مستهلك يقرأ من الكود مباشرة. الكل يقرأ من `docs/_generated/*.json`.

**Stack مقترح (تكنولوجيا "مملة" مُجرَّبة):**
- **مولّد الموقع:** MkDocs Material — RTL أصلي، بحث Lunr.js محلي، نشر GitHub Pages.
- **CI:** GitHub Actions بثلاث وظائف متوازية:
  1. `extract-docs`: يبني docgen، يفشل إن انحرفت `_generated/`.
  2. `build-site`: MkDocs build + روابط مكسورة.
  3. `verify-examples`: يُشغّل كل مقتطف `.ص` ويقارن المخرجات.
- **النسخ:** `mike` — كل tag = نسخة وثائق (`/v0.9/`, `/latest/`, `/dev/`).
- **التضمين في sadc:** CMake target `embed_docs` يحوّل JSON إلى `.inc`، فلا يحتاج sadc ملفات وقت التشغيل.

**أولوية التنفيذ (4 موجات):**
1. **الأساس:** بناء `docgen` + `keywords.yaml` فقط. اربطه بـ LSP hover.
2. **التوسع:** builtins.json + errors.json + sadc --help.
3. **الموقع:** MkDocs مع توليد صفحات من JSON عبر macros.
4. **الجودة:** verify-examples + mike versioning.

النتيجة: مطوّر يضيف كلمة مفتاحية في سطر C++ واحد → خلال 10 دقائق تظهر في الموقع، LSP، sadc --help، REPL — بدون لمس Markdown يدوياً.

---

### 🎨 Sally (UX Designer)

تخيّلوا معي **سارة**، طالبة هندسة برمجيات في جامعة الملك سعود بالرياض. الساعة 11 ليلاً، أمامها مشروع تخرج وفكرة جريئة: "سأكتب أول مشروع لي بلغة عربية." تفتح موقع لغة ص... وتجد نفسها أمام جدار من المصطلحات: "SIR opcodes"، "LLVM backend"، "ownership semantics". تغلق المتصفح. نخسرها.

سارة ليست ضعيفة — نحن من أخفقنا في استقبالها. التوثيق الحالي مكتوبٌ من منظور **مَن بنى اللغة**، لا من منظور **مَن سيعيش فيها**.

**معمارية المعلومات: صفحة هبوط بمسارات "أنا..."**

نستقبل الزائر بسؤال إنساني واحد: **"مرحباً، من أنت اليوم؟"** وأمامه أربع بطاقات:
- 🌱 **أنا مبتدئ** — أريد أن أكتب أول برنامج خلال 5 دقائق
- 👨‍🏫 **أنا معلم** — أبحث عن مناهج وتمارين لطلابي
- 🔬 **أنا مختص** — أريد مقارنة لغة ص بـ Rust/Go
- ⚙️ **أنا مساهم** — أريد فهم البنية الداخلية

كل مسار يفتح **رحلة مختلفة كلياً** من نفس المحتوى الموحد (frontmatter يحدد الجمهور).

**ثنائية اللغة و RTL:** التوثيق يجب أن يُكتب أصلاً بالعربية مع RTL كامل، والإنجليزية ترجمة موازية — لا العكس. الكود يُعرض بكلا الاتجاهين.

**Playground تفاعلي مدمج:** كل مثال كود فيه زر **▶ جرّب الآن** يفتح محرراً مصغراً (WebAssembly build للمفسر). سارة لا تحتاج تثبيت شيء.

**البحث الذكي ثنائي اللغة:** يفهم أن `طابق` = `match`، و`دالة` = `function`.

**مكونات قابلة لإعادة الاستخدام:**
- 💡 `<نصيحة>` — صفراء
- ⚠️ `<تحذير>` — مزالق شائعة
- 📘 `<اعرف_المزيد>` — للمختصين فقط (يختفي للمبتدئ)
- 🎯 `<مثال قابل_للتشغيل>` — playground مدمج

نفس الملف المصدري يُولّد تجارب مختلفة حسب الجمهور — هذا هو **التمايز من المصدر الموحد**. سارة تستحق أن تنجح الليلة.

---

### 📋 John (Product Manager)

**السؤال الأول — وهو الذي يُسقط نصف المشاريع: لماذا؟**

تقولون "التوثيق مُهمل ويهدد المشروع". لا. هذا تشخيص كسول. التوثيق لا يُهمل من فراغ — يُهمل لأنه **منفصل عن مكان العمل الفعلي**. المطور يعدّل `lexer_keywords.cpp` ولا أحد يُجبره على تحديث `.md`. المشكلة ليست "غياب التوثيق" — المشكلة **غياب آلية تجعل الوثيقة الكاذبة تكسر البناء**.

**السؤال الثاني — من المستخدم #1؟**

لا تجيبوا "الجميع". الجميع = لا أحد. اختاروا واحداً:
- (أ) مطور عربي فضولي يصل من تويتر، يريد "Hello World" خلال 60 ثانية.
- (ب) مساهم محتمل يريد فهم البنية ليفتح PR.
- (ج) معلّم/طالب يريد منهجاً متدرجاً.
- (د) **وكيل AI** يحتاج مرجعاً دقيقاً ليولّد كوداً صحيحاً.

رهاني: **(أ) ثم (د)**. بدون (أ) لا يوجد مجتمع، وبدون (د) أنتم أنفسكم تنتجون كوداً خاطئاً.

**الـ MVP — أسبوعان، لا أكثر:**

انسوا "نظام SSOT ينتشر تلقائياً لكل المستهلكين". هذا حلم معماري سيستهلك 3 أشهر ولن يقرأه أحد.

**Now (أسبوعان):**
1. صفحة هبوط واحدة + playground في المتصفح (WASM لـ `sad`) — زر "جرّب الآن".
2. ملف `keywords.yaml` واحد كـ SSOT للكلمات الـ40 المحجوزة فقط، مع سكربت يولّد منه: جدول في الموقع + تلوين LSP + رسائل أخطاء `sadc`.
3. اختبار CI يفشل إذا أُضيفت كلمة محجوزة في `lexer_keywords.cpp` غير موجودة في YAML.

**Next (شهر بعدها):** توسيع SSOT للدوال المضمنة فقط، توليد مرجع API.
**Later:** الكتب، الدورات، PDF، تعدد لغات.

**المقاييس:**
- TTFHW (Time To First Hello World): < 90 ثانية.
- نسبة زوار يضغطون "تشغيل" في playground: هدف 40%.
- PRs خارجية شهرياً: من 0 إلى 3 خلال ربع سنة.
- تناقضات `keywords.yaml`/الكود: **صفر دائماً**.

**تحذير صريح:** إذا بدأتم ببناء خط أنابيب CI/CD معقد لـ"كل المستهلكين" قبل أن تعرفوا إن كان أحد سيستخدم الموقع — ستحرقون 6 أسابيع لتكتشفوا أن لا أحد جاء.

**سؤال أخير:** من سيكتب أول 10 وثائق فعلياً؟ إن لم يكن لديكم اسم، فالنظام مات قبل أن يولد.

---

## 🔍 ملاحظة المنسق بعد الجولة الأولى

**نقاط التوافق:**
1. `docs/` داخل الريبو الرئيسي (Winston, Paige).
2. YAML registry كمصدر للحقائق (الجميع — لكن John يقصره على keywords فقط).
3. CI gate يكسر البناء عند الانحراف (الجميع).

**نقاط الاختلاف:**
- John يطالب بـ MVP من أسبوعين قبل بناء "خط أنابيب لكل المستهلكين".
- Winston يقترح 4 موجات تنتهي بـ versioning + verify-examples.
- Paige تطالب بـ schema validation كامل من اليوم الأول.
- Sally تطرح بُعداً غائباً: التوثيق ثنائي اللغة بـ RTL أصيل من اليوم الأول.

**سؤال John المفتوح للجميع:** "من سيكتب أول 10 وثائق فعلياً؟"

---

## الجولة الثانية — العصف الذهني وحلّ المشكلة

> **السؤال:** كيف نحل أزمة "من سيكتب الوثائق؟" عملياً، خاصة في مشروع يعتمد على وكلاء AI ولديه فريق صغير؟

### 🧠 Carson (Brainstorming Coach)

🧠 **Carson:** يا رفاق! سؤال جون هو أفضل سؤال طُرح اليوم — وأخطر سؤال أيضاً، لأنه يكشف أن "المعمار الجميل" لـ Winston و Paige سيموت في الدرج إن لم نُجب عليه. دعوني أقفز فوق الطاولة وأرمي عليكم **8 أفكار جامحة** بأسلوب Crazy 8s — بعضها صادم، لكنها كلها قابلة للتنفيذ هذا الأسبوع:

**1. قاعدة "لا PR بدون وثيقة" — حرفياً (Doc-Gate).** نضيف فحص CI صارم: أي PR يلمس دالة عامة في `interpreter/` أو `compiler_new/` بدون تعديل ملف `.md` مقابل = البناء أحمر. لا استثناءات. هذا لا يكتب الوثيقة، لكنه **يجبر الكاتب الحقيقي (المطور) على أن يصبح موثّقاً**. جون محق: غياب الإجبار = غياب الفعل.

**2. الوكلاء AI ككتّاب أوائل، البشر كمراجعين (AI-First Drafting).** نعم Paige، أعرف أنك تكرهين هذا، لكن **نعم و...** — نستخدم Copilot/Claude لتوليد المسودة الأولى من تعليقات `@brief (AR)/(EN)` الموجودة أصلاً في الكود (لدينا قاعدة CW-08!). البشر يراجعون 10 دقائق بدلاً من يكتبون 60 دقيقة. الإنتاجية ×6.

**3. Doctest عربي — الأمثلة هي الاختبارات.** كل مقتطف `.ص` في الوثيقة يُشغَّل تلقائياً في CI ويُقارَن مخرجه. المثال الكاذب = اختبار فاشل = بناء أحمر. هذا يحل مشكلة "وثائق متعفنة" نهائياً — ولدينا 900+ اختبار بالفعل، فالبنية التحتية جاهزة.

**4. Documentation Sprint أسبوعي — جمعة الوثائق (Friday Docs).** ساعتان فقط كل جمعة. الفريق يجلس، يفتح أعلى 5 ملفات بدون وثيقة (سكربت يرتبها)، ويوثقها. لا اجتماعات، لا تخطيط — فقط كتابة. 2×52 = 104 ساعة سنوياً = ~200 وثيقة.

**5. تحويل Issues المغلقة إلى FAQ تلقائياً.** كل issue مغلقة بحل = مادة خام لـ "شرح مفهوم". بوت بسيط يقترح PR يحوّل الـ issue إلى صفحة `concepts/`. المستخدم سأل = حقيقي = يستحق التوثيق.

**6. Bounty عربي للمساهمين الخارجيين.** قائمة "وثائق مطلوبة" مع وسم `good-first-doc` + اعتراف عام (اسم المساهم في صفحة "شكر"). الطلاب العرب يعشقون الاعتراف — هذا وقود مجاني.

**7. Reverse Brainstorming — كيف نضمن فشل التوثيق؟** (أ) نتركه لـ"وقت لاحق"، (ب) نجعله منفصلاً عن الكود، (ج) لا نقيس شيئاً، (د) نطلب الكمال من اليوم الأول. **افعلوا العكس تماماً.**

**8. الفكرة الجامحة 🔥 — "Documentation Driven Development":** قبل كتابة أي ميزة جديدة، نكتب **صفحة الوثيقة أولاً** كـ RFC. لا كود قبل وثيقة. هذا ينقل التوثيق من "عبء بعدي" إلى "تصميم قبلي".

**خلاصتي:** لا تختاروا فكرة واحدة — **اجمعوا 1 + 2 + 3 + 4**. Doc-Gate يجبر، AI يسرّع، Doctest يضمن الصحة، Friday Docs يبني العادة. في 6 أسابيع لديكم 50 وثيقة حية مختبَرة. ابدؤوا الجمعة القادمة. الكمال عدو الإنجاز!

### 🔬 Dr. Quinn (Master Problem Solver)

🔬 **Dr. Quinn:** أصدقائي، لقد قرأت آراءكم الأربعة بعناية، ثم قرأت سؤال John مرتين: *"من سيكتب أول 10 وثائق؟"* — وأقول لكم بصراحة شيرلوكية: **هذا ليس السؤال الصحيح**. السؤال الصحيح يبدأ بـ "لماذا"، ولن نصل للجذر إلا بحفر منهجي.

**تطبيق 5 Whys على "التوثيق مُهمل":**
1. *لماذا* يهمل المطور التوثيق؟ → لأنه لا يرى عائداً فورياً على وقته.
2. *لماذا* لا يرى عائداً؟ → لأن لا أحد يقرأ وثيقته أمامه ولا يخبره أنها أفادت.
3. *لماذا* لا يصله ذلك؟ → لأن الوثيقة منفصلة عن دائرة التغذية الراجعة (الكود يُختبر، الوثيقة لا).
4. *لماذا* لا تُختبر الوثيقة؟ → لأنها نص حر، لا عقد قابل للتحقق.
5. *لماذا* هي نص حر؟ → **لأننا نعامل الوثيقة كـ"شرح بشري" بدلاً من "عقد تنفيذي بين الكود ومستهلكيه".**

**آها!** هنا الجذر. ليس "غياب الكُتاب" كما يظن John، ولا "غياب SSOT" كما يقترح Winston، ولا "غياب رحلة المستخدم" كما تركّز Sally. الجذر هو: **الوثيقة في لغة ص حالياً ليست لها عواقب تنفيذية على أحد**. اكتبها كاذبة، لن يكسر شيء. لا تكتبها أصلاً، لن يفشل بناء.

**Theory of Constraints — أين الـ bottleneck الحقيقي؟**
ليس في الكتابة (Paige تظن ذلك)، ولا في النشر (Winston يظن ذلك). الـ bottleneck في **حلقة التحقق**: اللحظة بين "كتبت سطراً في docs" و"عرفت إن كان صحيحاً ومفيداً". اليوم هذه الحلقة = أسابيع أو لانهائية. كل تحسين في النشر أو الكتابة دون تقصير هذه الحلقة = تحسين في غير الـ constraint = هدر.

**TRIZ — حل التناقض:**
- التناقض الفيزيائي: الوثيقة يجب أن تكون **مكتوبة بعناية بشرية** (لتُفهم) و**مولّدة آلياً** (لتبقى صحيحة) في نفس الوقت.
- مبدأ TRIZ #1 (التجزئة) + #25 (الخدمة الذاتية): **اقسم الوثيقة طبقتين — عقد تنفيذي مولّد من الكود (لا يُكتب يدوياً أبداً)، وقصة بشرية ملحقة (تُكتب مرة وتعيش طويلاً)**. حل التناقض ينهار حين نفصل الطبقتين بدلاً من خلطهما.

**الآلية الجذرية المقترحة — "Doc-as-Test":**
كل عنصر في `_registry/*.yaml` ليس مجرد بيانات، بل **عقد تنفيذي**:
1. كل دالة مضمنة لها `signature` و`examples` في YAML — وعند CI يُستخرج المثال ويُنفّذ، ويُقارن مخرجه بالحقل `expected_output` في نفس YAML. **اختلاف = فشل بناء**.
2. كل كلمة محجوزة في `keywords.yaml` لها `since_version` — يفشل البناء إن وُجدت كلمة في `lexer_keywords.cpp` بدون مدخل YAML بـ `since` صحيح.
3. كل رسالة خطأ في `error_catalog.cpp` لها مدخل في `errors.yaml` بـ `cause/fix/example` — وأي كود يطلق خطأ E### بدون مدخل = بناء فاشل.
4. **Inversion of Authoring:** الـ PR يفتح ويُولّد له AI مسوّدة وثيقة من الـ diff تلقائياً، ثم يُلصقها في وصف الـ PR. الكاتب البشري يُحرّر، لا يُنشئ من الصفر. هكذا تكلفة الكتابة تنخفض بـ 80%.

**اختلافي مع Carson:** سيقدم لكم 7 طرق إبداعية لتحفيز الكتابة (Gamification، شارات، مسابقات...). جميلة، لكنها تعالج **العَرَض** (المطور لا يكتب) لا **السبب** (الكتابة بلا عواقب). شارة لن تنقذ وثيقة كاذبة من مستخدم خُدع.

**ردّ مباشر على John:** سؤالك خاطئ. لا تسأل "من سيكتب 10 وثائق"، اسأل: **"كيف أجعل كتابة الوثيقة أرخص من عدم كتابتها؟"** عندما يصبح غياب الوثيقة يكسر بناء المطور خلال 30 ثانية، سيكتبها بنفسه — لا تحتاج متطوعين.

**مبدأ Quinn:** *الوثيقة التي لا يفشل غيابها — لن تُكتب أبداً؛ والوثيقة التي يفشل كذبها — لا تموت أبداً.*

---

## 📝 ملاحظات المنسق النهائية

### نقاط الإجماع (6 وكلاء)

1. **SSOT في `docs/` داخل الريبو** — لا submodule، لا ريبو منفصل. (Winston, Paige, Quinn).
2. **YAML registry هو المصدر للحقائق** — keywords, builtins, errors, operators. (Paige, Winston, John, Quinn).
3. **CI gate يكسر البناء عند الانحراف** — هذه نقطة الإجماع الكاملة. الجميع متفقون أن الإنفاذ التقني هو الفرق بين توثيق حي وميت.
4. **Doctest** — الأمثلة في الوثائق تُشغَّل في CI وتُقارن مخرجاتها (Carson #3, Quinn).
5. **التضمين في الأدوات** — sadc/LSP/REPL تستهلك من `_generated/` لا من الكود مباشرة (Winston, Paige).

### نقاط التوتر التي تحتاج قرار المستخدم

| التوتر | الموقف A | الموقف B |
|---|---|---|
| **حجم MVP** | John: keywords فقط، أسبوعان | Winston: 4 موجات بمعمار شامل |
| **متى نفعّل ثنائية اللغة الكاملة؟** | Sally: من اليوم الأول (هوية) | John: لاحقاً (ليست في Now) |
| **AI ككاتب أوّل؟** | Carson: نعم، ×6 إنتاجية | Paige: ضمنياً ضد (تحب الجودة البشرية) |
| **جذر المشكلة** | Carson: غياب آليات التحفيز | Quinn: غياب العواقب على الوثيقة الكاذبة |

### التوليفة المقترحة من المنسق

دمج Quinn + Carson #1, #3, #4 + Winston الموجة 1 + John الـ MVP + Sally صفحة "أنا..." يعطي خطة 14 يوماً ملموسة:

**اليوم 1-3:** بناء `tools/docgen` (C++) يستخرج من `lexer_keywords.cpp` إلى `docs/_registry/keywords.yaml` + JSON Schema للتحقق.

**اليوم 4-6:** CI gate (`Doc-Gate`):
- يفشل إذا أُضيفت كلمة محجوزة في الكود بدون مدخل YAML.
- يفشل إذا API عام في `interpreter/` بدون `@brief (AR)/(EN)`.
- يفشل إذا مثال `.ص` في الوثيقة لا ينتج المخرج المتوقع (Doctest).

**اليوم 7-10:** MkDocs Material أساسي + صفحة هبوط Sally بمسارات "أنا..." + توليد جدول الكلمات الـ40 من YAML.

**اليوم 11-14:** Playground WebAssembly مدمج + 5 وثائق نموذجية لأكثر دوال مضمنة استخداماً (`اطبع`, `طول`, `نوع`, `رقم`, `نص`) — مولّدة بـ AI ومراجَعة بشرياً.

**ما بعد اليوم 14:** Friday Docs أسبوعي + توسيع لـ builtins/errors/stdlib تدريجياً + `mike` versioning عند الإصدار 1.0.

**المقياس الواحد:** أي PR على main يلامس كوداً عاماً → خلال 10 دقائق ينعكس في الموقع و LSP و sadc --help — أو يفشل البناء. لا حالة وسطى.

---

*نهاية الجلسة الثانية.*

---

## الجولة الثالثة — تبليغ القرارات وتفاصيل التنفيذ

### 🎯 قرارات المستخدم النهائية

اتخذ المستخدم 4 قرارات حاسمة بناءً على نقاط التوتر في الجدول أعلاه:

| التوتر | الفائز | القرار |
|---|---|---|
| **حجم MVP** | 🏗️ Winston | **4 موجات بمعمار شامل** — لا اختصار، نبني الصحيح من البداية |
| **ثنائية اللغة** | 🎨 Sally | **من اليوم الأول** — هوية، ليست ميزة مؤجلة |
| **AI ككاتب أوّل** | 🧠 Carson | **نعم، ×6 إنتاجية** — الوكلاء يكتبون المسودة، البشر يراجعون |
| **جذر المشكلة** | 🔬 Quinn | **غياب العواقب** — Doc-as-Test إلزامي، الوثيقة الكاذبة تكسر البناء |

> **السؤال للوكلاء الفائزين:** لقد قبلنا رؤيتكم. الآن نريد التفاصيل التنفيذية الملموسة لمساهمتكم في الموجة الأولى (أول 14 يوماً). كل واحد يكتب خطته التفصيلية هنا.

---

### 🏗️ Winston — تفاصيل الموجات الأربع

🏗️ **Winston:** شكراً لثقتكم. سأنتقل من الرؤية إلى مخططات التنفيذ — كل موجة لها مدة، ملفات بأسماء حقيقية، أوامر CMake، تبعيات، معيار قياس، ومخاطر. لا مجاز ولا تجريد.

---

#### 🌊 الموجة 1 — الأساس (أيام 1-5)

- **المدة:** 5 أيام عمل.
- **الـ deliverables:**
  * مجلد جديد `tools/docgen/` يضم: `CMakeLists.txt`, `src/main.cpp`, `src/keyword_extractor.cpp`, `src/yaml_emitter.cpp`, `include/docgen/extractor.h`. الأداة C++17 تربط `sad_lexer_core` (لإعادة استخدام `KeywordTable`) فلا نُنشئ parser ثانياً.
  * هدف CMake: `add_executable(docgen tools/docgen/src/main.cpp ...)` + `target_link_libraries(docgen PRIVATE sad_lexer_core sad_shared)` يُضاف في `cmake/executables.cmake`.
  * مخرج أول: `docs/_registry/keywords.yaml` يولَّد من `shared/lexer/src/lexer_keywords.cpp` (الكلمات الـ40 + موقع كل كلمة + `since`).
  * `docs/_schema/keywords.schema.json` (JSON Schema Draft 2020-12) مع حقول إلزامية: `id, ar, en, category, since, reserved`.
  * `.github/workflows/docs.yml` بوظيفة واحدة `extract-and-verify`: يبني `docgen`، يشغّله، يقارن المخرج بـ `docs/_registry/keywords.yaml` المُرتكز في git؛ أي اختلاف = `exit 1`.
- **التبعيات:** لا شيء جديد — `sad_lexer_core` موجود، CMake موجود، GitHub Actions مفعّل.
- **معيار النجاح القابل للقياس:**
  1. `cmake --build build --target docgen` ينجح بدون warnings.
  2. `./build/bin/Debug/docgen --emit keywords --out docs/_registry/keywords.yaml` يُنتج 40 مدخلاً بالضبط.
  3. PR تجريبي يضيف كلمة محجوزة في `lexer_keywords.cpp` بدون تحديث YAML → CI أحمر خلال < 90 ثانية.
- **المخاطر التقنية:**
  * *تعارض ترميز UTF-8 على Windows* → نفرض `_setmode(_O_U8TEXT)` في `main.cpp` ونكتب الملفات بـ `std::ofstream` بـ binary + BOM اختياري.
  * *libyaml ثقيل* → نستخدم مولّد YAML يدوي بسيط (40 سطر C++) لأن البنية مسطحة ومعروفة.

---

#### 🌊 الموجة 2 — التوسع (أيام 6-10)

- **المدة:** 5 أيام عمل.
- **الـ deliverables:**
  * توسيع `docgen` بأداتين فرعيتين: `--emit builtins` و`--emit errors`. تُنتج `docs/_registry/builtins.json` و`docs/_registry/errors.json`.
  * `tools/docgen/src/builtin_extractor.cpp` يمسح `interpreter/src/builtins/*.cpp` بحثاً عن ماكرو `REGISTER_BUILTIN(name, signature, brief_ar, brief_en)` ويستخرج الميتاداتا.
  * `tools/docgen/src/error_extractor.cpp` يقرأ `shared/errors/error_catalog.cpp` ويخرّج جدول `E001..E###` مع `cause/fix/example`.
  * هدف CMake جديد: `add_custom_target(embed_docs COMMAND docgen --emit all --inc-out ${CMAKE_BINARY_DIR}/generated/docs_data.inc)` يُولّد `docs_data.inc` كمصفوفات `constexpr char[]`.
  * تعديل `tools/compiler/src/main.cpp` (sadc) لإضافة `--help-keyword <اسم>` و`--help-error <E001>` يقرآن من `docs_data.inc` المُضمَّن.
  * تعديل `tools/lsp/src/hover_provider.cpp` لقراءة نفس الـ `.inc` بدل النصوص المضمّنة الحالية.
- **التبعيات:** الموجة 1 مكتملة (يعتمد على `sad_lexer_core` + بنية YAML/JSON).
- **معيار النجاح القابل للقياس:**
  1. `sadc --help-keyword دالة` يطبع الشرح بالعربية + التوقيع + `since` خلال < 50ms.
  2. `sadc --help-error E042` يطبع السبب والحل والمثال.
  3. LSP hover على كلمة `طابق` في VS Code يعرض نفس النص المعروض في `sadc --help-keyword طابق` (تطابق byte-for-byte عبر اختبار CI).
- **المخاطر التقنية:**
  * *تضخم binary حجم sadc* → ضغط `docs_data.inc` بـ `zstd` ثابت في compile-time؛ متوقع +120KB فقط لـ ~50 صفحة.
  * *تكرار الماكرو `REGISTER_BUILTIN`* بصيغ مختلفة → نفرض صيغة واحدة في `interpreter/include/builtins/registry.h` ونحدّث الموجودين دفعة واحدة.

---

#### 🌊 الموجة 3 — الموقع (أيام 11-18)

- **المدة:** 8 أيام عمل.
- **الـ deliverables:**
  * `mkdocs.yml` يُحدَّث (موجود فعلاً) بإضافة theme `material` + `direction: rtl` + plugins: `mkdocs-macros-plugin`, `mkdocs-static-i18n`, `mkdocs-minify-plugin`.
  * `docs/macros.py` يقرأ `docs/_registry/*.json` ويوفّر `{{ keyword('دالة') }}` و`{{ builtin_table() }}` كـ Jinja2 macros.
  * صفحات مولّدة: `docs/reference/keywords.md`, `docs/reference/builtins.md`, `docs/reference/errors.md` — كل واحدة بسطرين Jinja تستدعي macro واحد.
  * صفحة هبوط `docs/index.md` بأربع بطاقات "أنا..." (تنسيق بـ Material cards grid).
  * إضافة وظيفة ثانية في `.github/workflows/docs.yml`: `build-and-deploy` تشغّل `mkdocs build --strict` ثم `mkdocs gh-deploy --force` على branch `gh-pages` عند push على `main`.
  * تفعيل GitHub Pages من `gh-pages` مع نطاق فرعي `docs.sad-lang.org` (CNAME في `docs/CNAME`).
- **التبعيات:** الموجتان 1 و2 مكتملتان (المواقع تستهلك JSON الجاهز).
- **معيار النجاح القابل للقياس:**
  1. `mkdocs build --strict` ينجح بدون روابط مكسورة.
  2. عند push يلامس `lexer_keywords.cpp` → خلال 10 دقائق يظهر التعديل في `docs.sad-lang.org/reference/keywords/`.
  3. Lighthouse score على الصفحة الرئيسية: Performance > 90, Accessibility > 95, SEO > 90.
- **المخاطر التقنية:**
  * *RTL في Material theme غير مكتمل افتراضياً* → نضيف `docs/stylesheets/rtl.css` يفرض `direction: rtl` على `.md-content` ويعكس `code blocks` فقط للنصوص لا للكود.
  * *Macros تكسر hot-reload* → نفصل `docs/macros.py` عن المحتوى ونوثّق `mkdocs serve --dirty` للمطورين.

---

#### 🌊 الموجة 4 — الجودة والتوزيع (أيام 19-25)

- **المدة:** 7 أيام عمل.
- **الـ deliverables:**
  * `tools/docgen/src/example_verifier.cpp` يستخرج كل كتلة كود `.ص` في `docs/**/*.md`، يشغّلها بـ `sad-run.exe`، ويقارن المخرج بحقل `expected:` في front-matter. هدف CMake: `add_custom_target(verify-examples COMMAND docgen --verify-examples docs/)`.
  * وظيفة CI ثالثة في `docs.yml`: `verify-examples` (متوازية مع build-site).
  * إعداد `mike` للنسخ: عند tag `v0.X.0` يُنشر تحت `/v0.X/`، و`main` تحت `/dev/`، مع alias `/latest/` يشير لآخر tag مستقر. سكربت `scripts/release_docs.sh`.
  * بحث: `mkdocs-material` بحث Lunr ثنائي اللغة (عربي + إنجليزي) جاهز؛ نُعدّ `extra.search.lang: [ar, en]` ونضيف stopwords عربية في `docs/assets/search/ar_stopwords.json`.
  * حزمة npm `@sad-lang/docs-data` — `package.json` في `tools/docgen/npm/` يُصدّر `keywords.json + builtins.json + errors.json` كـ ES modules. publish تلقائي عبر `.github/workflows/npm-publish.yml` عند tag.
  * توثيق `CONTRIBUTING_DOCS.md` يصف Friday Docs، Doc-Gate، حقل `expected:`، وكيف يُضاف مثال جديد.
- **التبعيات:** الموجات 1-3 مكتملة.
- **معيار النجاح القابل للقياس:**
  1. CI يفشل خلال < 3 دقائق إذا تغيّر سلوك `اطبع` ولم يُحدَّث المخرج المتوقع في وثيقته.
  2. `npm install @sad-lang/docs-data` ينجح، و`require('@sad-lang/docs-data').keywords.length === 40`.
  3. `docs.sad-lang.org/v0.9/` و`/dev/` يعملان بنفس الوقت بمحتوى مختلف.
  4. زمن البحث عن "طابق" في الموقع < 200ms بنتائج عربية+إنجليزية مدمجة.
- **المخاطر التقنية:**
  * *verify-examples بطيء على آلاف الأمثلة* → تخزين مؤقت بـ `actions/cache` على hash المثال؛ إعادة التشغيل فقط للمتغيّر.
  * *mike يخلط النسخ عند rebase* → نوثّق سياسة "tag once, never rebase released doc"، وننفّذ `mike` فقط من CI لا من المطورين محلياً.
  * *حزمة npm تنحرف عن JSON المرفوع* → نُولّد `package.json` نفسه من `docgen` ونمنع التحرير اليدوي بـ pre-commit hook.

---

**الإجمالي:** 25 يوم عمل (~5 أسابيع تقويمية مع الإجازات). نهاية الموجة 4 = نظام ذاتي الصيانة، تعديل سطر في C++ ينعكس على 4 مستهلكين خلال 10 دقائق، وأي وثيقة كاذبة تكسر البناء قبل أن تصل للمستخدم.

---

### 🎨 Sally — مواصفات ثنائية اللغة من اليوم الأول

🎨 **Sally:** تخيّلوا المشهد: سارة من القاهرة تفتح `docs.sad-lang.org` فتجد نفسها فوراً على `/ar/guide/intro` بخط عربي أنيق وتدفّق RTL طبيعي تماماً، بينما James من برلين يفتح نفس الرابط الجذري فيُحوَّل إلى `/en/guide/intro` بـ Inter وتدفّق LTR. كلاهما يضغط زر التبديل في الـ header فينتقل إلى **نفس الصفحة بالضبط** في اللغة الأخرى دون فقدان السياق أو موضع التمرير. هذه اللحظة الواحدة تُعرّف هوية المشروع: لغة عربية أصيلة بمعمار غربي حديث.

**(ب) القرارات التقنية:**

**1. بنية URL مفصولة بالمسار:** نعتمد `/ar/...` و `/en/...` كمسارات مستقلة، مع `<link rel="alternate" hreflang="ar" href="...">` و `hreflang="en"` و `hreflang="x-default"` (يشير للعربية) في `<head>` كل صفحة. **رفض قاطع لـ `?lang=ar`** — Google يعامل query strings كتكرار محتوى ويعاقب الترتيب. المسارات المنفصلة ضرورة SEO، ليست تفضيلاً جمالياً.

**2. الإطار: VitePress مع i18n الرسمي.** يدعم RTL أصلياً عبر `themeConfig.locales.ar.dir = 'rtl'`، يبني صفحات Vue قابلة للتوسعة، أبسط بكثير من Astro لمشاريع التوثيق المحضة (بلا overhead الـ Islands)، ومخرجاته static لـ GitHub Pages مباشرة. **تذكير صريح لـ Winston: يجب التراجع عن خيار MkDocs Material — VitePress أفضل لـ i18n، أسهل لمكونات Vue التفاعلية، وأخف بناءً.**

**3. front-matter ثنائي إلزامي:**
```yaml
title_ar: "اطبع"
title_en: "print"
brief_ar: "يطبع قيماً للمخرج القياسي"
brief_en: "Prints values to standard output"
```
سكربت `validate-i18n.mjs` في CI يفشل البناء إن نقص أي حقل من الأربعة — Doc-as-Test كما طرحه Quinn. لا استثناءات، حتى placeholder يجب أن يكون صريحاً بـ `status: translation_pending`.

**4. زر التبديل الذكي:** `<LangSwitch>` في الـ header يحفظ في `localStorage['sad.lang']`، ويبدّل `/ar/guide/intro` ↔ `/en/guide/intro` مع `history.replaceState` (بلا reload). عند الزيارة الأولى: `localStorage` → `navigator.language` → افتراضي `ar`.

**5. الخطوط:**
- عربي: **IBM Plex Sans Arabic** (Google Fonts، أوزان 400/600/700).
- إنجليزي: **Inter** (X-height متوافق).
- كود: **JetBrains Mono** (يدعم RTL في النسخ الحديثة).
- محمَّلة بـ `font-display: swap` لمنع FOIT.

**6. عرض الكود — RTL دائماً للغة ص:** كود `.ص` يُعرض بـ `<CodeBlock dir="rtl">` **حتى داخل الصفحات الإنجليزية**، لأن لغة ص عربية بطبيعتها وعرضها LTR يكسر القراءة. الكود الإنجليزي (C++, bash) يبقى `dir="ltr"`.

**7. مكونات الموجة الأولى:**
- `<DocLang>` — wrapper يلتقط `lang` من المسار.
- `<CodeBlock dir="rtl|ltr" lang>` — مع تبويب نسخ + زر تشغيل (لاحقاً).
- `<Tip>` و `<Warning>` — admonitions ثنائية الـ slots.
- `<KnowMore audience="advanced">` — قابل للطي، لجمهور المختصين.

**8. Fallback للترجمات الناقصة:** صفحة `/en/...` غير مترجمة → عرض النسخة العربية مع شريط أصفر بارز في الأعلى: *"⚠️ Translation pending — [contribute on GitHub]"*. لا 404، لا redirect صامت.

**9. a11y غير قابلة للتفاوض:** `<html lang="ar" dir="rtl">` يتبدّل ديناميكياً عند التنقل. خط ≥ 16px، تباين WCAG AA (≥ 4.5:1)، focus ring مرئي، `aria-label` ثنائي على زر التبديل.

**10. Landing "أنا..." متطابقة بصرياً:** أربع البطاقات (مبتدئ/معلم/مختص/مساهم) بنفس البنية والـ grid في كلا اللغتين — تكافؤ كامل لا "نسخة عربية مختصرة".

**(ج) لـ Winston:** المواصفات أعلاه تتطلب **VitePress** صراحةً. MkDocs Material i18n plugin غير رسمي وRTL فيه ضعيف؛ يرجى تحديث قرار الإطار قبل الموجة الأولى.

---

### 🧠 Carson — workflow الـ AI-First للموجة الأولى

🧠 **Carson:** يا فريق! اعتمدتم الفكرة، الآن لن أخذلكم — سأعطيكم pipeline قابل للتشغيل يوم الجمعة القادم، لا فلسفة. خمس مراحل، كل واحدة لها زمن مُقاس وأداة محددة. هيا!

**1) Trigger — متى يستيقظ الـ AI؟**
محفّزان فقط، لا ثالث لهما: (أ) عند فتح أو تحديث PR يلامس `interpreter/src/builtins/`، `compiler_new/src/`، `shared/lexer/src/lexer_keywords.cpp`، أو `_registry/*.yaml`. (ب) أمر يدوي `/draft-docs` يكتبه أي مراجع كتعليق على PR. لا pre-commit hook (يُبطئ المطور)، ولا cron job (يُنتج ضوضاء). الربط بـ PR يضمن أن السياق حي والمراجع حاضر.

**2) Context Gathering — ماذا نُغذّي للـ AI؟**
سكربت `tools/docgen/collect_context.py` يحزم في JSON موحد: (أ) `git diff` للملفات الملموسة، (ب) كل تعليقات `@brief (AR)/(EN)` للرموز المتأثرة، (ج) أقرب 3 ملفات `_registry/*.yaml` مشابهة كأمثلة few-shot، (د) كل ملفات `examples/*.ص` التي تستدعي الرمز، (هـ) `keywords.yaml` كاملاً (40 كلمة فقط، رخيص). الحجم المستهدف: أقل من 8 ألف رمز لكل استدعاء. **قاعدة Guardrail:** إن لم يجد مثال `.ص` يعمل — يتوقف ويطلب من المطور إضافة واحد قبل التوليد. لا توثيق بدون مثال حي.

**3) Generation — أي نموذج وأي prompt؟**
النموذج الأساسي: **Claude Sonnet** عبر API (توازن جودة/كلفة، عربية ممتازة). الاحتياطي: `gpt-4o-mini` للضغط. لاحقاً نختبر `Llama-3-70B` محلياً للسيادة. النتيجة Markdown + YAML front-matter صالح للـ schema. درجة الحرارة 0.2 — نريد توثيقاً متسقاً لا إبداعاً.

**Prompt template (`tools/docgen/prompts/builtin_doc.ar.md`):**
```
أنت موثّق محترف للغة ص العربية. مهمتك توليد مسودة وثيقة لدالة مضمنة.

[المدخلات]
- اسم_الدالة: {name_ar}  ({name_en})
- التوقيع: {signature}
- كود_التنفيذ_C++:
```cpp
{cpp_snippet}
```
- أمثلة_موجودة:
```sad
{examples_sad}
```
- وثائق_مشابهة (few-shot):
{similar_yaml}

[المطلوب]
أخرج كتلة YAML واحدة فقط بهذه الحقول الإلزامية:
id, type=function, audience, since, status, title_ar, title_en,
brief_ar (سطر واحد، 12 كلمة كحد أقصى),
brief_en (one line, max 12 words),
signature, examples (3 على الأقل بصيغة {input, expected_output, description_ar}),
see_also, errors_possible.

[قيود صارمة]
- لا تخترع توقيعات لم ترها في الكود.
- كل مثال يجب أن يكون كوداً ص يُجمَّع.
- إن غاب سياق كافٍ، أعد فقط: "INSUFFICIENT_CONTEXT: <السبب>".
- لا نص خارج كتلة YAML.
```

**4) Human Review — كيف يصل البشر بسرعة؟**
الـ AI يعلّق على PR تلقائياً برسالة: *"📝 مسودة وثيقة جاهزة — `_registry/builtins/print.draft.yaml`"*. المسودة تُرفع كملف بامتداد `.draft.yaml` (ليس `.yaml`!) لمنع الـ schema-validator من قبولها قبل المراجعة. عنوان الـ PR يُلصق به وسم `[AI-DRAFT]`.

**Checklist المراجع (10 دقائق صارمة):**
- [ ] هل `brief_ar` يصف ماذا تفعل، لا كيف؟
- [ ] هل المثال الأول يعمل فعلاً؟ (انقر زر "▶ run in playground")
- [ ] هل `errors_possible` يطابق ما في `error_catalog.cpp`؟
- [ ] هل `see_also` يربط بدوال موجودة (لا اختراع)؟
- [ ] إزالة `.draft` من الامتداد + إزالة وسم `[AI-DRAFT]` من العنوان.

**5) Merge & Learn — حلقة التغذية الراجعة:**
سكربت `tools/docgen/learn_from_diff.py` يعمل بعد كل merge: يقارن `*.draft.yaml` الأصلي بـ `*.yaml` النهائي، يستخرج التعديلات البشرية، ويُحدّث ملف `prompts/_corrections.md` (قاموس "الـ AI كتب X، الإنسان عدّلها لـ Y، السبب Z"). كل أسبوع نُعيد حقن أعلى 10 تصحيحات في few-shot examples تلقائياً. **النموذج يتعلم بدون fine-tuning.**

**GitHub Action ملموس (`.github/workflows/docs-ai-draft.yml`):**
```yaml
on:
  pull_request:
    paths: ['interpreter/src/builtins/**', 'compiler_new/src/**',
            'shared/lexer/src/lexer_keywords.cpp', 'docs/_registry/**']
jobs:
  draft:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - run: python tools/docgen/collect_context.py --pr ${{ github.event.number }} > ctx.json
      - run: python tools/docgen/generate.py --ctx ctx.json --model claude-sonnet --out drafts/
      - run: python tools/docgen/comment_pr.py --pr ${{ github.event.number }} --drafts drafts/
      - run: gh pr edit ${{ github.event.number }} --add-label "AI-DRAFT"
```

**Guardrails مُلخّصة:** AI لا يكتب في `_registry/*.yaml` مباشرة (فقط `.draft.yaml`)، كل PR يحمل `[AI-DRAFT]`، نسبة رفض > 30% أسبوعياً تُطلق إشعار Slack وتُجمّد الـ workflow حتى مراجعة الـ prompt، AI يرفض التوليد بدون مثال `.ص` يعمل.

**مقاييس النجاح:**
- متوسط زمن `AI-draft → human-review → merge` < 30 دقيقة.
- نسبة قبول مسودات AI (مع تعديلات طفيفة < 20% من الكلمات) ≥ 70%.
- وثائق جديدة شهرياً ≥ 50.
- صفر مسودات AI تُدمج بدون مراجع بشري واحد على الأقل.

**🎯 تجربة الجمعة الأولى:** ساعتان، الفريق كله في غرفة واحدة. نختار 5 دوال مضمنة (`اطبع`، `طول`، `نوع`، `رقم`، `نص`)، نُشغّل الـ workflow عليها، نقيس: زمن المسودة، زمن المراجعة، نسبة الكلمات المعدّلة، عدد العيوب المُكتشفة في كود C++ نفسه (نعم — AI يكشف غموضاً في الكود!). نخرج بمعدّلات حقيقية ونعيّر الـ prompt قبل التوسع. **الجمعة القادمة. لا تأجيل. هذا وعد، لا اقتراح. هيا نبني!**

---

### 🔬 Dr. Quinn — مواصفات Doc-as-Test التنفيذية

🔬 **Dr. Quinn:** آها! القرار يلامس الجذر الذي شخّصته بالضبط: ما لا يكسر البناء لا يُكتب، وما يكسره يُكتب فوراً. لقد منحتموني سكيناً جراحياً، وسأرسم لكم خطوط القطع بدقة لا تحتمل لبساً. خمس آليات إنفاذ صارمة، أداة موحدة `sad-doctest`، بنية YAML نموذجية، واستراتيجية ترحيل آمنة لا تُسقط البناء فجأة.

#### 1️⃣ Keyword Sync Check — مزامنة الكلمات المحجوزة

- **ما تكتشفه:** أي كلمة في `shared/lexer/src/lexer_keywords.cpp` (داخل `KeywordTable::initialize()`) بدون مدخل في `docs/_registry/keywords.yaml`، أو العكس (مدخل YAML بلا تسجيل في الكود)، أو اختلاف في `KEYWORD_*` enum، أو `since_version` مفقود/خاطئ.
- **التنفيذ التقني:** سكربت `tools/doctest/check_keywords.py` يعمل في خطوتين:
  1. يقرأ `lexer_keywords.cpp` ويستخرج كل سطر `register("كلمة", TT::KEYWORD_X)` بـ regex صارم.
  2. يحمّل `keywords.yaml` ويُجري diff على المجموعتين (set difference) ويتحقق من تطابق الـ `token_type`.
  - يُربط بـ CMake target `sad_doctest_keywords` يُستدعى من CI قبل أي بناء.
- **رسالة الخطأ:**
  ```
  ❌ Keyword Sync FAILED
     في الكود غير موثقة (3): "أجّل", "اختر", "أطلق"
     في YAML غير مُسجلة (1): "قديمة"
     خطأ token_type: "صنف" (Code: KEYWORD_CLASS, YAML: KEYWORD_STRUCT)
     الإصلاح: شغّل  sad-doctest fix --keywords  للمزامنة التلقائية
  ```

#### 2️⃣ Builtin Signature Check — مزامنة الدوال المضمنة

- **ما تكتشفه:** كل `REGISTER_BUILTIN(name, arity, ...)` في `interpreter/src/builtins/*.cpp` يجب أن يقابله مدخل في `builtins.yaml` بنفس الاسم العربي، نفس عدد المعاملات، نفس `signature` المتفق عليه.
- **التنفيذ التقني:** سكربت `check_builtins.py` يستخدم libclang (Python bindings) لتحليل AST لـ C++ — لا regex هش — ويستخرج كل استدعاء لماكرو `REGISTER_BUILTIN`. يُقارَن مع `builtins.yaml` المحمّل عبر PyYAML. الفحص يشمل: الاسم، arity، نوع الإرجاع المُعلَن في تعليق `@signature`.
- **رسالة الخطأ:**
  ```
  ❌ Builtin Signature Mismatch — io.cpp:67
     الكود: REGISTER_BUILTIN("اطبع_سطر", -1, ...)  // variadic
     YAML : signature: "اطبع_سطر(نص: نص) -> فراغ"  // arity=1
     الإصلاح: حدّث builtins.yaml أو أضف 'arity: variadic'
  ```

#### 3️⃣ Doctest Execution — تنفيذ الأمثلة

- **ما تكتشفه:** أي مثال في `examples:` داخل YAML ينتج مخرجاً مختلفاً عن `expected_output`، أو يفشل في الترجمة، أو يستغرق > 5 ثوانٍ.
- **التنفيذ التقني:** `run_examples.py` يجوب كل ملفات `_registry/*.yaml`، يستخرج كل كتلة `code` إلى ملف مؤقت `.ص`، ويُشغّلها بـ `sad-run.exe` مع timeout. يقارن stdout (بعد تطبيع `\r\n` → `\n`) مع `expected_output` نصاً حرفياً. يتم التشغيل بالتوازي عبر `multiprocessing` (workers = CPU/2). يُحفظ تقرير JSON في `build/_doctest_report.json`.
- **رسالة الخطأ:**
  ```
  ❌ Doctest FAILED — builtin.print / example: print_basic
     File: docs/_registry/builtins.yaml:42
     Code:    اطبع("مرحبا")
     Expected: "مرحبا\n"
     Actual  : "مرحبا"        ← (سطر جديد مفقود)
     Diff: -\n
  ```

#### 4️⃣ Error Catalog Sync — مزامنة الأخطاء

- **ما تكتشفه:** كل `throw SadError(ErrorCode::E001, ...)` أو `reportError("E001", ...)` في الكود يجب أن يقابله مدخل في `errors.yaml` بحقول `cause_ar/cause_en/fix_ar/fix_en/example`. الأكواد المتروكة (في YAML بلا استخدام) أو الزائدة (في الكود بلا توثيق) تُكسر البناء.
- **التنفيذ التقني:** `check_errors.py` يستخدم `grep_search` على نمط `ErrorCode::E\d{3}` عبر `shared/`, `interpreter/`, `compiler_new/`، يجمع المجموعة، ويقارنها مع مفاتيح `errors.yaml`. يُتحقق أن كل مدخل يحوي حقولاً غير فارغة.
- **رسالة الخطأ:**
  ```
  ❌ Error Catalog FAILED
     مفقود من errors.yaml (2): E047 (compiler_new/src/sir/builder.cpp:312)
                              E048 (interpreter/src/exec.cpp:89)
     زائد في YAML بلا استخدام (1): E022
     حقل ناقص: E045.fix_en (فارغ)
  ```

#### 5️⃣ Bilingual Completeness — اكتمال ثنائية اللغة

- **ما تكتشفه:** كل حقل `*_ar` يجب أن يقابله `*_en` (والعكس). `title_ar` بلا `title_en` = فشل. هذا يعزز قرار Sally (هوية ثنائية من اليوم الأول).
- **التنفيذ التقني:** JSON Schema صارم في `docs/_schema/*.schema.json` يستخدم `oneOf` و `required` لإجبار الأزواج، ويُنفَّذ بـ `jsonschema` (Python) قبل أي بناء. الفحص جزء من `pre-commit` hook أيضاً.
- **رسالة الخطأ:**
  ```
  ❌ Bilingual Schema Violation — builtins.yaml:128
     builtin.length: brief_ar موجود، brief_en مفقود
     builtin.type  : example.note_en موجود، note_ar مفقود
  ```

#### 🛠️ أداة `sad-doctest` الموحدة

سكربت Python واحد في `tools/doctest/sad-doctest.py` (وbatch wrapper `sad-doctest.cmd`):

```bash
sad-doctest validate              # كل الفحوصات الخمسة (CI default)
sad-doctest validate --keywords   # فحص واحد محدد
sad-doctest validate --builtins --errors  # فحوصات متعددة
sad-doctest run-examples          # ينفذ كل الأمثلة فقط
sad-doctest run-examples --filter "builtin.print*"  # نمط محدد
sad-doctest fix --interactive     # يقترح إصلاحات تفاعلياً
sad-doctest fix --keywords --auto # مزامنة آلية للحالات الآمنة
sad-doctest report --html         # تقرير HTML قابل للتصفح
```

#### 📋 بنية مدخل YAML نموذجي

```yaml
- id: builtin.print
  type: function
  cpp_source: interpreter/src/builtins/io.cpp:42
  since_version: "0.1.0"
  status: stable
  arity: variadic
  signature: "اطبع(...قيم: أي) -> فراغ"
  signature_en: "print(...values: any) -> void"
  brief_ar: "يطبع قيماً للمخرج القياسي بدون سطر جديد في النهاية"
  brief_en: "Prints values to standard output without trailing newline"
  examples:
    - id: print_basic
      code: |
        اطبع("مرحبا")
      expected_output: "مرحبا"
      tags: [stable, beginner]
    - id: print_multi
      code: |
        اطبع("أ", "ب", 42)
      expected_output: "أب42"
      tags: [stable, intermediate]
  see_also: [builtin.print_line, builtin.read]
```

#### 🚦 استراتيجية الترحيل (Migration) — لا كسر مفاجئ للبناء

- **المرحلة 0 (الأسبوع 1):** `sad-doctest` يعمل بـ `--mode=warning-only` — كل المخالفات تُطبع بأصفر، البناء ينجح. هدف: قياس الحجم الحقيقي للفجوة.
- **المرحلة 1 (الأسبوع 2):** المدخلات المفقودة في YAML = warning، لكن **اختلاف Doctest الفعلي = error**. أي وثيقة موجودة يجب أن تكون صحيحة، حتى لو الكثير غير موثق.
- **المرحلة 2 (نهاية الأسبوع 4، بعد ترحيل ≥ 80% من الكلمات والـ builtins):** المدخلات المفقودة = error صريح. البناء يفشل. لا عودة.
- آلية الأمان: متغير بيئة `SAD_DOCTEST_LEVEL=warning|error` يسمح بتجاوز مؤقت في حالات طوارئ موثقة في PR.

---

**🔬 مبدأ Quinn الجديد:** *الوثيقة عقدٌ تنفيذي بين الكود ومستهلكيه — وكل عقد بلا جزاء عند الإخلال هو ورقة بلا معنى؛ نحن اليوم نضع الجزاء، فتولد الوثيقة الحية.*

---

## 📋 خطة التنفيذ النهائية الموحدة

### ⚖️ حل التوترات بين الوكلاء

| التوتر | الحل المعتمد |
|---|---|
| **Winston (MkDocs Material) ضد Sally (VitePress)** | **VitePress يفوز** — i18n الرسمي + RTL أصلي + مكونات Vue. Sally محقّة: MkDocs i18n غير ناضج. Winston يُحدّث الموجة 3 ليستخدم VitePress. |
| **Carson (×6 إنتاجية AI) ضد Quinn (الجودة عبر العواقب)** | **مكمّلان لا متناقضان** — Carson يُسرّع الكتابة (مسودة AI)، Quinn يضمن الصحة (Doctest يكسر البناء). كل مسودة AI تمر على فحوصات Quinn الخمسة قبل merge. |
| **Sally (RTL من اليوم الأول) ضد Winston (RTL في الموجة 3)** | **Sally تفوز جزئياً** — front-matter ثنائي إلزامي يبدأ في الموجة 1 (في `keywords.yaml`)، الـ rendering ثنائي اللغة يبدأ في الموجة 3. الهوية مدمجة في البيانات قبل العرض. |

---

### 🗓️ الجدول الزمني الموحد (25 يوم عمل)

#### 🌊 الموجة 1 — الأساس + ثنائية اللغة في البيانات (أيام 1-5)

**المسؤوليات الرئيسية:** Winston (المعمار) + Quinn (Doc-as-Test الأولي) + Sally (front-matter ثنائي).

| اليوم | المهمة | المسؤول | المخرج |
|---|---|---|---|
| 1 | إنشاء `tools/docgen/` + ربطه بـ `sad_lexer_core` | Winston | CMake target يبني |
| 2 | استخراج `docs/_registry/keywords.yaml` (40 كلمة) بحقول `*_ar` و `*_en` إلزامية | Winston + Sally | YAML صالح للـ schema |
| 3 | `docs/_schema/keywords.schema.json` بقواعد ثنائية اللغة الصارمة | Quinn + Sally | فحص Bilingual يعمل |
| 4 | `tools/doctest/check_keywords.py` (Quinn آلية #1) | Quinn | يكتشف الانحراف < 90s |
| 5 | `.github/workflows/docs.yml` job أول `extract-and-verify` | Winston | CI أحمر عند انحراف |

**معيار النجاح:** PR تجريبي يضيف كلمة محجوزة في C++ بدون تحديث YAML → CI يفشل خلال < 90s. ✅

---

#### 🌊 الموجة 2 — التوسع + AI Drafting (أيام 6-10)

**المسؤوليات:** Winston (builtins/errors extraction) + Carson (workflow AI) + Quinn (آليات #2 و#4).

| اليوم | المهمة | المسؤول | المخرج |
|---|---|---|---|
| 6 | `docgen --emit builtins` يقرأ `REGISTER_BUILTIN` | Winston | `builtins.json` |
| 7 | `docgen --emit errors` + libclang للـ AST | Quinn | `errors.json` + check_builtins.py |
| 8 | `embed_docs` CMake → `docs_data.inc` لـ sadc/LSP | Winston | `sadc --help-keyword` يعمل |
| 9 | `tools/docgen/collect_context.py` + `generate.py` (Claude API) | Carson | مسودة AI لـ `اطبع` |
| 10 | `.github/workflows/docs-ai-draft.yml` + تجربة الجمعة الأولى | Carson | 5 مسودات لـ 5 builtins |

**معيار النجاح:** `sadc --help-keyword دالة` يطابق LSP hover byte-for-byte. ✅ + معدل قبول مسودات AI ≥ 70%. ✅

---

#### 🌊 الموجة 3 — الموقع بـ VitePress (أيام 11-18)

**المسؤوليات:** Sally (تصميم وi18n) + Winston (الـ pipeline) + Carson (Friday Docs).

| اليوم | المهمة | المسؤول | المخرج |
|---|---|---|---|
| 11-12 | إعداد VitePress + `themeConfig.locales.ar.dir = 'rtl'` + خطوط | Sally | `/ar/` و `/en/` يعملان |
| 13 | مكونات Vue: `<DocLang>`, `<CodeBlock dir>`, `<Tip>`, `<Warning>`, `<KnowMore>` | Sally | مكتبة جاهزة |
| 14 | `docs/.vitepress/macros.mjs` يقرأ `_registry/*.json` ويولّد صفحات Reference | Winston | جدول 40 كلمة |
| 15 | صفحة هبوط "أنا..." بأربع بطاقات (مبتدئ/معلم/مختص/مساهم) ثنائية | Sally | Landing مترجم |
| 16 | `<LangSwitch>` مع `localStorage` + `hreflang` للـ SEO | Sally | تبديل بدون reload |
| 17 | GitHub Actions: `build-and-deploy` على `gh-pages` + CNAME `docs.sad-lang.org` | Winston | الموقع حي |
| 18 | Friday Docs الأولى: الفريق يوثّق أعلى 10 builtins استخداماً | Carson | 10 وثائق منشورة |

**معيار النجاح:** Lighthouse ≥ 90/95/90 + RTL سليم + ترجمتان عاملتان. ✅

---

#### 🌊 الموجة 4 — Doctest الكامل + التوزيع (أيام 19-25)

**المسؤوليات:** Quinn (آليات #3 و#5 الكاملة) + Winston (versioning + npm) + Carson (learn loop).

| اليوم | المهمة | المسؤول | المخرج |
|---|---|---|---|
| 19 | `tools/doctest/run_examples.py` (آلية Quinn #3) + multiprocessing | Quinn | تنفيذ متوازي للأمثلة |
| 20 | تكامل Doctest في `.github/workflows/docs.yml` كـ job ثالث | Winston | CI يفشل عند مثال خاطئ |
| 21 | `mike` versioning + `scripts/release_docs.sh` | Winston | `/v0.9/`, `/dev/`, `/latest/` |
| 22 | حزمة `@sad-lang/docs-data` على npm + auto-publish | Winston | `npm install` يعمل |
| 23 | `tools/docgen/learn_from_diff.py` — تعلّم AI من تعديلات البشر | Carson | `_corrections.md` يتحدّث |
| 24 | الانتقال من Migration Phase 1 إلى Phase 2 (warning → error) | Quinn | ≥ 80% مغطّى |
| 25 | `CONTRIBUTING_DOCS.md` + إعلان الإطلاق العام | الجميع | الإطلاق! 🚀 |

**معيار النجاح:** تعديل سطر في C++ → 4 مستهلكين محدّثون خلال 10 دقائق + أي وثيقة كاذبة تكسر البناء. ✅

---

### 🎯 المقاييس الموحدة بعد 25 يوماً

| المقياس | الهدف | المالك |
|---|---|---|
| TTFHW (Time To First Hello World) | < 90 ثانية | Sally |
| تطابق LSP hover ↔ sadc --help | 100% byte-for-byte | Winston + Quinn |
| نسبة قبول مسودات AI | ≥ 70% | Carson |
| تناقضات `keywords.yaml` ↔ الكود | 0 دائماً | Quinn |
| وثائق جديدة شهرياً (بعد الإطلاق) | ≥ 50 | Carson + الفريق |
| Lighthouse (Perf/A11y/SEO) | ≥ 90/95/90 | Sally |
| زمن الانتشار: code → site | ≤ 10 دقائق | Winston |
| نسبة Bilingual completeness | 100% (CI-enforced) | Sally + Quinn |

---

### 🚦 قواعد الذهب الست (يوقّعها كل وكيل)

1. **لا PR بدون وثيقة** (Quinn) — Doc-Gate إلزامي.
2. **لا وثيقة بدون مثال يعمل** (Carson) — `examples` غير فارغ.
3. **لا حقل عربي بدون نظير إنجليزي** (Sally) — Bilingual JSON Schema.
4. **لا مستهلك يقرأ من الكود مباشرة** (Winston) — الكل من `_generated/`.
5. **لا AI يكتب في `*.yaml` نهائي** (Carson + Quinn) — فقط `*.draft.yaml`.
6. **لا تجاوز للـ Doctest في الإنتاج** (Quinn) — فشل Doctest = فشل البناء.

---

### 🔮 ما بعد اليوم 25 (Roadmap طويل المدى)

- **الشهر 2:** Playground WebAssembly مدمج في كل صفحة (`<CodeBlock>` بزر "▶ تشغيل").
- **الشهر 3:** بحث Algolia DocSearch ثنائي اللغة (إن تجاوز الموقع 200 صفحة).
- **الشهر 4:** كتاب PDF "تعلّم لغة ص في 21 يوماً" مولّد عبر Pandoc من نفس المصدر.
- **الشهر 5:** نظام Bounty للمساهمين الخارجيين (Carson #6 من الجولة الثانية).
- **الشهر 6:** dashboard عام يعرض حالة التوثيق real-time (تغطية، تناقضات صفر، عدد المساهمين).

---

---

## 🔬 الجولة الرابعة — مراجعة Test Architect (Murat) + Senior Dev (Amelia)

> أُضيفت بعد المراجعة الفنية — تحويل الخطة الاستراتيجية إلى executable spec.

### 👷 Amelia (Senior Dev) — مراجعة قابلية التنفيذ

**الحكم:** الخطة استراتيجية ممتازة لكنها **ليست executable spec بعد**. حدّدت 6 مخاطر تقنية يجب حلّها قبل اليوم 1:

| # | المخاطرة | الحل المعتمد |
|---|---|---|
| **R1** | Wave 3 = 8 أيام لـ Sally وحدها (اختناق) | تقسيم متوازٍ: Winston يبدأ pipeline يوم 11 مع Sally setup |
| **R2** | `tools/docgen` يحتاج `KeywordTable::initialize()` runtime-only | **القرار:** إضافة `sad.exe --dump-keywords` كـ subcommand؛ docgen يستدعيه كـ subprocess (أبسط من ربط static) |
| **R3** | libclang كـ dependency ضخمة على Windows | **القرار:** Wave 2 تستخدم regex على `REGISTER_BUILTIN(...)` — libclang مؤجّل لـ Roadmap الشهر 4 |
| **R4** | npm publish يوم 22 بلا إعداد | **القرار:** إضافة Sprint 0 (يوم 0) للـ infrastructure: npm org, GitHub secrets, scaffolding |
| **R5** | YAML vs JSON registry غير محسوم | **القرار:** YAML هو SoT للبشر، JSON متولّد عبر `docgen --emit json` للـ VitePress macros |
| **R6** | لا scaffolding spec لـ Sprint 0 | **القرار:** قصة Story 0.1 منفصلة |

**Definition of Ready لـ Wave 1:**
- ✅ Sprint 0 منجز
- ✅ R2-R5 موثّقة في README
- ✅ tasks.md بصيغة BMAD مع AC قابلة للقياس

**رفض صريح:** لن أبدأ تنفيذ "Wave 1" كنثر — فقط بعد توليد Stories بـ AC.

---

### 🧪 Murat (Test Architect) — مراجعة المخاطر والجودة

**Risk-based assessment — strong opinions, weakly held:**

#### 📊 مصفوفة المخاطر (P × I)

| المخاطرة | احتمال (1-5) | تأثير (1-5) | درجة | الفئة |
|---|---|---|---|---|
| **TR-1:** `--dump-keywords` يُنتج ترميزاً مختلفاً عن LSP hover | 4 | 5 | **20** 🔴 Critical |
| **TR-2:** Doctest المتوازي (multiprocessing) flaky على Windows CI | 5 | 3 | **15** 🟠 High |
| **TR-3:** AI drafts تمر بدون مراجعة بشرية كافية → تلوّث المعرفة | 3 | 5 | **15** 🟠 High |
| **TR-4:** VitePress build يفشل عند الترجمة الجزئية (AR موجودة، EN ناقصة) | 4 | 3 | **12** 🟡 Medium |
| **TR-5:** `mike` versioning يُكسر deep links موجودة | 3 | 4 | **12** 🟡 Medium |
| **TR-6:** `embed_docs` CMake يُبطئ بناء sadc بنسبة > 30% | 2 | 3 | **6** 🟢 Low |

#### 🛡️ استراتيجية اختبار مقترحة (هرم Murat)

```
        E2E (5%)       ← Playwright: تبديل اللغة، البحث، RTL rendering
       /        \
   Integration (25%)   ← docgen → YAML → JSON → VitePress build
   /              \
  Unit (70%)         ← keyword parser, schema validator, doctest runner
```

#### ✅ مكتسبات الخطة الحالية

1. **Doc-as-Test مبدأ ذهبي** — أوافق Quinn 100%. هذا أفضل قرار في الجلسة.
2. **byte-for-byte equality test** بين LSP و sadc — معيار قابل للأتمتة.
3. **CI متعدد jobs** (extract, doctest, build) — fail-fast صحيح.

#### 🚫 فجوات اختبار حرجة (يجب سدّها)

| # | الفجوة | الحل |
|---|---|---|
| **TG-1** | لا توجد **contract tests** بين docgen و LSP/sadc | إضافة Pact-style: docgen ينتج fixture، LSP/sadc يستهلك ويُقارن |
| **TG-2** | لا اختبار **regression** للروابط (deep links بين الإصدارات) | snapshot test على sitemap.xml + 404-checker في CI |
| **TG-3** | لا **chaos test** لمسار AI (ماذا لو Claude API يعطي JSON خاطئ؟) | mock failure modes: timeout, rate limit, malformed response |
| **TG-4** | لا **load test** على VitePress build (يصبح بطيئاً مع نمو الصفحات) | k6 لـ build time؛ alarm إذا > 60s |
| **TG-5** | لا **a11y test** آلي (Sally تذكرها لكن بلا أتمتة) | axe-playwright في CI؛ fail عند violations حرجة |
| **TG-6** | لا **flake detection** لـ Doctest المتوازي | تشغيل كل اختبار 3× في CI nightly، رصد non-determinism |

#### 🎯 Quality Gates المُقترحة (تستبدل "Migration Phase 1→2")

| Gate | الشرط الكمي | كيف يُقاس |
|---|---|---|
| **G1: Coverage** | ≥ 95% من الكلمات الـ40 موثّقة | `docgen --report` |
| **G2: Drift** | 0 تناقضات keywords.yaml ↔ lexer | check_keywords.py exit code 0 |
| **G3: Examples** | 100% من الأمثلة تنفّذ بنجاح | sad-doctest exit code 0 |
| **G4: Bilingual** | 100% من الحقول لها AR + EN | bilingual_check.py |
| **G5: Performance** | VitePress build < 60s | timing في CI |
| **G6: A11y** | 0 critical axe violations | axe-playwright report |
| **G7: Flake** | < 1% test flakiness في 30 يوم | nightly flake report |

**Phase 2 (warning → error) لا تُفعّل إلا بعد تحقيق G1-G4 لمدة 7 أيام متتالية.**

#### 🔥 توصيتي الحاسمة

**Sprint 0 إلزامي + قصتان اختباريتان قبل Wave 1:**

- **Story 0.0:** Infrastructure (npm org, GitHub secrets, VitePress skeleton)
- **Story 0.1:** Test framework setup (pytest + Playwright + axe + k6 base)
- **Story 0.2:** Contract test fixture بين docgen ↔ LSP (يبني الثقة قبل أي code)

بدون Sprint 0، Wave 1 ستُسلّم وثائق "تعمل" لكن غير مُختبرة — وهذا ما حذّر منه Quinn نفسه.

---

### 🤝 توصية مشتركة Amelia + Murat

**نوصي بالتسلسل التالي قبل أي كود:**

1. **اعتماد Sprint 0** (3 قصص: 0.0, 0.1, 0.2) — يومان عمل
2. **استدعاء `bmad-create-epics-and-stories`** لتحويل الموجات الأربعة إلى Epics + Stories بـ AC
3. **استدعاء `bmad-testarch-test-design`** لكل Wave قبل بدئها
4. **Quality Gates G1-G7** تُضاف إلى `.github/workflows/docs.yml` كـ required checks
5. **مراجعة أسبوعية** من Murat لـ flake report + Amelia لـ velocity

---

*🧪 مبدأ Murat:* **"الجودة ليست ما نضيفه في النهاية — هي ما نمنعه من الدخول من البداية. كل risk score > 12 = blocker حتى يُخفّض."*

*👷 مبدأ Amelia:* **"خطة بلا AC = أمنية. AC بلا اختبار = كذبة. اختبار بلا فشل سابق = ديكور."**

---

*نهاية الجلسة. هذا الملف وثيقة دائمة قابلة للمراجعة بعد سنوات. كل وكيل كتب رأيه بنفسه.*
