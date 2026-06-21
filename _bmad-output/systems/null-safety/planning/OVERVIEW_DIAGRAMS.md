# نظام أمان null — الصورة الكاملة بالمخطّطات

> **الغرض:** توضيح بصريّ شامل لنظام أمان null في لغة ص — أين يقع، كيف يعمل،
> كيف تتدفّق البيانات، وكيف تترابط القصص والقرارات. مرجع موحَّد قبل التنفيذ.
>
> **مصدر الحقيقة للقرارات:** [ADR-NS-001](../decisions/ADR-NS-001-flow-analysis-scope-and-strictness.md)
> · **الترتيب:** [ROADMAP](ROADMAP.md) · **المعمارية:** [ARCHITECTURE](ARCHITECTURE.md)
> · **تاريخ:** 2026-06-20

---

## 1. أين يقع نظام أمان null في خطّ المعالجة؟

النظام **مكوّن مشترك واحد** (`NullSafetyAnalyzer`) يُستدعى من **كلا المحرّكين** —
المفسّر (`sad-run`) والمترجم (`sad-build`/sadc) — فالتحليل **مصدر حقيقة واحد** لا
يُكرَّر ولا يتباين بين المسارين (نظير `shared/ownership/borrow_checker`).

```mermaid
flowchart TB
    SRC["مصدر .ص<br/>Source"] --> LEX["المحلّل المعجمي<br/>Lexer"]
    LEX --> PRS["المحلّل النحوي<br/>Parser"]
    PRS --> AST["الشجرة المجرّدة<br/>AST"]

    AST --> SHARED{{"التحليلات المشتركة<br/>Shared analyses"}}

    subgraph ENGINES["المحرّكان — نقطة استدعاء واحدة"]
        direction LR
        INT["InterpreterCore<br/>(sad-run)"]
        CMP["CompilerDriver<br/>(sad-build / sadc)"]
    end

    SHARED --> INT
    SHARED --> CMP

    subgraph ANALYSES["مكوّنات مشتركة (مصدر حقيقة واحد)"]
        direction TB
        OWN["BorrowChecker<br/>shared/ownership"]
        NS["★ NullSafetyAnalyzer ★<br/>shared/null_safety<br/>(هذا النظام)"]
        TC["TypeChecker<br/>shared/semantic"]
    end

    INT -. يستدعي .-> NS
    CMP -. يستدعي .-> NS
    INT --> OWN & TC
    CMP --> OWN & TC

    INT --> EXEC["تنفيذ مباشر<br/>Execute"]
    CMP --> SIR["بناء SIR → LLVM<br/>codegen"]

    style NS fill:#2d6a4f,stroke:#95d5b2,stroke-width:3px,color:#fff
    style ANALYSES fill:#1b4332,color:#fff
    style ENGINES fill:#264653,color:#fff
```

**النقطة الجوهرية:** السهم المنقّط من المحرّكين إلى `NullSafetyAnalyzer` يمثّل
الاستدعاء الموحَّد — أُرسِيَ في **NS-01** (بلا منطق بعد)، وتُبنى دلالته في القصص التالية.

---

## 2. الوضع الحالي vs المنشود (المشكلة التي يحلّها النظام)

```mermaid
flowchart LR
    subgraph BEFORE["❌ الوضع الحالي — فرض موزَّع"]
        direction TB
        B1["type_checker<br/>(لا يُفعَّل في sad-run)"]
        B2["statement_executor<br/>فرض P9 زمن التشغيل"]
        B3["⇒ سلوك المفسّر والمترجم قد يتباين<br/>⇒ لا مصدر حقيقة واحد"]
        B1 --> B3
        B2 --> B3
    end

    subgraph AFTER["✅ المنشود — نقطة تحليل واحدة"]
        direction TB
        A1["NullSafetyAnalyzer<br/>مشترك بين المحرّكين"]
        A2["⇒ سلوك موحَّد مُختبَر بالتكافؤ<br/>⇒ مصدر حقيقة واحد (D8)"]
        A1 --> A2
    end

    BEFORE ==>|"NS-02 يحذف القديم<br/>ويعيد الإرساء"| AFTER

    style BEFORE fill:#5a1e1e,color:#fff
    style AFTER fill:#1b4332,color:#fff
```

