# معمارية Phase 6 — تقسيم SIRBuilder

## الهدف

تفكيك god-class `SIRBuilder` (2003 سطر header + 67 ملف cpp = ~37,121 سطر، 129 method،
30+ field) إلى مكونات صغيرة متخصصة وفق مبدأ المسؤولية الواحدة (CW-01).

## النمط المعماري

**Composition + Context Inheritance.** `SIRBuilder` يبقى الواجهة العامة، لكنه:

1. **يرث من `SIRBuilderContext`** (Step 1) — base struct يجمع كل الحالة (state fields)
2. **يحوي sub-builders كـ `unique_ptr`** (Step 2+) — كل sub-builder يأخذ `SIRBuilder&` ويصل
   للحقول والـhelpers عبر `b_.field_` / `b_.helper()`
3. **يُفوّض inline** — كل API عام محفوظ كـ wrapper سطر واحد يستدعي `subBuilder_->method()`

```
┌─────────────────────────────────────────┐
│  SIRBuilderContext  (base struct)       │
│  ─────────────────────                  │
│  • module_, currentFunction_            │
│  • scopeStack_, functionTable_          │
│  • errors_, classTable_, ...            │
│  • saveContext() / restoreContext()     │
└─────────────────┬───────────────────────┘
                  │ public inheritance
                  ▼
┌─────────────────────────────────────────┐
│  SIRBuilder                             │
│  ─────────                              │
│  • API عام (buildExpression, ...)       │
│  • methodCalls_  ──────► MethodCallBuilder
│  • (مستقبلاً) builtins_ ─► BuiltinBuilder │
│  • (مستقبلاً) calls_    ─► CallBuilder    │
│  • ...                                   │
└──────────────────┬──────────────────────┘
                   │ &b_  (مرجع)
                   ▼
┌─────────────────────────────────────────┐
│  MethodCallBuilder                      │
│  ──────────────────                     │
│  SIRBuilder &b_;                        │
│  • buildChannelMethodCall(...)          │
│  • buildMutexMethodCall(...)            │
│  • buildArrayBasicMethodCall(...)       │
│  • ... 8 methods total                  │
│                                         │
│  داخل التنفيذ: b_.currentBlock_,         │
│                b_.buildExpression(...), │
│                b_.errors_, ...          │
└─────────────────────────────────────────┘
```

## لماذا هذا النمط؟

- **CW-20 (Open/Closed):** إضافة sub-builder جديد لا تتطلب تعديل SIRBuilder سوى إضافة حقل + wrappers
- **CW-21 (Clear Interfaces):** كل sub-builder له header مستقل بحدود واضحة
- **CW-23 (Testability):** sub-builders قابلة للاختبار بـmock SIRBuilder
- **CW-24 (Backward Compatibility):** الكود المستهلك (`builder.buildXxx()`) لا يتغير — wrappers inline تُحافظ على نفس API
- **CW-29 (Avoid Deep Copy):** المرجع `SIRBuilder&` بدون نسخ، والحالة موحدة في base class

## بدائل مرفوضة

| البديل | السبب |
|---|---|
| `friend class` فقط | لا يقلل التعقيد — يبقى الـSIRBuilder ضخماً |
| Static helper functions | يفقد encapsulation ويجعل state عمومية |
| Pure virtual interface + impl | overkill — الـsub-builders محصورة في process واحد |
| Forward all 100+ helpers as static refs | تكلفة memory + تعقيد التهيئة |

## الخطوات (8 جلسات)

| # | المكون | Methods | Files | الحالة |
|---|---|---|---|---|
| 1 | `SIRBuilderContext` (base) | — | 1 (h) | ✅ commit `63b55d0f` |
| 2 | `MethodCallBuilder` | 8 | 4 (cpp) + 1 (h) | ✅ commit `7ad72506` |
| 3 | `BuiltinBuilder` | ~16 | ~13 (cpp) | ⏳ |
| 4 | `CallBuilder` | ~8 | ~3 (cpp) | ⏳ |
| 5 | `ClassBuilder` | ~6 | ~2 (cpp) | ⏳ |
| 6 | `StatementBuilder` | ~25 | ~6 (cpp) | ⏳ |
| 7 | `ExpressionBuilder` | ~28 | ~5 (cpp) | ⏳ |
| 8 | `TemplateBuilder` | ~14 | ~2 (cpp) | ⏳ |

## قواعد ذهبية للجلسات القادمة

1. **CMake GLOB:** `compiler/CMakeLists.txt` يستخدم `GLOB src/frontend/*.cpp` غير recursive.
   كل مجلد فرعي جديد تحت `src/frontend/` يحتاج إضافة `GLOB` صريح في `CMakeLists.txt`.

2. **مسار الـincludes:** من `src/frontend/builders/` استخدم `#include "sir_builder.h"`
   (وليس `"../sir_builder.h"`) — لأن include path هو `compiler/include/frontend/`.

3. **Free functions في cpp:** بعض الدوال (`toSafeOperand`) معرّفة كـ `static` داخل cpp
   وليست أعضاء في SIRBuilder. لا تُسبَق بـ`b_.`. أبقِ قائمة استثناءات صريحة.

4. **Type aliases في headers:** لا تُعِد تعريف `using MethodCallExpr = ...` داخل
   `Sad::Compiler::SIR::AST` — هذا يتعارض مع التعريف الموجود في `sir_builder.h`.
   استخدم النوع الكامل المؤهل: `Sad::AST::MethodCallExpr`.

5. **التحقق من baseline:** بعد كل Step، شغّل `python tests/dual_execution/runner.py`
   وتحقق أن النتيجة 793/837 = 94.7% (الـbaseline قبل Phase 6).

6. **Constructor init:** كل sub-builder يُهيّأ في `SIRBuilder::SIRBuilder()` body بـ
   `methodCalls_ = std::make_unique<MethodCallBuilder>(*this);` — لا يصح في initializer list
   لأن الحقول الموروثة من `SIRBuilderContext` تُهيّأ تلقائياً.

7. **Inline wrappers:** كل method منقولة تُستبدل في `sir_builder.h` بـwrapper سطر واحد:
   ```cpp
   std::optional<BuildResult> buildChannelMethodCall(AST::MethodCallExpr *e, const BuildResult &o)
   { return methodCalls_->buildChannelMethodCall(e, o); }
   ```
   هذا يحفظ التوافق الخلفي (CW-24).

## مرجع الـcommits

| Commit | الوصف |
|---|---|
| `8365bc1d` | Step 1a — حذف 9 dead method declarations |
| `63b55d0f` | Step 1 — استخراج `SIRBuilderContext` (base class) |
| `7ad72506` | Step 2 — استخراج `MethodCallBuilder` (8 methods, 4 ملفات) |
