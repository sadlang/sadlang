---
id: DIAGRAM-BUILTINS
title: "مخطط نظام الدوال المدمجة وأقسامه (بالرسومات)"
version: 1.0
status: ACTIVE
date: 2026-06-07
---

# مخطط نظام الدوال المدمجة — لغة ص

شرح مرئي لنظام الدوال المدمجة: من مصدر الحقيقة (YAML) إلى الثوابت المُولَّدة،
وكيف تستهلكها مكونات اللغة الثلاثة (مفسر/مترجم/VM).

---

## 0. التدفق العام: من YAML إلى التنفيذ

```mermaid
flowchart TB
  subgraph SoT["🟦 مصدر الحقيقة (وقت التطوير)"]
    Y1["language-truth/builtins/*.yaml<br/>(41 ملف)"]
    Y2["language-truth/type_methods.yaml<br/>(طرق الأنواع)"]
    IDX["_index.yaml<br/>(ترتيب الملفات)"]
  end

  subgraph Build["🟩 زمن البناء (CMake + Python)"]
    GEN["gen_builtins_registry.py"]
    HDR["shared/builtins/generated/<br/>builtin_registry_generated.h<br/>(1211 ثابت constexpr)"]
  end

  subgraph Wrap["🟧 الغلاف"]
    WRAP["builtin_registry.h<br/>(#include generated)"]
  end

  subgraph Consumers["🟨 المستهلكون (يستخدمون الثوابت)"]
    INT["المفسر (sad)"]
    CMP["المترجم (sadc)"]
    VM["الآلة الافتراضية (sad_vm) ⚠️ غير مُحوَّلة"]
  end

  Y1 --> GEN
  Y2 --> GEN
  IDX --> GEN
  GEN --> HDR
  HDR --> WRAP
  WRAP --> INT
  WRAP --> CMP
  WRAP -.->|الهدف| VM

  style Y1 fill:#e1f5ff,stroke:#0066cc
  style HDR fill:#e8f5e9,stroke:#2e7d32
  style WRAP fill:#fff3e0,stroke:#e65100
  style VM fill:#ffebee,stroke:#c62828
```

> **القاعدة:** YAML هو المصدر الوحيد. تعديل اسم = تعديل YAML → CMake يُعيد التوليد → كل المكونات تتحدث.

---

## 1. التصنيفات الأربعة للدوال المدمجة

كل دالة تنتمي لأحد أربعة تصنيفات (`builtin_categories.h`) تحدد كيف تُعامَل:

```mermaid
flowchart LR
  subgraph CAT["BuiltinCategory (4 أنواع)"]
    direction TB
    A["1️⃣ CORE_IO<br/>اطبع، اطبع_سطر، اقرأ<br/>━━━━━<br/>بلا استيراد<br/>(3 دوال)"]
    B["2️⃣ TYPE_CONSTRUCTOR<br/>رقم، عشري، نص، منطقي<br/>━━━━━<br/>بلا استيراد<br/>(4 دوال)"]
    C["3️⃣ MODULE_FUNCTION<br/>جذر، تحليل_جيسون، اقرأ_ملف<br/>━━━━━<br/>تحتاج استورد<br/>(65 في جدول البحث)"]
    D["4️⃣ TYPE_METHOD<br/>.رتب() .طول() .أرسل()<br/>━━━━━<br/>تُستدعى بـ . (نقطة)<br/>(32 في جدول البحث)"]
  end

  style A fill:#e8f5e9,stroke:#2e7d32
  style B fill:#e1f5ff,stroke:#0066cc
  style C fill:#fff3e0,stroke:#e65100
  style D fill:#f3e5f5,stroke:#6a1b9a
```

| التصنيف | الاستدعاء | يحتاج استيراد؟ | مثال |
|---------|-----------|----------------|------|
| `CORE_IO` | `اطبع(س)` | ❌ | اطبع، اقرأ |
| `TYPE_CONSTRUCTOR` | `رقم("42")` | ❌ | رقم، نص |
| `MODULE_FUNCTION` | `جذر(9)` | ✅ `استورد رياضيات` | جذر، تحليل_جيسون |
| `TYPE_METHOD` | `مصفوفة.رتب()` | ❌ | .رتب، .أرسل |

---

## 2. بنية الـ header المُولَّد (الأقسام الداخلية)

```mermaid
flowchart TB
  subgraph H["builtin_registry_generated.h"]
    direction TB
    U["🛡️ #undef ماكروز Windows<br/>(DELETE/UNION/IN/OUT…)<br/>لتجنب التعارض في sadc"]
    N["📛 namespace Names {<br/>Core, TypeCtor, Math, Strings,<br/>Maps, Sockets, Kernel*, Compiler*,<br/>TypeMethods::{Array,String,Map,Channel}<br/>}<br/>(1211 ثابت constexpr string_view)"]
    AR["📊 مصفوفات البحث:<br/>CORE_IO_BUILTINS[3]<br/>TYPE_CONSTRUCTOR_BUILTINS[4]<br/>MODULE_FUNCTION_BUILTINS[65]<br/>TYPE_METHOD_BUILTINS[32]"]
    SA["✅ static_assert<br/>(تحقق الأعداد وقت الترجمة)"]
    FN["🔍 دوال البحث:<br/>findBuiltinByName()<br/>isAvailableWithoutImport()<br/>getRequiredModule()<br/>suggestModuleForFunction()"]
  end

  U --> N --> AR --> SA --> FN

  style U fill:#ffebee,stroke:#c62828
  style N fill:#e8f5e9,stroke:#2e7d32
  style AR fill:#e1f5ff,stroke:#0066cc
  style FN fill:#fff3e0,stroke:#e65100
```

