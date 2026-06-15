# stories/ — نظام الأنواع الموحّد

قصص تنفيذ خطّة إكمال نظام الأنواع ([TYPE_SYSTEM_COMPLETION_PLAN.md](../docs/TYPE_SYSTEM_COMPLETION_PLAN.md)).
كل ملف = قصة واحدة قابلة للتطوير في sprint. مكتوبة بصيغة أميليا (ملف:سطر، AC مرقّمة).

## الخريطة (تسلسل التبعيات)

| القصة | العنوان | المايلستون | الجهد | يعتمد على |
|-------|---------|-----------|:-----:|-----------|
| [S-TS-P0.5](./STORY-TS-P0.5-generate-typekind.md) | **توليد SadTypeKind من types.yaml** (إنهاء خرق SoT) | M1-Credibility | L | — |
| [S-TS-P0](./STORY-TS-P0-consistency.md) | اتّساق الأرقام (34/46/49) — ثانوي بعد P0.5 | M1-Credibility | S | — |
| [S-TS-P5](./STORY-TS-P5-test-suite.md) | إحياء suite الاختبارات النوعية | M1-Credibility | M | — |
| [S-TS-P1](./STORY-TS-P1-null-type.md) | Null متمايز عن Void | M2-CoreSafety | M | P0 |
| [S-TS-P2](./STORY-TS-P2-semantic-unify.md) | توحيد المدقّق الدلالي (28 موضعًا) | M2-CoreSafety | L | P0 |
| [S-TS-P2.5a](./STORY-TS-P2.5a-migrate-ast-datatype.md) | ترحيل DataType في الـAST/المحلّل | M2-CoreSafety | L | P2 |
| [S-TS-P2.5b](./STORY-TS-P2.5b-delete-bridge.md) | **حذف دوال DataType من الجسر** | M2-CoreSafety | M | P2, P2.5a |
| [S-TS-P3](./STORY-TS-P3-result-type.md) | Result: enum ميتة → تطبيق | M3-AdvancedTypes | M | P2.5b |
| [S-TS-P4](./STORY-TS-P4-wire-advanced.md) | توصيل Optional/Future/Generator | M3-AdvancedTypes | L | P2.5b |
| [S-TS-P6](./STORY-TS-P6-enum-cleanup.md) | تدقيق وتصنيف enum (لا إزالة بلا إثبات) | M4-Cleanup | S | P4 |
| [S-TS-P7](./STORY-TS-P7-interning.md) | interning للأنواع المركّبة | M4-Cleanup | M | P2.5b |
| [S-TS-P8](./STORY-TS-P8-null-operators.md) | تسجيل ?./?? في SoT + !! + ؟ عربي | M3-AdvancedTypes | M | P1, P4 |
| [S-TS-P9](./STORY-TS-P9-nonnull-enforcement.md) | فرض أمان null (استنتاج لاشيء + areTypesCompatible) | M3-AdvancedTypes | L | P1, P2, P4, P8 |
| [S-TS-P10](./STORY-TS-P10-flow-narrowing.md) | تضييق التدفّق (smart narrowing) | M3-AdvancedTypes | L | P9 |
| [S-TS-P11](./STORY-TS-P11-graphics-types-sot.md) | تسجيل أنواع الرسومات في SoT (قابلية التوسّع) | M4-Cleanup | M | P0.5, P6 |

## نموذج أمان null (ADR-TYPESYSTEM-001 — مُراجَع v2)
[ADR-TYPESYSTEM-001](../decisions/ADR-TYPESYSTEM-001-NULL-SAFETY.md) (مقترَح، v2 بعد نقد أميليا): غير-قابل-لـnull افتراضيًّا + `T?` اختياري (لاتيني — المنفّذ).
> **مفاجأتان من الكود:** (1) سطح `?.`/`??` **منفّذ عبر المحرّكين** لكنه يفحص `isVoid()` (خلط) وغير مسجّل في SoT. (2) الدلالات تستنتج `لاشيء→Unknown` ([type_checker.cpp:400](../../../../shared/semantic/src/semantic/type_checker.cpp#L400)) فيهرب من الفحص (48 موضعًا). فالعمل **إرساء على أساس Null حقيقي** في الطبقتين.
سلسلة الأمان: **P1** (Null + إعادة إرساء العوامل) → **P4** (الاختياري + `isAssignableTo`) → **P8** (SoT + `!!`) → **P9** (الفرض: استنتاج + `areTypesCompatible`) → **P10** (تضييق التدفّق). لا يكتمل قبل P9؛ التجربة الكاملة بـP10.

> **حسم ملكية الدوال (نقد أميليا P9):** `isAssignableTo:1501`→P4؛ `areTypesCompatible:244`+استنتاج `لاشيء:400`→P9. لا ازدواج.

## الموجات (من الخطّة)
1. **مصداقية فورية**: P0 + P5
2. **أمان + توحيد جوهري (الاستبدال الكامل)**: P1 + P2 + **P2.5a** (ترحيل AST) + **P2.5b** (حذف دوال DataType)
3. **إكمال متقدّم**: P3 + P4
4. **تنظيف**: P6 + P7

## ملاحظات نقد أميليا المطبّقة
- **ع-2**: `Vector` مُطبَّق SIMD — أُخرج من «الميتة»؛ P6 صار تدقيقًا لا إزالة.
- **ع-6**: P2.5 قُسِّمت إلى P2.5a (ترحيل) + P2.5b (حذف دوال DataType فقط — الجسر يبقى لـ ValueType/runtime).
- **ع-3/ع-5/ع-7/ع-8**: مسارات وأسماء API مصحّحة (`value.cpp:1458`, `makeArray`, إنشاء `quality_gates`).
- تعارض دمج محتمل: `sad_type_system.h` enum يلمسه P0/P1/P6/P7 — تُنفَّذ بالتسلسل لا بالتوازي.

## التزام مهارة sad-lang-dev (تدقيق الحوكمة + SoT)
- **حوكمة**: هذه القصص محكومة (`_bmad-output/`). كل قصّة كودية: سطر إقرار + تحديث `status/` بدليل (GR-01) + ADR للقرارات (GR-02). انظر [الخطّة §0.5](../docs/TYPE_SYSTEM_COMPLETION_PLAN.md).
- **م-1 (SoT)**: `language-truth/types.yaml` يميّز `type.null` («عدم») عن `type.void` («فراغ») — فـ**P1 يُرجع «عدم» لا «لاشيء»** (صُحِّح).
- **م-3 (SoT-First)**: `SadTypeKind` يدوي (`gen_types.py` معطّل)؛ الأنواع السطحية الـ9 من `types.yaml` لا الـ49 الداخلية.
- **م-4 (DoD)**: كل قصّة كودية ترث بوّابات التسليم: اختبار في **المفسّر + المترجم sadc** (BF-08)، بناء Debug+Release، `.ص` إيجابي/سلبي عبر `runner.py`، تحليل أثر الأدوات (sadinfo/LSP).

*هذا المجلد جزء من نظام $s.*