---

## 3. فصل محورَي القرار (ADR-NS-001 D6) — الأهمّ بنيويًّا

محور **أمان null** منفصل داخليًّا عن محور **سياسة الذاكرة**، ولو شاركا واجهة اليوم.
المحلّل يأخذ **مستوى صرامة مجرّدًا** (`ignore/warn/fatal`) ولا يعرف أعلام الذاكرة.
الترجمة تحدث عند الحدود فقط.

```mermaid
flowchart TB
    subgraph POLICY["محور سياسة الذاكرة (CLI)"]
        GC["--gc<br/>OwnershipMode::Disabled"]
        LEARN["--learn<br/>OwnershipMode::Warnings"]
        PROD["--prod<br/>UltraStrict/Strict"]
    end

    TRANS{{"الترجمة عند الحدّ<br/>translateStrictness()<br/>(منطق dispatch نفسه)"}}

    subgraph ABSTRACT["محور الصرامة المجرّد (داخل المحلّل)"]
        IGN["Strictness::Ignore"]
        WRN["Strictness::Warn"]
        FAT["Strictness::Fatal"]
    end

    GC --> TRANS --> IGN
    LEARN --> TRANS --> WRN
    PROD --> TRANS --> FAT

    NS["NullSafetyAnalyzer<br/>يرى المستوى المجرّد فقط"]
    IGN & WRN & FAT --> NS

    NOTE["يسمح مستقبلًا بتوليفات مستقلّة:<br/>«GC + صرامة null قاتلة» دون إعادة هيكلة"]
    NS -.-> NOTE

    style ABSTRACT fill:#1b4332,color:#fff
    style POLICY fill:#264653,color:#fff
    style TRANS fill:#bc6c25,color:#fff
    style NOTE fill:#333,color:#fff,stroke-dasharray: 5 5
```

> **استثناء `!!` (D7):** التأكيد عقدٌ صريح من المستخدم — يرمي خطأ كتالوج
> `NS_FORCE_UNWRAP_ON_NULL` زمن التشغيل على `عدم` **حتى في `--gc`** (لا يتبع هذا الجدول).

---

## 4. بنية المكوّن — الحالي (NS-01) وما يُضاف لاحقًا

```mermaid
classDiagram
    class NullSafetyAnalyzer {
        -Strictness strictness_
        -bool useArabicMessages_
        +setStrictness(Strictness)
        +analyze(program) NullSafetyResult
    }
    class NullSafetyResult {
        +bool success
        +vector~string~ errors
        +vector~string~ warnings
    }
    class Strictness {
        <<enum>>
        Ignore
        Warn
        Fatal
    }
    class NullFlowEnvironment {
        ~NS-03~
        +narrow(var, T)
        +invalidate(var)
        +join(branches)
    }
    class NullSafetyVisitor {
        ~NS-04 + NS-03~
        +visitVarDecl()
        +visitMemberExpr()
        +visitForceUnwrap()
    }

    NullSafetyAnalyzer --> NullSafetyResult : ينتج
    NullSafetyAnalyzer --> Strictness : يستهلك
    NullSafetyAnalyzer ..> NullSafetyVisitor : NS-04 فصاعدًا
    NullSafetyVisitor ..> NullFlowEnvironment : NS-03

    note for NullSafetyAnalyzer "NS-01: analyze() يعيد نجاحًا فارغًا<br/>(هيكل + استدعاء فقط)"
    note for NullSafetyVisitor "يُضاف في NS-04 (رصد)<br/>ثم NS-03 (تضييق)"
```

---

## 5. تسلسل الاستدعاء — كيف يطلب المحرّكان التحليل