---

## 3. كيف يستهلك كل مكوّن الثوابت

```mermaid
flowchart TB
  HDR["Names::Math::SQRT = \"جذر\""]

  subgraph INT["🟢 المفسر (interpreter)"]
    I1["التسجيل:<br/>registerBuiltinFunction(<br/>std::string(Bm::SQRT), func)"]
    I2["الطرق:<br/>if (m == TM::Array::SORT)"]
  end

  subgraph CMP["🔵 المترجم (compiler)"]
    C1["المطابقة لتوليد SIR:<br/>if (funcName == Bm::SQRT)<br/>→ BUILTIN_MATH_SQRT"]
  end

  subgraph VM["🔴 الآلة الافتراضية (الهدف)"]
    V1["سجّل_دالة_أصلية(<br/>std::string(Bn::Core::PRINT), …)"]
  end

  HDR --> I1
  HDR --> I2
  HDR --> C1
  HDR -.->|غير مُنفَّذ بعد| V1

  style INT fill:#e8f5e9,stroke:#2e7d32
  style CMP fill:#e1f5ff,stroke:#0066cc
  style VM fill:#ffebee,stroke:#c62828
```

---

## 4. مسار استدعاء دالة (مثال: `جذر(9)` في المفسر)

```mermaid
sequenceDiagram
  participant U as كود .ص
  participant L as Lexer
  participant P as Parser
  participant E as Evaluator
  participant R as FunctionManager
  participant F as الدالة المدمجة

  U->>L: جذر(9)
  L->>P: IDENTIFIER("جذر") (بلا تشكيل)
  P->>E: CallExpr(name="جذر")
  E->>R: lookup("جذر")
  Note over R: مُسجَّلة بـ std::string(Bm::SQRT)<br/>SQRT = "جذر" من YAML
  R->>F: استدعاء lambda
  F->>E: Value(3.0)
  E->>U: 3.0
```

---

## 5. خريطة ملفات النظام (أين كل شيء)

```mermaid
flowchart LR
  subgraph S1["مصدر الحقيقة"]
    direction TB
    F1["language-truth/builtins/<br/>core, math, strings, maps,<br/>kernel_*, compiler_*, …"]
    F2["language-truth/type_methods.yaml"]
  end

  subgraph S2["أدوات التوليد"]
    direction TB
    T1["scripts/codegen/<br/>gen_builtins_registry.py"]
    T2["normalize_canonical.py<br/>(تطبيع الحركات)"]
    T3["convert_*.py + remove_aliases.py<br/>(أدوات التحويل)"]
  end

  subgraph S3["المُولَّد + العقد"]
    direction TB
    G1["shared/builtins/include/<br/>builtin_registry.h (wrapper)"]
    G2["shared/builtins/generated/<br/>builtin_registry_generated.h"]
    G3["builtin_categories.h<br/>module_definitions.h"]
  end

  subgraph S4["المستهلكون"]
    direction TB
    C1["interpreter/src/builtins/*"]
    C2["compiler/src/frontend/builders/*"]
    C3["vm/src/sad_vm_executor.cpp ⚠️"]
  end

  S1 --> T1 --> G2 --> G1 --> S4
  T2 -.-> S1
  G3 --> G2

  style S1 fill:#e1f5ff,stroke:#0066cc
  style G2 fill:#e8f5e9,stroke:#2e7d32
  style S4 fill:#fff3e0,stroke:#e65100
```

---

## 6. ملخص الأقسام

| القسم | الموقع | الوصف |
|-------|--------|-------|
| **مصدر الحقيقة** | `language-truth/builtins/*.yaml` | 41 ملف YAML — تعريف كل دالة مرة واحدة |
| **طرق الأنواع** | `language-truth/type_methods.yaml` | طرق `.رتب()` `.أرسل()` |
| **المُولِّد** | `scripts/codegen/gen_builtins_registry.py` | YAML → C++ header |
| **العقد** | `builtin_categories.h` + `module_definitions.h` | enums التصنيف والوحدات |
| **المُولَّد** | `generated/builtin_registry_generated.h` | 1211 ثابت + مصفوفات بحث |
| **الغلاف** | `builtin_registry.h` | `#include` المُولَّد |
| **المفسر** | `interpreter/src/builtins/` + `visitors/oop_*` | ✅ مُحوَّل |
| **المترجم** | `compiler/src/frontend/builders/` | ✅ مُحوَّل |
| **الآلة الافتراضية** | `vm/src/sad_vm_executor.cpp` | ⚠️ غير مُحوَّلة (الخطة التالية) |
| **ربط البناء** | `cmake/codegen.cmake` | target `sad_builtin_registry_codegen` |

---

*مرجع تفصيلي للإضافة: [HOW_TO_ADD_BUILTIN.md](HOW_TO_ADD_BUILTIN.md)*
