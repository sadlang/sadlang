---
title: دليل نظام الأخطاء في لغة ص — المعمارية + كيف تضيف/تستعمل خطأً
system: error-messages
date: 2026-06-11
author: Amelia (Dev)
status: ACTIVE
sources: shared/errors/* + language-truth/errors/* + interpreter/include/builtins/builtin_context.h + scripts/codegen/gen_error_messages.py
revision: "بعد اكتمال EM-CPP (26 PR): توحيد الدوال المضمنة + ICE المترجم/المفسر"
---

# دليل نظام الأخطاء في لغة ص

> توثيق شامل: المعمارية، تدفّق البيانات، الآليات الأربع للرمي، فئة الأخطاء الداخلية (ICE)،
> ودور كل ملف — بأدلة من الكود الفعلي. **المصدر الموحَّد:** `language-truth/errors/*.yaml`
> (يُولَّد منه كتالوج C++). **بعد ترحيل EM-CPP: كل خطأ يراه المستخدم — لغوياً أو داخلياً —
> يأتي من YAML؛ لا نصّ رسالة مكتوب يدوياً في C++** (الاستثناء الوحيد: "Debugger disconnected"
> وهو تدفّق تحكّم لا رسالة خطأ).

---

## 1. نظرة عامة — الفكرة الجوهرية

نظام الأخطاء **مدفوع بالبيانات وموحَّد**:
- **تعريفات الأخطاء** تعيش في YAML (مصدر واحد، **8 فئات / 84 رمزاً**) → تُولَّد إلى كتالوج C++.
- **كل موقع خطأ** يُشير إلى **رمز `ErrorCode`** (لا نص خام) + `placeholders` تحمل **بيانات فقط**
  (أسماء/قيم/مُعرِّفات) — لا نثراً.
- **الكتالوج** يرندر الرسالة النهائية ثنائية اللغة حسب مستوى الشرح واللغة.
- **المنسّق** يطبع التشخيص (caret + ألوان + تلميحات).

**القاعدة الذهبية:** نص الرسالة يعيش في YAML **وحده**. حتى أخطاء المترجم الداخلية (ICE)
وأخطاء المفسر الداخلية تأتي من الكتالوج، موسومةً «خطأ مترجم/مفسر — أبلِغ»، مع شرح للمطوّر.

| الفئة (YAML) | البادئة | عدد | أمثلة |
|------|:---:|:---:|------|
| lexical | LEX | 6 | حرف غير صالح، سلسلة غير منتهية |
| syntactic | SYN | 8 | فاصلة منقوطة ناقصة |
| semantic | SEM | 9 | متغير/دالة غير معرّف، إسناد لثابت |
| runtime | RUN | 52 | فهرس خارج النطاق، قسمة على صفر، خطأ دالة مضمنة |
| ownership | OWN | 0* | (مُصنَّفة تحت runtime في الكود) |
| import | IMP | 1* | (مرجع) |
| io | IO | 1* | (مرجع — RUN007) |
| **internal** | **INT** | **9** | **ICE: IR فارغ، operands، مرجع غير معرّف، مكدّس النطاقات** |

\* فئات مرجعية (رموزها الفعلية تحت runtime).

---

## 2. تدفّق البيانات: من YAML إلى الشاشة

```mermaid
flowchart LR
  YAML["language-truth/errors/*.yaml<br/>(8 فئات، 84 رمزاً)"]
  ENUM["error_codes.h<br/>enum ErrorCode (84)"]
  GEN["error_messages_generated.{h,cpp}<br/>(مُولَّد — لا يُحرَّر يدوياً)"]
  CAT["ErrorCatalog<br/>(قوالب + render)"]
  EM["ErrorManager<br/>(تجميع التشخيصات)"]
  FMT["Formatter<br/>(caret + ألوان)"]
  OUT["الإخراج للمستخدم"]

  YAML -->|"gen_error_messages.py (CMake، 6 فحوص دلالية)"| GEN
  ENUM -->|"تطابق code↔enum إلزامي"| GEN
  GEN -->|"registerDefaults()"| CAT
  CAT --> EM
  EM --> FMT --> OUT
```

> **دليل:** `scripts/codegen/gen_error_messages.py` → `shared/errors/generated/error_messages_generated.cpp`
> (`registerDefaults` + `kErrorMessages`). مربوط في `cmake/codegen.cmake`. يُستدعى من
> `error_manager.cpp` عند التهيئة. **المولّد يفرض:** كل code في enum، تطابق id↔prefix،
> تفرّد، تغطية كاملة (V5: 9 اختبارات حارسة في `test_gen_error_messages_v5.py`).

---

## 3. الآليات الأربع للرمي/الإبلاغ — حسب الطبقة

بعد EM-CPP صار للنظام **أربع نقاط دخول موحَّدة**، كلها تنتهي إلى `ErrorCatalog`:

```mermaid
flowchart TD
  subgraph CT["① وقت الترجمة (تجميع — غير رامٍ)"]
    LEX["lexer / parser / semantic / codegen"] --> RFC["reportFromCatalog(code, location, ctx)"]
  end
  subgraph RT["② وقت تشغيل المفسر العام (رامٍ)"]
    VIS["visitors / managers / dispatch"] --> TR["throwRuntime(code, Position, placeholders)"]
    TR --> ABORT["RuntimeAbort (لفّ المكدّس)"]
  end
  subgraph BX["③ الدوال المضمنة — سياق متاح"]
    CTXL["builtin ctx-lambda"] --> CE["ctx.error(code, placeholders)<br/>(يحقن func/builtin)"]
    CE --> TR
  end
  subgraph BC["④ الطبقة الأدنى — بلا Position"]
    LOW["shared/builtins (helpers/parsers)"] --> BE["BuiltinError(code, placeholders)<br/>(حامل بلا موقع)"]
    BE -.->|"يُلتقَط في callNative"| CN["FunctionDefinition::callNative<br/>(يحقن الموقع + func/builtin)"]
    CN --> TR
  end
  RFC --> EM2["ErrorManager"]
  TR --> EM2
  EM2 --> CATR["ErrorCatalog.render(code, ctx, level, lang)"]
```

| # | الآلية | الموقع | متى | استشهاد |
|---|--------|--------|-----|---------|
| ① | `reportFromCatalog` | lexer/parser/semantic/**codegen** | تجميع (له `SourceLocation`) | `error_manager.h:387` |
| ② | `throwRuntime` | visitors/managers (له `Position`) | تشغيل عام | `runtime_throw.h:63` |
| ③ | **`ctx.error`** | lambdas الدوال المضمنة (252 موقعاً) | تشغيل، السياق متاح | `builtin_context.h:63` |
| ④ | **`BuiltinError`** carrier | shared/builtins الأدنى (47 موقعاً) | تشغيل، لا موقع | `builtin_error.h` |

> **التوزيع بعد الترحيل:** الدوال المضمنة **100%** من الكتالوج (252 `ctx.error` + 47 `BuiltinError`).
> codegen المترجم **100%** (233 موقع → رموز ICE). الرمي الخام الوحيد المتبقّي: 5×
> "Debugger disconnected" (تدفّق تحكّم، يُلتقَط بـ`catch(runtime_error)` — ليس رسالة خطأ).

### نهاية دورة الحياة: من يلتقط ويطبع؟ (مهم)

`throwRuntime` يفعل **شيئين**: (1) يُبلّغ `ErrorManager` فوراً (الرسالة تُسجَّل)، ثم
(2) يرمي `RuntimeAbort` — وهو **إشارة فارغة** للفّ المكدّس فقط (لا يحمل الرسالة).

```mermaid
flowchart LR
  TR["throwRuntime(code, pos, ph)"] -->|"1) يُسجّل"| EM["ErrorManager (الرسالة محفوظة)"]
  TR -->|"2) يرمي إشارة"| RA["RuntimeAbort"]
  RA -->|"لفّ المكدّس"| TOP["interpreter_core.cpp:489<br/>catch(RuntimeAbort)"]
  TOP -->|"EM.flush(std::cerr) :500"| PR["طباعة التقرير (caret + ألوان)"]
```

> **دليل:** `interpreter_core.cpp:489` يلتقط `RuntimeAbort` (إشارة فارغة — الخطأ مُسجَّل سلفاً)
> ثم `EM.flush(std::cerr)` @ `:500` يطبع. هذا يفصل **التسجيل** (وقت الرمي) عن **العرض** (أعلى المكدّس).

### أي آلية أختار؟ (شجرة قرار)

```mermaid
flowchart TD
  Q1{"أين أنت؟"}
  Q1 -->|"تجميع (lexer/parser/semantic/codegen)"| A1["① reportFromCatalog<br/>(له SourceLocation)"]
  Q1 -->|"داخل lambda دالة مضمنة"| A3["③ ctx.error<br/>(يحقن func/builtin)"]
  Q1 -->|"shared/builtins طبقة أدنى (بلا Position)"| A4["④ throwBuiltin / BuiltinError"]
  Q1 -->|"visitor/manager في المفسر"| A2["② throwRuntime<br/>(له Position)"]
  A1 --> CAT3["ErrorCatalog"]
  A2 --> CAT3
  A3 --> CAT3
  A4 --> CAT3
```

### 3-أ. لماذا آليتان للدوال المضمنة (③ و ④)؟

```mermaid
flowchart LR
  subgraph L1["طبقة المفسر (لها BuiltinContext)"]
    A["ctx-lambda مُسجَّلة"] -->|"ctx يحمل args+pos+name"| B["ctx.error(code)"]
  end
  subgraph L2["الطبقة الأدنى shared/builtins (لا تعرف Position/المفسر)"]
    C["helper/parser داخلي"] -->|"يرمي حاملاً"| D["BuiltinError(code)"]
  end
  B --> E["throwRuntime بموقع الاستدعاء"]
  D -->|"callNative يلتقط ويُكمل الموقع"| E
```

- **③ `ctx.error`**: داخل lambda مُسجَّلة تملك `BuiltinContext` (الوسائط + الموقع + الاسم).
  تحقن تلقائياً `{func}` و`{builtin}` ثم تستدعي `throwRuntime`.
- **④ `BuiltinError`**: الطبقة الأدنى (`shared/builtins`) **لا** تملك `Position` ولا `BuiltinContext`
  (طبقية). ترمي حاملاً `(code + placeholders)` بلا موقع؛ **نقطة الاختناق** `callNative` تلتقطه
  وتُكمل الموقع + تحقن `func/builtin` ثم `throwRuntime`. (انظر `function_manager.h:213-224`.)
  ⚠️ **عُرف**: أي `catch(std::exception)` واسع يلفّ استدعاء دالة مضمنة يجب أن يُعيد رمي
  `BuiltinError` أولاً (موثَّق في `builtin_error.h`).

### تسلسل خطأ دالة مضمنة (مثال `طول()` بلا وسائط)

```mermaid
sequenceDiagram
  participant U as كود ص: طول()
  participant D as الموزّع → callNative
  participant L as builtin length() (shared)
  participant E as ErrorCatalog
  U->>D: استدعاء بلا وسائط
  D->>L: BuiltinFunctions::length(args)
  L-->>D: throw BuiltinError(RUN_BUILTIN_REQUIRES_ARG)
  D->>D: callNative يلتقط: ph["func"]="طول"
  D->>E: throwRuntime(code, pos, ph) → render
  E-->>U: «[RUN037] الدالة المدمجة 'طول' استُدعيت بوسائط ناقصة»
```

---

## 3-ب. طبقتا الأخطاء — حسب **البيئة** لا حسب مفسر/مترجم

> **مبدأ:** الطبقة تُحدَّد بـ**توفّر STL/runtime** لا بكون المسار مفسراً أم مترجماً.
> **المترجم على هدف مُضيف يحصل على الرسائل الغنية مثل المفسر تماماً.** التقييد **فقط** في **freestanding**.

```mermaid
flowchart TD
  SRC[".ص"]
  SRC -->|"المفسر sad-run"| I["مُضيف (STL + ErrorCatalog)"]
  SRC -->|"sadc → هدف مُضيف"| BH["ثنائي مُضيف (STL متاح)"]
  SRC -->|"sadc → freestanding/نواة"| BF["ثنائي bare-metal (لا STL)"]
  I --> T1["**Tier 1 (غني)**: الكتالوج الكامل<br/>ثنائي اللغة + fix-hint + مستويات + معلّم"]
  BH --> T1
  BF --> T2["**Tier 2 (أدنى)**: sad_panic + جدول مُولَّد<br/>رسالة قصيرة + halt — **فقط هنا**"]
```

`ErrorCatalog::render` يحتاج `std::string`+`std::map`+heap — غير موجودة على bare-metal،
فأقصى ممكن هناك كتابة بايتات على VGA/UART + `hlt`. **هذا القيد يخصّ freestanding وحده.**

---

## 4. فئة الأخطاء الداخلية (ICE) — internal.yaml

أخطاء «لا ينبغي أن تحدث» في المترجم أو المفسر. **تأتي من الكتالوج** (لا نصّ يدوي)، موسومةً
صراحةً كـICE مع **شرح موجَّه للمطوّر** (لماذا + كيف يُصلحها)، و`{detail}` يحمل **مُعرِّفاً** فقط.

```mermaid
flowchart TD
  subgraph COMP["المترجم (sadc) — 7 عائلات"]
    C1["cg_.reportError(code, {detail})"] --> CAT
    C2["sir_module: iceMsg(code, detail)"] --> CAT
  end
  subgraph INTERP["المفسر — رمزان"]
    I1["FunctionManager: تعريف بلا اسم"] --> CAT
    I2["ScopeManager: خلل مكدّس النطاقات"] --> CAT
  end
  CAT["ErrorCatalog (فئة internal)"]
```

| الرمز | id | متى |
|------|:--:|-----|
| INT_COMPILER_NULL_IR | INT001 | عقدة IR فارغة في codegen |
| INT_COMPILER_INVALID_OPERANDS | INT002 | عدد معاملات تعليمة خاطئ |
| INT_SIR_OPERAND_RESOLVE | INT003 | تعذّر حلّ معامل (SSA/value map) |
| INT_SIR_UNDEFINED_REF | INT004 | سجلّ/عام/دالة/صنف غير معرّف |
| INT_SIR_FIELD_LAYOUT | INT005 | تخطيط حقل/بنية صنف |
| INT_BACKEND_EMIT | INT006 | فشل إصدار LLVM (target/file) |
| INT_SIR_TYPE_CONSTRAINT | INT007 | انتهاك قيد نوع في codegen |
| INT_INTERP_NAMELESS_DEFINITION | INT008 | تسجيل دالة بلا اسم (مفسر) |
| INT_INTERP_SCOPE_STACK | INT009 | إزالة النطاق العام / مكدّس فارغ (مفسر) |

> **دليل:** `compiler/src/backend/llvm/llvm_codegen_context.cpp` (overload `reportError(ErrorCode, placeholders)`)
> · `compiler/src/frontend/sir_module.cpp` (`iceMsg`) · `interpreter/src/managers/*` (`throwRuntime(INT_*)`).

### مبدأ `{detail}`: بيانات لا نثر

`placeholders` تحمل **مُعرِّفات/قيماً** فقط؛ الجملة الوصفية كلها في YAML. مثال من ترحيل codegen:

```cpp
// ❌ قبل (نثر إنجليزي مكتوب يدوياً في C++):
cg_.reportError("Add instruction requires 2 operands");
// ✅ بعد ({detail} = مُعرِّف التعليمة فقط؛ الجملة في internal.yaml):
cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS, {{"detail", "Add"}});
```

---

## 4-ب. مكوّنات العرض وتجربة المستخدم (shared/errors)

طبقة العرض الذكية تحوّل الرمز + السياق إلى تشخيص تعليمي. (الكتالوج يرندر النص؛ هذه تُحسّن العرض.)

| الملف | الدور |
|------|------|
| `formatter.{h,cpp}` | طباعة موحَّدة: caret `^` + ألوان + ترتيب |
| `explanation_level.{h,cpp}` | مستوى الشرح (مبتدئ/خبير) + لغة الإخراج |
| `teacher_mode` (`smart_teacher_mode.cpp`) | وضع المعلّم — شروحات تعليمية مفصّلة |
| `error_hints.{h,cpp}` | نصائح للمبتدئين |
| `fix_suggestions` (`smart_fix_suggestions.cpp`) | اقتراحات إصلاح للأخطاء الشائعة |
| `suggestions` (`smart_suggestions.cpp`) | اقتراح ذكي بتشابه نصّي للأسماء (did-you-mean) |
| `type_explanations` (`smart_type_explanations.cpp`) | شرح تعليمي لعدم تطابق الأنواع |
| `cascade_prevention` (`smart_cascade_prevention.cpp`) | منع تسلسل الأخطاء (جذر واحد بدل أعراض) |
| `error_recovery` (`smart_error_recovery.cpp`) | استرداد المحلل النحوي لمواصلة التحليل |
| `diagnostic.{h,cpp}` · `multi_error.h` | نموذج التشخيص (fix-its/notes) + جمع أخطاء متعددة |

---

## 5. آلية الدوال المضمنة — التوقيع الموحَّد (BuiltinContext)

بعد EM-CPP، **كل** دالة مضمنة تُسجَّل بتوقيع موحَّد `(BuiltinContext& ctx)` — حُذف النوع
القديم `(args)` بالكامل (لا shim).

```mermaid
flowchart LR
  REG["registerBuiltinFunction(name, ctxLambda)"] --> FD["FunctionDefinition<br/>(nativeImplementationCtx_ فقط)"]
  CALL["استدعاء من كود ص"] --> CN["callNative(args, pos)"]
  CN -->|"يبني"| CTX["BuiltinContext(args, pos, name)"]
  CTX --> LAM["ctxLambda(ctx)"]
  LAM -->|"عند خطأ"| CE["ctx.error(code) → throwRuntime"]
  LAM -->|"BuiltinError من طبقة أدنى"| CAT2["callNative يلتقط + يحقن الموقع"]
```

> **علة dispatch حقيقية أُصلِحت (PR #31):** `hasNativeImplementation()` كان يفحص العضو القديم
> فقط → الدوال المُحوّلة لـctx تُرى «غير أصلية» فتفشل أولوية fallback في الموزّع (SEM004).
> الإصلاح: يفحص `nativeImplementationCtx_`. (`function_manager.h`.)

---

## 6. كيف تستعمل خطأً موجوداً (حسب الطبقة)

> اختر الرمز من `error_codes.h`، ومرّر `placeholders` (**بيانات فقط**). **لا تكتب نصاً.**

### أ) المفسر — visitor/manager (له `Position`)
```cpp
#include "runtime_throw.h"
Sad::Errors::throwRuntime(
    Sad::Errors::ErrorCode::RUN_INDEX_OUT_OF_RANGE,
    node.position,
    {{"index", std::to_string(i)}, {"length", std::to_string(n)}});
```

### ب) المترجم/المعجمي/النحوي (تجميع — له `SourceLocation`)
```cpp
#include "error_manager.h"
Sad::Errors::ErrorManager::getInstance().reportFromCatalog(
    Sad::Errors::ErrorCode::SYN_MISSING_SEMICOLON, location, ctx);
```

### ج) دالة مضمنة (لها `BuiltinContext`)
```cpp
// داخل ctx-lambda — يحقن func/builtin تلقائياً:
if (ctx.argCount() < 1)
    ctx.error(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
```

### د) طبقة shared/builtins الأدنى (لا Position)
```cpp
#include "builtin_error.h"
if (args.empty())
    Sad::Errors::throwBuiltin(Sad::Errors::ErrorCode::RUN_BUILTIN_REQUIRES_ARG);
```

### هـ) خطأ مترجم داخلي (ICE) في codegen
```cpp
cg_.reportError(::Sad::Errors::ErrorCode::INT_COMPILER_INVALID_OPERANDS,
                {{"detail", "Add"}});   // {detail} = مُعرِّف التعليمة فقط
```

---

## 7. كيف تضيف خطأً جديداً (الإجراء الكامل)

> ⚠️ يُمنع تحرير `generated/` يدوياً. ابدأ من YAML + enum.

```mermaid
flowchart LR
  S1["1. error_codes.h<br/>enum ErrorCode (PREFIX_NAME)"] --> S2["2. language-truth/errors/&lt;cat&gt;.yaml<br/>(title/brief/fix_hint/detailed/placeholders)"]
  S2 --> S3["3. أعد البناء<br/>(توليد آلي + 6 فحوص)"]
  S3 --> S4["4. أطلِق عبر الآلية المناسبة<br/>(القسم 6)"]
  S4 --> S5["5. اختبار حارس"]
```

1. **الرمز** في `error_codes.h`: `<PREFIX>_<NAME>` (PREFIX ∈ LEX/SYN/SEM/RUN/OWN/IMP/IO/**INT**) + تعليق `id`.
2. **التعريف** في `language-truth/errors/<category>.yaml` (انسخ بنية خطأ شقيق):
   ```yaml
   - code: RUN_MY_NEW_ERROR
     id: RUN099                     # ^[A-Z]{2,3}\d{3}$
     category: runtime
     title:    { ar: "...", en: "..." }
     brief:    { ar: "... {param}", en: "... {param}" }
     fix_hint: { ar: "...", en: "..." }
     placeholders: [param]          # بيانات فقط
     detailed: { ar: "...", en: "..." }
   ```
   ⚠️ اقتبس أي قيمة فيها `:` (مثل "Workaround:") لتفادي خطأ YAML.
3. **أعد البناء** — `sad_error_messages_codegen` يُعيد التوليد ويفرض الفحوص.
4. **أطلِق** عبر الآلية المناسبة (القسم 6).
5. **اكتب اختباراً** (وحدة في V5 و/أو سلوكي في `tests/builtin_errors/`).

---

## 8. دروس مستفادة (تصحيحات ذاتية — GR-01)

أثناء الترحيل صُحّحت ثلاثة تشخيصات خاطئة بشفافية — مفيدة لمن يصحّح النظام:
- **«علة arity»** → كانت **module-gating سليماً** (الدوال المُبوّبة تُحمَّل بـ`استورد`).
- **«علة الاستيراد»** → الاستيراد يعمل؛ الخطأ كان **أسماء دوال خاطئة في الاختبار** (لوغ ≠ لوغاريتم).
- **«249 reportFromCatalog في المترجم»** → كان **صفراً** قبل EM-CPP-7؛ المترجم لم يكن يستخدم الكتالوج.

**العلة الحقيقية الوحيدة:** `hasNativeImplementation` (القسم 5).

---

## 8-ب. منع تسرّب placeholders (دفاع 4 طبقات)

**العلّة المكتشَفة (مؤكَّدة حيّاً):** `ErrorCatalog::substitute` كان يُبقي `{key}` حرفياً عند نقص
placeholder («keep as-is to surface bugs») — لكنه يَطفو **للمستخدم**. مثال: `ل[10]` →
«...خارج نطاق `{container}`».

```mermaid
flowchart TD
  CALL["throwRuntime/ctx.error(code, {{...}})"] --> SUB["ErrorCatalog::substitute"]
  SUB --> Q{"placeholder موجود؟"}
  Q -->|"نعم"| FILL["استبدل بالقيمة"]
  Q -->|"لا (مفقود)"| ENV{"SAD_DEBUG_PLACEHOLDERS؟"}
  ENV -->|"إنتاج (افتراضي)"| BLANK["يُفرَّغ — لا يتسرّب {key} للمستخدم أبداً"]
  ENV -->|"تطوير"| KEEP["يُبقى {key} — لكشف العلّة للمطوّر"]
```

| الطبقة | الآلية | الدليل |
|--------|--------|--------|
| **1 — حاجز render** | `substitute` يُفرّغ المفقود افتراضياً؛ `SAD_DEBUG_PLACEHOLDERS` يُبقيه للمطوّر | `error_catalog.cpp::substitute` |
| **2 — تمييز اختياري** | `{suggestion_clause}` ونحوها لواحق تُلحَق-أو-تُترَك → يكفيها الحاجز | — |
| **3 — إثراء البيانات** | كل موقع يمرّر placeholders المطلوبة بقيمها الحقيقية (77/77) + تبسيط briefs مُفرطة التحديد | `runtime.yaml` + `expression_evaluator_*` |
| **4 — حارس سلوكي** | يُشغّل مسارات خطأ ويتأكّد لا `{key}` يتسرّب (regex `\{[a-z_]+\}`) | `tests/builtin_errors/test_no_placeholder_leaks.py` |

**القاعدة للمطوّر:** الحاجز **شبكة أمان لا رخصة إهمال** — مرّر **كل** placeholder يطلبه الـbrief
(`substitute` يحجب التسرّب، لكن الفراغ يُضعف الرسالة). ولا تستخدم رمزاً يتطلّب placeholders إضافية
دون تمريرها (مثل `RUN_TYPE_CHECK_FAILED`: `{expected}`/`{actual}`)؛ لفحص أرغ مضمنة استخدم
`RUN_BUILTIN_REQUIRES_ARG` (يكفيه `{func}` المحقون). **للتشخيص:** شغّل بـ`SAD_DEBUG_PLACEHOLDERS=1`
لرؤية أي `{key}` مفقود.

> **مبدأ التصميم:** صُمّمت بعض الـbriefs مُفرطة التحديد (placeholders أكثر مما تمرّره المواقع).
> الإصلاح الأنظف غالباً **تبسيط الـbrief** للمفاتيح المُمرَّرة بثبات (يُصلح عشرات المواقع بتعديل
> YAML واحد) لا تعديل كل موقع.

---

## 9. مراجع

- **المعمارية (BuiltinContext):** `docs/BUILTIN_CONTEXT_DESIGN.md` + `decisions/ADR-EM-CPP-1-BUILTIN-CONTEXT.md`.
- **الإبيك:** `epics/EPIC-EM-CPP-MIGRATION.md` · **التقدّم:** `status/em-cpp-progress.md`.
- **التكامل (المُولَّد = المصدر الحيّ):** `status/implementation_status.md` (EM-3).
- **ملفات مفتاحية:** `shared/errors/include/{error_codes.h, runtime_throw.h, builtin_error.h}` ·
  `interpreter/include/builtins/builtin_context.h` · `interpreter/include/managers/function_manager.h` ·
  `language-truth/errors/internal.yaml`.