```mermaid
sequenceDiagram
    participant ENG as المحرّك (مفسّر/مترجم)
    participant POL as MemoryModeSettings
    participant NS as NullSafetyAnalyzer
    participant DSP as dispatch() (NS-02)
    participant ERR as ErrorManager

    ENG->>POL: اقرأ سياسة الذاكرة (--gc/--learn/--prod)
    ENG->>NS: setStrictness(translate(policy))  %% D6
    ENG->>NS: analyze(program)
    activate NS
    Note over NS: NS-01: لا منطق<br/>NS-04: رصد الوصول غير الآمن<br/>NS-03: تضييق التدفّق
    NS-->>ENG: NullSafetyResult
    deactivate NS
    alt يوجد تشخيص (NS-02 فصاعدًا)
        ENG->>DSP: dispatch(NullSafetyErrorKind, settings, loc)
        DSP-->>ENG: DispatchResult(action, msg)
        alt Fatal (--prod)
            ENG->>ERR: throwRuntime / أوقف البناء
        else Warn (--learn)
            ENG->>ERR: reportWarning + ملاحظة تعليمية
        else Ignore (--gc)
            Note over ENG: تجاهل
        end
    end
```

---

## 6. خريطة القصص والاعتمادات (مُعاد ترتيبها)

```mermaid
flowchart LR
    NS01["NS-01<br/>الهيكل المشترك<br/>✅ منجز ومُتحقَّق"]
    NS02["NS-02<br/>dispatch + حذف P9<br/>(الحالي)"]
    NS04["NS-04<br/>رصد الوصول غير الآمن<br/>(قابل للشحن)"]
    NS03["NS-03<br/>تحليل التدفّق<br/>(عالي المخاطرة)"]
    NS05["NS-05<br/>عامل التأكيد !!"]
    NS06["NS-06<br/>codegen المترجم"]

    NS01 --> NS02 --> NS04 --> NS03 --> NS05 --> NS06

    style NS01 fill:#1b4332,color:#fff
    style NS02 fill:#bc6c25,color:#fff
    style NS03 fill:#5a1e1e,color:#fff
    style NS04 fill:#264653,color:#fff
```

> **تبرير قلب NS-04↔NS-03:** الوصول الخام قيمة مستقلّة على أساس مستقرّ؛ بناء
> التضييق عالي المخاطرة فوق رصدٍ يعمل أأمن من العكس (إجماع Winston + Amelia).

---

## 7. تحليل التدفّق (NS-03) — كيف يضيّق النوع

```mermaid
flowchart TB
    START["متغير اسم: نص؟<br/>(قد يكون عدمًا)"] --> CHECK{"إذا (اسم != لاشيء)"}

    CHECK -->|then| NARROW["اسم: نص ✅<br/>(مُضيَّق — وصول مباشر مسموح)"]
    CHECK -->|else| KEEP["اسم: نص؟<br/>(يبقى قابلًا للعدم)"]

    NARROW --> MUT{"تحوّر؟<br/>(إعادة إسناد / استدعاء مُعدِّل)"}
    MUT -->|نعم D2| INVALID["يُبطَل التضييق ← نص؟"]
    MUT -->|لا| USE["اسم.طول() مسموح"]

    NARROW --> EXIT["خارج الكتلة"]
    EXIT --> BACK["يتراجع ← نص؟ (D4)"]

    style NARROW fill:#1b4332,color:#fff
    style INVALID fill:#5a1e1e,color:#fff
    style KEEP fill:#264653,color:#fff
```

**القيود (الموجة الأولى — ADR-NS-001 D1-D5):**
- **محلّيّات فقط** — لا حقول (`س.حقل`) ولا عبر الإغلاقات.
- **إبطال بالتحوّر** (D2)، **نقطة ثابتة للحلقات** (D3)، **خروج حتميّ للتضييق العكسي** (D4).
- **سليم-متحفّظ لا متفائل** (D5) — يضيّق أقلّ عند الشكّ (الأمان الزائف أخطر من غيابه).

---

## 8. سؤال نقل P9 (D8) — ساكن vs زمنيّ (قرار NS-02)

الفرض الحاليّ **زمن تشغيليّ** (يُقيّم القيمة ثم يفحص `isNull()`) فيمسك الحالتين.
المحلّل المشترك **ساكن** فلا يرى القيم المحسوبة زمنًا. هذا جوهر القرار:

```mermaid
flowchart TB
    subgraph CASES["ما الذي يُمسَك؟"]
        LIT["رقم س = لاشيء<br/>(حرفيّ — ساكن ✅)"]
        RUN["رقم س = دالة_ترجع_لاشيء()<br/>(محسوب — زمنيّ فقط)"]
    end

    subgraph OPT1["خيار 1: ساكن للحرفيّات + حارس زمنيّ"]
        O1["المحلّل يمسك الحرفيّ<br/>+ فحص زمنيّ مبسّط للمحسوب<br/>⇒ لا فقدان تغطية"]
    end

    subgraph OPT2["خيار 2: ساكن بالكامل"]
        O2["حذف الزمنيّ تمامًا<br/>⇒ نقطة واحدة صافية<br/>⇒ فقدان رصد null المحسوب"]
    end

    LIT --> OPT1
    RUN --> OPT1
    LIT --> OPT2
    RUN -.->|"يُفقَد"| OPT2

    style OPT1 fill:#1b4332,color:#fff
    style OPT2 fill:#5a1e1e,color:#fff
    style RUN fill:#bc6c25,color:#fff
```

> **حالة القرار:** مفتوح — يُحسَم قبل تنفيذ D8. يُسجَّل في ADR-NS-001 عند الحسم.
>
> 📊 **مقارنة معمّقة بـ5 لغات** (Kotlin/Swift/Dart/TypeScript/C# + Rust مرجعًا) لهذا القرار:
> [LANGUAGE_COMPARISON](LANGUAGE_COMPARISON.md) — تخلص إلى ترجيح الخيار 1 (ساكن للحرفيّات + حارس زمنيّ).

---

## 9. خريطة الكود (أين يعيش كل شيء)

| المكوّن | المسار | الحالة |
|--------|--------|--------|
| المحلّل المشترك (رأس) | `shared/null_safety/include/null_safety/null_safety_analyzer.h` | ✅ NS-01 |
| المحلّل المشترك (تنفيذ) | `shared/null_safety/src/null_safety_analyzer.cpp` | ✅ NS-01 |
| مكتبة CMake | `shared/null_safety/CMakeLists.txt` → `sad_null_safety` | ✅ NS-01 |
| استدعاء المفسّر | `interpreter/src/core/interpreter_core.cpp` | ✅ NS-01 |
| استدعاء المترجم | `tools/compiler/compiler_driver_analysis.cpp::run_null_safety()` | ✅ NS-01 |
| نقطة القرار | `shared/errors/include/builders/dispatch.h` (+ `NullSafetyErrorKind`) | ⏳ NS-02 |
| فرض P9 القديم (للحذف) | `interpreter/src/visitors/statement_executor.cpp:138` | ⏳ NS-02 (D8) |
| بيئة التدفّق | `shared/null_safety/` (`NullFlowEnvironment`) | ⏳ NS-03 |
| عامل `!!` | `ForceUnwrapExpr` عبر الطبقات | ⏳ NS-05 |

---

## 10. الأساس الجاهز (من نظام الأنواع — لا يُعاد)

```mermaid
flowchart LR
    subgraph DONE["✅ نظام الأنواع (منجز)"]
        D1["SadTypeKind::Null متمايز<br/>TS-P1"]
        D2["SadOptionalType / T؟<br/>TS-P4"]
        D3["isAssignableTo: Null &lt;: T؟<br/>TS-P4-AC7"]
        D4["عوامل ?. ?? ؟<br/>TS-P8"]
    end
    subgraph NSADDS["★ يضيفه نظام أمان null"]
        N1["التحليل والإنفاذ الموحَّد"]
        N2["التضييق الذكيّ"]
        N3["دلالة !! الآمنة"]
    end
    DONE ==> NSADDS
    style DONE fill:#1b4332,color:#fff
    style NSADDS fill:#2d6a4f,color:#fff
```

---

> **ملاحظة:** المخطّطات بصيغة Mermaid تُعرَض تلقائيًّا على GitHub وفي mdBook.
> هذا الملف مرجع توضيحيّ؛ مصدر القرارات الملزِم يبقى [ADR-NS-001](../decisions/ADR-NS-001-flow-analysis-scope-and-strictness.md).
